// Copyright (C) 2024 Dave Moore
//
// This file is part of Sorcery: Shadows under Llylgamyn.
//
// Sorcery: Shadows under Llylgamyn is free software: you can redistribute
// it and/or modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation, either version 2 of the License,
// or (at your option) any later version.
//
// Sorcery: Shadows under Llylgamyn is distributed in the hope that it wil
// be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
// Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Sorcery: Shadows under Llylgamyn.  If not,
// see <http://www.gnu.org/licenses/>.
//
// If you modify this program, or any covered work, by linking or combining
// it with the libraries referred to in README (or a modified version of
// said libraries), containing parts covered by the terms of said libraries,
// the licensors of this program grant you additional permission to convey
// the resulting work.

#pragma once

#include "common/define.hpp"
#include "common/include.hpp"
#include "core/define.hpp"
#include "core/include.hpp"
#include "core/platform.hpp"
#include "core/type.hpp"

namespace Sorcery {

// Forward Declarations
class Component;
class ComponentStore;
class StringStore;
class System;

// Class to act as a wrapper around SFML Window etc
class Window {

	public:

		// Constructors
		Window(System *system, StringStore *string, ComponentStore *layout, const std::string &title);
		Window() = delete;

		// Public Methods
		auto clear_window() -> void;
		auto display_window() -> void;
		auto draw_text(sf::Text &text) -> void;
		auto draw_text(sf::Text &text, const Component &component, const double lerp = -1.0f) -> void;
		auto draw_text(sf::Text &text, const Component &component, const std::string &string) -> void;
		auto draw_text(sf::Text &text, const Component &component, const std::string &string, const double lerp)
			-> void;
		auto draw_gui() -> void;
		auto get_ch() const -> unsigned int;
		auto get_cw() const -> unsigned int;
		auto get_cursor() const -> sf::Sprite;
		auto set_cursor_coord(const bool value) -> void;
		auto get_cursor_coord() const -> bool;
		auto draw_cursor_coord(const sf::Vector2f mouse_pos) -> void;
		auto get_disc() const -> sf::Sprite;
		auto get_gui() -> tgui::Gui *;
		auto get_window() -> sf::RenderWindow *;
		auto get_cur() const -> sf::Vector2f;
		auto get_x(const sf::Sprite &sprite, int x_position) const -> unsigned int;
		auto get_y(const sf::Sprite &sprite, int y_position) const -> unsigned int;
		auto get_x(const sf::Text &text, int x_position) const -> unsigned int;
		auto get_y(const sf::Text &text, int y_position) const -> unsigned int;
		auto get_x(unsigned int width, int x_position) const -> unsigned int;
		auto get_y(unsigned int width, int y_position) const -> unsigned int;
		auto hl_text(sf::Text &text, const Component &component, double lerp) -> sf::RectangleShape;
		auto shove_text(const sf::Text &shovee, sf::Text &shover, unsigned int gap_units) const -> void;
		auto shove_text(const sf::Text &shovee, sf::Text &shover, float gap_units) const -> void;
		auto set_input_mode(WIM value) -> void;
		auto get_input_mode() const -> WIM;
		auto set_pos(Component *component, sf::Transformable *object) const -> void;
		auto save_screen() -> void;
		auto restore_screen() -> void;
		auto set_bold(const bool value) -> void;
		auto set_upper(const bool value) -> void;
		auto get_centre_x(unsigned int width) const -> unsigned int;
		auto get_centre_y(unsigned int height) const -> unsigned int;

		// Public Members
		std::vector<sf::Sprite> sprites;
		std::vector<sf::Texture> textures;
		sf::View view;
		Coordinate centre;
		sf::IntRect size;

	private:

		// Private Methods
		auto _adjust_brightness(sf::Color colour, double colour_lerp) const -> unsigned long long;
		auto _draw_text(sf::Text &text, const Component &component, const double lerp) -> void;
		auto _draw_text(sf::Text &text, const Component &component, const std::string &string, const double lerp)
			-> void;
		auto _draw_text(sf::Text &text, const Component &component, const std::string &string) -> void;
		auto _draw_text(sf::Text &text) -> void;
		auto _get_centre_x(const sf::Sprite &sprite) const -> unsigned int;
		auto _get_centre_x(const sf::Text &text) const -> unsigned int;
		auto _get_centre_x(unsigned int width) const -> unsigned int;
		auto _get_centre_y(const sf::Sprite &sprite) const -> unsigned int;
		auto _get_centre_y(const sf::Text &text) const -> unsigned int;
		auto _get_centre_y(unsigned int height) const -> unsigned int;
		auto _get_x(const sf::Sprite &sprite, const int x_position) const -> unsigned int;
		auto _get_x(const sf::Text &text, const int x_position) const -> unsigned int;
		auto _get_x(unsigned int width, const int x_position) const -> unsigned int;
		auto _get_y(const sf::Sprite &sprite, const int y_position) const -> unsigned int;
		auto _get_y(const sf::Text &text, const int y_position) const -> unsigned int;
		auto _get_y(unsigned int width, const int y_position) const -> unsigned int;
		auto _get_cursor_coord_text(sf::Vector2f mouse_pos, sf::Text &text) -> void;

		// Private Members
		ScreenSize _default_size;
		ScreenSize _current_size;
		System *_system;
		StringStore *_string;
		ComponentStore *_layout;
		std::string _title;
		sf::RenderWindow _window;
		tgui::Gui _gui;
		tgui::Theme _theme;
		unsigned int _cell_height;
		unsigned int _cell_width;
		WIM _input_mode;
		sf::Image _capture;
		sf::Texture _texture;
		bool _bold_text;
		bool _upper_text;
		bool _show_cursor_coords;
		sf::Text _cursor_coord;
};

}
