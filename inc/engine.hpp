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
#include "debuffbar.hpp"
#include "dialog.hpp"
#include "display.hpp"
#include "event.hpp"
#include "game.hpp"
#include "graphics.hpp"
#include "graveyard.hpp"
#include "iconpanel.hpp"
#include "inspect.hpp"
#include "main.hpp"
#include "map.hpp"
#include "menu.hpp"
#include "options.hpp"
#include "partypanel.hpp"
#include "render.hpp"
#include "reorder.hpp"
#include "search.hpp"
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
		auto _generate_display() -> void;
		auto _reset_components() -> void;
		auto _initalise_components() -> void;
		auto _place_components() -> void;
		auto _initialise_state() -> void;
		auto _refresh() const -> void;
		auto _set_maze_entry_start() -> void;
		auto _update_timers_and_components() -> void;
		auto _check_for_pending_events() -> void;
		auto _do_pause(sf::Event &event) -> void;
		auto _handle_confirm_exit(const sf::Event &event) -> void;
		auto _handle_confirm_search(const sf::Event &event) -> bool;
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
		auto _event_if() -> bool;
		auto _find_an_item(const std::string name) -> void;
		auto _tile_explored(const Coordinate loc) const -> bool;
		auto _set_tile_explored(const Coordinate loc) -> void;
		auto _reset_direction_indicator() -> void;
		auto _update_direction_indicator_timer() -> void;
		auto _is_mouse_over(sf::IntRect rect, sf::Vector2f mouse_pos) const -> bool;
		auto _unpoison_characters_on_return_to_town() -> void;
		auto _move_characters_to_temple_if_needed() -> void;
		auto _pit_oops() -> void;
		auto _check_for_wipe() const -> bool;
		auto _do_wipe() -> int;
		auto _go_back() -> std::optional<int>;
		auto _set_refresh_ui() -> void;
		auto _unhightlight_panels() -> void;

		auto _debug_go_back() -> std::optional<int>;
		auto _debug_give_first_character_gold_xp() -> std::optional<int>;
		auto _debug_give_party_random_status() -> std::optional<int>;
		auto _debug_give_party_random_hp() -> std::optional<int>;
		auto _debug_go_to_graveyard() -> std::optional<int>;
		auto _debug_go_down_a_level() -> std::optional<int>;
		auto _debug_go_up_a_level() -> std::optional<int>;
		auto _debug_heal_party_to_full() -> std::optional<int>;
		auto _debug_send_non_party_characters_to_tavern() -> std::optional<int>;
		auto _debug_kill_non_party_characters() -> std::optional<int>;
		auto _debug_level_first_character_down() -> std::optional<int>;
		auto _debug_level_first_character_up() -> std::optional<int>;
		auto _debug_light_on() -> std::optional<int>;
		auto _debug_light_off() -> std::optional<int>;
		auto _debug_monochrome_wireframe() -> std::optional<int>;
		auto _debug_colour_wireframe() -> std::optional<int>;
		auto _debug_set_quest_item_flags() -> std::optional<int>;
		auto _debug_clear_quest_item_flags() -> std::optional<int>;

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
		std::unique_ptr<Dialog> _confirm_exit;
		std::unique_ptr<Dialog> _ouch;
		std::unique_ptr<Dialog> _pit;
		std::unique_ptr<Dialog> _chute;
		std::unique_ptr<Dialog> _elevator;
		std::unique_ptr<Dialog> _confirm_search;
		std::unique_ptr<Dialog> _found_an_item;
		std::unique_ptr<Reorder> _reorder;
		std::unique_ptr<Inspect> _inspect;
		std::unique_ptr<Render> _render;
		std::unique_ptr<AutoMap> _automap;
		std::unique_ptr<Compass> _compass;
		std::unique_ptr<Console> _console;
		std::unique_ptr<IconPanel> _left_icon_panel;
		std::unique_ptr<IconPanel> _right_icon_panel;
		std::unique_ptr<Dialog> _confirm_stairs;
		std::unique_ptr<BuffBar> _buffbar;
		std::unique_ptr<DebuffBar> _debuffbar;
		std::unique_ptr<Search> _search;
		std::unique_ptr<Map> _map;
		std::unique_ptr<Graveyard> _graveyard;
		std::unique_ptr<Frame> _view_frame_big;
		std::unique_ptr<Frame> _view_frame_small;
		std::unique_ptr<PartyPanel> _party_panel;
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
		bool _show_found_an_item;
		bool _show_elevator;
		bool _show_party_panel;
		bool _show_confirm_search;
		bool _show_gui;
		bool _in_elevator_a_d;
		bool _in_elevator_a_f;
		bool _update_automap;
		bool _update_compass;
		bool _update_buffbar;
		bool _update_debuffbar;
		bool _update_icon_panels;
		bool _update_party_panel;
		bool _update_render;
		bool _update_search;
		bool _update_get;
		bool _exit_maze_now;
		bool _pending_chute;
		bool _pending_elevator;
		bool _pending_combat;
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
		bool _monochrome;
		bool _can_run_event;
		bool _display_cursor;
		bool _can_go_back;
};

}
