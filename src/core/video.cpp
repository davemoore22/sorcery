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

#include "core/video.hpp"

Sorcery::Video::Video() {

	textureID = 0;
	width = 0;
	height = 0;
	fmt_ctx = nullptr;
	codec_ctx = nullptr;
	sws_ctx = nullptr;
	frame = nullptr;
	frameRGBA = nullptr;
	buffer = nullptr;
	video_stream_index = -1;
	fps = 30.0;
}

auto Sorcery::Video::open(const char *filename) -> bool {
	avformat_open_input(&fmt_ctx, filename, nullptr, nullptr);
	avformat_find_stream_info(fmt_ctx, nullptr);

	// Find video stream
	for (unsigned i = 0; i < fmt_ctx->nb_streams; i++) {
		if (fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
			video_stream_index = i;
			break;
		}
	}
	if (video_stream_index == -1)
		return false;

	// Open codec
	const AVCodec *codec = avcodec_find_decoder(
		fmt_ctx->streams[video_stream_index]->codecpar->codec_id);
	codec_ctx = avcodec_alloc_context3(codec);
	avcodec_parameters_to_context(
		codec_ctx, fmt_ctx->streams[video_stream_index]->codecpar);
	avcodec_open2(codec_ctx, codec, nullptr);

	width = codec_ctx->width;
	height = codec_ctx->height;

	// FPS
	AVRational fr = fmt_ctx->streams[video_stream_index]->avg_frame_rate;
	fps = av_q2d(fr);

	// Allocate frames
	frame = av_frame_alloc();
	frameRGBA = av_frame_alloc();
	int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGBA, width, height, 1);
	buffer = (uint8_t *)av_malloc(numBytes * sizeof(uint8_t));
	av_image_fill_arrays(frameRGBA->data, frameRGBA->linesize, buffer,
						 AV_PIX_FMT_RGBA, width, height, 1);

	// SWS context for conversion
	sws_ctx = sws_getContext(width, height, codec_ctx->pix_fmt, width, height,
							 AV_PIX_FMT_RGBA, SWS_BILINEAR, nullptr, nullptr,
							 nullptr);

	// OpenGL texture
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
				 GL_UNSIGNED_BYTE, nullptr);

	return true;
}

auto Sorcery::Video::readFrame() -> bool {
	AVPacket packet;
	bool gotFrame = false;

	while (av_read_frame(fmt_ctx, &packet) >= 0) {
		if (packet.stream_index == video_stream_index) {
			if (avcodec_send_packet(codec_ctx, &packet) == 0) {
				while (avcodec_receive_frame(codec_ctx, frame) == 0) {
					// Convert to RGBA
					sws_scale(sws_ctx, frame->data, frame->linesize, 0, height,
							  frameRGBA->data, frameRGBA->linesize);

					// Upload to OpenGL texture
					glBindTexture(GL_TEXTURE_2D, textureID);
					glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height,
									GL_RGBA, GL_UNSIGNED_BYTE,
									frameRGBA->data[0]);
					gotFrame = true;
				}
			}
		}
		av_packet_unref(&packet);
		if (gotFrame)
			break;
	}

	// If we reached EOF → loop
	if (!gotFrame) {
		av_seek_frame(fmt_ctx, video_stream_index, 0, AVSEEK_FLAG_BACKWARD);
		avcodec_flush_buffers(codec_ctx);
	}

	return gotFrame;
}

auto Sorcery::Video::cleanup() -> void {
	if (textureID)
		glDeleteTextures(1, &textureID);
	if (buffer)
		av_free(buffer);
	if (frameRGBA)
		av_frame_free(&frameRGBA);
	if (frame)
		av_frame_free(&frame);
	if (codec_ctx)
		avcodec_free_context(&codec_ctx);
	if (fmt_ctx)
		avformat_close_input(&fmt_ctx);
	if (sws_ctx)
		sws_freeContext(sws_ctx);
};