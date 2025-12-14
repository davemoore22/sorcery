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

#include "core/ui.hpp"
#include "common/macro.hpp"
#include "core/animation.hpp"
#include "core/context.hpp"
#include "core/controller.hpp"
#include "core/display.hpp"
#include "core/render.hpp"
#include "core/resources.hpp"
#include "core/system.hpp"
#include "gui/dialog.hpp"
#include "gui/frame.hpp"
#include "gui/input.hpp"
#include "gui/menu.hpp"
#include "gui/message.hpp"
#include "gui/modal.hpp"
#include "gui/popup.hpp"
#include "gui/videoplayer.hpp"
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
#include "types/game.hpp"
#include "types/state.hpp"

Sorcery::UI::UI(Context &ctx)
	: _ctx{ctx} {

	// Storage
	components = std::make_unique<ComponentStore>(_ctx.get_file(LAYOUT_FILE));
	images = std::make_unique<ImageStore>(_ctx.system);

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
	dialog_exit = std::make_unique<Dialog>(
		_ctx.system, this, components->get("main_menu:dialog_exit"),
		Enums::Layout::DialogType::CONFIRM);
	dialog_new = std::make_unique<Dialog>(
		_ctx.system, this, components->get("main_menu:dialog_new"),
		Enums::Layout::DialogType::CONFIRM);
	dialog_leave = std::make_unique<Dialog>(
		_ctx.system, this, components->get("main_menu:dialog_leave"),
		Enums::Layout::DialogType::CONFIRM);
	notice_divvy = std::make_unique<Dialog>(
		_ctx.system, this, components->get("global:notice_divvy"),
		Enums::Layout::DialogType::OK);
	notice_pool_gold = std::make_unique<Dialog>(
		_ctx.system, this, components->get("global:notice_pool_gold"),
		Enums::Layout::DialogType::OK);
	notice_cannot_donate = std::make_unique<Dialog>(
		_ctx.system, this, components->get("global:notice_cannot_donate"),
		Enums::Layout::DialogType::OK);
	notice_donated_ok = std::make_unique<Dialog>(
		_ctx.system, this, components->get("global:notice_donated_ok"),
		Enums::Layout::DialogType::OK);
	notice_not_enough_gold = std::make_unique<Dialog>(
		_ctx.system, this, components->get("global:notice_not_enough_gold"),
		Enums::Layout::DialogType::OK);
	popup_ouch = std::make_unique<Popup>(
		_ctx.system, this, components->get("engine_base_ui:popup_ouch"));
	modal_camp =
		std::make_unique<Modal>(_ctx.system, this, _ctx.controller,
								components->get("engine_base_ui:modal_camp"));
	modal_drop = std::make_unique<Modal>(_ctx.system, this, _ctx.controller,
										 components->get("global:modal_drop"));
	modal_inspect =
		std::make_unique<Modal>(_ctx.system, this, _ctx.controller,
								components->get("global:modal_inspect"));
	modal_identify =
		std::make_unique<Modal>(_ctx.system, this, _ctx.controller,
								components->get("global:modal_identify"));
	modal_trade =
		std::make_unique<Modal>(_ctx.system, this, _ctx.controller,
								components->get("global:modal_trade"));
	modal_use = std::make_unique<Modal>(_ctx.system, this, _ctx.controller,
										components->get("global:modal_use"));
	modal_invoke =
		std::make_unique<Modal>(_ctx.system, this, _ctx.controller,
								components->get("global:modal_invoke"));

	input_donate =
		std::make_unique<Input>(_ctx.system, this, _ctx.controller,
								components->get("global:input_donate"));
	input_name = std::make_unique<Input>(_ctx.system, this, _ctx.controller,
										 components->get("global:input_name"));

	dialog_stairs_up = std::make_unique<Dialog>(
		_ctx.system, this, components->get("engine_base_ui:dialog_stairs_up"),
		Enums::Layout::DialogType::CONFIRM);
	dialog_stairs_down = std::make_unique<Dialog>(
		_ctx.system, this, components->get("engine_base_ui:dialog_stairs_down"),
		Enums::Layout::DialogType::CONFIRM);

	message_tile = std::make_unique<Message>(
		_ctx.system, this, components->get("engine_base_ui:message_tile"));

	// Window and Display Settings
	font_sz = std::stoi(_ctx.config->get("Font", "size"));
	grid_sz = std::stoi(_ctx.config->get("Grid", "size"));
	columns = std::stoi(_ctx.config->get("Grid", "columns"));
	rows = std::stoi(_ctx.config->get("Grid", "rows"));
	adj_grid_w = grid_sz;
	adj_grid_h = grid_sz;
	frame_rd = std::stoi(_ctx.config->get("Frame", "rounding"));
	ui_rd = std::stoi(_ctx.config->get("UI", "rounding"));

	// Render window
	_render = std::make_unique<Render>(_ctx.system, this);

	// Ticks
	ticks = SDL_GetTicks();

	// Initialise function tables for display methods
	_draw_frontend["atlas"] = [this] {
		_display_atlas();
	};
	_draw_frontend["bestiary"] = [this] {
		_display_bestiary();
	};
	_draw_frontend["compendium"] = [this] {
		_display_compendium();
	};
	_draw_frontend["main_menu"] = [this] {
		_display_main_menu();
	};
	_draw_frontend["museum"] = [this] {
		_display_museum();
	};
	_draw_frontend["options"] = [this] {
		_display_options();
	};
	_draw_frontend["spellbook"] = [this] {
		_display_spellbook();
	};
	_draw_frontend["splash"] = [this] {
		_display_splash();
	};

	_draw_modules["add"] = [this](Game *game) {
		_display_add(game);
	};
	_draw_modules["castle"] = [this](Game *game) {
		_display_castle(game);
	};
	_draw_modules["edge_of_town"] = [this](Game *game) {
		_display_edge_of_town(game);
	};
	_draw_modules["inn"] = [this](Game *game) {
		_display_inn(game);
	};
	_draw_modules["pay"] = [this](Game *game) {
		_display_pay(game);
	};
	_draw_modules["remove"] = [this](Game *game) {
		_display_remove(game);
	};
	_draw_modules["restart"] = [this](Game *game) {
		_display_restart(game);
	};
	_draw_modules["shop"] = [this](Game *game) {
		_display_shop(game);
	};
	_draw_modules["stay"] = [this](Game *game) {
		_display_stay(game);
	};
	_draw_modules["tavern"] = [this](Game *game) {
		_display_tavern(game);
	};
	_draw_modules["temple"] = [this](Game *game) {
		_display_temple(game);
	};
	_draw_modules["training_grounds"] = [this](Game *game) {
		_display_training_grounds(game);
	};
	_draw_modules["method"] = [this](Game *game) {
		_display_method(game);
	};

	_draw_game_int["roster"] = [this](Game *game, int n) {
		_display_roster(game, n);
	};

	_draw_game_int["create_name"] = [this](Game *game, int n) {
		_display_create_name(game, n);
	};

	_draw_game_int["create_race"] = [this](Game *game, int n) {
		_display_create_race(game, n);
	};

	_draw_game_int["create_alignment"] = [this](Game *game, int n) {
		_display_create_alignment(game, n);
	};

	_draw_game_int["create_confirm"] = [this](Game *game, int n) {
		_display_create_confirm(game, n);
	};

	_draw_game_int["create_class"] = [this](Game *game, int n) {
		_display_create_class(game, n);
	};

	_draw_game_int["heal"] = [this](Game *game, int n) {
		_display_heal(game, n);
	};
	_draw_game_int["inspect"] = [this](Game *game, int n) {
		_display_inspect(game, n);
	};
	_draw_game_int["level_up"] = [this](Game *game, int n) {
		_display_level_up(game, n);
	};
	_draw_game_int["nolevelup"] = [this](Game *game, int n) {
		_display_no_level_up(game, n);
	};
	_draw_game_int["recovery"] = [this](Game *game, int n) {
		_display_recovery(game, n);
	};
	_draw_game_int["reorder"] = [this](Game *game, int n) {
		_display_reorder(game, n);
	};
	_draw_game_int["roster"] = [this](Game *game, int n) {
		_display_roster(game, n);
	};

	_draw_string["license"] = [this](const std::string &string) {
		_display_license(string);
	};
};

Sorcery::UI::~UI() {}

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

// Create a Modal on Demand (used whenever data items on it aren't fixed - for
// example the Party Members); normally otherwise fixed Modals are created at
// the beginning as part of the Form/Module create
auto Sorcery::UI::create_dynamic_modal(Game *game, const std::string name)
	-> void {

	if (name == "modal_inspect") {
		if (modal_inspect.get())
			modal_inspect.reset();
		modal_inspect =
			std::make_unique<Modal>(_ctx.system, this, _ctx.controller,
									components->get("global:modal_inspect"));
		modal_inspect->regenerate(_ctx.controller, game);
	} else if (name == "modal_stay") {
		if (modal_stay.get())
			modal_stay.reset();
		modal_stay =
			std::make_unique<Modal>(_ctx.system, this, _ctx.controller,
									components->get("global:modal_stay"));
		modal_stay->regenerate(_ctx.controller, game);
	} else if (name == "modal_help") {
		if (modal_help.get())
			modal_help.reset();
		modal_help =
			std::make_unique<Modal>(_ctx.system, this, _ctx.controller,
									components->get("global:modal_help"));
		modal_help->regenerate(_ctx.controller, game);
	} else if (name == "modal_tithe") {
		if (modal_tithe.get())
			modal_tithe.reset();
		modal_tithe =
			std::make_unique<Modal>(_ctx.system, this, _ctx.controller,
									components->get("global:modal_tithe"));
		modal_tithe->regenerate(_ctx.controller, game);
	} else if (name == "modal_identify") {
		if (modal_identify.get())
			modal_identify.reset();
		modal_identify =
			std::make_unique<Modal>(_ctx.system, this, _ctx.controller,
									components->get("global:modal_identify"));
		modal_identify->regenerate(_ctx.controller, game);
	} else if (name == "modal_drop") {
		if (modal_drop.get())
			modal_drop.reset();
		modal_drop =
			std::make_unique<Modal>(_ctx.system, this, _ctx.controller,
									components->get("global:modal_drop"));
		modal_drop->regenerate(_ctx.controller, game);
	} else if (name == "modal_trade") {
		if (modal_trade.get())
			modal_trade.reset();
		modal_trade =
			std::make_unique<Modal>(_ctx.system, this, _ctx.controller,
									components->get("global:modal_trade"));
		modal_trade->regenerate(_ctx.controller, game);
	} else if (name == "modal_use") {
		if (modal_use.get())
			modal_use.reset();
		modal_use =
			std::make_unique<Modal>(_ctx.system, this, _ctx.controller,
									components->get("global:modal_use"));
		modal_use->regenerate(_ctx.controller, game);
	} else if (name == "modal_invoke") {
		if (modal_invoke.get())
			modal_invoke.reset();
		modal_invoke =
			std::make_unique<Modal>(_ctx.system, this, _ctx.controller,
									components->get("global:modal_invoke"));
		modal_invoke->regenerate(_ctx.controller, game);
	}

	// Note that modal_camp is not dynamic and thus isn't handled here
}

auto Sorcery::UI::_draw_window_menu() -> void {}

// Not an ideal function, really need to maintain a pointer status map instead
auto Sorcery::UI::_get_popups() const -> std::string {

	auto get_popup_status = [](void *component,
							   std::string type) -> std::string {
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
	};

	std::string output{};
	if (dialog_exit)
		output.append(get_popup_status((void *)dialog_exit.get(), "dialog"));
	if (dialog_leave)
		output.append(get_popup_status((void *)dialog_leave.get(), "dialog"));
	if (dialog_new)
		output.append(get_popup_status((void *)dialog_new.get(), "dialog"));
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
	if (modal_drop)
		output.append(get_popup_status((void *)modal_drop.get(), "modal"));
	if (modal_help)
		output.append(get_popup_status((void *)modal_help.get(), "modal"));
	if (modal_identify)
		output.append(get_popup_status((void *)modal_identify.get(), "modal"));
	if (modal_inspect)
		output.append(get_popup_status((void *)modal_inspect.get(), "modal"));
	if (modal_invoke)
		output.append(get_popup_status((void *)modal_invoke.get(), "modal"));
	if (modal_stay)
		output.append(get_popup_status((void *)modal_stay.get(), "modal"));
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
	if (popup_ouch)
		output.append(get_popup_status((void *)popup_ouch.get(), "popup"));
	if (notice_pool_gold)
		output.append(
			get_popup_status((void *)notice_pool_gold.get(), "dialog"));

	return output;
}

auto Sorcery::UI::start() -> void {

	// Initialise ImGUI to use SDL2/OpenGL
	ImGui::CreateContext();
	_io = ImGui::GetIO();
	_io.IniFilename = CSTR(_ctx.get_file(CONFIG_FILE));
	ImGui::StyleColorsClassic();
	ImGui_ImplSDL2_InitForOpenGL(_ctx.display->get_SDL_window(),
								 _ctx.display->get_GL_context());
	ImGui_ImplOpenGL3_Init(_ctx.display->get_GLSL_version());

	if (_ctx.get_config(Enums::Config::FULLSCREEN))
		set_fullscreen(true);
	else
		set_fullscreen(false);

	// Can create the fontstore now which loads the fonts
	fontstore = std::make_unique<FontStore>(_ctx.system, _io);
	ui_colour =
		ImVec4{std::stof(_ctx.config->get("Frame", "colour_red")),
			   std::stof(_ctx.config->get("Frame", "colour_green")),
			   std::stof(_ctx.config->get("Frame", "colour_blue")), 1.0};

	// Set the Default Fonts
	using enum Enums::Layout::Font;
	fontstore->set_current_font(TEXT, _ctx.config->get("Font", "text"));
	fontstore->set_current_font(DEFAULT, fontstore->get_default_font());
	fontstore->set_current_font(MONOSPACE,
								_ctx.config->get("Font", "monospace"));
	fontstore->set_current_font(PROPORTIONAL,
								_ctx.config->get("Font", "proportional"));

	grid_sz = std::stoi(_ctx.config->get("Grid", "size"));

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
	notice_divvy->show = false;
	notice_donated_ok->show = false;
	notice_cannot_donate->show = false;
	notice_not_enough_gold->show = false;
	notice_pool_gold->show = false;
	input_donate->show = false;
	input_name->show = false;
	popup_ouch->show = false;
	modal_camp->show = false;
	modal_identify->show = false;
	modal_drop->show = false;
	modal_trade->show = false;
	modal_use->show = false;
	modal_invoke->show = false;
	dialog_stairs_up->show = false;
	dialog_stairs_down->show = false;
	message_tile->show = false;

	_attract_data.clear();
}

auto Sorcery::UI::io() -> ImGuiIO & {

	return _io;
}

auto Sorcery::UI::stop() -> void {

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
	SDL_GL_DeleteContext(_ctx.display->get_GL_context());
	SDL_DestroyWindow(_ctx.display->get_SDL_window());
	SDL_Quit();
}

auto Sorcery::UI::display_refresh(std::any first, std::any second) -> void {

	// Refresh what we previously drew
	display(_ctx.controller->get_last_screen(), first, second);
}

