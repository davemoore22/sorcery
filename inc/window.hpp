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

// Class to act as a wrapper around SFML Window etc
namespace Sorcery {

	class Window {

		public:

			// Standard Constructor
			Window(std::string title, System& system, String& string);

			// Standard Destructor
			virtual ~Window();

			// Public Methods
			auto operator[] (std::string_view key) const -> sf::Texture;

			auto clear_window() -> void;
			auto display_window() -> void;
			auto draw_attract_mode(std::vector<unsigned int> attract_mode_data) -> void;
			auto draw_gui() -> void;
			auto get_banner() -> sf::Sprite;
			auto get_creature_gfx(const int creature_id, const bool known) -> sf::Sprite;
			auto get_cursor() -> sf::Sprite;
			auto get_gui() -> tgui::Gui*;
			auto get_gui_frame(const unsigned int width, const unsigned int height, const unsigned int alpha) -> sf::Sprite;
			auto get_splash() -> sf::Sprite;
			auto get_logo() -> sf::Sprite;
			auto get_window() -> sf::RenderWindow*;

			// Public Members
			std::vector<sf::Sprite> sprites;
			std::vector<sf::Texture> textures;
			sf::View view;
			Point centre;

		private:

			// Private Methods
			auto _get_attract_mode(std::vector<unsigned int> attract_mode_data) -> sf::Sprite;

			// Private Members
			ScreenSize _default_size;
			ScreenSize _current_size;
			std::string _title;
			System _system;
			String _string;
			sf::RenderWindow _window;
			tgui::Gui _gui;
			std::map<std::string_view, sf::Texture> _textures;
			std::vector<unsigned int> _attract_mode_data;
			unsigned int _creature_sprite_width;
			unsigned int _creature_sprite_height;
			unsigned int _creature_sprite_spacing;
			float _creature_sprite_scaling;
			sf::Texture _attract_mode_texture;
			sf::Texture _gui_frame_texture;
			sf::Font _system_font;
	};
}