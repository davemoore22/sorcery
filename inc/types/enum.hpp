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

namespace Enums::Monsters {

	enum class Category {
		NO_MONSTER_CATEGORY = 0,
		HUMANOID = 1,
		ANIMAL = 2,
		DEMON = 3,
		DRAGON = 4,
		ENCHANTED = 5,
		GIANT = 6,
		INSECT = 7,
		MYTHICAL = 8,
		UNDEAD = 9,
		WERE = 10
	};

	enum class Class {
		NO_CLASS = 0,
		FIGHTER = 1,
		MAGE = 2,
		PRIEST = 3,
		THIEF = 4,
		BISHOP = 5,
		SAMURAI = 6,
		LORD = 7,
		NINJA = 8
	};

	// Following the original numbering at http://www.pekori.jp/~emonoya/monster/index.html for
	// the sake of convenience and ease of replicating original item tables
	enum class TypeID {
		BUBBLY_SLIME = 0,
		ORC = 1,
		KOBOLD = 2,
		UNDEAD_KOBOLD = 3,
		ROGUE = 4,
		BUSHWACKER = 5,
		HIGHWAYMAN = 6,
		ZOMBIE = 7,
		CREEPING_CRUD = 8,
		GAS_CLOUD = 9,
		LVL_1_MAGE = 10,
		LVL_1_PRIEST = 11,
		CREEPING_COIN = 12,
		LVL_1_NINJA = 13,
		VORPAL_BUNNY = 14,
		CAPYBARA = 15,
		GIANT_TOAD = 16,
		COYOTE = 17,
		LVL_3_PRIEST = 18,
		LVL_3_SAMURAI = 19,
		LVL_3_NINJA = 20,
		WERE_BEAR = 21,
		DRAGON_FLY = 22,
		ROTTING_CORPSE = 23,
		OGRE = 24,
		HUGE_SPIDER_A = 25,
		WERERAT = 26,
		BORING_BEETLE = 27,
		GAS_DRAGON = 28,
		PRIESTESS = 29,
		SWORDSMAN = 30,
		HUGE_SPIDER_B = 31,
		ATTACK_DOG = 32,
		GARGOYLE = 33,
		GRAVE_MIST = 34,
		DRAGON_PUPPY = 35,
		WEREWOLF = 36,
		SHADE = 37,
		BISHOP = 38,
		MINOR_DAIMYO = 39,
		LVL_5_MAGE = 40,
		LVL_4_THIEF = 41,
		KILLER_WOLF = 42,
		SPIRIT = 43,
		GIANT_SPIDER = 44,
		WERETIGER = 45,
		MEDUSALIZARD = 46,
		LVL_5_PRIEST = 47,
		LVL_6_NINJA = 48,
		LVL_7_MAGE_A = 49,
		MASTER_THIEF_A = 50,
		MAJOR_DAIMYO = 51,
		HIGH_PRIEST_A = 52,
		CHAMP_SAMURAI = 53,
		ARCH_MAGE_A = 54,
		MASTER_THIEF_B = 55,
		GAZE_HOUND = 56,
		OGRE_LORD = 57,
		TROLL = 58,
		LIFESTEALER = 59,
		NIGHTSTALKER = 60,
		WYVERN = 61,
		LVL_8_PRIEST = 62,
		LVL_10_FIGHTER = 63,
		LVL_7_MAGE_B = 64,
		LVL_7_THIEF = 65,
		LVL_8_NINJA = 66,
		EARTH_GIANT = 67,
		LESSER_DEMON = 68,
		CHIMERA = 69,
		FIRE_GIANT = 70,
		GORGON = 71,
		LVL_8_BISHOP = 72,
		LVL_8_FIGHTER = 73,
		LVL_10_MAGE = 74,
		THIEF = 75,
		MASTER_NINJA = 76,
		MURPHYS_GHOST = 77,
		WILL_O_WISP = 78,
		BLEEB = 79,
		FROST_GIANT = 80,
		FIRE_DRAGON = 81,
		HIGH_PRIEST_B = 82,
		HIGH_WIZARD = 83,
		MASTER_THIEF_C = 84,
		HATAMOTO = 85,
		VAMPIRE = 86,
		GREATER_DEMON = 87,
		POISON_GIANT = 88,
		DRAGON_ZOMBIE = 89,
		RAVER_LORD = 90,
		THE_HIGH_MASTER = 91,
		FLACK = 92,
		ARCH_MAGE_B = 93,
		MAELIFIC = 94,
		VAMPIRE_LORD = 95,
		WERDNA = 96,
		HIGH_NINJA = 97,
		HIGH_PRIEST_C = 98,
		LVL_7_MAGE_C_ = 99,
		LVL_7_FIGHTER = 100
	};

	enum class Breath {
		NO_BREATH_WEAPON,
		COLD_BREATH,
		DRAIN_BREATH,
		FLAME_BREATH,
		POISON_BREATH,
		STONE_BREATH
	};

