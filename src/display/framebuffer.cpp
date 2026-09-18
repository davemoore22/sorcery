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
#include <core/debug.hpp>
#include <print>
#include <stdexcept>
#include <string_view>

namespace {

auto gl_error_name(const GLenum error) -> std::string_view {

	switch (error) {

	case GL_INVALID_ENUM:
		return "GL_INVALID_ENUM";

	case GL_INVALID_VALUE:
		return "GL_INVALID_VALUE";

	case GL_INVALID_OPERATION:
		return "GL_INVALID_OPERATION";

	case GL_INVALID_FRAMEBUFFER_OPERATION:
		return "GL_INVALID_FRAMEBUFFER_OPERATION";

	case GL_OUT_OF_MEMORY:
		return "GL_OUT_OF_MEMORY";

	default:
		return "UNKNOWN_GL_ERROR";
	}
}

auto framebuffer_status_name(const GLenum status) -> std::string_view {

	switch (status) {

	case GL_FRAMEBUFFER_COMPLETE:
		return "GL_FRAMEBUFFER_COMPLETE";

	case GL_FRAMEBUFFER_UNDEFINED:
		return "GL_FRAMEBUFFER_UNDEFINED";

	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
		return "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";

	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
		return "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";

	case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
		return "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER";

	case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
		return "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER";

	case GL_FRAMEBUFFER_UNSUPPORTED:
		return "GL_FRAMEBUFFER_UNSUPPORTED";

	case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
		return "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE";

	case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
		return "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS";

	default:
		return "UNKNOWN_FRAMEBUFFER_STATUS";
	}
}

auto log_gl_errors(const std::string_view where) -> void {

	for (auto error{glGetError()}; error != GL_NO_ERROR; error = glGetError()) {

		DEBUG_LOGF("OpenGL error at {}: {} (0x{:04x})", where, gl_error_name(error), static_cast<unsigned int>(error));
	}
}

} // namespace

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

	GLint max_texture_size{};
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texture_size);
	DEBUG_LOGF("DISPLAY Creating framebuffer: {}x{} (GL_MAX_TEXTURE_SIZE={})", _width, _height, max_texture_size);

	if (_width > max_texture_size || _height > max_texture_size) {
		DEBUG_LOGF("Framebuffer dimensions exceed GL_MAX_TEXTURE_SIZE: {}x{} > {}", _width, _height, max_texture_size);
		throw std::runtime_error{"Framebuffer dimensions exceed GL_MAX_TEXTURE_SIZE."};
	}

	// Create colour texture
	glGenTextures(1, &_texture);
	glBindTexture(GL_TEXTURE_2D, _texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

	log_gl_errors("FrameBuffer::create glTexImage2D");

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Create framebuffer
	glGenFramebuffers(1, &_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texture, 0);

	const auto status{glCheckFramebufferStatus(GL_FRAMEBUFFER)};

	DEBUG_LOGF("DISPLAY Framebuffer status: {} (0x{:04x}), fbo={}, texture={}", framebuffer_status_name(status),
			   static_cast<unsigned int>(status), _fbo, _texture);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	log_gl_errors("FrameBuffer::create");

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

	DEBUG_LOGF("DISPLAY Resizing framebuffer: {}x{} -> {}x{}", _width, _height, width, height);

	_width = width;
	_height = height;

	GLint max_texture_size{};
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texture_size);

	if (_width > max_texture_size || _height > max_texture_size) {

		DEBUG_LOGF("Framebuffer resize exceeds GL_MAX_TEXTURE_SIZE: {}x{} > {}", _width, _height, max_texture_size);

		throw std::runtime_error{"Framebuffer dimensions exceed GL_MAX_TEXTURE_SIZE."};
	}

	glBindTexture(GL_TEXTURE_2D, _texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

	glBindTexture(GL_TEXTURE_2D, 0);

	log_gl_errors("FrameBuffer::resize glTexImage2D");

	glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

	const auto status{glCheckFramebufferStatus(GL_FRAMEBUFFER)};
	if (status != GL_FRAMEBUFFER_COMPLETE) {

		DEBUG_LOGF("Framebuffer incomplete after resize: {} (0x{:04x})", framebuffer_status_name(status),
				   static_cast<unsigned int>(status));

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		throw std::runtime_error{"Framebuffer became incomplete after resize."};
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

auto Sorcery::FrameBuffer::destroy() -> void {

	if (_texture != 0) {

		DEBUG_LOGF("DISPLAY Deleting framebuffer texture: {}", _texture);

		glDeleteTextures(1, &_texture);
		_texture = 0;
	}

	if (_fbo != 0) {

		DEBUG_LOGF("DISPLAY Deleting framebuffer object: {}", _fbo);

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