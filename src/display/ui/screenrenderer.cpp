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

auto Sorcery::ScreenRenderer::ScreenRenderer(UI &ui, Context &ctx)
	: _ui{ui},
	  _ctx{ctx} {

	// Initialise function tables for display methods
	_draw_modules[Enums::Screen::ATLAS] = [this] {
		_display_atlas();
	};
	_draw_modules[Enums::Screen::BESTIARY] = [this] {
		_display_bestiary();
	};
	_draw_modules[Enums::Screen::COMPENDIUM] = [this] {
		_display_compendium();
	};
	_draw_modules[Enums::Screen::MAINMENU] = [this] {
		_display_main_menu();
	};
	_draw_modules[Enums::Screen::MUSEUM] = [this] {
		_display_museum();
	};
	_draw_modules[Enums::Screen::OPTIONS] = [this] {
		_display_options();
	};
	_draw_modules[Enums::Screen::SPELLBOOK] = [this] {
		_display_spellbook();
	};
	_draw_modules[Enums::Screen::SPLASH] = [this] {
		_display_splash();
	};

	_draw_modules[Enums::Screen::ADD] = [this]() {
		_display_add();
	};
	_draw_modules[Enums::Screen::BUY] = [this]() {
		_display_buy();
	};
	_draw_modules[Enums::Screen::CASTLE] = [this]() {
		_display_castle();
	};
	_draw_modules[Enums::Screen::EDGEOFTOWN] = [this]() {
		_display_edge_of_town();
	};
	_draw_modules[Enums::Screen::EDIT] = [this]() {
		_display_edit();
	};
	_draw_modules[Enums::Screen::RENAME] = [this]() {
		_display_rename();
	};
	_draw_modules[Enums::Screen::RECLASS] = [this]() {
		_display_reclass();
	};
	_draw_modules[Enums::Screen::AUTOMAP] = [this]() {
		_display_automap();
	};
	_draw_modules[Enums::Screen::DELETE] = [this]() {
		_display_delete();
	};
	_draw_modules[Enums::Screen::GRAVEYARD] = [this]() {
		_display_graveyard();
	};
	_draw_modules[Enums::Screen::VICTORY] = [this]() {
		_display_victory();
	};
	_draw_modules[Enums::Screen::IDENTIFY] = [this]() {
		_display_identify();
	};
	_draw_modules[Enums::Screen::INN] = [this]() {
		_display_inn();
	};
	_draw_modules[Enums::Screen::LEGATE] = [this]() {
		_display_legate();
	};
	_draw_modules[Enums::Screen::PAY] = [this]() {
		_display_pay();
	};
	_draw_modules[Enums::Screen::REMOVE] = [this]() {
		_display_remove();
	};
	_draw_modules[Enums::Screen::RESTART] = [this]() {
		_display_restart();
	};
	_draw_modules[Enums::Screen::RETRAIN] = [this]() {
		_display_retrain();
	};
	_draw_modules[Enums::Screen::ROSTER] = [this]() {
		_display_roster();
	};
	_draw_modules[Enums::Screen::SELECT] = [this]() {
		_display_select();
	};
	_draw_modules[Enums::Screen::SELL] = [this]() {
		_display_sell();
	};
	_draw_modules[Enums::Screen::SHOP] = [this]() {
		_display_shop();
	};
	_draw_modules[Enums::Screen::STAY] = [this]() {
		_display_stay();
	};
	_draw_modules[Enums::Screen::STORE] = [this]() {
		_display_store();
	};
	_draw_modules[Enums::Screen::TAVERN] = [this]() {
		_display_tavern();
	};
	_draw_modules[Enums::Screen::TEMPLE] = [this]() {
		_display_temple();
	};
	_draw_modules[Enums::Screen::TRAINING] = [this]() {
		_display_training_grounds();
	};
	_draw_modules[Enums::Screen::UNCURSE] = [this]() {
		_display_uncurse();
	};

	_draw_modules_with_int[Enums::Screen::CREATE_NAME] = [this](int n) {
		_display_create_name(n);
	};

	_draw_modules_with_int[Enums::Screen::CREATE_RACE] = [this](int n) {
		_display_create_race(n);
	};

	_draw_modules_with_int[Enums::Screen::CREATE_ALIGNMENT] = [this](int n) {
		_display_create_alignment(n);
	};

	_draw_modules_with_int[Enums::Screen::CREATE_CONFIRM] = [this](int n) {
		_display_create_confirm(n);
	};

	_draw_modules_with_int[Enums::Screen::CREATE_CLASS] = [this](int n) {
		_display_create_class(n);
	};
	_draw_modules_with_int[Enums::Screen::CHEST] = [this](int n) {
		_display_chest(n);
	};

	_draw_modules_with_int[Enums::Screen::HEAL] = [this](int n) {
		_display_heal(n);
	};
	_draw_modules_with_int[Enums::Screen::RITE] = [this](int n) {
		_display_rite(n);
	};
	_draw_modules_with_int[Enums::Screen::INSPECT] = [this](int n) {
		_display_inspect(n);
	};
	_draw_modules_with_int[Enums::Screen::LEVELUP] = [this](int n) {
		_display_level_up(n);
	};
	_draw_modules_with_int[Enums::Screen::NOLEVELUP] = [this](int n) {
		_display_no_level_up(n);
	};
	_draw_modules_with_int[Enums::Screen::RECOVERY] = [this](int n) {
		_display_recovery(n);
	};
	_draw_modules_with_int[Enums::Screen::REORDER] = [this](int n) {
		_display_reorder(n);
	};

	_draw_modules_with_string[Enums::Screen::LICENSE] =
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
	popups->notice_renamed_ok->display(_ctx.get_flag_ref("want_renamed_ok"));
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
	_ui._draw_current_level_map();
	auto legend{components->get("automap:automap_legend")};
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
	_draw_transient();

	if (popups->modal_chest->show)
		popups->modal_chest->display(_ctx.get_flag_ref("want_chest"));
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
	if (popups->dialog_rite->show)
		popups->dialog_rite->display(_ctx.get_flag_ref("want_rite_ok"));
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
	popups->notice_divvy->display(_ctx.get_flag_ref("want_divvy_gold"));
	popups->notice_pool_gold->display(_ctx.get_flag_ref("want_pool_gold"));
	popups->modal_inspect->display(_ctx.get_flag_ref("want_inspect"));
	popups->modal_equip->display(_ctx.get_flag_ref("want_equip"));
	popups->modal_remove->display(_ctx.get_flag_ref("want_remove"));
	popups->modal_spell->display(_ctx.get_flag_ref("want_spell"));
	popups->modal_identify->display(_ctx.get_flag_ref("want_identify"));
	popups->modal_drop->display(_ctx.get_flag_ref("want_drop"));
	popups->modal_use->display(_ctx.get_flag_ref("want_use"));
	popups->modal_give->display(_ctx.get_flag_ref("want_give"));
	popups->modal_trade->display(_ctx.get_flag_ref("want_trade"));
	popups->modal_invoke->display(_ctx.get_flag_ref("want_invoke"));
	_ui.draw_party_panel();
	_ui.draw_debug();
	_ui.draw_cursor();
}

