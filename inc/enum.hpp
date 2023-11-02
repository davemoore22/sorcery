// Copyright (C) 2023 Dave Moore
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

// Enums
namespace Sorcery {

namespace Enums::Controls {

	enum class Input {
		NO_INPUT,
		ANYTHING,
		UP,
		DOWN,
		LEFT,
		RIGHT,
		MOVE,
		CONFIRM,
		CONFIRM_NO_SPACE,
		CANCEL,
		YES,
		NO,
		ALPHANUMERIC,
		PAGE_UP,
		PAGE_DOWN,
		HOME,
		END,
		DELETE,
		SPACE,
		BACK,
		SELECT,
		SHOW_CONTROLS,
		MAZE_CHARACTERS,
		MAZE_FORWARD,
		MAZE_BACKWARD,
		MAZE_LEFT,
		MAZE_RIGHT,
		MAZE_TURN_AROUND,
		MAZE_CAMP,
		MAZE_ACTION,
		MAZE_SEARCH,
		MAZE_INSPECT,
		MAZE_GUI_TOGGLE,
		MAZE_STATUSBAR_TOGGLE,
		MAZE_QUIT,
		SHOW_HIDE_CONSOLE,
		MAZE_CHARACTER_1,
		MAZE_CHARACTER_2,
		MAZE_CHARACTER_3,
		MAZE_CHARACTER_4,
		MAZE_CHARACTER_5,
		MAZE_CHARACTER_6,
		MAZE_SHOW_MAP
	};
}

namespace Enums::Window {

	// Draw Mode
	enum class DrawMode {
		NO_DRAWMODE,
		MANUAL,
		AUTOMATIC
	};

	enum class Alignment {
		NO_ALIGNMENT,
		VERTICAL,
		HORIZONTAL
	};

	// Input Mode
	enum class InputMode {
		NO_INPUTMODE,
		ATTRACT_MODE,
		NAVIGATE_MENU,
		IN_GAME,
		DISPLAY_TEXT_FILE,
		GAME_OPTIONS,
		COMPENDIUM,
		CONFIRM_QUIT_GAME,
		SAVE_CHANGES,
		CANCEL_CHANGES,
		CONFIRM_STRICT_MODE,
		CONFIRM_LEAVE_GAME,
		CONFIRM_NEW_GAME,
		SHOW_OUCH,
		INPUT_NAME,
		ALLOCATE_STATS,
		CHOOSE_METHOD,
		CHOOSE_PORTRAIT,
		REVIEW_AND_CONFIRM,
		BROWSE_CHARACTER,
		CONFIRM_DELETE_CHARACTER,
		CONFIRM_CHANGE_CHARACTER_CLASS,
		CONFIRM_LEGATE
	};

	enum class InputCategory {
		MOUSE_MOVE = 0,
		UP = 1,
		DOWN = 2,
		LEFT = 3,
		RIGHT = 4,
		CONFIRM = 5,
		YES_NO = 6,
		ALPHANUMERIC = 7,
		PAGE_UP = 8,
		PAGE_DOWN = 9,
		HOME = 10,
		END = 11,
		BACK_DELETE_CANCEL = 12,
		ESCAPE = 13,
		HELP = 14,
		SHOW_CHARACTER = 15,
		MAZE_FORWARD = 16,
		MAZE_BACKWARD = 17,
		MAZE_LEFT = 18,
		MAZE_RIGHT = 19,
		MAZE_TURN_AROUND = 20,
		MAZE_CAMP = 21,
		MAZE_ACTION = 22,
		MAZE_SEARCH = 23,
		MAZE_INSPECT = 24,
		MAZE_GUI_TOGGLE = 25,
		MAZE_STATUSBAR_TOGGLE = 26,
		MAZE_QUIT = 27
	};

	enum class Confirm {
		NO_CONFIRM,
		YES,
		NO
	};

	enum class DialogButton {
		NO_BUTTON,
		OK,
		YES,
		NO,
		CLOSE
	};

