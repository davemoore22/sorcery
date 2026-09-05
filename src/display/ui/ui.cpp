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

#include "display/ui/ui.hpp"			 // for UI, TransientMessage, Trans...
#include "backends/imgui_impl_opengl3.h" // for ImGui_ImplOpenGL3_NewFrame
#include "backends/imgui_impl_sdl2.h"	 // for ImGui_ImplSDL2_NewFrame
#include "common/enum.hpp"				 // for Feature, Ability, Event
#include "common/macro.hpp"				 // for CAPITALISE
#include "common/types.hpp"				 // for Spell, Size, Coordinate
#include "core/context.hpp"				 // for Context
#include "core/controller/actionhandler.hpp"
#include "core/controller/controller.hpp"	// for Controller
#include "core/controller/inputhandler.hpp" // For ControllerInputHandler
#include "core/controller/menubuilder.hpp"	// for MenuBuilder
#include "core/controller/menuhandler.hpp"
#include "core/debug.hpp"				 // for DEBUG_LOG, DEBUG_LOGF, debu...
#include "core/define.hpp"				 // for WINDOW_LAYER_TEXTS, WINDOW_...
#include "core/enum.hpp"				 // for Screen, CharacterSlot
#include "core/macro.hpp"				 // for CSTR
#include "core/resources.hpp"			 // for Resources
#include "core/system.hpp"				 // for System
#include "display/animation.hpp"		 // for Animation
#include "display/display.hpp"			 // for Display, DisplayMetrics
#include "display/render.hpp"			 // for Render
#include "display/ui/popupmanager.hpp"	 // for PopupManager
#include "display/ui/screenrenderer.hpp" // for ScreenRenderer
#include "display/ui/uimetrics.hpp"		 // for UIMetrics, GLuint
#include "display/ui/uistyle.hpp"		 // for set_text_bright, set_text_dim
#include "drawables/dialog.hpp"			 // for Dialog
#include "drawables/frame.hpp"			 // for Frame
#include "drawables/input.hpp"			 // for Input
#include "drawables/menu.hpp"			 // for Menu
#include "drawables/message.hpp"		 // for Message
#include "drawables/videoplayer.hpp"	 // for VideoPlayer
#include "engine/define.hpp"			 // for GRAVESTONE_GFX_ID
#include "engine/types.hpp"				 // for Vertex, VertexArray
#include "imgui.h"						 // for ImVec2, ImVec4, TextUnforma...
#include "imgui_internal.h"				 // for ImGuiSelectableFlagsPrivate_
#include "misc/cpp/imgui_stdlib.h"		 // for InputText
#include "resources/componentstore.hpp"	 // for ComponentStore
#include "resources/define.hpp"			 // for MAPS_TEXTURE, ICONS_TEXTURE
#include "resources/fontstore.hpp"		 // for FontInfo, FontStore
#include "resources/imagestore.hpp"		 // for ImageStore
#include "resources/itemstore.hpp"		 // for ItemStore
#include "resources/levelstore.hpp"		 // for LevelStore
#include "resources/monsterstore.hpp"	 // for MonsterStore
#include "resources/spellstore.hpp"		 // for SpellStore
#include "types/character/character.hpp" // for Character
#include "types/character/create.hpp"	 // for CharacterCreate
#include "types/character/inventory.hpp" // for Inventory
#include "types/character/magic.hpp"	 // for ConstCharacterMagic
#include "types/component.hpp"			 // for Component
#include "types/config.hpp"				 // for Config
#include "types/dice.hpp"				 // for Dice
#include "types/enum.hpp"				 // for ComponentType, Font, Class
#include "types/error.hpp"				 // for Error, operator<<
#include "types/game.hpp"				 // for Game
#include "types/image.hpp"				 // for Image
#include "types/item/item.hpp"			 // for Item
#include "types/item/itemtype.hpp"		 // for ItemType
#include "types/meta.hpp"				 // for enum_cast, enum_name
#include "types/monstertype.hpp"		 // for MonsterType
#include "types/state.hpp"				 // for State
#include "types/world/explore.hpp"		 // for Explore
#include "types/world/level.hpp"		 // for Level
#include "types/world/tile.hpp"			 // for Tile
#include <SDL.h>						 // for SDL_Quit
#include <SDL_timer.h>					 // for SDL_GetTicks
#include <SDL_video.h>					 // for SDL_SetWindowFullscreen
#include <algorithm>					 // for min, find, remove_if, trans...
#include <any>							 // for any
#include <array>						 // for array
#include <cctype>						 // for tolower
#include <chrono>						 // for steady_clock, operator+
#include <compare>						 // for operator>=, strong_ordering
#include <cstdint>						 // for intptr_t, uintptr_t
#include <cstdlib>						 // for size_t, exit, EXIT_FAILURE
#include <exception>					 // for exception
#include <filesystem>					 // for path
#include <format>						 // for format
#include <functional>					 // for ref, reference_wrapper, invoke
#include <imgui_sugar.hpp>				 // for BooleanGuard, with_Window
#include <imgui_toggle.h>				 // for Toggle
#include <initializer_list>				 // for initializer_list
#include <iostream>						 // for basic_ostream, cerr
#include <map>							 // for map, operator==
#include <memory>						 // for unique_ptr, shared_ptr, mak...
#include <optional>						 // for optional, nullopt, nullopt_t
#include <ranges>						 // for _Filter, _Partial, filter
#include <regex>						 // for regex, regex_token_iterator
#include <string>						 // for basic_string, string, char_...
#include <string_view>					 // for basic_string_view, string_view
#include <utility>						 // for pair, to_underlying, move
#include <vector>						 // for vector

Sorcery::UI::UI(Context &ctx)
	: _ctx{ctx} {

	// Storage
	components = std::make_unique<ComponentStore>(_ctx.get_file(LAYOUT_FILE));
	images = std::make_unique<ImageStore>(_ctx);
	menubuilder = std::make_unique<MenuBuilder>(_ctx);

	_ctx.components = components.get();
	popup_manager = std::make_unique<PopupManager>(_ctx);
	screens = std::make_unique<ScreenRenderer>(*this, _ctx);
	metrics = std::make_unique<UIMetrics>(_ctx);

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

	// Window, Font, and Display Settings
	frame_rd = std::stoi(_ctx.get_config("Frame", "rounding"));
	ui_rd = std::stoi(_ctx.get_config("UI", "rounding"));

	// Updates _font_sz, _adj_grid_w, _adj_grid_h, and _grid_sz
	_ctx.display->update_display_metrics();
	metrics->update(_ctx.display->get_display_metrics());

	// Render window
	render = std::make_unique<Render>(_ctx);

	// Ticks
	ticks = SDL_GetTicks();
};

Sorcery::UI::~UI() {}

auto Sorcery::UI::set_monochrome(const bool value) -> void {

	render->set_monochrome(value);
}

auto Sorcery::UI::set_fullscreen(const bool value) -> void {

	if (value)
		SDL_SetWindowFullscreen(_ctx.display->get_SDL_window(),
								SDL_WINDOW_FULLSCREEN_DESKTOP);
	else
		SDL_SetWindowFullscreen(_ctx.display->get_SDL_window(), 0);

	_ctx.controller->set_fullscreen(value);
}

