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

// Enums
namespace Enums::Config {

	// Options
	enum Options {
		RECOMMENDED_MODE = 0,
		STRICT_MODE = 1,
		AUTO_SAVE = 2,
		CHEAT_MODE = 3,
		DICE_ROLLS = 4,
		WIREFRAME_MODE = 5,
		DISPLAY_TEXTURES = 6,
		DISPLAY_TRAPS = 7,
		DISPLAY_TELEPORTERS = 8,
		DISPLAY_ENCOUNTERS = 9,
		DISPLAY_PROGRESS = 10,
		ALLOW_MIXED_ALIGNMENT_PARTY = 11,
		STAT_LOSS_ON_LEVEL_UP = 12,
		REROLL_HIT_POINTS_ON_LEVEL_GAIN = 13,
		STAT_RESET_ON_CLASS_CHANGE = 14,
		AGING_ON_CLASS_CHANGE = 15,
		PROTECT_TELEPORT = 16,
		ALLOW_RANGED_WEAPONS = 17,
		SPELLCASTING_IN_SURPRISE_ROUND = 18,
		BATCH_HEALING_AFTER_RETURN_TO_CASTLE = 19,
		REROLL_ONES_ON_DICE = 20,
		ALLOW_LOST_LEGATED = 21,
		ALLOW_CURABLE_DRAIN = 22,
		ALLOW_SHARED_INVENTORY = 23,
		CAMPING_RESTORE_SPELL_POINTS = 24,
		CHARGED_ITEMS = 25,
		NONE = 26 // Note this is ignored by NUM_GAME_SETTINGS

		// need additional options (turned off in strict mode) for
		// dispelled creatures give 1/4 xp
		// non-dead characters get full xp
		// extra spell slots for first level characters
		// additional selected Wizardry V spells
		// LOKOFEIT - levitate spell
		// DESTO - unlocks door
		// CALIFIC - reveals secret door
		// LABADI - drain hp from enemy and add to caster
	};
}

using ConfigOption = Enums::Config::Options;

}