	enum class ComponentType {
		NO_COMPONENT_TYPE,
		IMAGE,
		TEXT,
		MENU,
		FRAME,
		ICON,
		DIALOG,
		UNKNOWN
	};

	enum class ComponentElement {
		NO_ELEMENT = -1,
		FONT = 1,
		SIZE = 2,
		COLOUR = 4,
		STRING = 8,
		OFFSET = 16,
		ORIGIN = 32,
		O_COLOUR = 64,
		O_THICKNESS = 128,
		JUSTIFICATION = 256
	};

	enum class DialogType {
		NO_DIALOG_TYPE,
		TIMED,
		OK,
		CONFIRM,
		MENU
	};

	enum class Option {
		SAVE,
		CANCEL,
		RESET,
		NONE
	};

	enum class Justification {
		NO_JUSTIFY,
		LEFT,
		RIGHT,
		CENTRE
	};

	enum class FrameParts {
		NO_PART = -1,
		TOP_LEFT = 0,
		TOP = 1,
		TOP_RIGHT = 2,
		LEFT = 3,
		BOTTOM_LEFT = 4,
		BOTTOM = 5,
		BOTTOM_RIGHT = 6,
		RIGHT = 7
	};
}

namespace Enums::Graphics {

	enum class Event {
		NO_IMAGE = -1,
		TREASURE = 0,
		GRAVESTONE = 1,
		CHEST = 2
	};

	enum class Texture {
		NO_TEXTURE,
		AUTOMAP,
		BACKGROUND,
		BANNER,
		CONTROLS,
		CREATURES_KNOWN,
		CREATURES_UNKNOWN,
		DOORS,
		EVENTS,
		FLOORS,
		ICONS,
		ITEMS,
		LOGO,
		PORTRAITS,
		SPLASH,
		TOWN,
		UI,
		VIEW,
		WALLS,
		WIREFRAME
	};

	enum class TextureType {
		NO_TEXTURE_TYPE,
		AUTOMAP,
		WALL,
		FLOOR,
		CEILING,
		DOOR,
		KNOWN_CREATURE,
		UNKNOWN_CREATURE,
		PORTRAIT,
		EVENTS,
		ITEMS,
		WALLPAPER
	};
}

namespace Enums::Internal {

	enum class FontType {
		NO_FONT,
		MONOSPACE,
		PROPORTIONAL,
		TEXT,
		INPUT
	};

	enum class MessageType {
		NO_MESSAGE_TYPE,
		STANDARD,
		ALERT,
		WARNING,
		ERROR,
		ROLL,
		GAME
	};

}

namespace Enums::Magic {

	enum class SpellType {
		NO_SPELL_TYPE,
		MAGE,
		PRIEST
	};

	enum class SpellPointStatus {
		NO_STATUS,
		CURRENT,
		MAXIMUM
	};

	enum class SpellCategory {
		NO_CATEGORY,
		HEALING,
		FIELD,
		SUPPORT,
		DISABLE,
		ATTACK
	};

