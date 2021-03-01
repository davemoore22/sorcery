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

#include "confirm.hpp"
#include "display.hpp"
#include "frame.hpp"
#include "graphics.hpp"
#include "layout.hpp"
#include "main.hpp"
#include "menu.hpp"
#include "system.hpp"
#include "tooltip.hpp"

namespace Sorcery {

	class Options {

	  public:
		// Constructors
		Options(System &system, Display &display, Graphics &graphics);
		Options() = delete;

		// Destructor
		~Options();

		// Public Members

		// Public Methods
		auto start() -> void;
		auto stop() -> void;

	  private:
		// Private Methods
		auto _draw() -> void;
		auto _set_tooltip(sf::Vector2f mouse_pos) -> bool;

		// Private Members
		System _system;
		Display _display;
		Graphics _graphics;
		sf::RenderWindow *_window;
		sf::Text _save_text;
		sf::Text _cancel_text;
		sf::Text _reset_text;
		Component _option_on;
		Component _option_off;
		std::shared_ptr<Menu> _menu;
		std::shared_ptr<Tooltip> _tt;
		bool _display_tt;
		std::shared_ptr<Confirm> _confirm_save;
		std::shared_ptr<Confirm> _confirm_cancel;
		std::shared_ptr<Confirm> _confirm_strict_on;
		WindowConfirm _yes_or_no;
	};
} // namespace Sorcery
