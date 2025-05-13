// Copyright (C) 2025 Dave Moore
//
// This file is part of Sorcery.
//
// Sorcery is free software: you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software
// Foundation, either version 2 of the License, or (at your option) any later
// version.
//
// Sorcery is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with
// Sorcery.  If not, see <http://www.gnu.org/licenses/>.
//
// If you modify this program, or any covered work, by linking or combining
// it with the libraries referred to in README (or a modified version of
// said libraries), containing parts covered by the terms of said libraries,
// the licensors of this program grant you additional permission to convey
// the resulting work.

#include "core/controller.hpp"
#include "core/define.hpp"
#include "core/display.hpp"
#include "core/system.hpp"
#include "core/ui.hpp"
#include "engine/define.hpp"
#include "gui/define.hpp"
#include "gui/modal.hpp"
#include "types/game.hpp"
#include "types/item.hpp"
#include "types/state.hpp"

Sorcery::Controller::Controller(System *system, Display *display)
	: _system{system},
	  _display{display} {

	initialise("");
	_game = nullptr;
}

auto Sorcery::Controller::initialise(std::string_view value) -> void {

	screen = value;
	_flags.clear();
	_texts.clear();
	selected.clear();
	_characters.clear();
	destination = DEST_NONE;
	busy = false;
	last = value;
	_has_save = _system->db->has_game();

	// Set default state (these must all be present and set to false/-1)
	clear_character("inspect");
	clear_character("restart");
	clear_character("stay");
	clear_character("tithe");
	clear_character("pay");
	clear_character("help");
	// Note that the show_* flags don't need to be preset
	unset_flag("after_tile_message");
	unset_flag("napping_finished");
	unset_flag("party_order_changed");
	unset_flag("recuperating_finished");
	unset_flag("select_previous_character");
	unset_flag("select_next_character");
	unset_flag("want_abort");
	unset_flag("want_continue_game");
	unset_flag("want_cannot_donate");
	unset_flag("want_divvy_gold");
	unset_flag("want_donate");
	unset_flag("want_donated_ok");
	unset_flag("want_enter_maze");
	unset_flag("want_exit_game");
	unset_flag("want_help");
	unset_flag("want_inspect");
	unset_flag("want_identify");
	unset_flag("want_leave_game");
	unset_flag("want_not_enough_gold");
	unset_flag("want_new_game");
	unset_flag("want_quit_expedition");
	unset_flag("want_stay");
	unset_flag("want_take_stairs_down");
	unset_flag("want_take_stairs_up");
	unset_flag("want_tithe");

	unset_text("heal_results");
}

auto Sorcery::Controller::get_flags() const -> std::string {

	std::string output{};
	for (const auto &flag : _flags)
		output.append(fmt::format("{:>26}: {}\n", flag.first, flag.second));

	return output;
}

auto Sorcery::Controller::get_characters() const -> std::string {

	std::string output{};
	for (const auto &character : _characters)
		output.append(
			fmt::format("{:>26}: {}\n", character.first, character.second));

	return output;
}

auto Sorcery::Controller::has_saved_game() const -> bool {

	return _has_save;
}

auto Sorcery::Controller::set_game(Game *game) -> void {

	_game = game;
}

auto Sorcery::Controller::check_for_debug(const SDL_Event event) -> void {

	if (event.type == SDL_KEYDOWN &&
		(event.key.keysym.sym >= SDLK_F1 && event.key.keysym.sym <= SDLK_F11)) {
		if (_game != nullptr)
			_game->call_debug(event.key.keysym.sym);
	} else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_F12) {

		std::cout << std::endl;
		for (auto const &[key, val] : _flags)
			std::cout << fmt::format("{:>32}: {}", key, val) << std::endl;
	}
}

auto Sorcery::Controller::check_for_ui_toggle(const SDL_Event event) -> void {

	if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_s)
		show_party_panel = !show_party_panel;
	else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_o)
		show_ui = !show_ui;
	else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_m)
		show_automap = !show_automap;
	else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_n)
		monochrome = !monochrome;
}

