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

// Menu Options
inline constexpr auto DEST_NONE{-1};
inline constexpr auto ABORT_GAME{-1}; // Quit Game Immediately
inline constexpr auto LEAVE_GAME{-2}; // Return to Main Menu
inline constexpr auto START_MAZE{-3}; // Start the Maze
inline constexpr auto LEAVE_MAZE{-4}; // Leave the Maze
inline constexpr auto MAIN_MENU_NEW_GAME{0};
inline constexpr auto MAIN_MENU_CONTINUE_GAME{1};
inline constexpr auto MAIN_MENU_OPTIONS{2};
inline constexpr auto MAIN_MENU_COMPENDIUM{3};
inline constexpr auto MAIN_MENU_LICENSE{4};
inline constexpr auto MAIN_MENU_EXIT_GAME{5};
inline constexpr auto CASTLE_GO_TO_TAVERN{0};
inline constexpr auto CASTLE_GO_TO_INN{1};
inline constexpr auto CASTLE_GO_TO_SHOP{2};
inline constexpr auto CASTLE_GO_TO_TEMPLE{3};
inline constexpr auto TAVERN_ADD{0};
inline constexpr auto TAVERN_REMOVE{1};
inline constexpr auto TAVERN_REORDER{2};
inline constexpr auto TAVERN_INSPECT{3};
inline constexpr auto TAVERN_DIVVY_GOLD{4};
inline constexpr auto CAMP_INSPECT{0};
inline constexpr auto CAMP_REORDER{1};
inline constexpr auto CAMP_OPTIONS{2};
inline constexpr auto CAMP_QUIT{3};
inline constexpr auto CAMP_LEAVE{4};
inline constexpr auto TEMPLE_HELP{0};
inline constexpr auto TEMPLE_INSPECT{1};
inline constexpr auto TEMPLE_TITHE{2};
inline constexpr auto SHOP_INSPECT{1};
inline constexpr auto INN_INSPECT{1};
inline constexpr auto INN_STAY{0};
inline constexpr auto HEALED_NOT{0};
inline constexpr auto HEALED_OK{1};
inline constexpr auto HEALED_DEAD_TO_ASHES{2};
inline constexpr auto HEALED_LOST{4};
inline constexpr auto CASTLE_GO_TO_EDGE_OF_TOWN{4};
inline constexpr auto EDGE_OF_TOWN_GO_TO_TRAINING{0};
inline constexpr auto EDGE_OF_TOWN_GO_TO_MAZE{1};
inline constexpr auto EDGE_OF_TOWN_RESTART{2};
inline constexpr auto EDGE_OF_TOWN_GO_TO_CASTLE{3};
inline constexpr auto EDGE_OF_TOWN_LEAVE_GAME{4};
inline constexpr auto BACK_TO_CASTLE{4};
inline constexpr auto BACK_TO_TAVERN{4};
inline constexpr auto BACK_TO_INN{4};
inline constexpr auto BACK_TO_STAY{4};
inline constexpr auto BACK_TO_TEMPLE{4};
inline constexpr auto BACK_TO_SHOP{4};
inline constexpr auto BACK_TO_EDGE_OF_TOWN{4};
inline constexpr auto BACK_FROM_REORDER{4};
inline constexpr auto BACK_FROM_ROSTER{4};
inline constexpr auto BACK_FROM_INSPECT{4};
inline constexpr auto BACK_FROM_CHOOSE{4};
inline constexpr auto CHECK_FOR_LEVEL_GAIN{5};
inline constexpr auto ROSTER_MODE_INN{1};
inline constexpr auto ROSTER_MODE_SHOP{2};
inline constexpr auto ROSTER_MODE_TAVERN{3};
inline constexpr auto ROSTER_MODE_TEMPLE{4};
inline constexpr auto CHOOSE_MODE_STAY{1};
inline constexpr auto REORDER_MODE_TAVERN{1};
inline constexpr auto REORDER_MODE_CAMP{2};
inline constexpr auto RECOVERY_BIRTHDAY{1};
inline constexpr auto RECOVERY_MODE_FREE{1};
inline constexpr auto RECOVERY_MODE_COST_10{2};
inline constexpr auto RECOVERY_MODE_COST_50{4};
inline constexpr auto RECOVERY_MODE_COST_200{8};
inline constexpr auto RECOVERY_MODE_COST_500{16};
inline constexpr auto INSPECT_MODE_TAVERN{1};
inline constexpr auto INSPECT_MODE_CAMP{2};
inline constexpr auto INSPECT_MODE_INN{3};
inline constexpr auto INSPECT_MODE_TEMPLE{4};
inline constexpr auto GO_TO_MAZE{-3};
inline constexpr auto RESTART_MAZE{-4};
inline constexpr auto GO_TO_FRONT_END{-1};
inline constexpr auto GO_TO_COMPENDIUM{0};
inline constexpr auto COMPENDIUM_ATLAS{0};
inline constexpr auto COMPENDIUM_BESTIARY{1};
inline constexpr auto COMPENDIUM_MUSEUM{3};
inline constexpr auto COMPENDIUM_SPELLBOOK{4};

// Party Menu Mode
inline constexpr auto NO_FLAGS{0};
inline constexpr auto MENU_SHOW_POSITION{1};
inline constexpr auto MENU_SHOW_GOLD{2};

// Item Menu Mode
inline constexpr auto MENU_IDENTIFY_ITEM{1};
inline constexpr auto MENU_USE_ITEM{2};
inline constexpr auto MENU_INVOKE_ITEM{4};
inline constexpr auto MENU_SHOP_IDENTIFY_ITEM{8};
}