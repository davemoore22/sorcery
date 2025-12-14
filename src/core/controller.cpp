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
#include "core/context.hpp"
#include "core/database.hpp"
#include "core/define.hpp"
#include "core/display.hpp"
#include "core/resources.hpp"
#include "core/system.hpp"
#include "core/ui.hpp"
#include "engine/define.hpp"
#include "gui/define.hpp"
#include "gui/dialog.hpp"
#include "gui/modal.hpp"
#include "resources/itemstore.hpp"
#include "types/character.hpp"
#include "types/config.hpp"
#include "types/game.hpp"
#include "types/item.hpp"
#include "types/state.hpp"

Sorcery::Controller::Controller(Context &ctx)
	: _ctx{ctx} {

	initialise("");
	_game = nullptr;
}

auto Sorcery::Controller::initialise(std::string_view value) -> void {

	_screen = value;
	_busy = false;
	_last_screen = value;
	_has_save = _ctx.database->has_game();

	// TODO: are these needed?
	_flags.clear();
	_texts.clear();
	_selected.clear();
	_characters.clear();

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
	unset_flag("want_camp");
	unset_flag("want_cannot_donate");
	unset_flag("want_continue_game");
	unset_flag("want_divvy_gold");
	unset_flag("want_donate");
	unset_flag("want_donated_ok");
	unset_flag("want_drop");
	unset_flag("want_enter_maze");
	unset_flag("want_exit_game");
	unset_flag("want_help");
	unset_flag("want_inspect");
	unset_flag("want_identify");
	unset_flag("want_invoke");
	unset_flag("want_leave_game");
	unset_flag("want_name");
	unset_flag("want_name_ok");
	unset_flag("want_not_enough_gold");
	unset_flag("want_new_game");
	unset_flag("want_pool_gold");
	unset_flag("want_quit_expedition");
	unset_flag("want_stay");
	unset_flag("want_take_stairs_down");
	unset_flag("want_take_stairs_up");
	unset_flag("want_tithe");
	unset_flag("want_trade");
	unset_flag("want_use");

	unset_flag("debug_ui");

	unset_text("heal_results");

	set_selected("bestiary_selected", 0);
	set_selected("spellbook_selected", 0);
	set_selected("museum_selected", 1);
	set_selected("class_selected", 8);
	set_selected("atlas_selected", 8);

	// need to set ui flags too, argh
}

auto Sorcery::Controller::add_to_candidate_party(unsigned int value) -> void {

	_candidate_party.emplace_back(value);
}

auto Sorcery::Controller::clear_candidate_party() -> void {

	_candidate_party.clear();
}
auto Sorcery::Controller::get_candidate_party() -> std::vector<unsigned int> & {

	return _candidate_party;
}
auto Sorcery::Controller::get_candidate_party() const
	-> const std::vector<unsigned int> & {

	return _candidate_party;
}

auto Sorcery::Controller::get_last_event() const -> Enums::Map::Event {

	return _last_event;
}

auto Sorcery::Controller::set_last_event(const Enums::Map::Event value)
	-> void {

	_last_event = value;
}

auto Sorcery::Controller::get_last_dir() const -> Enums::Map::Direction {

	return _last_dir;
}

auto Sorcery::Controller::set_last_dir(const Enums::Map::Direction value)
	-> void {

	_last_dir = value;
}

auto Sorcery::Controller::get_flags() const -> std::string {

	std::string output{};
	for (const auto &flag : _flags)
		output.append(std::format("{:>26}: {}\n", flag.first, flag.second));

	return output;
}

auto Sorcery::Controller::get_characters() const -> std::string {

	std::string output{};
	for (const auto &character : _characters)
		output.append(
			std::format("{:>26}: {}\n", character.first, character.second));

	return output;
}

auto Sorcery::Controller::set_monochrome(const bool value) -> void {

	_monochrome = value;
}
auto Sorcery::Controller::get_monochrome() const -> bool {

	return _monochrome;
}