	enum class Resistance {
		RESIST_COLD = 0,
		RESIST_FIRE = 1,
		RESIST_MAGIC = 2,
		RESIST_PHYSICAL = 3,
		RESIST_LEVEL_DRAIN = 4,
		RESIST_POISON = 5,
		RESIST_STONING = 6
	};

	enum class Property {
		CAN_AUTOKILL = 0,
		CAN_BE_SLEPT = 1,
		CAN_CALL_FOR_OTHERS = 2,
		CAN_FLEE = 3,
		CAN_PARALYSE = 4,
		CAN_PETRIFY = 5,
		CAN_POISON = 6
	};
}

namespace Enums::Items {

	enum class Category {
		NO_ITEM_CATEGORY = 0,
		WEAPON = 1,
		ARMOUR = 2,
		SHIELD = 3,
		HELMET = 4,
		GAUNTLETS = 5,
		MISCELLANEOUS = 6,
		SCROLL = 7,
		POTION = 8,
		SPECIAL_ITEM = 9
	};

	// Following the original numbering at http://www.pekori.jp/~emonoya/item/index.html for the
	// sake of convenience and ease of replicating original item tables
	enum class TypeID {
		BROKEN_ITEM = 0,
		LONG_SWORD = 1,
		SHORT_SWORD = 2,
		ANOINTED_MACE = 3,
		ANOINTED_FLAIL = 4,
		STAFF = 5,
		DAGGER = 6,
		SMALL_SHIELD = 7,
		LARGE_SHIELD = 8,
		ROBES = 9,
		LEATHER_ARMOR = 10,
		CHAIN_MAIL = 11,
		BREAST_PLATE = 12,
		PLATE_MAIL = 13,
		HELM = 14,
		POTION_OF_DIOS = 15,
		POTION_OF_LATUMOFIS = 16,
		LONG_SWORD_PLUS_1 = 17,
		SHORT_SWORD_PLUS_1 = 18,
		MACE_PLUS_1 = 19,
		STAFF_OF_MOGREF = 20,
		SCROLL_OF_KATINO = 21,
		LEATHER_PLUS_1 = 22,
		CHAIN_MAIL_PLUS_1 = 23,
		PLATE_MAIL_PLUS_1 = 24,
		SHIELD_PLUS_1 = 25,
		BREAST_PLATE_PLUS_1 = 26,
		SCROLL_OF_BADIOS = 27,
		SCROLL_OF_HALITO = 28,
		LONG_SWORD_MINUS_1 = 29,
		SHORT_SWORD_MINUS_1 = 30,
		MACE_MINUS_1 = 31,
		STAFF_PLUS_2 = 32,
		SLAYER_OF_DRAGONS = 33,
		HELM_PLUS_1 = 34,
		LEATHER_MINUS_1 = 35,
		CHAIN_MINUS_1 = 36,
		BREAST_PLATE_MINUS_1 = 37,
		SHIELD_MINUS_1 = 38,
		AMULET_OF_JEWELS = 39,
		// No, I don't know why this is duplicated either, but its in the original tables
		SCROLL_OF_BADIOS_2 = 40,
		POTION_OF_SOPIC = 41,
		LONG_SWORD_PLUS_2 = 42,
		SHORT_SWORD_PLUS_2 = 43,
		MACE_PLUS_2 = 44,
		SCROLL_OF_LOMILWA = 45,
		SCROLL_OF_DILTO = 46,
		GLOVES_OF_COPPER = 47,
		LEATHER_PLUS_2 = 48,
		CHAIN_PLUS_2 = 49,
		PLATE_MAIL_PLUS_2 = 50,
		SHIELD_PLUS_2 = 51,
		HELM_PLUS_2_EVIL = 52,
		POTION_OF_DIAL = 53,
		RING_OF_PORFIC = 54,
		WERE_SLAYER = 55,
		MAGE_MASHER = 56,
		MACE_OF_POISON = 57,
		STAFF_OF_MONTINO = 58,
		BLADE_CUSINART = 59,
		AMULET_OF_MANIFO = 60,
		ROD_OF_FLAME = 61,
		CHAIN_PLUS_2_EVIL = 62,
		PLATE_PLUS_2_NEUTRAL = 63,
		SHIELD_PLUS_3_EVIL = 64,
		AMULET_OF_MAKANITO = 65,
		HELM_OF_MALOR = 66,
		SCROLL_OF_BADIAL = 67,
		SHORT_SWORD_MINUS_2 = 68,
		DAGGER_PLUS_2 = 69,
		MACE_MINUS_2 = 70,
		STAFF_MINUS_2 = 71,
		DAGGER_OF_SPEED = 72,
		ROBE_OF_CURSES = 73,
		LEATHER_MINUS_2 = 74,
		CHAIN_MINUS_2 = 75,
		BREAST_PLATE_MINUS_2 = 76,
		SHIELD_MINUS_2 = 77,
		HELM_OF_CURSES = 78,
		BREAST_PLATE_PLUS_2 = 79,
		GLOVES_OF_SILVER = 80,
		SWORD_PLUS_3_EVIL = 81,
		S_MINUS_SWORD_PLUS_3_EVIL = 82,
		DAGGER_OF_THIEVES = 83,
		BREAST_PLATE_PLUS_3 = 84,
		GARB_OF_LORDS = 85,
		MURAMASA_BLADE = 86,
		SHURIKENS = 87,
		COLD_CHAIN_MAIL = 88,
		PLATE_PLUS_3_EVIL = 89,
		SHIELD_PLUS_3 = 90,
		RING_OF_HEALING = 91,
		PRIESTS_RING = 92,
		RING_OF_DEATH = 93,
		AMULET_OF_WERDNA = 94,
		STATUE_OF_BEAR = 95,
		STATUE_OF_FROG = 96,
		KEY_OF_BRONZE = 97,
		KEY_OF_SILVER = 98,
		KEY_OF_GOLD = 99,
		BLUE_RIBBON = 100
	};

