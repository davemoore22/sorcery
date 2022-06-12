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

#include "display.hpp"
#include "frame.hpp"
#include "graphics.hpp"
#include "main.hpp"
#include "system.hpp"

namespace Sorcery {

	class Confirm {

	  public:
		// Constructors
		Confirm(System *system, Display *display, Graphics *graphics, Component &gui_c, Component &text_c);
		Confirm() = delete;

		// Public Members
		WindowConfirm highlighted;

		// Public Methods
		auto check_for_mouse_move(const sf::Vector2f mouse_pos) -> std::optional<WindowConfirm>;
		auto check_if_option_selected(const sf::Vector2f mouse_pos) -> std::optional<WindowConfirm>;
		auto toggle_highlighted() -> WindowConfirm;
		auto draw(const double lerp) -> void;

	  private:
		// Private Methods

		// Private Members
		System *_system;
		Display *_display;
		Graphics *_graphics;
		Component _gui_c;
		Component _text_c;
		sf::RenderWindow *_window;
		std::string_view _text;
		sf::FloatRect _no_bg_tect;
		sf::FloatRect _yes_bg_rect;
		sf::Text _no_text;
		sf::Text _yes_text;
		std::unique_ptr<Frame> _frame;
		std::vector<std::string> _strings;
		std::vector<sf::Text> _texts;
	};
} // namespace Sorcery