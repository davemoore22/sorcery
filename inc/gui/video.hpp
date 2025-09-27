// Copyright (C) 2025 Dave Moore
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

#include "common/include.hpp"
#include "common/types.hpp"
#include "core/macro.hpp"

namespace Sorcery {

class Video {
	public:
		// Constructor
		Video();

		// Destructor
		~Video();

		// Public Methods
		auto load(const std::string &filename) -> bool;
		auto update(double playback_time) -> void;
		auto render() -> void;

		// Publuic Members
		bool loaded;

	private:
		// Private Data
		AVFormatContext *_format_ctx;
		AVCodecContext *_codec_ctx;
		AVFrame *_frame;
		AVFrame *_rgb_frame;
		AVPacket *_packet;
		SwsContext *_sws_ctx;
		GLuint _gl_texture;
		int _width;
		int _height;
		double _time_base;
		double _next_pts_sec;
		bool _has_frame_ready;
		int _video_stream_index;
};
};
