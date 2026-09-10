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

#pragma once

#include "core/audio/music.hpp"
#include <SDL2/SDL_audio.h> // for SDL_AudioDeviceID, SDL_AudioSpec
#include <chrono>
#include <cstdint> // for uint8_t
#include <string>  // for string
#include <string_view>
#include <vector> // for vector

struct AVCodecContext;	// Global Namespace Forward Declaration
struct AVFormatContext; // Global Namespace Forward Declaration
struct AVFrame;			// Global Namespace Forward Declaration
struct AVPacket;		// Global Namespace Forward Declaration
struct SwrContext;		// Global Namespace Forward Declaration

namespace Sorcery {

class FileStore;

class AudioPlayer {

		using Clock = std::chrono::steady_clock;

	public:
		AudioPlayer(FileStore *files);
		~AudioPlayer();

		void update(); // call every frame
		void set_volume(float volume);
		void set_track(Enums::Audio::Track track);

		bool mute;

	private:
		static constexpr auto FADE_DURATION{std::chrono::milliseconds{750}};
		static constexpr auto BUFFER_MS{100};

		// FFmpeg
		AVFormatContext *_fmt = nullptr;
		AVCodecContext *_codec = nullptr;
		AVPacket *_packet = nullptr;
		AVFrame *_frame = nullptr;
		SwrContext *_swr = nullptr;

		int _stream_index = -1;

		// SDL
		SDL_AudioDeviceID _device = 0;
		SDL_AudioSpec _spec{};

		std::vector<uint8_t> _buffer;

		bool _playing = false;

		float _volume{0.0f};
		float _fade{1.0f};

		Clock::time_point _fade_updated{};

		Enums::Audio::Track _current_track{Enums::Audio::Track::NONE};
		Enums::Audio::Track _requested_track{Enums::Audio::Track::NONE};
		Enums::Audio::State _state{Enums::Audio::State::STOPPED};

		FileStore *_files;

		void _free_resources();
		void _begin_fade_in();
		void _begin_fade_out();
		void _update_transition();
		void _stop_immediately();
		void _finish_fade_out();
		void _load(const std::string_view filename);
		void _play();
		void _stop();
		void _start_requested_track();
};

}