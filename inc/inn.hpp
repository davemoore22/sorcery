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

#include "dialog.hpp"
#include "display.hpp"
#include "frame.hpp"
#include "game.hpp"
#include "graphics.hpp"
#include "inspect.hpp"
#include "layout.hpp"
#include "main.hpp"
#include "menu.hpp"
#include "rest.hpp"
#include "statusbar.hpp"
#include "system.hpp"

namespace Sorcery {

	class Inn {

	  public:
		// Standard Constructor
		Inn(System *system, Display *display, Graphics *graphics, Game *game);
		Inn() = delete;

		// Public Members

		// Public Methods
		auto start() -> std::optional<MenuItem>;
		auto stop() -> void;

	  private:
		// Private Methods
		auto _draw() -> void;
		auto _update_and_draw_bed_screen() -> void;

		// Private Members
		System *_system;
		Display *_display;
		Graphics *_graphics;
		Game *_game;
		sf::RenderWindow *_window;
		std::unique_ptr<Frame> _frame;
		std::unique_ptr<Menu> _menu;
		sf::Sprite _bg;
		std::unique_ptr<StatusBar> _status_bar;
		std::unique_ptr<Inspect> _inspect;
		InnStage _stage;
		std::unique_ptr<Menu> _roster;
		std::unique_ptr<Menu> _bed;
		int _cur_char_id;
		std::optional<Character *> _cur_char;
		sf::Text _welcome_text;
		sf::Text _gold_text;
		bool _update;
		std::unique_ptr<Dialog> _pool;
		bool _show_pool;
		std::unique_ptr<Rest> _rest;
		std::map<std::string, sf::Sprite> _w_sprites;
		std::map<std::string, sf::Text> _w_texts;
		std::map<std::string, std::shared_ptr<Frame>> _w_frames;
		std::map<std::string, sf::Sprite> _c_sprites;
		std::map<std::string, sf::Text> _c_texts;
		std::map<std::string, std::shared_ptr<Frame>> _c_frames;
		std::map<std::string, sf::Sprite> _b_sprites;
		std::map<std::string, sf::Text> _b_texts;
		std::map<std::string, std::shared_ptr<Frame>> _b_frames;
	};
} // namespace Sorcery
