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

#include "core/application.hpp"
#include "core/animation.hpp"
#include "core/define.hpp"
#include "core/display.hpp"
#include "core/game.hpp"
#include "core/graphics.hpp"
#include "core/random.hpp"
#include "core/state.hpp"
#include "core/system.hpp"
#include "engine/engine.hpp"
#include "frontend/banner.hpp"
#include "frontend/compendium.hpp"
#include "frontend/license.hpp"
#include "frontend/mainmenu.hpp"
#include "frontend/options.hpp"
#include "frontend/splash.hpp"
#include "modules/castle.hpp"
#include "modules/edgeoftown.hpp"
#include "modules/inn.hpp"
#include "modules/restart.hpp"
#include "modules/shop.hpp"
#include "modules/tavern.hpp"
#include "modules/temple.hpp"
#include "modules/training.hpp"
#include "resources/define.hpp"
#include "resources/itemstore.hpp"
#include "resources/levelstore.hpp"
#include "types/character.hpp"
#include "types/enum.hpp"
#include "types/inventory.hpp"
#include "types/level.hpp"

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
	_restart = std::make_unique<Restart>(system.get(), display.get(), graphics.get(), _game.get());
}

// Standard Destructor
Sorcery::Application::~Application() {

	graphics->animation->stop_colcyc_threads();
	graphics->animation->stop_wallpaper_threads();
}

auto Sorcery::Application::_quickstart() -> void {

	_game->wipe_data();
	_game->create_game();

	// Create a new random party
	auto party_alignment{(*system->random)[RandomType::D2] == 1 ? CharacterAlignment::GOOD : CharacterAlignment::EVIL};
	for (int i = 0; i < 6; i++) {
		auto pc{Character(system.get(), display.get(), graphics.get())};
		switch (i) {
		case 0:
			pc.create_class_alignment(CharacterClass::FIGHTER, party_alignment);
			break;
		case 1:
			pc.create_class_alignment(CharacterClass::FIGHTER, CharacterAlignment::NEUTRAL);
			break;
		case 2:
			pc.create_class_alignment(CharacterClass::THIEF, CharacterAlignment::NEUTRAL);
			break;
		case 3:
			pc.create_class_alignment(CharacterClass::PRIEST, party_alignment);
			break;
		case 4:
			pc.create_class_alignment(CharacterClass::BISHOP, party_alignment);
			break;
		case 5:
			pc.create_class_alignment(CharacterClass::MAGE, CharacterAlignment::NEUTRAL);
			break;
		default:
			return;
		}

		pc.finalise();
		pc.set_location(CharacterLocation::PARTY);
		pc.set_stage(CharacterStage::COMPLETED);
		pc.inventory.clear();

		using enum Enums::Character::Class;
		using enum Enums::Items::TypeID;

		switch (pc.get_class()) { // NOLINT(clang-diagnostic-switch)
		case FIGHTER:
		case LORD:
		case SAMURAI:
			pc.inventory.add_type((*_game->itemstore)[LEATHER_ARMOR], true);
			pc.inventory.add_type((*_game->itemstore)[LONG_SWORD], true);
			break;
		case MAGE:
			pc.inventory.add_type((*_game->itemstore)[ROBES], true);
			pc.inventory.add_type((*_game->itemstore)[DAGGER], true);
			break;
		case PRIEST:
		case BISHOP:
			pc.inventory.add_type((*_game->itemstore)[ROBES], true);
			pc.inventory.add_type((*_game->itemstore)[STAFF], true);
			break;
		case THIEF:
		case NINJA:
			pc.inventory.add_type((*_game->itemstore)[LEATHER_ARMOR], true);
			pc.inventory.add_type((*_game->itemstore)[SHORT_SWORD], true);
		default:
			break;
		}

		auto char_id{_game->add_character(pc)};
		_game->characters[char_id] = pc;
		_game->state->add_character_by_id(char_id);
	}

	_game->save_game();
	_start_expedition();
}

