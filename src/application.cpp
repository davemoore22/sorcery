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
	_compendium = std::make_unique<Compendium>(system.get(), display.get(), graphics.get(), _game.get());
	_castle = std::make_unique<Castle>(system.get(), display.get(), graphics.get(), _game.get());
	_edgeoftown = std::make_unique<EdgeOfTown>(system.get(), display.get(), graphics.get(), _game.get());
	_training = std::make_unique<Training>(system.get(), display.get(), graphics.get(), _game.get());
	_tavern = std::make_unique<Tavern>(system.get(), display.get(), graphics.get(), _game.get());
	_inn = std::make_unique<Inn>(system.get(), display.get(), graphics.get(), _game.get());
	_shop = std::make_unique<Shop>(system.get(), display.get(), graphics.get(), _game.get());
	_temple = std::make_unique<Temple>(system.get(), display.get(), graphics.get(), _game.get());
}

// Standard Destructor
Sorcery::Application::~Application() {

	graphics->animation->stop_colcyc_threads();
	graphics->animation->stop_wallpaper_threads();
}

auto Sorcery::Application::_quickstart() -> void {

	_game->wipe_data();
	_game->create_game();
	auto pc_1{Character(system.get(), display.get(), graphics.get())};
	pc_1.create_random();
	pc_1.finalise();
	auto pc_2{Character(system.get(), display.get(), graphics.get())};
	pc_2.create_random();
	pc_2.finalise();
	auto num = pc_1.get_name();
	auto pc1_id{_game->add_character(pc_1)};
	num = pc_2.get_name();
	auto pc2_id{_game->add_character(pc_2)};
	_game->state->clear_party();
	_game->state->add_character_by_id(pc1_id);
	pc_1.set_location(CharacterLocation::PARTY);
	_game->state->add_character_by_id(pc2_id);
	pc_2.set_location(CharacterLocation::PARTY);
	_game->save_game();
	_game->load_game();
	auto _engine{std::make_unique<Engine>(system.get(), display.get(), graphics.get(), _game.get())};
	_game->enter_maze();
	_engine->start();
	_engine->stop();
	_game->save_game();
}

auto Sorcery::Application::start() -> int {

	using enum Enums::Menu::Item;

	std::optional<MenuItem> mm_opt{NO_MENU_ITEM};
	std::optional<MenuItem> ca_opt{NO_MENU_ITEM};
	std::optional<MenuItem> ed_opt{NO_MENU_ITEM};
	do {

		// Main Menu
		mm_opt = _run_main_menu();
		if ((mm_opt.value() == MM_NEW_GAME) || (mm_opt.value() == MM_CONTINUE_GAME)) {

			do {

				// Castle
				ca_opt = _run_castle();
				if (ca_opt == ITEM_ABORT) {
					_game->save_game();
					display->shutdown_SFML();
					return EXIT_ALL;
				} else if (ca_opt == ITEM_LEAVE_GAME) {
					_game->save_game();
					break;
				} else if (ca_opt == CA_EDGE_OF_TOWN) {

					do {

						// Edge of Town
						ed_opt = _run_edge_of_town();

						if (ed_opt == ITEM_ABORT) {
							_game->save_game();
							display->shutdown_SFML();
							return EXIT_ALL;
						} else if (ed_opt == ITEM_LEAVE_GAME) {
							_game->save_game();
							break;
						} else if (ed_opt == ET_RESTART) {
							// Do restart

						} else if (ed_opt == ET_MAZE) {
							// Do Engine
						} else if (ed_opt == ET_CASTLE) {

							break;
						}

					} while ((ed_opt != ITEM_LEAVE_GAME) && (ed_opt != ET_CASTLE));
				}
			} while ((ca_opt != ITEM_ABORT) && (ed_opt != ITEM_LEAVE_GAME));
		}
	} while ((mm_opt != ITEM_QUIT) && (mm_opt != ITEM_ABORT));

	display->shutdown_SFML();
	return EXIT_ALL;
}

