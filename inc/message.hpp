// Copyright (C) 2021 Dave Moore
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

#include "display.hpp"
#include "frame.hpp"
#include "graphics.hpp"
#include "layout.hpp"
#include "main.hpp"
#include "system.hpp"

namespace Sorcery {

	class Message : public sf::Transformable, public sf::Drawable {

	  public:
		// Constructors
		Message(System *system, Display *display, Graphics *graphics, Component &frame_c, Component &text_c);
		Message() = delete;

		// Public Members

		// Public Methods
		auto update(TileNote tile_note) -> void;

	  private:
		// Private Methods
		auto virtual draw(sf::RenderTarget &target, sf::RenderStates states) const -> void;

		// Private Members
		System *_system;
		Display *_display;
		Graphics *_graphics;

		Component _frame_c;
		Component _text_c;

		TileNote _tile_note;

		sf::RenderWindow *_window;

		unsigned int _width;
		unsigned int _height;

		std::vector<std::string> _strings;
		std::vector<sf::Sprite> _sprites;
		std::vector<sf::Text> _texts;

		std::unique_ptr<Frame> _frame;
	};
} // namespace Sorcery
