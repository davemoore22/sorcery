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

#include "changename.hpp"
#include "charpanel.hpp"
#include "display.hpp"
#include "frame.hpp"
#include "game.hpp"
#include "graphics.hpp"
#include "layout.hpp"
#include "main.hpp"
#include "menu.hpp"
#include "system.hpp"

namespace Sorcery {

	class Edit {

	  public:
		// Standard Constructor
		Edit(System *system, Display *display, Graphics *graphics, Game *game);
		Edit() = delete;

		// Standard Destructor
		~Edit();

		// Public Members

		// Public Methods
		auto start(int current_character_idx) -> std::optional<MenuItem>;
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
		std::shared_ptr<Menu> _menu;
		sf::Sprite _bg;
		std::unique_ptr<Frame> _menu_frame;
		std::optional<Character *> _current_character;
		std::unique_ptr<Frame> _current_character_frame;
		std::string _screen_key;
		int _current_character_idx;
		std::unique_ptr<CharPanel> _character_panel;
		std::unique_ptr<Frame> _preview_frame;
	};
} // namespace Sorcery