auto Sorcery::Application::start() -> int {

	using enum Enums::Menu::Item;

	// Check if we are doing any sort of shortcut
	auto do_restart{false};
	auto do_maze{false};
	auto destination{Destination::DEFAULT};
	if ((_check_param(CONTINUE_GAME)) && (_game->valid))
		destination = Destination::CONTINUE;
	else if (_check_param(NEW_GAME))
		destination = Destination::NEW;
	else if ((_check_param(RESTART_EXPEDITION)) && (_game->valid))
		do_restart = true;
	else if ((_check_param(START_EXPEDITION)) && (_game->valid)) {
		if (_game->state->party_has_members())
			do_maze = true;
	} else if (_check_param(QUICKSTART))
		_quickstart();
	else if (_check_param(GO_TO_COMPENDIUM))
		destination = Destination::COMPENDIUM;
	else if (_check_param(GO_TO_LICENSE))
		destination = Destination::LICENSE;
	else if (_check_param(GO_TO_OPTIONS))
		destination = Destination::OPTIONS;

	std::optional<MenuItem> mm_opt{std::nullopt};
	do {

		if (do_restart) {
			auto ra_opt{_run_restart()};
			if (ra_opt.value() == RS_RESTART) {
				_restart_expedition(_game->state->get_party_characters()[0]);
				do_restart = false;
				continue;
			}
		} else if (do_maze) {

			_start_expedition();
			do_maze = false;
			continue;
		}

		// Run the Main Menu
		mm_opt = _run_main_menu(destination);

		// If we are going via a shortcut after we have done that, disable it
		if (destination != Destination::DEFAULT)
			destination = Destination::DEFAULT;

		// That way the Program will always return to the Main Menu
		if (mm_opt.value() == ITEM_QUIT) {
			display->shutdown_SFML();
			return EXIT_APP;
		} else {

			// If we are starting a new game, or continuing an existing game
			std::optional<MenuItem> ca_opt{std::nullopt};
			std::optional<MenuItem> ed_opt{std::nullopt};
			do {

				// Go to the Castle
				ca_opt = _run_castle();
				if (ca_opt.value() == ITEM_ABORT) {
					display->shutdown_SFML();
					return EXIT_APP;
				} else if (ca_opt.value() == ITEM_LEAVE_GAME) {
					break;
				} else if (ca_opt.value() != CA_EDGE_OF_TOWN)
					continue;

				// Go to the Edge of Town
				ed_opt = _run_edge_of_town();
				if (ed_opt.value() == ITEM_ABORT) {
					display->shutdown_SFML();
					return EXIT_APP;
				} else if (ed_opt.value() == ET_MAZE) {

					// Go to the Maze
					if (_game->state->party_has_members())
						_start_expedition();
				} else if (ed_opt.value() == ET_RESTART) {

					std::optional<MenuItem> rs_opt{std::nullopt};
					unsigned int character_chosen{0u};
					rs_opt = _restart->start(character_chosen);
					_restart->stop();
					if (rs_opt.value() == ITEM_ABORT) {
						display->shutdown_SFML();
						return EXIT_APP;
					} else if (rs_opt == MenuItem::RS_RESTART)
						rs_opt = _restart_expedition(character_chosen);
					else
						ca_opt = CA_EDGE_OF_TOWN;

				} else if (ed_opt.value() == ITEM_LEAVE_GAME)
					break;

			} while ((ed_opt.value() != ITEM_LEAVE_GAME) && (ca_opt.value() != ITEM_LEAVE_GAME));
			_game->save_game();
		}
	} while ((mm_opt != ITEM_QUIT) && (mm_opt != ITEM_ABORT));

	display->shutdown_SFML();
	return EXIT_APP;
}

auto Sorcery::Application::_start_expedition() -> std::optional<MenuItem> {

	_game->enter_maze();
	auto engine{std::make_unique<Engine>(system.get(), display.get(), graphics.get(), _game.get())};
	if (auto result{engine->start()}; result == EXIT_APP) {
		_game->save_game();
		engine->stop();
		display->shutdown_SFML();
		return MenuItem::ITEM_ABORT;
	}
	_engine->stop();
	return MenuItem::ITEM_QUIT;
}

