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

#include "core/audioplayer.hpp"
#include "common/ffmpeg.hpp"
#include "common/sdl2.hpp"
#include "core/debug.hpp"
#include "types/scopedtimer.hpp"
#include <iostream>
#include <print>

Sorcery::AudioPlayer::AudioPlayer() {

	av_log_set_level(AV_LOG_ERROR);

	SDL_AudioSpec want{};

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

Sorcery::AudioPlayer::~AudioPlayer() {

	free_resources();

	if (_device)
		SDL_CloseAudioDevice(_device);
}

void Sorcery::AudioPlayer::free_resources() {

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

void Sorcery::AudioPlayer::load(const std::string &filename) {

	PROFILE_SCOPE("AudioPlayer::load");
	DEBUG_LOGF("Loading Resource: {}", filename);

	free_resources();

	if (avformat_open_input(&_fmt, filename.c_str(), nullptr, nullptr) < 0)
		throw std::runtime_error("Failed to open audio file");

	if (avformat_find_stream_info(_fmt, nullptr) < 0)
		throw std::runtime_error("Failed to read stream info");

	const AVCodec *codec{nullptr};

	_stream_index =
		av_find_best_stream(_fmt, AVMEDIA_TYPE_AUDIO, -1, -1, &codec, 0);

	if (_stream_index < 0)
		throw std::runtime_error("No audio stream");

	_codec = avcodec_alloc_context3(codec);

	avcodec_parameters_to_context(_codec,
								  _fmt->streams[_stream_index]->codecpar);

	if (avcodec_open2(_codec, codec, nullptr) < 0)
		throw std::runtime_error("Failed to open codec");

	_packet = av_packet_alloc();
	_frame = av_frame_alloc();

	// Resampler (modern FFmpeg API)
	AVChannelLayout out_layout{};
	av_channel_layout_default(&out_layout, _spec.channels);

	const AVChannelLayout *in_layout{&_codec->ch_layout};

	if (swr_alloc_set_opts2(&_swr, &out_layout, AV_SAMPLE_FMT_FLT, _spec.freq,
							in_layout, _codec->sample_fmt, _codec->sample_rate,
							0, nullptr) < 0) {

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

void Sorcery::AudioPlayer::play() {

	if (!_fmt)
		return;

	SDL_ClearQueuedAudio(_device);

	_playing = true;

	// Fill buffer while device is still paused
	update();

	SDL_PauseAudioDevice(_device, 0);
}

void Sorcery::AudioPlayer::stop() {

	_playing = false;
	SDL_PauseAudioDevice(_device, 1);
	SDL_ClearQueuedAudio(_device);
}

void Sorcery::AudioPlayer::set_volume(float v) {

	_volume = v;
}

void Sorcery::AudioPlayer::update() {
	if (!_playing || !_fmt)
		return;

	const Uint32 target_buffer{_spec.freq * _spec.channels * sizeof(float)};

	while (SDL_GetQueuedAudioSize(_device) < target_buffer) {

		if (av_read_frame(_fmt, _packet) < 0) {
			// loop
			av_seek_frame(_fmt, _stream_index, 0, AVSEEK_FLAG_BACKWARD);
			avcodec_flush_buffers(_codec);
			continue;
		}

		if (_packet->stream_index == _stream_index) {

			if (avcodec_send_packet(_codec, _packet) == 0) {

				while (avcodec_receive_frame(_codec, _frame) == 0) {

					int out_samples{
						swr_get_out_samples(_swr, _frame->nb_samples)};

					uint8_t *out_data{nullptr};
					int out_linesize;

					av_samples_alloc(&out_data, &out_linesize, _spec.channels,
									 out_samples, AV_SAMPLE_FMT_FLT, 0);

					int converted{swr_convert(_swr, &out_data, out_samples,
											  (const uint8_t **)_frame->data,
											  _frame->nb_samples)};

					int size{converted * _spec.channels * sizeof(float)};

					float *samples{reinterpret_cast<float *>(out_data)};
					int sample_count{converted * _spec.channels};

					for (int i = 0; i < sample_count; ++i)
						samples[i] *= _volume;

					SDL_QueueAudio(_device, out_data, size);

					av_freep(&out_data);
				}
			}
		}

		av_packet_unref(_packet);
	}
}