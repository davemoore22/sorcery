// Copyright (C) 2020 Dave Moore
//
// This file is part of Sorcery: Dreams of the Mad Overlord.
//
// Sorcery: Dreams of the Mad Overlord is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Sorcery: Dreams of the Mad Overlord is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Sorcery: Dreams of the Mad Overlord.  If not, see <http://www.gnu.org/licenses/>.
//
// If you modify this Program, or any covered work, by linking or combining it
// with the libraries referred to in README (or a modified version of said
// libraries), containing parts covered by the terms of said libraries, the
// licensors of this Program grant you additional permission to convey the
// resulting work.

#pragma once

#include "main.hpp"
#include "system.hpp"
#include "display.hpp"
#include "graphics.hpp"
#include "layout.hpp"
#include "frame.hpp"

namespace Sorcery {

	class License {

		public:

			// Constructor
			License(System& system, Display& display, Graphics& graphics);

			// Destructor
			~License();

			// Public Members

			// Public Methods
			auto start() -> void;
			auto stop() -> void;

		private:

			// Private Methods
			auto _draw() -> void;
			auto _display_file_contents() -> void;

			// Private Members
			System _system;
			Display _display;
			Graphics _graphics;
			sf::RenderWindow* _window;
			sf::Font _mono_system_font;
			sf::Font _proportional_system_font;
			sfe::Movie _background_movie;
			sf::Sprite _cursor;
			std::shared_ptr<TextFile> _textfile;
			unsigned int _current_line;
			sf::Text _progress_text;
			sf::Text _line_text;
			std::unique_ptr<Frame> _outside_frame;
			std::unique_ptr<Frame> _title_frame;
	};
}
