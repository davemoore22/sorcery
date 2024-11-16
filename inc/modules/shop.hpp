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
class Console;
class Display;
class Frame;
class Game;
class Graphics;
class Inspect;
class Menu;
class PartyPanel;
class Sell;
class System;

class Shop {

	public:

		// Standard Constructor
		Shop(System *system, Display *display, Graphics *graphics, Game *game);
		Shop() = delete;

		// Standard Destructor
		~Shop();

		// Public Members

		// Public Methods
		auto start() -> std::optional<MIM>;
		auto stop() -> void;

	private:

		// Private Methods
		auto _draw() -> void;
		auto _update_welcome() -> void;

		// Private Members
		System *_system;
		Display *_display;
		Graphics *_graphics;
		Game *_game;
		sf::RenderWindow *_window;
		std::unique_ptr<Sell> _sell;
		std::unique_ptr<Frame> _frame;
		std::unique_ptr<Menu> _menu;
		std::unique_ptr<Menu> _who;
		std::unique_ptr<Menu> _action;
		sf::Sprite _bg;
		STS _stage;
		std::unique_ptr<PartyPanel> _party_panel;
		std::unique_ptr<Inspect> _inspect;
		std::unique_ptr<Console> _console;
		bool _show_console;
		std::map<std::string, sf::Sprite> _w_sprites;
		std::map<std::string, sf::Text> _w_texts;
		std::map<std::string, std::shared_ptr<Frame>> _w_frames;
		std::map<std::string, sf::Sprite> _a_sprites;
		std::map<std::string, sf::Text> _a_texts;
		std::map<std::string, std::shared_ptr<Frame>> _a_frames;
		unsigned int _chosen_char_id;
		sf::Text _action_gold;
		sf::Text _action_name;
		std::string _gold;
		std::string _name;
};

}
