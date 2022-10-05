// Copyright (C) 2021 Dave Moore
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
			NONE,
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
		enum class DrawMode { NONE, MANUAL, AUTOMATIC };

		enum class Alignment { NONE, VERTICAL, HORIZONTAL };

		// Input Mode
		enum class InputMode {
			NONE,
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

		enum class Confirm { NONE, YES, NO };

		enum class DialogButton { NONE, OK, YES, NO, CLOSE };

		enum class ComponentType { NONE, IMAGE, TEXT, MENU, FRAME, ICON, DIALOG, UNKNOWN };

		enum class ComponentElement {
			NONE = -1,
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

		enum class DialogType { NONE, TIMED, OK, CONFIRM, MENU };

		enum class Option { SAVE, CANCEL, RESET, NONE };

		enum class Justification { NONE, LEFT, RIGHT, CENTRE };

		enum class FrameType { NONE, HINT, NORMAL };

		enum class FrameParts {
			NONE = -1,
			TOP_LEFT = 0,
			TOP = 1,
			TOP_RIGHT = 2,
			LEFT = 3,
			BOTTOM_LEFT = 4,
			BOTTOM = 5,
			BOTTOM_RIGHT = 6,
			RIGHT = 7
		};
	} // namespace Enums::Window

	namespace Enums::Graphics {

		enum class Background { NONE, CASTLE, TAVERN, INN, SHOP, TEMPLE, EDGE_OF_TOWN, TRAIN };

		enum class Texture {
			NONE,
			AUTOMAP,
			BACKGROUND,
			BANNER,
			CONTROLS,
			CREATURES_KNOWN,
			CREATURES_UNKNOWN,
			DOORS,
			FLOORS,
			ICONS,
			LOGO,
			NINEPATCH,
			PORTRAITS,
			SPLASH,
			TOWN,
			UI,
			VIEW,
			WALLS,
			WIREFRAME
		};

		enum class TextureType {
			NONE,
			AUTOMAP,
			WALL,
			FLOOR,
			CEILING,
			DOOR,
			KNOWN_CREATURE,
			UNKNOWN_CREATURE,
			PORTRAIT
		};
	} // namespace Enums::Graphics

	namespace Enums::Settings {

		// Settings
		enum class Category { NONE, GENERAL, GAMEPLAY, GRAPHICS };
	} // namespace Enums::Settings

	namespace Enums::Internal {

		enum class FontType { NONE, MONOSPACE, PROPORTIONAL, TEXT, INPUT };

		enum class StringType { NONE, NORMAL, EXPLAIN };

		enum class MessageType { NONE, STANDARD, ALERT, WARNING, ERROR };

	} // namespace Enums::Internal

	namespace Enums::Magic {

		enum class SpellType { NONE, MAGE, PRIEST };

		enum class SpellPointStatus { NONE, CURRENT, MAXIMUM };

		enum class SpellCategory { NONE, HEALING, FIELD, SUPPORT, DISABLE, ATTACK };

		// Wizardry 1 to 3 Spells
		enum class SpellID {
			NONE = -1,

			DUMAPIC = 0,
			HALITO = 1,
			KATINO = 2,
			MOGREF = 3,

			DILTO = 4,
			SOPIC = 5,

			MAHALITO = 6,
			MOLITO = 7,

			DALTO = 8,
			LAHALITO = 9,
			MORLIS = 10,

			MADALTO = 11,
			MAKANITO = 12,
			MAMORLIS = 13,

			HAMAN = 14,
			LAKANITO = 15,
			MASOPIC = 16,
			ZILWAN = 17,

			MAHAMAN = 18,
			MALOR = 19,
			TILTOWAIT = 20,

			END_OF_MAGE = 21,

			BADIOS = 22,
			DIOS = 23,
			KALKI = 24,
			MILWA = 25,
			PORFIC = 26,

			CALFO = 27,
			MANIFO = 28,
			MATU = 29,
			MONTINO = 30,

			BAMATU = 31,
			DIALKO = 32,
			LATUMAPIC = 33,
			LOMILWA = 34,

			BADIAL = 35,
			DIAL = 36,
			LATUMOFIS = 37,
			MAPORFIC = 38,

			BADI = 39,
			BADIALMA = 40,
			DI = 41,
			DIALMA = 42,
			KANDI = 43,
			LITOKAN = 44,

			LOKTOFEIT = 45,
			LORTO = 46,
			MABADI = 47,
			MADI = 48,

			KADORTO = 49,
			MALIKTO = 50,

			END_OF_PRIEST = 51

		};

		enum class SpellID_V {

			DUMAPIC = 0,
			HALITO = 1,
			KATINO = 2,
			MOGREF = 3,

			BOLATU = 4,
			DESTO = 5,
			MELITO = 6,
			MORLIS = 7,
			PONTI = 8,

			CALIFIC = 9,
			CORTU = 10,
			KANTIOS = 11,
			MAHALITO = 12,

			LAHALITO = 13,
			LITOFEIT = 14,
			ROKDO = 15,
			TZALIK = 16,

			BACORTU = 17,
			MADALTO = 18,
			PALIOS = 19,
			SOCORDI = 20,
			VASKYRE = 21,

			HAMAN = 22,
			LADALTO = 23,
			LOKARA = 24,
			MAMOGREF = 25,
			ZILWAN = 26,

			MAHAMAN = 27,
			MALOR = 28,
			MAWXIWTZ = 29,
			TILTOWAIT = 30,
			END_OF_MAGE = 31,

			BADIOS = 32,
			DIOS = 33,
			KALKI = 34,
			MILWA = 35,
			PORFIC = 36,

			CALFO = 37,
			KANDI = 38,
			KATU = 39,
			MONTINO = 40,

			BAMATU = 41,
			DIALKO = 42,
			HAKANIDO = 43,
			LATUMAPIC = 44,
			LOMILWA = 45,

			BADIAL = 46,
			BARIKO = 47,
			DIAL = 48,
			LATUMOFIS = 49,
			MAPORFIC = 50,

			BADI = 51,
			BAMORDI = 52,
			DI = 53,
			DIALMA = 54,
			MOGATO = 55,

			LABADI = 56,
			LOKTOFEIT = 57,
			KAKAMEN = 58,
			MADI = 59,

			BAKADI = 60,
			IHALON = 61,
			KADORTO = 62,
			MABARIKO = 63,
			END_OF_PRIEST = 64
		};
	} // namespace Enums::Magic

	// Management Menu
	namespace Enums::Manage {

		enum class Roster { NONE, INSPECT, EDIT, DELETE };

		enum class Category { NONE, MANAGE, CREATE, IMPORT };

		enum Method { FULL = 0, QUICK = 1, RANDOM = 2 };

		enum class Import { NONE, WIZARDRY, BARDS_TALE, ULTIMA };
	} // namespace Enums::Manage

	namespace Enums::Castle {

		enum class Tavern { NONE, MENU, ADD, REMOVE, REORDER, INSPECT };

		enum class Inn { NONE, MENU, STAY, INPECT };

		enum class Shop { NONE, MENU, TRADE, UNCURSE, IDENTIFY, INSPECT };

		enum class Temple { NONE, MENU, CURE, TITHE, INSPECT };
	} // namespace Enums::Castle

	// Text Files
	namespace Enums::File {

		// Text File Types
		enum class Type { NONE, HELP, VERSION, LICENSE };
	} // namespace Enums::File

	// Character Stuff
	namespace Enums::Character {

		enum class Location {

			NONE = -1,
			PARTY = 0,
			INN = 1,
			TEMPLE = 2,
			MAZE = 3
		};

		// State of Character Creation
		enum class Stage {
			NONE,
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

		enum class Legate { NONE, CONFIRM, CHANGE_ALIGNMENT, LEGATED };

		// View Sections
		enum class View {
			NONE = 0,
			SUMMARY = 1,
			DETAILED = 2,
			MAGE_SPELLS = 3,
			PRIEST_SPELLS = 4,
			INVENTORY = 5,
			// MESSAGES = 6,
			// ACHIEVEMENTS = 7
		};

		// Character Classes
		enum class Race { NONE, HUMAN, ELF, DWARF, GNOME, HOBBIT };

		enum class Class { NONE, FIGHTER, MAGE, PRIEST, THIEF, BISHOP, SAMURAI, LORD, NINJA };

		enum class Align { NONE, GOOD, NEUTRAL, EVIL };

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

		enum class Attribute { NONE = 0, STRENGTH = 1, IQ = 2, PIETY = 3, VITALITY = 4, AGILITY = 5, LUCK = 6 };

		enum class Ability_Type { NONE, NUMBER, MODIFIER, PERCENTAGE, STAT, AC, STATUS };

		enum class Ability {
			NONE,
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
	} // namespace Enums::Character

	// System
	namespace Enums::System {

		// Errors
		enum class Error {
			NONE = 0,
			JSON_PARSE_ERROR = 1,
			UNKNOWN_STRING_KEY = 2,
			UNKNOWN_COMPONENT = 3,
			SQLLITE_ERROR = 4,
			OPTIONAL_RETURNED = 5,
			UNHANDLED_EXCEPTION = 6
		};

		// Return values
		enum class ModuleResult { NONE, CLOSE, BACK, EXIT, NEXT, CANCEL, DELETE };

		// Random (Dice) Types
		enum class Random {
			NONE,
			D2,
			D3,
			D4,
			D5,
			D6,
			D7,
			D8,
			D10,
			D15,
			D16,
			D52,
			D100,
			D300,
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
	} // namespace Enums::System

	namespace Enums::MainMenu {

		enum class Type { NONE, ATTRACT_MODE, ATTRACT_MENU };
	}

	// Menu
	namespace Enums::Menu {

		// Button States
		enum class ButtonState { NONE, DISABLED, ENABLED, HOVERED, SELECTED };

		// Menu Type Tuple Fields
		enum class Field { NONE = -1, INDEX = 0, TYPE = 1, ITEM = 2, TEXT = 3, ENABLED = 4, CONFIG_OPTION = 5 };

		// Menu Types
		enum class Type {
			NONE,
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
			CAMP,
			SEARCH,
			ACTION,
			ELEVATOR_A_D,
			ELEVATOR_A_F,
			RESTART_EXPEDITION,
			CHARACTERS_HERE
		};

		enum class Mode { NONE, TAVERN, TEMPLE, INN, SHOP, TRAINING, CAMP, ACTION, SEARCH };

		// Menu Items
		enum class Item {
			NONE,
			ABORT,
			CANCEL,
			GO_BACK,
			GO_NEXT,
			RETURN,
			SAVE,
			MM_NEW_GAME,
			MM_CONTINUE_GAME,
			MM_OPTIONS,
			MM_COMPENDIUM,
			MM_LICENSE,
			QUIT,
			SEPARATOR,
			TITLE,
			SPACER,
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
			CAMP,
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
			IN_STAY,
			IN_INSPECT,
			IN_CASTLE,
			SH_BUY_AND_SELL,
			SH_UNCURSE,
			SH_IDENTIFY,
			SH_INSPECT,
			SH_CASTLE,
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
			AC_SHOW_MAP
		};

		// Menu Items
		enum class ItemType { NONE, ENTRY, TITLE, SEPARATOR, SPACER, TEXT, SAVE, CANCEL };
	} // namespace Enums::Menu

	namespace Enums {

		enum class Dialog { NONE, OK, CANCEL };

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
	} // namespace Enums

	namespace Enums {

		namespace Map {

			enum class Direction {

				NONE = -1,
				NORTH = 0,
				EAST = 1,
				SOUTH = 2,
				WEST = 3
			};

			enum class Type { NONE = -1, EMPTY = 0, START = 1, TUTORIAL = 2, MAZE = 3 };

		} // namespace Map

		namespace View {

			namespace Message {

				enum class Position {

					NONE = -1,
					MIDDLE = 0,
					TOP = 1,
					BOTTOM = 2
				};
			}

			namespace Cell {

				enum class Layer {

					NONE = -1,
					FLOOR = 0,
					CEILING = 1,
					WALLS = 2,
					OBJECT = 3,
					DECAL = 4
				};

				enum class Type {

					NONE = -1,
					FLOOR = 0,
					CEILING = 1,
					FRONT = 2,
					SIDE = 3,
					OBJECT = 4
				};
			} // namespace Cell
		}	  // namespace View

		namespace Tile {

			enum class Edge {

				NONE = 0,
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

			enum class Wall {

				NONE = -1,
				NORTH = 0,
				EAST = 1,
				SOUTH = 2,
				WEST = 3
			};

			enum class DoorType {

				NONE = 0,
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
				NONE = 0,
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

		} // namespace Tile

		namespace Automap {

			enum class Feature {

				NONE = -1,
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

		} // namespace Automap
	}	  // namespace Enums
} // namespace Sorcery
