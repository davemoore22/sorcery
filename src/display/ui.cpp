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
#include "display/ui.hpp"
#include "engine/define.hpp"
#include "engine/types.hpp"
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

Sorcery::UI::UI(Context &ctx)
	: _ctx{ctx} {

	// Storage
	components = std::make_unique<ComponentStore>(_ctx.get_file(LAYOUT_FILE));
	images = std::make_unique<ImageStore>(_ctx);
	menubuilder = std::make_unique<MenuBuilder>(_ctx);

	// Can't create fontstore just yet as it needs IMGUI initialised

	// VFX and SFX players
	vfx_player = std::make_unique<VideoPlayer>();

	// Initialise main menu background vfx
	try {

		auto bg_vfx_path{_ctx.get_file(MAINMENU_VIDEO).string()};
		vfx_player->load(bg_vfx_path.c_str());

	} catch (std::exception &e) {

		Error error{Enums::System::Error::VFX_ERROR, e,
					"could not load main menu vfx!"};
		std::cerr << error;
		exit(EXIT_FAILURE);
	}

	// Custom components
	dialog_exit =
		std::make_unique<Dialog>(_ctx, components->get("main_menu:dialog_exit"),
								 Enums::Layout::DialogType::CONFIRM);
	dialog_new =
		std::make_unique<Dialog>(_ctx, components->get("main_menu:dialog_new"),
								 Enums::Layout::DialogType::CONFIRM);
	dialog_leave = std::make_unique<Dialog>(
		_ctx, components->get("main_menu:dialog_leave"),
		Enums::Layout::DialogType::CONFIRM);
	dialog_rite =
		std::make_unique<Dialog>(_ctx, components->get("rite:dialog_rite"),
								 Enums::Layout::DialogType::CONFIRM);
	dialog_search = std::make_unique<Dialog>(
		_ctx, components->get("engine_base_ui:dialog_search"),
		Enums::Layout::DialogType::CONFIRM);
	dialog_delete =
		std::make_unique<Dialog>(_ctx, components->get("delete:dialog_delete"),
								 Enums::Layout::DialogType::CONFIRM);
	notice_divvy =
		std::make_unique<Dialog>(_ctx, components->get("global:notice_divvy"),
								 Enums::Layout::DialogType::OK);
	notice_renamed_ok = std::make_unique<Dialog>(
		_ctx, components->get("global:notice_renamed_ok"),
		Enums::Layout::DialogType::OK);
	notice_reclassed_ok = std::make_unique<Dialog>(
		_ctx, components->get("global:notice_reclassed_ok"),
		Enums::Layout::DialogType::OK);
	notice_pool_gold = std::make_unique<Dialog>(
		_ctx, components->get("global:notice_pool_gold"),
		Enums::Layout::DialogType::OK);
	notice_cannot_donate = std::make_unique<Dialog>(
		_ctx, components->get("global:notice_cannot_donate"),
		Enums::Layout::DialogType::OK);
	notice_donated_ok = std::make_unique<Dialog>(
		_ctx, components->get("global:notice_donated_ok"),
		Enums::Layout::DialogType::OK);
	notice_not_enough_gold = std::make_unique<Dialog>(
		_ctx, components->get("global:notice_not_enough_gold"),
		Enums::Layout::DialogType::OK);
	modal_camp = std::make_unique<Modal>(
		_ctx, components->get("engine_base_ui:modal_camp"));

	modal_elevator_top = std::make_unique<Modal>(
		_ctx, components->get("global:modal_elevator_top"));
	modal_elevator_bottom = std::make_unique<Modal>(
		_ctx, components->get("global:modal_elevator_bottom"));

	modal_drop =
		std::make_unique<Modal>(_ctx, components->get("global:modal_drop"));
	modal_inspect =
		std::make_unique<Modal>(_ctx, components->get("global:modal_inspect"));
	modal_identify =
		std::make_unique<Modal>(_ctx, components->get("global:modal_identify"));
	modal_chest =
		std::make_unique<Modal>(_ctx, components->get("global:modal_chest"));
	modal_equip =
		std::make_unique<Modal>(_ctx, components->get("global:modal_equip"));
	modal_remove = std::make_unique<Modal>(
		_ctx, components->get("global:modal_remove_item"));
	modal_trade =
		std::make_unique<Modal>(_ctx, components->get("global:modal_trade"));
	modal_give =
		std::make_unique<Modal>(_ctx, components->get("global:modal_give"));
	modal_use =
		std::make_unique<Modal>(_ctx, components->get("global:modal_use"));
	modal_invoke =
		std::make_unique<Modal>(_ctx, components->get("global:modal_invoke"));
	modal_spell =
		std::make_unique<Modal>(_ctx, components->get("global:modal_spell"));

	input_donate =
		std::make_unique<Input>(_ctx, components->get("global:input_donate"));
	input_name =
		std::make_unique<Input>(_ctx, components->get("global:input_name"));

	dialog_stairs_up = std::make_unique<Dialog>(
		_ctx, components->get("engine_base_ui:dialog_stairs_up"),
		Enums::Layout::DialogType::CONFIRM);
	dialog_stairs_down = std::make_unique<Dialog>(
		_ctx, components->get("engine_base_ui:dialog_stairs_down"),
		Enums::Layout::DialogType::CONFIRM);

	message_tile = std::make_unique<Message>(
		_ctx, components->get("engine_base_ui:message_tile"));

	// Window, Font, and Display Settings
	frame_rd = std::stoi(_ctx.get_config("Frame", "rounding"));
	ui_rd = std::stoi(_ctx.get_config("UI", "rounding"));

	// Updates _font_sz, _adj_grid_w, _adj_grid_h, and _grid_sz
	_ctx.display->update_display_metrics();
	update_grid_metrics(_ctx.display->get_display_metrics());

	// Render window
	_render = std::make_unique<Render>(_ctx);

	// Ticks
	ticks = SDL_GetTicks();

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
};

Sorcery::UI::~UI() {}

auto Sorcery::UI::grid_pos(const float x, const float y) const noexcept
	-> ImVec2 {

	const auto &metrics{_ctx.display->get_display_metrics()};

	return {metrics.offset_x + (x * _adj_grid_w),
			metrics.offset_y + (y * _adj_grid_h)};
}

auto Sorcery::UI::grid_delta(const float x, const float y) const noexcept
	-> ImVec2 {

	return {x * _adj_grid_w, y * _adj_grid_h};
}

auto Sorcery::UI::grid_x(const float x) const noexcept -> float {

	const auto &metrics{_ctx.display->get_display_metrics()};
	return metrics.offset_x + (x * _adj_grid_w);
}

auto Sorcery::UI::grid_y(const float y) const noexcept -> float {

	const auto &metrics{_ctx.display->get_display_metrics()};
	return metrics.offset_y + (y * _adj_grid_h);
}

auto Sorcery::UI::grid_sz() const noexcept -> unsigned int {

	return _grid_sz;
}

auto Sorcery::UI::columns() const noexcept -> unsigned int {

	return _columns;
}

auto Sorcery::UI::rows() const noexcept -> unsigned int {

	return _rows;
}

auto Sorcery::UI::set_monochrome(const bool value) -> void {

	_render->set_monochrome(value);
}

auto Sorcery::UI::set_fullscreen(const bool value) -> void {

	if (value)
		SDL_SetWindowFullscreen(_ctx.display->get_SDL_window(),
								SDL_WINDOW_FULLSCREEN_DESKTOP);
	else
		SDL_SetWindowFullscreen(_ctx.display->get_SDL_window(), 0);

	_ctx.controller->set_fullscreen(value);
}

auto Sorcery::UI::update_grid_metrics(const DisplayMetrics &metrics) noexcept
	-> void {

	const auto content_w{static_cast<float>(_base_width) * metrics.scale};
	const auto content_h{static_cast<float>(_base_height) * metrics.scale};

	_adj_grid_w = content_w / static_cast<float>(_columns);
	_adj_grid_h = content_h / static_cast<float>(_rows);
	_grid_sz = std::min(_adj_grid_w, _adj_grid_h);
	_base_font_sz = _base_width / static_cast<float>(_columns);
	_font_sz = _base_font_sz * metrics.scale;
}

// Create a Modal on Demand (used whenever data items on it aren't fixed - for
// example the Party Members); normally otherwise fixed Modals are created at
// the beginning as part of the Form/Module create
auto Sorcery::UI::create_dynamic_modal(const std::string name) -> void {

	// DEBUG_LOGF("Creating Dynamic Modal: {}", name);

	if (name == "modal_inspect") {
		if (modal_inspect.get())
			modal_inspect.reset();
		modal_inspect = std::make_unique<Modal>(
			_ctx, components->get("global:modal_inspect"));
		modal_inspect->regenerate();
	} else if (name == "modal_help") {
		if (modal_help.get())
			modal_help.reset();
		modal_help =
			std::make_unique<Modal>(_ctx, components->get("global:modal_help"));
		modal_help->regenerate();
	} else if (name == "modal_tithe") {
		if (modal_tithe.get())
			modal_tithe.reset();
		modal_tithe = std::make_unique<Modal>(
			_ctx, components->get("global:modal_tithe"));
		modal_tithe->regenerate();
	} else if (name == "modal_identify") {
		if (modal_identify.get())
			modal_identify.reset();
		modal_identify = std::make_unique<Modal>(
			_ctx, components->get("global:modal_identify"));
		modal_identify->regenerate();
	} else if (name == "modal_chest") {
		if (modal_chest.get())
			modal_chest.reset();
		modal_chest = std::make_unique<Modal>(
			_ctx, components->get("global:modal_chest"));
		modal_chest->regenerate();
	} else if (name == "modal_equip") {
		if (modal_equip.get())
			modal_equip.reset();
		modal_equip = std::make_unique<Modal>(
			_ctx, components->get("global:modal_equip"));
		modal_equip->regenerate();
	} else if (name == "modal_remove") {
		if (modal_remove.get())
			modal_remove.reset();
		modal_remove = std::make_unique<Modal>(
			_ctx, components->get("global:modal_remove_item"));
		modal_remove->regenerate();
	} else if (name == "modal_spell") {
		if (modal_spell.get())
			modal_spell.reset();
		modal_spell = std::make_unique<Modal>(
			_ctx, components->get("global:modal_spell"));
		modal_spell->regenerate();
	} else if (name == "modal_drop") {
		if (modal_drop.get())
			modal_drop.reset();
		modal_drop =
			std::make_unique<Modal>(_ctx, components->get("global:modal_drop"));
		modal_drop->regenerate();
	} else if (name == "modal_trade") {
		if (modal_trade.get())
			modal_trade.reset();
		modal_trade = std::make_unique<Modal>(
			_ctx, components->get("global:modal_trade"));
		modal_trade->regenerate();
	} else if (name == "modal_give") {
		if (modal_give.get())
			modal_give.reset();
		modal_give =
			std::make_unique<Modal>(_ctx, components->get("global:modal_give"));
		modal_give->regenerate();
	} else if (name == "modal_use") {
		if (modal_use.get())
			modal_use.reset();
		modal_use =
			std::make_unique<Modal>(_ctx, components->get("global:modal_use"));
		modal_use->regenerate();
	} else if (name == "modal_invoke") {
		if (modal_invoke.get())
			modal_invoke.reset();
		modal_invoke = std::make_unique<Modal>(
			_ctx, components->get("global:modal_invoke"));
		modal_invoke->regenerate();
	}

	// Note that modal_camp is not dynamic and thus isn't handled here
}

auto Sorcery::UI::_draw_window_menu() -> void {}

// Not an ideal function, really need to maintain a pointer status map instead
auto Sorcery::UI::_get_popups() const -> std::string {

	auto get_popup_status{[](void *component, std::string type) -> std::string {
		if (component != nullptr) {
			if (type == "modal") {
				auto casted{(Modal *)component};
				auto name{casted->name()};
				return std::format("{:>26}: {}\n", name, casted->show);
			} else if (type == "dialog") {
				auto casted{(Dialog *)component};
				auto name{casted->name()};
				return std::format("{:>26}: {}\n", name, casted->show);
			} else if (type == "message") {
				auto casted{(Message *)component};
				auto name{casted->name()};
				return std::format("{:>26}: {}\n", name, casted->show);
			} else if (type == "popup") {
				auto casted{(Popup *)component};
				auto name{casted->name()};
				return std::format("{:>26}: {}\n", name, casted->show);
			} else if (type == "input") {
				auto casted{(Input *)component};
				auto name{casted->name()};
				return std::format("{:>26}: {}\n", name, casted->show);
			}
		}

		return "";
	}};

	std::string output{};
	if (dialog_exit)
		output.append(get_popup_status((void *)dialog_exit.get(), "dialog"));
	if (dialog_leave)
		output.append(get_popup_status((void *)dialog_leave.get(), "dialog"));
	if (dialog_new)
		output.append(get_popup_status((void *)dialog_new.get(), "dialog"));
	if (dialog_rite)
		output.append(get_popup_status((void *)dialog_rite.get(), "dialog"));
	if (dialog_search)
		output.append(get_popup_status((void *)dialog_search.get(), "dialog"));
	if (dialog_delete)
		output.append(get_popup_status((void *)dialog_delete.get(), "dialog"));
	if (dialog_stairs_down)
		output.append(
			get_popup_status((void *)dialog_stairs_down.get(), "dialog"));
	if (dialog_stairs_up)
		output.append(
			get_popup_status((void *)dialog_stairs_up.get(), "dialog"));
	if (input_donate)
		output.append(get_popup_status((void *)input_donate.get(), "input"));
	if (input_name)
		output.append(get_popup_status((void *)input_name.get(), "input"));
	if (message_tile)
		output.append(get_popup_status((void *)message_tile.get(), "message"));
	if (modal_camp)
		output.append(get_popup_status((void *)modal_camp.get(), "modal"));
	if (modal_chest)
		output.append(get_popup_status((void *)modal_chest.get(), "modal"));
	if (modal_elevator_bottom)
		output.append(
			get_popup_status((void *)modal_elevator_bottom.get(), "modal"));
	if (modal_elevator_top)
		output.append(
			get_popup_status((void *)modal_elevator_top.get(), "modal"));
	if (modal_drop)
		output.append(get_popup_status((void *)modal_drop.get(), "modal"));
	if (modal_equip)
		output.append(get_popup_status((void *)modal_equip.get(), "modal"));
	if (modal_give)
		output.append(get_popup_status((void *)modal_give.get(), "modal"));
	if (modal_help)
		output.append(get_popup_status((void *)modal_help.get(), "modal"));
	if (modal_identify)
		output.append(get_popup_status((void *)modal_identify.get(), "modal"));
	if (modal_inspect)
		output.append(get_popup_status((void *)modal_inspect.get(), "modal"));
	if (modal_invoke)
		output.append(get_popup_status((void *)modal_invoke.get(), "modal"));
	if (modal_remove)
		output.append(get_popup_status((void *)modal_remove.get(), "modal"));
	if (modal_spell)
		output.append(get_popup_status((void *)modal_spell.get(), "modal"));
	if (modal_tithe)
		output.append(get_popup_status((void *)modal_tithe.get(), "modal"));
	if (modal_trade)
		output.append(get_popup_status((void *)modal_trade.get(), "modal"));
	if (modal_use)
		output.append(get_popup_status((void *)modal_use.get(), "modal"));

	if (notice_cannot_donate)
		output.append(
			get_popup_status((void *)notice_cannot_donate.get(), "dialog"));
	if (notice_divvy)
		output.append(get_popup_status((void *)notice_divvy.get(), "dialog"));
	if (notice_donated_ok)
		output.append(
			get_popup_status((void *)notice_donated_ok.get(), "dialog"));
	if (notice_not_enough_gold)
		output.append(
			get_popup_status((void *)notice_not_enough_gold.get(), "dialog"));
	if (notice_pool_gold)
		output.append(
			get_popup_status((void *)notice_pool_gold.get(), "dialog"));
	if (notice_renamed_ok)
		output.append(
			get_popup_status((void *)notice_renamed_ok.get(), "dialog"));
	if (notice_reclassed_ok)
		output.append(
			get_popup_status((void *)notice_reclassed_ok.get(), "dialog"));

	return output;
}

auto Sorcery::UI::start() -> void {

	DEBUG_LOG("Starting UI...");

	// Initialise ImGUI to use SDL2/OpenGL
	ImGui::CreateContext();
	_io = &ImGui::GetIO();

	_imgui_ini_path = _ctx.get_file(IMGUI_INI_FILE).string();
	_io->IniFilename = CSTR(_imgui_ini_path);

	DEBUG_LOGF("ImGui ini: {}", _imgui_ini_path);

	ImGui::StyleColorsClassic();
	ImGui_ImplSDL2_InitForOpenGL(_ctx.display->get_SDL_window(),
								 _ctx.display->get_GL_context());
	ImGui_ImplOpenGL3_Init(_ctx.display->get_GLSL_version());

	if (_ctx.get_config(Enums::Config::FULLSCREEN))
		set_fullscreen(true);
	else
		set_fullscreen(false);

	// Can create the fontstore now which loads the fonts
	fontstore = std::make_unique<FontStore>(_ctx, _io);
	ui_colour = ImVec4{std::stof(_ctx.get_config("Frame", "colour_red")),
					   std::stof(_ctx.get_config("Frame", "colour_green")),
					   std::stof(_ctx.get_config("Frame", "colour_blue")), 1.0};

	// Set the Default Fonts
	using enum Enums::Layout::Font;
	fontstore->set_current_font(TEXT, _ctx.get_config("Font", "text"));
	fontstore->set_current_font(DEFAULT, fontstore->get_default_font());
	fontstore->set_current_font(MONOSPACE,
								_ctx.get_config("Font", "monospace"));
	fontstore->set_current_font(PROPORTIONAL,
								_ctx.get_config("Font", "proportional"));

	// Set the styles
	ImGuiStyle &style = ImGui::GetStyle();
	style.WindowRounding = ui_rd;
	style.ChildRounding = ui_rd;
	style.FrameRounding = ui_rd;
	style.PopupRounding = ui_rd;
	style.TabRounding = ui_rd;
	style.ChildBorderSize = ui_rd;

	selected.clear();
	highlighted.clear();

	selected["main_menu"] = 0;
	highlighted["main_menu"] = 0;
	selected["compendium_menu"] = 0;
	highlighted["compendium_menu"] = 0;

	selected["bestiary"] = 0;
	highlighted["bestiary"] = 0;
	selected["museum"] = 0;
	highlighted["museum"] = 0;
	selected["spellbook"] = 0;
	highlighted["spellbook"] = 0;
	selected["atlas"] = 0;
	highlighted["atlas"] = 0;
	selected["class_menu"] = 8;
	highlighted["class_menu"] = 8;

	ms_selected.fill(false);
	ps_selected.fill(false);

	dialog_exit->show = false;
	dialog_new->show = false;
	dialog_leave->show = false;
	dialog_rite->show = false;
	dialog_search->show = false;
	dialog_delete->show = false;
	notice_divvy->show = false;
	notice_donated_ok->show = false;
	notice_cannot_donate->show = false;
	notice_not_enough_gold->show = false;
	notice_pool_gold->show = false;
	notice_renamed_ok->show = false;
	notice_reclassed_ok->show = false;
	input_donate->show = false;
	input_name->show = false;
	modal_camp->show = false;
	modal_equip->show = false;
	modal_remove->show = false;
	modal_identify->show = false;
	modal_chest->show = false;
	modal_drop->show = false;
	modal_elevator_bottom->show = false;
	modal_elevator_top->show = false;
	modal_trade->show = false;
	modal_give->show = false;
	modal_use->show = false;
	modal_invoke->show = false;
	modal_spell->show = false;
	dialog_stairs_up->show = false;
	dialog_stairs_down->show = false;
	message_tile->show = false;

	_attract_data.clear();
}

auto Sorcery::UI::stop() -> void {

	DEBUG_LOG("Stopping UI...");

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
	SDL_GL_DeleteContext(_ctx.display->get_GL_context());
	SDL_DestroyWindow(_ctx.display->get_SDL_window());
	SDL_Quit();
}

auto Sorcery::UI::display_refresh(std::any payload) -> void {

	// Refresh what we previously drew
	display(_ctx.controller->get_last_screen(), payload);
}

auto Sorcery::UI::display_engine() -> void {

	// Start a new Rendering Frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	_setup_windows();
	//_draw_window_menu();

	// Background
	_draw_components("engine_base_ui");

	if (!_ctx.controller->get_monochrome()) {
		auto bg_c{components->get("engine_base_ui:background_image")};
		_draw_tiled_bg(&bg_c);
	}

	dialog_leave->display(_ctx.controller->want_to_leave_game());
	dialog_stairs_up->display(_ctx.get_flag_ref("want_take_stairs_up"));
	dialog_stairs_down->display(_ctx.get_flag_ref("want_take_stairs_down"));
	message_tile->display(_ctx.get_flag_ref("after_tile_message"));
	if (modal_camp->show)
		modal_camp->display(_ctx.get_flag_ref("want_camp"));
	if (modal_elevator_top->show)
		modal_elevator_top->display(_ctx.get_flag_ref("want_elevator_top"));
	if (modal_elevator_bottom->show)
		modal_elevator_bottom->display(
			_ctx.get_flag_ref("want_elevator_bottom"));
	if (modal_inspect->show)
		modal_inspect->display(_ctx.get_flag_ref("want_inspect"));
	if (dialog_search->show)
		dialog_search->display(_ctx.get_flag_ref("want_search"));
	if (modal_identify->show)
		modal_identify->display(_ctx.get_flag_ref("want_identify"));
	if (modal_chest->show)
		modal_chest->display(_ctx.get_flag_ref("want_chest"));
	if (modal_equip->show)
		modal_equip->display(_ctx.get_flag_ref("want_equip"));
	if (modal_remove->show)
		modal_remove->display(_ctx.get_flag_ref("want_remove"));
	if (modal_drop->show)
		modal_drop->display(_ctx.get_flag_ref("want_drop"));
	if (modal_trade->show)
		modal_trade->display(_ctx.get_flag_ref("want_trade"));
	if (modal_give->show)
		modal_give->display(_ctx.get_flag_ref("want_give"));
	if (modal_use->show)
		modal_use->display(_ctx.get_flag_ref("want_use"));
	if (modal_invoke->show)
		modal_invoke->display(_ctx.get_flag_ref("want_invoke"));
	if (modal_spell->show)
		modal_spell->display(_ctx.get_flag_ref("want_spell"));
	if (notice_pool_gold->show)
		notice_pool_gold->display(_ctx.get_flag_ref("want_pool_gold"));
	if (_ctx.get_flag("interface_ui") && _ctx.get_flag("interface_party_panel"))
		_draw_party_panel();
	if (_ctx.get_flag("interface_ui")) {
		_draw_compass();
		_draw_buffbar();
		_draw_level_name();
		_draw_icons();
		_draw_save();
	}

	// Dungeon View
	auto component{components->get("engine_base_ui:wire_frame_view")};
	_render->draw(&component);

	// Transient overlay
	_draw_transient();

	// And Cursor on Top
	_draw_debug();
	_draw_ui_status();
	_draw_cursor();

	// bool show{true};
	// ImGui::PushFont(fontstore->get_default_font());
	// ImGui::ShowDemoWindow(&show);
	// ImGui::PopFont();

	ImGui::Render();

	_ctx.display->present(ImGui::GetDrawData());
}

