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

#include "gui/menu.hpp"
#include "common/enum.hpp"
#include "common/macro.hpp"
#include "core/animation.hpp"
#include "core/display.hpp"
#include "core/game.hpp"
#include "core/graphics.hpp"
#include "core/state.hpp"
#include "core/system.hpp"
#include "core/window.hpp"
#include "resources/componentstore.hpp"
#include "resources/itemstore.hpp"
#include "resources/monsterstore.hpp"
#include "resources/resourcemanager.hpp"
#include "resources/stringstore.hpp"
#include "types/character.hpp"
#include "types/component.hpp"
#include "types/configfile.hpp"
#include "types/itemtype.hpp"
#include "types/monstertype.hpp"

// Standard Constructor
Sorcery::Menu::Menu(System *system, Display *display, Graphics *graphics,
	Game *game, const MTP type, std::optional<MMD> mode,
	std::optional<unsigned int> data)
	: _system{system}, _display{display}, _graphics{graphics}, _game{game},
	  _type{type}, _mode{mode} {

	// Clear the Items
	items.clear();
	bounds.clear();
	count = 0;
	_texts.clear();
	_options.clear();

	// Now depending on the menu type, add the relevant items
	switch (_type) {
	case MTP::CHARACTER_ROSTER:
		[[fallthrough]];
	case MTP::PARTY_CHARACTERS:
		[[fallthrough]];
	case MTP::AVAILABLE_CHARACTERS:
		[[fallthrough]];
	case MTP::INVALID_CHARACTERS:
		[[fallthrough]];
	case MTP::PARTY_CHARACTER_NAMES:
		[[fallthrough]];
	case MTP::RESTART_EXPEDITION:
		[[fallthrough]];
	case MTP::BUY_SELL_CHARACTERS:
		[[fallthrough]];
	case MTP::CURSED_CHARACTERS:
		[[fallthrough]];
	case MTP::IDENTIFY_CHARACTERS:
		[[fallthrough]];
	case MTP::CHARACTERS_HERE:
		_populate_chars();
		if (_go_first)
			selected = items.begin();
		else
			selected = items.end();
		break;
	case MTP::CHARACTER_TRADE:
		_populate_trade_chars(data.value_or(-1));
		selected = items.begin();
		break;
	case MTP::ITEM_ACTION:
		// Note these are enabled/disabled depending on the context in
		// inspect.cpp
		_add_item(0, MIT::ENTRY, MIM::C_ACTION_EQUIP,
			TRIM_COPY((*_display->string)["C_ACTION_EQUIP"]));
		_add_item(1, MIT::ENTRY, MIM::C_ACTION_UNEQUIP,
			TRIM_COPY((*_display->string)["C_ACTION_UNEQUIP"]));
		_add_item(2, MIT::ENTRY, MIM::C_ACTION_TRADE,
			TRIM_COPY((*_display->string)["C_ACTION_TRADE"]));
		_add_item(3, MIT::ENTRY, MIM::C_ACTION_EXAMINE,
			TRIM_COPY((*_display->string)["C_ACTION_EXAMINE"]));
		_add_item(4, MIT::ENTRY, MIM::C_ACTION_INVOKE,
			TRIM_COPY((*_display->string)["C_ACTION_INVOKE"]));
		_add_item(5, MIT::ENTRY, MIM::C_ACTION_USE,
			TRIM_COPY((*_display->string)["C_ACTION_USE"]));
		_add_item(6, MIT::ENTRY, MIM::C_ACTION_IDENTIFY,
			TRIM_COPY((*_display->string)["C_ACTION_IDENTIFY"]));
		_add_item(7, MIT::ENTRY, MIM::C_ACTION_DROP,
			TRIM_COPY((*_display->string)["C_ACTION_DROP"]));
		_add_item(8, MIT::ENTRY, MIM::C_ACTION_LEAVE,
			TRIM_COPY((*_display->string)["C_ACTION_LEAVE"]));
		selected = items.begin();
		break;
	case MTP::COMPENDIUM:
		_add_item(0, MIT::ENTRY, MIM::CO_MONSTERS,
			(*_display->string)["MENU_MONSTERS"]);
		_add_item(
			1, MIT::ENTRY, MIM::CO_ITEMS, (*_display->string)["MENU_ITEMS"]);
		_add_item(
			2, MIT::ENTRY, MIM::CO_SPELLS, (*_display->string)["MENU_SPELLS"]);
		_add_item(3, MIT::ENTRY, MIM::CO_FEATURES,
			(*_display->string)["MENU_FEATURES"]);
		_add_item(
			4, MIT::ENTRY, MIM::CO_STATS, (*_display->string)["MENU_STATS"]);
		_add_item(5, MIT::ENTRY, MIM::ITEM_RETURN,
			(*_display->string)["MENU_RETURN"]);
		selected = items.begin();
		break;
	case MTP::EDIT_CHARACTER:
		_add_item(0, MIT::ENTRY, MIM::EC_CHANGE_CLASS,
			(*_display->string)["EDIT_CHARACTER_CHANGE_CLASS"]);
		_add_item(1, MIT::ENTRY, MIM::EC_CHANGE_NAME,
			(*_display->string)["EDIT_CHARACTER_CHANGE_NAME"]);
		_add_item(2, MIT::ENTRY, MIM::EC_LEGATE_CHARACTER,
			(*_display->string)["EDIT_CHARACTER_LEGATE_CHARACTER"]);
		_add_item(3, MIT::SPACER, MIM::ITEM_SPACER,
			(*_display->string)["MENU_SPACER"]);
		_add_item(4, MIT::ENTRY, MIM::EC_RETURN_EDIT,
			(*_display->string)["EDIT_CHARACTER_RETURN_EDIT"]);
		selected = items.begin();
		break;
	case MTP::MAIN:
		_add_item(0, MIT::ENTRY, MIM::MM_NEW_GAME,
			(*_display->string)["MAIN_MENU_OPTION_START"]);
		_add_item(1, MIT::ENTRY, MIM::MM_CONTINUE_GAME,
			(*_display->string)["MAIN_MENU_OPTION_CONTINUE"]);
		_add_item(2, MIT::ENTRY, MIM::MM_OPTIONS,
			(*_display->string)["MAIN_MENU_OPTION_OPTIONS"]);
		_add_item(3, MIT::ENTRY, MIM::MM_COMPENDIUM,
			(*_display->string)["MAIN_MENU_OPTION_COMPENDIUM"]);
		_add_item(4, MIT::ENTRY, MIM::MM_LICENSE,
			(*_display->string)["MAIN_MENU_OPTION_LICENSE"]);
		_add_item(5, MIT::ENTRY, MIM::ITEM_QUIT,
			(*_display->string)["MAIN_MENU_OPTION_EXIT"]);
		selected = items.begin();
		break;
	case MTP::REVIEW_AND_CONFIRM:
		_add_item(
			0, MIT::ENTRY, MIM::RC_ACCEPT, (*_display->string)["RC_ACCEPT"]);
		_add_item(
			1, MIT::ENTRY, MIM::RC_REJECT, (*_display->string)["RC_REJECT"]);
		selected = items.begin();
		break;
	case MTP::ELEVATOR_A_D:
		_add_item(0, MIT::TEXT, MIM::NC_WARNING,
			(*_display->string)["BUTTONS_A_TO_D"]);
		_add_item(
			1, MIT::TEXT, MIM::NC_WARNING, (*_display->string)["PRESS_ONE"]);
		_add_item(2, MIT::SPACER, MIM::ITEM_SPACER,
			(*_display->string)["MENU_SPACER"]);
		_add_item(3, MIT::ENTRY, MIM::EL_A, (*_display->string)["ELEVATOR_A"]);
		_add_item(4, MIT::ENTRY, MIM::EL_B, (*_display->string)["ELEVATOR_B"]);
		_add_item(5, MIT::ENTRY, MIM::EL_C, (*_display->string)["ELEVATOR_C"]);
		_add_item(6, MIT::ENTRY, MIM::EL_D, (*_display->string)["ELEVATOR_D"]);
		_add_item(7, MIT::SPACER, MIM::ITEM_SPACER,
			(*_display->string)["MENU_SPACER"]);
		_add_item(8, MIT::ENTRY, MIM::EL_LEAVE,
			(*_display->string)["ELEVATOR_LEAVE"]);
		selected = items.end();
		break;
	case MTP::ELEVATOR_A_F:
		_add_item(0, MIT::TEXT, MIM::NC_WARNING,
			(*_display->string)["BUTTONS_A_TO_F"]);
		_add_item(
			1, MIT::TEXT, MIM::NC_WARNING, (*_display->string)["PRESS_ONE"]);
		_add_item(2, MIT::SPACER, MIM::ITEM_SPACER,
			(*_display->string)["MENU_SPACER"]);
		_add_item(3, MIT::ENTRY, MIM::EL_A, (*_display->string)["ELEVATOR_A"]);
		_add_item(4, MIT::ENTRY, MIM::EL_B, (*_display->string)["ELEVATOR_B"]);
		_add_item(5, MIT::ENTRY, MIM::EL_C, (*_display->string)["ELEVATOR_C"]);
		_add_item(6, MIT::ENTRY, MIM::EL_D, (*_display->string)["ELEVATOR_D"]);
		_add_item(7, MIT::ENTRY, MIM::EL_E, (*_display->string)["ELEVATOR_E"]);
		_add_item(8, MIT::ENTRY, MIM::EL_F, (*_display->string)["ELEVATOR_F"]);
		_add_item(9, MIT::SPACER, MIM::ITEM_SPACER,
			(*_display->string)["MENU_SPACER"]);
		_add_item(10, MIT::ENTRY, MIM::EL_LEAVE,
			(*_display->string)["ELEVATOR_LEAVE"]);
		selected = items.end();
		break;
	case MTP::CASTLE:
		_add_item(
			0, MIT::ENTRY, MIM::CA_TAVERN, (*_display->string)["MENU_TAVERN"]);
		_add_item(1, MIT::ENTRY, MIM::CA_INN, (*_display->string)["MENU_INN"]);
		_add_item(
			2, MIT::ENTRY, MIM::CA_SHOP, (*_display->string)["MENU_SHOP"]);
		_add_item(
			3, MIT::ENTRY, MIM::CA_TEMPLE, (*_display->string)["MENU_TEMPLE"]);
		_add_item(4, MIT::ENTRY, MIM::CA_EDGE_OF_TOWN,
			(*_display->string)["MENU_EDGE_OF_TOWN"]);
		selected = items.begin();
		break;
	case MTP::EDGE_OF_TOWN:
		_add_item(
			0, MIT::ENTRY, MIM::ET_CASTLE, (*_display->string)["MENU_CASTLE"]);
		_add_item(
			1, MIT::ENTRY, MIM::ET_TRAIN, (*_display->string)["MENU_TRAIN"]);
		_add_item(
			2, MIT::ENTRY, MIM::ET_MAZE, (*_display->string)["MENU_MAZE"]);
		_add_item(3, MIT::ENTRY, MIM::ET_RESTART,
			(*_display->string)["MENU_RESTART"]);
		_add_item(4, MIT::ENTRY, MIM::ET_LEAVE_GAME,
			(*_display->string)["MENU_LEAVE_GAME"]);
		selected = items.begin();
		break;
	case MTP::INN_CHOOSE_BED:
		_add_item(0, MIT::ENTRY, MIM::IN_STABLES, (*_display->string)["INN_1"]);
		_add_item(1, MIT::ENTRY, MIM::IN_COT, (*_display->string)["INN_2"]);
		_add_item(2, MIT::ENTRY, MIM::IN_ECONOMY, (*_display->string)["INN_3"]);
		_add_item(
			3, MIT::ENTRY, MIM::IN_MERCHANT, (*_display->string)["INN_4"]);
		_add_item(4, MIT::ENTRY, MIM::IN_ROYAL, (*_display->string)["INN_5"]);
		_add_item(5, MIT::SPACER, MIM::ITEM_SPACER,
			(*_display->string)["MENU_SPACER"]);
		_add_item(
			6, MIT::ENTRY, MIM::IN_POOL_GOLD, (*_display->string)["INN_POOL"]);
		_add_item(7, MIT::SPACER, MIM::ITEM_SPACER,
			(*_display->string)["MENU_SPACER"]);
		_add_item(8, MIT::ENTRY, MIM::IN_BACK, (*_display->string)["INN_BACK"]);
		selected = items.begin();
		break;
	case MTP::CONTINUE:
		_add_item(
			0, MIT::ENTRY, MIM::ITEM_CONTINUE, (*_display->string)["CONTINUE"]);
		selected = items.begin();
		break;
	case MTP::GRAVEYARD:
		_add_item(0, MIT::ENTRY, MIM::ITEM_CONTINUE,
			(*_display->string)["GRAVEYARD_MENU"]);
		selected = items.begin();
		break;
	case MTP::STOP:
		_add_item(0, MIT::ENTRY, MIM::ITEM_STOP, (*_display->string)["STOP"]);
		selected = items.begin();
		break;
	case MTP::TAVERN:
		_add_item(0, MIT::ENTRY, MIM::TA_ADD_TO_PARTY,
			(*_display->string)["TAVERN_ADD_TO_PARTY"]);
		_add_item(1, MIT::ENTRY, MIM::TA_REMOVE_FROM_PARTY,
			(*_display->string)["TAVERN_REMOVE_FROM_PARTY"]);
		_add_item(2, MIT::ENTRY, MIM::TA_INSPECT,
			(*_display->string)["TAVERN_INSPECT"]);
		_add_item(3, MIT::ENTRY, MIM::TA_REORDER,
			(*_display->string)["TAVERN_REORDER_PARTY"]);
		_add_item(4, MIT::ENTRY, MIM::TA_DIVVY_GOLD,
			(*_display->string)["TAVERN_DIVVY_GOLD"]);
		_add_item(5, MIT::ENTRY, MIM::TA_CASTLE,
			(*_display->string)["TAVERN_CASTLE"]);
		selected = items.begin();
		break;
	case MTP::INN:
		_add_item(0, MIT::ENTRY, MIM::IN_STAY_CHARACTER,
			(*_display->string)["INN_STAY_ONE"]);
		_add_item(
			1, MIT::ENTRY, MIM::IN_INSPECT, (*_display->string)["INN_INSPECT"]);
		_add_item(
			2, MIT::ENTRY, MIM::IN_CASTLE, (*_display->string)["INN_CASTLE"]);
		selected = items.begin();
		break;
	case MTP::SHOP:
		_add_item(0, MIT::ENTRY, MIM::SH_BUY_AND_SELL,
			(*_display->string)["SHOP_ENTER"]);
		_add_item(1, MIT::ENTRY, MIM::SH_INSPECT,
			(*_display->string)["SHOP_INSPECT"]);
		_add_item(
			2, MIT::ENTRY, MIM::SH_CASTLE, (*_display->string)["SHOP_CASTLE"]);
		selected = items.begin();
		break;
	case MTP::SHOP_ACTION:
		_add_item(0, MIT::ENTRY, MIM::SH_BUY, (*_display->string)["SHOP_BUY"]);
		_add_item(
			1, MIT::ENTRY, MIM::SH_SELL, (*_display->string)["SHOP_SELL"]);
		_add_item(2, MIT::ENTRY, MIM::SH_UNCURSE,
			(*_display->string)["SHOP_UNCURSE"]);
		_add_item(3, MIT::ENTRY, MIM::SH_IDENTIFY,
			(*_display->string)["SHOP_IDENTIFY"]);
		_add_item(
			4, MIT::ENTRY, MIM::SH_POOL_GOLD, (*_display->string)["SHOP_POOL"]);
		_add_item(
			5, MIT::ENTRY, MIM::SH_BACK, (*_display->string)["SHOP_BACK"]);
		selected = items.begin();
		break;
	case MTP::TEMPLE:
		_add_item(
			0, MIT::ENTRY, MIM::TE_HELP, (*_display->string)["TEMPLE_HELP"]);
		_add_item(
			1, MIT::ENTRY, MIM::TE_TITHE, (*_display->string)["TEMPLE_TITHE"]);
		_add_item(2, MIT::ENTRY, MIM::TE_INSPECT,
			(*_display->string)["TEMPLE_INSPECT"]);
		_add_item(3, MIT::ENTRY, MIM::TE_CASTLE,
			(*_display->string)["TEMPLE_CASTLE"]);
		selected = items.begin();
		break;
	case MTP::TRAINING_GROUNDS:
		_add_item(0, MIT::ENTRY, MIM::TR_CREATE,
			(*_display->string)["TRAINING_GROUNDS_MENU_OPTION_CREATE"]);
		_add_item(1, MIT::ENTRY, MIM::TR_EDIT,
			(*_display->string)["TRAINING_GROUNDS_MENU_OPTION_EDIT"]);
		_add_item(2, MIT::ENTRY, MIM::TR_DELETE,
			(*_display->string)["TRAINING_GROUNDS_MENU_OPTION_DELETE"]);
		_add_item(3, MIT::ENTRY, MIM::TR_INSPECT,
			(*_display->string)["TRAINING_GROUNDS_MENU_OPTION_INSPECT"]);
		_add_item(4, MIT::ENTRY, MIM::TR_EDGE_OF_TOWN,
			(*_display->string)["TRAINING_GROUNDS_MENU_OPTION_RETURN"]);
		selected = items.begin();
		break;
	case MTP::ALLOCATE_CHARACTER_ATTRIBUTES:
		_add_item(0, MIT::ENTRY, MIM::CS_STRENGTH,
			(*_display->string)["CHARACTER_STAT_STRENGTH"], true, CFG::NONE,
			(*_display->string)["HINT_CHARACTER_STAT_STRENGTH"]);
		_add_item(1, MIT::ENTRY, MIM::CS_IQ,
			(*_display->string)["CHARACTER_STAT_INTELLIGENCE"], true, CFG::NONE,
			(*_display->string)["HINT_CHARACTER_STAT_INTELLIGENCE"]);
		_add_item(2, MIT::ENTRY, MIM::CS_PIETY,
			(*_display->string)["CHARACTER_STAT_PIETY"], true, CFG::NONE,
			(*_display->string)["HINT_CHARACTER_STAT_PIETY"]);
		_add_item(3, MIT::ENTRY, MIM::CS_VITALITY,
			(*_display->string)["CHARACTER_STAT_VITALITY"], true, CFG::NONE,
			(*_display->string)["HINT_CHARACTER_STAT_VITALITY"]);
		_add_item(4, MIT::ENTRY, MIM::CS_AGILITY,
			(*_display->string)["CHARACTER_STAT_AGILITY"], true, CFG::NONE,
			(*_display->string)["HINT_CHARACTER_STAT_AGILITY"]);
		_add_item(5, MIT::ENTRY, MIM::CS_LUCK,
			(*_display->string)["CHARACTER_STAT_LUCK"], true, CFG::NONE,
			(*_display->string)["HINT_CHARACTER_STAT_LUCK"]);
		selected = items.begin();
		break;
	case MTP::CHOOSE_CHARACTER_ALIGNMENT:
		_add_item(0, MIT::ENTRY, MIM::CA_GOOD,
			(*_display->string)["CHARACTER_ALIGNMENT_GOOD"], true, CFG::NONE,
			(*_display->string)["HINT_CHARACTER_ALIGNMENT_GOOD"]);
		_add_item(1, MIT::ENTRY, MIM::CA_NEUTRAL,
			(*_display->string)["CHARACTER_ALIGNMENT_NEUTRAL"], true, CFG::NONE,
			(*_display->string)["HINT_CHARACTER_ALIGNMENT_NEUTRAL"]);
		_add_item(2, MIT::ENTRY, MIM::CA_EVIL,
			(*_display->string)["CHARACTER_ALIGNMENT_EVIL"], true, CFG::NONE,
			(*_display->string)["HINT_CHARACTER_ALIGNMENT_EVIL"]);
		selected = items.begin();
		break;
	case MTP::CHOOSE_CHARACTER_CLASS:
		[[fallthrough]];
	case MTP::CHANGE_CHARACTER_CLASS:
		_add_item(0, MIT::ENTRY, MIM::CC_SAMURAI,
			(*_display->string)["CHARACTER_CLASS_SAMURAI"], true, CFG::NONE,
			(*_display->string)["HINT_CHARACTER_CLASS_SAMURAI"]);
		_add_item(1, MIT::ENTRY, MIM::CC_FIGHTER,
			(*_display->string)["CHARACTER_CLASS_FIGHTER"], true, CFG::NONE,
			(*_display->string)["HINT_CHARACTER_CLASS_FIGHTER"]);
		_add_item(2, MIT::ENTRY, MIM::CC_LORD,
			(*_display->string)["CHARACTER_CLASS_LORD"], true, CFG::NONE,
			(*_display->string)["HINT_CHARACTER_CLASS_LORD"]);
		_add_item(3, MIT::ENTRY, MIM::CC_THIEF,
			(*_display->string)["CHARACTER_CLASS_THIEF"], true, CFG::NONE,
			(*_display->string)["HINT_CHARACTER_CLASS_THIEF"]);
		_add_item(4, MIT::ENTRY, MIM::CC_NINJA,
			(*_display->string)["CHARACTER_CLASS_NINJA"], true, CFG::NONE,
			(*_display->string)["HINT_CHARACTER_CLASS_NINJA"]);
		_add_item(5, MIT::ENTRY, MIM::CC_PRIEST,
			(*_display->string)["CHARACTER_CLASS_PRIEST"], true, CFG::NONE,
			(*_display->string)["HINT_CHARACTER_CLASS_PRIEST"]);
		_add_item(6, MIT::ENTRY, MIM::CC_BISHOP,
			(*_display->string)["CHARACTER_CLASS_BISHOP"], true, CFG::NONE,
			(*_display->string)["HINT_CHARACTER_CLASS_BISHOP"]);
		_add_item(7, MIT::ENTRY, MIM::CC_MAGE,
			(*_display->string)["CHARACTER_CLASS_MAGE"], true, CFG::NONE,
			(*_display->string)["HINT_CHARACTER_CLASS_MAGE"]);
		selected = items.begin();
		break;
	case MTP::CHOOSE_CHARACTER_PORTRAIT:
		break;
	case MTP::CHOOSE_CHARACTER_RACE:
		_add_item(0, MIT::ENTRY, MIM::CR_HUMAN,
			(*_display->string)["CHARACTER_RACE_HUMAN"], true, CFG::NONE,
			(*_display->string)["HINT_CHARACTER_RACE_HUMAN"]);
		_add_item(1, MIT::ENTRY, MIM::CR_ELF,
			(*_display->string)["CHARACTER_RACE_ELF"], true, CFG::NONE,
			(*_display->string)["HINT_CHARACTER_RACE_ELF"]);
		_add_item(2, MIT::ENTRY, MIM::CR_DWARF,
			(*_display->string)["CHARACTER_RACE_DWARF"], true, CFG::NONE,
			(*_display->string)["HINT_CHARACTER_RACE_DWARF"]);
		_add_item(3, MIT::ENTRY, MIM::CR_GNOME,
			(*_display->string)["CHARACTER_RACE_GNOME"], true, CFG::NONE,
			(*_display->string)["HINT_CHARACTER_RACE_GNOME"]);
		_add_item(4, MIT::ENTRY, MIM::CR_HOBBIT,
			(*_display->string)["CHARACTER_RACE_HOBBIT"], true, CFG::NONE,
			(*_display->string)["HINT_CHARACTER_RACE_HOBBIT"]);
		selected = items.begin();
		break;
	case MTP::CHOOSE_METHOD:
		_add_item(0, MIT::ENTRY, MIM::CM_FULL,
			(*_display->string)["CHARACTER_CREATION_METHOD_FULL"], true,
			CFG::NONE,
			(*_display->string)["HINT_CHARACTER_CREATION_METHOD_FULL"]);
		_add_item(1, MIT::ENTRY, MIM::CM_QUICK,
			(*_display->string)["CHARACTER_CREATION_METHOD_QUICK"], true,
			CFG::NONE,
			(*_display->string)["HINT_CHARACTER_CREATION_METHOD_QUICK"]);
		_add_item(2, MIT::ENTRY, MIM::CM_RANDOM,
			(*_display->string)["CHARACTER_CREATION_METHOD_RANDOM"], true,
			CFG::NONE,
			(*_display->string)["HINT_CHARACTER_CREATION_METHOD_RANDOM"]);
		selected = items.begin();
		break;
	case MTP::OPTIONS:
		_add_item(0, MIT::ENTRY, MIM::OP_RECOMMENDED_MODE,
			(*_display->string)["CONFIG_RECOMMENDED_MODE"], true,
			CFG::RECOMMENDED_MODE,
			(*_display->string)["HINT_CONFIG_RECOMMENDED_MODE"]);
		_add_item(1, MIT::ENTRY, MIM::OP_STRICT_MODE,
			(*_display->string)["CONFIG_STRICT_MODE"], true, CFG::STRICT_MODE,
			(*_display->string)["HINT_CONFIG_STRICT_MODE"]);
		_add_item(2, MIT::ENTRY, MIM::OP_CHEAT_MODE,
			(*_display->string)["CONFIG_CHEAT_MODE"], true, CFG::CHEAT_MODE,
			(*_display->string)["HINT_CONFIG_CHEAT_MODE"]);
		_add_item(3, MIT::ENTRY, MIM::OP_AUTO_SAVE,
			(*_display->string)["CONFIG_AUTO_SAVE"], true, CFG::AUTO_SAVE,
			(*_display->string)["HINT_CONFIG_AUTO_SAVE"]);
		_add_item(4, MIT::ENTRY, MIM::OP_DICE_ROLLS,
			(*_display->string)["CONFIG_DICE_ROLLS"], true, CFG::DICE_ROLLS,
			(*_display->string)["HINT_CONFIG_DICE_ROLLS"]);
		_add_item(5, MIT::ENTRY, MIM::OP_ALLOW_MIXED_ALIGNMENT_PARTY,
			(*_display->string)["GAME_ALLOW_MIXED_ALIGNMENT"], true,
			CFG::ALLOW_MIXED_ALIGNMENT_PARTY,
			(*_display->string)["HINT_GAME_ALLOW_MIXED_ALIGNMENT"]);
		_add_item(6, MIT::ENTRY, MIM::OP_STAT_LOSS_ON_LEVEL_UP,
			(*_display->string)["GAME_STAT_LOSS_LEVEL_GAIN"], true,
			CFG::STAT_LOSS_ON_LEVEL_UP,
			(*_display->string)["HINT_GAME_STAT_LOSS_LEVEL_GAIN"]);
		_add_item(7, MIT::ENTRY, MIM::OP_REROLL_HIT_POINTS_ON_LEVEL_GAIN,
			(*_display->string)["GAME_REROLL_HIT_POINTS"], true,
			CFG::REROLL_HIT_POINTS_ON_LEVEL_GAIN,
			(*_display->string)["HINT_GAME_REROLL_HIT_POINTS"]);
		_add_item(8, MIT::ENTRY, MIM::OP_STAT_RESET_ON_CLASS_CHANGE,
			(*_display->string)["GAME_STAT_RESET_CLASS_CHANGE"], true,
			CFG::STAT_RESET_ON_CLASS_CHANGE,
			(*_display->string)["HINT_GAME_STAT_RESET_CLASS_CHANGE"]);
		_add_item(9, MIT::ENTRY, MIM::OP_AGING_ON_CLASS_CHANGE,
			(*_display->string)["GAME_AGING_CLASS_CHANGE"], true,
			CFG::AGING_ON_CLASS_CHANGE,
			(*_display->string)["HINT_GAME_AGING_CLASS_CHANGE"]);
		_add_item(10, MIT::ENTRY, MIM::OP_PROTECT_TELEPORT,
			(*_display->string)["GAME_PROTECT_TELEPORT"], true,
			CFG::PROTECT_TELEPORT,
			(*_display->string)["HINT_GAME_PROTECT_TELEPORT"]);
		_add_item(11, MIT::ENTRY, MIM::OP_ALLOW_RANGED_WEAPONS,
			(*_display->string)["GAME_ALLOW_RANGED_WEAPONS"], true,
			CFG::ALLOW_RANGED_WEAPONS,
			(*_display->string)["HINT_GAME_ALLOW_RANGED_WEAPONS"]);
		_add_item(13, MIT::ENTRY, MIM::OP_BATCH_HEALING_AFTER_RETURN_TO_CASTLE,
			(*_display->string)["GAME_BATCH_HEALING_AFTER_RETURN"], true,
			CFG::BATCH_HEALING_AFTER_RETURN_TO_CASTLE,
			(*_display->string)["HINT_GAME_BATCH_HEALING_AFTER_RETURN"]);
		_add_item(14, MIT::ENTRY, MIM::OP_REROLL_ONES_ON_DICE,
			(*_display->string)["GAME_REROLL_ONES"], true,
			CFG::REROLL_ONES_ON_DICE,
			(*_display->string)["HINT_GAME_REROLL_ONES"]);
		_add_item(15, MIT::ENTRY, MIM::OP_ALLOW_LOST_LEGATED,
			(*_display->string)["GAME_ALLOW_LOST_LEGATED"], true,
			CFG::ALLOW_LOST_LEGATED,
			(*_display->string)["HINT_GAME_ALLOW_LOST_LEGATED"]);
		_add_item(16, MIT::ENTRY, MIM::OP_ALLOW_CURABLE_DRAIN,
			(*_display->string)["GAME_ALLOW_CURABLE_DRAIN"], true,
			CFG::ALLOW_CURABLE_DRAIN,
			(*_display->string)["HINT_GAME_ALLOW_CURABLE_DRAIN"]);
		_add_item(19, MIT::ENTRY, MIM::OP_CHARGED_ITEMS,
			(*_display->string)["GAME_CHARGED_ITEMS"], true, CFG::CHARGED_ITEMS,
			(*_display->string)["HINT_GAME_CHARGED_ITEMS"]);
		_add_item(20, MIT::SPACER, MIM::ITEM_SPACER,
			(*_display->string)["MENU_SPACER"]);
		_add_item(21, MIT::SAVE, MIM::ITEM_SAVE,
			(*_display->string)["MENU_OPTIONS_SAVE"]);
		_add_item(22, MIT::CANCEL, MIM::ITEM_CANCEL,
			(*_display->string)["MENU_OPTIONS_CANCEL"]);
		selected = items.begin();
		break;
	case MTP::PAUSE:
		[[fallthrough]];
	case MTP::CAMP:
		_add_item(0, MIT::ENTRY, MIM::CP_INSPECT,
			(*_display->string)["CAMP_INSPECT"]);
		_add_item(1, MIT::ENTRY, MIM::CP_REORDER,
			(*_display->string)["CAMP_REORDER"]);
		_add_item(
			2, MIT::ENTRY, MIM::CP_SAVE, (*_display->string)["CAMP_SAVE"]);
		_add_item(3, MIT::ENTRY, MIM::CP_OPTIONS,
			(*_display->string)["CAMP_OPTIONS"]);
		_add_item(
			4, MIT::ENTRY, MIM::ITEM_QUIT, (*_display->string)["CAMP_QUIT"]);
		_add_item(
			5, MIT::ENTRY, MIM::CP_LEAVE, (*_display->string)["CAMP_LEAVE"]);
		selected = items.begin();
		break;
	case MTP::SEARCH:
		_add_item(
			0, MIT::ENTRY, MIM::AC_LEAVE, (*_display->string)["ACTION_LEAVE"]);
		selected = items.begin();
		break;
	case MTP::ACTION:
		_add_item(0, MIT::ENTRY, MIM::CP_INSPECT,
			(*_display->string)["ACTION_INSPECT"]);
		_add_item(1, MIT::ENTRY, MIM::AC_CAST_SPELL,
			(*_display->string)["ACTION_SPELL"]);
		_add_item(
			2, MIT::ENTRY, MIM::AC_USE_ITEM, (*_display->string)["ACTION_USE"]);
		_add_item(4, MIT::ENTRY, MIM::AC_SEARCH_CHARACTERS,
			(*_display->string)["ACTION_SEARCH_CHARACTERS"]);
		_add_item(5, MIT::ENTRY, MIM::AC_SHOW_MAP,
			(*_display->string)["ACTION_FLOOR_MAP"]);
		_add_item(5, MIT::ENTRY, MIM::AC_SEARCH_ITEMS,
			(*_display->string)["ACTION_SEARCH_ITEMS"]);
		_add_item(6, MIT::ENTRY, MIM::AC_SEARCH_SECRET,
			(*_display->string)["ACTION_SEARCH_SECRET"]);
		_add_item(
			7, MIT::ENTRY, MIM::AC_LEAVE, (*_display->string)["ACTION_LEAVE"]);
		selected = items.begin();
		break;
	case MTP::MUSEUM:
		_add_all_items();
		selected = items.begin();
		break;
	case MTP::BESTIARY:
		_add_all_monsters();
		selected = items.begin();
		break;
	case MTP::SPELLBOOK:
		_add_all_spells();
		selected = items.begin();
		break;
	default:
		break;
	}
}

