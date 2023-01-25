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

#include "menu.hpp"

// Standard Constructor
Sorcery::Menu::Menu(
	System *system, Display *display, Graphics *graphics, Game *game, const MenuType type, std::optional<MenuMode> mode)
	: _system{system}, _display{display}, _graphics{graphics}, _game{game}, _type{type}, _mode{mode} {

	// Clear the Items
	items.clear();
	bounds.clear();
	count = 0;
	_texts.clear();
	_options.clear();

	// Now depending on the menu type, add the relevant items
	switch (_type) {
	case MenuType::CHARACTER_ROSTER:
	case MenuType::PARTY_CHARACTERS:
	case MenuType::AVAILABLE_CHARACTERS:
	case MenuType::INVALID_CHARACTERS:
	case MenuType::PARTY_CHARACTER_NAMES:
	case MenuType::RESTART_EXPEDITION:
	case MenuType::CHARACTERS_HERE:
		_populate_chars();
		selected = items.begin();
		break;
	case MenuType::EDIT_CHARACTER:
		_add_item(
			0, MenuItemType::ENTRY, MenuItem::EC_CHANGE_CLASS, (*_display->string)["EDIT_CHARACTER_CHANGE_CLASS"]);
		_add_item(1, MenuItemType::ENTRY, MenuItem::EC_CHANGE_NAME, (*_display->string)["EDIT_CHARACTER_CHANGE_NAME"]);
		_add_item(2, MenuItemType::ENTRY, MenuItem::EC_LEGATE_CHARACTER,
			(*_display->string)["EDIT_CHARACTER_LEGATE_CHARACTER"]);
		_add_item(3, MenuItemType::SPACER, MenuItem::SPACER, (*_display->string)["MENU_SPACER"]);
		_add_item(4, MenuItemType::ENTRY, MenuItem::EC_RETURN_EDIT, (*_display->string)["EDIT_CHARACTER_RETURN_EDIT"]);
		selected = items.begin();
		break;
	case MenuType::MAIN:
		_add_item(0, MenuItemType::ENTRY, MenuItem::MM_NEW_GAME, (*_display->string)["MAIN_MENU_OPTION_START"]);
		_add_item(1, MenuItemType::ENTRY, MenuItem::MM_CONTINUE_GAME, (*_display->string)["MAIN_MENU_OPTION_CONTINUE"]);
		_add_item(2, MenuItemType::ENTRY, MenuItem::MM_OPTIONS, (*_display->string)["MAIN_MENU_OPTION_OPTIONS"]);
		_add_item(3, MenuItemType::ENTRY, MenuItem::MM_COMPENDIUM, (*_display->string)["MAIN_MENU_OPTION_COMPENDIUM"]);
		_add_item(4, MenuItemType::ENTRY, MenuItem::MM_LICENSE, (*_display->string)["MAIN_MENU_OPTION_LICENSE"]);
		_add_item(5, MenuItemType::ENTRY, MenuItem::QUIT, (*_display->string)["MAIN_MENU_OPTION_EXIT"]);
		selected = items.begin();
		break;
	case MenuType::REVIEW_AND_CONFIRM:
		_add_item(0, MenuItemType::ENTRY, MenuItem::RC_ACCEPT, (*_display->string)["RC_ACCEPT"]);
		_add_item(1, MenuItemType::ENTRY, MenuItem::RC_REJECT, (*_display->string)["RC_REJECT"]);
		selected = items.begin();
		break;
	case MenuType::ELEVATOR_A_D:
		_add_item(0, MenuItemType::TEXT, MenuItem::NC_WARNING, (*_display->string)["BUTTONS_A_TO_D"]);
		_add_item(1, MenuItemType::SPACER, MenuItem::SPACER, (*_display->string)["MENU_SPACER"]);
		_add_item(2, MenuItemType::ENTRY, MenuItem::EL_A, (*_display->string)["ELEVATOR_A"]);
		_add_item(3, MenuItemType::ENTRY, MenuItem::EL_B, (*_display->string)["ELEVATOR_B"]);
		_add_item(4, MenuItemType::ENTRY, MenuItem::EL_C, (*_display->string)["ELEVATOR_C"]);
		_add_item(5, MenuItemType::ENTRY, MenuItem::EL_D, (*_display->string)["ELEVATOR_D"]);
		_add_item(6, MenuItemType::SPACER, MenuItem::SPACER, (*_display->string)["MENU_SPACER"]);
		_add_item(7, MenuItemType::ENTRY, MenuItem::EL_LEAVE, (*_display->string)["ELEVATOR_LEAVE"]);
		selected = items.end();
		break;
	case MenuType::ELEVATOR_A_F:
		_add_item(0, MenuItemType::TEXT, MenuItem::NC_WARNING, (*_display->string)["BUTTONS_A_TO_F"]);
		_add_item(1, MenuItemType::SPACER, MenuItem::SPACER, (*_display->string)["MENU_SPACER"]);
		_add_item(2, MenuItemType::ENTRY, MenuItem::EL_A, (*_display->string)["ELEVATOR_A"]);
		_add_item(3, MenuItemType::ENTRY, MenuItem::EL_B, (*_display->string)["ELEVATOR_B"]);
		_add_item(4, MenuItemType::ENTRY, MenuItem::EL_C, (*_display->string)["ELEVATOR_C"]);
		_add_item(5, MenuItemType::ENTRY, MenuItem::EL_D, (*_display->string)["ELEVATOR_D"]);
		_add_item(6, MenuItemType::ENTRY, MenuItem::EL_E, (*_display->string)["ELEVATOR_E"]);
		_add_item(7, MenuItemType::ENTRY, MenuItem::EL_F, (*_display->string)["ELEVATOR_F"]);
		_add_item(8, MenuItemType::SPACER, MenuItem::SPACER, (*_display->string)["MENU_SPACER"]);
		_add_item(9, MenuItemType::ENTRY, MenuItem::EL_LEAVE, (*_display->string)["ELEVATOR_LEAVE"]);
		selected = items.end();
		break;
	case MenuType::CASTLE:
		_add_item(0, MenuItemType::ENTRY, MenuItem::CA_TAVERN, (*_display->string)["MENU_TAVERN"]);
		_add_item(1, MenuItemType::ENTRY, MenuItem::CA_INN, (*_display->string)["MENU_INN"]);
		_add_item(2, MenuItemType::ENTRY, MenuItem::CA_SHOP, (*_display->string)["MENU_SHOP"]);
		_add_item(3, MenuItemType::ENTRY, MenuItem::CA_TEMPLE, (*_display->string)["MENU_TEMPLE"]);
		_add_item(4, MenuItemType::ENTRY, MenuItem::CA_EDGE_OF_TOWN, (*_display->string)["MENU_EDGE_OF_TOWN"]);
		selected = items.begin();
		break;
	case MenuType::EDGE_OF_TOWN:
		_add_item(0, MenuItemType::ENTRY, MenuItem::ET_CASTLE, (*_display->string)["MENU_CASTLE"]);
		_add_item(1, MenuItemType::ENTRY, MenuItem::ET_TRAIN, (*_display->string)["MENU_TRAIN"]);
		_add_item(2, MenuItemType::ENTRY, MenuItem::ET_MAZE, (*_display->string)["MENU_MAZE"]);
		_add_item(3, MenuItemType::ENTRY, MenuItem::ET_RESTART, (*_display->string)["MENU_RESTART"]);
		_add_item(4, MenuItemType::ENTRY, MenuItem::ET_LEAVE_GAME, (*_display->string)["MENU_LEAVE_GAME"]);
		selected = items.begin();
		break;
	case MenuType::INN_CHOOSE_BED:
		_add_item(0, MenuItemType::ENTRY, MenuItem::IN_STABLES, (*_display->string)["INN_1"]);
		_add_item(1, MenuItemType::ENTRY, MenuItem::IN_COT, (*_display->string)["INN_2"]);
		_add_item(2, MenuItemType::ENTRY, MenuItem::IN_ECONOMY, (*_display->string)["INN_3"]);
		_add_item(3, MenuItemType::ENTRY, MenuItem::IN_MERCHANT, (*_display->string)["INN_4"]);
		_add_item(4, MenuItemType::ENTRY, MenuItem::IN_ROYAL, (*_display->string)["INN_5"]);
		_add_item(5, MenuItemType::SPACER, MenuItem::SPACER, (*_display->string)["MENU_SPACER"]);
		_add_item(6, MenuItemType::ENTRY, MenuItem::IN_POOL_GOLD, (*_display->string)["INN_POOL"]);
		_add_item(7, MenuItemType::SPACER, MenuItem::SPACER, (*_display->string)["MENU_SPACER"]);
		_add_item(8, MenuItemType::ENTRY, MenuItem::IN_BACK, (*_display->string)["INN_BACK"]);
		selected = items.begin();
		break;
	case MenuType::CONTINUE:
		_add_item(0, MenuItemType::ENTRY, MenuItem::CONTINUE, (*_display->string)["CONTINUE"]);
		selected = items.begin();
		break;
	case MenuType::GRAVEYARD:
		_add_item(0, MenuItemType::ENTRY, MenuItem::CONTINUE, (*_display->string)["GRAVEYARD_MENU"]);
		selected = items.begin();
		break;
	case MenuType::STOP:
		_add_item(0, MenuItemType::ENTRY, MenuItem::STOP, (*_display->string)["STOP"]);
		selected = items.begin();
		break;
	case MenuType::TAVERN:
		_add_item(0, MenuItemType::ENTRY, MenuItem::TA_ADD_TO_PARTY, (*_display->string)["TAVERN_ADD_TO_PARTY"]);
		_add_item(
			1, MenuItemType::ENTRY, MenuItem::TA_REMOVE_FROM_PARTY, (*_display->string)["TAVERN_REMOVE_FROM_PARTY"]);
		_add_item(2, MenuItemType::ENTRY, MenuItem::TA_INSPECT, (*_display->string)["TAVERN_INSPECT"]);
		_add_item(3, MenuItemType::ENTRY, MenuItem::TA_REORDER, (*_display->string)["TAVERN_REORDER_PARTY"]);
		_add_item(4, MenuItemType::ENTRY, MenuItem::TA_DIVVY_GOLD, (*_display->string)["TAVERN_DIVVY_GOLD"]);
		_add_item(5, MenuItemType::ENTRY, MenuItem::TA_CASTLE, (*_display->string)["TAVERN_CASTLE"]);
		selected = items.begin();
		break;
	case MenuType::INN:
		_add_item(0, MenuItemType::ENTRY, MenuItem::IN_STAY_CHARACTER, (*_display->string)["INN_STAY_ONE"]);
		_add_item(1, MenuItemType::ENTRY, MenuItem::IN_INSPECT, (*_display->string)["INN_INSPECT"]);
		_add_item(2, MenuItemType::ENTRY, MenuItem::IN_CASTLE, (*_display->string)["INN_CASTLE"]);
		selected = items.begin();
		break;
	case MenuType::SHOP:
		_add_item(0, MenuItemType::ENTRY, MenuItem::SH_BUY_AND_SELL, (*_display->string)["SHOP_BUY_AND_SELL"]);
		_add_item(1, MenuItemType::ENTRY, MenuItem::SH_UNCURSE, (*_display->string)["SHOP_UNCURSE"]);
		_add_item(2, MenuItemType::ENTRY, MenuItem::SH_IDENTIFY, (*_display->string)["SHOP_IDENTIFY"]);
		_add_item(3, MenuItemType::ENTRY, MenuItem::SH_INSPECT, (*_display->string)["SHOP_INSPECT"]);
		_add_item(4, MenuItemType::ENTRY, MenuItem::SH_CASTLE, (*_display->string)["SHOP_CASTLE"]);
		selected = items.begin();
		break;
	case MenuType::TEMPLE:
		_add_item(0, MenuItemType::ENTRY, MenuItem::TE_HELP, (*_display->string)["TEMPLE_HELP"]);
		_add_item(1, MenuItemType::ENTRY, MenuItem::TE_TITHE, (*_display->string)["TEMPLE_TITHE"]);
		_add_item(2, MenuItemType::ENTRY, MenuItem::TE_INSPECT, (*_display->string)["TEMPLE_INSPECT"]);
		_add_item(3, MenuItemType::ENTRY, MenuItem::TE_CASTLE, (*_display->string)["TEMPLE_CASTLE"]);
		selected = items.begin();
		break;
	case MenuType::TRAINING_GROUNDS:
		_add_item(
			0, MenuItemType::ENTRY, MenuItem::TR_CREATE, (*_display->string)["TRAINING_GROUNDS_MENU_OPTION_CREATE"]);
		_add_item(1, MenuItemType::ENTRY, MenuItem::TR_EDIT, (*_display->string)["TRAINING_GROUNDS_MENU_OPTION_EDIT"]);
		_add_item(
			2, MenuItemType::ENTRY, MenuItem::TR_DELETE, (*_display->string)["TRAINING_GROUNDS_MENU_OPTION_DELETE"]);
		_add_item(
			3, MenuItemType::ENTRY, MenuItem::TR_INSPECT, (*_display->string)["TRAINING_GROUNDS_MENU_OPTION_INSPECT"]);
		_add_item(4, MenuItemType::ENTRY, MenuItem::TR_EDGE_OF_TOWN,
			(*_display->string)["TRAINING_GROUNDS_MENU_OPTION_RETURN"]);
		selected = items.begin();
		break;
	case MenuType::ALLOCATE_CHARACTER_ATTRIBUTES:
		_add_item(0, MenuItemType::ENTRY, MenuItem::CS_STRENGTH, (*_display->string)["CHARACTER_STAT_STRENGTH"], true,
			ConfigOption::NONE, (*_display->string)["HINT_CHARACTER_STAT_STRENGTH"]);
		_add_item(1, MenuItemType::ENTRY, MenuItem::CS_IQ, (*_display->string)["CHARACTER_STAT_INTELLIGENCE"], true,
			ConfigOption::NONE, (*_display->string)["HINT_CHARACTER_STAT_INTELLIGENCE"]);
		_add_item(2, MenuItemType::ENTRY, MenuItem::CS_PIETY, (*_display->string)["CHARACTER_STAT_PIETY"], true,
			ConfigOption::NONE, (*_display->string)["HINT_CHARACTER_STAT_PIETY"]);
		_add_item(3, MenuItemType::ENTRY, MenuItem::CS_VITALITY, (*_display->string)["CHARACTER_STAT_VITALITY"], true,
			ConfigOption::NONE, (*_display->string)["HINT_CHARACTER_STAT_VITALITY"]);
		_add_item(4, MenuItemType::ENTRY, MenuItem::CS_AGILITY, (*_display->string)["CHARACTER_STAT_AGILITY"], true,
			ConfigOption::NONE, (*_display->string)["HINT_CHARACTER_STAT_AGILITY"]);
		_add_item(5, MenuItemType::ENTRY, MenuItem::CS_LUCK, (*_display->string)["CHARACTER_STAT_LUCK"], true,
			ConfigOption::NONE, (*_display->string)["HINT_CHARACTER_STAT_LUCK"]);
		selected = items.begin();
		break;
	case MenuType::CHOOSE_CHARACTER_ALIGNMENT:
		_add_item(0, MenuItemType::ENTRY, MenuItem::CA_GOOD, (*_display->string)["CHARACTER_ALIGNMENT_GOOD"], true,
			ConfigOption::NONE, (*_display->string)["HINT_CHARACTER_ALIGNMENT_GOOD"]);
		_add_item(1, MenuItemType::ENTRY, MenuItem::CA_NEUTRAL, (*_display->string)["CHARACTER_ALIGNMENT_NEUTRAL"],
			true, ConfigOption::NONE, (*_display->string)["HINT_CHARACTER_ALIGNMENT_NEUTRAL"]);
		_add_item(2, MenuItemType::ENTRY, MenuItem::CA_EVIL, (*_display->string)["CHARACTER_ALIGNMENT_EVIL"], true,
			ConfigOption::NONE, (*_display->string)["HINT_CHARACTER_ALIGNMENT_EVIL"]);
		selected = items.begin();
		break;
	case MenuType::CHOOSE_CHARACTER_CLASS:
	case MenuType::CHANGE_CHARACTER_CLASS:

		_add_item(0, MenuItemType::ENTRY, MenuItem::CC_SAMURAI, (*_display->string)["CHARACTER_CLASS_SAMURAI"], true,
			ConfigOption::NONE, (*_display->string)["HINT_CHARACTER_CLASS_SAMURAI"]);
		_add_item(1, MenuItemType::ENTRY, MenuItem::CC_FIGHTER, (*_display->string)["CHARACTER_CLASS_FIGHTER"], true,
			ConfigOption::NONE, (*_display->string)["HINT_CHARACTER_CLASS_FIGHTER"]);
		_add_item(2, MenuItemType::ENTRY, MenuItem::CC_LORD, (*_display->string)["CHARACTER_CLASS_LORD"], true,
			ConfigOption::NONE, (*_display->string)["HINT_CHARACTER_CLASS_LORD"]);
		_add_item(3, MenuItemType::ENTRY, MenuItem::CC_THIEF, (*_display->string)["CHARACTER_CLASS_THIEF"], true,
			ConfigOption::NONE, (*_display->string)["HINT_CHARACTER_CLASS_THIEF"]);
		_add_item(4, MenuItemType::ENTRY, MenuItem::CC_NINJA, (*_display->string)["CHARACTER_CLASS_NINJA"], true,
			ConfigOption::NONE, (*_display->string)["HINT_CHARACTER_CLASS_NINJA"]);
		_add_item(5, MenuItemType::ENTRY, MenuItem::CC_PRIEST, (*_display->string)["CHARACTER_CLASS_PRIEST"], true,
			ConfigOption::NONE, (*_display->string)["HINT_CHARACTER_CLASS_PRIEST"]);
		_add_item(6, MenuItemType::ENTRY, MenuItem::CC_BISHOP, (*_display->string)["CHARACTER_CLASS_BISHOP"], true,
			ConfigOption::NONE, (*_display->string)["HINT_CHARACTER_CLASS_BISHOP"]);
		_add_item(7, MenuItemType::ENTRY, MenuItem::CC_MAGE, (*_display->string)["CHARACTER_CLASS_MAGE"], true,
			ConfigOption::NONE, (*_display->string)["HINT_CHARACTER_CLASS_MAGE"]);
		selected = items.begin();
		break;
	case MenuType::CHOOSE_CHARACTER_PORTRAIT:
		break;
	case MenuType::CHOOSE_CHARACTER_RACE:
		_add_item(0, MenuItemType::ENTRY, MenuItem::CR_HUMAN, (*_display->string)["CHARACTER_RACE_HUMAN"], true,
			ConfigOption::NONE, (*_display->string)["HINT_CHARACTER_RACE_HUMAN"]);
		_add_item(1, MenuItemType::ENTRY, MenuItem::CR_ELF, (*_display->string)["CHARACTER_RACE_ELF"], true,
			ConfigOption::NONE, (*_display->string)["HINT_CHARACTER_RACE_ELF"]);
		_add_item(2, MenuItemType::ENTRY, MenuItem::CR_DWARF, (*_display->string)["CHARACTER_RACE_DWARF"], true,
			ConfigOption::NONE, (*_display->string)["HINT_CHARACTER_RACE_DWARF"]);
		_add_item(3, MenuItemType::ENTRY, MenuItem::CR_GNOME, (*_display->string)["CHARACTER_RACE_GNOME"], true,
			ConfigOption::NONE, (*_display->string)["HINT_CHARACTER_RACE_GNOME"]);
		_add_item(4, MenuItemType::ENTRY, MenuItem::CR_HOBBIT, (*_display->string)["CHARACTER_RACE_HOBBIT"], true,
			ConfigOption::NONE, (*_display->string)["HINT_CHARACTER_RACE_HOBBIT"]);
		selected = items.begin();
		break;
	case MenuType::CHOOSE_METHOD:
		_add_item(0, MenuItemType::ENTRY, MenuItem::CM_FULL, (*_display->string)["CHARACTER_CREATION_METHOD_FULL"],
			true, ConfigOption::NONE, (*_display->string)["HINT_CHARACTER_CREATION_METHOD_FULL"]);
		_add_item(1, MenuItemType::ENTRY, MenuItem::CM_QUICK, (*_display->string)["CHARACTER_CREATION_METHOD_QUICK"],
			true, ConfigOption::NONE, (*_display->string)["HINT_CHARACTER_CREATION_METHOD_QUICK"]);
		_add_item(2, MenuItemType::ENTRY, MenuItem::CM_RANDOM, (*_display->string)["CHARACTER_CREATION_METHOD_RANDOM"],
			true, ConfigOption::NONE, (*_display->string)["HINT_CHARACTER_CREATION_METHOD_RANDOM"]);
		selected = items.begin();
		break;
	case MenuType::OPTIONS:
		_add_item(0, MenuItemType::ENTRY, MenuItem::OP_RECOMMENDED_MODE, (*_display->string)["CONFIG_RECOMMENDED_MODE"],
			true, ConfigOption::RECOMMENDED_MODE, (*_display->string)["HINT_CONFIG_RECOMMENDED_MODE"]);
		_add_item(1, MenuItemType::ENTRY, MenuItem::OP_STRICT_MODE, (*_display->string)["CONFIG_STRICT_MODE"], true,
			ConfigOption::STRICT_MODE, (*_display->string)["HINT_CONFIG_STRICT_MODE"]);
		_add_item(2, MenuItemType::ENTRY, MenuItem::OP_CHEAT_MODE, (*_display->string)["CONFIG_CHEAT_MODE"], true,
			ConfigOption::CHEAT_MODE, (*_display->string)["HINT_CONFIG_CHEAT_MODE"]);
		_add_item(3, MenuItemType::ENTRY, MenuItem::OP_AUTO_SAVE, (*_display->string)["CONFIG_AUTO_SAVE"], true,
			ConfigOption::AUTO_SAVE, (*_display->string)["HINT_CONFIG_AUTO_SAVE"]);
		_add_item(4, MenuItemType::ENTRY, MenuItem::OP_DICE_ROLLS, (*_display->string)["CONFIG_DICE_ROLLS"], true,
			ConfigOption::DICE_ROLLS, (*_display->string)["HINT_CONFIG_DICE_ROLLS"]);
		_add_item(5, MenuItemType::ENTRY, MenuItem::OP_ALLOW_MIXED_ALIGNMENT_PARTY,
			(*_display->string)["GAME_ALLOW_MIXED_ALIGNMENT"], true, ConfigOption::ALLOW_MIXED_ALIGNMENT_PARTY,
			(*_display->string)["HINT_GAME_ALLOW_MIXED_ALIGNMENT"]);
		_add_item(6, MenuItemType::ENTRY, MenuItem::OP_STAT_LOSS_ON_LEVEL_UP,
			(*_display->string)["GAME_STAT_LOSS_LEVEL_GAIN"], true, ConfigOption::STAT_LOSS_ON_LEVEL_UP,
			(*_display->string)["HINT_GAME_STAT_LOSS_LEVEL_GAIN"]);
		_add_item(7, MenuItemType::ENTRY, MenuItem::OP_REROLL_HIT_POINTS_ON_LEVEL_GAIN,
			(*_display->string)["GAME_REROLL_HIT_POINTS"], true, ConfigOption::REROLL_HIT_POINTS_ON_LEVEL_GAIN,
			(*_display->string)["HINT_GAME_REROLL_HIT_POINTS"]);
		_add_item(8, MenuItemType::ENTRY, MenuItem::OP_STAT_RESET_ON_CLASS_CHANGE,
			(*_display->string)["GAME_STAT_RESET_CLASS_CHANGE"], true, ConfigOption::STAT_RESET_ON_CLASS_CHANGE,
			(*_display->string)["HINT_GAME_STAT_RESET_CLASS_CHANGE"]);
		_add_item(9, MenuItemType::ENTRY, MenuItem::OP_AGING_ON_CLASS_CHANGE,
			(*_display->string)["GAME_AGING_CLASS_CHANGE"], true, ConfigOption::AGING_ON_CLASS_CHANGE,
			(*_display->string)["HINT_GAME_AGING_CLASS_CHANGE"]);
		_add_item(10, MenuItemType::ENTRY, MenuItem::OP_PROTECT_TELEPORT, (*_display->string)["GAME_PROTECT_TELEPORT"],
			true, ConfigOption::PROTECT_TELEPORT, (*_display->string)["HINT_GAME_PROTECT_TELEPORT"]);
		_add_item(11, MenuItemType::ENTRY, MenuItem::OP_ALLOW_RANGED_WEAPONS,
			(*_display->string)["GAME_ALLOW_RANGED_WEAPONS"], true, ConfigOption::ALLOW_RANGED_WEAPONS,
			(*_display->string)["HINT_GAME_ALLOW_RANGED_WEAPONS"]);
		_add_item(12, MenuItemType::ENTRY, MenuItem::OP_SPELLCASTING_IN_SURPRISE_ROUND,
			(*_display->string)["GAME_SPELL_CASTING_SURPRISE_ROUND"], true,
			ConfigOption::SPELLCASTING_IN_SURPRISE_ROUND,
			(*_display->string)["HINT_GAME_SPELL_CASTING_SURPRISE_ROUND"]);
		_add_item(13, MenuItemType::ENTRY, MenuItem::OP_BATCH_HEALING_AFTER_RETURN_TO_CASTLE,
			(*_display->string)["GAME_BATCH_HEALING_AFTER_RETURN"], true,
			ConfigOption::BATCH_HEALING_AFTER_RETURN_TO_CASTLE,
			(*_display->string)["HINT_GAME_BATCH_HEALING_AFTER_RETURN"]);
		_add_item(14, MenuItemType::ENTRY, MenuItem::OP_REROLL_ONES_ON_DICE, (*_display->string)["GAME_REROLL_ONES"],
			true, ConfigOption::REROLL_ONES_ON_DICE, (*_display->string)["HINT_GAME_REROLL_ONES"]);

		_add_item(15, MenuItemType::ENTRY, MenuItem::OP_ALLOW_LOST_LEGATED,
			(*_display->string)["GAME_ALLOW_LOST_LEGATED"], true, ConfigOption::ALLOW_LOST_LEGATED,
			(*_display->string)["HINT_GAME_ALLOW_LOST_LEGATED"]);

		_add_item(16, MenuItemType::ENTRY, MenuItem::OP_ALLOW_CURABLE_DRAIN,
			(*_display->string)["GAME_ALLOW_CURABLE_DRAIN"], true, ConfigOption::ALLOW_CURABLE_DRAIN,
			(*_display->string)["HINT_GAME_ALLOW_CURABLE_DRAIN"]);

		_add_item(17, MenuItemType::ENTRY, MenuItem::OP_ALLOW_SHARED_INVENTORY,
			(*_display->string)["GAME_ALLOW_SHARED_INVENTORY"], true, ConfigOption::ALLOW_SHARED_INVENTORY,
			(*_display->string)["HINT_GAME_ALLOW_SHARED_INVENTORY"]);

		_add_item(18, MenuItemType::ENTRY, MenuItem::OP_CAMPING_RESTORE_SPELL_POINTS,
			(*_display->string)["GAME_CAMPING_SPELL_POINTS"], true, ConfigOption::CAMPING_RESTORE_SPELL_POINTS,
			(*_display->string)["HINT_GAME_CAMPING_SPELL_POINTS"]);

		_add_item(19, MenuItemType::ENTRY, MenuItem::OP_CHARGED_ITEMS, (*_display->string)["GAME_CHARGED_ITEMS"], true,
			ConfigOption::CHARGED_ITEMS, (*_display->string)["HINT_GAME_CHARGED_ITEMS"]);

		_add_item(20, MenuItemType::SPACER, MenuItem::SPACER, (*_display->string)["MENU_SPACER"]);
		_add_item(21, MenuItemType::SAVE, MenuItem::SAVE, (*_display->string)["MENU_OPTIONS_SAVE"]);
		_add_item(22, MenuItemType::CANCEL, MenuItem::CANCEL, (*_display->string)["MENU_OPTIONS_CANCEL"]);
		selected = items.begin();
		break;
	case MenuType::PAUSE:
	case MenuType::CAMP:
		_add_item(0, MenuItemType::ENTRY, MenuItem::CP_INSPECT, (*_display->string)["CAMP_INSPECT"]);
		_add_item(1, MenuItemType::ENTRY, MenuItem::CP_REORDER, (*_display->string)["CAMP_REORDER"]);
		_add_item(2, MenuItemType::ENTRY, MenuItem::CP_SAVE, (*_display->string)["CAMP_SAVE"]);
		_add_item(3, MenuItemType::ENTRY, MenuItem::CP_OPTIONS, (*_display->string)["CAMP_OPTIONS"]);
		_add_item(4, MenuItemType::ENTRY, MenuItem::QUIT, (*_display->string)["CAMP_QUIT"]);
		_add_item(5, MenuItemType::ENTRY, MenuItem::CP_LEAVE, (*_display->string)["CAMP_LEAVE"]);
		selected = items.begin();
		break;
	case MenuType::SEARCH:
		// Not Used
		_add_item(0, MenuItemType::ENTRY, MenuItem::AC_LEAVE, (*_display->string)["ACTION_LEAVE"]);
		break;
	case MenuType::ACTION:
		_add_item(0, MenuItemType::ENTRY, MenuItem::CP_INSPECT, (*_display->string)["ACTION_INSPECT"]);
		_add_item(1, MenuItemType::ENTRY, MenuItem::AC_CAST_SPELL, (*_display->string)["ACTION_SPELL"]);
		_add_item(2, MenuItemType::ENTRY, MenuItem::AC_USE_ITEM, (*_display->string)["ACTION_USE"]);
		_add_item(
			4, MenuItemType::ENTRY, MenuItem::AC_SEARCH_CHARACTERS, (*_display->string)["ACTION_SEARCH_CHARACTERS"]);
		_add_item(5, MenuItemType::ENTRY, MenuItem::AC_SHOW_MAP, (*_display->string)["ACTION_FLOOR_MAP"]);
		_add_item(5, MenuItemType::ENTRY, MenuItem::AC_SEARCH_ITEMS, (*_display->string)["ACTION_SEARCH_ITEMS"]);
		_add_item(6, MenuItemType::ENTRY, MenuItem::AC_SEARCH_SECRET, (*_display->string)["ACTION_SEARCH_SECRET"]);
		_add_item(7, MenuItemType::ENTRY, MenuItem::AC_LEAVE, (*_display->string)["ACTION_LEAVE"]);
		break;
	default:
		break;
	}
}

