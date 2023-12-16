// Copyright (C) 2023 Dave Moore
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

#include "core/display.hpp"
#include "core/graphics.hpp"
#include "core/layout.hpp"
#include "core/system.hpp"
#include "gui/frame.hpp"
#include "main.hpp"

namespace Sorcery {

class License {

	public:

		// Constructor
		License(System *system, Display *display, Graphics *graphics);

		// Destructor
		~License();

		// Public Members

		// Public Methods
		auto start() -> int;
		auto stop() -> void;

	private:

		// Private Methods
		auto _draw() -> void;
		auto _display_file() -> void;

		// Private Members
		System *_system;
		Display *_display;
		Graphics *_graphics;
		sf::RenderWindow *_window;

		std::shared_ptr<TextFile> _textfile;
		unsigned int _current_line;
		sf::Text _progress_text;
		sf::Text _line_text;
};

}
