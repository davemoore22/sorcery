// Copyright (C) 2025 Dave Moore
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

#include "gui/menu.hpp"
#include "common/macro.hpp"
#include "core/controller.hpp"
#include "core/resources.hpp"
#include "core/system.hpp"
#include "core/ui.hpp"
#include "gui/define.hpp"
#include "gui/dialog.hpp"
#include "gui/modal.hpp"
#include "types/component.hpp"
#include "types/game.hpp"
#include "types/state.hpp"

Sorcery::Menu::Menu(System *system, Resources *resources, UI *ui,
					Controller *controller, Component *component, Game *game)
	: _system{system},
	  _resources{resources},
	  _ui{ui},
	  _controller{controller},
	  _component{component},
	  _game{game} {

	_name = _component->name;
	_pos = ImVec2{_component->x, _component->y};
	_width = std::stoi((*_component)["width"].value());
	_height = std::stoi((*_component)["height"].value());
	_colour = _component->colour;
	_bg_colour = _component->background;
	_hi_colour = std::stof((*_component)["highlight"].value());
	_font = _component->font;
	if ((*_component)["reorder"])
		_reorder = (*_component)["reorder"].value() == "yes";
	else
		_reorder = false;
	if ((*_component)["across"])
		_across = (*_component)["across"].value() == "yes";
	else
		_across = false;
}

// Handle special menu items
auto Sorcery::Menu::regenerate() -> void {

	_load_dynamic_items();
	_load_fixed_items();
}