auto Sorcery::UI::start() -> void {

	// DEBUG_LOG("Starting UI...");

	// Initialise ImGUI to use SDL2/OpenGL
	ImGui::CreateContext();
	_io = &ImGui::GetIO();

	_imgui_ini_path = _ctx.get_file(IMGUI_INI_FILE).string();
	_io->IniFilename = CSTR(_imgui_ini_path);

	// DEBUG_LOGF("ImGui ini: {}", _imgui_ini_path);

	ImGui::StyleColorsClassic();
	ImGui_ImplSDL2_InitForOpenGL(_ctx.display->get_SDL_window(),
								 _ctx.display->get_GL_context());
	ImGui_ImplOpenGL3_Init(_ctx.display->get_GLSL_version());

	if (_ctx.get_config(Enums::Config::FULLSCREEN))
		set_fullscreen(true);
	else
		set_fullscreen(false);

	// Can create the fontstore now which loads the fonts
	fonts = std::make_unique<FontStore>(_ctx, _io);
	ui_colour = ImVec4{std::stof(_ctx.get_config("Frame", "colour_red")),
					   std::stof(_ctx.get_config("Frame", "colour_green")),
					   std::stof(_ctx.get_config("Frame", "colour_blue")), 1.0};

	// Set the Default Fonts
	using enum Enums::Layout::Font;
	fonts->set_current_font(TEXT, _ctx.get_config("Font", "text"));
	fonts->set_current_font(DEFAULT, fonts->get_default_font());
	fonts->set_current_font(MONOSPACE, _ctx.get_config("Font", "monospace"));
	fonts->set_current_font(PROPORTIONAL,
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

	_attract_data.clear();
}

auto Sorcery::UI::stop() -> void {

	// DEBUG_LOG("Stopping UI...");

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
	SDL_GL_DeleteContext(_ctx.display->get_GL_context());
	SDL_DestroyWindow(_ctx.display->get_SDL_window());
	SDL_Quit();
}

auto Sorcery::UI::display_refresh(std::any payload) -> void {

	// Refresh what we previously drew
	display_screen(_ctx.controller->get_last_screen(), payload);
}

auto Sorcery::UI::close_all_popups() -> void {

	popup_manager->reset();
}

// TODO: Don't move this for now
auto Sorcery::UI::display_engine() -> void {

	// Start a new Rendering Frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	_setup_windows();

	// Background
	draw_components("engine_base_ui");

	if (!_ctx.controller->get_monochrome()) {
		auto bg_c{components->get("engine_base_ui:background_image")};
		draw_tiled_bg(&bg_c);
	}

	popup_manager->display();

	if (_ctx.get_flag("interface_ui") && _ctx.get_flag("interface_party_panel"))
		draw_party_panel();
	if (_ctx.get_flag("interface_ui")) {
		draw_compass();
		draw_buffbar();
		draw_level_name();
		draw_icons();
		draw_save();
	}

	// Dungeon View
	auto component{components->get("engine_base_ui:wire_frame_view")};
	render->draw(&component);

	// Transient overlay
	draw_transient();

	// And Cursor on Top
	draw_debug();
	draw_ui_status();
	draw_cursor();

	// bool show{true};
	// ImGui::PushFont(fontstore->get_default_font());
	// ImGui::ShowDemoWindow(&show);
	// ImGui::PopFont();

	ImGui::Render();

	_ctx.display->present(ImGui::GetDrawData());
}

auto Sorcery::UI::display_screen(const Enums::Screen screen,
								 const std::any &payload) -> void {

	_ctx.controller->set_last_screen(screen);

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	_setup_windows();

	screens->display(screen, payload);

	draw_cursor();

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

	draw_fg_image_with_idx(source, idx, p_min, p_sz);
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
auto Sorcery::UI::draw_fg_image_with_idx(
	std::string_view layer, std::string_view source, const int idx,
	const ImVec2 p_min, const ImVec2 p_sz, const ImVec4 tint,
	std::optional<ImageOffsetEffect> effect) -> void {

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

		const auto texture{ImTextureRef(_to_imgui(src_image.texture))};

		if (effect) {

			const auto &offset_effect{*effect};

			const auto offset_colour{offset_effect.colour.value_or(
				ImVec4{tint.x * offset_effect.brightness,
					   tint.y * offset_effect.brightness,
					   tint.z * offset_effect.brightness, tint.w})};

			const ImVec4 offset_tint{
				offset_colour.x, offset_colour.y, offset_colour.z,
				offset_colour.w * offset_effect.alpha * _ctx.animation->fade};

			const ImVec2 offset_pos{p_min.x + offset_effect.offset.x,
									p_min.y + offset_effect.offset.y};

			ImGui::SetCursorPos(offset_pos);

			ImGui::ImageWithBg(texture, p_sz, uv_0, uv_1,
							   ImVec4{0.0f, 0.0f, 0.0f, 0.0f}, offset_tint);
		}

		ImGui::SetCursorPos(p_min);

		const ImVec4 tint_col{tint.x, tint.y, tint.z,
							  tint.w * _ctx.animation->fade};

		ImGui::ImageWithBg(texture, p_sz, uv_0, uv_1,
						   ImVec4{0.0f, 0.0f, 0.0f, 0.0f}, tint_col);
	}
}

// Handle drawing parts of a texture as specified by a tile index
auto Sorcery::UI::draw_fg_image_with_idx(
	std::string_view source, const int idx, const ImVec2 p_min,
	const ImVec2 p_sz, const ImVec4 tint,
	std::optional<ImageOffsetEffect> effect) -> void {

	draw_fg_image_with_idx(WINDOW_LAYER_IMAGES, source, idx, p_min, p_sz, tint);
}

auto Sorcery::UI::draw_fg_image(Component *component) -> void {

	if (!images->show_images) {

		with_Window(WINDOW_LAYER_IMAGES, nullptr,
					ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs) {

			const auto x{std::invoke([&] {
				if (component->x == -1) {
					const auto viewport{ImGui::GetMainViewport()};
					return (viewport->Size.x - 200) / 2;
				} else
					return metrics->grid_pos(component->x, component->y).x;
			})};
			const auto y{std::invoke([&] {
				if (component->y == -1) {
					const auto viewport{ImGui::GetMainViewport()};
					return (viewport->Size.y - 200) / 2;
				} else
					return metrics->grid_pos(component->x, component->y).y;
			})};

			ImGui::SetCursorPos(metrics->grid_pos(x, y));
			const auto scaling{_ctx.display->get_display_metrics().scale};
			ImGui::GetWindowDrawList()->AddRectFilled(
				metrics->grid_pos(x, y),
				metrics->grid_pos(x + 200 * scaling, y + 200 * scaling),
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
				return metrics->grid_pos(component->x, component->y).x;
		})};
		const auto y{std::invoke([&] {
			if (component->y == -1) {
				const auto viewport{ImGui::GetMainViewport()};
				return (viewport->Size.y - resized.h) / 2;
			} else
				return metrics->grid_pos(component->x, component->y).y;
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

auto Sorcery::UI::draw_tiled_bg(Component *component) -> void {

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

auto Sorcery::UI::draw_bg_image(Component *component) -> void {

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
	draw_cursor();
}

auto Sorcery::UI::draw_ui_status() -> void {

	constexpr auto ICON_CGA_ON{108u};
	constexpr auto ICON_CGA_OFF{109u};

	if (images->has_loaded(std::string{ICONS_TEXTURE})) {

		const auto music_status{_ctx.get_config(Enums::Config::MUSIC)};
		const auto sound_status{_ctx.get_config(Enums::Config::SOUND)};
		const auto cga_status{!render->get_monochrome()};
		const auto music_icon{music_status ? ICON_MUSIC_ON : ICON_MUSIC_OFF};
		const auto sound_icon{sound_status ? ICON_SOUND_ON : ICON_SOUND_OFF};
		const auto cga_icon{cga_status ? ICON_CGA_ON : ICON_CGA_OFF};

		const auto tint{_ctx.controller->get_monochrome()
							? ImVec4{1.0f, 1.0f, 1.0f, _ctx.animation->fade}
							: ImVec4{0.5f, 0.5f, 1.0f, _ctx.animation->fade}};

		const auto scale{_ctx.display->get_display_metrics().scale};
		auto pos{ImVec2{4 * scale, 4 * scale}};
		auto size{ImVec2{16 * scale, 16 * scale}};

		// Get Icon Effects
		const auto icon_depth{UIStyle::icon_depth(scale)};

		with_Window(WINDOW_LAYER_TEXTS, nullptr,
					ImGuiWindowFlags_NoDecoration) {

			draw_fg_image_with_idx(WINDOW_LAYER_TEXTS, ICONS_TEXTURE,
								   music_icon, pos, size, tint, icon_depth);
			pos.x += 16 * scale;
			draw_fg_image_with_idx(WINDOW_LAYER_TEXTS, ICONS_TEXTURE,
								   sound_icon, pos, size, tint, icon_depth);

			pos.x += 16 * scale;
			draw_fg_image_with_idx(WINDOW_LAYER_TEXTS, ICONS_TEXTURE, cga_icon,
								   pos, size, tint, icon_depth);
		}
	};
};

auto Sorcery::UI::draw_cursor() -> void {

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
		// list (we don't use draw_fg_image_with_idx() since that draws to
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
auto Sorcery::UI::draw_frame(Component *component) -> void {

	// Note the Frame class calls Gui::->draw_frame() below
	auto frame{std::make_shared<Frame>(_ctx, component)};
	_frames.emplace_back(std::move(frame));
}

// Draw a Menu
auto Sorcery::UI::draw_menu(Component *component) -> void {

	auto menu{std::make_shared<Menu>(_ctx, component, _ctx.game)};
	menu->regenerate();
	menu->draw();
	_menus.emplace_back(std::move(menu));
}

auto Sorcery::UI::draw_debug() -> void {

	if (!_ctx.controller->get_flag("debug_ui"))
		return;

	with_Window(WINDOW_LAYER_MENUS, nullptr,
				ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs) {

		set_Font(_io->FontDefault, metrics->font_sz());
		ImGui::SetCursorPos(ImVec2{8, 8});
		set_StyleColor(ImGuiCol_Text, ImVec4{1.0f, 0.0f, 0.0f, 1.0f});
		ImGui::TextUnformatted(_ctx.controller->get_flags().c_str());

		ImGui::SetCursorPos(ImVec2{8, 700});
		ImGui::TextUnformatted(_ctx.controller->get_characters().c_str());

		ImGui::SetCursorPos(ImVec2{1000, 8});
		auto font_list{fonts->get_all_fonts()};
		for (const auto &font : font_list) {
			ImGui::TextUnformatted(font.name.c_str());
			ImGui::SetCursorPosX(1000);
		}
	}
}

// Draw a Paragraph (Wrapped Multiline Text)
auto Sorcery::UI::draw_paragraph(Component *component) -> void {

	with_Window(WINDOW_LAYER_TEXTS, nullptr,
				ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs) {

		set_Font(fonts->get_current_font(component->font).value(),
				 metrics->font_sz());
		const auto wrap{component->get_float("width") * metrics->font_sz()};
		auto p_min{metrics->grid_pos(component->x, component->y)};

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

		set_Font(fonts->get_current_font(font).value(), metrics->font_sz());

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

	set_Font(fonts->get_current_font(font).value(), metrics->font_sz());

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

auto Sorcery::UI::draw_button_click(Component *component, bool &flag,
									const bool reverse) -> void {

	// Need to push font first before calculating size else it will
	// assume monospace font size!
	set_Font(fonts->get_current_font(component->font).value(),
			 metrics->font_sz());
	const auto name{component->name};
	const auto col{get_hl_colour(_ctx.animation->lerp)};
	auto x{std::invoke([&] {
		if (component->x == -1) {
			const auto viewport{ImGui::GetMainViewport()};
			const auto width{ImGui::CalcTextSize(
				CSTR(_ctx.get_string(component->string_key)))};
			return (viewport->Size.x - width.x) / 2;
		} else
			return metrics->grid_pos(component->x, component->y).x;
	})};
	auto y{std::invoke([&] {
		if (component->y == -1) {
			const auto viewport{ImGui::GetMainViewport()};
			const auto height{ImGui::CalcTextSize(
				CSTR(_ctx.get_string(component->string_key)))};
			return (viewport->Size.y - height.y) / 2;
		} else
			return metrics->grid_pos(component->x, component->y).y;
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
			_ctx.controller->actions->button(component->name, -1);
		}
	}
}

// Draw a Button
auto Sorcery::UI::draw_button(Component *component,
							  std::optional<bool *> is_clicked) -> void {

	with_Window(WINDOW_LAYER_MENUS, nullptr, ImGuiWindowFlags_NoDecoration) {

		// Need to push font first before calculating size else it will
		// assume monospace font size!
		set_Font(fonts->get_current_font(component->font).value(),
				 metrics->font_sz());
		const auto name{component->name};
		const auto col{get_hl_colour(_ctx.animation->lerp)};
		auto x{std::invoke([&] {
			if (component->x == -1) {
				const auto viewport{ImGui::GetMainViewport()};
				const auto width{ImGui::CalcTextSize(
					CSTR(_ctx.get_string(component->string_key)))};
				return (viewport->Size.x - width.x) / 2;
			} else
				return metrics->grid_pos(component->x, component->y).x;
		})};
		auto y{std::invoke([&] {
			if (component->y == -1) {
				const auto viewport{ImGui::GetMainViewport()};
				const auto height{ImGui::CalcTextSize(
					CSTR(_ctx.get_string(component->string_key)))};
				return (viewport->Size.y - height.y) / 2;
			} else
				return metrics->grid_pos(component->x, component->y).y;
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

				_ctx.controller->actions->button(component->name, -1);

				if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNone)) {
				}
			}
		}
	}
}

auto Sorcery::UI::draw_character_detailed(Component *component,
										  const Character *character) -> void {

	const auto left_col{component->x + 0};
	const auto right_col{component->x + 19};

	using enum Enums::Character::Ability;
	using enum Enums::Character::Attribute;
	UIStyle::set_text_bright(_ctx);
	auto pos{metrics->grid_pos(left_col, component->y)};
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(std::format("{:>14} {:>2}", "Strength",
									   character->get_cur_attr(STRENGTH))
							   .c_str());

	UIStyle::set_text_dark(_ctx);
	pos.y += metrics->grid_delta(0, 1).y;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:+>2}", "Atk Mod",
					character->abilities().at(ATTACK_MODIFIER))
			.c_str());
	pos.y += metrics->grid_delta(0, 1).y;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:+>2}", "Hit Prob",
					character->abilities().at(HIT_PROBABILITY))
			.c_str());
	pos.y += metrics->grid_delta(0, 1).y;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(std::format("{:>14} {:+>2}", "Bonus Damg",
									   character->abilities().at(BONUS_DAMAGE))
							   .c_str());
	pos.y += metrics->grid_delta(0, 1).y;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}", "Num Attacks",
					character->abilities().at(BASE_NUMBER_OF_ATTACKS))
			.c_str());
	pos.y += metrics->grid_delta(0, 1).y;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}", "Unarmed Damg",
					character->abilities().at(UNARMED_DAMAGE))
			.c_str());

	UIStyle::set_text_bright(_ctx);
	pos.y += metrics->grid_delta(0, 1).y;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(std::format("{:>14} {:>2}", "Vitality",
									   character->get_cur_attr(VITALITY))
							   .c_str());

	UIStyle::set_text_dark(_ctx);
	pos.y += metrics->grid_delta(0, 1).y;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:+>2}", "Vit Bonus",
					character->abilities().at(VITALITY_BONUS))
			.c_str());
	pos.y += metrics->grid_delta(0, 1).y;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:+>2}", "Bonus HP",
					character->abilities().at(BONUS_HIT_POINTS))
			.c_str());
	pos.y += metrics->grid_delta(0, 1).y;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}%", "Ress / Dead",
					character->abilities().at(DEAD_RESURRECT))
			.c_str());
	pos.y += metrics->grid_delta(0, 1).y;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}%", "Ress / Ashes",
					character->abilities().at(ASHES_RESURRECT))
			.c_str());
	pos.y += metrics->grid_delta(0, 1).y;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}%", "Ress / Spell",
					character->abilities().at(DI_KADORTO_RESURRECT))
			.c_str());

	pos = metrics->grid_pos(right_col, component->y);
	ImGui::SetCursorPos(pos);
	UIStyle::set_text_bright(_ctx);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}", "I.Q.", character->get_cur_attr(IQ))
			.c_str());

	UIStyle::set_text_dark(_ctx);
	pos.y += metrics->grid_delta(0, 1).y;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}%", "Spell Learn",
					character->abilities().at(MAGE_SPELL_LEARN))
			.c_str());
	pos.y += metrics->grid_delta(0, 1).y;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}%", "ID Items",
					character->abilities().at(IDENTIFY_ITEMS))
			.c_str());
	pos.y += metrics->grid_delta(0, 1).y;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}%", "ID Curse",
					character->abilities().at(IDENTIFY_CURSE))
			.c_str());
	pos.y += metrics->grid_delta(0, 1).y;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(std::format("{:>14} {:>2}%", "ID Foes",
									   character->abilities().at(IDENTIFY_FOES))
							   .c_str());

	UIStyle::set_text_bright(_ctx);
	pos.y += metrics->grid_delta(0, 2).y;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}", "Agility", character->get_cur_attr(AGILITY))
			.c_str());

	UIStyle::set_text_dark(_ctx);
	pos.y += metrics->grid_delta(0, 1).y;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:+>2}", "Int Mod",
					character->abilities().at(INITIATIVE_MODIFIER))
			.c_str());
	pos.y += metrics->grid_delta(0, 1).y;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}%", "Crit Hit",
					character->abilities().at(BASE_CRITICAL_HIT))
			.c_str());
	pos.y += metrics->grid_delta(0, 1).y;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(std::format("{:>14} {:>2}%", "ID Trap",
									   character->abilities().at(IDENTIFY_TRAP))
							   .c_str());
	pos.y += metrics->grid_delta(0, 1).y;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}%", "Disarm Trap",
					character->abilities().at(BASE_DISARM_TRAP))
			.c_str());
	pos.y += metrics->grid_delta(0, 1).y;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}%", "Avoid Trap",
					100 - character->abilities().at(ACTIVATE_TRAP))
			.c_str());
	pos.y += metrics->grid_delta(0, 1).y;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}%", "Avoid Pit",
					character->abilities().at(BASE_AVOID_PIT))
			.c_str());
}

