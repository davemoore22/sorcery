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

#include "display/ui/screenrenderer.hpp"	 // for ScreenRenderer
#include "common/enum.hpp"					 // for Attribute, Attribute::LUCK
#include "core/context.hpp"					 // for Context
#include "core/controller/actionhandler.hpp" // for ControllerActionHandler
#include "core/controller/controller.hpp"	 // for Controller
#include "core/define.hpp"	   // for WINDOW_LAYER_MENUS, WINDOW_...
#include "core/enum.hpp"	   // for Screen, CharacterSlot
#include "display/display.hpp" // for Display, DisplayMetrics
#include "display/render.hpp"  // for Render
#include "display/ui/popupmanager.hpp"
#include "display/ui/ui.hpp"			 // for UI
#include "display/ui/uimetrics.hpp"		 // for UIMetrics
#include "display/ui/uistyle.hpp"		 // for set_text_dim
#include "drawables/define.hpp"			 // for RECOVERY_BIRTHDAY, CHOOSE_M...
#include "drawables/dialog.hpp"			 // for Dialog
#include "engine/define.hpp"			 // for CHEST_GFX_ID
#include "imgui.h"						 // for ImVec2, ImGuiWindowFlags_
#include "resources/componentstore.hpp"	 // for ComponentStore
#include "resources/define.hpp"			 // for EVENTS_TEXTURE
#include "resources/fontstore.hpp"		 // for FontStore
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

namespace Sorcery {
namespace Enums {
	namespace Chests { enum class State; }
}
}

Sorcery::ScreenRenderer::ScreenRenderer(UI &ui, Context &ctx)
	: _ui{ui},
	  _ctx{ctx} {

	// Initialise function tables for display methods
	_draw_modules = {
		{Enums::Screen::ATLAS, &ScreenRenderer::_display_atlas},
		{Enums::Screen::BESTIARY, &ScreenRenderer::_display_bestiary},
		{Enums::Screen::COMPENDIUM, &ScreenRenderer::_display_compendium},
		{Enums::Screen::MAINMENU, &ScreenRenderer::_display_main_menu},
		{Enums::Screen::MUSEUM, &ScreenRenderer::_display_museum},
		{Enums::Screen::OPTIONS, &ScreenRenderer::_display_options},
		{Enums::Screen::SPELLBOOK, &ScreenRenderer::_display_spellbook},
		{Enums::Screen::SPLASH, &ScreenRenderer::_display_splash},

		{Enums::Screen::ADD, &ScreenRenderer::_display_add},
		{Enums::Screen::BUY, &ScreenRenderer::_display_buy},
		{Enums::Screen::CASTLE, &ScreenRenderer::_display_castle},
		{Enums::Screen::EDGEOFTOWN, &ScreenRenderer::_display_edge_of_town},
		{Enums::Screen::EDIT, &ScreenRenderer::_display_edit},
		{Enums::Screen::RENAME, &ScreenRenderer::_display_rename},
		{Enums::Screen::RECLASS, &ScreenRenderer::_display_reclass},
		{Enums::Screen::AUTOMAP, &ScreenRenderer::_display_automap},
		{Enums::Screen::DELETE, &ScreenRenderer::_display_delete},
		{Enums::Screen::GRAVEYARD, &ScreenRenderer::_display_graveyard},
		{Enums::Screen::VICTORY, &ScreenRenderer::_display_victory},
		{Enums::Screen::IDENTIFY, &ScreenRenderer::_display_identify},
		{Enums::Screen::INN, &ScreenRenderer::_display_inn},
		{Enums::Screen::LEGATE, &ScreenRenderer::_display_legate},
		{Enums::Screen::PAY, &ScreenRenderer::_display_pay},
		{Enums::Screen::REMOVE, &ScreenRenderer::_display_remove},
		{Enums::Screen::RESTART, &ScreenRenderer::_display_restart},
		{Enums::Screen::RETRAIN, &ScreenRenderer::_display_retrain},
		{Enums::Screen::ROSTER, &ScreenRenderer::_display_roster},
		{Enums::Screen::SELECT, &ScreenRenderer::_display_select},
		{Enums::Screen::SELL, &ScreenRenderer::_display_sell},
		{Enums::Screen::SHOP, &ScreenRenderer::_display_shop},
		{Enums::Screen::STAY, &ScreenRenderer::_display_stay},
		{Enums::Screen::STORE, &ScreenRenderer::_display_store},
		{Enums::Screen::TAVERN, &ScreenRenderer::_display_tavern},
		{Enums::Screen::TEMPLE, &ScreenRenderer::_display_temple},
		{Enums::Screen::TRAINING, &ScreenRenderer::_display_training_grounds},
		{Enums::Screen::UNCURSE, &ScreenRenderer::_display_uncurse},
	};

	_draw_modules_with_int = {
		{Enums::Screen::CREATE_NAME, &ScreenRenderer::_display_create_name},
		{Enums::Screen::CREATE_RACE, &ScreenRenderer::_display_create_race},
		{Enums::Screen::CREATE_ALIGNMENT,
		 &ScreenRenderer::_display_create_alignment},
		{Enums::Screen::CREATE_CONFIRM,
		 &ScreenRenderer::_display_create_confirm},
		{Enums::Screen::CREATE_CLASS, &ScreenRenderer::_display_create_class},
		{Enums::Screen::CHEST, &ScreenRenderer::_display_chest},
		{Enums::Screen::HEAL, &ScreenRenderer::_display_heal},
		{Enums::Screen::RITE, &ScreenRenderer::_display_rite},
		{Enums::Screen::INSPECT, &ScreenRenderer::_display_inspect},
		{Enums::Screen::LEVELUP, &ScreenRenderer::_display_level_up},
		{Enums::Screen::NOLEVELUP, &ScreenRenderer::_display_no_level_up},
		{Enums::Screen::RECOVERY, &ScreenRenderer::_display_recovery},
		{Enums::Screen::REORDER, &ScreenRenderer::_display_reorder},
	};

	_draw_modules_with_string = {
		{Enums::Screen::LICENSE, &ScreenRenderer::_display_license},
	};
}