auto Sorcery::UI::display_engine(Game *game) -> void {

	// Start a new Rendering Frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	_setup_windows();
	//_draw_window_menu();

	// Background
	_draw_components("engine_base_ui");

	dialog_leave->display(_ctx.get_flag_ref("want_leave_game"));
	dialog_stairs_up->display(_ctx.get_flag_ref("want_take_stairs_up"));
	dialog_stairs_down->display(_ctx.get_flag_ref("want_take_stairs_down"));
	// if (popup_ouch->show)
	popup_ouch->display();
	message_tile->display(_ctx.get_flag_ref("after_tile_message"));
	if (modal_camp->show)
		modal_camp->display(_ctx.get_flag_ref("want_camp"));
	if (modal_inspect->show)
		modal_inspect->display(_ctx.get_flag_ref("want_inspect"));
	if (modal_identify->show)
		modal_identify->display(_ctx.get_flag_ref("want_identify"));
	if (modal_drop->show)
		modal_drop->display(_ctx.get_flag_ref("want_drop"));
	if (modal_trade->show)
		modal_trade->display(_ctx.get_flag_ref("want_trade"));
	if (modal_use->show)
		modal_use->display(_ctx.get_flag_ref("want_use"));
	if (modal_invoke->show)
		modal_invoke->display(_ctx.get_flag_ref("want_invoke"));
	if (notice_pool_gold->show)
		notice_pool_gold->display(_ctx.get_flag_ref("want_pool_gold"));
	if (_ctx.get_flag("interface_ui") && _ctx.get_flag("interface_party_panel"))
		_draw_party_panel(game);
	if (_ctx.get_flag("interface_ui")) {
		_draw_compass(game);
		_draw_buffbar(game);
		_draw_icons(game);
		_draw_save(game);
	}

	// Dungeon View
	auto component{components->get("engine_base_ui:wire_frame_view")};
	_render->draw(game, &component);

	// And Cursor on Top
	_draw_debug();
	_draw_cursor();

	bool show{true};
	ImGui::SetCurrentFont(fontstore->get_default_font());
	ImGui::ShowDemoWindow(&show);

	// And finally clear and render everything
	ImGui::Render();
	glViewport(0, 0, (int)io().DisplaySize.x, (int)io().DisplaySize.y);
	glClearColor(0, 0, 0, 255);
	glClear(GL_COLOR_BUFFER_BIT);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	SDL_GL_SwapWindow(_ctx.display->get_SDL_window());
}

auto Sorcery::UI::display(const std::string screen, std::any first,
						  std::any second) -> void {

	// Store what we want to draw for next refresh
	_ctx.controller->set_last_screen(screen);

	// Start a new Rendering Frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	_setup_windows();
	//_draw_window_menu();

	if (first.type() == typeid(Game *) && second.type() != typeid(int)) {
		if (auto it = _draw_modules.find(screen); it != _draw_modules.end())
			it->second(std::any_cast<Game *>(first));

	} else if (first.type() == typeid(Game *) && second.type() == typeid(int)) {
		if (auto it = _draw_game_int.find(screen); it != _draw_game_int.end())
			it->second(std::any_cast<Game *>(first),
					   std::any_cast<int>(second));

	} else if (first.type() == typeid(std::string)) {
		if (auto it = _draw_string.find(screen); it != _draw_string.end())
			it->second(std::any_cast<std::string>(first));

	} else {
		if (auto it = _draw_frontend.find(screen); it != _draw_frontend.end())
			it->second();
	}

	_draw_cursor();

	// And finally clear and render everything
	ImGui::Render();
	glViewport(0, 0, (int)io().DisplaySize.x, (int)io().DisplaySize.y);
	glClearColor(0, 0, 0, 255);
	glClear(GL_COLOR_BUFFER_BIT);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	SDL_GL_SwapWindow(_ctx.display->get_SDL_window());
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
		ImGui::Image((intptr_t)src_image.texture, p_sz, uv_0, uv_1, tint_col);
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
					return static_cast<float>(adj_grid_w * component->x);
			})};
			const auto y{std::invoke([&] {
				if (component->y == -1) {
					const auto viewport{ImGui::GetMainViewport()};
					return (viewport->Size.y - 200) / 2;
				} else
					return static_cast<float>(adj_grid_h * component->y);
			})};

			ImGui::SetCursorPos(ImVec2{x, y});
			ImGui::GetWindowDrawList()->AddRectFilled(
				ImVec2{x, y}, ImVec2(x + 200, y + 200),
				ImColor{ImVec4{0.5f, 0.5f, 0.5f, _ctx.animation->fade}});
		}

		return;
	}

	if (component->get("source")) {
		const auto source{component->get("source").value()};
		const auto scale{std::stof(component->get("scale").value())};

		// Load the image if necessary
		if (!images->has_loaded(source))
			images->load_image(source);

		// Work out any scaling if needed
		auto src_image{images->get(source)};
		const auto resized{Size{(intptr_t)src_image.width * scale,
								(intptr_t)src_image.height * scale}};

		// Work out where to draw the image
		const auto x{std::invoke([&] {
			if (component->x == -1) {
				const auto viewport{ImGui::GetMainViewport()};
				return (viewport->Size.x - resized.w) / 2;
			} else
				return static_cast<float>(adj_grid_w * component->x);
		})};
		const auto y{std::invoke([&] {
			if (component->y == -1) {
				const auto viewport{ImGui::GetMainViewport()};
				return (viewport->Size.y - resized.h) / 2;
			} else
				return static_cast<float>(adj_grid_h * component->y);
		})};

		// Draw the Image
		with_Window(WINDOW_LAYER_IMAGES, nullptr,
					ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs) {
			ImGui::SetCursorPos(ImVec2{x, y});
			ImVec4 tint_col{ImVec4(1.0f, 1.0f, 1.0f, _ctx.animation->fade)};
			ImGui::Image(src_image.texture,
						 ImVec2{static_cast<float>(resized.w),
								static_cast<float>(resized.h)},
						 ImVec2{0.0f, 0.0f}, ImVec2{1.0f, 1.0f}, tint_col);
		}
	}
}

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

auto Sorcery::UI::_draw_cursor() -> void {

	// Just check if we can draw
	if (!images->has_loaded(std::string{ICONS_TEXTURE})) {
		ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
		return;
	} else
		ImGui::SetMouseCursor(ImGuiMouseCursor_None);

	// Get the current mouse position
	ImGuiIO &io{ImGui::GetIO()};
	const auto pos{ImVec2{io.MousePos.x, io.MousePos.y}};
	if (ImGui::IsMousePosValid()) {

		// Grab and extract the image manually and add to the foreground list
		// (we don't use _draw_fg_image_with_idx() since that draws to one of
		// the standard layers, whereas we want the cursor to be always visible
		// no matter what)

		// Work out what cursor to draw
		auto src_image{images->get(std::string{ICONS_TEXTURE})};
		const auto icon_sz{src_image.width / ICONS_TILE_ROW_COUNT};
		const auto texture_sz{ImVec2{src_image.width, src_image.height}};
		const auto dest_sz{ImVec2{32, 32}};
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
	auto frame{std::make_shared<Frame>(_ctx.system, this, component)};
	_frames.emplace_back(std::move(frame));
}

// Draw a Menu
auto Sorcery::UI::_draw_menu(Component *component, Game *game) -> void {

	auto menu{std::make_shared<Menu>(_ctx.system, _ctx.resources, this,
									 this->_ctx.controller, component, game)};
	menu->regenerate();
	menu->draw();
	_menus.emplace_back(std::move(menu));
}

auto Sorcery::UI::_draw_debug() -> void {

	if (!_ctx.controller->get_flag("debug_ui"))
		return;

	with_Window(WINDOW_LAYER_MENUS, nullptr,
				ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs) {

		set_Font(_io.FontDefault);
		ImGui::SetCursorPos(ImVec2{8, 8});
		set_StyleColor(ImGuiCol_Text, ImVec4{1.0f, 0.0f, 0.0f, 1.0f});
		ImGui::TextUnformatted(_ctx.controller->get_flags().c_str());

		ImGui::SetCursorPos(ImVec2{8, 400});
		set_StyleColor(ImGuiCol_Text, ImVec4{1.0f, 0.0f, 0.0f, 1.0f});
		ImGui::TextUnformatted(_get_popups().c_str());

		ImGui::SetCursorPos(ImVec2{8, 700});
		set_StyleColor(ImGuiCol_Text, ImVec4{1.0f, 0.0f, 0.0f, 1.0f});
		ImGui::TextUnformatted(_ctx.controller->get_characters().c_str());

		ImGui::SetCursorPos(ImVec2{1000, 8});
		set_StyleColor(ImGuiCol_Text, ImVec4{1.0f, 0.0f, 0.0f, 1.0f});
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

		set_Font(fontstore->get_current_font(component->font).value());
		const auto wrap{std::stof(component->get("width").value()) * font_sz};
		auto p_min{
			ImVec2{component->x * adj_grid_w, component->y * adj_grid_h}};

		ImGui::SetCursorPos(p_min);
		with_TextWrapPos(p_min.x + wrap) {
			set_StyleColor(ImGuiCol_Text, component->colour);
			ImGui::TextUnformatted(
				_ctx.get_string(component->string_key).c_str());
		}
	}
}

auto Sorcery::UI::draw_text(const std::string string, const ImColor colour,
							const ImVec2 pos, const Enums::Layout::Font font)
	-> void {

	set_Font(fontstore->get_current_font(font).value());

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
	set_Font(fontstore->get_current_font(component->font).value());
	const auto name{component->name};
	const auto col{get_hl_colour(_ctx.animation->lerp)};
	auto x{std::invoke([&] {
		if (component->x == -1) {
			const auto viewport{ImGui::GetMainViewport()};
			const auto width{ImGui::CalcTextSize(
				CSTR(_ctx.get_string(component->string_key)))};
			return (viewport->Size.x - width.x) / 2;
		} else
			return static_cast<float>(adj_grid_w * component->x);
	})};
	auto y{std::invoke([&] {
		if (component->y == -1) {
			const auto viewport{ImGui::GetMainViewport()};
			const auto height{ImGui::CalcTextSize(
				CSTR(_ctx.get_string(component->string_key)))};
			return (viewport->Size.y - height.y) / 2;
		} else
			return static_cast<float>(adj_grid_h * component->y);
	})};

	if (component->get("adjust_x"))
		x += std::stof(component->get("adjust_x").value());
	if (component->get("adjust_y"))
		y += std::stof(component->get("adjust_y").value());

	set_StyleColor(ImGuiCol_Text,
				   ImVec4{1.0f, 1.0f, 1.0f, _ctx.animation->fade});
	set_StyleColor(ImGuiCol_Button,
				   ImVec4{0.0f, 0.0f, 0.0f, _ctx.animation->fade});
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
		set_Font(fontstore->get_current_font(component->font).value());
		const auto name{component->name};
		const auto col{get_hl_colour(_ctx.animation->lerp)};
		auto x{std::invoke([&] {
			if (component->x == -1) {
				const auto viewport{ImGui::GetMainViewport()};
				const auto width{ImGui::CalcTextSize(
					CSTR(_ctx.get_string(component->string_key)))};
				return (viewport->Size.x - width.x) / 2;
			} else
				return static_cast<float>(adj_grid_w * component->x);
		})};
		auto y{std::invoke([&] {
			if (component->y == -1) {
				const auto viewport{ImGui::GetMainViewport()};
				const auto height{ImGui::CalcTextSize(
					CSTR(_ctx.get_string(component->string_key)))};
				return (viewport->Size.y - height.y) / 2;
			} else
				return static_cast<float>(adj_grid_h * component->y);
		})};
		if (component->get("adjust_x"))
			x += std::stof(component->get("adjust_x").value());
		if (component->get("adjust_y"))
			y += std::stof(component->get("adjust_y").value());

		set_StyleColor(ImGuiCol_Text,
					   ImVec4{1.0f, 1.0f, 1.0f, _ctx.animation->fade});
		set_StyleColor(ImGuiCol_Button,
					   ImVec4{0.0f, 0.0f, 0.0f, _ctx.animation->fade});
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
										   [[maybe_unused]] Game *game,
										   Character *character) -> void {

	const auto left_col{component->x + 0};
	const auto right_col{component->x + 19};

	using enum Enums::Character::Ability;
	using enum Enums::Character::Attribute;
	set_StyleColor(ImGuiCol_Text,
				   ImVec4{1.0f, 1.0f, 1.0f, _ctx.animation->fade});
	ImVec2 pos{};
	pos = ImVec2{left_col * adj_grid_w, component->y * adj_grid_h};
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(std::format("{:>14} {:>2}", "Strength",
									   character->get_cur_attr(STRENGTH))
							   .c_str());

	set_StyleColor(ImGuiCol_Text,
				   ImVec4{0.5f, 0.5f, 0.5f, _ctx.animation->fade});
	pos.y += adj_grid_h;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:+>2}", "Atk Mod",
					character->abilities().at(ATTACK_MODIFIER))
			.c_str());
	pos.y += adj_grid_h;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:+>2}", "Hit Prob",
					character->abilities().at(HIT_PROBABILITY))
			.c_str());
	pos.y += adj_grid_h;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(std::format("{:>14} {:+>2}", "Bonus Damg",
									   character->abilities().at(BONUS_DAMAGE))
							   .c_str());
	pos.y += adj_grid_h;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}", "Num Attacks",
					character->abilities().at(BASE_NUMBER_OF_ATTACKS))
			.c_str());
	pos.y += adj_grid_h;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}", "Unarmed Damg",
					character->abilities().at(UNARMED_DAMAGE))
			.c_str());

	set_StyleColor(ImGuiCol_Text,
				   ImVec4{1.0f, 1.0f, 1.0f, _ctx.animation->fade});
	pos.y += (adj_grid_h * 2);
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(std::format("{:>14} {:>2}", "Vitality",
									   character->get_cur_attr(VITALITY))
							   .c_str());

	set_StyleColor(ImGuiCol_Text,
				   ImVec4{0.5f, 0.5f, 0.5f, _ctx.animation->fade});
	pos.y += adj_grid_h;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:+>2}", "Vit Bonus",
					character->abilities().at(VITALITY_BONUS))
			.c_str());
	pos.y += adj_grid_h;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:+>2}", "Bonus HP",
					character->abilities().at(BONUS_HIT_POINTS))
			.c_str());
	pos.y += adj_grid_h;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}%", "Ress / Dead",
					character->abilities().at(DEAD_RESURRECT))
			.c_str());
	pos.y += adj_grid_h;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}%", "Ress / Ashes",
					character->abilities().at(ASHES_RESURRECT))
			.c_str());
	pos.y += adj_grid_h;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}%", "Ress / Spell",
					character->abilities().at(DI_KADORTO_RESURRECT))
			.c_str());

	pos = ImVec2{right_col * adj_grid_w, component->y * adj_grid_h};
	ImGui::SetCursorPos(pos);
	set_StyleColor(ImGuiCol_Text,
				   ImVec4{1.0f, 1.0f, 1.0f, _ctx.animation->fade});
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}", "I.Q.", character->get_cur_attr(IQ))
			.c_str());

	set_StyleColor(ImGuiCol_Text,
				   ImVec4{0.5f, 0.5f, 0.5f, _ctx.animation->fade});
	pos.y += adj_grid_h;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}%", "Spell Learn",
					character->abilities().at(MAGE_SPELL_LEARN))
			.c_str());
	pos.y += adj_grid_h;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}%", "ID Items",
					character->abilities().at(IDENTIFY_ITEMS))
			.c_str());
	pos.y += adj_grid_h;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}%", "ID Curse",
					character->abilities().at(IDENTIFY_CURSE))
			.c_str());
	pos.y += adj_grid_h;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(std::format("{:>14} {:>2}%", "ID Foes",
									   character->abilities().at(IDENTIFY_FOES))
							   .c_str());

	set_StyleColor(ImGuiCol_Text,
				   ImVec4{1.0f, 1.0f, 1.0f, _ctx.animation->fade});
	pos.y += (adj_grid_h * 2);
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}", "Agility", character->get_cur_attr(AGILITY))
			.c_str());

	set_StyleColor(ImGuiCol_Text,
				   ImVec4{0.5f, 0.5f, 0.5f, _ctx.animation->fade});
	pos.y += adj_grid_h;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:+>2}", "Int Mod",
					character->abilities().at(INITIATIVE_MODIFIER))
			.c_str());
	pos.y += adj_grid_h;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}%", "Crit Hit",
					character->abilities().at(BASE_CRITICAL_HIT))
			.c_str());
	pos.y += adj_grid_h;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(std::format("{:>14} {:>2}%", "ID Trap",
									   character->abilities().at(IDENTIFY_TRAP))
							   .c_str());
	pos.y += adj_grid_h;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}%", "Disarm Trap",
					character->abilities().at(BASE_DISARM_TRAP))
			.c_str());
	pos.y += adj_grid_h;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}%", "Avoid Trap",
					100 - character->abilities().at(ACTIVATE_TRAP))
			.c_str());
	pos.y += adj_grid_h;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}%", "Avoid Pit",
					character->abilities().at(BASE_AVOID_PIT))
			.c_str());
}