	// Wizardry 1 to 3 Spells
	enum class SpellID {
		NO_SPELL = -1,
		DUMAPIC = 0,	// Mage 1
		HALITO = 1,		// Mage 1
		KATINO = 2,		// Mage 1
		MOGREF = 3,		// Mage 1
		DILTO = 4,		// Mage 2
		SOPIC = 5,		// Mage 2
		MAHALITO = 6,	// Mage 3
		MOLITO = 7,		// Mage 3
		DALTO = 8,		// Mage 4
		LAHALITO = 9,	// Mage 4
		MORLIS = 10,	// Mage 4
		MADALTO = 11,	// Mage 5
		MAKANITO = 12,	// Mage 5
		MAMORLIS = 13,	// Mage 5
		HAMAN = 14,		// Mage 6
		LAKANITO = 15,	// Mage 6
		MASOPIC = 16,	// Mage 6
		ZILWAN = 17,	// Mage 6
		MAHAMAN = 18,	// Mage 7
		MALOR = 19,		// Mage 7
		TILTOWAIT = 20, // Mage 7
		END_OF_MAGE = 21,
		BADIOS = 22,	// Priest 1
		DIOS = 23,		// Priest 1
		KALKI = 24,		// Priest 1
		MILWA = 25,		// Priest 1
		PORFIC = 26,	// Priest 1
		CALFO = 27,		// Priest 2
		MANIFO = 28,	// Priest 2
		MATU = 29,		// Priest 2
		MONTINO = 30,	// Priest 2
		BAMATU = 31,	// Priest 3
		DIALKO = 32,	// Priest 3
		LATUMAPIC = 33, // Priest 3
		LOMILWA = 34,	// Priest 3
		BADIAL = 35,	// Priest 4
		DIAL = 36,		// Priest 4
		LATUMOFIS = 37, // Priest 4
		MAPORFIC = 38,	// Priest 4
		BADI = 39,		// Priest 5
		BADIALMA = 40,	// Priest 5
		DI = 41,		// Priest 5
		DIALMA = 42,	// Priest 5
		KANDI = 43,		// Priest 5
		LITOKAN = 44,	// Priest 5
		LOKTOFEIT = 45, // Priest 6
		LORTO = 46,		// Priest 6
		MABADI = 47,	// Priest 6
		MADI = 48,		// Priest 6
		KADORTO = 49,	// Priest 7
		MALIKTO = 50,	// Priest 7
		END_OF_PRIEST = 51
	};
}

// Management Menu
namespace Enums::Manage {

	enum class Roster {
		NO_MODE,
		INSPECT,
		EDIT,
		DELETE
	};

	enum class Method {
		FULL = 0,
		QUICK = 1,
		RANDOM = 2
	};
}

namespace Enums::Castle {

	namespace Rest {

		enum class Mode {
			SINGLE,
			ALL
		};

		enum class Stage {
			REGEN,
			RESULTS
		};

		enum class Type {
			STABLES,
			COT,
			ECONOMY,
			MERCHANT,
			ROYAL
		};

	}

	enum class Tavern {
		NO_STAGE,
		MENU,
		ADD,
		REMOVE,
		REORDER,
		INSPECT
	};

	enum class Inn {
		NO_STAGE,
		MENU,
		BED,
		INPECT,
		CHOOSE
	};

	enum class Shop {
		NO_STAGE,
		MENU,
		WHO,
		ACTION,
		UNCURSE,
		IDENTIFY,
		INSPECT
	};

	enum class Temple {
		NO_STAGE,
		MENU,
		HELP,
		PAY,
		RESS,
		TITHE,
		INSPECT
	};
}

// Character Stuff
namespace Enums::Character {

	enum class Location {
		NO_LOCATION = -1,
		PARTY = 0,
		TAVERN = 1,
		TEMPLE = 2,
		MAZE = 3,
		TRAINING = 4
	};

	// Affects what options are available in the character menu
	enum class Mode {
		NO_MODE = -1,
		IN_MAZE,
		AT_CASTLE,
		AT_TRAINING,
		IN_CREATE
	};

	// State of Character Creation
	enum class Stage {
		NOT_STARTED,
		CHOOSE_METHOD,
		CHOOSE_RACE,
		CHOOSE_ALIGNMENT,
		ALLOCATE_STATS,
		CHOOSE_CLASS,
		CHOOSE_PORTRAIT,
		REVIEW_AND_CONFIRM,
		ENTER_NAME,
		FINISH_AND_SAVE,
		COMPLETED
	};

	enum class Legate {
		NO_STAGE,
		CONFIRM,
		CHANGE_ALIGNMENT,
		LEGATED
	};

	// View Sections
	enum class View {
		NO_VIEW = 0,
		SUMMARY = 1,
		DETAILED = 2,
		RESISTANCES = 3,
		MAGE_SPELLS = 4,
		PRIEST_SPELLS = 5,
		// MESSAGES = 6,
		// ACHIEVEMENTS = 7
	};