auto Sorcery::ScreenRenderer::display(const Enums::Screen screen,
									  const std::any &payload) -> void {

	if (const auto it{_draw_modules.find(screen)}; it != _draw_modules.end()) {

		std::invoke(it->second, this);

	} else if (const auto it{_draw_modules_with_int.find(screen)};
			   it != _draw_modules_with_int.end()) {

		std::invoke(it->second, this, std::any_cast<int>(payload));

	} else if (const auto it{_draw_modules_with_string.find(screen)};
			   it != _draw_modules_with_string.end()) {

		std::invoke(it->second, this,
					std::any_cast<const std::string &>(payload));
	}
}

auto Sorcery::ScreenRenderer::_display_main_menu() -> void {

	_ui.draw_components("main_menu");
	_ui.draw_attract_mode();
	_ui.draw_bg_video();

	_ui.popup_manager->display();

	// bool show = true;
	// ImGui::PushFont(fontstore->get_default_font());
	// ImGui::ShowDemoWindow(&show);
	// ImGui::PopFont();

	// ImGui::SetNextWindowPos(ImVec2{1, 1});
	// ImGui::SetNextWindowSize(ImVec2{1000, 1000});
}

auto Sorcery::ScreenRenderer::_display_bestiary() -> void {

	_ui.draw_components("bestiary");
	_ui.draw_bg_video();
	_ui.draw_monster_info();
}

auto Sorcery::ScreenRenderer::_display_compendium() -> void {

	_ui.draw_components("compendium");
	_ui.draw_bg_video();
}

auto Sorcery::ScreenRenderer::_display_edge_of_town() -> void {

	_ui.draw_components("edge_of_town");
	_ui.popup_manager->display();
	_ui.draw_party_panel();
	_ui.draw_debug();
}

auto Sorcery::ScreenRenderer::_display_castle() -> void {

	_ui.draw_components("castle");
	_ui.popup_manager->display();
	_ui.draw_party_panel();
	_ui.draw_debug();
}

auto Sorcery::ScreenRenderer::_display_training_grounds() -> void {

	_ui.draw_components("training_grounds");
}

auto Sorcery::ScreenRenderer::_display_edit() -> void {

	_ui.draw_components("edit");
}

auto Sorcery::ScreenRenderer::_display_splash() -> void {

	// Standard Components
	_ui.draw_components("splash");

	// Cursor (if available)

	// Progress Bar
	_ui.draw_loading_progress();
}

auto Sorcery::ScreenRenderer::_display_reclass() -> void {

	_ui.draw_components("change_class");
	_draw_reclass();
	_ui.popup_manager->display();
}

auto Sorcery::ScreenRenderer::_display_rename() -> void {

	_ui.draw_components("rename");
	_draw_rename();
	_ui.popup_manager->display();
}

auto Sorcery::ScreenRenderer::_display_roster() -> void {

	_ui.draw_components("roster");
}

auto Sorcery::ScreenRenderer::_display_select() -> void {

	_ui.draw_components("select");
}

auto Sorcery::ScreenRenderer::_display_retrain() -> void {

	_ui.draw_components("retrain");
}

auto Sorcery::ScreenRenderer::_display_delete() -> void {

	_ui.draw_components("delete");
	_ui.popup_manager->display();
}

