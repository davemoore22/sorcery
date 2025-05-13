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
#include "resources/componentstore.hpp"
#include "resources/imagestore.hpp"
#include "resources/itemstore.hpp"
#include "resources/levelstore.hpp"
#include "resources/monsterstore.hpp"
#include "resources/spellstore.hpp"
#include "types/component.hpp"
#include "types/enum.hpp"
#include "types/game.hpp"
#include "types/state.hpp"

Sorcery::UI::UI(System *system, Display *display, Resources *resources,
				Controller *controller)
	: _system{system},
	  _display{display},
	  _resources{resources},
	  _controller{controller} {

	// Storage
	components =
		std::make_unique<ComponentStore>((*_system->files)[LAYOUT_FILE]);
	images = std::make_unique<ImageStore>(_system);

	// Custom Components
	dialog_exit = std::make_unique<Dialog>(
		_system, this, (*components)["main_menu:dialog_exit"],
		Enums::Layout::DialogType::CONFIRM);
	dialog_new = std::make_unique<Dialog>(_system, this,
										  (*components)["main_menu:dialog_new"],
										  Enums::Layout::DialogType::CONFIRM);
	dialog_leave = std::make_unique<Dialog>(
		_system, this, (*components)["main_menu:dialog_leave"],
		Enums::Layout::DialogType::CONFIRM);
	notice_divvy = std::make_unique<Dialog>(
		_system, this, (*components)["tavern:notice_divvy"],
		Enums::Layout::DialogType::OK);
	notice_cannot_donate = std::make_unique<Dialog>(
		_system, this, (*components)["global:notice_cannot_donate"],
		Enums::Layout::DialogType::OK);
	notice_donated_ok = std::make_unique<Dialog>(
		_system, this, (*components)["global:notice_donated_ok"],
		Enums::Layout::DialogType::OK);
	notice_not_enough_gold = std::make_unique<Dialog>(
		_system, this, (*components)["global:notice_not_enough_gold"],
		Enums::Layout::DialogType::OK);
	popup_ouch = std::make_unique<Popup>(
		_system, this, (*components)["engine_base_ui:popup_ouch"]);
	modal_camp = std::make_unique<Modal>(
		_system, this, _controller, (*components)["engine_base_ui:modal_camp"]);
	modal_inspect = std::make_unique<Modal>(
		_system, this, _controller, (*components)["global:modal_inspect"]);
	modal_identify = std::make_unique<Modal>(
		_system, this, _controller, (*components)["global:modal_identify"]);
	input_donate = std::make_unique<Input>(
		_system, this, _controller, (*components)["global:input_donate"]);

	dialog_stairs_up = std::make_unique<Dialog>(
		_system, this, (*components)["engine_base_ui:dialog_stairs_up"],
		Enums::Layout::DialogType::CONFIRM);
	dialog_stairs_down = std::make_unique<Dialog>(
		_system, this, (*components)["engine_base_ui:dialog_stairs_down"],
		Enums::Layout::DialogType::CONFIRM);

	message_tile = std::make_unique<Message>(
		_system, this, (*components)["engine_base_ui:message_tile"]);

	// Window and Display Settings
	font_sz = std::stoi((*_system->config).get("Font", "size"));
	grid_sz = std::stoi((*_system->config).get("Grid", "size"));
	columns = std::stoi((*_system->config).get("Grid", "columns"));
	rows = std::stoi((*_system->config).get("Grid", "rows"));
	adj_grid_w = grid_sz;
	adj_grid_h = grid_sz;
	frame_rd = std::stoi((*_system->config).get("Frame", "rounding"));
	ui_rd = std::stoi((*_system->config).get("UI", "rounding"));

	_render = std::make_unique<Render>(_system, _display, this, _controller);
};

Sorcery::UI::~UI() {}

auto Sorcery::UI::set_monochrome(const bool value) -> void {

	_render->set_monochrome(value);
}

// Create a Modal on Demand (used whenever data items on it aren't fixed - for
// example the Party Members); normally otherwise fixed Modals are created at
// the beginning as part of the Form/Module create
auto Sorcery::UI::create_dynamic_modal(Game *game, const std::string name)
	-> void {

	if (name == "modal_inspect") {
		if (modal_inspect.get())
			modal_inspect.reset();
		modal_inspect = std::make_unique<Modal>(
			_system, this, _controller, (*components)["global:modal_inspect"]);
		modal_inspect->regenerate(_controller, game);
	} else if (name == "modal_stay") {
		if (modal_stay.get())
			modal_stay.reset();
		modal_stay = std::make_unique<Modal>(
			_system, this, _controller, (*components)["global:modal_stay"]);
		modal_stay->regenerate(_controller, game);
	} else if (name == "modal_help") {
		if (modal_help.get())
			modal_help.reset();
		modal_help = std::make_unique<Modal>(
			_system, this, _controller, (*components)["global:modal_help"]);
		modal_help->regenerate(_controller, game);
	} else if (name == "modal_tithe") {
		if (modal_tithe.get())
			modal_tithe.reset();
		modal_tithe = std::make_unique<Modal>(
			_system, this, _controller, (*components)["global:modal_tithe"]);
		modal_tithe->regenerate(_controller, game);
	} else if (name == "modal_identify") {
		if (modal_identify.get())
			modal_identify.reset();
		modal_identify = std::make_unique<Modal>(
			_system, this, _controller, (*components)["global:modal_identify"]);
		modal_identify->regenerate(_controller, game);
	}
}

// Load/reload the Fonts but scale them as needed
auto Sorcery::UI::load_fonts() -> void {

	using Enums::Layout::Font;

	fonts.clear();
	auto font_size{std::stof((*_system->config).get("Font", "size"))};
	if (adj_grid_w > font_size)
		font_size = adj_grid_w;
	auto font_name{(*_system->config).get("Font", "name")};
	std::string font_file{""};

	if (font_name == "wiz1to4dos")
		font_file = MONOSPACE_A_FONT_FILE;
	else if (font_name == "wiz5fmtowns")
		font_file = MONOSPACE_C_FONT_FILE;
	else if (font_name == "wiz5dos")
		font_file = MONOSPACE_B_FONT_FILE;
	else
		font_file = MONOSPACE_D_FONT_FILE;

	fonts[Font::MONOSPACE] = _io.Fonts->AddFontFromFileTTF(
		CSTR((*_system->files)[font_file]), font_size);
	fonts[Font::PROPORTIONAL] = _io.Fonts->AddFontFromFileTTF(
		CSTR((*_system->files)[PROPORTIONAL_FONT_FILE]), font_size);
	fonts[Font::TEXT] = _io.Fonts->AddFontFromFileTTF(
		CSTR((*_system->files)[TEXT_FONT_FILE]), font_size);
	fonts[Font::DEFAULT] = _io.Fonts->AddFontDefault();
}

// Not an ideal function, really need to maintain a pointer status map instead
auto Sorcery::UI::_get_popups() const -> std::string {

	auto get_popup_status = [](void *component,
							   std::string type) -> std::string {
		if (component != nullptr) {
			if (type == "modal") {
				auto casted{(Modal *)component};
				auto name{casted->name()};
				return fmt::format("{:>26}: {}\n", name, casted->show);
			} else if (type == "dialog") {
				auto casted{(Dialog *)component};
				auto name{casted->name()};
				return fmt::format("{:>26}: {}\n", name, casted->show);
			} else if (type == "message") {
				auto casted{(Message *)component};
				auto name{casted->name()};
				return fmt::format("{:>26}: {}\n", name, casted->show);
			} else if (type == "popup") {
				auto casted{(Popup *)component};
				auto name{casted->name()};
				return fmt::format("{:>26}: {}\n", name, casted->show);
			} else if (type == "input") {
				auto casted{(Input *)component};
				auto name{casted->name()};
				return fmt::format("{:>26}: {}\n", name, casted->show);
			}
		}
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
	if (message_tile)
		output.append(get_popup_status((void *)message_tile.get(), "message"));
	if (modal_camp)
		output.append(get_popup_status((void *)modal_camp.get(), "modal"));
	if (modal_help)
		output.append(get_popup_status((void *)modal_help.get(), "modal"));
	if (modal_identify)
		output.append(get_popup_status((void *)modal_identify.get(), "modal"));
	if (modal_inspect)
		output.append(get_popup_status((void *)modal_inspect.get(), "modal"));
	if (modal_stay)
		output.append(get_popup_status((void *)modal_stay.get(), "modal"));
	if (modal_tithe)
		output.append(get_popup_status((void *)modal_tithe.get(), "modal"));
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

	return output;
}

auto Sorcery::UI::start() -> void {

	// Initialise ImGUI to use SDL2/OpenGL
	ImGui::CreateContext();
	_io = ImGui::GetIO();
	_io.IniFilename = CSTR((*_system->files)[CONFIG_FILE]);
	ImGui::StyleColorsClassic();
	ImGui_ImplSDL2_InitForOpenGL(_display->get_SDL_window(),
								 _display->get_GL_context());
	ImGui_ImplOpenGL3_Init(_display->get_GLSL_version());

	load_fonts();

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

	ms_selected.fill(false);
	ps_selected.fill(false);

	dialog_exit->show = false;
	dialog_new->show = false;
	dialog_leave->show = false;
	notice_divvy->show = false;
	notice_donated_ok->show = false;
	notice_cannot_donate->show = false;
	notice_not_enough_gold->show = false;
	input_donate->show = false;
	popup_ouch->show = false;
	modal_camp->show = false;
	modal_identify->show = false;
	dialog_stairs_up->show = false;
	dialog_stairs_down->show = false;
	message_tile->show = false;

	_attract_data.clear();

	_setup_draw_frontend();
	_setup_draw_modules();
}

auto Sorcery::UI::io() -> ImGuiIO & {

	return _io;
}

auto Sorcery::UI::stop() -> void {

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
	SDL_GL_DeleteContext(_display->get_GL_context());
	SDL_DestroyWindow(_display->get_SDL_window());
	SDL_Quit();
}

auto Sorcery::UI::display_refresh(std::any first, std::any second) -> void {

	// Refresh what we previously drew
	display(_controller->last, first, second);
}

auto Sorcery::UI::_setup_draw_frontend() -> void {

	_draw_frontend.clear();
	_draw_frontend["atlas"] = std::bind(&UI::_display_atlas, this);
	_draw_frontend["compendium"] = std::bind(&UI::_display_compendium, this);
	_draw_frontend["main_menu"] = std::bind(&UI::_display_main_menu, this);
	_draw_frontend["museum"] = std::bind(&UI::_display_museum, this);
	_draw_frontend["options"] = std::bind(&UI::_display_options, this);
	_draw_frontend["spellbook"] = std::bind(&UI::_display_spellbook, this);
	_draw_frontend["splash"] = std::bind(&UI::_display_splash, this);
}

auto Sorcery::UI::_setup_draw_modules() -> void {

	_draw_modules.clear();
	_draw_modules["add"] =
		std::bind(&UI::_display_add, this, std::placeholders::_1);
	_draw_modules["castle"] =
		std::bind(&UI::_display_castle, this, std::placeholders::_1);
	_draw_modules["edge_of_town"] =
		std::bind(&UI::_display_edge_of_town, this, std::placeholders::_1);
	_draw_modules["inn"] =
		std::bind(&UI::_display_inn, this, std::placeholders::_1);
	_draw_modules["pay"] =
		std::bind(&UI::_display_pay, this, std::placeholders::_1);
	_draw_modules["remove"] =
		std::bind(&UI::_display_remove, this, std::placeholders::_1);
	_draw_modules["restart"] =
		std::bind(&UI::_display_restart, this, std::placeholders::_1);
	_draw_modules["shop"] =
		std::bind(&UI::_display_shop, this, std::placeholders::_1);
	_draw_modules["stay"] =
		std::bind(&UI::_display_stay, this, std::placeholders::_1);
	_draw_modules["tavern"] =
		std::bind(&UI::_display_tavern, this, std::placeholders::_1);
	_draw_modules["temple"] =
		std::bind(&UI::_display_temple, this, std::placeholders::_1);
	_draw_modules["training_grounds"] =
		std::bind(&UI::_display_training_grounds, this, std::placeholders::_1);
}

auto Sorcery::UI::display_engine(Game *game) -> void {

	// Start a new Rendering Frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	_setup_windows();

	// Background
	_draw_components("engine_base_ui");

	dialog_leave->display(_controller->get_flag_ref("want_leave_game"));
	dialog_stairs_up->display(_controller->get_flag_ref("want_take_stairs_up"));
	dialog_stairs_down->display(
		_controller->get_flag_ref("want_take_stairs_down"));
	popup_ouch->display();
	message_tile->display(_controller->get_flag_ref("after_tile_message"));
	if (modal_camp->show)
		modal_camp->display(_controller->get_flag_ref("want_quit_expedition"));
	if (modal_inspect->show)
		modal_inspect->display(_controller->get_flag_ref("want_inspect"));
	if (modal_identify->show)
		modal_identify->display(_controller->get_flag_ref("want_identify"));
	if (_controller->show_ui && _controller->show_party_panel)
		_draw_party_panel(game);
	if (_controller->show_ui) {
		_draw_compass(game);
		_draw_buffbar(game);
		_draw_icons(game);
		_draw_save(game);
	}

	// Dungeon View
	auto component{(*components)["engine_base_ui:wire_frame_view"]};
	_render->draw(game, &component);

	// And Cursor on Top
	_draw_debug();
	_draw_cursor();

	bool show = true;
	ImGui::SetCurrentFont(fonts[Enums::Layout::Font::DEFAULT]);
	ImGui::ShowDemoWindow(&show);

	// And finally clear and render everything
	ImGui::Render();
	glViewport(0, 0, (int)io().DisplaySize.x, (int)io().DisplaySize.y);
	glClearColor(0, 0, 0, 255);
	glClear(GL_COLOR_BUFFER_BIT);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	SDL_GL_SwapWindow(_display->get_SDL_window());
}

auto Sorcery::UI::display(const std::string screen, std::any first,
						  std::any second) -> void {

	// Store what we want to draw
	_controller->last = screen;

	// Start a new Rendering Frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	_setup_windows();

	if (first.type() == typeid(Game *) && second.type() != typeid(int))
		_draw_modules[screen](std::any_cast<Game *>(first));
	else if (first.type() == typeid(Game *) && second.type() == typeid(int)) {
	} else if (first.type() == typeid(std::string)) {
	} else
		_draw_frontend[screen]();

	if (screen == "roster") {
		if (first.type() == typeid(Game *) && second.type() == typeid(int))
			_display_roster(std::any_cast<Game *>(first),
							std::any_cast<int>(second));
	}
	if (screen == "heal") {
		if (first.type() == typeid(Game *) && second.type() == typeid(int))
			_display_heal(std::any_cast<Game *>(first),
						  std::any_cast<int>(second));
	} else if (screen == "choose") {
		if (first.type() == typeid(Game *) && second.type() == typeid(int))
			_display_choose(std::any_cast<Game *>(first),
							std::any_cast<int>(second));
	} else if (screen == "inspect") {
		if (first.type() == typeid(Game *) && second.type() == typeid(int))
			_display_inspect(std::any_cast<Game *>(first),
							 std::any_cast<int>(second));
	} else if (screen == "license") {
		if (first.type() == typeid(std::string))
			_display_license(std::any_cast<std::string>(first));
	} else if (screen == "reorder") {
		if (first.type() == typeid(Game *) && second.type() == typeid(int))
			_display_reorder(std::any_cast<Game *>(first),
							 std::any_cast<int>(second));
	} else if (screen == "recovery") {
		if (first.type() == typeid(Game *) && second.type() == typeid(int))
			_display_recovery(std::any_cast<Game *>(first),
							  std::any_cast<int>(second));
	} else if (screen == "nolevelup") {
		if (first.type() == typeid(Game *) && second.type() == typeid(int))
			_display_no_level_up(std::any_cast<Game *>(first),
								 std::any_cast<int>(second));
	} else if (screen == "levelup") {
		if (first.type() == typeid(Game *) && second.type() == typeid(int))
			_display_level_up(std::any_cast<Game *>(first),
							  std::any_cast<int>(second));
	}

	// And finally clear and render everything
	ImGui::Render();
	glViewport(0, 0, (int)io().DisplaySize.x, (int)io().DisplaySize.y);
	glClearColor(0, 0, 0, 255);
	glClear(GL_COLOR_BUFFER_BIT);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	SDL_GL_SwapWindow(_display->get_SDL_window());
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

	const auto first{ImVec4{0xbf, 0xbf, 0xff, _system->animation->fade}};
	const auto second{ImVec4{0x00, 0x00, 0x3f, _system->animation->fade}};

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
		ImVec4{col_from.x, col_from.y, col_from.z, _system->animation->fade}};
	const auto second{
		ImVec4{col_to.x, col_to.y, col_to.z, _system->animation->fade}};

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
		ImVec4 tint_col{
			ImVec4(tint.x, tint.y, tint.z, _system->animation->fade)};
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

	if ((*component)["source"]) {
		const auto source{(*component)["source"].value()};
		const auto scale{std::stof((*component)["scale"].value())};

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
			ImVec4 tint_col{ImVec4(1.0f, 1.0f, 1.0f, _system->animation->fade)};
			ImGui::Image(src_image.texture,
						 ImVec2{static_cast<float>(resized.w),
								static_cast<float>(resized.h)},
						 ImVec2{0.0f, 0.0f}, ImVec2{1.0f, 1.0f}, tint_col);
		}
	}
}