	// Character Classes
	enum class Race {
		NO_RACE,
		HUMAN,
		ELF,
		DWARF,
		GNOME,
		HOBBIT
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

	enum class Align {
		NO_ALIGN = 0,
		GOOD = 1,
		NEUTRAL = 2,
		EVIL = 3
	};

	enum class CStatus {
		OK = 0,
		AFRAID = 1,
		SILENCED = 2,
		ASLEEP = 3,
		HELD = 4,
		STONED = 5,
		DEAD = 6,
		ASHES = 7,
		LOST = 8
	};

	enum class Attribute {
		NO_ATTRIBUTE = 0,
		STRENGTH = 1,
		IQ = 2,
		PIETY = 3,
		VITALITY = 4,
		AGILITY = 5,
		LUCK = 6
	};

	enum class Ability_Type {
		NO_ABILITY_TYPE,
		NUMBER,
		MODIFIER,
		PERCENTAGE,
		STAT,
		AC,
		STATUS
	};

	enum class Ability {
		NO_ABILITY,
		CURRENT_HP,
		MAX_HP,
		HIT_DICE,
		CURRENT_LEVEL,
		MAX_LEVEL,
		NEGATIVE_LEVEL,
		GOLD,
		CURRENT_XP,
		NEXT_LEVEL_XP,
		AGE,
		SWIM,
		MARKS,
		DEATHS,
		BASE_ARMOUR_CLASS,
		CURRENT_ARMOUR_CLASS,
		UNARMED_DAMAGE,
		ATTACK_MODIFIER,
		HIT_PROBABILITY,
		BONUS_DAMAGE,
		BASE_NUMBER_OF_ATTACKS,
		MAGE_SPELL_LEARN,
		IDENTIFY_ITEMS,
		IDENTIFY_CURSE,
		IDENTIFY_FOES,
		BONUS_MAGE_SPELLS,
		PRIEST_SPELL_LEARN,
		LOKTOFELT_SUCCESS,
		BASE_DISPELL,
		BONUS_PRIEST_SPELLS,
		VITALITY_BONUS,
		BONUS_HIT_POINTS,
		DEAD_RESURRECT,
		ASHES_RESURRECT,
		DI_KADORTO_RESURRECT,
		INITIATIVE_MODIFIER,
		BASE_CRITICAL_HIT,
		IDENTIFY_TRAP,
		BASE_DISARM_TRAP,
		ACTIVATE_TRAP,
		BASE_AVOID_PIT,
		BASE_RESIST_BONUS,
		EQUIPMENT_INTACT_ON_WIPE,
		RECOVER_FROM_SLEEP,
		RECOVER_FROM_FEAR,
		RESISTANCE_VS_CRITICAL_HIT,
		RESISTANCE_VS_POISON_PARALYSIS,
		RESISTANCE_VS_STONING,
		RESISTANCE_VS_BREATH_ATTACKS,
		RESISTANCE_VS_POISON_GAS_TRAP,
		RESISTANCE_VS_MAGE_PRIEST_TRAP,
		RESISTANCE_VS_SILENCE,
		RESISTANCE_VS_KATINO,
		RESISTANCE_VS_BADI,
		RESISTANCE_VS_MANIFO,
		HP_LOSS_PER_TURN,
		HP_GAIN_PER_TURN,
		POISON_STRENGTH
	};
}

// System
namespace Enums::System {

	// Quick Start
	enum class Destination {
		DEFAULT = 0,
		CASTLE = 1,
		TAVERN = 2,
		TRAINING = 3,
		INN = 4,
		TEMPLE = 6,
		SHOP = 7,
		MAZE = 8
	};

	// Errors
	enum class Error {
		NO_ERROR = 0,
		JSON_PARSE_ERROR = 1,
		UNKNOWN_STRING_KEY = 2,
		UNKNOWN_COMPONENT = 3,
		SQLLITE_ERROR = 4,
		OPTIONAL_RETURNED = 5,
		UNHANDLED_EXCEPTION = 6
	};

