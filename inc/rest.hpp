// Copyright (C) 2021 Dave Moore
//
// This file is part of Sorcery: Shadows under Llylgamyn
//
// Sorcery is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Sorcery is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with sorcery-sfml.  If not, see <http://www.gnu.org/licenses/>.
// Additional permission under GNU GPL version 3 section 7:
//
// If you modify this Program, or any covered work, by linking or combining it
// with the libraries referred to in README (or a modified version of said
// libraries), containing parts covered by the terms of said libraries, the
// licensors of this Program grant you additional permission to convey the
// resulting work.

#pragma once

#include "character.hpp"
#include "display.hpp"
#include "graphics.hpp"
#include "main.hpp"
#include "menu.hpp"
#include "statusbar.hpp"
#include "system.hpp"

namespace Sorcery {

class Rest {

	public:

		// Standard Constructor
		Rest(System *system, Display *display, Graphics *graphics, Game *game);
		Rest() = delete;

		// Public Members

		// Public Methods
		auto start(Character *character, RestMode mode, RestType type) -> std::optional<MenuItem>;
		auto stop() -> void;

	private:

		// Private Methods
		auto _draw() -> void;
		auto _go_to_results() -> void;

		// Private Members
		System *_system;
		Display *_display;
		Graphics *_graphics;
		Game *_game;
		Character *_character;
		RestMode _mode;
		RestType _type;
		RestStage _stage;
		std::unique_ptr<StatusBar> _status_bar;
		sf::RenderWindow *_window;
		std::optional<std::chrono::time_point<std::chrono::system_clock>> _start;
		std::optional<std::chrono::time_point<std::chrono::system_clock>> _current_time;
		unsigned int _duration;
		std::unique_ptr<Menu> _menu;
		sf::Text _nap_text;
		sf::Text _no_level_text_1;
		sf::Text _no_level_text_2;
		std::string _nap_message;
		std::string _no_level_message_1;
		std::string _no_level_message_2;
		std::string _string;
		std::vector<std::string> _strings;
		std::vector<sf::Text> _texts;
		bool _level_up;
};

}