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

#include "application.hpp"

// Standard Constructor
Sorcery::Application::Application(int argc, char **argv) {

	// Get any command line arguments
	_arguments.clear();
	for (auto loop = 0; loop < argc; ++loop) {
		std::string arg{argv[loop]};
		std::transform(arg.begin(), arg.end(), arg.begin(), ::tolower);
		_arguments.push_back(arg);
	}

	// First set up System modules
	system = std::make_unique<System>(argc, argv);
	display = std::make_unique<Display>(system.get());
	graphics = std::make_unique<Graphics>(system.get(), display.get());

	if (!_check_for_parameter(SKIP_INTRO)) {

		// Show the Splash Screen and the Banner before starting the Main Menu
		_splash = std::make_shared<Splash>(system.get(), display.get(), graphics.get());
		_splash->start();
		_banner = std::make_shared<Banner>(system.get(), display.get(), graphics.get());
		_banner->start();
	}

	// Start relevant animation worker threads
	graphics->animation->force_refresh_colour_cycling();
	graphics->animation->start_colour_cycling_threads();

	// Create a Game (load the existing one if possible)
	_game = std::make_shared<Game>(system.get(), display.get(), graphics.get());

	// Generate the necessary modules
	_mainmenu =
		std::make_shared<MainMenu>(system.get(), display.get(), graphics.get(), _game.get());
	_license = std::make_shared<License>(system.get(), display.get(), graphics.get());
	_options = std::make_shared<Options>(system.get(), display.get(), graphics.get());
	_compendium = std::make_shared<Compendium>(system.get(), display.get(), graphics.get());

	// TODO need to pass in game object into these two
	_castle = std::make_shared<Castle>(system.get(), display.get(), graphics.get(), _game.get());
	_engine = std::make_shared<Engine>(system.get(), display.get(), graphics.get());
}

// Standard Destructor
Sorcery::Application::~Application() {

	graphics->animation->stop_colour_cycling_threads();
}

auto Sorcery::Application::start() -> int {

	if (_check_for_parameter(CONTINUE_GAME)) {
		_castle->start();
		_castle->stop();
	}

	std::optional<MenuItem> option_chosen{MenuItem::NONE};
	MainMenuType menu_stage{MainMenuType::ATTRACT_MODE};
	do {

		option_chosen = _mainmenu->start(menu_stage);
		_mainmenu->stop();
		if (option_chosen) {

			switch (option_chosen.value()) {
			case MenuItem::MM_NEW_GAME:
				_game->start_new_game();
				option_chosen = _castle->start();
				_castle->stop();
				break;
			case MenuItem::MM_CONTINUE_GAME:
				option_chosen = _castle->start();
				_castle->stop();
				break;
			case MenuItem::QUIT:
				return EXIT_OK;
				break;
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
			case MenuItem::ET_MAZE:
				_engine->start();
				_engine->stop();
			default:
				break;
			}

			menu_stage = MainMenuType::ATTRACT_MENU;
		}

	} while (option_chosen);

	return EXIT_OK;
}

// Check for a command line parameter
auto Sorcery::Application::_check_for_parameter(std::string_view parameter) const -> bool {
	for (auto arg : _arguments)
		if (const auto match_found{arg.find(parameter)}; match_found != std::string::npos)
			return true;

	return false;
}
