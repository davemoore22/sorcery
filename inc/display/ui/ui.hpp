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

#include "display/ui/imageeffect.hpp"
#include "imgui.h"	   // for ImVec2 (ptr only), ImVec4, ImColor, ImGuiIO (...
#include <GL/glew.h>   // for GLuint
#include <any>		   // for any
#include <array>	   // for array
#include <chrono>	   // for seconds, milliseconds, steady_clock, time_point
#include <cstddef>	   // for size_t
#include <cstdint>	   // for uint32_t
#include <functional>  // for reference_wrapper
#include <map>		   // for map
#include <memory>	   // for unique_ptr, shared_ptr
#include <optional>	   // for optional, nullopt, nullopt_t
#include <string>	   // for string, basic_string
#include <string_view> // for string_view`
#include <vector>	   // for vector

namespace Sorcery { class Character; }
namespace Sorcery { class Component; }
namespace Sorcery { class ComponentStore; }
namespace Sorcery { class FontStore; }
namespace Sorcery { class Frame; }
namespace Sorcery { class ImageStore; }
namespace Sorcery { class Menu; }
namespace Sorcery { class MenuBuilder; }
namespace Sorcery { class PopupManager; }
namespace Sorcery { class Render; }
namespace Sorcery { class ScreenRenderer; }
namespace Sorcery { class UIMetrics; }
namespace Sorcery { class VideoPlayer; }
namespace Sorcery {
namespace Enums { enum class Screen; }
}
namespace Sorcery {
namespace Enums {
	namespace Layout { enum class Font; }
}
}
namespace Sorcery {
namespace Enums {
	namespace Magic { enum class SpellID; }
}
}
namespace Sorcery {
namespace Enums {
	namespace Map { enum class Event; }
}
}
namespace Sorcery { struct Context; }
namespace Sorcery { struct Tile; }
namespace Sorcery { struct VertexArray; }

// UI Generation Class (sits on top of, and heavily uses, ImGui)
namespace Sorcery {

enum class TransientWidth {
	FIT_TEXT,
	FULL
};

enum class TransientMode {
	DISMISS_ON_ACTION,
	UNTIL_EXPIRY
};

struct TransientMessage {
		std::string text;
		std::chrono::steady_clock::time_point expires;
		TransientWidth width{TransientWidth::FIT_TEXT};
		TransientMode mode{TransientMode::DISMISS_ON_ACTION};
};

class UI {

	public:
		// Standard Constructor
		UI(Context &ctx);

		// standard Destructor
		~UI();

		// Public Methods
		auto start() -> void;
		auto stop() -> void;

		// Main Dispatch Methods
		auto display_screen(Enums::Screen screen,
							const std::any &param = nullptr) -> void;
		auto display_engine() -> void;
		auto display_refresh(std::any payload = nullptr) -> void;

		// Helpers
		auto get_hl_colour(const double percent) const -> ImColor;
		auto lerp_colour(const ImVec4 col_from, const ImVec4 col_yo,
						 const double percent) const -> ImVec4;
		auto load_message(const Enums::Map::Event event)
			-> std::vector<std::string>;
		auto set_monochrome(const bool value) -> void;
		auto set_fullscreen(const bool value) -> void;

		[[nodiscard]] auto transient_blocks_input() const -> bool;
		[[nodiscard]] auto has_transient() const -> bool;

		// Primitive Drawables
		auto draw_button(Component *component,
						 std::optional<bool *> is_clicked = std::nullopt)
			-> void;
		auto draw_button_click(Component *component, bool &is_clicked,
							   const bool reverse = false) -> void;
		auto draw_fg_image(Component *component) -> void;
		auto draw_fg_image_with_idx(
			std::string_view source, const int idx, const ImVec2 p_min,
			const ImVec2 p_sz,
			const ImVec4 tint = ImVec4{1.0f, 1.0f, 1.0f, 1.0f},
			std::optional<ImageOffsetEffect> effect = std::nullopt) -> void;
		auto draw_fg_image_with_idx(
			std::string_view layer, std::string_view source, const int idx,
			const ImVec2 p_min, const ImVec2 p_sz,
			const ImVec4 tint = ImVec4{1.0f, 1.0f, 1.0f, 1.0f},
			std::optional<ImageOffsetEffect> effect = std::nullopt) -> void;
		auto draw_frame(Component *component) -> void;
		auto draw_frame(const ImVec2 p_min, const ImVec2 p_max,
						const ImVec4 colour, const int rounding) -> void;
		auto draw_image(std::string_view source, const int idx,
						const ImVec2 p_min, const ImVec2 p_sz) -> void;

