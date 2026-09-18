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

#include "core/audio/music.hpp" // for Track, State
#include <SDL2/SDL_audio.h>		// for SDL_AudioDeviceID, SDL_AudioSpec
#include <chrono>				// for milliseconds, steady_clock
#include <cstdint>				// for uint8_t
#include <filesystem>
#include <string_view> // for string_view
#include <vector>	   // for vector

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

		auto update() -> void; // call every frame
		auto set_music_volume(float volume) -> void;
		[[nodiscard]] auto get_music_volume() const -> float;
		auto set_track(Enums::Audio::Track track) -> void;

		bool mute;

	private:
		static constexpr auto FADE_DURATION{std::chrono::milliseconds{750}};
		static constexpr auto BUFFER_MS{200};

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

		float _music_volume{0.0f};
		float _fade{1.0f};

		Clock::time_point _fade_updated{};

		Enums::Audio::Track _current_track{Enums::Audio::Track::NONE};
		Enums::Audio::Track _requested_track{Enums::Audio::Track::NONE};
		Enums::Audio::State _state{Enums::Audio::State::STOPPED};

		bool _debug_first_buffer{false};

		FileStore *_files;

		auto _free_resources() -> void;
		auto _begin_fade_in() -> void;
		auto _begin_fade_out() -> void;
		auto _update_transition() -> bool;
		auto _stop_immediately() -> void;
		auto _finish_fade_out() -> void;
		auto _load(const std::filesystem::path &filename) -> void;
		auto _play() -> void;
		auto _stop() -> void;
		auto _start_requested_track() -> void;
		auto _queued_ms() const -> float;
		auto _device_status() const -> int;
};

}