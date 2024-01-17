// Copyright (C) 2024 Dave Moore
//
// This file is part of Sorcery: Shadows under Llylgamyn.
//
// Sorcery: Shadows under Llylgamyn is free software: you can redistribute
// it and/or modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation, either version 2 of the License,
// or (at your option) any later version.
//
// Sorcery: Shadows under Llylgamyn is distributed in the hope that it wil
// be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
// Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Sorcery: Shadows under Llylgamyn.  If not,
// see <http://www.gnu.org/licenses/>.
//
// If you modify this program, or any covered work, by linking or combining
// it with the libraries referred to in README (or a modified version of
// said libraries), containing parts covered by the terms of said libraries,
// the licensors of this program grant you additional permission to convey
// the resulting work.

#pragma once

#include "common/include.hpp"
#include "core/display.hpp"
#include "core/graphics.hpp"
#include "core/system.hpp"
#include "frontend/define.hpp"
#include "gui/frame.hpp"

namespace Sorcery {

class Banner {

	public:

		// Constructors
		Banner(System *system, Display *display, Graphics *graphics);
		Banner() = delete;

		// Public Members

		// Public Methods
		auto start() -> int;

	private:

		// Private Methods
		auto _draw() -> void;
		auto _update() -> void;

		// Private Members
		System *_system;
		Display *_display;
		Graphics *_graphics;
		sf::RenderWindow *_window;
		unsigned int _alpha;
		bool _fading_in;
		bool _fading_out;
		bool _finished;
};

}