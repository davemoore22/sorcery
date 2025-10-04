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

#include "gui/videoplayer.hpp"

Sorcery::VideoPlayer::VideoPlayer()
	: _format_ctx{nullptr},
	  _codec_ctx{nullptr},
	  _frame{nullptr},
	  _rgb_frame{nullptr},
	  _packet{nullptr},
	  _sws_ctx{nullptr},
	  _rgb_buffer{},
	  _gl_texture{0},
	  _width{0},
	  _height{0},
	  _video_stream_index{-1},
	  _time_base{0.0},
	  _next_pts_sec{0.0},
	  _has_frame_ready{false} {}

Sorcery::VideoPlayer::~VideoPlayer() {
	free_resources();
}

auto Sorcery::VideoPlayer::load(const std::string &filename) -> void {
	free_resources();

	if (avformat_open_input(&_format_ctx, filename.c_str(), nullptr, nullptr) <
		0) {
		throw std::runtime_error{"Failed to open video file"};
	}
	if (avformat_find_stream_info(_format_ctx, nullptr) < 0) {
		throw std::runtime_error{"Failed to get stream info"};
	}

	const AVCodec *codec{nullptr};
	_video_stream_index =
		av_find_best_stream(_format_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, &codec, 0);
	if (_video_stream_index < 0) {
		throw std::runtime_error{"Failed to find video stream"};
	}

	_codec_ctx = avcodec_alloc_context3(codec);
	avcodec_parameters_to_context(
		_codec_ctx, _format_ctx->streams[_video_stream_index]->codecpar);

	if (avcodec_open2(_codec_ctx, codec, nullptr) < 0) {
		throw std::runtime_error{"Failed to open codec"};
	}

	_width = _codec_ctx->width;
	_height = _codec_ctx->height;

	_frame = av_frame_alloc();
	_rgb_frame = av_frame_alloc();
	_packet = av_packet_alloc();

	auto num_bytes{
		av_image_get_buffer_size(AV_PIX_FMT_RGB24, _width, _height, 1)};
	_rgb_buffer.resize(static_cast<size_t>(num_bytes));

	av_image_fill_arrays(_rgb_frame->data, _rgb_frame->linesize,
						 _rgb_buffer.data(), AV_PIX_FMT_RGB24, _width, _height,
						 1);

	_sws_ctx = sws_getContext(_width, _height, _codec_ctx->pix_fmt, _width,
							  _height, AV_PIX_FMT_RGB24, SWS_BILINEAR, nullptr,
							  nullptr, nullptr);

	glGenTextures(1, &_gl_texture);
	glBindTexture(GL_TEXTURE_2D, _gl_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, _width, _height, 0, GL_RGB,
				 GL_UNSIGNED_BYTE, nullptr);

	_time_base = av_q2d(_format_ctx->streams[_video_stream_index]->time_base);
}

auto Sorcery::VideoPlayer::free_resources() -> void {
	if (_packet) {
		av_packet_free(&_packet);
		_packet = nullptr;
	}
	if (_frame) {
		av_frame_free(&_frame);
		_frame = nullptr;
	}
	if (_rgb_frame) {
		av_frame_free(&_rgb_frame);
		_rgb_frame = nullptr;
	}
	if (_codec_ctx) {
		avcodec_free_context(&_codec_ctx);
		_codec_ctx = nullptr;
	}
	if (_format_ctx) {
		avformat_close_input(&_format_ctx);
		_format_ctx = nullptr;
	}
	if (_sws_ctx) {
		sws_freeContext(_sws_ctx);
		_sws_ctx = nullptr;
	}

	if (_gl_texture != 0) {
		glDeleteTextures(1, &_gl_texture);
		_gl_texture = 0;
	}

	_rgb_buffer.clear();

	_width = 0;
	_height = 0;
	_video_stream_index = -1;
	_time_base = 0.0;
	_next_pts_sec = 0.0;
	_has_frame_ready = false;
}

auto Sorcery::VideoPlayer::update(double playback_time) -> void {
	if (!_format_ctx || !_codec_ctx) {
		return; // nothing loaded
	}

	if (_has_frame_ready && playback_time < _next_pts_sec) {
		return;
	}

	while (av_read_frame(_format_ctx, _packet) >= 0) {
		if (_packet->stream_index == _video_stream_index) {
			if (avcodec_send_packet(_codec_ctx, _packet) == 0) {
				while (avcodec_receive_frame(_codec_ctx, _frame) == 0) {
					if (_frame->pts != AV_NOPTS_VALUE) {
						_next_pts_sec = _frame->pts * _time_base;
					}

					sws_scale(_sws_ctx, _frame->data, _frame->linesize, 0,
							  _height, _rgb_frame->data, _rgb_frame->linesize);

					glBindTexture(GL_TEXTURE_2D, _gl_texture);
					glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, _width, _height,
									GL_RGB, GL_UNSIGNED_BYTE,
									_rgb_frame->data[0]);

					_has_frame_ready = true;
					av_packet_unref(_packet);
					return;
				}
			}
		}
		av_packet_unref(_packet);
	}
}

auto Sorcery::VideoPlayer::render() -> void {
	if (!_has_frame_ready) {
		return;
	}

	ImGui::Begin("Video Player");
	ImGui::Image(
		(ImTextureID)(intptr_t)_gl_texture,
		ImVec2{static_cast<float>(_width), static_cast<float>(_height)});
	ImGui::End();
}