		auto draw_input(Component &component, std::string &input,
						const ImGuiInputTextFlags input_flags) -> bool;
		auto draw_menu(Component *component) -> void;
		auto draw_menu(const std::string name, const ImColor sel_colour,
					   const ImVec2 pos, const ImVec2 sz,
					   const Enums::Layout::Font font,
					   std::vector<std::string> &items, std::vector<int> &data,
					   const bool reorder, const bool across = false,
					   const bool numeric_shortcuts = false) -> void;
		auto draw_paragraph(Component *component) -> void;
		auto draw_stepper(Component *component, const std::string &name,
						  int &value) -> void;
		auto draw_text(Component *component) -> void;
		auto draw_text(Component *component, const std::string &string) -> void;
		auto draw_text(const std::string string, const ImColor colour,
					   const ImVec2 pos, const Enums::Layout::Font font)
			-> void;
		auto draw_text_with_layer(const std::string string,
								  const ImColor colour, const ImVec2 pos,
								  const Enums::Layout::Font font) -> void;
		auto draw_tiled_bg(Component *component) -> void;
		auto draw_view_image(std::string_view source, const VertexArray &array)
			-> void;

		// Draw Automatic Components
		auto draw_components(std::string_view screen, const int mode = -1)
			-> void;

		// Handle Transient Messages
		auto show_transient(
			std::string text,
			std::chrono::milliseconds duration = std::chrono::seconds{2},
			TransientWidth width = TransientWidth::FIT_TEXT,
			TransientMode mode = TransientMode::DISMISS_ON_ACTION) -> void;
		auto clear_transient() -> void;
		auto clear_transient_on_action() -> void;

		auto close_all_popups() -> void;

		// Composite Drawing Drawables
		auto draw_attract_mode() -> void;
		auto draw_automap_legend(Component *component) -> void;
		auto draw_bg_image(Component *component) -> void;
		auto draw_bg_video() -> void;
		auto draw_buffbar() -> void;
		auto draw_character_detailed(Component *component,
									 const Character *character) -> void;
		auto draw_character_detailed_again(Component *component,
										   const Character *character) -> void;
		auto draw_character_mage_spells(Component *component,
										const Character *character) -> void;
		auto draw_character_priest_spells(Component *component,
										  const Character *character) -> void;
		auto draw_character_summary(Component *component,
									const Character *character) -> void;
		auto draw_compass() -> void;
		auto draw_current_character(const int mode) -> void;
		auto draw_current_level_map() -> void;
		auto draw_cursor() -> void;
		auto draw_cursor(const bool value) -> void;
		auto draw_debug() -> void;
		auto draw_icons() -> void;
		auto draw_item_info() -> void;
		auto draw_level_name() -> void;
		auto draw_level_no_player() -> void;
		auto draw_loading_progress() -> void;
		auto draw_map_tile(const Tile &tile, const ImVec2 pos, const ImVec2 sz)
			-> void;
		auto draw_monster_info() -> void;
		auto draw_pay_info() -> void;
		auto draw_options() -> void;
		auto draw_save() -> void;
		auto draw_spell_info() -> void;
		auto draw_party_panel() -> void;
		auto draw_party_wipe() -> void;
		auto draw_transient() -> void;
		auto draw_ui_status() -> void;

		// Public Members
		std::unique_ptr<ImageStore> images;
		std::unique_ptr<ComponentStore> components;
		std::unique_ptr<FontStore> fonts;
		std::unique_ptr<PopupManager> popup_manager;
		std::unique_ptr<MenuBuilder> menubuilder;
		std::unique_ptr<VideoPlayer> vfx_player;
		std::unique_ptr<Render> render;
		std::unique_ptr<ScreenRenderer> screens;
		std::unique_ptr<UIMetrics> metrics;
		unsigned int frame_rd;
		unsigned int ui_rd;
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
		ImGuiIO *_io;
		std::vector<std::shared_ptr<Frame>> _frames;
		std::vector<std::shared_ptr<Menu>> _menus;
		std::vector<unsigned int> _attract_data;
		std::string _imgui_ini_path;
		std::optional<TransientMessage> _transient_message;

		auto _get_status_color(Character *character) const -> ImVec4;
		auto _setup_windows() -> void;

		auto _to_imgui(GLuint tex) -> ImTextureID;

		auto _mage_spell_index(Enums::Magic::SpellID id) -> std::size_t;
		auto _priest_spell_index(Enums::Magic::SpellID id) -> std::size_t;

		auto _activate_menu_item(const std::string_view name,
								 const int selection, const int data_item,
								 const std::vector<std::string> &items) -> void;
		auto _handle_menu_reordering(const std::string_view name,
									 std::vector<std::string> &items,
									 std::vector<int> &data,
									 const std::size_t index,
									 const int data_item) -> void;
};
};