auto Sorcery::Controller::check_for_movement(const SDL_Event event) -> int {

	if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_x)
		return MOVE_TURN_AROUND;
	else if (event.type == SDL_KEYDOWN && (event.key.keysym.sym == SDLK_a ||
										   event.key.keysym.sym == SDLK_l ||
										   event.key.keysym.sym == SDLK_LEFT))
		return MOVE_TURN_LEFT;
	else if (event.type == SDL_KEYDOWN && (event.key.keysym.sym == SDLK_d ||
										   event.key.keysym.sym == SDLK_r ||
										   event.key.keysym.sym == SDLK_RIGHT))
		return MOVE_TURN_RIGHT;
	else if (event.type == SDL_KEYDOWN && (event.key.keysym.sym == SDLK_w ||
										   event.key.keysym.sym == SDLK_f ||
										   event.key.keysym.sym == SDLK_UP))
		return MOVE_FORWARD;
	else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_DOWN)
		return MOVE_BACKWARD;
	else
		return MOVE_NONE;
}

auto Sorcery::Controller::set(std::string_view value) -> void {

	screen = value;
	last = value;
}

auto Sorcery::Controller::get() const -> std::string_view {

	return screen;
}

// Special Handling for Disable or Enable Menu Items
auto Sorcery::Controller::is_menu_item_disabled(const std::string &component,
												const int selection,
												const int data) -> bool {

	// Remember this is returning true if the item is meant to be disabled!
	if (component == "main_menu" && selection == MAIN_MENU_CONTINUE_GAME) {

		// Check to see if we have a saved game
		return !has_saved_game();
	} else if (component == "castle_menu") {
		if (_game != nullptr) {

			// Check for Party Members
			switch (selection) {
			case 1: // Inn
				[[fallthrough]];
			case 2: // Shop
				return !_game->state->party_has_members();
				break;
			default:
				return false;
				break;
			};
		}
	} else if (component == "edge_menu") {
		if (_game != nullptr) {

			// Check for Party Members
			switch (selection) {
			case 1: // Enter Maze
				return !_game->state->party_has_members();
				break;
			case 2: // Restart
				return _game->state->party_has_members();
				break;
			default:
				return false;
			};
		}
	} else if (component == "tavern_menu") {
		if (_game != nullptr) {

			// Check for Party Members
			switch (selection) {
			case 0: // Add to Party
				return _game->state->get_party_size() == 6;
				break;
			case 1: // Remove from Party
				[[fallthrough]];
			case 2: // Reorder Party
				[[fallthrough]];
			case 3: // Inspect Party
				[[fallthrough]];
			case 4: // Divvy Gold
				return !_game->state->party_has_members();
				break;
			default:
				return false;
			};
		}
	} else if (component == "add_menu") {
		if (_game != nullptr) {

			if (data == -1)
				return false;

			if (_game->state->get_party_size() == 6)
				return true;

			// Check for Alignment
			const auto party_align{_game->get_party_alignment()};
			const auto &candidate{_game->characters[data]};
			if (candidate.get_alignment() == Enums::Character::Align::NEUTRAL)
				return false;
			else if (party_align != Enums::Character::Align::NEUTRAL)
				return candidate.get_alignment() != party_align;
			else
				return false;
		}
	} else if (component == "rest_menu") {

		if (_game != nullptr) {

			// Work out what menu items are disabled due to lack of money
			const auto character{_game->characters.at(_characters["stay"])};
			const auto gold{character.get_gold()};

			switch (selection) {
			case 0:
				// The Stables
				return false;
			case 1:
				// A Cot
				return gold < 10;
			case 2:
				// Economy Rooms
				return gold < 50;
			case 3:
				// Merchant Suites
				return gold < 200;
			case 4:
				// The Royal Suite
				return gold < 500;
			default:
				return false;
			};
		}
	} else if (component == "pay_menu") {
		if (_game != nullptr) {

			if (data == -1)
				return false;

			const auto &help{_game->characters.at(_characters["help"])};
			const auto &who{_game->characters[data]};
			return help.get_cure_cost() > who.get_gold();
		}
	} else if (component == "identify_menu" || component == "modal_identify") {

		if (has_character("inspect")) {

			const auto &who{_game->characters.at(_characters["inspect"])};
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
			if (selection < who.inventory.items().size()) {
				const auto item{who.inventory.items().at(selection)};
				return item.get_known();
			} else
				return false;
#pragma GCC diagnostic pop
		} else
			return false;
	}
}