auto Sorcery::Menu::set_visible_size(const unsigned int value) -> void {

	_visible_size = value;
}

// Item List is special (but don't add Broken Item)
auto Sorcery::Menu::_add_all_items() -> void {

	const auto item_types{_game->itemstore->get_all_types()};
	for (auto &item_type : item_types)
		if (item_type.get_type_id() != ITT::BROKEN_ITEM)
			_add_item(unenum(item_type.get_type_id()), MIT::ENTRY, MIM::MU_ITEM,
				item_type.get_display_name(), unenum(item_type.get_type_id()));
}

// Monster List is special
auto Sorcery::Menu::_add_all_monsters() -> void {

	const auto monster_types{_game->monsterstore->get_all_types()};
	for (auto &monster : monster_types)
		_add_item(unenum(monster.get_type_id()), MIT::ENTRY, MIM::MU_ITEM,
			monster.get_known_name(), unenum(monster.get_type_id()));
}

// Spellbook is special too
auto Sorcery::Menu::_add_all_spells() -> void {
}

// Item Menus
auto Sorcery::Menu::_add_inventory_items(
	const MIA mode, const unsigned int character_id) -> void {

	const auto items{_game->characters[character_id].inventory.items()};
	auto slot{1u};

	for (const auto &item : items) {
		const std::string flag{std::invoke([&] {
			if (!item.get_known())
				return "?";
			else if (item.get_cursed() && item.get_equipped())
				return "-";
			else if (item.get_equipped())
				return "*";
			else if (!item.get_usable())
				return "#";
			else
				return " ";
		})};

		const auto name{std::invoke([&] {
			if (mode == MIA::SELL || mode == MIA::UNCURSE ||
				mode == MIA::IDENTIFY)
				return fmt::format("{}){}{:<16}{:>10} GP", slot, flag,
					item.get_display_name(),
					_game->itemstore->sellable_price(item.get_type_id()));
			else
				return fmt::format(
					"{}){}{}", slot, flag, item.get_display_name());
		})};

		const auto enabled{std::invoke([&] {
			// We can't selecte Equipped Items
			if (mode == MIA::DROP || mode == MIA::TRADE || mode == MIA::SELL) {
				if (item.get_cursed() && item.get_equipped())
					return false;
				else if (item.get_equipped())
					return false;
				else
					return true;
			} else if (mode == MIA::EQUIP)
				return !item.get_equipped() && item.get_usable();
			else if (mode == MIA::UNCURSE)
				return item.get_cursed() && item.get_equipped();
			else if (mode == MIA::IDENTIFY)
				return !item.get_known();
			else if (mode == MIA::USE) {
				return item.get_known() &&
					   _game->itemstore->has_usable(item.get_type_id());
			} else if (mode == MIA::INVOKE) {
				return item.get_known() &&
					   _game->itemstore->has_invokable(item.get_type_id());
			} else
				return false;
		})};

		if (enabled)
			_add_item(slot - 1, MIT::ENTRY, MIM::MU_ITEM, name);
		else
			_add_item_disabled(slot - 1, MIT::ENTRY, MIM::MU_ITEM, name);

		++slot;
	}

	_add_item(++slot, MIT::SPACER, MIM::ITEM_SPACER,
		(*_display->string)["MENU_SPACER"]);
	_add_item(++slot, MIT::CANCEL, MIM::AC_LEAVE,
		(*_display->string)["ACTION_CANCEL"]);
}

