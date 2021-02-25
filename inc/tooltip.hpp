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

	class Display;

	class Tooltip: public sf::Transformable, public sf::Drawable {

	public:

			// Constructors
			Tooltip(System& system, Display& display, Graphics& graphics);
			Tooltip() = delete;

			// Public Members
			std::optional<std::chrono::time_point<std::chrono::steady_clock>> entry_time;
			sf::FloatRect bounding_rectangle;
			bool valid;

			// Public Methods
			auto set(const std::string& string) -> void;

		private:

			// Private Methods
			auto virtual draw(sf::RenderTarget& target, sf::RenderStates states) const -> void;

			// Private Members
			System _system;
			Display _display;
			Graphics _graphics;
			Component _layout;
			unsigned int _width;
			unsigned int _height;
			std::string _string;
			std::vector<std::string> _strings;
			std::vector<sf::Text> _texts;
			std::unique_ptr<Frame> _frame;
	};
}