	// Return values
	enum class ModuleResult {
		NO_RESULT,
		CLOSE,
		BACK,
		EXIT,
		NEXT,
		CANCEL,
		DELETE
	};

	// Random (Dice) Types
	enum class Random {
		NO_DICE = 1,
		D2 = 2,
		D3 = 3,
		D4 = 4,
		D5 = 5,
		D6 = 6,
		D7 = 7,
		D8 = 8,
		D9 = 9,
		D10 = 10,
		D12 = 12,
		D15,
		D16,
		D52,
		D100,
		D165,
		D300,
		D1000,
		D2000,
		ZERO_TO_2,
		ZERO_TO_3,
		ZERO_TO_4,
		ZERO_TO_7,
		ZERO_TO_8,
		ZERO_TO_14,
		ZERO_TO_19,
		ZERO_TO_29,
		ZERO_TO_34,
		ZERO_TO_69,
		ZERO_TO_99,
		ZERO_TO_100,
		ZERO_TO_101,
		ZERO_TO_129,
		ZERO_TO_299,
		ZERO_TO_399,
		ZERO_TO_437
	};
}

namespace Enums::MainMenu {

	enum class Type {
		ATTRACT_MODE,
		ATTRACT_MENU
	};
}

// Menu
namespace Enums::Menu {

	// Menu Types
	enum class Type {
		MAIN,
		OPTIONS,
		PAUSE,
		CASTLE,
		EDGE_OF_TOWN,
		TAVERN,
		INN,
		SHOP,
		TEMPLE,
		TRAINING_GROUNDS,
		CHOOSE_CHARACTER_RACE,
		CHOOSE_CHARACTER_ALIGNMENT,
		ALLOCATE_CHARACTER_ATTRIBUTES,
		CHOOSE_CHARACTER_CLASS,
		CHOOSE_CHARACTER_PORTRAIT,
		REVIEW_AND_CONFIRM,
		CHOOSE_METHOD,
		CHARACTER_ROSTER,
		EDIT_CHARACTER,
		CHANGE_CHARACTER_CLASS,
		PARTY_CHARACTERS,
		AVAILABLE_CHARACTERS,
		INVALID_CHARACTERS,
		PARTY_CHARACTER_NAMES,
		TEMPLE_CHARACTERS,
		BUY_SELL_CHARACTERS,
		CURSED_CHARACTERS,
		IDENTIFY_CHARACTERS,
		CAMP,
		SEARCH,
		ACTION,
		ELEVATOR_A_D,
		ELEVATOR_A_F,
		RESTART_EXPEDITION,
		CHARACTERS_HERE,
		INN_CHOOSE_CHARACTER,
		INN_CHOOSE_BED,
		CONTINUE,
		STOP,
		GRAVEYARD,
		COMPENDIUM,
		MUSEUM,
		SHOP_ACTION
	};

	enum class Mode {
		NO_MODE,
		TAVERN,
		TEMPLE,
		INN,
		SHOP,
		TRAINING,
		CAMP,
		ACTION,
		SEARCH
	};

