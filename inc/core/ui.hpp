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

#pragma once

using GLuint = unsigned int;

#include "common/enum.hpp"
#include "common/imgui.hpp"
#include "core/enum.hpp"
#include "types/enum.hpp"

#include <any>
#include <array>
#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>

// UI Generation Class (sits on top of, and heavil;y uses, ImGui)
namespace Sorcery {

// Forward Declaration
struct Context;
class Character;
class Controller;
class Component;
class ComponentStore;
class Dialog;
class Display;
class FontStore;
class Frame;
class Game;
class ImageStore;
class Input;
class Message;
class Menu;
class MenuBuilder;
class Modal;
class Render;
class Popup;
struct Tile;
class VideoPlayer;
struct VertexArray;

class UI {

	public:
		// Standard Constructor
		UI(Context &ctx);

		// standard Destructor
		~UI();

		// Public Methods
		auto create_dynamic_modal(const std::string name) -> void;
		auto display(Enums::Screen screen, std::any param = nullptr) -> void;
		auto display_engine() -> void;
		auto display_refresh(std::any payload = nullptr) -> void;
		auto draw_cursor(const bool value) -> void;
		auto draw_frame(const ImVec2 p_min, const ImVec2 p_max,
						const ImVec4 colour, const int rounding) -> void;
		auto draw_image(std::string_view source, const int idx,
						const ImVec2 p_min, const ImVec2 p_sz) -> void;
		auto draw_view_image(std::string_view source, const VertexArray &array)
			-> void;
		auto draw_menu(const std::string name, const ImColor sel_colour,
					   const ImVec2 pos, const ImVec2 sz,
					   const Enums::Layout::Font font,
					   std::vector<std::string> &items, std::vector<int> &data,
					   const bool reorder, const bool across) -> void;
		auto draw_text(const std::string string, const ImColor colour,
					   const ImVec2 pos, const Enums::Layout::Font font)
			-> void;
		auto draw_ui_status() -> void;
		auto get_hl_colour(const double percent) const -> ImColor;
		auto lerp_colour(const ImVec4 col_from, const ImVec4 col_yo,
						 const double percent) const -> ImVec4;
		auto io() -> ImGuiIO &;
		auto load_message(const Enums::Map::Event event)
			-> std::vector<std::string>;
		auto set_monochrome(const bool value) -> void;
		auto set_fullscreen(const bool value) -> void;
		auto start() -> void;
		auto stop() -> void;
		auto in_popup() const -> bool;
		auto close_all_popups() -> void;
		auto active_popup_count() const -> int;

		// Public Members
		std::unique_ptr<ImageStore> images;
		std::unique_ptr<ComponentStore> components;
		std::unique_ptr<FontStore> fontstore;

		std::unique_ptr<MenuBuilder> menubuilder;

		std::unique_ptr<Dialog> dialog_exit;
		std::unique_ptr<Dialog> dialog_new;
		std::unique_ptr<Dialog> dialog_leave;
		std::unique_ptr<Dialog> notice_cannot_donate;
		std::unique_ptr<Dialog> notice_donated_ok;
		std::unique_ptr<Dialog> notice_not_enough_gold;
		std::unique_ptr<Dialog> notice_divvy;
		std::unique_ptr<Dialog> notice_pool_gold;
		std::unique_ptr<Dialog> dialog_stairs_up;
		std::unique_ptr<Dialog> dialog_stairs_down;
		std::unique_ptr<Input> input_donate;
		std::unique_ptr<Input> input_name;
		std::unique_ptr<Popup> popup_ouch;
		std::unique_ptr<Modal> modal_camp;
		std::unique_ptr<Message> message_tile;
		std::unique_ptr<Modal> modal_inspect;
		std::unique_ptr<Modal> modal_stay;
		std::unique_ptr<Modal> modal_help;
		std::unique_ptr<Modal> modal_tithe;
		std::unique_ptr<Modal> modal_identify;
		std::unique_ptr<Modal> modal_drop;
		std::unique_ptr<Modal> modal_trade;
		std::unique_ptr<Modal> modal_give;
		std::unique_ptr<Modal> modal_use;
		std::unique_ptr<Modal> modal_invoke;
		std::unique_ptr<VideoPlayer> vfx_player;
		unsigned int font_sz;
		unsigned int grid_sz;
		unsigned int frame_rd;
		unsigned int ui_rd;
		unsigned int columns;
		unsigned int rows;
		unsigned int adj_grid_w;
		unsigned int adj_grid_h;
		ImVec4 ui_colour;
		std::map<std::string, int> selected;
		std::map<std::string, int> highlighted;
		std::map<std::string, bool> pressed;
		std::array<bool, 21> ms_selected;
		std::array<bool, 29> ps_selected;
		std::uint32_t ticks;
		bool first_frame;

	private:
		// Private Members
		Context &_ctx;
		ImGuiIO _io;
		std::unique_ptr<Render> _render;
		std::vector<std::shared_ptr<Frame>> _frames;
		std::vector<std::shared_ptr<Menu>> _menus;
		std::vector<unsigned int> _attract_data;

		std::map<Enums::Screen, std::function<void()>> _draw_modules;
		std::map<Enums::Screen, std::function<void(int)>>
			_draw_modules_with_int;
		std::map<Enums::Screen, std::function<void(const std::string &)>>
			_draw_modules_with_string;