auto Sorcery::UI::display(Enums::Screen screen, std::any payload) -> void {

	// Store what we want to draw for next refresh
	_ctx.controller->set_last_screen(screen);

	// Start a new Rendering Frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	_setup_windows();

	if (payload.type() == typeid(std::string)) {
		if (auto it = _draw_modules_with_string.find(screen);
			it != _draw_modules_with_string.end())
			it->second(std::any_cast<std::string>(payload));
	} else if (payload.type() == typeid(int)) {
		if (auto it = _draw_modules_with_int.find(screen);
			it != _draw_modules_with_int.end())
			it->second(std::any_cast<int>(payload));
	} else {
		if (auto it = _draw_modules.find(screen); it != _draw_modules.end())
			it->second();
	}

	_draw_cursor();

	ImGui::Render();

	_ctx.display->present(ImGui::GetDrawData());
}

// Preset all the (transparent) windows we will need (this should be called
// before anything else is drawn to the screen during each render)
auto Sorcery::UI::_setup_windows() -> void {

	const std::vector<std::string> windows{
		WINDOW_LAYER_BG,	 WINDOW_LAYER_FRAMES, WINDOW_LAYER_VIEW,
		WINDOW_LAYER_IMAGES, WINDOW_LAYER_TEXTS,  WINDOW_LAYER_MENUS};

	const auto viewport{ImGui::GetMainViewport()};
	for (const auto &window : windows) {

		ImGui::SetNextWindowPos(ImVec2{0, 0});
		ImGui::SetNextWindowSize(viewport->Size);
		auto alpha{window != WINDOW_LAYER_BG ? 0.0 : 1.0f};
		ImGui::SetNextWindowBgAlpha(alpha);
		set_StyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
		set_StyleVar(ImGuiStyleVar_WindowBorderSize, 0);
		set_StyleVar(ImGuiStyleVar_WindowRounding, 0);
		auto flags{window == WINDOW_LAYER_MENUS
					   ? ImGuiWindowFlags_NoDecoration
					   : ImGuiWindowFlags_NoDecoration |
							 ImGuiWindowFlags_NoInputs};
		with_Window(window.c_str(), nullptr, flags){};
	}
}

// Colour Gradient Helper function
auto Sorcery::UI::get_hl_colour(const double percent) const -> ImColor {

	const auto first{ImVec4{0xbf, 0xbf, 0xff, _ctx.animation->fade}};
	const auto second{ImVec4{0x00, 0x00, 0x3f, _ctx.animation->fade}};

	const auto red{static_cast<int>(first.x + percent * (second.x - first.x))};
	const auto green{
		static_cast<int>(first.y + percent * (second.y - first.y))};
	const auto blue{static_cast<int>(first.z + percent * (second.z - first.z))};

	return ImColor{red, green, blue};
}

// Colour Gradient Helper function
auto Sorcery::UI::lerp_colour(const ImVec4 col_from, const ImVec4 col_to,
							  const double percent) const -> ImVec4 {

	const auto first{
		ImVec4{col_from.x, col_from.y, col_from.z, _ctx.animation->fade}};
	const auto second{
		ImVec4{col_to.x, col_to.y, col_to.z, _ctx.animation->fade}};

	const auto red{static_cast<int>(first.x + percent * (second.x - first.x))};
	const auto green{
		static_cast<int>(first.y + percent * (second.y - first.y))};
	const auto blue{static_cast<int>(first.z + percent * (second.z - first.z))};

	return ImVec4{red, green, blue, 1.0f};
}

// Wrapper for next function
auto Sorcery::UI::draw_image(std::string_view source, const int idx,
							 const ImVec2 p_min, const ImVec2 p_sz) -> void {

	_draw_fg_image_with_idx(source, idx, p_min, p_sz);
}

auto Sorcery::UI::draw_view_image(std::string_view source,
								  const VertexArray &array) -> void {

	with_Window(WINDOW_LAYER_VIEW, nullptr,
				ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs) {

		// Load the image if necessary
		if (!images->has_loaded(std::string{source}))
			images->load_image(std::string{source});

		ImDrawList *draw_list{ImGui::GetWindowDrawList()};
		auto old_flags{draw_list->Flags};
		draw_list->Flags = ImDrawListFlags_None;
		auto src_image{images->get(std::string{source})};
		draw_list->AddImageQuad(
			src_image.texture,
			ImVec2{array.data[0].position.x, array.data[0].position.y},
			ImVec2{array.data[1].position.x, array.data[1].position.y},
			ImVec2{array.data[2].position.x, array.data[2].position.y},
			ImVec2{array.data[3].position.x, array.data[3].position.y},
			ImVec2{array.data[0].tex_coord.x, array.data[0].tex_coord.y},
			ImVec2{array.data[1].tex_coord.x, array.data[1].tex_coord.y},
			ImVec2{array.data[2].tex_coord.x, array.data[2].tex_coord.y},
			ImVec2{array.data[3].tex_coord.x, array.data[3].tex_coord.y},
			ImGui::ColorConvertFloat4ToU32(array.data[0].colour));
		draw_list->Flags = old_flags;
	}
}

// Handle drawing parts of a texture as specified by a tile index
auto Sorcery::UI::_draw_fg_image_with_idx(std::string_view layer,
										  std::string_view source,
										  const int idx, const ImVec2 p_min,
										  const ImVec2 p_sz, const ImVec4 tint)
	-> void {

	if (!images->show_images) {

		// If we aren't drawing images, draw a suitable placeholder
		with_Window(std::string(layer).c_str(), nullptr,
					ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs) {
			ImGui::SetCursorPos(p_min);
			ImGui::GetWindowDrawList()->AddRectFilled(
				p_min, ImVec2(p_min.x + p_sz.x, p_min.y + p_sz.y),
				ImColor{ImVec4{0.5f, 0.5f, 0.5f, _ctx.animation->fade}});
		}

		return;
	}

	auto tile_row_count{0};
	auto tile_size{0};
	auto image_size{ImVec2{0, 0}};
	auto src_image{images->get(std::string{source})};
	if (source == KNOWN_CREATURES_TEXTURE ||
		source == UNKNOWN_CREATURES_TEXTURE) {
		tile_row_count = CREATURE_TILE_ROW_COUNT;
		tile_size = src_image.width / CREATURE_TILE_ROW_COUNT;
		image_size.x = src_image.width;
		image_size.y = src_image.height;
	} else if (source == ITEMS_TEXTURE) {
		tile_row_count = ITEM_TILE_ROW_COUNT;
		tile_size = src_image.width / ITEM_TILE_ROW_COUNT;
		image_size.x = src_image.width;
		image_size.y = src_image.height;
	} else if (source == BACKGROUNDS_TEXTURE) {
		tile_row_count = BACKGROUNDS_TILE_ROW_COUNT;
		tile_size = src_image.width / BACKGROUNDS_TILE_ROW_COUNT;
		image_size.x = src_image.width;
		image_size.y = src_image.height;
	} else if (source == MAPS_TEXTURE) {
		tile_row_count = MAP_TILE_ROW_COUNT;
		tile_size = src_image.width / MAP_TILE_ROW_COUNT;
		image_size.x = src_image.width;
		image_size.y = src_image.height;
	} else if (source == ICONS_TEXTURE) {
		tile_row_count = ICONS_TILE_ROW_COUNT;
		tile_size = src_image.width / ICONS_TILE_ROW_COUNT;
		image_size.x = src_image.width;
		image_size.y = src_image.height;
	} else if (source == EVENTS_TEXTURE) {
		tile_row_count = EVENTS_TILE_ROW_COUNT;
		tile_size = src_image.width / EVENTS_TILE_ROW_COUNT;
		image_size.x = src_image.width;
		image_size.y = src_image.height;
	}

	// Work out the source rect to copy
	const auto from{ImVec4{tile_size * (idx % tile_row_count),
						   tile_size * (idx / tile_row_count), tile_size,
						   tile_size}};
	const auto uv_0{ImVec2{from.x / image_size.x, from.y / image_size.y}};
	const auto uv_1{ImVec2{(from.x + tile_size) / image_size.x,
						   (from.y + tile_size) / image_size.y}};

	with_Window(std::string(layer).c_str(), nullptr,
				ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs) {
		ImGui::SetCursorPos(ImVec2{p_min});
		auto src_image{images->get(std::string{source})};
		ImVec4 tint_col{ImVec4(tint.x, tint.y, tint.z, _ctx.animation->fade)};
		ImGui::ImageWithBg(ImTextureRef(_to_imgui(src_image.texture)), p_sz,
						   uv_0, uv_1, ImVec4(0.0f, 0.0f, 0.0f, 0.0f),
						   tint_col);
	}
}

// Handle drawing parts of a texture as specified by a tile index
auto Sorcery::UI::_draw_fg_image_with_idx(std::string_view source,
										  const int idx, const ImVec2 p_min,
										  const ImVec2 p_sz, const ImVec4 tint)
	-> void {

	_draw_fg_image_with_idx(WINDOW_LAYER_IMAGES, source, idx, p_min, p_sz,
							tint);
}

auto Sorcery::UI::_draw_fg_image(Component *component) -> void {

	if (!images->show_images) {

		with_Window(WINDOW_LAYER_IMAGES, nullptr,
					ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs) {

			const auto x{std::invoke([&] {
				if (component->x == -1) {
					const auto viewport{ImGui::GetMainViewport()};
					return (viewport->Size.x - 200) / 2;
				} else
					return grid_pos(component->x, component->y).x;
			})};
			const auto y{std::invoke([&] {
				if (component->y == -1) {
					const auto viewport{ImGui::GetMainViewport()};
					return (viewport->Size.y - 200) / 2;
				} else
					return grid_pos(component->x, component->y).y;
			})};

			ImGui::SetCursorPos(grid_pos(x, y));
			const auto scaling{_ctx.display->get_display_metrics().scale};
			ImGui::GetWindowDrawList()->AddRectFilled(
				grid_pos(x, y), grid_pos(x + 200 * scaling, y + 200 * scaling),
				ImColor{ImVec4{0.5f, 0.5f, 0.5f, _ctx.animation->fade}});
		}

		return;
	}

	if (component->get("source")) {
		const auto source{component->get("source").value()};
		const auto scale{component->get_float("scale")};

		// Load the image if necessary
		if (!images->has_loaded(source))
			images->load_image(source);

		// Work out any scaling if needed
		const auto scaling{_ctx.display->get_display_metrics().scale};
		auto src_image{images->get(source)};
		const auto resized{Size{(intptr_t)src_image.width * scale * scaling,
								(intptr_t)src_image.height * scale * scaling}};

		// Work out where to draw the image
		const auto x{std::invoke([&] {
			if (component->x == -1) {
				const auto viewport{ImGui::GetMainViewport()};
				return (viewport->Size.x - resized.w) / 2;
			} else
				return grid_pos(component->x, component->y).x;
		})};
		const auto y{std::invoke([&] {
			if (component->y == -1) {
				const auto viewport{ImGui::GetMainViewport()};
				return (viewport->Size.y - resized.h) / 2;
			} else
				return grid_pos(component->x, component->y).y;
		})};

		// Draw the Image (with Alpha as well as Fade!)
		with_Window(WINDOW_LAYER_IMAGES, nullptr,
					ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs) {
			ImGui::SetCursorPos(ImVec2{x, y});
			ImVec4 tint_col{ImVec4(1.0f, 1.0f, 1.0f,
								   component->alpha * _ctx.animation->fade)};
			ImGui::ImageWithBg(_to_imgui(src_image.texture),
							   ImVec2{static_cast<float>(resized.w),
									  static_cast<float>(resized.h)},
							   ImVec2{0.0f, 0.0f}, ImVec2{1.0f, 1.0f},
							   ImVec4(0.0f, 0.0f, 0.0f, 0.0f), tint_col);
		}
	}
}

auto Sorcery::UI::_draw_tiled_bg(Component *component) -> void {

	if (!images->show_images) {

		// If we aren't drawing images, draw a suitable placeholder
		with_Window(WINDOW_LAYER_BG, nullptr,
					ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs) {
			const auto viewport{ImGui::GetMainViewport()};
			ImGui::SetCursorPos(ImVec2{0, 0});
			ImGui::GetWindowDrawList()->AddRectFilled(
				ImVec2{0, 0}, viewport->Size,
				ImColor{ImVec4{0.2f, 0.2f, 0.2f, _ctx.animation->fade}});
		}
	}

	with_Window(WINDOW_LAYER_BG, nullptr,
				ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs) {

		auto src_image{images->get(std::string{BACKGROUNDS_TEXTURE})};

		constexpr float TILE_SIZE{400.0f};

		const int tiles_per_row{src_image.width / static_cast<int>(TILE_SIZE)};

		const int idx{_ctx.animation->wp_idx};

		const int tile_x{idx % tiles_per_row};
		const int tile_y{idx / tiles_per_row};

		// UV coordinates into atlas
		const float u0{(tile_x * TILE_SIZE) / src_image.width};
		const float v0{(tile_y * TILE_SIZE) / src_image.height};

		const float u1{((tile_x + 1) * TILE_SIZE) / src_image.width};
		const float v1{((tile_y + 1) * TILE_SIZE) / src_image.height};

		const auto viewport{ImGui::GetMainViewport()};
		auto *draw_list{ImGui::GetWindowDrawList()};

		for (float y = 0; y < viewport->Size.y; y += TILE_SIZE) {

			for (float x = 0; x < viewport->Size.x; x += TILE_SIZE) {

				draw_list->AddImage((ImTextureID)(intptr_t)src_image.texture,
									ImVec2{x, y},
									ImVec2{x + TILE_SIZE, y + TILE_SIZE},
									ImVec2{u0, v0}, ImVec2{u1, v1});
			}
		}
	}
};

auto Sorcery::UI::_draw_bg_image(Component *component) -> void {

	if (!images->show_images) {

		// If we aren't drawing images, draw a suitable placeholder
		with_Window(WINDOW_LAYER_BG, nullptr,
					ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs) {
			const auto viewport{ImGui::GetMainViewport()};
			ImGui::SetCursorPos(ImVec2{0, 0});
			ImGui::GetWindowDrawList()->AddRectFilled(
				ImVec2{0, 0}, viewport->Size,
				ImColor{ImVec4{0.2f, 0.2f, 0.2f, _ctx.animation->fade}});
		}
	}

	if (component->get("source")) {

		// Load the image if necessary
		const auto source{component->get("source").value()};
		if (!images->has_loaded(source))
			images->load_image(source);

		const auto viewport{ImGui::GetMainViewport()};
		auto src_image{images->get(source)};
		const auto size{
			Size((intptr_t)src_image.width, (intptr_t)src_image.height)};

		// Draw the Image
		with_Window(WINDOW_LAYER_BG, nullptr,
					ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs) {
			ImGui::SetCursorPos(ImVec2{0, 0});
			ImGui::Image((intptr_t)src_image.texture, viewport->Size,
						 ImVec2{0.0f, 0.0f}, ImVec2{1.0f, 1.0f});
		}
	}
}

auto Sorcery::UI::draw_cursor(const bool value) -> void {

	_ctx.controller->set_busy(value);
	_draw_cursor();
}

auto Sorcery::UI::draw_ui_status() -> void {

	_draw_ui_status();
}

auto Sorcery::UI::_draw_ui_status() -> void {

	constexpr auto ICON_CGA_ON{108u};
	constexpr auto ICON_CGA_OFF{109u};

	if (images->has_loaded(std::string{ICONS_TEXTURE})) {

		const auto music_status{_ctx.get_config(Enums::Config::MUSIC)};
		const auto sound_status{_ctx.get_config(Enums::Config::SOUND)};
		const auto cga_status{!_render->get_monochrome()};
		const auto music_icon{music_status ? ICON_MUSIC_ON : ICON_MUSIC_OFF};
		const auto sound_icon{sound_status ? ICON_SOUND_ON : ICON_SOUND_OFF};
		const auto cga_icon{cga_status ? ICON_CGA_ON : ICON_CGA_OFF};

		const auto tint{_ctx.controller->get_monochrome()
							? ImVec4{1.0f, 1.0f, 1.0f, _ctx.animation->fade}
							: ImVec4{1.0f, 0.0f, 1.0f, _ctx.animation->fade}};

		const auto scale{_ctx.display->get_display_metrics().scale};
		auto pos{ImVec2{4 * scale, 4 * scale}};
		auto size{ImVec2{32 * scale, 32 * scale}};

		with_Window(WINDOW_LAYER_TEXTS, nullptr,
					ImGuiWindowFlags_NoDecoration) {

			_draw_fg_image_with_idx(WINDOW_LAYER_TEXTS, ICONS_TEXTURE,
									music_icon, pos, size, tint);
			pos.x += 32 * scale;
			_draw_fg_image_with_idx(WINDOW_LAYER_TEXTS, ICONS_TEXTURE,
									sound_icon, pos, size, tint);

			pos.x += 32 * scale;
			_draw_fg_image_with_idx(WINDOW_LAYER_TEXTS, ICONS_TEXTURE, cga_icon,
									pos, size, tint);
		}
	};
};

auto Sorcery::UI::_draw_cursor() -> void {

	// Just check if we can draw
	if (!images->has_loaded(std::string{ICONS_TEXTURE})) {
		ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
		return;
	} else
		ImGui::SetMouseCursor(ImGuiMouseCursor_None);

	// Get the current mouse position
	const auto pos{ImVec2{_io->MousePos.x, _io->MousePos.y}};
	if (ImGui::IsMousePosValid()) {

		// Grab and extract the image manually and add to the foreground
		// list (we don't use _draw_fg_image_with_idx() since that draws to
		// one of the standard layers, whereas we want the cursor to be
		// always visible no matter what)

		// Work out what cursor to draw
		auto src_image{images->get(std::string{ICONS_TEXTURE})};
		const auto icon_sz{src_image.width / ICONS_TILE_ROW_COUNT};
		const auto texture_sz{ImVec2{src_image.width, src_image.height}};
		const auto scale{_ctx.display->get_display_metrics().scale};
		const auto dest_sz{ImVec2{32 * scale, 32 * scale}};
		const auto cursor_idx{_ctx.controller->get_busy() ? ICON_HOURGLASS
														  : ICON_CURSOR};
		const auto cursor_col{_ctx.controller->get_busy()
								  ? lerp_colour(ImVec4{1.0f, 0.0f, 0.0f, 1.0f},
												ImVec4{1.0f, 0.8f, 0.8f, 1.0f},
												_ctx.animation->lerp)
								  : lerp_colour(ImVec4{0.0f, 1.0f, 0.0f, 1.0f},
												ImVec4{0.8f, 1.0f, 0.8f, 1.0f},
												_ctx.animation->lerp)};

		// Work out the source rect to copy (normalise to 0.0f - 1.0f)
		const auto from{ImVec4{icon_sz * (cursor_idx % ICONS_TILE_ROW_COUNT),
							   icon_sz * (cursor_idx / ICONS_TILE_ROW_COUNT),
							   icon_sz, icon_sz}};
		const auto uv_0{ImVec2{from.x / texture_sz.x, from.y / texture_sz.y}};
		const auto uv_1{ImVec2{(from.x + icon_sz) / texture_sz.x,
							   (from.y + icon_sz) / texture_sz.y}};

		ImGui::GetForegroundDrawList()->AddImage(
			(intptr_t)src_image.texture, ImVec2{pos.x, pos.y},
			ImVec2{pos.x + dest_sz.x, pos.y + dest_sz.y}, uv_0, uv_1,
			ImGui::ColorConvertFloat4ToU32(cursor_col));
	}
}

// Draw a Frame
auto Sorcery::UI::_draw_frame(Component *component) -> void {

	// Note the Frame class calls Gui::->draw_frame() below
	auto frame{std::make_shared<Frame>(_ctx, component)};
	_frames.emplace_back(std::move(frame));
}

// Draw a Menu
auto Sorcery::UI::_draw_menu(Component *component) -> void {

	auto menu{std::make_shared<Menu>(_ctx, component, _ctx.game)};
	menu->regenerate();
	menu->draw();
	_menus.emplace_back(std::move(menu));
}

auto Sorcery::UI::font_sz() const noexcept -> float {

	return _font_sz;
}

auto Sorcery::UI::base_font_sz() const noexcept -> float {

	return _base_font_sz;
}

auto Sorcery::UI::_draw_debug() -> void {

	if (!_ctx.controller->get_flag("debug_ui"))
		return;

	with_Window(WINDOW_LAYER_MENUS, nullptr,
				ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs) {

		set_Font(_io->FontDefault, font_sz());
		ImGui::SetCursorPos(ImVec2{8, 8});
		set_StyleColor(ImGuiCol_Text, ImVec4{1.0f, 0.0f, 0.0f, 1.0f});
		ImGui::TextUnformatted(_ctx.controller->get_flags().c_str());

		ImGui::SetCursorPos(ImVec2{1000, 400});
		ImGui::TextUnformatted(_get_popups().c_str());

		ImGui::SetCursorPos(ImVec2{8, 700});
		ImGui::TextUnformatted(_ctx.controller->get_characters().c_str());

		ImGui::SetCursorPos(ImVec2{1000, 8});
		auto fonts{fontstore->get_all_fonts()};
		for (const auto &font : fonts) {
			ImGui::TextUnformatted(font.name.c_str());
			ImGui::SetCursorPosX(1000);
		}
	}
}