// The Character Menu is a special case and needs to be reloaded often when
// names and classes change
auto Sorcery::Menu::reload() -> void {

	// Clear the Items
	items.clear();
	bounds.clear();
	count = 0;
	_texts.clear();
	_options.clear();

	// Now depending on the menu type, add the relevant items
	switch (_type) {
	case MTP::CHARACTER_ROSTER:
		[[fallthrough]];
	case MTP::PARTY_CHARACTERS:
		[[fallthrough]];
	case MTP::AVAILABLE_CHARACTERS:
		[[fallthrough]];
	case MTP::INVALID_CHARACTERS:
		[[fallthrough]];
	case MTP::BUY_SELL_CHARACTERS:
		[[fallthrough]];
	case MTP::CURSED_CHARACTERS:
		[[fallthrough]];
	case MTP::IDENTIFY_CHARACTERS:
		[[fallthrough]];
	case MTP::CHARACTERS_HERE:
		_populate_chars();
		selected = items.begin();
		break;
	default:
		break;
	}
}

// Overload [] Operator
auto Sorcery::Menu::operator[](const unsigned int index) -> MenuEntry & {

	return items.at(index);
}

auto Sorcery::Menu::get_type() const -> MTP {

	return _type;
}

// Add an item to the Menu
auto Sorcery::Menu::_add_item(
	int index, const MIT itemtype, const MIM code, std::string key) -> void {

	// Note passing key by value as we are modifying the key here
	if (key.length() % 2 == 0)
		key.resize(key.length() + 1, 32);

	auto hint{""s};
	items.emplace_back(static_cast<unsigned int>(index), itemtype, code, key,
		true, CFG::NONE, hint);
	++count;
}