auto Sorcery::UI::_draw_bg_image(Component *component) -> void {

	if ((*component)["source"]) {

		// Load the image if necessary
		const auto source{(*component)["source"].value()};
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

	_controller->busy = value;
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
		const auto cursor_idx{_controller->busy ? ICON_HOURGLASS : ICON_CURSOR};
		const auto cursor_col{_controller->busy
								  ? lerp_colour(ImVec4{1.0f, 0.0f, 0.0f, 1.0f},
												ImVec4{1.0f, 0.8f, 0.8f, 1.0f},
												_system->animation->lerp)
								  : lerp_colour(ImVec4{0.0f, 1.0f, 0.0f, 1.0f},
												ImVec4{0.8f, 1.0f, 0.8f, 1.0f},
												_system->animation->lerp)};

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
	auto frame{std::make_shared<Frame>(_system, this, component)};
	_frames.emplace_back(std::move(frame));
}

// Draw a Menu
auto Sorcery::UI::_draw_menu(Component *component, Game *game) -> void {

	auto menu{std::make_shared<Menu>(_system, _resources, this,
									 this->_controller, component, game)};
	menu->regenerate();
	menu->draw();
	_menus.emplace_back(std::move(menu));
}

auto Sorcery::UI::_draw_debug() -> void {

	with_Window(WINDOW_LAYER_MENUS, nullptr,
				ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs) {

		set_Font(fonts.at(Enums::Layout::Font::DEFAULT));
		ImGui::SetCursorPos(ImVec2{8, 8});
		set_StyleColor(ImGuiCol_Text, ImVec4{1.0f, 0.0f, 0.0f, 1.0f});
		ImGui::TextUnformatted(_controller->get_flags().c_str());

		ImGui::SetCursorPos(ImVec2{8, 400});
		set_StyleColor(ImGuiCol_Text, ImVec4{1.0f, 0.0f, 0.0f, 1.0f});
		ImGui::TextUnformatted(_get_popups().c_str());

		ImGui::SetCursorPos(ImVec2{8, 700});
		set_StyleColor(ImGuiCol_Text, ImVec4{1.0f, 0.0f, 0.0f, 1.0f});
		ImGui::TextUnformatted(_controller->get_characters().c_str());
	}
}

// Draw a Paragraph (Wrapped Multiline Text)
auto Sorcery::UI::_draw_paragraph(Component *component) -> void {

	with_Window(WINDOW_LAYER_TEXTS, nullptr,
				ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs) {

		set_Font(fonts.at(component->font));
		const auto wrap{std::stof((*component)["width"].value()) * font_sz};
		auto p_min{
			ImVec2{component->x * adj_grid_w, component->y * adj_grid_h}};

		ImGui::SetCursorPos(p_min);
		with_TextWrapPos(p_min.x + wrap) {
			set_StyleColor(ImGuiCol_Text, component->colour);
			ImGui::TextUnformatted(
				(*_system->strings)[component->string_key].c_str());
		}
	}
}

auto Sorcery::UI::draw_text(const std::string string, const ImColor colour,
							const ImVec2 pos, const Enums::Layout::Font font)
	-> void {

	set_Font(fonts.at(font));

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
	set_Font(fonts.at(component->font));
	const auto name{component->name};
	const auto col{get_hl_colour(_system->animation->lerp)};
	auto x{std::invoke([&] {
		if (component->x == -1) {
			const auto viewport{ImGui::GetMainViewport()};
			const auto width{ImGui::CalcTextSize(
				CSTR((*_system->strings)[component->string_key]))};
			return (viewport->Size.x - width.x) / 2;
		} else
			return static_cast<float>(adj_grid_w * component->x);
	})};
	auto y{std::invoke([&] {
		if (component->y == -1) {
			const auto viewport{ImGui::GetMainViewport()};
			const auto height{ImGui::CalcTextSize(
				CSTR((*_system->strings)[component->string_key]))};
			return (viewport->Size.y - height.y) / 2;
		} else
			return static_cast<float>(adj_grid_h * component->y);
	})};

	if ((*component)["adjust_x"])
		x += std::stof((*component)["adjust_x"].value());
	if ((*component)["adjust_y"])
		y += std::stof((*component)["adjust_y"].value());

	set_StyleColor(ImGuiCol_Text,
				   ImVec4{1.0f, 1.0f, 1.0f, _system->animation->fade});
	set_StyleColor(ImGuiCol_Button,
				   ImVec4{0.0f, 0.0f, 0.0f, _system->animation->fade});
	set_StyleColor(ImGuiCol_ButtonHovered, (ImVec4)col);
	ImGui::SetCursorPos(ImVec2{x, y});
	with_ID(name.c_str()) {
		if (ImGui::Button(CSTR((*_system->strings)[component->string_key]))) {

			// Handle buttons being used to switch on AND off the flag
			flag = !reverse;
			_controller->handle_button_click(component->name, this, -1);
		}
	}
}

// Draw a Button
auto Sorcery::UI::_draw_button(Component *component,
							   std::optional<bool *> is_clicked) -> void {

	with_Window(WINDOW_LAYER_MENUS, nullptr, ImGuiWindowFlags_NoDecoration) {

		// Need to push font first before calculating size else it will
		// assume monospace font size!
		set_Font(fonts.at(component->font));
		const auto name{component->name};
		const auto col{get_hl_colour(_system->animation->lerp)};
		auto x{std::invoke([&] {
			if (component->x == -1) {
				const auto viewport{ImGui::GetMainViewport()};
				const auto width{ImGui::CalcTextSize(
					CSTR((*_system->strings)[component->string_key]))};
				return (viewport->Size.x - width.x) / 2;
			} else
				return static_cast<float>(adj_grid_w * component->x);
		})};
		auto y{std::invoke([&] {
			if (component->y == -1) {
				const auto viewport{ImGui::GetMainViewport()};
				const auto height{ImGui::CalcTextSize(
					CSTR((*_system->strings)[component->string_key]))};
				return (viewport->Size.y - height.y) / 2;
			} else
				return static_cast<float>(adj_grid_h * component->y);
		})};
		if ((*component)["adjust_x"])
			x += std::stof((*component)["adjust_x"].value());
		if ((*component)["adjust_y"])
			y += std::stof((*component)["adjust_y"].value());

		set_StyleColor(ImGuiCol_Text,
					   ImVec4{1.0f, 1.0f, 1.0f, _system->animation->fade});
		set_StyleColor(ImGuiCol_Button,
					   ImVec4{0.0f, 0.0f, 0.0f, _system->animation->fade});
		set_StyleColor(ImGuiCol_ButtonHovered, (ImVec4)col);
		set_StyleColor(ImGuiCol_ButtonActive, (ImVec4)col);
		ImGui::SetCursorPos(ImVec2{x, y});
		with_ID(name.c_str()) {
			if (ImGui::Button(
					CSTR((*_system->strings)[component->string_key]))) {
				if (is_clicked)
					*is_clicked.value() = true;

				_controller->handle_button_click(component->name, this, -1);

				if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNone)) {
				}
			}
		}
	}
}

