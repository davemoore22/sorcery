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

#include "core/controller/menuhandler.hpp" // for ScreenRenderer
#include "common/enum.hpp"				   // for Attribute, Attribute::LUCK
#include "core/context.hpp"				   // for Context
#include "core/controller/controller.hpp"  // for Controller
#include "core/controller/menuaction.hpp"  // for Controller
#include "core/debug.hpp"
#include "core/define.hpp" // for WINDOW_LAYER_MENUS, WINDOW_...
#include "core/enum.hpp"   // for Screen, CharacterSlot
#include "core/resources.hpp"
#include "display/display.hpp"			// for Display, DisplayMetrics
#include "display/render.hpp"			// for Render
#include "display/ui/popupmanager.hpp"	// for PopupManager
#include "display/ui/popupstore.hpp"	// for PopupStore
#include "display/ui/ui.hpp"			// for UI
#include "display/ui/uimetrics.hpp"		// for UIMetrics
#include "display/ui/uistyle.hpp"		// for set_text_dim
#include "drawables/define.hpp"			// for RECOVERY_BIRTHDAY, CHOOSE_M...
#include "drawables/dialog.hpp"			// for Dialog
#include "drawables/modal.hpp"			// for Modal
#include "engine/define.hpp"			// for CHEST_GFX_ID
#include "imgui.h"						// for ImVec2, ImGuiWindowFlags_
#include "resources/componentstore.hpp" // for ComponentStore
#include "resources/define.hpp"			// for EVENTS_TEXTURE
#include "resources/fontstore.hpp"		// for FontStore
#include "resources/itemstore.hpp"
#include "types/character/character.hpp" // for Character
#include "types/character/create.hpp"	 // for CharacterCreate
#include "types/component.hpp"			 // for Component
#include "types/game.hpp"				 // for Game
#include "types/meta.hpp"				 // for enum_cast
#include <any>							 // for any_cast, any
#include <format>						 // for format
#include <functional>					 // for invoke
#include <imgui_sugar.hpp>				 // for BooleanGuard, with_Window
#include <map>							 // for map
#include <memory>						 // for unique_ptr
#include <optional>						 // for optional
#include <string>						 // for basic_string, string
#include <unordered_map>				 // for unordered_map, operator==
#include <utility>						 // for pair, to_underlying
#include <vector>						 // for vector

Sorcery::ControllerMenuHandler::ControllerMenuHandler(Controller &host,
													  Context &ctx)
	: _host{host},
	  _ctx{ctx} {

	  };

