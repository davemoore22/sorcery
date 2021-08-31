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

	if (!_check_param(SKIP_INTRO)) {

		// Show the Splash Screen and the Banner before starting the Main Menu
		_splash = std::make_unique<Splash>(
			system.get(), display.get(), graphics.get());
		_splash->start();
		_banner = std::make_unique<Banner>(
			system.get(), display.get(), graphics.get());
		_banner->start();
	}

	// Start relevant animation worker threads
	graphics->animation->refresh_colcyc();
	graphics->animation->start_colcycl_threads();

	// Create a Game (load the existing one if possible)
	_game = std::make_unique<Game>(system.get(), display.get(), graphics.get());

	// Generate the necessary modules
	_mainmenu = std::make_unique<MainMenu>(
		system.get(), display.get(), graphics.get(), _game.get());
	_license =
		std::make_unique<License>(system.get(), display.get(), graphics.get());
	_options =
		std::make_unique<Options>(system.get(), display.get(), graphics.get());
	_compendium = std::make_unique<Compendium>(
		system.get(), display.get(), graphics.get());
	_castle = std::make_unique<Castle>(
		system.get(), display.get(), graphics.get(), _game.get());
}

// Standard Destructor
Sorcery::Application::~Application() {

	graphics->animation->stop_colcyc_threads();
}

auto Sorcery::Application::start() -> int {

	if (_check_param(CONTINUE_GAME)) {
		auto result{_castle->start()};
		if (result && result == MenuItem::ABORT) {
			_game->save_game();
			display->shutdown_SFML();
			return EXIT_ALL;
		}
		_castle->stop();
		_game->save_game();
	}

	if (_check_param(GO_TO_MAZE)) {
		if (_castle->start(true) == MenuItem::ABORT) {
			_game->save_game();
			display->shutdown_SFML();
			return EXIT_ALL;
		}
		_castle->stop();
		_game->save_game();
	}

	std::optional<MenuItem> option_chosen{MenuItem::NONE};
	MainMenuType menu_stage{MainMenuType::ATTRACT_MODE};
	do {

		option_chosen = _mainmenu->start(menu_stage);
		_mainmenu->stop();
		if (option_chosen) {

			switch (option_chosen.value()) {
			case MenuItem::MM_NEW_GAME:
				_game->create_game();
				option_chosen = _castle->start();
				if (option_chosen == MenuItem::ABORT) {
					_game->save_game();
					display->shutdown_SFML();
					return EXIT_ALL;
				}
				_castle->stop();
				_game->save_game();
				break;
			case MenuItem::MM_CONTINUE_GAME:
				option_chosen = _castle->start();
				_castle->stop();
				_game->save_game();
				break;
			case MenuItem::QUIT:
				_game->save_game();
				display->shutdown_SFML();
				return EXIT_ALL;
				break;
			case MenuItem::MM_OPTIONS:
				if (_options->start() == EXIT_ALL) {
					display->shutdown_SFML();
					return EXIT_ALL;
				}
				_options->stop();
				break;
			case MenuItem::MM_LICENSE:
				if (_license->start() == EXIT_ALL) {
					display->shutdown_SFML();
					return EXIT_ALL;
				}
				_license->stop();
				break;
			case MenuItem::MM_COMPENDIUM:
				if (_compendium->start() == EXIT_ALL) {
					display->shutdown_SFML();
					return EXIT_ALL;
				}
				_compendium->stop();
				break;
			default:
				break;
			}

			menu_stage = MainMenuType::ATTRACT_MENU;
		}

	} while (option_chosen);

	display->shutdown_SFML();
	return EXIT_ALL;
}

// Check for a command line parameter
auto Sorcery::Application::_check_param(std::string_view parameter) const
	-> bool {

	for (auto arg : _arguments)
		if (const auto match_found{arg.find(parameter)};
			match_found != std::string::npos)
			return true;

	return false;
}
