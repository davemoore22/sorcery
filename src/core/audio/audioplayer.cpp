// Copyright (C) 2026 Dave Moore
//
// This file is part of Sorcery.
//
// Sorcery is free software: you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software
// Foundation, either version 2 of the License, or (at your option) any later
// version.
//
// Sorcery is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with
// Sorcery.  If not, see <http://www.gnu.org/licenses/>.
//
// If you modify this program, or any covered work, by linking or combining
// it with the libraries referred to in README (or a modified version of
// said libraries), containing parts covered by the terms of said libraries,
// the licensors of this program grant you additional permission to convey
// the resulting work.

#include "core/audio/audioplayer.hpp"
#include "common/ffmpeg.hpp" // IWYU pragma: keep
#include "core/debug.hpp"	 // for DEBUG_LOGF, debug_logf
#include "resources/filestore.hpp"
#include "types/scopedtimer.hpp" // for PROFILE_SCOPE
#include <SDL_audio.h>			 // for SDL_PauseAudioDevice, SDL_Clea...
#include <SDL_error.h>			 // for SDL_GetError
#include <SDL_stdinc.h>			 // for Uint32
#include <algorithm>
#include <iostream>	 // for basic_ostream, operator<<, cerr
#include <stdexcept> // for runtime_error

/// @brief
/// @param files
Sorcery::AudioPlayer::AudioPlayer(FileStore *files)
	: _files{files} {

	av_log_set_level(AV_LOG_ERROR);

	SDL_AudioSpec want{};

	mute = false;

	want.freq = 48000;
	want.format = AUDIO_F32SYS;
	want.channels = 2;
	want.samples = 4096;
	want.callback = nullptr;

	_device = SDL_OpenAudioDevice(nullptr, 0, &want, &_spec, 0);

	if (!_device) {
		std::cerr << "SDL audio error: " << SDL_GetError() << "\n";
		throw std::runtime_error("Failed to open audio device");
	}

	SDL_PauseAudioDevice(_device, 1);
}

/// @brief
Sorcery::AudioPlayer::~AudioPlayer() {

	_free_resources();

	if (_device)
		SDL_CloseAudioDevice(_device);
}

/// @brief
/// @return
auto Sorcery::AudioPlayer::_free_resources() -> void {

	if (_packet)
		av_packet_free(&_packet);
	if (_frame)
		av_frame_free(&_frame);
	if (_codec)
		avcodec_free_context(&_codec);
	if (_fmt)
		avformat_close_input(&_fmt);
	if (_swr)
		swr_free(&_swr);

	_packet = nullptr;
	_frame = nullptr;
	_codec = nullptr;
	_fmt = nullptr;
	_swr = nullptr;

	_stream_index = -1;
}

/// @brief
/// @param filename
/// @return
auto Sorcery::AudioPlayer::_load(const std::string_view filename) -> void {

	// PROFILE_SCOPE("AudioPlayer::load");
	//  DEBUG_LOGF("Loading Resource: {}", filename);

	_free_resources();

	std::string str(filename);
	if (avformat_open_input(&_fmt, str.c_str(), nullptr, nullptr) < 0)
		throw std::runtime_error("Failed to open audio file");

	if (avformat_find_stream_info(_fmt, nullptr) < 0)
		throw std::runtime_error("Failed to read stream info");

	const AVCodec *codec{nullptr};

	_stream_index = av_find_best_stream(_fmt, AVMEDIA_TYPE_AUDIO, -1, -1, &codec, 0);

	if (_stream_index < 0)
		throw std::runtime_error("No audio stream");

	_codec = avcodec_alloc_context3(codec);

	avcodec_parameters_to_context(_codec, _fmt->streams[_stream_index]->codecpar);

	if (avcodec_open2(_codec, codec, nullptr) < 0)
		throw std::runtime_error("Failed to open codec");

	_packet = av_packet_alloc();
	_frame = av_frame_alloc();

	// Resampler (modern FFmpeg API)
	AVChannelLayout out_layout{};
	av_channel_layout_default(&out_layout, _spec.channels);

	const AVChannelLayout *in_layout{&_codec->ch_layout};

	if (swr_alloc_set_opts2(&_swr, &out_layout, AV_SAMPLE_FMT_FLT, _spec.freq, in_layout, _codec->sample_fmt,
							_codec->sample_rate, 0, nullptr) < 0) {

		av_channel_layout_uninit(&out_layout);
		throw std::runtime_error("Failed to allocate resampler");
	}

	if (swr_init(_swr) < 0) {
		av_channel_layout_uninit(&out_layout);
		throw std::runtime_error("Failed to initialise resampler");
	}

	// Clean up temp layout
	av_channel_layout_uninit(&out_layout);
}

/// @brief
/// @return
auto Sorcery::AudioPlayer::_play() -> void {

	if (!_fmt)
		return;

	// Already playing: don't restart the track.
	if (_playing) {

		// If it was fading out, simply reverse direction.
		if (_state == Enums::Audio::State::FADING_OUT)
			_begin_fade_in();

		return;
	}

	SDL_ClearQueuedAudio(_device);

	_playing = true;
	_fade = 0.0f;

	_begin_fade_in();

	// Fill buffer while device is still paused.
	update();

	SDL_PauseAudioDevice(_device, 0);
}

