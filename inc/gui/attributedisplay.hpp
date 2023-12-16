
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

// clang-format off
#include "core/display.hpp"
#include "core/graphics.hpp"
#include "resources/componentstore.hpp"
#include "main.hpp"
#include "core/system.hpp"
#include "types/character.hpp"

// clang-format on

namespace Sorcery {

class Character;

class AttributeDisplay: public sf::Transformable, public sf::Drawable {

	public:

		// Constructors
		AttributeDisplay(
			System *system, Display *display, Graphics *graphics, Character *character, Alignment alignment);
		AttributeDisplay() = default;

		// Copy Constructors
		AttributeDisplay(const AttributeDisplay &other);
		auto operator=(const AttributeDisplay &other) -> AttributeDisplay &;

		// Move Constructors
		AttributeDisplay(AttributeDisplay &&other) noexcept;
		auto operator=(AttributeDisplay &&other) noexcept -> AttributeDisplay &;

		// Public Members
		bool valid;
		std::map<std::string, sf::Sprite> sprites;
		std::map<std::string, sf::Text> texts;
		std::map<std::string, std::shared_ptr<Frame>> frames;

		// Public Methods
		auto set() -> void;

	private:

		// Private Methods
		auto virtual draw(sf::RenderTarget &target, sf::RenderStates states) const -> void;
		auto _get_bar(CharacterAttribute attribute) -> sf::RectangleShape;
		auto _get_text(CharacterAttribute attribute) -> sf::Text;

		// Private Members
		System *_system;
		Display *_display;
		Graphics *_graphics;
		Character *_character;

		Component _bar_c;
		Component _icons_c;
		Component _text_c;
		unsigned int _width;
		unsigned int _height;
		Alignment _alignment;

		std::vector<sf::RectangleShape> _bars;
		std::array<sf::Sprite, 6> _attribute_icons;
		std::vector<sf::Text> _texts;
};

}