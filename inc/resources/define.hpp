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

#include "main.hpp"

namespace Sorcery {

using namespace std::literals;

// Directories
inline constexpr auto CONFIG_DIR{"cfg"sv};
inline constexpr auto DATA_DIR{"dat"sv};
inline constexpr auto DOCUMENTS_DIR{"doc"sv};
inline constexpr auto GUI_DIR{"gui"sv};
inline constexpr auto GRAPHICS_DIR{"gfx"sv};
inline constexpr auto VIDEO_DIR{"vfx"sv};

// Graphics
inline constexpr auto AUTOMAP_TEXTURE{"automap.png"sv};
inline constexpr auto BACKGROUND_TEXTURE{"background.png"sv};
inline constexpr auto BANNER_TEXTURE{"banner.png"sv};
inline constexpr auto CONTROLS_TEXTURE{"controls.png"sv};
inline constexpr auto CREATURES_KNOWN_TEXTURE{"creaturesknown.jpg"sv};
inline constexpr auto CREATURES_UNKNOWN_TEXTURE{"creaturesunknown.jpg"sv};
inline constexpr auto DOORS_TEXTURE{"doors.png"sv};
inline constexpr auto EVENTS_TEXTURE{"events.png"sv};
inline constexpr auto FLOORS_TEXTURE{"floors.png"sv};
inline constexpr auto ICONS_TEXTURE{"icons.png"sv};
inline constexpr auto ITEMS_TEXTURE{"items.png"sv};
inline constexpr auto LOGO_TEXTURE{"logo.png"sv};
inline constexpr auto PORTRAITS_TEXTURE{"potraits.png"sv};
inline constexpr auto SPLASH_TEXTURE{"splash.png"sv};
inline constexpr auto TOWN_TEXTURE{"town.png"sv};
inline constexpr auto UI_TEXTURE{"ui.png"sv};
inline constexpr auto VIEW_TEXTURE{"view.png"sv};
inline constexpr auto WALLS_TEXTURE{"walls.png"sv};
inline constexpr auto WIREFRAME_TEXTURE{"wireframe.png"sv};

// Graphics Sizes
inline constexpr auto ITEM_TILE_SIZE{108};
inline constexpr auto DUNGEON_TILE_SIZE{400};
inline constexpr auto WALLPAPER_TILE_SIZE{400};
inline constexpr auto PORTRAIT_TILE_SIZE{600};
inline constexpr auto VIEW_OFFSET_SIZE{1046};
inline constexpr auto ICON_TILE_SIZE{511.f};
inline constexpr auto EVENT_TILE_SIZE{511};
inline constexpr auto CREATURE_TILE_SIZE{400};

// Graphics Grid Width
inline constexpr auto CREATURE_TILE_ROW_COUNT{20};
inline constexpr auto DUNGEON_TILE_ROW_COUNT{15};
inline constexpr auto EVENT_TILE_ROW_COUNT{15};
inline constexpr auto ITEM_TILE_ROW_COUNT{15};
inline constexpr auto WALLPAPER_TILE_ROW_COUNT{15};
inline constexpr auto PORTRAIT_TILE_ROW_COUNT{6};
inline constexpr auto ICON_TILE_ROW_COUNT{15};
inline constexpr auto ATTRACT_MODE_TILE_SPACING{8};
inline constexpr auto AUTOMAP_TILE_SIZE{21};
inline constexpr auto AUTOMAP_TILE_ROW_COUNT{10};

// Miscellaneous error strings
static const std::string EXPLAIN_KEY_NOT_FOUND{"EXPLAIN KEY NOT FOUND"};
static const std::string KEY_NOT_FOUND{"KEY NOT FOUND"};
static const std::string STRINGS_NOT_LOADED{"GAME STRINGS NOT LOADED"};
static const std::string FILE_NOT_FOUND{"FILE NOT FOUND"};

}