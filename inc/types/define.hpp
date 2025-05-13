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

// On/Off Settings
inline constexpr auto OPT_ON{"on"};
inline constexpr auto OPT_OFF{"off"};

// Gameplay settings
static const int NUM_GAME_SETTINGS{19};
static const std::string OPT_RECOMMENDED_MODE{"recommended_mode"};
static const std::string OPT_STRICT_MODE{"strict_mode"};
static const std::string OPT_CHEAT_MODE{"cheat_mode"};
static const std::string OPT_AUTO_SAVE{"autosave"};
static const std::string OPT_DICE_ROLLS{"dice_rolls"};
static const std::string OPT_COLOURED_WIREFRAME{"coloured_wireframe"};
static const std::string OPT_MIXED_ALIGNMENT{"mixed_alignment"};
static const std::string OPT_LEVEL_STAT_LOSS{"level_stat_loss"};
static const std::string OPT_LEVEL_REROLL_HP{"level_reroll_hp"};
static const std::string OPT_CLASS_CHANGE_RESET{"class_change_reset"};
static const std::string OPT_CLASS_CHANGE_AGING{"class_change_aging"};
static const std::string OPT_AMBUSH_HIDE{"ambush_hide"};
static const std::string OPT_SURPRISE_SPELLCASTING{"surprise_spellcasting"};
static const std::string OPT_INN_HEALING{"inn_healing"};
static const std::string OPT_REROLL_ONES{"reroll_ones"};
static const std::string OPT_LOST_LEGATION{"lost_legation"};
static const std::string OPT_CURABLE_DRAINING{"curable_draining"};
static const std::string OPT_SHARED_INVENTORY{"shared_inventory"};
static const std::string OPT_PROTECT_TELEPORT{"protect_teleport"};
}
