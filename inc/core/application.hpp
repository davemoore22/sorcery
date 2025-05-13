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

#include "common/define.hpp"
#include "common/include.hpp"
#include "core/include.hpp"

namespace Sorcery {

// Forward Declarations
class Animation;
class Bestiary;
class Castle;
class Controller;
class Display;
class EdgeOfTown;
class Engine;
class Game;
class MainMenu;
class Resources;
class Splash;
class System;
class UI;

// Application Class
class Application {

	public:
		// Standard Constructor
		Application(int argc, char **argv);
		Application() = delete;

		// Standard Destructor
		~Application();

		// Public Methods
		auto start() -> int;
		auto stop() -> void;

	private:
		// Private Methods
		auto _add_quickstart_party() -> bool;
		auto _check_param(std::string_view param) const -> bool;
		auto _get_exe_path() const -> std::string_view;
		auto _load_existing_game() -> void;
		auto _start_new_game(const bool quickstart) -> void;
		auto _continue_existing_game() -> int;
		auto _do_start_expedition(const int mode) -> int;
		auto _do_restart_expedition(const int mode) -> int;
		auto _do_town() -> int;

		// Private Members
		std::vector<std::string> _args;
		std::unique_ptr<System> _system;
		std::unique_ptr<Resources> _resources;
		std::unique_ptr<Display> _display;
		std::unique_ptr<Controller> _controller;
		std::unique_ptr<UI> _ui;
		std::unique_ptr<MainMenu> _main_menu;
		std::unique_ptr<Splash> _splash;
		std::unique_ptr<Game> _game;
		std::unique_ptr<Castle> _castle;
		std::unique_ptr<EdgeOfTown> _edge_of_town;
		std::unique_ptr<Engine> _engine;
};
}