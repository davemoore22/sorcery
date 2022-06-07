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

#include "automap.hpp"
#include "compass.hpp"
#include "console.hpp"
#include "dialog.hpp"
#include "display.hpp"
#include "game.hpp"
#include "graphics.hpp"
#include "iconpanel.hpp"
#include "inspect.hpp"
#include "main.hpp"
#include "menu.hpp"
#include "message.hpp"
#include "options.hpp"
#include "render.hpp"
#include "reorder.hpp"
#include "statusbar.hpp"
#include "system.hpp"

namespace Sorcery {

	class Engine {

	  public:
		// Standard Constructor - need to pass in game object to this to
		Engine(
			System *system, Display *display, Graphics *graphics, Game *game);
		Engine() = delete;

		// Standard Destructor
		~Engine();

		// Public Members

		// Public Methods
		auto start() -> int;
		auto stop() -> void;

	  private:
		// Private Methods
		auto _draw() -> void;
		auto _move_forward() -> bool;
		auto _move_backward() -> bool;
		auto _turn_left() -> void;
		auto _turn_right() -> void;
		auto _teleport_if() -> bool;
		auto _stairs_if() -> bool;

		// Private Members
		System *_system;
		Display *_display;
		Graphics *_graphics;
		Game *_game;
		sf::RenderWindow *_window;
		std::unique_ptr<Menu> _camp_menu;
		std::unique_ptr<Frame> _camp_menu_frame;
		std::unique_ptr<StatusBar> _status_bar;
		std::unique_ptr<Dialog> _confirm_exit;
		std::unique_ptr<Dialog> _ouch;
		std::unique_ptr<Reorder> _reorder;
		std::unique_ptr<Inspect> _inspect;
		std::unique_ptr<Render> _render;
		std::unique_ptr<AutoMap> _automap;
		std::unique_ptr<Compass> _compass;
		std::unique_ptr<Console> _console;
		std::unique_ptr<IconPanel> _left_icon_panel;
		std::unique_ptr<IconPanel> _right_icon_panel;
		std::unique_ptr<Message> _tile_note;
		std::unique_ptr<Dialog> _confirm_stairs;
		bool _in_camp;
		bool _in_character;
		bool _show_confirm_exit;
		bool _show_confirm_stairs;
		bool _show_console;
		bool _show_ouch;
		bool _show_tile_note;
		bool _update_automap;
		bool _update_compass;
		bool _update_icon_panels;
		bool _update_status_bar;
		bool _update_render;
		bool _update_tile_note;
		bool _exit_maze_now;
		std::optional<Character *> _cur_char;
		std::unique_ptr<Frame> _cur_char_frame;
	};
} // namespace Sorcery
