// Copyright (C) 2021 Dave Moore
//
// This file is part of Sorcery: Dreams of the Mad Overlord.
//
// Sorcery: Dreams of the Mad Overlord is free software: you can redistribute
// it and/or modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation, either version 2 of the License,
// or (at your option) any later version.
//
// Sorcery: Dreams of the Mad Overlord is distributed in the hope that it wil
// be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
// Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Sorcery: Dreams of the Mad Overlord.  If not,
// see <http://www.gnu.org/licenses/>.
//
// If you modify this program, or any covered work, by linking or combining
// it with the libraries referred to in README (or a modified version of
// said libraries), containing parts covered by the terms of said libraries,
// the licensors of this program grant you additional permission to convey
// the resulting work.

#pragma once

#include "character.hpp"
#include "display.hpp"
#include "frame.hpp"
#include "graphics.hpp"
#include "infopanel.hpp"
#include "keyboard.hpp"
#include "layout.hpp"
#include "main.hpp"
#include "menu.hpp"
#include "system.hpp"

namespace Sorcery {

	class Create {

	  public:
		// Constructor
		Create(System &system, Display &display, Graphics &graphics);
		Create() = delete;

		// Destructor
		~Create();

		// Public Members

		// Public Methods
		auto start() -> std::optional<MenuItem>;
		auto stop() -> void;

	  private:
		// Private Methods
		auto _draw() -> void;
		auto _do_event_loop() -> std::optional<ModuleResult>;
		auto _handle_input(const sf::Event &event) -> std::optional<ModuleResult>;
		auto _generate_character(const sf::Event &event) -> std::optional<ModuleResult>;
		auto _go_to_next_stage() -> void;
		auto _go_to_previous_stage() -> void;
		auto _set_info_panel_contents(std::vector<Sorcery::MenuEntry>::const_iterator it) -> void;

		// Private Members
		System _system;
		Display _display;
		Graphics _graphics;
		sf::RenderWindow *_window;
		sf::Sprite _bg;
		std::shared_ptr<Character> _candidate;
		Component _name_c;
		Component _keyb_c;
		Component _ip_race_c;
		Component _ip_alignment_c;
		std::shared_ptr<Keyboard> _keyboard;
		std::shared_ptr<Menu> _race_menu;
		std::shared_ptr<Menu> _alignment_menu;
		std::shared_ptr<InfoPanel> _ip;
	};
} // namespace Sorcery