auto Sorcery::ScreenRenderer::_display_legate() -> void {

	_ui.draw_components("legate");
}

auto Sorcery::ScreenRenderer::_display_reorder(const int mode) -> void {

	_ui.draw_components("reorder", mode);
}

auto Sorcery::ScreenRenderer::_display_choose(const int mode) -> void {

	_ui.draw_components("choose", mode);
	_draw_choose(mode);
}

auto Sorcery::ScreenRenderer::_display_inspect(const int mode) -> void {

	_ui.draw_components("inspect", mode);
	if (mode & INSPECT_MODE_ACTIONS)
		_ui.draw_components("inspect_actions", mode);
	_ui.draw_current_character(mode);

	_ui.popup_manager->display();
	_ui.draw_debug();
}

auto Sorcery::ScreenRenderer::_display_spellbook() -> void {

	_ui.draw_components("spellbook");
	_ui.draw_bg_video();
	_ui.draw_spell_info();
}

auto Sorcery::ScreenRenderer::_display_graveyard() -> void {

	_ui.draw_components("graveyard");
	_ui.draw_party_wipe();
}

auto Sorcery::ScreenRenderer::_display_victory() -> void {

	_ui.draw_components("victory");
}

auto Sorcery::ScreenRenderer::_display_automap() -> void {

	_ui.draw_components("automap");
	_ui.draw_current_level_map();
	auto legend{_ui.components->get("automap:automap_legend")};
	_ui.draw_automap_legend(&legend);
}

auto Sorcery::ScreenRenderer::_display_atlas() -> void {

	_ui.draw_components("atlas");
	_ui.draw_bg_video();
	_ui.draw_level_no_player();
}

auto Sorcery::ScreenRenderer::_display_museum() -> void {

	_ui.draw_components("museum");
	_ui.draw_bg_video();
	_ui.draw_item_info();
}

auto Sorcery::ScreenRenderer::_display_inn() -> void {

	_ui.draw_components("inn");
	_ui.draw_party_panel();
	_ui.popup_manager->display();
	_ui.draw_debug();
}

auto Sorcery::ScreenRenderer::_display_shop() -> void {

	_ui.draw_components("shop");
	_ui.draw_party_panel();
	_ui.draw_debug();
}

auto Sorcery::ScreenRenderer::_display_stay() -> void {

	_ui.draw_components("stay");
	_draw_stay();
	_ui.draw_party_panel();
	_ui.draw_debug();
}

auto Sorcery::ScreenRenderer::_display_buy() -> void {

	_ui.draw_components("buy");
	_draw_buy();
	_ui.draw_party_panel();
	_ui.draw_debug();
}

auto Sorcery::ScreenRenderer::_display_sell() -> void {

	_ui.draw_components("sell");
	_draw_sell();
	_ui.draw_party_panel();
	_ui.draw_debug();
}

auto Sorcery::ScreenRenderer::_display_identify() -> void {
	_ui.draw_components("identify");
	_draw_identify();
	_ui.draw_party_panel();
	_ui.draw_debug();
}

auto Sorcery::ScreenRenderer::_display_uncurse() -> void {

	_ui.draw_components("uncurse");
	_draw_uncurse();
	_ui.draw_party_panel();
	_ui.draw_debug();
}

auto Sorcery::ScreenRenderer::_display_store() -> void {

	_ui.draw_components("store");
	_draw_store();
	_ui.draw_party_panel();
	_ui.popup_manager->display();
	_ui.draw_debug();
}

auto Sorcery::ScreenRenderer::_display_chest(const int stage) -> void {

	_ui.draw_components("chest");
	_draw_chest(enum_cast<Enums::Chests::State>(stage).value());
	_ui.popup_manager->display();
	_ui.draw_transient();
}

auto Sorcery::ScreenRenderer::_display_recovery(const int mode) -> void {

	_ui.draw_components("recovery");
	_draw_recovery(mode);
	_ui.draw_party_panel();
	_ui.draw_debug();
}

auto Sorcery::ScreenRenderer::_display_heal(int stage) -> void {

	_ui.draw_components("heal");
	_draw_heal(stage);
	_ui.draw_party_panel();
	_ui.draw_debug();
}

auto Sorcery::ScreenRenderer::_display_rite(int stage) -> void {

	_ui.draw_components("rite");
	_ui.popup_manager->display();
	_draw_rite(stage);
}

auto Sorcery::ScreenRenderer::_display_no_level_up(const int mode) -> void {

	_ui.draw_components("nolevelup");
	_draw_no_level_up(mode);
	_ui.draw_party_panel();
	_ui.draw_debug();
}