auto Sorcery::UI::draw_character_mage_spells(Component *component,
											 const Character *character)
	-> void {

	ImVec2 pos{metrics->grid_pos(component->x, component->y)};
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

auto Sorcery::UI::draw_character_priest_spells(Component *component,
											   const Character *character)
	-> void {

	auto pos{metrics->grid_pos(component->x, component->y)};
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

auto Sorcery::UI::draw_character_detailed_again(Component *component,
												const Character *character)
	-> void {

	const auto left_col{component->x + 0};
	const auto right_col{component->x + 19};

	using enum Enums::Character::Ability;
	using enum Enums::Character::Attribute;
	UIStyle::set_text_bright(_ctx);
	auto pos{metrics->grid_pos(left_col, component->y)};
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}", "Piety", character->get_cur_attr(PIETY))
			.c_str());

	UIStyle::set_text_dark(_ctx);
	pos.y += metrics->grid_delta(0, 1).y;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}%", "Spell Learn",
					character->abilities().at(PRIEST_SPELL_LEARN))
			.c_str());
	pos.y += metrics->grid_delta(0, 1).y;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}%", "Rec Chance",
					character->abilities().at(LOKTOFELT_SUCCESS))
			.c_str());
	pos.y += metrics->grid_delta(0, 1).y;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(std::format("{:>14} {:>2}%", "Base Dispell",
									   character->abilities().at(BASE_DISPELL))
							   .c_str());

	UIStyle::set_text_bright(_ctx);
	pos.y += metrics->grid_delta(0, 2).y;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}", "Luck", character->get_cur_attr(LUCK))
			.c_str());
	UIStyle::set_text_dark(_ctx);
	pos.y += metrics->grid_delta(0, 1).y;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}%", "Res Bonus",
					character->abilities().at(BASE_RESIST_BONUS))
			.c_str());
	pos.y += metrics->grid_delta(0, 1).y;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}%", "Wipe Rec",
					character->abilities().at(EQUIPMENT_INTACT_ON_WIPE))
			.c_str());

	pos = metrics->grid_pos(right_col, component->y);
	ImGui::SetCursorPos(pos);
	UIStyle::set_text_bright(_ctx);
	ImGui::TextUnformatted(std::format("{:>14}", "Resistances").c_str());

	UIStyle::set_text_dark(_ctx);
	pos.y += metrics->grid_delta(0, 1).y;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}%", "vs Crit Hit",
					character->abilities().at(RESISTANCE_VS_CRITICAL_HIT) * 5)
			.c_str());
	pos.y += metrics->grid_delta(0, 1).y;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}%", "vs Pois / Para",
					character->abilities().at(RESISTANCE_VS_POISON_PARALYSIS) *
						5)
			.c_str());
	pos.y += metrics->grid_delta(0, 1).y;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}%", "vs Stoning",
					character->abilities().at(RESISTANCE_VS_STONING) * 5)
			.c_str());
	pos.y += metrics->grid_delta(0, 1).y;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}%", "vs Breath",
					character->abilities().at(RESISTANCE_VS_BREATH_ATTACKS) * 5)
			.c_str());
	pos.y += metrics->grid_delta(0, 1).y;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}%", "vs Gas Trap",
					character->abilities().at(RESISTANCE_VS_POISON_GAS_TRAP) *
						5)
			.c_str());
	pos.y += metrics->grid_delta(0, 1).y;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}%", "vs Spell Trap",
					character->abilities().at(RESISTANCE_VS_MAGE_PRIEST_TRAP) *
						5)
			.c_str());
	pos.y += metrics->grid_delta(0, 1).y;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}%", "vs Silence",
					character->abilities().at(RESISTANCE_VS_SILENCE) * 5)
			.c_str());
	pos.y += metrics->grid_delta(0, 1).y;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}%", "vs Sleep",
					character->abilities().at(RESISTANCE_VS_KATINO))
			.c_str());
	pos.y += metrics->grid_delta(0, 1).y;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}%", "vs Death",
					character->abilities().at(RESISTANCE_VS_BADI))
			.c_str());
	pos.y += metrics->grid_delta(0, 1).y;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}%", "vs Statue",
					character->abilities().at(RESISTANCE_VS_MANIFO))
			.c_str());

	pos.y += metrics->grid_delta(0, 1).y;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}%", "Rec / Sleep",
					character->abilities().at(RECOVER_FROM_SLEEP) * 5)
			.c_str());
	pos.y += metrics->grid_delta(0, 1).y;
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>14} {:>2}%", "Rec / Fear",
					character->abilities().at(RECOVER_FROM_FEAR) * 5)
			.c_str());
}