// Draw a Paragraph (Wrapped Multiline Text)
auto Sorcery::UI::_draw_paragraph(Component *component) -> void {

	with_Window(WINDOW_LAYER_TEXTS, nullptr,
				ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs) {

		set_Font(fontstore->get_current_font(component->font).value(),
				 font_sz());
		const auto wrap{component->get_float("width") * font_sz()};
		auto p_min{grid_pos(component->x, component->y)};

		ImGui::SetCursorPos(p_min);
		with_TextWrapPos(p_min.x + wrap) {
			set_StyleColor(ImGuiCol_Text, component->colour);
			ImGui::TextUnformatted(
				_ctx.get_string(component->string_key).c_str());
		}
	}
}

auto Sorcery::UI::draw_text_with_layer(const std::string string,
									   const ImColor colour, const ImVec2 pos,
									   const Enums::Layout::Font font) -> void {

	with_Window(WINDOW_LAYER_TEXTS, nullptr,
				ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs |
					ImGuiWindowFlags_NoBackground) {

		set_Font(fontstore->get_current_font(font).value(), font_sz());

		const auto x{std::invoke([&] {
			if (pos.x == -1) {
				const auto viewport{ImGui::GetMainViewport()};
				const auto width{ImGui::CalcTextSize(string.c_str())};
				return (viewport->Size.x - width.x) / 2;
			} else
				return static_cast<float>(pos.x);
		})};
		const auto y{std::invoke([&] {
			if (pos.y == -1) {
				const auto viewport{ImGui::GetMainViewport()};
				const auto height{ImGui::CalcTextSize(string.c_str())};
				return (viewport->Size.y - height.y) / 2;
			} else
				return static_cast<float>(pos.y);
		})};

		set_StyleColor(ImGuiCol_Text, ImVec4{colour});
		ImGui::SetCursorScreenPos(ImVec2{x, y});
		ImGui::TextUnformatted(string.c_str());
	}
}

auto Sorcery::UI::draw_text(const std::string string, const ImColor colour,
							const ImVec2 pos, const Enums::Layout::Font font)
	-> void {

	set_Font(fontstore->get_current_font(font).value(), font_sz());

	const auto x{std::invoke([&] {
		if (pos.x == -1) {
			const auto viewport{ImGui::GetMainViewport()};
			const auto width{ImGui::CalcTextSize(string.c_str())};
			return (viewport->Size.x - width.x) / 2;
		} else
			return static_cast<float>(pos.x);
	})};
	const auto y{std::invoke([&] {
		if (pos.y == -1) {
			const auto viewport{ImGui::GetMainViewport()};
			const auto height{ImGui::CalcTextSize(string.c_str())};
			return (viewport->Size.y - height.y) / 2;
		} else
			return static_cast<float>(pos.y);
	})};

	set_StyleColor(ImGuiCol_Text, ImVec4{colour});
	ImGui::SetCursorPos(ImVec2{x, y});
	ImGui::TextUnformatted(string.c_str());
}

auto Sorcery::UI::_draw_button_click(Component *component, bool &flag,
									 const bool reverse) -> void {

	// Need to push font first before calculating size else it will
	// assume monospace font size!
	set_Font(fontstore->get_current_font(component->font).value(), font_sz());
	const auto name{component->name};
	const auto col{get_hl_colour(_ctx.animation->lerp)};
	auto x{std::invoke([&] {
		if (component->x == -1) {
			const auto viewport{ImGui::GetMainViewport()};
			const auto width{ImGui::CalcTextSize(
				CSTR(_ctx.get_string(component->string_key)))};
			return (viewport->Size.x - width.x) / 2;
		} else
			return grid_pos(component->x, component->y).x;
	})};
	auto y{std::invoke([&] {
		if (component->y == -1) {
			const auto viewport{ImGui::GetMainViewport()};
			const auto height{ImGui::CalcTextSize(
				CSTR(_ctx.get_string(component->string_key)))};
			return (viewport->Size.y - height.y) / 2;
		} else
			return grid_pos(component->x, component->y).y;
	})};

	if (component->get("adjust_x"))
		x += component->get_float("adjust_x");
	if (component->get("adjust_y"))
		y += component->get_float("adjust_y");
	UIStyle::set_faded(_ctx);
	set_StyleColor(ImGuiCol_ButtonHovered, (ImVec4)col);
	ImGui::SetCursorPos(ImVec2{x, y});
	with_ID(name.c_str()) {
		if (ImGui::Button(CSTR(_ctx.get_string(component->string_key)))) {
			// Handle buttons being used to switch on AND off the flag
			flag = !reverse;
			_ctx.controller->handle_button_click(component->name, this, -1);
		}
	}
}

// Draw a Button
auto Sorcery::UI::_draw_button(Component *component,
							   std::optional<bool *> is_clicked) -> void {

	with_Window(WINDOW_LAYER_MENUS, nullptr, ImGuiWindowFlags_NoDecoration) {

		// Need to push font first before calculating size else it will
		// assume monospace font size!
		set_Font(fontstore->get_current_font(component->font).value(),
				 font_sz());
		const auto name{component->name};
		const auto col{get_hl_colour(_ctx.animation->lerp)};
		auto x{std::invoke([&] {
			if (component->x == -1) {
				const auto viewport{ImGui::GetMainViewport()};
				const auto width{ImGui::CalcTextSize(
					CSTR(_ctx.get_string(component->string_key)))};
				return (viewport->Size.x - width.x) / 2;
			} else
				return grid_pos(component->x, component->y).x;
		})};
		auto y{std::invoke([&] {
			if (component->y == -1) {
				const auto viewport{ImGui::GetMainViewport()};
				const auto height{ImGui::CalcTextSize(
					CSTR(_ctx.get_string(component->string_key)))};
				return (viewport->Size.y - height.y) / 2;
			} else
				return grid_pos(component->x, component->y).y;
		})};
		if (component->get("adjust_x"))
			x += component->get_float("adjust_x");
		if (component->get("adjust_y"))
			y += component->get_float("adjust_y");

		UIStyle::set_faded(_ctx);
		set_StyleColor(ImGuiCol_ButtonHovered, (ImVec4)col);
		set_StyleColor(ImGuiCol_ButtonActive, (ImVec4)col);
		ImGui::SetCursorPos(ImVec2{x, y});
		with_ID(name.c_str()) {
			if (ImGui::Button(CSTR(_ctx.get_string(component->string_key)))) {
				if (is_clicked)
					*is_clicked.value() = true;

				_ctx.controller->handle_button_click(component->name, this, -1);

				if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNone)) {
				}
			}
		}
	}
}

auto Sorcery::UI::_draw_character_detailed(Component *component,
										   const Character *character) -> void {

	const auto left_col{component->x + 0};
	const auto right_col{component->x + 19};

	using enum Enums::Character::Ability;
	using enum Enums::Character::Attribute;
	UIStyle::set_text_bright(_ctx);
	auto pos{grid_pos(left_col, component->y)};
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(std::format("{:>14} {:>2}", "Strength",
									   character->get_cur_attr(STRENGTH))
							   .c_str());

	UIStyle::set_text_dark(_ctx);
	pos.y += grid_delta(0, 1).y;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:+>2}", "Atk Mod",
					character->abilities().at(ATTACK_MODIFIER))
			.c_str());
	pos.y += grid_delta(0, 1).y;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:+>2}", "Hit Prob",
					character->abilities().at(HIT_PROBABILITY))
			.c_str());
	pos.y += grid_delta(0, 1).y;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(std::format("{:>14} {:+>2}", "Bonus Damg",
									   character->abilities().at(BONUS_DAMAGE))
							   .c_str());
	pos.y += grid_delta(0, 1).y;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}", "Num Attacks",
					character->abilities().at(BASE_NUMBER_OF_ATTACKS))
			.c_str());
	pos.y += grid_delta(0, 1).y;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}", "Unarmed Damg",
					character->abilities().at(UNARMED_DAMAGE))
			.c_str());

	UIStyle::set_text_bright(_ctx);
	pos.y += grid_delta(0, 1).y;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(std::format("{:>14} {:>2}", "Vitality",
									   character->get_cur_attr(VITALITY))
							   .c_str());

	UIStyle::set_text_dark(_ctx);
	pos.y += grid_delta(0, 1).y;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:+>2}", "Vit Bonus",
					character->abilities().at(VITALITY_BONUS))
			.c_str());
	pos.y += grid_delta(0, 1).y;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:+>2}", "Bonus HP",
					character->abilities().at(BONUS_HIT_POINTS))
			.c_str());
	pos.y += grid_delta(0, 1).y;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}%", "Ress / Dead",
					character->abilities().at(DEAD_RESURRECT))
			.c_str());
	pos.y += grid_delta(0, 1).y;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}%", "Ress / Ashes",
					character->abilities().at(ASHES_RESURRECT))
			.c_str());
	pos.y += grid_delta(0, 1).y;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}%", "Ress / Spell",
					character->abilities().at(DI_KADORTO_RESURRECT))
			.c_str());

	pos = grid_pos(right_col, component->y);
	ImGui::SetCursorPos(pos);
	UIStyle::set_text_bright(_ctx);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}", "I.Q.", character->get_cur_attr(IQ))
			.c_str());

	UIStyle::set_text_dark(_ctx);
	pos.y += grid_delta(0, 1).y;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}%", "Spell Learn",
					character->abilities().at(MAGE_SPELL_LEARN))
			.c_str());
	pos.y += grid_delta(0, 1).y;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}%", "ID Items",
					character->abilities().at(IDENTIFY_ITEMS))
			.c_str());
	pos.y += grid_delta(0, 1).y;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}%", "ID Curse",
					character->abilities().at(IDENTIFY_CURSE))
			.c_str());
	pos.y += grid_delta(0, 1).y;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(std::format("{:>14} {:>2}%", "ID Foes",
									   character->abilities().at(IDENTIFY_FOES))
							   .c_str());

	UIStyle::set_text_bright(_ctx);
	pos.y += grid_delta(0, 2).y;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}", "Agility", character->get_cur_attr(AGILITY))
			.c_str());

	UIStyle::set_text_dark(_ctx);
	pos.y += grid_delta(0, 1).y;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:+>2}", "Int Mod",
					character->abilities().at(INITIATIVE_MODIFIER))
			.c_str());
	pos.y += grid_delta(0, 1).y;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}%", "Crit Hit",
					character->abilities().at(BASE_CRITICAL_HIT))
			.c_str());
	pos.y += grid_delta(0, 1).y;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(std::format("{:>14} {:>2}%", "ID Trap",
									   character->abilities().at(IDENTIFY_TRAP))
							   .c_str());
	pos.y += grid_delta(0, 1).y;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}%", "Disarm Trap",
					character->abilities().at(BASE_DISARM_TRAP))
			.c_str());
	pos.y += grid_delta(0, 1).y;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}%", "Avoid Trap",
					100 - character->abilities().at(ACTIVATE_TRAP))
			.c_str());
	pos.y += grid_delta(0, 1).y;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}%", "Avoid Pit",
					character->abilities().at(BASE_AVOID_PIT))
			.c_str());
}

auto Sorcery::UI::_draw_character_mage_spells(Component *component,
											  const Character *character)
	-> void {

	ImVec2 pos{grid_pos(component->x, component->y)};
	ImGui::SetCursorPos(pos);
	with_Table("mage_spells_1", 3, ImGuiTableFlags_NoSavedSettings) {

		UIStyle::set_faded_with_disabled(_ctx);

		// Row order since we need to do via rows not columns
		auto comp_id{""s};
		auto spell_idx{0};
		for (auto spell_order = {0,	 4,	 6,	 1,	 5,	 7,	 2,	 -1, -1, 3,	 -1,
								 -1, -1, -1, -1, 8,	 11, 14, 9,	 12, 15, 10,
								 13, 16, -1, -1, 17, -1, -1, -1, 18, 19, 20};
			 const int index : spell_order) {
			comp_id = std::format("##spell_{}", spell_idx);
			if (index != -1) {
				ImGui::TableNextColumn();

				auto spell_id{enum_cast<Enums::Magic::SpellID>(index).value()};
				auto spells{character->magic().get_spells() |
							std::views::filter([&](Spell spell) {
								return (spell.id == spell_id);
							})};
				auto spell{spells.begin()};

				if (!(*spell).known)
					ImGui::BeginDisabled();

				if (ImGui::Selectable(
						(*spell).name.c_str(),
						&ms_selected[_mage_spell_index(spell_id)])) {
				};

				if (!(*spell).known)
					ImGui::EndDisabled();

			} else {
				ImGui::TableNextColumn();
				ImGui::BeginDisabled();
				ImGui::Selectable(comp_id.c_str());
				ImGui::EndDisabled();
			}
			++spell_idx;
		}
	}
}

auto Sorcery::UI::_draw_character_priest_spells(Component *component,
												const Character *character)
	-> void {

	auto pos{grid_pos(component->x, component->y)};
	ImGui::SetCursorPos(pos);
	with_Table("priest_spells_1", 3, ImGuiTableFlags_NoSavedSettings) {

		UIStyle::set_faded_with_disabled(_ctx);

		// Row order since we need to do via rows not columns
		auto comp_id{""s};
		auto spell_idx{0};
		for (auto spell_order = {21, 26, 30, 22, 27, 31, 23, 28, 32, 24, 29,
								 33, 25, -1, -1, -1, -1, -1, 34, 38, 44, 35,
								 39, 45, 36, 40, 46, 37, 41, 47, -1, 42, -1,
								 -1, 43, -1, -1, -1, -1, 48, 49, -1};
			 const auto index : spell_order) {
			comp_id = std::format("##spell_{}", spell_idx);
			if (index != -1) {
				ImGui::TableNextColumn();

				auto spell_id{enum_cast<Enums::Magic::SpellID>(index).value()};
				auto spells{character->magic().get_spells() |
							std::views::filter([&](Spell spell) {
								return (spell.id == spell_id);
							})};
				auto spell{spells.begin()};

				if (!(*spell).known)
					ImGui::BeginDisabled();

				if (ImGui::Selectable(
						(*spell).name.c_str(),
						&ps_selected[_priest_spell_index(spell_id)])) {
				};

				if (!(*spell).known)
					ImGui::EndDisabled();

			} else {
				ImGui::TableNextColumn();
				ImGui::BeginDisabled();
				ImGui::Selectable(comp_id.c_str());
				ImGui::EndDisabled();
			}
			++spell_idx;
		}
	}
}

auto Sorcery::UI::_draw_character_detailed_again(Component *component,
												 const Character *character)
	-> void {

	const auto left_col{component->x + 0};
	const auto right_col{component->x + 19};

	using enum Enums::Character::Ability;
	using enum Enums::Character::Attribute;
	UIStyle::set_text_bright(_ctx);
	auto pos{grid_pos(left_col, component->y)};
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}", "Piety", character->get_cur_attr(PIETY))
			.c_str());

	UIStyle::set_text_dark(_ctx);
	pos.y += grid_delta(0, 1).y;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}%", "Spell Learn",
					character->abilities().at(PRIEST_SPELL_LEARN))
			.c_str());
	pos.y += grid_delta(0, 1).y;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}%", "Rec Chance",
					character->abilities().at(LOKTOFELT_SUCCESS))
			.c_str());
	pos.y += grid_delta(0, 1).y;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(std::format("{:>14} {:>2}%", "Base Dispell",
									   character->abilities().at(BASE_DISPELL))
							   .c_str());

	UIStyle::set_text_bright(_ctx);
	pos.y += grid_delta(0, 2).y;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}", "Luck", character->get_cur_attr(LUCK))
			.c_str());
	UIStyle::set_text_dark(_ctx);
	pos.y += grid_delta(0, 1).y;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}%", "Res Bonus",
					character->abilities().at(BASE_RESIST_BONUS))
			.c_str());
	pos.y += grid_delta(0, 1).y;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}%", "Wipe Rec",
					character->abilities().at(EQUIPMENT_INTACT_ON_WIPE))
			.c_str());

	pos = grid_pos(right_col, component->y);
	ImGui::SetCursorPos(pos);
	UIStyle::set_text_bright(_ctx);
	ImGui::TextUnformatted(std::format("{:>14}", "Resistances").c_str());

	UIStyle::set_text_dark(_ctx);
	pos.y += grid_delta(0, 1).y;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}%", "vs Crit Hit",
					character->abilities().at(RESISTANCE_VS_CRITICAL_HIT) * 5)
			.c_str());
	pos.y += grid_delta(0, 1).y;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}%", "vs Pois / Para",
					character->abilities().at(RESISTANCE_VS_POISON_PARALYSIS) *
						5)
			.c_str());
	pos.y += grid_delta(0, 1).y;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}%", "vs Stoning",
					character->abilities().at(RESISTANCE_VS_STONING) * 5)
			.c_str());
	pos.y += grid_delta(0, 1).y;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}%", "vs Breath",
					character->abilities().at(RESISTANCE_VS_BREATH_ATTACKS) * 5)
			.c_str());
	pos.y += grid_delta(0, 1).y;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}%", "vs Gas Trap",
					character->abilities().at(RESISTANCE_VS_POISON_GAS_TRAP) *
						5)
			.c_str());
	pos.y += grid_delta(0, 1).y;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}%", "vs Spell Trap",
					character->abilities().at(RESISTANCE_VS_MAGE_PRIEST_TRAP) *
						5)
			.c_str());
	pos.y += grid_delta(0, 1).y;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}%", "vs Silence",
					character->abilities().at(RESISTANCE_VS_SILENCE) * 5)
			.c_str());
	pos.y += grid_delta(0, 1).y;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}%", "vs Sleep",
					character->abilities().at(RESISTANCE_VS_KATINO))
			.c_str());
	pos.y += grid_delta(0, 1).y;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}%", "vs Death",
					character->abilities().at(RESISTANCE_VS_BADI))
			.c_str());
	pos.y += grid_delta(0, 1).y;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}%", "vs Statue",
					character->abilities().at(RESISTANCE_VS_MANIFO))
			.c_str());

	pos.y += grid_delta(0, 1).y;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}%", "Rec / Sleep",
					character->abilities().at(RECOVER_FROM_SLEEP) * 5)
			.c_str());
	pos.y += grid_delta(0, 1).y;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}%", "Rec / Fear",
					character->abilities().at(RECOVER_FROM_FEAR) * 5)
			.c_str());
}

auto Sorcery::UI::_draw_character_summary(Component *component,
										  const Character *character) -> void {

	const auto left_col{component->x};
	const auto middle_col{component->x + 13};
	const auto right_col{component->x + 31};

	using enum Enums::Character::Ability;
	using enum Enums::Character::Attribute;
	auto pos{grid_pos(left_col, component->y)};
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(std::format("{:>8} {:>2}", "Strength",
									   character->get_cur_attr(STRENGTH))
							   .c_str());
	pos = grid_pos(left_col, component->y + 1);
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>8} {:>2}", "I.Q.", character->get_cur_attr(IQ))
			.c_str());
	pos = grid_pos(left_col, component->y + 2);
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>8} {:>2}", "Piety", character->get_cur_attr(PIETY))
			.c_str());
	pos = grid_pos(left_col, component->y + 3);
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(std::format("{:>8} {:>2}", "Vitality",
									   character->get_cur_attr(VITALITY))
							   .c_str());
	pos = grid_pos(left_col, component->y + 4);
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>8} {:>2}", "Agility", character->get_cur_attr(AGILITY))
			.c_str());
	pos = grid_pos(left_col, component->y + 5);
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>8} {:>2}", "Luck", character->get_cur_attr(LUCK))
			.c_str());

	pos = grid_pos(left_col, component->y + 7);
	ImGui::SetCursorPos(pos);
	const auto &sp{character->magic().mage_current_spellpoints()};
	auto mage{std::format("{}/{}/{}/{}/{}/{}/{}", sp.at(1), sp.at(2), sp.at(3),
						  sp.at(4), sp.at(5), sp.at(6), sp.at(7))};
	ImGui::TextUnformatted(std::format("Mage {}", mage).c_str());

	pos = grid_pos(middle_col, component->y);
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:<6} {:>10}", "Gold", character->get_gold()).c_str());
	pos = grid_pos(middle_col, component->y + 1);
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:<6} {:>10}", "E.P.", character->get_cur_xp()).c_str());
	pos = grid_pos(middle_col, component->y + 2);
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:<6} {:>10}", "Next", character->get_next_xp()).c_str());
	pos = grid_pos(middle_col, component->y + 3);
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:<6} {:>10}", "Marks", character->abilities().at(MARKS))
			.c_str());
	pos = grid_pos(middle_col, component->y + 4);
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(std::format("{:<4} {}/{}", "H.P.",
									   character->abilities().at(CURRENT_HP),
									   character->abilities().at(MAX_HP))
							   .c_str());
	pos = grid_pos(middle_col, component->y + 5);
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:<6} {:>18}", "Status", character->get_status_string())
			.c_str());

	const auto &sp2{character->magic().priest_current_spellpoints()};
	auto priest{std::format("{}/{}/{}/{}/{}/{}/{}", sp2.at(1), sp2.at(2),
							sp2.at(3), sp2.at(4), sp2.at(5), sp2.at(6),
							sp2.at(7))};

	pos = grid_pos(component->x + 20, component->y + 7);
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(std::format("Prst {}", priest).c_str());

	pos = grid_pos(right_col, component->y);
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("Swim{:>3}", character->abilities().at(SWIM)).c_str());
	pos = grid_pos(right_col, component->y + 1);
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format(" Age{:>3}", character->abilities().at(AGE) / 52).c_str());
	pos = grid_pos(right_col, component->y + 2);
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format(" RIP{:>3}", character->abilities().at(DEATHS)).c_str());

	auto slot{1u};
	pos = grid_pos(left_col, component->y + 9);
	ImGui::SetCursorPos(pos);
	for (const auto &item : character->inventory.items()) {
		const std::string flag{std::invoke([&] {
			if (!item.get_known())
				return "?";
			else if (item.get_cursed() && item.get_equipped())
				return "-";
			else if (item.get_equipped())
				return "*";
			else if (!item.get_usable())
				return "#";
			else
				return " ";
		})};
		if (slot % 2 == 1)
			pos = grid_pos(left_col, component->y + 9 + slot / 2);
		else
			pos =
				grid_pos(component->x + 17, component->y + 9 + (slot - 1) / 2);
		ImGui::SetCursorPos(pos);
		auto line{std::format("{}){}{}", slot, flag, item.get_display_name())};
		ImGui::TextUnformatted(line.c_str());
		++slot;
	}
}

