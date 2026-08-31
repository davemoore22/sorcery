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

#include "core/controller/menubuilder.hpp"
#include "common/enum.hpp"
#include "core/context.hpp"
#include "core/controller/controller.hpp"
#include "core/debug.hpp"
#include "core/define.hpp"
#include "core/resources.hpp"
#include "drawables/define.hpp"
#include "resources/itemstore.hpp"
#include "resources/monsterstore.hpp"
#include "resources/spellstore.hpp"
#include "resources/stringstore.hpp"
#include "types/game.hpp"
#include "types/meta.hpp"
#include "types/state.hpp"
#include <algorithm>
#include <ranges>

namespace {

using StringList = std::vector<std::string>;

const std::unordered_map<std::string, StringList> FIXED_MENUS = {

	{"compendium_menu",
	 {"COMPENDIUM_ATLAS", "COMPENDIUM_BESTIARY", "COMPENDIUM_GAZETTEER",
	  "COMPENDIUM_MUSEUM", "COMPENDIUM_SPELLBOOK", "COMPENDIUM_RETURN"}},

	{"camp_menu",
	 {"CAMP_INSPECT", "CAMP_REORDER", "CAMP_OPTIONS", "CAMP_QUIT",
	  "CAMP_LEAVE"}},

	{"roster_menu", {"ROSTER_RETURN"}},
	{"select_menu", {"SELECT_RETURN"}},
	{"retrain_menu", {"RETRAIN_RETURN"}},
	{"legate_menu", {"LEGATE_RETURN"}},
	{"choose_menu", {"CHOOSE_RETURN"}},
	{"remove_character_menu", {"REMOVE_CHARACTER_RETURN"}},
	{"reorder_menu", {"REORDER_RETURN"}},

	{"chest_menu",
	 {"CHEST_MENU_OPEN", "CHEST_MENU_INPSECT", "CHEST_MENU_CALFO",
	  "CHEST_MENU_DISARM", "CHEST_MENU_LEAVE"}},

	{"chest_trap_menu",
	 {"CHEST_POISON_NEEDLE", "CHEST_GAS_BOMB", "CHEST_CROSSBOW_BOLT",
	  "CHEST_EXPLODING_BOX", "CHEST_SPLINTERS", "CHEST_BLADES", "CHEST_STUNNER",
	  "CHEST_TELEPORTER", "CHEST_MAGE_BLASTER", "CHEST_PRIEST_BLASTER",
	  "CHEST_ALARM", "CHEST_TRAP_LEAVE"}},

	{"inspect_menu", {"INSPECT_RETURN"}},
	{"modal_inspect", {"INSPECT_RETURN"}},

	{"chest_inspect_menu", {"CHEST_RETURN"}},
	{"chest_open_menu", {"CHEST_RETURN"}},
	{"chest_calfo_menu", {"CHEST_RETURN"}},
	{"chest_disarm_menu", {"CHEST_RETURN"}},

	{"drop_menu", {"DROP_RETURN"}},
	{"identify_menu", {"IDENTIFY_RETURN"}},
	{"equip_menu", {"EQUIP_RETURN"}},
	{"trade_menu", {"TRADE_RETURN"}},
	{"use_menu", {"USE_RETURN"}},
	{"invoke_menu", {"INVOKE_RETURN"}},
	{"give_menu", {"GIVE_RETURN"}},
	{"remove_item_menu", {"REMOVE_ITEM_RETURN, RETURN"}},

	{"inn_menu", {"INN_RETURN"}},

	{"temple_heal_menu", {"TEMPLE_RETURN"}},

	{"temple_pay_menu", {"TEMPLE_RETURN"}},

	{"shop_menu", {"SHOP_RETURN"}},

	{"help_menu", {"HELP_RETURN"}},

	{"tithe_menu", {"TITHE_RETURN"}},

	{"pay_menu", {"PAY_RETURN"}},

	{"sell_menu", {"SELL_RETURN"}},

	{"shop_identify_menu", {"SHOP_IDENTIFY_RETURN"}},

	{"shop_uncurse_menu", {"SHOP_UNCURSE_RETURN"}},

	{"add_menu", {"ADD_RETURN"}},

	{"restart_menu", {"RESTART_RETURN"}},

	{"change_class_menu", {"CHANGE_CLASS_RETURN"}},

	{"castle_menu",
	 {"CASTLE_TAVERN", "CASTLE_INN", "CASTLE_SHOP", "CASTLE_TEMPLE",
	  "CASTLE_EDGE_OF_TOWN"}},

	{"rest_menu",
	 {"STAY_1", "STAY_2", "STAY_3", "STAY_4", "STAY_5", "STAY_RETURN"}},

	{"store_menu",
	 {"STORE_BUY", "STORE_SELL", "STORE_UNCURSE", "STORE_IDENTIFY",
	  "STORE_POOL", "STORE_RETURN"}},

	{"tavern_menu",
	 {"TAVERN_ADD_TO_PARTY", "TAVERN_REMOVE_FROM_PARTY", "TAVERN_REORDER_PARTY",
	  "TAVERN_DIVVY_GOLD", "TAVERN_CASTLE"}},

	{"temple_menu", {"TEMPLE_HELP", "TEMPLE_TITHE", "TEMPLE_CASTLE"}},

	{"edge_menu",
	 {"EDGE_OF_TOWN_TRAIN", "EDGE_OF_TOWN_MAZE", "EDGE_OF_TOWN_RESTART",
	  "EDGE_OF_TOWN_CASTLE", "EDGE_OF_TOWN_LEAVE_GAME"}},

	{"atlas_menu",
	 {"ATLAS_MENU_B1F", "ATLAS_MENU_B2F", "ATLAS_MENU_B3F", "ATLAS_MENU_B4F",
	  "ATLAS_MENU_B5F", "ATLAS_MENU_B6F", "ATLAS_MENU_B7F", "ATLAS_MENU_B8F",
	  "ATLAS_MENU_B9F", "ATLAS_MENU_B10F", "ATLAS_RETURN"}},

	{"training_menu",
	 {"TRAINING_GROUNDS_CREATE", "TRAINING_GROUNDS_EDIT",
	  "TRAINING_GROUNDS_DELETE", "TRAINING_GROUNDS_ROSTER",
	  "TRAINING_GROUNDS_RETURN"}},

	{"edit_menu",
	 {"EDIT_CHANGE_NAME", "EDIT_CHANGE_CLASS", "EDIT_LEGATE_CHARACTER",
	  "EDIT_RETURN"}},

	{"main_menu",
	 {"MAIN_MENU_OPTION_START", "MAIN_MENU_OPTION_CONTINUE",
	  "MAIN_MENU_OPTION_OPTIONS", "MAIN_MENU_OPTION_COMPENDIUM",
	  "MAIN_MENU_OPTION_LICENSE", "MAIN_MENU_OPTION_EXIT"}},

	{"race_menu",
	 {"CHARACTER_RACE_HUMAN", "CHARACTER_RACE_ELF", "CHARACTER_RACE_DWARF",
	  "CHARACTER_RACE_GNOME", "CHARACTER_RACE_HOBBIT",
	  "CHARACTER_RACE_RETURN"}},

	{"alignment_menu",
	 {"CHARACTER_ALIGNMENT_GOOD", "CHARACTER_ALIGNMENT_NEUTRAL",
	  "CHARACTER_ALIGNMENT_EVIL", "CHARACTER_ALIGNMENT_RETURN"}},

	{"class_menu",
	 {"CHARACTER_CLASS_FIGHTER", "CHARACTER_CLASS_MAGE",
	  "CHARACTER_CLASS_PRIEST", "CHARACTER_CLASS_THIEF",
	  "CHARACTER_CLASS_BISHOP", "CHARACTER_CLASS_SAMURAI",
	  "CHARACTER_CLASS_LORD", "CHARACTER_CLASS_NINJA",
	  "CHARACTER_CLASS_RETURN"}},

	{"modal_elevator_top",
	 {"ELEVATOR_A", "ELEVATOR_B", "ELEVATOR_C", "ELEVATOR_D",
	  "ELEVATOR_LEAVE"}},

	{"modal_elevator_bottom",
	 {"ELEVATOR_A", "ELEVATOR_B", "ELEVATOR_C", "ELEVATOR_D", "ELEVATOR_E",
	  "ELEVATOR_F", "ELEVATOR_LEAVE"}},

	{"top_elevator_menu",
	 {"ELEVATOR_A", "ELEVATOR_B", "ELEVATOR_C", "ELEVATOR_D",
	  "ELEVATOR_LEAVE"}},

	{"bottom_elevator_menu",
	 {"ELEVATOR_A", "ELEVATOR_B", "ELEVATOR_C", "ELEVATOR_D", "ELEVATOR_E",
	  "ELEVATOR_F", "ELEVATOR_LEAVE"}}};

}