auto Sorcery::Application::_run_edge_of_town() -> std::optional<MenuItem> {

	using enum Enums::Menu::Item;

	std::optional<MenuItem> option_chosen{NO_MENU_ITEM};

	do {
		option_chosen = _edgeoftown->start(Destination::DEFAULT);
		if (option_chosen == ET_CASTLE) {
			return ET_CASTLE;
		} else if (option_chosen == ET_TRAIN) {

			// Remove everyone from the Party
			for (auto &[character_id, character] : _game->characters) {
				if (character.get_location() == CharacterLocation::PARTY)
					character.set_location(CharacterLocation::TAVERN);
			}
			_game->state->clear_party();
			_game->save_game();
			_training->start();
			_training->stop();
			display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
		} else if (option_chosen == ET_RESTART) {
			return ET_RESTART;
		} else if (option_chosen == ET_MAZE) {
			return ET_MAZE;
		}

	} while ((option_chosen != ITEM_LEAVE_GAME) && (option_chosen != ITEM_ABORT));

	return ITEM_LEAVE_GAME;
}

auto Sorcery::Application::_run_castle() -> std::optional<MenuItem> {

	using enum Enums::Menu::Item;

	std::optional<MenuItem> option_chosen{NO_MENU_ITEM};

	do {

		option_chosen = _castle->start();
		if (option_chosen == CA_TAVERN) {

			// Tavern
			if (auto tavern_option{_tavern->start()}; tavern_option && tavern_option.value() == ITEM_ABORT) {
				return ITEM_ABORT;
			}
			_tavern->stop();

		} else if (option_chosen == CA_INN) {

			// Inn
			if (auto inn_option{_inn->start()}; inn_option && inn_option.value() == ITEM_ABORT) {
				return ITEM_ABORT;
			}
			_inn->stop();
		} else if (option_chosen == CA_SHOP) {

			// Shop
			if (auto shop_option{_shop->start()}; shop_option && shop_option.value() == ITEM_ABORT) {
				return ITEM_ABORT;
			}
			_shop->stop();
		} else if (option_chosen == CA_TEMPLE) {
			if (auto temple_option{_temple->start()}; temple_option && temple_option.value() == ITEM_ABORT) {
				return ITEM_ABORT;
			}
			_temple->stop();
		} else if (option_chosen == CA_EDGE_OF_TOWN)
			return CA_EDGE_OF_TOWN;

		_game->save_game();
		display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
		_castle->stop();

	} while ((option_chosen != ITEM_ABORT) && (option_chosen != CA_EDGE_OF_TOWN));

	return CA_EDGE_OF_TOWN;
}

// Run the Main Menu
auto Sorcery::Application::_run_main_menu() -> std::optional<MenuItem> {

	using enum Enums::Menu::Item;
	using enum Enums::MainMenu::Type;

	std::optional<MenuItem> option_chosen{NO_MENU_ITEM};
	MainMenuType menu_stage{ATTRACT_MODE};

	do {

		option_chosen = _mainmenu->start(menu_stage);
		_mainmenu->stop();
		if (option_chosen) {

			switch (option_chosen.value()) {
			case MM_NEW_GAME:
				_game->create_game();
				_game->save_game();
				return MM_NEW_GAME;
				break;
			case MM_CONTINUE_GAME:
				return MM_CONTINUE_GAME;
				break;
			case ITEM_QUIT:
				_game->save_game();
				display->shutdown_SFML();
				return ITEM_QUIT;
				break;
			case MM_OPTIONS:
				if (_options->start() == EXIT_ALL) {
					display->shutdown_SFML();
					return ITEM_QUIT;
				}
				_options->stop();
				break;
			case MM_LICENSE:
				if (_license->start() == EXIT_ALL) {
					display->shutdown_SFML();
					return ITEM_QUIT;
				}
				_license->stop();
				break;
			case MM_COMPENDIUM:
				if (_compendium->start() == EXIT_ALL) {
					display->shutdown_SFML();
					return ITEM_QUIT;
				}
				_compendium->stop();
				break;
			default:
				break;
			}

			menu_stage = ATTRACT_MENU;
		}

	} while (option_chosen);

	display->shutdown_SFML();
	return ITEM_QUIT;
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