auto Sorcery::ScreenRenderer::_display_level_up(const int mode) -> void {

	_ui.draw_components("levelup");
	_draw_level_up(mode);
	_ui.draw_party_panel();
	_ui.draw_debug();
}

auto Sorcery::ScreenRenderer::_display_tavern() -> void {

	_ui.draw_components("tavern");
	_ui.popup_manager->display();
	_ui.draw_party_panel();
	_ui.draw_debug();
}

auto Sorcery::ScreenRenderer::_display_pay() -> void {

	_ui.draw_components("pay");
	_ui.draw_pay_info();
	_ui.draw_party_panel();
	_ui.draw_debug();
}

auto Sorcery::ScreenRenderer::_display_temple() -> void {

	_ui.draw_components("temple");
	_ui.draw_party_panel();
	_ui.draw_debug();
}

auto Sorcery::ScreenRenderer::_display_remove() -> void {

	_ui.draw_components("remove");
}

auto Sorcery::ScreenRenderer::_display_restart() -> void {

	_ui.draw_components("restart");
}

auto Sorcery::ScreenRenderer::_display_add() -> void {

	_ui.draw_components("add");
}

auto Sorcery::ScreenRenderer::_display_options() -> void {

	_ui.draw_components("options");
	_ui.draw_bg_video();
	_ui.draw_options();
}

auto Sorcery::ScreenRenderer::_display_license(const std::string &string)
	-> void {

	_ui.draw_components("license");
	_ui.draw_bg_video();

	auto component{_ui.components->get("license:license_info")};
	_draw_license(&component, string);
}

auto Sorcery::ScreenRenderer::_display_create_alignment(const int mode)
	-> void {

	_ui.draw_components("create_alignment", mode);
	_draw_create_alignment(mode);
}

auto Sorcery::ScreenRenderer::_display_create_confirm(const int mode) -> void {

	_ui.draw_components("create_confirm", mode);
	_draw_create_confirm(mode);
}

auto Sorcery::ScreenRenderer::_display_create_class(const int mode) -> void {

	_ui.draw_components("create_class", mode);
	_draw_create_class(mode);
}

auto Sorcery::ScreenRenderer::_display_create_race(const int mode) -> void {

	_ui.draw_components("create_race", mode);
	_draw_create_race(mode);
}

auto Sorcery::ScreenRenderer::_display_create_name(const int mode) -> void {

	_ui.draw_components("create_name", mode);
	_draw_create_name(mode);
}

////////////////////////////////////////////////////////////////////////////////

auto Sorcery::ScreenRenderer::_draw_reclass() -> void {

	auto cmp_summary{_ui.components->get("change_class:summary_text")};
	auto character{_ctx.game->characters.at(
		_ctx.controller->get_character(Enums::CharacterSlot::EDIT))};
	auto summary_text{character.summary_text()};
	_ui.draw_text(&cmp_summary, summary_text);
}

auto Sorcery::ScreenRenderer::_draw_stay() -> void {

	const auto character{_ctx.game->characters.at(
		_ctx.controller->get_character(Enums::CharacterSlot::STAY))};

	auto cmp_welcome{_ui.components->get("stay:stay_welcome")};
	auto welcome_text{std::format("{}{}{}", _ctx.get_string("STAY_WELCOME_P"),
								  character.get_name(),
								  _ctx.get_string("STAY_WELCOME_S"))};
	_ui.draw_text(&cmp_welcome, welcome_text);

	auto cmp_gold{_ui.components->get("stay:stay_gold")};
	auto gold_text{std::format("{}{}{}", _ctx.get_string("STAY_GOLD_P"),
							   character.get_gold(),
							   _ctx.get_string("STAY_GOLD_S"))};
	_ui.draw_text(&cmp_gold, gold_text);
}

auto Sorcery::ScreenRenderer::_draw_buy() -> void {

	const auto character{_ctx.game->characters.at(
		_ctx.controller->get_character(Enums::CharacterSlot::STORE))};

	auto cmp_welcome{_ui.components->get("buy:buy_welcome")};
	auto welcome_text{std::format("{}{}{}", _ctx.get_string("BUY_WELCOME_P"),
								  character.get_name(),
								  _ctx.get_string("BUY_WELCOME_S"))};
	_ui.draw_text(&cmp_welcome, welcome_text);

	auto cmp_gold{_ui.components->get("buy:buy_gold")};
	auto gold_text{std::format("{}{}{}", _ctx.get_string("BUY_GOLD_P"),
							   character.get_gold(),
							   _ctx.get_string("BUY_GOLD_S"))};
	_ui.draw_text(&cmp_gold, gold_text);
}

