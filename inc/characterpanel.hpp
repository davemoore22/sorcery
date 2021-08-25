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

// clang-format off
#include "display.hpp"
#include "graphics.hpp"
#include "layout.hpp"
#include "main.hpp"
#include "system.hpp"
#include "character.hpp"
// clang-format on

namespace Sorcery {

	class Display;

	class CharacterPanel : public sf::Transformable, public sf::Drawable {

	  public:
		// Constructors
		CharacterPanel(System *system, Display *display, Graphics *graphics);
		CharacterPanel() = delete;

		// Public Members
		bool valid;
		std::map<std::string, sf::Sprite> sprites;
		std::map<std::string, sf::Text> texts;
		std::map<std::string, std::shared_ptr<Frame>> frames;

		// Public Methods
		auto set(Character *character) -> void;

	  private:
		// Private Methods
		auto virtual draw(
			sf::RenderTarget &target, sf::RenderStates states) const -> void;

		// Private Members
		System *_system;
		Display *_display;
		Graphics *_graphics;
		Character *_character;
		Component _layout;

		std::vector<sf::Text> _texts;
		std::vector<sf::Sprite> _icons;
		sf::Sprite _portrait;
	};
} // namespace Sorcery