auto Sorcery::UI::_draw_create_alignment([[maybe_unused]] const int mode)
	-> void {

	auto cmp_summary{components->get("create_alignment:summary_text")};
	auto summary_text{
		_ctx.controller->get_candidate_character()->summary_text()};
	_draw_text(&cmp_summary, summary_text);
}

auto Sorcery::UI::_draw_create_confirm([[maybe_unused]] const int mode)
	-> void {

	auto cmp_summary{components->get("create_confirm:summary_text")};
	auto summary_text{
		_ctx.controller->get_candidate_character()->summary_text()};
	_draw_text(&cmp_summary, summary_text);

	auto cmp_char{components->get("create_confirm:character_data")};
	with_Window(WINDOW_LAYER_TEXTS, nullptr,
				ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs) {
		set_Font(fontstore->get_current_font(cmp_char.font).value(), font_sz());
		_draw_character_summary(&cmp_char,
								_ctx.controller->get_candidate_character());
	}
}

auto Sorcery::UI::_draw_create_class([[maybe_unused]] const int mode) -> void {

	auto cmp_summary{components->get("create_class:summary_text")};
	auto summary_text{
		_ctx.controller->get_candidate_character()->summary_text()};
	_draw_text(&cmp_summary, summary_text);

	auto cmp_points_left{components->get("create_class:points_left_text")};
	const auto points_left_text{
		std::format("{:>2}", _ctx.controller->get_candidate_character()
								 ->create()
								 .get_points_left())};
	_draw_text(&cmp_points_left, points_left_text);

	// Now draw the class buttons
	using enum Enums::Character::Attribute;
	auto cmp_attribute{components->get("create_class:current_stats")};
	for (auto i = std::to_underlying(STRENGTH); i <= std::to_underlying(LUCK);
		 ++i) {
		auto attribute{_ctx.controller->get_candidate_character()->get_attr_ptr(
			enum_cast<Enums::Character::Attribute>(i).value())};
		auto cmp_name{std::format("stepper_attribute_{}", i)};
		_draw_stepper(&cmp_attribute, cmp_name, attribute);
		++cmp_attribute.y;
	}
}

auto Sorcery::UI::_draw_create_race([[maybe_unused]] const int mode) -> void {

	auto cmp_summary{components->get("create_race:summary_text")};
	auto summary_text{
		_ctx.controller->get_candidate_character()->summary_text()};
	_draw_text(&cmp_summary, summary_text);
}

auto Sorcery::UI::_draw_create_name([[maybe_unused]] const int mode) -> void {

	auto cmp_summary{components->get("create_name:summary_text")};
	auto summary_text{
		_ctx.controller->get_candidate_character()->summary_text()};
	_draw_text(&cmp_summary, summary_text);

	// As next custom component is a text box, focus on that initially
	if (first_frame) {
		ImGui::SetKeyboardFocusHere();
		first_frame = false;
	}

	auto cmp_name{components->get("create_name:name_input")};
	_draw_input(&cmp_name, &_ctx.controller->get_input_buffer());
}

auto Sorcery::UI::_draw_reclass() -> void {

	auto cmp_summary{components->get("change_class:summary_text")};
	auto character{_ctx.game->characters.at(
		_ctx.controller->get_character(Enums::CharacterSlot::EDIT))};
	auto summary_text{character.summary_text()};
	_draw_text(&cmp_summary, summary_text);
}

auto Sorcery::UI::_draw_rename() -> void {

	auto cmp_summary{components->get("rename:summary_text")};
	auto character{_ctx.game->characters.at(
		_ctx.controller->get_character(Enums::CharacterSlot::EDIT))};
	auto summary_text{character.summary_text()};
	_draw_text(&cmp_summary, summary_text);

	// As next custom component is a text box, focus on that initially
	if (first_frame) {
		ImGui::SetKeyboardFocusHere();
		first_frame = false;
	}

	auto cmp_name{components->get("rename:rename_input")};
	_draw_input(&cmp_name, &_ctx.controller->get_input_buffer());
}

auto Sorcery::UI::_display_create_alignment(const int mode) -> void {

	_draw_components("create_alignment", mode);
	_draw_create_alignment(mode);
	_draw_cursor();
}

auto Sorcery::UI::_display_create_confirm(const int mode) -> void {

	_draw_components("create_confirm", mode);
	_draw_create_confirm(mode);
	_draw_cursor();
}

auto Sorcery::UI::_display_create_class(const int mode) -> void {

	_draw_components("create_class", mode);
	_draw_create_class(mode);
	_draw_cursor();
}

auto Sorcery::UI::_display_create_race(const int mode) -> void {

	_draw_components("create_race", mode);
	_draw_create_race(mode);
	_draw_cursor();
}

auto Sorcery::UI::_display_create_name(const int mode) -> void {

	_draw_components("create_name", mode);
	_draw_create_name(mode);
	_draw_cursor();
}

auto Sorcery::UI::_draw_choose(const int mode) -> void {

	if (mode & CHOOSE_MODE_STAY) {

		auto cmp{components->get("choose:choose_stay")};
		_draw_text(&cmp);
		_draw_party_panel();
	}
}

auto Sorcery::UI::_draw_level_up(const int mode) -> void {

	auto &character{_ctx.game->characters.at(
		_ctx.controller->get_character(Enums::CharacterSlot::STAY))};

	if (mode & RECOVERY_BIRTHDAY) {

		const auto birth_text{_ctx.get_string("REST_BIRTHDAY_YOU")};
		auto cmp{components->get("levelup:levelup_birthday")};
		_draw_text(&cmp, birth_text);
		cmp = components->get("levelup:levelup_results");
		for (const auto &result : character.level_up_results) {
			_draw_text(&cmp, result);
			cmp.y += grid_delta(0, 1).y;
		}
	} else {

		auto cmp{components->get("levelup:levelup_birthday")};
		for (const auto &result : character.level_up_results) {
			_draw_text(&cmp, result);
			++cmp.y;
		}
	}

	with_Window(WINDOW_LAYER_MENUS, nullptr, ImGuiWindowFlags_NoTitleBar) {
		auto leave{components->get("levelup:levelup_leave")};
		_draw_button_click(&leave, _ctx.get_flag_ref("show_levelup"), true);
	}
}

auto Sorcery::UI::_draw_pay_info() -> void {

	// Work out healing cost
	const auto character{_ctx.game->characters.at(
		_ctx.controller->get_character(Enums::CharacterSlot::HELP))};
	const auto cost(character.get_cure_cost());
	const auto cost_text{std::format("{} {} {}",
									 _ctx.get_string("PAY_COST_PREFIX"), cost,
									 _ctx.get_string("PAY_COST_SUFFIX"))};
	auto cmp{components->get("pay:pay_cost")};
	_draw_text(&cmp, cost_text);
}

auto Sorcery::UI::_draw_no_level_up(const int mode) -> void {

	const auto character{_ctx.game->characters.at(
		_ctx.controller->get_character(Enums::CharacterSlot::STAY))};
	const auto birth_text{_ctx.get_string("REST_BIRTHDAY_YOU")};
	const auto needed{character.get_next_xp() - character.get_cur_xp()};
	const auto need_text{std::format("{}{}{}", _ctx.get_string("REST_NEED_1_P"),
									 needed, _ctx.get_string("REST_NEED_1_S"))};
	const auto make_text{_ctx.get_string("REST_NEED_2")};

	if (mode & RECOVERY_BIRTHDAY) {

		auto cmp{components->get("nolevelup:nolevelup_birthday")};
		_draw_text(&cmp, birth_text);
		cmp = components->get("nolevelup:nolevelup_need_1");
		_draw_text(&cmp, need_text);
		cmp = components->get("nolevelup:nolevelup_need_2");
		_draw_text(&cmp, make_text);
	} else {

		auto cmp{components->get("nolevelup:nolevelup_birthday")};
		_draw_text(&cmp, need_text);
		cmp = components->get("nolevelup:nolevelup_need_1");
		_draw_text(&cmp, make_text);
	}

	with_Window(WINDOW_LAYER_MENUS, nullptr, ImGuiWindowFlags_NoTitleBar) {
		auto leave{components->get("nolevelup:nolevelup_leave")};
		_draw_button_click(&leave, _ctx.get_flag_ref("show_nolevelup"), true);
	}
}

auto Sorcery::UI::_draw_rite(const int stage) -> void {

	auto cmp_summary{components->get("rite:summary_text")};
	auto character{_ctx.game->characters.at(
		_ctx.controller->get_character(Enums::CharacterSlot::EDIT))};
	auto summary_text{character.summary_text()};
	_draw_text(&cmp_summary, summary_text);

	if (stage == 0)
		return;

	auto cmp_progress{components->get("rite:progress_text")};
	auto progress_text{_ctx.get_string("RITE_PROGRESS")};

	auto cmp{components->get("rite:rite_stage")};

	std::string text;

	switch (stage) {

	case 1:
		text = _ctx.get_string("RITE_STAGE_1");
		break;

	case 2:
		text = _ctx.get_string("RITE_STAGE_2");
		_draw_text(&cmp_progress, progress_text);
		break;

	case 3:
		text = _ctx.get_string("RITE_STAGE_3");
		_draw_text(&cmp_progress, progress_text);
		break;

	case 4:
		text = _ctx.get_string("RITE_STAGE_4");
		_draw_text(&cmp_progress, progress_text);
		break;

	case 5:
		text = _ctx.get_string("RITE_STAGE_5");
		break;

	default:
		return;
	}

	_draw_text(&cmp, text);
}

auto Sorcery::UI::_draw_heal(int stage) -> void {

	auto cmp{components->get("heal:heal_status")};
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
		_draw_text(&cmp, text);

	if (_ctx.controller->has_flag("heal_finished") &&
		_ctx.controller->has_text("heal_results")) {

		auto summary{components->get("heal:heal_results")};
		const auto results{_ctx.controller->get_text("heal_results")};
		_draw_text(&summary, results);
		with_Window(WINDOW_LAYER_MENUS, nullptr, ImGuiWindowFlags_NoTitleBar) {

			auto leave{components->get("heal:button_heal_return")};
			_draw_button_click(&leave, _ctx.get_flag_ref("heal_return"), true);
		}
	}
}
auto Sorcery::UI::_draw_chest(const Enums::Chests::State state) -> void {

	_draw_components("engine_base_ui");

	if (!_ctx.controller->get_monochrome()) {
		auto bg_c{components->get("engine_base_ui:background_image")};
		_draw_tiled_bg(&bg_c);
	}

	if (_ctx.get_flag("interface_ui") && _ctx.get_flag("interface_party_panel"))
		_draw_party_panel();

	// Dungeon View
	auto component{components->get("engine_base_ui:wire_frame_view")};
	_render->draw(&component);

	// Only the main chest state has a persistent menu.
	_draw_components("chest_menu");

	// Chest
	const auto chest_idx{CHEST_GFX_ID};
	const auto cmp{components->get("chest:chest_image")};
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

	_draw_fg_image_with_idx(EVENTS_TEXTURE, chest_idx, p_min,
							ImVec2{chest_w, chest_h});

	_draw_debug();
	_draw_ui_status();
	_draw_cursor();
}

auto Sorcery::UI::_draw_recovery(const int mode) -> void {

	const auto character{_ctx.game->characters.at(
		_ctx.controller->get_character(Enums::CharacterSlot::STAY))};
	if (mode == RECOVERY_MODE_FREE) {

		auto cmp{components->get("recovery:recovery_napping")};
		auto text{std::format("{}{}", character.get_name(),
							  _ctx.get_string("RECOVERY_NAPPING"))};
		_draw_text(&cmp, text);

	} else {

		auto cmp{components->get("recovery:recovery_recuperating")};
		auto text{std::format("{} {}", character.get_name(),
							  _ctx.get_string("REST_RECUPERATING"))};
		_draw_text(&cmp, text);

		cmp = components->get("recovery:recovery_recuperating_hp");
		text = std::format("{} ({:>5}/{:>5})", _ctx.get_string("REST_HP"),
						   character.get_current_hp(), character.get_max_hp());
		_draw_text(&cmp, text);

		cmp = components->get("recovery:recovery_recuperating_gold");
		text = std::format("{} {:>7}", _ctx.get_string("REST_GOLD"),
						   character.get_gold());
		_draw_text(&cmp, text);

		with_Window(WINDOW_LAYER_MENUS, nullptr, ImGuiWindowFlags_NoTitleBar) {
			auto stop{components->get("recovery:recovery_stop")};
			_draw_button_click(&stop, _ctx.get_flag_ref("show_recovery"), true);
		}
	}
}

auto Sorcery::UI::_draw_stay() -> void {

	const auto character{_ctx.game->characters.at(
		_ctx.controller->get_character(Enums::CharacterSlot::STAY))};

	auto cmp_welcome{components->get("stay:stay_welcome")};
	auto welcome_text{std::format("{}{}{}", _ctx.get_string("STAY_WELCOME_P"),
								  character.get_name(),
								  _ctx.get_string("STAY_WELCOME_S"))};
	_draw_text(&cmp_welcome, welcome_text);

	auto cmp_gold{components->get("stay:stay_gold")};
	auto gold_text{std::format("{}{}{}", _ctx.get_string("STAY_GOLD_P"),
							   character.get_gold(),
							   _ctx.get_string("STAY_GOLD_S"))};
	_draw_text(&cmp_gold, gold_text);
}

auto Sorcery::UI::_draw_buy() -> void {

	const auto character{_ctx.game->characters.at(
		_ctx.controller->get_character(Enums::CharacterSlot::STORE))};

	auto cmp_welcome{components->get("buy:buy_welcome")};
	auto welcome_text{std::format("{}{}{}", _ctx.get_string("BUY_WELCOME_P"),
								  character.get_name(),
								  _ctx.get_string("BUY_WELCOME_S"))};
	_draw_text(&cmp_welcome, welcome_text);

	auto cmp_gold{components->get("buy:buy_gold")};
	auto gold_text{std::format("{}{}{}", _ctx.get_string("BUY_GOLD_P"),
							   character.get_gold(),
							   _ctx.get_string("BUY_GOLD_S"))};
	_draw_text(&cmp_gold, gold_text);
}

auto Sorcery::UI::_draw_sell() -> void {

	const auto character{_ctx.game->characters.at(
		_ctx.controller->get_character(Enums::CharacterSlot::STORE))};

	auto cmp_welcome{components->get("sell:sell_welcome")};
	auto welcome_text{std::format("{}{}{}", _ctx.get_string("SELL_WELCOME_P"),
								  character.get_name(),
								  _ctx.get_string("SELL_WELCOME_S"))};
	_draw_text(&cmp_welcome, welcome_text);

	auto cmp_gold{components->get("sell:sell_gold")};
	auto gold_text{std::format("{}{}{}", _ctx.get_string("SELL_GOLD_P"),
							   character.get_gold(),
							   _ctx.get_string("SELL_GOLD_S"))};
	_draw_text(&cmp_gold, gold_text);

	// And regenerate sell menu
	//_draw_menu
}

auto Sorcery::UI::_draw_identify() -> void {

	const auto character{_ctx.game->characters.at(
		_ctx.controller->get_character(Enums::CharacterSlot::STORE))};

	auto cmp_welcome{components->get("identify:identify_welcome")};
	auto welcome_text{std::format(
		"{}{}{}", _ctx.get_string("IDENTIFY_WELCOME_P"), character.get_name(),
		_ctx.get_string("IDENTIFY_WELCOME_S"))};
	_draw_text(&cmp_welcome, welcome_text);

	auto cmp_gold{components->get("identify:identify_gold")};
	auto gold_text{std::format("{}{}{}", _ctx.get_string("IDENTIFY_GOLD_P"),
							   character.get_gold(),
							   _ctx.get_string("IDENTIFY_GOLD_S"))};
	_draw_text(&cmp_gold, gold_text);

	// And regenerate sell menu
	//_draw_menu
}

auto Sorcery::UI::_draw_uncurse() -> void {

	const auto character{_ctx.game->characters.at(
		_ctx.controller->get_character(Enums::CharacterSlot::STORE))};

	auto cmp_welcome{components->get("uncurse:uncurse_welcome")};
	auto welcome_text{std::format(
		"{}{}{}", _ctx.get_string("UNCURSE_WELCOME_P"), character.get_name(),
		_ctx.get_string("UNCURSE_WELCOME_S"))};
	_draw_text(&cmp_welcome, welcome_text);

	auto cmp_gold{components->get("uncurse:uncurse_gold")};
	auto gold_text{std::format("{}{}{}", _ctx.get_string("UNCURSE_GOLD_P"),
							   character.get_gold(),
							   _ctx.get_string("UNCURSE_GOLD_S"))};
	_draw_text(&cmp_gold, gold_text);

	// And regenerate sell menu
	//_draw_menu
}

auto Sorcery::UI::_draw_store() -> void {

	const auto character{_ctx.game->characters.at(
		_ctx.controller->get_character(Enums::CharacterSlot::STORE))};

	auto cmp_welcome{components->get("store:store_welcome")};
	auto welcome_text{std::format("{}{}{}", _ctx.get_string("STORE_WELCOME_P"),
								  character.get_name(),
								  _ctx.get_string("STORE_WELCOME_S"))};
	_draw_text(&cmp_welcome, welcome_text);

	auto cmp_gold{components->get("store:store_gold")};
	auto gold_text{std::format("{}{}{}", _ctx.get_string("STORE_GOLD_P"),
							   character.get_gold(),
							   _ctx.get_string("STORE_GOLD_S"))};
	_draw_text(&cmp_gold, gold_text);
}

auto Sorcery::UI::_draw_current_character([[maybe_unused]] const int mode)
	-> void {

	auto character{_ctx.game->characters.at(
		_ctx.controller->get_character(Enums::CharacterSlot::INSPECT))};

	auto title{components->get("inspect:character_title")};
	_draw_text(&title, character.summary_text_with_awards());

	with_Window(WINDOW_LAYER_MENUS, nullptr, ImGuiWindowFlags_NoTitleBar) {
		auto prev{components->get("inspect:character_previous")};
		_draw_button_click(&prev,
						   _ctx.get_flag_ref("select_previous_character"));
		auto next{components->get("inspect:character_next")};
		_draw_button_click(&next, _ctx.get_flag_ref("select_next_character"));

		auto cmp{components->get("inspect:character_data")};
		auto pos{grid_pos(cmp.x, cmp.y)};

		ImGuiTabBarFlags tb_flags{ImGuiTabBarFlags_None};
		ImGui::SetCursorPos(pos);
		with_Child("character_tab_bar_child",
				   ImVec2(grid_sz() * cmp.w, grid_sz() * cmp.h)) {
			UIStyle::set_tab_black(_ctx);
			auto char_cmp{components->get("inspect:character_tab_data")};
			set_Font(fontstore->get_current_font(cmp.font).value(), font_sz());
			with_TabBar("character_tab_bar", tb_flags) {
				with_TabItem("Info") {
					_draw_character_summary(&char_cmp, &character);
				}
				with_TabItem("Stats##1") {
					_draw_character_detailed(&char_cmp, &character);
				}
				with_TabItem("Stats##2") {
					_draw_character_detailed_again(&char_cmp, &character);
				}
				with_TabItem("Arcane") {
					_draw_character_mage_spells(&char_cmp, &character);
				}
				with_TabItem("Divine") {
					_draw_character_priest_spells(&char_cmp, &character);
				}
			}
		}
	}
}

auto Sorcery::UI::_draw_stepper(Component *component, const std::string &name,
								int *value) -> void {

	bool disabled{false};

	with_Window(WINDOW_LAYER_MENUS, nullptr, ImGuiWindowFlags_NoTitleBar) {

		auto pos{grid_pos(component->x, component->y)};
		ImGui::SetCursorPos(pos);

		set_Font(fontstore->get_current_font(component->font).value(),
				 font_sz());

		const auto stepper_name{std::format("##{}", name)};
		const auto stepper_minus{std::format("##{}_minus", name)};
		const auto stepper_plus{std::format("##{}_plus", name)};

		const auto col{get_hl_colour(_ctx.animation->lerp)};
		UIStyle::set_faded(_ctx);
		set_StyleColor(ImGuiCol_ButtonHovered, (ImVec4)col);

		if (component->name == "current_stats") {
			using enum Enums::Character::Attribute;
			const auto mins{_ctx.controller->get_candidate_character()
								->create()
								.get_start_attributes()};
			if (name == "stepper_attribute_1")
				disabled = !(*value > mins.at(STRENGTH));
			else if (name == "stepper_attribute_2")
				disabled = !(*value > mins.at(IQ));
			else if (name == "stepper_attribute_3")
				disabled = !(*value > mins.at(PIETY));
			else if (name == "stepper_attribute_4")
				disabled = !(*value > mins.at(VITALITY));
			else if (name == "stepper_attribute_5")
				disabled = !(*value > mins.at(AGILITY));
			else if (name == "stepper_attribute_6")
				disabled = !(*value > mins.at(LUCK));
		};

		with_ID(stepper_minus.c_str()) {

			if (disabled)
				ImGui::BeginDisabled();
			if (ImGui::Button("<")) {
				_ctx.controller->handle_stepper_button_click(stepper_plus, this,
															 false, value);
			}
			if (disabled)
				ImGui::EndDisabled();
		}

		pos.x += grid_delta(1, 0).x;

		ImVec4 alpha_col{ImGui::ColorConvertU32ToFloat4(component->colour)};
		alpha_col.w = _ctx.animation->fade;

		set_StyleColor(ImGuiCol_Text, alpha_col);
		ImGui::SetCursorPos(pos);
		ImGui::TextUnformatted(std::format("{:>2}", *value).c_str());

		disabled = false;
		if (component->name == "current_stats") {
			if ((*value >= 18) || (_ctx.controller->get_candidate_character()
									   ->create()
									   .get_points_left() == 0))
				disabled = true;
		};

		pos.x += grid_delta(2, 0).x;
		ImGui::SetCursorPos(pos);
		with_ID(stepper_plus.c_str()) {
			if (disabled)
				ImGui::BeginDisabled();
			if (ImGui::Button(">")) {
				_ctx.controller->handle_stepper_button_click(stepper_minus,
															 this, true, value);
			}
			if (disabled)
				ImGui::EndDisabled();
		}
	}
}