auto Sorcery::Menu::_add_item_disabled(
	int index, const MIT itemtype, const MIM code, std::string key) -> void {

	// Note passing key by value as we are modifying the key here
	if (key.length() % 2 == 0)
		key.resize(key.length() + 1, 32);

	auto hint{""s};
	items.emplace_back(static_cast<unsigned int>(index), itemtype, code, key,
		false, CFG::NONE, hint);
	++count;
}

auto Sorcery::Menu::_add_item(int index, const MIT itemtype, const MIM code,
	std::string key, unsigned int idx) -> void {

	// Note passing key by value as we are modifying the key here
	if (key.length() % 2 == 0)
		key.resize(key.length() + 1, 32);

	auto hint{""s};
	items.emplace_back(
		static_cast<unsigned int>(index), itemtype, code, key, true, idx);
	++count;
}

// Add an item to the Menu
auto Sorcery::Menu::_add_item(const int index, const MIT itemtype,
	const MIM code, std::string key, const bool enabled, const CFG option,
	const std::string &hint) -> void {

	// Note passing key by value as we are modifying the key here
	if (key.length() % 2 == 0)
		key.resize(key.length() + 1, 32);

	items.emplace_back(static_cast<unsigned int>(index), itemtype, code, key,
		enabled, option, hint);
	++count;
}