auto Sorcery::UI::_draw_character_detailed(Component *component,
										   [[maybe_unused]] Game *game,
										   Character *character) -> void {

	using Enums::Character::Ability;
	using Enums::Character::Attribute;

	const auto left_col{component->x + 0};
	const auto right_col{component->x + 19};

	set_StyleColor(ImGuiCol_Text,
				   ImVec4{1.0f, 1.0f, 1.0f, _system->animation->fade});
	ImVec2 pos{};
	pos = ImVec2{left_col * adj_grid_w, component->y * adj_grid_h};
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		fmt::format("{:>14} {:>2}", "Strength",
					character->get_cur_attr(Attribute::STRENGTH))
			.c_str());

	set_StyleColor(ImGuiCol_Text,
				   ImVec4{0.5f, 0.5f, 0.5f, _system->animation->fade});
	pos.y += adj_grid_h;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		fmt::format("{:>14} {:+>2}", "Atk Mod",
					character->abilities().at(Ability::ATTACK_MODIFIER))
			.c_str());
	pos.y += adj_grid_h;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		fmt::format("{:>14} {:+>2}", "Hit Prob",
					character->abilities().at(Ability::HIT_PROBABILITY))
			.c_str());
	pos.y += adj_grid_h;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		fmt::format("{:>14} {:+>2}", "Bonus Damg",
					character->abilities().at(Ability::BONUS_DAMAGE))
			.c_str());
	pos.y += adj_grid_h;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		fmt::format("{:>14} {:>2}", "Num Attacks",
					character->abilities().at(Ability::BASE_NUMBER_OF_ATTACKS))
			.c_str());
	pos.y += adj_grid_h;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		fmt::format("{:>14} {:>2}", "Unarmed Damg",
					character->abilities().at(Ability::UNARMED_DAMAGE))
			.c_str());

	set_StyleColor(ImGuiCol_Text,
				   ImVec4{1.0f, 1.0f, 1.0f, _system->animation->fade});
	pos.y += (adj_grid_h * 2);
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		fmt::format("{:>14} {:>2}", "Vitality",
					character->get_cur_attr(Attribute::VITALITY))
			.c_str());

	set_StyleColor(ImGuiCol_Text,
				   ImVec4{0.5f, 0.5f, 0.5f, _system->animation->fade});
	pos.y += adj_grid_h;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		fmt::format("{:>14} {:+>2}", "Vit Bonus",
					character->abilities().at(Ability::VITALITY_BONUS))
			.c_str());
	pos.y += adj_grid_h;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		fmt::format("{:>14} {:+>2}", "Bonus HP",
					character->abilities().at(Ability::BONUS_HIT_POINTS))
			.c_str());
	pos.y += adj_grid_h;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		fmt::format("{:>14} {:>2}%", "Ress / Dead",
					character->abilities().at(Ability::DEAD_RESURRECT))
			.c_str());
	pos.y += adj_grid_h;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		fmt::format("{:>14} {:>2}%", "Ress / Ashes",
					character->abilities().at(Ability::ASHES_RESURRECT))
			.c_str());
	pos.y += adj_grid_h;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		fmt::format("{:>14} {:>2}%", "Ress / Spell",
					character->abilities().at(Ability::DI_KADORTO_RESURRECT))
			.c_str());

	pos = ImVec2{right_col * adj_grid_w, component->y * adj_grid_h};
	ImGui::SetCursorPos(pos);
	set_StyleColor(ImGuiCol_Text,
				   ImVec4{1.0f, 1.0f, 1.0f, _system->animation->fade});
	ImGui::TextUnformatted(fmt::format("{:>14} {:>2}", "I.Q.",
									   character->get_cur_attr(Attribute::IQ))
							   .c_str());

	set_StyleColor(ImGuiCol_Text,
				   ImVec4{0.5f, 0.5f, 0.5f, _system->animation->fade});
	pos.y += adj_grid_h;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		fmt::format("{:>14} {:>2}%", "Spell Learn",
					character->abilities().at(Ability::MAGE_SPELL_LEARN))
			.c_str());
	pos.y += adj_grid_h;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		fmt::format("{:>14} {:>2}%", "ID Items",
					character->abilities().at(Ability::IDENTIFY_ITEMS))
			.c_str());
	pos.y += adj_grid_h;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		fmt::format("{:>14} {:>2}%", "ID Curse",
					character->abilities().at(Ability::IDENTIFY_CURSE))
			.c_str());
	pos.y += adj_grid_h;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		fmt::format("{:>14} {:>2}%", "ID Foes",
					character->abilities().at(Ability::IDENTIFY_FOES))
			.c_str());

	set_StyleColor(ImGuiCol_Text,
				   ImVec4{1.0f, 1.0f, 1.0f, _system->animation->fade});
	pos.y += (adj_grid_h * 2);
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		fmt::format("{:>14} {:>2}", "Agility",
					character->get_cur_attr(Attribute::AGILITY))
			.c_str());

	set_StyleColor(ImGuiCol_Text,
				   ImVec4{0.5f, 0.5f, 0.5f, _system->animation->fade});
	pos.y += adj_grid_h;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		fmt::format("{:>14} {:+>2}", "Int Mod",
					character->abilities().at(Ability::INITIATIVE_MODIFIER))
			.c_str());
	pos.y += adj_grid_h;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		fmt::format("{:>14} {:>2}%", "Crit Hit",
					character->abilities().at(Ability::BASE_CRITICAL_HIT))
			.c_str());
	pos.y += adj_grid_h;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		fmt::format("{:>14} {:>2}%", "ID Trap",
					character->abilities().at(Ability::IDENTIFY_TRAP))
			.c_str());
	pos.y += adj_grid_h;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		fmt::format("{:>14} {:>2}%", "Disarm Trap",
					character->abilities().at(Ability::BASE_DISARM_TRAP))
			.c_str());
	pos.y += adj_grid_h;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		fmt::format("{:>14} {:>2}%", "Avoid Trap",
					100 - character->abilities().at(Ability::ACTIVATE_TRAP))
			.c_str());
	pos.y += adj_grid_h;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		fmt::format("{:>14} {:>2}%", "Avoid Pit",
					character->abilities().at(Ability::BASE_AVOID_PIT))
			.c_str());
}

auto Sorcery::UI::_draw_character_mage_spells(Component *component,
											  [[maybe_unused]] Game *game,
											  Character *character) -> void {

	using Enums::Magic::SpellID;
	using Enums::Magic::SpellType;

	ImVec2 pos{component->x * adj_grid_w, component->y * adj_grid_h};
	ImGui::SetCursorPos(pos);
	with_Table("mage_spells_1", 3, ImGuiTableFlags_NoSavedSettings) {

		set_StyleColor(ImGuiCol_Text,
					   ImVec4{1.0f, 1.0f, 1.0f, _system->animation->fade});
		set_StyleColor(ImGuiCol_TextDisabled,
					   ImVec4{0.5f, 0.5f, 0.5f, _system->animation->fade});

		// Row order since we need to do via rows not columns
		auto spell_order = {0,	4,	6,	1,	5,	7,	2,	-1, -1, 3,	-1,
							-1, -1, -1, -1, 8,	11, 14, 9,	12, 15, 10,
							13, 16, -1, -1, 17, -1, -1, -1, 18, 19, 20};
		auto comp_id{""s};
		auto spell_idx{0};
		for (const auto index : spell_order) {
			comp_id = fmt::format("##spell_{}", spell_idx);
			if (index != -1) {
				ImGui::TableNextColumn();

				auto spell_id{magic_enum::enum_cast<SpellID>(index).value()};
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

	using Enums::Magic::SpellID;
	using Enums::Magic::SpellType;

	ImVec2 pos{component->x * adj_grid_w, component->y * adj_grid_h};
	ImGui::SetCursorPos(pos);
	with_Table("priest_spells_1", 3, ImGuiTableFlags_NoSavedSettings) {

		set_StyleColor(ImGuiCol_Text,
					   ImVec4{1.0f, 1.0f, 1.0f, _system->animation->fade});
		set_StyleColor(ImGuiCol_TextDisabled,
					   ImVec4{0.5f, 0.5f, 0.5f, _system->animation->fade});

		// Row order since we need to do via rows not columns
		auto spell_order = {21, 26, 30, 22, 27, 31, 23, 28, 32, 24, 29,
							33, 25, -1, -1, -1, -1, -1, 34, 38, 44, 35,
							39, 45, 36, 40, 46, 37, 41, 47, -1, 42, -1,
							-1, 43, -1, -1, -1, -1, 48, 49, -1};
		auto comp_id{""s};
		auto spell_idx{0};
		for (const auto index : spell_order) {
			comp_id = fmt::format("##spell_{}", spell_idx);
			if (index != -1) {
				ImGui::TableNextColumn();

				auto spell_id{magic_enum::enum_cast<SpellID>(index).value()};
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
	using Enums::Character::Ability;
	using Enums::Character::Attribute;

	const auto left_col{component->x + 0};
	const auto right_col{component->x + 19};

	set_StyleColor(ImGuiCol_Text,
				   ImVec4{1.0f, 1.0f, 1.0f, _system->animation->fade});
	ImVec2 pos{left_col * adj_grid_w, component->y * adj_grid_h};
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		fmt::format("{:>14} {:>2}", "Piety",
					character->get_cur_attr(Attribute::PIETY))
			.c_str());

	set_StyleColor(ImGuiCol_Text,
				   ImVec4{0.5f, 0.5f, 0.5f, _system->animation->fade});
	pos.y += adj_grid_h;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		fmt::format("{:>14} {:>2}%", "Spell Learn",
					character->abilities().at(Ability::PRIEST_SPELL_LEARN))
			.c_str());
	pos.y += adj_grid_h;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		fmt::format("{:>14} {:>2}%", "Rec Chance",
					character->abilities().at(Ability::LOKTOFELT_SUCCESS))
			.c_str());
	pos.y += adj_grid_h;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		fmt::format("{:>14} {:>2}%", "Base Dispell",
					character->abilities().at(Ability::BASE_DISPELL))
			.c_str());

	set_StyleColor(ImGuiCol_Text,
				   ImVec4{1.0f, 1.0f, 1.0f, _system->animation->fade});
	pos.y += (adj_grid_h * 2);
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(fmt::format("{:>14} {:>2}", "Luck",
									   character->get_cur_attr(Attribute::LUCK))
							   .c_str());
	set_StyleColor(ImGuiCol_Text,
				   ImVec4{0.5f, 0.5f, 0.5f, _system->animation->fade});
	pos.y += adj_grid_h;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		fmt::format("{:>14} {:>2}%", "Res Bonus",
					character->abilities().at(Ability::BASE_RESIST_BONUS))
			.c_str());
	pos.y += adj_grid_h;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(fmt::format("{:>14} {:>2}%", "Wipe Rec",
									   character->abilities().at(
										   Ability::EQUIPMENT_INTACT_ON_WIPE))
							   .c_str());

	pos = ImVec2{right_col * adj_grid_w, component->y * adj_grid_h};
	ImGui::SetCursorPos(pos);
	set_StyleColor(ImGuiCol_Text,
				   ImVec4{1.0f, 1.0f, 1.0f, _system->animation->fade});
	ImGui::TextUnformatted(fmt::format("{:>14}", "Resistances").c_str());

	set_StyleColor(ImGuiCol_Text,
				   ImVec4{0.5f, 0.5f, 0.5f, _system->animation->fade});
	pos.y += adj_grid_h;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		fmt::format(
			"{:>14} {:>2}%", "vs Crit Hit",
			character->abilities().at(Ability::RESISTANCE_VS_CRITICAL_HIT) * 5)
			.c_str());
	pos.y += adj_grid_h;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		fmt::format(
			"{:>14} {:>2}%", "vs Pois / Para",
			character->abilities().at(Ability::RESISTANCE_VS_POISON_PARALYSIS) *
				5)
			.c_str());
	pos.y += adj_grid_h;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		fmt::format("{:>14} {:>2}%", "vs Stoning",
					character->abilities().at(Ability::RESISTANCE_VS_STONING) *
						5)
			.c_str());
	pos.y += adj_grid_h;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		fmt::format(
			"{:>14} {:>2}%", "vs Breath",
			character->abilities().at(Ability::RESISTANCE_VS_BREATH_ATTACKS) *
				5)
			.c_str());
	pos.y += adj_grid_h;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		fmt::format(
			"{:>14} {:>2}%", "vs Gas Trap",
			character->abilities().at(Ability::RESISTANCE_VS_POISON_GAS_TRAP) *
				5)
			.c_str());
	pos.y += adj_grid_h;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		fmt::format(
			"{:>14} {:>2}%", "vs Spell Trap",
			character->abilities().at(Ability::RESISTANCE_VS_MAGE_PRIEST_TRAP) *
				5)
			.c_str());
	pos.y += adj_grid_h;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		fmt::format("{:>14} {:>2}%", "vs Silence",
					character->abilities().at(Ability::RESISTANCE_VS_SILENCE) *
						5)
			.c_str());
	pos.y += adj_grid_h;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		fmt::format("{:>14} {:>2}%", "vs Sleep",
					character->abilities().at(Ability::RESISTANCE_VS_KATINO))
			.c_str());
	pos.y += adj_grid_h;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		fmt::format("{:>14} {:>2}%", "vs Death",
					character->abilities().at(Ability::RESISTANCE_VS_BADI))
			.c_str());
	pos.y += adj_grid_h;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		fmt::format("{:>14} {:>2}%", "vs Statue",
					character->abilities().at(Ability::RESISTANCE_VS_MANIFO))
			.c_str());

	pos.y += adj_grid_h;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		fmt::format("{:>14} {:>2}%", "Rec / Sleep",
					character->abilities().at(Ability::RECOVER_FROM_SLEEP) * 5)
			.c_str());
	pos.y += adj_grid_h;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		fmt::format("{:>14} {:>2}%", "Rec / Fear",
					character->abilities().at(Ability::RECOVER_FROM_FEAR) * 5)
			.c_str());
}