Sorcery::MenuBuilder::MenuBuilder(Context &ctx)
	: _ctx{ctx} {}

Sorcery::MenuBuilder::~MenuBuilder() {}

auto Sorcery::MenuBuilder::_load_roster_characters(
	std::vector<std::string> &items, std::vector<int> &data) -> void {

	// Alphabetically Sort Characters
	std::vector<decltype(_ctx.game->characters)::const_iterator> sorted;
	sorted.reserve(_ctx.game->characters.size());
	for (auto it = _ctx.game->characters.cbegin();
		 it != _ctx.game->characters.cend(); ++it)
		sorted.push_back(it);
	std::ranges::sort(sorted, {}, [](const auto &it) {
		return it->second.get_name_status_and_loc();
	});

	for (const auto &it : sorted) {
		const auto &[id, character] = *it;
		items.emplace_back(character.get_name_status_and_loc());
		data.emplace_back(id);
	}
}

auto Sorcery::MenuBuilder::_load_retrainable_characters(
	std::vector<std::string> &items, std::vector<int> &data) -> void {

	// Alphabetically Sort Characters
	std::vector<decltype(_ctx.game->characters)::iterator> sorted;
	sorted.reserve(_ctx.game->characters.size());
	for (auto it = _ctx.game->characters.begin();
		 it != _ctx.game->characters.end(); ++it)
		sorted.push_back(it);
	std::ranges::sort(sorted, {}, [](const auto &it) {
		return it->second.get_name_and_status();
	});

	for (const auto &it : sorted) {
		auto &[id, character] = *it;
		const auto name{character.get_name_status_and_loc()};
		character.create().set_possible_classes();
		const auto possible_classes{
			character.create().get_possible_classes_display()};
		const auto menu_value{std::format("{} {}", name, possible_classes)};
		items.emplace_back(menu_value);
		data.emplace_back(id);
	}
}

