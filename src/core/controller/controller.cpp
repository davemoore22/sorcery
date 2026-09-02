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

#include "core/controller/controller.hpp"
#include "common/enum.hpp"	// for Attribute, Options, Class
#include "common/types.hpp" // for Spell
#include "core/context.hpp" // for Context
#include "core/controller/actionhandler.hpp"
#include "core/controller/inputhandler.hpp"
#include "core/controller/menuaction.hpp" // for MenuAction, MENU_ACTIONS
#include "core/controller/menuhandler.hpp"
#include "core/debug.hpp"				 // for DEBUG_LOGF
#include "core/define.hpp"				 // for ICON_CAMP, ICON_CAST, ICON_...
#include "core/enum.hpp"				 // for CharacterSlot, Screen
#include "core/resources.hpp"			 // for Resources
#include "display/display.hpp"			 // for Display
#include "display/ui/popupstore.hpp"	 // for PopupStore
#include "display/ui/ui.hpp"			 // for UI
#include "display/ui/uimetrics.hpp"		 // for UIMetrics
#include "drawables/define.hpp"			 // for MAIN_MENU_CONTINUE_GAME
#include "drawables/dialog.hpp"			 // for Dialog
#include "drawables/modal.hpp"			 // for Modal
#include "engine/define.hpp"			 // for MOVE_BACKWARD, MOVE_FORWARD
#include "resources/itemstore.hpp"		 // for ItemStore
#include "resources/savestore.hpp"		 // for SaveStore
#include "types/character/character.hpp" // for Character
#include "types/character/create.hpp"	 // for CharacterCreate
#include "types/character/inventory.hpp" // for Inventory
#include "types/character/magic.hpp"	 // for ConstCharacterMagic
#include "types/config.hpp"				 // for Config
#include "types/enum.hpp"				 // for TypeID, TypeID::LEATHER_ARMOR
#include "types/game.hpp"				 // for Game
#include "types/item/item.hpp"			 // for Item
#include "types/item/itemtype.hpp"		 // for ItemType
#include "types/meta.hpp"				 // for enum_cast, enum_name
#include "types/state.hpp"				 // for State
#include <SDL_events.h>					 // for SDL_EventType, SDL_Event
#include <SDL_keycode.h>				 // for SDL_KeyCode
#include <SDL_mouse.h>					 // for SDL_BUTTON_RIGHT
#include <SDL_scancode.h>				 // for SDL_Scancode
#include <SDL_video.h>					 // for SDL_WindowEventID, SDL_GetW...
#include <algorithm>					 // for find
#include <format>						 // for format
#include <initializer_list>				 // for initializer_list
#include <memory>						 // for shared_ptr, unique_ptr
#include <ranges>						 // for __find_fn
#include <unordered_map>				 // for unordered_map, operator==
#include <utility>						 // for pair, get, exchange

Sorcery::Controller::Controller(Context &ctx)
	: _ctx{ctx} {

	initialise();
	_game = nullptr;

	menus = std::make_unique<ControllerMenuHandler>(*this, ctx);
	input = std::make_unique<ControllerInputHandler>(*this, ctx);
	actions = std::make_unique<ControllerActionHandler>(*this, ctx);
}

Sorcery::Controller::~Controller() = default;

