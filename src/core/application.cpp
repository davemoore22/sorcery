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
#include "resources/factory.hpp"
#include "resources/itemstore.hpp"
#include "resources/levelstore.hpp"
#include "types/character.hpp"
#include "types/enum.hpp"
#include "types/inventory.hpp"
#include "types/level.hpp"
#include <libgen.h>

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
	_update_loading_window();
	system = std::make_unique<System>(argc, argv);
	_update_loading_window();
	display = std::make_unique<Display>(system.get());
	_update_loading_window();
	graphics = std::make_unique<Graphics>(system.get(), display.get());
	_update_loading_window();

	// Start relevant animation worker threads
	graphics->animation->refresh_colcyc();
	graphics->animation->start_colcycl_th();
	graphics->animation->refresh_wp();
	graphics->animation->start_wp_th();

	// Create a Game (load the existing one if possible)
	_game = std::make_unique<Game>(system.get(), display.get(), graphics.get());
	_update_loading_window();

	// Generate the necessary modules
	_mainmenu = std::make_unique<MainMenu>(
		system.get(), display.get(), graphics.get(), _game.get());
	_update_loading_window();
	_license =
		std::make_unique<License>(system.get(), display.get(), graphics.get());
	_update_loading_window();
	_options =
		std::make_unique<Options>(system.get(), display.get(), graphics.get());
	_update_loading_window();
	_compendium = std::make_unique<Compendium>(
		system.get(), display.get(), graphics.get(), _game.get());
	_update_loading_window();
	_castle = std::make_unique<Castle>(
		system.get(), display.get(), graphics.get(), _game.get());
	_update_loading_window();
	_edgeoftown = std::make_unique<EdgeOfTown>(
		system.get(), display.get(), graphics.get(), _game.get());
	_update_loading_window();
	_training = std::make_unique<Training>(
		system.get(), display.get(), graphics.get(), _game.get());
	_update_loading_window();
	_tavern = std::make_unique<Tavern>(
		system.get(), display.get(), graphics.get(), _game.get());
	_update_loading_window();
	_inn = std::make_unique<Inn>(
		system.get(), display.get(), graphics.get(), _game.get());
	_update_loading_window();
	_shop = std::make_unique<Shop>(
		system.get(), display.get(), graphics.get(), _game.get());
	_update_loading_window();
	_temple = std::make_unique<Temple>(
		system.get(), display.get(), graphics.get(), _game.get());
	_update_loading_window();
	_restart = std::make_unique<Restart>(
		system.get(), display.get(), graphics.get(), _game.get());
	_hide_loading_window();

	if (!_check_param(SKIP_INTRO)) {

		// Show the Splash Screen and the Banner before starting the Main Menu
		_splash = std::make_unique<Splash>(
			system.get(), display.get(), graphics.get());
		_splash->start();
		_banner = std::make_unique<Banner>(
			system.get(), display.get(), graphics.get());
		_banner->start();
	}
}

// Standard Destructor
Sorcery::Application::~Application() {

	graphics->animation->stop_colcyc_th();
	graphics->animation->stop_wp_th();
}

