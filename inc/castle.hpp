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

#include "dialog.hpp"
#include "display.hpp"
#include "edgeoftown.hpp"
#include "frame.hpp"
#include "graphics.hpp"
#include "layout.hpp"
#include "main.hpp"
#include "menu.hpp"
#include "statusbar.hpp"
#include "system.hpp"

namespace Sorcery {

	class Castle {

	  public:
		// Standard Constructor
		Castle(System *system, Display *display, Graphics *graphics);
		Castle() = delete;

		// Standard Destructor
		~Castle();

		// Public Members

		// Public Methods
		auto start() -> std::optional<MenuItem>;
		auto stop() -> void;

	  private:
		// Private Methods
		auto _draw() -> void;

		// Private Members
		System *_system;
		Display *_display;
		Graphics *_graphics;
		sf::RenderWindow *_window;
		std::unique_ptr<Frame> _frame;
		std::shared_ptr<Menu> _menu;
		sf::Sprite _bg;
		std::shared_ptr<Dialog> _dialog_leave_game;
		WindowConfirm _yes_or_no;
		std::shared_ptr<EdgeOfTown> _edge_of_town;
		std::unique_ptr<StatusBar> _status_bar;
	};
} // namespace Sorcery
