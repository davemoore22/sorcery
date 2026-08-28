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

#include <algorithm>
#include <ranges>
#include <regex>
#include <vector>

#include "common/enum.hpp"
#include "common/imgui.hpp"
#include "common/macro.hpp"
#include "common/opengl.hpp"
#include "common/sdl2.hpp"
#include "common/types.hpp"
#include "core/context.hpp"
#include "core/controller.hpp"
#include "core/debug.hpp"
#include "core/enum.hpp"
#include "core/macro.hpp"
#include "core/resources.hpp"
#include "core/system.hpp"
#include "display/animation.hpp"
#include "display/display.hpp"
#include "display/render.hpp"
#include "display/ui/popupstore.hpp"
#include "display/ui/screenrenderer.hpp"
#include "display/ui/ui.hpp"
#include "drawables/dialog.hpp"
#include "drawables/frame.hpp"
#include "drawables/input.hpp"
#include "drawables/menu.hpp"
#include "drawables/menubuilder.hpp"
#include "drawables/message.hpp"
#include "drawables/modal.hpp"
#include "drawables/popup.hpp"
#include "drawables/uistyle.hpp"
#include "drawables/videoplayer.hpp"
#include "engine/define.hpp"
#include "engine/types.hpp"
#include "resources/componentstore.hpp"
#include "resources/filestore.hpp"
#include "resources/fontstore.hpp"
#include "resources/imagestore.hpp"
#include "resources/itemstore.hpp"
#include "resources/levelstore.hpp"
#include "resources/monsterstore.hpp"
#include "resources/spellstore.hpp"
#include "resources/stringstore.hpp"
#include "types/component.hpp"
#include "types/config.hpp"
#include "types/enum.hpp"
#include "types/error.hpp"
#include "types/game.hpp"
#include "types/image.hpp"
#include "types/meta.hpp"
#include "types/state.hpp"
#include "types/world/tile.hpp"