auto Sorcery::ScreenRenderer::_draw_sell() -> void {

	const auto character{_ctx.game->characters.at(
		_ctx.controller->get_character(Enums::CharacterSlot::STORE))};

	auto cmp_welcome{_ui.components->get("sell:sell_welcome")};
	auto welcome_text{std::format("{}{}{}", _ctx.get_string("SELL_WELCOME_P"),
								  character.get_name(),
								  _ctx.get_string("SELL_WELCOME_S"))};
	_ui.draw_text(&cmp_welcome, welcome_text);

	auto cmp_gold{_ui.components->get("sell:sell_gold")};
	auto gold_text{std::format("{}{}{}", _ctx.get_string("SELL_GOLD_P"),
							   character.get_gold(),
							   _ctx.get_string("SELL_GOLD_S"))};
	_ui.draw_text(&cmp_gold, gold_text);
}

auto Sorcery::ScreenRenderer::_draw_identify() -> void {

	const auto character{_ctx.game->characters.at(
		_ctx.controller->get_character(Enums::CharacterSlot::STORE))};

	auto cmp_welcome{_ui.components->get("identify:identify_welcome")};
	auto welcome_text{std::format(
		"{}{}{}", _ctx.get_string("IDENTIFY_WELCOME_P"), character.get_name(),
		_ctx.get_string("IDENTIFY_WELCOME_S"))};
	_ui.draw_text(&cmp_welcome, welcome_text);

	auto cmp_gold{_ui.components->get("identify:identify_gold")};
	auto gold_text{std::format("{}{}{}", _ctx.get_string("IDENTIFY_GOLD_P"),
							   character.get_gold(),
							   _ctx.get_string("IDENTIFY_GOLD_S"))};
	_ui.draw_text(&cmp_gold, gold_text);

	// And regenerate sell menu
	//_draw_menu
}

auto Sorcery::ScreenRenderer::_draw_uncurse() -> void {

	const auto character{_ctx.game->characters.at(
		_ctx.controller->get_character(Enums::CharacterSlot::STORE))};

	auto cmp_welcome{_ui.components->get("uncurse:uncurse_welcome")};
	auto welcome_text{std::format(
		"{}{}{}", _ctx.get_string("UNCURSE_WELCOME_P"), character.get_name(),
		_ctx.get_string("UNCURSE_WELCOME_S"))};
	_ui.draw_text(&cmp_welcome, welcome_text);

	auto cmp_gold{_ui.components->get("uncurse:uncurse_gold")};
	auto gold_text{std::format("{}{}{}", _ctx.get_string("UNCURSE_GOLD_P"),
							   character.get_gold(),
							   _ctx.get_string("UNCURSE_GOLD_S"))};
	_ui.draw_text(&cmp_gold, gold_text);

	// And regenerate sell menu
	//_draw_menu
}

auto Sorcery::ScreenRenderer::_draw_store() -> void {

	const auto character{_ctx.game->characters.at(
		_ctx.controller->get_character(Enums::CharacterSlot::STORE))};

	auto cmp_welcome{_ui.components->get("store:store_welcome")};
	auto welcome_text{std::format("{}{}{}", _ctx.get_string("STORE_WELCOME_P"),
								  character.get_name(),
								  _ctx.get_string("STORE_WELCOME_S"))};
	_ui.draw_text(&cmp_welcome, welcome_text);

	auto cmp_gold{_ui.components->get("store:store_gold")};
	auto gold_text{std::format("{}{}{}", _ctx.get_string("STORE_GOLD_P"),
							   character.get_gold(),
							   _ctx.get_string("STORE_GOLD_S"))};
	_ui.draw_text(&cmp_gold, gold_text);
}

auto Sorcery::ScreenRenderer::_draw_rename() -> void {

	auto cmp_summary{_ui.components->get("rename:summary_text")};

	const auto &character{_ctx.game->characters.at(
		_ctx.controller->get_character(Enums::CharacterSlot::EDIT))};

	const auto summary_text{character.summary_text()};

	_ui.draw_text(&cmp_summary, summary_text);

	// As next custom component is a text box, focus on that initially
	if (_ui.first_frame) {

		ImGui::SetKeyboardFocusHere();

		_ui.first_frame = false;
	}

	auto cmp_name{_ui.components->get("rename:rename_input")};

	auto &input{_ctx.controller->get_input_buffer()};

	if (_ui.draw_input(cmp_name, input, ImGuiInputTextFlags_None)) {

		_ctx.controller->actions->input("rename_input_ok", input);
	}
}