auto Sorcery::ControllerMenuHandler::handle_standard(
	std::string_view component, const std::vector<std::string> &items, int data,
	int selection) -> void {

	DEBUG_LOGF("Standard Menu: {} {} {}", component, data, selection);

	if (component == "remove_character_menu") {

		if (selection == (static_cast<int>(items.size()) - 1))
			_host.go_to(Enums::Screen::TAVERN);
		else {

			// if we can, remove the character from the party
			auto &character{_host._game->characters[data]};
			character.set_location(Enums::Character::Location::TAVERN);
			_host._game->state->remove_character_by_id(data);
			_host._game->save_game();
		}

	} else if (component == "chest_menu") {
		if (selection == static_cast<int>(items.size()) - 1) {

			_host.clear_character(Enums::CharacterSlot::TRAP);
			_host.set_flag("chest_character_cancelled");

		} else {

			_host.set_selected("chest_menu_action", selection);
		}

	} else if (component == "inn_menu") {

		// Get the Character ID of the Selected Character and set it
		if (selection == (static_cast<int>(items.size()) - 1)) {
			_host.clear_character(Enums::CharacterSlot::STAY);
			_host.go_to(Enums::Screen::CASTLE);
		} else
			_host.set_character(Enums::CharacterSlot::STAY, data);

	} else if (component == "shop_menu") {

		// Get the Character ID of the Selected Character and set it
		if (selection == (static_cast<int>(items.size()) - 1)) {
			_host.clear_character(Enums::CharacterSlot::STORE);
			_host.go_to(Enums::Screen::CASTLE);
		} else
			_host.set_character(Enums::CharacterSlot::STORE, data);

	} else if (component == "restart_menu") {

		// Restart Menu
		if (selection == (static_cast<int>(items.size()) - 1))
			_host.go_to(Enums::Screen::EDGEOFTOWN);
		else {

			// Get the ID of the Character if we can, add the character to
			// the party
			_host.set_character(Enums::CharacterSlot::RESTART, data);
			_host._flags["want_restart_expedition"] = true;
		}

	} else if (component == "add_menu") {

		if (selection == (static_cast<int>(items.size()) - 1))
			_host.go_to(Enums::Screen::TAVERN);
		else {

			// if we can, add the character to the party
			auto &character{_host._game->characters[data]};
			character.set_location(Enums::Character::Location::PARTY);
			_host._game->state->add_character_to_party(data);
			_host._game->save_game();
		}

	} else if (component == "race_menu") {

		if (selection == (static_cast<int>(items.size()) - 1))
			_host.go_to(Enums::Screen::TRAINING);
		else {
			_host._game->creation_candidate->create().set_race(
				enum_cast<Enums::Character::Race>(selection + 1).value());
			_host._game->creation_candidate->create().set_stage(
				Enums::Character::Stage::CHOOSE_ALIGNMENT);
			_host._game->creation_candidate->create().set_start_attr();
		}
	} else if (component == "alignment_menu") {

		if (selection == (static_cast<int>(items.size()) - 1))
			_host.go_to(Enums::Screen::TRAINING);
		else {
			_host._game->creation_candidate->create().set_alignment(
				enum_cast<Enums::Character::Align>(selection + 1).value());
			_host._game->creation_candidate->create().set_stage(
				Enums::Character::Stage::CHOOSE_CLASS);
			_host._game->creation_candidate->create().set_start_attr();
			_host._game->creation_candidate->create().set_possible_classes();
		}
	} else if (component == "class_menu") {

		if (selection == (static_cast<int>(items.size()) - 1))
			_host.go_to(Enums::Screen::TRAINING);
		else {
			auto candidate{_host._game->creation_candidate};
			if (candidate->create().get_points_left() == 0) {

				candidate->create().set_class(
					enum_cast<Enums::Character::Class>(selection + 1).value());
				candidate->create().set_stage(
					Enums::Character::Stage::REVIEW_AND_CONFIRM);
				candidate->create().finalise();

				// TODO: refactor this
				candidate->inventory.clear();

				switch (
					candidate->get_class()) { // NOLINT(clang-diagnostic-switch)
					using enum Enums::Character::Class;
					using enum Enums::Items::TypeID;
				case FIGHTER:
				case LORD:
				case SAMURAI:
					candidate->inventory.add_type(
						_ctx.resources->items->get(LEATHER_ARMOR), true);
					candidate->inventory.add_type(
						_ctx.resources->items->get(LONG_SWORD), true);
					break;
				case MAGE:
					candidate->inventory.add_type(
						_ctx.resources->items->get(ROBES), true);
					candidate->inventory.add_type(
						_ctx.resources->items->get(DAGGER), true);
					break;
				case PRIEST:
				case BISHOP:
					candidate->inventory.add_type(
						_ctx.resources->items->get(ROBES), true);
					candidate->inventory.add_type(
						_ctx.resources->items->get(STAFF), true);
					break;
				case THIEF:
				case NINJA:
					candidate->inventory.add_type(
						_ctx.resources->items->get(LEATHER_ARMOR), true);
					candidate->inventory.add_type(
						_ctx.resources->items->get(SHORT_SWORD), true);
				default:
					break;
				}

				_host.set_flag("want_choose_confirm");
				_host.unset_flag("want_choose_class");
			}
		};
	} else if (component == "reorder_menu") {

		// Reorder has multiple entry points so need to rely upon calling
		// screen to enable itself
		if (selection == (static_cast<int>(items.size()) - 1)) {
			_host._flags["show_reorder"] = false;
			_host.request_back();
		}
	} else if (component == "pay_menu") {

		if (selection == (static_cast<int>(items.size()) - 1))
			_host._flags["show_pay"] = false;
		else
			_host._selected["pay_selected"] = selection;
	} else if (component == "shop_menu") {

		// Shop
		if (selection == (static_cast<int>(items.size()) - 1))
			_host.go_to(Enums::Screen::CASTLE);
		else {
		}
	} else if (component == "bestiary_menu") {

		// Bestiary
		_host._selected["bestiary_selected"] = selection;
		if (selection == (static_cast<int>(items.size()) - 1))
			_host.go_to(Enums::Screen::COMPENDIUM);
	} else if (component == "museum_menu") {

		// Museum
		_host._selected["museum_selected"] = selection;
		if (selection == (static_cast<int>(items.size()) - 1))
			_host.go_to(Enums::Screen::COMPENDIUM);
	} else if (component == "atlas_menu") {

		// Atlas
		_host._selected["atlas_selected"] = selection;
		if (selection == (static_cast<int>(items.size()) - 1))
			_host.go_to(Enums::Screen::COMPENDIUM);
	} else if (component == "spellbook_menu") {

		// Spellbook
		_host._selected["spellbook_selected"] = selection;
		if (selection == (static_cast<int>(items.size()) - 1))
			_host.go_to(Enums::Screen::COMPENDIUM);
	} else if (component == "choose_menu") {

		// Character Selection
		if (selection == (static_cast<int>(items.size()) - 1)) {
			_host._flags["show_choose"] = false;
			_host.clear_character(Enums::CharacterSlot::CHOOSE);
		} else
			_host.set_character(Enums::CharacterSlot::CHOOSE, data);
	} else if (component == "shop_menu") {

		// Boltacs
		_host._selected["store_selected"] = selection;
		if (selection == (static_cast<int>(items.size()) - 1))
			_host.go_to(Enums::Screen::SHOP);
		else
			_host.go_to(Enums::Screen::STORE);
	} else if (component == "store_menu") {

		// Store
		_host._selected["store_selected"] = selection;
		if (selection == (static_cast<int>(items.size()) - 1))
			_host.go_to(Enums::Screen::SHOP);
	}
}

