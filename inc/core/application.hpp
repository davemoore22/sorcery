// Copyright (C) 2024 Dave Moore
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

#include "common/define.hpp"
#include "common/include.hpp"
#include "core/define.hpp"
#include "core/include.hpp"

namespace Sorcery {

// Forward Declarations
class Banner;
class Castle;
class Compendium;
class Display;
class EdgeOfTown;
class Engine;
class Game;
class Graphics;
class Inn;
class License;
class MainMenu;
class Options;
class Restart;
class Shop;
class Splash;
class System;
class Tavern;
class Temple;
class Training;

// Application is a container class to hold the entire game structure, data, and any associated meta-objects. Only one
// instance is ever created, at start-up, and it is (automatically) freed just before the game is exited. This class is
// responsible for creating the various subclasses to handle all the other parts of the game. It's technically not a
// singleton even though only one instance is ever created in main.cpp as there's nothing to prevent another copy being
// created.
class Application {

	public:

		// Constructors
		Application(int argc, char **argv);
		Application() = delete;

		// Standard Destructor
		~Application();

		// Overload [] operator (for file operations)
		auto operator[](const char *key) const -> std::string;
		auto operator[](std::string key) const -> std::string;

		// Public Members
		std::unique_ptr<System> system;
		std::unique_ptr<Display> display;
		std::unique_ptr<Graphics> graphics;

		// Public Methods
		auto start() -> int;

	private:

		// Private Methods
		auto _check_param(std::string_view parameter) const -> bool;
		auto _display_loading_window() -> void;
		auto _hide_loading_window() -> void;
		auto _get_exe_path() const -> std::string_view;
		auto _quickstart() -> void;
		auto _start_expedition() -> std::optional<MenuItem>;
		auto _run_main_menu(const Destination destination) -> std::optional<MenuItem>;
		auto _run_castle() -> std::optional<MenuItem>;
		auto _run_edge_of_town() -> std::optional<MenuItem>;
		auto _run_restart() -> std::optional<MenuItem>;
		auto _restart_expedition(const unsigned int character_chosen) -> std::optional<MenuItem>;

		// Private Members
		std::unique_ptr<Banner> _banner;
		std::unique_ptr<Splash> _splash;
		std::unique_ptr<MainMenu> _mainmenu;
		std::unique_ptr<License> _license;
		std::unique_ptr<Options> _options;
		std::unique_ptr<Compendium> _compendium;
		std::unique_ptr<Castle> _castle;
		std::unique_ptr<EdgeOfTown> _edgeoftown;
		std::unique_ptr<Training> _training;
		std::unique_ptr<Game> _game;
		std::unique_ptr<Engine> _engine;
		std::unique_ptr<Tavern> _tavern;
		std::unique_ptr<Inn> _inn;
		std::unique_ptr<Shop> _shop;
		std::unique_ptr<Temple> _temple;
		std::unique_ptr<Restart> _restart;
		std::vector<std::string> _arguments;
		sf::RenderWindow _load_window;
};

}