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

#include "core/controller/menuaction.hpp"

namespace Sorcery {

using enum Enums::MenuAction::Type;
using enum Enums::MenuAction::Function;
using enum Enums::Screen;

const ActionList COMPENDIUM_ACTIONS{{{.type = GOTOSCREEN, .screen = ATLAS}},
									{{.type = GOTOSCREEN, .screen = BESTIARY}},
									{{.type = NO_ACTION, .screen = NONE}},
									{{.type = GOTOSCREEN, .screen = MUSEUM}},
									{{.type = GOTOSCREEN, .screen = SPELLBOOK}},
									{{.type = GOTOSCREEN, .screen = MAINMENU}}};

const ActionList CASTLE_ACTIONS{{{.type = GOTOSCREEN, .screen = TAVERN}},
								{{.type = GOTOSCREEN, .screen = INN}},
								{{.type = GOTOSCREEN, .screen = SHOP}},
								{{.type = GOTOSCREEN, .screen = TEMPLE}},
								{{.type = GOTOSCREEN, .screen = EDGEOFTOWN}}};

const ActionList MAIN_MENU_ACTIONS{
	{{.type = SET_UI_BOOL, .ui_index = 0}},
	{{.type = SETFLAG, .flag = "want_continue_game"}},
	{{.type = GOTOSCREEN, .screen = OPTIONS}},
	{{.type = GOTOSCREEN, .screen = COMPENDIUM}},
	{{.type = GOTOSCREEN, .screen = LICENSE}},
	{{.type = SET_UI_BOOL, .ui_index = 1}}};

const ActionList EDGE_ACTIONS{{{.type = GOTOSCREEN, .screen = TRAINING}},
							  {{.type = GOTOSCREEN, .screen = ENGINE}},
							  {{.type = GOTOSCREEN, .screen = RESTART}},
							  {{.type = GOTOSCREEN, .screen = CASTLE}},
							  {{.type = SET_UI_BOOL, .ui_index = 0}}};

const ActionList STORE_ACTIONS{{{.type = GOTOSCREEN, .screen = BUY}},
							   {{.type = GOTOSCREEN, .screen = SELL}},
							   {{.type = GOTOSCREEN, .screen = UNCURSE}},
							   {{.type = GOTOSCREEN, .screen = IDENTIFY}},
							   {{.type = SET_UI_BOOL, .ui_index = 0},
								{.type = CUSTOM, .custom_function = POOL_GOLD}},
							   {{.type = GOTOSCREEN, .screen = SHOP}}};

const ActionList TAVERN_ACTIONS{{{.type = GOTOSCREEN, .screen = ADD}},
								{{.type = GOTOSCREEN, .screen = REMOVE}},
								{{.type = GOTOSCREEN, .screen = REORDER}},
								{{.type = SET_UI_BOOL, .ui_index = 0}},
								{{.type = GOTOSCREEN, .screen = CASTLE}}};

const ActionList TEMPLE_ACTIONS{{{.type = SET_UI_BOOL, .ui_index = 0}},
								{{.type = SET_UI_BOOL, .ui_index = 1}},
								{{.type = GOTOSCREEN, .screen = CASTLE}}};

const ActionList TRAINING_ACTIONS{{{.type = GOTOSCREEN, .screen = CREATE}},
								  {{.type = GOTOSCREEN, .screen = EDIT}},
								  {{.type = GOTOSCREEN, .screen = DELETE}},
								  {{.type = GOTOSCREEN, .screen = ROSTER}},
								  {{.type = GOTOSCREEN, .screen = EDGEOFTOWN}}};

const ActionList EDIT_ACTIONS{{{.type = GOTOSCREEN, .screen = SELECT}},
							  {{.type = GOTOSCREEN, .screen = RETRAIN}},
							  {{.type = GOTOSCREEN, .screen = LEGATE}},
							  {{.type = GOTOSCREEN, .screen = TRAINING}}};

const ActionList CAMP_ACTIONS{
	{{.type = GOTOSCREEN, .screen = INSPECT},
	 {.type = CLEAR_UI_BOOL, .ui_index = 0}},
	{{.type = GOTOSCREEN, .screen = REORDER},
	 {.type = CLEAR_UI_BOOL, .ui_index = 0}},
	{{.type = GOTOSCREEN, .screen = OPTIONS},
	 {.type = CLEAR_UI_BOOL, .ui_index = 0}},
	{{.type = SETFLAG, .flag = "want_quit_expedition"},
	 {.type = CLEAR_UI_BOOL, .ui_index = 0}},
	{{.type = CLEAR_UI_BOOL, .ui_index = 0}}};

const ActionList REST_ACTIONS{// Stables
							  {{.type = SET_SELECTED,
								.selected_key = "room_selected",
								.selected_value = 0},
							   {.type = GOTOSCREEN, .screen = RECOVERY}},

							  // Cot
							  {{.type = SET_SELECTED,
								.selected_key = "room_selected",
								.selected_value = 1},
							   {.type = GOTOSCREEN, .screen = RECOVERY}},

							  // Economy Rooms
							  {{.type = SET_SELECTED,
								.selected_key = "room_selected",
								.selected_value = 2},
							   {.type = GOTOSCREEN, .screen = RECOVERY}},

							  // Merchant Suites
							  {{.type = SET_SELECTED,
								.selected_key = "room_selected",
								.selected_value = 3},
							   {.type = GOTOSCREEN, .screen = RECOVERY}},

							  // Royal Suite
							  {{.type = SET_SELECTED,
								.selected_key = "room_selected",
								.selected_value = 4},
							   {.type = GOTOSCREEN, .screen = RECOVERY}},

							  // Return
							  {{.type = SET_SELECTED,
								.selected_key = "room_selected",
								.selected_value = -1},
							   {.type = GOTOSCREEN, .screen = INN}}};

const ActionList TOP_ELEVATOR_ACTIONS{
	// A - B1F
	{{.type = SET_SELECTED,
	  .selected_key = "elevator_selected",
	  .selected_value = -1},
	 {.type = SETFLAG, .flag = "want_take_elevator"},
	 {.type = CLEAR_UI_BOOL, .ui_index = 0}},

	// B - B2F
	{{.type = SET_SELECTED,
	  .selected_key = "elevator_selected",
	  .selected_value = -2},
	 {.type = SETFLAG, .flag = "want_take_elevator"},
	 {.type = CLEAR_UI_BOOL, .ui_index = 0}},

	// C - B3F
	{{.type = SET_SELECTED,
	  .selected_key = "elevator_selected",
	  .selected_value = -3},
	 {.type = SETFLAG, .flag = "want_take_elevator"},
	 {.type = CLEAR_UI_BOOL, .ui_index = 0}},

	// D - B4F
	{{.type = SET_SELECTED,
	  .selected_key = "elevator_selected",
	  .selected_value = -4},
	 {.type = SETFLAG, .flag = "want_take_elevator"},
	 {.type = CLEAR_UI_BOOL, .ui_index = 0}},

	// Leave
	{{.type = CLEAR_UI_BOOL, .ui_index = 0}}};

const ActionList BOTTOM_ELEVATOR_ACTIONS{
	// A - B4F
	{{.type = SET_SELECTED,
	  .selected_key = "elevator_selected",
	  .selected_value = -4},
	 {.type = SETFLAG, .flag = "want_take_elevator"},
	 {.type = CLEAR_UI_BOOL, .ui_index = 0}},

	// B - B5F
	{{.type = SET_SELECTED,
	  .selected_key = "elevator_selected",
	  .selected_value = -5},
	 {.type = SETFLAG, .flag = "want_take_elevator"},
	 {.type = CLEAR_UI_BOOL, .ui_index = 0}},

	// C - B6F
	{{.type = SET_SELECTED,
	  .selected_key = "elevator_selected",
	  .selected_value = -6},
	 {.type = SETFLAG, .flag = "want_take_elevator"},
	 {.type = CLEAR_UI_BOOL, .ui_index = 0}},

	// D - B7F
	{{.type = SET_SELECTED,
	  .selected_key = "elevator_selected",
	  .selected_value = -7},
	 {.type = SETFLAG, .flag = "want_take_elevator"},
	 {.type = CLEAR_UI_BOOL, .ui_index = 0}},

	// E - B8F
	{{.type = SET_SELECTED,
	  .selected_key = "elevator_selected",
	  .selected_value = -8},
	 {.type = SETFLAG, .flag = "want_take_elevator"},
	 {.type = CLEAR_UI_BOOL, .ui_index = 0}},

	// F - B9F
	{{.type = SET_SELECTED,
	  .selected_key = "elevator_selected",
	  .selected_value = -9},
	 {.type = SETFLAG, .flag = "want_take_elevator"},
	 {.type = CLEAR_UI_BOOL, .ui_index = 0}},

	// Leave
	{{.type = CLEAR_UI_BOOL, .ui_index = 0}}};

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
	{"top_elevator_menu", TOP_ELEVATOR_ACTIONS},
	{"bottom_elevator_menu", BOTTOM_ELEVATOR_ACTIONS},
	{"rest_menu", REST_ACTIONS},
};
#pragma GCC diagnostic pop

}