		// Private Methods
		auto _display_atlas() -> void;
		auto _display_bestiary() -> void;
		auto _display_compendium() -> void;
		auto _display_main_menu() -> void;
		auto _display_museum() -> void;
		auto _display_options() -> void;
		auto _display_spellbook() -> void;
		auto _display_splash() -> void;

		auto _display_add() -> void;
		auto _display_castle() -> void;
		auto _display_edge_of_town() -> void;
		auto _display_inn() -> void;
		auto _display_pay() -> void;
		auto _display_remove() -> void;
		auto _display_restart() -> void;
		auto _display_shop() -> void;
		auto _display_stay() -> void;
		auto _display_tavern() -> void;
		auto _display_temple() -> void;
		auto _display_training_grounds() -> void;

		auto _display_automap() -> void;

		auto _display_choose(const int mode) -> void;
		auto _display_create_name(const int stage) -> void;
		auto _display_create_race(const int stage) -> void;
		auto _display_create_alignment(const int stage) -> void;
		auto _display_create_class(const int stage) -> void;
		auto _display_create_confirm(const int stage) -> void;
		auto _display_heal(const int stage) -> void;
		auto _display_inspect(const int mode) -> void;
		auto _display_level_up(const int mode) -> void;
		auto _display_no_level_up(const int mode) -> void;
		auto _display_recovery(const int mode) -> void;
		auto _display_reorder(const int mode) -> void;
		auto _display_roster(const int mode) -> void;

		auto _display_license(const std::string &string) -> void;

		auto _draw_attract_mode() -> void;
		auto _draw_tiled_bg(Component *component) -> void;
		auto _draw_bg_image(Component *component) -> void;
		auto _draw_bg_video() -> void;

		auto _draw_buffbar() -> void;
		auto _draw_button(Component *component,
						  std::optional<bool *> is_clicked = std::nullopt)
			-> void;
		auto _draw_button_click(Component *component, bool &is_clicked,
								const bool reverse = false) -> void;
		auto _draw_character_summary(Component *component, Character *character)
			-> void;
		auto _draw_character_detailed(Component *component,
									  Character *character) -> void;
		auto _draw_character_detailed_again(Component *component,
											Character *character) -> void;
		auto _draw_character_mage_spells(Component *component,
										 Character *character) -> void;
		auto _draw_character_priest_spells(Component *component,
										   Character *character) -> void;
		auto _draw_choose(const int mode) -> void;
		auto _draw_create_name(const int mode) -> void;
		auto _draw_create_race(const int mode) -> void;
		auto _draw_create_alignment(const int mode) -> void;
		auto _draw_create_class(const int mode) -> void;
		auto _draw_create_confirm(const int mode) -> void;
		auto _draw_compass() -> void;
		auto _draw_components(std::string_view screen, const int mode = -1)
			-> void;
		auto _draw_current_character(const int mode) -> void;
		auto _draw_cursor() -> void;
		auto _draw_fg_image(Component *component) -> void;
		auto _draw_fg_image_with_idx(std::string_view source, const int idx,
									 const ImVec2 p_min, const ImVec2 p_sz,
									 const ImVec4 tint = ImVec4{
										 1.0f, 1.0f, 1.0f, 1.0f}) -> void;
		auto _draw_fg_image_with_idx(std::string_view layer,
									 std::string_view source, const int idx,
									 const ImVec2 p_min, const ImVec2 p_sz,
									 const ImVec4 tint = ImVec4{
										 1.0f, 1.0f, 1.0f, 1.0f}) -> void;
		auto _draw_frame(Component *component) -> void;
		auto _draw_heal(const int stage) -> void;
		auto _draw_icons() -> void;
		auto _draw_input(Component *component, std::string *input) -> void;
		auto _draw_item_info() -> void;
		auto _draw_license(Component *component, const std::string &string)
			-> void;
		auto _draw_loading_progress() -> void;
		auto _draw_current_level_map() -> void;
		auto _draw_level_no_player() -> void;
		auto _draw_level_up(const int mode) -> void;
		auto _draw_map_tile(const Tile &tile, const ImVec2 pos, const ImVec2 sz)
			-> void;
		auto _draw_menu(Component *component) -> void;
		auto _draw_monster_info() -> void;
		auto _draw_no_level_up(const int mode) -> void;
		auto _draw_paragraph(Component *component) -> void;
		auto _draw_party_panel() -> void;
		auto _draw_pay_info() -> void;
		auto _draw_options() -> void;
		auto _draw_recovery(const int mode) -> void;
		auto _draw_save() -> void;
		auto _draw_stay() -> void;
		auto _draw_spell_info() -> void;
		auto _draw_stepper(Component *component, const std::string &name,
						   int *value) -> void;
		auto _draw_text(Component *component) -> void;
		auto _draw_text(Component *component, const std::string &string)
			-> void;
		auto _get_status_color(Character *character) const -> ImVec4;
		auto _get_popups() const -> std::string;
		auto _setup_windows() -> void;

		auto _draw_debug() -> void;
		auto _draw_window_menu() -> void;
		auto _draw_ui_status() -> void;

		auto _get_menu_ui_flags(std::string_view menu)
			-> std::vector<std::reference_wrapper<bool>>;

		auto _to_imgui(GLuint tex) -> ImTextureID;

		auto _mage_spell_index(Enums::Magic::SpellID id) -> std::size_t;
		auto _priest_spell_index(Enums::Magic::SpellID id) -> std::size_t;

		auto _popup_states() const -> std::vector<bool *>;
};
};
