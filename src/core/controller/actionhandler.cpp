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

#include "core/controller/actionhandler.hpp" // for ScreenRenderer
#include "common/enum.hpp"					 // for Attribute, Attribute::LUCK
#include "core/context.hpp"					 // for Context
#include "core/controller/controller.hpp"	 // for Controller
#include "core/controller/menuaction.hpp"	 // for Controller
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
#include "types/config.hpp"
#include "types/game.hpp"  // for Game
#include "types/meta.hpp"  // for enum_cast
#include <SDL_events.h>	   // for SDL_EventType, SDL_Event
#include <any>			   // for any_cast, any
#include <format>		   // for format
#include <functional>	   // for invoke
#include <imgui_sugar.hpp> // for BooleanGuard, with_Window
#include <map>			   // for map
#include <memory>		   // for unique_ptr
#include <optional>		   // for optional
#include <string>		   // for basic_string, string
#include <unordered_map>   // for unordered_map, operator==
#include <utility>		   // for pair, to_underlying
#include <vector>		   // for vector

Sorcery::ControllerActionHandler::ControllerActionHandler(Controller &host,
														  Context &ctx)
	: _host{host},
	  _ctx{ctx} {

	  };
auto Sorcery::ControllerActionHandler::button(const std::string_view component,
											  [[maybe_unused]] const int data)
	-> void {

	DEBUG_LOGF("Button Click: {} {}", component, data);

	if (component == "button_identify") {

		_ctx.ui->popup_manager->open_modal("global:modal_identify");

		_host.set_flag("want_identify");

	} else if (component == "button_pool") {

		_host._game->pool_party_gold(
			_host.get_character(Enums::CharacterSlot::INSPECT));

		_ctx.ui->popup_manager->open_dialog("global:notice_pool_gold",
											Enums::Layout::DialogType::OK);

	} else if (component == "button_leave") {

		_host.unset_flag("want_inspect");
		_host.request_back();

	} else if (component == "button_drop") {

		_ctx.ui->popup_manager->open_modal("global:modal_drop");

		_host.set_flag("want_drop");

	} else if (component == "button_trade") {

		// Leave this legacy for now.
		_ctx.ui->popups->modal_trade->regenerate();
		_ctx.ui->popups->modal_trade->show = true;

		_ctx.ui->popups->modal_give->regenerate();
		_ctx.ui->popups->modal_give->show = false;

		_host.set_flag("want_trade");
		_host.unset_flag("want_give");

	} else if (component == "button_use") {

		_ctx.ui->popup_manager->open_modal("global:modal_use");

		_host.set_flag("want_use");

	} else if (component == "button_equip") {

		_ctx.ui->popup_manager->open_modal("global:modal_equip");

		_host.set_flag("want_equip");

	} else if (component == "button_remove") {

		_ctx.ui->popup_manager->open_modal("global:modal_remove_item");

		_host.set_flag("want_remove");

	} else if (component == "button_spell") {

		_ctx.ui->popup_manager->open_modal("global:modal_spell");

		_host.set_flag("want_spell");

	} else if (component == "button_invoke") {

		_ctx.ui->popup_manager->open_modal("global:modal_invoke");

		_host.set_flag("want_invoke");

	} else if (component == "button_keep_yes") {

		_host.set_flag("confirm_keep_character");

	} else if (component == "button_keep_no") {

		_host.set_flag("confirm_discard_character");

	} else if (component == "button_buy_leave") {

		_host.go_to(Enums::Screen::STORE);

	} else if (component == "button_heal_return") {

		_host.go_to(Enums::Screen::TEMPLE);

	} else if (component == "license_return") {

		_host.go_to(Enums::Screen::MAINMENU);

	} else if (component == "graveyard_return") {

		_host.go_to(Enums::Screen::CASTLE);

	} else if (component == "nolevelup_leave" || component == "levelup_leave") {

		_host.go_to(Enums::Screen::INN);

	} else if (component == "automap_return") {

		_host.go_to(Enums::Screen::ENGINE);

	} else if (component == "button_victory") {

		_host.go_to(Enums::Screen::CASTLE);
	}
}
auto Sorcery::ControllerActionHandler::input(const std::string_view component,
											 std::string &data) -> void {

	DEBUG_LOGF("Input Button Click: {} {}", component, data);

	if (component == "name_input_ok") {

		if (!data.empty()) {

			_host._game->creation_candidate->create().set_name(data);

			_host._game->creation_candidate->create().set_stage(
				Enums::Character::Stage::CHOOSE_RACE);
		}

	} else if (component == "rename_input_ok") {

		if (!data.empty()) {

			auto &character{_host._game->characters.at(
				_host.get_character(Enums::CharacterSlot::EDIT))};

			character.create().set_name(data);

			_host._game->save_game();

			_ctx.ui->popup_manager->open_dialog("global:notice_renamed_ok",
												Enums::Layout::DialogType::OK);

		} else {

			_host.clear_character(Enums::CharacterSlot::EDIT);
			_host.go_to(Enums::Screen::EDIT);
		}
	}
}