auto Sorcery::MenuBuilder::_load_party_characters(
	std::vector<std::string> &items, std::vector<int> &data, const int flags,
	const bool reorder) -> void {

	const auto party{_ctx.game->state->get_party_characters()};
	auto pos{1};

	for (const auto id : party) {
		const auto &character{_ctx.game->characters.at(id)};
		if (character.get_location() != Enums::Character::Location::PARTY)
			continue;

		const auto name_str{flags & (MENU_FULL_NAME | MENU_SHOW_POSITION)
								? character.full_desc_text()
								: character.get_name()};

		if (flags & MENU_SHOW_POSITION)
			items.emplace_back(std::format("{}:{:^28}", pos, name_str));
		else if (flags & MENU_SHOW_GOLD)
			items.emplace_back(std::format("{:<16} {:>8} G.P.", name_str,
										   character.get_gold()));
		else if (flags & MENU_SHOW_IDENTIFY_TRAP) {
			items.emplace_back(std::format("{:<21} {:>3}%", name_str,
										   character.get_identify_trap()));

		} else if (flags & MENU_SHOW_AVOID_TRAP) {
			items.emplace_back(
				std::format("{:<21} {:>3}%", name_str,
							100 - character.get_activate_trap()));

		} else if (flags & MENU_SHOW_DISARM_TRAP) {
			items.emplace_back(std::format("{:<21} {:>3}%", name_str,
										   character.get_disarm_trap()));

		} else if (flags & MENU_SHOW_CALFO_USES_LEFT) {
			items.emplace_back(
				std::format("{:<21} ({:>1})", name_str,
							character.magic().get_calfo_uses_left()));

		} else if (flags & MENU_SHOW_SPACE) {
			const auto slots_free{character.inventory.get_empty_slots()};
			items.emplace_back(
				std::format("{:<21} ({:>1})", name_str, slots_free));
		} else
			items.emplace_back(std::format("{:^26}", name_str));

		data.emplace_back(id);

		if (reorder)
			_ctx.controller->add_to_candidate_party(id);

		++pos;
	}
}

