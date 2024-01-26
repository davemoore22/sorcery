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
#include "types/component.hpp"

namespace Sorcery {

// Forward Declarations
class Display;
class Graphics;
class System;

class SpellPanel: public sf::Transformable, public sf::Drawable {

	public:

		// Constructors
		SpellPanel(System *system, Display *display, Graphics *graphics);
		SpellPanel() = delete;

		// Public Members
		auto set(Spell spell) -> void;
		bool valid;

	private:

		// Private Methods
		auto virtual draw(sf::RenderTarget &target, sf::RenderStates states) const -> void;

		// Private Members
		System *_system;
		Display *_display;
		Graphics *_graphics;
		Component _layout;
		std::vector<std::string> _strings;
		std::vector<sf::Text> _texts;
		sf::Sprite _icon;
		unsigned int _width;
		unsigned int _height;
};

}
