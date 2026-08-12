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

#include "common/opengl.hpp"

namespace Sorcery {

class FrameBuffer final {

	public:
		FrameBuffer() = default;
		FrameBuffer(const int width, const int height);
		~FrameBuffer();

		FrameBuffer(const FrameBuffer &) = delete;
		auto operator=(const FrameBuffer &) -> FrameBuffer & = delete;

		FrameBuffer(FrameBuffer &&) = delete;
		auto operator=(FrameBuffer &&) -> FrameBuffer & = delete;

		auto create(const int width, const int height) -> void;
		auto resize(const int width, const int height) -> void;
		auto destroy() -> void;

		auto bind() const -> void;
		static auto unbind() -> void;

		[[nodiscard]]
		auto texture() const -> GLuint;

		[[nodiscard]]
		auto width() const -> int;

		[[nodiscard]]
		auto height() const -> int;

	private:
		GLuint _fbo{};
		GLuint _texture{};

		int _width{};
		int _height{};
};

}