	// Menu Items
	enum class Item {
		NO_MENU_ITEM,
		ITEM_ABORT,
		ITEM_CANCEL,
		GO_BACK,
		GO_NEXT,
		ITEM_RETURN,
		ITEM_SAVE,
		MM_NEW_GAME,
		MM_CONTINUE_GAME,
		MM_OPTIONS,
		MM_COMPENDIUM,
		MM_LICENSE,
		ITEM_QUIT,
		ITEM_SEPARATOR,
		ITEM_TITLE,
		ITEM_SPACER,
		NC_WARNING,
		OP_AUTO_SAVE,
		OP_STRICT_MODE,
		OP_RECOMMENDED_MODE,
		OP_CHEAT_MODE,
		OP_DICE_ROLLS,
		OP_WIREFRAME_MODE,
		OP_DISPLAY_TEXTURES,
		OP_DISPLAY_TRAPS,
		OP_DISPLAY_TELEPORTERS,
		OP_DISPLAY_ENCOUNTERS,
		OP_DISPLAY_PROGRESS,
		OP_ALLOW_MIXED_ALIGNMENT_PARTY,
		OP_STAT_LOSS_ON_LEVEL_UP,
		OP_REROLL_HIT_POINTS_ON_LEVEL_GAIN,
		OP_STAT_RESET_ON_CLASS_CHANGE,
		OP_AGING_ON_CLASS_CHANGE,
		OP_PROTECT_TELEPORT,
		OP_ALLOW_RANGED_WEAPONS,
		OP_SPELLCASTING_IN_SURPRISE_ROUND,
		OP_BATCH_HEALING_AFTER_RETURN_TO_CASTLE,
		OP_REROLL_ONES_ON_DICE,
		OP_ALLOW_LOST_LEGATED,
		OP_ALLOW_CURABLE_DRAIN,
		OP_ALLOW_SHARED_INVENTORY,
		OP_CAMPING_RESTORE_SPELL_POINTS,
		OP_CHARGED_ITEMS,
		OP_ALL,
		CR_RANDOM,
		CR_HUMAN,
		CR_ELF,
		CR_DWARF,
		CR_GNOME,
		CR_HOBBIT,
		CR_QUIT,
		CA_GOOD,
		CA_NEUTRAL,
		CA_EVIL,
		CS_STRENGTH,
		CS_IQ,
		CS_PIETY,
		CS_VITALITY,
		CS_AGILITY,
		CS_LUCK,
		CS_POINTS_TO_ALLOCATE,
		CC_FIGHTER,
		CC_MAGE,
		CC_PRIEST,
		CC_THIEF,
		CC_BISHOP,
		CC_SAMURAI,
		CC_LORD,
		CC_NINJA,
		CP_PREVIOUS,
		CP_NEXT,
		FINISH_AND_SAVE,
		CA_TAVERN,
		CA_INN,
		CA_SHOP,
		CA_TEMPLE,
		CA_EDGE_OF_TOWN,
		ITEM_CAMP,
		ET_CASTLE,
		ET_TRAIN,
		ET_MAZE,
		ET_LEAVE_GAME,
		ET_RESTART,
		TR_CREATE,
		TR_EDIT,
		TR_DELETE,
		TR_INSPECT,
		TR_EDGE_OF_TOWN,
		RC_ACCEPT,
		RC_REJECT,
		RC_CANCEL,
		CM_FULL,
		CM_QUICK,
		CM_RANDOM,
		IC_CHARACTER,
		EC_CHANGE_CLASS,
		EC_CHANGE_NAME,
		EC_IMPORT_CHARACTER,
		EC_LEGATE_CHARACTER,
		EC_RETURN_EDIT,
		TA_ADD_TO_PARTY,
		TA_REMOVE_FROM_PARTY,
		REORDER_PARTY,
		TA_INSPECT,
		TA_REORDER,
		TA_DIVVY_GOLD,
		TA_CASTLE,
		IN_STAY_CHARACTER,
		IN_STAY_PARTY,
		IN_INSPECT,
		IN_CASTLE,
		SH_BUY_AND_SELL,
		SH_UNCURSE,
		SH_IDENTIFY,
		SH_INSPECT,
		SH_CASTLE,
		SH_BUY,
		SH_SELL,
		SH_POOL_GOLD,
		SH_BACK,
		TE_HELP,
		TE_TITHE,
		TE_INSPECT,
		TE_CASTLE,
		CP_INSPECT,
		CP_REORDER,
		CP_SAVE,
		CP_OPTIONS,
		CP_LEAVE,
		EL_A,
		EL_B,
		EL_C,
		EL_D,
		EL_E,
		EL_F,
		EL_LEAVE,
		AC_SEARCH_CHARACTERS,
		AC_SEARCH_ITEMS,
		AC_SEARCH_SECRET,
		AC_CAST_SPELL,
		AC_USE_ITEM,
		AC_LEAVE,
		AC_SHOW_MAP,
		IN_STABLES,
		IN_COT,
		IN_ECONOMY,
		IN_MERCHANT,
		IN_ROYAL,
		IN_POOL_GOLD,
		IN_BACK,
		ITEM_STOP,
		ITEM_CONTINUE,
		C_ACTION_TRADE,
		C_ACTION_IDENTIFY,
		C_ACTION_POOL,
		C_ACTION_EQUIP,
		C_ACTION_DROP,
		C_ACTION_SPELL,
		C_ACTION_USE,
		C_ACTION_LEAVE,
		C_ACTION_READ,
		C_ACTION_NEXT,
		CO_MONSTERS,
		CO_ITEMS,
		CO_SPELLS,
		CO_FEATURES,
		CO_STATS,
		MU_ITEM
	};