// Toggle Handling
auto Sorcery::Controller::handle_toggle(const std::string &component,
										const std::string &tab,
										const int selection) -> void {

	using Enums::Config::Options;

	if (component == "options_info") {
		if (tab == "Options") {

			// This happens after the corresponding data is changed
			if (selection == static_cast<int>(Options::RECOMMENDED_MODE) &&
				(*_system->config)[Options::RECOMMENDED_MODE])
				_system->config->set_rec_mode();
			else if (selection == static_cast<int>(Options::STRICT_MODE) &&
					 (*_system->config)[Options::STRICT_MODE])
				_system->config->set_strict_mode();
			else {
				if (_system->config->is_strict_mode())
					_system->config->set_strict_mode();
				else if (_system->config->is_rec_mode())
					_system->config->set_rec_mode();
			}

		} else if (tab == "Gameplay") {

			// Only need to check if strict and reommended modes are on
			(*_system->config)[Options::RECOMMENDED_MODE] =
				_system->config->is_rec_mode();
			(*_system->config)[Options::STRICT_MODE] =
				_system->config->is_strict_mode();

		} else if (tab == "Graphics") {

			// Only need to check if strict and reommended modes are on
			(*_system->config)[Options::RECOMMENDED_MODE] =
				_system->config->is_rec_mode();
			(*_system->config)[Options::STRICT_MODE] =
				_system->config->is_strict_mode();
		}
	}
}

