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
			_host.go_back = true;
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
	} else if (component == "temple_menu") {

		// Temple
		if (selection == (static_cast<int>(items.size()) - 1))
			_host.go_to(Enums::Screen::CASTLE);
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
	int selection, std::vector<std::reference_wrapper<bool>> &flags) -> bool {}

auto Sorcery::ControllerMenuHandler::handle_actions(
	std::string_view menu, int selection, int data,
	std::vector<std::reference_wrapper<bool>> &flags) -> bool {}

auto Sorcery::ControllerMenuHandler::item_disabled(std::string_view component,
												   int selection, int data)
	-> bool {}

auto Sorcery::ControllerMenuHandler::_execute(
	const MenuAction &action, int data,
	std::vector<std::reference_wrapper<bool>> &flags) -> void {}

auto Sorcery::ControllerMenuHandler::_execute_custom(
	Enums::MenuAction::Function function, int data) -> void {}