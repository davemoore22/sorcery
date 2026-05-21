// Copyright (C) 2026 Dave Moore
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
#include "common/cereal.hpp"
#include "common/enum.hpp"
#include "common/opengl.hpp"
#include "common/sdl2.hpp"
#include "core/context.hpp"
#include "core/database.hpp"
#include "core/debug.hpp"
#include "core/define.hpp"
#include "core/display.hpp"
#include "core/enum.hpp"
#include "core/resources.hpp"
#include "core/system.hpp"
#include "core/ui.hpp"
#include "engine/define.hpp"
#include "gui/define.hpp"
#include "gui/dialog.hpp"
#include "gui/menuaction.hpp"
#include "gui/modal.hpp"
#include "resources/itemstore.hpp"
#include "types/character.hpp"
#include "types/config.hpp"
#include "types/game.hpp"
#include "types/item.hpp"
#include "types/state.hpp"

Sorcery::Controller::Controller(Context &ctx)
	: _ctx{ctx} {

	initialise();
	_game = nullptr;
}

auto Sorcery::Controller::initialise() -> void {

	_busy = false;
	_has_save = _ctx.database->has_game();

	_abort = false;
	_leave = false;

	// Store these flags (if set)
	auto show_automap{_flags["interface_automap"]};
	auto show_party_panel{_flags["interface_party_panel"]};
	auto show_ui{_flags["interface_ui"]};

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

	unset_flag("in_engine");

	unset_text("heal_results");

	set_selected("bestiary_selected", 0);
	set_selected("spellbook_selected", 0);
	set_selected("museum_selected", 1);
	set_selected("class_selected", 8);
	set_selected("atlas_selected", 8);

	// set ui flags again
	_flags["interface_automap"] = show_automap;
	_flags["interface_party_panel"] = show_party_panel;
	_flags["interface_ui"] = show_ui;
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
		for (auto const &[key, val] : _flags)
			DEBUG_LOGF("{}", std::format("{:>32}: {}", key, val));
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
	else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_p)
		toggle_flag("debug_ui");
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
	} else if (component == "give_menu" || component == "modal_give") {

		if (_game != nullptr) {

			const auto current_char_id{_characters["inspect"]};

			if (current_char_id == data)
				return true;

			const auto slots_free{
				_game->characters[data].inventory.get_empty_slots()};
			return slots_free == 0;
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

		const auto classes{_game->creation_candidate->get_pos_class()};
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

	DEBUG_LOGF("Toggle: {} {} {}", component, tab, selection);

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

auto Sorcery::Controller::get_last_screen() const -> Enums::Screen {

	return _last_screen;
}
auto Sorcery::Controller::set_last_screen(const Enums::Screen value) -> void {

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

	DEBUG_LOGF("Menu with Flags: {} {} {}", component, data, selection);

	if (component == "stay_menu" || component == "modal_stay") {

		// Get the Character ID of the Selected Character and set it
		if (selection == (static_cast<int>(items.size()) - 1))
			clear_character("stay");
		else
			set_character("stay", data);

		// Remove the Modal
		in_flags.at(0).get() = false;
	} else if (component == "inspect_menu") {

		// Get the Character ID of the Selected Character and set it
		if (selection == (static_cast<int>(items.size()) - 1)) {
			clear_character("inspect");
			set_flag("back_from_inspect");
		} else
			set_character("inspect", data);

		in_flags[0].get() = false;
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

		// Flags = &_ui->modal_trade->show, &_ui->modal_give->show
		if (selection == (static_cast<int>(items.size()) - 1)) {
			_flags["want_trade"] = true;
			_flags["want_give"] = true;
			in_flags.at(0).get() = false;
		} else {
			in_flags.at(0).get() = false;
			in_flags.at(1).get() = true;
			_flags["want_give"] = true;
			_flags["want_trade"] = false;

			// Handle Trade
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

auto Sorcery::Controller::clear_modal_flags() -> void {

	for (const auto flag : {
			 "want_camp",
			 "want_inspect",
			 "want_stay",
			 "want_help",
			 "want_tithe",
			 "want_identify",
			 "want_drop",
			 "want_trade",
			 "want_give",
			 "want_use",
			 "want_invoke",
			 "want_take_stairs_up",
			 "want_take_stairs_down",
			 "after_tile_message",
		 })
		unset_flag(flag);
}

auto Sorcery::Controller::check_for_quick_inspect(const SDL_Event event)
	-> int {

	if (event.type != SDL_KEYDOWN)
		return -1;

	const auto scancode{event.key.keysym.scancode};

	int position{-1};

	// Main keyboard number row
	if (scancode >= SDL_SCANCODE_1 && scancode <= SDL_SCANCODE_6)
		position = static_cast<int>(scancode - SDL_SCANCODE_1) + 1;

	// Numeric keypad
	else if (scancode >= SDL_SCANCODE_KP_1 && scancode <= SDL_SCANCODE_KP_6)
		position = static_cast<int>(scancode - SDL_SCANCODE_KP_1) + 1;

	if (position == -1)
		return -1;

	const auto party_count{_game->state->get_party_size()};

	if (position > party_count)
		return -1;

	return position;
}

// Check if the SDL event is a Window-Shut-Down event
auto Sorcery::Controller::check_for_abort(const SDL_Event event) -> bool {

	// SDL_QUIT event
	if (event.type == SDL_QUIT) {
		_abort = true;
		return true;
	}

	// Window Close event
	if (event.type == SDL_WINDOWEVENT &&
		event.window.event == SDL_WINDOWEVENT_CLOSE &&
		event.window.windowID ==
			SDL_GetWindowID(_ctx.display->get_SDL_window())) {
		_abort = true;
		return true;
	}

	return false;
}

auto Sorcery::Controller::abort(const bool value) -> void {

	_abort = value;
}

auto Sorcery::Controller::want_to_abort() const -> bool {

	return _abort || has_flag("want_abort_game");
}

auto Sorcery::Controller::leave_game(const bool value) -> void {

	_leave = value;
}

auto Sorcery::Controller::want_to_leave_game() -> bool & {

	return _leave;
}

auto Sorcery::Controller::get_input_buffer() -> std::string & {

	return _input_buffer;
}
auto Sorcery::Controller::set_input_buffer(const std::string &value) -> void {

	_input_buffer = value;
}

auto Sorcery::Controller::clear_input_buffer() -> void {

	_input_buffer.clear();
}

auto Sorcery::Controller::handle_stepper_button_click(
	const std::string &component, [[maybe_unused]] UI *ui, const bool positive,
	int *data) -> void {

	DEBUG_LOGF("Stepper Button Click: {} {}", component, positive);

	auto candidate{_ctx.game->creation_candidate};

	if (component.starts_with("##stepper_attribute_")) {

		if (positive) {

			// Up: If we have points left and the value is less than 18
			if ((candidate->get_points_left() > 0) && (*data <= 17)) {

				(*data)++;
				candidate->set_points_left(candidate->get_points_left() - 1);
				candidate->set_pos_class();
			}

		} else {

			if (candidate->get_points_left() < candidate->get_start_points()) {

				// Down: If we are above staring points
				using enum Enums::Character::Attribute;
				if (component.starts_with("##stepper_attribute_1")) {
					if (candidate->get_cur_attr(STRENGTH) >
						candidate->get_start_attr(STRENGTH)) {
						(*data)--;
						candidate->set_points_left(
							candidate->get_points_left() + 1);
						candidate->set_pos_class();
					}
				} else if (component.starts_with("##stepper_attribute_2")) {
					if (candidate->get_cur_attr(IQ) >
						candidate->get_start_attr(IQ)) {
						(*data)--;
						candidate->set_points_left(
							candidate->get_points_left() + 1);
						candidate->set_pos_class();
					}
				} else if (component.starts_with("##stepper_attribute_3")) {
					if (candidate->get_cur_attr(PIETY) >
						candidate->get_start_attr(PIETY)) {
						(*data)--;
						candidate->set_points_left(
							candidate->get_points_left() + 1);
						candidate->set_pos_class();
					}
				} else if (component.starts_with("##stepper_attribute_4")) {
					if (candidate->get_cur_attr(VITALITY) >
						candidate->get_start_attr(VITALITY)) {
						(*data)--;
						candidate->set_points_left(
							candidate->get_points_left() + 1);
						candidate->set_pos_class();
					}
				} else if (component.starts_with("##stepper_attribute_5")) {
					if (candidate->get_cur_attr(AGILITY) >
						candidate->get_start_attr(AGILITY)) {
						(*data)--;
						candidate->set_points_left(
							candidate->get_points_left() + 1);
						candidate->set_pos_class();
					}
				} else if (component.starts_with("##stepper_attribute_6")) {
					if (candidate->get_cur_attr(LUCK) >
						candidate->get_start_attr(LUCK)) {
						(*data)--;
						candidate->set_points_left(
							candidate->get_points_left() + 1);
						candidate->set_pos_class();
					}
				}
			}
		}
	};
};

auto Sorcery::Controller::handle_input_button_click(
	const std::string &component, [[maybe_unused]] UI *ui, std::string *data)
	-> void {

	DEBUG_LOGF("Input Button Click: {} {}", component, *data);

	if (component == "name_input_ok") {

		if (data->length() > 0) {

			_game->creation_candidate->set_name(*data);
			_game->creation_candidate->set_stage(
				Enums::Character::Stage::CHOOSE_RACE);
		}
	}
}

auto Sorcery::Controller::handle_button_click(const std::string &component,
											  UI *ui,
											  [[maybe_unused]] const int data)
	-> void {

	DEBUG_LOGF("Button Click: {} {}", component, data);

	if (component == "button_identify") {
		// Show Identify Modal
		ui->modal_identify->regenerate();
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
		ui->modal_drop->regenerate();
		ui->modal_drop->show = true;
		set_flag("want_drop");
	} else if (component == "button_trade") {
		// Show Trade Modal
		ui->modal_trade->regenerate();
		ui->modal_trade->show = true;
		ui->modal_give->regenerate();
		ui->modal_give->show = false;
		set_flag("want_trade");
		unset_flag("want_give");
	} else if (component == "button_use") {
		// Show Use Modal
		ui->modal_use->regenerate();
		ui->modal_use->show = true;
		set_flag("want_use");
	} else if (component == "button_invoke") {
		// Show Use Modal
		ui->modal_invoke->regenerate();
		ui->modal_invoke->show = true;
		set_flag("want_invoke");
	} else if (component == "button_keep_yes") {

		// Save Character
		_ctx.controller->set_flag("confirm_keep_character");

	} else if (component == "button_keep_no") {

		// Don't save Character
		_ctx.controller->set_flag("confirm_discard_character");

	} else if (component == "license_return") {

		// Return to Main Menu
		go_to(Enums::Screen::MAINMENU);
	}
}

// Menu Handling
auto Sorcery::Controller::handle_menu(const std::string &component,
									  const std::vector<std::string> &items,
									  const int data, const int selection)
	-> void {

	DEBUG_LOGF("Menu: {} {} {}", component, data, selection);

	if (component == "remove_menu") {

		if (selection == (static_cast<int>(items.size()) - 1))
			go_to(Enums::Screen::TAVERN);
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
			go_to(Enums::Screen::EDGEOFTOWN);
		else {

			// Get the ID of the Character if we can, add the character to
			// the party
			set_character("restart", data);
			_flags["want_restart_expedition"] = true;
		}

	} else if (component == "add_menu") {

		if (selection == (static_cast<int>(items.size()) - 1))
			go_to(Enums::Screen::TAVERN);
		else {

			// if we can, add the character to the party
			auto &character{_game->characters[data]};
			character.set_location(Enums::Character::Location::PARTY);
			_game->state->add_character_to_party(data);
			_game->save_game();
		}

	} else if (component == "race_menu") {

		if (selection == (static_cast<int>(items.size()) - 1))
			go_to(Enums::Screen::TRAINING);
		else {
			_game->creation_candidate->set_race(
				magic_enum::enum_cast<Enums::Character::Race>(selection + 1)
					.value());
			_game->creation_candidate->set_stage(
				Enums::Character::Stage::CHOOSE_ALIGNMENT);
			_game->creation_candidate->set_start_attr();
		}

	} else if (component == "alignment_menu") {

		if (selection == (static_cast<int>(items.size()) - 1))
			go_to(Enums::Screen::TRAINING);
		else {
			_game->creation_candidate->set_alignment(
				magic_enum::enum_cast<Enums::Character::Align>(selection + 1)
					.value());
			_game->creation_candidate->set_stage(
				Enums::Character::Stage::CHOOSE_CLASS);
			_game->creation_candidate->set_start_attr();
			_game->creation_candidate->set_pos_class();
		}
	} else if (component == "class_menu") {

		if (selection == (static_cast<int>(items.size()) - 1))
			go_to(Enums::Screen::TRAINING);
		else {
			auto candidate{_ctx.game->creation_candidate};
			if (candidate->get_points_left() == 0) {

				candidate->set_class(
					magic_enum::enum_cast<Enums::Character::Class>(selection +
																   1)
						.value());
				candidate->set_stage(
					Enums::Character::Stage::REVIEW_AND_CONFIRM);
				candidate->finalise();

				// TODO: refactor this
				candidate->inventory.clear();

				switch (
					candidate->get_class()) { // NOLINT(clang-diagnostic-switch)
					using enum Enums::Character::Class;
					using enum Enums::Items::TypeID;
				case FIGHTER:
				case LORD:
				case SAMURAI:
					candidate->inventory.add_type(
						_ctx.resources->items->get(LEATHER_ARMOR), true);
					candidate->inventory.add_type(
						_ctx.resources->items->get(LONG_SWORD), true);
					break;
				case MAGE:
					candidate->inventory.add_type(
						_ctx.resources->items->get(ROBES), true);
					candidate->inventory.add_type(
						_ctx.resources->items->get(DAGGER), true);
					break;
				case PRIEST:
				case BISHOP:
					candidate->inventory.add_type(
						_ctx.resources->items->get(ROBES), true);
					candidate->inventory.add_type(
						_ctx.resources->items->get(STAFF), true);
					break;
				case THIEF:
				case NINJA:
					candidate->inventory.add_type(
						_ctx.resources->items->get(LEATHER_ARMOR), true);
					candidate->inventory.add_type(
						_ctx.resources->items->get(SHORT_SWORD), true);
				default:
					break;
				}

				set_flag("want_choose_confirm");
				unset_flag("want_choose_class");
			}
		};
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
			go_to(Enums::Screen::ROSTER);
		else if (selection == (static_cast<int>(items.size()) - 1))
			go_to(Enums::Screen::CASTLE);
	} else if (component == "temple_menu") {

		// Temple
		if (selection == TEMPLE_INSPECT)
			go_to(Enums::Screen::ROSTER);
		else if (selection == (static_cast<int>(items.size()) - 1))
			go_to(Enums::Screen::CASTLE);
	} else if (component == "training_menu") {

		// Training Grounds
		if (selection == TRAINING_CREATE)
			go_to(Enums::Screen::CREATE);
		else if (selection == (static_cast<int>(items.size()) - 1))
			go_to(Enums::Screen::EDGEOFTOWN);
	} else if (component == "bestiary_menu") {

		// Bestiary
		_selected["bestiary_selected"] = selection;
		if (selection == (static_cast<int>(items.size()) - 1))
			go_to(Enums::Screen::COMPENDIUM);
	} else if (component == "museum_menu") {

		// Museum
		_selected["museum_selected"] = selection;
		if (selection == (static_cast<int>(items.size()) - 1))
			go_to(Enums::Screen::COMPENDIUM);
	} else if (component == "atlas_menu") {

		// Atlas
		_selected["atlas_selected"] = selection;
		if (selection == (static_cast<int>(items.size()) - 1))
			go_to(Enums::Screen::COMPENDIUM);
	} else if (component == "spellbook_menu") {

		// Spellbook
		_selected["spellbook_selected"] = selection;
		if (selection == (static_cast<int>(items.size()) - 1))
			go_to(Enums::Screen::COMPENDIUM);
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
			go_to(Enums::Screen::INN);
		else
			go_to(Enums::Screen::RECOVERY);
	}
}

auto Sorcery::Controller::get_character() const -> Character * {

	return _game->creation_candidate.get();
}

auto Sorcery::Controller::go_to(const Enums::Screen screen) -> void {

	DEBUG_LOGF("Go To Screen: {}", magic_enum::enum_name(screen));

	_last_screen = _screen;
	_screen = screen;
}

auto Sorcery::Controller::wants(const Enums::Screen value) const -> bool {

	return _screen == value;
}

auto Sorcery::Controller::is_at() const -> Enums::Screen {

	return _screen;
}

auto Sorcery::Controller::handle_menu(
	std::string_view menu, int selection, int data,
	std::vector<std::reference_wrapper<bool>> &ui_flags) -> bool {

	DEBUG_LOGF("Handle Menu: {} {} {}", menu, selection, data);

	const auto it{MENU_ACTIONS.find(menu)};
	if (it == MENU_ACTIONS.end())
		return false;

	if (selection < 0 || selection >= static_cast<int>(it->second.size()))
		return false;

	const auto &actions = it->second[selection];
	for (const auto &action : actions)
		execute_action(action, data, ui_flags);

	return true;
}

auto Sorcery::Controller::execute_action(
	const MenuAction &action, int data,
	std::vector<std::reference_wrapper<bool>> &ui_flags) -> void {
	switch (action.type) {
	case MenuAction::Type::GOTOSCREEN:
		go_to(action.screen);
		break;
	case MenuAction::Type::SETFLAG:
		_flags[action.flag] = true;
		break;

	case MenuAction::Type::CLEARFLAG:
		_flags[action.flag] = false;
		break;

	case MenuAction::Type::SET_UI_BOOL:
		if (action.ui_index >= 0 &&
			static_cast<size_t>(action.ui_index) < ui_flags.size())
			ui_flags[action.ui_index].get() = true;
		break;

	case MenuAction::Type::CLEAR_UI_BOOL:
		if (action.ui_index >= 0 &&
			static_cast<size_t>(action.ui_index) < ui_flags.size())
			ui_flags[action.ui_index].get() = false;
		break;

	case MenuAction::Type::SETCHARACTER:
		set_character(action.character_key, data);
		break;

	case MenuAction::Type::CLEARCHARACTER:
		clear_character(action.character_key);
		break;

	default:
		break;
	}
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