auto Sorcery::Controller::set_busy(const bool value) -> void {

	_busy = value;
}
auto Sorcery::Controller::get_busy() const -> bool {

	return _busy;
}

auto Sorcery::Controller::set_fullscreen(const bool value) -> void {

	_fullscreen = value;
}
auto Sorcery::Controller::get_fullscreen() const -> bool {

	return _fullscreen;
}

auto Sorcery::Controller::has_saved_game() const -> bool {

	return _has_save;
}

auto Sorcery::Controller::set_flag_value(const std::string &flag,
										 const bool value) -> void {

	_flags[flag] = value;
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

		std::println();
		for (auto const &[key, val] : _flags)
			std::println("{}", std::format("{:>32}: {}", key, val));
	}
}

auto Sorcery::Controller::check_for_quicksave(const SDL_Event event) -> bool {

	return (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_F9);
}

auto Sorcery::Controller::check_for_quickload(const SDL_Event event) -> bool {

	return (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_F10);
}

auto Sorcery::Controller::check_for_ui_toggle(const SDL_Event event) -> void {

	if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_s)
		toggle_flag("interface_party_panel");
	else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_o)
		toggle_flag("interface_ui");
	else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_m)
		toggle_flag("interface_automap");
	else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_n)
		_monochrome = !_monochrome;
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

auto Sorcery::Controller::set_screen(std::string_view value) -> void {

	_screen = value;
	_last_screen = value;
}

auto Sorcery::Controller::get_screen() const -> std::string_view {

	return _screen;
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
	} else if (component == "drop_menu" || component == "modal_drop") {

		if (has_character("inspect")) {

			const auto &who{_game->characters.at(_characters["inspect"])};
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
			if (selection < who.inventory.items().size()) {
				const auto item{who.inventory.items().at(selection)};
				return item.get_equipped();
			} else
				return false;
#pragma GCC diagnostic pop
		} else
			return false;
	} else if (component == "trade_menu" || component == "modal_trade") {

		if (has_character("inspect")) {

			const auto &who{_game->characters.at(_characters["inspect"])};
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
			if (selection < who.inventory.items().size()) {
				const auto item{who.inventory.items().at(selection)};
				return item.get_equipped();
			} else
				return false;
#pragma GCC diagnostic pop
		} else
			return false;
	} else if (component == "use_menu" || component == "modal_use") {

		if (has_character("inspect")) {

			const auto &who{_game->characters.at(_characters["inspect"])};
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
			if (selection < who.inventory.items().size()) {
				const auto item{who.inventory.items().at(selection)};
				const auto item_type{
					_ctx.resources->items->get_item_type(item.get_type_id())};
				return !(item_type.has_usable() && item.get_known());
			} else
				return false;
#pragma GCC diagnostic pop
		} else
			return false;
	} else if (component == "invoke_menu" || component == "modal_invoke") {

		if (has_character("inspect")) {

			const auto &who{_game->characters.at(_characters["inspect"])};
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
			if (selection < who.inventory.items().size()) {
				const auto item{who.inventory.items().at(selection)};
				const auto item_type{
					_ctx.resources->items->get_item_type(item.get_type_id())};
				return !(item_type.has_invokable() && item.get_known());
			} else
				return false;
#pragma GCC diagnostic pop
		} else
			return false;
	} else if (component == "class_menu") {

		const auto classes{_create->get_pos_class()};
		if (selection >= 0 && selection < 8) {
			return !classes.at(
				magic_enum::enum_cast<Enums::Character::Class>(selection + 1)
					.value());
		} else
			return false;
	}

	return false;
}