auto Sorcery::UI::_draw_input(Component *component, std::string *input)
	-> void {

	with_Window(WINDOW_LAYER_MENUS, nullptr, ImGuiWindowFlags_NoTitleBar) {

		auto pos{grid_pos(component->x, component->y)};
		ImGui::SetCursorPos(pos);

		set_Font(fontstore->get_current_font(component->font).value(),
				 font_sz());

		ImGuiInputTextFlags flags{ImGuiInputTextFlags_AutoSelectAll |
								  ImGuiInputTextFlags_EnterReturnsTrue};

		const auto input_name{std::format("##{}", component->name)};
		const auto input_button_name{std::format("##{}_ok", component->name)};
		const auto input_button_id{std::format("{}_ok", component->name)};
		ImGui::SetNextItemWidth(ImGui::GetFontSize() * component->w);
		if (ImGui::InputText(input_name.c_str(), input, flags)) {
			_ctx.controller->handle_input_button_click(input_button_id, this,
													   input);
		}

		ImGui::SameLine();
		const auto col{get_hl_colour(_ctx.animation->lerp)};
		UIStyle::set_faded(_ctx);
		set_StyleColor(ImGuiCol_ButtonHovered, (ImVec4)col);

		with_ID(input_button_name.c_str()) {
			if (ImGui::Button(">")) {

				// Handle buttons being used to switch on AND off the flag
				// flag = !reverse;
				_ctx.controller->handle_input_button_click(input_button_id,
														   this, input);
			}
		}
	}
}

auto Sorcery::UI::_draw_text(Component *component, const std::string &string)
	-> void {

	with_Window(WINDOW_LAYER_TEXTS, nullptr,
				ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs) {

		// Need to push font first before calculating size else it will
		// assume monospace font size!
		set_Font(fontstore->get_current_font(component->font).value(),
				 font_sz());

		const auto x{std::invoke([&] {
			if (component->x == -1) {
				const auto viewport{ImGui::GetMainViewport()};
				const auto width{ImGui::CalcTextSize(CSTR(string))};
				return (viewport->Size.x - width.x) / 2;
			} else
				return grid_pos(component->x, component->y).x;
		})};
		const auto y{std::invoke([&] {
			if (component->y == -1) {
				const auto viewport{ImGui::GetMainViewport()};
				const auto height{ImGui::CalcTextSize(CSTR(string))};
				return (viewport->Size.y - height.y) / 2;
			} else
				return grid_pos(component->x, component->y).y;
		})};

		// Adjust Alpha of Text
		ImVec4 alpha_col{ImGui::ColorConvertU32ToFloat4(component->colour)};
		alpha_col.w = _ctx.animation->fade;

		set_StyleColor(ImGuiCol_Text, alpha_col);
		ImGui::SetCursorPos(ImVec2{x, y});
		ImGui::TextUnformatted(string.c_str());
	}
}

auto Sorcery::UI::_draw_party_wipe() -> void {

	const auto grave_cmp{components->get("graveyard:gravestone")};
	const auto text_cmp{components->get("graveyard:party_members")};

	constexpr auto max_cols{3};

	const auto grave_idx{GRAVESTONE_GFX_ID};
	const auto scale{_ctx.display->get_display_metrics().scale};
	const auto grave_w{grave_cmp.get_float("tile_width") * scale};
	const auto grave_h{grave_cmp.get_float("tile_height") * scale};

	const auto gap{grid_delta(grave_cmp.get_float("spacing_x") * scale,
							  grave_cmp.get_float("spacing_y") * scale)};

	std::vector<std::string> names;

	const auto party{_ctx.game->state->get_party_characters()};
	for (const auto character_id : party) {
		const auto character{_ctx.game->characters.find(character_id)};

		if (character == _ctx.game->characters.end())
			continue;

		names.emplace_back(character->second.get_name());
	}

	if (names.empty())
		return;

	const auto count{static_cast<int>(names.size())};
	const auto layout_cols{std::min(max_cols, count)};
	const auto rows{(count + max_cols - 1) / max_cols};

	const auto cell_w{grave_w + gap.x};
	const auto cell_h{grave_h + gap.y};

	const auto origin_x{grid_x(grave_cmp.x)};

	// The component's Y coordinate represents the vertical centre of the
	// complete gravestone arrangement.
	const auto centre_y{grid_y(grave_cmp.y)};
	const auto layout_w{(layout_cols * grave_w) + ((layout_cols - 1) * gap.x)};
	const auto layout_h{(rows * grave_h) + ((rows - 1) * gap.y)};

	const auto origin_y{centre_y - (layout_h * 0.5f)};

	for (auto row = 0; row < rows; ++row) {

		const auto first_index{row * max_cols};
		const auto row_count{std::min(max_cols, count - first_index)};

		// Centre each row independently.
		const auto row_w{(row_count * grave_w) + ((row_count - 1) * gap.x)};
		const auto row_x{origin_x + ((layout_w - row_w) * 0.5f)};

		for (auto col = 0; col < row_count; ++col) {

			const auto index{first_index + col};

			const ImVec2 grave_pos{row_x + (static_cast<float>(col) * cell_w),
								   origin_y +
									   (static_cast<float>(row) * cell_h)};

			_draw_fg_image_with_idx(EVENTS_TEXTURE, grave_idx, grave_pos,
									ImVec2{grave_w, grave_h});

			const auto &name{names.at(index)};

			set_Font(fontstore->get_current_font(text_cmp.font).value(),
					 font_sz());

			const auto text_size{ImGui::CalcTextSize(name.c_str())};

			const ImVec2 text_pos{grave_pos.x +
									  ((grave_w - text_size.x) * 0.5f),
								  grave_pos.y + grave_h - grid_delta(0, 1).y};

			draw_text_with_layer(name, text_cmp.colour, text_pos,
								 text_cmp.font);
		}
	}
}

auto Sorcery::UI::_draw_automap_legend(Component *component) -> void {

	struct MapLegendItem {
			Enums::DrawMap::Feature feature;
			std::string_view label;
	};

	using enum Enums::DrawMap::Feature;

	static constexpr std::array legend{
		MapLegendItem{FLOOR, "Floor"},
		MapLegendItem{NORTH_WALL, "Wall"},
		MapLegendItem{NORTH_DOOR, "Door"},
		MapLegendItem{NORTH_SECRET, "Secret"},
		MapLegendItem{MAP_DARKNESS, "Darkness"},
		MapLegendItem{MAP_STAIRS_UP, "Stairs Up"},
		MapLegendItem{MAP_STAIRS_DOWN, "Stairs Down"},
		MapLegendItem{MAP_ELEVATOR, "Elevator"},
		MapLegendItem{MAP_PIT, "Pit"},
		MapLegendItem{MAP_SPINNER, "Spinner"},
		MapLegendItem{MAP_CHUTE, "Chute"},
		MapLegendItem{MAP_TELEPORT_FROM, "Teleport From"},
		MapLegendItem{MAP_TELEPORT_TO, "Teleport To"},
		MapLegendItem{EXCLAMATION, "Message"},
	};

	const auto scale{_ctx.display->get_display_metrics().scale};
	const auto icon_size{component->get_int("tile_size") * scale};
	const auto row_gap{component->get_int("row_gap") * scale};

	auto pos{grid_pos(component->x, component->y)};

	with_Window(WINDOW_LAYER_MENUS, nullptr, ImGuiWindowFlags_NoDecoration) {

		set_Font(fontstore->get_current_font(component->font).value(),
				 font_sz());

		auto cmp_level{components->get("automap:automap_level")};
		_draw_text(&cmp_level, _ctx.game->state->level->name());

		for (const auto &item : legend) {
			_draw_fg_image_with_idx(WINDOW_LAYER_MENUS, MAPS_TEXTURE,
									std::to_underlying(item.feature), pos,
									ImVec2{static_cast<float>(icon_size),
										   static_cast<float>(icon_size)});

			const auto delta{grid_delta(1.0f, 0.0f)};
			ImGui::SetCursorPos(ImVec2{pos.x + icon_size + delta.x, pos.y});

			ImGui::TextUnformatted(item.label.data());

			pos.y += icon_size + row_gap;
		}
	}

	with_Window(WINDOW_LAYER_MENUS, nullptr, ImGuiWindowFlags_NoTitleBar) {
		auto leave{components->get("automap:automap_return")};
		_draw_button_click(&leave, _ctx.get_flag_ref("show_automap"), true);
	}
}

// Draw a Text (String)
auto Sorcery::UI::_draw_text(Component *component) -> void {
	with_Window(WINDOW_LAYER_TEXTS, nullptr,
				ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs) {

		// Need to push font first before calculating size else it will
		// assume monospace font size!
		set_Font(fontstore->get_current_font(component->font).value(),
				 font_sz());

		const auto x{std::invoke([&] {
			if (component->x == -1) {
				const auto viewport{ImGui::GetMainViewport()};
				const auto width{ImGui::CalcTextSize(
					CSTR(_ctx.get_string(component->string_key)))};
				return (viewport->Size.x - width.x) / 2;
			} else
				return grid_pos(component->x, component->y).x;
		})};
		const auto y{std::invoke([&] {
			if (component->y == -1) {
				const auto viewport{ImGui::GetMainViewport()};
				const auto height{ImGui::CalcTextSize(
					CSTR(_ctx.get_string(component->string_key)))};
				return (viewport->Size.y - height.y) / 2;
			} else
				return grid_pos(component->x, component->y).y;
		})};

		// Adjust Alpha of Text
		ImVec4 alpha_col{ImGui::ColorConvertU32ToFloat4(component->colour)};
		alpha_col.w = _ctx.animation->fade;

		set_StyleColor(ImGuiCol_Text, alpha_col);
		ImGui::SetCursorPos(ImVec2{x, y});
		ImGui::TextUnformatted(_ctx.get_string(component->string_key).c_str());
	}
}

auto Sorcery::UI::_draw_components(std::string_view screen,
								   [[maybe_unused]] const int mode) -> void {

	_frames.clear();
	_menus.clear();

	// Draw every component specified in order
	for (auto cmps{(*components)(screen)}; auto c : cmps.value()) {
		using enum Enums::Layout::ComponentType;
		if (c.type == IMAGE_BG)
			_draw_tiled_bg(&c);
		else if (c.type == FRAME)
			_draw_frame(&c);
		else if (c.type == IMAGE_FG)
			_draw_fg_image(&c);
		else if (c.type == TEXT)
			_draw_text(&c);
		else if (c.type == BUTTON)
			_draw_button(&c);
		else if (c.type == PARAGRAPH)
			_draw_paragraph(&c);
		else if (c.type == MENU)
			_draw_menu(&c);
	}
}

auto Sorcery::UI::_display_splash() -> void {

	// Standard Components
	_draw_components("splash");

	// Cursor (if available)
	_draw_cursor();

	// Progress Bar
	_draw_loading_progress();
}

auto Sorcery::UI::_draw_item_info() -> void {
	// Custom Rendering
	const auto idx{_ctx.get_selected("museum_selected")};
	if (idx >= 100)
		return;

	const auto item{_ctx.resources->items->get(idx + 1)};
	auto item_c{components->get("museum:item_graphic")};
	auto item_pos{grid_pos(item_c.x, item_c.y)};
	const auto scale{_ctx.display->get_display_metrics().scale};
	_draw_fg_image_with_idx(ITEMS_TEXTURE, idx, item_pos,
							ImVec2{item_c.get_float("tile_width") * scale,
								   item_c.get_float("tile_width") * scale});

	auto cmp{components->get("museum:item_data")};
	auto pos{grid_pos(cmp.x, cmp.y)};

	with_Window(WINDOW_LAYER_MENUS, nullptr, ImGuiWindowFlags_NoDecoration) {

		const auto name{std::format("  {:>03}:{}/{}", idx + 1,
									item.get_known_name(),
									item.get_unknown_name())};

		ImGuiTabBarFlags tb_flags{ImGuiTabBarFlags_None};
		ImGui::SetCursorPos(pos);
		with_Child("museum_tab_bar_child",
				   ImVec2(grid_sz() * cmp.w, grid_sz() * cmp.h)) {
			UIStyle::set_tab_black(_ctx);
			set_Font(fontstore->get_current_font(cmp.font).value(), font_sz());
			with_TabBar("museum_tab_bar", tb_flags) {
				with_TabItem("Info") {
					{
						UIStyle::set_text_bright(_ctx);
						ImGui::TextUnformatted(name.c_str());
					}

					UIStyle::set_text_dim(_ctx);

					std::string item_category{enum_name(item.get_category())};
					if (item_category == "SPECIAL_ITEM")
						item_category = "Quest Item";

					auto it_cat{CAPITALISE(item_category)};
					const std::string it_cursed{
						item.get_cursed() == true ? "Yes" : "No"};
					const auto cat{std::format(" Type:{}", it_cat)};
					const auto cost{std::format(" Cost:{}", item.get_value())};
					const auto curse{std::format("Curse:{}", it_cursed)};
					const auto allow{
						std::format("Allow:{}", item.get_equippable_display())};
					ImGui::TextUnformatted(cat.c_str());
					ImGui::TextUnformatted(allow.c_str());
					ImGui::TextUnformatted(cost.c_str());
					ImGui::TextUnformatted(curse.c_str());
				}
				with_TabItem("Stats") {
					{
						UIStyle::set_text_bright(_ctx);
						ImGui::TextUnformatted(name.c_str());
					}

					UIStyle::set_text_dim(_ctx);

					const auto ac{std::format(
						"   AC:{}", std::to_string(item.get_ac_mod()))};
					auto value{std::format("{:+d}", item.get_to_hit_mod())};
					auto hit{std::format("  Hit:{:}", value)};
					const auto damage{
						std::format("  Dam:{}", item.get_damage())};
					const auto atks{std::format(
						" Atks:{}", std::to_string(item.get_swings()))};

					ImGui::TextUnformatted(atks.c_str());
					ImGui::TextUnformatted(hit.c_str());
					ImGui::TextUnformatted(damage.c_str());
					ImGui::TextUnformatted(ac.c_str());
				}
				with_TabItem("Traits") {
					{
						UIStyle::set_text_bright(_ctx);
						ImGui::TextUnformatted(name.c_str());
					}

					UIStyle::set_text_dim(_ctx);

					auto effects{item.get_effects()};
					if (effects.length() > 0) {
						std::vector<std::string> strings;
						const std::regex regex(R"([|]+)");
						std::sregex_token_iterator it{effects.begin(),
													  effects.end(), regex, -1};
						std::vector<std::string> split{it, {}};
						split.erase(std::remove_if(split.begin(), split.end(),
												   [](std::string const &s) {
													   return s.size() == 0;
												   }),
									split.end());
						strings = split;

						for (const auto &each_string : strings)
							ImGui::TextUnformatted(each_string.c_str());
					}
				}
				with_TabItem("Powers") {
					{
						UIStyle::set_text_bright(_ctx);
						ImGui::TextUnformatted(name.c_str());
					}
					UIStyle::set_text_dim(_ctx);

					const auto invokage{item.get_invokage()};
					const auto usage{item.get_usage()};

					if (usage.length() > 0)
						ImGui::TextUnformatted(usage.c_str());
					if (invokage.length() > 0)
						ImGui::TextUnformatted(invokage.c_str());
				}
			}
		}
	}
}

auto Sorcery::UI::_draw_license(Component *component, const std::string &string)
	-> void {
	with_Window(WINDOW_LAYER_MENUS, nullptr, ImGuiWindowFlags_NoTitleBar) {

		// To adjust for Window Resizing etc
		const auto x{std::invoke([&] {
			const auto width{grid_sz() * component->get_float("grid_width")};
			const auto viewport{ImGui::GetMainViewport()};
			return (viewport->Size.x - width) / 2;
		})};

		const auto pos{ImVec2{x, grid_y(component->y)}};
		ImGui::SetNextWindowPos(pos);
		with_Child("license_child",
				   ImVec2(grid_sz() * component->w, grid_sz() * component->h),
				   ImGuiChildFlags_NavFlattened,
				   ImGuiWindowFlags_AlwaysVerticalScrollbar) {

			UIStyle::set_text_dim(_ctx);
			set_Font(fontstore->get_current_font(component->font).value(),
					 font_sz());
			with_TextWrapPos(ImGui::GetFontSize() * component->w) {
				ImGui::TextUnformatted(string.c_str());
			}
		}

		// Special Handling for Return Button
		Component cmp{components->get("license:license_return")};
		_draw_button_click(&cmp, _ctx.get_flag_ref("show_license"), true);
	}
}

