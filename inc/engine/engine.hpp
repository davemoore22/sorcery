// Copyright (C) 2025 Dave Moore
//
// This file is part of Sorcery.
//
// Sorcery is free software: you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software
// Foundation, either version 2 of the License, or (at your option) any later
// version.
//
// Sorcery is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with
// Sorcery.  If not, see <http://www.gnu.org/licenses/>.
//
// If you modify this program, or any covered work, by linking or combining
// it with the libraries referred to in README (or a modified version of
// said libraries), containing parts covered by the terms of said libraries,
// the licensors of this program grant you additional permission to convey
// the resulting work.

#pragma once

#include "common/include.hpp"
#include "common/types.hpp"
#include "engine/enum.hpp"
#include "engine/type.hpp"
#include "frontend/options.hpp"
#include "modules/inspect.hpp"
#include "modules/reorder.hpp"

namespace Sorcery {

// Forward Declarations
class Character;
class Controller;
class Display;
class Game;
class System;
class UI;

class Engine {

	public:
		// Standard Constructor
		Engine(System *system, Display *display, UI *ui,
			   Controller *controller);
		Engine() = delete;

		// Standard Destructor
		~Engine();

		// Public Members

		// Public Methods
		auto start(Game *game, const int mode) -> int;
		auto stop() -> void;

	private:
		// Private Methods
		static auto _callback_stop_popup_ouch(Uint32, void *param) -> Uint32;
		auto _initialise() -> bool;
		auto _go_back_to_town() -> int;
		auto _go_down_a_level() -> void;
		auto _go_up_a_level() -> void;
		auto _go_to_location(const int depth, const Coordinate loc,
							 const Enums::Map::Direction dir) -> void;
		auto _move_backward() -> bool;
		auto _move_forward() -> bool;
		auto _set_tile_explored(const Coordinate loc) -> void;
		auto _start_expedition(const int mode) -> bool;
		auto _start_popup_ouch() -> void;
		auto _tile_explored(const Coordinate loc) const -> bool;
		auto _turn_around() -> void;
		auto _turn_left() -> void;
		auto _turn_right() -> void;

		// Private Members
		System *_system;
		Display *_display;
		UI *_ui;
		Controller *_controller;
		Game *_game;
		std::unique_ptr<Options> _options;
		std::unique_ptr<Reorder> _reorder;
		std::unique_ptr<Inspect> _inspect;
};

}