auto Sorcery::ControllerMenuHandler::handle_dynamic(
	std::string_view component, const std::vector<std::string> &items, int data,
	int selection) -> bool {

	DEBUG_LOGF("Dynamic Menu: {} {} {}", component, data, selection);

	if (component == "inspect_menu") {

		// Get the Character ID of the Selected Character and set it
		if (selection == (static_cast<int>(items.size()) - 1)) {
			_host.clear_character(Enums::CharacterSlot::INSPECT);
			_host.request_back();
		} else
			_host.set_character(Enums::CharacterSlot::INSPECT, data);

		return true;
	} else if (component == "chest_open_menu" ||
			   component == "chest_calfo_menu" ||
			   component == "chest_inspect_menu" ||
			   component == "chest_disarm_menu") {

		if (selection == static_cast<int>(items.size()) - 1) {

			_host.clear_character(Enums::CharacterSlot::TRAP);

			_host.set_flag("chest_character_cancelled");

		} else {

			_host.set_character(Enums::CharacterSlot::TRAP, data);
		}

		_ctx.ui->popup_manager->close();

		return true;

	} else if (component == "chest_trap_menu") {

		if (selection == static_cast<int>(items.size()) - 1) {

			_host.set_flag("chest_trap_cancelled");

		} else {

			_host.set_selected("chest_trap_selection", selection);
		}

		_ctx.ui->popup_manager->close();

		return true;
	} else if (component == "change_class_menu") {
		if (selection == (static_cast<int>(items.size()) - 1)) {
			_host.clear_character(Enums::CharacterSlot::EDIT);
			_host.go_to(Enums::Screen::EDIT);
		} else {

			auto &character{_host._game->characters.at(
				_host.get_character(Enums::CharacterSlot::EDIT))};
			const auto class_to_change_to{
				enum_cast<Enums::Character::Class>(data).value()};
			character.create().change_class(class_to_change_to);

			_host._game->save_game();

			_ctx.ui->popup_manager->open_dialog("global:notice_reclassed_ok",
												Enums::Layout::DialogType::OK);
		}

		return true;

	} else if (component == "delete_menu") {

		if (selection == static_cast<int>(items.size()) - 1) {

			_host.clear_character(Enums::CharacterSlot::EDIT);
			_host.go_to(Enums::Screen::TRAINING);
		} else {

			_host.set_character(Enums::CharacterSlot::EDIT, data);
		}

		return true;
	} else if (component == "roster_menu") {

		// Get the Character ID of the Selected Character and set it
		if (selection == (static_cast<int>(items.size()) - 1)) {
			_host.clear_character(Enums::CharacterSlot::INSPECT);
			_host.go_to(Enums::Screen::TRAINING);
		} else
			_host.set_character(Enums::CharacterSlot::INSPECT, data);
		return true;
	} else if (component == "select_menu") {

		// Get the Character ID of the Selected Character and set it
		if (selection == (static_cast<int>(items.size()) - 1)) {
			_host.clear_character(Enums::CharacterSlot::EDIT);
			_host.go_to(Enums::Screen::EDIT);
		} else
			_host.set_character(Enums::CharacterSlot::EDIT, data);
		return true;
	} else if (component == "retrain_menu") {

		// Get the Character ID of the Selected Character and set it
		if (selection == (static_cast<int>(items.size()) - 1)) {
			_host.clear_character(Enums::CharacterSlot::EDIT);
			_host.go_to(Enums::Screen::EDIT);
		} else
			_host.set_character(Enums::CharacterSlot::EDIT, data);
		return true;
	} else if (component == "legate_menu") {

		// Get the Character ID of the Selected Character and set it
		if (selection == (static_cast<int>(items.size()) - 1)) {
			_host.clear_character(Enums::CharacterSlot::EDIT);
			_host.go_to(Enums::Screen::EDIT);
		} else
			_host.set_character(Enums::CharacterSlot::EDIT, data);
		return true;
	} else if (component == "temple_heal_menu") {

		if (selection == static_cast<int>(items.size()) - 1) {

			_host.clear_character(Enums::CharacterSlot::HELP);
			_host.go_to(Enums::Screen::CASTLE);
		} else {

			_host.set_character(Enums::CharacterSlot::HELP, data);
		}

		return true;
	} else if (component == "temple_pay_menu") {

		if (selection == static_cast<int>(items.size()) - 1) {

			_host.clear_character(Enums::CharacterSlot::PAY);
			_host.go_to(Enums::Screen::TEMPLE);
		} else {
			_host.set_character(Enums::CharacterSlot::PAY, data);
		}

		return true;
	} else if (component == "identify_menu") {

		if (selection == static_cast<int>(items.size()) - 1) {

			_host.set_flag("want_identify");
			_ctx.ui->popup_manager->close();

		} else {

			// TODO
		}

		return true;
	} else if (component == "remove_item_menu") {

		if (selection == static_cast<int>(items.size()) - 1) {

			_host.set_flag("want_remove");
			_ctx.ui->popup_manager->close();

		} else {

			// TODO
		}

		return true;
	} else if (component == "equip_menu") {

		if (selection == static_cast<int>(items.size()) - 1) {

			_host.set_flag("want_equip");
			_ctx.ui->popup_manager->close();

		} else {

			// TODO
		}

		return true;
	} else if (component == "spell_menu") {

		if (selection == static_cast<int>(items.size()) - 1) {

			_host.set_flag("want_spell");
			_ctx.ui->popup_manager->close();

		} else {

			// TODO
		}

		return true;
	} else if (component == "drop_menu") {

		if (selection == static_cast<int>(items.size()) - 1) {

			_host.set_flag("want_drop");
			_ctx.ui->popup_manager->close();

		} else {

			// TODO
		}

		return true;
	} else if (component == "trade_menu") {

		if (selection == static_cast<int>(items.size()) - 1) {

			_host.unset_selected("trade_item_selected");

			_ctx.ui->popup_manager->close();

			return true;
		}

		_host.set_selected("trade_item_selected", data);

		_ctx.ui->popup_manager->open_modal("global:modal_give");

		return true;
	} else if (component == "give_menu") {

		if (selection == static_cast<int>(items.size()) - 1) {

			_host.unset_selected("trade_item_selected");

			_host.unset_selected("trade_target_selected");

			_ctx.ui->popup_manager->close();

			return true;
		}

		_host.set_selected("trade_target_selected", data);

		//
		// TODO: Perform the actual trade.
		//

		const auto source{_host.get_character(Enums::CharacterSlot::INSPECT)};

		const auto item_slot{_host.get_selected("trade_item_selected")};

		const auto target{_host.get_selected("trade_target_selected")};

		DEBUG_LOGF("Trade item slot {} from character {} to character {}",
				   item_slot, source, target);

		_host.unset_selected("trade_item_selected");

		_host.unset_selected("trade_target_selected");

		_ctx.ui->popup_manager->close();

		return true;
	} else if (component == "use_menu") {

		if (selection == static_cast<int>(items.size()) - 1) {

			_host.set_flag("want_use");
			_ctx.ui->popup_manager->close();

		} else {

			// TODO
		}

		return true;
	}

	else if (component == "invoke_menu") {

		if (selection == static_cast<int>(items.size()) - 1) {

			_host.set_flag("want_invoke");
			_ctx.ui->popup_manager->close();

		} else {

			// TODO
		}

		return true;
	}

	return false;
}