auto Sorcery::MenuBuilder::_load_maze_characters(
	std::vector<std::string> &items, std::vector<int> &data) -> void {

	if (_ctx.game->characters.empty())
		return;

	for (auto &[id, character] : _ctx.game->characters) {
		if (character.get_location() == Enums::Character::Location::MAZE &&
			character.get_status() == Enums::Character::Status::OK) {

			items.emplace_back(
				std::format("{:^24}", character.full_desc_text()));
			data.emplace_back(id);
		}
	}
}

auto Sorcery::MenuBuilder::_load_tavern_characters(
	std::vector<std::string> &items, std::vector<int> &data) -> void {

	if (_ctx.game->characters.empty())
		return;

	for (auto &[id, character] : _ctx.game->characters) {
		if (character.get_location() == Enums::Character::Location::TAVERN) {

			items.emplace_back(
				std::format("{:^24}", character.full_desc_text()));
			data.emplace_back(id);
		}
	}
}

auto Sorcery::MenuBuilder::_load_sick_characters(
	std::vector<std::string> &items, std::vector<int> &data) -> void {

	if (_ctx.game->characters.empty())
		return;

	using enum Enums::Character::Status;

	for (auto &[id, character] : _ctx.game->characters) {
		if (character.get_status() == ASHES || character.get_status() == DEAD ||
			character.get_status() == HELD ||
			character.get_status() == STONED) {

			items.emplace_back(std::format("{:<16} {:>8}", character.get_name(),
										   character.get_status_string()));
			data.emplace_back(id);
		}
	}
}

auto Sorcery::MenuBuilder::_load_bestiary_menu(unsigned int width,
											   std::vector<std::string> &items)
	-> void {

	for (const auto &monster : _ctx.resources->monsters->get_all_types()) {

		if (monster.get_type_id() > Enums::Monsters::TypeID::WERDNA)
			continue;

		const auto padded{
			std::format("{:^{}}", monster.get_known_name(), width)};

		items.emplace_back(std::format(
			"{}##{}", padded, std::to_underlying(monster.get_type_id())));
	}

	items.emplace_back(
		std::format("{:^{}}", _ctx.get_string("BESTIARY_RETURN"), width));
}

auto Sorcery::MenuBuilder::_load_spellbook_menu(unsigned int width,
												std::vector<std::string> &items)
	-> void {

	for (const auto &spell : _ctx.resources->spells->get_all()) {

		items.emplace_back(std::format("{:^{}}", spell.name, width));
	}

	items.emplace_back(
		std::format("{:^{}}", _ctx.get_string("SPELLBOOK_RETURN"), width));
}

auto Sorcery::MenuBuilder::_load_buy_menu(unsigned int width,
										  std::vector<std::string> &items,
										  std::vector<int> &data) -> void {

	const auto char_id{
		_ctx.controller->get_character(Enums::CharacterSlot::STORE)};
	auto &character{_ctx.game->characters.at(char_id)};

	for (const auto &item_type : _ctx.resources->items->get_all_types()) {

		// Either has a fixed amount (> 0) or endless supply (-1)
		const auto in_stock{
			_ctx.game->state->check_shop_stock(item_type.get_type_id()) != 0};
		const auto will_sell{
			_ctx.game->state->check_shop_will_sell(item_type.get_type_id())};
		if (in_stock && will_sell) {

			const auto suffix{item_type.is_class_usable(character.get_class())
								  ? enum_name(item_type.get_category())
								  : " (Not Usable)"};
			const auto price{item_type.get_value()};
			const auto entry{std::format("{:<20} {:>6} {:<13}",
										 item_type.get_known_name(), price,
										 suffix)};

			items.emplace_back(entry);
			data.emplace_back(std::to_underlying(item_type.get_type_id()));
		}
	}
}