auto Sorcery::Menu::_load_fixed_items() -> void {

	std::vector<std::string> sources;
	if (_component->name == "compendium_menu") {
		sources.insert(sources.end(),
					   {"COMPENDIUM_ATLAS", "COMPENDIUM_BESTIARY",
						"COMPENDIUM_GAZETTEER", "COMPENDIUM_MUSEUM",
						"COMPENDIUM_SPELLBOOK", "COMPENDIUM_RETURN"});
		for (const auto &source : sources)
			_items.emplace_back(
				fmt::format("{:^{}}", (*_system->strings)[source], _width));

	} else if (_component->name == "camp_menu") {
		sources.insert(sources.end(),
					   {"CAMP_INSPECT", "CAMP_REORDER", "CAMP_OPTIONS",
						"CAMP_QUIT", "CAMP_LEAVE"});
		for (const auto &source : sources)
			_items.emplace_back(
				fmt::format("{:^{}}", (*_system->strings)[source], _width));

	} else if (_component->name == "roster_menu") {
		sources.insert(sources.end(), {"ROSTER_RETURN"});
		for (const auto &source : sources)
			_items.emplace_back(
				fmt::format("{:^{}}", (*_system->strings)[source], _width));

	} else if (_component->name == "identify_menu" ||
			   _component->name == "modal_identify") {
		sources.insert(sources.end(), {"IDENTIFY_RETURN"});
		for (const auto &source : sources)
			_items.emplace_back(
				fmt::format("{:^{}}", (*_system->strings)[source], _width));

	} else if (_component->name == "inspect_menu") {
		sources.insert(sources.end(), {"INSPECT_RETURN"});
		for (const auto &source : sources)
			_items.emplace_back(
				fmt::format("{:^{}}", (*_system->strings)[source], _width));

	} else if (_component->name == "choose_menu") {
		sources.insert(sources.end(), {"CHOOSE_RETURN"});
		for (const auto &source : sources)
			_items.emplace_back(
				fmt::format("{:^{}}", (*_system->strings)[source], _width));

	} else if (_component->name == "stay_menu") {
		sources.insert(sources.end(), {"STAY_RETURN"});
		for (const auto &source : sources)
			_items.emplace_back(
				fmt::format("{:^{}}", (*_system->strings)[source], _width));

	} else if (_component->name == "pay_menu") {
		sources.insert(sources.end(), {"PAY_RETURN"});
		for (const auto &source : sources)
			_items.emplace_back(
				fmt::format("{:^{}}", (*_system->strings)[source], _width));

	} else if (_component->name == "help_menu") {
		sources.insert(sources.end(), {"HELP_RETURN"});
		for (const auto &source : sources)
			_items.emplace_back(
				fmt::format("{:^{}}", (*_system->strings)[source], _width));

	} else if (_component->name == "tithe_menu") {
		sources.insert(sources.end(), {"TITHE"});
		for (const auto &source : sources)
			_items.emplace_back(
				fmt::format("{:^{}}", (*_system->strings)[source], _width));

	} else if (_component->name == "remove_menu") {
		sources.insert(sources.end(), {"REMOVE_RETURN"});
		for (const auto &source : sources)
			_items.emplace_back(
				fmt::format("{:^{}}", (*_system->strings)[source], _width));

	} else if (_component->name == "add_menu") {
		sources.insert(sources.end(), {"ADD_RETURN"});
		for (const auto &source : sources)
			_items.emplace_back(
				fmt::format("{:^{}}", (*_system->strings)[source], _width));

	} else if (_component->name == "restart_menu") {
		sources.insert(sources.end(), {"RESTART_RETURN"});
		for (const auto &source : sources)
			_items.emplace_back(
				fmt::format("{:^{}}", (*_system->strings)[source], _width));

	} else if (_component->name == "reorder_menu") {
		sources.insert(sources.end(), {"REORDER_RETURN"});
		for (const auto &source : sources)
			_items.emplace_back(
				fmt::format("{:^{}}", (*_system->strings)[source], _width));

	} else if (_component->name == "castle_menu") {
		sources.insert(sources.end(),
					   {"CASTLE_TAVERN", "CASTLE_INN", "CASTLE_SHOP",
						"CASTLE_TEMPLE", "CASTLE_EDGE_OF_TOWN"});
		for (const auto &source : sources)
			_items.emplace_back(
				fmt::format("{:^{}}", (*_system->strings)[source], _width));

	} else if (_component->name == "rest_menu") {

		sources.insert(sources.end(), {"STAY_1", "STAY_2", "STAY_3", "STAY_4",
									   "STAY_5", "STAY_RETURN"});
		for (const auto &source : sources)
			_items.emplace_back(
				fmt::format("{:^{}}", (*_system->strings)[source], _width));

	} else if (_component->name == "tavern_menu") {
		sources.insert(sources.end(),
					   {"TAVERN_ADD_TO_PARTY", "TAVERN_REMOVE_FROM_PARTY",
						"TAVERN_REORDER_PARTY", "TAVERN_INSPECT",
						"TAVERN_DIVVY_GOLD", "TAVERN_CASTLE"});
		for (const auto &source : sources)
			_items.emplace_back(
				fmt::format("{:^{}}", (*_system->strings)[source], _width));

	} else if (_component->name == "inn_menu") {
		sources.insert(sources.end(),
					   {"INN_STAY", "INN_INSPECT", "INN_CASTLE"});
		for (const auto &source : sources)
			_items.emplace_back(
				fmt::format("{:^{}}", (*_system->strings)[source], _width));

	} else if (_component->name == "temple_menu") {
		sources.insert(sources.end(), {"TEMPLE_HELP", "TEMPLE_INSPECT",
									   "TEMPLE_TITHE", "TEMPLE_CASTLE"});
		for (const auto &source : sources)
			_items.emplace_back(
				fmt::format("{:^{}}", (*_system->strings)[source], _width));

	} else if (_component->name == "shop_menu") {
		sources.insert(sources.end(),
					   {"SHOP_ENTER", "SHOP_INSPECT", "SHOP_CASTLE"});
		for (const auto &source : sources)
			_items.emplace_back(
				fmt::format("{:^{}}", (*_system->strings)[source], _width));

	} else if (_component->name == "edge_menu") {
		sources.insert(sources.end(),
					   {"EDGE_OF_TOWN_TRAIN", "EDGE_OF_TOWN_MAZE",
						"EDGE_OF_TOWN_RESTART", "EDGE_OF_TOWN_CASTLE",
						"EDGE_OF_TOWN_LEAVE_GAME"});
		for (const auto &source : sources)
			_items.emplace_back(
				fmt::format("{:^{}}", (*_system->strings)[source], _width));

	} else if (_component->name == "atlas_menu") {
		sources.insert(sources.end(),
					   {"ATLAS_MENU_B1F", "ATLAS_MENU_B2F", "ATLAS_MENU_B3F",
						"ATLAS_MENU_B4F", "ATLAS_MENU_B5F", "ATLAS_MENU_B6F",
						"ATLAS_MENU_B7F", "ATLAS_MENU_B8F", "ATLAS_MENU_B9F",
						"ATLAS_MENU_B10F", "ATLAS_RETURN"});
		for (const auto &source : sources)
			_items.emplace_back(
				fmt::format("{:^{}}", (*_system->strings)[source], _width));

	} else if (_component->name == "training_menu") {
		sources.insert(sources.end(),
					   {"TRAINING_GROUNDS_CREATE", "TRAINING_GROUNDS_EDIT",
						"TRAINING_GROUNDS_DELETE", "TRAINING_GROUNDS_INSPECT",
						"TRAINING_GROUNDS_RETURN"});
		for (const auto &source : sources)
			_items.emplace_back(
				fmt::format("{:^{}}", (*_system->strings)[source], _width));

	} else if (_component->name == "main_menu") {
		sources.insert(sources.end(),
					   {"MAIN_MENU_OPTION_START", "MAIN_MENU_OPTION_CONTINUE",
						"MAIN_MENU_OPTION_OPTIONS",
						"MAIN_MENU_OPTION_COMPENDIUM",
						"MAIN_MENU_OPTION_LICENSE", "MAIN_MENU_OPTION_EXIT"});
		for (const auto &source : sources)
			_items.emplace_back(
				fmt::format("{:^{}}", (*_system->strings)[source], _width));

	} else if (_component->name == "bestiary_menu") {
		const auto monster_types{_resources->monsters->get_all_types()};
		for (auto &monster : monster_types) {
			if (monster.get_type_id() > Enums::Monsters::TypeID::WERDNA)
				continue;
			auto mname{monster.get_known_name()};
			auto mid{unenum(monster.get_type_id())};
			auto padded{fmt::format("{:^{}}", mname, _width)};
			auto menu_item{fmt::format("{}##{}", padded, mid)};
			_items.emplace_back(fmt::format("{}", menu_item));
		}
		_items.emplace_back(fmt::format(
			"{:^{}}", (*_system->strings)["BESTIARY_RETURN"], _width));

	} else if (_component->name == "spellbook_menu") {
		const auto spells{_resources->spells->get_all()};
		for (auto &spell : spells) {
			auto sname{spell.name};
			auto padded{fmt::format("{:^{}}", sname, _width)};
			_items.emplace_back(fmt::format("{}", padded));
		}
		_items.emplace_back(fmt::format(
			"{:^{}}", (*_system->strings)["SPELLBOOK_RETURN"], _width));

	} else if (_component->name == "museum_menu") {
		const auto item_types{_resources->items->get_all_types()};
		for (auto &item_type : item_types) {
			if (item_type.get_type_id() != Enums::Items::TypeID::BROKEN_ITEM) {
				auto iname{item_type.get_known_name()};
				auto iid{unenum(item_type.get_type_id())};
				auto padded{fmt::format("{:^{}}", iname, _width)};
				auto menu_item{fmt::format("{}##{}", padded, iid)};
				_items.emplace_back(fmt::format("{}", menu_item));
			}
		}
		_items.emplace_back(fmt::format(
			"{:^{}}", (*_system->strings)["MUSEUM_RETURN"], _width));
	}
}