auto Sorcery::Controller::initialise() -> void {

	_busy = false;
	_has_save = _ctx.saves->has_game();

	_abort = false;

	_go_back = false;

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
			 "want_chest",
			 "want_donate",
			 "want_drop",
			 "want_enter_maze",
			 "want_help",
			 "want_inspect",
			 "want_identify",
			 "want_equip",
			 "want_remove",
			 "want_invoke",
			 "want_spell",

			 "want_name",
			 "want_name_ok",
			 "want_not_enough_gold",
			 "want_new_game",
			 "want_quit_expedition",
			 "want_stay",
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

auto Sorcery::Controller::set_flag_value(const std::string_view flag,
										 const bool value) -> void {

	_flags[std::string{flag}] = value;
}

auto Sorcery::Controller::set_game(Game *game) -> void {

	_game = game;
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

auto Sorcery::Controller::get_flag_ref(const std::string_view flag) -> bool & {

	if (!_flags.contains(flag))
		_flags[std::string{flag}] = false;

	return _flags.at(std::string{flag});
}

auto Sorcery::Controller::get_flag(const std::string_view flag) const -> bool {

	if (_flags.contains(flag))
		return _flags.at(std::string{flag});
	else
		return false;
}

auto Sorcery::Controller::set_flag(const std::string_view flag) -> void {

	_flags[std::string{flag}] = true;
}

auto Sorcery::Controller::toggle_flag(const std::string_view flag) -> void {

	if (_flags.contains(flag))
		_flags.at(std::string{flag}) = !_flags.at(std::string{flag});
	else
		_flags[std::string{flag}] = true;
}

auto Sorcery::Controller::unset_flag(const std::string_view flag) -> void {

	_flags[std::string{flag}] = false;
}

auto Sorcery::Controller::has_flag(const std::string_view flag) const -> bool {

	if (_flags.contains(flag))
		return _flags.at(std::string{flag}) == true;

	return false;
}

auto Sorcery::Controller::has_selected(const std::string_view flag) const
	-> bool {

	if (_selected.contains(flag))
		return _selected.at(std::string{flag}) != -1;

	return false;
}

auto Sorcery::Controller::set_selected(const std::string_view flag,
									   const int value) -> void {

	_selected[std::string{flag}] = value;
}

auto Sorcery::Controller::get_selected(const std::string_view flag) const
	-> int {

	if (_selected.contains(flag))
		return _selected.at(std::string{flag});
	else
		return -1;
}

auto Sorcery::Controller::unset_selected(const std::string_view flag) -> void {

	_selected[std::string{flag}] = -1;
}

auto Sorcery::Controller::has_text(const std::string_view flag) const -> bool {

	if (_texts.contains(flag))
		return _texts.at(std::string{flag}).length() > 0;

	return false;
}
auto Sorcery::Controller::set_text(const std::string_view flag,
								   const std::string &text) -> void {

	_texts[std::string{flag}] = text;
}
auto Sorcery::Controller::unset_text(const std::string_view flag) -> void {

	_texts[std::string{flag}] = "";
}

auto Sorcery::Controller::get_text(const std::string_view flag) const
	-> std::string {
	if (_texts.contains(std::string{flag}))
		return _texts.at(std::string{flag});

	return "";
}

auto Sorcery::Controller::get_character(const Enums::CharacterSlot slot) const
	-> int {

	if (_characters.contains(slot))
		return _characters.at(slot);
	else
		return -1;
}

auto Sorcery::Controller::has_character(const Enums::CharacterSlot slot) const
	-> bool {

	if (_characters.contains(slot))
		if (_characters.at(slot) != -1)
			return true;

	return false;
}

auto Sorcery::Controller::set_character(const Enums::CharacterSlot slot,
										const int value) -> void {

	_characters[slot] = value;
}

auto Sorcery::Controller::clear_character(const Enums::CharacterSlot slot)
	-> void {

	_characters[slot] = -1;
}

auto Sorcery::Controller::clear_modal_flags() -> void {

	for (const auto flag : {
			 "want_camp",
			 "want_inspect",
			 "want_stay",
			 "want_help",
			 "want_tithe",
			 "want_identify",
			 "want_chest",
			 "want_drop",
			 "want_trade",
			 "want_give",
			 "want_use",
			 "want_invoke",
			 "want_equip",
			 "want_remove",
			 "want_spell",
		 })
		unset_flag(flag);
}

auto Sorcery::Controller::abort(const bool value) -> void {

	_abort = value;
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

auto Sorcery::Controller::request_back() -> void {

	_go_back = true;
}

auto Sorcery::Controller::get_back() const -> bool {

	return _go_back;
}

auto Sorcery::Controller::consume_back() -> bool {

	return std::exchange(_go_back, false);
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