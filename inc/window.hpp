// Copyright (C) 2020 Dave Moore
//
// This file is part of Sorcery: Dreams of the Mad Overlord.
//
// Sorcery: Dreams of the Mad Overlord is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Sorcery: Dreams of the Mad Overlord is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Sorcery: Dreams of the Mad Overlord.  If not, see <http://www.gnu.org/licenses/>.
//
// If you modify this Program, or any covered work, by linking or combining it
// with the libraries referred to in README (or a modified version of said
// libraries), containing parts covered by the terms of said libraries, the
// licensors of this Program grant you additional permission to convey the
// resulting work.

#pragma once

#include "main.hpp"
#include "system.hpp"
#include "string.hpp"
#include "layout.hpp"

// Class to act as a wrapper around SFML Window etc
namespace Sorcery {

	class Window {

		public:

			// Standard Constructor
			Window(std::string title, System& system, String& string, Layout& layout);

			// Standard Destructor
			virtual ~Window();

			// Public Methods
			auto change_colour(sf::Color colour, double lerp) -> sf::Color;
			auto clear_window() -> void;
			auto darken_colour(sf::Color colour, double lerp) -> sf::Color;
			auto display_window() -> void;
			auto draw_menu(std::vector<MenuEntry>& items, std::vector<sf::FloatRect>& bounds,
				std::vector<MenuEntry>::const_iterator iterator, Component& component, double lerp) -> void;
			auto draw_text(sf::Text& text) -> void;
			auto draw_text(sf::Text& text, Component& component, double lerp  = -1.0f) -> void;
			auto draw_text(sf::Text& text, Component& component, std::string& string) -> void;
			auto draw_gui() -> void;
			auto get_cell_height() -> unsigned int;
			auto get_cell_width() -> unsigned int;
			auto get_cursor() -> sf::Sprite;
			auto get_gui() -> tgui::Gui*;
			auto get_gui_frame(sf::RenderTexture& gui_frame_rt, sf::Texture& gui_frame_t, const unsigned int width_units,
				const unsigned int height_units, unsigned int alpha) -> sf::Sprite;
			auto get_window() -> sf::RenderWindow*;
			auto get_x(sf::Sprite& sprite, int x_position) -> unsigned int;
			auto get_y(sf::Sprite& sprite, int y_position) -> unsigned int;
			auto lighten_colour(sf::Color colour, double lerp) -> sf::Color;

			// Public Members
			std::vector<sf::Sprite> sprites;
			std::vector<sf::Texture> textures;
			sf::View view;
			Point centre;
			WindowInputMode input_mode;

		private:

			// Private Methods
			auto _change_colour(sf::Color colour, double lerp) -> sf::Color;
			auto _draw_menu(std::vector<MenuEntry>& items, std::vector<sf::FloatRect>& bounds,
				std::vector<MenuEntry>::const_iterator iterator, Component& component, double lerp) -> void;
			auto _draw_text(sf::Text& text) -> void;
			auto _draw_text(sf::Text& text, Component& component, double lerp) -> void;
			auto _draw_text(sf::Text& text, Component& component, std::string& string) -> void;
			auto _get_centre_x(sf::Sprite& sprite) -> unsigned int;
			auto _get_centre_y(sf::Sprite& sprite) -> unsigned int;
			auto _get_centre_x(sf::Text& text) -> unsigned int;
			auto _get_centre_y(sf::Text& text) -> unsigned int;
			auto _get_x(sf::Sprite& sprite, int x_position) -> unsigned int;
			auto _get_x(sf::Text& text, int x_position) -> unsigned int;
			auto _get_y(sf::Sprite& sprite, int y_position) -> unsigned int;
			auto _get_y(sf::Text& text, int y_position) -> unsigned int;

			// Private Members
			ScreenSize _default_size;
			ScreenSize _current_size;
			std::string _title;
			System _system;
			String _string;
			Layout _layout;
			sf::RenderWindow _window;
			tgui::Gui _gui;
			unsigned int _cell_height;
			unsigned int _cell_width;
			unsigned int _creature_sprite_width;
			unsigned int _creature_sprite_height;
			unsigned int _creature_sprite_spacing;
			float _creature_sprite_scaling;
			sf::Texture _gui_frame_texture;
			sf::Font _mono_system_font;
			sf::Font _proportional_system_font;
	};
}