/// @brief
/// @return
auto Sorcery::AudioPlayer::_stop() -> void {

	if (!_playing)
		return;

	if (_state == Enums::Audio::State::FADING_OUT)
		return;

	_begin_fade_out();
}

/// @brief
/// @param volume
auto Sorcery::AudioPlayer::set_volume(float volume) -> void {

	_volume = std::clamp(volume, 0.0f, 1.0f);
}

/// @brief
/// @return
auto Sorcery::AudioPlayer::update() -> void {

	_update_transition();

	if (!_playing || !_fmt)
		return;

	if (mute)
		return;

	const Uint32 target_buffer{
		static_cast<Uint32>(_spec.freq * _spec.channels * sizeof(float) * (BUFFER_MS / 1000.0f))};

	while (SDL_GetQueuedAudioSize(_device) < target_buffer) {

		if (av_read_frame(_fmt, _packet) < 0) {
			av_seek_frame(_fmt, _stream_index, 0, AVSEEK_FLAG_BACKWARD);
			avcodec_flush_buffers(_codec);
			continue;
		}

		if (_packet->stream_index == _stream_index) {

			if (avcodec_send_packet(_codec, _packet) == 0) {

				while (avcodec_receive_frame(_codec, _frame) == 0) {

					int out_samples{swr_get_out_samples(_swr, _frame->nb_samples)};

					uint8_t *out_data{nullptr};
					int out_linesize;

					av_samples_alloc(&out_data, &out_linesize, _spec.channels, out_samples, AV_SAMPLE_FMT_FLT, 0);

					int converted{
						swr_convert(_swr, &out_data, out_samples, (const uint8_t **)_frame->data, _frame->nb_samples)};

					int size{converted * _spec.channels * sizeof(float)};

					float *samples{reinterpret_cast<float *>(out_data)};
					int sample_count{converted * _spec.channels};

					const auto gain{_volume * _fade};
					for (int i = 0; i < sample_count; ++i)
						samples[i] *= gain;

					SDL_QueueAudio(_device, out_data, size);

					av_freep(&out_data);
				}
			}
		}

		av_packet_unref(_packet);
	}
}

/// @brief
/// @return
auto Sorcery::AudioPlayer::_begin_fade_in() -> void {

	_fade_updated = Clock::now();
	_state = Enums::Audio::State::FADING_IN;
}

/// @brief
/// @return
auto Sorcery::AudioPlayer::_begin_fade_out() -> void {

	_fade_updated = Clock::now();
	_state = Enums::Audio::State::FADING_OUT;
}

/// @brief
/// @return
auto Sorcery::AudioPlayer::_stop_immediately() -> void {

	_playing = false;
	_state = Enums::Audio::State::STOPPED;
	_fade = 0.0f;

	SDL_PauseAudioDevice(_device, 1);
	SDL_ClearQueuedAudio(_device);
}

/// @brief
/// @return
auto Sorcery::AudioPlayer::_update_transition() -> void {

	if (_state != Enums::Audio::State::FADING_IN && _state != Enums::Audio::State::FADING_OUT)
		return;

	const auto now{Clock::now()};
	const auto elapsed{std::chrono::duration<float>{now - _fade_updated}.count()};
	const auto duration{std::chrono::duration<float>{FADE_DURATION}.count()};

	_fade_updated = now;

	const auto delta{elapsed / duration};

	if (_state == Enums::Audio::State::FADING_IN) {

		_fade = std::min(1.0f, _fade + delta);

		if (_fade >= 1.0f) {
			_fade = 1.0f;
			_state = Enums::Audio::State::PLAYING;
		}

	} else {

		_fade = std::max(0.0f, _fade - delta);

		if (_fade <= 0.0f) {

			_fade = 0.0f;
			_finish_fade_out();
		}
	}
}

/// @brief
/// @return
auto Sorcery::AudioPlayer::_finish_fade_out() -> void {

	_stop_immediately();

	_current_track = Enums::Audio::Track::NONE;

	if (_requested_track != Enums::Audio::Track::NONE)
		_start_requested_track();
}

/// @brief
/// @param track
/// @return
auto Sorcery::AudioPlayer::set_track(const Enums::Audio::Track track) -> void {

	// We already want this track.
	if (track == _requested_track)
		return;

	_requested_track = track;

	// Requesting the track already playing.
	// If it happens to be fading out, reverse the fade.
	if (track == _current_track) {

		if (_state == Enums::Audio::State::FADING_OUT)
			_begin_fade_in();

		return;
	}

	// Nothing currently playing.
	if (_current_track == Enums::Audio::Track::NONE) {

		if (_requested_track == Enums::Audio::Track::NONE)
			return;

		_start_requested_track();
		return;
	}

	// Different track requested: fade the current one out first.
	_begin_fade_out();
}

/// @brief
/// @return
auto Sorcery::AudioPlayer::_start_requested_track() -> void {

	if (_requested_track == Enums::Audio::Track::NONE)
		return;

	const auto track{_requested_track};
	const auto resource{Music::resource(track)};

	_load(_files->get_path(resource));

	_current_track = track;

	_play();
}