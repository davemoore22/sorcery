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
#include "modules/enum.hpp"

namespace Sorcery {

// Forward Declarations
class Character;
class CharacterDisplay;
class CharacterPanel;
class Dialog;
class Display;
class Edit;
class Factory;
class Frame;
class Game;
class Graphics;
class Menu;
class System;

class Roster {

	public:

		// Standard Constructor
		Roster(System *system, Display *display, Graphics *graphics, Game *game,
			ROM mode);
		Roster() = delete;

		// Standard Destructor
		~Roster();

		// Public Members

		// Public Methods
		auto start() -> std::optional<MIM>;
		auto stop() -> void;

	private:

		// Private Methods
		auto _draw() -> void;

		// Private Members
		System *_system;
		Display *_display;
		Graphics *_graphics;
		Game *_game;
		sf::RenderWindow *_window;
		std::unique_ptr<Menu> _menu;
		sf::Sprite _bg;
		std::unique_ptr<Frame> _menu_frame;
		std::optional<Character *> _cur_char;
		std::unique_ptr<Frame> _char_frame;
		ROM _mode;
		std::string _screen_key;
		std::unique_ptr<CharacterPanel> _char_panel;
		int _cur_char_id;
		std::unique_ptr<Frame> _preview_frame;
		std::unique_ptr<Dialog> _delete;
		std::unique_ptr<Edit> _edit;
		std::unique_ptr<CharacterDisplay> _character_display;
		std::unique_ptr<Factory> _factory;
};

}