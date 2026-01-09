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

#pragma once

#include "common/include.hpp"
#include "common/types.hpp"
#include "core/enum.hpp"
#include "gui/define.hpp"

namespace Sorcery {

struct MenuAction {
		enum class Type {
			NONE,
			GOTOSCREEN,
			OPENMODAL,
			CLOSEMODAL,
			SETFLAG,
			CLEARFLAG,
			SETCHARACTER,
			CUSTOM
		};

		Type type{Type::NONE};

		// Payload
		Enums::Screen screen{};
		std::string flag;
		int character_id{-1};
};

using ActionList = std::vector<MenuAction>;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"

const std::vector<MenuAction> COMPENDIUM_ACTIONS{
	{MenuAction::Type::GOTOSCREEN, Enums::Screen::ATLAS},
	{MenuAction::Type::GOTOSCREEN, Enums::Screen::BESTIARY},
	{MenuAction::Type::NONE, Enums::Screen::NONE},
	{MenuAction::Type::GOTOSCREEN, Enums::Screen::MUSEUM},
	{MenuAction::Type::GOTOSCREEN, Enums::Screen::SPELLBOOK},
	{MenuAction::Type::GOTOSCREEN, Enums::Screen::MAINMENU}};

const std::vector<MenuAction> CASTLE_ACTIONS{
	{MenuAction::Type::GOTOSCREEN, Enums::Screen::TAVERN},
	{MenuAction::Type::GOTOSCREEN, Enums::Screen::INN},
	{MenuAction::Type::GOTOSCREEN, Enums::Screen::SHOP},
	{MenuAction::Type::GOTOSCREEN, Enums::Screen::TEMPLE},
	{MenuAction::Type::GOTOSCREEN, Enums::Screen::EDGEOFTOWN}};

const std::unordered_map<std::string_view, ActionList> MENU_ACTIONS{
	{"compendium_menu", COMPENDIUM_ACTIONS},
	{"castle_menu", CASTLE_ACTIONS},
};

#pragma GCC diagnostic pop

}