Sorcery::ScreenRenderer::ScreenRenderer(UI &ui, Context &ctx)
	: _ui{ui},
	  _ctx{ctx} {

	// Initialise function tables for display methods
	draw_modules[Enums::Screen::ATLAS] = [this] {
		_display_atlas();
	};
	draw_modules[Enums::Screen::BESTIARY] = [this] {
		_display_bestiary();
	};
	draw_modules[Enums::Screen::COMPENDIUM] = [this] {
		_display_compendium();
	};
	draw_modules[Enums::Screen::MAINMENU] = [this] {
		_display_main_menu();
	};
	draw_modules[Enums::Screen::MUSEUM] = [this] {
		_display_museum();
	};
	draw_modules[Enums::Screen::OPTIONS] = [this] {
		_display_options();
	};
	draw_modules[Enums::Screen::SPELLBOOK] = [this] {
		_display_spellbook();
	};
	draw_modules[Enums::Screen::SPLASH] = [this] {
		_display_splash();
	};

	draw_modules[Enums::Screen::ADD] = [this]() {
		_display_add();
	};
	draw_modules[Enums::Screen::BUY] = [this]() {
		_display_buy();
	};
	draw_modules[Enums::Screen::CASTLE] = [this]() {
		_display_castle();
	};
	draw_modules[Enums::Screen::EDGEOFTOWN] = [this]() {
		_display_edge_of_town();
	};
	draw_modules[Enums::Screen::EDIT] = [this]() {
		_display_edit();
	};
	draw_modules[Enums::Screen::RENAME] = [this]() {
		_display_rename();
	};
	draw_modules[Enums::Screen::RECLASS] = [this]() {
		_display_reclass();
	};
	draw_modules[Enums::Screen::AUTOMAP] = [this]() {
		_display_automap();
	};
	draw_modules[Enums::Screen::DELETE] = [this]() {
		_display_delete();
	};
	draw_modules[Enums::Screen::GRAVEYARD] = [this]() {
		_display_graveyard();
	};
	draw_modules[Enums::Screen::VICTORY] = [this]() {
		_display_victory();
	};
	draw_modules[Enums::Screen::IDENTIFY] = [this]() {
		_display_identify();
	};
	draw_modules[Enums::Screen::INN] = [this]() {
		_display_inn();
	};
	draw_modules[Enums::Screen::LEGATE] = [this]() {
		_display_legate();
	};
	draw_modules[Enums::Screen::PAY] = [this]() {
		_display_pay();
	};
	draw_modules[Enums::Screen::REMOVE] = [this]() {
		_display_remove();
	};
	draw_modules[Enums::Screen::RESTART] = [this]() {
		_display_restart();
	};
	draw_modules[Enums::Screen::RETRAIN] = [this]() {
		_display_retrain();
	};
	draw_modules[Enums::Screen::ROSTER] = [this]() {
		_display_roster();
	};
	draw_modules[Enums::Screen::SELECT] = [this]() {
		_display_select();
	};
	draw_modules[Enums::Screen::SELL] = [this]() {
		_display_sell();
	};
	draw_modules[Enums::Screen::SHOP] = [this]() {
		_display_shop();
	};
	draw_modules[Enums::Screen::STAY] = [this]() {
		_display_stay();
	};
	draw_modules[Enums::Screen::STORE] = [this]() {
		_display_store();
	};
	draw_modules[Enums::Screen::TAVERN] = [this]() {
		_display_tavern();
	};
	draw_modules[Enums::Screen::TEMPLE] = [this]() {
		_display_temple();
	};
	draw_modules[Enums::Screen::TRAINING] = [this]() {
		_display_training_grounds();
	};
	draw_modules[Enums::Screen::UNCURSE] = [this]() {
		_display_uncurse();
	};

	draw_modules_with_int[Enums::Screen::CREATE_NAME] = [this](int n) {
		_display_create_name(n);
	};

	draw_modules_with_int[Enums::Screen::CREATE_RACE] = [this](int n) {
		_display_create_race(n);
	};

	draw_modules_with_int[Enums::Screen::CREATE_ALIGNMENT] = [this](int n) {
		_display_create_alignment(n);
	};

	draw_modules_with_int[Enums::Screen::CREATE_CONFIRM] = [this](int n) {
		_display_create_confirm(n);
	};

	draw_modules_with_int[Enums::Screen::CREATE_CLASS] = [this](int n) {
		_display_create_class(n);
	};
	draw_modules_with_int[Enums::Screen::CHEST] = [this](int n) {
		_display_chest(n);
	};

	draw_modules_with_int[Enums::Screen::HEAL] = [this](int n) {
		_display_heal(n);
	};
	draw_modules_with_int[Enums::Screen::RITE] = [this](int n) {
		_display_rite(n);
	};
	draw_modules_with_int[Enums::Screen::INSPECT] = [this](int n) {
		_display_inspect(n);
	};
	draw_modules_with_int[Enums::Screen::LEVELUP] = [this](int n) {
		_display_level_up(n);
	};
	draw_modules_with_int[Enums::Screen::NOLEVELUP] = [this](int n) {
		_display_no_level_up(n);
	};
	draw_modules_with_int[Enums::Screen::RECOVERY] = [this](int n) {
		_display_recovery(n);
	};
	draw_modules_with_int[Enums::Screen::REORDER] = [this](int n) {
		_display_reorder(n);
	};

	draw_modules_with_string[Enums::Screen::LICENSE] =
		[this](const std::string &string) {
			_display_license(string);
		};
}

auto Sorcery::ScreenRenderer::_display_main_menu() -> void {

	_ui.draw_components("main_menu");
	_ui.draw_attract_mode();
	_ui.draw_bg_video();

	_ui.popups->dialog_exit->display(_ctx.get_flag_ref("want_exit_game"));
	_ui.popups->dialog_new->display(_ctx.get_flag_ref("want_new_game"));
	_ui.popups->dialog_leave->display(_ctx.controller->want_to_leave_game());

	_ui.draw_cursor();

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
	_ui.draw_cursor();
}

auto Sorcery::ScreenRenderer::_display_compendium() -> void {

	_ui.draw_components("compendium");
	_ui.draw_bg_video();
	_ui.draw_cursor();
}

auto Sorcery::ScreenRenderer::_display_edge_of_town() -> void {

	_ui.draw_components("edge_of_town");
	_ui.popups->dialog_leave->display(_ctx.controller->want_to_leave_game());
	_ui.draw_party_panel();
	_ui.draw_debug();
	_ui.draw_cursor();
}

