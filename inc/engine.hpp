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

#include "automap.hpp"
#include "buffbar.hpp"
#include "compass.hpp"
#include "console.hpp"
#include "dialog.hpp"
#include "display.hpp"
#include "game.hpp"
#include "graphics.hpp"
#include "iconpanel.hpp"
#include "inspect.hpp"
#include "main.hpp"
#include "map.hpp"
#include "menu.hpp"
#include "message.hpp"
#include "options.hpp"
#include "render.hpp"
#include "reorder.hpp"
#include "search.hpp"
#include "statusbar.hpp"
#include "system.hpp"

namespace Sorcery {

class Engine {

	public:

		// Standard Constructor - need to pass in game object to this to
		Engine(System *system, Display *display, Graphics *graphics, Game *game);
		Engine() = delete;

		// Standard Destructor
		~Engine() = default;

		// Public Members

		// Public Methods
		auto start() -> int;
		auto stop() const -> void;

	private:

		// Private Methods
		auto _initalise_components() -> void;
		auto _place_components() -> void;
		auto _initialise_state() -> void;
		auto _refresh() const -> void;
		auto _set_maze_entry_start() -> void;
		auto _update_timers_and_components() -> void;
		auto _check_for_pending_events() -> void;
		auto _do_pause(sf::Event &event) -> void;
		auto _handle_confirm_exit(const sf::Event &event) -> void;
		auto _handle_in_character(const sf::Event &event) -> void;
		auto _handle_in_camp(const sf::Event &event) -> std::optional<int>;
		auto _handle_elevator_a_d(const sf::Event &event) -> std::optional<int>;
		auto _handle_elevator_a_f(const sf::Event &event) -> std::optional<int>;
		auto _handle_in_game(const sf::Event &event) -> std::optional<int>;
		auto _handle_in_search(const sf::Event &event) -> std::optional<int>;
		auto _handle_in_action(const sf::Event &event) -> std::optional<int>;
		auto _handle_in_map(const sf::Event &event) -> std::optional<int>;
		auto _handle_in_get(const sf::Event &event) -> std::optional<int>;
		auto _refresh_display() -> void;
		auto _update_display() -> void;
		auto _draw() -> void;
		auto _move_forward() -> bool;
		auto _move_backward() -> bool;
		auto _turn_left() -> void;
		auto _turn_right() -> void;
		auto _turn_around() -> void;
		auto _teleport_if() -> bool;
		auto _stairs_if() -> bool;
		auto _spinner_if() const -> bool;
		auto _pit_if() -> bool;
		auto _chute_if() -> bool;
		auto _elevator_if() -> bool;
		auto _tile_explored(const Coordinate loc) const -> bool;
		auto _set_tile_explored(const Coordinate loc) -> void;
		auto _reset_direction_indicator() -> void;
		auto _update_direction_indicator_timer() -> void;
		auto _is_mouse_over(sf::IntRect rect, sf::Vector2f mouse_pos) const -> bool;
		auto _unpoison_characters_on_return_to_town() -> void;
		auto _move_characters_to_temple_if_needed() -> void;

		// Private Members
		System *_system;
		Display *_display;
		Graphics *_graphics;
		Game *_game;
		sf::RenderWindow *_window;
		std::unique_ptr<Menu> _camp_menu;
		std::unique_ptr<Menu> _search_menu;
		std::unique_ptr<Menu> _action_menu;
		std::unique_ptr<Menu> _get_menu;
		std::unique_ptr<Menu> _elevator_a_d_menu;
		std::unique_ptr<Menu> _elevator_a_f_menu;
		std::unique_ptr<Frame> _camp_menu_frame;
		std::unique_ptr<Frame> _search_menu_frame;
		std::unique_ptr<Frame> _action_menu_frame;
		std::unique_ptr<Frame> _elevator_a_d_menu_frame;
		std::unique_ptr<Frame> _elevator_a_f_menu_frame;
		std::unique_ptr<Frame> _get_menu_frame;
		std::unique_ptr<StatusBar> _status_bar;
		std::unique_ptr<Dialog> _confirm_exit;
		std::unique_ptr<Dialog> _ouch;
		std::unique_ptr<Dialog> _pit;
		std::unique_ptr<Dialog> _chute;
		std::unique_ptr<Dialog> _elevator;
		std::unique_ptr<Reorder> _reorder;
		std::unique_ptr<Inspect> _inspect;
		std::unique_ptr<Render> _render;
		std::unique_ptr<AutoMap> _automap;
		std::unique_ptr<Compass> _compass;
		std::unique_ptr<Console> _console;
		std::unique_ptr<IconPanel> _left_icon_panel;
		std::unique_ptr<IconPanel> _right_icon_panel;
		std::unique_ptr<Message> _tile_note;
		std::unique_ptr<Dialog> _confirm_stairs;
		std::unique_ptr<BuffBar> _buffbar;
		std::unique_ptr<Search> _search;
		std::unique_ptr<Map> _map;
		bool _in_camp;
		bool _in_action;
		bool _in_search;
		bool _in_character;
		bool _in_map;
		bool _in_get;
		bool _show_confirm_exit;
		bool _show_confirm_stairs;
		bool _show_console;
		bool _show_ouch;
		bool _show_pit;
		bool _show_chute;
		bool _show_elevator;
		bool _show_tile_note;
		bool _show_status;
		bool _show_gui;
		bool _in_elevator_a_d;
		bool _in_elevator_a_f;
		bool _update_automap;
		bool _update_compass;
		bool _update_buffbar;
		bool _update_icon_panels;
		bool _update_status_bar;
		bool _update_render;
		bool _update_search;
		bool _update_tile_note;
		bool _update_get;
		bool _exit_maze_now;
		bool _pending_chute;
		bool _pending_elevator;
		std::optional<Character *> _cur_char;
		std::unique_ptr<Frame> _cur_char_frame;
		MapDirection _last_movement;
		int _destination_floor;
		sf::IntRect _search_bounds;
		std::optional<std::chrono::time_point<std::chrono::system_clock>> _direction_start;
		std::optional<std::chrono::time_point<std::chrono::system_clock>> _direction_current_time;
		bool _show_direction_indicatior;
		std::optional<std::vector<MenuEntry>::const_iterator> _get_option;
		std::optional<std::vector<MenuEntry>::const_iterator> _camp_option;
		std::optional<std::vector<MenuEntry>::const_iterator> _action_option;
		std::optional<std::vector<MenuEntry>::const_iterator> _search_option;
		std::optional<std::vector<MenuEntry>::const_iterator> _elevator_a_d_option;
		std::optional<std::vector<MenuEntry>::const_iterator> _elevator_a_f_option;
};

}
