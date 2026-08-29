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

#include "core/context.hpp" // for Context
#include <atomic>			// for atomic_bool
#include <memory>			// for unique_ptr
#include <string>			// for string, basic_string
#include <string_view>		// for string_view
#include <vector>			// for vector
namespace Sorcery {
class Castle;
} // lines 40-40
namespace Sorcery {
class Controller;
} // lines 41-41
namespace Sorcery {
class Display;
} // lines 42-42
namespace Sorcery {
class EdgeOfTown;
} // lines 43-43
namespace Sorcery {
class Engine;
} // lines 44-44
namespace Sorcery {
class Game;
} // lines 45-45
namespace Sorcery {
class MainMenu;
} // lines 46-46
namespace Sorcery {
class Resources;
} // lines 47-47
namespace Sorcery {
class Splash;
} // lines 48-48
namespace Sorcery {
class System;
} // lines 49-49
namespace Sorcery {
class UI;
} // lines 50-50
namespace Sorcery {

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