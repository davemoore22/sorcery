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

#include "core/display.hpp"
#include "core/system.hpp"

Sorcery::Display::Display(System *system)
	: _system{system} {

	initialise_SDL();
};

auto Sorcery::Display::initialise_SDL() -> int {

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) !=
		0) {
		std::cout << "Error: " << SDL_GetError() << std::endl;
		return -1;
	}

	// Using GL 3.0 + GLSL 130
	_GLSL_version = "#version 130";
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
						SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

	// Create SDL Window with an Open GL Graphics Context
	auto window_title{(*_system->strings)["WINDOW_TITLE"]};
	auto window_w{std::stoi((*_system->config).get("Window", "width"))};
	auto window_h{std::stoi((*_system->config).get("Window", "height"))};
	auto min_window_w{std::stoi((*_system->config).get("Window", "min_width"))};
	auto min_window_h{
		std::stoi((*_system->config).get("Window", "min_height"))};
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

	_SDL_window_flags =
		(SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI |
						  SDL_WINDOW_RESIZABLE);
	_SDL_window = SDL_CreateWindow(window_title.c_str(), SDL_WINDOWPOS_CENTERED,
								   SDL_WINDOWPOS_CENTERED, window_w, window_h,
								   _SDL_window_flags);

	SDL_SetWindowMinimumSize(_SDL_window, min_window_w, min_window_h);
	// SDL_MaximizeWindow(_SDL_window);
	_GL_context = SDL_GL_CreateContext(_SDL_window);
	SDL_GL_MakeCurrent(_SDL_window, _GL_context);
	SDL_GL_SetSwapInterval(1); // Enable vsync

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