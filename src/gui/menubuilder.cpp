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

#include "gui/menubuilder.hpp"
#include "core/context.hpp"
#include "core/controller.hpp"
#include "core/resources.hpp"
#include "gui/define.hpp"
#include "resources/itemstore.hpp"
#include "resources/monsterstore.hpp"
#include "resources/spellstore.hpp"
#include "resources/stringstore.hpp"
#include "types/game.hpp"
#include "types/state.hpp"

namespace {

using StringList = std::vector<std::string>;

const std::unordered_map<std::string, StringList> FIXED_MENUS = {

	{"compendium_menu",
	 {"COMPENDIUM_ATLAS", "COMPENDIUM_BESTIARY", "COMPENDIUM_GAZETTEER",
	  "COMPENDIUM_MUSEUM", "COMPENDIUM_SPELLBOOK", "COMPENDIUM_RETURN"}},

	{"camp_menu",
	 {"CAMP_INSPECT", "CAMP_REORDER", "CAMP_OPTIONS", "CAMP_QUIT",
	  "CAMP_LEAVE"}},

	{"modal_camp",
	 {"CAMP_INSPECT", "CAMP_REORDER", "CAMP_OPTIONS", "CAMP_QUIT",
	  "CAMP_LEAVE"}},

	{"roster_menu", {"ROSTER_RETURN"}},
	{"choose_menu", {"CHOOSE_RETURN"}},
	{"remove_menu", {"REMOVE_RETURN"}},
	{"reorder_menu", {"REORDER_RETURN"}},

	{"inspect_menu", {"INSPECT_RETURN"}},
	{"modal_inspect", {"INSPECT_RETURN"}},

	{"stay_menu", {"STAY_RETURN"}},
	{"modal_stay", {"STAY_RETURN"}},

	{"help_menu", {"HELP_RETURN"}},
	{"modal_help", {"HELP_RETURN"}},

	{"tithe_menu", {"TITHE_RETURN"}},
	{"modal_tithe", {"TITHE_RETURN"}},

	{"pay_menu", {"PAY_RETURN"}},

	{"add_menu", {"ADD_RETURN"}},
	{"restart_menu", {"RESTART_RETURN"}},

	{"castle_menu",
	 {"CASTLE_TAVERN", "CASTLE_INN", "CASTLE_SHOP", "CASTLE_TEMPLE",
	  "CASTLE_EDGE_OF_TOWN"}},

	{"rest_menu",
	 {"STAY_1", "STAY_2", "STAY_3", "STAY_4", "STAY_5", "STAY_RETURN"}},

	{"tavern_menu",
	 {"TAVERN_ADD_TO_PARTY", "TAVERN_REMOVE_FROM_PARTY", "TAVERN_REORDER_PARTY",
	  "TAVERN_INSPECT", "TAVERN_DIVVY_GOLD", "TAVERN_CASTLE"}},

	{"inn_menu", {"INN_STAY", "INN_INSPECT", "INN_CASTLE"}},

	{"temple_menu",
	 {"TEMPLE_HELP", "TEMPLE_INSPECT", "TEMPLE_TITHE", "TEMPLE_CASTLE"}},

	{"shop_menu", {"SHOP_ENTER", "SHOP_INSPECT", "SHOP_CASTLE"}},

	{"edge_menu",
	 {"EDGE_OF_TOWN_TRAIN", "EDGE_OF_TOWN_MAZE", "EDGE_OF_TOWN_RESTART",
	  "EDGE_OF_TOWN_CASTLE", "EDGE_OF_TOWN_LEAVE_GAME"}},

	{"atlas_menu",
	 {"ATLAS_MENU_B1F", "ATLAS_MENU_B2F", "ATLAS_MENU_B3F", "ATLAS_MENU_B4F",
	  "ATLAS_MENU_B5F", "ATLAS_MENU_B6F", "ATLAS_MENU_B7F", "ATLAS_MENU_B8F",
	  "ATLAS_MENU_B9F", "ATLAS_MENU_B10F", "ATLAS_RETURN"}},

	{"training_menu",
	 {"TRAINING_GROUNDS_CREATE", "TRAINING_GROUNDS_EDIT",
	  "TRAINING_GROUNDS_DELETE", "TRAINING_GROUNDS_INSPECT",
	  "TRAINING_GROUNDS_RETURN"}},

	{"method_menu",
	 {"METHOD_FULL", "METHOD_QUICK", "METHOD_RANDOM", "METHOD_RETURN"}},

	{"main_menu",
	 {"MAIN_MENU_OPTION_START", "MAIN_MENU_OPTION_CONTINUE",
	  "MAIN_MENU_OPTION_OPTIONS", "MAIN_MENU_OPTION_COMPENDIUM",
	  "MAIN_MENU_OPTION_LICENSE", "MAIN_MENU_OPTION_EXIT"}},
};

} // anonymous namespace