auto Sorcery::ScreenRenderer::_display_pay() -> void {

	_ui.draw_components("pay");
	_draw_pay_info();
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
	_draw_options();
	_ui.draw_cursor();
}

auto Sorcery::ScreenRenderer::_display_license(const std::string &string)
	-> void {

	_ui.draw_components("license");
	_ui.draw_bg_video();

	auto component{components->get("license:license_info")};
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
		auto bg_c{components->get("engine_base_ui:background_image")};
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

	const auto x{(grid_x(cmp.x) - (chest_w / 2)) + 2};

	const auto adj_y{_ctx.get_flag("interface_ui") &&
							 _ctx.get_flag("interface_party_panel")
						 ? cmp.y
						 : cmp.y + 7};

	const auto y{grid_y(adj_y) - (chest_h / 2)};

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
		set_Font(fontstore->get_current_font(cmp_char.font).value(), font_sz());
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
	draw_text(&cmp_summary, summary_text);
}

auto Sorcery::ScreenRenderer::_draw_create_name([[maybe_unused]] const int mode)
	-> void {

	auto cmp_summary{_ui.components->get("create_name:summary_text")};
	auto summary_text{
		_ctx.controller->get_candidate_character()->summary_text()};
	draw_text(&cmp_summary, summary_text);

	// As next custom component is a text box, focus on that initially
	if (_ui.first_frame) {
		ImGui::SetKeyboardFocusHere();
		_ui.first_frame = false;
	}

	auto cmp_name{_ui.components->get("create_name:name_input")};
	_draw_input(&cmp_name, &_ctx.controller->get_input_buffer());
}