auto Sorcery::ScreenRenderer::_display_castle() -> void {

	_ui.draw_components("castle");
	_ui.popups->dialog_leave->display(_ctx.controller->want_to_leave_game());
	_ui.draw_party_panel();
	_ui.draw_debug();
	_ui.draw_cursor();
}

auto Sorcery::ScreenRenderer::_display_training_grounds() -> void {

	_ui.draw_components("training_grounds");
	_ui.draw_cursor();
}

auto Sorcery::ScreenRenderer::_display_edit() -> void {

	_ui.draw_components("edit");
	_ui.draw_cursor();
}

auto Sorcery::ScreenRenderer::_display_splash() -> void {

	// Standard Components
	_ui.draw_components("splash");

	// Cursor (if available)
	_ui.draw_cursor();

	// Progress Bar
	_ui.draw_loading_progress();
}

auto Sorcery::ScreenRenderer::_display_reclass() -> void {

	_ui.draw_components("change_class");
	_draw_reclass();
	_ui.popups->notice_reclassed_ok->display(
		_ctx.get_flag_ref("want_reclassed_ok"));
	_ui.draw_cursor();
}

auto Sorcery::ScreenRenderer::_display_rename() -> void {

	_ui.draw_components("rename");
	_draw_rename();
	_ui.popups->notice_renamed_ok->display(
		_ctx.get_flag_ref("want_renamed_ok"));
	_ui.draw_cursor();
}

auto Sorcery::ScreenRenderer::_display_roster() -> void {

	_ui.draw_components("roster");
	_ui.draw_cursor();
}

auto Sorcery::ScreenRenderer::_display_select() -> void {

	_ui.draw_components("select");
	_ui.draw_cursor();
}

auto Sorcery::ScreenRenderer::_display_retrain() -> void {

	_ui.draw_components("retrain");
	_ui.draw_cursor();
}

auto Sorcery::ScreenRenderer::_display_delete() -> void {

	_ui.draw_components("delete");
	if (_ui.popups->dialog_delete->show)
		_ui.popups->dialog_delete->display(_ctx.get_flag_ref("want_delete_ok"));
	_ui.draw_cursor();
}

auto Sorcery::ScreenRenderer::_display_legate() -> void {

	_ui.draw_components("legate");
	_ui.draw_cursor();
}

auto Sorcery::ScreenRenderer::_display_reorder(const int mode) -> void {

	_ui.draw_components("reorder", mode);
	_ui.draw_cursor();
}

auto Sorcery::ScreenRenderer::_display_choose(const int mode) -> void {

	_ui.draw_components("choose", mode);
	_draw_choose(mode);
	_ui.draw_cursor();
}

auto Sorcery::ScreenRenderer::_display_inspect(const int mode) -> void {

	_ui.draw_components("inspect", mode);
	if (mode & INSPECT_MODE_ACTIONS)
		_ui.draw_components("inspect_actions", mode);
	_ui.draw_current_character(mode);
	if (_ui.popups->modal_identify->show)
		_ui.popups->modal_identify->display(_ctx.get_flag_ref("want_identify"));
	if (_ui.popups->modal_equip->show)
		_ui.popups->modal_equip->display(_ctx.get_flag_ref("want_equip"));
	if (_ui.popups->modal_remove->show)
		_ui.popups->modal_remove->display(_ctx.get_flag_ref("want_remove"));
	if (_ui.popups->modal_spell->show)
		_ui.popups->modal_spell->display(_ctx.get_flag_ref("want_spell"));
	if (_ui.popups->modal_drop->show)
		_ui.popups->modal_drop->display(_ctx.get_flag_ref("want_drop"));
	if (_ui.popups->modal_trade->show)
		_ui.popups->modal_trade->display(_ctx.get_flag_ref("want_trade"));
	if (_ui.popups->modal_give->show)
		_ui.popups->modal_give->display(_ctx.get_flag_ref("want_give"));
	if (_ui.popups->modal_use->show)
		_ui.popups->modal_use->display(_ctx.get_flag_ref("want_use"));
	if (_ui.popups->modal_invoke->show)
		_ui.popups->modal_invoke->display(_ctx.get_flag_ref("want_invoke"));
	if (_ui.popups->notice_pool_gold->show)
		_ui.popups->notice_pool_gold->display(
			_ctx.get_flag_ref("want_pool_gold"));
	_ui.draw_debug();
	_ui.draw_cursor();
}