auto Sorcery::UI::draw_character_summary(Component *component,
										 const Character *character) -> void {

	const auto left_col{component->x};
	const auto middle_col{component->x + 13};
	const auto right_col{component->x + 31};

	using enum Enums::Character::Ability;
	using enum Enums::Character::Attribute;
	auto pos{metrics->grid_pos(left_col, component->y)};
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(std::format("{:>8} {:>2}", "Strength",
									   character->get_cur_attr(STRENGTH))
							   .c_str());
	pos = metrics->grid_pos(left_col, component->y + 1);
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>8} {:>2}", "I.Q.", character->get_cur_attr(IQ))
			.c_str());
	pos = metrics->grid_pos(left_col, component->y + 2);
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>8} {:>2}", "Piety", character->get_cur_attr(PIETY))
			.c_str());
	pos = metrics->grid_pos(left_col, component->y + 3);
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(std::format("{:>8} {:>2}", "Vitality",
									   character->get_cur_attr(VITALITY))
							   .c_str());
	pos = metrics->grid_pos(left_col, component->y + 4);
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>8} {:>2}", "Agility", character->get_cur_attr(AGILITY))
			.c_str());
	pos = metrics->grid_pos(left_col, component->y + 5);
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:>8} {:>2}", "Luck", character->get_cur_attr(LUCK))
			.c_str());

	pos = metrics->grid_pos(left_col, component->y + 7);
	ImGui::SetCursorPos(pos);
	const auto &sp{character->magic().mage_current_spellpoints()};
	auto mage{std::format("{}/{}/{}/{}/{}/{}/{}", sp.at(1), sp.at(2), sp.at(3),
						  sp.at(4), sp.at(5), sp.at(6), sp.at(7))};
	ImGui::TextUnformatted(std::format("Mage {}", mage).c_str());

	pos = metrics->grid_pos(middle_col, component->y);
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:<6} {:>10}", "Gold", character->get_gold()).c_str());
	pos = metrics->grid_pos(middle_col, component->y + 1);
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:<6} {:>10}", "E.P.", character->get_cur_xp()).c_str());
	pos = metrics->grid_pos(middle_col, component->y + 2);
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:<6} {:>10}", "Next", character->get_next_xp()).c_str());
	pos = metrics->grid_pos(middle_col, component->y + 3);
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:<6} {:>10}", "Marks", character->abilities().at(MARKS))
			.c_str());
	pos = metrics->grid_pos(middle_col, component->y + 4);
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(std::format("{:<4} {}/{}", "H.P.",
									   character->abilities().at(CURRENT_HP),
									   character->abilities().at(MAX_HP))
							   .c_str());
	pos = metrics->grid_pos(middle_col, component->y + 5);
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("{:<6} {:>18}", "Status", character->get_status_string())
			.c_str());

	const auto &sp2{character->magic().priest_current_spellpoints()};
	auto priest{std::format("{}/{}/{}/{}/{}/{}/{}", sp2.at(1), sp2.at(2),
							sp2.at(3), sp2.at(4), sp2.at(5), sp2.at(6),
							sp2.at(7))};

	pos = metrics->grid_pos(component->x + 20, component->y + 7);
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(std::format("Prst {}", priest).c_str());

	pos = metrics->grid_pos(right_col, component->y);
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format("Swim{:>3}", character->abilities().at(SWIM)).c_str());
	pos = metrics->grid_pos(right_col, component->y + 1);
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format(" Age{:>3}", character->abilities().at(AGE) / 52).c_str());
	pos = metrics->grid_pos(right_col, component->y + 2);
	ImGui::SetCursorPos(pos);
	ImGui::TextUnformatted(
		std::format(" RIP{:>3}", character->abilities().at(DEATHS)).c_str());

	auto slot{1u};
	pos = metrics->grid_pos(left_col, component->y + 9);
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
			pos = metrics->grid_pos(left_col, component->y + 9 + slot / 2);
		else
			pos = metrics->grid_pos(component->x + 17,
									component->y + 9 + (slot - 1) / 2);
		ImGui::SetCursorPos(pos);
		auto line{std::format("{}){}{}", slot, flag, item.get_display_name())};
		ImGui::TextUnformatted(line.c_str());
		++slot;
	}
}

auto Sorcery::UI::draw_pay_info() -> void {

	// Work out healing cost
	const auto character{_ctx.game->characters.at(
		_ctx.controller->get_character(Enums::CharacterSlot::HELP))};
	const auto cost(character.get_cure_cost());
	const auto cost_text{std::format("{} {} {}",
									 _ctx.get_string("PAY_COST_PREFIX"), cost,
									 _ctx.get_string("PAY_COST_SUFFIX"))};
	auto cmp{components->get("pay:pay_cost")};
	draw_text(&cmp, cost_text);
}

auto Sorcery::UI::draw_current_character([[maybe_unused]] const int mode)
	-> void {

	auto character{_ctx.game->characters.at(
		_ctx.controller->get_character(Enums::CharacterSlot::INSPECT))};

	auto title{components->get("inspect:character_title")};
	draw_text(&title, character.summary_text_with_awards());

	with_Window(WINDOW_LAYER_MENUS, nullptr, ImGuiWindowFlags_NoTitleBar) {
		auto prev{components->get("inspect:character_previous")};
		draw_button_click(&prev,
						  _ctx.get_flag_ref("select_previous_character"));
		auto next{components->get("inspect:character_next")};
		draw_button_click(&next, _ctx.get_flag_ref("select_next_character"));

		auto cmp{components->get("inspect:character_data")};
		auto pos{metrics->grid_pos(cmp.x, cmp.y)};

		ImGuiTabBarFlags tb_flags{ImGuiTabBarFlags_None};
		ImGui::SetCursorPos(pos);
		with_Child(
			"character_tab_bar_child",
			ImVec2(metrics->grid_sz() * cmp.w, metrics->grid_sz() * cmp.h)) {
			UIStyle::set_tab_black(_ctx);
			auto char_cmp{components->get("inspect:character_tab_data")};
			set_Font(fonts->get_current_font(cmp.font).value(),
					 metrics->font_sz());
			with_TabBar("character_tab_bar", tb_flags) {
				with_TabItem("Info") {
					draw_character_summary(&char_cmp, &character);
				}
				with_TabItem("Stats##1") {
					draw_character_detailed(&char_cmp, &character);
				}
				with_TabItem("Stats##2") {
					draw_character_detailed_again(&char_cmp, &character);
				}
				with_TabItem("Arcane") {
					draw_character_mage_spells(&char_cmp, &character);
				}
				with_TabItem("Divine") {
					draw_character_priest_spells(&char_cmp, &character);
				}
			}
		}
	}
}

