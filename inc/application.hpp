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

#include "banner.hpp"
#include "compendium.hpp"
#include "create.hpp" // temp
#include "display.hpp"
#include "engine.hpp"
#include "game.hpp"
#include "gamemenu.hpp"
#include "graphics.hpp"
#include "license.hpp"
#include "main.hpp"
#include "mainmenu.hpp"
#include "options.hpp"
#include "splash.hpp"
#include "system.hpp"

namespace Sorcery {

	// Application is a container class to hold the entire game structure, data, and any associated
	// meta-objects. Only one instance is ever created, at start-up, and it is (automatically)
	// freed just before the game is exited. This class is responsible for creating the various
	// subclasses to handle all the other parts of the game. It's technically not a singleton even
	// though only one instance is ever created in main.cpp as there's nothing to prevent another
	// copy being created, and game state isn’t held in this class. So there!
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
		auto start() -> void;

	  private:
		// Private Methods
		auto _check_for_parameter(std::string_view parameter) const -> bool;

		// Private Members
		std::shared_ptr<Banner> _banner;
		std::shared_ptr<Splash> _splash;
		std::shared_ptr<MainMenu> _mainmenu;
		std::shared_ptr<License> _license;
		std::shared_ptr<Options> _options;
		std::shared_ptr<Compendium> _compendium;
		std::shared_ptr<Game> _game;
		std::shared_ptr<GameMenu> _gamemenu;
		std::shared_ptr<Engine> _engine;
		std::vector<std::string> _arguments;
	};
} // namespace Sorcery