auto Sorcery::ControllerMenuHandler::handle_actions(std::string_view menu,
													int selection, int data)
	-> bool {

	DEBUG_LOGF("Action Table Menu: {} {} {}", menu, selection, data);

	const auto it{MENU_ACTIONS.find(menu)};
	if (it == MENU_ACTIONS.end())
		return false;

	if (selection < 0 || selection >= static_cast<int>(it->second.size()))
		return false;

	const auto &actions = it->second[selection];
	for (const auto &action : actions)
		_execute(action, data);

	return true;
}

auto Sorcery::ControllerMenuHandler::item_disabled(std::string_view component,
												   int selection, int data)
	-> bool {

	// Remember this is returning true if the item is meant to be disabled!
	if (component == "main_menu" && selection == MAIN_MENU_CONTINUE_GAME) {

		// Check to see if we have a saved game
		return !_host.has_saved_game();
	} else if (component == "castle_menu") {
		if (_host._game != nullptr) {

			// Check for Party Members
			switch (selection) {
			case 1: // Inn
				[[fallthrough]];
			case 2: // Shop
				[[fallthrough]];
			case 3: // Temple
				return !_host._game->state->party_has_members();
				break;
			default:
				return false;
				break;
			};
		}
	} else if (component == "edge_menu") {
		if (_host._game != nullptr) {

			// Check for Party Members
			switch (selection) {
			case 1: // Enter Maze
				return !_host._game->state->party_has_members();
				break;
			case 2: // Restart
				return _host._game->state->party_has_members();
				break;
			default:
				return false;
			};
		}
	} else if (component == "tavern_menu") {
		if (_host._game != nullptr) {

			// Check for Party Members
			switch (selection) {
			case 0: // Add to Party
				return _host._game->state->get_party_size() == 6;
				break;
			case 1: // Remove from Party
				[[fallthrough]];
			case 2: // Reorder Party
				[[fallthrough]];
			case 3: // Divvy Gold
				return !_host._game->state->party_has_members();
				break;
			default:
				return false;
			};
		}
	} else if (component == "add_menu") {
		if (_host._game != nullptr) {

			if (data == -1)
				return false;

			if (_host._game->state->get_party_size() == 6)
				return true;

			// Check for Alignment
			const auto party_align{_host._game->get_party_alignment()};
			const auto &candidate{_host._game->characters[data]};
			if (candidate.get_alignment() == Enums::Character::Align::NEUTRAL)
				return false;
			else if (party_align != Enums::Character::Align::NEUTRAL)
				return candidate.get_alignment() != party_align;
			else
				return false;
		}
	} else if (component == "give_menu") {

		if (_host._game != nullptr) {

			const auto current_char_id{
				_host._characters[Enums::CharacterSlot::INSPECT]};

			if (current_char_id == data)
				return true;

			const auto slots_free{
				_host._game->characters[data].inventory.get_empty_slots()};
			return slots_free == 0;
		}
	} else if (component == "rest_menu") {

		if (_host._game != nullptr) {

			// Work out what menu items are disabled due to lack of money
			const auto character{_host._game->characters.at(
				_host._characters[Enums::CharacterSlot::STAY])};
			const auto gold{character.get_gold()};
			switch (selection) {
			case 0:
				// The Stables
				return false;
			case 1:
				// A Cot
				return gold < 10;
			case 2:
				// Economy Rooms
				return gold < 50;
			case 3:
				// Merchant Suites
				return gold < 200;
			case 4:
				// The Royal Suite
				return gold < 500;
			default:
				return false;
			};

			// And although we will never reach here really, onlu OK
			// characters can be selected
			return character.get_status() != Enums::Character::Status::OK;
		}
	} else if (component == "temple_pay_menu") {
		if (_host._game != nullptr) {

			if (data == -1)
				return false;

			const auto &help{_host._game->characters.at(
				_host._characters[Enums::CharacterSlot::HELP])};
			const auto &who{_host._game->characters[data]};
			return help.get_cure_cost() > who.get_gold();
		}
	} else if (component == "identify_menu") {

		if (_host.has_character(Enums::CharacterSlot::INSPECT)) {

			const auto &who{_host._game->characters.at(
				_host._characters[Enums::CharacterSlot::INSPECT])};
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
			if (selection < who.inventory.items().size()) {
				const auto item{who.inventory.items().at(selection)};
				return item.get_known();
			} else
				return false;
#pragma GCC diagnostic pop
		} else
			return false;
	} else if (component == "shop_identify_menu") {

		if (_host.has_character(Enums::CharacterSlot::STORE)) {
			const auto &who{_host._game->characters.at(
				_host._characters[Enums::CharacterSlot::STORE])};
			const auto gold{who.get_gold()};

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
			if (selection < who.inventory.items().size()) {

				// Data is not the item type here - its the slot so we can't
				// use it to work out what is disabled or not!
				const auto item{who.inventory.items().at(selection)};
				const auto item_type{
					_ctx.resources->items->get_item_type(item.get_type_id())};
				if (gold < item_type.get_value())
					return true;
				else if (item.get_known())
					return true;
				else
					return false;
#pragma GCC diagnostic pop
			} else
				return false;
		}

	} else if (component == "shop_uncurse_menu") {

		if (_host.has_character(Enums::CharacterSlot::STORE)) {
			const auto &who{_host._game->characters.at(
				_host._characters[Enums::CharacterSlot::STORE])};
			const auto gold{who.get_gold()};

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
			if (selection < who.inventory.items().size()) {

				// Data is not the item type here - its the slot so we can't
				// use it to work out what is disabled or not!
				const auto item{who.inventory.items().at(selection)};
				const auto item_type{
					_ctx.resources->items->get_item_type(item.get_type_id())};
				if (gold < item_type.get_value())
					return true;
				else if (!(item.get_cursed() && item.get_equipped()))
					return true;
				else
					return false;
#pragma GCC diagnostic pop
			} else
				return false;
		}

	} else if (component == "equip_menu") {

		// Remember this is returning true if the item is meant to be
		// disabled!
		if (_host.has_character(Enums::CharacterSlot::INSPECT)) {

			const auto &who{_host._game->characters.at(
				_host._characters[Enums::CharacterSlot::INSPECT])};
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
			if (selection < who.inventory.items().size()) {
				const auto item{who.inventory.items().at(selection)};
				return !(!item.get_equipped() && item.get_usable());
			} else
				return false;
#pragma GCC diagnostic pop
		} else
			return false;
	} else if (component == "remove_item_menu") {

		// Remember this is returning true if the item is meant to be
		// disabled!
		if (_host.has_character(Enums::CharacterSlot::INSPECT)) {

			const auto &who{_host._game->characters.at(
				_host._characters[Enums::CharacterSlot::INSPECT])};
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
			if (selection < who.inventory.items().size()) {
				const auto item{who.inventory.items().at(selection)};
				return !(item.get_equipped() && !item.get_cursed());
			} else
				return false;
#pragma GCC diagnostic pop
		} else
			return false;
	} else if (component == "sell_menu") {

		if (_host.has_character(Enums::CharacterSlot::STORE)) {

			const auto &who{_host._game->characters.at(
				_host._characters[Enums::CharacterSlot::STORE])};
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
			if (selection < who.inventory.items().size()) {
				const auto item{who.inventory.items().at(selection)};
				return item.get_equipped() && item.get_cursed();
			} else
				return false;
#pragma GCC diagnostic pop
		} else
			return false;
	} else if (component == "drop_menu") {

		if (_host.has_character(Enums::CharacterSlot::INSPECT)) {

			const auto &who{_host._game->characters.at(
				_host._characters[Enums::CharacterSlot::INSPECT])};
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
			if (selection < who.inventory.items().size()) {
				const auto item{who.inventory.items().at(selection)};
				return item.get_equipped();
			} else
				return false;
#pragma GCC diagnostic pop
		} else
			return false;
	} else if (component == "trade_menu") {

		if (_host.has_character(Enums::CharacterSlot::INSPECT)) {

			const auto &who{_host._game->characters.at(
				_host._characters[Enums::CharacterSlot::INSPECT])};
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
			if (selection < who.inventory.items().size()) {
				const auto item{who.inventory.items().at(selection)};
				return item.get_equipped();
			} else
				return false;
#pragma GCC diagnostic pop
		} else
			return false;
	} else if (component == "use_menu") {

		if (_host.has_character(Enums::CharacterSlot::INSPECT)) {

			const auto &who{_host._game->characters.at(
				_host._characters[Enums::CharacterSlot::INSPECT])};
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
			if (selection < who.inventory.items().size()) {
				const auto item{who.inventory.items().at(selection)};
				const auto item_type{
					_ctx.resources->items->get_item_type(item.get_type_id())};
				return !(item_type.has_usable() && item.get_known());
			} else
				return false;
#pragma GCC diagnostic pop
		} else
			return false;
	} else if (component == "invoke_menu") {

		if (_host.has_character(Enums::CharacterSlot::INSPECT)) {

			const auto &who{_host._game->characters.at(
				_host._characters[Enums::CharacterSlot::INSPECT])};
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
			if (selection < who.inventory.items().size()) {
				const auto item{who.inventory.items().at(selection)};
				const auto item_type{
					_ctx.resources->items->get_item_type(item.get_type_id())};
				return !(item_type.has_invokable() && item.get_known());
			} else
				return false;
#pragma GCC diagnostic pop
		} else
			return false;
	} else if (component == "spell_menu") {

		if (_host.has_character(Enums::CharacterSlot::INSPECT)) {

			// Work out from the Spell ID if we have enough sp to cast it
			const auto &who{_host._game->characters.at(
				_host._characters[Enums::CharacterSlot::INSPECT])};
			const auto spell_id{enum_cast<Enums::Magic::SpellID>(data)};

			if (!spell_id)
				return false;

			const auto spell_it{std::ranges::find(who.magic().get_spells(),
												  *spell_id, &Spell::id)};

			if (spell_it == who.magic().get_spells().end())
				return false;

			const auto &spell{*spell_it};
			const std::map<unsigned int, unsigned int> *spell_points{};

			switch (spell.type) {
			case Enums::Magic::SpellType::ARCANE:
				spell_points = &who.magic().mage_current_spellpoints();
				break;
			case Enums::Magic::SpellType::DIVINE:
				spell_points = &who.magic().priest_current_spellpoints();
				break;
			default:
				return false;
			}

			const auto points_it{spell_points->find(spell.level)};

			return !(points_it != spell_points->end() && points_it->second > 0);

		} else
			return false;
	} else if (component == "class_menu") {

		const auto classes{
			_host._game->creation_candidate->create().get_possible_classes()};
		if (selection >= 0 && selection < 8) {
			return !classes.at(
				enum_cast<Enums::Character::Class>(selection + 1).value());
		} else
			return false;
	} else if (component == "buy_menu") {

		if (_host.has_character(Enums::CharacterSlot::STORE)) {
			const auto &who{_host._game->characters.at(
				_host._characters[Enums::CharacterSlot::STORE])};
			const auto gold{who.get_gold()};

			// Data is the item type
			const auto item_type{_ctx.resources->items->get_item_type(
				enum_cast<Enums::Items::TypeID>(data).value())};
			if (gold < item_type.get_value())
				return true;
		};
	} else if (component == "retrain_menu") {

		if (_host._game == nullptr || data == -1)
			return false;

		auto &character{_host._game->characters.at(data)};
		character.create().set_possible_classes();
		return !character.create().can_change_class();

	} else if (component == "legate_menu") {

		if (_host._game == nullptr || data == -1)
			return false;

		auto &character{_host._game->characters.at(data)};

		return !(character.get_status() == Enums::Character::Status::LOST);

	} else if (component == "chest_calfo_menu") {

		if (_host._game == nullptr || data == -1)
			return false;

		const auto &character{_host._game->characters.at(data)};
		return character.magic().get_calfo_uses_left() == 0;

	} else if (component == "store_menu") {

		// No gold, can't buy anything
		// No items, can't sell anything
		// no cursed items, can't uncurse anything
		// no unidentified items, can't identify anything
		if (_host.has_character(Enums::CharacterSlot::STORE)) {
			const auto &who{_host._game->characters.at(
				_host._characters[Enums::CharacterSlot::STORE])};
			switch (selection) {
			case 0: // Buy
				return who.get_gold() == 0 || who.inventory.is_full();
				break;
			case 1: // Sell
				return who.inventory.items().empty();
				break;
			case 2: // Uncurse
				return !who.inventory.has_cursed_items();
				break;
			case 3: // Identify
				return !who.inventory.has_unidentified_items();
				break;
			default:
				return false;
			};
		} else
			return false;
	};

	return false;
}

