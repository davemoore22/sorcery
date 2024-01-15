// Copyright (C) 2024 Dave Moore
//
// This file is part of Sorcery: Shadows under Llylgamyn.
//
// Sorcery: Shadows under Llylgamyn is free software: you can redistribute
// it and/or modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation, either version 2 of the License,
// or (at your option) any later version.
//
// Sorcery: Shadows under Llylgamyn is distributed in the hope that it wil
// be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
// Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Sorcery: Shadows under Llylgamyn.  If not,
// see <http://www.gnu.org/licenses/>.
//
// If you modify this program, or any covered work, by linking or combining
// it with the libraries referred to in README (or a modified version of
// said libraries), containing parts covered by the terms of said libraries,
// the licensors of this program grant you additional permission to convey
// the resulting work.

#pragma once

#include "common/include.hpp"

namespace Sorcery {

using namespace std::literals;

inline constexpr auto SAVE_VERSION{1};
inline constexpr auto MAX_PARTY_SIZE{6};

// Files
inline constexpr auto CONFIG_FILE{"config.ini"sv};
inline constexpr auto DATABASE_FILE{"sorcery.db3"sv};
inline constexpr auto GUI_FILE{"nanogui.style"sv};
inline constexpr auto MONO_FONT_FILE{"monospace.ttf"sv};
inline constexpr auto PROPORTIONAL_FONT_FILE{"proportional.ttf"sv};
inline constexpr auto TEXT_FONT_FILE{"text.ttf"sv};
inline constexpr auto INPUT_FONT_FILE{"input.ttf"sv};
inline constexpr auto HELP_FILE{"HELP"sv};
inline constexpr auto ICONS_FILE{"icons.json"sv};
inline constexpr auto ITEMS_FILE{"items.json"sv};
inline constexpr auto LAYOUT_FILE{"layout.json"sv};
inline constexpr auto LICENSE_FILE{"LICENSE"sv};
inline constexpr auto LEVELS_FILE{"levels.json"sv};
inline constexpr auto MONSTERS_FILE{"monsters.json"sv};
inline constexpr auto README_FILE{"README"sv};
inline constexpr auto STRINGS_FILE{"strings.json"sv};
inline constexpr auto TEXTURES_FILE{"textures.json"sv};
inline constexpr auto VERSION_FILE{"VERSION"sv};

// Videos
inline constexpr auto MENU_VIDEO{"menu.mp4"sv};

inline constexpr auto CREATURE_TILE_SIZE{400};

inline constexpr auto ON{"on"};
inline constexpr auto OFF{"off"};

inline constexpr auto EXIT_ALL{0};
inline constexpr auto EXIT_MODULE{1};

inline constexpr auto DELAY_DEFAULT{1000u};
}