auto Sorcery::MenuBuilder::_load_museum_menu(unsigned int width,
											 std::vector<std::string> &items)
	-> void {

	for (const auto &item_type : _ctx.resources->items->get_all_types()) {

		if (item_type.get_type_id() == Enums::Items::TypeID::BROKEN_ITEM)
			continue;

		const auto padded{
			std::format("{:^{}}", item_type.get_known_name(), width)};

		items.emplace_back(std::format(
			"{}##{}", padded, std::to_underlying(item_type.get_type_id())));
	}

	items.emplace_back(
		std::format("{:^{}}", _ctx.get_string("MUSEUM_RETURN"), width));
}

auto Sorcery::MenuBuilder::build(const std::string &menu_name,
								 unsigned int width,
								 std::vector<std::string> &items,
								 std::vector<int> &data, bool reorder) -> void {

	items.clear();
	data.clear();

	// DEBUG_LOGF("Building menu: {}", menu_name);
	auto flags{_get_menu_flags(menu_name)};

	// Dynamic menus
	if (menu_name == "choose_menu" || menu_name == "inspect_menu" ||
		menu_name == "remove_character_menu" || menu_name == "tithe_menu" ||
		menu_name == "pay_menu" || menu_name == "give_menu") {

		_load_party_characters(items, data, flags, reorder);
		_load_fixed_menu(menu_name, width, items);
	} else if (menu_name == "change_class_menu") {
		_load_possible_classes(menu_name, items, data,
							   Enums::CharacterSlot::EDIT);
		_load_fixed_menu(menu_name, width, items);
	} else if (menu_name == "roster_menu" || menu_name == "delete_menu") {
		_load_roster_characters(items, data);
		_load_fixed_menu(menu_name, width, items);
	} else if (menu_name == "select_menu") {
		_load_roster_characters(items, data);
		_load_fixed_menu(menu_name, width, items);
	} else if (menu_name == "retrain_menu") {

		_load_retrainable_characters(items, data);
		_load_fixed_menu(menu_name, width, items);
	} else if (menu_name == "chest_menu") {
		_load_fixed_menu(menu_name, width, items);
	} else if (menu_name == "legate_menu") {
		_load_roster_characters(items, data);
		_load_fixed_menu(menu_name, width, items);
	} else if (menu_name == "buy_menu") {

		// No fixed menu for this one, as the items are dynamic and depend
		// on the store stock, and to leave the screen click on a button.
		_load_buy_menu(width, items, data);

	} else if (menu_name == "chest_inspect_menu") {
		_load_party_characters(items, data, flags, reorder);
		_load_fixed_menu(menu_name, width, items);
	} else if (menu_name == "chest_open_menu") {
		_load_party_characters(items, data, flags, reorder);
		_load_fixed_menu(menu_name, width, items);
	} else if (menu_name == "chest_disarm_menu") {
		_load_party_characters(items, data, flags, reorder);
		_load_fixed_menu(menu_name, width, items);
	} else if (menu_name == "chest_calfo_menu") {
		_load_party_characters(items, data, flags, reorder);
		_load_fixed_menu(menu_name, width, items);
	} else if (menu_name == "reorder_menu") {

		_load_party_characters(items, data, MENU_SHOW_POSITION, reorder);
		_load_fixed_menu(menu_name, width, items);
	} else if (menu_name == "inn_menu") {
		_load_party_characters(items, data, flags,
							   reorder); // MENU_FULL_NAME
		_load_fixed_menu(menu_name, width, items);
	} else if (menu_name == "temple_pay_menu") {
		_load_party_characters(items, data, flags,
							   reorder); // MENU_SHOW_GOLD
		_load_fixed_menu(menu_name, width, items);
	} else if (menu_name == "shop_menu") {
		_load_party_characters(items, data, flags,
							   reorder); // MENU_FULL_NAME
		_load_fixed_menu(menu_name, width, items);
	} else if (menu_name == "add_menu") {

		_load_tavern_characters(items, data); // MENU_FULL_NAME
		_load_fixed_menu(menu_name, width, items);
	} else if (menu_name == "restart_menu") {

		_load_maze_characters(items, data); // MENU_FULL_NAME
		_load_fixed_menu(menu_name, width, items);
	} else if (menu_name == "help_menu") {

		_load_sick_characters(items, data); // MENU_FULL_NAME
		_load_fixed_menu(menu_name, width, items);
	} else if (menu_name == "temple_heal_menu") {

		_load_sick_characters(items, data); // MENU_FULL_NAME
		_load_fixed_menu(menu_name, width, items);
	} else if (menu_name == "identify_menu" || menu_name == "drop_menu" ||
			   menu_name == "trade_menu" || menu_name == "use_menu" ||
			   menu_name == "invoke_menu" || menu_name == "equip_menu" ||
			   menu_name == "remove_item_menu") {

		_load_character_items(menu_name, items, data,
							  Enums::CharacterSlot::INSPECT);
		_load_fixed_menu(menu_name, width, items);
	} else if (menu_name == "sell_menu") {
		_load_character_items(menu_name, items, data,
							  Enums::CharacterSlot::STORE);
		_load_fixed_menu(menu_name, width, items);
	} else if (menu_name == "shop_identify_menu") {
		_load_character_items(menu_name, items, data,
							  Enums::CharacterSlot::STORE);
		_load_fixed_menu(menu_name, width, items);
	} else if (menu_name == "shop_uncurse_menu") {
		_load_character_items(menu_name, items, data,
							  Enums::CharacterSlot::STORE);
		_load_fixed_menu(menu_name, width, items);
	} else if (menu_name == "bestiary_menu") {
		_load_bestiary_menu(width, items);
	} else if (menu_name == "spellbook_menu") {
		_load_spellbook_menu(width, items);
	} else if (menu_name == "museum_menu") {
		_load_museum_menu(width, items);
	} else if (menu_name == "spell_menu") {
		_load_character_spells(menu_name, items, data);
		_load_fixed_menu(menu_name, width, items);
	} else if (menu_name == "top_elevator_menu" ||
			   menu_name == "bottom_elevator_menu") {
		_load_fixed_menu(menu_name, width, items);
	} else
		_load_fixed_menu(menu_name, width, items);
}