auto Sorcery::ScreenRenderer::_display_spellbook() -> void {

	_ui.draw_components("spellbook");
	_ui.draw_bg_video();
	_ui.draw_spell_info();
	_ui.draw_cursor();
}

auto Sorcery::ScreenRenderer::_display_graveyard() -> void {

	_ui.draw_components("graveyard");
	_ui.draw_party_wipe();
	_ui.draw_cursor();
}

auto Sorcery::ScreenRenderer::_display_victory() -> void {

	_ui.draw_components("victory");
	_ui.draw_cursor();
}

auto Sorcery::ScreenRenderer::_display_automap() -> void {

	_ui.draw_components("automap");
	_ui.draw_current_level_map();
	auto legend{_ui.components->get("automap:automap_legend")};
	_ui.draw_automap_legend(&legend);
	_ui.draw_cursor();
}

auto Sorcery::ScreenRenderer::_display_atlas() -> void {

	_ui.draw_components("atlas");
	_ui.draw_bg_video();
	_ui.draw_level_no_player();
	_ui.draw_cursor();
}

auto Sorcery::ScreenRenderer::_display_museum() -> void {

	_ui.draw_components("museum");
	_ui.draw_bg_video();
	_ui.draw_item_info();
	_ui.draw_cursor();
}

auto Sorcery::ScreenRenderer::_display_inn() -> void {

	_ui.draw_components("inn");
	_ui.draw_party_panel();
	_ui.popups->modal_inspect->display(_ctx.get_flag_ref("want_inspect"));
	_ui.popups->modal_equip->display(_ctx.get_flag_ref("want_equip"));
	_ui.popups->modal_remove->display(_ctx.get_flag_ref("want_remove"));
	_ui.popups->modal_spell->display(_ctx.get_flag_ref("want_spell"));
	_ui.popups->modal_identify->display(_ctx.get_flag_ref("want_identify"));
	_ui.popups->modal_drop->display(_ctx.get_flag_ref("want_drop"));
	_ui.popups->modal_give->display(_ctx.get_flag_ref("want_give"));
	_ui.popups->modal_trade->display(_ctx.get_flag_ref("want_trade"));
	_ui.popups->modal_use->display(_ctx.get_flag_ref("want_use"));
	_ui.popups->modal_invoke->display(_ctx.get_flag_ref("want_invoke"));
	_ui.popups->notice_pool_gold->display(_ctx.get_flag_ref("want_pool_gold"));
	_ui.draw_debug();
	_ui.draw_cursor();
}

auto Sorcery::ScreenRenderer::_display_shop() -> void {

	_ui.draw_components("shop");
	_ui.draw_party_panel();
	_ui.draw_debug();
	_ui.draw_cursor();
}

auto Sorcery::ScreenRenderer::_display_stay() -> void {

	_ui.draw_components("stay");
	_draw_stay();
	_ui.draw_party_panel();
	_ui.draw_debug();
	_ui.draw_cursor();
}

auto Sorcery::ScreenRenderer::_display_buy() -> void {

	_ui.draw_components("buy");
	_draw_buy();
	_ui.draw_party_panel();
	_ui.draw_debug();
	_ui.draw_cursor();
}

auto Sorcery::ScreenRenderer::_display_sell() -> void {

	_ui.draw_components("sell");
	_draw_sell();
	_ui.draw_party_panel();
	_ui.draw_debug();
	_ui.draw_cursor();
}

auto Sorcery::ScreenRenderer::_display_identify() -> void {
	_ui.draw_components("identify");
	_draw_identify();
	_ui.draw_party_panel();
	_ui.draw_debug();
	_ui.draw_cursor();
}

auto Sorcery::ScreenRenderer::_display_uncurse() -> void {

	_ui.draw_components("uncurse");
	_draw_uncurse();
	_ui.draw_party_panel();
	_ui.draw_debug();
	_ui.draw_cursor();
}

auto Sorcery::ScreenRenderer::_display_store() -> void {

	_ui.draw_components("store");
	_draw_store();
	_ui.draw_party_panel();
	_ui.popups->notice_pool_gold->display(_ctx.get_flag_ref("want_pool_gold"));
	_ui.draw_debug();
	_ui.draw_cursor();
}