// Menu Setting but with Flags (used with a temporary vector of references)
// - note that we need to use this instead of the normal handle_menu method
// for any menus that has an option for a confirm/notice from it
auto Sorcery::Controller::handle_menu_with_flags(
	const std::string &component,
	[[maybe_unused]] const std::vector<std::string> &items,
	[[maybe_unused]] const int data, const int selection,
	std::vector<std::reference_wrapper<bool>> in_flags) -> void {

	if (component == "main_menu") {

		// Flags = &_ui->dialog_new->show, &_ui->dialog_exit->show
		switch (selection) {
		case MAIN_MENU_NEW_GAME:
			in_flags.at(0).get() = true;
			break;
		case MAIN_MENU_CONTINUE_GAME:
			busy = true;
			_flags["want_continue_game"] = true;
			break;
		case MAIN_MENU_OPTIONS:
			destination = selection;
			move_screen("show_main_menu", "show_options");
			break;
		case MAIN_MENU_COMPENDIUM:
			destination = selection;
			move_screen("show_main_menu", "show_compendium");
			break;
		case MAIN_MENU_LICENSE:
			destination = selection;
			move_screen("show_main_menu", "show_license");
			break;
		case MAIN_MENU_EXIT_GAME:
			in_flags.at(1).get() = true;
			break;
		default:
			break;
		};
	} else if (component == "edge_menu") {

		// Flags = &_ui->dialog_leave->show,
		switch (selection) {
		case EDGE_OF_TOWN_GO_TO_CASTLE:
			move_screen("show_edge_of_town", "show_castle");
			break;
		case EDGE_OF_TOWN_GO_TO_TRAINING:
			move_screen("show_edge_of_town", "show_training_grounds");
			break;
		case EDGE_OF_TOWN_RESTART:
			move_screen("show_edge_of_town", "show_restart");
			break;
		case EDGE_OF_TOWN_GO_TO_MAZE:
			_flags["want_enter_maze"] = true;
			break;
		case EDGE_OF_TOWN_LEAVE_GAME:
			in_flags.at(0).get() = true;
			break;
		default:
			break;
		};

	} else if (component == "inn_menu") {

		// Inn

		// Flags = &_ui->modal_inspect->show, &_ui->modal_stay->show
		if (selection == INN_STAY)
			in_flags.at(1).get() = true;
		else if (selection == INN_INSPECT)
			in_flags.at(0).get() = true;
		else if (selection == (static_cast<int>(items.size()) - 1))
			move_screen("show_inn", "show_castle");

	} else if (component == "tavern_menu") {

		// Tavern

		// Flags = &_ui->notice_divvy->show, &_ui->modal_inspect->show,
		if (selection == TAVERN_ADD)
			move_screen("show_tavern", "show_add");
		else if (selection == TAVERN_REMOVE)
			move_screen("show_tavern", "show_remove");
		else if (selection == TAVERN_INSPECT)
			in_flags.at(1).get() = true;
		else if (selection == TAVERN_REORDER)
			move_screen("show_tavern", "show_reorder");
		else if (selection == TAVERN_DIVVY_GOLD)
			in_flags.at(0).get() = true;
		else if (selection == (static_cast<int>(items.size()) - 1))
			move_screen("show_tavern", "show_castle");

	} else if (component == "temple_menu") {

		// Inn

		// Flags = &_ui->modal_inspect->show, &_ui->modal_help->show,
		// &_ui->modal_tithe->show
		if (selection == TEMPLE_TITHE)
			in_flags.at(2).get() = true;
		else if (selection == TEMPLE_HELP)
			in_flags.at(1).get() = true;
		else if (selection == TEMPLE_INSPECT)
			in_flags.at(0).get() = true;
		else if (selection == (static_cast<int>(items.size()) - 1))
			move_screen("show_temple", "show_castle");

	} else if (component == "camp_menu") {

		// Camp

		// Flags = &_ui->modal_camp->show
		if (selection == CAMP_INSPECT)
			move_screen("show_engine", "show_inspect");
		else if (selection == CAMP_REORDER)
			move_screen("show_engine", "show_reorder");
		else if (selection == CAMP_OPTIONS)
			move_screen("show_engine", "show_options");
		else if (selection == CAMP_QUIT)
			_flags["want_quit_expedition"] = true;
		else if (selection == CAMP_LEAVE) {
			// Exit from Camp
		}

		// In all cases remove modal
		in_flags.at(0).get() = false;

	} else if (component == "inspect_menu" || component == "modal_inspect") {

		// Get the Character ID of the Selected Character and set it
		if (selection == (static_cast<int>(items.size()) - 1))
			clear_character("inspect");
		else
			set_character("inspect", data);

		// Remove the Modal
		in_flags.at(0).get() = false;

	} else if (component == "stay_menu" || component == "modal_stay") {

		// Get the Character ID of the Selected Character and set it
		if (selection == (static_cast<int>(items.size()) - 1))
			clear_character("stay");
		else
			set_character("stay", data);

		// Remove the Modal
		in_flags.at(0).get() = false;
	} else if (component == "tithe_menu" || component == "modal_tithe") {

		// Flags = &_ui->modal_tithe->show, &_ui->input_donate->show,

		// Get the Character ID of the Selected Character and set it
		if (selection == (static_cast<int>(items.size()) - 1))
			clear_character("tithe");
		else {
			set_character("tithe", data);
			_flags["want_donate"] = true;
			in_flags.at(1).get() = true;
		}

		// Remove the Modal
		in_flags.at(0).get() = false;
	} else if (component == "help_menu" || component == "modal_help") {

		// Flags = &_ui->modal_help->show

		// Get the Character ID of the Selected Character and set it
		if (selection == (static_cast<int>(items.size()) - 1))
			clear_character("help");
		else
			set_character("help", data);

		// Remove the Modal
		in_flags.at(0).get() = false;
	} else if (component == "identify_menu" || component == "modal_identify") {

		// Flags = &_ui->modal_identify->show
		if (selection == (static_cast<int>(items.size()) - 1)) {
			_flags["want_identify"] = true;
			in_flags.at(1).get() = false;
		} else {
		}
	}
}

auto Sorcery::Controller::get_flag_ref(const std::string &flag) -> bool & {

	return _flags.at(flag);
}

auto Sorcery::Controller::get_flag(const std::string &flag) const -> bool {

	if (_flags.contains(flag))
		return _flags.at(flag);
	else
		return false;
}

auto Sorcery::Controller::set_flag(const std::string &flag) -> void {

	_flags[flag] = true;
}

auto Sorcery::Controller::unset_flag(const std::string &flag) -> void {

	_flags[flag] = false;
}

auto Sorcery::Controller::has_flag(const std::string &flag) const -> bool {

	if (_flags.contains(flag))
		return _flags.at(flag) == true;

	return false;
}

auto Sorcery::Controller::has_text(const std::string &flag) const -> bool {

	if (_texts.contains(flag))
		return _texts.at(flag).length() > 0;

	return false;
}
auto Sorcery::Controller::set_text(const std::string &flag,
								   const std::string &text) -> void {

	_texts[flag] = text;
}
auto Sorcery::Controller::unset_text(const std::string &flag) -> void {

	_texts[flag] = "";
}