auto Sorcery::ControllerMenuHandler::_execute(const MenuAction &action,
											  int data) -> void {

	using enum Enums::MenuAction::Type;
	using enum Enums::MenuAction::Function;

	switch (action.type) {
	case SETFLAG:
		_host._flags[action.flag] = true;
		break;

	case CLEARFLAG:
		_host._flags[action.flag] = false;
		break;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtype-limits"

	case SET_UI_BOOL:
		// if (action.ui_index >= 0 &&
		//	static_cast<size_t>(action.ui_index) < flags.size())
		//	flags[action.ui_index].get() = true;
		break;

	case CLEAR_UI_BOOL:
		// if (action.ui_index >= 0 &&
		//	static_cast<size_t>(action.ui_index) < flags.size())
		//	flags[action.ui_index].get() = false;
		break;

#pragma GCC diagnostic pop

	case SET_CHARACTER:
		_host.set_character(action.character_key, data);
		break;

	case CLEAR_CHARACTER:
		_host.clear_character(action.character_key);
		break;
	case GO_BACK:
		_host.request_back();
		break;
	case CUSTOM:
		// Handle custom actions here if needed
		if (action.custom_function == POOL_GOLD)
			_host._game->pool_party_gold(
				_host.get_character(Enums::CharacterSlot::STORE));
		break;
	case SET_SELECTED:
		_host.set_selected(action.selected_key, action.selected_value);
		break;
	case GOTOSCREEN:
		_host.go_to(action.screen);
		break;
	case OPEN_DIALOG:
		_ctx.ui->popup_manager->open_dialog(action.popup_component,
											action.dialog_type);
		break;
	case CLOSE_POPUP:
		_ctx.ui->popup_manager->close();
		break;
	default:
		break;
	}
}
