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

namespace Sorcery {

// Forward Declarations
class Character;
class CharacterDisplay;
class CharacterPanel;
class Display;
class Dialog;
class Frame;
class Game;
class Graphics;
class ItemDisplay;
class Menu;
class System;

class Inspect {

	public:

		// Standard Constructor
		Inspect(System *system, Display *display, Graphics *graphics, Game *game, MenuMode mode);
		Inspect() = delete;

		// Standard Destructor
		~Inspect();

		// Public Members

		// Public Methods
		auto start(std::optional<unsigned int> character_id) -> std::optional<MenuItem>;
		auto stop() -> void;

	private:

		// Private Methods
		auto _draw() -> void;
		auto _handle_in_character(unsigned int character_id) -> std::optional<MenuItem>;
		auto _set_in_item_action_menu(unsigned int character_id, unsigned int slot_id) -> void;

		auto _examine_item(const unsigned int character_id) -> void;

		// Private Members
		System *_system;
		Display *_display;
		Graphics *_graphics;
		Game *_game;
		bool _restricted;
		bool _in_character;
		sf::RenderWindow *_window;
		std::unique_ptr<Menu> _menu;
		sf::Sprite _bg;
		std::unique_ptr<Frame> _menu_frame;
		std::optional<Character *> _cur_char;
		std::unique_ptr<Frame> _cur_char_frame;
		std::unique_ptr<Dialog> _pool;
		std::unique_ptr<Dialog> _cursed;
		bool _in_pool;
		bool _in_cursed;
		MenuMode _mode;
		std::string _screen_key;
		std::unique_ptr<CharacterPanel> _char_panel;
		int _cur_char_id;
		std::unique_ptr<Frame> _preview_frame;
		std::unique_ptr<CharacterDisplay> _character_display;
		std::unique_ptr<Menu> _item_action_menu;
		std::unique_ptr<Frame> _item_action_menu_frame;
		bool _in_item_action;
		bool _in_item_display;
		std::unique_ptr<ItemDisplay> _item_display;
		sf::Sprite _item_display_gfx;
		std::unique_ptr<Frame> _item_display_frame;
};

}