auto Sorcery::UI::_draw_character_mage_spells(Component *component,
											  [[maybe_unused]] Game *game,
											  Character *character) -> void {

	ImVec2 pos{component->x * adj_grid_w, component->y * adj_grid_h};
	ImGui::SetCursorPos(pos);
	with_Table("mage_spells_1", 3, ImGuiTableFlags_NoSavedSettings) {

		set_StyleColor(ImGuiCol_Text,
					   ImVec4{1.0f, 1.0f, 1.0f, _ctx.animation->fade});
		set_StyleColor(ImGuiCol_TextDisabled,
					   ImVec4{0.5f, 0.5f, 0.5f, _ctx.animation->fade});

		// Row order since we need to do via rows not columns
		auto comp_id{""s};
		auto spell_idx{0};
		for (auto spell_order = {0,	 4,	 6,	 1,	 5,	 7,	 2,	 -1, -1, 3,	 -1,
								 -1, -1, -1, -1, 8,	 11, 14, 9,	 12, 15, 10,
								 13, 16, -1, -1, 17, -1, -1, -1, 18, 19, 20};
			 const auto index : spell_order) {
			comp_id = std::format("##spell_{}", spell_idx);
			if (index != -1) {
				ImGui::TableNextColumn();

				auto spell_id{
					magic_enum::enum_cast<Enums::Magic::SpellID>(index)
						.value()};
				auto spells{character->spells() |
							std::views::filter([&](Spell spell) {
								return (spell.id == spell_id);
							})};
				auto spell{spells.begin()};

				if (!(*spell).known)
					ImGui::BeginDisabled();

				if (ImGui::Selectable((*spell).name.c_str(),
									  &ms_selected[unenum(spell_id)])) {
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
												[[maybe_unused]] Game *game,
												Character *character) -> void {

	ImVec2 pos{component->x * adj_grid_w, component->y * adj_grid_h};
	ImGui::SetCursorPos(pos);
	with_Table("priest_spells_1", 3, ImGuiTableFlags_NoSavedSettings) {

		set_StyleColor(ImGuiCol_Text,
					   ImVec4{1.0f, 1.0f, 1.0f, _ctx.animation->fade});
		set_StyleColor(ImGuiCol_TextDisabled,
					   ImVec4{0.5f, 0.5f, 0.5f, _ctx.animation->fade});

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

				auto spell_id{
					magic_enum::enum_cast<Enums::Magic::SpellID>(index)
						.value()};
				auto spells{character->spells() |
							std::views::filter([&](Spell spell) {
								return (spell.id == spell_id);
							})};
				auto spell{spells.begin()};

				if (!(*spell).known)
					ImGui::BeginDisabled();

				if (ImGui::Selectable((*spell).name.c_str(),
									  &ms_selected[unenum(spell_id)])) {
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
												 [[maybe_unused]] Game *game,
												 Character *character) -> void {

	const auto left_col{component->x + 0};
	const auto right_col{component->x + 19};

	using enum Enums::Character::Ability;
	using enum Enums::Character::Attribute;
	set_StyleColor(ImGuiCol_Text,
				   ImVec4{1.0f, 1.0f, 1.0f, _ctx.animation->fade});
	ImVec2 pos{left_col * adj_grid_w, component->y * adj_grid_h};
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}", "Piety", character->get_cur_attr(PIETY))
			.c_str());

	set_StyleColor(ImGuiCol_Text,
				   ImVec4{0.5f, 0.5f, 0.5f, _ctx.animation->fade});
	pos.y += adj_grid_h;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}%", "Spell Learn",
					character->abilities().at(PRIEST_SPELL_LEARN))
			.c_str());
	pos.y += adj_grid_h;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}%", "Rec Chance",
					character->abilities().at(LOKTOFELT_SUCCESS))
			.c_str());
	pos.y += adj_grid_h;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(std::format("{:>14} {:>2}%", "Base Dispell",
									   character->abilities().at(BASE_DISPELL))
							   .c_str());

	set_StyleColor(ImGuiCol_Text,
				   ImVec4{1.0f, 1.0f, 1.0f, _ctx.animation->fade});
	pos.y += (adj_grid_h * 2);
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}", "Luck", character->get_cur_attr(LUCK))
			.c_str());
	set_StyleColor(ImGuiCol_Text,
				   ImVec4{0.5f, 0.5f, 0.5f, _ctx.animation->fade});
	pos.y += adj_grid_h;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}%", "Res Bonus",
					character->abilities().at(BASE_RESIST_BONUS))
			.c_str());
	pos.y += adj_grid_h;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}%", "Wipe Rec",
					character->abilities().at(EQUIPMENT_INTACT_ON_WIPE))
			.c_str());

	pos = ImVec2{right_col * adj_grid_w, component->y * adj_grid_h};
	ImGui::SetCursorPos(pos);
	set_StyleColor(ImGuiCol_Text,
				   ImVec4{1.0f, 1.0f, 1.0f, _ctx.animation->fade});
	ImGui::TextUnformatted(std::format("{:>14}", "Resistances").c_str());

	set_StyleColor(ImGuiCol_Text,
				   ImVec4{0.5f, 0.5f, 0.5f, _ctx.animation->fade});
	pos.y += adj_grid_h;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}%", "vs Crit Hit",
					character->abilities().at(RESISTANCE_VS_CRITICAL_HIT) * 5)
			.c_str());
	pos.y += adj_grid_h;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}%", "vs Pois / Para",
					character->abilities().at(RESISTANCE_VS_POISON_PARALYSIS) *
						5)
			.c_str());
	pos.y += adj_grid_h;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}%", "vs Stoning",
					character->abilities().at(RESISTANCE_VS_STONING) * 5)
			.c_str());
	pos.y += adj_grid_h;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}%", "vs Breath",
					character->abilities().at(RESISTANCE_VS_BREATH_ATTACKS) * 5)
			.c_str());
	pos.y += adj_grid_h;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}%", "vs Gas Trap",
					character->abilities().at(RESISTANCE_VS_POISON_GAS_TRAP) *
						5)
			.c_str());
	pos.y += adj_grid_h;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}%", "vs Spell Trap",
					character->abilities().at(RESISTANCE_VS_MAGE_PRIEST_TRAP) *
						5)
			.c_str());
	pos.y += adj_grid_h;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}%", "vs Silence",
					character->abilities().at(RESISTANCE_VS_SILENCE) * 5)
			.c_str());
	pos.y += adj_grid_h;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}%", "vs Sleep",
					character->abilities().at(RESISTANCE_VS_KATINO))
			.c_str());
	pos.y += adj_grid_h;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}%", "vs Death",
					character->abilities().at(RESISTANCE_VS_BADI))
			.c_str());
	pos.y += adj_grid_h;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}%", "vs Statue",
					character->abilities().at(RESISTANCE_VS_MANIFO))
			.c_str());

	pos.y += adj_grid_h;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}%", "Rec / Sleep",
					character->abilities().at(RECOVER_FROM_SLEEP) * 5)
			.c_str());
	pos.y += adj_grid_h;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}%", "Rec / Fear",
					character->abilities().at(RECOVER_FROM_FEAR) * 5)
			.c_str());
}

auto Sorcery::UI::_draw_character_summary(Component *component,
										  [[maybe_unused]] Game *game,
										  Character *character) -> void {

	const auto left_col{component->x};
	const auto middle_col{component->x + 13};
	const auto right_col{component->x + 31};

	using enum Enums::Character::Ability;
	using enum Enums::Character::Attribute;
	ImVec2 pos{};
	pos = ImVec2{left_col * adj_grid_w, component->y * adj_grid_h};
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(std::format("{:>8} {:>2}", "Strength",
									   character->get_cur_attr(STRENGTH))
							   .c_str());
	pos = ImVec2{left_col * adj_grid_w, (component->y + 1) * adj_grid_h};
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>8} {:>2}", "I.Q.", character->get_cur_attr(IQ))
			.c_str());
	pos = ImVec2{left_col * adj_grid_w, (component->y + 2) * adj_grid_h};
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>8} {:>2}", "Piety", character->get_cur_attr(PIETY))
			.c_str());
	pos = ImVec2{left_col * adj_grid_w, (component->y + 3) * adj_grid_h};
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(std::format("{:>8} {:>2}", "Vitality",
									   character->get_cur_attr(VITALITY))
							   .c_str());
	pos = ImVec2{left_col * adj_grid_w, (component->y + 4) * adj_grid_h};
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>8} {:>2}", "Agility", character->get_cur_attr(AGILITY))
			.c_str());
	pos = ImVec2{left_col * adj_grid_w, (component->y + 5) * adj_grid_h};
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>8} {:>2}", "Luck", character->get_cur_attr(LUCK))
			.c_str());

	pos = ImVec2{left_col * adj_grid_w, (component->y + 7) * adj_grid_h};
	ImGui::SetCursorPos(pos);
	auto mage{std::format(
		"{}/{}/{}/{}/{}/{}/{}", character->mage_cur_sp().at(1),
		character->mage_cur_sp().at(2), character->mage_cur_sp().at(3),
		character->mage_cur_sp().at(4), character->mage_cur_sp().at(5),
		character->mage_cur_sp().at(6), character->mage_cur_sp().at(7))};
	ImGui::TextUnformatted(std::format("Mage {}", mage).c_str());

	pos = ImVec2{middle_col * adj_grid_w, component->y * adj_grid_h};
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:<6} {:>10}", "Gold", character->get_gold()).c_str());
	pos = ImVec2{middle_col * adj_grid_w, (component->y + 1) * adj_grid_h};
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:<6} {:>10}", "E.P.", character->get_cur_xp()).c_str());
	pos = ImVec2{middle_col * adj_grid_w, (component->y + 2) * adj_grid_h};
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:<6} {:>10}", "Next", character->get_next_xp()).c_str());
	pos = ImVec2{middle_col * adj_grid_w, (component->y + 3) * adj_grid_h};
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:<6} {:>10}", "Marks", character->abilities().at(MARKS))
			.c_str());
	pos = ImVec2{middle_col * adj_grid_w, (component->y + 4) * adj_grid_h};
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(std::format("{:<4} {}/{}", "H.P.",
									   character->abilities().at(CURRENT_HP),
									   character->abilities().at(MAX_HP))
							   .c_str());
	pos = ImVec2{middle_col * adj_grid_w, (component->y + 5) * adj_grid_h};
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:<6} {:>18}", "Status", character->get_status_string())
			.c_str());

	auto priest{std::format(
		"{}/{}/{}/{}/{}/{}/{}", character->priest_cur_sp().at(1),
		character->priest_cur_sp().at(2), character->priest_cur_sp().at(3),
		character->priest_cur_sp().at(4), character->priest_cur_sp().at(5),
		character->priest_cur_sp().at(6), character->priest_cur_sp().at(7))};
	pos = ImVec2{(component->x + 20) * adj_grid_w,
				 (component->y + 7) * adj_grid_h};
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(std::format("Prst {}", priest).c_str());

	pos = ImVec2{right_col * adj_grid_w, component->y * adj_grid_h};
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("Swim{:>3}", character->abilities().at(SWIM)).c_str());
	pos = ImVec2{right_col * adj_grid_w, (component->y + 1) * adj_grid_h};
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format(" Age{:>3}", character->abilities().at(AGE) / 52).c_str());
	pos = ImVec2{right_col * adj_grid_w, (component->y + 2) * adj_grid_h};
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format(" RIP{:>3}", character->abilities().at(DEATHS)).c_str());

	auto slot{1u};
	pos = ImVec2{left_col * adj_grid_w, (component->y + 9) * adj_grid_h};
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
			pos = ImVec2{left_col * adj_grid_w,
						 (component->y + 9 + slot / 2) * adj_grid_h};
		else
			pos = ImVec2{(component->y + 17) * adj_grid_w,
						 (component->y + 9 + (slot - 1) / 2) * adj_grid_h};
		ImGui::SetCursorPos(pos);
		auto line{std::format("{}){}{}", slot, flag, item.get_display_name())};
		ImGui::TextUnformatted(line.c_str());
		++slot;
	}
}

auto Sorcery::UI::_draw_create_alignment(Game *game, const int mode) -> void {

	auto cmp_summary{components->get("create_alignment:summary_text")};
	auto summary_text{_ctx.controller->get_character()->summary_text()};
	_draw_text(&cmp_summary, summary_text);
}

auto Sorcery::UI::_draw_create_confirm(Game *game, const int mode) -> void {

	auto cmp_summary{components->get("create_confirm:summary_text")};
	auto summary_text{_ctx.controller->get_character()->summary_text()};
	_draw_text(&cmp_summary, summary_text);

	auto cmp_char{components->get("create_confirm:character_data")};
	with_Window(WINDOW_LAYER_TEXTS, nullptr,
				ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs) {
		set_Font(fontstore->get_current_font(cmp_char.font).value());
		_draw_character_summary(&cmp_char, game,
								_ctx.controller->get_character());
	}
}

auto Sorcery::UI::_draw_create_class(Game *game, const int mode) -> void {

	auto cmp_summary{components->get("create_class:summary_text")};
	auto summary_text{_ctx.controller->get_character()->summary_text()};
	_draw_text(&cmp_summary, summary_text);

	auto cmp_points_left{components->get("create_class:points_left_text")};
	const auto points_left_text{std::format(
		"{:>2}", _ctx.controller->get_character()->get_points_left())};
	_draw_text(&cmp_points_left, points_left_text);

	// Now draw the class buttons
	using enum Enums::Character::Attribute;
	auto cmp_attribute{components->get("create_class:current_stats")};
	for (auto i = unenum(STRENGTH); i <= unenum(LUCK); ++i) {
		auto attribute{_ctx.controller->get_character()->get_attr_ptr(
			magic_enum::enum_cast<Enums::Character::Attribute>(i).value())};
		auto cmp_name{std::format("stepper_attribute_{}", i)};
		_draw_stepper(&cmp_attribute, cmp_name, attribute);
		++cmp_attribute.y;
	}
}