auto Sorcery::MenuBuilder::_load_fixed_menu(const std::string &menu_name,
											unsigned int width,
											std::vector<std::string> &items)
	-> void {

	assert(items.size() < 1000);

	const auto it{FIXED_MENUS.find(menu_name)};
	if (it == FIXED_MENUS.end())
		return;

	for (const auto &key : it->second) {
		items.emplace_back(std::format("{:^{}}", _ctx.get_string(key), width));
	}
}

auto Sorcery::MenuBuilder::_get_menu_flags(std::string_view menu_name) const
	-> int {

	constexpr std::array MENU_FLAG_MAP{
		std::pair{"identify_menu", MENU_IDENTIFY_ITEM},
		std::pair{"drop_menu", MENU_DROP_ITEM},
		std::pair{"sell_menu", MENU_SHOP_SELL_ITEM},
		std::pair{"shop_identify_menu", MENU_SHOP_IDENTIFY_ITEM},
		std::pair{"shop_uncurse_menu", MENU_SHOP_UNCURSE_ITEM},
		std::pair{"trade_menu", MENU_TRADE_ITEM},
		std::pair{"use_menu", MENU_USE_ITEM},
		std::pair{"invoke_menu", MENU_INVOKE_ITEM},
		std::pair{"equip_menu", MENU_EQUIP_ITEM},
		std::pair{"remove_menu", MENU_REMOVE_ITEM},
		std::pair{"choose_menu", MENU_FULL_NAME},
		std::pair{"inn_menu", MENU_FULL_NAME},
		std::pair{"shop_menu", MENU_FULL_NAME},
		std::pair{"remove_character_menu", MENU_FULL_NAME},
		std::pair{"tithe_menu", MENU_SHOW_GOLD},
		std::pair{"temple_pay_menu", MENU_SHOW_GOLD},
		std::pair{"pay_menu", MENU_SHOW_GOLD},
		std::pair{"reorder_menu", MENU_SHOW_POSITION},
		std::pair{"sell_menu", MENU_SHOP_SELL_ITEM},
		std::pair{"chest_inspect_menu", MENU_SHOW_IDENTIFY_TRAP},
		std::pair{"chest_open_menu", MENU_SHOW_AVOID_TRAP},
		std::pair{"chest_calfo_menu", MENU_SHOW_CALFO_USES_LEFT},
		std::pair{"chest_disarm_menu", MENU_SHOW_DISARM_TRAP},
	};

	if (const auto it = std::ranges::find(MENU_FLAG_MAP, menu_name,
										  &std::pair<const char *, int>::first);
		it != MENU_FLAG_MAP.end())
		return it->second;

	return NO_FLAGS;
}

