// Copyright (C) 2021 Dave Moore
//
// This file is part of Sorcery: Dreams of the Mad Overlord.
//
// Sorcery: Dreams of the Mad Overlord is free software: you can redistribute
// it and/or modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation, either version 2 of the License,
// or (at your option) any later version.
//
// Sorcery: Dreams of the Mad Overlord is distributed in the hope that it wil
// be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
// Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Sorcery: Dreams of the Mad Overlord.  If not,
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
		Confirm(
			System &system, Display &display, Graphics &graphics, Component &gui_component, Component &text_component);
		Confirm() = delete;

		// Public Members
		WindowConfirm currently_highlighted;

		// Public Methods
		auto check_for_mouse_move(sf::Vector2f mouse_position) -> std::optional<WindowConfirm>;
		auto check_if_option_selected(sf::Vector2f mouse_position) -> std::optional<WindowConfirm>;
		auto toggle_highlighted() -> WindowConfirm;
		auto draw(double lerp) -> void;

	  private:
		// Private Methods

		// Private Members
		System _system;
		Display _display;
		Graphics _graphics;
		Component _gui_component;
		Component _text_component;
		sf::RenderWindow *_window;
		std::string_view _text;
		sf::FloatRect _no_background_rect;
		sf::FloatRect _yes_background_rect;
		sf::Text _no_text;
		sf::Text _yes_text;
		std::unique_ptr<Frame> _frame;
	};
} // namespace Sorcery