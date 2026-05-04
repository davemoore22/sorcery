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
			SETFLAG,
			CLEARFLAG,
			SETCHARACTER,
			CLEARCHARACTER,
			SET_UI_BOOL,
			CLEAR_UI_BOOL,
			CUSTOM
		};

		Type type{Type::NONE};

		// Payload
		Enums::Screen screen{};
		std::string flag;
		std::string character_slot;
		std::size_t ui_index{0};
		std::string character_key;
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
	{{.type = MenuAction::Type::SETFLAG, .flag = "want_enter_maze"}},
	{{.type = MenuAction::Type::GOTOSCREEN, .screen = Enums::Screen::RESTART}},
	{{.type = MenuAction::Type::GOTOSCREEN, .screen = Enums::Screen::CASTLE}},
	{{.type = MenuAction::Type::SET_UI_BOOL, .ui_index = 0}}};

const ActionList INN_ACTIONS{
	{{.type = MenuAction::Type::GOTOSCREEN, .ui_index = 1}},
	{{.type = MenuAction::Type::GOTOSCREEN, .ui_index = 0}},
	{{.type = MenuAction::Type::GOTOSCREEN, .screen = Enums::Screen::CASTLE}}};

const ActionList TAVERN_ACTIONS{
	{{.type = MenuAction::Type::GOTOSCREEN, .screen = Enums::Screen::ADD}},
	{{.type = MenuAction::Type::GOTOSCREEN, .screen = Enums::Screen::REMOVE}},
	{{.type = MenuAction::Type::GOTOSCREEN, .screen = Enums::Screen::REORDER}},
	{{.type = MenuAction::Type::SET_UI_BOOL, .ui_index = 1}},
	{{.type = MenuAction::Type::SET_UI_BOOL, .ui_index = 0}},
	{{.type = MenuAction::Type::GOTOSCREEN, .screen = Enums::Screen::CASTLE}}};

const ActionList TEMPLE_ACTIONS{
	{{.type = MenuAction::Type::SET_UI_BOOL, .ui_index = 2}},
	{{.type = MenuAction::Type::SET_UI_BOOL, .ui_index = 1}},
	{{.type = MenuAction::Type::SET_UI_BOOL, .ui_index = 0}},
	{{.type = MenuAction::Type::GOTOSCREEN, .screen = Enums::Screen::CASTLE}}};

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

const ActionList INSPECT_ACTIONS{
	{{.type = MenuAction::Type::SETCHARACTER, .character_key = "inspect"},
	 {.type = MenuAction::Type::CLEAR_UI_BOOL, .ui_index = 0}},
	{{.type = MenuAction::Type::CLEARCHARACTER, .character_key = "inspect"},
	 {.type = MenuAction::Type::CLEAR_UI_BOOL, .ui_index = 0}}};

const std::unordered_map<std::string_view, ActionList> MENU_ACTIONS{
	{"compendium_menu", COMPENDIUM_ACTIONS},
	{"castle_menu", CASTLE_ACTIONS},
	{"main_menu", MAIN_MENU_ACTIONS},
	{"edge_menu", EDGE_ACTIONS},
	{"inn_menu", INN_ACTIONS},
	{"tavern_menu", TAVERN_ACTIONS},
	{"temple_menu", TEMPLE_ACTIONS},
	{"camp_menu", CAMP_ACTIONS},
	{"inspect_menu", INSPECT_ACTIONS},

};
#pragma GCC diagnostic pop
}