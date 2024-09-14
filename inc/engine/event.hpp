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
#include "gui/frame.hpp"
#include "gui/menu.hpp"
#include "types/component.hpp"

namespace Sorcery {

// Forward Declarations
class Display;
class Game;
class Graphics;
class System;

class Event {

	public:

		// Standard Constructor
		Event(System *system, Display *display, Graphics *graphics, Game *game, MapEvent type, unsigned int stage = 1);
		Event() = delete;

		// Standard Destructor
		~Event() = default;

		// Public Members

		// Public Methods
		auto start() -> std::optional<MI>;
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
		sf::Sprite _bg;
		std::unique_ptr<Frame> _frame;
		MapEvent _type;
		unsigned int _stage;
		std::map<std::string, sf::Sprite> _sprites;
		std::map<std::string, sf::Text> _texts;
		std::map<std::string, std::shared_ptr<Frame>> _frames;
		std::unique_ptr<Menu> _continue_menu;
		DungeonEvent _dungeon_event;
		Component _event_layout;
		std::optional<sf::Sprite> _icon;
};

}