auto Sorcery::Controller::get_text(const std::string &flag) const
	-> const std::string {
	if (_texts.contains(flag))
		return _texts.at(flag);

	return "";
}

auto Sorcery::Controller::get_character(const std::string &flag) const -> int {

	if (_characters.contains(flag))
		return _characters.at(flag);
	else
		return -1;
}

auto Sorcery::Controller::has_character(const std::string &flag) const -> bool {

	if (_characters.contains(flag))
		if (_characters.at(flag) != -1)
			return true;

	return false;
}
auto Sorcery::Controller::set_character(const std::string &flag,
										const int value) -> void {

	_characters[flag] = value;
}

auto Sorcery::Controller::clear_character(const std::string &flag) -> void {

	_characters[flag] = -1;
}

// Check if the SDL event is go-back-to-previous event
auto Sorcery::Controller::check_for_back(const SDL_Event event) -> bool {

	if (event.type == SDL_KEYDOWN &&
		event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
		return true;
	else if (event.type == SDL_MOUSEBUTTONDOWN &&
			 event.button.button == SDL_BUTTON_RIGHT)
		return true;

	return false;
}

// Check for a resize event
auto Sorcery::Controller::check_for_resize(const SDL_Event event, UI *ui)
	-> void {

	if (event.type == SDL_WINDOWEVENT &&
		event.window.event == SDL_WINDOWEVENT_RESIZED) {

		// Handle Resizing of Windows
		auto sz{_display->get_SDL_window_size()};
		ui->adj_grid_w = sz.w / ui->columns;
		ui->adj_grid_h = sz.h / ui->rows;
	};
}

// Check if the SDL event is go-back-to-previous event (override to
// set a flag, for example to display a dialog box!)
auto Sorcery::Controller::check_for_back(const SDL_Event event, bool &flag)
	-> void {

	if (event.type == SDL_KEYDOWN &&
		event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
		flag = true;
	else if (event.type == SDL_MOUSEBUTTONDOWN &&
			 event.button.button == SDL_BUTTON_RIGHT)
		flag = true;
}

// Check if the SDL event is a Window-Shut-Down event
auto Sorcery::Controller::check_for_abort(const SDL_Event event) -> bool {

	// SDL_QUIT event
	if (event.type == SDL_QUIT) {
		_flags["want_abort"] = true;
		return true;
	}

	// Window Close event
	if (event.type == SDL_WINDOWEVENT &&
		event.window.event == SDL_WINDOWEVENT_CLOSE &&
		event.window.windowID == SDL_GetWindowID(_display->get_SDL_window())) {
		_flags["want_abort"] = true;
		return true;
	}

	return false;
}

auto Sorcery::Controller::handle_button_click(const std::string &component,
											  UI *ui,
											  [[maybe_unused]] const int data)
	-> void {

	if (component == "button_identify") {

		ui->modal_identify->regenerate(this, _game);
		ui->modal_identify->show = true;
		set_flag("want_identify");
	}
}

// Menu Handling
auto Sorcery::Controller::handle_menu(const std::string &component,
									  const std::vector<std::string> &items,
									  const int data, const int selection)
	-> void {

	if (component == "compendium_menu") {

		// Compendium
		if (selection == COMPENDIUM_ATLAS)
			move_screen("show_compendium", "show_atlas");
		else if (selection == COMPENDIUM_BESTIARY)
			move_screen("show_compendium", "show_bestiary");
		else if (selection == COMPENDIUM_MUSEUM)
			move_screen("show_compendium", "show_museum");
		else if (selection == COMPENDIUM_SPELLBOOK)
			move_screen("show_compendium", "show_spellbook");
		else if (selection == (static_cast<int>(items.size()) - 1))
			move_screen("show_compendium", "show_main_menu");
	} else if (component == "castle_menu") {

		// Castle
		if (selection == CASTLE_GO_TO_EDGE_OF_TOWN)
			move_screen("show_castle", "show_edge_of_town");
		else if (selection == CASTLE_GO_TO_TAVERN)
			move_screen("show_castle", "show_tavern");
		else if (selection == CASTLE_GO_TO_INN)
			move_screen("show_castle", "show_inn");
		else if (selection == CASTLE_GO_TO_SHOP)
			move_screen("show_castle", "show_shop");
		else if (selection == CASTLE_GO_TO_TEMPLE)
			move_screen("show_castle", "show_temple");
	} else if (component == "remove_menu") {

		if (selection == (static_cast<int>(items.size()) - 1))
			move_screen("show_remove", "show_tavern");
		else {

			// if we can, remove the character from the party
			auto &character{_game->characters[data]};
			character.set_location(Enums::Character::Location::TAVERN);
			_game->state->remove_character_by_id(data);
			_game->save_game();
		}

	} else if (component == "restart_menu") {

		// Restart Menu
		if (selection == (static_cast<int>(items.size()) - 1))
			move_screen("show_restart", "show_edge_of_town");
		else {

			// Get the ID of the Character if we can, add the character to
			// the party
			set_character("restart", data);
			_flags["want_restart_expedition"] = true;
		}

	} else if (component == "add_menu") {

		if (selection == (static_cast<int>(items.size()) - 1))
			move_screen("show_add", "show_tavern");
		else {

			// if we can, add the character to the party
			auto &character{_game->characters[data]};
			character.set_location(Enums::Character::Location::PARTY);
			_game->state->add_character_to_party(data);
			_game->save_game();
		}

	} else if (component == "roster_menu") {

		// Roster has multiple entry points so need to rely upon
		// calling screen to enable itself
		if (selection == (static_cast<int>(items.size()) - 1)) {
			_flags["show_roster"] = false;
			clear_character("inspect");
		} else {
			set_character("inspect", data);
			_flags["show_roster"] = false;
		}
	} else if (component == "reorder_menu") {

		// Reorder has multiple entry points so need to rely upon
		// calling screen to enable itself
		if (selection == (static_cast<int>(items.size()) - 1))
			_flags["show_reorder"] = false;

	} else if (component == "pay_menu") {

		if (selection == (static_cast<int>(items.size()) - 1))
			_flags["show_pay"] = false;
		else
			selected["pay_selected"] = selection;

	} else if (component == "shop_menu") {

		// Shop
		if (selection == SHOP_INSPECT)
			move_screen("show_shop", "show_roster");
		else if (selection == (static_cast<int>(items.size()) - 1))
			move_screen("show_shop", "show_castle");
	} else if (component == "temple_menu") {

		// Temple
		if (selection == TEMPLE_INSPECT)
			move_screen("show_temple", "show_roster");
		else if (selection == (static_cast<int>(items.size()) - 1))
			move_screen("show_temple", "show_castle");
	} else if (component == "training_menu") {

		// Training Grounds
		if (selection == (static_cast<int>(items.size()) - 1))
			move_screen("show_training_grounds", "show_edge_of_town");
	} else if (component == "bestiary_menu") {

		// Bestiary
		selected["bestiary_selected"] = selection;
		if (selection == (static_cast<int>(items.size()) - 1))
			move_screen("show_bestiary", "show_compendium");
	} else if (component == "museum_menu") {

		// Museum
		selected["museum_selected"] = selection;
		if (selection == (static_cast<int>(items.size()) - 1))
			move_screen("show_museum", "show_compendium");
	} else if (component == "atlas_menu") {

		// Atlas
		selected["atlas_selected"] = selection;
		if (selection == (static_cast<int>(items.size()) - 1))
			move_screen("show_atlas", "show_compendium");
	} else if (component == "spellbook_menu") {

		// Spellbook
		selected["spellbook_selected"] = selection;
		if (selection == (static_cast<int>(items.size()) - 1))
			move_screen("show_spellbook", "show_compendium");
	} else if (component == "choose_menu") {

		// Character Selection
		if (selection == (static_cast<int>(items.size()) - 1)) {
			_flags["show_choose"] = false;
			clear_character("choose");
		} else
			set_character("choose", data);
	} else if (component == "rest_menu") {

		// Resting
		selected["stay_selected"] = selection + 1;
		if (selection == (static_cast<int>(items.size()) - 1 + 1))
			move_screen("show_stay", "show_inn");
		else
			move_screen("show_stay", "show_recovery");
	}
}

// Move from one screen to another
auto Sorcery::Controller::move_screen(const std::string &from_screen,
									  const std::string &to_screen) -> void {

	_flags[to_screen] = true;
	_flags[from_screen] = false;
}