Sorcery::MenuBuilder::MenuBuilder(Context &ctx)
	: _ctx{ctx} {}

Sorcery::MenuBuilder::~MenuBuilder() {}

auto Sorcery::MenuBuilder::_load_party_characters(
	std::vector<std::string> &items, std::vector<int> &data, const int flags,
	const bool reorder) -> void {

	const auto party{_ctx.game->state->get_party_characters()};
	auto pos{1};

	for (const auto id : party) {
		const auto &character{_ctx.game->characters.at(id)};
		if (character.get_location() != Enums::Character::Location::PARTY)
			continue;

		if (flags & MENU_SHOW_POSITION)
			items.emplace_back(
				std::format("{}:{:^19}", pos, character.get_name()));
		else if (flags & MENU_SHOW_GOLD)
			items.emplace_back(std::format("{:<16} {:>8} G.P.",
										   character.get_name(),
										   character.get_gold()));
		else
			items.emplace_back(std::format("{:^21}", character.get_name()));

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

			items.emplace_back(std::format("{:^21}", character.get_name()));
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

			items.emplace_back(std::format("{:^21}", character.get_name()));
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

		items.emplace_back(
			std::format("{}##{}", padded, unenum(monster.get_type_id())));
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

auto Sorcery::MenuBuilder::_load_museum_menu(unsigned int width,
											 std::vector<std::string> &items)
	-> void {

	for (const auto &item_type : _ctx.resources->items->get_all_types()) {

		if (item_type.get_type_id() == Enums::Items::TypeID::BROKEN_ITEM)
			continue;

		const auto padded{
			std::format("{:^{}}", item_type.get_known_name(), width)};

		items.emplace_back(
			std::format("{}##{}", padded, unenum(item_type.get_type_id())));
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

	// -------- Dynamic menus --------
	if (menu_name == "roster_menu" || menu_name == "choose_menu" ||
		menu_name == "inspect_menu" || menu_name == "remove_menu" ||
		menu_name == "modal_inspect") {

		_load_party_characters(items, data, NO_FLAGS, reorder);
		return;
	}

	if (menu_name == "tithe_menu" || menu_name == "modal_tithe" ||
		menu_name == "pay_menu") {

		_load_party_characters(items, data, MENU_SHOW_GOLD, reorder);
		return;
	}

	if (menu_name == "reorder_menu") {
		_load_party_characters(items, data, MENU_SHOW_POSITION, reorder);
		return;
	}

	if (menu_name == "add_menu") {
		_load_tavern_characters(items, data);
		return;
	}

	if (menu_name == "restart_menu") {
		_load_maze_characters(items, data);
		return;
	}

	if (menu_name == "modal_help") {
		_load_sick_characters(items, data);
		return;
	}

	if (menu_name == "modal_identify" || menu_name == "modal_drop" ||
		menu_name == "modal_trade" || menu_name == "modal_use" ||
		menu_name == "modal_invoke") {

		_load_character_items(menu_name, items, data);
		return;
	}

	// -------- Enumerated menus --------
	if (menu_name == "bestiary_menu") {
		_load_bestiary_menu(width, items);
		return;
	}

	if (menu_name == "spellbook_menu") {
		_load_spellbook_menu(width, items);
		return;
	}

	if (menu_name == "museum_menu") {
		_load_museum_menu(width, items);
		return;
	}

	// -------- Fixed menus --------
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

auto Sorcery::MenuBuilder::_get_item_menu_flags(
	std::string_view menu_name) const -> int {

	if (menu_name == "modal_identify")
		return MENU_IDENTIFY_ITEM;
	if (menu_name == "modal_drop")
		return MENU_DROP_ITEM;
	if (menu_name == "modal_trade")
		return MENU_TRADE_ITEM;
	if (menu_name == "modal_use")
		return MENU_USE_ITEM;
	if (menu_name == "modal_invoke")
		return MENU_INVOKE_ITEM;

	return NO_FLAGS;
}

auto Sorcery::MenuBuilder::_load_character_items(
	std::string_view menu_name, std::vector<std::string> &items,
	std::vector<int> &data) -> void {

	if (!_ctx.game || _ctx.game->characters.empty())
		return;

	if (!_ctx.controller->has_character("inspect"))
		return;

	const auto flags{_get_item_menu_flags(menu_name)};
	const auto char_id{_ctx.controller->get_character("inspect")};
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