auto Sorcery::UI::load_message(const Enums::Map::Event event)
	-> std::vector<std::string> {

	switch (event) {
		using enum Enums::Map::Event;
	case AREA_OF_OUT_BOUNDS:
		return {
			"GAME_MESSAGE_AREA_OUT_OF_BOUNDS_1",
			"GAME_MESSAGE_AREA_OUT_OF_BOUNDS_2",
		};
		break;
	case MAN_TELEPORT_CASTLE:
		return {
			"GAME_MESSAGE_MAN_TELEPORT_CASTLE_1",
			"GAME_MESSAGE_MAN_TELEPORT_CASTLE_2",
			"GAME_MESSAGE_MAN_TELEPORT_CASTLE_3",
			"GAME_MESSAGE_MAN_TELEPORT_CASTLE_4",
			"GAME_MESSAGE_MAN_TELEPORT_CASTLE_45",
			"GAME_MESSAGE_MAN_TELEPORT_CASTLE_5",
			"GAME_MESSAGE_MAN_TELEPORT_CASTLE_6",
		};
		break;
	case OBTAIN_SILVER_KEY:
		return {
			"GAME_MESSAGE_SILVER_KEY_1", "GAME_MESSAGE_SILVER_KEY_2",
			"GAME_MESSAGE_SILVER_KEY_3", "GAME_MESSAGE_SILVER_KEY_4",
			"GAME_MESSAGE_SILVER_KEY_5", "GAME_MESSAGE_SILVER_KEY_6",
			"GAME_MESSAGE_SILVER_KEY_7", "GAME_MESSAGE_SILVER_KEY_8",
		};
		break;
	case OBTAIN_BRONZE_KEY:
		return {
			"GAME_MESSAGE_BRONZE_KEY_1", "GAME_MESSAGE_BRONZE_KEY_2",
			"GAME_MESSAGE_BRONZE_KEY_3", "GAME_MESSAGE_BRONZE_KEY_4",
			"GAME_MESSAGE_BRONZE_KEY_5", "GAME_MESSAGE_BRONZE_KEY_6",
		};
		break;
	case MURPHYS_GHOSTS:
		return {
			"GAME_MESSAGE_MURPHYS_GHOSTS_1", "GAME_MESSAGE_MURPHYS_GHOSTS_2",
			"GAME_MESSAGE_MURPHYS_GHOSTS_3", "GAME_MESSAGE_MURPHYS_GHOSTS_4",
			"GAME_MESSAGE_MURPHYS_GHOSTS_5",
		};
		break;
	case OBTAIN_BEAR_STATUE:
		return {
			"GAME_MESSAGE_BEAR_STATUE_1",
			"GAME_MESSAGE_BEAR_STATUE_2",
			"GAME_MESSAGE_BEAR_STATUE_3",
		};
		break;
	case OBTAIN_FROG_STATUE:
		return {
			"GAME_MESSAGE_FROG_STATUE_1",
			"GAME_MESSAGE_FROG_STATUE_2",
			"GAME_MESSAGE_FROG_STATUE_3",
			"GAME_MESSAGE_FROG_STATUE_4",
		};
		break;
	case OBTAIN_GOLD_KEY:
		return {
			"GAME_MESSAGE_GOLD_KEY_1", "GAME_MESSAGE_GOLD_KEY_2",
			"GAME_MESSAGE_GOLD_KEY_3", "GAME_MESSAGE_GOLD_KEY_4",
			"GAME_MESSAGE_GOLD_KEY_5", "GAME_MESSAGE_GOLD_KEY_6",
		};
		break;
	case NEED_SILVER_KEY:
		return {
			"GAME_MESSAGE_NEED_SILVER_KEY_1",
			"GAME_MESSAGE_NEED_SILVER_KEY_2",
			"GAME_MESSAGE_NEED_SILVER_KEY_3",
			"GAME_MESSAGE_NEED_SILVER_KEY_4",
		};
		break;
	case NEED_BRONZE_KEY:
		return {
			"GAME_MESSAGE_NEED_BRONZE_KEY_1",
			"GAME_MESSAGE_NEED_BRONZE_KEY_2",
			"GAME_MESSAGE_NEED_BRONZE_KEY_3",
		};
		break;
	case NEED_BEAR_STATUE:
		return {
			"GAME_MESSAGE_NEED_BEAR_STATUE_1",
			"GAME_MESSAGE_NEED_BEAR_STATUE_2",
		};
		break;
	case NEED_GOLD_KEY:
		return {
			"GAME_MESSAGE_NEED_GOLD_KEY_1",
			"GAME_MESSAGE_NEED_GOLD_KEY_2",
		};
		break;
	case NEED_FROG_STATUE:
		return {
			"GAME_MESSAGE_NEED_FROG_STATUE_1",
			"GAME_MESSAGE_NEED_FROG_STATUE_2",
		};
		break;
	case PLACARD_PIT_1:
		return {
			"GAME_MESSAGE_PLACARD_PIT_1_1",
			"GAME_MESSAGE_PLACARD_PIT_1_2",
		};
		break;
	case PLACARD_PIT_2:
		return {
			"GAME_MESSAGE_PLACARD_PIT_2_1",
			"GAME_MESSAGE_PLACARD_PIT_2_2",
		};
		break;
	case PLACARD_PIT_3:
		return {
			"GAME_MESSAGE_PLACARD_PIT_3_1",
			"GAME_MESSAGE_PLACARD_PIT_3_2",
		};
		break;
	case TURN_AROUND:
		return {
			"GAME_MESSAGE_TURN_AROUND_1",
			"GAME_MESSAGE_TURN_AROUND_2",
		};
		break;
	case TURN_LEFT:
		return {
			"GAME_MESSAGE_TURN_LEFT_1",
			"GAME_MESSAGE_TURN_LEFT_2",
		};
		break;
	case TURN_RIGHT:
		return {
			"GAME_MESSAGE_TURN_RIGHT_1",
			"GAME_MESSAGE_TURN_RIGHT_2",
		};
		break;
	case TESTING_GROUNDS:
		return {
			"GAME_MESSAGE_TESTING_GROUNDS_1",
			"GAME_MESSAGE_TESTING_GROUNDS_2",
			"GAME_MESSAGE_TESTING_GROUNDS_3",
			"GAME_MESSAGE_TESTING_GROUNDS_4",
		};
		break;
	case ALARM_BELLS:
		return {
			"GAME_MESSAGE_ALARM_BELLS_1",
			"GAME_MESSAGE_ALARM_BELLS_2",
			"GAME_MESSAGE_ALARM_BELLS_3",
			"GAME_MESSAGE_ALARM_BELLS_4",
		};
		break;
	case TREASURE_REPOSITORY:
		return {
			"GAME_MESSAGE_TREASURE_REPOSITORY_1",
			"GAME_MESSAGE_TREASURE_REPOSITORY_2",
		};
		break;
	case MONSTER_ALLOCATION_CENTRE:
		return {
			"GAME_MESSAGE_MONSTER_ALLOCATION_CENTRE_1",
			"GAME_MESSAGE_MONSTER_ALLOCATION_CENTRE_2",
		};
		break;
	case LARGE_DESK:
		return {
			"GAME_MESSAGE_LARGE_DESK_1",   "GAME_MESSAGE_LARGE_DESK_2",
			"GAME_MESSAGE_LARGE_DESK_3",   "GAME_MESSAGE_LARGE_DESK_4",
			"GAME_MESSAGE_LARGE_DESK_4_5", "GAME_MESSAGE_LARGE_DESK_4_6",
			"GAME_MESSAGE_LARGE_DESK_5",   "GAME_MESSAGE_LARGE_DESK_6",
			"GAME_MESSAGE_LARGE_DESK_6_5", "GAME_MESSAGE_LARGE_DESK_7",
			"GAME_MESSAGE_LARGE_DESK_8",   "GAME_MESSAGE_LARGE_DESK_9",
		};
		break;
	case TREBOR_VOICE:
		return {
			"GAME_MESSAGE_TREBOR_VOICE_1_1",   "GAME_MESSAGE_TREBOR_VOICE_1_2",
			"GAME_MESSAGE_TREBOR_VOICE_1_3",   "GAME_MESSAGE_TREBOR_VOICE_1_4",
			"GAME_MESSAGE_TREBOR_VOICE_1_5",   "GAME_MESSAGE_TREBOR_VOICE_2_1",
			"GAME_MESSAGE_TREBOR_VOICE_2_2",   "GAME_MESSAGE_TREBOR_VOICE_2_3",
			"GAME_MESSAGE_TREBOR_VOICE_2_4",   "GAME_MESSAGE_TREBOR_VOICE_2_5",
			"GAME_MESSAGE_TREBOR_VOICE_2_6",   "GAME_MESSAGE_TREBOR_VOICE_2_7",
			"GAME_MESSAGE_TREBOR_VOICE_2_7_5", "GAME_MESSAGE_TREBOR_VOICE_2_8",
			"GAME_MESSAGE_TREBOR_VOICE_2_9",   "GAME_MESSAGE_TREBOR_VOICE_2_10",
			"GAME_MESSAGE_TREBOR_VOICE_2_11",
		};
		break;
	case SERVICE_ELEVATOR:
		return {
			"GAME_MESSAGE_SERVICE_ELEVATOR_1",
			"GAME_MESSAGE_SERVICE_ELEVATOR_2",
			"GAME_MESSAGE_SERVICE_ELEVATOR_3",
		};
		break;
	case WERDNA_BOAST:
		return {
			"GAME_MESSAGE_WERDNA_BOAST_1_1", "GAME_MESSAGE_WERDNA_BOAST_1_2",
			"GAME_MESSAGE_WERDNA_BOAST_1_3", "GAME_MESSAGE_WERDNA_BOAST_1_35",
			"GAME_MESSAGE_WERDNA_BOAST_1_4", "GAME_MESSAGE_WERDNA_BOAST_1_5",
			"GAME_MESSAGE_WERDNA_BOAST_1_6", "GAME_MESSAGE_WERDNA_BOAST_1_7",
			"GAME_MESSAGE_WERDNA_BOAST_1_8", "GAME_MESSAGE_WERDNA_BOAST_2_1",
			"GAME_MESSAGE_WERDNA_BOAST_2_2", "GAME_MESSAGE_WERDNA_BOAST_2_3",
			"GAME_MESSAGE_WERDNA_BOAST_3_1",
		};
		break;
	case TURN_BACK:
		return {
			"GAME_MESSAGE_TURN_BACK_1",
			"GAME_MESSAGE_TURN_BACK_2",
			"GAME_MESSAGE_TURN_BACK_3",
		};
		break;
	case WERDNA_SIGN_IN:
		return {
			"GAME_MESSAGE_WERDNA_SIGN_1_1", "GAME_MESSAGE_WERDNA_SIGN_1_15",
			"GAME_MESSAGE_WERDNA_SIGN_1_2", "GAME_MESSAGE_WERDNA_SIGN_1_3",
			"GAME_MESSAGE_WERDNA_SIGN_1_4", "GAME_MESSAGE_WERDNA_SIGN_1_45",
			"GAME_MESSAGE_WERDNA_SIGN_1_5",
		};
		break;
	case WERDNA_SIGN_OUT:
		return {
			"GAME_MESSAGE_WERDNA_SIGN_2_1", "GAME_MESSAGE_WERDNA_SIGN_2_15",
			"GAME_MESSAGE_WERDNA_SIGN_2_2", "GAME_MESSAGE_WERDNA_SIGN_2_3",
			"GAME_MESSAGE_WERDNA_SIGN_2_4", "GAME_MESSAGE_WERDNA_SIGN_2_45",
			"GAME_MESSAGE_WERDNA_SIGN_2_5",
		};
		break;
	case THREE_HUMANOIDS:
		return {
			"GAME_MESSAGE_THREE_HUMANOIDS_1", "GAME_MESSAGE_THREE_HUMANOIDS_2",
			"GAME_MESSAGE_THREE_HUMANOIDS_3", "GAME_MESSAGE_THREE_HUMANOIDS_4",
			"GAME_MESSAGE_THREE_HUMANOIDS_5", "GAME_MESSAGE_THREE_HUMANOIDS_6",
			"GAME_MESSAGE_THREE_HUMANOIDS_7", "GAME_MESSAGE_THREE_HUMANOIDS_8",
		};
		break;
	case GETTING_WARM_1:
		return {"GAME_MESSAGE_GETTING_WARM_1_1"};
		break;
	case GETTING_WARM_2:
		return {"GAME_MESSAGE_GETTING_WARM_2_1"};
		break;
	case GETTING_WARM_3:
		return {"GAME_MESSAGE_GETTING_WARM_3_1"};
		break;
	case FIRE_DRAGONS_COMBAT:
		return {};
		break;
	case DEADLY_RING_COMBAT:
		return {};
		break;
	case WERDNA_COMBAT:
		return {};
		break;
	case NEED_BLUE_RIBBON:
		return {
			"GAME_MESSAGE_NEED_BLUE_RIBBON_1",
			"GAME_MESSAGE_NEED_BLUE_RIBBON_2",
			"GAME_MESSAGE_NEED_BLUE_RIBBON_3",
			"GAME_MESSAGE_NEED_BLUE_RIBBON_4",
		};
		break;
	default:
		return {};
	};
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

auto Sorcery::UI::_draw_level_name() -> void {

	auto text_cmp{components->get("engine_base_ui:level_name")};
	auto frame_cmp{components->get("engine_base_ui:level_name_frame")};

	with_Window(WINDOW_LAYER_TEXTS, nullptr, ImGuiWindowFlags_NoDecoration) {

		_draw_frame(&frame_cmp);
		_draw_text(&text_cmp, _ctx.game->state->level->name());
	}
}

auto Sorcery::UI::_draw_buffbar() -> void {
	auto cmp{components->get("engine_base_ui:buffbar")};
	auto frame_cmp{components->get("engine_base_ui:buffbar_frame")};

	const auto x{grid_x(cmp.x)};
	auto y{grid_y(cmp.y)};
	const auto scale{_ctx.display->get_display_metrics().scale};
	const auto width{cmp.w * grid_sz() * scale};
	const auto height{cmp.h * grid_sz()};

	auto tint{_ctx.controller->get_monochrome()
				  ? ImVec4{1.0f, 1.0f, 1.0f, _ctx.animation->fade}
				  : ImVec4{1.0f, 0.33f, 0.33f, _ctx.animation->fade}};

	with_Window(WINDOW_LAYER_TEXTS, nullptr, ImGuiWindowFlags_NoDecoration) {

		_draw_frame(&frame_cmp);
		ImGui::SetCursorPos(ImVec2{x, y});
		const auto light_idx{_ctx.game->state->get_lit() ? ICON_BUFF_EXTRA_LIGHT
														 : ICON_BUFF_LIGHT};
		_draw_fg_image_with_idx(WINDOW_LAYER_TEXTS, ICONS_TEXTURE, light_idx,
								ImVec2{x, y}, ImVec2{width, height}, tint);

		y += height;

		// TODO
	}
}

auto Sorcery::UI::_draw_icons() -> void {

	auto cmp{components->get("engine_base_ui:icons")};
	auto frame_cmp{components->get("engine_base_ui:icons_frame")};

	constexpr std::array icons{
		ICON_CAMP, ICON_PARTY, ICON_MAP, ICON_LOOK, ICON_CAST, ICON_USE,
	};

	const auto x{grid_x(cmp.x)};
	const auto start_y{grid_y(cmp.y)};

	const auto scale{_ctx.display->get_display_metrics().scale};
	const auto width{static_cast<float>(cmp.w * grid_sz()) * scale};
	const auto height{static_cast<float>(cmp.h * grid_sz()) * scale};

	const ImVec2 icon_size{width, height};

	const auto normal_tint{
		_ctx.controller->get_monochrome()
			? ImVec4{1.0f, 1.0f, 1.0f, _ctx.animation->fade}
			: ImVec4{0.33f, 1.0f, 1.0f, _ctx.animation->fade}};

	const auto hovered_tint{ImVec4{get_hl_colour(_ctx.animation->lerp)}};

	// Passive frame.
	with_Window(WINDOW_LAYER_TEXTS, nullptr,
				ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs) {

		_draw_frame(&frame_cmp);
	}

	// Interactive icons.
	with_Window(WINDOW_LAYER_MENUS, nullptr,
				ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoTitleBar) {

		auto y{start_y};

		for (const auto icon_idx : icons) {
			const ImVec2 icon_pos{x, y};

			ImGui::SetCursorPos(icon_pos);
			ImGui::PushID(icon_idx);

			const auto activated{ImGui::InvisibleButton("##icon", icon_size)};

			const auto hovered{ImGui::IsItemHovered()};
			const auto tint{hovered ? hovered_tint : normal_tint};

			_draw_fg_image_with_idx(WINDOW_LAYER_MENUS, ICONS_TEXTURE, icon_idx,
									icon_pos, icon_size, tint);

			if (activated)
				_ctx.controller->handle_icon_click(icon_idx);

			ImGui::PopID();

			y += height + 3.0f;
		}
	}
}

auto Sorcery::UI::_draw_save() -> void {

	auto cmp{components->get("engine_base_ui:save")};
	auto frame_cmp{components->get("engine_base_ui:save_frame")};

	const auto x{grid_x(cmp.x)};
	const auto y{grid_y(cmp.y)};

	const auto width{static_cast<float>(cmp.w * grid_sz())};
	const auto height{static_cast<float>(cmp.h * grid_sz())};

	const ImVec2 save_pos{x, y};
	const ImVec2 save_size{width, height};

	const auto normal_tint{
		_ctx.controller->get_monochrome()
			? ImVec4{1.0f, 1.0f, 1.0f, _ctx.animation->fade}
			: ImVec4{0.33f, 1.0f, 1.0f, _ctx.animation->fade}};

	const auto hovered_tint{ImVec4{get_hl_colour(_ctx.animation->lerp)}};

	// Passive frame.
	with_Window(WINDOW_LAYER_TEXTS, nullptr,
				ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs) {

		_draw_frame(&frame_cmp);
	}

	// Interactive save icon.
	with_Window(WINDOW_LAYER_MENUS, nullptr,
				ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoTitleBar) {

		ImGui::SetCursorPos(save_pos);

		const auto activated{
			ImGui::InvisibleButton("##save_and_quit", save_size)};

		const auto hovered{ImGui::IsItemHovered()};
		const auto tint{hovered ? hovered_tint : normal_tint};

		_draw_fg_image_with_idx(WINDOW_LAYER_MENUS, ICONS_TEXTURE,
								ICON_SAVE_AND_QUIT, save_pos, save_size, tint);

		if (activated)
			_ctx.game->save_game();
	}
}

auto Sorcery::UI::_draw_compass() -> void {

	auto cmp{components->get("engine_base_ui:compass")};
	auto frame_cmp{components->get("engine_base_ui:compass_frame")};

	auto tint{_ctx.controller->get_monochrome()
				  ? ImVec4{1.0f, 1.0f, 1.0f, _ctx.animation->fade}
				  : ImVec4{1.0f, 0.33f, 0.33f, _ctx.animation->fade}};

	const auto x{grid_x(cmp.x)};
	const auto y{grid_y(cmp.y)};
	const auto scale{_ctx.display->get_display_metrics().scale};
	const auto width{cmp.w * grid_sz() * scale};
	const auto height{cmp.h * grid_sz() * scale};

	with_Window(WINDOW_LAYER_TEXTS, nullptr, ImGuiWindowFlags_NoDecoration) {

		_draw_frame(&frame_cmp);
		ImGui::SetCursorPos(ImVec2{x, y});
		with_Child("compass_child", ImVec2(width, height)) {
			auto icon_idx{ICON_COMPASS_NORTH};
			switch (_ctx.game->state->get_player_facing()) {
				using enum Enums::Map::Direction;
			case NORTH:
				icon_idx = ICON_COMPASS_NORTH;
				break;
			case SOUTH:
				icon_idx = ICON_COMPASS_SOUTH;
				break;
			case EAST:
				icon_idx = ICON_COMPASS_EAST;
				break;
			case WEST:
				icon_idx = ICON_COMPASS_WEST;
				break;
			default:
				icon_idx = ICON_COMPASS_NORTH;
				break;
			}

			_draw_fg_image_with_idx(WINDOW_LAYER_TEXTS, ICONS_TEXTURE, icon_idx,
									ImVec2{x, y}, ImVec2{width, height}, tint);
		}
	}
}

auto Sorcery::UI::_draw_party_panel() -> void {

	auto cmp{components->get("global:party_panel")};
	auto frame_cmp{components->get("engine_base_ui:party_frame")};

	const auto width{static_cast<float>(cmp.w * grid_sz())};
	const auto height{static_cast<float>(cmp.h * grid_sz())};

	const auto x{cmp.x == -1 ? (ImGui::GetMainViewport()->Size.x - width) / 2.0f
							 : grid_x(cmp.x)};

	const auto y{grid_y(cmp.y)};
	const ImVec2 panel_pos{x, y};
	const ImVec2 panel_size{width, height};

	// The frame itself is passive.
	with_Window(WINDOW_LAYER_TEXTS, nullptr,
				ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs) {

		_draw_frame(&frame_cmp);
	}

	// Interactive panel content must be on the menu/input layer.
	with_Window(WINDOW_LAYER_MENUS, nullptr,
				ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoTitleBar) {

		ImGui::SetCursorPos(panel_pos);

		with_Child("party_panel_child", panel_size, ImGuiChildFlags_None,
				   ImGuiWindowFlags_NoScrollbar |
					   ImGuiWindowFlags_NoScrollWithMouse) {

			const auto font{fontstore->get_current_font(cmp.font).value()};

			set_Font(font, font_sz());

			UIStyle::set_text_bright(_ctx);

			ImGui::TextUnformatted(
				_ctx.get_string("PARTY_PANEL_LEGEND").c_str());

			if (!_ctx.game->state->party_has_members())
				return;

			const auto row_height{static_cast<float>(grid_sz())};
			const auto highlight_colour{
				ImVec4{get_hl_colour(_ctx.animation->lerp)}};

			set_StyleColor(ImGuiCol_HeaderHovered, highlight_colour);

			set_StyleColor(ImGuiCol_HeaderActive, highlight_colour);

			auto position{1u};

			for (const auto party{_ctx.game->state->get_party_characters()};
				 const auto character_id : party) {

				auto &character{_ctx.game->characters.at(character_id)};

				const auto summary{character.get_party_panel_text(position)};

				const auto text_colour{_get_status_color(&character)};

				// Preserve the original fixed row positions.
				ImGui::SetCursorPosY(static_cast<float>(position) * row_height);

				const auto row_screen_pos{ImGui::GetCursorScreenPos()};

				ImGui::PushID(character_id);

				const auto activated{
					ImGui::Selectable("##party_member", false,
									  ImGuiSelectableFlags_SpanAvailWidth,
									  ImVec2{0.0f, row_height})};

				ImGui::PopID();

				// Draw after the Selectable so the text appears above its
				// hover background.
				const auto text_size{ImGui::CalcTextSize(summary.c_str())};

				const ImVec2 text_pos{row_screen_pos.x,
									  row_screen_pos.y +
										  ((row_height - text_size.y) / 2.0f)};

				ImGui::GetWindowDrawList()->AddText(
					font, font_sz(), text_pos,
					ImGui::ColorConvertFloat4ToU32(ImVec4{text_colour}),
					summary.c_str());

				if (activated)
					_ctx.controller->inspect_party_member(character_id);

				++position;
			}
		}
	}
}

auto Sorcery::UI::_get_status_color(Character *character) const -> ImVec4 {

	switch (character->get_status()) {
		using enum Enums::Character::Status;
	case OK:
		if (character->get_poisoned_rate() > 0)
			return ImVec4{0.0f, 1.0f, 0.0f, _ctx.animation->fade};
		else if (character->get_max_hp() / character->get_current_hp() > 10)
			return ImVec4{1.0f, 0.0f, 0.0f, _ctx.animation->fade};
		else if (character->get_max_hp() / character->get_current_hp() > 5)
			return ImVec4{1.0f, 1.0f, 0.0f, _ctx.animation->fade};
		else
			return ImVec4{1.0f, 1.0f, 1.0f, _ctx.animation->fade};
		break;
	case AFRAID:
		[[fallthrough]];
	case SILENCED:
		[[fallthrough]];
	case ASLEEP:
		if (character->get_max_hp() / character->get_current_hp() > 10)
			return ImVec4{1.0f, 0.0f, 0.0f, _ctx.animation->fade};
		else if (character->get_max_hp() / character->get_current_hp() > 5)
			return ImVec4{1.0f, 1.0f, 0.0f, _ctx.animation->fade};
		else
			return ImVec4{1.0f, 1.0f, 1.0f, _ctx.animation->fade};
		break;
	case ASHES:
		return ImVec4{1.0f, 0.0f, 0.0f, _ctx.animation->fade};
		break;
	case DEAD:
		return ImVec4{1.0f, 0.0f, 0.0f, _ctx.animation->fade};
		break;
	case HELD:
		return ImVec4{0.5f, 0.0f, 0.5f, _ctx.animation->fade};
		break;
	case LOST:
		return ImVec4{1.0f, 0.0f, 0.0f, _ctx.animation->fade};
		break;
	case STONED:
		return ImVec4{0.5f, 0.5f, 0.5f, _ctx.animation->fade};
		break;
	default:
		return ImVec4{1.0f, 1.0f, 1.0f, _ctx.animation->fade};
		break;
	}
}

auto Sorcery::UI::_draw_spell_info() -> void {

	const auto idx{_ctx.get_selected("spellbook_selected")};
	if (idx == 50)
		return;

	auto cmp{components->get("spellbook:spell_data")};
	auto pos{grid_pos(cmp.x, cmp.y)};
	ImGui::SetNextWindowPos(pos);
	with_Window(WINDOW_LAYER_TEXTS, nullptr, ImGuiWindowFlags_NoDecoration) {
		with_Child("spell_child",
				   ImVec2(grid_sz() * cmp.w, grid_sz() * cmp.h)) {

			auto spell{_ctx.resources->spells->get(
				enum_cast<Enums::Magic::SpellID>(idx).value())};

			const auto spell_name{
				std::format("{} \"{}\"", spell.name, spell.translated_name)};
			const auto spell_type{spell.type == Enums::Magic::SpellType::ARCANE
									  ? "Mage"
									  : "Priest"};

			std::string spell_cat{enum_name(spell.category)};
			std::transform(spell_cat.begin(), spell_cat.end(),
						   spell_cat.begin(), ::tolower);
			auto summary{std::format("Level {} {} {} spell", spell.level,
									 spell_type, spell_cat)};

			{
				UIStyle::set_text_bright(_ctx);
				set_Font(fontstore->get_current_font(cmp.font).value(),
						 font_sz());
				ImGui::TextUnformatted(spell_name.c_str());
				ImGui::NewLine();
				ImGui::TextUnformatted(summary.c_str());
				ImGui::NewLine();
			}

			set_Font(fontstore->get_current_font(cmp.font).value(), font_sz());
			UIStyle::set_text_dim(_ctx);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-security"
			ImGui::TextWrapped(spell.details.c_str());
#pragma GCC diagnostic pop
		}
	}
}

auto Sorcery::UI::_draw_monster_info() -> void {
	// Custom Rendering
	const auto idx{_ctx.get_selected("bestiary_selected")};
	const auto mon{_ctx.resources->monsters->get(idx)};
	const auto k_gfx{mon.get_known_gfx()};
	const auto u_gfx{mon.get_unknown_gfx()};
	auto k_mg_c{components->get("bestiary:known_monster_graphic")};
	auto u_mg_c{components->get("bestiary:unknown_monster_graphic")};
	auto k_mg_pos{grid_pos(k_mg_c.x, k_mg_c.y)};
	auto u_mg_pos{grid_pos(u_mg_c.x, u_mg_c.y)};
	const auto scale{_ctx.display->get_display_metrics().scale};
	_draw_fg_image_with_idx(KNOWN_CREATURES_TEXTURE, k_gfx, k_mg_pos,
							ImVec2{k_mg_c.get_float("tile_width") * scale,
								   k_mg_c.get_float("tile_width") * scale});
	_draw_fg_image_with_idx(UNKNOWN_CREATURES_TEXTURE, u_gfx, u_mg_pos,
							ImVec2{u_mg_c.get_float("tile_width") * scale,
								   u_mg_c.get_float("tile_width") * scale});

	auto cmp{components->get("bestiary:monster_data")};
	auto pos{grid_pos(cmp.x, cmp.y)};

	with_Window(WINDOW_LAYER_MENUS, nullptr, ImGuiWindowFlags_NoDecoration) {
		const auto name{std::format("  {:>03}:{}/{}", idx, mon.get_known_name(),
									mon.get_unknown_name())};
		ImGui::SetCursorPos(pos);
		with_Child("bestiary_tab_bar_child",
				   ImVec2(grid_sz() * cmp.w, grid_sz() * cmp.h)) {
			UIStyle::set_tab_black(_ctx);
			set_Font(fontstore->get_current_font(cmp.font).value(), font_sz());
			ImGuiTabBarFlags tb_flags{ImGuiTabBarFlags_None};
			with_TabBar("bestiary_tab_bar", tb_flags) {
				with_TabItem("Info") {
					{
						UIStyle::set_text_bright(_ctx);
						ImGui::TextUnformatted(name.c_str());
					}

					UIStyle::set_text_dim(_ctx);

					const std::string mon_type{enum_name(mon.get_category())};
					auto mon_t{CAPITALISE(mon_type)};
					const std::string mon_class{
						mon.get_class() != Enums::Monsters::Class::NO_CLASS
							? enum_name(mon.get_class())
							: ""};
					auto mon_c{CAPITALISE(mon_class)};
					std::string type;
					if (mon_c.length() > 0)
						type = std::format(" Type:{} ({})", mon_t, mon_c);
					else
						type = std::format(" Type:{}", mon_t);
					const auto level{std::format("Level:{}", mon.get_level())};
					const auto xp{std::format("   XP:{}", mon.get_xp())};
					const auto group{
						std::format("Group:{}", mon.get_group_size().str())};
					ImGui::TextUnformatted(type.c_str());
					ImGui::TextUnformatted(level.c_str());
					ImGui::TextUnformatted(xp.c_str());
					ImGui::TextUnformatted(group.c_str());
				}
				with_TabItem("Stats") {

					{
						UIStyle::set_text_bright(_ctx);
						ImGui::TextUnformatted(name.c_str());
					}

					UIStyle::set_text_dim(_ctx);

					const auto atks{
						std::format(" Atks:{}", mon.get_attacks_str())};
					const auto ac{
						std::format("   Ac:{}", mon.get_armour_class())};
					const auto sr{
						std::format("   SR:{}%", mon.get_spell_resistance())};
					const auto hd{
						std::format("   HD:{}", mon.get_hit_dice().str())};

					ImGui::TextUnformatted(atks.c_str());
					ImGui::TextUnformatted(ac.c_str());
					ImGui::TextUnformatted(hd.c_str());
					ImGui::TextUnformatted(sr.c_str());
				}
				with_TabItem("Traits") {
					{
						UIStyle::set_text_bright(_ctx);
						ImGui::TextUnformatted(name.c_str());
					}

					UIStyle::set_text_dim(_ctx);

					auto traits{mon.get_traits()};
					if (traits.length() > 0) {
						// TRIM(traits);

						std::vector<std::string> strings;
						const std::regex regex(R"([|]+)");
						std::sregex_token_iterator it{traits.begin(),
													  traits.end(), regex, -1};
						std::vector<std::string> split{it, {}};
						split.erase(std::remove_if(split.begin(), split.end(),
												   [](std::string const &s) {
													   return s.size() == 0;
												   }),
									split.end());
						strings = split;

						for (const auto &each_string : strings)
							ImGui::TextUnformatted(each_string.c_str());
					}
				}
				with_TabItem("Weaknesses") {

					{
						UIStyle::set_text_bright(_ctx);
						ImGui::TextUnformatted(name.c_str());
					}

					UIStyle::set_text_dim(_ctx);

					auto weaknesses{mon.get_weaknesses()};
					ImGui::TextUnformatted(weaknesses.c_str());
				}
			}
		}
	}
}

auto Sorcery::UI::_display_bestiary() -> void {

	_draw_components("bestiary");
	_draw_bg_video();
	_draw_monster_info();
	_draw_cursor();
}

auto Sorcery::UI::_display_compendium() -> void {

	_draw_components("compendium");
	_draw_bg_video();
	_draw_cursor();
}

auto Sorcery::UI::_display_edge_of_town() -> void {
	_draw_components("edge_of_town");
	dialog_leave->display(_ctx.controller->want_to_leave_game());
	_draw_party_panel();
	_draw_debug();
	_draw_cursor();
}

auto Sorcery::UI::_display_castle() -> void {
	_draw_components("castle");
	dialog_leave->display(_ctx.controller->want_to_leave_game());
	_draw_party_panel();
	_draw_debug();
	_draw_cursor();
}

auto Sorcery::UI::_display_training_grounds() -> void {
	_draw_components("training_grounds");
	_draw_cursor();
}

auto Sorcery::UI::_display_edit() -> void {
	_draw_components("edit");
	_draw_cursor();
}

auto Sorcery::UI::_display_reclass() -> void {
	_draw_components("change_class");
	_draw_reclass();
	notice_reclassed_ok->display(_ctx.get_flag_ref("want_reclassed_ok"));
	_draw_cursor();
}

auto Sorcery::UI::_display_rename() -> void {
	_draw_components("rename");
	_draw_rename();
	notice_renamed_ok->display(_ctx.get_flag_ref("want_renamed_ok"));
	_draw_cursor();
}

auto Sorcery::UI::_display_roster() -> void {
	_draw_components("roster");
	_draw_cursor();
}

auto Sorcery::UI::_display_select() -> void {
	_draw_components("select");
	_draw_cursor();
}

auto Sorcery::UI::_display_retrain() -> void {
	_draw_components("retrain");
	_draw_cursor();
}

auto Sorcery::UI::_display_delete() -> void {
	_draw_components("delete");
	if (dialog_delete->show)
		dialog_delete->display(_ctx.get_flag_ref("want_delete_ok"));
	_draw_cursor();
}

auto Sorcery::UI::_display_legate() -> void {
	_draw_components("legate");
	_draw_cursor();
}

auto Sorcery::UI::_display_reorder(const int mode) -> void {
	_draw_components("reorder", mode);
	_draw_cursor();
}

auto Sorcery::UI::_display_choose(const int mode) -> void {
	_draw_components("choose", mode);
	_draw_choose(mode);
	_draw_cursor();
}

auto Sorcery::UI::_display_inspect(const int mode) -> void {

	_draw_components("inspect", mode);
	if (mode & INSPECT_MODE_ACTIONS)
		_draw_components("inspect_actions", mode);
	_draw_current_character(mode);
	if (modal_identify->show)
		modal_identify->display(_ctx.get_flag_ref("want_identify"));
	if (modal_equip->show)
		modal_equip->display(_ctx.get_flag_ref("want_equip"));
	if (modal_remove->show)
		modal_remove->display(_ctx.get_flag_ref("want_remove"));
	if (modal_spell->show)
		modal_spell->display(_ctx.get_flag_ref("want_spell"));
	if (modal_drop->show)
		modal_drop->display(_ctx.get_flag_ref("want_drop"));
	if (modal_trade->show)
		modal_trade->display(_ctx.get_flag_ref("want_trade"));
	if (modal_give->show)
		modal_give->display(_ctx.get_flag_ref("want_give"));
	if (modal_use->show)
		modal_use->display(_ctx.get_flag_ref("want_use"));
	if (modal_invoke->show)
		modal_invoke->display(_ctx.get_flag_ref("want_invoke"));
	if (notice_pool_gold->show)
		notice_pool_gold->display(_ctx.get_flag_ref("want_pool_gold"));
	_draw_debug();
	_draw_cursor();
}

auto Sorcery::UI::_display_spellbook() -> void {

	_draw_components("spellbook");
	_draw_bg_video();
	_draw_spell_info();
	_draw_cursor();
}

auto Sorcery::UI::_display_graveyard() -> void {

	_draw_components("graveyard");
	_draw_party_wipe();
	_draw_cursor();
}

auto Sorcery::UI::_display_victory() -> void {

	_draw_components("victory");
	_draw_cursor();
}

auto Sorcery::UI::_display_automap() -> void {

	_draw_components("automap");
	_draw_current_level_map();

	auto legend{components->get("automap:automap_legend")};
	_draw_automap_legend(&legend);

	_draw_cursor();
}

auto Sorcery::UI::_display_atlas() -> void {

	_draw_components("atlas");
	_draw_bg_video();
	_draw_level_no_player();
	_draw_cursor();
}

auto Sorcery::UI::_display_museum() -> void {

	_draw_components("museum");
	_draw_bg_video();
	_draw_item_info();
	_draw_cursor();
}

auto Sorcery::UI::_display_inn() -> void {
	_draw_components("inn");
	_draw_party_panel();
	modal_inspect->display(_ctx.get_flag_ref("want_inspect"));
	modal_equip->display(_ctx.get_flag_ref("want_equip"));
	modal_remove->display(_ctx.get_flag_ref("want_remove"));
	modal_spell->display(_ctx.get_flag_ref("want_spell"));
	modal_identify->display(_ctx.get_flag_ref("want_identify"));
	modal_drop->display(_ctx.get_flag_ref("want_drop"));
	modal_give->display(_ctx.get_flag_ref("want_give"));
	modal_trade->display(_ctx.get_flag_ref("want_trade"));
	modal_use->display(_ctx.get_flag_ref("want_use"));
	modal_invoke->display(_ctx.get_flag_ref("want_invoke"));
	notice_pool_gold->display(_ctx.get_flag_ref("want_pool_gold"));
	_draw_debug();
	_draw_cursor();
}

auto Sorcery::UI::_display_shop() -> void {
	_draw_components("shop");
	_draw_party_panel();
	_draw_debug();
	_draw_cursor();
}

auto Sorcery::UI::_display_stay() -> void {
	_draw_components("stay");
	_draw_stay();
	_draw_party_panel();
	_draw_debug();
	_draw_cursor();
}

auto Sorcery::UI::_display_buy() -> void {
	_draw_components("buy");
	_draw_buy();
	_draw_party_panel();
	_draw_debug();
	_draw_cursor();
}

auto Sorcery::UI::_display_sell() -> void {
	_draw_components("sell");
	_draw_sell();
	_draw_party_panel();
	_draw_debug();
	_draw_cursor();
}

auto Sorcery::UI::_display_identify() -> void {
	_draw_components("identify");
	_draw_identify();
	_draw_party_panel();
	_draw_debug();
	_draw_cursor();
}

auto Sorcery::UI::_display_uncurse() -> void {
	_draw_components("uncurse");
	_draw_uncurse();
	_draw_party_panel();
	_draw_debug();
	_draw_cursor();
}

auto Sorcery::UI::_display_store() -> void {
	_draw_components("store");
	_draw_store();
	_draw_party_panel();
	notice_pool_gold->display(_ctx.get_flag_ref("want_pool_gold"));
	_draw_debug();
	_draw_cursor();
}

auto Sorcery::UI::_display_chest(const int stage) -> void {

	_draw_components("chest");
	_draw_chest(enum_cast<Enums::Chests::State>(stage).value());

	// Transient overlay
	_draw_transient();

	if (modal_chest->show)
		modal_chest->display(_ctx.get_flag_ref("want_chest"));
	_draw_cursor();
}

auto Sorcery::UI::_display_recovery(const int mode) -> void {
	_draw_components("recovery");
	_draw_recovery(mode);
	_draw_party_panel();
	_draw_debug();
	_draw_cursor();
}

auto Sorcery::UI::_display_heal(int stage) -> void {
	_draw_components("heal");
	_draw_heal(stage);
	_draw_party_panel();
	_draw_debug();
	_draw_cursor();
}

auto Sorcery::UI::_display_rite(int stage) -> void {
	_draw_components("rite");
	if (dialog_rite->show)
		dialog_rite->display(_ctx.get_flag_ref("want_rite_ok"));
	_draw_rite(stage);
	_draw_cursor();
}

auto Sorcery::UI::_display_no_level_up(const int mode) -> void {
	_draw_components("nolevelup");
	_draw_no_level_up(mode);
	_draw_party_panel();
	_draw_debug();
	_draw_cursor();
}

auto Sorcery::UI::_display_level_up(const int mode) -> void {

	_draw_components("levelup");
	_draw_level_up(mode);
	_draw_party_panel();
	_draw_debug();
	_draw_cursor();
}

auto Sorcery::UI::_display_tavern() -> void {

	_draw_components("tavern");
	notice_divvy->display(_ctx.get_flag_ref("want_divvy_gold"));
	notice_pool_gold->display(_ctx.get_flag_ref("want_pool_gold"));
	modal_inspect->display(_ctx.get_flag_ref("want_inspect"));
	modal_equip->display(_ctx.get_flag_ref("want_equip"));
	modal_remove->display(_ctx.get_flag_ref("want_remove"));
	modal_spell->display(_ctx.get_flag_ref("want_spell"));
	modal_identify->display(_ctx.get_flag_ref("want_identify"));
	modal_drop->display(_ctx.get_flag_ref("want_drop"));
	modal_use->display(_ctx.get_flag_ref("want_use"));
	modal_give->display(_ctx.get_flag_ref("want_give"));
	modal_trade->display(_ctx.get_flag_ref("want_trade"));
	modal_invoke->display(_ctx.get_flag_ref("want_invoke"));
	_draw_party_panel();
	_draw_debug();
	_draw_cursor();
}

auto Sorcery::UI::_display_pay() -> void {

	_draw_components("pay");
	_draw_pay_info();
	_draw_party_panel();
	_draw_debug();
	_draw_cursor();
}

auto Sorcery::UI::_display_temple() -> void {

	_draw_components("temple");
	_draw_party_panel();
	_draw_debug();
	_draw_cursor();
}

auto Sorcery::UI::_display_remove() -> void {
	_draw_components("remove");
	_draw_cursor();
}

auto Sorcery::UI::_display_restart() -> void {
	_draw_components("restart");
	_draw_cursor();
}

auto Sorcery::UI::_display_add() -> void {
	_draw_components("add");
	_draw_cursor();
}

auto Sorcery::UI::_display_options() -> void {

	_draw_components("options");
	_draw_bg_video();
	_draw_options();
	_draw_cursor();
}

auto Sorcery::UI::_display_license(const std::string &string) -> void {

	_draw_components("license");
	_draw_bg_video();

	auto component{components->get("license:license_info")};
	_draw_license(&component, string);
	_draw_cursor();
}

auto Sorcery::UI::_draw_current_level_map() -> void {

	const auto level{_ctx.game->state->level.get()};
	if (!level)
		return;

	const auto depth{_ctx.game->state->get_depth()};
	const auto explored_it{_ctx.game->state->explored.find(depth)};
	if (explored_it == _ctx.game->state->explored.end())
		return;

	const auto &explored{explored_it->second};

	constexpr auto tc{20};
	const auto map_c{components->get("automap:map_graphic")};
	const ImVec2 top_left_pos{grid_pos(map_c.x, map_c.y)};

	const auto scale{_ctx.display->get_display_metrics().scale};
	const auto spacing{map_c.get_int("tile_spacing") * scale};
	const ImVec2 tile_sz{map_c.get_int("tile_size") * scale,
						 map_c.get_int("tile_size") * scale};

	// Remember to flip in Y-direction as (0,0) is at bottom left of map
	const auto reverse_y{(tile_sz.x * tc) + ((tc - 1) * spacing) + 2};

	for (auto y = 0; y < tc; ++y) {
		for (auto x = 0; x < tc; ++x) {

			if (const Coordinate loc{x, y}; !explored.at(loc))
				continue;

			const auto &tile{level->at(x, y)};
			const auto tile_x{(x * tile_sz.x) + (x * spacing)};
			const auto tile_y{(y * tile_sz.y) + (y * spacing)};

			const ImVec2 tile_pos{top_left_pos.x + tile_x,
								  top_left_pos.y + reverse_y - tile_y};

			_draw_map_tile(tile, tile_pos, tile_sz);
		}
	}

	auto player_icon{ICON_COMPASS_NORTH};
	auto tint{ImVec4{0.33f, 1.0f, 1.0f, _ctx.animation->fade}};

	switch (_ctx.game->state->get_player_facing()) {
		using enum Enums::Map::Direction;

	case NORTH:
		player_icon = ICON_COMPASS_NORTH;
		break;
	case SOUTH:
		player_icon = ICON_COMPASS_SOUTH;
		break;
	case EAST:
		player_icon = ICON_COMPASS_EAST;
		break;
	case WEST:
		player_icon = ICON_COMPASS_WEST;
		break;
	default:
		break;
	}

	const auto player_pos{_ctx.game->state->get_player_pos()};

	const auto player_tile_x{(player_pos.x * tile_sz.x) +
							 (player_pos.x * spacing)};
	const auto player_tile_y{(player_pos.y * tile_sz.y) +
							 (player_pos.y * spacing)};

	const ImVec2 player_draw_pos{top_left_pos.x + player_tile_x,
								 top_left_pos.y + reverse_y - player_tile_y};

	_draw_fg_image_with_idx(WINDOW_LAYER_TEXTS, ICONS_TEXTURE, player_icon,
							player_draw_pos, tile_sz, tint);
}

auto Sorcery::UI::_draw_level_no_player() -> void {

	// Menu Selection for B1F to B10F is 0 to 0, thus convert it into -1 to
	// -10 for depth
	if (_ctx.get_selected("atlas_selected") == 10)
		return;

	const auto depth{-1 - _ctx.get_selected("atlas_selected")};
	Level level{_ctx.resources->levels->get(depth).value()};

	// Work out where and how to draw the grid
	auto tc{20};
	const auto map_c{components->get("atlas:map_graphic")};
	ImVec2 top_left_pos{grid_pos(map_c.x, map_c.y)};
	const auto spacing{map_c.get_int("tile_spacing")};
	const auto scale{_ctx.display->get_display_metrics().scale};
	ImVec2 tile_sz{map_c.get_int("tile_size") * scale,
				   map_c.get_int("tile_size") * scale};

	// Remember to flip in Y-direction as (0,0) is at bottom left of map
	const auto reverse_y{(tile_sz.x * tc) + ((tc - 1) * spacing) + 2};
	auto tcx{0};
	auto tcy{0};

	// Draw Map
	for (auto y = 0; y <= 19; y++) {
		for (auto x = 0; x <= 19; x++) {
			const auto tile{level.at(x, y)};
			const auto tile_x{(tcx * tile_sz.x) + (tcx * spacing)};
			const auto tile_y{(tcy * tile_sz.y) + (tcy * spacing)};
			const auto tile_pos{ImVec2{top_left_pos.x + tile_x,
									   top_left_pos.y + reverse_y - tile_y}};
			_draw_map_tile(tile, tile_pos, tile_sz);
			++tcx;
		}
		++tcy;
		tcx = 0;
	}
}

auto Sorcery::UI::_draw_loading_progress() -> void {

	auto pb_c{components->get("splash:progress_bar")};

	const auto width{pb_c.w * grid_sz()};
	const float progress{static_cast<float>(images->progress - 1) /
						 static_cast<float>(images->capacity)};
	const auto x{std::invoke([&] {
		if (pb_c.x == -1) {
			const auto viewport{ImGui::GetMainViewport()};
			return (viewport->Size.x - width) / 2;
		} else
			return grid_x(pb_c.x);
	})};
	const auto y{grid_y(pb_c.y)};

	with_Window(WINDOW_LAYER_IMAGES, nullptr,
				ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoTitleBar) {
		set_Font(fontstore->get_default_font(), font_sz());
		set_StyleColor(ImGuiCol_PlotHistogram,
					   ImGui::GetColorU32(ImGuiCol_ButtonHovered));
		ImGui::SetCursorPos(ImVec2{x, y});

		ImGui::ProgressBar(progress, ImVec2(width, 4), "");
	}
}

auto Sorcery::UI::_draw_bg_video() -> void {

	auto elapsed_sec{(SDL_GetTicks() - ticks) / 1000.0};
	vfx_player->update(elapsed_sec);
	vfx_player->render(WINDOW_LAYER_BG);
}

auto Sorcery::UI::_display_main_menu() -> void {

	_draw_components("main_menu");
	_draw_attract_mode();
	_draw_bg_video();

	dialog_exit->display(_ctx.get_flag_ref("want_exit_game"));
	dialog_new->display(_ctx.get_flag_ref("want_new_game"));
	dialog_leave->display(_ctx.controller->want_to_leave_game());

	_draw_cursor();

	// bool show = true;
	// ImGui::PushFont(fontstore->get_default_font());
	// ImGui::ShowDemoWindow(&show);
	// ImGui::PopFont();

	ImGui::SetNextWindowPos(ImVec2{1, 1});
	ImGui::SetNextWindowSize(ImVec2{1000, 1000});
}

auto Sorcery::UI::_draw_attract_mode() -> void {

	// Get the Attract Data
	const auto attract{components->get("main_menu:attract_mode")};
	_attract_data = _ctx.animation->get_attract_data();

	// Work out the size and this where to draw it- (as its centred)!
	const auto scale{_ctx.display->get_display_metrics().scale};
	auto am_size{_attract_data.size() *
				 std::stoi(attract.get("tile_width").value()) * scale};
	am_size += (_attract_data.size() - 1) *
			   std::stoi(attract.get("tile_spacing").value()) * scale;
	const auto viewport{ImGui::GetMainViewport()};
	auto tile_pos{ImVec2{(viewport->Size.x - am_size) / 2, grid_y(attract.y)}};

	// And draw each tile (this will draw to the correct layer)
	for (auto idx : _attract_data) {

		_draw_fg_image_with_idx(
			KNOWN_CREATURES_TEXTURE, idx, tile_pos,
			ImVec2{attract.get_float("tile_width") * scale,
				   attract.get_float("tile_width") * scale});
		tile_pos.x += (attract.get_float("tile_width") * scale +
					   attract.get_float("tile_spacing") * scale);
	}
}

// Draw a Frame (a Rect) to the current window
auto Sorcery::UI::draw_frame(const ImVec2 p_min, const ImVec2 p_max,
							 const ImVec4 colour, const int rounding) -> void {
	// Work out where to draw it on the screen
	const auto adj{rounding / 2.0f};
	const auto fr_min{ImVec2{p_min.x + adj, p_min.y + adj}};
	const auto fr_max{ImVec2{p_max.x - adj, p_max.y - adj}};
	const ImU32 col{ImColor{colour}};
	const ImU32 bg{ImColor{ImVec4{0.0f, 0.0f, 0.0f, _ctx.animation->fade}}};

	// Black Background and Colour Foreground
	ImGui::GetWindowDrawList()->AddRectFilled(p_min, p_max, bg,
											  static_cast<float>(rounding));
	ImGui::GetWindowDrawList()->AddRect(fr_min, fr_max, col,
										static_cast<float>(rounding),
										static_cast<float>(rounding));
}

auto Sorcery::UI::_get_legacy_menu_ui_flags(const std::string_view name)
	-> std::vector<std::reference_wrapper<bool>> {

	using Flags = std::vector<std::reference_wrapper<bool>>;

	constexpr auto UI_FLAGS_COUNT{26};

	const std::array<std::pair<std::string_view, Flags>, UI_FLAGS_COUNT> flags{{
		{"tavern_menu", {std::ref(notice_divvy->show)}},
		{"store_menu", {std::ref(notice_pool_gold->show)}},
		{"temple_menu",
		 {std::ref(modal_help->show), std::ref(modal_tithe->show)}},
		{"camp_menu", {std::ref(modal_camp->show)}},
		{"top_elevator_menu", {std::ref(modal_elevator_top->show)}},
		{"bottom_elevator_menu", {std::ref(modal_elevator_bottom->show)}},
		{"inspect_menu", {std::ref(modal_inspect->show)}},
		{"roster_menu", {std::ref(modal_inspect->show)}},
		{"help_menu", {std::ref(modal_help->show)}},
		{"tithe_menu",
		 {std::ref(modal_tithe->show), std::ref(input_donate->show)}},
		{"identify_menu", {std::ref(modal_identify->show)}},
		{"equip_menu", {std::ref(modal_equip->show)}},
		{"remove_item_menu", {std::ref(modal_remove->show)}},
		{"spell_menu", {std::ref(modal_spell->show)}},
		{"drop_menu", {std::ref(modal_drop->show)}},
		{"use_menu", {std::ref(modal_use->show)}},
		{"invoke_menu", {std::ref(modal_invoke->show)}},
		{"trade_menu",
		 {std::ref(modal_trade->show), std::ref(modal_give->show)}},
		{"give_menu", {std::ref(modal_give->show)}},
		{"main_menu",
		 {std::ref(dialog_new->show), std::ref(dialog_exit->show)}},
		{"edge_menu", {std::ref(dialog_leave->show)}},
		{"chest_open_menu", {std::ref(modal_chest->show)}},
		{"chest_inspect_menu", {std::ref(modal_chest->show)}},
		{"chest_calfo_menu", {std::ref(modal_chest->show)}},
		{"chest_disarm_menu", {std::ref(modal_chest->show)}},
		{"chest_trap_menu", {std::ref(modal_chest->show)}},
	}};

	if (const auto it{std::ranges::find(flags, name,
										&decltype(flags)::value_type::first)};
		it != flags.end()) {

		return it->second;
	}

	return {};
}

auto Sorcery::UI::_handle_menu_reordering(const std::string_view name,
										  std::vector<std::string> &items,
										  std::vector<int> &data,
										  const std::size_t index,
										  const int data_item) -> void {

	if (name != "reorder_menu" || data_item == -1)
		return;

	const auto source_index{static_cast<int>(index)};

	with_DragDropSource(ImGuiDragDropFlags_None) {
		ImGui::SetDragDropPayload("REORDER_CELL", &source_index,
								  sizeof(source_index));
	}

	with_DragDropTarget {
		const auto payload{ImGui::AcceptDragDropPayload("REORDER_CELL")};

		if (payload == nullptr)
			return;

		const auto payload_index{*static_cast<const int *>(payload->Data)};

		const auto from{static_cast<std::size_t>(payload_index)};

		if (from >= items.size() || from >= data.size() || index >= data.size())
			return;

		std::swap(items[index], items[from]);
		std::swap(data[index], data[from]);

		_ctx.controller->clear_candidate_party();

		for (const auto character_id : data)
			_ctx.controller->add_to_candidate_party(character_id);

		_ctx.controller->set_flag("party_order_changed");
	}
}

auto Sorcery::UI::_activate_menu_item(const std::string_view name,
									  const int selection, const int data_item,
									  const std::vector<std::string> &items)
	-> void {

	auto ui_flags{_get_legacy_menu_ui_flags(name)};

	if (_ctx.controller->handle_action_table_menu(name, selection, data_item,
												  ui_flags))
		return;

	if (_ctx.controller->handle_dynamic_menu(name, items, data_item, selection,
											 ui_flags))
		return;

	_ctx.controller->handle_standard_menu(name, items, data_item, selection);
}

auto Sorcery::UI::draw_menu(
	const std::string name, const ImColor sel_color, const ImVec2 pos,
	const ImVec2 sz, const Enums::Layout::Font font,
	std::vector<std::string> &items, std::vector<int> &data, const bool reorder,
	const bool across, [[maybe_unused]] const bool numeric_shortcuts) -> void {

	// Work out size and positon of the menu, and the display name (which is
	// used for the ImGui ID)
	const std::string display_name{"##" + name};

	// Note that pos is in grid units, whereas sz is in pixels.
	const auto x{pos.x == -1.0f
					 ? (ImGui::GetMainViewport()->Size.x - sz.x) / 2.0f
					 : grid_x(pos.x)};

	const auto y{pos.y == -1.0f
					 ? (ImGui::GetMainViewport()->Size.y - sz.y) / 2.0f
					 : grid_y(pos.y)};

	// Set the Style for the Menu (this is a bit of a hack, but it works)
	set_StyleColor(ImGuiCol_FrameBg,
				   ImVec4{0.0f, 0.0f, 0.0f, 1.0f - _ctx.animation->fade});
	set_StyleColor(ImGuiCol_HeaderActive, ImVec4{sel_color});
	set_StyleColor(ImGuiCol_HeaderHovered, ImVec4{sel_color});
	UIStyle::set_faded_with_disabled(_ctx);
	set_Font(fontstore->get_current_font(font).value(), font_sz());

	const ImVec2 menu_pos{x, y};
	ImGui::SetCursorPos(menu_pos);

	// Look for a Key Selection (if numeric_shortcuts is true, then we will
	// consume a key from the controller)
	const auto key_selection{
		numeric_shortcuts ? _ctx.controller->consume_menu_key(items.size())
						  : std::nullopt};

	// Draw the Menu (as a ListBox)
	with_ListBox(display_name.c_str(), sz) {

		const auto item_width{std::invoke([&] {
			if (!across || items.empty())
				return 0.0f;

			const auto available{ImGui::GetContentRegionAvail().x};
			const auto spacing{ImGui::GetStyle().ItemSpacing.x};
			const auto gaps{spacing * static_cast<float>(items.size() - 1)};

			return (available - gaps) / static_cast<float>(items.size());
		})};

		for (std::size_t i{0}; i < items.size(); ++i) {

			// DEBUG_LOGF("menu={} available={} item_width={} items={}", name,
			//		   ImGui::GetContentRegionAvail().x, item_width,
			//		   items.size());

			// DEBUG_LOGF("{} text width={}", items[i],
			//		   ImGui::CalcTextSize(items[i].c_str()).x);

			const auto display_item{std::invoke([&] {
				if (!across)
					return items[i];

				const std::string_view item{items[i]};

				const auto first{item.find_first_not_of(' ')};

				if (first == std::string_view::npos)
					return std::format("##{}_{}", name, i);

				const auto last{item.find_last_not_of(' ')};

				return std::format(
					"{}##{}_{}", item.substr(first, last - first + 1), name, i);
			})};

			const auto index{static_cast<int>(i)};

			const auto is_selected{selected[name] == index};

			const auto flags{is_selected ? ImGuiSelectableFlags_Highlight
										 : ImGuiSelectableFlags_None};

			const auto data_item{i < data.size() ? data[i] : -1};

			const auto disabled{
				_ctx.controller->is_menu_item_disabled(name, index, data_item)};

			if (disabled)
				ImGui::BeginDisabled();

			if (across)
				ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign,
									ImVec2{0.5f, 0.5f});

			const auto clicked{ImGui::Selectable(display_item.c_str(),
												 is_selected, flags,
												 ImVec2{item_width, 0.0f})};

			if (across)
				ImGui::PopStyleVar();

			const auto keyed{key_selection && *key_selection == i};

			const auto activated{clicked || keyed};

			if (activated && !disabled) {

				if (reorder) {

					_ctx.controller->handle_standard_menu(name, items,
														  data_item, index);

				} else {

					_activate_menu_item(name, index, data_item, items);
				}
			}

			if (ImGui::IsItemHovered()) {

				selected[name] = index;
				highlighted[name] = index;
			}

			if (is_selected)
				ImGui::SetItemDefaultFocus();

			if (reorder && !disabled)
				_handle_menu_reordering(name, items, data, i, data_item);

			if (disabled)
				ImGui::EndDisabled();

			if (across && i + 1 < items.size())
				ImGui::SameLine();
		}
	}
}