auto Sorcery::UI::_draw_character_summary(Component *component,
										  [[maybe_unused]] Game *game,
										  Character *character) -> void {

	using Enums::Character::Ability;
	using Enums::Character::Attribute;

	const auto left_col{component->x};
	const auto middle_col{component->x + 13};
	const auto right_col{component->x + 31};

	ImVec2 pos{};
	pos = ImVec2{left_col * adj_grid_w, component->y * adj_grid_h};
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		fmt::format("{:>8} {:>2}", "Strength",
					character->get_cur_attr(Attribute::STRENGTH))
			.c_str());
	pos = ImVec2{left_col * adj_grid_w, (component->y + 1) * adj_grid_h};
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(fmt::format("{:>8} {:>2}", "I.Q.",
									   character->get_cur_attr(Attribute::IQ))
							   .c_str());
	pos = ImVec2{left_col * adj_grid_w, (component->y + 2) * adj_grid_h};
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		fmt::format("{:>8} {:>2}", "Piety",
					character->get_cur_attr(Attribute::PIETY))
			.c_str());
	pos = ImVec2{left_col * adj_grid_w, (component->y + 3) * adj_grid_h};
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		fmt::format("{:>8} {:>2}", "Vitality",
					character->get_cur_attr(Attribute::VITALITY))
			.c_str());
	pos = ImVec2{left_col * adj_grid_w, (component->y + 4) * adj_grid_h};
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		fmt::format("{:>8} {:>2}", "Agility",
					character->get_cur_attr(Attribute::AGILITY))
			.c_str());
	pos = ImVec2{left_col * adj_grid_w, (component->y + 5) * adj_grid_h};
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(fmt::format("{:>8} {:>2}", "Luck",
									   character->get_cur_attr(Attribute::LUCK))
							   .c_str());

	pos = ImVec2{left_col * adj_grid_w, (component->y + 7) * adj_grid_h};
	ImGui::SetCursorPos(pos);
	auto mage{fmt::format(
		"{}/{}/{}/{}/{}/{}/{}", character->mage_cur_sp().at(1),
		character->mage_cur_sp().at(2), character->mage_cur_sp().at(3),
		character->mage_cur_sp().at(4), character->mage_cur_sp().at(5),
		character->mage_cur_sp().at(6), character->mage_cur_sp().at(7))};
	ImGui::TextUnformatted(fmt::format("Mage {}", mage).c_str());

	pos = ImVec2{middle_col * adj_grid_w, component->y * adj_grid_h};
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		fmt::format("{:<6} {:>10}", "Gold", character->get_gold()).c_str());
	pos = ImVec2{middle_col * adj_grid_w, (component->y + 1) * adj_grid_h};
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		fmt::format("{:<6} {:>10}", "E.P.", character->get_cur_xp()).c_str());
	pos = ImVec2{middle_col * adj_grid_w, (component->y + 2) * adj_grid_h};
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		fmt::format("{:<6} {:>10}", "Next", character->get_next_xp()).c_str());
	pos = ImVec2{middle_col * adj_grid_w, (component->y + 3) * adj_grid_h};
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		fmt::format("{:<6} {:>10}", "Marks",
					character->abilities().at(Ability::MARKS))
			.c_str());
	pos = ImVec2{middle_col * adj_grid_w, (component->y + 4) * adj_grid_h};
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		fmt::format("{:<4} {}/{}", "H.P.",
					character->abilities().at(Ability::CURRENT_HP),
					character->abilities().at(Ability::MAX_HP))
			.c_str());
	pos = ImVec2{middle_col * adj_grid_w, (component->y + 5) * adj_grid_h};
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		fmt::format("{:<6} {:>18}", "Status", character->get_status_string())
			.c_str());

	auto priest{fmt::format(
		"{}/{}/{}/{}/{}/{}/{}", character->priest_cur_sp().at(1),
		character->priest_cur_sp().at(2), character->priest_cur_sp().at(3),
		character->priest_cur_sp().at(4), character->priest_cur_sp().at(5),
		character->priest_cur_sp().at(6), character->priest_cur_sp().at(7))};
	pos = ImVec2{(component->x + 20) * adj_grid_w,
				 (component->y + 7) * adj_grid_h};
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(fmt::format("Prst {}", priest).c_str());

	pos = ImVec2{right_col * adj_grid_w, component->y * adj_grid_h};
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		fmt::format("Swim{:>3}", character->abilities().at(Ability::SWIM))
			.c_str());
	pos = ImVec2{right_col * adj_grid_w, (component->y + 1) * adj_grid_h};
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		fmt::format(" Age{:>3}", character->abilities().at(Ability::AGE) / 52)
			.c_str());
	pos = ImVec2{right_col * adj_grid_w, (component->y + 2) * adj_grid_h};
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		fmt::format(" RIP{:>3}", character->abilities().at(Ability::DEATHS))
			.c_str());

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
		auto line{fmt::format("{}){}{}", slot, flag, item.get_display_name())};
		ImGui::TextUnformatted(line.c_str());
		++slot;
	}
}

auto Sorcery::UI::_draw_choose(Game *game, const int mode) -> void {

	if (mode & CHOOSE_MODE_STAY) {

		auto cmp{(*components)["choose:choose_stay"]};
		_draw_text(&cmp);
		_draw_party_panel(game);
	}
}

auto Sorcery::UI::_draw_level_up(Game *game, const int mode) -> void {

	auto &character{game->characters.at(_controller->get_character("restart"))};

	if (mode & RECOVERY_BIRTHDAY) {

		const auto birth_text{(*_system->strings)["REST_BIRTHDAY_YOU"]};
		auto cmp{(*components)["levelup:levelup_birthday"]};
		_draw_text(&cmp, birth_text);
		cmp = (*components)["levelup:levelup_results"];
		for (const auto &result : character.level_up_results) {
			_draw_text(&cmp, result);
			cmp.y += adj_grid_h;
		}
	} else {

		auto cmp{(*components)["levelup:levelup_birthday"]};
		for (const auto &result : character.level_up_results) {
			_draw_text(&cmp, result);
			++cmp.y;
		}
	}

	with_Window(WINDOW_LAYER_MENUS, nullptr, ImGuiWindowFlags_NoTitleBar) {
		auto leave{(*components)["levelup:levelup_leave"]};
		_draw_button_click(&leave, _controller->get_flag_ref("show_levelup"),
						   true);
	}
}

auto Sorcery::UI::_draw_pay_info(Game *game) -> void {

	// Work out healing cost
	const auto character{
		game->characters.at(_controller->get_character("help"))};
	const auto cost(character.get_cure_cost());
	const auto cost_text{
		fmt::format("{} {} {}", (*_system->strings)["PAY_COST_PREFIX"], cost,
					(*_system->strings)["PAY_COST_SUFFIX"])};
	auto cmp{(*components)["pay:pay_cost"]};
	_draw_text(&cmp, cost_text);
}

auto Sorcery::UI::_draw_no_level_up(Game *game, const int mode) -> void {

	const auto character{
		game->characters.at(_controller->get_character("stay"))};
	const auto birth_text{(*_system->strings)["REST_BIRTHDAY_YOU"]};
	const auto needed{character.get_next_xp() - character.get_cur_xp()};
	const auto need_text{
		fmt::format("{}{}{}", (*_system->strings)["REST_NEED_1_P"], needed,
					(*_system->strings)["REST_NEED_1_S"])};
	const auto make_text{(*_system->strings)["REST_NEED_2"]};

	if (mode & RECOVERY_BIRTHDAY) {

		auto cmp{(*components)["nolevelup:nolevelup_birthday"]};
		_draw_text(&cmp, birth_text);
		cmp = (*components)["nolevelup:nolevelup_need_1"];
		_draw_text(&cmp, need_text);
		cmp = (*components)["nolevelup:nolevelup_need_2"];
		_draw_text(&cmp, make_text);
	} else {

		auto cmp{(*components)["nolevelup:nolevelup_birthday"]};
		_draw_text(&cmp, need_text);
		cmp = (*components)["nolevelup:nolevelup_need_1"];
		_draw_text(&cmp, make_text);
	}

	with_Window(WINDOW_LAYER_MENUS, nullptr, ImGuiWindowFlags_NoTitleBar) {
		auto leave{(*components)["nolevelup:nolevelup_leave"]};
		_draw_button_click(&leave, _controller->get_flag_ref("show_nolevelup"),
						   true);
	}
}

auto Sorcery::UI::_draw_heal([[maybe_unused]] Game *game, int stage) -> void {

	auto cmp{(*components)["heal:heal_status"]};
	auto text{""s};
	switch (stage) {
	case 4:
		text = (*_system->strings)["TEMPLE_HEAL_1"];
		break;
	case 3:
		text = (*_system->strings)["TEMPLE_HEAL_2"];
		break;
	case 2:
		text = (*_system->strings)["TEMPLE_HEAL_3"];
		break;
	case 1:
		text = (*_system->strings)["TEMPLE_HEAL_4"];
		break;
	case 0:
		text = (*_system->strings)["TEMPLE_HEAL_4"];
		break;
	default:
		break;
	}
	_draw_text(&cmp, text);

	if (_controller->has_flag("heal_finished")) {

		if (_controller->has_text("heal_results")) {
			auto summary{(*components)["heal:heal_results"]};
			auto results{_controller->get_text("heal_results")};
			_draw_text(&summary, results);

			with_Window(WINDOW_LAYER_MENUS, nullptr,
						ImGuiWindowFlags_NoTitleBar) {
				auto leave{(*components)["heal:heal_return"]};
				_draw_button_click(
					&leave, _controller->get_flag_ref("show_heal"), true);
			}
		}
	}
}

auto Sorcery::UI::_draw_recovery(Game *game, const int mode) -> void {

	const auto character{
		game->characters.at(_controller->get_character("stay"))};
	if (mode & RECOVERY_MODE_FREE) {

		auto cmp{(*components)["recovery:recovery_napping"]};
		auto text{fmt::format("{}{}", character.get_name(),
							  (*_system->strings)["RECOVERY_NAPPING"])};
		_draw_text(&cmp, text);

	} else {

		auto cmp{(*components)["recovery:recovery_recuperating"]};
		auto text{fmt::format("{} {}", character.get_name(),
							  (*_system->strings)["REST_RECUPERATING"])};
		_draw_text(&cmp, text);

		cmp = (*components)["recovery:recovery_recuperating_hp"];
		text = fmt::format("{} ({:>5}/{:>5})", (*_system->strings)["REST_HP"],
						   character.get_current_hp(), character.get_max_hp());
		_draw_text(&cmp, text);

		cmp = (*components)["recovery:recovery_recuperating_gold"];
		text = fmt::format("{} {:>7}", (*_system->strings)["REST_GOLD"],
						   character.get_gold());
		_draw_text(&cmp, text);

		with_Window(WINDOW_LAYER_MENUS, nullptr, ImGuiWindowFlags_NoTitleBar) {
			auto stop{(*components)["recovery:recovery_stop"]};
			_draw_button_click(
				&stop, _controller->get_flag_ref("show_recovery"), true);
		}
	}
}

auto Sorcery::UI::_draw_stay(Game *game) -> void {

	const auto character{
		game->characters.at(_controller->get_character("stay"))};

	auto cmp_welcome{(*components)["stay:stay_welcome"]};
	auto welcome_text{fmt::format(
		"{}{}{}", (*_system->strings)["STAY_WELCOME_P"], character.get_name(),
		(*_system->strings)["STAY_WELCOME_S"])};
	_draw_text(&cmp_welcome, welcome_text);

	auto cmp_gold{(*components)["stay:stay_gold"]};
	auto gold_text{fmt::format("{}{}{}", (*_system->strings)["STAY_GOLD_P"],
							   character.get_gold(),
							   (*_system->strings)["STAY_GOLD_S"])};
	_draw_text(&cmp_gold, gold_text);
}