auto Sorcery::ScreenRenderer::_draw_chest(const Enums::Chests::State state)
	-> void {

	_ui.draw_components("engine_base_ui");

	if (!_ctx.controller->get_monochrome()) {
		auto bg_c{_ui.components->get("engine_base_ui:background_image")};
		_ui.draw_tiled_bg(&bg_c);
	}

	if (_ctx.get_flag("interface_ui") && _ctx.get_flag("interface_party_panel"))
		_ui.draw_party_panel();

	// Dungeon View
	auto component{_ui.components->get("engine_base_ui:wire_frame_view")};
	_ui.render->draw(&component);

	// Only the main chest state has a persistent menu.
	_ui.draw_components("chest_menu");

	// Chest
	const auto chest_idx{CHEST_GFX_ID};
	const auto cmp{_ui.components->get("chest:chest_image")};
	const auto scale{_ctx.display->get_display_metrics().scale};

	const auto chest_w{cmp.get_float("tile_width") * scale};
	const auto chest_h{cmp.get_float("tile_height") * scale};

	const auto x{(_ui.metrics->grid_x(cmp.x) - (chest_w / 2)) + 2};

	const auto adj_y{_ctx.get_flag("interface_ui") &&
							 _ctx.get_flag("interface_party_panel")
						 ? cmp.y
						 : cmp.y + 7};

	const auto y{_ui.metrics->grid_y(adj_y) - (chest_h / 2)};

	const auto p_min{ImVec2{x, y}};
	const auto p_max{ImVec2{x + chest_w, y + chest_h}};

	// Opaque backing behind the chest graphic.
	with_Window(WINDOW_LAYER_IMAGES, nullptr,
				ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs) {

		ImGui::GetWindowDrawList()->AddRectFilled(p_min, p_max,
												  IM_COL32(0, 0, 0, 255));
	}

	_ui.draw_fg_image_with_idx(EVENTS_TEXTURE, chest_idx, p_min,
							   ImVec2{chest_w, chest_h});

	_ui.draw_debug();
	_ui.draw_ui_status();
}

auto Sorcery::ScreenRenderer::_draw_create_alignment(
	[[maybe_unused]] const int mode) -> void {

	auto cmp_summary{_ui.components->get("create_alignment:summary_text")};
	auto summary_text{
		_ctx.controller->get_candidate_character()->summary_text()};
	_ui.draw_text(&cmp_summary, summary_text);
}

auto Sorcery::ScreenRenderer::_draw_create_confirm(
	[[maybe_unused]] const int mode) -> void {

	auto cmp_summary{_ui.components->get("create_confirm:summary_text")};
	auto summary_text{
		_ctx.controller->get_candidate_character()->summary_text()};
	_ui.draw_text(&cmp_summary, summary_text);

	auto cmp_char{_ui.components->get("create_confirm:character_data")};
	with_Window(WINDOW_LAYER_TEXTS, nullptr,
				ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs) {
		set_Font(_ui.fonts->get_current_font(cmp_char.font).value(),
				 _ui.metrics->font_sz());
		_ui.draw_character_summary(&cmp_char,
								   _ctx.controller->get_candidate_character());
	}
}

auto Sorcery::ScreenRenderer::_draw_create_class(
	[[maybe_unused]] const int mode) -> void {

	auto cmp_summary{_ui.components->get("create_class:summary_text")};
	auto summary_text{
		_ctx.controller->get_candidate_character()->summary_text()};
	_ui.draw_text(&cmp_summary, summary_text);

	auto cmp_points_left{_ui.components->get("create_class:points_left_text")};
	const auto points_left_text{
		std::format("{:>2}", _ctx.controller->get_candidate_character()
								 ->create()
								 .get_points_left())};
	_ui.draw_text(&cmp_points_left, points_left_text);

	// Now draw the class buttons
	using enum Enums::Character::Attribute;
	auto cmp_attribute{_ui.components->get("create_class:current_stats")};
	for (auto i = std::to_underlying(STRENGTH); i <= std::to_underlying(LUCK);
		 ++i) {
		auto attribute{_ctx.controller->get_candidate_character()->get_attr_ptr(
			enum_cast<Enums::Character::Attribute>(i).value())};
		auto cmp_name{std::format("stepper_attribute_{}", i)};
		_ui.draw_stepper(&cmp_attribute, cmp_name, *attribute);
		++cmp_attribute.y;
	}
}

auto Sorcery::ScreenRenderer::_draw_create_race([[maybe_unused]] const int mode)
	-> void {

	auto cmp_summary{_ui.components->get("create_race:summary_text")};
	auto summary_text{
		_ctx.controller->get_candidate_character()->summary_text()};
	_ui.draw_text(&cmp_summary, summary_text);
}