	// From Snarfaru's updated list (https://www.zimlab.com/wizardry/walk/wizardry-1-items-list.htm)
	namespace Effects {

		enum class Defensive {
			NO_DEF_EFFECT = 0,
			// Protection effects mean that if a player is targed by a creature of that type,
			// then there is a 50% chance that the creature will skip its attack
			// (see https://datadrivengamer.blogspot.com/2019/08/the-treasury-of-wizardry.html)
			PROTECTION_VS_ANIMAL = 1,
			PROTECTION_VS_CLERIC = 2,
			PROTECTION_VS_DEMON = 3,
			PROTECTION_VS_DRAGON = 4,
			PROTECTION_VS_ENCHANTED = 5,
			PROTECTION_VS_FIGHTER = 6,
			PROTECTION_VS_GIANT = 7,
			PROTECTION_VS_INSECT = 8,
			PROTECTION_VS_MAGE = 9,
			PROTECTION_VS_MYTHICAL = 10,
			PROTECTION_VS_THIEF = 11,
			PROTECTION_VS_UNDEAD = 12,
			PROTECTION_VS_WERE = 13,
			// Breath Resistances work as follows ("DOBREATH" in the original code):
			// There is a Resistance vs Breath Attacks chance of halving the incoming damage
			// If a character is wearing an item with the appropriate Breath Resistance, then
			// any incoming damage is halved again - so it is possible for a character to
			// suffer only 1/4 damage on a successful Saving Throw + Wearing an appropriate item
			RESIST_COLD = 14,
			RESIST_FIRE = 15,
			RESIST_MAGIC = 16,
			// Immunity to critical hits and paralysis
			RESIST_PHYSICAL = 17,
			// Immunity to theese effects ("DRAINLEV")
			RESIST_LEVEL_DRAIN = 18,
			RESIST_POISON = 19,
			RESIST_STONING = 20,
			// Immunity to decapitation (how is this different from RESIST_PHYSICAL?)
			PREVENT_DECAPITATION = 21
		};

		enum class Offensive {
			// Double Damage versus these types of enemies
			NO_OFF_EFFECT = 0,
			PURPOSED_VS_ANIMAL = 1,
			PURPOSED_VS_CLERIC = 2,
			PURPOSED_VS_DEMON = 3,
			PURPOSED_VS_DRAGON = 4,
			PURPOSED_VS_ENCHANTED = 5,
			PURPOSED_VS_FIGHTER = 6,
			PURPOSED_VS_GIANT = 7,
			PURPOSED_VS_INSECT = 8,
			PURPOSED_VS_MAGE = 9,
			PURPOSED_VS_MYTHICAL = 10,
			PURPOSED_VS_THIEF = 11,
			PURPOSED_VS_UNDEAD = 12,
			PURPOSED_VS_WERE = 13,
			// On a critical hit, this kills the enemy regardless of type
			AUTOKILL = 14
		};

		enum class Invoke {
			NO_INV_EFFECT = 0,
			BECOME_NINJA = 1,
			HEAL_ALL = 2,
			INC_HP = 3,
			INC_STRENGTH = 4,
			// These ones are possible in Wiz 1 but never used according to Snafaru
			AGE_BY_A_YEAR = 5,
			BECOME_LORD = 6,
			BECOME_SAMURAI = 7,
			GRANT_50000_EXP = 8,
			GRANT_50000_GOLD = 9,
			STATUS_TO_LOST = 10
		};
	};
}

using ConfigOption = Enums::Config::Options;
using ItemCategory = Enums::Items::Category;
using ItemDef = Enums::Items::Effects::Defensive;
using ItemOff = Enums::Items::Effects::Offensive;
using ItemInv = Enums::Items::Effects::Invoke;
using ItemTypeID = Enums::Items::TypeID;
using MonsterBreath = Enums::Monsters::Breath;
using MonsterClass = Enums::Monsters::Class;
using MonsterCategory = Enums::Monsters::Category;
using MonsterResist = Enums::Monsters::Resistance;
using MonsterResistances = std::array<bool, 7>;
using MonsterProperty = Enums::Monsters::Property;
using MonsterProperties = std::array<bool, 7>;
using MonsterTypeID = Enums::Monsters::TypeID;

}