auto Sorcery::UI::_draw_current_character(Game *game,
										  [[maybe_unused]] const int mode)
	-> void {

	auto character{game->characters.at(_controller->get_character("inspect"))};

	auto title{(*components)["inspect:character_title"]};
	_draw_text(&title, character.summary_text());

	with_Window(WINDOW_LAYER_MENUS, nullptr, ImGuiWindowFlags_NoTitleBar) {
		auto prev{(*components)["inspect:character_previous"]};
		_draw_button_click(
			&prev, _controller->get_flag_ref("select_previous_character"));
		auto next{(*components)["inspect:character_next"]};
		_draw_button_click(&next,
						   _controller->get_flag_ref("select_next_character"));

		auto cmp{(*components)["inspect:character_data"]};
		ImVec2 pos{cmp.x * adj_grid_w, cmp.y * adj_grid_h};

		ImGuiTabBarFlags tb_flags{ImGuiTabBarFlags_None};
		ImGui::SetCursorPos(pos);
		with_Child("character_tab_bar_child",
				   ImVec2(grid_sz * cmp.w, grid_sz * cmp.h)) {
			set_StyleColor(ImGuiCol_Tab,
						   ImVec4{0.0f, 0.0f, 0.0f, _system->animation->fade});
			auto char_cmp{(*components)["inspect:character_tab_data"]};
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

auto Sorcery::UI::_draw_text(Component *component, const std::string &string)
	-> void {
	with_Window(WINDOW_LAYER_TEXTS, nullptr,
				ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs) {

		// Need to push font first before calculating size else it will
		// assume monospace font size!
		set_Font(fonts.at(component->font));

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
		alpha_col.w = _system->animation->fade;

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
		set_Font(fonts.at(component->font));

		const auto x{std::invoke([&] {
			if (component->x == -1) {
				const auto viewport{ImGui::GetMainViewport()};
				const auto width{ImGui::CalcTextSize(
					CSTR((*_system->strings)[component->string_key]))};
				return (viewport->Size.x - width.x) / 2;
			} else
				return static_cast<float>(adj_grid_w * component->x);
		})};
		const auto y{std::invoke([&] {
			if (component->y == -1) {
				const auto viewport{ImGui::GetMainViewport()};
				const auto height{ImGui::CalcTextSize(
					CSTR((*_system->strings)[component->string_key]))};
				return (viewport->Size.y - height.y) / 2;
			} else
				return static_cast<float>(adj_grid_h * component->y);
		})};

		// Adjust Alpha of Text
		ImVec4 alpha_col{ImGui::ColorConvertU32ToFloat4(component->colour)};
		alpha_col.w = _system->animation->fade;

		set_StyleColor(ImGuiCol_Text, alpha_col);
		ImGui::SetCursorPos(ImVec2{x, y});
		ImGui::TextUnformatted(
			(*_system->strings)[component->string_key].c_str());
	}
}

auto Sorcery::UI::_draw_components(std::string_view screen, Game *game,
								   [[maybe_unused]] const int mode) -> void {
	using Enums::Layout::ComponentType;

	_frames.clear();
	_menus.clear();

	// Draw every component specified in order
	auto cmps{(*components)(screen)};

	for (auto c : cmps.value())
		if (c.type == ComponentType::IMAGE_BG)
			_draw_bg_image(&c);
	for (auto c : cmps.value())
		if (c.type == ComponentType::FRAME)
			_draw_frame(&c);
	for (auto c : cmps.value())
		if (c.type == ComponentType::IMAGE_FG)
			_draw_fg_image(&c);
	for (auto c : cmps.value())
		if (c.type == ComponentType::TEXT)
			_draw_text(&c);
	for (auto c : cmps.value())
		if (c.type == ComponentType::BUTTON)
			_draw_button(&c);
	for (auto c : cmps.value())
		if (c.type == ComponentType::PARAGRAPH)
			_draw_paragraph(&c);
	for (auto c : cmps.value())
		if (c.type == ComponentType::MENU)
			_draw_menu(&c, game);
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
	const auto idx{_controller->selected["museum_selected"]};
	if (idx >= 100)
		return;

	const auto item{(*_resources->items)[idx + 1]};
	auto item_c{(*components)["museum:item_graphic"]};
	auto item_pos{ImVec2{item_c.x * adj_grid_w, item_c.y * adj_grid_h}};
	_draw_fg_image_with_idx(ITEMS_TEXTURE, idx, item_pos,
							ImVec2{std::stof(item_c["tile_width"].value()),
								   std::stof(item_c["tile_width"].value())});

	auto cmp{(*components)["museum:item_data"]};
	ImVec2 pos{cmp.x * adj_grid_w, cmp.y * adj_grid_h};

	with_Window(WINDOW_LAYER_MENUS, nullptr, ImGuiWindowFlags_NoDecoration) {

		const auto name{fmt::format("  {:>03}:{}/{}", idx + 1,
									item.get_known_name(),
									item.get_unknown_name())};

		ImGuiTabBarFlags tb_flags{ImGuiTabBarFlags_None};
		ImGui::SetCursorPos(pos);
		with_Child("museum_tab_bar_child",
				   ImVec2(grid_sz * cmp.w, grid_sz * cmp.h)) {
			set_StyleColor(ImGuiCol_Tab,
						   ImVec4{0.0f, 0.0f, 0.0f, _system->animation->fade});
			with_TabBar("museum_tab_bar", tb_flags) {
				with_TabItem("Info") {
					{
						set_StyleColor(
							ImGuiCol_Text,
							ImVec4{1.0f, 1.0f, 1.0f, _system->animation->fade});
						ImGui::TextUnformatted(name.c_str());
					}

					set_StyleColor(
						ImGuiCol_Text,
						ImVec4{0.8f, 0.8f, 0.8f, _system->animation->fade});
					std::string item_category{
						magic_enum::enum_name(item.get_category())};
					if (item_category == "SPECIAL_ITEM")
						item_category = "Quest Item";

					auto it_cat{CAPITALISE(item_category)};
					const std::string it_cursed{
						item.get_cursed() == true ? "Yes" : "No"};
					const auto cat{fmt::format(" Type:{}", it_cat)};
					const auto cost{fmt::format(" Cost:{}", item.get_value())};
					const auto curse{fmt::format("Curse:{}", it_cursed)};
					const auto allow{
						fmt::format("Allow:{}", item.get_equippable_display())};
					ImGui::TextUnformatted(cat.c_str());
					ImGui::TextUnformatted(allow.c_str());
					ImGui::TextUnformatted(cost.c_str());
					ImGui::TextUnformatted(curse.c_str());
				}
				with_TabItem("Stats") {
					{
						set_StyleColor(
							ImGuiCol_Text,
							ImVec4{1.0f, 1.0f, 1.0f, _system->animation->fade});
						ImGui::TextUnformatted(name.c_str());
					}

					set_StyleColor(
						ImGuiCol_Text,
						ImVec4{0.8f, 0.8f, 0.8f, _system->animation->fade});
					const auto ac{fmt::format(
						"   AC:{}", std::to_string(item.get_ac_mod()))};
					auto value{fmt::format(fmt::runtime("{:+d}"),
										   item.get_to_hit_mod())};
					auto hit{fmt::format("  Hit:{:}", value)};
					const auto damage{
						fmt::format("  Dam:{}", item.get_damage())};
					const auto atks{fmt::format(
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
							ImVec4{1.0f, 1.0f, 1.0f, _system->animation->fade});
						ImGui::TextUnformatted(name.c_str());
					}

					set_StyleColor(
						ImGuiCol_Text,
						ImVec4{0.8f, 0.8f, 0.8f, _system->animation->fade});
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
							ImVec4{1.0f, 1.0f, 1.0f, _system->animation->fade});
						ImGui::TextUnformatted(name.c_str());
					}
					set_StyleColor(
						ImGuiCol_Text,
						ImVec4{0.8f, 0.8f, 0.8f, _system->animation->fade});

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
							 std::stof((*component)["grid_width"].value())};
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
						   ImVec4{0.8f, 0.8f, 0.8f, _system->animation->fade});
			set_Font(fonts.at(component->font));
			with_TextWrapPos(ImGui::GetFontSize() * component->w) {
				ImGui::TextUnformatted(string.c_str());
			}
		}

		// Special Handling for Return Button
		Component cmp{(*components)["license:license_return"]};
		auto show{true};
		_draw_button_click(&cmp, show);
		if (!show)
			_controller->unset_flag("show_license");
	}
}

auto Sorcery::UI::load_message(const Enums::Map::Event event)
	-> std::vector<std::string> {
	using Enums::Map::Event;

	switch (event) {
	case Event::AREA_OF_OUT_BOUNDS:
		return {"GAME_MESSAGE_AREA_OUT_OF_BOUNDS_1",
				"GAME_MESSAGE_AREA_OUT_OF_BOUNDS_2"};
		break;
	case Event::MAN_TELEPORT_CASTLE:
		return {"GAME_MESSAGE_MAN_TELEPORT_CASTLE_1",
				"GAME_MESSAGE_MAN_TELEPORT_CASTLE_2",
				"GAME_MESSAGE_MAN_TELEPORT_CASTLE_3",
				"GAME_MESSAGE_MAN_TELEPORT_CASTLE_4",
				"GAME_MESSAGE_MAN_TELEPORT_CASTLE_5",
				"GAME_MESSAGE_MAN_TELEPORT_CASTLE_6"};
		break;
	case Event::SILVER_KEY:
		return {"GAME_MESSAGE_SILVER_KEY_1", "GAME_MESSAGE_SILVER_KEY_2",
				"GAME_MESSAGE_SILVER_KEY_3", "GAME_MESSAGE_SILVER_KEY_4",
				"GAME_MESSAGE_SILVER_KEY_5", "GAME_MESSAGE_SILVER_KEY_6",
				"GAME_MESSAGE_SILVER_KEY_7", "GAME_MESSAGE_SILVER_KEY_8"};
		break;
	case Event::BRONZE_KEY:
		return {"GAME_MESSAGE_BRONZE_KEY_1", "GAME_MESSAGE_BRONZE_KEY_2",
				"GAME_MESSAGE_BRONZE_KEY_3", "GAME_MESSAGE_BRONZE_KEY_4",
				"GAME_MESSAGE_BRONZE_KEY_5", "GAME_MESSAGE_BRONZE_KEY_6"};
		break;
	case Event::MURPHYS_GHOSTS:
		return {
			"GAME_MESSAGE_MURPHYS_GHOSTS_1", "GAME_MESSAGE_MURPHYS_GHOSTS_2",
			"GAME_MESSAGE_MURPHYS_GHOSTS_3", "GAME_MESSAGE_MURPHYS_GHOSTS_4",
			"GAME_MESSAGE_MURPHYS_GHOSTS_5"};
		break;
	case Event::BEAR_STATUE:
		return {"GAME_MESSAGE_BEAR_STATUE_1", "GAME_MESSAGE_BEAR_STATUE_2",
				"GAME_MESSAGE_BEAR_STATUE_3"};
		break;
	case Event::FROG_STATUE:
		return {"GAME_MESSAGE_FROG_STATUE_1", "GAME_MESSAGE_FROG_STATUE_2",
				"GAME_MESSAGE_FROG_STATUE_3", "GAME_MESSAGE_FROG_STATUE_4"};
		break;
	case Event::GOLD_KEY:
		return {"GAME_MESSAGE_GOLD_KEY_1", "GAME_MESSAGE_GOLD_KEY_2",
				"GAME_MESSAGE_GOLD_KEY_3", "GAME_MESSAGE_GOLD_KEY_4",
				"GAME_MESSAGE_GOLD_KEY_5", "GAME_MESSAGE_GOLD_KEY_6"};
		break;
	case Event::NEED_SILVER_KEY:
		return {};
		break;
	case Event::NEED_BRONZE_KEY:
		return {};
		break;
	case Event::NEED_BEAR_STATUE:
		return {};
		break;
	case Event::NEED_FROG_STATUE:
		return {};
		break;
	case Event::PLACARD_PIT_1:
		return {};
		break;
	case Event::PLACARD_PIT_2:
		return {};
		break;
	case Event::PLACARD_PIT_3:
		return {};
		break;
	case Event::TURN_AROUND:
		return {};
		break;
	case Event::TURN_LEFT:
		return {};
		break;
	case Event::TURN_RIGHT:
		return {};
		break;
	case Event::NEED_BEAR_STATUE_2:
		return {};
		break;
	case Event::TESTING_GROUNDS:
		return {};
		break;
	case Event::ALARM_BELLS:
		return {};
		break;
	case Event::TREASURE_REPOSITORY:
		return {};
		break;
	case Event::MONSTER_ALLOCATION_CENTRE:
		return {};
		break;
	case Event::LARGE_DESK:
		return {};
		break;
	case Event::TREBOR_VOICE:
		return {};
		break;
	case Event::SERVICE_ELEVATOR:
		return {};
		break;
	case Event::WERDNA_BOAST:
		return {};
		break;
	case Event::TURN_BACK:
		return {};
		break;
	case Event::WERDNA_SIGN:
		return {};
		break;
	case Event::THREE_HUMANOIDS:
		return {};
		break;
	case Event::GETTING_WARM_1:
		return {};
		break;
	case Event::GETTING_WARM_2:
		return {};
		break;
	case Event::GETTING_WARM_3:
		return {};
		break;
	case Event::FIRE_DRAGONS_COMBAT:
		return {};
		break;
	case Event::DEADLY_RING_COMBAT:
		return {};
		break;
	case Event::WERDNA_COMBAT:
		return {};
		break;
	case Event::GUARANTEED_COMBAT:
		return {};
		break;
	case Event::NEED_BLUE_RIBBON:
		return {};
		break;
	default:
		return {};
	};
}

auto Sorcery::UI::_draw_options() -> void {
	const auto component{(*components)["options:options_info"]};

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

	std::vector<std::string> graphics_opts{"OPT_COLOURED_WIREFRAME"};

	const auto save_lbl{(*_system->strings)["DIALOG_SAVE"]};
	const auto cancel_lbl{(*_system->strings)["DIALOG_CANCEL"]};
	set_Font(fonts.at(component.font));
	const auto col{get_hl_colour(_system->animation->lerp)};
	with_Window(WINDOW_LAYER_MENUS, nullptr, ImGuiWindowFlags_NoDecoration) {

		// To adjust for Window Resizing etc
		const auto x{std::invoke([&] {
			const auto width{grid_sz *
							 std::stof((component)["grid_width"].value())};
			const auto viewport{ImGui::GetMainViewport()};
			return (viewport->Size.x - width) / 2;
		})};

		const auto pos{ImVec2{x, component.y * adj_grid_h}};
		ImGui::SetCursorPos(pos);

		// Now draw tab bar
		set_StyleColor(ImGuiCol_Text,
					   ImVec4{1.0f, 1.0f, 1.0f, _system->animation->fade});
		set_StyleColor(ImGuiCol_Button,
					   ImVec4{0.0f, 0.0f, 0.0f, _system->animation->fade});
		set_StyleColor(ImGuiCol_ButtonHovered, (ImVec4)col);
		const auto tabs_width{component.w * grid_sz};
		const auto tabs_height{component.h * grid_sz};
		ImGuiTabBarFlags tb_flags{ImGuiTabBarFlags_None};
		with_Child("options_tab_bar_child", ImVec2(tabs_width, tabs_height)) {
			set_StyleColor(ImGuiCol_Tab, ImVec4{0.0f, 0.0f, 0.0f, 0.0f});

			auto summary_idx{0u};
			auto gameplay_idx(5u);
			auto graphics_idx(18u);

			set_StyleColor(ImGuiCol_Button, ImVec4{0.16f, 0.66f, 0.45f,
												   _system->animation->fade});
			set_StyleColor(ImGuiCol_ButtonHovered,
						   ImVec4{0.0f, 1.0f, 0.57f, _system->animation->fade});
			set_StyleColor(ImGuiCol_Text,
						   ImVec4{1.0f, 1.0f, 1.0f, _system->animation->fade});

			// now draw tabs
			with_TabBar("options_tab_bar", tb_flags) {
				auto tabname{"Options"};
				with_TabItem(tabname) {
					for (const auto &opt : summary_opts) {
						if (opt.length() > 0) {
							const auto text{
								fmt::format(" {}", (*_system->strings)[opt])};
							if (ImGui::Toggle(
									text.c_str(),
									&(*_system->config)[summary_idx])) {

								// Do additonal handling such as switching
								// on strict mode etc inside the controller
								_controller->handle_toggle(
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
								fmt::format(" {}", (*_system->strings)[opt])};
							if (ImGui::Toggle(
									text.c_str(),
									&(*_system->config)[gameplay_idx])) {

								// Do additonal handling such as switching
								// on strict mode etc
								_controller->handle_toggle(
									component.name, tabname, gameplay_idx);
							};
						}
						++gameplay_idx;
					}
				}
				tabname = "Graphics";
				with_TabItem(tabname) {

					for (const auto &opt : graphics_opts) {
						if (opt.length() > 0) {
							const auto text{
								fmt::format(" {}", (*_system->strings)[opt])};
							if (ImGui::Toggle(
									text.c_str(),
									&(*_system->config)[graphics_idx])) {

								// Do additonal handling such as switching
								// on strict mode etc
								_controller->handle_toggle(
									component.name, tabname, graphics_idx);
							};
						}
						++graphics_idx;
					}
				}

				// Save and Cancel Buttons
				const auto centre{(tabs_width / 2)};
				const auto button_y{std::stoi(component["button_y"].value())};
				ImVec2 btn_size{ImGui::GetFontSize() * 7.0f, 0.0f};

				set_StyleColor(ImGuiCol_Text, ImVec4{1.0f, 1.0f, 1.0f,
													 _system->animation->fade});
				set_StyleColor(
					ImGuiCol_Button,
					ImVec4{0.0f, 0.0f, 0.0f, _system->animation->fade});
				set_StyleColor(ImGuiCol_ButtonHovered, ImVec4{col});
				set_StyleColor(ImGuiCol_ButtonActive, (ImVec4)col);

				ImGui::SetCursorPos(ImVec2{centre - (btn_size.x + grid_sz),
										   button_y * grid_sz});
				if (ImGui::Button(save_lbl.c_str(), btn_size)) {
					_system->config->save();
					_controller->unset_flag("show_options");
				}
				ImGui::SetCursorPos(
					ImVec2{centre + grid_sz, button_y * grid_sz});
				if (ImGui::Button(cancel_lbl.c_str(), btn_size)) {
					_system->config->load();
					_controller->unset_flag("show_options");
				}
			}
		}
	}
}

auto Sorcery::UI::_draw_buffbar(Game *game) -> void {
	auto cmp{(*components)["engine_base_ui:buffbar"]};
	auto frame_cmp{(*components)["engine_base_ui:buffbar_frame"]};

	const auto x{static_cast<float>(adj_grid_w * cmp.x)};
	auto y{static_cast<float>(adj_grid_h * cmp.y)};
	const auto width{cmp.w * grid_sz};
	const auto height{cmp.h * grid_sz};

	auto tint{_controller->monochrome
				  ? ImVec4{1.0f, 1.0f, 1.0f, _system->animation->fade}
				  : ImVec4{1.0f, 0.33f, 0.33f, _system->animation->fade}};

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
	auto cmp{(*components)["engine_base_ui:icons"]};
	auto frame_cmp{(*components)["engine_base_ui:icons_frame"]};

	const auto icons = {ICON_CAMP, ICON_PARTY, ICON_MAP,
						ICON_LOOK, ICON_CAST,  ICON_USE};

	const auto x{static_cast<float>(adj_grid_w * cmp.x)};
	auto y{static_cast<float>(adj_grid_h * cmp.y)};
	const auto width{cmp.w * grid_sz};
	const auto height{cmp.h * grid_sz};

	auto tint{_controller->monochrome
				  ? ImVec4{1.0f, 1.0f, 1.0f, _system->animation->fade}
				  : ImVec4{0.33f, 1.0f, 1.0f, _system->animation->fade}};

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
	using Enums::Map::Direction;

	auto cmp{(*components)["engine_base_ui:save"]};
	auto frame_cmp{(*components)["engine_base_ui:save_frame"]};

	const auto x{static_cast<float>(adj_grid_w * cmp.x)};
	const auto y{static_cast<float>(adj_grid_h * cmp.y)};
	const auto width{cmp.w * grid_sz};
	const auto height{cmp.h * grid_sz};

	auto tint{_controller->monochrome
				  ? ImVec4{1.0f, 1.0f, 1.0f, _system->animation->fade}
				  : ImVec4{0.33f, 1.0f, 1.0f, _system->animation->fade}};

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
	using Enums::Map::Direction;

	auto cmp{(*components)["engine_base_ui:compass"]};
	auto frame_cmp{(*components)["engine_base_ui:compass_frame"]};

	auto tint{_controller->monochrome
				  ? ImVec4{1.0f, 1.0f, 1.0f, _system->animation->fade}
				  : ImVec4{1.0f, 0.33f, 0.33f, _system->animation->fade}};

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
			case Direction::NORTH:
				icon_idx = ICON_COMPASS_NORTH;
				break;
			case Direction::SOUTH:
				icon_idx = ICON_COMPASS_SOUTH;
				break;
			case Direction::EAST:
				icon_idx = ICON_COMPASS_EAST;
				break;
			case Direction::WEST:
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
	auto cmp{(*components)["global:party_panel"]};
	auto frame_cmp{(*components)["engine_base_ui:party_frame"]};

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
			set_Font(fonts.at(cmp.font));
			const auto hl_col{get_hl_colour(_system->animation->lerp)};
			set_StyleColor(ImGuiCol_Text,
						   ImVec4{1.0f, 1.0f, 1.0f, _system->animation->fade});
			ImGui::TextUnformatted(
				(*_system->strings)["PARTY_PANEL_LEGEND"].c_str());
			if (game->state->get_party_size() > 0) {
				auto position{1u};
				const auto party{game->state->get_party_characters()};
				for (auto char_id : party) {
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
	using Enums::Character::CStatus;

	switch (character->get_status()) {
	case CStatus::OK:
		if (character->get_poisoned_rate() > 0)
			return ImVec4{0.0f, 1.0f, 0.0f, _system->animation->fade};
		else if (character->get_max_hp() / character->get_current_hp() > 10)
			return ImVec4{1.0f, 0.0f, 0.0f, _system->animation->fade};
		else if (character->get_max_hp() / character->get_current_hp() > 5)
			return ImVec4{1.0f, 1.0f, 0.0f, _system->animation->fade};
		else
			return ImVec4{1.0f, 1.0f, 1.0f, _system->animation->fade};
		break;
	case CStatus::AFRAID:
		[[fallthrough]];
	case CStatus::SILENCED:
		[[fallthrough]];
	case CStatus::ASLEEP:
		if (character->get_max_hp() / character->get_current_hp() > 10)
			return ImVec4{1.0f, 0.0f, 0.0f, _system->animation->fade};
		else if (character->get_max_hp() / character->get_current_hp() > 5)
			return ImVec4{1.0f, 1.0f, 0.0f, _system->animation->fade};
		else
			return ImVec4{1.0f, 1.0f, 1.0f, _system->animation->fade};
		break;
	case CStatus::ASHES:
		return ImVec4{1.0f, 0.0f, 0.0f, _system->animation->fade};
		break;
	case CStatus::DEAD:
		return ImVec4{1.0f, 0.0f, 0.0f, _system->animation->fade};
		break;
	case CStatus::HELD:
		return ImVec4{0.5f, 0.0f, 0.5f, _system->animation->fade};
		break;
	case CStatus::LOST:
		return ImVec4{1.0f, 0.0f, 0.0f, _system->animation->fade};
		break;
	case CStatus::STONED:
		return ImVec4{0.5f, 0.5f, 0.5f, _system->animation->fade};
		break;
	default:
		return ImVec4{1.0f, 1.0f, 1.0f, _system->animation->fade};
		break;
	}
}

auto Sorcery::UI::_draw_spell_info() -> void {
	const auto idx{_controller->selected["spellbook_selected"]};
	if (idx == 50)
		return;

	auto cmp{(*components)["spellbook:spell_data"]};
	ImVec2 pos{cmp.x * adj_grid_w, cmp.y * adj_grid_h};
	ImGui::SetNextWindowPos(pos);
	with_Window(WINDOW_LAYER_TEXTS, nullptr, ImGuiWindowFlags_NoDecoration) {
		with_Child("spell_child", ImVec2(grid_sz * cmp.w, grid_sz * cmp.h)) {

			auto spell{(
				*_resources
					 ->spells)[magic_enum::enum_cast<Enums::Magic::SpellID>(idx)
								   .value()]};

			const auto spell_name{
				fmt::format("{} \"{}\"", spell.name, spell.translated_name)};
			const auto spell_type{spell.type == Enums::Magic::SpellType::MAGE
									  ? "Mage"
									  : "Priest"};

			std::string spell_category{
				magic_enum::enum_name<Enums::Magic::SpellCategory>(
					spell.category)};
			std::transform(spell_category.begin(), spell_category.end(),
						   spell_category.begin(), ::tolower);
			auto summary{fmt::format("Level {} {} {} spell", spell.level,
									 spell_type, spell_category)};

			{
				set_StyleColor(ImGuiCol_Text, ImVec4{1.0f, 1.0f, 1.0f,
													 _system->animation->fade});
				ImGui::TextUnformatted(spell_name.c_str());
				ImGui::NewLine();
				ImGui::TextUnformatted(summary.c_str());
				ImGui::NewLine();
			}

			set_StyleColor(ImGuiCol_Text,
						   ImVec4{0.8f, 0.8f, 0.8f, _system->animation->fade});
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-security"
			ImGui::TextWrapped(spell.details.c_str());
#pragma GCC diagnostic pop
		}
	}
}

auto Sorcery::UI::_draw_monster_info() -> void {
	// Custom Rendering
	const auto idx{_controller->selected["bestiary_selected"]};
	const auto mon{(*_resources->monsters)[idx]};
	const auto k_gfx{mon.get_known_gfx()};
	const auto u_gfx{mon.get_unknown_gfx()};
	auto k_mg_c{(*components)["bestiary:known_monster_graphic"]};
	auto u_mg_c{(*components)["bestiary:unknown_monster_graphic"]};
	auto k_mg_pos{ImVec2{k_mg_c.x * adj_grid_w, k_mg_c.y * adj_grid_h}};
	auto u_mg_pos{ImVec2{u_mg_c.x * adj_grid_w, u_mg_c.y * adj_grid_h}};
	_draw_fg_image_with_idx(KNOWN_CREATURES_TEXTURE, k_gfx, k_mg_pos,
							ImVec2{std::stof(k_mg_c["tile_width"].value()),
								   std::stof(k_mg_c["tile_width"].value())});
	_draw_fg_image_with_idx(UNKNOWN_CREATURES_TEXTURE, u_gfx, u_mg_pos,
							ImVec2{std::stof(u_mg_c["tile_width"].value()),
								   std::stof(u_mg_c["tile_width"].value())});

	auto cmp{(*components)["bestiary:monster_data"]};
	ImVec2 pos{cmp.x * adj_grid_w, cmp.y * adj_grid_h};

	with_Window(WINDOW_LAYER_MENUS, nullptr, ImGuiWindowFlags_NoDecoration) {
		const auto name{fmt::format("  {:>03}:{}/{}", idx, mon.get_known_name(),
									mon.get_unknown_name())};
		ImGui::SetCursorPos(pos);
		with_Child("bestiary_tab_bar_child",
				   ImVec2(grid_sz * cmp.w, grid_sz * cmp.h)) {
			set_StyleColor(ImGuiCol_Tab,
						   ImVec4{0.0f, 0.0f, 0.0f, _system->animation->fade});
			ImGuiTabBarFlags tb_flags{ImGuiTabBarFlags_None};
			with_TabBar("bestiary_tab_bar", tb_flags) {
				with_TabItem("Info") {
					{
						set_StyleColor(
							ImGuiCol_Text,
							ImVec4{1.0f, 1.0f, 1.0f, _system->animation->fade});
						ImGui::TextUnformatted(name.c_str());
					}

					set_StyleColor(
						ImGuiCol_Text,
						ImVec4{0.8f, 0.8f, 0.8f, _system->animation->fade});

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
						type = fmt::format(" Type:{} ({})", mon_t, mon_c);
					else
						type = fmt::format(" Type:{}", mon_t);
					const auto level{fmt::format("Level:{}", mon.get_level())};
					const auto xp{fmt::format("   XP:{}", mon.get_xp())};
					const auto group{
						fmt::format("Group:{}", mon.get_group_size().str())};
					ImGui::TextUnformatted(type.c_str());
					ImGui::TextUnformatted(level.c_str());
					ImGui::TextUnformatted(xp.c_str());
					ImGui::TextUnformatted(group.c_str());
				}
				with_TabItem("Stats") {

					{
						set_StyleColor(
							ImGuiCol_Text,
							ImVec4{1.0f, 1.0f, 1.0f, _system->animation->fade});
						ImGui::TextUnformatted(name.c_str());
					}

					set_StyleColor(
						ImGuiCol_Text,
						ImVec4{0.8f, 0.8f, 0.8f, _system->animation->fade});

					const auto atks{
						fmt::format(" Atks:{}", mon.get_attacks_str())};
					const auto ac{
						fmt::format("   Ac:{}", mon.get_armour_class())};
					const auto sr{
						fmt::format("   SR:{}%", mon.get_spell_resistance())};
					const auto hd{
						fmt::format("   HD:{}", mon.get_hit_dice().str())};

					ImGui::TextUnformatted(atks.c_str());
					ImGui::TextUnformatted(ac.c_str());
					ImGui::TextUnformatted(hd.c_str());
					ImGui::TextUnformatted(sr.c_str());
				}
				with_TabItem("Traits") {
					{
						set_StyleColor(
							ImGuiCol_Text,
							ImVec4{1.0f, 1.0f, 1.0f, _system->animation->fade});
						ImGui::TextUnformatted(name.c_str());
					}

					set_StyleColor(
						ImGuiCol_Text,
						ImVec4{0.8f, 0.8f, 0.8f, _system->animation->fade});

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
							ImVec4{1.0f, 1.0f, 1.0f, _system->animation->fade});
						ImGui::TextUnformatted(name.c_str());
					}

					set_StyleColor(
						ImGuiCol_Text,
						ImVec4{0.8f, 0.8f, 0.8f, _system->animation->fade});

					auto weaknesses{mon.get_weaknesses()};
					ImGui::TextUnformatted(weaknesses.c_str());
				}
			}
		}
	}
}

auto Sorcery::UI::_display_bestiary() -> void {
	_draw_components("bestiary");
	_draw_monster_info();
	_draw_cursor();
}

auto Sorcery::UI::_display_compendium() -> void {
	_draw_components("compendium");
	_draw_cursor();
}

auto Sorcery::UI::_display_edge_of_town(Game *game) -> void {
	_draw_components("edge_of_town");
	dialog_leave->display(_controller->get_flag_ref("want_leave_game"));
	_draw_party_panel(game);
	_draw_debug();
	_draw_cursor();
}

auto Sorcery::UI::_display_castle(Game *game) -> void {
	_draw_components("castle");
	dialog_leave->display(_controller->get_flag_ref("want_leave_game"));
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

auto Sorcery::UI::_display_inspect(Game *game, const int mode) -> void {
	_draw_components("inspect", game, mode);
	_draw_current_character(game, mode);
	if (modal_identify->show)
		modal_identify->display(_controller->get_flag_ref("want_identify"));
	_draw_debug();
	_draw_cursor();
}

auto Sorcery::UI::_display_spellbook() -> void {
	_draw_components("spellbook");
	_draw_spell_info();
	_draw_cursor();
}

auto Sorcery::UI::_display_atlas() -> void {
	_draw_components("atlas");
	_draw_level_no_player();
	_draw_cursor();
}

auto Sorcery::UI::_display_museum() -> void {
	_draw_components("museum");
	_draw_item_info();
	_draw_cursor();
}

auto Sorcery::UI::_display_inn(Game *game) -> void {
	_draw_components("inn");
	_draw_party_panel(game);
	modal_inspect->display(_controller->get_flag_ref("want_inspect"));
	modal_stay->display(_controller->get_flag_ref("want_stay"));
	modal_identify->display(_controller->get_flag_ref("want_identify"));
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
	notice_divvy->display(_controller->get_flag_ref("want_divvy_gold"));
	modal_inspect->display(_controller->get_flag_ref("want_inspect"));
	modal_identify->display(_controller->get_flag_ref("want_identify"));
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
	modal_inspect->display(_controller->get_flag_ref("want_inspect"));
	modal_help->display(_controller->get_flag_ref("want_help"));
	modal_tithe->display(_controller->get_flag_ref("want_tithe"));
	modal_identify->display(_controller->get_flag_ref("want_identify"));
	input_donate->display(_controller->get_flag_ref("want_donate"));
	notice_donated_ok->display(_controller->get_flag_ref("want_donated_ok"));
	notice_cannot_donate->display(
		_controller->get_flag_ref("want_cannot_donate"));
	notice_not_enough_gold->display(
		_controller->get_flag_ref("want_not_enough_gold"));
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
	_draw_options();
	_draw_cursor();
}

auto Sorcery::UI::_display_license(const std::string &string) -> void {
	_draw_components("license");
	auto component{(*components)["license:license_info"]};
	_draw_license(&component, string);
	_draw_cursor();
}

auto Sorcery::UI::_draw_level_no_player() -> void {
	// Menu Selection for B1F to B10F is 0 to 0, thus convert it into -1 to
	// -10 for depth
	if (_controller->selected["atlas_selected"] == 10)
		return;

	const auto depth{-1 - _controller->selected["atlas_selected"]};
	Level level{(*_resources->levels)[depth].value()};

	// Work out where and how to draw the grid
	auto tc{20};
	const auto map_c{(*components)["atlas:map_graphic"]};
	ImVec2 top_left_pos{map_c.x * adj_grid_w, map_c.y * adj_grid_h};
	const auto spacing{std::stoi(map_c["tile_spacing"].value())};
	ImVec2 tile_sz{std::stoi(map_c["tile_size"].value()),
				   std::stoi(map_c["tile_size"].value())};

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

	auto pb_c{(*components)["splash:progress_bar"]};

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
		set_Font(fonts.at(Enums::Layout::Font::DEFAULT));
		set_StyleColor(ImGuiCol_PlotHistogram,
					   ImGui::GetColorU32(ImGuiCol_ButtonHovered));
		ImGui::SetCursorPos(ImVec2{x, y});

		ImGui::ProgressBar(progress, ImVec2(width, 4), "");
	}
}

auto Sorcery::UI::_display_main_menu() -> void {
	_draw_components("main_menu");
	_draw_attract_mode();
	dialog_exit->display(_controller->get_flag_ref("want_exit_game"));
	dialog_new->display(_controller->get_flag_ref("want_new_game"));
	dialog_leave->display(_controller->get_flag_ref("want_leave_game"));

	_draw_cursor();

	bool show = true;
	ImGui::SetCurrentFont(fonts[Enums::Layout::Font::DEFAULT]);
	ImGui::ShowDemoWindow(&show);

	ImGui::SetNextWindowPos(ImVec2{1, 1});
	ImGui::SetNextWindowSize(ImVec2{1000, 1000});
}

auto Sorcery::UI::_draw_attract_mode() -> void {
	// Get the Attract Data
	const auto attract{(*components)["main_menu:attract_mode"]};
	_attract_data = _system->animation->get_attract_data();

	// Work out the size and this where to draw it (as its centred)!
	auto am_size{_attract_data.size() *
				 std::stoi(attract["tile_width"].value())};
	am_size +=
		(_attract_data.size() - 1) * std::stoi(attract["tile_spacing"].value());
	const auto viewport{ImGui::GetMainViewport()};
	auto tile_pos{
		ImVec2{(viewport->Size.x - am_size) / 2, attract.y * adj_grid_h}};

	// And draw each tile (this will draw to the correct layer)
	for (auto idx : _attract_data) {

		_draw_fg_image_with_idx(
			KNOWN_CREATURES_TEXTURE, idx, tile_pos,
			ImVec2{std::stof(attract["tile_width"].value()),
				   std::stof(attract["tile_width"].value())});
		tile_pos.x += (std::stoi(attract["tile_width"].value()) +
					   (std::stoi(attract["tile_spacing"].value())));
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
	const ImU32 bg{ImColor{ImVec4{0.0f, 0.0f, 0.0f, _system->animation->fade}}};

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
						items.emplace_back(fmt::format("{}:{:^19}", pos,
													   character.get_name()));

					else if (flags & MENU_SHOW_GOLD)
						items.emplace_back(fmt::format("{:<16} {:>8} G.P.",
													   character.get_name(),
													   character.get_gold()));
					else
						items.emplace_back(
							fmt::format("{:^21}", character.get_name()));
					data.emplace_back(id);
					if (reorder)
						controller->candidate_party.emplace_back(id);
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
					character.get_status() == Enums::Character::CStatus::OK) {
					items.emplace_back(
						fmt::format("{:^21}", character.get_name()));
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
						fmt::format("{:^21}", character.get_name()));
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

			using Enums::Character::CStatus;
			for (auto &[id, character] : game->characters) {
				if (character.get_status() == CStatus::ASHES ||
					character.get_status() == CStatus::DEAD ||
					character.get_status() == CStatus::HELD ||
					character.get_status() == CStatus::STONED) {
					items.emplace_back(
						fmt::format("{:<16} {:>8}", character.get_name(),
									character.get_status_string()));
					data.emplace_back(id);
				}
			}
		}
	};
	auto load_character_items = []([[maybe_unused]] Controller *controller,
								   Game *game, std::vector<std::string> &items,
								   std::vector<int> &data,
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
								fmt::format("{}){}{:<16}", slot, flag,
											item.get_display_name())};
							items.emplace_back(item_display);
						} else {
							auto item_display{
								fmt::format("{}){}{:<16} {:>5}%", slot, flag,
											item.get_display_name(), chance)};
							items.emplace_back(item_display);
						}
					} else {
						auto item_display{fmt::format("{}){}{:<16}", slot, flag,
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
		load_character_items(controller, game, items, data, MENU_IDENTIFY_ITEM);
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
				fmt::format("{:^{}}", (*_system->strings)[source], width));

	} else if (component == "camp_menu") {
		sources.insert(sources.end(),
					   {"CAMP_INSPECT", "CAMP_REORDER", "CAMP_OPTIONS",
						"CAMP_QUIT", "CAMP_LEAVE"});
		for (const auto &source : sources)
			items.emplace_back(
				fmt::format("{:^{}}", (*_system->strings)[source], width));

	} else if (component == "roster_menu") {
		sources.insert(sources.end(), {"ROSTER_RETURN"});
		for (const auto &source : sources)
			items.emplace_back(
				fmt::format("{:^{}}", (*_system->strings)[source], width));

	} else if (component == "inspect_menu" || component == "modal_inspect") {
		sources.insert(sources.end(), {"INSPECT_RETURN"});
		for (const auto &source : sources)
			items.emplace_back(
				fmt::format("{:^{}}", (*_system->strings)[source], width));

	} else if (component == "stay_menu" || component == "modal_stay") {
		sources.insert(sources.end(), {"STAY_RETURN"});
		for (const auto &source : sources)
			items.emplace_back(
				fmt::format("{:^{}}", (*_system->strings)[source], width));

	} else if (component == "help_menu" || component == "modal_help") {
		sources.insert(sources.end(), {"HELP_RETURN"});
		for (const auto &source : sources)
			items.emplace_back(
				fmt::format("{:^{}}", (*_system->strings)[source], width));

	} else if (component == "tithe_menu" || component == "modal_tithe") {
		sources.insert(sources.end(), {"TITHE_RETURN"});
		for (const auto &source : sources)
			items.emplace_back(
				fmt::format("{:^{}}", (*_system->strings)[source], width));

	} else if (component == "pay_menu") {
		sources.insert(sources.end(), {"PAY_RETURN"});
		for (const auto &source : sources)
			items.emplace_back(
				fmt::format("{:^{}}", (*_system->strings)[source], width));

	} else if (component == "choose_menu") {
		sources.insert(sources.end(), {"CHOOSE_RETURN"});
		for (const auto &source : sources)
			items.emplace_back(
				fmt::format("{:^{}}", (*_system->strings)[source], width));

	} else if (component == "remove_menu") {
		sources.insert(sources.end(), {"REMOVE_RETURN"});
		for (const auto &source : sources)
			items.emplace_back(
				fmt::format("{:^{}}", (*_system->strings)[source], width));

	} else if (component == "add_menu") {
		sources.insert(sources.end(), {"ADD_RETURN"});
		for (const auto &source : sources)
			items.emplace_back(
				fmt::format("{:^{}}", (*_system->strings)[source], width));

	} else if (component == "restart_menu") {
		sources.insert(sources.end(), {"RESTART_RETURN"});
		for (const auto &source : sources)
			items.emplace_back(
				fmt::format("{:^{}}", (*_system->strings)[source], width));

	} else if (component == "identify_menu" || component == "modal_identify") {
		sources.insert(sources.end(), {"IDENTIFY_RETURN"});
		for (const auto &source : sources)
			items.emplace_back(
				fmt::format("{:^{}}", (*_system->strings)[source], width));

	} else if (component == "reorder_menu") {
		sources.insert(sources.end(), {"REORDER_RETURN"});
		for (const auto &source : sources)
			items.emplace_back(
				fmt::format("{:^{}}", (*_system->strings)[source], width));

	} else if (component == "castle_menu") {
		sources.insert(sources.end(),
					   {"CASTLE_TAVERN", "CASTLE_INN", "CASTLE_SHOP",
						"CASTLE_TEMPLE", "CASTLE_EDGE_OF_TOWN"});
		for (const auto &source : sources)
			items.emplace_back(
				fmt::format("{:^{}}", (*_system->strings)[source], width));

	} else if (component == "rest_menu") {
		sources.insert(sources.end(), {"STAY_1", "STAY_2", "STAY_3", "STAY_4",
									   "STAY_5", "STAY_RETURN"});
		for (const auto &source : sources)
			items.emplace_back(
				fmt::format("{:^{}}", (*_system->strings)[source], width));
	} else if (component == "tavern_menu") {
		sources.insert(sources.end(),
					   {"TAVERN_ADD_TO_PARTY", "TAVERN_REMOVE_FROM_PARTY",
						"TAVERN_REORDER_PARTY", "TAVERN_INSPECT",
						"TAVERN_DIVVY_GOLD", "TAVERN_CASTLE"});
		for (const auto &source : sources)
			items.emplace_back(
				fmt::format("{:^{}}", (*_system->strings)[source], width));

	} else if (component == "inn_menu") {
		sources.insert(sources.end(),
					   {"INN_STAY", "INN_INSPECT", "INN_CASTLE"});
		for (const auto &source : sources)
			items.emplace_back(
				fmt::format("{:^{}}", (*_system->strings)[source], width));

	} else if (component == "temple_menu") {
		sources.insert(sources.end(), {"TEMPLE_HELP", "TEMPLE_INSPECT",
									   "TEMPLE_TITHE", "TEMPLE_CASTLE"});
		for (const auto &source : sources)
			items.emplace_back(
				fmt::format("{:^{}}", (*_system->strings)[source], width));

	} else if (component == "shop_menu") {
		sources.insert(sources.end(),
					   {"SHOP_ENTER", "SHOP_INSPECT", "SHOP_CASTLE"});
		for (const auto &source : sources)
			items.emplace_back(
				fmt::format("{:^{}}", (*_system->strings)[source], width));

	} else if (component == "edge_menu") {
		sources.insert(sources.end(),
					   {"EDGE_OF_TOWN_TRAIN", "EDGE_OF_TOWN_MAZE",
						"EDGE_OF_TOWN_RESTART", "EDGE_OF_TOWN_CASTLE",
						"EDGE_OF_TOWN_LEAVE_GAME"});
		for (const auto &source : sources)
			items.emplace_back(
				fmt::format("{:^{}}", (*_system->strings)[source], width));

	} else if (component == "atlas_menu") {
		sources.insert(sources.end(),
					   {"ATLAS_MENU_B1F", "ATLAS_MENU_B2F", "ATLAS_MENU_B3F",
						"ATLAS_MENU_B4F", "ATLAS_MENU_B5F", "ATLAS_MENU_B6F",
						"ATLAS_MENU_B7F", "ATLAS_MENU_B8F", "ATLAS_MENU_B9F",
						"ATLAS_MENU_B10F", "ATLAS_RETURN"});
		for (const auto &source : sources)
			items.emplace_back(
				fmt::format("{:^{}}", (*_system->strings)[source], width));

	} else if (component == "training_menu") {
		sources.insert(sources.end(),
					   {"TRAINING_GROUNDS_CREATE", "TRAINING_GROUNDS_EDIT",
						"TRAINING_GROUNDS_DELETE", "TRAINING_GROUNDS_INSPECT",
						"TRAINING_GROUNDS_RETURN"});
		for (const auto &source : sources)
			items.emplace_back(
				fmt::format("{:^{}}", (*_system->strings)[source], width));

	} else if (component == "main_menu") {
		sources.insert(sources.end(),
					   {"MAIN_MENU_OPTION_START", "MAIN_MENU_OPTION_CONTINUE",
						"MAIN_MENU_OPTION_OPTIONS",
						"MAIN_MENU_OPTION_COMPENDIUM",
						"MAIN_MENU_OPTION_LICENSE", "MAIN_MENU_OPTION_EXIT"});
		for (const auto &source : sources)
			items.emplace_back(
				fmt::format("{:^{}}", (*_system->strings)[source], width));

	} else if (component == "bestiary_menu") {
		const auto monster_types{_resources->monsters->get_all_types()};
		for (auto &monster : monster_types) {
			if (monster.get_type_id() > Enums::Monsters::TypeID::WERDNA)
				continue;
			auto mname{monster.get_known_name()};
			auto mid{unenum(monster.get_type_id())};
			auto padded{fmt::format("{:^{}}", mname, width)};
			auto menu_item{fmt::format("{}##{}", padded, mid)};
			items.emplace_back(fmt::format("{}", menu_item));
		}
		items.emplace_back(fmt::format(
			"{:^{}}", (*_system->strings)["BESTIARY_RETURN"], width));

	} else if (component == "spellbook_menu") {
		const auto spells{_resources->spells->get_all()};
		for (auto &spell : spells) {
			auto sname{spell.name};
			auto padded{fmt::format("{:^{}}", sname, width)};
			items.emplace_back(fmt::format("{}", padded));
		}
		items.emplace_back(fmt::format(
			"{:^{}}", (*_system->strings)["SPELLBOOK_RETURN"], width));
	} else if (component == "museum_menu") {
		const auto item_types{_resources->items->get_all_types()};
		for (auto &item_type : item_types) {
			if (item_type.get_type_id() != Enums::Items::TypeID::BROKEN_ITEM) {
				auto iname{item_type.get_known_name()};
				auto iid{unenum(item_type.get_type_id())};
				auto padded{fmt::format("{:^{}}", iname, width)};
				auto menu_item{fmt::format("{}##{}", padded, iid)};
				items.emplace_back(fmt::format("{}", menu_item));
			}
		}
		items.emplace_back(
			fmt::format("{:^{}}", (*_system->strings)["MUSEUM_RETURN"], width));
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
				   ImVec4{0.0f, 0.0f, 0.0f, 1.0f - _system->animation->fade});
	set_StyleColor(ImGuiCol_HeaderActive, (ImVec4)sel_color);
	set_StyleColor(ImGuiCol_HeaderHovered, (ImVec4)sel_color);
	set_StyleColor(ImGuiCol_Text,
				   ImVec4{1.0f, 1.0f, 1.0f, _system->animation->fade});
	set_StyleColor(ImGuiCol_TextDisabled,
				   ImVec4{0.5f, 0.5f, 0.5f, _system->animation->fade});
	set_Font(fonts.at(font));
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
				_controller->is_menu_item_disabled(name, i, data_item)};
			if (disabled)
				ImGui::BeginDisabled();

			if (reorder) {

				if (ImGui::Selectable(items[i].c_str(), is_selected, flags)) {
					_controller->handle_menu(name, items, data_item, i);
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

								_controller->candidate_party.clear();
								for (auto char_id : data)
									_controller->candidate_party.emplace_back(
										char_id);
								_controller->set_flag("party_order_changed");
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
						_controller->handle_menu_with_flags(
							name, items, data_item, i, out_flags);
					} else if (name == "edge_menu") {
						std::vector<std::reference_wrapper<bool>> out_flags{
							{std::ref(dialog_leave->show)}};
						_controller->handle_menu_with_flags(
							name, items, data_item, i, out_flags);
					} else if (name == "tavern_menu") {
						std::vector<std::reference_wrapper<bool>> out_flags{
							{std::ref(notice_divvy->show),
							 std::ref(modal_inspect->show)}};
						_controller->handle_menu_with_flags(
							name, items, data_item, i, out_flags);
					} else if (name == "inn_menu") {
						std::vector<std::reference_wrapper<bool>> out_flags{
							{std::ref(modal_inspect->show),
							 std::ref(modal_stay->show)}};
						_controller->handle_menu_with_flags(
							name, items, data_item, i, out_flags);
					} else if (name == "temple_menu") {
						std::vector<std::reference_wrapper<bool>> out_flags{
							{std::ref(modal_inspect->show),
							 std::ref(modal_help->show),
							 std::ref(modal_tithe->show)}};
						_controller->handle_menu_with_flags(
							name, items, data_item, i, out_flags);
					} else if (name == "camp_menu") {
						std::vector<std::reference_wrapper<bool>> out_flags{
							{std::ref(modal_camp->show)}};
						_controller->handle_menu_with_flags(
							name, items, data_item, i, out_flags);
					} else if (name == "inspect_menu") {
						std::vector<std::reference_wrapper<bool>> out_flags{
							{std::ref(modal_inspect->show)}};
						_controller->handle_menu_with_flags(
							name, items, data_item, i, out_flags);
					} else if (name == "stay_menu") {
						std::vector<std::reference_wrapper<bool>> out_flags{
							{std::ref(modal_stay->show)}};
						_controller->handle_menu_with_flags(
							name, items, data_item, i, out_flags);
					} else if (name == "help_menu") {
						std::vector<std::reference_wrapper<bool>> out_flags{
							{std::ref(modal_help->show)}};
						_controller->handle_menu_with_flags(
							name, items, data_item, i, out_flags);
					} else if (name == "tithe_menu") {
						std::vector<std::reference_wrapper<bool>> out_flags{
							{std::ref(modal_tithe->show),
							 std::ref(input_donate->show)}};
						_controller->handle_menu_with_flags(
							name, items, data_item, i, out_flags);
					} else if (name == "identify_menu") {
						std::vector<std::reference_wrapper<bool>> out_flags{
							{std::ref(modal_identify->show)}};
						_controller->handle_menu_with_flags(
							name, items, data_item, i, out_flags);
					}

					else

						// Any other menus
						_controller->handle_menu(name, items, data_item, i);
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
	using Enums::DrawMap::Feature;
	using Enums::Map::Direction;
	using Enums::Tile::Edge;
	using Enums::Tile::Features;
	using Enums::Tile::Properties;

	// Background Graphic
	_draw_fg_image_with_idx(MAPS_TEXTURE, unenum(Feature::FLOOR), pos, sz);

	// Darkness
	if (tile.is(Enums::Tile::Properties::DARKNESS))
		_draw_fg_image_with_idx(MAPS_TEXTURE, unenum(Feature::DARKNESS), pos,
								sz);

	// Walls for all 4 directions
	if (tile.has(Direction::NORTH, Edge::SECRET_DOOR) ||
		tile.has(Direction::NORTH, Edge::ONE_WAY_HIDDEN_DOOR))
		_draw_fg_image_with_idx(MAPS_TEXTURE, unenum(Feature::NORTH_SECRET),
								pos, sz);
	else if (tile.has(Direction::NORTH, Edge::UNLOCKED_DOOR) ||
			 tile.has(Direction::NORTH, Edge::ONE_WAY_DOOR))
		_draw_fg_image_with_idx(MAPS_TEXTURE, unenum(Feature::NORTH_DOOR), pos,
								sz);
	else if (tile.has(Direction::NORTH, Edge::ONE_WAY_WALL))
		_draw_fg_image_with_idx(MAPS_TEXTURE,
								unenum(Feature::NORTH_ONE_WAY_WALL), pos, sz);
	else if (tile.has(Direction::NORTH))
		_draw_fg_image_with_idx(MAPS_TEXTURE, unenum(Feature::NORTH_WALL), pos,
								sz);

	if (tile.has(Direction::SOUTH, Edge::SECRET_DOOR) ||
		tile.has(Direction::SOUTH, Edge::ONE_WAY_HIDDEN_DOOR))
		_draw_fg_image_with_idx(MAPS_TEXTURE, unenum(Feature::SOUTH_SECRET),
								pos, sz);
	else if (tile.has(Direction::SOUTH, Edge::UNLOCKED_DOOR) ||
			 tile.has(Direction::SOUTH, Edge::ONE_WAY_DOOR))
		_draw_fg_image_with_idx(MAPS_TEXTURE, unenum(Feature::SOUTH_DOOR), pos,
								sz);
	else if (tile.has(Direction::SOUTH, Edge::ONE_WAY_WALL))
		_draw_fg_image_with_idx(MAPS_TEXTURE,
								unenum(Feature::SOUTH_ONE_WAY_WALL), pos, sz);
	else if (tile.has(Direction::SOUTH))
		_draw_fg_image_with_idx(MAPS_TEXTURE, unenum(Feature::SOUTH_WALL), pos,
								sz);

	if (tile.has(Direction::EAST, Edge::SECRET_DOOR) ||
		tile.has(Direction::EAST, Edge::ONE_WAY_HIDDEN_DOOR))
		_draw_fg_image_with_idx(MAPS_TEXTURE, unenum(Feature::EAST_SECRET), pos,
								sz);
	else if (tile.has(Direction::EAST, Edge::UNLOCKED_DOOR) ||
			 tile.has(Direction::EAST, Edge::ONE_WAY_DOOR))
		_draw_fg_image_with_idx(MAPS_TEXTURE, unenum(Feature::EAST_DOOR), pos,
								sz);
	else if (tile.has(Direction::EAST, Edge::ONE_WAY_WALL))
		_draw_fg_image_with_idx(MAPS_TEXTURE,
								unenum(Feature::EAST_ONE_WAY_WALL), pos, sz);
	else if (tile.has(Direction::EAST))
		_draw_fg_image_with_idx(MAPS_TEXTURE, unenum(Feature::EAST_WALL), pos,
								sz);

	if (tile.has(Direction::WEST, Edge::SECRET_DOOR) ||
		tile.has(Direction::WEST, Edge::ONE_WAY_HIDDEN_DOOR))
		_draw_fg_image_with_idx(MAPS_TEXTURE, unenum(Feature::WEST_SECRET), pos,
								sz);
	else if (tile.has(Direction::WEST, Edge::UNLOCKED_DOOR) ||
			 tile.has(Direction::WEST, Edge::ONE_WAY_DOOR))
		_draw_fg_image_with_idx(MAPS_TEXTURE, unenum(Feature::WEST_DOOR), pos,
								sz);
	else if (tile.has(Direction::WEST, Edge::ONE_WAY_WALL))
		_draw_fg_image_with_idx(MAPS_TEXTURE,
								unenum(Feature::WEST_ONE_WAY_WALL), pos, sz);
	else if (tile.has(Direction::WEST))
		_draw_fg_image_with_idx(MAPS_TEXTURE, unenum(Feature::WEST_WALL), pos,
								sz);

	// And Tile Contents
	if (tile.has(Features::STAIRS_UP) || tile.has(Features::LADDER_UP))
		_draw_fg_image_with_idx(MAPS_TEXTURE, unenum(Feature::STAIRS_UP), pos,
								sz);
	else if (tile.has(Features::STAIRS_DOWN) || tile.has(Features::LADDER_DOWN))
		_draw_fg_image_with_idx(MAPS_TEXTURE, unenum(Feature::STAIRS_DOWN), pos,
								sz);
	else if (tile.has(Features::ELEVATOR))
		_draw_fg_image_with_idx(MAPS_TEXTURE, unenum(Feature::ELEVATOR), pos,
								sz);
	else if (tile.has(Features::SPINNER))
		_draw_fg_image_with_idx(MAPS_TEXTURE, unenum(Feature::SPINNER), pos,
								sz);
	else if (tile.has(Features::PIT))
		_draw_fg_image_with_idx(MAPS_TEXTURE, unenum(Feature::PIT), pos, sz);
	else if (tile.has(Features::CHUTE))
		_draw_fg_image_with_idx(MAPS_TEXTURE, unenum(Feature::CHUTE), pos, sz);
	else if (tile.has(Features::TELEPORT_TO))
		_draw_fg_image_with_idx(MAPS_TEXTURE, unenum(Feature::TELEPORT_TO), pos,
								sz);
	else if (tile.has(Features::TELEPORT_FROM))
		_draw_fg_image_with_idx(MAPS_TEXTURE, unenum(Feature::TELEPORT_FROM),
								pos, sz);
	else if (tile.has(Features::MESSAGE) || tile.has(Features::NOTICE))
		_draw_fg_image_with_idx(MAPS_TEXTURE, unenum(Feature::EXCLAMATION), pos,
								sz);
}
