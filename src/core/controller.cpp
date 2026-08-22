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
#include "common/sdl2.hpp"
#include "core/context.hpp"
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
#include "resources/savestore.hpp"
#include "types/character.hpp"
#include "types/config.hpp"
#include "types/game.hpp"
#include "types/item.hpp"
#include "types/meta.hpp"
#include "types/state.hpp"

Sorcery::Controller::Controller(Context &ctx)
	: _ctx{ctx} {

	initialise();
	_game = nullptr;
}

auto Sorcery::Controller::initialise() -> void {

	_busy = false;
	_has_save = _ctx.saves->has_game();

	_abort = false;
	_leave = false;

	go_back = false;

	// Store these flags (if set)
	// auto show_automap{_flags["show_automap"]};
	auto show_party_panel{_flags["interface_party_panel"]};
	auto show_ui{_flags["interface_ui"]};

	// TODO: are these needed?
	_flags.clear();
	_texts.clear();
	_selected.clear();
	_characters.clear();

	// Set default state (these must all be present and set to false/-1)

	clear_character(Enums::CharacterSlot::INSPECT);
	clear_character(Enums::CharacterSlot::RESTART);
	clear_character(Enums::CharacterSlot::STAY);
	clear_character(Enums::CharacterSlot::STORE);
	clear_character(Enums::CharacterSlot::TITHE);
	clear_character(Enums::CharacterSlot::PAY);
	clear_character(Enums::CharacterSlot::HELP);

	// Note that the show_* flags don't need to be preset
	for (const auto flag : {
			 "after_tile_message",
			 "napping_finished",
			 "party_order_changed",
			 "recuperating_finished",
			 "select_previous_character",
			 "select_next_character",

			 "want_camp",
			 "want_elevator_top",
			 "want_elevator_bottom",
			 "want_cannot_donate",
			 "want_continue_game",
			 "want_divvy_gold",
			 "want_donate",
			 "want_donated_ok",
			 "want_drop",
			 "want_enter_maze",
			 "want_exit_game",
			 "want_help",
			 "want_inspect",
			 "want_identify",
			 "want_equip",
			 "want_remove",
			 "want_renamed_ok",
			 "want_invoke",
			 "want_spell",

			 "want_name",
			 "want_name_ok",
			 "want_not_enough_gold",
			 "want_new_game",
			 "want_pool_gold",
			 "want_quit_expedition",
			 "want_stay",
			 "want_take_stairs_down",
			 "want_take_stairs_up",
			 "want_tithe",
			 "want_trade",
			 "want_use",

			 "debug_ui",

			 "in_engine",
		 })
		unset_flag(flag);

	unset_text("heal_results");

	unset_text("heal_results");

	set_selected("bestiary_selected", 0);
	set_selected("spellbook_selected", 0);
	set_selected("museum_selected", 1);
	set_selected("class_selected", 8);
	set_selected("atlas_selected", 8);

	// set ui flags again
	//_flags["show_automap"] = show_automap;
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
		output.append(std::format("{:>26}: {}\n", enum_name(character.first),
								  character.second));

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

auto Sorcery::Controller::check_for_automap(const SDL_Event event) -> bool {

	return (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_m);
}

auto Sorcery::Controller::check_for_ui_toggle(const SDL_Event event) -> void {

	if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_s)
		toggle_flag("interface_party_panel");
	else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_o)
		toggle_flag("interface_ui");
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

