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

#include "core/display.hpp"
#include "core/game.hpp"
#include "core/graphics.hpp"
#include "core/system.hpp"
#include "gui/characterpanel.hpp"
#include "gui/frame.hpp"
#include "gui/menu.hpp"
#include "gui/partypanel.hpp"
#include "main.hpp"
#include "modules/create.hpp"
#include "modules/edit.hpp"
#include "resources/componentstore.hpp"

namespace Sorcery {

class Reorder {

	public:

		// Standard Constructor
		Reorder(System *system, Display *display, Graphics *graphics, Game *game, MenuMode mode = MenuMode::NO_MODE);
		Reorder() = delete;

		// Standard Destructor
		~Reorder();

		// Public Members

		// Public Methods
		auto start() -> std::optional<std::vector<unsigned int>>;
		auto stop() -> void;

	private:

		// Private Methods
		auto _draw() -> void;
		auto _populate_candidate() -> void;

		// Private Members
		System *_system;
		Display *_display;
		Graphics *_graphics;
		Game *_game;
		sf::RenderWindow *_window;
		std::unique_ptr<Menu> _menu;
		sf::Sprite _bg;
		std::unique_ptr<PartyPanel> _party_panel;
		std::unique_ptr<Frame> _menu_frame;
		std::string _screen_key;
		int _cur_char_id;
		std::vector<unsigned int> _candidate_party;
		Component _candidate_c;
		std::vector<sf::Text> _texts;
		MenuMode _mode;
};

}
