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

#include "application.hpp"

// Standard Constructor
Sorcery::Application::Application(int argc, char** argv) {

	// First set up System modules
	system = std::make_unique<System>(argc, argv);
	display = std::make_unique<Display>(system.get());
	graphics = std::make_unique<Graphics>(system.get(), display.get());

	// Set up Game files
	_help = std::make_shared<TextFile>((*system->files)[HELP_FILE]);
	_version = std::make_shared<TextFile>((*system->files)[VERSION_FILE]);
	_license = std::make_shared<TextFile>((*system->files)[LICENSE_FILE]);

	// Show the Splash Screen and the Banner before starting the Main Menu
	/* sf::RenderWindow* window = display->window->get_window();
	_splash = std::make_shared<Splash>(*system, *display, *graphics);
	_splash->start();
	_banner = std::make_shared<Banner>(*system, *display, *graphics);
	_banner->start(); */

	// Start relevant animation worker threads
	graphics->animation->force_refresh_colour_cycling();
	graphics->animation->start_colour_cycling_threads();

	// Display the main menu
	_mainmenu = std::make_shared<MainMenu>(*system, *display, *graphics);
	_mainmenu->start();
}

// Standard Destructor
Sorcery::Application::~Application() {
	graphics->animation->stop_colour_cycling_threads();
}