auto Sorcery::UI::_draw_create_race(Game *game, const int mode) -> void {

	auto cmp_summary{components->get("create_race:summary_text")};
	auto summary_text{_ctx.controller->get_character()->summary_text()};
	_draw_text(&cmp_summary, summary_text);
}

auto Sorcery::UI::_draw_create_name(Game *game, const int mode) -> void {

	auto cmp_summary{components->get("create_name:summary_text")};
	auto summary_text{_ctx.controller->get_character()->summary_text()};
	_draw_text(&cmp_summary, summary_text);

	// As next custom component is a text box, focus on that initially
	if (first_frame) {
		ImGui::SetKeyboardFocusHere();
		first_frame = false;
	}

	auto cmp_name{components->get("create_name:name_input")};
	_draw_input(&cmp_name, &_ctx.controller->get_input_buffer());
}

auto Sorcery::UI::_display_create_alignment(Game *game, const int mode)
	-> void {

	_draw_components("create_alignment", game, mode);
	_draw_create_alignment(game, mode);
	_draw_cursor();
}

auto Sorcery::UI::_display_create_confirm(Game *game, const int mode) -> void {

	_draw_components("create_confirm", game, mode);
	_draw_create_confirm(game, mode);
	_draw_cursor();
}

auto Sorcery::UI::_display_create_class(Game *game, const int mode) -> void {

	_draw_components("create_class", game, mode);
	_draw_create_class(game, mode);
	_draw_cursor();
}

auto Sorcery::UI::_display_create_race(Game *game, const int mode) -> void {

	_draw_components("create_race", game, mode);
	_draw_create_race(game, mode);
	_draw_cursor();
}

auto Sorcery::UI::_display_create_name(Game *game, const int mode) -> void {

	_draw_components("create_name", game, mode);
	_draw_create_name(game, mode);
	_draw_cursor();
}

auto Sorcery::UI::_draw_choose(Game *game, const int mode) -> void {

	if (mode & CHOOSE_MODE_STAY) {

		auto cmp{components->get("choose:choose_stay")};
		_draw_text(&cmp);
		_draw_party_panel(game);
	}
}