auto Sorcery::ScreenRenderer::_draw_create_name([[maybe_unused]] const int mode)
	-> void {

	auto cmp_summary{_ui.components->get("create_name:summary_text")};

	const auto summary_text{
		_ctx.controller->get_candidate_character()->summary_text()};

	_ui.draw_text(&cmp_summary, summary_text);

	// As next custom component is a text box, focus on that initially
	if (_ui.first_frame) {

		ImGui::SetKeyboardFocusHere();

		_ui.first_frame = false;
	}

	auto cmp_name{_ui.components->get("create_name:name_input")};

	auto &input{_ctx.controller->get_input_buffer()};

	if (_ui.draw_input(cmp_name, input, ImGuiInputTextFlags_None)) {

		_ctx.controller->actions->input("name_input_ok", input);
	}
}

auto Sorcery::ScreenRenderer::_draw_choose(const int mode) -> void {

	if (mode & CHOOSE_MODE_STAY) {

		auto cmp{_ui.components->get("choose:choose_stay")};
		_ui.draw_text(&cmp);
		_ui.draw_party_panel();
	}
}

auto Sorcery::ScreenRenderer::_draw_level_up(const int mode) -> void {

	auto &character{_ctx.game->characters.at(
		_ctx.controller->get_character(Enums::CharacterSlot::STAY))};

	if (mode & RECOVERY_BIRTHDAY) {

		const auto birth_text{_ctx.get_string("REST_BIRTHDAY_YOU")};
		auto cmp{_ui.components->get("levelup:levelup_birthday")};
		_ui.draw_text(&cmp, birth_text);
		cmp = _ui.components->get("levelup:levelup_results");
		for (const auto &result : character.level_up_results) {
			_ui.draw_text(&cmp, result);
			cmp.y += _ui.metrics->grid_delta(0, 1).y;
		}
	} else {

		auto cmp{_ui.components->get("levelup:levelup_birthday")};
		for (const auto &result : character.level_up_results) {
			_ui.draw_text(&cmp, result);
			++cmp.y;
		}
	}

	with_Window(WINDOW_LAYER_MENUS, nullptr, ImGuiWindowFlags_NoTitleBar) {
		auto leave{_ui.components->get("levelup:levelup_leave")};
		_ui.draw_button_click(&leave, _ctx.get_flag_ref("show_levelup"), true);
	}
}

auto Sorcery::ScreenRenderer::_draw_heal(int stage) -> void {

	auto cmp{_ui.components->get("heal:heal_status")};
	auto text{""s};

	switch (stage) {
	case 4:
		text = _ctx.get_string("TEMPLE_HEAL_1");
		break;

	case 3:
		text = _ctx.get_string("TEMPLE_HEAL_2");
		break;

	case 2:
		text = _ctx.get_string("TEMPLE_HEAL_3");
		break;

	case 1:
		text = _ctx.get_string("TEMPLE_HEAL_4");
		break;

	default:
		break;
	}

	if (!text.empty())
		_ui.draw_text(&cmp, text);

	if (_ctx.controller->has_flag("heal_finished") &&
		_ctx.controller->has_text("heal_results")) {

		auto summary{_ui.components->get("heal:heal_results")};
		const auto results{_ctx.controller->get_text("heal_results")};
		_ui.draw_text(&summary, results);
		with_Window(WINDOW_LAYER_MENUS, nullptr, ImGuiWindowFlags_NoTitleBar) {

			auto leave{_ui.components->get("heal:button_heal_return")};
			_ui.draw_button_click(&leave, _ctx.get_flag_ref("heal_return"),
								  true);
		}
	}
}

auto Sorcery::ScreenRenderer::_draw_rite(const int stage) -> void {

	auto cmp_summary{_ui.components->get("rite:summary_text")};
	auto character{_ctx.game->characters.at(
		_ctx.controller->get_character(Enums::CharacterSlot::EDIT))};
	auto summary_text{character.summary_text()};
	_ui.draw_text(&cmp_summary, summary_text);

	if (stage == 0)
		return;

	auto cmp_progress{_ui.components->get("rite:progress_text")};
	auto progress_text{_ctx.get_string("RITE_PROGRESS")};

	auto cmp{_ui.components->get("rite:rite_stage")};

	std::string text;

	switch (stage) {

	case 1:
		text = _ctx.get_string("RITE_STAGE_1");
		break;

	case 2:
		text = _ctx.get_string("RITE_STAGE_2");
		_ui.draw_text(&cmp_progress, progress_text);
		break;

	case 3:
		text = _ctx.get_string("RITE_STAGE_3");
		_ui.draw_text(&cmp_progress, progress_text);
		break;

	case 4:
		text = _ctx.get_string("RITE_STAGE_4");
		_ui.draw_text(&cmp_progress, progress_text);
		break;

	case 5:
		text = _ctx.get_string("RITE_STAGE_5");
		break;

	default:
		return;
	}

	_ui.draw_text(&cmp, text);
}