auto Sorcery::ScreenRenderer::_display_chest(const int stage) -> void {

	_ui.draw_components("chest");
	_draw_chest(enum_cast<Enums::Chests::State>(stage).value());

	// Transient overlay
	_ui.draw_transient();

	if (_ui.popups->modal_chest->show)
		_ui.popups->modal_chest->display(_ctx.get_flag_ref("want_chest"));
	_ui.draw_cursor();
}

auto Sorcery::ScreenRenderer::_display_recovery(const int mode) -> void {

	_ui.draw_components("recovery");
	_draw_recovery(mode);
	_ui.draw_party_panel();
	_ui.draw_debug();
	_ui.draw_cursor();
}

auto Sorcery::ScreenRenderer::_display_heal(int stage) -> void {

	_ui.draw_components("heal");
	_draw_heal(stage);
	_ui.draw_party_panel();
	_ui.draw_debug();
	_ui.draw_cursor();
}

auto Sorcery::ScreenRenderer::_display_rite(int stage) -> void {

	_ui.draw_components("rite");
	if (_ui.popups->dialog_rite->show)
		_ui.popups->dialog_rite->display(_ctx.get_flag_ref("want_rite_ok"));
	_draw_rite(stage);
	_ui.draw_cursor();
}

auto Sorcery::ScreenRenderer::_display_no_level_up(const int mode) -> void {

	_ui.draw_components("nolevelup");
	_draw_no_level_up(mode);
	_ui.draw_party_panel();
	_ui.draw_debug();
	_ui.draw_cursor();
}

auto Sorcery::ScreenRenderer::_display_level_up(const int mode) -> void {

	_ui.draw_components("levelup");
	_draw_level_up(mode);
	_ui.draw_party_panel();
	_ui.draw_debug();
	_ui.draw_cursor();
}

auto Sorcery::ScreenRenderer::_display_tavern() -> void {

	_ui.draw_components("tavern");
	_ui.popups->notice_divvy->display(_ctx.get_flag_ref("want_divvy_gold"));
	_ui.popups->notice_pool_gold->display(_ctx.get_flag_ref("want_pool_gold"));
	_ui.popups->modal_inspect->display(_ctx.get_flag_ref("want_inspect"));
	_ui.popups->modal_equip->display(_ctx.get_flag_ref("want_equip"));
	_ui.popups->modal_remove->display(_ctx.get_flag_ref("want_remove"));
	_ui.popups->modal_spell->display(_ctx.get_flag_ref("want_spell"));
	_ui.popups->modal_identify->display(_ctx.get_flag_ref("want_identify"));
	_ui.popups->modal_drop->display(_ctx.get_flag_ref("want_drop"));
	_ui.popups->modal_use->display(_ctx.get_flag_ref("want_use"));
	_ui.popups->modal_give->display(_ctx.get_flag_ref("want_give"));
	_ui.popups->modal_trade->display(_ctx.get_flag_ref("want_trade"));
	_ui.popups->modal_invoke->display(_ctx.get_flag_ref("want_invoke"));
	_ui.draw_party_panel();
	_ui.draw_debug();
	_ui.draw_cursor();
}

auto Sorcery::ScreenRenderer::_display_pay() -> void {

	_ui.draw_components("pay");
	_ui.draw_pay_info();
	_ui.draw_party_panel();
	_ui.draw_debug();
	_ui.draw_cursor();
}

auto Sorcery::ScreenRenderer::_display_temple() -> void {

	_ui.draw_components("temple");
	_ui.draw_party_panel();
	_ui.draw_debug();
	_ui.draw_cursor();
}

auto Sorcery::ScreenRenderer::_display_remove() -> void {

	_ui.draw_components("remove");
	_ui.draw_cursor();
}

auto Sorcery::ScreenRenderer::_display_restart() -> void {

	_ui.draw_components("restart");
	_ui.draw_cursor();
}

auto Sorcery::ScreenRenderer::_display_add() -> void {

	_ui.draw_components("add");
	_ui.draw_cursor();
}