auto Sorcery::UI::_draw_map_tile(const Tile &tile, const ImVec2 pos,
								 const ImVec2 sz) -> void {
	using enum Enums::DrawMap::Feature;
	using enum Enums::Tile::Features;
	using enum Enums::Tile::Properties;

	// Background Graphic
	_draw_fg_image_with_idx(MAPS_TEXTURE, std::to_underlying(FLOOR), pos, sz);

	// Darkness
	if (tile.is(Enums::Tile::Properties::DARKNESS))
		_draw_fg_image_with_idx(MAPS_TEXTURE, std::to_underlying(MAP_DARKNESS),
								pos, sz);

	// Walls for all 4 directions
	using enum Enums::Tile::Edge;
	using enum Enums::Map::Direction;
	if (tile.has(NORTH, SECRET_DOOR) || tile.has(NORTH, ONE_WAY_HIDDEN_DOOR))
		_draw_fg_image_with_idx(MAPS_TEXTURE, std::to_underlying(NORTH_SECRET),
								pos, sz);
	else if (tile.has(NORTH, UNLOCKED_DOOR) || tile.has(NORTH, ONE_WAY_DOOR))
		_draw_fg_image_with_idx(MAPS_TEXTURE, std::to_underlying(NORTH_DOOR),
								pos, sz);
	else if (tile.has(NORTH, ONE_WAY_WALL))
		_draw_fg_image_with_idx(
			MAPS_TEXTURE, std::to_underlying(NORTH_ONE_WAY_WALL), pos, sz);
	else if (tile.has(NORTH))
		_draw_fg_image_with_idx(MAPS_TEXTURE, std::to_underlying(NORTH_WALL),
								pos, sz);

	if (tile.has(SOUTH, SECRET_DOOR) || tile.has(SOUTH, ONE_WAY_HIDDEN_DOOR))
		_draw_fg_image_with_idx(MAPS_TEXTURE, std::to_underlying(SOUTH_SECRET),
								pos, sz);
	else if (tile.has(SOUTH, UNLOCKED_DOOR) || tile.has(SOUTH, ONE_WAY_DOOR))
		_draw_fg_image_with_idx(MAPS_TEXTURE, std::to_underlying(SOUTH_DOOR),
								pos, sz);
	else if (tile.has(SOUTH, ONE_WAY_WALL))
		_draw_fg_image_with_idx(
			MAPS_TEXTURE, std::to_underlying(SOUTH_ONE_WAY_WALL), pos, sz);
	else if (tile.has(SOUTH))
		_draw_fg_image_with_idx(MAPS_TEXTURE, std::to_underlying(SOUTH_WALL),
								pos, sz);

	if (tile.has(EAST, SECRET_DOOR) || tile.has(EAST, ONE_WAY_HIDDEN_DOOR))
		_draw_fg_image_with_idx(MAPS_TEXTURE, std::to_underlying(EAST_SECRET),
								pos, sz);
	else if (tile.has(EAST, UNLOCKED_DOOR) || tile.has(EAST, ONE_WAY_DOOR))
		_draw_fg_image_with_idx(MAPS_TEXTURE, std::to_underlying(EAST_DOOR),
								pos, sz);
	else if (tile.has(EAST, ONE_WAY_WALL))
		_draw_fg_image_with_idx(MAPS_TEXTURE,
								std::to_underlying(EAST_ONE_WAY_WALL), pos, sz);
	else if (tile.has(EAST))
		_draw_fg_image_with_idx(MAPS_TEXTURE, std::to_underlying(EAST_WALL),
								pos, sz);

	if (tile.has(WEST, SECRET_DOOR) || tile.has(WEST, ONE_WAY_HIDDEN_DOOR))
		_draw_fg_image_with_idx(MAPS_TEXTURE, std::to_underlying(WEST_SECRET),
								pos, sz);
	else if (tile.has(WEST, UNLOCKED_DOOR) || tile.has(WEST, ONE_WAY_DOOR))
		_draw_fg_image_with_idx(MAPS_TEXTURE, std::to_underlying(WEST_DOOR),
								pos, sz);
	else if (tile.has(WEST, ONE_WAY_WALL))
		_draw_fg_image_with_idx(MAPS_TEXTURE,
								std::to_underlying(WEST_ONE_WAY_WALL), pos, sz);
	else if (tile.has(WEST))
		_draw_fg_image_with_idx(MAPS_TEXTURE, std::to_underlying(WEST_WALL),
								pos, sz);

	// And Tile Contents
	if (tile.has(STAIRS_UP) || tile.has(LADDER_UP))
		_draw_fg_image_with_idx(MAPS_TEXTURE, std::to_underlying(MAP_STAIRS_UP),
								pos, sz);
	else if (tile.has(STAIRS_DOWN) || tile.has(LADDER_DOWN))
		_draw_fg_image_with_idx(MAPS_TEXTURE,
								std::to_underlying(MAP_STAIRS_DOWN), pos, sz);
	else if (tile.has(ELEVATOR))
		_draw_fg_image_with_idx(MAPS_TEXTURE, std::to_underlying(MAP_ELEVATOR),
								pos, sz);
	else if (tile.has(SPINNER))
		_draw_fg_image_with_idx(MAPS_TEXTURE, std::to_underlying(MAP_SPINNER),
								pos, sz);
	else if (tile.has(PIT))
		_draw_fg_image_with_idx(MAPS_TEXTURE, std::to_underlying(MAP_PIT), pos,
								sz);
	else if (tile.has(CHUTE))
		_draw_fg_image_with_idx(MAPS_TEXTURE, std::to_underlying(MAP_CHUTE),
								pos, sz);
	else if (tile.has(TELEPORT_TO))
		_draw_fg_image_with_idx(MAPS_TEXTURE,
								std::to_underlying(MAP_TELEPORT_TO), pos, sz);
	else if (tile.has(TELEPORT_FROM))
		_draw_fg_image_with_idx(MAPS_TEXTURE,
								std::to_underlying(MAP_TELEPORT_FROM), pos, sz);
	else if (tile.has(MESSAGE) || tile.has(NOTICE))
		_draw_fg_image_with_idx(MAPS_TEXTURE, std::to_underlying(EXCLAMATION),
								pos, sz);
}

