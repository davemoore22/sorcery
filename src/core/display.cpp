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

#include "core/display.hpp"
#include "common/types.hpp"

#include "common/opengl.hpp"
#include "core/context.hpp"
#include "core/debug.hpp"
#include "core/system.hpp"
#include "resources/stringstore.hpp"
#include "types/config.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>

#include <cstdlib>
#include <print>
#include <string>

Sorcery::Display::Display(Context &ctx)
	: _ctx{ctx} {

	initialise_SDL();
};

auto Sorcery::Display::initialise_SDL() -> int {

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) !=
		0) {

		std::println("SDL initialisation failed: {}", SDL_GetError());
		return -1;
	}

	_GLSL_version = "#version 130";

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
						SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

	const auto window_title{_ctx.get_string("WINDOW_TITLE")};

	_base_window_w = std::stoi(_ctx.get_config("Window", "width"));

	_base_window_h = std::stoi(_ctx.get_config("Window", "height"));

	const auto min_window_w{std::stoi(_ctx.get_config("Window", "min_width"))};

	const auto min_window_h{std::stoi(_ctx.get_config("Window", "min_height"))};

	_SDL_window_flags = static_cast<SDL_WindowFlags>(
		SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE);

	_SDL_window = SDL_CreateWindow(window_title.c_str(), SDL_WINDOWPOS_CENTERED,
								   SDL_WINDOWPOS_CENTERED, _base_window_w,
								   _base_window_h, _SDL_window_flags);

	if (_SDL_window == nullptr) {
		std::println("SDL window creation failed: {}", SDL_GetError());

		return -1;
	}

	SDL_SetWindowMinimumSize(_SDL_window, min_window_w, min_window_h);

	_GL_context = SDL_GL_CreateContext(_SDL_window);

	if (_GL_context == nullptr) {
		std::println("OpenGL context creation failed: {}", SDL_GetError());

		SDL_DestroyWindow(_SDL_window);
		_SDL_window = nullptr;

		return -1;
	}

	if (SDL_GL_MakeCurrent(_SDL_window, _GL_context) != 0) {
		std::println("Making OpenGL context current failed: {}",
					 SDL_GetError());

		SDL_GL_DeleteContext(_GL_context);
		_GL_context = nullptr;

		SDL_DestroyWindow(_SDL_window);
		_SDL_window = nullptr;

		return -1;
	}

	if (SDL_GL_SetSwapInterval(1) != 0) {
		std::println("Warning: unable to enable VSync: {}", SDL_GetError());
	}

	update_display_metrics();

	int gl_major{};
	int gl_minor{};
	int gl_profile{};

	SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &gl_major);

	SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &gl_minor);

	SDL_GL_GetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, &gl_profile);

	DEBUG_LOGF("OpenGL context: {}.{}, profile mask: {}", gl_major, gl_minor,
			   gl_profile);

	return 1;
}
auto Sorcery::Display::get_SDL_window() -> SDL_Window * {

	return _SDL_window;
}

auto Sorcery::Display::get_SDL_window_size() -> Size {

	int w{0}, h{0};
	SDL_GetWindowSize(_SDL_window, &w, &h);
	return Size{w, h};
}

auto Sorcery::Display::get_GL_context() -> SDL_GLContext {

	return _GL_context;
}

auto Sorcery::Display::get_GLSL_version() const -> const char * {

	return _GLSL_version.c_str();
}

auto Sorcery::Display::get_display_metrics() const noexcept
	-> const DisplayMetrics & {

	return _metrics;
}

auto Sorcery::Display::update_display_metrics() noexcept -> void {

	if (_SDL_window == nullptr)
		return;

	SDL_GetWindowSize(_SDL_window, &_metrics.window_w, &_metrics.window_h);

	SDL_GL_GetDrawableSize(_SDL_window, &_metrics.drawable_w,
						   &_metrics.drawable_h);

	if (_metrics.window_w <= 0 || _metrics.window_h <= 0)
		return;

	_metrics.framebuffer_scale_x = static_cast<float>(_metrics.drawable_w) /
								   static_cast<float>(_metrics.window_w);

	_metrics.framebuffer_scale_y = static_cast<float>(_metrics.drawable_h) /
								   static_cast<float>(_metrics.window_h);

	_metrics.scale_x = static_cast<float>(_metrics.window_w) /
					   static_cast<float>(_base_window_w);

	_metrics.scale_y = static_cast<float>(_metrics.window_h) /
					   static_cast<float>(_base_window_h);

	_metrics.scale = std::min(_metrics.scale_x, _metrics.scale_y);

	const auto content_w{static_cast<float>(_base_window_w) * _metrics.scale};

	const auto content_h{static_cast<float>(_base_window_h) * _metrics.scale};

	_metrics.offset_x =
		(static_cast<float>(_metrics.window_w) - content_w) / 2.0f;

	_metrics.offset_y =
		(static_cast<float>(_metrics.window_h) - content_h) / 2.0f;

	glViewport(0, 0, _metrics.drawable_w, _metrics.drawable_h);
}