// Select the first enabled menu item
auto Sorcery::Menu::_select_first() -> MenuSelect {

	auto it{std::ranges::find_if(
		items.begin(), items.end(), [&](const auto &menu_item) {
			return (((menu_item.type == MIT::ENTRY) ||
						(menu_item.type == MIT::SAVE) ||
						(menu_item.type == MIT::CANCEL)) &&
					(menu_item.enabled));
		})};

	if (it != items.end()) {
		selected = it;
		return it;
	} else
		return std::nullopt;
}

// Select the last enabled menu item
auto Sorcery::Menu::_select_last() -> MenuSelect {

	// Would be nice to use a ranges reverse view to handle this, or a
	// std::find_last_if, instead we have to do a forward iterator backwards
	// since we can't use a backwards iterator either!
	for (std::vector<MenuEntry>::const_iterator it = items.end() - 1;
		it != items.begin(); --it)
		if (((*it).type == MIT::ENTRY || (*it).type == MIT::SAVE ||
				(*it).type == MIT::CANCEL) &&
			(*it).enabled) {
			selected = it;
			if (_type == MTP::MUSEUM) {

				// Change the working window
			}
			return selected;
		}

	return std::nullopt;
}

// Check if the mouse cursor is on a menu item, and if so set it
auto Sorcery::Menu::set_mouse_selected(sf::Vector2f mouse_pos) -> MenuSelect {

	if (!bounds.empty()) {

		// Look for the bounds the mouse cursor is in, but select and return
		// the associated item with the same index, since both containers
		// track each other
		const sf::Vector2f global_pos{this->getPosition()};
		mouse_pos -= global_pos;
		auto it{std::ranges::find_if(
			bounds.begin(), bounds.end(), [&mouse_pos](const auto &item) {
				return item.contains(mouse_pos);
			})};
		if (it != bounds.end()) {
			auto dist{std::distance(bounds.begin(), it)};
			auto candidate{_visible_items.begin() + dist};
			if (candidate->enabled) {
				selected = _choose_by_index(candidate->index).value();
				return selected;
			} else
				return std::nullopt;
		} else
			return std::nullopt;
	}

	// If we reach here it means that bounds (which requites a draw to take
	// place, hasn't been populated yet)
	return std::nullopt;
}

// Set selected based upon the item passed in
auto Sorcery::Menu::choose(std::any option) -> MenuSelect {

	MIM search_for{MIM::NO_MENU_ITEM};
	switch (_type) {
	case MTP::CHOOSE_METHOD:
		switch (std::any_cast<CRM>(option)) {
		case CRM::FULL:
			search_for = MIM::CM_FULL;
			break;
		case CRM::QUICK:
			search_for = MIM::CM_QUICK;
			break;
		case CRM::RANDOM:
			search_for = MIM::CM_RANDOM;
			break;
		default:
			break;
		}
		break;
	case MTP::CHOOSE_CHARACTER_RACE:
		switch (std::any_cast<CHR>(option)) {
		case CHR::DWARF:
			search_for = MIM::CR_DWARF;
			break;
		case CHR::ELF:
			search_for = MIM::CR_ELF;
			break;
		case CHR::GNOME:
			search_for = MIM::CR_GNOME;
			break;
		case CHR::HOBBIT:
			search_for = MIM::CR_HOBBIT;
			break;
		case CHR::HUMAN:
			search_for = MIM::CR_HUMAN;
			break;
		default:
			break;
		}
		break;
	case MTP::CHOOSE_CHARACTER_ALIGNMENT:
		switch (std::any_cast<CAL>(option)) {
		case CAL::EVIL:
			search_for = MIM::CA_EVIL;
			break;
		case CAL::GOOD:
			search_for = MIM::CA_GOOD;
			break;
		case CAL::NEUTRAL:
			search_for = MIM::CA_NEUTRAL;
			break;
		default:
			break;
		}
		break;
	case MTP::CHOOSE_CHARACTER_CLASS:
		switch (std::any_cast<CHC>(option)) {
		case CHC::SAMURAI:
			search_for = MIM::CC_SAMURAI;
			break;
		case CHC::FIGHTER:
			search_for = MIM::CC_FIGHTER;
			break;
		case CHC::LORD:
			search_for = MIM::CC_LORD;
			break;
		case CHC::THIEF:
			search_for = MIM::CC_THIEF;
			break;
		case CHC::NINJA:
			search_for = MIM::CC_NINJA;
			break;
		case CHC::PRIEST:
			search_for = MIM::CC_PRIEST;
			break;
		case CHC::BISHOP:
			search_for = MIM::CC_BISHOP;
			break;
		case CHC::MAGE:
			search_for = MIM::CC_MAGE;
			break;
		default:
			break;
		}
		break;
	default:
		return std::nullopt;
		break;
	}

	auto it{std::ranges::find_if(items.begin(), items.end(),
		[&](const auto &item) { return item.item == search_for; })};
	if (it != items.end()) {
		selected = it;
		return selected;
	} else
		return std::nullopt;
}

auto Sorcery::Menu::_choose_by_index(const unsigned int index) -> MenuSelect {

	// Iterate through til we have found the item with the associated index
	if (auto it{std::ranges::find_if(items.begin(), items.end(),
			[&](const auto &item) { return item.index == index; })};
		it != items.end())
		return it;
	else
		return std::nullopt;

	// If we reach here the mouse cursor is outside the items so we don't do
	// anything
	return std::nullopt;
}

// Set selected based upon the item index
auto Sorcery::Menu::choose(const unsigned int index) -> MenuSelect {

	// Iterate through til we have found the item with the associated index
	if (auto it{std::ranges::find_if(items.begin(), items.end(),
			[&](const auto &item) { return item.index == index; })};
		it != items.end())
		return it;
	else
		return std::nullopt;

	// If we reach here the mouse cursor is outside the items so we don't do
	// anything
	return std::nullopt;
}

auto Sorcery::Menu::choose_first() -> MenuSelect {

	return _select_first();
}

auto Sorcery::Menu::choose_last() -> MenuSelect {

	return _select_last();
}

