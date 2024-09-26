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
#include "gui/frame.hpp"
#include "types/component.hpp"
#include "types/item.hpp"

namespace Sorcery {

// Forward Declarations
class Display;
class Game;
class Graphics;
class ItemStore;
class System;

class ItemDisplay: public sf::Transformable, public sf::Drawable {

	public:

		// Constructors
		ItemDisplay(
			System *system, Display *display, Graphics *graphics, Game *game);
		ItemDisplay() = delete;

		// Public Members
		int width;
		int height;
		bool valid;

		// Public Methods
		auto set(const unsigned int item_idx) -> void;

	private:

		// Private Methods
		auto virtual draw(
			sf::RenderTarget &target, sf::RenderStates states) const -> void;
		auto _add_text(Component &component, std::string format,
			std::string value) -> sf::Text *;

		// Private Members
		System *_system;
		Display *_display;
		Graphics *_graphics;
		Game *_game;
		Component _layout;
		ItemStore *_itemstore;

		std::unique_ptr<Item> _item;
		std::unique_ptr<Frame> _frame;
		std::map<std::string, sf::Sprite> _sprites;
		std::map<std::string, sf::Text> _texts;
		std::map<std::string, std::shared_ptr<Frame>> _frames;
		std::array<sf::Sprite, 8> _class_icons;
		std::array<sf::Sprite, 3> _align_icons;
};

}