auto Sorcery::ScreenRenderer::_display_options() -> void {

	_ui.draw_components("options");
	_ui.draw_bg_video();
	_ui.draw_options();
	_ui.draw_cursor();
}

auto Sorcery::ScreenRenderer::_display_license(const std::string &string)
	-> void {

	_ui.draw_components("license");
	_ui.draw_bg_video();

	auto component{_ui.components->get("license:license_info")};
	_draw_license(&component, string);
	_ui.draw_cursor();
}

auto Sorcery::ScreenRenderer::_display_create_alignment(const int mode)
	-> void {

	_ui.draw_components("create_alignment", mode);
	_draw_create_alignment(mode);
	_ui.draw_cursor();
}

auto Sorcery::ScreenRenderer::_display_create_confirm(const int mode) -> void {

	_ui.draw_components("create_confirm", mode);
	_draw_create_confirm(mode);
	_ui.draw_cursor();
}

auto Sorcery::ScreenRenderer::_display_create_class(const int mode) -> void {

	_ui.draw_components("create_class", mode);
	_draw_create_class(mode);
	_ui.draw_cursor();
}

auto Sorcery::ScreenRenderer::_display_create_race(const int mode) -> void {

	_ui.draw_components("create_race", mode);
	_draw_create_race(mode);
	_ui.draw_cursor();
}

auto Sorcery::ScreenRenderer::_display_create_name(const int mode) -> void {

	_ui.draw_components("create_name", mode);
	_draw_create_name(mode);
	_ui.draw_cursor();
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
	auto character{_ctx.game->characters.at(
		_ctx.controller->get_character(Enums::CharacterSlot::EDIT))};
	auto summary_text{character.summary_text()};
	_ui.draw_text(&cmp_summary, summary_text);

	// As next custom component is a text box, focus on that initially
	if (_ui.first_frame) {
		ImGui::SetKeyboardFocusHere();
		_ui.first_frame = false;
	}

	auto cmp_name{_ui.components->get("rename:rename_input")};
	_ui.draw_input(&cmp_name, &_ctx.controller->get_input_buffer());
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

	const auto x{(_ui.grid_x(cmp.x) - (chest_w / 2)) + 2};

	const auto adj_y{_ctx.get_flag("interface_ui") &&
							 _ctx.get_flag("interface_party_panel")
						 ? cmp.y
						 : cmp.y + 7};

	const auto y{_ui.grid_y(adj_y) - (chest_h / 2)};

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
	_ui.draw_cursor();
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
		set_Font(_ui.fontstore->get_current_font(cmp_char.font).value(),
				 _ui.font_sz());
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
		_ui.draw_stepper(&cmp_attribute, cmp_name, attribute);
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
	auto summary_text{
		_ctx.controller->get_candidate_character()->summary_text()};
	_ui.draw_text(&cmp_summary, summary_text);

	// As next custom component is a text box, focus on that initially
	if (_ui.first_frame) {
		ImGui::SetKeyboardFocusHere();
		_ui.first_frame = false;
	}

	auto cmp_name{_ui.components->get("create_name:name_input")};
	_ui.draw_input(&cmp_name, &_ctx.controller->get_input_buffer());
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
			cmp.y += _ui.grid_delta(0, 1).y;
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
			const auto width{_ui.grid_sz() *
							 component->get_float("grid_width")};
			const auto viewport{ImGui::GetMainViewport()};
			return (viewport->Size.x - width) / 2;
		})};

		const auto pos{ImVec2{x, _ui.grid_y(component->y)}};
		ImGui::SetNextWindowPos(pos);
		with_Child(
			"license_child",
			ImVec2(_ui.grid_sz() * component->w, _ui.grid_sz() * component->h),
			ImGuiChildFlags_NavFlattened,
			ImGuiWindowFlags_AlwaysVerticalScrollbar) {

			UIStyle::set_text_dim(_ctx);
			set_Font(_ui.fontstore->get_current_font(component->font).value(),
					 _ui.font_sz());
			with_TextWrapPos(ImGui::GetFontSize() * component->w) {
				ImGui::TextUnformatted(string.c_str());
			}
		}

		// Special Handling for Return Button
		Component cmp{_ui.components->get("license:license_return")};
		_ui.draw_button_click(&cmp, _ctx.get_flag_ref("show_license"), true);
	}
}