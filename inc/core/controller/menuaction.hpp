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

#include "core/enum.hpp" // for Screen, CharacterSlot (ptr only)
#include <cstddef>		 // for size_t
#include <string>		 // for basic_string, string
#include <string_view>	 // for hash, operator==, string_view
#include <unordered_map> // for unordered_map
#include <vector>		 // for vector

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

extern const std::unordered_map<std::string_view, ActionList> MENU_ACTIONS;

}