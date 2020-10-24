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


	// Get the SFML Window
	_window = display->window->get_window();

	// Show the Splash Screen and the Banner before starting the Main Menu
	_splash = std::make_shared<Splash>(*system, *display, *graphics);
	_splash->start();
	_banner = std::make_shared<Banner>(*system, *display, *graphics);
	_banner->start();

	// Start relevant animation worker threads
	graphics->animation->force_refresh_colour_cycling();
	graphics->animation->start_colour_cycling_threads();

	// Generate the necessary modules
	_mainmenu = std::make_shared<MainMenu>(*system, *display, *graphics);
	_license = std::make_shared<License>(*system, *display, *graphics);
	_options = std::make_shared<Options>(*system, *display, *graphics);
	_compendium = std::make_shared<Compendium>(*system, *display, *graphics);
}

// Standard Destructor
Sorcery::Application::~Application() {
	graphics->animation->stop_colour_cycling_threads();
}

auto Sorcery::Application::start() -> void {

	std::optional<MenuItem> option_chosen {MenuItem::NONE};
	MainMenuType menu_stage {MainMenuType::ATTRACT_MODE};
	do {
		_mainmenu->start(menu_stage);
		_mainmenu->stop();

		option_chosen = _mainmenu->start(menu_stage);
		_mainmenu->stop();
		if (option_chosen) {

			switch (option_chosen.value()) {
			case MenuItem::MM_OPTIONS:
				_options->start();
				_options->stop();
				break;
			case MenuItem::MM_LICENSE:
				_license->start();
				_license->stop();
				break;
			case MenuItem::MM_COMPENDIUM:
				_compendium->start();
				_compendium->stop();
				break;
			default:
				break;
			}

			menu_stage = MainMenuType::ATTRACT_MENU;
		}

	} while (option_chosen);

	_window->close();
}