auto Sorcery::ControllerActionHandler::stepper(const std::string_view component,
											   const bool positive, int &data)
	-> void {

	DEBUG_LOGF("Stepper Button Click: {} {}", component, positive);

	auto candidate{_host._game->creation_candidate};

	if (component.starts_with("##stepper_attribute_")) {

		if (positive) {

			// Up: If we have points left and the value is less than 18
			if ((candidate->create().get_points_left() > 0) && (data <= 17)) {

				data++;
				candidate->create().set_points_left(
					candidate->create().get_points_left() - 1);
				candidate->create().set_possible_classes();
			}

		} else {

			if (candidate->create().get_points_left() <
				candidate->create().get_start_points()) {

				// Down: If we are above staring points
				using enum Enums::Character::Attribute;
				if (component.starts_with("##stepper_attribute_1")) {
					if (candidate->get_cur_attr(STRENGTH) >
						candidate->create().get_start_attribute(STRENGTH)) {
						data--;
						candidate->create().set_points_left(
							candidate->create().get_points_left() + 1);
						candidate->create().set_possible_classes();
					}
				} else if (component.starts_with("##stepper_attribute_2")) {
					if (candidate->get_cur_attr(IQ) >
						candidate->create().get_start_attribute(IQ)) {
						data--;
						candidate->create().set_points_left(
							candidate->create().get_points_left() + 1);
						candidate->create().set_possible_classes();
					}
				} else if (component.starts_with("##stepper_attribute_3")) {
					if (candidate->get_cur_attr(PIETY) >
						candidate->create().get_start_attribute(PIETY)) {
						data--;
						candidate->create().set_points_left(
							candidate->create().get_points_left() + 1);
						candidate->create().set_possible_classes();
					}
				} else if (component.starts_with("##stepper_attribute_4")) {
					if (candidate->get_cur_attr(VITALITY) >
						candidate->create().get_start_attribute(VITALITY)) {
						data--;
						candidate->create().set_points_left(
							candidate->create().get_points_left() + 1);
						candidate->create().set_possible_classes();
					}
				} else if (component.starts_with("##stepper_attribute_5")) {
					if (candidate->get_cur_attr(AGILITY) >
						candidate->create().get_start_attribute(AGILITY)) {
						data--;
						candidate->create().set_points_left(
							candidate->create().get_points_left() + 1);
						candidate->create().set_possible_classes();
					}
				} else if (component.starts_with("##stepper_attribute_6")) {
					if (candidate->get_cur_attr(LUCK) >
						candidate->create().get_start_attribute(LUCK)) {
						data--;
						candidate->create().set_points_left(
							candidate->create().get_points_left() + 1);
						candidate->create().set_possible_classes();
					}
				}
			}
		}
	};
};

// Toggle Handling
auto Sorcery::ControllerActionHandler::toggle(const std::string_view component,
											  const std::string_view tab,
											  const int selection) -> void {

	DEBUG_LOGF("Toggle: {} {} {}", component, tab, selection);

	if (component == "options_info") {

		using enum Enums::Config::Options;
		if (tab == "Options") {

			// This happens after the corresponding data is changed
			if (selection == static_cast<int>(RECOMMENDED_MODE) &&
				_ctx.get_config(RECOMMENDED_MODE))
				_ctx.config->set_rec_mode();
			else if (selection == static_cast<int>(STRICT_MODE) &&
					 _ctx.get_config(STRICT_MODE))
				_ctx.config->set_strict_mode();
			else {
				if (_ctx.config->is_strict_mode())
					_ctx.config->set_strict_mode();
				else if (_ctx.config->is_rec_mode())
					_ctx.config->set_rec_mode();
			}

		} else if (tab == "Gameplay") {

			// Only need to check if strict and reommended modes are on
			_ctx.get_config(RECOMMENDED_MODE) = _ctx.config->is_rec_mode();
			_ctx.get_config(STRICT_MODE) = _ctx.config->is_strict_mode();

		} else if (tab == "Graphics") {

			// Only need to check if strict and reommended modes are on
			_ctx.get_config(RECOMMENDED_MODE) = _ctx.config->is_rec_mode();
			_ctx.get_config(STRICT_MODE) = _ctx.config->is_strict_mode();
		}
	}
}

auto Sorcery::ControllerActionHandler::icon(const int icon_idx) -> void {

	DEBUG_LOGF("Icon Click: {}", icon_idx);

	switch (icon_idx) {
	case ICON_CAMP:
		_ctx.ui->popup_manager->open_modal("engine_base_ui:modal_camp");
		break;

	case ICON_PARTY:
		_host.set_flag("want_inspect");
		break;

	case ICON_MAP:
		_host.set_flag("want_automap");
		break;

	case ICON_LOOK:
		_host.set_flag("want_look");
		break;

	case ICON_CAST:
		_host.set_flag("want_spell");
		break;

	case ICON_USE:
		_host.set_flag("want_use");
		break;

	default:
		break;
	}
}

auto Sorcery::ControllerActionHandler::inspect(const int character_id) -> void {

	_host.set_character(Enums::CharacterSlot::INSPECT, character_id);
	_host.go_to(Enums::Screen::INSPECT);
}
