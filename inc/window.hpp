// Copyright (C) 2021 Dave Moore
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

#include "layout.hpp"
#include "main.hpp"
#include "string.hpp"
#include "system.hpp"

// Class to act as a wrapper around SFML Window etc
namespace Sorcery {

	class Window {

	  public:
		// Constructors
		Window(System *system, String *string, Layout *layout, const std::string &title);
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
		auto get_gui() -> tgui::Gui *;
		auto get_window() -> sf::RenderWindow *;
		auto get_x(const sf::Sprite &sprite, int x_position) const -> unsigned int;
		auto get_y(const sf::Sprite &sprite, int y_position) const -> unsigned int;
		auto get_x(const sf::Text &text, int x_position) const -> unsigned int;
		auto get_y(const sf::Text &text, int y_position) const -> unsigned int;
		auto get_x(unsigned int width, int x_position) const -> unsigned int;
		auto get_y(unsigned int width, int y_position) const -> unsigned int;
		auto hl_text(sf::Text &text, Component component, double lerp) -> sf::RectangleShape;
		auto shove_text(sf::Text &shovee, sf::Text &shover, unsigned int gap_units) -> void;
		auto shove_text(sf::Text &shovee, sf::Text &shover, float gap_units) -> void;
		auto set_input_mode(WindowInputMode value) -> void;
		auto get_input_mode() const -> WindowInputMode;
		auto set_pos(Component *component, sf::Transformable *object) -> void;
		auto save_screen() -> void;
		auto restore_screen() -> void;

		// Public Members
		std::vector<sf::Sprite> sprites;
		std::vector<sf::Texture> textures;
		sf::View view;
		Point centre;
		sf::IntRect size;

	  private:
		// Private Methods
		auto _adjust_brightness(sf::Color colour, double colour_lerp) -> unsigned long long;
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
		auto _get_centre_y(unsigned int width) const -> unsigned int;
		auto _get_x(const sf::Sprite &sprite, const int x_position) const -> unsigned int;
		auto _get_x(const sf::Text &text, const int x_position) const -> unsigned int;
		auto _get_x(unsigned int width, const int x_position) const -> unsigned int;
		auto _get_y(const sf::Sprite &sprite, const int y_position) const -> unsigned int;
		auto _get_y(const sf::Text &text, const int y_position) const -> unsigned int;
		auto _get_y(unsigned int width, const int y_position) const -> unsigned int;

		// Private Members
		ScreenSize _default_size;
		ScreenSize _current_size;
		System *_system;
		String *_string;
		Layout *_layout;
		std::string _title;
		sf::RenderWindow _window;
		tgui::Gui _gui;
		tgui::Theme _theme;
		unsigned int _cell_height;
		unsigned int _cell_width;
		WindowInputMode _input_mode;
		sf::Image _capture;
		sf::Texture _texture;
	};
} // namespace Sorcery