// Not used for Modal Menus - see UI->load_dynamic_menu_items() instead
auto Sorcery::Menu::_load_dynamic_items() -> void {

	// Handle special menu items
	_items.clear();
	_data.clear();
	if (_name == "roster_menu" && _game != nullptr)
		_load_party_characters(NO_FLAGS);
	else if (_name == "choose_menu" && _game != nullptr)
		_load_party_characters(NO_FLAGS);
	else if (_name == "tithe_menu" && _game != nullptr)
		_load_party_characters(MENU_SHOW_GOLD);
	else if (_name == "pay_menu" && _game != nullptr)
		_load_party_characters(MENU_SHOW_GOLD);
	else if (_name == "inspect_menu" && _game != nullptr)
		_load_party_characters(NO_FLAGS);
	else if ((_name == "help_menu" || _name == "modal_help") &&
			 _game != nullptr)
		_load_sick_characters();
	else if (_name == "remove_menu" && _game != nullptr)
		_load_party_characters(NO_FLAGS);
	else if (_name == "add_menu" && _game != nullptr)
		_load_tavern_characters();
	else if (_name == "restart_menu" && _game != nullptr)
		_load_maze_characters();
	else if (_name == "reorder_menu" && _game != nullptr)
		_load_party_characters(MENU_SHOW_POSITION);
}

