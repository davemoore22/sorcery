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

#include <SDL2/SDL_audio.h>
#include <string>
#include <vector>

// Forward declare FFmpeg types
struct AVFormatContext;
struct AVCodecContext;
struct AVPacket;
struct AVFrame;
struct SwrContext;

namespace Sorcery {
class AudioPlayer {
	public:
		AudioPlayer();
		~AudioPlayer();

		void load(const std::string &filename);
		void play();
		void stop();
		void update(); // call every frame

	private:
		void free_resources();

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
};

}