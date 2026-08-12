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

#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#include <imgui.h>

#include "core/framebuffer.hpp"

namespace Sorcery {

struct Context;
class Size;

struct DisplayMetrics {

		int window_w{};
		int window_h{};

		int drawable_w{};
		int drawable_h{};

		float framebuffer_scale_x{1.0f};
		float framebuffer_scale_y{1.0f};

		float scale_x{1.0f};
		float scale_y{1.0f};
		float scale{1.0f};

		float offset_x{};
		float offset_y{};
};

class Display {

	public:
		Display(Context &ctx);

		auto get_SDL_window() -> SDL_Window *;
		auto get_SDL_window_size() -> Size;
		auto get_GL_context() -> SDL_GLContext;
		auto get_GLSL_version() const -> const char *;
		auto present(ImDrawData *draw_data) -> void;
		auto resize() -> void;
		auto update_display_metrics() noexcept -> void;
		auto get_display_metrics() const noexcept -> const DisplayMetrics &;

		// Simple OpenGL Shaders
		auto set_fade(float fade) -> void;

	private:
		auto initialise_SDL() -> int;

		Context &_ctx;
		SDL_Window *_SDL_window;
		SDL_GLContext _GL_context;
		std::string _GLSL_version;
		SDL_WindowFlags _SDL_window_flags;
		DisplayMetrics _metrics;
		int _base_window_w;
		int _base_window_h;
		FrameBuffer _framebuffer;
		GLuint _post_program{};
		GLuint _post_vao{};
		float _fade{};
};

}