// Choose the previous selected item
auto Sorcery::Menu::choose_previous() -> MenuSelect {

	if (selected > items.begin()) {

		// Repeat the comment from above, that it would be nice to use a
		// ranges reverse view to handle this, or a std::find_last_if,
		// instead we have to do a forward iterator backwards since we can't
		// use a backwards iterator either!

		// Iterate backwards until we find the first previous enabled menu
		// if we can
		bool found{false};
		std::vector<MenuEntry>::const_iterator working{selected};
		do {
			--working;
			found =
				((*working).enabled) && (((*working).type == MIT::ENTRY) ||
											((*working).type == MIT::SAVE) ||
											((*working).type == MIT::CANCEL));
		} while ((working >= items.begin()) && (!found));
		if (found) {
			selected = working;
			return selected;
		}
	} else
		return selected;

	// TODO: need to change _visible_items range if we have to

	return std::nullopt;
}

// Choose the next selected item
auto Sorcery::Menu::choose_next() -> MenuSelect {

	if (selected < (items.end() - 1)) {

		// Iterate forwards until we find the first next enabled menu if we
		// can
		bool found{false};
		std::vector<MenuEntry>::const_iterator working{selected};
		do {
			++working;
			found =
				((*working).enabled) && (((*working).type == MIT::ENTRY) ||
											((*working).type == MIT::SAVE) ||
											((*working).type == MIT::CANCEL));
		} while ((working <= items.end() - 1) && (!found));
		if (found) {

			selected = working;
			return selected;
		}
	} else
		return selected;

	// TODO: need to change _visible_items range if we have to

	return std::nullopt;
}

// TODO: optimise this so that it isn't created on every refresh!
auto Sorcery::Menu::generate(const Component &component, bool force_refresh)
	-> void {

	// Figure out if we can display all the items to begin with or just a
	// moving "window"
	if (_type != MTP::MUSEUM && _type != MTP::BESTIARY) {
		_visible_items = items;
	} else {
		auto current{static_cast<unsigned int>(
			std::distance<std::vector<MenuEntry>::const_iterator>(
				items.begin(), selected))};

		if (current < component.h) {

			// In this case, only display the first part of the vector of
			// items as we haven't reached the bottom yet
			_visible_items = std::span<Sorcery::MenuEntry>(
				items.begin(), items.begin() + component.h);
		} else {

			// Otherwise scroll the items appropriately with the selected
			// item at the bottom
			const auto start_span{items.begin() + current + 1 - component.h};
			const auto end_span{items.begin() + current + 1};
			_visible_items =
				std::span<Sorcery::MenuEntry>(start_span, end_span);
		}
	}

	auto do_refresh{force_refresh || _texts.empty()};
	if (do_refresh) {

		// In case we are generating the Options Menu
		const Component on_c{(*_display->layout)["options:on"]};
		const Component off_c{(*_display->layout)["options:off"]};

		// Work out total size of texture needed
		const auto texture_w{component.w * _display->window->get_cw()};
		const auto texture_h{component.h * _display->window->get_ch()};
		const sf::Vector2f texture_size(texture_w, texture_h);
		_rtexture.create(texture_size.x, texture_size.y);

		_width = texture_size.x;
		_height = texture_size.y;

		// Bounds are generated for each menu item to handle mouse over
		_texts.clear();
		_options.clear();
		bounds.clear();
		auto index{0};
		auto entry_y{0};
		for (const auto &item : _visible_items) {

			auto current{_visible_items.begin() + index};
			if (item.type == MIT::TEXT || item.type == MIT::ENTRY ||
				item.type == MIT::SAVE || item.type == MIT::CANCEL) {
				auto text_string{item.key};
				sf::Text text{};
				text.setFont(_system->resources->fonts[component.font]);
				text.setCharacterSize(component.size);
				if (_display->get_bold())
					text.setStyle(sf::Text::Bold);
				if (item.enabled)
					text.setFillColor(sf::Color(component.colour));
				else
					text.setFillColor(
						sf::Color(0x606060ff)); // TODO change this to be
												// located in the layout file
				if (_display->get_upper())
					std::transform(text_string.begin(), text_string.end(),
						text_string.begin(), ::toupper);
				text.setString(text_string);

				// Check for alignment and set location appropriately
				auto entry_x{(component.justification == JUS::CENTRE)
								 ? texture_w / 2
								 : 0};
				text.setPosition(entry_x - text.getLocalBounds().left,
					entry_y - text.getLocalBounds().top);

				// If we have a selected entry, change the background colour
				if ((*selected).index == (*current).index) {

					// See
					// https://stackoverflow.com/questions/14505571/centering-text-on-the-screen-with-sfml/15253837#15253837
					sf::RectangleShape bg(
						sf::Vector2f(component.w * _display->window->get_cw(),
							_system->resources->get_font_height(
								component.font, component.size)));
					bg.setPosition(0, entry_y);
					if (component.animated)
						bg.setFillColor(_graphics->animation->select_col);
					else
						bg.setFillColor(sf::Color(component.background));

					text.setFillColor(sf::Color(component.colour));
					text.setOutlineColor(sf::Color(0, 0, 0));
					text.setOutlineThickness(1);

					_selected_bg = bg;
				}

				// Handle Justification
				if (_type == MTP::OPTIONS ||
					_type == MTP::ALLOCATE_CHARACTER_ATTRIBUTES) {
					if (item.type == MIT::ENTRY) {
						if (component.justification == JUS::CENTRE)
							text.setOrigin(
								text.getLocalBounds().width / 2.0f, 0);
					} else if (item.type == MIT::SAVE ||
							   item.type == MIT::CANCEL) {
						entry_x =
							(component.width * _display->window->get_cw()) / 2;
						text.setPosition(entry_x, entry_y);
						text.setOrigin(text.getLocalBounds().width / 2.0f, 0);
					}
				} else if (_type == MTP::TEMPLE) {

					// Override Justification for Invalid Characters so that
					// it looks better
					if (item.item == MIM::IC_CHARACTER)
						text.setOrigin(0, 0);
					else if (component.justification == JUS::CENTRE)
						text.setOrigin(text.getLocalBounds().width / 2.0f, 0);
				} else {
					if (component.justification == JUS::CENTRE)
						text.setOrigin(text.getLocalBounds().width / 2.0f, 0);
				}

				_texts.emplace_back(text);

				// Now handle the mouse move/select!
				if (item.type == MIT::ENTRY || item.type == MIT::SAVE ||
					item.type == MIT::CANCEL) {
					const sf::FloatRect actual_rect{text.getGlobalBounds()};
					bounds.push_back(actual_rect);
				} else {
					const sf::FloatRect actual_rect;
					bounds.push_back(actual_rect);
				}

				// Add options in case of the Options Menu
				if (_type == MTP::OPTIONS && item.type == MIT::ENTRY) {
					auto option_y{entry_y};
					auto option_x{component.w * _display->window->get_cw()};
					const auto option_value{
						(*_system->config)[item.config] ? true : false};
					sf::Text option_text{};
					if (option_value) {

						// On
						option_text.setFont(
							_system->resources->fonts[on_c.font]);
						option_text.setCharacterSize(on_c.size);
						if (_display->get_bold())
							option_text.setStyle(sf::Text::Bold);
						option_text.setFillColor(sf::Color(on_c.colour));
						option_text.setString(
							(*_display->string)[on_c.string_key]);
						sf::FloatRect bounds{option_text.getGlobalBounds()};
						option_text.setPosition(
							option_x - bounds.width, option_y);
					} else {

						// Off
						option_text.setFont(
							_system->resources->fonts[off_c.font]);
						option_text.setCharacterSize(off_c.size);
						option_text.setFillColor(sf::Color(off_c.colour));
						option_text.setString(
							(*_display->string)[off_c.string_key]);
						sf::FloatRect bounds{option_text.getGlobalBounds()};
						option_text.setPosition(
							option_x - bounds.width, option_y);
					}

					if ((*selected).index == (*current).index) {
						option_text.setOutlineColor(sf::Color(0, 0, 0));
						option_text.setOutlineThickness(1);
					}

					_options.emplace_back(option_text);
				}

				entry_y += _display->window->get_ch();
			} else {
				sf::Text empty_text{};
				const sf::FloatRect actual_rect{};
				bounds.push_back(actual_rect);
				_texts.emplace_back(empty_text);
				entry_y += _display->window->get_ch();
			}
			++index;
		}
	} else {

		// Only change what needs to be changed
		auto entry_y{0};
		auto index{0};
		auto options_index{0};
		auto option_x{0};
		auto option_y{0};
		const Component on_c{(*_display->layout)["options:on"]};
		const Component off_c{(*_display->layout)["options:off"]};
		for (const auto &item : _visible_items) {
			auto current{_visible_items.begin() + index};
			if (item.type == MIT::TEXT || item.type == MIT::ENTRY ||
				item.type == MIT::SAVE || item.type == MIT::CANCEL) {
				if ((*selected).index == (*current).index) {
					const sf::FloatRect bg_rect{
						_texts.at(index).getGlobalBounds()};
					sf::RectangleShape bg(
						sf::Vector2f(component.w * _display->window->get_cw(),
							bg_rect.height));
					bg.setPosition(0, entry_y);
					if (component.animated)
						bg.setFillColor(_graphics->animation->select_col);
					else
						bg.setFillColor(sf::Color(component.background));

					if (item.enabled)
						_texts.at(index).setFillColor(
							sf::Color(component.colour));
					else
						_texts.at(index).setFillColor(sf::Color(0x606060ff));
					_texts.at(index).setOutlineColor(sf::Color(0, 0, 0));
					_texts.at(index).setOutlineThickness(1);

					_selected_bg = bg;
				}

				if (_type == MTP::OPTIONS && item.type == MIT::ENTRY) {

					option_y = entry_y;
					option_x = component.w * _display->window->get_cw();
					if (const bool option_value{
							(*_system->config)[item.config] ? true : false};
						option_value) {

						// On
						_options.at(options_index)
							.setString((*_display->string)[on_c.string_key]);
						sf::FloatRect bounds{
							_options.at(options_index).getGlobalBounds()};
						_options.at(options_index)
							.setPosition(option_x - bounds.width, option_y);
						_options.at(options_index)
							.setFillColor(sf::Color(on_c.colour));
					} else {

						// Off
						_options.at(options_index)
							.setString((*_display->string)[off_c.string_key]);
						sf::FloatRect bounds{
							_options.at(options_index).getGlobalBounds()};
						_options.at(options_index)
							.setPosition(option_x - bounds.width, option_y);
						_options.at(options_index)
							.setFillColor(sf::Color(off_c.colour));
					}

					if ((*selected).index == (*current).index) {
						_options.at(options_index)
							.setOutlineColor(sf::Color(0, 0, 0));
						_options.at(options_index).setOutlineThickness(1);
					}
					++options_index;
				}
			}
			++index;
			entry_y += _display->window->get_ch();
		}
	}
}