auto Sorcery::ScreenRenderer::_draw_no_level_up(const int mode) -> void {

	const auto character{_ctx.game->characters.at(
		_ctx.controller->get_character(Enums::CharacterSlot::STAY))};
	const auto birth_text{_ctx.get_string("REST_BIRTHDAY_YOU")};
	const auto needed{character.get_next_xp() - character.get_cur_xp()};
	const auto need_text{std::format("{}{}{}", _ctx.get_string("REST_NEED_1_P"),
									 needed, _ctx.get_string("REST_NEED_1_S"))};
	const auto make_text{_ctx.get_string("REST_NEED_2")};

	if (mode & RECOVERY_BIRTHDAY) {

		auto cmp{_ui.components->get("nolevelup:nolevelup_birthday")};
		_ui.draw_text(&cmp, birth_text);
		cmp = _ui.components->get("nolevelup:nolevelup_need_1");
		_ui.draw_text(&cmp, need_text);
		cmp = _ui.components->get("nolevelup:nolevelup_need_2");
		_ui.draw_text(&cmp, make_text);
	} else {

		auto cmp{_ui.components->get("nolevelup:nolevelup_birthday")};
		_ui.draw_text(&cmp, need_text);
		cmp = _ui.components->get("nolevelup:nolevelup_need_1");
		_ui.draw_text(&cmp, make_text);
	}

	with_Window(WINDOW_LAYER_MENUS, nullptr, ImGuiWindowFlags_NoTitleBar) {
		auto leave{_ui.components->get("nolevelup:nolevelup_leave")};
		_ui.draw_button_click(&leave, _ctx.get_flag_ref("show_nolevelup"),
							  true);
	}
}

auto Sorcery::ScreenRenderer::_draw_recovery(const int mode) -> void {

	const auto character{_ctx.game->characters.at(
		_ctx.controller->get_character(Enums::CharacterSlot::STAY))};
	if (mode == RECOVERY_MODE_FREE) {

		auto cmp{_ui.components->get("recovery:recovery_napping")};
		auto text{std::format("{}{}", character.get_name(),
							  _ctx.get_string("RECOVERY_NAPPING"))};
		_ui.draw_text(&cmp, text);

	} else {

		auto cmp{_ui.components->get("recovery:recovery_recuperating")};
		auto text{std::format("{} {}", character.get_name(),
							  _ctx.get_string("REST_RECUPERATING"))};
		_ui.draw_text(&cmp, text);

		cmp = _ui.components->get("recovery:recovery_recuperating_hp");
		text = std::format("{} ({:>5}/{:>5})", _ctx.get_string("REST_HP"),
						   character.get_current_hp(), character.get_max_hp());
		_ui.draw_text(&cmp, text);

		cmp = _ui.components->get("recovery:recovery_recuperating_gold");
		text = std::format("{} {:>7}", _ctx.get_string("REST_GOLD"),
						   character.get_gold());
		_ui.draw_text(&cmp, text);

		with_Window(WINDOW_LAYER_MENUS, nullptr, ImGuiWindowFlags_NoTitleBar) {
			auto stop{_ui.components->get("recovery:recovery_stop")};
			_ui.draw_button_click(&stop, _ctx.get_flag_ref("show_recovery"),
								  true);
		}
	}
}

auto Sorcery::ScreenRenderer::_draw_license(Component *component,
											const std::string &string) -> void {
	with_Window(WINDOW_LAYER_MENUS, nullptr, ImGuiWindowFlags_NoTitleBar) {

		// To adjust for Window Resizing etc
		const auto x{std::invoke([&] {
			const auto width{_ui.metrics->grid_sz() *
							 component->get_float("grid_width")};
			const auto viewport{ImGui::GetMainViewport()};
			return (viewport->Size.x - width) / 2;
		})};

		const auto pos{ImVec2{x, _ui.metrics->grid_y(component->y)}};
		ImGui::SetNextWindowPos(pos);
		with_Child("license_child",
				   ImVec2(_ui.metrics->grid_sz() * component->w,
						  _ui.metrics->grid_sz() * component->h),
				   ImGuiChildFlags_NavFlattened,
				   ImGuiWindowFlags_AlwaysVerticalScrollbar) {

			UIStyle::set_text_dim(_ctx);
			set_Font(_ui.fonts->get_current_font(component->font).value(),
					 _ui.metrics->font_sz());
			with_TextWrapPos(ImGui::GetFontSize() * component->w) {
				ImGui::TextUnformatted(string.c_str());
			}
		}

		// Special Handling for Return Button
		Component cmp{_ui.components->get("license:license_return")};
		_ui.draw_button_click(&cmp, _ctx.get_flag_ref("show_license"), true);
	}
}