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

#pragma once

#include "common/enum.hpp"
#include "common/types.hpp"
#include "core/enum.hpp"
#include "gui/define.hpp"

namespace Sorcery {

struct MenuAction {
		enum class Type {
			NONE,
			GOTOSCREEN,
			GO_BACK,
			SETFLAG,
			CLEARFLAG,
			SET_CHARACTER,
			CLEAR_CHARACTER,
			SET_UI_BOOL,
			CLEAR_UI_BOOL,
			SET_SELECTED,
			CUSTOM
		};

		Type type{Type::NONE};

		// Payload
		Enums::Screen screen{};
		std::string flag{};
		std::string character_slot{};
		std::size_t ui_index{};
		Enums::CharacterSlot character_key{};
		std::string custom_function{};
		std::string selected_key{};
		int selected_value{};
};

using ActionList = std::vector<std::vector<MenuAction>>;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"

const ActionList COMPENDIUM_ACTIONS{
	{{.type = MenuAction::Type::GOTOSCREEN, .screen = Enums::Screen::ATLAS}},
	{{.type = MenuAction::Type::GOTOSCREEN, .screen = Enums::Screen::BESTIARY}},
	{{.type = MenuAction::Type::NONE, .screen = Enums::Screen::NONE}},
	{{.type = MenuAction::Type::GOTOSCREEN, .screen = Enums::Screen::MUSEUM}},
	{{.type = MenuAction::Type::GOTOSCREEN,
	  .screen = Enums::Screen::SPELLBOOK}},
	{{.type = MenuAction::Type::GOTOSCREEN,
	  .screen = Enums::Screen::MAINMENU}}};

const ActionList CASTLE_ACTIONS{
	{{.type = MenuAction::Type::GOTOSCREEN, .screen = Enums::Screen::TAVERN}},
	{{.type = MenuAction::Type::GOTOSCREEN, .screen = Enums::Screen::INN}},
	{{.type = MenuAction::Type::GOTOSCREEN, .screen = Enums::Screen::SHOP}},
	{{.type = MenuAction::Type::GOTOSCREEN, .screen = Enums::Screen::TEMPLE}},
	{{.type = MenuAction::Type::GOTOSCREEN,
	  .screen = Enums::Screen::EDGEOFTOWN}}};

const ActionList MAIN_MENU_ACTIONS{
	{{.type = MenuAction::Type::SET_UI_BOOL, .ui_index = 0}},
	{{.type = MenuAction::Type::SETFLAG, .flag = "want_continue_game"}},
	{{.type = MenuAction::Type::GOTOSCREEN, .screen = Enums::Screen::OPTIONS}},
	{{.type = MenuAction::Type::GOTOSCREEN,
	  .screen = Enums::Screen::COMPENDIUM}},
	{{.type = MenuAction::Type::GOTOSCREEN, .screen = Enums::Screen::LICENSE}},
	{{.type = MenuAction::Type::SET_UI_BOOL, .ui_index = 1}}};

const ActionList EDGE_ACTIONS{
	{{.type = MenuAction::Type::GOTOSCREEN, .screen = Enums::Screen::TRAINING}},
	{{.type = MenuAction::Type::GOTOSCREEN, .screen = Enums::Screen::ENGINE}},
	{{.type = MenuAction::Type::GOTOSCREEN, .screen = Enums::Screen::RESTART}},
	{{.type = MenuAction::Type::GOTOSCREEN, .screen = Enums::Screen::CASTLE}},
	{{.type = MenuAction::Type::SET_UI_BOOL, .ui_index = 0}}};

const ActionList STORE_ACTIONS{
	{{.type = MenuAction::Type::GOTOSCREEN,
	  .screen = Enums::Screen::BUY}}, // Buy
	{{.type = MenuAction::Type::GOTOSCREEN,
	  .screen = Enums::Screen::SELL}}, // Sell
	{{.type = MenuAction::Type::GOTOSCREEN,
	  .screen = Enums::Screen::UNCURSE}}, // Uncurse
	{{.type = MenuAction::Type::GOTOSCREEN,
	  .screen = Enums::Screen::IDENTIFY}}, // Identify
	{{.type = MenuAction::Type::SET_UI_BOOL, .ui_index = 0},
	 {.type = MenuAction::Type::CUSTOM, .custom_function = "handle_pool_gold"}},
	{{.type = MenuAction::Type::GOTOSCREEN, .screen = Enums::Screen::SHOP}}};

const ActionList TAVERN_ACTIONS{
	{{.type = MenuAction::Type::GOTOSCREEN, .screen = Enums::Screen::ADD}},
	{{.type = MenuAction::Type::GOTOSCREEN, .screen = Enums::Screen::REMOVE}},
	{{.type = MenuAction::Type::GOTOSCREEN, .screen = Enums::Screen::REORDER}},
	{{.type = MenuAction::Type::SET_UI_BOOL, .ui_index = 0}},
	{{.type = MenuAction::Type::GOTOSCREEN, .screen = Enums::Screen::CASTLE}}};

const ActionList TEMPLE_ACTIONS{
	{{.type = MenuAction::Type::SET_UI_BOOL, .ui_index = 0}},
	{{.type = MenuAction::Type::SET_UI_BOOL, .ui_index = 1}},
	{{.type = MenuAction::Type::GOTOSCREEN, .screen = Enums::Screen::CASTLE}}};

const ActionList TRAINING_ACTIONS{
	{{.type = MenuAction::Type::GOTOSCREEN, .screen = Enums::Screen::CREATE}},
	{{.type = MenuAction::Type::GOTOSCREEN, .screen = Enums::Screen::EDIT}},
	{{.type = MenuAction::Type::GOTOSCREEN, .screen = Enums::Screen::DELETE}},
	{{.type = MenuAction::Type::GOTOSCREEN, .screen = Enums::Screen::ROSTER}},
	{{.type = MenuAction::Type::GOTOSCREEN,
	  .screen = Enums::Screen::EDGEOFTOWN}}};

const ActionList EDIT_ACTIONS{
	{{.type = MenuAction::Type::GOTOSCREEN, .screen = Enums::Screen::SELECT},
	 {.type = MenuAction::Type::SETFLAG, .flag = "want_rename"}},
	{{.type = MenuAction::Type::GOTOSCREEN, .screen = Enums::Screen::RETRAIN},
	 {.type = MenuAction::Type::SETFLAG, .flag = "want_reclass"}},
	{{.type = MenuAction::Type::GOTOSCREEN, .screen = Enums::Screen::SELECT},
	 {.type = MenuAction::Type::SETFLAG, .flag = "want_legate"}},
	{{.type = MenuAction::Type::GOTOSCREEN,
	  .screen = Enums::Screen::TRAINING}}};

const ActionList CAMP_ACTIONS{
	{{.type = MenuAction::Type::GOTOSCREEN, .screen = Enums::Screen::INSPECT},
	 {.type = MenuAction::Type::CLEAR_UI_BOOL, .ui_index = 0}},
	{{.type = MenuAction::Type::GOTOSCREEN, .screen = Enums::Screen::REORDER},
	 {.type = MenuAction::Type::CLEAR_UI_BOOL, .ui_index = 0}},
	{{.type = MenuAction::Type::GOTOSCREEN, .screen = Enums::Screen::OPTIONS},
	 {.type = MenuAction::Type::CLEAR_UI_BOOL, .ui_index = 0}},
	{{.type = MenuAction::Type::SETFLAG, .flag = "want_quit_expedition"},
	 {.type = MenuAction::Type::CLEAR_UI_BOOL, .ui_index = 0}},
	{{.type = MenuAction::Type::CLEAR_UI_BOOL, .ui_index = 0}}};

const ActionList REST_ACTIONS{
	// Stables
	{{.type = MenuAction::Type::SET_SELECTED,
	  .selected_key = "room_selected",
	  .selected_value = 0},
	 {.type = MenuAction::Type::GOTOSCREEN, .screen = Enums::Screen::RECOVERY}},

	// Cot
	{{.type = MenuAction::Type::SET_SELECTED,
	  .selected_key = "room_selected",
	  .selected_value = 1},
	 {.type = MenuAction::Type::GOTOSCREEN, .screen = Enums::Screen::RECOVERY}},

	// Economy Rooms
	{{.type = MenuAction::Type::SET_SELECTED,
	  .selected_key = "room_selected",
	  .selected_value = 2},
	 {.type = MenuAction::Type::GOTOSCREEN, .screen = Enums::Screen::RECOVERY}},

	// Merchant Suites
	{{.type = MenuAction::Type::SET_SELECTED,
	  .selected_key = "room_selected",
	  .selected_value = 3},
	 {.type = MenuAction::Type::GOTOSCREEN, .screen = Enums::Screen::RECOVERY}},

	// Royal Suite
	{{.type = MenuAction::Type::SET_SELECTED,
	  .selected_key = "room_selected",
	  .selected_value = 4},
	 {.type = MenuAction::Type::GOTOSCREEN, .screen = Enums::Screen::RECOVERY}},

	// Return
	{{.type = MenuAction::Type::SET_SELECTED,
	  .selected_key = "room_selected",
	  .selected_value = -1},
	 {.type = MenuAction::Type::GOTOSCREEN, .screen = Enums::Screen::INN}}};

const std::unordered_map<std::string_view, ActionList> MENU_ACTIONS{
	{"compendium_menu", COMPENDIUM_ACTIONS},
	{"castle_menu", CASTLE_ACTIONS},
	{"main_menu", MAIN_MENU_ACTIONS},
	{"edge_menu", EDGE_ACTIONS},
	{"tavern_menu", TAVERN_ACTIONS},
	{"training_menu", TRAINING_ACTIONS},
	{"store_menu", STORE_ACTIONS},
	{"temple_menu", TEMPLE_ACTIONS},
	{"camp_menu", CAMP_ACTIONS},
	{"edit_menu", EDIT_ACTIONS},
	{"rest_menu", REST_ACTIONS}

};
#pragma GCC diagnostic pop
}