auto Sorcery::Menu::draw() -> void {

	with_Window(WINDOW_LAYER_MENUS, nullptr,
				ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoTitleBar) {

		const auto col{_ui->get_hl_colour(_system->animation->lerp)};
		const auto sz{
			ImVec2{static_cast<float>(_width * _ui->font_sz),
				   static_cast<float>(
					   (_height * ImGui::GetTextLineHeightWithSpacing()) + 2)}};

		// Note that _pos is in grid units whereas sz is in pixels!
		_ui->draw_menu(_name, col, _pos, sz, _font, _items, _data, _reorder,
					   _across);

		// Handle SpecialEvents such as Reordering Party Menu
		if (_controller->has_flag("party_order_changed")) {

			_game->state->reorder_party(_controller->candidate_party);
			_load_dynamic_items();
			_controller->candidate_party.clear();
			_controller->unset_flag("party_order_changed");
		}
	}
}

// NOT USED HERE - see methods in ui.cpp
auto Sorcery::Menu::_load_sick_characters() -> void {

	using Enums::Character::CStatus;

	if (!_game->characters.empty()) {
		for (auto &[id, character] : _game->characters) {
			if (character.get_status() == CStatus::ASHES ||
				character.get_status() == CStatus::DEAD ||
				character.get_status() == CStatus::HELD) {
				_items.emplace_back(fmt::format("{:^16} ({:^<8})",
												character.get_name(),
												character.get_status_string()));
				_data.emplace_back(id);
			}
		}
	}
}

// Get currently party character names
auto Sorcery::Menu::_load_party_characters(const int flags) -> void {

	const auto party{_game->state->get_party_characters()};
	if (!party.empty()) {
		auto pos{1};
		for (const auto &id : party) {
			const auto &character{_game->characters.at(id)};
			if (character.get_location() == Enums::Character::Location::PARTY) {
				if (flags & MENU_SHOW_POSITION)
					_items.emplace_back(
						fmt::format("{}:{:^19}", pos, character.get_name()));
				else if (flags & MENU_SHOW_GOLD)
					_items.emplace_back(fmt::format("{:<16} {:>8} G.P.",
													character.get_name(),
													character.get_gold()));
				else
					_items.emplace_back(
						fmt::format("{:^21}", character.get_name()));
				_data.emplace_back(id);
				if (_reorder)
					_controller->candidate_party.emplace_back(id);
				++pos;
			}
		}
	}
}

// Get currently "out" character names
auto Sorcery::Menu::_load_maze_characters() -> void {

	if (!_game->characters.empty()) {
		for (auto &[id, character] : _game->characters) {
			if (character.get_location() == Enums::Character::Location::MAZE &&
				character.get_status() == Enums::Character::CStatus::OK) {
				_items.emplace_back(
					fmt::format("{:^21}", character.get_name()));
				_data.emplace_back(id);
			}
		};
	}
}

auto Sorcery::Menu::_load_tavern_characters() -> void {

	if (!_game->characters.empty()) {
		for (auto &[id, character] : _game->characters) {
			if (character.get_location() ==
				Enums::Character::Location::TAVERN) {
				_items.emplace_back(
					fmt::format("{:^21}", character.get_name()));
				_data.emplace_back(id);
			}
		}
	}
}
