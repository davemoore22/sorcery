// Copyright (C) 2021 Dave Moore
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

#include "character.hpp"
#include "dialog.hpp"
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

	class ChangeClass {

	  public:
		// Standard Constructor
		ChangeClass(System *system, Display *display, Graphics *graphics, Character *character);
		ChangeClass() = delete;

		// Standard Destructor
		~ChangeClass();

		// Public Members

		// Public Methods
		auto start() -> std::optional<CharacterClass>;
		auto stop() -> void;

	  private:
		// Private Methods
		auto _draw() -> void;
		auto _set_classes_menu() -> void;
		auto _set_info_panel_contents(std::vector<Sorcery::MenuEntry>::const_iterator it) -> void;

		// Private Members
		System *_system;
		Display *_display;
		Graphics *_graphics;
		sf::RenderWindow *_window;
		Character *_character;
		sf::Sprite _bg;
		std::unique_ptr<Frame> _frame;
		std::unique_ptr<Menu> _menu;
		std::unique_ptr<Dialog> _confirm;
		std::unique_ptr<Dialog> _not_changed;
		std::unique_ptr<InfoPanel> _ip;
		bool _valid;
		bool _show_confirm;
		bool _show_not_changed;
		std::optional<CharacterClass> _new_class;
	};
} // namespace Sorcery