auto Sorcery::ScreenRenderer::_draw_choose(const int mode) -> void {

	if (mode & CHOOSE_MODE_STAY) {

		auto cmp{c_ui.omponents->get("choose:choose_stay")};
		_ui.draw_text(&cmp);
		_ui.draw_party_panel();
	}
}

auto Sorcery::ScreenRenderer::_draw_level_up(const int mode) -> void {

	auto &character{_ctx.game->characters.at(
		_ctx.controller->get_character(Enums::CharacterSlot::STAY))};

	if (mode & RECOVERY_BIRTHDAY) {

		const auto birth_text{_ctx.get_string("REST_BIRTHDAY_YOU")};
		auto cmp{components->get("levelup:levelup_birthday")};
		_ui.draw_text(&cmp, birth_text);
		cmp = components->get("levelup:levelup_results");
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

auto Sorcery::UI::_draw_options() -> void {
	const auto component{components->get("options:options_info")};

	std::vector<std::string> summary_opts{"OPT_RECOMMENDED_MODE",
										  "OPT_STRICT_MODE", "OPT_CHEAT_MODE",
										  "OPT_AUTO_SAVE", "OPT_DICE_ROLLS"};
	std::vector<std::string> gameplay_opts{
		"OPT_MIXED_ALIGNMENT",		 "OPT_LEVEL_STAT_LOSS",
		"OPT_LEVEL_REROLL_HP",		 "OPT_CLASS_CHANGE_RESET",
		"OPT_CLASS_CHANGE_AGING",	 "OPT_AMBUSH_HIDE",
		"OPT_SURPRISE_SPELLCASTING", "OPT_INN_HEALING",
		"OPT_REROLL_ONES",			 "OPT_LOST_LEGATION",
		"OPT_CURABLE_DRAINING",		 "OPT_SHARED_INVENTORY",
		"OPT_PROTECT_TELEPORT"};

	std::vector<std::string> graphics_opts{"OPT_COLOURED_WIREFRAME",
										   "OPT_FULLSCREEN", "OPT_UI_MUSIC",
										   "OPT_UI_SOUND"};

	const auto save_lbl{_ctx.get_string("DIALOG_SAVE")};
	const auto cancel_lbl{_ctx.get_string("DIALOG_CANCEL")};
	set_Font(fontstore->get_current_font(component.font).value(), font_sz());
	const auto col{get_hl_colour(_ctx.animation->lerp)};
	with_Window(WINDOW_LAYER_MENUS, nullptr, ImGuiWindowFlags_NoDecoration) {

		// To adjust for Window Resizing etc
		const auto x{std::invoke([&] {
			const auto width{grid_sz() * component.get_float("grid_width")};
			const auto viewport{ImGui::GetMainViewport()};
			return (viewport->Size.x - width) / 2;
		})};

		const auto pos{ImVec2{x, grid_y(component.y)}};
		ImGui::SetCursorPos(pos);

		// Now draw tab bar
		UIStyle::set_faded(_ctx);
		set_StyleColor(ImGuiCol_ButtonHovered, (ImVec4)col);
		const auto tabs_width{component.w * grid_sz()};
		const auto tabs_height{component.h * grid_sz()};
		ImGuiTabBarFlags tb_flags{ImGuiTabBarFlags_None};
		with_Child("options_tab_bar_child", ImVec2(tabs_width, tabs_height)) {
			UIStyle::set_tab_black(_ctx);

			auto summary_idx{0u};
			auto gameplay_idx(5u);
			auto graphics_idx(18u);

			set_StyleColor(ImGuiCol_Button,
						   ImVec4{0.16f, 0.66f, 0.45f, _ctx.animation->fade});
			set_StyleColor(ImGuiCol_ButtonHovered,
						   ImVec4{0.0f, 1.0f, 0.57f, _ctx.animation->fade});
			set_StyleColor(ImGuiCol_Text,
						   ImVec4{1.0f, 1.0f, 1.0f, _ctx.animation->fade});

			// now draw tabs
			with_TabBar("options_tab_bar", tb_flags) {
				auto tabname{"Options"};
				with_TabItem(tabname) {
					for (const auto &opt : summary_opts) {
						if (opt.length() > 0) {
							const auto text{
								std::format(" {}", _ctx.get_string(opt))};
							if (ImGui::Toggle(text.c_str(),
											  &_ctx.get_config(summary_idx))) {

								// Do additonal handling such as switching
								// on strict mode etc inside the controller
								_ctx.controller->handle_toggle(
									component.name, tabname, summary_idx);
							};
						}
						++summary_idx;
					}
				}
				tabname = "Gameplay";
				with_TabItem(tabname) {
					for (const auto &opt : gameplay_opts) {
						if (opt.length() > 0) {
							const auto text{
								std::format(" {}", _ctx.get_string(opt))};
							if (ImGui::Toggle(text.c_str(),
											  &_ctx.get_config(gameplay_idx))) {

								// Do additonal handling such as switching
								// on strict mode etc
								_ctx.controller->handle_toggle(
									component.name, tabname, gameplay_idx);
							};
						}
						++gameplay_idx;
					}
				}
				tabname = "UI";
				with_TabItem(tabname) {

					for (const auto &opt : graphics_opts) {
						if (opt.length() > 0) {
							const auto text{
								std::format(" {}", _ctx.get_string(opt))};
							if (ImGui::Toggle(text.c_str(),
											  &_ctx.get_config(graphics_idx))) {

								// Do additonal handling such as switching
								// on strict mode etc
								_ctx.controller->handle_toggle(
									component.name, tabname, graphics_idx);
							};
						}
						++graphics_idx;
					}

					// Font Selection dropdown
					ImGui::Separator();
					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + grid_sz());
					const auto item_height{
						ImGui::GetTextLineHeightWithSpacing()};
					const auto max_visible_items{10};
					ImGui::SetNextWindowSize(
						ImVec2(0, item_height * max_visible_items));
					with_Combo("##font_combobox", "Chooose Font...") {

						auto fonts{fontstore->get_all_monospace_fonts()};
						auto font_idx{0u};
						for (const auto &font : fonts) {
							const bool is_selected{
								font.name ==
								fontstore->get_current_monospace_font_name()};
							set_Font(font.font, font_sz());
							auto selectable_name{
								std::format("{}##{}", font.name, font_idx)};
							if (ImGui::Selectable(selectable_name.c_str(),
												  is_selected)) {
								fontstore->set_current_font(
									Enums::Layout::Font::MONOSPACE, font.name);
							}
							++font_idx;
						}
					}

					ImGui::NewLine();

					// Color Pickers
					ImGuiColorEditFlags flags{ImGuiColorEditFlags_NoAlpha |
											  ImGuiColorEditFlags_NoInputs |
											  ImGuiColorEditFlags_NoTooltip |
											  ImGuiColorEditFlags_NoOptions};
					auto frame_name{std::format("{}##1", "UI Colour")};
					// ImGui::SetCursorPosY(ImGui::GetCursorPosY() +
					// grid_sz);
					ImGui::SetNextItemWidth(28.f);
					ImGui::ColorEdit3(frame_name.c_str(), (float *)&ui_colour,
									  flags);
				};
			}
			set_Font(fontstore->get_current_font(component.font).value());

			// Save and Cancel Buttons
			const auto centre{(tabs_width / 2)};
			const auto button_y{std::stoi(component.get("button_y").value())};
			ImVec2 btn_size{ImGui::GetFontSize() * 7.0f, 0.0f};

			UIStyle::set_faded(_ctx);
			set_StyleColor(ImGuiCol_ButtonHovered, ImVec4{col});
			set_StyleColor(ImGuiCol_ButtonActive, (ImVec4)col);

			ImGui::SetCursorPos(ImVec2{centre - (btn_size.x + grid_sz()),
									   button_y * grid_sz()});
			if (ImGui::Button(save_lbl.c_str(), btn_size)) {
				_ctx.system->config->save();

				if (_ctx.get_flag("in_engine"))
					_ctx.controller->go_to(Enums::Screen::ENGINE);
				else
					_ctx.controller->go_to(Enums::Screen::MAINMENU);

				//_ctx.controller->unset_flag("show_options");
			}
			ImGui::SetCursorPos(
				ImVec2{centre + grid_sz(), button_y * grid_sz()});
			if (ImGui::Button(cancel_lbl.c_str(), btn_size)) {
				_ctx.system->config->load();

				if (_ctx.get_flag("in_engine"))
					_ctx.controller->go_to(Enums::Screen::ENGINE);
				else
					_ctx.controller->go_to(Enums::Screen::MAINMENU);
				//_ctx.controller->unset_flag("show_options");
			}
		}
	}
}
