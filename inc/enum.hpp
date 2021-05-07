// Copyright (C) 2021 Dave Moore
//
// This file is part of Sorcery: Dreams of the Mad Overlord.
//
// Sorcery: Dreams of the Mad Overlord is free software: you can redistribute
// it and/or modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation, either version 2 of the License,
// or (at your option) any later version.
//
// Sorcery: Dreams of the Mad Overlord is distributed in the hope that it wil
// be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
// Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Sorcery: Dreams of the Mad Overlord.  If not,
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
			SHOW_CONTROLS
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
			DISPLAY_TEXT_FILE,
			GAME_OPTIONS,
			COMPENDIUM,
			CONFIRM_QUIT_GAME,
			SAVE_CHANGES,
			CANCEL_CHANGES,
			CONFIRM_STRICT_MODE,
			CONFIRM_LEAVE_GAME,
			CONFIRM_NEW_GAME,
			INPUT_NAME,
			ALLOCATE_STATS,
			CHOOSE_METHOD,
			CHOOSE_PORTRAIT,
			REVIEW_AND_CONFIRM
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
			HELP = 14
		};

		enum class Confirm { NONE, YES, NO };

		enum class DialogButton { NONE, OK, YES, NO };

		enum class ComponentType { NONE, IMAGE, TEXT, TOOLTIP, MENU, FRAME, ICON, DIALOG, UNKNOWN };

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
			BACKGROUND,
			BANNER,
			CONTROLS,
			CREATURES,
			ICONS,
			LOGO,
			NINEPATCH,
			PORTRAITS,
			SPLASH,
			TOWN,
			UI
		};
	} // namespace Enums::Graphics

	namespace Enums::Settings {

		// Settings
		enum class Category { NONE, GENERAL, GAMEPLAY, GRAPHICS };
	} // namespace Enums::Settings

	namespace Enums::Internal {

		enum class FontType { NONE, MONOSPACE, PROPORTIONAL, TEXT };

		enum class StringType { NONE, NORMAL, EXPLAIN };

	} // namespace Enums::Internal

	namespace Enums::Magic {

		enum class SpellType { NONE, MAGE, PRIEST };

		enum class SpellPointStatus { NONE, CURRENT, MAXIMUM };

		enum class SpellCategory { NONE, HEALING, FIELD, SUPPORT, DISABLE, ATTACK };

		enum class SpellID {
			NONE,
			HALITO,
			MOGREF,
			KATINO,
			DUMAPIC,
			DESTO,
			DILTO,
			SOPIC,
			CALIFIC,
			MAHALITO,
			MOLITO,
			MORLIS,
			DALTO,
			LAHALITO,
			MAMORLIS,
			LITOFEIT,
			MAKANITO,
			MADALTO,
			LAKANITO,
			ZILWAN,
			MASOPIC,
			HAMAN,
			MALOR,
			MAHAMAN,
			TILTOWAIT,
			KALKI,
			DIOS,
			BADIOS,
			MILWA,
			PORFIC,
			MATU,
			CALFO,
			MANIFO,
			MONTINO,
			LOMILWA,
			DIALKO,
			LATUMAPIC,
			BAMATU,
			DIAL,
			BADIAL,
			LATUMOFIS,
			MAPORFIC,
			DIALMA,
			BADIALMA,
			LITOKAN,
			KANDI,
			DI,
			BADI,
			LORTO,
			MADI,
			MABADI,
			LABADI,
			LOKTOFEIT,
			MALIKTO,
			KADORTO
		};
	} // namespace Enums::Magic

	// Management Menu
	namespace Enums::Manage {

		enum class Category { NONE, MANAGE, CREATE, IMPORT };

		enum Method { FULL = 0, QUICK = 1, RANDOM = 2 };

		enum class Import { NONE, WIZARDRY, BARDS_TALE, ULTIMA };
	} // namespace Enums::Manage

	// Text Files
	namespace Enums::File {

		// Text File Types
		enum class Type { NONE, HELP, VERSION, LICENSE };
	} // namespace Enums::File

	// Character Stuff
	namespace Enums::Character {

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

		// View Sections
		enum class View {
			NONE = 0,
			MAIN = 1,
			STRICT = 2,
			DETAILED = 3,
			INVENTORY = 4,
			MAGE_SPELLS = 5,
			PRIEST_SPELLS = 6
			// MESSAGES = 6,
			// ACHIEVEMENTS = 7,
			// STRICT = 8
		};

		// Character Classes
		enum class Race { NONE, HUMAN, ELF, DWARF, GNOME, HOBBIT };

		enum class Class { NONE, FIGHTER, MAGE, PRIEST, THIEF, BISHOP, SAMURAI, LORD, NINJA };

		enum class Align { NONE, GOOD, NEUTRAL, EVIL };

		/* enum class Status {
			OK,
			AFRAID,
			ASLEEP,
			PARALYSED,
			STONED,
			DEAD,
			ASHES,
			LOST
		}; */

		enum class Attribute {
			NONE = 0,
			STRENGTH = 1,
			IQ = 2,
			PIETY = 3,
			VITALITY = 4,
			AGILITY = 5,
			LUCK = 6
		};

		enum class Ability_Type { NONE, NUMBER, MODIFIER, PERCENTAGE, STAT };

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
			RESISTANCE_VS_MANIFO
		};
	} // namespace Enums::Character

	// System
	namespace Enums::System {

		// Errors
		enum class Error {
			NONE = 0,
			JSON_PARSE_ERROR = 1,
			UNKNOWN_STRING_KEY = 2,
			UNKNOWN_COMPONNENT = 3
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
			ZERO_TO_299
		};
	} // namespace Enums::System

	namespace Enums::MainMenu {

		enum class Type { NONE, ATTRACT_MODE, ATTRACT_MENU };
	}

	namespace Enums::GameMenu {

		enum class Type { NONE, CASTLE, EDGE_OF_TOWN };
	}

	// Menu
	namespace Enums::Menu {

		// Button States
		enum class ButtonState { NONE, DISABLED, ENABLED, HOVERED, SELECTED };

		// Menu Type Tuple Fields
		enum class Field {
			NONE = -1,
			INDEX = 0,
			TYPE = 1,
			ITEM = 2,
			TEXT = 3,
			ENABLED = 4,
			CONFIG_OPTION = 5
		};

		// Menu Types
		enum class Type {
			NONE,
			MAIN,
			OPTIONS,
			PAUSE,
			CASTLE,
			EDGE_OF_TOWN,
			TRAINING_GROUNDS,
			CHOOSE_CHARACTER_RACE,
			CHOOSE_CHARACTER_ALIGNMENT,
			ALLOCATE_CHARACTER_ATTRIBUTES,
			CHOOSE_CHARACTER_CLASS,
			CHOOSE_CHARACTER_PORTRAIT,
			REVIEW_AND_CONFIRM,
			CHOOSE_METHOD
		};

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
			OP_ALLOW_AMBUSH_HIDE,
			OP_ALLOW_RANGED_WEAPONS,
			OP_SPELLCASTING_IN_SURPRISE_ROUND,
			OP_BATCH_HEALING_AFTER_RETURN_TO_CASTLE,
			OP_REROLL_ONES_ON_DICE,
			OP_ALLOW_LOST_LEGATED,
			OP_ALLOW_CURABLE_DRAIN,
			OP_ALLOW_SHARED_INVENTORY,
			OP_CAMPING_RESTORE_SPELL_POINTS,
			OP_CHARGED_ITEMS,
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
			ET_CASTLE,
			ET_TRAIN,
			ET_MAZE,
			ET_LEAVE_GAME,
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
			CM_RANDOM
		};

		// Menu Items
		enum class ItemType { NONE, ENTRY, TITLE, SEPARATOR, SPACER, TEXT, SAVE, CANCEL };
	} // namespace Enums::Menu

	// Display
	namespace Enums::Display {

		// Colour Types
		enum class Colour {
			NONE,
			DEFAULT_BG,
			DEFAULT_FG,
			TEXT_FILE_CONTENTS,
			TEXT_FILE_INSTRUCTIONS,
			MAIN_MENU_TITLE,
			MAIN_MENU_COPYRIGHT,
			MAIN_MENU_BLURB,
			MAIN_MENU_INSTRUCTIONS,
			MAIN_MENU_GPL_BLURB,
			MAIN_MENU_GPL_TEXT,
			MAIN_MENU_SELECTED_FOREGROUND,
			MAIN_MENU_SELECTED_BACKGROUND,
			MAIN_MENU_NOT_SELECTED_FOREGROUND,
			MAIN_MENU_NOT_SELECTED_BACKGROUND,
			MAIN_MENU_FRAME,
			CONFIRM_LEAVE_GAME,
			CONFIRM_STRICT_MODE_TEXT,
			CONFIRM_STRICT_MODE_FRAME,
			CONFIRM_SAVE_GAME_OPTIONS_TEXT,
			CONFIRM_SAVE_GAME_OPTIONS_FRAME,
			GAME_OPTIONS_GROUP_FRAME_SELECTED,
			GAME_OPTIONS_GROUP_FRAME_NOT_SELECTED,
			GAME_OPTION_SELECTED_BOLD,
			GAME_OPTION_SELECTED_NORMAL,
			GAME_OPTION_NOT_SELECTED_BOLD,
			GAME_OPTION_NOT_SELECTED_NORMAL,
			CONFIRM_LEAVE_GAME_OPTIONS,
			GAME_OPTIONS_INSTRUCTIONS,
			CHARACTER_CREATION_INSTRUCTIONS,
			MENU_EXPLAIN_FOREGROUND,
			MENU_EXPLAIN_BACKGROUND,
			MENU_SELECTED_FOREGROUND,
			MENU_SELECTED_BACKGROUND,
			MENU_NOT_SELECTED_FOREGROUND,
			MENU_NOT_SELECTED_BACKGROUND,
			MENU_NOT_ENABLED_FOREGROUND,
			MENU_NOT_ENABLED_BACKGROUND,
			MENU_FRAME_FOREGROUND,
			FPS_COUNTER,
			FPS_COUNTER_VALUE,
			OPTION_ON,
			OPTION_OFF,
			MANAGE_SELECTED,
			HIGHLIGHT_BUTTON_FOR_MOUSE_FOREGROUND,
			HIGHLIGHT_BUTTON_FOR_MOUSE_BACKGROUND,
			TEXT_FILE_HIGHLIGHTED_FOREGROUND,
			TEXT_FILE_HIGHLIGHTED_BACKGROUND,
			MENU_TITLE_FOREGROUND,
			MENU_SEPARATOR_FOREGROUND,
			GAME_OPTION_ON,
			GAME_OPTION_OFF,
			CHARACTER_CREATION_SUBTITLES,
			CHARACTER_CREATION_STAGE,
			POINTS_STARTING,
			POINTS_ADDED,
			NO_POINTS_LEFT,
			POINTS_LEFT,
			CLASS_ALLOWED,
			CLASS_NOT_ALLOWED,
			INPUT_TEXT,
			INPUT_TEXT_FRAME,
			CHARACTER_SUMMARY_INFORMATION,
			CHARACTER_SUMMARY_ATTRIBUTES_MAX,
			CHARACTER_SUMMARY_ATTRIBUTES_MIN,
			CHARACTER_SUMMARY_ABILITIES,
			CHARACTER_SUMMARY_SPELLS_MAX,
			CHARACTER_SUMMARY_SPELLS_MIN,
			CHARACTER_SUMMARY_STATUS,
			CHARACTER_SUMMARY_HP,
			CHARACTER_DETAILED_ATTRIBUTES,
			CHARACTER_DETAILED_ABILITIES
		};
	} // namespace Enums::Display

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
			ALLOW_AMBUSH_HIDE = 16,
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
} // namespace Sorcery
