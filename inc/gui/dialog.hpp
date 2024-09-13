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

#include "common/include.hpp"
#include "gui/frame.hpp"
#include "types/component.hpp"

namespace Sorcery {

// Forward Declarations
class Display;
class Graphics;
class System;

class Dialog: public sf::Transformable, public sf::Drawable {

	public:

		// Constructors
		Dialog(
			System *system, Display *display, Graphics *graphics, Component &_frame_c, Component &string_c, WDT type);
		Dialog() = delete;

		// Public Members

		// Public Methods
		auto check_for_mouse_move(const sf::Vector2f mouse_pos) -> std::optional<WDB>;
		auto check_if_option_selected(const sf::Vector2f mouse_pos) -> std::optional<WDB>;
		auto get_selected() const -> WDB;
		auto set_selected(WDB value) -> void;
		auto toggle_highlighted() -> WDB;
		auto update() -> void;
		auto handle_input(sf::Event event) -> std::optional<WDB>;
		auto get_valid() const -> bool;
		auto set_valid(const bool valid) -> void;
		auto reset_timed() -> void;
		auto set(Component &string_c, const std::string &new_text = "") -> void;
		auto set_duration(unsigned int duration) -> void;
		auto get_width() const -> unsigned int;
		auto get_height() const -> unsigned int;
		auto get_size() const -> sf::Vector2f;

	private:

		// Private Methods
		auto virtual draw(sf::RenderTarget &target, sf::RenderStates states) const -> void;
		auto _refresh(Component &string_c, const std::string &new_text) -> void;

		// Private Members
		System *_system;
		Display *_display;
		Graphics *_graphics;

		sf::RenderWindow *_window;

		WDB _selected;

		Component _frame_c;
		Component _string_c;

		Component _text_c;
		Component _buttons_c;

		WDT _type;

		std::vector<std::string> _strings;
		std::vector<sf::Sprite> _sprites;
		std::vector<sf::Text> _texts;
		std::vector<sf::FloatRect> _rects;
		std::map<WDB, sf::Text> _buttons;
		std::map<WDB, sf::Text> _buttons_hl;
		std::map<WDB, sf::FloatRect> _buttons_fr;
		std::map<WDB, sf::RectangleShape> _highlights;

		std::unique_ptr<Frame> _frame;

		std::optional<std::chrono::time_point<std::chrono::system_clock>> _start;
		std::optional<std::chrono::time_point<std::chrono::system_clock>> _current_time;
		bool _valid;
		unsigned int _duration;

		int _width;
		int _height;
};

}