auto Sorcery::MenuBuilder::_load_character_spells(
	std::string_view menu_name, std::vector<std::string> &items,
	std::vector<int> &data) -> void {

	// Get the character that is currently being inspected, and then filter
	// their known spells to only those that are castable (i.e. known, of
	// the correct category, and with sufficient spell points for the
	// relevant level).
	if (!_ctx.game || _ctx.game->characters.empty())
		return;

	if (!_ctx.controller->has_character(Enums::CharacterSlot::INSPECT))
		return;

	const auto char_id{
		_ctx.controller->get_character(Enums::CharacterSlot::INSPECT)};
	const auto &character{_ctx.game->characters.at(char_id)};

	// Work out castable spells for the character, filtering out as above.
	auto castable_spells{
		character.magic().get_spells() |
		std::views::filter([&character](const Spell &spell) {
			return (spell.known &&
					(spell.category != Enums::Magic::SpellCategory::HEALING ||
					 spell.category != Enums::Magic::SpellCategory::FIELD));
		})};

	// Build up the spell list (note that spells that are unable to be
	// currently cast due to lack of spell points are also included here,
	// but are disabled)
	for (const auto &spell : castable_spells) {

		const auto spell_type{enum_name(spell.type)};
		const auto spell_level{spell.level};
		const auto spell_english{spell.translated_name};
		const auto spell_name{spell.name};

		const auto spell_desc{
			std::format("{} ({})", spell_name, spell_english)};

		std::string line{
			std::format("{:<22} {} {}", spell_desc, spell_type, spell_level)};

		items.emplace_back(std::move(line));
		data.emplace_back(std::to_underlying(spell.id));
	}
}

auto Sorcery::MenuBuilder::_load_possible_classes(
	std::string_view menu_name, std::vector<std::string> &items,
	std::vector<int> &data, const Enums::CharacterSlot source) -> void {

	if (!_ctx.game || _ctx.game->characters.empty())
		return;

	if (!_ctx.controller->has_character(source))
		return;

	const auto char_id{_ctx.controller->get_character(source)};
	auto &character{_ctx.game->characters.at(char_id)};

	const auto classes{character.create().get_possible_classes()};
	for (const auto &[class_name, possible] : classes) {
		if (possible) {

			auto class_str{enum_name(class_name)};

			auto line{std::format("{:^11}", class_str)};
			items.emplace_back(std::move(line));
			data.emplace_back(std::to_underlying(class_name));
		}
	};
}