// Toggle Handling
auto Sorcery::Controller::handle_toggle(const std::string &component,
										const std::string &tab,
										const int selection) -> void {

	if (component == "options_info") {

		using enum Enums::Config::Options;
		if (tab == "Options") {

			// This happens after the corresponding data is changed
			if (selection == static_cast<int>(RECOMMENDED_MODE) &&
				_ctx.get_config(RECOMMENDED_MODE))
				_ctx.config->set_rec_mode();
			else if (selection == static_cast<int>(STRICT_MODE) &&
					 _ctx.get_config(STRICT_MODE))
				_ctx.config->set_strict_mode();
			else {
				if (_ctx.config->is_strict_mode())
					_ctx.config->set_strict_mode();
				else if (_ctx.config->is_rec_mode())
					_ctx.config->set_rec_mode();
			}

		} else if (tab == "Gameplay") {

			// Only need to check if strict and reommended modes are on
			_ctx.get_config(RECOMMENDED_MODE) = _ctx.config->is_rec_mode();
			_ctx.get_config(STRICT_MODE) = _ctx.config->is_strict_mode();

		} else if (tab == "Graphics") {

			// Only need to check if strict and reommended modes are on
			_ctx.get_config(RECOMMENDED_MODE) = _ctx.config->is_rec_mode();
			_ctx.get_config(STRICT_MODE) = _ctx.config->is_strict_mode();
		}
	}
}

auto Sorcery::Controller::set_can_undo(const bool value) -> void {

	_can_undo = value;
}
auto Sorcery::Controller::get_can_undo() const -> bool {

	return _can_undo;
}

auto Sorcery::Controller::get_last_screen() const -> std::string {

	return _last_screen;
}
auto Sorcery::Controller::set_last_screen(const std::string &value) -> void {

	_last_screen = value;
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
			_busy = true;
			_flags["want_continue_game"] = true;
			break;
		case MAIN_MENU_OPTIONS:
			move_screen("show_main_menu", "show_options");
			break;
		case MAIN_MENU_COMPENDIUM:
			move_screen("show_main_menu", "show_compendium");
			break;
		case MAIN_MENU_LICENSE:
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
			move_screen("show_edge_of_town", "show_training");
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
			in_flags.at(0).get() = false;
		} else {
		}
	} else if (component == "drop_menu" || component == "modal_drop") {

		// Flags = &_ui->modal_drop->show
		if (selection == (static_cast<int>(items.size()) - 1)) {
			_flags["want_drop"] = true;
			in_flags.at(0).get() = false;
		} else {
		}
	} else if (component == "trade_menu" || component == "modal_trade") {

		// Flags = &_ui->modal_trade->show
		if (selection == (static_cast<int>(items.size()) - 1)) {
			_flags["want_trade"] = true;
			in_flags.at(0).get() = false;
		} else {
		}
	} else if (component == "use_menu" || component == "modal_use") {

		// Flags = &_ui->modal_use->show
		if (selection == (static_cast<int>(items.size()) - 1)) {
			_flags["want_use"] = true;
			in_flags.at(0).get() = false;
		} else {
		}
	} else if (component == "invoke_menu" || component == "modal_invoke") {

		// Flags = &_ui->modal_invoke->show
		if (selection == (static_cast<int>(items.size()) - 1)) {
			_flags["want_invoke"] = true;
			in_flags.at(0).get() = false;
		} else {
		}
	}
}

