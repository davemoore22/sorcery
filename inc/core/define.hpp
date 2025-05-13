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

namespace Sorcery {

// Timed Settings (Milliseconds)
inline constexpr auto DELAY_TSLEEP{500u};
inline constexpr auto WALLPAPER_INTERVAL{30000u};
inline constexpr auto DELAY_ATTRACT{5000u};

// Graphics Data
inline constexpr auto CREATURE_TILE_ROW_COUNT{20u};
inline constexpr auto ICONS_TILE_ROW_COUNT{15u};
inline constexpr auto ITEM_TILE_ROW_COUNT{15u};
inline constexpr auto MAP_TILE_ROW_COUNT{10u};

// Command Line Parameters
inline constexpr auto CONTINUE_GAME_PARAM{"--continue"sv};
inline constexpr auto NEW_GAME_PARAM{"--new"sv};
inline constexpr auto QUICKSTART_PARAM{"--quickstart"sv};
inline constexpr auto GO_TO_MAZE_PARAM{"--go-to-maze"sv};

// Window Data
#define WINDOW_LAYER_BG "##layer_bg"
#define WINDOW_LAYER_FRAMES "##layer_frames"
#define WINDOW_LAYER_VIEW "##layer_view"
#define WINDOW_LAYER_IMAGES "##layer_images"
#define WINDOW_LAYER_TEXTS "##layer_texts"
#define WINDOW_LAYER_MENUS "##layer_menu"

// Icon Data
#define ICON_HOURGLASS 59
#define ICON_BUFF_LIGHT 78
#define ICON_BUFF_EXTRA_LIGHT 75
#define ICON_BUFF_SHIELD 76
#define ICON_BUFF_KNOWLEDGE 77
#define ICON_LEVITATE 79
#define ICON_CURSOR 91
#define ICON_COMPASS_NORTH 92
#define ICON_COMPASS_SOUTH 93
#define ICON_COMPASS_EAST 94
#define ICON_COMPASS_WEST 95
#define ICON_CAMP 54
#define ICON_PARTY 62
#define ICON_CAST 63
#define ICON_USE 64
#define ICON_OPTIONS 67
#define ICON_MAP 73
#define ICON_LOOK 60
#define ICON_SAVE_AND_QUIT 69

// Expedition Mode
inline constexpr auto EXPEDITION_START{1};
inline constexpr auto EXPEDITION_RESTART{2};
inline constexpr auto EXPEDITION_GOTO{16};

};