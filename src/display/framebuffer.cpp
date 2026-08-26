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

#include "display/framebuffer.hpp"

#include <stdexcept>

Sorcery::FrameBuffer::FrameBuffer(const int width, const int height) {

	create(width, height);
}

Sorcery::FrameBuffer::~FrameBuffer() {

	destroy();
}

auto Sorcery::FrameBuffer::create(const int width, const int height) -> void {

	if (width <= 0 || height <= 0)
		throw std::runtime_error{"Invalid framebuffer dimensions."};

	destroy();

	_width = width;
	_height = height;

	// Create colour texture
	glGenTextures(1, &_texture);
	glBindTexture(GL_TEXTURE_2D, _texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, _width, _height, 0, GL_RGBA,
				 GL_UNSIGNED_BYTE, nullptr);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Create framebuffer
	glGenFramebuffers(1, &_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
						   _texture, 0);

	const auto status{glCheckFramebufferStatus(GL_FRAMEBUFFER)};

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	if (status != GL_FRAMEBUFFER_COMPLETE) {
		destroy();

		throw std::runtime_error{"Failed to create framebuffer."};
	}
}

auto Sorcery::FrameBuffer::resize(const int width, const int height) -> void {

	if (width <= 0 || height <= 0)
		return;

	if (width == _width && height == _height)
		return;

	_width = width;
	_height = height;

	glBindTexture(GL_TEXTURE_2D, _texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, _width, _height, 0, GL_RGBA,
				 GL_UNSIGNED_BYTE, nullptr);

	glBindTexture(GL_TEXTURE_2D, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		throw std::runtime_error{"Framebuffer became incomplete after resize."};
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

auto Sorcery::FrameBuffer::destroy() -> void {

	if (_texture != 0) {
		glDeleteTextures(1, &_texture);
		_texture = 0;
	}

	if (_fbo != 0) {
		glDeleteFramebuffers(1, &_fbo);
		_fbo = 0;
	}

	_width = 0;
	_height = 0;
}

auto Sorcery::FrameBuffer::bind() const -> void {

	glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
}

auto Sorcery::FrameBuffer::unbind() -> void {

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

auto Sorcery::FrameBuffer::texture() const -> GLuint {

	return _texture;
}

auto Sorcery::FrameBuffer::width() const -> int {

	return _width;
}

auto Sorcery::FrameBuffer::height() const -> int {

	return _height;
}