auto Sorcery::Application::_run_restart() -> std::optional<MenuItem> {

	std::optional<MenuItem> rs_opt{std::nullopt};
	unsigned int character_chosen{0u};

	rs_opt = _restart->start(character_chosen);
	_restart->stop();
	if (rs_opt.value() == MenuItem::ITEM_ABORT) {
		display->shutdown_SFML();
		return MenuItem::ITEM_QUIT;
	} else if (rs_opt == MenuItem::RS_RESTART)
		return _restart_expedition(character_chosen);
	else
		return MenuItem::CA_EDGE_OF_TOWN;
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
		} else if (option_chosen == ET_LEAVE_GAME)
			return ITEM_LEAVE_GAME;

	} while (option_chosen != ITEM_ABORT);

	return ITEM_ABORT;
}

auto Sorcery::Application::_restart_expedition(const unsigned int character_chosen) -> std::optional<MenuItem> {

	// Find the location and floor of the character pointed to, and reload the maze, repopulate the
	// party and restart the game from there
	auto &character{_game->characters[character_chosen]};
	auto to_depth{character.depth.value()};
	auto to_loc{character.coordinate.value()};
	_game->state->clear_party();
	for (auto &[character_id, character] : _game->characters) {
		if (character.get_location() == CharacterLocation::MAZE) {
			if ((character.depth.value() == to_depth) && (character.coordinate.value() == to_loc)) {
				character.set_location(CharacterLocation::PARTY);
				_game->state->add_character_by_id(character_id);
			}
		}
	}

	_game->state->set_depth(to_depth);
	_game->state->set_player_prev_depth(_game->state->get_depth());
	_game->state->set_player_pos(to_loc);
	Level level{((*_game->levelstore)[to_depth]).value()};
	_game->state->set_current_level(&level);

	auto engine{std::make_unique<Engine>(system.get(), display.get(), graphics.get(), _game.get())};
	if (auto result{engine->start()}; result == EXIT_APP) {
		_game->save_game();
		engine->stop();
		display->shutdown_SFML();
		return MenuItem::ITEM_ABORT;
	}
	_engine->stop();

	return MenuItem::RS_RESTART;
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
		else if (option_chosen == ITEM_LEAVE_GAME)
			return ITEM_LEAVE_GAME;

		_game->save_game();
		display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
		_castle->stop();

	} while (option_chosen != ITEM_ABORT);

	return ITEM_ABORT;
}

// Run the Main Menu
auto Sorcery::Application::_run_main_menu(const Destination destination) -> std::optional<MenuItem> {

	using enum Enums::Menu::Item;
	using enum Enums::MainMenu::Type;

	// Handle shortcuts
	if (destination == Destination::CONTINUE)
		return MM_CONTINUE_GAME;
	else if (destination == Destination::NEW) {
		_game->wipe_data();
		_game->create_game();
		_game->save_game();
		return MM_NEW_GAME;
	} else if (destination == Destination::RESTART) {
		return RS_RESTART;
	}

	std::optional<MenuItem> option_chosen{NO_MENU_ITEM};
	MainMenuType menu_stage{ATTRACT_MODE};

	do {
		if (destination != Destination::DEFAULT) {
			switch (destination) {
			case Destination::OPTIONS:
				option_chosen = MM_OPTIONS;
				break;
			case Destination::LICENSE:
				option_chosen = MM_LICENSE;
				break;
			case Destination::COMPENDIUM:
				option_chosen = MM_COMPENDIUM;
				break;
			default:
				option_chosen = std::nullopt;
			}

		} else {
			option_chosen = _mainmenu->start(menu_stage);
			_mainmenu->stop();
		}
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
				return ITEM_QUIT;
				break;
			case MM_OPTIONS:
				if (_options->start() == EXIT_APP) {
					_options->stop();
					return ITEM_QUIT;
				} else
					_options->stop();
				break;
			case MM_LICENSE:
				if (_license->start() == EXIT_APP) {
					_license->stop();
					return ITEM_QUIT;
				} else
					_license->stop();
				break;
			case MM_COMPENDIUM:
				if (_compendium->start() == EXIT_APP) {
					_compendium->stop();
					return ITEM_QUIT;
				} else
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