auto Sorcery::UI::draw_stepper(Component *component, const std::string &name,
							   int &value) -> void {

	bool disabled{false};

	with_Window(WINDOW_LAYER_MENUS, nullptr, ImGuiWindowFlags_NoTitleBar) {

		auto pos{metrics->grid_pos(component->x, component->y)};
		ImGui::SetCursorPos(pos);

		set_Font(fonts->get_current_font(component->font).value(),
				 metrics->font_sz());

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
				disabled = !(value > mins.at(STRENGTH));
			else if (name == "stepper_attribute_2")
				disabled = !(value > mins.at(IQ));
			else if (name == "stepper_attribute_3")
				disabled = !(value > mins.at(PIETY));
			else if (name == "stepper_attribute_4")
				disabled = !(value > mins.at(VITALITY));
			else if (name == "stepper_attribute_5")
				disabled = !(value > mins.at(AGILITY));
			else if (name == "stepper_attribute_6")
				disabled = !(value > mins.at(LUCK));
		};

		with_ID(stepper_minus.c_str()) {

			if (disabled)
				ImGui::BeginDisabled();
			if (ImGui::Button("<")) {
				_ctx.controller->actions->stepper(stepper_plus, false, value);
			}
			if (disabled)
				ImGui::EndDisabled();
		}

		pos.x += metrics->grid_delta(1, 0).x;

		ImVec4 alpha_col{ImGui::ColorConvertU32ToFloat4(component->colour)};
		alpha_col.w = _ctx.animation->fade;

		set_StyleColor(ImGuiCol_Text, alpha_col);
		ImGui::SetCursorPos(pos);
		ImGui::TextUnformatted(std::format("{:>2}", value).c_str());

		disabled = false;
		if (component->name == "current_stats") {
			if ((value >= 18) || (_ctx.controller->get_candidate_character()
									  ->create()
									  .get_points_left() == 0))
				disabled = true;
		};

		pos.x += metrics->grid_delta(2, 0).x;
		ImGui::SetCursorPos(pos);
		with_ID(stepper_plus.c_str()) {
			if (disabled)
				ImGui::BeginDisabled();
			if (ImGui::Button(">")) {
				_ctx.controller->actions->stepper(stepper_minus, true, value);
			}
			if (disabled)
				ImGui::EndDisabled();
		}
	}
}

auto Sorcery::UI::draw_input(Component &component, std::string &input,
							 const ImGuiInputTextFlags input_flags) -> bool {

	with_Window(WINDOW_LAYER_MENUS, nullptr, ImGuiWindowFlags_NoTitleBar) {

		const auto pos{metrics->grid_pos(component.x, component.y)};

		ImGui::SetCursorPos(pos);

		set_Font(fonts->get_current_font(component.font).value(),
				 metrics->font_sz());

		const auto flags{input_flags | ImGuiInputTextFlags_AutoSelectAll |
						 ImGuiInputTextFlags_EnterReturnsTrue};

		const auto input_name{std::format("##{}", component.name)};

		const auto button_name{std::format("##{}_ok", component.name)};

		ImGui::SetNextItemWidth(ImGui::GetFontSize() * component.w);

		auto submitted{ImGui::InputText(input_name.c_str(), &input, flags)};

		ImGui::SameLine();

		const auto col{get_hl_colour(_ctx.animation->lerp)};

		UIStyle::set_faded(_ctx);

		set_StyleColor(ImGuiCol_ButtonHovered, ImVec4{col});

		with_ID(button_name.c_str()) {

			if (ImGui::Button(">"))
				submitted = true;
		}

		return submitted;
	}

	return false;
}

auto Sorcery::UI::draw_text(Component *component, const std::string &string)
	-> void {

	with_Window(WINDOW_LAYER_TEXTS, nullptr,
				ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs) {

		// Need to push font first before calculating size else it will
		// assume monospace font size!
		set_Font(fonts->get_current_font(component->font).value(),
				 metrics->font_sz());

		const auto x{std::invoke([&] {
			if (component->x == -1) {
				const auto viewport{ImGui::GetMainViewport()};
				const auto width{ImGui::CalcTextSize(CSTR(string))};
				return (viewport->Size.x - width.x) / 2;
			} else
				return metrics->grid_pos(component->x, component->y).x;
		})};
		const auto y{std::invoke([&] {
			if (component->y == -1) {
				const auto viewport{ImGui::GetMainViewport()};
				const auto height{ImGui::CalcTextSize(CSTR(string))};
				return (viewport->Size.y - height.y) / 2;
			} else
				return metrics->grid_pos(component->x, component->y).y;
		})};

		// Adjust Alpha of Text
		ImVec4 alpha_col{ImGui::ColorConvertU32ToFloat4(component->colour)};
		alpha_col.w = _ctx.animation->fade;

		set_StyleColor(ImGuiCol_Text, alpha_col);
		ImGui::SetCursorPos(ImVec2{x, y});
		ImGui::TextUnformatted(string.c_str());
	}
}

auto Sorcery::UI::draw_party_wipe() -> void {

	const auto grave_cmp{components->get("graveyard:gravestone")};
	const auto text_cmp{components->get("graveyard:party_members")};

	constexpr auto max_cols{3};

	const auto grave_idx{GRAVESTONE_GFX_ID};
	const auto scale{_ctx.display->get_display_metrics().scale};
	const auto grave_w{grave_cmp.get_float("tile_width") * scale};
	const auto grave_h{grave_cmp.get_float("tile_height") * scale};

	const auto gap{
		metrics->grid_delta(grave_cmp.get_float("spacing_x") * scale,
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

	const auto origin_x{metrics->grid_x(grave_cmp.x)};

	// The component's Y coordinate represents the vertical centre of the
	// complete gravestone arrangement.
	const auto centre_y{metrics->grid_y(grave_cmp.y)};
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

			draw_fg_image_with_idx(EVENTS_TEXTURE, grave_idx, grave_pos,
								   ImVec2{grave_w, grave_h});

			const auto &name{names.at(index)};

			set_Font(fonts->get_current_font(text_cmp.font).value(),
					 metrics->font_sz());

			const auto text_size{ImGui::CalcTextSize(name.c_str())};

			const ImVec2 text_pos{
				grave_pos.x + ((grave_w - text_size.x) * 0.5f),
				grave_pos.y + grave_h - metrics->grid_delta(0, 1).y};

			draw_text_with_layer(name, text_cmp.colour, text_pos,
								 text_cmp.font);
		}
	}
}

auto Sorcery::UI::draw_automap_legend(Component *component) -> void {

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

	auto pos{metrics->grid_pos(component->x, component->y)};

	with_Window(WINDOW_LAYER_MENUS, nullptr, ImGuiWindowFlags_NoDecoration) {

		set_Font(fonts->get_current_font(component->font).value(),
				 metrics->font_sz());

		auto cmp_level{components->get("automap:automap_level")};
		draw_text(&cmp_level, _ctx.game->state->level->name());

		for (const auto &item : legend) {
			draw_fg_image_with_idx(WINDOW_LAYER_MENUS, MAPS_TEXTURE,
								   std::to_underlying(item.feature), pos,
								   ImVec2{static_cast<float>(icon_size),
										  static_cast<float>(icon_size)});

			const auto delta{metrics->grid_delta(1.0f, 0.0f)};
			ImGui::SetCursorPos(ImVec2{pos.x + icon_size + delta.x, pos.y});

			ImGui::TextUnformatted(item.label.data());

			pos.y += icon_size + row_gap;
		}
	}

	with_Window(WINDOW_LAYER_MENUS, nullptr, ImGuiWindowFlags_NoTitleBar) {
		auto leave{components->get("automap:automap_return")};
		draw_button_click(&leave, _ctx.get_flag_ref("show_automap"), true);
	}
}

// Draw a Text (String)
auto Sorcery::UI::draw_text(Component *component) -> void {
	with_Window(WINDOW_LAYER_TEXTS, nullptr,
				ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs) {

		// Need to push font first before calculating size else it will
		// assume monospace font size!
		set_Font(fonts->get_current_font(component->font).value(),
				 metrics->font_sz());

		const auto x{std::invoke([&] {
			if (component->x == -1) {
				const auto viewport{ImGui::GetMainViewport()};
				const auto width{ImGui::CalcTextSize(
					CSTR(_ctx.get_string(component->string_key)))};
				return (viewport->Size.x - width.x) / 2;
			} else
				return metrics->grid_pos(component->x, component->y).x;
		})};
		const auto y{std::invoke([&] {
			if (component->y == -1) {
				const auto viewport{ImGui::GetMainViewport()};
				const auto height{ImGui::CalcTextSize(
					CSTR(_ctx.get_string(component->string_key)))};
				return (viewport->Size.y - height.y) / 2;
			} else
				return metrics->grid_pos(component->x, component->y).y;
		})};

		// Adjust Alpha of Text
		ImVec4 alpha_col{ImGui::ColorConvertU32ToFloat4(component->colour)};
		alpha_col.w = _ctx.animation->fade;

		set_StyleColor(ImGuiCol_Text, alpha_col);
		ImGui::SetCursorPos(ImVec2{x, y});
		ImGui::TextUnformatted(_ctx.get_string(component->string_key).c_str());
	}
}

auto Sorcery::UI::draw_components(std::string_view screen,
								  [[maybe_unused]] const int mode) -> void {

	_frames.clear();
	_menus.clear();

	// Draw every component specified in order
	for (auto cmps{(*components)(screen)}; auto c : cmps.value()) {
		using enum Enums::Layout::ComponentType;
		if (c.type == IMAGE_BG)
			draw_tiled_bg(&c);
		else if (c.type == FRAME)
			draw_frame(&c);
		else if (c.type == IMAGE_FG)
			draw_fg_image(&c);
		else if (c.type == TEXT)
			draw_text(&c);
		else if (c.type == BUTTON)
			draw_button(&c);
		else if (c.type == PARAGRAPH)
			draw_paragraph(&c);
		else if (c.type == MENU)
			draw_menu(&c);
	}
}

