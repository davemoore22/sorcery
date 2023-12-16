// Copyright (C) 2023 Dave Moore
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

#include "core/display.hpp"
#include "core/game.hpp"
#include "core/graphics.hpp"
#include "core/layout.hpp"
#include "core/system.hpp"
#include "engine/inspect.hpp"
#include "gui/console.hpp"
#include "gui/frame.hpp"
#include "gui/menu.hpp"
#include "gui/partypanel.hpp"
#include "main.hpp"

namespace Sorcery {

class Temple {

	public:

		// Standard Constructor
		Temple(System *system, Display *display, Graphics *graphics, Game *game);
		Temple() = delete;

		// Standard Destructor
		~Temple();

		// Public Members

		// Public Methods
		auto start() -> std::optional<MenuItem>;
		auto stop() -> void;

	private:

		// Private Members
		System *_system;
		Display *_display;
		Graphics *_graphics;
		Game *_game;
		sf::RenderWindow *_window;
		std::unique_ptr<Frame> _frame;
		std::unique_ptr<Menu> _menu;
		sf::Sprite _bg;
		std::unique_ptr<PartyPanel> _party_panel;
		std::unique_ptr<Inspect> _inspect;
		TempleStage _stage;
		std::unique_ptr<Menu> _help;
		std::unique_ptr<Menu> _pay;
		std::map<std::string, sf::Sprite> _h_sprites;
		std::map<std::string, sf::Text> _h_texts;
		std::map<std::string, std::shared_ptr<Frame>> _h_frames;
		std::map<std::string, sf::Sprite> _p_sprites;
		std::map<std::string, sf::Text> _p_texts;
		std::map<std::string, std::shared_ptr<Frame>> _p_frames;
		std::map<std::string, sf::Sprite> _r_sprites;
		std::map<std::string, sf::Text> _r_texts;
		std::map<std::string, std::shared_ptr<Frame>> _r_frames;
		sf::Text _cost;
		sf::Text _ress_count;
		sf::Text _ress_result;
		std::string _cost_text;
		std::string _ress_text;
		std::string _result_text;
		std::jthread _count_thread;
		std::atomic<bool> _allow_count;
		std::chrono::time_point<std::chrono::system_clock> _last_count;
		std::chrono::time_point<std::chrono::system_clock> _current_time;
		std::mutex _count_mutex;
		unsigned int _t_count;
		bool _t_finished;
		unsigned int _duration;
		std::unique_ptr<Menu> _continue_menu;
		std::unique_ptr<Console> _console;
		bool _show_console;

		// Private Methods
		auto _draw() -> void;
		auto _refresh_pay_menu(const unsigned int cost) -> void;
		auto _update_cost(const unsigned int cost) -> void;
		auto _thread_count() -> void;
		auto _do_count() -> void;
		auto _start_count() -> void;
		auto _stop_count() -> void;
		auto _start_count_thread() -> void;
		auto _stop_count_thread() -> void;
		auto _set_ress_status() -> void;
		auto _get_ress_status() -> std::string;
		auto _get_ress_count() -> unsigned int;
		auto _try_cure_or_ress(unsigned int heal_char_id, unsigned int pay_char_id) -> bool;
};

}
