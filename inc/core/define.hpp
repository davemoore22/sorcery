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

#include "common/define.hpp"
#include "common/include.hpp"

namespace Sorcery {

using namespace std::literals;

// Gameplay settings
static const int NUM_GAME_SETTINGS{26};
static const std::string OPT_AUTO_SAVE{"autosave"};
static const std::string OPT_RECOMMENDED_MODE{"recommended_mode"};
static const std::string OPT_STRICT_MODE{"strict_mode"};
static const std::string OPT_CHEAT_MODE{"cheat_mode"};
static const std::string OPT_DICE_ROLLS{"dice_rolls"};
static const std::string GFX_WIREFRAME{"wireframe_mode"};
static const std::string GFX_TEXTURES{"display_textures"};
static const std::string GFX_TRAPS{"display_traps"};
static const std::string GFX_TELEPORTERS{"display_teleporters"};
static const std::string GFX_ENCOUNTERS{"display_encounters"};
static const std::string GFX_PROGRESS{"display_progress"};
static const std::string GAME_ALLOW_MIXED_ALIGNMENT_PARTY{"allow_mixed_alignment_party"};
static const std::string GAME_STAT_LOSS_ON_LEVEL_UP{"stat_loss_on_level_up"};
static const std::string GAME_REROLL_HIT_POINTS_ON_LEVEL_GAIN{"reroll_hit_points_on_level_gain"};
static const std::string GAME_STAT_RESET_ON_CLASS_CHANGE{"stat_reset_on_class_change"};
static const std::string GAME_AGING_ON_CLASS_CHANGE{"aging_on_class_change"};
static const std::string GAME_ALLOW_RANGED_WEAPONS{"allow_ranged_weapons"};
static const std::string GAME_SPELLCASTING_IN_SURPRISE_ROUND{"spellcasting_in_surprise_round"};
static const std::string GAME_BATCH_HEALING_AFTER_RETURN_TO_CASTLE{"batch_healing_after_return_to_castle"};
static const std::string GAME_REROLL_ONES_ON_DICE{"reroll_ones_on_dice"};
static const std::string GAME_PROTECT_TELEPORT{"protect_teleport_rock"};
static const std::string GAME_ALLOW_LOST_LEGATED{"allow_lost_legated"};
static const std::string GAME_ALLOW_CURABLE_DRAIN{"allow_curable_drained_levels"};
static const std::string GAME_ALLOW_SHARED_INVENTORY{"allow_shared_inventory"};
static const std::string GAME_CAMPING_RESTORE_SPELL_POINTS{"camping_restores_spell_points"};
static const std::string GAME_CHARGED_ITEMS{"charged_items"};

// Command Line Arguments
inline constexpr auto SKIP_INTRO{"skip-intro"sv};
inline constexpr auto CONTINUE_GAME{"continue-game"sv};
inline constexpr auto NEW_GAME{"new-game"sv};
inline constexpr auto RESTART_EXPEDITION{"restart-expedition"sv};
inline constexpr auto START_EXPEDITION{"start-expedition"sv};
inline constexpr auto QUICKSTART{"quickstart"sv};
inline constexpr auto GO_TO_COMPENDIUM{"go-to-compendium"sv};
inline constexpr auto GO_TO_LICENSE{"go-to-license"sv};
inline constexpr auto GO_TO_OPTIONS{"go-to-options"sv};

inline constexpr auto GO_TO_TRAINING{"go-to-training"sv};
inline constexpr auto GO_TO_TAVERN{"go-to-tavern"sv};
inline constexpr auto GO_TO_INN{"go-to-inn"sv};
inline constexpr auto GO_TO_TEMPLE{"go-to-temple"sv};
inline constexpr auto GO_TO_SHOP{"go-to-shop"sv};

// Other Images
inline constexpr auto LOADING_IMAGE{"loading.png"sv};

// Frame Limit
inline constexpr auto FRAMERATE{60u};

// Timed Settings (Milliseconds)
inline constexpr auto DELAY_TSLEEP{500u};
inline constexpr auto WALLPAPER_INTERVAL{30000u};
inline constexpr auto DELAY_ATTRACT{5000u};

// Game Config
static const std::string DEFAULT_SCREEN_HEIGHT{"default_height"};
static const std::string DEFAULT_SCREEN_WIDTH{"default_width"};
static const std::string CURRENT_SCREEN_HEIGHT{"current_height"};
static const std::string CURRENT_SCREEN_WIDTH{"current_width"};
static const std::string CELL_HEIGHT{"cell_height"};
static const std::string CELL_WIDTH{"cell_width"};
}