// The Character Menu is a special case and needs to be reloaded often when
// names and classes change
auto Sorcery::Menu::reload() -> void {

	using enum Enums::Menu::Type;

	// Clear the Items
	items.clear();
	bounds.clear();
	count = 0;
	_texts.clear();
	_options.clear();

	// Now depending on the menu type, add the relevant items
	switch (_type) {
	case CHARACTER_ROSTER:
	case PARTY_CHARACTERS:
	case AVAILABLE_CHARACTERS:
	case INVALID_CHARACTERS:
	case CHARACTERS_HERE:
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

auto Sorcery::Menu::get_type() const -> MenuType {

	return _type;
}

// Add an item to the Menu
auto Sorcery::Menu::_add_item(int index, const MenuItemType itemtype, const MenuItem code, std::string key) -> void {

	// Note passing key by value as we are modifying the key here
	if (key.length() % 2 == 0)
		key.resize(key.length() + 1, 32);

	auto hint{""s};
	items.emplace_back(static_cast<unsigned int>(index), itemtype, code, key, true, ConfigOption::NONE, hint);
	++count;
}

// Add an item to the Menu
auto Sorcery::Menu::_add_item(const int index, const MenuItemType itemtype, const MenuItem code, std::string key,
	const bool enabled, const ConfigOption option, const std::string &hint) -> void {

	// Note passing key by value as we are modifying the key here
	if (key.length() % 2 == 0)
		key.resize(key.length() + 1, 32);

	items.emplace_back(static_cast<unsigned int>(index), itemtype, code, key, enabled, option, hint);
	++count;
}

// Select the first enabled menu item
auto Sorcery::Menu::_select_first() -> std::optional<std::vector<MenuEntry>::const_iterator> {

	using enum Enums::Menu::ItemType;

	auto it{std::ranges::find_if(items.begin(), items.end(), [&](const auto &menu_item) {
		return (((menu_item.type == ENTRY) || (menu_item.type == SAVE) || (menu_item.type == CANCEL)) &&
				(menu_item.enabled));
	})};

	if (it != items.end()) {
		selected = it;
		return it;
	} else
		return std::nullopt;
}

// Select the last enabled menu item
auto Sorcery::Menu::_select_last() -> std::optional<std::vector<MenuEntry>::const_iterator> {

	using enum Enums::Menu::ItemType;

	// Would be nice to use a ranges reverse view to handle this, or a std::find_last_if, instead we have to do a
	// forward iterator backwards since we can't use a backwards iterator either!
	for (std::vector<MenuEntry>::const_iterator it = items.end() - 1; it != items.begin(); --it)
		if ((((*it).type == ENTRY) || ((*it).type == SAVE) || ((*it).type == CANCEL)) && ((*it).enabled)) {
			selected = it;
			return selected;
		}

	return std::nullopt;
}

// Check if the mouse cursor is on a menu item
auto Sorcery::Menu::check_menu_mouseover(sf::Vector2f mouse_pos)
	-> std::optional<std::vector<MenuEntry>::const_iterator> {

	if (!bounds.empty()) {

		// Look for the bounds the mouse cursor is in, but return the associated item with the same index, since both
		// containers track each other
		const sf::Vector2f global_pos{this->getPosition()};
		mouse_pos -= global_pos;
		auto it{std::ranges::find_if(
			bounds.begin(), bounds.end(), [&mouse_pos](const auto &item) { return item.contains(mouse_pos); })};
		if (it != bounds.end()) {
			auto dist{std::distance(bounds.begin(), it)};
			return items.begin() + dist;
		} else
			return std::nullopt;
	}

	// If we reach here it means that bounds (which requites a draw to take
	// place, hasn't been populated yet)
	return std::nullopt;
}

// Check if the mouse cursor is on a menu item, and if so set it
auto Sorcery::Menu::set_mouse_selected(sf::Vector2f mouse_pos)
	-> std::optional<std::vector<MenuEntry>::const_iterator> {

	if (!bounds.empty()) {

		// Look for the bounds the mouse cursor is in, but select and return the
		// associated item with the same index, since both containers track each
		// other
		const sf::Vector2f global_pos{this->getPosition()};
		mouse_pos -= global_pos;
		auto it{std::ranges::find_if(
			bounds.begin(), bounds.end(), [&mouse_pos](const auto &item) { return item.contains(mouse_pos); })};
		if (it != bounds.end()) {
			auto dist{std::distance(bounds.begin(), it)};
			auto candidate{items.begin() + dist};
			if (candidate->enabled) {
				selected = candidate;
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
auto Sorcery::Menu::choose(std::any option) -> std::optional<std::vector<MenuEntry>::const_iterator> {

	using enum Enums::Character::Align;
	using enum Enums::Character::Class;
	using enum Enums::Character::Race;
	using enum Enums::Menu::Item;
	using enum Enums::Manage::Method;

	MenuItem search_for{NO_ITEM};
	switch (_type) {
	case MenuType::CHOOSE_METHOD:
		switch (std::any_cast<CreateMethod>(option)) {
		case FULL:
			search_for = CM_FULL;
			break;
		case QUICK:
			search_for = CM_QUICK;
			break;
		case RANDOM:
			search_for = CM_RANDOM;
			break;
		default:
			break;
		}
		break;
	case MenuType::CHOOSE_CHARACTER_RACE:
		switch (std::any_cast<CharacterRace>(option)) {
		case DWARF:
			search_for = CR_DWARF;
			break;
		case ELF:
			search_for = CR_ELF;
			break;
		case GNOME:
			search_for = CR_GNOME;
			break;
		case HOBBIT:
			search_for = CR_HOBBIT;
			break;
		case HUMAN:
			search_for = CR_HUMAN;
			break;
		default:
			break;
		}
		break;
	case MenuType::CHOOSE_CHARACTER_ALIGNMENT:
		switch (std::any_cast<CharacterAlignment>(option)) {
		case EVIL:
			search_for = CA_EVIL;
			break;
		case GOOD:
			search_for = CA_GOOD;
			break;
		case NEUTRAL:
			search_for = CA_NEUTRAL;
			break;
		default:
			break;
		}
		break;
	case MenuType::CHOOSE_CHARACTER_CLASS:
		switch (std::any_cast<CharacterClass>(option)) {
		case SAMURAI:
			search_for = CC_SAMURAI;
			break;
		case FIGHTER:
			search_for = CC_FIGHTER;
			break;
		case LORD:
			search_for = CC_LORD;
			break;
		case THIEF:
			search_for = CC_THIEF;
			break;
		case NINJA:
			search_for = CC_NINJA;
			break;
		case PRIEST:
			search_for = CC_PRIEST;
			break;
		case BISHOP:
			search_for = CC_BISHOP;
			break;
		case MAGE:
			search_for = CC_MAGE;
			break;
		default:
			break;
		}
		break;
	default:
		return std::nullopt;
		break;
	}

	auto it{
		std::ranges::find_if(items.begin(), items.end(), [&](const auto &item) { return item.item == search_for; })};
	if (it != items.end()) {
		selected = it;
		return selected;
	} else
		return std::nullopt;
}

// Set selected based upon the item index
auto Sorcery::Menu::choose(const unsigned int index) -> std::optional<std::vector<MenuEntry>::const_iterator> {

	// Iterate through til we have found the item with the associated index
	if (auto it{
			std::ranges::find_if(items.begin(), items.end(), [&](const auto &item) { return item.index == index; })};
		it != items.end())
		return it;
	else
		return std::nullopt;

	// If we reach here the mouse cursor is outside the items so we don't do
	// anything
	return std::nullopt;
}

auto Sorcery::Menu::choose_first() -> std::optional<std::vector<MenuEntry>::const_iterator> {

	return _select_first();
}

auto Sorcery::Menu::choose_last() -> std::optional<std::vector<MenuEntry>::const_iterator> {

	return _select_last();
}

// Choose the previous selected item
auto Sorcery::Menu::choose_previous() -> std::optional<std::vector<MenuEntry>::const_iterator> {

	using enum Enums::Menu::ItemType;

	if (selected > items.begin()) {

		// Repeat the comment from above, that it would be nice to use a ranges reverse view to handle this, or a
		// std::find_last_if, instead we have to do a forward iterator backwards since we can't use a backwards iterator
		// either!

		// Iterate backwards until we find the first previous enabled menu if we can
		bool found{false};
		std::vector<MenuEntry>::const_iterator working{selected};
		do {
			--working;
			found = ((*working).enabled) &&
					(((*working).type == ENTRY) || ((*working).type == SAVE) || ((*working).type == CANCEL));
		} while ((working >= items.begin()) && (!found));
		if (found) {
			selected = working;
			return selected;
		}
	} else
		return selected;

	return std::nullopt;
}

// Choose the next selected item
auto Sorcery::Menu::choose_next() -> std::optional<std::vector<MenuEntry>::const_iterator> {

	using enum Enums::Menu::ItemType;

	if (selected < (items.end() - 1)) {

		// Iterate forwards until we find the first next enabled menu if we can
		bool found{false};
		std::vector<MenuEntry>::const_iterator working{selected};
		do {
			++working;
			found = ((*working).enabled) &&
					(((*working).type == ENTRY) || ((*working).type == SAVE) || ((*working).type == CANCEL));
		} while ((working <= items.end() - 1) && (!found));
		if (found) {
			selected = working;
			return selected;
		}
	} else
		return selected;

	return std::nullopt;
}

// TODO: optimise this so that it isn't created on every refresh!
auto Sorcery::Menu::generate(const Component &component, bool force_refresh) -> void {

	using enum Enums::Menu::Type;
	using enum Enums::Menu::ItemType;

	auto do_refresh{force_refresh || _texts.empty()};
	if (do_refresh) {

		// In case we are generating the Options Menu
		const Component on_c{(*_display->layout)["options:on"]};
		const Component off_c{(*_display->layout)["options:off"]};
		const sf::Vector2f global_pos(component.x, component.y);

		// Work out total size of texture needed
		const auto texture_w{component.w * _display->window->get_cw()};
		const auto texture_h{component.h * _display->window->get_ch()};
		const sf::Vector2f texture_size(texture_w, texture_h);
		_rtexture.create(texture_size.x, texture_size.y);

		auto adjustment{_system->resources->fonts[component.font].getLineSpacing(4)};

		// Bounds are generated for each menu item to handle mouse over
		_texts.clear();
		_options.clear();
		bounds.clear();
		auto index{0};
		auto entry_y{0};
		for (const auto &item : items) {

			auto current{items.begin() + index};
			if ((item.type == TEXT) || (item.type == ENTRY) || (item.type == SAVE) || (item.type == CANCEL)) {
				auto text_string{item.key};
				sf::Text text{};
				// text.setStyle(sf::Text::Bold);
				text.setFont(_system->resources->fonts[component.font]);
				text.setCharacterSize(component.size);
				// text.setStyle(sf::Text::Bold);
				if (item.enabled)
					text.setFillColor(sf::Color(component.colour));
				else
					text.setFillColor(sf::Color(0x606060ff));
				std::transform(text_string.begin(), text_string.end(), text_string.begin(), ::toupper);
				text.setString(text_string);

				// Check for alignment and set location appropriately
				auto entry_x{(component.justification == Justification::CENTRE) ? texture_w / 2 : 0};
				entry_y += _display->window->get_ch();
				text.setPosition(entry_x, entry_y);

				// If we have a selected entry, change the background colour
				if (selected == current) {
					const sf::FloatRect bg_rect{text.getLocalBounds()};
					sf::RectangleShape bg(sf::Vector2f(component.w * _display->window->get_cw(), bg_rect.height));
					bg.setPosition(0, entry_y - text.getLocalBounds().height / 2.0f + adjustment);
					if (component.animated)
						bg.setFillColor(_graphics->animation->selected_colour);
					else
						bg.setFillColor(sf::Color(component.background));

					text.setFillColor(sf::Color(component.colour));
					text.setOutlineColor(sf::Color(0, 0, 0));
					text.setOutlineThickness(1);

					_selected_bg = bg;
				}

				// Handle Justification
				if ((_type == OPTIONS) || (_type == ALLOCATE_CHARACTER_ATTRIBUTES)) {
					if (item.type == ENTRY) {
						if (component.justification == Justification::CENTRE)
							text.setOrigin(text.getLocalBounds().width / 2.0f, text.getLocalBounds().height / 2.0f);
						else
							text.setOrigin(0, text.getLocalBounds().height / 2.0f);
					} else if ((item.type == SAVE) || (item.type == CANCEL)) {
						entry_x = (component.width * _display->window->get_cw()) / 2;
						text.setPosition(entry_x, entry_y);
						text.setOrigin(text.getLocalBounds().width / 2.0f, text.getLocalBounds().height / 2.0f);
					}
				} else if (_type == TEMPLE) {

					// Override Justification for Invalid Characters so that it looks better
					if (item.item == MenuItem::IC_CHARACTER) {
						text.setOrigin(0, text.getLocalBounds().height / 2.0f);
					} else {
						if (component.justification == Justification::CENTRE)
							text.setOrigin(text.getLocalBounds().width / 2.0f, text.getLocalBounds().height / 2.0f);
						else
							text.setOrigin(0, text.getLocalBounds().height / 2.0f);
					}
				} else {
					if (component.justification == Justification::CENTRE)
						text.setOrigin(text.getLocalBounds().width / 2.0f, text.getLocalBounds().height / 2.0f);
					else
						text.setOrigin(0, text.getLocalBounds().height / 2.0f);
				}

				_texts.emplace_back(text);

				// Now handle the mouse move/select!
				if ((item.type == ENTRY) || (item.type == SAVE) || (item.type == CANCEL)) {
					const sf::FloatRect actual_rect{text.getGlobalBounds()};
					bounds.push_back(actual_rect);
				} else {
					const sf::FloatRect actual_rect;
					bounds.push_back(actual_rect);
				}

				// Add options in case of the Options Menu
				if ((_type == OPTIONS) && (item.type == ENTRY)) {
					auto option_y{entry_y};
					auto option_x{component.w * _display->window->get_cw()};
					const auto option_value{(*_system->config)[item.config] ? true : false};
					sf::Text option_text{};
					if (option_value) {

						// On
						option_text.setFont(_system->resources->fonts[on_c.font]);
						option_text.setCharacterSize(on_c.size);
						// option_text.setStyle(sf::Text::Bold);
						option_text.setFillColor(sf::Color(on_c.colour));
						option_text.setString((*_display->string)[on_c.string_key]);
						sf::FloatRect bounds{option_text.getLocalBounds()};
						option_text.setPosition(option_x - bounds.width, option_y);
						option_text.setOrigin(0, option_text.getLocalBounds().height / 2.0f);
					} else {

						// Off
						option_text.setFont(_system->resources->fonts[off_c.font]);
						option_text.setCharacterSize(off_c.size);
						option_text.setFillColor(sf::Color(off_c.colour));
						option_text.setString((*_display->string)[off_c.string_key]);
						sf::FloatRect bounds{option_text.getLocalBounds()};
						option_text.setPosition(option_x - bounds.width, option_y);
						option_text.setOrigin(0, option_text.getLocalBounds().height / 2.0f);
					}

					if (selected == current) {
						option_text.setOutlineColor(sf::Color(0, 0, 0));
						option_text.setOutlineThickness(1);
					}

					_options.emplace_back(option_text);
				}
			} else {
				const sf::FloatRect actual_rect{};
				bounds.push_back(actual_rect);
				_texts.emplace_back();
				entry_y += _display->window->get_ch();
			}
			++index;
		}
	} else {

		auto adjustment{_system->resources->fonts[component.font].getLineSpacing(4)};

		// Only change what needs to be changed
		auto entry_y{0};
		auto index{0};
		auto options_index{0};
		auto option_x{0};
		auto option_y{0};
		const Component on_c{(*_display->layout)["options:on"]};
		const Component off_c{(*_display->layout)["options:off"]};
		for (const auto &item : items) {
			auto current{items.begin() + index};
			entry_y += _display->window->get_ch();
			if ((item.type == TEXT) || (item.type == ENTRY) || (item.type == SAVE) || (item.type == CANCEL)) {
				if (selected == current) {
					const sf::FloatRect bg_rect{_texts.at(index).getLocalBounds()};
					sf::RectangleShape bg(sf::Vector2f(component.w * _display->window->get_cw(), bg_rect.height));
					bg.setPosition(0, entry_y - _texts.at(index).getLocalBounds().height / 2.0f + adjustment);
					if (component.animated)
						bg.setFillColor(_graphics->animation->selected_colour);
					else
						bg.setFillColor(sf::Color(component.background));

					if (item.enabled)
						_texts.at(index).setFillColor(sf::Color(component.colour));
					else
						_texts.at(index).setFillColor(sf::Color(0x606060ff));
					_texts.at(index).setOutlineColor(sf::Color(0, 0, 0));
					_texts.at(index).setOutlineThickness(1);

					_selected_bg = bg;
				}

				if ((_type == OPTIONS) && (item.type == ENTRY)) {

					option_y = entry_y;
					option_x = component.w * _display->window->get_cw();
					if (const bool option_value{(*_system->config)[item.config] ? true : false}; option_value) {

						// On
						_options.at(options_index).setString((*_display->string)[on_c.string_key]);
						sf::FloatRect bounds{_options.at(options_index).getLocalBounds()};
						_options.at(options_index).setPosition(option_x - bounds.width, option_y);
						_options.at(options_index)
							.setOrigin(0, _options.at(options_index).getLocalBounds().height / 2.0f);
						_options.at(options_index).setFillColor(sf::Color(on_c.colour));
					} else {

						// Off
						_options.at(options_index).setString((*_display->string)[off_c.string_key]);
						sf::FloatRect bounds{_options.at(options_index).getLocalBounds()};
						_options.at(options_index).setPosition(option_x - bounds.width, option_y);
						_options.at(options_index)
							.setOrigin(0, _options.at(options_index).getLocalBounds().height / 2.0f);

						_options.at(options_index).setFillColor(sf::Color(off_c.colour));
					}

					if (selected == current) {
						_options.at(options_index).setOutlineColor(sf::Color(0, 0, 0));
						_options.at(options_index).setOutlineThickness(1);
					}
					++options_index;
				}
			}
			++index;
		}
	}
}

auto Sorcery::Menu::draw(sf::RenderTarget &target, sf::RenderStates states) const -> void {

	states.transform *= getTransform();

	target.draw(_selected_bg, states);
	for (auto &text : _texts)
		target.draw(text, states);

	if (_type == MenuType::OPTIONS)
		for (auto &option : _options)
			target.draw(option, states);
}

auto Sorcery::Menu::_populate_chars() -> void {

	using enum Enums::Menu::Type;
	using enum Enums::Menu::ItemType;

	items.clear();
	bounds.clear();
	count = 0;
	_texts.clear();
	_options.clear();
	auto max_id{0};
	switch (_type) {
	case CHARACTER_ROSTER: {
		if (!_game->characters.empty()) {
			for (auto &[character_id, character] : _game->characters) {
				if (_mode.value() == MenuMode::TAVERN) {
					if ((character.get_location() == CharacterLocation::TAVERN) ||
						(character.get_location() == CharacterLocation::PARTY)) {
						_add_item(character_id, ENTRY, MenuItem::IC_CHARACTER, character.get_summary());
						++max_id;
					}
				} else if (_mode.value() == MenuMode::INN) {
					if (character.get_location() == CharacterLocation::PARTY) {
						_add_item(character_id, ENTRY, MenuItem::IC_CHARACTER, character.get_summary());
						++max_id;
					}
				} else if (_mode.value() == MenuMode::TEMPLE) {
					if ((character.get_location() == CharacterLocation::TEMPLE) ||
						(character.get_location() == CharacterLocation::PARTY)) {
						_add_item(character_id, ENTRY, MenuItem::IC_CHARACTER, character.get_summary());
						++max_id;
					}
				} else if (_mode.value() == MenuMode::TRAINING) {
					_add_item(character_id, ENTRY, MenuItem::IC_CHARACTER, character.get_summary_and_out());
					++max_id;
				}
			}
		} else
			_add_item(++max_id, TEXT, MenuItem::NC_WARNING, (*_display->string)["MENU_NO_CHARACTERS"]);

		if (_mode) {
			if (_mode.value() == MenuMode::TRAINING) {
				_add_item(++max_id, SPACER, MenuItem::SPACER, (*_display->string)["MENU_SPACER"]);
				_add_item(++max_id, ENTRY, MenuItem::ET_TRAIN, (*_display->string)["MENU_TRAIN"]);
			} else if (_mode.value() == MenuMode::TAVERN) {
				_add_item(++max_id, SPACER, MenuItem::SPACER, (*_display->string)["MENU_SPACER"]);
				_add_item(++max_id, ENTRY, MenuItem::CA_TAVERN, (*_display->string)["MENU_TAVERN"]);
			} else if (_mode.value() == MenuMode::INN) {
				_add_item(++max_id, SPACER, MenuItem::SPACER, (*_display->string)["MENU_SPACER"]);
				_add_item(++max_id, ENTRY, MenuItem::CA_TAVERN, (*_display->string)["MENU_INN"]);
			} else if (_mode.value() == MenuMode::TEMPLE) {
				_add_item(++max_id, SPACER, MenuItem::SPACER, (*_display->string)["MENU_SPACER"]);
				_add_item(++max_id, ENTRY, MenuItem::CA_TAVERN, (*_display->string)["MENU_TEMPLE"]);
			}
		}

	} break;
	case CHARACTERS_HERE: {
		if (_game->state->get_party_size() < 6) {

			// Check for any characters in same square
			bool found{false};
			for (const auto &[character_id, character] : _game->characters) {
				if ((character.get_location() == CharacterLocation::MAZE) &&
					(character.coordinate == _game->state->get_player_pos()) &&
					(character.depth == _game->state->get_depth())) {
					found = true;
					break;
				}
			}
			if (found) {

				_add_item(++max_id, TEXT, MenuItem::NC_WARNING, (*_display->string)["ACTION_FOUND_1"]);
				_add_item(++max_id, TEXT, MenuItem::NC_WARNING, (*_display->string)["ACTION_FOUND_2"]);
				_add_item(++max_id, SPACER, MenuItem::SPACER, (*_display->string)["MENU_SPACER"]);
				for (const auto &[character_id, character] : _game->characters) {
					if ((character.get_location() == CharacterLocation::MAZE) &&
						(character.coordinate == _game->state->get_player_pos()) &&
						(character.depth == _game->state->get_depth()))
						_add_item(character_id, ENTRY, MenuItem::IC_CHARACTER,
							_game->characters[character_id].get_name_and_status());
					++max_id;
				}
			} else {
				_add_item(++max_id, TEXT, MenuItem::NC_WARNING, (*_display->string)["ACTION_FOUND_1"]);
				_add_item(++max_id, TEXT, MenuItem::NC_WARNING, (*_display->string)["ACTION_FOUND_2"]);
				_add_item(++max_id, SPACER, MenuItem::SPACER, (*_display->string)["MENU_SPACER"]);
				_add_item(++max_id, TEXT, MenuItem::NC_WARNING, (*_display->string)["MENU_NO_CHARACTERS_HERE"]);
			}
		} else {
			_add_item(++max_id, TEXT, MenuItem::NC_WARNING, (*_display->string)["ACTION_FOUND_1"]);
			_add_item(++max_id, TEXT, MenuItem::NC_WARNING, (*_display->string)["ACTION_FOUND_2"]);
			_add_item(++max_id, SPACER, MenuItem::SPACER, (*_display->string)["MENU_SPACER"]);
			_add_item(++max_id, TEXT, MenuItem::NC_WARNING, (*_display->string)["PARTY_FULL"]);
		}

		_add_item(++max_id, MenuItemType::SPACER, MenuItem::SPACER, (*_display->string)["MENU_SPACER"]);
		_add_item(++max_id, MenuItemType::CANCEL, MenuItem::AC_LEAVE, (*_display->string)["ACTION_LEAVE"]);
	} break;
	case RESTART_EXPEDITION: {
		_add_item(++max_id, MenuItemType::TEXT, MenuItem::NC_WARNING, (*_display->string)["RESTART_TEXT_1"]);
		_add_item(++max_id, MenuItemType::TEXT, MenuItem::NC_WARNING, (*_display->string)["RESTART_TEXT_2"]);
		_add_item(++max_id, MenuItemType::SPACER, MenuItem::SPACER, (*_display->string)["MENU_SPACER"]);
		auto possible{0U};
		for (const auto &[character_id, character] : _game->characters) {
			if ((character.get_location() == CharacterLocation::MAZE) &&
				(character.get_status() == CharacterStatus::OK)) {
				_add_item(character_id, MenuItemType::ENTRY, MenuItem::IC_CHARACTER,
					_game->characters[character_id].get_name_and_location());
				++possible;
				++max_id;
			}
		}
		if (possible == 0)
			_add_item(
				++max_id, MenuItemType::TEXT, MenuItem::NC_WARNING, (*_display->string)["MENU_NO_CHARACTERS_IN_MAZE"]);
		_add_item(++max_id, MenuItemType::SPACER, MenuItem::SPACER, (*_display->string)["MENU_SPACER"]);
		_add_item(++max_id, MenuItemType::CANCEL, MenuItem::TR_EDGE_OF_TOWN,
			(*_display->string)["RESTART_GROUNDS_MENU_OPTION_RETURN"]);
	} break;
	case PARTY_CHARACTER_NAMES: {
		if (_game->state->party_has_members()) {
			auto party{_game->state->get_party_characters()};
			for (auto character_id : party) {
				_add_item(character_id, MenuItemType::ENTRY, MenuItem::IC_CHARACTER,
					_game->characters[character_id].get_name());
				++max_id;
			}
		} else
			_add_item(++max_id, MenuItemType::TEXT, MenuItem::NC_WARNING, (*_display->string)["MENU_NO_CHARACTERS"]);
	} break;
	case PARTY_CHARACTERS: {
		if (_game->state->party_has_members()) {
			auto party{_game->state->get_party_characters()};
			for (auto character_id : party) {
				_add_item(character_id, MenuItemType::ENTRY, MenuItem::IC_CHARACTER,
					_game->characters[character_id].get_summary());
				++max_id;
			}
		} else
			_add_item(++max_id, MenuItemType::TEXT, MenuItem::NC_WARNING, (*_display->string)["MENU_NO_CHARACTERS"]);

		if (_mode) {
			if (_mode.value() == MenuMode::TAVERN) {
				_add_item(++max_id, MenuItemType::SPACER, MenuItem::SPACER, (*_display->string)["MENU_SPACER"]);
				_add_item(++max_id, MenuItemType::ENTRY, MenuItem::CA_TAVERN, (*_display->string)["MENU_TAVERN"]);
			} else if (_mode.value() == MenuMode::INN) {
				_add_item(++max_id, MenuItemType::SPACER, MenuItem::SPACER, (*_display->string)["MENU_SPACER"]);
				_add_item(++max_id, MenuItemType::ENTRY, MenuItem::CA_INN, (*_display->string)["MENU_INN"]);
			} else if (_mode.value() == MenuMode::SHOP) {
				_add_item(++max_id, MenuItemType::SPACER, MenuItem::SPACER, (*_display->string)["MENU_SPACER"]);
				_add_item(++max_id, MenuItemType::ENTRY, MenuItem::CA_SHOP, (*_display->string)["MENU_SHOP"]);
			} else if (_mode.value() == MenuMode::TEMPLE) {
				_add_item(++max_id, MenuItemType::SPACER, MenuItem::SPACER, (*_display->string)["MENU_SPACER"]);
				_add_item(++max_id, MenuItemType::ENTRY, MenuItem::CA_TEMPLE, (*_display->string)["MENU_TEMPLE"]);
			} else if (_mode.value() == MenuMode::CAMP) {
				_add_item(++max_id, MenuItemType::SPACER, MenuItem::SPACER, (*_display->string)["MENU_SPACER"]);
				_add_item(++max_id, MenuItemType::ENTRY, MenuItem::CAMP, (*_display->string)["MENU_LEAVE"]);
			}
		}
	} break;
	case AVAILABLE_CHARACTERS: {
		auto count{0u};
		auto party{_game->state->get_party_characters()};
		for (auto &[character_id, character] : _game->characters) {
			if (std::ranges::find(party.begin(), party.end(), character_id) == party.end()) {

				// TODO:: good and evil exclusion if in strict mode
				if (character.get_location() == CharacterLocation::TAVERN) {
					_add_item(character_id, MenuItemType::ENTRY, MenuItem::IC_CHARACTER, character.get_summary());
					++max_id;
					++count;
				}
			}
		}

		if (count == 0) {
			_add_item(
				++max_id, MenuItemType::TEXT, MenuItem::NC_WARNING, (*_display->string)["MENU_NO_MORE_CHARACTERS"]);
		}

		if (_mode) {
			if (_mode.value() == MenuMode::TAVERN) {
				_add_item(++max_id, MenuItemType::SPACER, MenuItem::SPACER, (*_display->string)["MENU_SPACER"]);
				_add_item(++max_id, MenuItemType::ENTRY, MenuItem::CA_TAVERN, (*_display->string)["MENU_TAVERN"]);
			}
		}
	} break;
	case INVALID_CHARACTERS: {
		auto count{0u};
		auto last_id{0u};
		if (!_game->characters.empty()) {
			for (const auto &[character_id, character] : _game->characters) {
				if ((character.get_status() != CharacterStatus::OK) &&
					(character.get_status() != CharacterStatus::LOST) &&
					(character.get_location() == CharacterLocation::TEMPLE)) {
					const auto status{character.get_name_and_status()};
					_add_item(character_id, MenuItemType::ENTRY, MenuItem::IC_CHARACTER, status);
					++count;
					if (character_id > last_id)
						last_id = character_id;
				}
			}
			++last_id;
		}

		if (count == 0) {
			_add_item(
				++max_id, MenuItemType::TEXT, MenuItem::NC_WARNING, (*_display->string)["MENU_NO_HELP_CHARACTERS"]);
		}

		if (_mode) {
			if (_mode.value() == MenuMode::TEMPLE) {
				_add_item(++max_id, MenuItemType::SPACER, MenuItem::SPACER, (*_display->string)["MENU_SPACER"]);
				_add_item(++max_id, MenuItemType::ENTRY, MenuItem::CA_TEMPLE, (*_display->string)["MENU_TEMPLE"]);
			}
		}
	} break;
	default:
		break;
	}
}

auto Sorcery::Menu::get_by_index(unsigned int index) -> std::optional<std::vector<MenuEntry>::iterator> {

	using enum Enums::Menu::ItemType;

	auto it{std::ranges::find_if(items.begin(), items.end(), [index](const auto &menu_item) {
		return (((menu_item.type == ENTRY) || (menu_item.type == SAVE) || (menu_item.type == CANCEL)) &&
				(menu_item.index == index));
	})};

	if (it != items.end()) {
		selected = it;
		return it;
	} else
		return std::nullopt;
}

auto Sorcery::Menu::num_enabled() -> unsigned int {

	return std::ranges::count_if(items.begin(), items.end(), [](const auto &menu_item) { return menu_item.enabled; });
}

auto Sorcery::Menu::num_disabled() -> unsigned int {

	return std::ranges::count_if(
		items.begin(), items.end(), [](const auto &menu_item) { return menu_item.enabled == false; });
}

auto Sorcery::Menu::enable_entry(const Component &component, unsigned int index) -> void {

	auto entry{items.begin() + index};
	auto current{(*entry).enabled};
	if (current == false) {
		(*entry).enabled = true;
		if (_texts.size() >= index)
			_texts.at(index).setFillColor(sf::Color(component.colour));
	}
}

auto Sorcery::Menu::disable_entry(__attribute__((unused)) const Component &component, unsigned int index) -> void {

	auto entry{items.begin() + index};
	auto current{(*entry).enabled};
	if (current) {
		(*entry).enabled = false;
		if (_texts.size() >= index)
			_texts.at(index).setFillColor(sf::Color(0x606060ff));
	}
}
