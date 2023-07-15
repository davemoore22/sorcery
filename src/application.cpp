// Copyright (C) 2023 Dave Moore
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
		std::ranges::transform(arg.begin(), arg.end(), arg.begin(), ::tolower);
		_arguments.push_back(arg);
	}

	// First set up System modules
	_display_loading_window();
	system = std::make_unique<System>(argc, argv);
	display = std::make_unique<Display>(system.get());
	graphics = std::make_unique<Graphics>(system.get(), display.get());
	_hide_loading_window();

	if (!_check_param(SKIP_INTRO)) {

		// Show the Splash Screen and the Banner before starting the Main Menu
		_splash = std::make_unique<Splash>(system.get(), display.get(), graphics.get());
		_splash->start();
		_banner = std::make_unique<Banner>(system.get(), display.get(), graphics.get());
		_banner->start();
	}

	// Start relevant animation worker threads
	graphics->animation->refresh_colcyc();
	graphics->animation->start_colcycl_threads();
	graphics->animation->refresh_wallpaper();
	graphics->animation->start_wallpaper_threads();

	// Create a Game (load the existing one if possible)
	_game = std::make_unique<Game>(system.get(), display.get(), graphics.get());

	// Generate the necessary modules
	_mainmenu = std::make_unique<MainMenu>(system.get(), display.get(), graphics.get(), _game.get());
	_license = std::make_unique<License>(system.get(), display.get(), graphics.get());
	_options = std::make_unique<Options>(system.get(), display.get(), graphics.get());
	_compendium = std::make_unique<Compendium>(system.get(), display.get(), graphics.get());
	_castle = std::make_unique<Castle>(system.get(), display.get(), graphics.get(), _game.get());
}

// Standard Destructor
Sorcery::Application::~Application() {

	graphics->animation->stop_colcyc_threads();
	graphics->animation->stop_wallpaper_threads();
}

auto Sorcery::Application::start() -> int {

	if (_check_param(GO_TO_MAZE)) {
		if (_game->valid) {
			if (_game->state->party_has_members()) {
				if (_castle->start(Destination::MAZE) == MenuItem::ITEM_ABORT) {
					_game->save_game();
					display->shutdown_SFML();
					return EXIT_ALL;
				}
				_castle->stop();
				_game->save_game();
			}
		}
	} else if (_check_param(GO_TO_COMPENDIUM)) {
		_compendium->start();
		_compendium->stop();
	} else if (_check_param(GO_TO_TAVERN)) {
		if (_game->valid) {
			if (_castle->start(Destination::TAVERN) == MenuItem::ITEM_ABORT) {
				_game->save_game();
				display->shutdown_SFML();
				return EXIT_ALL;
			}
			_castle->stop();
		}
	} else if (_check_param(GO_TO_INN)) {
		if (_game->valid) {
			if (_game->state->party_has_members()) {
				if (_castle->start(Destination::INN) == MenuItem::ITEM_ABORT) {
					_game->save_game();
					display->shutdown_SFML();
					return EXIT_ALL;
				}
				_castle->stop();
			}
		}
	} else if (_check_param(GO_TO_TEMPLE)) {
		if (_game->valid) {
			if (_game->state->party_has_members()) {
				if (_castle->start(Destination::TEMPLE) == MenuItem::ITEM_ABORT) {
					_game->save_game();
					display->shutdown_SFML();
					return EXIT_ALL;
				}
				_castle->stop();
			}
		}
	} else if (_check_param(GO_TO_SHOP)) {
		if (_game->valid) {
			if (_game->state->party_has_members()) {
				if (_castle->start(Destination::SHOP) == MenuItem::ITEM_ABORT) {
					_game->save_game();
					display->shutdown_SFML();
					return EXIT_ALL;
				}
				_castle->stop();
			}
		}
	} else if (_check_param(GO_TO_TRAINING)) {
		if (_game->valid) {
			if (_castle->start(Destination::TRAINING) == MenuItem::ITEM_ABORT) {
				_game->save_game();
				display->shutdown_SFML();
				return EXIT_ALL;
			}
			_castle->stop();
		}
		_game->save_game();
	} else if (_check_param(CONTINUE_GAME)) {
		if (_game->valid) {
			if (auto result{_castle->start()}; result && result == MenuItem::ITEM_ABORT) {
				_game->save_game();
				display->shutdown_SFML();
				return EXIT_ALL;
			}
			_castle->stop();
			_game->save_game();
		}
	}

	std::optional<MenuItem> option_chosen{MenuItem::NO_MENU_ITEM};
	MainMenuType menu_stage{MainMenuType::ATTRACT_MODE};
	do {

		option_chosen = _mainmenu->start(menu_stage);
		_mainmenu->stop();
		if (option_chosen) {

			switch (option_chosen.value()) {
			case MenuItem::MM_NEW_GAME:
				_game->create_game();
				option_chosen = _castle->start();
				if (option_chosen == MenuItem::ITEM_ABORT) {
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
			case MenuItem::ITEM_QUIT:
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
auto Sorcery::Application::_check_param(std::string_view parameter) const -> bool {

	return std::ranges::any_of(_arguments, [&](const auto &arg) { return arg.find(parameter) != std::string::npos; });
}

auto Sorcery::Application::_display_loading_window() -> void {

	// Just use the 2nd highest screen resolution to handle multimonitor modes
	std::vector<sf::VideoMode> video_modes{sf::VideoMode::getFullscreenModes()};
	sf::Vector2i screen_size{static_cast<int>(video_modes[1].width), static_cast<int>(video_modes[1].height)};

	// Hard Coded since we don't have access to any file resources at this point
	const std::filesystem::path base_path{_get_exe_path()};
	const std::filesystem::path image_path{base_path / GRAPHICS_DIR / LOADING_IMAGE};
	auto scale{0.75f};

	sf::Image loading{};
	loading.loadFromFile(image_path.string());

	const sf::Vector2u splash_size{
		static_cast<unsigned int>(loading.getSize().x * scale), static_cast<unsigned int>(loading.getSize().y * scale)};

	// can't use sf::Style::None here due to None being defined somewhere else (no I don't know either), so use 0
	_load_window.create(sf::VideoMode(splash_size.x, splash_size.y), "Sorcery: Shadows under Llylgamyn", 0);
	_load_window.setVerticalSyncEnabled(true);
	_load_window.setPosition(sf::Vector2i((screen_size.x - splash_size.x) / 2, (screen_size.y - splash_size.y) / 2));
	_load_window.clear({0, 0, 0, 175});

	sf::Texture texture{};
	texture.loadFromImage(loading);
	sf::Sprite sprite{};
	sprite.setTexture(texture, true);
	sprite.setPosition(0, 0);
	sprite.setScale(scale, scale);
	_load_window.draw(sprite);

	_load_window.display();
}

auto Sorcery::Application::_hide_loading_window() -> void {

	_load_window.close();
}

// This is linux only
auto Sorcery::Application::_get_exe_path() const -> std::string_view {

	char result[PATH_MAX];
	if (const ssize_t count{readlink("/proc/self/exe", result, PATH_MAX)}; count != -1) {
		const char *path{dirname(result)};
		std::string_view base_path{path};
		return base_path;
	} else
		return "";
}
