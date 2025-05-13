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

namespace Sorcery {

using namespace std::literals;

// Directories
inline constexpr auto CONFIG_DIR{"cfg"sv};
inline constexpr auto DATA_DIR{"dat"sv};
inline constexpr auto DOCUMENTS_DIR{"doc"sv};
inline constexpr auto GFX_DIR{"gfx"sv};
inline constexpr auto VFX_DIR{"vfx"sv};

// Files
inline constexpr auto CONFIG_FILE{"config.ini"sv};
inline constexpr auto DATABASE_FILE{"save.db3"sv};
inline constexpr auto ITEMS_FILE{"items.json"sv};
inline constexpr auto LAYOUT_FILE{"layout.json"sv};
inline constexpr auto LICENSE_FILE{"LICENSE"sv};
inline constexpr auto MAPS_FILE{"maps.json"sv};
inline constexpr auto MONSTERS_FILE{"monsters.json"sv};
inline constexpr auto STRINGS_FILE{"strings.json"sv};

inline constexpr auto MONOSPACE_A_FONT_FILE{"monospace-a.ttf"sv};
inline constexpr auto MONOSPACE_B_FONT_FILE{"monospace-b.ttf"sv};
inline constexpr auto MONOSPACE_C_FONT_FILE{"monospace-c.ttf"sv};
inline constexpr auto MONOSPACE_D_FONT_FILE{"monospace-d.ttf"sv};
inline constexpr auto PROPORTIONAL_FONT_FILE{"proportional.ttf"sv};
inline constexpr auto TEXT_FONT_FILE{"text.ttf"sv};

inline constexpr auto BACKGROUND_MAIN_MENU_TEXTURE{"background-mm.png"sv};
inline constexpr auto BANNER_TEXTURE{"banner.png"sv};
inline constexpr auto KNOWN_CREATURES_TEXTURE{"known.png"sv};
inline constexpr auto ICONS_TEXTURE{"icons.png"sv};
inline constexpr auto ITEMS_TEXTURE{"items.png"sv};
inline constexpr auto LOGO_TEXTURE{"logo.png"sv};
inline constexpr auto MAPS_TEXTURE{"maps.png"sv};
inline constexpr auto MAINMENU_VIDEO{"mainmenu.mp4"sv};
inline constexpr auto UNKNOWN_CREATURES_TEXTURE{"unknown.png"sv};
inline constexpr auto WIREFRAME_TEXTURE{"wireframe.png"sv};

// Miscellaneous error strings
static const std::string KEY_NOT_FOUND{"KEY NOT FOUND"};
static const std::string STRINGS_NOT_LOADED{"GAME STRINGS NOT LOADED"};
static const std::string FILE_NOT_FOUND{"FILE NOT FOUND"};

}