	// Menu Items
	enum class ItemType {
		NO_MENU_ITEM_TYPE,
		ENTRY,
		TITLE,
		SEPARATOR,
		SPACER,
		TEXT,
		SAVE,
		CANCEL
	};
}

namespace Enums {

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

namespace Enums {

	namespace Map {

		enum class Direction {
			NO_DIRECTION = -1,
			NORTH = 0,
			EAST = 1,
			SOUTH = 2,
			WEST = 3
		};

		enum class Type {
			NO_MAP = -1,
			EMPTY = 0,
			START = 1,
			TUTORIAL = 2,
			MAZE = 3
		};

		enum class Event {

			NO_EVENT = -1,
			AREA_OF_OUT_BOUNDS = 0,
			MAN_TELEPORT_CASTLE = 1,
			SILVER_KEY = 2,
			BRONZE_KEY = 3,
			MURPHYS_GHOSTS = 4,
			BEAR_STATUE = 5,
			FROG_STATUE = 6,
			GOLD_KEY = 7,
			NEED_SILVER_KEY = 8,
			NEED_BRONZE_KEY = 9,
			NEED_BEAR_STATUE = 10,
			NEED_FROG_STATUE = 11,
			PLACARD_PIT_1 = 12,
			PLACARD_PIT_2 = 13,
			PLACARD_PIT_3 = 14,
			TURN_AROUND = 15,
			TURN_LEFT = 16,
			TURN_RIGHT = 17,
			NEED_BEAR_STATUE_2 = 18,
			TESTING_GROUNDS = 19,
			ALARM_BELLS = 20,
			TREASURE_REPOSITORY = 21,
			MONSTER_ALLOCATION_CENTRE = 22,
			LARGE_DESK = 23,
			TREBOR_VOICE = 24,
			SERVICE_ELEVATOR = 25,
			WERDNA_BOAST = 26,
			TURN_BACK = 27,
			WERDNA_SIGN = 28,
			THREE_HUMANOIDS = 29,
			GETTING_WARM_1 = 30,
			GETTING_WARM_2 = 31,
			GETTING_WARM_3 = 32,
			FIRE_DRAGONS_COMBAT = 33,
			DEADLY_RING_COMBAT = 34,
			WERDNA_COMBAT = 35,
			GUARANTEED_COMBAT = 36,
			NEED_BLUE_RIBBON = 37
		};

	}

	namespace View {

		namespace Message {

			enum class Position {
				NO_POSITION = -1,
				MIDDLE = 0,
				TOP = 1,
				BOTTOM = 2
			};
		}
	}

	namespace Tile {

		enum class Edge {
			NO_EDGE = 0,
			WALL = 1,
			UNLOCKED_DOOR = 2,
			LOCKED_DOOR = 3,
			HIDDEN_DOOR = 4,
			ONE_WAY_DOOR = 5,
			ONE_WAY_HIDDEN_DOOR = 6,
			ONE_WAY_WALL = 7,
			SECRET_DOOR = 8,
			SECRET_WALL = 9
		};

