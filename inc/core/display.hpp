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

// Internal Includes
#include "common/include.hpp"
#include "core/include.hpp"
#include "resources/stringstore.hpp"

namespace Sorcery {

class System;
class Size;

class Display {

	public:
		explicit Display(System *system);

		auto get_SDL_window() -> SDL_Window *;
		auto get_SDL_window_size() -> Size;
		auto get_GL_context() -> SDL_GLContext;
		auto get_GLSL_version() const -> const char *;

	private:
		auto initialise_SDL() -> int;

		System *_system;
		SDL_Window *_SDL_window;
		SDL_GLContext _GL_context;
		std::string _GLSL_version;
		SDL_WindowFlags _SDL_window_flags;
};

}