auto Sorcery::UI::_draw_level_up(Game *game, const int mode) -> void {

	auto &character{
		game->characters.at(_ctx.controller->get_character("restart"))};

	if (mode & RECOVERY_BIRTHDAY) {

		const auto birth_text{_ctx.get_string("REST_BIRTHDAY_YOU")};
		auto cmp{components->get("levelup:levelup_birthday")};
		_draw_text(&cmp, birth_text);
		cmp = components->get("levelup:levelup_results");
		for (const auto &result : character.level_up_results) {
			_draw_text(&cmp, result);
			cmp.y += adj_grid_h;
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

auto Sorcery::UI::_draw_pay_info(Game *game) -> void {

	// Work out healing cost
	const auto character{
		game->characters.at(_ctx.controller->get_character("help"))};
	const auto cost(character.get_cure_cost());
	const auto cost_text{std::format("{} {} {}",
									 _ctx.get_string("PAY_COST_PREFIX"), cost,
									 _ctx.get_string("PAY_COST_SUFFIX"))};
	auto cmp{components->get("pay:pay_cost")};
	_draw_text(&cmp, cost_text);
}

auto Sorcery::UI::_draw_no_level_up(Game *game, const int mode) -> void {

	const auto character{
		game->characters.at(_ctx.controller->get_character("stay"))};
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

auto Sorcery::UI::_draw_heal([[maybe_unused]] Game *game, int stage) -> void {

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
	case 0:
		text = _ctx.get_string("TEMPLE_HEAL_4");
		break;
	default:
		break;
	}
	_draw_text(&cmp, text);

	if (_ctx.controller->has_flag("heal_finished")) {

		if (_ctx.controller->has_text("heal_results")) {
			auto summary{components->get("heal:heal_results")};
			auto results{_ctx.controller->get_text("heal_results")};
			_draw_text(&summary, results);

			with_Window(WINDOW_LAYER_MENUS, nullptr,
						ImGuiWindowFlags_NoTitleBar) {
				auto leave{components->get("heal:heal_return")};
				_draw_button_click(&leave, _ctx.get_flag_ref("show_heal"),
								   true);
			}
		}
	}
}

auto Sorcery::UI::_draw_recovery(Game *game, const int mode) -> void {

	const auto character{
		game->characters.at(_ctx.controller->get_character("stay"))};
	if (mode & RECOVERY_MODE_FREE) {

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

auto Sorcery::UI::_draw_stay(Game *game) -> void {

	const auto character{
		game->characters.at(_ctx.controller->get_character("stay"))};

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

auto Sorcery::UI::_draw_current_character(Game *game,
										  [[maybe_unused]] const int mode)
	-> void {

	auto character{
		game->characters.at(_ctx.controller->get_character("inspect"))};

	auto title{components->get("inspect:character_title")};
	_draw_text(&title, character.summary_text());

	with_Window(WINDOW_LAYER_MENUS, nullptr, ImGuiWindowFlags_NoTitleBar) {
		auto prev{components->get("inspect:character_previous")};
		_draw_button_click(&prev,
						   _ctx.get_flag_ref("select_previous_character"));
		auto next{components->get("inspect:character_next")};
		_draw_button_click(&next, _ctx.get_flag_ref("select_next_character"));

		auto cmp{components->get("inspect:character_data")};
		ImVec2 pos{cmp.x * adj_grid_w, cmp.y * adj_grid_h};

		ImGuiTabBarFlags tb_flags{ImGuiTabBarFlags_None};
		ImGui::SetCursorPos(pos);
		with_Child("character_tab_bar_child",
				   ImVec2(grid_sz * cmp.w, grid_sz * cmp.h)) {
			set_StyleColor(ImGuiCol_Tab,
						   ImVec4{0.0f, 0.0f, 0.0f, _ctx.animation->fade});
			auto char_cmp{components->get("inspect:character_tab_data")};
			set_Font(fontstore->get_current_font(cmp.font).value());
			with_TabBar("character_tab_bar", tb_flags) {
				with_TabItem("Info") {
					_draw_character_summary(&char_cmp, game, &character);
				}
				with_TabItem("Stats##1") {
					_draw_character_detailed(&char_cmp, game, &character);
				}
				with_TabItem("Stats##2") {
					_draw_character_detailed_again(&char_cmp, game, &character);
				}
				with_TabItem("Arcane") {
					_draw_character_mage_spells(&char_cmp, game, &character);
				}
				with_TabItem("Divine") {
					_draw_character_priest_spells(&char_cmp, game, &character);
				}
			}
		}
	}
}

auto Sorcery::UI::_draw_stepper(Component *component, const std::string &name,
								int *value) -> void {

	bool disabled{false};

	with_Window(WINDOW_LAYER_MENUS, nullptr, ImGuiWindowFlags_NoTitleBar) {

		ImVec2 pos{component->x * adj_grid_w, component->y * adj_grid_h};
		ImGui::SetCursorPos(pos);

		set_Font(fontstore->get_current_font(component->font).value());

		const auto stepper_name{std::format("##{}", name)};
		const auto stepper_minus{std::format("##{}_minus", name)};
		const auto stepper_plus{std::format("##{}_plus", name)};

		const auto col{get_hl_colour(_ctx.animation->lerp)};
		set_StyleColor(ImGuiCol_Text,
					   ImVec4{1.0f, 1.0f, 1.0f, _ctx.animation->fade});
		set_StyleColor(ImGuiCol_Button,
					   ImVec4{0.0f, 0.0f, 0.0f, _ctx.animation->fade});
		set_StyleColor(ImGuiCol_ButtonHovered, (ImVec4)col);

		if (component->name == "current_stats") {
			using enum Enums::Character::Attribute;
			const auto mins{_ctx.controller->get_character()->get_start_attr()};
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

		// const auto
		// classes{_ctx.controller->get_character()->get_pos_class()};

		pos.x += 1 * adj_grid_w;

		ImVec4 alpha_col{ImGui::ColorConvertU32ToFloat4(component->colour)};
		alpha_col.w = _ctx.animation->fade;

		set_StyleColor(ImGuiCol_Text, alpha_col);
		ImGui::SetCursorPos(pos);
		ImGui::TextUnformatted(std::format("{:>2}", *value).c_str());

		disabled = false;
		if (component->name == "current_stats") {
			if ((*value >= 18) ||
				(_ctx.controller->get_character()->get_points_left() == 0))
				disabled = true;
		};

		pos.x += 2 * adj_grid_w;
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

		ImVec2 pos{component->x * adj_grid_w, component->y * adj_grid_h};
		ImGui::SetCursorPos(pos);

		set_Font(fontstore->get_current_font(component->font).value());

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
		set_StyleColor(ImGuiCol_Text,
					   ImVec4{1.0f, 1.0f, 1.0f, _ctx.animation->fade});
		set_StyleColor(ImGuiCol_Button,
					   ImVec4{0.0f, 0.0f, 0.0f, _ctx.animation->fade});
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
		set_Font(fontstore->get_current_font(component->font).value());

		const auto x{std::invoke([&] {
			if (component->x == -1) {
				const auto viewport{ImGui::GetMainViewport()};
				const auto width{ImGui::CalcTextSize(CSTR(string))};
				return (viewport->Size.x - width.x) / 2;
			} else
				return static_cast<float>(adj_grid_w * component->x);
		})};
		const auto y{std::invoke([&] {
			if (component->y == -1) {
				const auto viewport{ImGui::GetMainViewport()};
				const auto height{ImGui::CalcTextSize(CSTR(string))};
				return (viewport->Size.y - height.y) / 2;
			} else
				return static_cast<float>(adj_grid_h * component->y);
		})};

		// Adjust Alpha of Text
		ImVec4 alpha_col{ImGui::ColorConvertU32ToFloat4(component->colour)};
		alpha_col.w = _ctx.animation->fade;

		set_StyleColor(ImGuiCol_Text, alpha_col);
		ImGui::SetCursorPos(ImVec2{x, y});
		ImGui::TextUnformatted(string.c_str());
	}
}

// Draw a Text (String)
auto Sorcery::UI::_draw_text(Component *component) -> void {
	with_Window(WINDOW_LAYER_TEXTS, nullptr,
				ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs) {

		// Need to push font first before calculating size else it will
		// assume monospace font size!
		set_Font(fontstore->get_current_font(component->font).value());

		const auto x{std::invoke([&] {
			if (component->x == -1) {
				const auto viewport{ImGui::GetMainViewport()};
				const auto width{ImGui::CalcTextSize(
					CSTR(_ctx.get_string(component->string_key)))};
				return (viewport->Size.x - width.x) / 2;
			} else
				return static_cast<float>(adj_grid_w * component->x);
		})};
		const auto y{std::invoke([&] {
			if (component->y == -1) {
				const auto viewport{ImGui::GetMainViewport()};
				const auto height{ImGui::CalcTextSize(
					CSTR(_ctx.get_string(component->string_key)))};
				return (viewport->Size.y - height.y) / 2;
			} else
				return static_cast<float>(adj_grid_h * component->y);
		})};

		// Adjust Alpha of Text
		ImVec4 alpha_col{ImGui::ColorConvertU32ToFloat4(component->colour)};
		alpha_col.w = _ctx.animation->fade;

		set_StyleColor(ImGuiCol_Text, alpha_col);
		ImGui::SetCursorPos(ImVec2{x, y});
		ImGui::TextUnformatted(_ctx.get_string(component->string_key).c_str());
	}
}

auto Sorcery::UI::_draw_components(std::string_view screen, Game *game,
								   [[maybe_unused]] const int mode) -> void {

	_frames.clear();
	_menus.clear();

	// Draw every component specified in order
	for (auto cmps{(*components)(screen)}; auto c : cmps.value()) {
		using enum Enums::Layout::ComponentType;
		if (c.type == IMAGE_BG)
			_draw_bg_image(&c);
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
			_draw_menu(&c, game);
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
	auto item_pos{ImVec2{item_c.x * adj_grid_w, item_c.y * adj_grid_h}};
	_draw_fg_image_with_idx(
		ITEMS_TEXTURE, idx, item_pos,
		ImVec2{std::stof(item_c.get("tile_width").value()),
			   std::stof(item_c.get("tile_width").value())});

	auto cmp{components->get("museum:item_data")};
	ImVec2 pos{cmp.x * adj_grid_w, cmp.y * adj_grid_h};

	with_Window(WINDOW_LAYER_MENUS, nullptr, ImGuiWindowFlags_NoDecoration) {

		const auto name{std::format("  {:>03}:{}/{}", idx + 1,
									item.get_known_name(),
									item.get_unknown_name())};

		ImGuiTabBarFlags tb_flags{ImGuiTabBarFlags_None};
		ImGui::SetCursorPos(pos);
		with_Child("museum_tab_bar_child",
				   ImVec2(grid_sz * cmp.w, grid_sz * cmp.h)) {
			set_StyleColor(ImGuiCol_Tab,
						   ImVec4{0.0f, 0.0f, 0.0f, _ctx.animation->fade});
			set_Font(fontstore->get_current_font(cmp.font).value());
			with_TabBar("museum_tab_bar", tb_flags) {
				with_TabItem("Info") {
					{
						set_StyleColor(
							ImGuiCol_Text,
							ImVec4{1.0f, 1.0f, 1.0f, _ctx.animation->fade});
						ImGui::TextUnformatted(name.c_str());
					}

					set_StyleColor(ImGuiCol_Text, ImVec4{0.8f, 0.8f, 0.8f,
														 _ctx.animation->fade});
					std::string item_category{
						magic_enum::enum_name(item.get_category())};
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
						set_StyleColor(
							ImGuiCol_Text,
							ImVec4{1.0f, 1.0f, 1.0f, _ctx.animation->fade});
						ImGui::TextUnformatted(name.c_str());
					}

					set_StyleColor(ImGuiCol_Text, ImVec4{0.8f, 0.8f, 0.8f,
														 _ctx.animation->fade});
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
						set_StyleColor(
							ImGuiCol_Text,
							ImVec4{1.0f, 1.0f, 1.0f, _ctx.animation->fade});
						ImGui::TextUnformatted(name.c_str());
					}

					set_StyleColor(ImGuiCol_Text, ImVec4{0.8f, 0.8f, 0.8f,
														 _ctx.animation->fade});
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
						set_StyleColor(
							ImGuiCol_Text,
							ImVec4{1.0f, 1.0f, 1.0f, _ctx.animation->fade});
						ImGui::TextUnformatted(name.c_str());
					}
					set_StyleColor(ImGuiCol_Text, ImVec4{0.8f, 0.8f, 0.8f,
														 _ctx.animation->fade});

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
			const auto width{grid_sz *
							 std::stof(component->get("grid_width").value())};
			const auto viewport{ImGui::GetMainViewport()};
			return (viewport->Size.x - width) / 2;
		})};

		const auto pos{ImVec2{x, component->y * adj_grid_h}};
		ImGui::SetNextWindowPos(pos);
		with_Child("license_child",
				   ImVec2(grid_sz * component->w, grid_sz * component->h),
				   ImGuiChildFlags_NavFlattened,
				   ImGuiWindowFlags_AlwaysVerticalScrollbar) {

			set_StyleColor(ImGuiCol_Text,
						   ImVec4{0.8f, 0.8f, 0.8f, _ctx.animation->fade});
			set_Font(fontstore->get_current_font(component->font).value());
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
		return {"GAME_MESSAGE_AREA_OUT_OF_BOUNDS_1",
				"GAME_MESSAGE_AREA_OUT_OF_BOUNDS_2"};
		break;
	case MAN_TELEPORT_CASTLE:
		return {"GAME_MESSAGE_MAN_TELEPORT_CASTLE_1",
				"GAME_MESSAGE_MAN_TELEPORT_CASTLE_2",
				"GAME_MESSAGE_MAN_TELEPORT_CASTLE_3",
				"GAME_MESSAGE_MAN_TELEPORT_CASTLE_4",
				"GAME_MESSAGE_MAN_TELEPORT_CASTLE_5",
				"GAME_MESSAGE_MAN_TELEPORT_CASTLE_6"};
		break;
	case SILVER_KEY:
		return {"GAME_MESSAGE_SILVER_KEY_1", "GAME_MESSAGE_SILVER_KEY_2",
				"GAME_MESSAGE_SILVER_KEY_3", "GAME_MESSAGE_SILVER_KEY_4",
				"GAME_MESSAGE_SILVER_KEY_5", "GAME_MESSAGE_SILVER_KEY_6",
				"GAME_MESSAGE_SILVER_KEY_7", "GAME_MESSAGE_SILVER_KEY_8"};
		break;
	case BRONZE_KEY:
		return {"GAME_MESSAGE_BRONZE_KEY_1", "GAME_MESSAGE_BRONZE_KEY_2",
				"GAME_MESSAGE_BRONZE_KEY_3", "GAME_MESSAGE_BRONZE_KEY_4",
				"GAME_MESSAGE_BRONZE_KEY_5", "GAME_MESSAGE_BRONZE_KEY_6"};
		break;
	case MURPHYS_GHOSTS:
		return {
			"GAME_MESSAGE_MURPHYS_GHOSTS_1", "GAME_MESSAGE_MURPHYS_GHOSTS_2",
			"GAME_MESSAGE_MURPHYS_GHOSTS_3", "GAME_MESSAGE_MURPHYS_GHOSTS_4",
			"GAME_MESSAGE_MURPHYS_GHOSTS_5"};
		break;
	case BEAR_STATUE:
		return {"GAME_MESSAGE_BEAR_STATUE_1", "GAME_MESSAGE_BEAR_STATUE_2",
				"GAME_MESSAGE_BEAR_STATUE_3"};
		break;
	case FROG_STATUE:
		return {"GAME_MESSAGE_FROG_STATUE_1", "GAME_MESSAGE_FROG_STATUE_2",
				"GAME_MESSAGE_FROG_STATUE_3", "GAME_MESSAGE_FROG_STATUE_4"};
		break;
	case GOLD_KEY:
		return {"GAME_MESSAGE_GOLD_KEY_1", "GAME_MESSAGE_GOLD_KEY_2",
				"GAME_MESSAGE_GOLD_KEY_3", "GAME_MESSAGE_GOLD_KEY_4",
				"GAME_MESSAGE_GOLD_KEY_5", "GAME_MESSAGE_GOLD_KEY_6"};
		break;
	case NEED_SILVER_KEY:
		return {};
		break;
	case NEED_BRONZE_KEY:
		return {};
		break;
	case NEED_BEAR_STATUE:
		return {};
		break;
	case NEED_FROG_STATUE:
		return {};
		break;
	case PLACARD_PIT_1:
		return {};
		break;
	case PLACARD_PIT_2:
		return {};
		break;
	case PLACARD_PIT_3:
		return {};
		break;
	case TURN_AROUND:
		return {};
		break;
	case TURN_LEFT:
		return {};
		break;
	case TURN_RIGHT:
		return {};
		break;
	case NEED_BEAR_STATUE_2:
		return {};
		break;
	case TESTING_GROUNDS:
		return {};
		break;
	case ALARM_BELLS:
		return {};
		break;
	case TREASURE_REPOSITORY:
		return {};
		break;
	case MONSTER_ALLOCATION_CENTRE:
		return {};
		break;
	case LARGE_DESK:
		return {};
		break;
	case TREBOR_VOICE:
		return {};
		break;
	case SERVICE_ELEVATOR:
		return {};
		break;
	case WERDNA_BOAST:
		return {};
		break;
	case TURN_BACK:
		return {};
		break;
	case WERDNA_SIGN:
		return {};
		break;
	case THREE_HUMANOIDS:
		return {};
		break;
	case GETTING_WARM_1:
		return {};
		break;
	case GETTING_WARM_2:
		return {};
		break;
	case GETTING_WARM_3:
		return {};
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
	case GUARANTEED_COMBAT:
		return {};
		break;
	case NEED_BLUE_RIBBON:
		return {};
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
										   "OPT_FULLSCREEN"};

	const auto save_lbl{_ctx.get_string("DIALOG_SAVE")};
	const auto cancel_lbl{_ctx.get_string("DIALOG_CANCEL")};
	set_Font(fontstore->get_current_font(component.font).value());
	const auto col{get_hl_colour(_ctx.animation->lerp)};
	with_Window(WINDOW_LAYER_MENUS, nullptr, ImGuiWindowFlags_NoDecoration) {

		// To adjust for Window Resizing etc
		const auto x{std::invoke([&] {
			const auto width{grid_sz *
							 std::stof(component.get("grid_width").value())};
			const auto viewport{ImGui::GetMainViewport()};
			return (viewport->Size.x - width) / 2;
		})};

		const auto pos{ImVec2{x, component.y * adj_grid_h}};
		ImGui::SetCursorPos(pos);

		// Now draw tab bar
		set_StyleColor(ImGuiCol_Text,
					   ImVec4{1.0f, 1.0f, 1.0f, _ctx.animation->fade});
		set_StyleColor(ImGuiCol_Button,
					   ImVec4{0.0f, 0.0f, 0.0f, _ctx.animation->fade});
		set_StyleColor(ImGuiCol_ButtonHovered, (ImVec4)col);
		const auto tabs_width{component.w * grid_sz};
		const auto tabs_height{component.h * grid_sz};
		ImGuiTabBarFlags tb_flags{ImGuiTabBarFlags_None};
		with_Child("options_tab_bar_child", ImVec2(tabs_width, tabs_height)) {
			set_StyleColor(ImGuiCol_Tab, ImVec4{0.0f, 0.0f, 0.0f, 0.0f});

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
				tabname = "Display";
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
					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + grid_sz);
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
							set_Font(font.font);
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

			set_StyleColor(ImGuiCol_Text,
						   ImVec4{1.0f, 1.0f, 1.0f, _ctx.animation->fade});
			set_StyleColor(ImGuiCol_Button,
						   ImVec4{0.0f, 0.0f, 0.0f, _ctx.animation->fade});
			set_StyleColor(ImGuiCol_ButtonHovered, ImVec4{col});
			set_StyleColor(ImGuiCol_ButtonActive, (ImVec4)col);

			ImGui::SetCursorPos(
				ImVec2{centre - (btn_size.x + grid_sz), button_y * grid_sz});
			if (ImGui::Button(save_lbl.c_str(), btn_size)) {
				_ctx.system->config->save();
				_ctx.controller->unset_flag("show_options");
			}
			ImGui::SetCursorPos(ImVec2{centre + grid_sz, button_y * grid_sz});
			if (ImGui::Button(cancel_lbl.c_str(), btn_size)) {
				_ctx.system->config->load();
				_ctx.controller->unset_flag("show_options");
			}
		}
	}
}
auto Sorcery::UI::_draw_buffbar(Game *game) -> void {
	auto cmp{components->get("engine_base_ui:buffbar")};
	auto frame_cmp{components->get("engine_base_ui:buffbar_frame")};

	const auto x{static_cast<float>(adj_grid_w * cmp.x)};
	auto y{static_cast<float>(adj_grid_h * cmp.y)};
	const auto width{cmp.w * grid_sz};
	const auto height{cmp.h * grid_sz};

	auto tint{_ctx.controller->get_monochrome()
				  ? ImVec4{1.0f, 1.0f, 1.0f, _ctx.animation->fade}
				  : ImVec4{1.0f, 0.33f, 0.33f, _ctx.animation->fade}};

	with_Window(WINDOW_LAYER_TEXTS, nullptr, ImGuiWindowFlags_NoDecoration) {

		_draw_frame(&frame_cmp);
		ImGui::SetCursorPos(ImVec2{x, y});
		const auto light_idx{game->state->get_lit() ? ICON_BUFF_EXTRA_LIGHT
													: ICON_BUFF_LIGHT};
		_draw_fg_image_with_idx(WINDOW_LAYER_TEXTS, ICONS_TEXTURE, light_idx,
								ImVec2{x, y}, ImVec2{width, height}, tint);

		y += height;

		// TODO
	}
}

auto Sorcery::UI::_draw_icons([[maybe_unused]] Game *game) -> void {
	auto cmp{components->get("engine_base_ui:icons")};
	auto frame_cmp{components->get("engine_base_ui:icons_frame")};

	const auto icons = {ICON_CAMP, ICON_PARTY, ICON_MAP,
						ICON_LOOK, ICON_CAST,  ICON_USE};

	const auto x{static_cast<float>(adj_grid_w * cmp.x)};
	auto y{static_cast<float>(adj_grid_h * cmp.y)};
	const auto width{cmp.w * grid_sz};
	const auto height{cmp.h * grid_sz};

	auto tint{_ctx.controller->get_monochrome()
				  ? ImVec4{1.0f, 1.0f, 1.0f, _ctx.animation->fade}
				  : ImVec4{0.33f, 1.0f, 1.0f, _ctx.animation->fade}};

	with_Window(WINDOW_LAYER_TEXTS, nullptr, ImGuiWindowFlags_NoDecoration) {
		_draw_frame(&frame_cmp);

		for (const auto icon_idx : icons) {
			ImGui::SetCursorPos(ImVec2{x, y});
			_draw_fg_image_with_idx(WINDOW_LAYER_TEXTS, ICONS_TEXTURE, icon_idx,
									ImVec2{x, y}, ImVec2{width, height}, tint);
			y += (height + 3);
		}
	}
}

auto Sorcery::UI::_draw_save([[maybe_unused]] Game *game) -> void {

	auto cmp{components->get("engine_base_ui:save")};
	auto frame_cmp{components->get("engine_base_ui:save_frame")};

	const auto x{static_cast<float>(adj_grid_w * cmp.x)};
	const auto y{static_cast<float>(adj_grid_h * cmp.y)};
	const auto width{cmp.w * grid_sz};
	const auto height{cmp.h * grid_sz};

	auto tint{_ctx.controller->get_monochrome()
				  ? ImVec4{1.0f, 1.0f, 1.0f, _ctx.animation->fade}
				  : ImVec4{0.33f, 1.0f, 1.0f, _ctx.animation->fade}};

	with_Window(WINDOW_LAYER_TEXTS, nullptr, ImGuiWindowFlags_NoDecoration) {

		_draw_frame(&frame_cmp);
		ImGui::SetCursorPos(ImVec2{x, y});
		with_Child("save_child", ImVec2(width, height)) {
			_draw_fg_image_with_idx(WINDOW_LAYER_TEXTS, ICONS_TEXTURE,
									ICON_SAVE_AND_QUIT, ImVec2{x, y},
									ImVec2{width, height}, tint);
		}
	}
}

auto Sorcery::UI::_draw_compass(Game *game) -> void {

	auto cmp{components->get("engine_base_ui:compass")};
	auto frame_cmp{components->get("engine_base_ui:compass_frame")};

	auto tint{_ctx.controller->get_monochrome()
				  ? ImVec4{1.0f, 1.0f, 1.0f, _ctx.animation->fade}
				  : ImVec4{1.0f, 0.33f, 0.33f, _ctx.animation->fade}};

	const auto x{static_cast<float>(adj_grid_w * cmp.x)};
	const auto y{static_cast<float>(adj_grid_h * cmp.y)};
	const auto width{cmp.w * grid_sz};
	const auto height{cmp.h * grid_sz};

	with_Window(WINDOW_LAYER_TEXTS, nullptr, ImGuiWindowFlags_NoDecoration) {

		_draw_frame(&frame_cmp);
		ImGui::SetCursorPos(ImVec2{x, y});
		with_Child("compass_child", ImVec2(width, height)) {
			auto icon_idx{ICON_COMPASS_NORTH};
			switch (game->state->get_player_facing()) {
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

auto Sorcery::UI::_draw_party_panel(Game *game) -> void {

	auto cmp{components->get("global:party_panel")};
	auto frame_cmp{components->get("engine_base_ui:party_frame")};

	const auto width{cmp.w * grid_sz};
	const auto height{cmp.h * grid_sz};
	const auto x{std::invoke([&] {
		if (cmp.x == -1) {
			const auto viewport{ImGui::GetMainViewport()};
			return (viewport->Size.x - width) / 2;
		} else
			return static_cast<float>(adj_grid_w * cmp.x);
	})};
	const auto y{static_cast<float>(adj_grid_h * cmp.y)};
	with_Window(WINDOW_LAYER_TEXTS, nullptr, ImGuiWindowFlags_NoDecoration) {

		_draw_frame(&frame_cmp);
		ImGui::SetCursorPos(ImVec2{x, y});
		with_Child("party_panel_child", ImVec2(width, height)) {
			set_Font(fontstore->get_current_font(cmp.font).value());
			const auto hl_col{get_hl_colour(_ctx.animation->lerp)};
			set_StyleColor(ImGuiCol_Text,
						   ImVec4{1.0f, 1.0f, 1.0f, _ctx.animation->fade});
			ImGui::TextUnformatted(
				_ctx.get_string("PARTY_PANEL_LEGEND").c_str());
			if (game->state->get_party_size() > 0) {
				auto position{1u};
				for (const auto party{game->state->get_party_characters()};
					 auto char_id : party) {
					auto p_y{0 + (position * grid_sz)};
					auto character{game->characters.at(char_id)};
					auto colour{_get_status_color(&character)};
					auto summary{character.get_party_panel_text(position)};
					set_StyleColor(ImGuiCol_Text, colour);
					ImGui::SetCursorPos(ImVec2{0, p_y});
					ImGui::TextUnformatted(summary.c_str());
					++position;
				}
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
	ImVec2 pos{cmp.x * adj_grid_w, cmp.y * adj_grid_h};
	ImGui::SetNextWindowPos(pos);
	with_Window(WINDOW_LAYER_TEXTS, nullptr, ImGuiWindowFlags_NoDecoration) {
		with_Child("spell_child", ImVec2(grid_sz * cmp.w, grid_sz * cmp.h)) {

			auto spell{_ctx.resources->spells->get(
				magic_enum::enum_cast<Enums::Magic::SpellID>(idx).value())};

			const auto spell_name{
				std::format("{} \"{}\"", spell.name, spell.translated_name)};
			const auto spell_type{spell.type == Enums::Magic::SpellType::ARCANE
									  ? "Mage"
									  : "Priest"};

			std::string spell_cat{
				magic_enum::enum_name<Enums::Magic::SpellCategory>(
					spell.category)};
			std::transform(spell_cat.begin(), spell_cat.end(),
						   spell_cat.begin(), ::tolower);
			auto summary{std::format("Level {} {} {} spell", spell.level,
									 spell_type, spell_cat)};

			{
				set_StyleColor(ImGuiCol_Text,
							   ImVec4{1.0f, 1.0f, 1.0f, _ctx.animation->fade});
				set_Font(fontstore->get_current_font(cmp.font).value());
				ImGui::TextUnformatted(spell_name.c_str());
				ImGui::NewLine();
				ImGui::TextUnformatted(summary.c_str());
				ImGui::NewLine();
			}

			set_Font(fontstore->get_current_font(cmp.font).value());
			set_StyleColor(ImGuiCol_Text,
						   ImVec4{0.8f, 0.8f, 0.8f, _ctx.animation->fade});
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
	auto k_mg_pos{ImVec2{k_mg_c.x * adj_grid_w, k_mg_c.y * adj_grid_h}};
	auto u_mg_pos{ImVec2{u_mg_c.x * adj_grid_w, u_mg_c.y * adj_grid_h}};
	_draw_fg_image_with_idx(
		KNOWN_CREATURES_TEXTURE, k_gfx, k_mg_pos,
		ImVec2{std::stof(k_mg_c.get("tile_width").value()),
			   std::stof(k_mg_c.get("tile_width").value())});
	_draw_fg_image_with_idx(
		UNKNOWN_CREATURES_TEXTURE, u_gfx, u_mg_pos,
		ImVec2{std::stof(u_mg_c.get("tile_width").value()),
			   std::stof(u_mg_c.get("tile_width").value())});

	auto cmp{components->get("bestiary:monster_data")};
	ImVec2 pos{cmp.x * adj_grid_w, cmp.y * adj_grid_h};

	with_Window(WINDOW_LAYER_MENUS, nullptr, ImGuiWindowFlags_NoDecoration) {
		const auto name{std::format("  {:>03}:{}/{}", idx, mon.get_known_name(),
									mon.get_unknown_name())};
		ImGui::SetCursorPos(pos);
		with_Child("bestiary_tab_bar_child",
				   ImVec2(grid_sz * cmp.w, grid_sz * cmp.h)) {
			set_StyleColor(ImGuiCol_Tab,
						   ImVec4{0.0f, 0.0f, 0.0f, _ctx.animation->fade});
			set_Font(fontstore->get_current_font(cmp.font).value());
			ImGuiTabBarFlags tb_flags{ImGuiTabBarFlags_None};
			with_TabBar("bestiary_tab_bar", tb_flags) {
				with_TabItem("Info") {
					{
						set_StyleColor(
							ImGuiCol_Text,
							ImVec4{1.0f, 1.0f, 1.0f, _ctx.animation->fade});
						ImGui::TextUnformatted(name.c_str());
					}

					set_StyleColor(ImGuiCol_Text, ImVec4{0.8f, 0.8f, 0.8f,
														 _ctx.animation->fade});

					const std::string mon_type{
						magic_enum::enum_name(mon.get_category())};
					auto mon_t{CAPITALISE(mon_type)};
					const std::string mon_class{
						mon.get_class() != Enums::Monsters::Class::NO_CLASS
							? magic_enum::enum_name(mon.get_class())
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
						set_StyleColor(
							ImGuiCol_Text,
							ImVec4{1.0f, 1.0f, 1.0f, _ctx.animation->fade});
						ImGui::TextUnformatted(name.c_str());
					}

					set_StyleColor(ImGuiCol_Text, ImVec4{0.8f, 0.8f, 0.8f,
														 _ctx.animation->fade});

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
						set_StyleColor(
							ImGuiCol_Text,
							ImVec4{1.0f, 1.0f, 1.0f, _ctx.animation->fade});
						ImGui::TextUnformatted(name.c_str());
					}

					set_StyleColor(ImGuiCol_Text, ImVec4{0.8f, 0.8f, 0.8f,
														 _ctx.animation->fade});

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
						set_StyleColor(
							ImGuiCol_Text,
							ImVec4{1.0f, 1.0f, 1.0f, _ctx.animation->fade});
						ImGui::TextUnformatted(name.c_str());
					}

					set_StyleColor(ImGuiCol_Text, ImVec4{0.8f, 0.8f, 0.8f,
														 _ctx.animation->fade});

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

auto Sorcery::UI::_display_edge_of_town(Game *game) -> void {
	_draw_components("edge_of_town");
	dialog_leave->display(_ctx.get_flag_ref("want_leave_game"));
	_draw_party_panel(game);
	_draw_debug();
	_draw_cursor();
}

auto Sorcery::UI::_display_castle(Game *game) -> void {
	_draw_components("castle");
	dialog_leave->display(_ctx.get_flag_ref("want_leave_game"));
	_draw_party_panel(game);
	_draw_debug();
	_draw_cursor();
}

auto Sorcery::UI::_display_training_grounds(Game *game) -> void {
	_draw_components("training_grounds", game);
	_draw_cursor();
}

auto Sorcery::UI::_display_reorder(Game *game, const int mode) -> void {
	_draw_components("reorder", game, mode);
	_draw_cursor();
}

auto Sorcery::UI::_display_roster(Game *game, const int mode) -> void {
	_draw_components("roster", game, mode);
	_draw_cursor();
}

auto Sorcery::UI::_display_choose(Game *game, const int mode) -> void {
	_draw_components("choose", game, mode);
	_draw_choose(game, mode);
	_draw_cursor();
}

auto Sorcery::UI::_display_method(Game *game) -> void {

	_draw_components("method", game);
	// input_name->display(_ctx.get_flag_ref("want_name"));
	_draw_cursor();
}

auto Sorcery::UI::_display_inspect(Game *game, const int mode) -> void {

	_draw_components("inspect", game, mode);
	_draw_current_character(game, mode);
	if (modal_identify->show)
		modal_identify->display(_ctx.get_flag_ref("want_identify"));
	if (modal_drop->show)
		modal_drop->display(_ctx.get_flag_ref("want_drop"));
	if (modal_trade->show)
		modal_trade->display(_ctx.get_flag_ref("want_trade"));
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

auto Sorcery::UI::_display_inn(Game *game) -> void {
	_draw_components("inn");
	_draw_party_panel(game);
	modal_inspect->display(_ctx.get_flag_ref("want_inspect"));
	modal_stay->display(_ctx.get_flag_ref("want_stay"));
	modal_identify->display(_ctx.get_flag_ref("want_identify"));
	modal_drop->display(_ctx.get_flag_ref("want_drop"));
	modal_trade->display(_ctx.get_flag_ref("want_trade"));
	modal_use->display(_ctx.get_flag_ref("want_use"));
	modal_invoke->display(_ctx.get_flag_ref("want_invoke"));
	notice_pool_gold->display(_ctx.get_flag_ref("want_pool_gold"));
	_draw_debug();
	_draw_cursor();
}

auto Sorcery::UI::_display_shop(Game *game) -> void {
	_draw_components("shop");
	_draw_party_panel(game);
	_draw_debug();
	_draw_cursor();
}

auto Sorcery::UI::_display_stay(Game *game) -> void {
	_draw_components("stay");
	_draw_stay(game);
	_draw_party_panel(game);
	_draw_debug();
	_draw_cursor();
}

auto Sorcery::UI::_display_recovery(Game *game, const int mode) -> void {
	_draw_components("recovery");
	_draw_recovery(game, mode);
	_draw_party_panel(game);
	_draw_debug();
	_draw_cursor();
}

auto Sorcery::UI::_display_heal(Game *game, int stage) -> void {
	_draw_components("heal", game);
	_draw_heal(game, stage);
	_draw_party_panel(game);
	_draw_debug();
	_draw_cursor();
}

auto Sorcery::UI::_display_no_level_up(Game *game, const int mode) -> void {
	_draw_components("nolevelup");
	_draw_no_level_up(game, mode);
	_draw_party_panel(game);
	_draw_debug();
	_draw_cursor();
}

auto Sorcery::UI::_display_level_up(Game *game, const int mode) -> void {

	_draw_components("levelup");
	_draw_level_up(game, mode);
	_draw_party_panel(game);
	_draw_debug();
	_draw_cursor();
}

auto Sorcery::UI::_display_tavern(Game *game) -> void {

	_draw_components("tavern");
	notice_divvy->display(_ctx.get_flag_ref("want_divvy_gold"));
	notice_pool_gold->display(_ctx.get_flag_ref("want_pool_gold"));
	modal_inspect->display(_ctx.get_flag_ref("want_inspect"));
	modal_identify->display(_ctx.get_flag_ref("want_identify"));
	modal_drop->display(_ctx.get_flag_ref("want_drop"));
	modal_use->display(_ctx.get_flag_ref("want_use"));
	modal_trade->display(_ctx.get_flag_ref("want_trade"));
	modal_invoke->display(_ctx.get_flag_ref("want_invoke"));
	_draw_party_panel(game);
	_draw_debug();
	_draw_cursor();
}

auto Sorcery::UI::_display_pay(Game *game) -> void {

	_draw_components("pay", game);
	_draw_pay_info(game);
	_draw_party_panel(game);
	_draw_debug();
	_draw_cursor();
}

auto Sorcery::UI::_display_temple(Game *game) -> void {

	_draw_components("temple");
	_draw_party_panel(game);
	modal_inspect->display(_ctx.get_flag_ref("want_inspect"));
	modal_help->display(_ctx.get_flag_ref("want_help"));
	modal_tithe->display(_ctx.get_flag_ref("want_tithe"));
	modal_identify->display(_ctx.get_flag_ref("want_identify"));
	modal_drop->display(_ctx.get_flag_ref("want_drop"));
	modal_trade->display(_ctx.get_flag_ref("want_trade"));
	modal_use->display(_ctx.get_flag_ref("want_use"));
	modal_invoke->display(_ctx.get_flag_ref("want_invoke"));
	input_donate->display(_ctx.get_flag_ref("want_donate"));
	notice_donated_ok->display(_ctx.get_flag_ref("want_donated_ok"));
	notice_cannot_donate->display(_ctx.get_flag_ref("want_cannot_donate"));
	notice_not_enough_gold->display(_ctx.get_flag_ref("want_not_enough_gold"));
	notice_pool_gold->display(_ctx.get_flag_ref("want_pool_gold"));
	_draw_debug();
	_draw_cursor();
}

auto Sorcery::UI::_display_remove(Game *game) -> void {
	_draw_components("remove", game);
	_draw_cursor();
}

auto Sorcery::UI::_display_restart(Game *game) -> void {
	_draw_components("restart", game);
	_draw_cursor();
}

auto Sorcery::UI::_display_add(Game *game) -> void {
	_draw_components("add", game);
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
	ImVec2 top_left_pos{map_c.x * adj_grid_w, map_c.y * adj_grid_h};
	const auto spacing{std::stoi(map_c.get("tile_spacing").value())};
	ImVec2 tile_sz{std::stoi(map_c.get("tile_size").value()),
				   std::stoi(map_c.get("tile_size").value())};

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

	const auto width{pb_c.w * grid_sz};
	const float progress{static_cast<float>(images->progress - 1) /
						 static_cast<float>(images->capacity)};
	const auto x{std::invoke([&] {
		if (pb_c.x == -1) {
			const auto viewport{ImGui::GetMainViewport()};
			return (viewport->Size.x - width) / 2;
		} else
			return static_cast<float>(adj_grid_w * pb_c.x);
	})};
	const auto y{static_cast<float>(adj_grid_h * pb_c.y)};

	with_Window(WINDOW_LAYER_IMAGES, nullptr,
				ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoTitleBar) {
		// set_Font(fonts.at(Enums::Layout::Font::DEFAULT));
		set_Font(fontstore->get_default_font());
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
	dialog_leave->display(_ctx.get_flag_ref("want_leave_game"));

	_draw_cursor();

	bool show = true;
	ImGui::SetCurrentFont(fontstore->get_default_font());
	ImGui::ShowDemoWindow(&show);

	ImGui::SetNextWindowPos(ImVec2{1, 1});
	ImGui::SetNextWindowSize(ImVec2{1000, 1000});
}

auto Sorcery::UI::_draw_attract_mode() -> void {

	// Get the Attract Data
	const auto attract{components->get("main_menu:attract_mode")};
	_attract_data = _ctx.animation->get_attract_data();

	// Work out the size and this where to draw it- (as its centred)!
	auto am_size{_attract_data.size() *
				 std::stoi(attract.get("tile_width").value())};
	am_size += (_attract_data.size() - 1) *
			   std::stoi(attract.get("tile_spacing").value());
	const auto viewport{ImGui::GetMainViewport()};
	auto tile_pos{
		ImVec2{(viewport->Size.x - am_size) / 2, attract.y * adj_grid_h}};

	// And draw each tile (this will draw to the correct layer)
	for (auto idx : _attract_data) {

		_draw_fg_image_with_idx(
			KNOWN_CREATURES_TEXTURE, idx, tile_pos,
			ImVec2{std::stof(attract.get("tile_width").value()),
				   std::stof(attract.get("tile_width").value())});
		tile_pos.x += (std::stoi(attract.get("tile_width").value()) +
					   (std::stoi(attract.get("tile_spacing").value())));
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
	ImGui::GetWindowDrawList()->AddRectFilled(p_min, p_max, bg);
	ImGui::GetWindowDrawList()->AddRect(fr_min, fr_max, col, rounding,
										ImDrawFlags_None, rounding);
}

// Used by Modal (Popup) Menus
auto Sorcery::UI::load_dynamic_menu_items(
	Game *game, Controller *controller, std::string_view component,
	[[maybe_unused]] const unsigned int width, std::vector<std::string> &items,
	std::vector<int> &data, const bool reorder) -> void {

	auto load_party_characters = [](Controller *controller, Game *game,
									std::vector<std::string> &items,
									std::vector<int> &data, const int flags,
									const bool reorder) -> void {
		const auto party{game->state->get_party_characters()};
		if (!party.empty()) {
			auto pos{1};
			for (const auto &id : party) {
				const auto &character{game->characters.at(id)};
				if (character.get_location() ==
					Enums::Character::Location::PARTY) {
					if (flags & MENU_SHOW_POSITION)
						items.emplace_back(std::format("{}:{:^19}", pos,
													   character.get_name()));

					else if (flags & MENU_SHOW_GOLD)
						items.emplace_back(std::format("{:<16} {:>8} G.P.",
													   character.get_name(),
													   character.get_gold()));
					else
						items.emplace_back(
							std::format("{:^21}", character.get_name()));
					data.emplace_back(id);
					if (reorder)
						controller->add_to_candidate_party(id);
					++pos;
				}
			}
		}
	};
	auto load_maze_characters =
		[]([[maybe_unused]] Controller *controller, Game *game,
		   std::vector<std::string> &items, std::vector<int> &data,
		   [[maybe_unused]] const int flags,
		   [[maybe_unused]] const bool reorder) -> void {
		if (!game->characters.empty()) {
			for (auto &[id, character] : game->characters) {
				if (character.get_location() ==
						Enums::Character::Location::MAZE &&
					character.get_status() == Enums::Character::Status::OK) {
					items.emplace_back(
						std::format("{:^21}", character.get_name()));
					data.emplace_back(id);
				}
			};
		}
	};
	auto load_tavern_characters =
		[]([[maybe_unused]] Controller *controller, Game *game,
		   std::vector<std::string> &items, std::vector<int> &data,
		   [[maybe_unused]] const int flags,
		   [[maybe_unused]] const bool reorder) -> void {
		if (!game->characters.empty()) {
			for (auto &[id, character] : game->characters) {
				if (character.get_location() ==
					Enums::Character::Location::TAVERN) {
					items.emplace_back(
						std::format("{:^21}", character.get_name()));
					data.emplace_back(id);
				}
			}
		}
	};
	auto load_sick_characters =
		[]([[maybe_unused]] Controller *controller, Game *game,
		   std::vector<std::string> &items, std::vector<int> &data,
		   [[maybe_unused]] const int flags,
		   [[maybe_unused]] const bool reorder) -> void {
		if (!game->characters.empty()) {
			for (auto &[id, character] : game->characters) {
				using enum Enums::Character::Status;
				if (character.get_status() == ASHES ||
					character.get_status() == DEAD ||
					character.get_status() == HELD ||
					character.get_status() == STONED) {
					items.emplace_back(
						std::format("{:<16} {:>8}", character.get_name(),
									character.get_status_string()));
					data.emplace_back(id);
				}
			}
		}
	};
	auto load_character_items =
		[]([[maybe_unused]] Controller *controller, Resources *resources,
		   Game *game, std::vector<std::string> &items, std::vector<int> &data,
		   [[maybe_unused]] const int flags) -> void {
		if (!game->characters.empty()) {
			if (controller->has_character("inspect")) {
				auto character{
					game->characters.at(controller->get_character("inspect"))};
				auto chance{character.abilities().at(
					Enums::Character::Ability::IDENTIFY_ITEMS)};
				auto slot{1u};
				for (const auto &item : character.inventory.items()) {
					const auto flag{std::invoke([&] {
						if (!item.get_known())
							return "?"s;
						else if (item.get_cursed() && item.get_equipped())
							return "-"s;
						else if (item.get_equipped())
							return "*"s;
						else if (!item.get_usable())
							return "#"s;
						else
							return " "s;
					})};
					if (flags & MENU_IDENTIFY_ITEM) {
						if (item.get_known()) {
							auto item_display{
								std::format("{}){}{:<16}", slot, flag,
											item.get_display_name())};
							items.emplace_back(item_display);
						} else {
							auto item_display{
								std::format("{}){}{:<16} {:>5}%", slot, flag,
											item.get_display_name(), chance)};
							items.emplace_back(item_display);
						}
					} else if (flags & MENU_USE_ITEM) {
						if (item.get_known() && item.get_usable()) {
							auto usage{resources->items
										   ->get_item_type(item.get_type_id())
										   .get_usage()};
							if (usage.length() > 0)
								usage = usage.substr(6);
							auto item_display{
								std::format("{}){}{:<16} {:>16}", slot, flag,
											item.get_display_name(), usage)};
							items.emplace_back(item_display);
						} else {
							auto item_display{
								std::format("{}){}{:<16}", slot, flag,
											item.get_display_name())};
							items.emplace_back(item_display);
						}

					} else if (flags & MENU_INVOKE_ITEM) {
						if (item.get_known() && item.get_usable()) {
							auto invokage{
								resources->items
									->get_item_type(item.get_type_id())
									.get_invokage()};
							if (invokage.length() > 0)
								invokage = invokage.substr(6);
							auto item_display{
								std::format("{}){}{:<16} {:>16}", slot, flag,
											item.get_display_name(), invokage)};
							items.emplace_back(item_display);
						} else {
							auto item_display{
								std::format("{}){}{:<16}", slot, flag,
											item.get_display_name())};
							items.emplace_back(item_display);
						}

					} else {

						// Drop Menu handled here
						auto item_display{std::format("{}){}{:<16}", slot, flag,
													  item.get_display_name())};
						items.emplace_back(item_display);
					}

					data.emplace_back(slot);
					++slot;
				}
			}
		}
	};

	// Handle special menu items (including those in Modals)
	items.clear();
	data.clear();
	if (component == "roster_menu" && game != nullptr)
		load_party_characters(controller, game, items, data, NO_FLAGS, reorder);
	else if (component == "choose_menu" && game != nullptr)
		load_party_characters(controller, game, items, data, NO_FLAGS, reorder);
	else if (component == "inspect_menu" && game != nullptr)
		load_party_characters(controller, game, items, data, NO_FLAGS, reorder);
	else if (component == "remove_menu" && game != nullptr)
		load_party_characters(controller, game, items, data, NO_FLAGS, reorder);
	else if (component == "add_menu" && game != nullptr)
		load_tavern_characters(controller, game, items, data, NO_FLAGS,
							   reorder);
	else if (component == "restart_menu" && game != nullptr)
		load_maze_characters(controller, game, items, data, NO_FLAGS, reorder);
	else if (component == "reorder_menu" && game != nullptr)
		load_party_characters(controller, game, items, data, MENU_SHOW_POSITION,
							  reorder);
	else if (component == "modal_help" && game != nullptr)
		load_sick_characters(controller, game, items, data, NO_FLAGS, reorder);

	else if (component == "modal_tithe" && game != nullptr)
		load_party_characters(controller, game, items, data, MENU_SHOW_GOLD,
							  reorder);
	else if (component == "pay_menu" && game != nullptr)
		load_party_characters(controller, game, items, data, MENU_SHOW_GOLD,
							  reorder);
	else if (component == "modal_inspect" && game != nullptr)
		load_party_characters(controller, game, items, data, NO_FLAGS, reorder);
	else if (component == "modal_stay" && game != nullptr)
		load_party_characters(controller, game, items, data, NO_FLAGS, reorder);
	else if (component == "modal_identify" && game != nullptr)
		load_character_items(controller, _ctx.resources, game, items, data,
							 MENU_IDENTIFY_ITEM);
	else if (component == "modal_drop" && game != nullptr)
		load_character_items(controller, _ctx.resources, game, items, data,
							 MENU_DROP_ITEM);
	else if (component == "modal_trade" && game != nullptr)
		load_character_items(controller, _ctx.resources, game, items, data,
							 MENU_TRADE_ITEM);
	else if (component == "modal_use" && game != nullptr)
		load_character_items(controller, _ctx.resources, game, items, data,
							 MENU_USE_ITEM);
	else if (component == "modal_invoke" && game != nullptr)
		load_character_items(controller, _ctx.resources, game, items, data,
							 MENU_INVOKE_ITEM);

	// Note that modal_camp only has fixed items and thus isn't handled here
}

auto Sorcery::UI::load_fixed_items(std::string_view component,
								   const unsigned int width,
								   std::vector<std::string> &items,
								   [[maybe_unused]] const bool reorder)
	-> void {
	std::vector<std::string> sources;
	if (component == "compendium_menu") {
		sources.insert(sources.end(),
					   {"COMPENDIUM_ATLAS", "COMPENDIUM_BESTIARY",
						"COMPENDIUM_GAZETTEER", "COMPENDIUM_MUSEUM",
						"COMPENDIUM_SPELLBOOK", "COMPENDIUM_RETURN"});
		for (const auto &source : sources)
			items.emplace_back(
				std::format("{:^{}}", _ctx.get_string(source), width));

	} else if (component == "camp_menu" || component == "modal_camp") {
		sources.insert(sources.end(),
					   {"CAMP_INSPECT", "CAMP_REORDER", "CAMP_OPTIONS",
						"CAMP_QUIT", "CAMP_LEAVE"});
		for (const auto &source : sources)
			items.emplace_back(
				std::format("{:^{}}", _ctx.get_string(source), width));

	} else if (component == "roster_menu") {
		sources.insert(sources.end(), {"ROSTER_RETURN"});
		for (const auto &source : sources)
			items.emplace_back(
				std::format("{:^{}}", _ctx.get_string(source), width));

	} else if (component == "inspect_menu" || component == "modal_inspect") {
		sources.insert(sources.end(), {"INSPECT_RETURN"});
		for (const auto &source : sources)
			items.emplace_back(
				std::format("{:^{}}", _ctx.get_string(source), width));

	} else if (component == "stay_menu" || component == "modal_stay") {
		sources.insert(sources.end(), {"STAY_RETURN"});
		for (const auto &source : sources)
			items.emplace_back(
				std::format("{:^{}}", _ctx.get_string(source), width));

	} else if (component == "help_menu" || component == "modal_help") {
		sources.insert(sources.end(), {"HELP_RETURN"});
		for (const auto &source : sources)
			items.emplace_back(
				std::format("{:^{}}", _ctx.get_string(source), width));

	} else if (component == "tithe_menu" || component == "modal_tithe") {
		sources.insert(sources.end(), {"TITHE_RETURN"});
		for (const auto &source : sources)
			items.emplace_back(
				std::format("{:^{}}", _ctx.get_string(source), width));

	} else if (component == "pay_menu") {
		sources.insert(sources.end(), {"PAY_RETURN"});
		for (const auto &source : sources)
			items.emplace_back(
				std::format("{:^{}}", _ctx.get_string(source), width));

	} else if (component == "choose_menu") {
		sources.insert(sources.end(), {"CHOOSE_RETURN"});
		for (const auto &source : sources)
			items.emplace_back(
				std::format("{:^{}}", _ctx.get_string(source), width));

	} else if (component == "remove_menu") {
		sources.insert(sources.end(), {"REMOVE_RETURN"});
		for (const auto &source : sources)
			items.emplace_back(
				std::format("{:^{}}", _ctx.get_string(source), width));

	} else if (component == "add_menu") {
		sources.insert(sources.end(), {"ADD_RETURN"});
		for (const auto &source : sources)
			items.emplace_back(
				std::format("{:^{}}", _ctx.get_string(source), width));

	} else if (component == "restart_menu") {
		sources.insert(sources.end(), {"RESTART_RETURN"});
		for (const auto &source : sources)
			items.emplace_back(
				std::format("{:^{}}", _ctx.get_string(source), width));

	} else if (component == "modal_identify") {
		sources.insert(sources.end(), {"IDENTIFY_RETURN"});
		for (const auto &source : sources)
			items.emplace_back(
				std::format("{:^{}}", _ctx.get_string(source), width));

	} else if (component == "modal_drop") {
		sources.insert(sources.end(), {"DROP_RETURN"});
		for (const auto &source : sources)
			items.emplace_back(
				std::format("{:^{}}", _ctx.get_string(source), width));

	} else if (component == "modal_trade") {
		sources.insert(sources.end(), {"TRADE_RETURN"});
		for (const auto &source : sources)
			items.emplace_back(
				std::format("{:^{}}", _ctx.get_string(source), width));

	} else if (component == "modal_use") {
		sources.insert(sources.end(), {"USE_RETURN"});
		for (const auto &source : sources)
			items.emplace_back(
				std::format("{:^{}}", _ctx.get_string(source), width));

	} else if (component == "modal_invoke") {
		sources.insert(sources.end(), {"INVOKE_RETURN"});
		for (const auto &source : sources)
			items.emplace_back(
				std::format("{:^{}}", _ctx.get_string(source), width));

	} else if (component == "reorder_menu") {
		sources.insert(sources.end(), {"REORDER_RETURN"});
		for (const auto &source : sources)
			items.emplace_back(
				std::format("{:^{}}", _ctx.get_string(source), width));

	} else if (component == "castle_menu") {
		sources.insert(sources.end(),
					   {"CASTLE_TAVERN", "CASTLE_INN", "CASTLE_SHOP",
						"CASTLE_TEMPLE", "CASTLE_EDGE_OF_TOWN"});
		for (const auto &source : sources)
			items.emplace_back(
				std::format("{:^{}}", _ctx.get_string(source), width));

	} else if (component == "rest_menu") {
		sources.insert(sources.end(), {"STAY_1", "STAY_2", "STAY_3", "STAY_4",
									   "STAY_5", "STAY_RETURN"});
		for (const auto &source : sources)
			items.emplace_back(
				std::format("{:^{}}", _ctx.get_string(source), width));
	} else if (component == "tavern_menu") {
		sources.insert(sources.end(),
					   {"TAVERN_ADD_TO_PARTY", "TAVERN_REMOVE_FROM_PARTY",
						"TAVERN_REORDER_PARTY", "TAVERN_INSPECT",
						"TAVERN_DIVVY_GOLD", "TAVERN_CASTLE"});
		for (const auto &source : sources)
			items.emplace_back(
				std::format("{:^{}}", _ctx.get_string(source), width));

	} else if (component == "inn_menu") {
		sources.insert(sources.end(),
					   {"INN_STAY", "INN_INSPECT", "INN_CASTLE"});
		for (const auto &source : sources)
			items.emplace_back(
				std::format("{:^{}}", _ctx.get_string(source), width));

	} else if (component == "temple_menu") {
		sources.insert(sources.end(), {"TEMPLE_HELP", "TEMPLE_INSPECT",
									   "TEMPLE_TITHE", "TEMPLE_CASTLE"});
		for (const auto &source : sources)
			items.emplace_back(
				std::format("{:^{}}", _ctx.get_string(source), width));

	} else if (component == "shop_menu") {
		sources.insert(sources.end(),
					   {"SHOP_ENTER", "SHOP_INSPECT", "SHOP_CASTLE"});
		for (const auto &source : sources)
			items.emplace_back(
				std::format("{:^{}}", _ctx.get_string(source), width));

	} else if (component == "edge_menu") {
		sources.insert(sources.end(),
					   {"EDGE_OF_TOWN_TRAIN", "EDGE_OF_TOWN_MAZE",
						"EDGE_OF_TOWN_RESTART", "EDGE_OF_TOWN_CASTLE",
						"EDGE_OF_TOWN_LEAVE_GAME"});
		for (const auto &source : sources)
			items.emplace_back(
				std::format("{:^{}}", _ctx.get_string(source), width));

	} else if (component == "atlas_menu") {
		sources.insert(sources.end(),
					   {"ATLAS_MENU_B1F", "ATLAS_MENU_B2F", "ATLAS_MENU_B3F",
						"ATLAS_MENU_B4F", "ATLAS_MENU_B5F", "ATLAS_MENU_B6F",
						"ATLAS_MENU_B7F", "ATLAS_MENU_B8F", "ATLAS_MENU_B9F",
						"ATLAS_MENU_B10F", "ATLAS_RETURN"});
		for (const auto &source : sources)
			items.emplace_back(
				std::format("{:^{}}", _ctx.get_string(source), width));

	} else if (component == "training_menu") {
		sources.insert(sources.end(),
					   {"TRAINING_GROUNDS_CREATE", "TRAINING_GROUNDS_EDIT",
						"TRAINING_GROUNDS_DELETE", "TRAINING_GROUNDS_INSPECT",
						"TRAINING_GROUNDS_RETURN"});
		for (const auto &source : sources)
			items.emplace_back(
				std::format("{:^{}}", _ctx.get_string(source), width));

	} else if (component == "main_menu") {
		sources.insert(sources.end(),
					   {"MAIN_MENU_OPTION_START", "MAIN_MENU_OPTION_CONTINUE",
						"MAIN_MENU_OPTION_OPTIONS",
						"MAIN_MENU_OPTION_COMPENDIUM",
						"MAIN_MENU_OPTION_LICENSE", "MAIN_MENU_OPTION_EXIT"});
		for (const auto &source : sources)
			items.emplace_back(
				std::format("{:^{}}", _ctx.get_string(source), width));

	} else if (component == "bestiary_menu") {
		for (const auto monster_types{
				 _ctx.resources->monsters->get_all_types()};
			 auto &monster : monster_types) {
			if (monster.get_type_id() > Enums::Monsters::TypeID::WERDNA)
				continue;
			auto mname{monster.get_known_name()};
			auto mid{unenum(monster.get_type_id())};
			auto padded{std::format("{:^{}}", mname, width)};
			auto menu_item{std::format("{}##{}", padded, mid)};
			items.emplace_back(std::format("{}", menu_item));
		}
		items.emplace_back(
			std::format("{:^{}}", _ctx.get_string("BESTIARY_RETURN"), width));

	} else if (component == "spellbook_menu") {
		for (const auto spells{_ctx.resources->spells->get_all()};
			 auto &spell : spells) {
			auto sname{spell.name};
			auto padded{std::format("{:^{}}", sname, width)};
			items.emplace_back(std::format("{}", padded));
		}
		items.emplace_back(
			std::format("{:^{}}", _ctx.get_string("SPELLBOOK_RETURN"), width));
	} else if (component == "museum_menu") {
		for (const auto item_types{_ctx.resources->items->get_all_types()};
			 auto &item_type : item_types) {
			if (item_type.get_type_id() != Enums::Items::TypeID::BROKEN_ITEM) {
				auto iname{item_type.get_known_name()};
				auto iid{unenum(item_type.get_type_id())};
				auto padded{std::format("{:^{}}", iname, width)};
				auto menu_item{std::format("{}##{}", padded, iid)};
				items.emplace_back(std::format("{}", menu_item));
			}
		}
		items.emplace_back(
			std::format("{:^{}}", _ctx.get_string("MUSEUM_RETURN"), width));
	}
}

auto Sorcery::UI::draw_menu(const std::string name, const ImColor sel_color,
							const ImVec2 pos, const ImVec2 sz,
							const Enums::Layout::Font font,
							std::vector<std::string> &items,
							std::vector<int> &data, const bool reorder,
							[[maybe_unused]] const bool across) -> void {
	const std::string display_name{"##" + name};

	// Note that pos is in grid units whereas sz is in pixels!
	const auto x{std::invoke([&] {
		if (pos.x == -1) {
			const auto viewport{ImGui::GetMainViewport()};
			return (viewport->Size.x - sz.x) / 2;
		} else
			return adj_grid_w * pos.x;
	})};
	const auto y{std::invoke([&] {
		if (pos.y == -1) {
			const auto viewport{ImGui::GetMainViewport()};
			return (viewport->Size.y - sz.y) / 2;
		} else
			return adj_grid_h * pos.y;
	})};

	set_StyleColor(ImGuiCol_FrameBg,
				   ImVec4{0.0f, 0.0f, 0.0f, 1.0f - _ctx.animation->fade});
	set_StyleColor(ImGuiCol_HeaderActive, (ImVec4)sel_color);
	set_StyleColor(ImGuiCol_HeaderHovered, (ImVec4)sel_color);
	set_StyleColor(ImGuiCol_Text,
				   ImVec4{1.0f, 1.0f, 1.0f, _ctx.animation->fade});
	set_StyleColor(ImGuiCol_TextDisabled,
				   ImVec4{0.5f, 0.5f, 0.5f, _ctx.animation->fade});

	set_Font(fontstore->get_current_font(font).value());
	ImGui::SetCursorPos(ImVec2{x, y});
	const auto start_pos{ImVec2{x, y}};
	auto cursor_pos{ImVec2{x, y}};

	with_ListBox(display_name.c_str(), sz) {
		for (auto i = 0; i < static_cast<int>(items.size()); i++) {

			bool is_selected{selected[name] == i};
			ImGuiSelectableFlags flags{
				is_selected ? ImGuiSelectableFlags_Highlight : 0};

// Special Handling for Disabled Menus here
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
			std::string output;
			const auto data_item{data.size() > i ? data.at(i) : -1};
#pragma GCC diagnostic pop
			auto disabled{
				_ctx.controller->is_menu_item_disabled(name, i, data_item)};
			if (disabled)
				ImGui::BeginDisabled();

			if (reorder) {

				if (ImGui::Selectable(items[i].c_str(), is_selected, flags)) {
					_ctx.controller->handle_menu(name, items, data_item, i);
				}

				if (ImGui::IsItemHovered()) {
					selected[name] = i;
					highlighted[name] = i;
				}

				if (is_selected)
					ImGui::SetItemDefaultFocus();

				if (disabled)
					ImGui::EndDisabled();

				if (name == "reorder_menu") {

					// Handle Drag-and-drop reordering via ImGUI
					if (data_item != -1) {
						with_DragDropSource(ImGuiDragDropFlags_None) {
							ImGui::SetDragDropPayload("REORDER_CELL", &i,
													  sizeof(int));
						}
						with_DragDropTarget {

							if (const ImGuiPayload *payload =
									ImGui::AcceptDragDropPayload(
										"REORDER_CELL")) {
								int payload_i = *(const int *)payload->Data;
								std::swap(data.at(i), data.at(payload_i));
								std::swap(items.at(i), items.at(payload_i));

								_ctx.controller->clear_candidate_party();
								for (auto char_id : data)
									_ctx.controller->add_to_candidate_party(
										char_id);
								_ctx.controller->set_flag(
									"party_order_changed");
							}
						}
					}
				}
			} else {

				if (ImGui::Selectable(items[i].c_str(), is_selected, flags)) {

					if (name == "main_menu") {
						std::vector<std::reference_wrapper<bool>> out_flags{
							{std::ref(dialog_new->show),
							 std::ref(dialog_exit->show)}};
						_ctx.controller->handle_menu_with_flags(
							name, items, data_item, i, out_flags);
					} else if (name == "edge_menu") {
						std::vector<std::reference_wrapper<bool>> out_flags{
							{std::ref(dialog_leave->show)}};
						_ctx.controller->handle_menu_with_flags(
							name, items, data_item, i, out_flags);
					} else if (name == "tavern_menu") {
						std::vector<std::reference_wrapper<bool>> out_flags{
							{std::ref(notice_divvy->show),
							 std::ref(modal_inspect->show)}};
						_ctx.controller->handle_menu_with_flags(
							name, items, data_item, i, out_flags);
					} else if (name == "inn_menu") {
						std::vector<std::reference_wrapper<bool>> out_flags{
							{std::ref(modal_inspect->show),
							 std::ref(modal_stay->show)}};
						_ctx.controller->handle_menu_with_flags(
							name, items, data_item, i, out_flags);
					} else if (name == "temple_menu") {
						std::vector<std::reference_wrapper<bool>> out_flags{
							{std::ref(modal_inspect->show),
							 std::ref(modal_help->show),
							 std::ref(modal_tithe->show)}};
						_ctx.controller->handle_menu_with_flags(
							name, items, data_item, i, out_flags);
					} else if (name == "camp_menu") {
						std::vector<std::reference_wrapper<bool>> out_flags{
							{std::ref(modal_camp->show)}};
						_ctx.controller->handle_menu_with_flags(
							name, items, data_item, i, out_flags);
					} else if (name == "inspect_menu") {
						std::vector<std::reference_wrapper<bool>> out_flags{
							{std::ref(modal_inspect->show)}};
						_ctx.controller->handle_menu_with_flags(
							name, items, data_item, i, out_flags);
					} else if (name == "stay_menu") {
						std::vector<std::reference_wrapper<bool>> out_flags{
							{std::ref(modal_stay->show)}};
						_ctx.controller->handle_menu_with_flags(
							name, items, data_item, i, out_flags);
					} else if (name == "help_menu") {
						std::vector<std::reference_wrapper<bool>> out_flags{
							{std::ref(modal_help->show)}};
						_ctx.controller->handle_menu_with_flags(
							name, items, data_item, i, out_flags);
					} else if (name == "tithe_menu") {
						std::vector<std::reference_wrapper<bool>> out_flags{
							{std::ref(modal_tithe->show),
							 std::ref(input_donate->show)}};
						_ctx.controller->handle_menu_with_flags(
							name, items, data_item, i, out_flags);
					} else if (name == "identify_menu") {
						std::vector<std::reference_wrapper<bool>> out_flags{
							{std::ref(modal_identify->show)}};
						_ctx.controller->handle_menu_with_flags(
							name, items, data_item, i, out_flags);
					} else if (name == "drop_menu") {
						std::vector<std::reference_wrapper<bool>> out_flags{
							{std::ref(modal_drop->show)}};
						_ctx.controller->handle_menu_with_flags(
							name, items, data_item, i, out_flags);
					} else if (name == "use_menu") {
						std::vector<std::reference_wrapper<bool>> out_flags{
							{std::ref(modal_use->show)}};
						_ctx.controller->handle_menu_with_flags(
							name, items, data_item, i, out_flags);
					} else if (name == "invoke_menu") {
						std::vector<std::reference_wrapper<bool>> out_flags{
							{std::ref(modal_invoke->show)}};
						_ctx.controller->handle_menu_with_flags(
							name, items, data_item, i, out_flags);
					} else

						// Any other menus
						_ctx.controller->handle_menu(name, items, data_item, i);
				}

				if (ImGui::IsItemHovered()) {
					selected[name] = i;
					highlighted[name] = i;
				}

				if (is_selected)
					ImGui::SetItemDefaultFocus();

				if (disabled)
					ImGui::EndDisabled();
			}
		}
	}
}

auto Sorcery::UI::_draw_map_tile(const Tile &tile, const ImVec2 pos,
								 const ImVec2 sz) -> void {
	using enum Enums::DrawMap::Feature;
	using enum Enums::Tile::Features;
	using enum Enums::Tile::Properties;

	// Background Graphic
	_draw_fg_image_with_idx(MAPS_TEXTURE, unenum(FLOOR), pos, sz);

	// Darkness
	if (tile.is(Enums::Tile::Properties::DARKNESS))
		_draw_fg_image_with_idx(MAPS_TEXTURE, unenum(MAP_DARKNESS), pos, sz);

	// Walls for all 4 directions
	using enum Enums::Tile::Edge;
	using enum Enums::Map::Direction;
	if (tile.has(NORTH, SECRET_DOOR) || tile.has(NORTH, ONE_WAY_HIDDEN_DOOR))
		_draw_fg_image_with_idx(MAPS_TEXTURE, unenum(NORTH_SECRET), pos, sz);
	else if (tile.has(NORTH, UNLOCKED_DOOR) || tile.has(NORTH, ONE_WAY_DOOR))
		_draw_fg_image_with_idx(MAPS_TEXTURE, unenum(NORTH_DOOR), pos, sz);
	else if (tile.has(NORTH, ONE_WAY_WALL))
		_draw_fg_image_with_idx(MAPS_TEXTURE, unenum(NORTH_ONE_WAY_WALL), pos,
								sz);
	else if (tile.has(NORTH))
		_draw_fg_image_with_idx(MAPS_TEXTURE, unenum(NORTH_WALL), pos, sz);

	if (tile.has(SOUTH, SECRET_DOOR) || tile.has(SOUTH, ONE_WAY_HIDDEN_DOOR))
		_draw_fg_image_with_idx(MAPS_TEXTURE, unenum(SOUTH_SECRET), pos, sz);
	else if (tile.has(SOUTH, UNLOCKED_DOOR) || tile.has(SOUTH, ONE_WAY_DOOR))
		_draw_fg_image_with_idx(MAPS_TEXTURE, unenum(SOUTH_DOOR), pos, sz);
	else if (tile.has(SOUTH, ONE_WAY_WALL))
		_draw_fg_image_with_idx(MAPS_TEXTURE, unenum(SOUTH_ONE_WAY_WALL), pos,
								sz);
	else if (tile.has(SOUTH))
		_draw_fg_image_with_idx(MAPS_TEXTURE, unenum(SOUTH_WALL), pos, sz);

	if (tile.has(EAST, SECRET_DOOR) || tile.has(EAST, ONE_WAY_HIDDEN_DOOR))
		_draw_fg_image_with_idx(MAPS_TEXTURE, unenum(EAST_SECRET), pos, sz);
	else if (tile.has(EAST, UNLOCKED_DOOR) || tile.has(EAST, ONE_WAY_DOOR))
		_draw_fg_image_with_idx(MAPS_TEXTURE, unenum(EAST_DOOR), pos, sz);
	else if (tile.has(EAST, ONE_WAY_WALL))
		_draw_fg_image_with_idx(MAPS_TEXTURE, unenum(EAST_ONE_WAY_WALL), pos,
								sz);
	else if (tile.has(EAST))
		_draw_fg_image_with_idx(MAPS_TEXTURE, unenum(EAST_WALL), pos, sz);

	if (tile.has(WEST, SECRET_DOOR) || tile.has(WEST, ONE_WAY_HIDDEN_DOOR))
		_draw_fg_image_with_idx(MAPS_TEXTURE, unenum(WEST_SECRET), pos, sz);
	else if (tile.has(WEST, UNLOCKED_DOOR) || tile.has(WEST, ONE_WAY_DOOR))
		_draw_fg_image_with_idx(MAPS_TEXTURE, unenum(WEST_DOOR), pos, sz);
	else if (tile.has(WEST, ONE_WAY_WALL))
		_draw_fg_image_with_idx(MAPS_TEXTURE, unenum(WEST_ONE_WAY_WALL), pos,
								sz);
	else if (tile.has(WEST))
		_draw_fg_image_with_idx(MAPS_TEXTURE, unenum(WEST_WALL), pos, sz);

	// And Tile Contents
	if (tile.has(STAIRS_UP) || tile.has(LADDER_UP))
		_draw_fg_image_with_idx(MAPS_TEXTURE, unenum(MAP_STAIRS_UP), pos, sz);
	else if (tile.has(STAIRS_DOWN) || tile.has(LADDER_DOWN))
		_draw_fg_image_with_idx(MAPS_TEXTURE, unenum(MAP_STAIRS_DOWN), pos, sz);
	else if (tile.has(ELEVATOR))
		_draw_fg_image_with_idx(MAPS_TEXTURE, unenum(MAP_ELEVATOR), pos, sz);
	else if (tile.has(SPINNER))
		_draw_fg_image_with_idx(MAPS_TEXTURE, unenum(MAP_SPINNER), pos, sz);
	else if (tile.has(PIT))
		_draw_fg_image_with_idx(MAPS_TEXTURE, unenum(MAP_PIT), pos, sz);
	else if (tile.has(CHUTE))
		_draw_fg_image_with_idx(MAPS_TEXTURE, unenum(MAP_CHUTE), pos, sz);
	else if (tile.has(TELEPORT_TO))
		_draw_fg_image_with_idx(MAPS_TEXTURE, unenum(MAP_TELEPORT_TO), pos, sz);
	else if (tile.has(TELEPORT_FROM))
		_draw_fg_image_with_idx(MAPS_TEXTURE, unenum(MAP_TELEPORT_FROM), pos,
								sz);
	else if (tile.has(MESSAGE) || tile.has(NOTICE))
		_draw_fg_image_with_idx(MAPS_TEXTURE, unenum(EXCLAMATION), pos, sz);
}