auto Sorcery::MenuBuilder::_load_character_items(
	std::string_view menu_name, std::vector<std::string> &items,
	std::vector<int> &data, const Enums::CharacterSlot source) -> void {

	if (!_ctx.game || _ctx.game->characters.empty())
		return;

	if (!_ctx.controller->has_character(source))
		return;

	const auto flags{_get_menu_flags(menu_name)};
	const auto char_id{_ctx.controller->get_character(source)};
	const auto &character{_ctx.game->characters.at(char_id)};

	const auto chance{
		character.abilities().at(Enums::Character::Ability::IDENTIFY_ITEMS)};

	unsigned int slot{1};

	for (const auto &item : character.inventory.items()) {

		const auto flag = [&]() -> std::string {
			if (!item.get_known())
				return "?";
			if (item.get_cursed() && item.get_equipped())
				return "-";
			if (item.get_equipped())
				return "*";
			if (!item.get_usable())
				return "#";
			return " ";
		}();

		std::string line;

		// IDENTIFY
		if (flags & MENU_IDENTIFY_ITEM) {
			if (item.get_known()) {
				line = std::format("{}){}{:<16}", slot, flag,
								   item.get_display_name());
			} else {
				line = std::format("{}){}{:<16} {:>5}%", slot, flag,
								   item.get_display_name(), chance);
			}
		}
		// EQUIP
		else if (flags & MENU_EQUIP_ITEM) {
			if (item.get_known()) {
				line = std::format("{}){}{:<16}", slot, flag,
								   item.get_display_name());
			} else {
				line = std::format("{}){}{:<16} {:>5}%", slot, flag,
								   item.get_display_name(), chance);
			}
		}
		// REMOVE
		else if (flags & MENU_REMOVE_ITEM) {
			if (item.get_known()) {
				line = std::format("{}){}{:<16}", slot, flag,
								   item.get_display_name());
			} else {
				line = std::format("{}){}{:<16} {:>5}%", slot, flag,
								   item.get_display_name(), chance);
			}
		}
		// USE
		else if (flags & MENU_USE_ITEM) {
			if (item.get_known() && item.get_usable()) {
				auto usage{
					_ctx.resources->items->get_item_type(item.get_type_id())
						.get_usage()};
				if (!usage.empty())
					usage = usage.substr(6);

				line = std::format("{}){}{:<16} {:>16}", slot, flag,
								   item.get_display_name(), usage);
			} else {
				line = std::format("{}){}{:<16}", slot, flag,
								   item.get_display_name());
			}
		}
		// SELL
		else if (flags & MENU_SHOP_SELL_ITEM) {
			const auto sell_to_shop_value{
				_ctx.resources->items->get_item_type(item.get_type_id())
					.get_value() /
				2};
			line = std::format("{}){}{:<16} {:>8} G.P.", slot, flag,
							   item.get_display_name(), sell_to_shop_value);
		} // SELL
		else if (flags & MENU_SHOP_IDENTIFY_ITEM) {
			const auto sell_to_shop_value{
				_ctx.resources->items->get_item_type(item.get_type_id())
					.get_value()};
			line = std::format("{}){}{:<16} {:>8} G.P.", slot, flag,
							   item.get_display_name(), sell_to_shop_value);
		} // UNCURSE
		else if (flags & MENU_SHOP_UNCURSE_ITEM) {
			const auto sell_to_shop_value{
				_ctx.resources->items->get_item_type(item.get_type_id())
					.get_value()};
			line = std::format("{}){}{:<16} {:>8} G.P.", slot, flag,
							   item.get_display_name(), sell_to_shop_value);
		}
		// INVOKE
		else if (flags & MENU_INVOKE_ITEM) {
			if (item.get_known() && item.get_usable()) {
				auto invokage{
					_ctx.resources->items->get_item_type(item.get_type_id())
						.get_invokage()};
				if (!invokage.empty())
					invokage = invokage.substr(6);

				line = std::format("{}){}{:<16} {:>16}", slot, flag,
								   item.get_display_name(), invokage);
			} else {
				line = std::format("{}){}{:<16}", slot, flag,
								   item.get_display_name());
			}
		}
		// DROP / TRADE (default)
		else {
			line =
				std::format("{}){}{:<16}", slot, flag, item.get_display_name());
		}

		items.emplace_back(std::move(line));
		data.emplace_back(slot);

		++slot;
	}
}