auto Sorcery::UI::_to_imgui(GLuint tex) -> ImTextureID {

	return (ImTextureID)(uintptr_t)tex;
}

auto Sorcery::UI::_mage_spell_index(Enums::Magic::SpellID id) -> std::size_t {
	return static_cast<std::size_t>(std::to_underlying(id));
}

auto Sorcery::UI::_priest_spell_index(Enums::Magic::SpellID id) -> std::size_t {
	return static_cast<std::size_t>(
		std::to_underlying(id) -
		std::to_underlying(Enums::Magic::SpellID::BADIOS));
}

auto Sorcery::UI::in_popup() const -> bool {

	return active_popup_count() > 0;
}

auto Sorcery::UI::close_all_popups() -> void {

	for (auto *show : _popup_states())
		*show = false;
}

auto Sorcery::UI::active_popup_count() const -> int {

	const auto states{_popup_states()};

	return std::count_if(states.begin(), states.end(), [](const bool *show) {
		return *show;
	});
}

auto Sorcery::UI::_popup_states() const -> std::vector<bool *> {

	std::vector<bool *> states;

	auto add = [&](const auto &ptr, const std::string_view name) {
		if (ptr) {
			// DEBUG_LOGF("Popup state: {:<24} {}", name, ptr->show);
			states.emplace_back(&ptr->show);
		}
	};

#define ADD_POPUP(popup) add(popup, #popup)

	ADD_POPUP(dialog_exit);
	ADD_POPUP(dialog_new);
	ADD_POPUP(dialog_leave);
	ADD_POPUP(dialog_rite);
	ADD_POPUP(dialog_search);
	ADD_POPUP(dialog_delete);
	ADD_POPUP(notice_cannot_donate);
	ADD_POPUP(notice_donated_ok);
	ADD_POPUP(notice_not_enough_gold);
	ADD_POPUP(notice_divvy);
	ADD_POPUP(notice_pool_gold);
	ADD_POPUP(notice_renamed_ok);
	ADD_POPUP(notice_reclassed_ok);
	ADD_POPUP(dialog_stairs_up);
	ADD_POPUP(dialog_stairs_down);
	ADD_POPUP(input_donate);
	ADD_POPUP(input_name);
	ADD_POPUP(modal_camp);
	ADD_POPUP(modal_elevator_top);
	ADD_POPUP(modal_elevator_bottom);
	ADD_POPUP(message_tile);
	ADD_POPUP(modal_inspect);
	ADD_POPUP(modal_help);
	ADD_POPUP(modal_tithe);
	ADD_POPUP(modal_identify);
	ADD_POPUP(modal_chest);
	ADD_POPUP(modal_equip);
	ADD_POPUP(modal_remove);
	ADD_POPUP(modal_spell);
	ADD_POPUP(modal_drop);
	ADD_POPUP(modal_trade);
	ADD_POPUP(modal_give);
	ADD_POPUP(modal_use);
	ADD_POPUP(modal_invoke);

#undef ADD_POPUP

	return states;
}

auto Sorcery::UI::show_transient(std::string text,
								 const std::chrono::milliseconds duration,
								 const TransientWidth width,
								 const TransientMode mode) -> void {

	_transient_message =
		TransientMessage{.text = std::move(text),
						 .expires = std::chrono::steady_clock::now() + duration,
						 .width = width,
						 .mode = mode};
}

auto Sorcery::UI::clear_transient_on_action() -> void {

	if (!_transient_message)
		return;

	if (_transient_message->mode == TransientMode::DISMISS_ON_ACTION) {

		_transient_message.reset();
	}
}

auto Sorcery::UI::clear_transient() -> void {

	if (!_transient_message)
		return;

	_transient_message.reset();
}

auto Sorcery::UI::transient_blocks_input() const -> bool {

	return _transient_message &&
		   _transient_message->mode == TransientMode::UNTIL_EXPIRY;
}

auto Sorcery::UI::has_transient() const -> bool {

	return _transient_message.has_value();
}

auto Sorcery::UI::_draw_transient() -> void {

	if (!_transient_message)
		return;

	if (std::chrono::steady_clock::now() >= _transient_message->expires) {
		_transient_message.reset();
		return;
	}

	const auto &message{*_transient_message};

	const auto component{components->get("engine_base_ui:transient_message")};

	set_Font(fontstore->get_current_font(component.font).value(), font_sz());

	const auto text_size{ImGui::CalcTextSize(message.text.c_str())};

	const auto padding{grid_sz() * 2.0f};

	const auto width{message.width == TransientWidth::FULL
						 ? ImGui::GetMainViewport()->Size.x
						 : text_size.x + (padding * 2.0f)};

	const auto height{component.h * grid_sz() + (padding * 2.0f)};

	const auto centre{ImGui::GetMainViewport()->GetCenter()};

	ImGui::SetNextWindowPos(centre, ImGuiCond_Always, ImVec2{0.5f, 0.5f});

	ImGui::SetNextWindowSize(ImVec2{width, height});

	ImGui::SetNextWindowBgAlpha(1.0f);

	set_StyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});

	set_StyleVar(ImGuiStyleVar_WindowBorderSize, 0);

	set_StyleVar(ImGuiStyleVar_WindowRounding, frame_rd);

	set_StyleColor(ImGuiCol_WindowBg, component.background);

	with_Window("##transient_message", nullptr,
				ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
					ImGuiWindowFlags_NoInputs |
					ImGuiWindowFlags_NoSavedSettings) {

		const auto p_min{ImGui::GetWindowPos()};
		const auto p_max{ImVec2{p_min.x + width, p_min.y + height}};

		draw_frame(
			p_min, p_max,
			ImVec4{ui_colour.x, ui_colour.y, ui_colour.z, _ctx.animation->fade},
			frame_rd);

		ImGui::SetCursorPos(ImVec2{padding, padding});

		ImGui::TextUnformatted(message.text.c_str());
	}
}