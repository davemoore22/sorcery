// Copyright (C) 2026 Dave Moore
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

#include <atomic>
#include <csignal>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "common/define.hpp"
#include "core/context.hpp"
#include "core/types.hpp"

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

		enum class GameBootstrap {
			NONE,
			LOAD_GAME,
			NEW_GAME,
		};

		enum class PartyMode {
			NONE,
			QUICKSTART,
		};

		enum class StartLocation {
			NONE,
			CASTLE,
			MAZE,
			MAZE_WITH_GOTO
		};

		enum class AppFlow {
			NONE,
			ABORT,
			QUIT,

			MAIN_MENU,
			NEW_GAME,
			CONTINUE_GAME,

			TOWN,
			CASTLE,
			EDGE_OF_TOWN,
			TRAINING,
			MAZE,
			MAZE_WITH_GOTO,
			RESTART_MAZE,
			LEAVE_GAME
		};

		struct StartupPlan {
				bool bypass_menu{false};
				GameBootstrap bootstrap{GameBootstrap::NONE};
				PartyMode party{PartyMode::NONE};
				StartLocation location{StartLocation::CASTLE};
				bool go_to{false};
		};

	public:
		// Standard Constructor
		Application(int argc, char **argv);

		// Standard Destructor
		~Application();

		// Public Methods
		auto start() -> int;
		auto stop() -> void;
		auto save_state_to_binary(const std::string &filename) -> bool;
		auto load_state_from_binary(const std::string &filename) -> bool;
		auto get_resources() const -> Resources *;
		auto update() -> void;
		static auto signal_shutdown_requested() -> bool;
		static auto install_signal_handlers() -> void;

		Context ctx;

	private:
		// Private Methods
		auto _flow_from_startup_plan(const StartupPlan &plan) -> AppFlow;
		auto _run_maze(const int mode) -> AppFlow;
		auto _run_town() -> AppFlow;
		auto _run_restart_maze(const int mode) -> AppFlow;
		auto _run_main_menu() -> AppFlow;
		auto _build_startup_plan() -> StartupPlan;
		auto _add_quickstart_party() -> void;
		auto _check_param(std::string_view param) const -> bool;
		auto _get_exe_path() const -> std::string_view;
		auto _load_existing_game() -> void;
		auto _start_new_game(const bool quickstart) -> void;
		auto _continue_existing_game() -> int;
		auto _do_start_expedition(const int mode) -> int;
		auto _do_restart_expedition(const int mode) -> int;
		static auto _handle_signal(int signal) -> void;

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
		static inline std::atomic_bool _signal_shutdown_requested{false};
};
}