		enum class DoorType {
			NO_DOOR_TYPE = 0,
			NORMAL = 1,
			LOCKED = 2
		};

		enum class Properties {
			WALKABLE = 0,
			ROCK = 1,
			EXPLORED = 2,
			DARKNESS = 3,
			ANTIMAGIC = 4
		};

		enum class Features {
			NO_FEATURE = 0,
			SPINNER = 1,
			PIT = 2,
			TELEPORT_FROM = 3,
			POOL = 4,
			FOUNTAIN = 5,
			MESSAGE = 6,
			MOVEMENT_NORTH = 7,
			MOVEMENT_SOUTH = 8,
			MOVEMENT_EAST = 9,
			MOVEMENT_WEST = 10,
			STAIRS_UP = 11,
			STAIRS_DOWN = 12,
			LADDER_UP = 13,
			LADDER_DOWN = 14,
			TELEPORT_TO = 15,
			PORTAL = 16,
			ELEVATOR = 17,
			NOTICE = 18,
			ELEVATOR_UP = 19,
			ELEVATOR_DOWN = 20,
			CHUTE = 21,

			// TODO: all the others from Wiz6 because why not?
		};

	}

	namespace Automap {

		enum class Feature {
			NO_FEATURE = -1,
			FLOOR = 0,
			NORTH_WALL = 1,
			SOUTH_WALL = 2,
			EAST_WALL = 3,
			WEST_WALL = 4,
			NORTH_DOOR = 5,
			SOUTH_DOOR = 6,
			EAST_DOOR = 7,
			WEST_DOOR = 8,
			NORTH_SECRET = 9,
			SOUTH_SECRET = 10,
			EAST_SECRET = 11,
			WEST_SECRET = 12,
			DARKNESS = 13,
			STAIRS_DOWN = 14,
			STAIRS_UP = 15,
			CHUTE = 16,
			ELEVATOR = 17,
			ROCK = 18,
			PIT = 19,
			SPINNER = 20,
			TELEPORT_FROM = 21,
			ENCOUNTER = 22,
			ANTI_MAGIC = 23,
			LAIR = 24,
			FOUNTAIN = 25,
			POOL = 26,
			LETTER_C = 27,
			LETTER_T = 28,
			EXCLAMATION = 29,
			QUESTION_MARK = 30,
			NORTH_FORCED = 31,
			SOUTH_FORCED = 32,
			EAST_FORCED = 33,
			WEST_FORCED = 34,
			STAR = 35,
			MINUS_HP = 36,
			PLUS_HP = 37,
			PLAYER_NORTH = 38,
			PLAYER_SOUTH = 39,
			PLAYER_EAST = 40,
			PLAYER_WEST = 41,
			NORTH_ONE_WAY_WALL = 42,
			SOUTH_ONE_WAY_WALL = 43,
			EAST_ONE_WAY_WALL = 44,
			WEST_ONE_WAY_WALL = 45,
			TELEPORT_TO = 46
		};

	}

	namespace Combat {

		enum class Type {
			RANDOM = 0,
			MURPHYS_GHOSTS = 1,
			RING_PARTY = 2,
			DRAOGNS = 3,
			WERDNA = 4
		};

	}

	namespace Creatures {

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
			LVL_7_FIGHTER = 100,
		};
	}

	namespace Items {

		enum class Quest {
			NO_QUEST_ITEM = 0,
			AMULET_OF_WERDNA = 1,
			BEAR_STATUE = 2,
			BLUE_RIBBON = 3,
			BRONZE_KEY = 4,
			FROG_STATUE = 5,
			GOLD_KEY = 6,
			SILVER_KEY = 7
		};

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

		enum class Location {
			NO_LOCATION = 0,
			INVENTORY = 1,
			FLOOR = 2,
			SHOP = 3
		};

		enum class View {

			SUMMARY = 0,
			DETAILED = 1,
			SPECIALS = 2
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
		}
	}
}
}
