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
#include "common/type.hpp"
#include "types/enum.hpp"

namespace Sorcery {

// Forward Declarations
class Character;
class Display;
class Game;
class Graphics;
class System;

class MenuPaged: public sf::Transformable, public sf::Drawable {

	public:

		MenuPaged(System *system, Display *display, Graphics *graphics,
			Game *game, const MTP type, const unsigned int page_size);
		MenuPaged() = delete;

		// Overload operators
		auto operator[](const unsigned int index) -> MenuEntry &;
		auto friend operator<<(std::ostream &out_stream, const MenuPaged &menu)
			-> std::ostream &;

		// Public Methods
		auto refresh_contents() -> void;
		auto get_item_count() const -> unsigned int;
		auto get_type() const -> MTP;
		auto get_page_size() const -> unsigned int;
		auto load_entries() -> unsigned int;
		auto print() -> void;

		// Public Members
		std::vector<MenuEntry> items;	   // Visible Items (a Window of _Items)
		std::vector<sf::FloatRect> bounds; // For Mouse Movement
		std::optional<unsigned int> selected; // Index of Selected Item

	private:

		// Private Methods
		auto virtual draw(
			sf::RenderTarget &target, sf::RenderStates states) const -> void;
		auto _add_item(const int index, const MIT item_type, const MIM code,
			std::string key, const bool enabled = true) -> void;
		auto _add_bestiary_creatures() -> void;
		auto _get_first_enabled() -> std::optional<unsigned int>;

		// Private Members
		System *_system;
		Display *_display;
		Graphics *_graphics;
		Game *_game;
		MTP _type;
		unsigned int _page_size;
		sf::RenderTexture _render_tx;
		sf::Texture *_tx;
		unsigned int _tx_width;
		unsigned int _tx_height;
		std::vector<sf::Text> _texts;
		std::vector<MenuEntry> _items;
		sf::RectangleShape _selected_bg;
		unsigned int _current_page;
		unsigned int _resized_item_count;
};

}