auto Sorcery::Controller::get_flag_ref(const std::string &flag) -> bool & {

	if (!_flags.contains(flag))
		_flags[flag] = false;

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

auto Sorcery::Controller::toggle_flag(const std::string &flag) -> void {

	if (_flags.contains(flag))
		_flags.at(flag) = !_flags.at(flag);
	else
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

auto Sorcery::Controller::has_selected(const std::string &flag) const -> bool {

	if (_selected.contains(flag))
		return _selected.at(flag) != -1;

	return false;
}

auto Sorcery::Controller::set_selected(const std::string &flag, const int value)
	-> void {

	_selected[flag] = value;
}

auto Sorcery::Controller::get_selected(const std::string &flag) const -> int {

	if (_selected.contains(flag))
		return _selected.at(flag);
	else
		return -1;
}

auto Sorcery::Controller::unset_selected(const std::string &flag) -> void {

	_selected[flag] = -1;
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
		int w = event.window.data1;
		int h = event.window.data2;
		ui->adj_grid_w = w / ui->columns;
		ui->adj_grid_h = h / ui->rows;
		glViewport(0, 0, w, h);
		ImGui::GetIO().DisplaySize = ImVec2((float)w, (float)h);
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
		event.window.windowID ==
			SDL_GetWindowID(_ctx.display->get_SDL_window())) {
		_flags["want_abort"] = true;
		return true;
	}

	return false;
}

auto Sorcery::Controller::get_input_buffer() -> std::string & {

	return _input_buffer;
}
auto Sorcery::Controller::set_input_buffer(const std::string &value) -> void {

	_input_buffer = value;
}

auto Sorcery::Controller::handle_stepper_button_click(
	const std::string &component, UI *ui, const bool positive, int *data)
	-> void {

	if (component.starts_with("##stepper_attribute_")) {

		if (positive) {

			// Up: If we have points left and the value is less than 18
			if ((_create->get_points_left() > 0) && (*data <= 17)) {

				(*data)++;
				_create->set_points_left(_create->get_points_left() - 1);
				_create->set_pos_class();
			}

		} else {

			if (_create->get_points_left() < _create->get_start_points()) {

				// Down: If we are above staring points
				using enum Enums::Character::Attribute;
				if (component.starts_with("##stepper_attribute_1")) {
					if (_create->get_cur_attr(STRENGTH) >
						_create->get_start_attr(STRENGTH)) {
						(*data)--;
						_create->set_points_left(_create->get_points_left() +
												 1);
						_create->set_pos_class();
					}
				} else if (component.starts_with("##stepper_attribute_2")) {
					if (_create->get_cur_attr(IQ) >
						_create->get_start_attr(IQ)) {
						(*data)--;
						_create->set_points_left(_create->get_points_left() +
												 1);
						_create->set_pos_class();
					}
				} else if (component.starts_with("##stepper_attribute_3")) {
					if (_create->get_cur_attr(PIETY) >
						_create->get_start_attr(PIETY)) {
						(*data)--;
						_create->set_points_left(_create->get_points_left() +
												 1);
						_create->set_pos_class();
					}
				} else if (component.starts_with("##stepper_attribute_4")) {
					if (_create->get_cur_attr(VITALITY) >
						_create->get_start_attr(VITALITY)) {
						(*data)--;
						_create->set_points_left(_create->get_points_left() +
												 1);
						_create->set_pos_class();
					}
				} else if (component.starts_with("##stepper_attribute_5")) {
					if (_create->get_cur_attr(AGILITY) >
						_create->get_start_attr(AGILITY)) {
						(*data)--;
						_create->set_points_left(_create->get_points_left() +
												 1);
						_create->set_pos_class();
					}
				} else if (component.starts_with("##stepper_attribute_6")) {
					if (_create->get_cur_attr(LUCK) >
						_create->get_start_attr(LUCK)) {
						(*data)--;
						_create->set_points_left(_create->get_points_left() +
												 1);
						_create->set_pos_class();
					}
				}
			}
		}
	};
};

auto Sorcery::Controller::handle_input_button_click(
	const std::string &component, UI *ui, std::string *data) -> void {

	if (component == "name_input_ok") {

		if (data->length() > 0) {

			_create->set_name(*data);
			_create->set_stage(Enums::Character::Stage::CHOOSE_RACE);

			// Finished entering name
			unset_flag("want_enter_name");
			set_flag("want_choose_race");
		}
	}
}

auto Sorcery::Controller::handle_button_click(const std::string &component,
											  UI *ui,
											  [[maybe_unused]] const int data)
	-> void {

	if (component == "button_identify") {

		// Show Identify Modal
		ui->modal_identify->regenerate(this, _game);
		ui->modal_identify->show = true;
		set_flag("want_identify");
	} else if (component == "button_pool") {

		// Show Pool Gold Notice
		ui->notice_pool_gold->show = true;
		set_flag("want_pool_gold");
		_game->pool_party_gold(get_character("inspect"));
	} else if (component == "button_leave") {

		// Leave Inspect
		unset_flag("want_inspect");
		unset_flag("show_inspect");
		ui->modal_inspect->show = false;
	} else if (component == "button_drop") {

		// Show Drop Modal
		ui->modal_drop->regenerate(this, _game);
		ui->modal_drop->show = true;
		set_flag("want_drop");
	} else if (component == "button_trade") {

		// Show Trade Modal
		ui->modal_trade->regenerate(this, _game);
		ui->modal_trade->show = true;
		set_flag("want_trade");
	} else if (component == "button_use") {

		// Show Use Modal
		ui->modal_use->regenerate(this, _game);
		ui->modal_use->show = true;
		set_flag("want_use");
	} else if (component == "button_invoke") {

		// Show Use Modal
		ui->modal_invoke->regenerate(this, _game);
		ui->modal_invoke->show = true;
		set_flag("want_invoke");
	} else if (component == "button_keep_yes") {

		// Save Character
		_create->set_stage(Enums::Character::Stage::COMPLETED);
		_create->set_location(Enums::Character::Location::TAVERN);

		Character pc{*_create};
		auto char_id{_game->save_character(pc)};
		_game->characters[char_id] = pc;
		_game->save_game();
		_create->reset(Enums::Character::Stage::CHOOSE_METHOD);
		unset_flag("create_confirm");
		unset_flag("show_create");
		set_flag("show_training");

	} else if (component == "button_keep_no") {

		_create->reset(Enums::Character::Stage::CHOOSE_METHOD);
		unset_flag("create_confirm");
		unset_flag("show_create");
		set_flag("show_training");
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

	} else if (component == "race_menu") {

		if (selection == (static_cast<int>(items.size()) - 1))
			move_screen("show_create", "show_method");
		else {
			_create->set_race(
				magic_enum::enum_cast<Enums::Character::Race>(selection + 1)
					.value());
			_create->set_stage(Enums::Character::Stage::CHOOSE_ALIGNMENT);
			_create->set_start_attr();
			unset_flag("want_choose_race");
			set_flag("want_choose_alignment");
		}

	} else if (component == "alignment_menu") {

		if (selection == (static_cast<int>(items.size()) - 1))
			move_screen("show_create", "show_method");
		else {
			_create->set_alignment(
				magic_enum::enum_cast<Enums::Character::Align>(selection + 1)
					.value());
			_create->set_stage(Enums::Character::Stage::CHOOSE_CLASS);
			_create->set_start_attr();
			_create->set_pos_class();
			set_flag("want_choose_class");
			unset_flag("want_choose_alignment");
		}
	} else if (component == "class_menu") {

		if (selection == (static_cast<int>(items.size()) - 1))
			move_screen("show_create", "show_method");
		else {
			if (_create->get_points_left() == 0) {

				_create->set_class(
					magic_enum::enum_cast<Enums::Character::Class>(selection +
																   1)
						.value());
				_create->set_stage(Enums::Character::Stage::REVIEW_AND_CONFIRM);
				_create->finalise();

				// TODO: refactor this
				_create->inventory.clear();

				switch (
					_create->get_class()) { // NOLINT(clang-diagnostic-switch)
					using enum Enums::Character::Class;
					using enum Enums::Items::TypeID;
				case FIGHTER:
				case LORD:
				case SAMURAI:
					_create->inventory.add_type(
						_ctx.resources->items->get(LEATHER_ARMOR), true);
					_create->inventory.add_type(
						_ctx.resources->items->get(LONG_SWORD), true);
					break;
				case MAGE:
					_create->inventory.add_type(
						_ctx.resources->items->get(ROBES), true);
					_create->inventory.add_type(
						_ctx.resources->items->get(DAGGER), true);
					break;
				case PRIEST:
				case BISHOP:
					_create->inventory.add_type(
						_ctx.resources->items->get(ROBES), true);
					_create->inventory.add_type(
						_ctx.resources->items->get(STAFF), true);
					break;
				case THIEF:
				case NINJA:
					_create->inventory.add_type(
						_ctx.resources->items->get(LEATHER_ARMOR), true);
					_create->inventory.add_type(
						_ctx.resources->items->get(SHORT_SWORD), true);
				default:
					break;
				}

				set_flag("want_choose_confirm");
				unset_flag("want_choose_class");
			}
		};
	} else if (component == "method_menu") {

		if (selection == (static_cast<int>(items.size()) - 1))
			move_screen("show_method", "show_training");
		else {

			// Which method of character creation do we want to do?

			if (selection == METHOD_FULL) {
				_method = Enums::Character::Method::FULL;
				move_screen("show_method", "show_create");
			} else if (selection == METHOD_QUICK) {
				_method = Enums::Character::Method::QUICK;
				move_screen("show_method", "show_create");
			} else if (selection == METHOD_RANDOM) {
				_method = Enums::Character::Method::RANDOM;
				move_screen("show_method", "show_create");
			}
		}
	} else if (component == "roster_menu") {

		// Roster has multiple entry points so need to rely uponcalling
		// screen to enable itself
		if (selection == (static_cast<int>(items.size()) - 1)) {
			_flags["show_roster"] = false;
			clear_character("inspect");
		} else {
			set_character("inspect", data);
			_flags["show_roster"] = false;
		}
	} else if (component == "reorder_menu") {

		// Reorder has multiple entry points so need to rely upon calling
		// screen to enable itself
		if (selection == (static_cast<int>(items.size()) - 1))
			_flags["show_reorder"] = false;
	} else if (component == "pay_menu") {

		if (selection == (static_cast<int>(items.size()) - 1))
			_flags["show_pay"] = false;
		else
			_selected["pay_selected"] = selection;
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
		if (selection == TRAINING_CREATE)
			move_screen("show_training", "show_method");
		else if (selection == (static_cast<int>(items.size()) - 1))
			move_screen("show_training", "show_edge_of_town");
	} else if (component == "bestiary_menu") {

		// Bestiary
		_selected["bestiary_selected"] = selection;
		if (selection == (static_cast<int>(items.size()) - 1))
			move_screen("show_bestiary", "show_compendium");
	} else if (component == "museum_menu") {

		// Museum
		_selected["museum_selected"] = selection;
		if (selection == (static_cast<int>(items.size()) - 1))
			move_screen("show_museum", "show_compendium");
	} else if (component == "atlas_menu") {

		// Atlas
		_selected["atlas_selected"] = selection;
		if (selection == (static_cast<int>(items.size()) - 1))
			move_screen("show_atlas", "show_compendium");
	} else if (component == "spellbook_menu") {

		// Spellbook
		_selected["spellbook_selected"] = selection;
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
		_selected["stay_selected"] = selection + 1;
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

auto Sorcery::Controller::set_method(const Enums::Character::Method method)
	-> void {

	_method = method;
}

auto Sorcery::Controller::get_method() const -> Enums::Character::Method {

	return _method;
}

auto Sorcery::Controller::inject_character(std::shared_ptr<Character> character)
	-> void {

	_create = character;
}

auto Sorcery::Controller::get_character() const -> Character * {

	return _create.get();
}

namespace Sorcery {

auto operator<<(std::ostream &out_stream, const Sorcery::Controller &controller)
	-> std::ostream & {

	for (const auto &f : controller._flags)
		out_stream << "  Flag: " << f.first << " = " << f.second << std::endl;

	for (const auto &s : controller._selected)
		out_stream << "  Selected: " << s.first << " = " << s.second
				   << std::endl;

	for (const auto &t : controller._characters)
		out_stream << "  Character: " << t.first << " = " << t.second
				   << std::endl;

	return out_stream << std::endl;
}
} // namespace Sorcery