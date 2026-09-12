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
#include <string>

namespace Sorcery {

// On/Off Settings
inline constexpr auto OPT_ON{"on"};
inline constexpr auto OPT_OFF{"off"};

// Gameplay settings
inline constexpr auto NUM_GAME_SETTINGS{static_cast<std::size_t>(Enums::Config::Options::NUM_OPTIONS)};
inline constexpr std::string_view OPT_RECOMMENDED_MODE{"recommended_mode"};
inline constexpr std::string_view OPT_STRICT_MODE{"strict_mode"};
inline constexpr std::string_view OPT_CHEAT_MODE{"cheat_mode"};
inline constexpr std::string_view OPT_AUTO_SAVE{"autosave"};
inline constexpr std::string_view OPT_DICE_ROLLS{"dice_rolls"};

inline constexpr std::string_view OPT_MIXED_ALIGNMENT{"mixed_alignment"};
inline constexpr std::string_view OPT_LEVEL_STAT_LOSS{"level_stat_loss"};
inline constexpr std::string_view OPT_LEVEL_REROLL_HP{"level_reroll_hp"};
inline constexpr std::string_view OPT_CLASS_CHANGE_RESET{"class_change_reset"};
inline constexpr std::string_view OPT_CLASS_CHANGE_AGING{"class_change_aging"};
inline constexpr std::string_view OPT_AMBUSH_HIDE{"ambush_hide"};
inline constexpr std::string_view OPT_SURPRISE_SPELLCASTING{"surprise_spellcasting"};
inline constexpr std::string_view OPT_INN_HEALING{"inn_healing"};
inline constexpr std::string_view OPT_REROLL_ONES{"reroll_ones"};
inline constexpr std::string_view OPT_LOST_LEGATION{"lost_legation"};
inline constexpr std::string_view OPT_CURABLE_DRAINING{"curable_draining"};
inline constexpr std::string_view OPT_SHARED_INVENTORY{"shared_inventory"};
inline constexpr std::string_view OPT_PROTECT_TELEPORT{"protect_teleport"};

inline constexpr std::string_view OPT_CGA_GRAPHICS{"cga_graphics"};
inline constexpr std::string_view OPT_FULLSCREEN{"fullscreen"};

inline constexpr std::string_view OPT_SOUND{"sound"};
inline constexpr std::string_view OPT_MUSIC{"music"};
}