auto Sorcery::Menu::draw(
	sf::RenderTarget &target, sf::RenderStates states) const -> void {

	states.transform *= getTransform();

	target.draw(_selected_bg, states);
	for (auto &text : _texts)
		target.draw(text, states);

	if (_type == MTP::OPTIONS)
		for (auto &option : _options)
			target.draw(option, states);
}

auto Sorcery::Menu::_populate_trade_chars(const unsigned int current_char)
	-> void {

	items.clear();
	bounds.clear();
	count = 0;
	_texts.clear();
	_options.clear();
	auto max_id{0};
	if (_game->state->party_has_members()) {
		_add_item(++max_id, MIT::TEXT, MIM::NC_WARNING,
			(*_display->string)["ACTION_TRADE_TO"]);
		_add_item(++max_id, MIT::SPACER, MIM::ITEM_SPACER,
			(*_display->string)["MENU_SPACER"]);
		auto party{_game->state->get_party_characters()};
		for (auto id : party) {
			if (id != current_char) {
				auto slots_free{
					_game->characters[id].inventory.get_empty_slots()};
				auto text{fmt::format(
					"{} ({})", _game->characters[id].get_name(), slots_free)};
				if (slots_free == 0)
					_add_item_disabled(id, MIT::ENTRY, MIM::IC_CHARACTER, text);
				else
					_add_item(id, MIT::ENTRY, MIM::IC_CHARACTER, text);
				++max_id;
			}
		}
	} else
		_add_item(++max_id, MIT::TEXT, MIM::NC_WARNING,
			(*_display->string)["MENU_NO_CHARACTERS"]);

	_add_item(++max_id, MIT::SPACER, MIM::ITEM_SPACER,
		(*_display->string)["MENU_SPACER"]);
	_add_item(++max_id, MIT::CANCEL, MIM::AC_LEAVE,
		(*_display->string)["ACTION_CANCEL"]);
}