auto Sorcery::Application::_quickstart() -> void {

	_game->wipe_data();
	_game->create_game();

	// Create a new random party of a random alignment
	const auto align{(*system->random)[RNT::D2] == 1 ? CAL::GOOD : CAL::EVIL};
	for (int i = 0; i < 6; i++) {
		auto pc{Character(system.get(), display.get(), graphics.get(),
			_game->itemstore.get())};
		switch (i) {
		case 0:
			pc.create_class_alignment(CHC::FIGHTER, align);
			break;
		case 1:
			pc.create_class_alignment(CHC::FIGHTER, CAL::NEUTRAL);
			break;
		case 2:
			pc.create_class_alignment(CHC::THIEF, CAL::NEUTRAL);
			break;
		case 3:
			pc.create_class_alignment(CHC::PRIEST, align);
			break;
		case 4:
			pc.create_class_alignment(CHC::BISHOP, align);
			break;
		case 5:
			pc.create_class_alignment(CHC::MAGE, CAL::NEUTRAL);
			break;
		default:
			return;
		}

		pc.finalise();
		pc.set_location(CHL::PARTY);
		pc.set_stage(CHS::COMPLETED);
		pc.inventory.clear();

		switch (pc.get_class()) { // NOLINT(clang-diagnostic-switch)
		case CHC::FIGHTER:
		case CHC::LORD:
		case CHC::SAMURAI:
			pc.inventory.add_type(
				(*_game->itemstore)[ITT::LEATHER_ARMOR], true);
			pc.inventory.add_type((*_game->itemstore)[ITT::LONG_SWORD], true);
			break;
		case CHC::MAGE:
			pc.inventory.add_type((*_game->itemstore)[ITT::ROBES], true);
			pc.inventory.add_type((*_game->itemstore)[ITT::DAGGER], true);
			break;
		case CHC::PRIEST:
		case CHC::BISHOP:
			pc.inventory.add_type((*_game->itemstore)[ITT::ROBES], true);
			pc.inventory.add_type((*_game->itemstore)[ITT::STAFF], true);
			// pc.inventory.add_type((*_game->itemstore)[LEATHER_ARMOR], true);
			// pc.inventory.add_type((*_game->itemstore)[CHAIN_MAIL], true);
			// pc.inventory.add_type((*_game->itemstore)[LONG_SWORD], false,
			// true); pc.inventory.add_type((*_game->itemstore)[CHAIN_MINUS_2],
			// false); pc.inventory.add_type((*_game->itemstore)[MACE_MINUS_1],
			// false); pc.inventory.add_type((*_game->itemstore)[PLATE_MAIL],
			// false, false);
			break;
		case CHC::THIEF:
		case CHC::NINJA:
			pc.inventory.add_type(
				(*_game->itemstore)[ITT::LEATHER_ARMOR], true);
			pc.inventory.add_type((*_game->itemstore)[ITT::SHORT_SWORD], true);
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

	// Check if we are doing any sort of shortcut
	auto do_restart{false};
	auto do_maze{false};
	auto dest{DES::DEFAULT};
	if (_check_param(CONTINUE_GAME) && _game->valid)
		dest = DES::CONTINUE;
	else if (_check_param(NEW_GAME))
		dest = DES::NEW;
	else if (_check_param(RESTART_EXPEDITION) && _game->valid)
		do_restart = true;
	else if (_check_param(START_EXPEDITION) && _game->valid) {
		if (_game->state->party_has_members())
			do_maze = true;
	} else if (_check_param(QUICKSTART))
		_quickstart();
	else if (_check_param(GO_TO_COMPENDIUM))
		dest = DES::COMPENDIUM;
	else if (_check_param(GO_TO_LICENSE))
		dest = DES::LICENSE;
	else if (_check_param(GO_TO_OPTIONS))
		dest = DES::OPTIONS;

	std::optional<MIM> mm_opt{std::nullopt};
	do {

		if (do_restart) {
			auto ra_opt{_run_restart()};
			if (ra_opt.value() == MIM::RS_RESTART) {
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
		mm_opt = _run_main_menu(dest);

		// If we are going via a shortcut after we have done that, disable it
		if (dest != DES::DEFAULT)
			dest = DES::DEFAULT;

		// That way the Program will always return to the Main Menu
		if (mm_opt.value() == MIM::ITEM_QUIT) {
			display->shutdown_SFML();
			return EXIT_APP;
		} else {

			// If we are starting a new game, or continuing an existing game
			std::optional<MIM> ca_opt{std::nullopt};
			std::optional<MIM> ed_opt{std::nullopt};
			do {

				// Go to the Castle
				ca_opt = _run_castle();
				if (ca_opt.value() == MIM::ITEM_ABORT) {
					display->shutdown_SFML();
					return EXIT_APP;
				} else if (ca_opt.value() == MIM::ITEM_LEAVE_GAME) {
					break;
				} else if (ca_opt.value() != MIM::CA_EDGE_OF_TOWN)
					continue;

				// Go to the Edge of Town
				ed_opt = _run_edge_of_town();
				if (ed_opt.value() == MIM::ITEM_ABORT) {
					display->shutdown_SFML();
					return EXIT_APP;
				} else if (ed_opt.value() == MIM::ET_MAZE) {

					// Go to the Maze
					if (_game->state->party_has_members())
						_start_expedition();
				} else if (ed_opt.value() == MIM::ET_RESTART) {

					std::optional<MIM> rs_opt{std::nullopt};
					unsigned int character_chosen{0u};
					rs_opt = _restart->start(character_chosen);
					_restart->stop();
					if (rs_opt.value() == MIM::ITEM_ABORT) {
						display->shutdown_SFML();
						return EXIT_APP;
					} else if (rs_opt == MIM::RS_RESTART)
						rs_opt = _restart_expedition(character_chosen);
					else
						ca_opt = MIM::CA_EDGE_OF_TOWN;

				} else if (ed_opt.value() == MIM::ITEM_LEAVE_GAME)
					break;

			} while ((ed_opt.value() != MIM::ITEM_LEAVE_GAME) &&
					 (ca_opt.value() != MIM::ITEM_LEAVE_GAME));
			_game->save_game();
		}
	} while ((mm_opt != MIM::ITEM_QUIT) && (mm_opt != MIM::ITEM_ABORT));

	display->shutdown_SFML();
	return EXIT_APP;
}

auto Sorcery::Application::_start_expedition() -> std::optional<MIM> {

	_game->enter_maze();
	auto engine{std::make_unique<Engine>(
		system.get(), display.get(), graphics.get(), _game.get())};
	if (auto result{engine->start()}; result == EXIT_APP) {
		_game->save_game();
		engine->stop();
		display->shutdown_SFML();
		return MIM::ITEM_ABORT;
	}
	_engine->stop();
	return MIM::ITEM_QUIT;
}

auto Sorcery::Application::_run_restart() -> std::optional<MIM> {

	std::optional<MIM> rs_opt{std::nullopt};
	unsigned int chosen{0u};

	rs_opt = _restart->start(chosen);
	_restart->stop();
	if (rs_opt.value() == MIM::ITEM_ABORT) {
		display->shutdown_SFML();
		return MIM::ITEM_QUIT;
	} else if (rs_opt == MIM::RS_RESTART)
		return _restart_expedition(chosen);
	else
		return MIM::CA_EDGE_OF_TOWN;
}

auto Sorcery::Application::_run_edge_of_town() -> std::optional<MIM> {

	std::optional<MIM> opt{MIM::NO_MENU_ITEM};

	do {
		opt = _edgeoftown->start(DES::DEFAULT);
		if (opt == MIM::ET_CASTLE) {
			return MIM::ET_CASTLE;
		} else if (opt == MIM::ET_TRAIN) {

			// Remove everyone from the Party
			for (auto &[id, character] : _game->characters) {
				if (character.get_location() == CHL::PARTY)
					character.set_location(CHL::TAVERN);
			}
			_game->state->clear_party();
			_game->save_game();
			_training->start();
			_training->stop();
			display->set_input_mode(WIM::NAVIGATE_MENU);
		} else if (opt == MIM::ET_RESTART) {
			return MIM::ET_RESTART;
		} else if (opt == MIM::ET_MAZE) {
			return MIM::ET_MAZE;
		} else if (opt == MIM::ET_LEAVE_GAME)
			return MIM::ITEM_LEAVE_GAME;

	} while (opt != MIM::ITEM_ABORT);

	return MIM::ITEM_ABORT;
}

auto Sorcery::Application::_restart_expedition(
	const unsigned int character_chosen) -> std::optional<MIM> {

	// Find the location and floor of the character pointed to, and reload the
	// maze, repopulate the party and restart the game from there
	auto &character{_game->characters[character_chosen]};
	const auto to_depth{character.depth.value()};
	const auto to_loc{character.coordinate.value()};
	_game->state->clear_party();
	for (auto &[id, character] : _game->characters) {
		if (character.get_location() == CHL::MAZE) {
			if (character.depth.value() == to_depth &&
				character.coordinate.value() == to_loc) {
				character.set_location(CHL::PARTY);
				_game->state->add_character_by_id(id);
			}
		}
	}

	_game->state->set_depth(to_depth);
	_game->state->set_player_prev_depth(_game->state->get_depth());
	_game->state->set_player_pos(to_loc);
	Level level{((*_game->levelstore)[to_depth]).value()};
	_game->state->set_current_level(&level);

	auto engine{std::make_unique<Engine>(
		system.get(), display.get(), graphics.get(), _game.get())};
	if (auto result{engine->start()}; result == EXIT_APP) {
		_game->save_game();
		engine->stop();
		display->shutdown_SFML();
		return MIM::ITEM_ABORT;
	}
	_engine->stop();

	return MIM::RS_RESTART;
}

auto Sorcery::Application::_run_castle() -> std::optional<MIM> {

	std::optional<MIM> opt{MIM::NO_MENU_ITEM};

	do {

		opt = _castle->start();
		if (opt == MIM::CA_TAVERN) {

			// Tavern
			if (auto opt_tav{_tavern->start()};
				opt_tav && opt_tav.value() == MIM::ITEM_ABORT) {
				return MIM::ITEM_ABORT;
			}
			_tavern->stop();

		} else if (opt == MIM::CA_INN) {

			// Inn
			if (auto opt_inn{_inn->start()};
				opt_inn && opt_inn.value() == MIM::ITEM_ABORT) {
				return MIM::ITEM_ABORT;
			}
			_inn->stop();
		} else if (opt == MIM::CA_SHOP) {

			// Shop
			if (auto opt_shop{_shop->start()};
				opt_shop && opt_shop.value() == MIM::ITEM_ABORT) {
				return MIM::ITEM_ABORT;
			}
			_shop->stop();
		} else if (opt == MIM::CA_TEMPLE) {
			if (auto opt_temp{_temple->start()};
				opt_temp && opt_temp.value() == MIM::ITEM_ABORT) {
				return MIM::ITEM_ABORT;
			}
			_temple->stop();
		} else if (opt == MIM::CA_EDGE_OF_TOWN)
			return MIM::CA_EDGE_OF_TOWN;
		else if (opt == MIM::ITEM_LEAVE_GAME)
			return MIM::ITEM_LEAVE_GAME;

		_game->save_game();
		display->set_input_mode(WIM::NAVIGATE_MENU);
		_castle->stop();

	} while (opt != MIM::ITEM_ABORT);

	return MIM::ITEM_ABORT;
}

// Run the Main Menu
auto Sorcery::Application::_run_main_menu(const DES dest)
	-> std::optional<MIM> {

	// Handle shortcuts
	if (dest == DES::CONTINUE)
		return MIM::MM_CONTINUE_GAME;
	else if (dest == DES::NEW) {
		_game->wipe_data();
		_game->create_game();
		_game->save_game();
		return MIM::MM_NEW_GAME;
	} else if (dest == DES::RESTART) {
		return MIM::RS_RESTART;
	}

	std::optional<MIM> opt{MIM::NO_MENU_ITEM};
	MMT stage{MMT::ATTRACT_MODE};

	do {
		if (dest != DES::DEFAULT) {
			switch (dest) {
			case DES::OPTIONS:
				opt = MIM::MM_OPTIONS;
				break;
			case DES::LICENSE:
				opt = MIM::MM_LICENSE;
				break;
			case DES::COMPENDIUM:
				opt = MIM::MM_COMPENDIUM;
				break;
			default:
				opt = std::nullopt;
			}

		} else {
			opt = _mainmenu->start(stage);
			_mainmenu->stop();
		}

		if (opt) {
			switch (opt.value()) {
			case MIM::MM_NEW_GAME:
				_game->create_game();
				_game->save_game();
				return MIM::MM_NEW_GAME;
				break;
			case MIM::MM_CONTINUE_GAME:
				return MIM::MM_CONTINUE_GAME;
				break;
			case MIM::ITEM_QUIT:
				_game->save_game();
				return MIM::ITEM_QUIT;
				break;
			case MIM::MM_OPTIONS:
				if (_options->start() == EXIT_APP) {
					_options->stop();
					return MIM::ITEM_QUIT;
				} else
					_options->stop();
				break;
			case MIM::MM_LICENSE:
				if (_license->start() == EXIT_APP) {
					_license->stop();
					return MIM::ITEM_QUIT;
				} else
					_license->stop();
				break;
			case MIM::MM_COMPENDIUM:
				if (_compendium->start() == EXIT_APP) {
					_compendium->stop();
					return MIM::ITEM_QUIT;
				} else
					_compendium->stop();
				break;
			default:
				break;
			}

			stage = MMT::ATTRACT_MENU;
		}

	} while (opt);

	display->shutdown_SFML();
	return MIM::ITEM_QUIT;
}

// Check for a command line parameter
auto Sorcery::Application::_check_param(std::string_view param) const -> bool {

	return std::ranges::any_of(_arguments,
		[&](const auto &arg) { return arg.find(param) != std::string::npos; });
}

auto Sorcery::Application::_display_loading_window() -> void {

	// Just use the 2nd highest screen resolution to handle multimonitor modes
	const std::vector<sf::VideoMode> vm{sf::VideoMode::getFullscreenModes()};
	const sf::Vector2i scr{
		static_cast<int>(vm[1].width), static_cast<int>(vm[1].height)};

	// Hard Coded since we don't have access to any file resources at this point
	const std::filesystem::path base_path{_get_exe_path()};
	const std::filesystem::path image_path{
		base_path / GRAPHICS_DIR / LOADING_IMAGE};
	const std::filesystem::path font_path{
		base_path / DATA_DIR / MONO_FONT_FILE};
	auto scale{0.75f};
	sf::Image loading{};
	loading.loadFromFile(image_path.string());

	const sf::Vector2u ss{
		static_cast<unsigned int>(loading.getSize().x * scale),
		static_cast<unsigned int>(loading.getSize().y * scale)};

	_load_font.loadFromFile(font_path);
	_load_text.setFont(_load_font);
	_load_text.setColor(sf::Color::White);
	_load_text.setCharacterSize(24);
	_load_text.setPosition(16, ss.y - 48);

	// can't use sf::Style::None here due to None being defined somewhere else
	// (no I don't know either), so use 0
	_load_window.create(
		sf::VideoMode(ss.x, ss.y), "Sorcery: Shadows under Llylgamyn", 0);
	_load_window.setVerticalSyncEnabled(true);
	_load_window.setPosition(
		sf::Vector2i((scr.x - ss.x) / 2, (scr.y - ss.y) / 2));
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

auto Sorcery::Application::_update_loading_window() -> void {

	static std::string message{"LOADING"};

	message.append(".");
	_load_text.setString(message);
	_load_window.draw(_load_text);
	_load_window.display();
}

// This is linux only
auto Sorcery::Application::_get_exe_path() const -> std::string_view {

	char result[PATH_MAX];
	if (const ssize_t count{readlink("/proc/self/exe", result, PATH_MAX)};
		count != -1) {
		const char *path{dirname(result)};
		std::string_view base_path{path};
		return base_path;
	} else
		return "";
}