auto Sorcery::UI::draw_item_info() -> void {
	// Custom Rendering
	const auto idx{_ctx.get_selected("museum_selected")};
	if (idx >= 100)
		return;

	const auto item{_ctx.resources->items->get(idx + 1)};
	auto item_c{components->get("museum:item_graphic")};
	auto item_pos{metrics->grid_pos(item_c.x, item_c.y)};
	const auto scale{_ctx.display->get_display_metrics().scale};
	draw_fg_image_with_idx(ITEMS_TEXTURE, idx, item_pos,
						   ImVec2{item_c.get_float("tile_width") * scale,
								  item_c.get_float("tile_width") * scale});

	auto cmp{components->get("museum:item_data")};
	auto pos{metrics->grid_pos(cmp.x, cmp.y)};

	with_Window(WINDOW_LAYER_MENUS, nullptr, ImGuiWindowFlags_NoDecoration) {

		const auto name{std::format("  {:>03}:{}/{}", idx + 1,
									item.get_known_name(),
									item.get_unknown_name())};

		ImGuiTabBarFlags tb_flags{ImGuiTabBarFlags_None};
		ImGui::SetCursorPos(pos);
		with_Child("museum_tab_bar_child", ImVec2(metrics->grid_sz() * cmp.w,
												  metrics->grid_sz() * cmp.h)) {
			UIStyle::set_tab_black(_ctx);
			set_Font(fonts->get_current_font(cmp.font).value(),
					 metrics->font_sz());
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

auto Sorcery::UI::draw_options() -> void {
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
	set_Font(fonts->get_current_font(component.font).value(),
			 metrics->font_sz());
	const auto col{get_hl_colour(_ctx.animation->lerp)};
	with_Window(WINDOW_LAYER_MENUS, nullptr, ImGuiWindowFlags_NoDecoration) {

		// To adjust for Window Resizing etc
		const auto x{std::invoke([&] {
			const auto width{metrics->grid_sz() *
							 component.get_float("grid_width")};
			const auto viewport{ImGui::GetMainViewport()};
			return (viewport->Size.x - width) / 2;
		})};

		const auto pos{ImVec2{x, metrics->grid_y(component.y)}};
		ImGui::SetCursorPos(pos);

		// Now draw tab bar
		UIStyle::set_faded(_ctx);
		set_StyleColor(ImGuiCol_ButtonHovered, (ImVec4)col);
		const auto tabs_width{component.w * metrics->grid_sz()};
		const auto tabs_height{component.h * metrics->grid_sz()};
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
								_ctx.controller->actions->toggle(
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
								_ctx.controller->actions->toggle(
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
								_ctx.controller->actions->toggle(
									component.name, tabname, graphics_idx);
							};
						}
						++graphics_idx;
					}

					// Font Selection dropdown
					ImGui::Separator();
					ImGui::SetCursorPosY(ImGui::GetCursorPosY() +
										 metrics->grid_sz());
					const auto item_height{
						ImGui::GetTextLineHeightWithSpacing()};
					const auto max_visible_items{10};
					ImGui::SetNextWindowSize(
						ImVec2(0, item_height * max_visible_items));
					with_Combo("##font_combobox", "Chooose Font...") {

						auto font_list{fonts->get_all_monospace_fonts()};
						auto font_idx{0u};
						for (const auto &font : font_list) {
							const bool is_selected{
								font.name ==
								fonts->get_current_monospace_font_name()};
							set_Font(font.font, metrics->font_sz());
							auto selectable_name{
								std::format("{}##{}", font.name, font_idx)};
							if (ImGui::Selectable(selectable_name.c_str(),
												  is_selected)) {
								fonts->set_current_font(
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
					ImGui::SetNextItemWidth(28.f);
					ImGui::ColorEdit3(frame_name.c_str(), (float *)&ui_colour,
									  flags);
				};
			}
			set_Font(fonts->get_current_font(component.font).value());

			// Save and Cancel Buttons
			const auto centre{(tabs_width / 2)};
			const auto button_y{std::stoi(component.get("button_y").value())};
			ImVec2 btn_size{ImGui::GetFontSize() * 7.0f, 0.0f};

			UIStyle::set_faded(_ctx);
			set_StyleColor(ImGuiCol_ButtonHovered, ImVec4{col});
			set_StyleColor(ImGuiCol_ButtonActive, (ImVec4)col);

			ImGui::SetCursorPos(
				ImVec2{centre - (btn_size.x + metrics->grid_sz()),
					   button_y * metrics->grid_sz()});
			if (ImGui::Button(save_lbl.c_str(), btn_size)) {
				_ctx.system->config->save();

				if (_ctx.get_flag("in_engine"))
					_ctx.controller->go_to(Enums::Screen::ENGINE);
				else
					_ctx.controller->go_to(Enums::Screen::MAINMENU);

				//_ctx.controller->unset_flag("show_options");
			}
			ImGui::SetCursorPos(ImVec2{centre + metrics->grid_sz(),
									   button_y * metrics->grid_sz()});
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

auto Sorcery::UI::draw_level_name() -> void {

	auto text_cmp{components->get("engine_base_ui:level_name")};
	auto frame_cmp{components->get("engine_base_ui:level_name_frame")};

	with_Window(WINDOW_LAYER_TEXTS, nullptr, ImGuiWindowFlags_NoDecoration) {

		draw_frame(&frame_cmp);
		draw_text(&text_cmp, _ctx.game->state->level->name());
	}
}

auto Sorcery::UI::draw_buffbar() -> void {
	auto cmp{components->get("engine_base_ui:buffbar")};
	auto frame_cmp{components->get("engine_base_ui:buffbar_frame")};

	const auto x{metrics->grid_x(cmp.x)};
	auto y{metrics->grid_y(cmp.y)};
	const auto scale{_ctx.display->get_display_metrics().scale};
	const auto width{cmp.w * metrics->grid_sz() * scale};
	const auto height{cmp.h * metrics->grid_sz()};

	// Get Icon Effects
	const auto icon_depth{UIStyle::icon_depth(scale)};

	const auto light_idx{_ctx.game->state->get_lit() ? ICON_BUFF_EXTRA_LIGHT
													 : ICON_BUFF_LIGHT};

	const auto tint{_ctx.controller->get_monochrome()
						? ImVec4{1.0f, 1.0f, 1.0f, 1.0f}
						: UIStyle::icon_colour(light_idx)};

	with_Window(WINDOW_LAYER_TEXTS, nullptr, ImGuiWindowFlags_NoDecoration) {

		draw_frame(&frame_cmp);
		ImGui::SetCursorPos(ImVec2{x, y});

		draw_fg_image_with_idx(WINDOW_LAYER_TEXTS, ICONS_TEXTURE, light_idx,
							   ImVec2{x, y}, ImVec2{width, height}, tint,
							   icon_depth);

		y += height;

		// TODO
	}
}

auto Sorcery::UI::draw_icons() -> void {

	auto cmp{components->get("engine_base_ui:icons")};
	auto frame_cmp{components->get("engine_base_ui:icons_frame")};

	constexpr std::array icons{
		ICON_CAMP, ICON_PARTY, ICON_MAP, ICON_LOOK, ICON_CAST, ICON_USE,
	};

	const auto x{metrics->grid_x(cmp.x)};
	const auto start_y{metrics->grid_y(cmp.y)};

	const auto scale{_ctx.display->get_display_metrics().scale};
	const auto width{static_cast<float>(cmp.w * metrics->grid_sz()) * scale};
	const auto height{static_cast<float>(cmp.h * metrics->grid_sz()) * scale};

	const ImVec2 icon_size{width, height};

	const auto hovered_tint{ImVec4{get_hl_colour(_ctx.animation->lerp)}};

	// Passive frame.
	with_Window(WINDOW_LAYER_TEXTS, nullptr,
				ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs) {

		draw_frame(&frame_cmp);
	}

	// Get Icon Effects
	const auto icon_depth{UIStyle::icon_depth(scale)};

	// Interactive icons.
	with_Window(WINDOW_LAYER_MENUS, nullptr,
				ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoTitleBar) {

		auto y{start_y};

		for (const auto icon_idx : icons) {
			const ImVec2 icon_pos{x, y};

			ImGui::SetCursorPos(icon_pos);
			ImGui::PushID(icon_idx);

			const auto activated{ImGui::InvisibleButton("##icon", icon_size)};

			const auto colour{UIStyle::icon_colour(icon_idx)};

			const auto normal_tint{
				_ctx.controller->get_monochrome()
					? ImVec4{1.0f, 1.0f, 1.0f, _ctx.animation->fade}
					: ImVec4{colour.x, colour.y, colour.z,
							 _ctx.animation->fade}};

			const auto hovered{ImGui::IsItemHovered()};
			const auto tint{hovered ? hovered_tint : normal_tint};

			draw_fg_image_with_idx(WINDOW_LAYER_MENUS, ICONS_TEXTURE, icon_idx,
								   icon_pos, icon_size, tint, icon_depth);

			if (activated)
				_ctx.controller->actions->icon(icon_idx);

			ImGui::PopID();

			y += height + 3.0f;
		}
	}
}

auto Sorcery::UI::draw_save() -> void {

	auto cmp{components->get("engine_base_ui:save")};
	auto frame_cmp{components->get("engine_base_ui:save_frame")};

	const auto x{metrics->grid_x(cmp.x)};
	const auto y{metrics->grid_y(cmp.y)};

	const auto width{static_cast<float>(cmp.w * metrics->grid_sz())};
	const auto height{static_cast<float>(cmp.h * metrics->grid_sz())};

	const ImVec2 save_pos{x, y};
	const ImVec2 save_size{width, height};

	// Get Icon Effects
	const auto scale{_ctx.display->get_display_metrics().scale};
	const auto icon_depth{UIStyle::icon_depth(scale)};

	const auto normal_tint{_ctx.controller->get_monochrome()
							   ? ImVec4{1.0f, 1.0f, 1.0f, 1.0f}
							   : UIStyle::icon_colour(ICON_SAVE_AND_QUIT)};

	const auto hovered_tint{ImVec4{get_hl_colour(_ctx.animation->lerp)}};

	// Passive frame.
	with_Window(WINDOW_LAYER_TEXTS, nullptr,
				ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs) {

		draw_frame(&frame_cmp);
	}

	// Interactive save icon.
	with_Window(WINDOW_LAYER_MENUS, nullptr,
				ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoTitleBar) {

		ImGui::SetCursorPos(save_pos);

		const auto activated{
			ImGui::InvisibleButton("##save_and_quit", save_size)};

		const auto hovered{ImGui::IsItemHovered()};
		const auto tint{hovered ? hovered_tint : normal_tint};

		draw_fg_image_with_idx(WINDOW_LAYER_MENUS, ICONS_TEXTURE,
							   ICON_SAVE_AND_QUIT, save_pos, save_size, tint,
							   icon_depth);

		if (activated)
			_ctx.game->save_game();
	}
}

auto Sorcery::UI::draw_compass() -> void {

	auto cmp{components->get("engine_base_ui:compass")};
	auto frame_cmp{components->get("engine_base_ui:compass_frame")};

	const auto x{metrics->grid_x(cmp.x)};
	const auto y{metrics->grid_y(cmp.y)};
	const auto scale{_ctx.display->get_display_metrics().scale};
	const auto width{cmp.w * metrics->grid_sz() * scale};
	const auto height{cmp.h * metrics->grid_sz() * scale};

	// Get Icon Effects
	const auto icon_depth{UIStyle::icon_depth(scale)};

	with_Window(WINDOW_LAYER_TEXTS, nullptr, ImGuiWindowFlags_NoDecoration) {

		draw_frame(&frame_cmp);
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

			const auto tint{_ctx.controller->get_monochrome()
								? ImVec4{1.0f, 1.0f, 1.0f, 1.0f}
								: UIStyle::icon_colour(icon_idx)};

			draw_fg_image_with_idx(WINDOW_LAYER_TEXTS, ICONS_TEXTURE, icon_idx,
								   ImVec2{x, y}, ImVec2{width, height}, tint,
								   icon_depth);
		}
	}
}

auto Sorcery::UI::draw_party_panel() -> void {

	auto cmp{components->get("global:party_panel")};
	auto frame_cmp{components->get("engine_base_ui:party_frame")};

	const auto width{static_cast<float>(cmp.w * metrics->grid_sz())};
	const auto height{static_cast<float>(cmp.h * metrics->grid_sz())};

	const auto x{cmp.x == -1 ? (ImGui::GetMainViewport()->Size.x - width) / 2.0f
							 : metrics->grid_x(cmp.x)};

	const auto y{metrics->grid_y(cmp.y)};
	const ImVec2 panel_pos{x, y};
	const ImVec2 panel_size{width, height};

	// The frame itself is passive.
	with_Window(WINDOW_LAYER_TEXTS, nullptr,
				ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs) {

		draw_frame(&frame_cmp);
	}

	// Interactive panel content must be on the menu/input layer.
	with_Window(WINDOW_LAYER_MENUS, nullptr,
				ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoTitleBar) {

		ImGui::SetCursorPos(panel_pos);

		with_Child("party_panel_child", panel_size, ImGuiChildFlags_None,
				   ImGuiWindowFlags_NoScrollbar |
					   ImGuiWindowFlags_NoScrollWithMouse) {

			const auto font{fonts->get_current_font(cmp.font).value()};

			set_Font(font, metrics->font_sz());

			UIStyle::set_text_bright(_ctx);

			ImGui::TextUnformatted(
				_ctx.get_string("PARTY_PANEL_LEGEND").c_str());

			if (!_ctx.game->state->party_has_members())
				return;

			const auto row_height{static_cast<float>(metrics->grid_sz())};
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
					font, metrics->font_sz(), text_pos,
					ImGui::ColorConvertFloat4ToU32(ImVec4{text_colour}),
					summary.c_str());

				if (activated)
					_ctx.controller->actions->inspect(character_id);

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

auto Sorcery::UI::draw_spell_info() -> void {

	const auto idx{_ctx.get_selected("spellbook_selected")};
	if (idx == 50)
		return;

	auto cmp{components->get("spellbook:spell_data")};
	auto pos{metrics->grid_pos(cmp.x, cmp.y)};
	ImGui::SetNextWindowPos(pos);
	with_Window(WINDOW_LAYER_TEXTS, nullptr, ImGuiWindowFlags_NoDecoration) {
		with_Child("spell_child", ImVec2(metrics->grid_sz() * cmp.w,
										 metrics->grid_sz() * cmp.h)) {

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
				set_Font(fonts->get_current_font(cmp.font).value(),
						 metrics->font_sz());
				ImGui::TextUnformatted(spell_name.c_str());
				ImGui::NewLine();
				ImGui::TextUnformatted(summary.c_str());
				ImGui::NewLine();
			}

			set_Font(fonts->get_current_font(cmp.font).value(),
					 metrics->font_sz());
			UIStyle::set_text_dim(_ctx);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-security"
			ImGui::TextWrapped(spell.details.c_str());
#pragma GCC diagnostic pop
		}
	}
}

auto Sorcery::UI::draw_monster_info() -> void {
	// Custom Rendering
	const auto idx{_ctx.get_selected("bestiary_selected")};
	const auto mon{_ctx.resources->monsters->get(idx)};
	const auto k_gfx{mon.get_known_gfx()};
	const auto u_gfx{mon.get_unknown_gfx()};
	auto k_mg_c{components->get("bestiary:known_monster_graphic")};
	auto u_mg_c{components->get("bestiary:unknown_monster_graphic")};
	auto k_mg_pos{metrics->grid_pos(k_mg_c.x, k_mg_c.y)};
	auto u_mg_pos{metrics->grid_pos(u_mg_c.x, u_mg_c.y)};
	const auto scale{_ctx.display->get_display_metrics().scale};
	draw_fg_image_with_idx(KNOWN_CREATURES_TEXTURE, k_gfx, k_mg_pos,
						   ImVec2{k_mg_c.get_float("tile_width") * scale,
								  k_mg_c.get_float("tile_width") * scale});
	draw_fg_image_with_idx(UNKNOWN_CREATURES_TEXTURE, u_gfx, u_mg_pos,
						   ImVec2{u_mg_c.get_float("tile_width") * scale,
								  u_mg_c.get_float("tile_width") * scale});

	auto cmp{components->get("bestiary:monster_data")};
	auto pos{metrics->grid_pos(cmp.x, cmp.y)};

	with_Window(WINDOW_LAYER_MENUS, nullptr, ImGuiWindowFlags_NoDecoration) {
		const auto name{std::format("  {:>03}:{}/{}", idx, mon.get_known_name(),
									mon.get_unknown_name())};
		ImGui::SetCursorPos(pos);
		with_Child(
			"bestiary_tab_bar_child",
			ImVec2(metrics->grid_sz() * cmp.w, metrics->grid_sz() * cmp.h)) {
			UIStyle::set_tab_black(_ctx);
			set_Font(fonts->get_current_font(cmp.font).value(),
					 metrics->font_sz());
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

auto Sorcery::UI::draw_current_level_map() -> void {

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
	const ImVec2 top_left_pos{metrics->grid_pos(map_c.x, map_c.y)};

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

			draw_map_tile(tile, tile_pos, tile_sz);
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

	draw_fg_image_with_idx(WINDOW_LAYER_TEXTS, ICONS_TEXTURE, player_icon,
						   player_draw_pos, tile_sz, tint);
}

auto Sorcery::UI::draw_level_no_player() -> void {

	// Menu Selection for B1F to B10F is 0 to 0, thus convert it into -1 to
	// -10 for depth
	if (_ctx.get_selected("atlas_selected") == 10)
		return;

	const auto depth{-1 - _ctx.get_selected("atlas_selected")};
	Level level{_ctx.resources->levels->get(depth).value()};

	// Work out where and how to draw the grid
	auto tc{20};
	const auto map_c{components->get("atlas:map_graphic")};
	ImVec2 top_left_pos{metrics->grid_pos(map_c.x, map_c.y)};
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
			draw_map_tile(tile, tile_pos, tile_sz);
			++tcx;
		}
		++tcy;
		tcx = 0;
	}
}

auto Sorcery::UI::draw_loading_progress() -> void {

	auto pb_c{components->get("splash:progress_bar")};

	const auto width{pb_c.w * metrics->grid_sz()};
	const float progress{static_cast<float>(images->progress - 1) /
						 static_cast<float>(images->capacity)};
	const auto x{std::invoke([&] {
		if (pb_c.x == -1) {
			const auto viewport{ImGui::GetMainViewport()};
			return (viewport->Size.x - width) / 2;
		} else
			return metrics->grid_x(pb_c.x);
	})};
	const auto y{metrics->grid_y(pb_c.y)};

	with_Window(WINDOW_LAYER_IMAGES, nullptr,
				ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoTitleBar) {
		set_Font(fonts->get_default_font(), metrics->font_sz());
		set_StyleColor(ImGuiCol_PlotHistogram,
					   ImGui::GetColorU32(ImGuiCol_ButtonHovered));
		ImGui::SetCursorPos(ImVec2{x, y});

		ImGui::ProgressBar(progress, ImVec2(width, 4), "");
	}
}

auto Sorcery::UI::draw_bg_video() -> void {

	auto elapsed_sec{(SDL_GetTicks() - ticks) / 1000.0};
	vfx_player->update(elapsed_sec);
	vfx_player->render(WINDOW_LAYER_BG);
}

auto Sorcery::UI::draw_attract_mode() -> void {

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
	auto tile_pos{
		ImVec2{(viewport->Size.x - am_size) / 2, metrics->grid_y(attract.y)}};

	// And draw each tile (this will draw to the correct layer)
	for (auto idx : _attract_data) {

		draw_fg_image_with_idx(KNOWN_CREATURES_TEXTURE, idx, tile_pos,
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

	if (_ctx.controller->menus->handle_actions(name, selection, data_item))
		return;

	if (_ctx.controller->menus->handle_dynamic(name, items, data_item,
											   selection))
		return;

	_ctx.controller->menus->handle_standard(name, items, data_item, selection);
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
					 : metrics->grid_x(pos.x)};

	const auto y{pos.y == -1.0f
					 ? (ImGui::GetMainViewport()->Size.y - sz.y) / 2.0f
					 : metrics->grid_y(pos.y)};

	// Set the Style for the Menu (this is a bit of a hack, but it works)
	set_StyleColor(ImGuiCol_FrameBg,
				   ImVec4{0.0f, 0.0f, 0.0f, 1.0f - _ctx.animation->fade});
	set_StyleColor(ImGuiCol_HeaderActive, ImVec4{sel_color});
	set_StyleColor(ImGuiCol_HeaderHovered, ImVec4{sel_color});
	UIStyle::set_faded_with_disabled(_ctx);
	set_Font(fonts->get_current_font(font).value(), metrics->font_sz());

	const ImVec2 menu_pos{x, y};
	ImGui::SetCursorPos(menu_pos);

	// Look for a Key Selection (if numeric_shortcuts is true, then we will
	// consume a key from the controller)
	const auto key_selection{
		numeric_shortcuts
			? _ctx.controller->input->consume_menu_key(items.size())
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
				_ctx.controller->menus->item_disabled(name, index, data_item)};

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

					_ctx.controller->menus->handle_standard(name, items,
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

auto Sorcery::UI::draw_map_tile(const Tile &tile, const ImVec2 pos,
								const ImVec2 sz) -> void {
	using enum Enums::DrawMap::Feature;
	using enum Enums::Tile::Features;
	using enum Enums::Tile::Properties;

	// Background Graphic
	draw_fg_image_with_idx(MAPS_TEXTURE, std::to_underlying(FLOOR), pos, sz);

	// Darkness
	if (tile.is(Enums::Tile::Properties::DARKNESS))
		draw_fg_image_with_idx(MAPS_TEXTURE, std::to_underlying(MAP_DARKNESS),
							   pos, sz);

	// Walls for all 4 directions
	using enum Enums::Tile::Edge;
	using enum Enums::Map::Direction;
	if (tile.has(NORTH, SECRET_DOOR) || tile.has(NORTH, ONE_WAY_HIDDEN_DOOR))
		draw_fg_image_with_idx(MAPS_TEXTURE, std::to_underlying(NORTH_SECRET),
							   pos, sz);
	else if (tile.has(NORTH, UNLOCKED_DOOR) || tile.has(NORTH, ONE_WAY_DOOR))
		draw_fg_image_with_idx(MAPS_TEXTURE, std::to_underlying(NORTH_DOOR),
							   pos, sz);
	else if (tile.has(NORTH, ONE_WAY_WALL))
		draw_fg_image_with_idx(MAPS_TEXTURE,
							   std::to_underlying(NORTH_ONE_WAY_WALL), pos, sz);
	else if (tile.has(NORTH))
		draw_fg_image_with_idx(MAPS_TEXTURE, std::to_underlying(NORTH_WALL),
							   pos, sz);

	if (tile.has(SOUTH, SECRET_DOOR) || tile.has(SOUTH, ONE_WAY_HIDDEN_DOOR))
		draw_fg_image_with_idx(MAPS_TEXTURE, std::to_underlying(SOUTH_SECRET),
							   pos, sz);
	else if (tile.has(SOUTH, UNLOCKED_DOOR) || tile.has(SOUTH, ONE_WAY_DOOR))
		draw_fg_image_with_idx(MAPS_TEXTURE, std::to_underlying(SOUTH_DOOR),
							   pos, sz);
	else if (tile.has(SOUTH, ONE_WAY_WALL))
		draw_fg_image_with_idx(MAPS_TEXTURE,
							   std::to_underlying(SOUTH_ONE_WAY_WALL), pos, sz);
	else if (tile.has(SOUTH))
		draw_fg_image_with_idx(MAPS_TEXTURE, std::to_underlying(SOUTH_WALL),
							   pos, sz);

	if (tile.has(EAST, SECRET_DOOR) || tile.has(EAST, ONE_WAY_HIDDEN_DOOR))
		draw_fg_image_with_idx(MAPS_TEXTURE, std::to_underlying(EAST_SECRET),
							   pos, sz);
	else if (tile.has(EAST, UNLOCKED_DOOR) || tile.has(EAST, ONE_WAY_DOOR))
		draw_fg_image_with_idx(MAPS_TEXTURE, std::to_underlying(EAST_DOOR), pos,
							   sz);
	else if (tile.has(EAST, ONE_WAY_WALL))
		draw_fg_image_with_idx(MAPS_TEXTURE,
							   std::to_underlying(EAST_ONE_WAY_WALL), pos, sz);
	else if (tile.has(EAST))
		draw_fg_image_with_idx(MAPS_TEXTURE, std::to_underlying(EAST_WALL), pos,
							   sz);

	if (tile.has(WEST, SECRET_DOOR) || tile.has(WEST, ONE_WAY_HIDDEN_DOOR))
		draw_fg_image_with_idx(MAPS_TEXTURE, std::to_underlying(WEST_SECRET),
							   pos, sz);
	else if (tile.has(WEST, UNLOCKED_DOOR) || tile.has(WEST, ONE_WAY_DOOR))
		draw_fg_image_with_idx(MAPS_TEXTURE, std::to_underlying(WEST_DOOR), pos,
							   sz);
	else if (tile.has(WEST, ONE_WAY_WALL))
		draw_fg_image_with_idx(MAPS_TEXTURE,
							   std::to_underlying(WEST_ONE_WAY_WALL), pos, sz);
	else if (tile.has(WEST))
		draw_fg_image_with_idx(MAPS_TEXTURE, std::to_underlying(WEST_WALL), pos,
							   sz);

	// And Tile Contents
	if (tile.has(STAIRS_UP) || tile.has(LADDER_UP))
		draw_fg_image_with_idx(MAPS_TEXTURE, std::to_underlying(MAP_STAIRS_UP),
							   pos, sz);
	else if (tile.has(STAIRS_DOWN) || tile.has(LADDER_DOWN))
		draw_fg_image_with_idx(MAPS_TEXTURE,
							   std::to_underlying(MAP_STAIRS_DOWN), pos, sz);
	else if (tile.has(ELEVATOR))
		draw_fg_image_with_idx(MAPS_TEXTURE, std::to_underlying(MAP_ELEVATOR),
							   pos, sz);
	else if (tile.has(SPINNER))
		draw_fg_image_with_idx(MAPS_TEXTURE, std::to_underlying(MAP_SPINNER),
							   pos, sz);
	else if (tile.has(PIT))
		draw_fg_image_with_idx(MAPS_TEXTURE, std::to_underlying(MAP_PIT), pos,
							   sz);
	else if (tile.has(CHUTE))
		draw_fg_image_with_idx(MAPS_TEXTURE, std::to_underlying(MAP_CHUTE), pos,
							   sz);
	else if (tile.has(TELEPORT_TO))
		draw_fg_image_with_idx(MAPS_TEXTURE,
							   std::to_underlying(MAP_TELEPORT_TO), pos, sz);
	else if (tile.has(TELEPORT_FROM))
		draw_fg_image_with_idx(MAPS_TEXTURE,
							   std::to_underlying(MAP_TELEPORT_FROM), pos, sz);
	else if (tile.has(MESSAGE) || tile.has(NOTICE))
		draw_fg_image_with_idx(MAPS_TEXTURE, std::to_underlying(EXCLAMATION),
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

auto Sorcery::UI::draw_transient() -> void {

	if (!_transient_message)
		return;

	if (std::chrono::steady_clock::now() >= _transient_message->expires) {
		_transient_message.reset();
		return;
	}

	const auto &message{*_transient_message};

	const auto component{components->get("engine_base_ui:transient_message")};

	set_Font(fonts->get_current_font(component.font).value(),
			 metrics->font_sz());

	const auto text_size{ImGui::CalcTextSize(message.text.c_str())};

	const auto padding{metrics->grid_sz() * 2.0f};

	const auto width{message.width == TransientWidth::FULL
						 ? ImGui::GetMainViewport()->Size.x
						 : text_size.x + (padding * 2.0f)};

	const auto height{component.h * metrics->grid_sz() + (padding * 2.0f)};

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