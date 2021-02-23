// Copyright (C) 2020 Dave Moore
//
// This file is part of Sorcery: Dreams of the Mad Overlord.
//
// Sorcery: Dreams of the Mad Overlord is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Sorcery: Dreams of the Mad Overlord is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Sorcery: Dreams of the Mad Overlord.  If not, see <http://www.gnu.org/licenses/>.
//
// If you modify this Program, or any covered work, by linking or combining it
// with the libraries referred to in README (or a modified version of said
// libraries), containing parts covered by the terms of said libraries, the
// licensors of this Program grant you additional permission to convey the
// resulting work.

#pragma once

#include "main.hpp"
#include "system.hpp"
#include "display.hpp"
#include "graphics.hpp"
#include "layout.hpp"
#include "menu.hpp"
#include "confirm.hpp"
#include "frame.hpp"
#include "attractmode.hpp"

namespace Sorcery {

	class MainMenu {

		public:

			// Constructors
			MainMenu(System& system, Display& display, Graphics& graphics);
			MainMenu() = delete;

			// Standard Destructor
			virtual ~MainMenu();

			// Public Members

			// Public Methods
			auto start(MainMenuType menu_stage) -> std::optional<MenuItem>;
			auto stop() -> void;

		private:

			// Private Methods
			auto _draw() -> void;
			auto _get_attract_mode() -> sf::Sprite;
			auto _get_creature_gfx(const int creature_id, const bool known) -> sf::Sprite;

			// Private Members
			System _system;
			Display _display;
			Graphics _graphics;
			sf::RenderWindow* _window;
			std::vector<unsigned int> _attract_mode_data;
			std::vector<unsigned int> _attract_mode_data_temp;
			unsigned int _creature_sprite_width;
			unsigned int _creature_sprite_height;
			unsigned int _creature_sprite_spacing;
			float _creature_sprite_scaling;
			sf::Texture _attract_mode_texture;
			MainMenuType _menu_stage;
			std::shared_ptr<Menu> _main_menu;
			std::shared_ptr<Confirm> _confirm_exit;
			std::shared_ptr<AttractMode> _attract_mode;
			WindowConfirm _yes_or_no;
			Component _attract_creatures_c;
			sf::Text _press_any_key;
	};
}