auto Sorcery::Menu::_populate_chars() -> void {

	_go_first = true;
	items.clear();
	bounds.clear();
	count = 0;
	_texts.clear();
	_options.clear();
	auto max_id{0};
	switch (_type) {
	case MTP::CHARACTER_ROSTER: {
		if (!_game->characters.empty()) {
			for (auto &[id, character] : _game->characters) {
				if (_mode.value() == MMD::TAVERN) {
					if (character.get_location() == CHL::TAVERN ||
						character.get_location() == CHL::PARTY) {
						_add_item(id, MIT::ENTRY, MIM::IC_CHARACTER,
							character.get_summary());
						++max_id;
					}
				} else if (_mode.value() == MMD::INN) {
					if (character.get_location() == CHL::PARTY) {
						_add_item(id, MIT::ENTRY, MIM::IC_CHARACTER,
							character.get_summary());
						++max_id;
					}
				} else if (_mode.value() == MMD::TEMPLE) {
					if (character.get_location() == CHL::TEMPLE ||
						character.get_location() == CHL::PARTY) {
						_add_item(id, MIT::ENTRY, MIM::IC_CHARACTER,
							character.get_summary());
						++max_id;
					}
				} else if (_mode.value() == MMD::TRAINING) {
					_add_item(id, MIT::ENTRY, MIM::IC_CHARACTER,
						character.get_summary_and_out());
					++max_id;
				}
			}
		} else
			_add_item(++max_id, MIT::TEXT, MIM::NC_WARNING,
				(*_display->string)["MENU_NO_CHARACTERS"]);

		if (_mode) {
			if (_mode.value() == MMD::TRAINING) {
				_add_item(++max_id, MIT::SPACER, MIM::ITEM_SPACER,
					(*_display->string)["MENU_SPACER"]);
				_add_item(++max_id, MIT::ENTRY, MIM::ET_TRAIN,
					(*_display->string)["MENU_TRAIN"]);
			} else if (_mode.value() == MMD::TAVERN) {
				_add_item(++max_id, MIT::SPACER, MIM::ITEM_SPACER,
					(*_display->string)["MENU_SPACER"]);
				_add_item(++max_id, MIT::ENTRY, MIM::CA_TAVERN,
					(*_display->string)["MENU_TAVERN"]);
			} else if (_mode.value() == MMD::INN) {
				_add_item(++max_id, MIT::SPACER, MIM::ITEM_SPACER,
					(*_display->string)["MENU_SPACER"]);
				_add_item(++max_id, MIT::ENTRY, MIM::CA_TAVERN,
					(*_display->string)["MENU_INN"]);
			} else if (_mode.value() == MMD::TEMPLE) {
				_add_item(++max_id, MIT::SPACER, MIM::ITEM_SPACER,
					(*_display->string)["MENU_SPACER"]);
				_add_item(++max_id, MIT::ENTRY, MIM::CA_TAVERN,
					(*_display->string)["MENU_TEMPLE"]);
			}
		}

	} break;
	case MTP::CHARACTERS_HERE: {
		if (_game->state->get_party_size() < 6) {

			// Check for any characters in same square
			bool found{false};
			for (const auto &[id, character] : _game->characters) {
				if (character.get_location() == CHL::MAZE &&
					character.coordinate.value() ==
						_game->state->get_player_pos() &&
					character.depth.value() == _game->state->get_depth()) {
					found = true;
					break;
				}
			}
			if (found) {

				_add_item(++max_id, MIT::TEXT, MIM::NC_WARNING,
					(*_display->string)["ACTION_FOUND_1"]);
				_add_item(++max_id, MIT::TEXT, MIM::NC_WARNING,
					(*_display->string)["ACTION_FOUND_2"]);
				_add_item(++max_id, MIT::SPACER, MIM::ITEM_SPACER,
					(*_display->string)["MENU_SPACER"]);
				for (const auto &[id, character] : _game->characters) {
					if (character.get_location() == CHL::MAZE &&
						character.coordinate.value() ==
							_game->state->get_player_pos() &&
						character.depth.value() == _game->state->get_depth())
						_add_item(id, MIT::ENTRY, MIM::IC_CHARACTER,
							_game->characters[id].get_name_and_status());
					++max_id;
				}
			} else {
				_add_item(++max_id, MIT::TEXT, MIM::NC_WARNING,
					(*_display->string)["ACTION_FOUND_1"]);
				_add_item(++max_id, MIT::TEXT, MIM::NC_WARNING,
					(*_display->string)["ACTION_FOUND_2"]);
				_add_item(++max_id, MIT::SPACER, MIM::ITEM_SPACER,
					(*_display->string)["MENU_SPACER"]);
				_add_item(++max_id, MIT::TEXT, MIM::NC_WARNING,
					(*_display->string)["MENU_NO_CHARACTERS_HERE"]);
			}
		} else {
			_add_item(++max_id, MIT::TEXT, MIM::NC_WARNING,
				(*_display->string)["ACTION_FOUND_1"]);
			_add_item(++max_id, MIT::TEXT, MIM::NC_WARNING,
				(*_display->string)["ACTION_FOUND_2"]);
			_add_item(++max_id, MIT::SPACER, MIM::ITEM_SPACER,
				(*_display->string)["MENU_SPACER"]);
			_add_item(++max_id, MIT::TEXT, MIM::NC_WARNING,
				(*_display->string)["PARTY_FULL"]);
		}

		_add_item(++max_id, MIT::SPACER, MIM::ITEM_SPACER,
			(*_display->string)["MENU_SPACER"]);
		_add_item(++max_id, MIT::CANCEL, MIM::AC_LEAVE,
			(*_display->string)["ACTION_LEAVE"]);
	} break;
	case MTP::RESTART_EXPEDITION: {
		_add_item(++max_id, MIT::TEXT, MIM::NC_WARNING,
			(*_display->string)["RESTART_TEXT_1"]);
		_add_item(++max_id, MIT::TEXT, MIM::NC_WARNING,
			(*_display->string)["RESTART_TEXT_2"]);
		_add_item(++max_id, MIT::SPACER, MIM::ITEM_SPACER,
			(*_display->string)["MENU_SPACER"]);
		auto possible{0U};
		for (const auto &[id, character] : _game->characters) {
			if (character.get_location() == CHL::MAZE &&
				character.get_status() == CHT::OK) {
				_add_item(id, MIT::ENTRY, MIM::IC_CHARACTER,
					_game->characters[id].get_name_and_loc());
				++possible;
				++max_id;
			}
		}
		if (possible == 0)
			_add_item(++max_id, MIT::TEXT, MIM::NC_WARNING,
				(*_display->string)["MENU_NO_CHARACTERS_IN_MAZE"]);
		_add_item(++max_id, MIT::SPACER, MIM::ITEM_SPACER,
			(*_display->string)["MENU_SPACER"]);
		_add_item(++max_id, MIT::CANCEL, MIM::TR_EDGE_OF_TOWN,
			(*_display->string)["RESTART_GROUNDS_MENU_OPTION_RETURN"]);
		_go_first = false;
	} break;
	case MTP::PARTY_CHARACTER_NAMES: {
		if (_game->state->party_has_members()) {
			auto party{_game->state->get_party_characters()};
			for (auto id : party) {
				_add_item(id, MIT::ENTRY, MIM::IC_CHARACTER,
					_game->characters[id].get_name());
				++max_id;
			}
		} else
			_add_item(++max_id, MIT::TEXT, MIM::NC_WARNING,
				(*_display->string)["MENU_NO_CHARACTERS"]);
	} break;
	case MTP::PARTY_CHARACTERS: {
		if (_game->state->party_has_members()) {
			auto party{_game->state->get_party_characters()};
			for (auto id : party) {
				_add_item(id, MIT::ENTRY, MIM::IC_CHARACTER,
					_game->characters[id].get_summary());
				++max_id;
			}
		} else
			_add_item(++max_id, MIT::TEXT, MIM::NC_WARNING,
				(*_display->string)["MENU_NO_CHARACTERS"]);

		if (_mode) {
			if (_mode.value() == MMD::TAVERN) {
				_add_item(++max_id, MIT::SPACER, MIM::ITEM_SPACER,
					(*_display->string)["MENU_SPACER"]);
				_add_item(++max_id, MIT::ENTRY, MIM::CA_TAVERN,
					(*_display->string)["MENU_TAVERN"]);
			} else if (_mode.value() == MMD::INN) {
				_add_item(++max_id, MIT::SPACER, MIM::ITEM_SPACER,
					(*_display->string)["MENU_SPACER"]);
				_add_item(++max_id, MIT::ENTRY, MIM::CA_INN,
					(*_display->string)["MENU_INN"]);
			} else if (_mode.value() == MMD::SHOP) {
				_add_item(++max_id, MIT::SPACER, MIM::ITEM_SPACER,
					(*_display->string)["MENU_SPACER"]);
				_add_item(++max_id, MIT::ENTRY, MIM::CA_SHOP,
					(*_display->string)["MENU_SHOP"]);
			} else if (_mode.value() == MMD::TEMPLE) {
				_add_item(++max_id, MIT::SPACER, MIM::ITEM_SPACER,
					(*_display->string)["MENU_SPACER"]);
				_add_item(++max_id, MIT::ENTRY, MIM::CA_TEMPLE,
					(*_display->string)["MENU_TEMPLE"]);
			} else if (_mode.value() == MMD::CAMP) {
				_add_item(++max_id, MIT::SPACER, MIM::ITEM_SPACER,
					(*_display->string)["MENU_SPACER"]);
				_add_item(++max_id, MIT::ENTRY, MIM::ITEM_CAMP,
					(*_display->string)["MENU_LEAVE"]);
			}
		}
	} break;
	case MTP::AVAILABLE_CHARACTERS: {
		auto count{0u};
		auto party{_game->state->get_party_characters()};
		for (auto &[id, character] : _game->characters) {
			if (std::ranges::find(party.begin(), party.end(), id) ==
				party.end()) {

				// TODO:: good and evil exclusion if in strict mode
				if (character.get_location() == CHL::TAVERN) {
					_add_item(id, MIT::ENTRY, MIM::IC_CHARACTER,
						character.get_summary());
					++max_id;
					++count;
				}
			}
		}

		if (count == 0) {
			_add_item(++max_id, MIT::TEXT, MIM::NC_WARNING,
				(*_display->string)["MENU_NO_MORE_CHARACTERS"]);
		}

		if (_mode) {
			if (_mode.value() == MMD::TAVERN) {
				_add_item(++max_id, MIT::SPACER, MIM::ITEM_SPACER,
					(*_display->string)["MENU_SPACER"]);
				_add_item(++max_id, MIT::ENTRY, MIM::CA_TAVERN,
					(*_display->string)["MENU_TAVERN"]);
			}
		}
	} break;
	case MTP::INVALID_CHARACTERS: {
		auto count{0u};
		auto last_id{0u};
		if (!_game->characters.empty()) {
			for (const auto &[id, character] : _game->characters) {
				if (character.get_status() != CHT::OK &&
					character.get_status() != CHT::LOST &&
					character.get_location() == CHL::TEMPLE) {
					const auto status{character.get_name_and_status()};
					_add_item(id, MIT::ENTRY, MIM::IC_CHARACTER, status);
					++count;
					if (id > last_id)
						last_id = id;
				}
			}
			++last_id;
		}

		if (count == 0) {
			_add_item(++max_id, MIT::TEXT, MIM::NC_WARNING,
				(*_display->string)["MENU_NO_HELP_CHARACTERS"]);
		}

		if (_mode) {
			if (_mode.value() == MMD::TEMPLE) {
				_add_item(++max_id, MIT::SPACER, MIM::ITEM_SPACER,
					(*_display->string)["MENU_SPACER"]);
				_add_item(++max_id, MIT::ENTRY, MIM::CA_TEMPLE,
					(*_display->string)["MENU_TEMPLE"]);
			}
		}
	} break;
	default:
		break;
	}
}

auto Sorcery::Menu::get_by_index(unsigned int index)
	-> std::optional<std::vector<MenuEntry>::iterator> {

	auto it{std::ranges::find_if(
		items.begin(), items.end(), [index](const auto &menu_item) {
			return (((menu_item.type == MIT::ENTRY) ||
						(menu_item.type == MIT::SAVE) ||
						(menu_item.type == MIT::CANCEL)) &&
					(menu_item.index == index));
		})};

	if (it != items.end()) {
		selected = it;
		return it;
	} else
		return std::nullopt;
}

auto Sorcery::Menu::num_enabled() -> unsigned int {

	return std::ranges::count_if(items.begin(), items.end(),
		[](const auto &menu_item) { return menu_item.enabled; });
}

auto Sorcery::Menu::num_disabled() -> unsigned int {

	return std::ranges::count_if(items.begin(), items.end(),
		[](const auto &menu_item) { return menu_item.enabled == false; });
}

auto Sorcery::Menu::enable_entry(const Component &component, unsigned int index)
	-> void {

	auto entry{items.begin() + index};
	auto current{(*entry).enabled};
	if (current == false) {
		(*entry).enabled = true;
		if (_texts.size() >= index)
			_texts.at(index).setFillColor(sf::Color(component.colour));
	}
}

auto Sorcery::Menu::disable_by_index(
	__attribute__((unused)) const Component &component, unsigned int index)
	-> void {

	auto it{std::ranges::find_if(
		items.begin(), items.end(), [index](const auto &menu_item) {
			return (
				(menu_item.type == MIT::ENTRY) && (menu_item.index == index));
		})};

	if (it != items.end()) {
		(*it).enabled = false;
	}
}

auto Sorcery::Menu::disable_entry(
	__attribute__((unused)) const Component &component, unsigned int index)
	-> void {

	auto entry{items.begin() + index};
	auto current{(*entry).enabled};
	if (current) {
		(*entry).enabled = false;
		if (_texts.size() >= index)
			_texts.at(index).setFillColor(sf::Color(0x606060ff));
	}
}

auto Sorcery::Menu::get_width() const -> unsigned int {

	return _width;
}

auto Sorcery::Menu::get_height() const -> unsigned int {

	return _height;
}

auto Sorcery::Menu::get_size() const -> sf::Vector2f {

	return sf::Vector2f{_width * 1.0f, _height * 1.0f};
}

auto Sorcery::Menu::print() -> void {

	std::string title{magic_enum::enum_name<MTP>(_type)};
	auto body{title + "\n\n"s};
	int slot{0};

	for (const auto &item : items) {

		std::string entry{item.key};
		std::string flag{item.enabled ? "Yes" : " No"};
		auto line{fmt::format("{} ({}) {}", slot, flag, entry)};
		body.append(line);
		body.append("\n");
		++slot;
	}

	std::cout << body << std::endl;
}