// Special Handling for Disable or Enable Menu Items (0-indexed!)
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
				[[fallthrough]];
			case 3: // Temple
				return !_game->state->party_has_members();
				break;
			default:
				return false;
				break;
			};
		}
	} else if (component == "temple_menu") {

		// Check for Party Members
		switch (selection) {
		case 0: // Help
			[[fallthrough]];
		case 1: // Title
			return !_game->state->party_has_members();
			break;
		default:
			return false;
		};

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
			case 3: // Divvy Gold
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
	} else if (component == "give_menu") {

		if (_game != nullptr) {

			const auto current_char_id{
				_characters[Enums::CharacterSlot::INSPECT]};

			if (current_char_id == data)
				return true;

			const auto slots_free{
				_game->characters[data].inventory.get_empty_slots()};
			return slots_free == 0;
		}
	} else if (component == "rest_menu") {

		if (_game != nullptr) {

			// Work out what menu items are disabled due to lack of money
			const auto character{
				_game->characters.at(_characters[Enums::CharacterSlot::STAY])};
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

			// And although we will never reach here really, onlu OK characters
			// can be selected
			return character.get_status() != Enums::Character::Status::OK;
		}
	} else if (component == "temple_pay_menu") {
		if (_game != nullptr) {

			if (data == -1)
				return false;

			const auto &help{
				_game->characters.at(_characters[Enums::CharacterSlot::HELP])};
			const auto &who{_game->characters[data]};
			return help.get_cure_cost() > who.get_gold();
		}
	} else if (component == "identify_menu") {

		if (has_character(Enums::CharacterSlot::INSPECT)) {

			const auto &who{_game->characters.at(
				_characters[Enums::CharacterSlot::INSPECT])};
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
	} else if (component == "shop_identify_menu") {

		if (has_character(Enums::CharacterSlot::STORE)) {
			const auto &who{
				_game->characters.at(_characters[Enums::CharacterSlot::STORE])};
			const auto gold{who.get_gold()};

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
			if (selection < who.inventory.items().size()) {

				// Data is not the item type here - its the slot so we can't use
				// it to work out what is disabled or not!
				const auto item{who.inventory.items().at(selection)};
				const auto item_type{
					_ctx.resources->items->get_item_type(item.get_type_id())};
				if (gold < item_type.get_value())
					return true;
				else if (item.get_known())
					return true;
				else
					return false;
#pragma GCC diagnostic pop
			} else
				return false;
		}

	} else if (component == "shop_uncurse_menu") {

		if (has_character(Enums::CharacterSlot::STORE)) {
			const auto &who{
				_game->characters.at(_characters[Enums::CharacterSlot::STORE])};
			const auto gold{who.get_gold()};

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
			if (selection < who.inventory.items().size()) {

				// Data is not the item type here - its the slot so we can't use
				// it to work out what is disabled or not!
				const auto item{who.inventory.items().at(selection)};
				const auto item_type{
					_ctx.resources->items->get_item_type(item.get_type_id())};
				if (gold < item_type.get_value())
					return true;
				else if (!(item.get_cursed() && item.get_equipped()))
					return true;
				else
					return false;
#pragma GCC diagnostic pop
			} else
				return false;
		}

	} else if (component == "equip_menu") {

		// Remember this is returning true if the item is meant to be disabled!
		if (has_character(Enums::CharacterSlot::INSPECT)) {

			const auto &who{_game->characters.at(
				_characters[Enums::CharacterSlot::INSPECT])};
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
			if (selection < who.inventory.items().size()) {
				const auto item{who.inventory.items().at(selection)};
				return !(!item.get_equipped() && item.get_usable());
			} else
				return false;
#pragma GCC diagnostic pop
		} else
			return false;
	} else if (component == "remove_item_menu") {

		// Remember this is returning true if the item is meant to be disabled!
		if (has_character(Enums::CharacterSlot::INSPECT)) {

			const auto &who{_game->characters.at(
				_characters[Enums::CharacterSlot::INSPECT])};
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
			if (selection < who.inventory.items().size()) {
				const auto item{who.inventory.items().at(selection)};
				return !(item.get_equipped() && !item.get_cursed());
			} else
				return false;
#pragma GCC diagnostic pop
		} else
			return false;
	} else if (component == "sell_menu") {

		if (has_character(Enums::CharacterSlot::STORE)) {

			const auto &who{
				_game->characters.at(_characters[Enums::CharacterSlot::STORE])};
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
			if (selection < who.inventory.items().size()) {
				const auto item{who.inventory.items().at(selection)};
				return item.get_equipped() && item.get_cursed();
			} else
				return false;
#pragma GCC diagnostic pop
		} else
			return false;
	} else if (component == "drop_menu") {

		if (has_character(Enums::CharacterSlot::INSPECT)) {

			const auto &who{_game->characters.at(
				_characters[Enums::CharacterSlot::INSPECT])};
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
	} else if (component == "trade_menu") {

		if (has_character(Enums::CharacterSlot::INSPECT)) {

			const auto &who{_game->characters.at(
				_characters[Enums::CharacterSlot::INSPECT])};
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
	} else if (component == "use_menu") {

		if (has_character(Enums::CharacterSlot::INSPECT)) {

			const auto &who{_game->characters.at(
				_characters[Enums::CharacterSlot::INSPECT])};
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
	} else if (component == "invoke_menu") {

		if (has_character(Enums::CharacterSlot::INSPECT)) {

			const auto &who{_game->characters.at(
				_characters[Enums::CharacterSlot::INSPECT])};
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
	} else if (component == "spell_menu") {

		if (has_character(Enums::CharacterSlot::INSPECT)) {

			// Work out from the Spell ID if we have enough sp to cast it
			const auto &who{_game->characters.at(
				_characters[Enums::CharacterSlot::INSPECT])};
			const auto spell_id{enum_cast<Enums::Magic::SpellID>(data)};

			if (!spell_id)
				return false;

			const auto spell_it{
				std::ranges::find(who.spells(), *spell_id, &Spell::id)};

			if (spell_it == who.spells().end())
				return false;

			const auto &spell{*spell_it};
			const std::map<unsigned int, unsigned int> *spell_points{};

			switch (spell.type) {
			case Enums::Magic::SpellType::ARCANE:
				spell_points = &who.mage_cur_sp();
				break;
			case Enums::Magic::SpellType::DIVINE:
				spell_points = &who.priest_cur_sp();
				break;
			default:
				return false;
			}

			const auto points_it{spell_points->find(spell.level)};

			return !(points_it != spell_points->end() && points_it->second > 0);

		} else
			return false;
	} else if (component == "class_menu") {

		const auto classes{_game->creation_candidate->get_pos_class()};
		if (selection >= 0 && selection < 8) {
			return !classes.at(
				enum_cast<Enums::Character::Class>(selection + 1).value());
		} else
			return false;
	} else if (component == "buy_menu") {

		if (has_character(Enums::CharacterSlot::STORE)) {
			const auto &who{
				_game->characters.at(_characters[Enums::CharacterSlot::STORE])};
			const auto gold{who.get_gold()};

			// Data is the item type
			const auto item_type{_ctx.resources->items->get_item_type(
				enum_cast<Enums::Items::TypeID>(data).value())};
			if (gold < item_type.get_value())
				return true;
		};
	} else if (component == "retrain_menu") {

		if (_game == nullptr || data == -1)
			return false;

		auto &character{_game->characters.at(data)};

		return !character.can_change_class();

	} else if (component == "legate_menu") {

		if (_game == nullptr || data == -1)
			return false;

		auto &character{_game->characters.at(data)};

		return !(character.get_status() == Enums::Character::Status::LOST);

	} else if (component == "store_menu") {

		// No gold, can't buy anything
		// No items, can't sell anything
		// no cursed items, can't uncurse anything
		// no unidentified items, can't identify anything
		if (has_character(Enums::CharacterSlot::STORE)) {
			const auto &who{
				_game->characters.at(_characters[Enums::CharacterSlot::STORE])};
			switch (selection) {
			case 0: // Buy
				return who.get_gold() == 0 || who.inventory.is_full();
				break;
			case 1: // Sell
				return who.inventory.items().empty();
				break;
			case 2: // Uncurse
				return !who.inventory.has_cursed_items();
				break;
			case 3: // Identify
				return !who.inventory.has_unidentified_items();
				break;
			default:
				return false;
			};
		} else
			return false;
	};

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
auto Sorcery::Controller::handle_dynamic_menu(
	std::string_view component,
	[[maybe_unused]] const std::vector<std::string> &items,
	[[maybe_unused]] const int data, const int selection,
	std::vector<std::reference_wrapper<bool>> in_flags) -> bool {

	DEBUG_LOGF("Dynamic Menu: {} {} {}", component, data, selection);

	if (component == "inspect_menu") {

		// Get the Character ID of the Selected Character and set it
		if (selection == (static_cast<int>(items.size()) - 1)) {
			clear_character(Enums::CharacterSlot::INSPECT);
			go_back = true;
		} else
			set_character(Enums::CharacterSlot::INSPECT, data);

		in_flags[0].get() = false;
		return true;
	} else if (component == "change_class_menu") {
		if (selection == (static_cast<int>(items.size()) - 1)) {
			clear_character(Enums::CharacterSlot::EDIT);
			go_to(Enums::Screen::EDIT);
		} else {

			auto &character{_game->characters.at(
				get_character(Enums::CharacterSlot::EDIT))};
			const auto class_to_change_to{
				enum_cast<Enums::Character::Class>(data).value()};
			character.change_class(class_to_change_to);

			_game->save_game();

			unset_flag("want_reclassed_ok");
			_ctx.ui->notice_reclassed_ok->show = true;
		}

		return true;

	} else if (component == "delete_menu") {

		if (selection == static_cast<int>(items.size()) - 1) {

			clear_character(Enums::CharacterSlot::EDIT);

			go_to(Enums::Screen::TRAINING);

		} else {

			set_character(Enums::CharacterSlot::EDIT, data);
		}

		return true;
	} else if (component == "roster_menu") {

		// Get the Character ID of the Selected Character and set it
		if (selection == (static_cast<int>(items.size()) - 1)) {
			clear_character(Enums::CharacterSlot::INSPECT);
			go_to(Enums::Screen::TRAINING);
		} else
			set_character(Enums::CharacterSlot::INSPECT, data);
		return true;
	} else if (component == "select_menu") {

		// Get the Character ID of the Selected Character and set it
		if (selection == (static_cast<int>(items.size()) - 1)) {
			clear_character(Enums::CharacterSlot::EDIT);
			go_to(Enums::Screen::EDIT);
		} else
			set_character(Enums::CharacterSlot::EDIT, data);
		return true;
	} else if (component == "retrain_menu") {

		// Get the Character ID of the Selected Character and set it
		if (selection == (static_cast<int>(items.size()) - 1)) {
			clear_character(Enums::CharacterSlot::EDIT);
			go_to(Enums::Screen::EDIT);
		} else
			set_character(Enums::CharacterSlot::EDIT, data);
		return true;
	} else if (component == "legate_menu") {

		// Get the Character ID of the Selected Character and set it
		if (selection == (static_cast<int>(items.size()) - 1)) {
			clear_character(Enums::CharacterSlot::EDIT);
			go_to(Enums::Screen::EDIT);
		} else
			set_character(Enums::CharacterSlot::EDIT, data);
		return true;
	} else if (component == "tithe_menu") {

		// Flags = &_ui->modal_tithe->show, &_ui->input_donate->show,

		// Get the Character ID of the Selected Character and set it
		if (selection == (static_cast<int>(items.size()) - 1)) {
			clear_character(Enums::CharacterSlot::TITHE);
			go_back = true;
		} else {
			set_character(Enums::CharacterSlot::TITHE, data);
			_flags["want_donate"] = true;
			in_flags.at(1).get() = true;
		}

		// Remove the Modal
		in_flags.at(0).get() = false;
		return true;
	} else if (component == "help_menu") {

		// Flags = &_ui->modal_help->show

		// Get the Character ID of the Selected Character and set it
		if (selection == (static_cast<int>(items.size()) - 1)) {
			clear_character(Enums::CharacterSlot::HELP);
			go_back = true;
		} else
			set_character(Enums::CharacterSlot::HELP, data);

		// Remove the Modal
		in_flags.at(0).get() = false;
		return true;
	} else if (component == "temple_heal_menu") {

		if (selection == static_cast<int>(items.size()) - 1) {

			clear_character(Enums::CharacterSlot::HELP);

			go_to(Enums::Screen::CASTLE);

		} else {

			set_character(Enums::CharacterSlot::HELP, data);
		}

		return true;
	} else if (component == "temple_pay_menu") {

		if (selection == static_cast<int>(items.size()) - 1) {

			clear_character(Enums::CharacterSlot::PAY);
			go_to(Enums::Screen::TEMPLE);
		} else {
			set_character(Enums::CharacterSlot::PAY, data);
		}

		return true;
	} else if (component == "identify_menu") {

		// Flags = &_ui->modal_identify->show
		if (selection == (static_cast<int>(items.size()) - 1)) {
			_flags["want_identify"] = true;
			in_flags.at(0).get() = false;
		} else {
			// TODO
		}
		return true;
	} else if (component == "equip_menu") {

		// Flags = &_ui->modal_equip->show
		if (selection == (static_cast<int>(items.size()) - 1)) {
			_flags["want_equip"] = true;
			in_flags.at(0).get() = false;
		} else {
			// TODO
		}
		return true;
	} else if (component == "remove_menu") {

		// Flags = &_ui->modal_remove->show
		if (selection == (static_cast<int>(items.size()) - 1)) {
			_flags["want_remove"] = true;
			in_flags.at(0).get() = false;
		} else {
			// TODO
		}
		return true;
	} else if (component == "spell_menu") {

		// Flags = &_ui->modalspell->show
		if (selection == (static_cast<int>(items.size()) - 1)) {
			_flags["want_spell"] = true;
			in_flags.at(0).get() = false;
		} else {
			// TODO
		}
		return true;
	} else if (component == "drop_menu") {

		// Flags = &_ui->modal_drop->show
		if (selection == (static_cast<int>(items.size()) - 1)) {
			_flags["want_drop"] = true;
			in_flags.at(0).get() = false;
		} else {
		}
		return true;
	} else if (component == "trade_menu") {

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
		return true;
	} else if (component == "use_menu") {

		// Flags = &_ui->modal_use->show
		if (selection == (static_cast<int>(items.size()) - 1)) {
			_flags["want_use"] = true;
			in_flags.at(0).get() = false;
		} else {
		}
		return true;
	} else if (component == "invoke_menu") {

		// Flags = &_ui->modal_invoke->show
		if (selection == (static_cast<int>(items.size()) - 1)) {
			_flags["want_invoke"] = true;
			in_flags.at(0).get() = false;
		} else {
		}
		return true;
	}

	return false;
}

auto Sorcery::Controller::handle_icon_click(const int icon_idx) -> void {

	DEBUG_LOGF("Icon Click: {}", icon_idx);

	switch (icon_idx) {
	case ICON_CAMP:
		_ctx.ui->modal_camp->show = true;
		break;

	case ICON_PARTY:
		set_flag("want_inspect");
		break;

	case ICON_MAP:
		set_flag("want_automap");
		break;

	case ICON_LOOK:
		set_flag("want_look");
		break;

	case ICON_CAST:
		set_flag("want_spell");
		break;

	case ICON_USE:
		set_flag("want_use");
		break;

	default:
		break;
	}
}

auto Sorcery::Controller::inspect_party_member(const int character_id) -> void {

	set_character(Enums::CharacterSlot::INSPECT, character_id);
	go_to(Enums::Screen::INSPECT);
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

auto Sorcery::Controller::get_character(const Enums::CharacterSlot flag) const
	-> int {

	if (_characters.contains(flag))
		return _characters.at(flag);
	else
		return -1;
}

auto Sorcery::Controller::has_character(const Enums::CharacterSlot flag) const
	-> bool {

	if (_characters.contains(flag))
		if (_characters.at(flag) != -1)
			return true;

	return false;
}

auto Sorcery::Controller::set_character(const Enums::CharacterSlot flag,
										const int value) -> void {

	_characters[flag] = value;
}

auto Sorcery::Controller::clear_character(const Enums::CharacterSlot flag)
	-> void {

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

	if (event.type != SDL_WINDOWEVENT)
		return;

	if (event.window.event != SDL_WINDOWEVENT_RESIZED &&
		event.window.event != SDL_WINDOWEVENT_SIZE_CHANGED)
		return;

	_ctx.display->update_display_metrics();
	_ctx.ui->update_grid_metrics(_ctx.display->get_display_metrics());
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
			 "want_equip",
			 "want_remove",
			 "want_spell",
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

	const int party_count{_game->state->get_party_size()};

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
	} else if (component == "rename_input_ok") {

		if (!data->empty()) {

			auto &character{_game->characters.at(
				get_character(Enums::CharacterSlot::EDIT))};

			character.set_name(*data);

			_game->save_game();

			unset_flag("want_renamed_ok");
			ui->notice_renamed_ok->show = true;

		} else {

			clear_character(Enums::CharacterSlot::EDIT);

			go_to(Enums::Screen::EDIT);
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
		_game->pool_party_gold(get_character(Enums::CharacterSlot::INSPECT));
	} else if (component == "button_leave") {
		// Leave Inspect
		unset_flag("want_inspect");
		go_back = true;
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
	} else if (component == "button_equip") {
		// Show Equip Modal
		ui->modal_equip->regenerate();
		ui->modal_equip->show = true;
		set_flag("want_equip");
	} else if (component == "button_remove") {
		// Show Remove Modal
		ui->modal_remove->regenerate();
		ui->modal_remove->show = true;
		set_flag("want_remove");
	} else if (component == "button_spell") {
		// Show Spell Modal
		ui->modal_spell->regenerate();
		ui->modal_spell->show = true;
		set_flag("want_spell");
	} else if (component == "button_invoke") {
		// Show Invoke Modal
		ui->modal_invoke->regenerate();
		ui->modal_invoke->show = true;
		set_flag("want_invoke");
	} else if (component == "button_keep_yes") {

		// Save Character
		_ctx.controller->set_flag("confirm_keep_character");

	} else if (component == "button_keep_no") {

		// Don't save Character
		_ctx.controller->set_flag("confirm_discard_character");

	} else if (component == "button_buy_leave") {

		// Return to the Store
		go_to(Enums::Screen::STORE);

	} else if (component == "button_heal_return") {

		go_to(Enums::Screen::TEMPLE);

	} else if (component == "license_return") {

		// Return to Main Menu
		go_to(Enums::Screen::MAINMENU);
	} else if (component == "graveyard_return") {

		// Return to Castle on a wipe
		go_to(Enums::Screen::CASTLE);
	} else if (component == "nolevelup_leave") {

		// No Level Up Notice - Return to Inn
		go_to(Enums::Screen::INN);
	} else if (component == "levelup_leave") {

		// No Level Up Notice - C Return to Inn
		go_to(Enums::Screen::INN);
	} else if (component == "automap_return") {
		go_to(Enums::Screen::ENGINE);
	}
}

// Menu Handling
auto Sorcery::Controller::handle_standard_menu(
	std::string_view component, const std::vector<std::string> &items,
	const int data, const int selection) -> void {

	DEBUG_LOGF("Standard Menu: {} {} {}", component, data, selection);

	if (component == "remove_character_menu") {

		if (selection == (static_cast<int>(items.size()) - 1))
			go_to(Enums::Screen::TAVERN);
		else {

			// if we can, remove the character from the party
			auto &character{_game->characters[data]};
			character.set_location(Enums::Character::Location::TAVERN);
			_game->state->remove_character_by_id(data);
			_game->save_game();
		}

	} else if (component == "inn_menu") {

		// Get the Character ID of the Selected Character and set it
		if (selection == (static_cast<int>(items.size()) - 1)) {
			clear_character(Enums::CharacterSlot::STAY);
			go_to(Enums::Screen::CASTLE);
		} else
			set_character(Enums::CharacterSlot::STAY, data);

	} else if (component == "shop_menu") {

		// Get the Character ID of the Selected Character and set it
		if (selection == (static_cast<int>(items.size()) - 1)) {
			clear_character(Enums::CharacterSlot::STORE);
			go_to(Enums::Screen::CASTLE);
		} else
			set_character(Enums::CharacterSlot::STORE, data);

	} else if (component == "restart_menu") {

		// Restart Menu
		if (selection == (static_cast<int>(items.size()) - 1))
			go_to(Enums::Screen::EDGEOFTOWN);
		else {

			// Get the ID of the Character if we can, add the character to
			// the party
			set_character(Enums::CharacterSlot::RESTART, data);
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
				enum_cast<Enums::Character::Race>(selection + 1).value());
			_game->creation_candidate->set_stage(
				Enums::Character::Stage::CHOOSE_ALIGNMENT);
			_game->creation_candidate->set_start_attr();
		}
	} else if (component == "alignment_menu") {

		if (selection == (static_cast<int>(items.size()) - 1))
			go_to(Enums::Screen::TRAINING);
		else {
			_game->creation_candidate->set_alignment(
				enum_cast<Enums::Character::Align>(selection + 1).value());
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
					enum_cast<Enums::Character::Class>(selection + 1).value());
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
	} else if (component == "reorder_menu") {

		// Reorder has multiple entry points so need to rely upon calling
		// screen to enable itself
		if (selection == (static_cast<int>(items.size()) - 1)) {
			_flags["show_reorder"] = false;
			go_back = true;
		}
	} else if (component == "pay_menu") {

		if (selection == (static_cast<int>(items.size()) - 1))
			_flags["show_pay"] = false;
		else
			_selected["pay_selected"] = selection;
	} else if (component == "shop_menu") {

		// Shop
		if (selection == (static_cast<int>(items.size()) - 1))
			go_to(Enums::Screen::CASTLE);
		else {
		}
	} else if (component == "temple_menu") {

		// Temple
		if (selection == (static_cast<int>(items.size()) - 1))
			go_to(Enums::Screen::CASTLE);
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
			clear_character(Enums::CharacterSlot::CHOOSE);
		} else
			set_character(Enums::CharacterSlot::CHOOSE, data);
	} else if (component == "shop_menu") {

		// Boltacs
		_selected["store_selected"] = selection;
		if (selection == (static_cast<int>(items.size()) - 1))
			go_to(Enums::Screen::SHOP);
		else
			go_to(Enums::Screen::STORE);
	} else if (component == "store_menu") {

		// Store
		_selected["store_selected"] = selection;
		if (selection == (static_cast<int>(items.size()) - 1))
			go_to(Enums::Screen::SHOP);
	}
}

auto Sorcery::Controller::get_candidate_character() const -> Character * {

	return _game->creation_candidate.get();
}

auto Sorcery::Controller::go_to(const Enums::Screen screen) -> void {

	DEBUG_LOGF("Go To Screen: {}", enum_name(screen));

	_last_screen = _screen;
	_screen = screen;
}

auto Sorcery::Controller::wants(const Enums::Screen value) const -> bool {

	return _screen == value;
}

auto Sorcery::Controller::is_at() const -> Enums::Screen {

	return _screen;
}

auto Sorcery::Controller::handle_action_table_menu(
	std::string_view menu, int selection, int data,
	std::vector<std::reference_wrapper<bool>> &ui_flags) -> bool {

	DEBUG_LOGF("Action Table Menu: {} {} {}", menu, selection, data);

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
	case MenuAction::Type::SETFLAG:
		_flags[action.flag] = true;
		break;

	case MenuAction::Type::CLEARFLAG:
		_flags[action.flag] = false;
		break;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtype-limits"

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

#pragma GCC diagnostic pop

	case MenuAction::Type::SET_CHARACTER:
		set_character(action.character_key, data);
		break;

	case MenuAction::Type::CLEAR_CHARACTER:
		clear_character(action.character_key);
		break;
	case MenuAction::Type::GO_BACK:
		go_back = true;
		break;
	case MenuAction::Type::CUSTOM:
		// Handle custom actions here if needed
		if (action.custom_function == "handle_pool_gold")
			_game->pool_party_gold(get_character(Enums::CharacterSlot::STORE));
		break;
	case MenuAction::Type::SET_SELECTED:
		set_selected(action.selected_key, action.selected_value);
		break;
	case MenuAction::Type::GOTOSCREEN:
		go_to(action.screen);
		break;
	default:
		break;
	}
}

auto Sorcery::Controller::consume_menu_key(const std::size_t item_count)
	-> std::optional<std::size_t> {

	if (!_menu_key || item_count == 0)
		return std::nullopt;

	const auto key{std::exchange(_menu_key, std::nullopt)};

	if (*key == 0)
		return item_count - 1;

	const auto index{static_cast<std::size_t>(*key - 1)};

	if (index >= item_count)
		return std::nullopt;

	return index;
}

auto Sorcery::Controller::check_for_menu_key(const SDL_Event &event) -> void {

	if (event.type != SDL_KEYDOWN)
		return;

	switch (event.key.keysym.sym) {

	case SDLK_1:
	case SDLK_KP_1:
		_menu_key = 1;
		break;

	case SDLK_2:
	case SDLK_KP_2:
		_menu_key = 2;
		break;

	case SDLK_3:
	case SDLK_KP_3:
		_menu_key = 3;
		break;

	case SDLK_4:
	case SDLK_KP_4:
		_menu_key = 4;
		break;

	case SDLK_5:
	case SDLK_KP_5:
		_menu_key = 5;
		break;

	case SDLK_6:
	case SDLK_KP_6:
		_menu_key = 6;
		break;

	case SDLK_7:
	case SDLK_KP_7:
		_menu_key = 7;
		break;

	case SDLK_8:
	case SDLK_KP_8:
		_menu_key = 8;
		break;

	case SDLK_9:
	case SDLK_KP_9:
		_menu_key = 9;
		break;

	case SDLK_0:
	case SDLK_KP_0:
		_menu_key = 0;
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

	for (const auto &[slot, character_id] : controller._characters) {
		out_stream << std::format("  Character: {:>26} = {}\n", enum_name(slot),
								  character_id);
	}

	return out_stream << std::endl;
}

} // namespace Sorcery