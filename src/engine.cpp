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

#include "engine.hpp"

// Standard Constructor
Sorcery::Engine::Engine(System *system, Display *display, Graphics *graphics, Game *game)
	: _system{system}, _display{display}, _graphics{graphics}, _game{game} {

	_game->hide_console();
}

auto Sorcery::Engine::_generate_display() -> void {

	_reset_components();
	_initalise_components();
	_display->generate("engine_base_ui");
	_refresh();
	_place_components();
}

auto Sorcery::Engine::_initialise_state() -> void {

	_update_automap = false;
	_update_compass = false;
	_update_buffbar = false;
	_update_icon_panels = false;
	_update_status_bar = false;
	_update_render = false;
	_update_search = false;
	_update_tile_note = false;
	_exit_maze_now = false;
	_pending_chute = false;
	_pending_elevator = false;
	_show_status = true;
	_show_gui = true;
	_display_cursor = true;

	_monochrome = true;
}

auto Sorcery::Engine::_reset_components() -> void {

	if (_camp_menu.get()) {
		_camp_menu.release();
		_camp_menu.reset();
	}
	if (_camp_menu_frame.get()) {
		_camp_menu_frame.release();
		_camp_menu_frame.reset();
	}
	if (_search_menu.get()) {
		_search_menu.release();
		_search_menu.reset();
	}
	if (_search_menu_frame.get()) {
		_search_menu_frame.release();
		_search_menu_frame.reset();
	}
	if (_get_menu.get()) {
		_get_menu.release();
		_get_menu.reset();
	}
	if (_get_menu_frame.get()) {
		_get_menu_frame.release();
		_get_menu_frame.reset();
	}
	if (_action_menu.get()) {
		_action_menu.release();
		_action_menu.reset();
	}
	if (_action_menu.get()) {
		_action_menu.release();
		_action_menu.reset();
	}
	if (_elevator_a_d_menu.get()) {
		_elevator_a_d_menu.release();
		_elevator_a_d_menu.reset();
	}
	if (_elevator_a_d_menu_frame.get()) {
		_elevator_a_d_menu_frame.release();
		_elevator_a_d_menu_frame.reset();
	}
	if (_elevator_a_f_menu.get()) {
		_elevator_a_f_menu.release();
		_elevator_a_f_menu.reset();
	}
	if (_elevator_a_f_menu_frame.get()) {
		_elevator_a_f_menu_frame.release();
		_elevator_a_f_menu_frame.reset();
	}
	if (_confirm_exit.get()) {
		_confirm_exit.release();
		_confirm_exit.reset();
	}
	if (_confirm_stairs.get()) {
		_confirm_stairs.release();
		_confirm_stairs.reset();
	}
	if (_confirm_search.get()) {
		_confirm_search.release();
		_confirm_search.reset();
	}
	if (_ouch.get()) {
		_ouch.release();
		_ouch.reset();
	}
	if (_pit.get()) {
		_pit.release();
		_pit.reset();
	}

	if (_chute.get()) {
		_chute.release();
		_chute.reset();
	}

	if (_found_an_item.get()) {
		_found_an_item.release();
		_found_an_item.reset();
	}

	if (_elevator.get()) {
		_elevator.release();
		_elevator.reset();
	}

	if (_status_bar.get()) {
		_status_bar.release();
		_status_bar.reset();
	}
	if (_reorder.get()) {
		_reorder.release();
		_reorder.reset();
	}
	if (_inspect.get()) {
		_inspect.release();
		_inspect.reset();
	}
	if (_render.get()) {
		_render.release();
		_render.reset();
	}
	if (_graveyard.get()) {
		_graveyard.release();
		_graveyard.reset();
	}
	if (_automap.get()) {
		_automap.release();
		_automap.reset();
	}
	if (_compass.get()) {
		_compass.release();
		_compass.reset();
	}
	if (_buffbar.get()) {
		_buffbar.release();
		_buffbar.reset();
	}
	if (_search.get()) {
		_search.release();
		_search.reset();
	}
	if (_left_icon_panel.get()) {
		_left_icon_panel.release();
		_left_icon_panel.reset();
	}
	if (_right_icon_panel.get()) {
		_right_icon_panel.release();
		_right_icon_panel.reset();
	}
	if (_map.get()) {
		_map.release();
		_map.reset();
	}
	if (_tile_note.get()) {
		_tile_note.release();
		_tile_note.reset();
	}
}

auto Sorcery::Engine::_initalise_components() -> void {

	// Get the Window and Graphics to Display
	_window = _display->window->get_window();

	// Setup Custom Components
	_camp_menu = std::make_unique<Menu>(_system, _display, _graphics, _game, MenuType::CAMP);
	_camp_menu->generate((*_display->layout)["engine_base_ui:camp_menu"]);
	_camp_menu->setPosition(
		_display->get_centre_x(_camp_menu->get_width()), (*_display->layout)["engine_base_ui:camp_menu"].y);

	_search_menu = std::make_unique<Menu>(_system, _display, _graphics, _game, MenuType::SEARCH);
	_search_menu->generate((*_display->layout)["engine_base_ui:search_menu"]);
	_search_menu->setPosition(
		_display->get_centre_x(_search_menu->get_width()), (*_display->layout)["engine_base_ui:search_menu"].y);

	_get_menu = std::make_unique<Menu>(_system, _display, _graphics, _game, MenuType::CHARACTERS_HERE);
	_get_menu->generate((*_display->layout)["engine_base_ui:get_menu"]);
	_get_menu->setPosition(
		_display->get_centre_x(_get_menu->get_width()), (*_display->layout)["engine_base_ui:get_menu"].y);

	_action_menu = std::make_unique<Menu>(_system, _display, _graphics, _game, MenuType::ACTION);
	_action_menu->generate((*_display->layout)["engine_base_ui:action_menu"]);
	_action_menu->setPosition(
		_display->get_centre_x(_action_menu->get_width()), (*_display->layout)["engine_base_ui:action_menu"].y);

	_elevator_a_d_menu = std::make_unique<Menu>(_system, _display, _graphics, _game, MenuType::ELEVATOR_A_D);
	_elevator_a_d_menu->generate((*_display->layout)["engine_base_ui:elevator_a_d_menu"]);
	_elevator_a_d_menu->setPosition(_display->get_centre_x(_elevator_a_d_menu->get_width()),
		(*_display->layout)["engine_base_ui:elevator_a_d_menu"].y);

	_elevator_a_f_menu = std::make_unique<Menu>(_system, _display, _graphics, _game, MenuType::ELEVATOR_A_F);
	_elevator_a_f_menu->generate((*_display->layout)["engine_base_ui:elevator_a_f_menu"]);
	_elevator_a_f_menu->setPosition(_display->get_centre_x(_elevator_a_f_menu->get_width()),
		(*_display->layout)["engine_base_ui:elevator_a_f_menu"].y);

	const Component menu_fc{(*_display->layout)["engine_base_ui:camp_menu_frame"]};
	_camp_menu_frame = std::make_unique<Frame>(
		_display->ui_texture, menu_fc.w, menu_fc.h, menu_fc.colour, menu_fc.background, menu_fc.alpha);
	_camp_menu_frame->setPosition(_display->window->get_x(_camp_menu_frame->sprite, menu_fc.x),
		_display->window->get_y(_camp_menu_frame->sprite, menu_fc.y));

	const Component search_fc{(*_display->layout)["engine_base_ui:search_menu_frame"]};
	_search_menu_frame = std::make_unique<Frame>(
		_display->ui_texture, search_fc.w, search_fc.h, search_fc.colour, search_fc.background, search_fc.alpha);
	_search_menu_frame->setPosition(_display->window->get_x(_search_menu_frame->sprite, search_fc.x),
		_display->window->get_y(_search_menu_frame->sprite, search_fc.y));

	const Component get_fc{(*_display->layout)["engine_base_ui:get_menu_frame"]};
	_get_menu_frame = std::make_unique<Frame>(
		_display->ui_texture, get_fc.w, get_fc.h, get_fc.colour, get_fc.background, get_fc.alpha);
	_get_menu_frame->setPosition(_display->window->get_x(_get_menu_frame->sprite, get_fc.x),
		_display->window->get_y(_get_menu_frame->sprite, get_fc.y));

	const Component action_fc{(*_display->layout)["engine_base_ui:action_menu_frame"]};
	_action_menu_frame = std::make_unique<Frame>(
		_display->ui_texture, action_fc.w, action_fc.h, action_fc.colour, action_fc.background, action_fc.alpha);
	_action_menu_frame->setPosition(_display->window->get_x(_action_menu_frame->sprite, action_fc.x),
		_display->window->get_y(_action_menu_frame->sprite, action_fc.y));

	const Component elevator_a_d_fc{(*_display->layout)["engine_base_ui:elevator_a_d_menu_frame"]};
	_elevator_a_d_menu_frame = std::make_unique<Frame>(_display->ui_texture, elevator_a_d_fc.w, elevator_a_d_fc.h,
		elevator_a_d_fc.colour, elevator_a_d_fc.background, elevator_a_d_fc.alpha);
	_elevator_a_d_menu_frame->setPosition(_display->window->get_x(_elevator_a_d_menu_frame->sprite, elevator_a_d_fc.x),
		_display->window->get_y(_elevator_a_d_menu_frame->sprite, elevator_a_d_fc.y));

	const Component elevator_a_f_fc{(*_display->layout)["engine_base_ui:elevator_a_f_menu_frame"]};
	_elevator_a_f_menu_frame = std::make_unique<Frame>(_display->ui_texture, elevator_a_f_fc.w, elevator_a_f_fc.h,
		elevator_a_f_fc.colour, elevator_a_f_fc.background, elevator_a_f_fc.alpha);
	_elevator_a_f_menu_frame->setPosition(_display->window->get_x(_elevator_a_f_menu_frame->sprite, elevator_a_f_fc.x),
		_display->window->get_y(_elevator_a_f_menu_frame->sprite, elevator_a_f_fc.y));

	_ouch = std::make_unique<Dialog>(_system, _display, _graphics, (*_display->layout)["engine_base_ui:ouch"],
		(*_display->layout)["engine_base_ui:ouch_text"], WindowDialogType::TIMED);
	_ouch->setPosition(_display->get_centre_pos(_ouch->get_size()));
	_ouch->set_duration(DELAY_OUCH);

	_pit = std::make_unique<Dialog>(_system, _display, _graphics, (*_display->layout)["engine_base_ui:pit"],
		(*_display->layout)["engine_base_ui:pit_text"], WindowDialogType::TIMED);
	_pit->setPosition(_display->get_centre_pos(_pit->get_size()));
	_pit->set_duration(DELAY_PIT);

	_chute = std::make_unique<Dialog>(_system, _display, _graphics, (*_display->layout)["engine_base_ui:chute"],
		(*_display->layout)["engine_base_ui:chute_text"], WindowDialogType::TIMED);
	_chute->setPosition(_display->get_centre_pos(_chute->get_size()));
	_chute->set_duration(DELAY_CHUTE);

	_found_an_item =
		std::make_unique<Dialog>(_system, _display, _graphics, (*_display->layout)["engine_base_ui:found_an_item"],
			(*_display->layout)["engine_base_ui:found_an_item_text"], WindowDialogType::TIMED);
	_found_an_item->setPosition(_display->get_centre_pos(_found_an_item->get_size()));
	_found_an_item->set_duration(DELAY_FIND_AN_ITEM);

	_elevator = std::make_unique<Dialog>(_system, _display, _graphics, (*_display->layout)["engine_base_ui:one_moment"],
		(*_display->layout)["engine_base_ui:one_moment_text"], WindowDialogType::TIMED);
	_elevator->setPosition(_display->get_centre_pos(_elevator->get_size()));
	_elevator->set_duration(DELAY_ELEVATOR);

	_confirm_stairs =
		std::make_unique<Dialog>(_system, _display, _graphics, (*_display->layout)["engine_base_ui:dialog_stairs"],
			(*_display->layout)["engine_base_ui:dialog_ladder_up_text"], WindowDialogType::CONFIRM);
	_confirm_stairs->setPosition(_display->get_centre_pos(_confirm_stairs->get_size()));

	_confirm_exit =
		std::make_unique<Dialog>(_system, _display, _graphics, (*_display->layout)["engine_base_ui:dialog_exit"],
			(*_display->layout)["engine_base_ui:dialog_exit_text"], WindowDialogType::CONFIRM);
	_confirm_exit->setPosition(_display->get_centre_pos(_confirm_exit->get_size()));

	_confirm_search =
		std::make_unique<Dialog>(_system, _display, _graphics, (*_display->layout)["engine_base_ui:dialog_search"],
			(*_display->layout)["engine_base_ui:dialog_search_text"], WindowDialogType::CONFIRM);
	_confirm_search->setPosition(_display->get_centre_pos(_confirm_search->get_size()));

	// Modules
	_status_bar = std::make_unique<StatusBar>(_system, _display, _graphics, _game,
		(*_display->layout)["engine_base_ui:status_bar"], (*_display->layout)["engine_base_ui:status_bar_outer_frame"]);
	_reorder = std::make_unique<Reorder>(_system, _display, _graphics, _game, MenuMode::CAMP);
	_inspect = std::make_unique<Inspect>(_system, _display, _graphics, _game, MenuMode::CAMP);
	_render = std::make_unique<Render>(_system, _display, _graphics, _game);
	_graveyard = std::make_unique<Graveyard>(_system, _display, _graphics, _game);
	_automap = std::make_unique<AutoMap>(_system, _display, _graphics, _game, (*_display->layout)["global:automap"]);
	_compass = std::make_unique<Compass>(_system, _display, _graphics, _game, (*_display->layout)["global:compass"]);
	_buffbar = std::make_unique<BuffBar>(_system, _display, _graphics, _game, (*_display->layout)["global:buffbar"]);
	_search = std::make_unique<Search>(_system, _display, _graphics, _game, (*_display->layout)["global:search"]);
	_console = std::make_unique<Console>(_display->window->get_gui(), _system, _display, _graphics, _game);
	_left_icon_panel = std::make_unique<IconPanel>(
		_system, _display, _graphics, _game, (*_display->layout)["engine_base_ui:left_icon_panel"], true);
	_right_icon_panel = std::make_unique<IconPanel>(
		_system, _display, _graphics, _game, (*_display->layout)["engine_base_ui:right_icon_panel"], false);
	_map = std::make_unique<Map>(_system, _display, _graphics, _game, (*_display->layout)["engine_base_ui:map"]);
	_tile_note = std::make_unique<Message>(_system, _display, _graphics,
		(*_display->layout)["engine_base_ui:message_panel"], (*_display->layout)["engine_base_ui:message_text"]);
}

auto Sorcery::Engine::_update_direction_indicator_timer() -> void {

	if (_show_direction_indicatior) {
		if (!_direction_start)
			_direction_start = std::chrono::system_clock::now();

		_direction_current_time = std::chrono::system_clock::now();

		const auto time_elapsed{_direction_current_time.value() - _direction_start.value()};
		const auto time_elapsed_msec{std::chrono::duration_cast<std::chrono::milliseconds>(time_elapsed)};
		if (time_elapsed_msec.count() > DELAY_DIRECTION) {
			_show_direction_indicatior = false;
			_direction_start = std::nullopt;
		}
	}
}

auto Sorcery::Engine::_reset_direction_indicator() -> void {

	_show_direction_indicatior = true;
	_direction_start = std::nullopt;
	_direction_current_time = std::nullopt;
}

auto Sorcery::Engine::_place_components() -> void {

	// Generate the Custom Components
	const Component status_bar_c{(*_display->layout)["engine_base_ui:status_bar"]};
	_status_bar->setPosition(_display->window->get_x(_status_bar->sprite, status_bar_c.x),
		_display->window->get_y(_status_bar->sprite, status_bar_c.y));

	const Component automap_c{(*_display->layout)["global:automap"]};
	_automap->setPosition(automap_c.x, automap_c.y);
	const Component compass_c{(*_display->layout)["global:compass"]};
	_compass->setPosition(compass_c.x, compass_c.y);
	const Component buffbar_c{(*_display->layout)["global:buffbar"]};
	_buffbar->setPosition(buffbar_c.x, buffbar_c.y);
	const Component search_c{(*_display->layout)["global:search"]};
	_search->setPosition(search_c.x, search_c.y);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnarrowing"
	_search_bounds = sf::IntRect{search_c.x, search_c.y, _search->width, _search->height};
#pragma GCC diagnostic pop

	const Component l_icon_panel_c{(*_display->layout)["engine_base_ui:left_icon_panel"]};
	_left_icon_panel->setPosition(l_icon_panel_c.x, l_icon_panel_c.y);
	const Component r_icon_panel_c{(*_display->layout)["engine_base_ui:right_icon_panel"]};
	_right_icon_panel->setPosition(r_icon_panel_c.x, r_icon_panel_c.y);

	const Component map_c{(*_display->layout)["engine_base_ui:map"]};
	_map->setPosition(map_c.x, map_c.y);

	const Component t_n_c{(*_display->layout)["engine_base_ui:message"]};
	_tile_note->setPosition(t_n_c.x, t_n_c.y);

	const Component cc_fc{(*_display->layout)["engine_base_ui:character_frame"]};
	_cur_char_frame =
		std::make_unique<Frame>(_display->ui_texture, cc_fc.w, cc_fc.h, cc_fc.colour, cc_fc.background, cc_fc.alpha);
	_cur_char_frame->setPosition(_display->window->get_x(_cur_char_frame->sprite, cc_fc.x),
		_display->window->get_y(_cur_char_frame->sprite, cc_fc.y));
}

auto Sorcery::Engine::_refresh() const -> void {

	_render->refresh();
	_status_bar->refresh();
	_automap->refresh();
	_compass->refresh();
	_buffbar->refresh();
	_search->refresh();
	_map->refresh();
	_left_icon_panel->refresh(true);
	_right_icon_panel->refresh(true);
}

auto Sorcery::Engine::_tile_explored(const Coordinate loc) const -> bool {

	return _game->state->explored[_game->state->get_depth()].at(loc);
}

auto Sorcery::Engine::_set_tile_explored(const Coordinate loc) -> void {

	_game->state->explored[_game->state->get_depth()].set(loc);
}

auto Sorcery::Engine::_set_maze_entry_start() -> void {

	_in_get = false;
	_in_map = false;
	_in_camp = true;
	_in_search = false;
	_in_action = false;
	_in_character = false;
	_in_elevator_a_d = false;
	_in_elevator_a_f = false;
	_show_confirm_exit = false;
	_show_confirm_search = false;
	_show_ouch = false;
	_show_pit = false;
	_show_chute = false;
	_show_found_an_item = false;
	_show_tile_note = false;
	_show_elevator = false;
	_show_status = true;
	_show_gui = true;
	_exit_maze_now = false;
	_automap->refresh();
	_map->refresh();
	_system->stop_pause();
	_last_movement = MapDirection::NO_DIRECTION;
	_can_run_event = false;
	const auto &starting_tile{_game->state->level->at(_game->state->get_player_pos())};

	if (!_tile_explored(_game->state->get_player_pos()))
		_set_tile_explored(_game->state->get_player_pos());

	// Now, we can also be on an elevator or a set of stairs too when we begin
	_show_confirm_stairs = (_game->state->get_player_pos() == Coordinate{0, 0}) && (_game->state->get_depth() == -1);
	if ((_game->state->get_player_pos() == Coordinate{0, 0}) && (_game->state->get_depth() == -1)) {
		_show_confirm_stairs = true;
		_game->state->set_player_facing(MapDirection::NORTH);
		_game->state->set_lit(false);
		if (!_tile_explored(_game->state->get_player_pos()))
			_set_tile_explored(_game->state->get_player_pos());
	} else
		_show_confirm_stairs = false;

	_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
	_camp_option = _camp_menu->items.begin();
	_action_option = _camp_menu->items.begin();
	_search_option = _camp_menu->items.begin();

	auto has_elevator{starting_tile.has_elevator()};
	if (has_elevator) {

		// TODO: clunky need to fix this
		if (has_elevator.value().bottom_depth == -4) {
			_in_elevator_a_d = starting_tile.has(TileFeature::ELEVATOR);
			_elevator_a_d_option = _elevator_a_d_menu->items.end();
		} else if (has_elevator.value().bottom_depth == -9) {
			_in_elevator_a_f = starting_tile.has(TileFeature::ELEVATOR);
			_elevator_a_f_option = _elevator_a_f_menu->items.end();
		}
	}
}

auto Sorcery::Engine::_update_timers_and_components() -> void {

	_update_direction_indicator_timer();
	_ouch->update();
	_pit->update();
	_chute->update();
	_found_an_item->update();
	_elevator->update();
}

auto Sorcery::Engine::_check_for_pending_events() -> void {

	if (auto pending{_system->update_pause()}; pending) {
		if (_pending_chute) {

			const auto tile{_game->state->level->at(_game->state->get_player_pos())};
			if (tile.has(TileFeature::CHUTE)) {

				auto destination{tile.has_teleport().value()};
				auto dest_level{destination.to_level};
				Level level{((*_game->levelstore)[dest_level]).value()};
				_game->state->set_current_level(&level);
				_game->state->set_player_pos(destination.to_loc);
				_game->state->set_depth(dest_level);
				_set_tile_explored(_game->state->get_player_pos());
				_update_automap = true;
				_update_compass = true;
				_update_buffbar = true;
				_update_search = true;
				_update_render = true;
				_pending_chute = false;

				_display->set_disc(true);
				_refresh_display();
				_game->save_game();
				_display->set_disc(false);
			}
		} else if (_pending_elevator) {

			const auto tile{_game->state->level->at(_game->state->get_player_pos())};
			if (tile.has(TileFeature::ELEVATOR)) {

				Level level{((*_game->levelstore)[_destination_floor]).value()};
				_game->state->set_current_level(&level);
				_game->state->set_depth(_destination_floor);
				_set_tile_explored(_game->state->get_player_pos());
				_update_automap = true;
				_update_compass = true;
				_update_buffbar = true;
				_update_search = true;
				_update_render = true;
				_pending_elevator = false;

				_display->set_disc(true);
				_refresh_display();
				_game->save_game();
				_display->set_disc(false);

				_destination_floor = 0;
			}
		}
	}
}

auto Sorcery::Engine::_do_pause(sf::Event &event) -> void {

	_refresh_display();

	if (auto any_event{_window->pollEvent(event)}; any_event) {
		if (_system->input->check(WindowInput::ANYTHING, event))
			_system->stop_pause();
	}
}

auto Sorcery::Engine::_handle_confirm_search(const sf::Event &event) -> bool {

	if (_left_icon_panel->selected)
		_left_icon_panel->selected = std::nullopt;
	if (_right_icon_panel->selected)
		_right_icon_panel->selected = std::nullopt;
	if (_status_bar->selected)
		_status_bar->selected = std::nullopt;

	if (auto dialog_input{_confirm_search->handle_input(event)}; dialog_input) {
		if ((dialog_input.value() == WindowDialogButton::CLOSE) || (dialog_input.value() == WindowDialogButton::NO)) {
			_display->set_input_mode(WindowInputMode::IN_GAME);
			_show_confirm_search = false;
			return false;
		} else if (dialog_input.value() == WindowDialogButton::YES) {
			_display->set_input_mode(WindowInputMode::IN_GAME);
			_show_confirm_search = false;

			_show_found_an_item = true;

			// random character who has inventory free unless its a targeted search (TODO)
			const auto &character{_game->characters[_game->state->get_character_by_position(1).value()]};
			const auto text{fmt::format("{}{}", character.get_name(), (*_display->string)["FOUND_AN_ITEM"])};
			_found_an_item->set((*_display->layout)["engine_base_ui:found_an_item"], text);
			_found_an_item->reset_timed();
			return true;
		}
	}

	return false;
}

auto Sorcery::Engine::_handle_confirm_exit(const sf::Event &event) -> void {

	using enum Enums::Window::DialogButton;

	if (_left_icon_panel->selected)
		_left_icon_panel->selected = std::nullopt;
	if (_right_icon_panel->selected)
		_right_icon_panel->selected = std::nullopt;
	if (_status_bar->selected)
		_status_bar->selected = std::nullopt;

	auto dialog_input{_confirm_exit->handle_input(event)};
	if (dialog_input) {
		if ((dialog_input.value() == CLOSE) || (dialog_input.value() == NO)) {
			_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
			_show_confirm_exit = false;
		} else if (dialog_input.value() == YES) {
			_window->close();
		}
	}
}

auto Sorcery::Engine::_handle_in_character(const sf::Event &event) -> void {

	using enum Enums::Controls::Input;

	if (_system->input->check(SHOW_HIDE_CONSOLE, event))
		_game->toggle_console();
	else if (_system->input->check(LEFT, event))
		_cur_char.value()->left_view();
	else if (_system->input->check(RIGHT, event))
		_cur_char.value()->right_view();
	else if (_system->input->check(CANCEL, event)) {
		_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
		_cur_char = std::nullopt;
	} else if (_system->input->check(BACK, event)) {
		_display->set_input_mode(WindowInputMode::IN_GAME);
		_cur_char = std::nullopt;

		_display->set_disc(true);
		_refresh_display();
		_game->save_game();
		_display->set_disc(false);

		_status_bar->refresh();
		_in_character = false;
		_display->generate("engine_base_ui");
	} else if (_system->input->check(CONFIRM, event)) {
		_cur_char.value()->right_view();
	} else if (_system->input->check(UP, event)) {
		if (_cur_char.value()->get_view() == CharacterView::MAGE_SPELLS)
			_cur_char.value()->dec_hl_spell(SpellType::MAGE);
		else if (_cur_char.value()->get_view() == CharacterView::PRIEST_SPELLS)
			_cur_char.value()->dec_hl_spell(SpellType::PRIEST);

	} else if (_system->input->check(DOWN, event)) {
		if (_cur_char.value()->get_view() == CharacterView::MAGE_SPELLS)
			_cur_char.value()->inc_hl_spell(SpellType::MAGE);
		else if (_cur_char.value()->get_view() == CharacterView::PRIEST_SPELLS)
			_cur_char.value()->inc_hl_spell(SpellType::PRIEST);
	} else if (_system->input->check(MOVE, event)) {
		if (_cur_char.value()->check_for_mouse_move(sf::Vector2f(static_cast<float>(sf::Mouse::getPosition(*_window).x),
				static_cast<float>(sf::Mouse::getPosition(*_window).y)))) {
			_cur_char.value()->set_view(_cur_char.value()->get_view());
		} else {
			_status_bar->selected = std::nullopt;
		}
	}
}

auto Sorcery::Engine::_handle_in_search(const sf::Event &event) -> std::optional<int> {

	if (_left_icon_panel->selected)
		_left_icon_panel->selected = std::nullopt;
	if (_right_icon_panel->selected)
		_right_icon_panel->selected = std::nullopt;
	if (_status_bar->selected)
		_status_bar->selected = std::nullopt;

	if (_system->input->check(WindowInput::CANCEL, event))
		_in_search = false;

	if (_system->input->check(WindowInput::BACK, event))
		_in_search = false;

	if (_system->input->check(WindowInput::SHOW_HIDE_CONSOLE, event))
		_game->toggle_console();
	else if (_system->input->check(WindowInput::UP, event))
		_search_option = _search_menu->choose_previous();
	else if (_system->input->check(WindowInput::DOWN, event))
		_search_option = _search_menu->choose_next();
	else if (_system->input->check(WindowInput::MOVE, event))
		_search_option = _search_menu->set_mouse_selected(static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
	else if (_system->input->check(WindowInput::CONFIRM, event)) {

		// We have selected something from the menu
		if (_search_option) {

			if (const MenuItem option_chosen{(*_search_option.value()).item}; option_chosen == MenuItem::AC_LEAVE) {

				_display->set_disc(true);
				_refresh_display();
				_game->save_game();
				_display->set_disc(false);

				_status_bar->refresh();
				_in_search = false;
				_display->generate("engine_base_ui");
				_display->set_input_mode(WindowInputMode::IN_GAME);
				return CONTINUE;
			} else if (option_chosen == MenuItem::AC_SEARCH_CHARACTERS) {
				_in_search = false;
				_in_get = true;
				_get_menu->reload();
				_display->generate("engine_base_ui");
				_display->set_input_mode(WindowInputMode::IN_GAME);
				return CONTINUE;
			}
		}
	}

	return std::nullopt;
}

auto Sorcery::Engine::_handle_in_action(const sf::Event &event) -> std::optional<int> {

	if (_left_icon_panel->selected)
		_left_icon_panel->selected = std::nullopt;
	if (_right_icon_panel->selected)
		_right_icon_panel->selected = std::nullopt;
	if (_status_bar->selected)
		_status_bar->selected = std::nullopt;

	if (_system->input->check(WindowInput::CANCEL, event))
		_in_action = false;

	if (_system->input->check(WindowInput::BACK, event))
		_in_action = false;

	if (_system->input->check(WindowInput::SHOW_HIDE_CONSOLE, event))
		_game->toggle_console();
	else if (_system->input->check(WindowInput::UP, event))
		_action_option = _action_menu->choose_previous();
	else if (_system->input->check(WindowInput::DOWN, event))
		_action_option = _action_menu->choose_next();
	else if (_system->input->check(WindowInput::MOVE, event))
		_action_option = _action_menu->set_mouse_selected(static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
	else if (_system->input->check(WindowInput::CONFIRM, event)) {

		// We have selected something from the menu
		if (_action_option) {

			if (const MenuItem option_chosen{(*_action_option.value()).item}; option_chosen == MenuItem::AC_LEAVE) {

				_display->set_disc(true);
				_refresh_display();
				_game->save_game();
				_display->set_disc(false);

				_status_bar->refresh();
				_in_action = false;
				_display->generate("engine_base_ui");
				_display->set_input_mode(WindowInputMode::IN_GAME);
				return CONTINUE;
			} else if (option_chosen == MenuItem::AC_SEARCH_CHARACTERS) {

				_in_action = false;
				_in_get = true;
				_get_menu->reload();
				return CONTINUE;
			}
		}
	}

	return std::nullopt;
}

auto Sorcery::Engine::_handle_in_get(const sf::Event &event) -> std::optional<int> {

	if (_left_icon_panel->selected)
		_left_icon_panel->selected = std::nullopt;
	if (_right_icon_panel->selected)
		_right_icon_panel->selected = std::nullopt;
	if (_status_bar->selected)
		_status_bar->selected = std::nullopt;

	if (_system->input->check(WindowInput::CANCEL, event))
		_in_get = false;

	if (_system->input->check(WindowInput::BACK, event))
		_in_get = false;

	if (_system->input->check(WindowInput::SHOW_HIDE_CONSOLE, event))
		_game->toggle_console();
	else if (_system->input->check(WindowInput::UP, event))
		_get_option = _get_menu->choose_previous();
	else if (_system->input->check(WindowInput::DOWN, event))
		_get_option = _get_menu->choose_next();
	else if (_system->input->check(WindowInput::MOVE, event))
		_get_option = _get_menu->set_mouse_selected(static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
	else if (_system->input->check(WindowInput::CONFIRM, event)) {

		// We have selected something from the menu
		if (_get_option) {

			if (const MenuItem option_chosen{(*_get_option.value()).item}; option_chosen == MenuItem::AC_LEAVE) {

				_display->set_disc(true);
				_refresh_display();
				_game->save_game();
				_display->set_disc(false);

				_status_bar->refresh();
				_in_get = false;
				_display->generate("engine_base_ui");
				_display->set_input_mode(WindowInputMode::IN_GAME);
				return CONTINUE;
			} else if ((*_get_option.value()).type == MenuItemType::ENTRY) {

				const auto character_chosen{(*_get_option.value()).index};
				_cur_char = &_game->characters[character_chosen];
				if (_cur_char) {
					_cur_char.value()->set_location(CharacterLocation::PARTY);
					_game->state->add_character_by_id(character_chosen);

					_display->set_disc(true);
					_refresh_display();
					_game->save_game();
					_display->set_disc(false);
				}

				_status_bar->refresh();
				_get_menu->reload();
				_get_option = _get_menu->items.begin();
			}
		}
	}

	return std::nullopt;
}

auto Sorcery::Engine::_handle_in_camp(const sf::Event &event) -> std::optional<int> {

	if (_left_icon_panel->selected)
		_left_icon_panel->selected = std::nullopt;
	if (_right_icon_panel->selected)
		_right_icon_panel->selected = std::nullopt;
	if (_status_bar->selected)
		_status_bar->selected = std::nullopt;

	if (_system->input->check(WindowInput::CANCEL, event))
		_in_camp = false;

	if (_system->input->check(WindowInput::BACK, event))
		_in_camp = false;

	if (_system->input->check(WindowInput::SHOW_HIDE_CONSOLE, event))
		_game->toggle_console();
	else if (_system->input->check(WindowInput::UP, event))
		_camp_option = _camp_menu->choose_previous();
	else if (_system->input->check(WindowInput::DOWN, event))
		_camp_option = _camp_menu->choose_next();
	else if (_system->input->check(WindowInput::MOVE, event))
		_camp_option = _camp_menu->set_mouse_selected(static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
	else if (_system->input->check(WindowInput::CONFIRM, event)) {

		// We have selected something from the menu
		if (_camp_option) {

			if (const MenuItem option_chosen{(*_camp_option.value()).item}; option_chosen == MenuItem::CP_LEAVE) {

				_display->set_disc(true);
				_refresh_display();
				_game->save_game();
				_display->set_disc(false);

				_status_bar->refresh();
				_in_camp = false;
				_display->generate("engine_base_ui");
				_display->set_input_mode(WindowInputMode::IN_GAME);
				return CONTINUE;
			} else if (option_chosen == MenuItem::CP_SAVE) {

				auto party{_game->state->get_party_characters()};
				for (auto &[character_id, character] : _game->characters) {
					if (std::find(party.begin(), party.end(), character_id) != party.end()) {
						character.set_location(CharacterLocation::MAZE);
					}
				}

				_display->set_disc(true);
				_refresh_display();
				_game->save_game();
				_display->set_disc(false);

				_game->state->clear_party();
				return EXIT_MODULE;
			} else if (option_chosen == MenuItem::QUIT) {
				_show_confirm_exit = true;
				return CONTINUE;
			} else if (option_chosen == MenuItem::CP_OPTIONS) {

				auto options{std::make_unique<Options>(_system, _display, _graphics)};
				if (auto result{options->start()}; result == EXIT_ALL) {
					options->stop();
					return EXIT_ALL;
				}
				options->stop();
				_status_bar->refresh();
				_display->generate("engine_base_ui");
			} else if (option_chosen == MenuItem::CP_INSPECT) {
				_status_bar->refresh();
				if (auto result{_inspect->start()}; result == MenuItem::ABORT) {
					_inspect->stop();
					return EXIT_ALL;
				}
				_inspect->stop();
				_status_bar->refresh();
				_display->generate("engine_base_ui");
			} else if (option_chosen == MenuItem::CP_REORDER) {
				_status_bar->refresh();
				if (auto new_party{_reorder->start()}; new_party) {

					// TODO: handle aborts here too
					_game->state->set_party(new_party.value());

					_display->set_disc(true);
					_refresh_display();
					_game->save_game();
					_display->set_disc(false);

					_status_bar->refresh();
				}
				_reorder->stop();
				_status_bar->refresh();
				_display->generate("engine_base_ui");
			}
		}
	}

	return std::nullopt;
}

// Will return true in the event of a wipe
auto Sorcery::Engine::_check_for_wipe() const -> bool {

	using enum Enums::Character::CStatus;

	const auto party{_game->state->get_party_characters()};
	for (auto &[character_id, character] : _game->characters) {
		if (std::find(party.begin(), party.end(), character_id) != party.end()) {
			if ((character.get_status() == OK) || (character.get_status() == AFRAID) ||
				(character.get_status() == SILENCED))
				return false;
		}
	}

	return true;
}

auto Sorcery::Engine::_do_wipe() -> int {

	std::this_thread::sleep_for(std::chrono::milliseconds(2000));

	const auto party{_game->state->get_party_characters()};
	for (auto &[character_id, character] : _game->characters) {
		if (std::find(party.begin(), party.end(), character_id) != party.end()) {
			character.set_location(CharacterLocation::MAZE);
			character.set_current_hp(0);
		}
	}
	_graveyard->start();
	_graveyard->stop();

	_display->set_disc(true);
	_refresh_display();
	_game->save_game();
	_display->set_disc(false);

	_game->state->clear_party();

	return EXIT_MODULE;
}

auto Sorcery::Engine::_handle_elevator_a_f(const sf::Event &event) -> std::optional<int> {

	if (_left_icon_panel->selected)
		_left_icon_panel->selected = std::nullopt;
	if (_right_icon_panel->selected)
		_right_icon_panel->selected = std::nullopt;
	if (_status_bar->selected)
		_status_bar->selected = std::nullopt;

	if (_system->input->check(WindowInput::CANCEL, event))
		_in_elevator_a_f = false;

	if (_system->input->check(WindowInput::BACK, event))
		_in_elevator_a_f = false;

	if (_system->input->check(WindowInput::SHOW_HIDE_CONSOLE, event))
		_game->toggle_console();
	else if (_system->input->check(WindowInput::UP, event))
		_elevator_a_f_option = _elevator_a_f_menu->choose_previous();
	else if (_system->input->check(WindowInput::DOWN, event))
		_elevator_a_f_option = _elevator_a_f_menu->choose_next();
	else if (_system->input->check(WindowInput::MOVE, event))
		_elevator_a_f_option =
			_elevator_a_f_menu->set_mouse_selected(static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
	else if (_system->input->check(WindowInput::CONFIRM, event)) {

		// We have selected something from the menu
		if (_elevator_a_f_option) {

			if (const MenuItem option_chosen{(*_elevator_a_f_option.value()).item};
				option_chosen == MenuItem::EL_LEAVE) {
				_in_elevator_a_f = false;
				_display->generate("engine_base_ui");
				_display->set_input_mode(WindowInputMode::IN_GAME);
				_pending_elevator = false;
				_destination_floor = 0;
				return CONTINUE;
			} else if ((option_chosen == MenuItem::EL_A) && (_game->state->get_depth() != -4)) {
				_destination_floor = -4;
				_pending_elevator = true;
				_elevator_if();
			} else if ((option_chosen == MenuItem::EL_B) && (_game->state->get_depth() != -5)) {
				_destination_floor = -5;
				_pending_elevator = true;
				_elevator_if();
			} else if ((option_chosen == MenuItem::EL_C) && (_game->state->get_depth() != -6)) {
				_destination_floor = -6;
				_pending_elevator = true;
				_elevator_if();
			} else if ((option_chosen == MenuItem::EL_D) && (_game->state->get_depth() != -7)) {
				_destination_floor = -7;
				_pending_elevator = true;
				_elevator_if();
			} else if ((option_chosen == MenuItem::EL_E) && (_game->state->get_depth() != -8)) {
				_destination_floor = -8;
				_pending_elevator = true;
				_elevator_if();
			} else if ((option_chosen == MenuItem::EL_F) && (_game->state->get_depth() != -9)) {
				_destination_floor = -9;
				_pending_elevator = true;
				_elevator_if();
			}
		} else {
			_in_elevator_a_f = false;
			_display->generate("engine_base_ui");
			_display->set_input_mode(WindowInputMode::IN_GAME);
			_pending_elevator = false;
			_destination_floor = 0;
		}
	}

	return std::nullopt;
}

auto Sorcery::Engine::_handle_elevator_a_d(const sf::Event &event) -> std::optional<int> {

	if (_left_icon_panel->selected)
		_left_icon_panel->selected = std::nullopt;
	if (_right_icon_panel->selected)
		_right_icon_panel->selected = std::nullopt;
	if (_status_bar->selected)
		_status_bar->selected = std::nullopt;

	if (_system->input->check(WindowInput::CANCEL, event))
		_in_elevator_a_d = false;

	if (_system->input->check(WindowInput::BACK, event))
		_in_elevator_a_d = false;

	if (_system->input->check(WindowInput::SHOW_HIDE_CONSOLE, event))
		_game->toggle_console();
	else if (_system->input->check(WindowInput::UP, event))
		_elevator_a_d_option = _elevator_a_d_menu->choose_previous();
	else if (_system->input->check(WindowInput::DOWN, event))
		_elevator_a_d_option = _elevator_a_d_menu->choose_next();
	else if (_system->input->check(WindowInput::MOVE, event))
		_elevator_a_d_option =
			_elevator_a_d_menu->set_mouse_selected(static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
	else if (_system->input->check(WindowInput::CONFIRM, event)) {

		// We have selected something from the menu
		if (_elevator_a_d_option) {

			if (const MenuItem option_chosen{(*_elevator_a_d_option.value()).item};
				option_chosen == MenuItem::EL_LEAVE) {
				_in_elevator_a_d = false;
				_display->generate("engine_base_ui");
				_display->set_input_mode(WindowInputMode::IN_GAME);
				_pending_elevator = false;
				_destination_floor = 0;
				return CONTINUE;
			} else if ((option_chosen == MenuItem::EL_A) && (_game->state->get_depth() != -1)) {
				_destination_floor = -1;
				_pending_elevator = true;
				_elevator_if();
			} else if ((option_chosen == MenuItem::EL_B) && (_game->state->get_depth() != -2)) {
				_destination_floor = -2;
				_pending_elevator = true;
				_elevator_if();
			} else if ((option_chosen == MenuItem::EL_C) && (_game->state->get_depth() != -3)) {
				_destination_floor = -3;
				_pending_elevator = true;
				_elevator_if();
			} else if ((option_chosen == MenuItem::EL_D) && (_game->state->get_depth() != -4)) {
				_destination_floor = -4;
				_pending_elevator = true;
				_elevator_if();
			} else {
				_in_elevator_a_d = false;
				_display->generate("engine_base_ui");
				_display->set_input_mode(WindowInputMode::IN_GAME);
				_pending_elevator = false;
				_destination_floor = 0;
			}
		}
	}

	return std::nullopt;
}

auto Sorcery::Engine::_handle_in_map(const sf::Event &event) -> std::optional<int> {

	using enum Enums::Controls::Input;

	if ((_system->input->check(MAZE_SHOW_MAP, event)) || (_system->input->check(CANCEL, event)) ||
		(_system->input->check(CONFIRM, event))) {
		_update_automap = true;
		_update_compass = true;
		_update_buffbar = true;
		_update_search = true;
		_update_render = true;
		return CONTINUE;
	}

	return std::nullopt;
}

auto Sorcery::Engine::_unpoison_characters_on_return_to_town() -> void {

	using enum Enums::Character::CStatus;

	const auto party{_game->state->get_party_characters()};
	for (auto &[character_id, character] : _game->characters) {
		if (std::find(party.begin(), party.end(), character_id) != party.end()) {
			if ((character.get_status() == AFRAID) || (character.get_status() == SILENCED))
				character.set_status(OK);
			character.set_poisoned_rate(0);
		}
	}
}

auto Sorcery::Engine::_move_characters_to_temple_if_needed() -> void {

	using enum Enums::Character::CStatus;

	const auto party{_game->state->get_party_characters()};
	for (auto &[character_id, character] : _game->characters) {
		if (std::find(party.begin(), party.end(), character_id) != party.end()) {
			if ((character.get_status() == DEAD) || (character.get_status() == ASHES) ||
				(character.get_status() == STONED) || (character.get_status() == HELD)) {
				character.set_location(CharacterLocation::TEMPLE);
				_game->state->remove_character_by_id(character_id);
			} else if (character.get_status() == LOST) {
				character.set_location(CharacterLocation::TRAINING);
				_game->state->remove_character_by_id(character_id);
			}
		}
	}
}

auto Sorcery::Engine::_handle_in_game(const sf::Event &event) -> std::optional<int> {

	// Handle any events first - will run once then set _can_run_event to false
	_event_if();
	if (_exit_maze_now) {

		_display->set_disc(true);
		_refresh_display();
		_game->save_game();
		_display->set_disc(false);

		_exit_maze_now = false;
		return EXIT_MODULE;
	}

	// Various Debug Commands can be put here
	if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::F2))
		_debug_give_first_character_gold_xp();
	else if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::F3))
		_debug_give_party_random_status();
	else if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::F5))
		_debug_send_non_party_characters_to_tavern();
	else if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::F6))
		_debug_kill_non_party_characters();
	else if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::F4))
		_debug_heal_party_to_full();
	else if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::F7)) {
		_monochrome = true;
		_debug_monochrome_wireframe();
	} else if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::F8)) {
		_monochrome = false;
		_debug_colour_wireframe();
	} else if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Pause)) {
		_display_cursor = true;
		_refresh_display();
	}
	if (_system->input->check(WindowInput::MAZE_SHOW_MAP, event)) {
		_in_map = !_in_map;
		_update_automap = true;
		_update_compass = true;
		_update_buffbar = true;
		_update_search = true;
		_update_render = true;
	} else if (_system->input->check(WindowInput::MAZE_SEARCH, event)) {
		_in_search = true;
		_update_automap = true;
		_update_compass = true;
		_update_buffbar = true;
		_update_search = true;
		_update_render = true;
	} else if (_system->input->check(WindowInput::MAZE_INSPECT, event)) {
		_in_get = true;
		_get_menu->reload();
		_update_automap = true;
		_update_compass = true;
		_update_buffbar = true;
		_update_search = true;
		_update_render = true;
	} else if (_system->input->check(WindowInput::MAZE_CAMP, event)) {
		_in_camp = true;
		_update_automap = true;
		_update_compass = true;
		_update_buffbar = true;
		_update_search = true;
		_update_render = true;
	} else if (_system->input->check(WindowInput::MAZE_STATUSBAR_TOGGLE, event)) {
		_show_status = !_show_status;
		_update_automap = true;
		_update_compass = true;
		_update_buffbar = true;
		_update_search = true;
		_update_render = true;
	} else if (_system->input->check(WindowInput::MAZE_GUI_TOGGLE, event)) {
		_show_gui = !_show_gui;
		_update_automap = true;
		_update_compass = true;
		_update_buffbar = true;
		_update_search = true;
		_update_render = true;
	}

	if (_show_ouch) {
		_show_direction_indicatior = false;
		auto dialog_input{_ouch->handle_input(event)};
		if (dialog_input) {
			if (dialog_input.value() == WindowDialogButton::OK) {

				_display->set_input_mode(WindowInputMode::IN_GAME);
				_show_ouch = false;
				_ouch->set_valid(false);
			}
		}
	} else if (_show_found_an_item) {
		_show_direction_indicatior = false;
		auto dialog_input{_found_an_item->handle_input(event)};
		if (dialog_input) {
			if (dialog_input.value() == WindowDialogButton::OK) {

				_display->set_input_mode(WindowInputMode::IN_GAME);
				_show_found_an_item = false;
				_found_an_item->set_valid(false);
			}
		}
	} else if (_show_pit) {
		auto dialog_input{_pit->handle_input(event)};
		if (dialog_input) {
			if (dialog_input.value() == WindowDialogButton::OK) {

				_display->set_input_mode(WindowInputMode::IN_GAME);
				_show_pit = false;
				_pit->set_valid(false);
			}
		}
	} else if (_show_chute) {
		auto dialog_input{_chute->handle_input(event)};
		if (dialog_input) {
			if (dialog_input.value() == WindowDialogButton::OK) {

				_display->set_input_mode(WindowInputMode::IN_GAME);
				_show_chute = false;
				_chute->set_valid(false);

				if (const auto &next_tile{_game->state->level->at(_game->state->get_player_pos())};
					(next_tile.is(TileProperty::DARKNESS)) && (_game->state->get_lit()))
					_game->state->set_lit(false);
			}
		}
	} else if (_show_elevator) {
		auto dialog_one_moment{_elevator->handle_input(event)};
		if (dialog_one_moment) {
			if (dialog_one_moment.value() == WindowDialogButton::OK) {

				_display->set_input_mode(WindowInputMode::IN_GAME);
				_show_elevator = false;
				_elevator->set_valid(false);

				if (const auto &next_tile{_game->state->level->at(_game->state->get_player_pos())};
					(next_tile.is(TileProperty::DARKNESS)) && (_game->state->get_lit()))
					_game->state->set_lit(false);
			}
		}
	} else if (_show_confirm_stairs) {
		if (_left_icon_panel->selected)
			_left_icon_panel->selected = std::nullopt;
		if (_right_icon_panel->selected)
			_right_icon_panel->selected = std::nullopt;
		if (_status_bar->selected)
			_status_bar->selected = std::nullopt;

		auto dialog_input{_confirm_stairs->handle_input(event)};
		if (dialog_input) {
			if ((dialog_input.value() == WindowDialogButton::CLOSE) ||
				(dialog_input.value() == WindowDialogButton::NO)) {
				_display->set_input_mode(WindowInputMode::IN_GAME);
				_show_confirm_stairs = false;
			} else if (dialog_input.value() == WindowDialogButton::YES) {
				_show_confirm_stairs = false;

				const auto current_loc{_game->state->get_player_pos()};
				if ((current_loc == Coordinate{0, 0}) && (_game->state->get_depth() == -1)) {

					// On return to time - need also to do this on MALOR back as well
					_unpoison_characters_on_return_to_town();
					_move_characters_to_temple_if_needed();

					_display->set_disc(true);
					_refresh_display();
					_game->save_game();
					_display->set_disc(false);

					return EXIT_MODULE;
				} else {

					_stairs_if();
					if (!_tile_explored(_game->state->get_player_pos()))
						_set_tile_explored(_game->state->get_player_pos());
					_update_automap = true;
					_show_confirm_stairs = true;

					_display->set_disc(true);
					_refresh_display();
					_game->save_game();
					_display->set_disc(false);
				}
			}
		}
	} else {
		if (_system->input->check(WindowInput::MAZE_TURN_AROUND, event)) {
			_show_direction_indicatior = true;
			_reset_direction_indicator();
			_turn_around();
			_spinner_if();
			_update_automap = true;
			_update_compass = true;
			_update_render = true;
			_update_buffbar = true;
			_update_search = true;
			_game->pass_turn();
		}
		if ((_system->input->check(WindowInput::LEFT, event)) ||
			(_system->input->check(WindowInput::MAZE_LEFT, event))) {
			_show_direction_indicatior = true;
			_reset_direction_indicator();
			_turn_left();
			_spinner_if();
			_update_automap = true;
			_update_compass = true;
			_update_render = true;
			_update_buffbar = true;
			_update_search = true;
			_game->pass_turn();
		} else if ((_system->input->check(WindowInput::RIGHT, event)) ||
				   (_system->input->check(WindowInput::MAZE_RIGHT, event))) {
			_show_direction_indicatior = true;
			_reset_direction_indicator();
			_turn_right();
			_spinner_if();
			_update_automap = true;
			_update_compass = true;
			_update_render = true;
			_update_buffbar = true;
			_update_search = true;
			_game->pass_turn();
		} else if ((_system->input->check(WindowInput::UP, event)) ||
				   (_system->input->check(WindowInput::MAZE_FORWARD, event))) {
			_game->pass_turn();
			if (auto has_moved{_move_forward()}; !has_moved) {
				_show_direction_indicatior = false;
				_show_ouch = true;
				_ouch->reset_timed();
			} else {
				_show_direction_indicatior = true;
				if (!_tile_explored(_game->state->get_player_pos()))
					_set_tile_explored(_game->state->get_player_pos());
				_reset_direction_indicator();
				_teleport_if();
				_spinner_if();
				_pit_if();
				_chute_if();
				_update_automap = true;
			}
			if (_exit_maze_now) {

				_display->set_disc(true);
				_refresh_display();
				_game->save_game();
				_display->set_disc(false);

				_exit_maze_now = false;
				return EXIT_MODULE;
			}
			_update_automap = true;
			_update_compass = true;
			_update_render = true;
			_update_buffbar = true;
			_update_search = true;
			_can_run_event = true;
		} else if ((_system->input->check(WindowInput::DOWN, event)) ||
				   (_system->input->check(WindowInput::MAZE_BACKWARD, event))) {
			_game->pass_turn();
			if (auto has_moved{_move_backward()}; !has_moved) {
				_show_direction_indicatior = false;
				_show_ouch = true;
				_ouch->reset_timed();
			} else {
				_show_direction_indicatior = true;
				if (!_tile_explored(_game->state->get_player_pos()))
					_set_tile_explored(_game->state->get_player_pos());
				_update_automap = true;
				_reset_direction_indicator();
				_spinner_if();
				_teleport_if();
				_pit_if();
				_chute_if();
			}
			if (_exit_maze_now) {

				_display->set_disc(true);
				_refresh_display();
				_game->save_game();
				_display->set_disc(false);

				_exit_maze_now = false;
				return EXIT_MODULE;
			}
			_update_automap = true;
			_update_compass = true;
			_update_render = true;
			_update_buffbar = true;
			_update_search = true;
			_can_run_event = true;

		} else if (_system->input->check(WindowInput::CANCEL, event))
			_in_camp = true;
		else if (_system->input->check(WindowInput::BACK, event))
			_in_camp = true;
		else if (_system->input->check(WindowInput::CONFIRM, event)) {
			sf::Vector2f mouse_pos{static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window))};
			if (_status_bar->selected) {

				// Status-bar selected is 1-indexed, not 0-indexed
				const auto character_chosen{(_status_bar->selected.value())};
				_cur_char = &_game->characters[_game->state->get_party_characters().at(character_chosen - 1)];
				if (_cur_char) {
					_display->set_input_mode(WindowInputMode::BROWSE_CHARACTER);
					_cur_char.value()->set_view(CharacterView::SUMMARY);
					_in_character = true;
					return CONTINUE;
				}
			} else if (_left_icon_panel->is_mouse_over(
						   (*_display->layout)["engine_base_ui:left_icon_panel"], mouse_pos)) {

				if (std::optional<std::string> left_selected{_left_icon_panel->set_mouse_selected(
						(*_display->layout)["engine_base_ui:left_icon_panel"], mouse_pos)};
					left_selected) {
					const auto &what(left_selected.value());
					if (what.ends_with("reorder")) {
						_status_bar->refresh();
						if (auto new_party{_reorder->start()}; new_party) {

							// TODO: handle aborts here too
							_game->state->set_party(new_party.value());

							_display->set_disc(true);
							_refresh_display();
							_game->save_game();
							_display->set_disc(false);

							_status_bar->refresh();
						}
						_reorder->stop();
						_status_bar->refresh();
						_display->generate("engine_base_ui");
						_display->set_input_mode(WindowInputMode::IN_GAME);

					} else if (what.ends_with("items")) {
						// TODO
					} else if (what.ends_with("achievements")) {
						// TODO
					} else if (what.ends_with("talk")) {
						// TODO
					} else if (what.ends_with("controls")) {
						_display->show_overlay();
						_system->set_pause(10000);
						_display->hide_overlay();

					} else if (what.ends_with("kick")) {
						// TODO
					} else if (what.ends_with("options")) {
						auto options{std::make_unique<Options>(_system, _display, _graphics)};
						if (auto result{options->start()}; result == EXIT_ALL) {
							options->stop();
							return EXIT_ALL;
						}
						options->stop();
						_status_bar->refresh();
						_display->generate("engine_base_ui");
						_display->set_input_mode(WindowInputMode::IN_GAME);
					} else if (what.ends_with("save")) {
						auto party{_game->state->get_party_characters()};
						for (auto &[character_id, character] : _game->characters) {
							if (std::find(party.begin(), party.end(), character_id) != party.end()) {
								character.set_location(CharacterLocation::MAZE);
							}
						}
						_display->set_disc(true);
						_refresh_display();
						_game->save_game();
						_display->set_disc(false);

						_game->state->clear_party();
						return EXIT_MODULE;

					} else if (what.ends_with("exit")) {
						_show_confirm_exit = true;
						_display->set_input_mode(WindowInputMode::CONFIRM_QUIT_GAME);
						return CONTINUE;
					}
				}
			} else if (_right_icon_panel->is_mouse_over(
						   (*_display->layout)["engine_base_ui:right_icon_panel"], mouse_pos)) {

				if (std::optional<std::string> right_selected{_right_icon_panel->set_mouse_selected(
						(*_display->layout)["engine_base_ui:right_icon_panel"], mouse_pos)};
					right_selected) {
					const auto &what{right_selected.value()};
					if (what.ends_with("left")) {
						_show_direction_indicatior = true;
						_reset_direction_indicator();
						_turn_left();
						_spinner_if();
						_update_automap = true;
						_update_compass = true;
						_update_render = true;
						_update_buffbar = true;
						_update_search = true;
					} else if (what.ends_with("right")) {
						_show_direction_indicatior = true;
						_reset_direction_indicator();
						_turn_right();
						_spinner_if();
						_update_automap = true;
						_update_compass = true;
						_update_render = true;
						_update_buffbar = true;
						_update_search = true;
					} else if (what.ends_with("forward")) {
						if (auto has_moved{_move_forward()}; !has_moved) {
							_show_direction_indicatior = false;
							_show_ouch = true;
							_ouch->reset_timed();
						} else {
							_show_direction_indicatior = true;
							_reset_direction_indicator();
							_teleport_if();
							_spinner_if();
							_pit_if();
							_chute_if();
							if (!_tile_explored(_game->state->get_player_pos()))
								_set_tile_explored(_game->state->get_player_pos());
							_update_automap = true;
						}
					} else if (what.ends_with("backward")) {
						if (auto has_moved{_move_backward()}; !has_moved) {
							_show_direction_indicatior = false;
							_show_ouch = true;
							_ouch->reset_timed();
						} else {
							_show_direction_indicatior = true;
							_reset_direction_indicator();
							_spinner_if();
							_teleport_if();
							_pit_if();
							_chute_if();
							if (!_tile_explored(_game->state->get_player_pos()))
								_set_tile_explored(_game->state->get_player_pos());
							_update_automap = true;
						}
					} else if (what.ends_with("camp")) {
						_in_camp = true;
						_update_automap = true;
						_update_compass = true;
						_update_buffbar = true;
						_update_search = true;
						_update_render = true;
						_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
					} else if (what.ends_with("use")) {
						// TODO
					} else if (what.ends_with("magic")) {
						// TODO
					} else if (what.ends_with("examine")) {
						// TODO
					} else if (what.ends_with("party")) {
						_status_bar->refresh();
						if (auto result{_inspect->start()}; result == MenuItem::ABORT) {
							_inspect->stop();
							return EXIT_ALL;
						}
						_inspect->stop();
						_status_bar->refresh();
						_display->generate("engine_base_ui");
						_display->set_input_mode(WindowInputMode::IN_GAME);
					}
				} else if (_right_icon_panel->is_mouse_over((*_display->layout)["global:automap"], mouse_pos)) {

					_in_map = true;
					_update_automap = true;
					_update_compass = true;
					_update_buffbar = true;
					_update_search = true;
					_update_render = true;

				} else if (_is_mouse_over(_search_bounds, mouse_pos) && (_search->characters_here)) {

					_in_get = true;
					_get_menu->reload();
					_update_automap = true;
					_update_compass = true;
					_update_buffbar = true;
					_update_search = true;
					_update_render = true;
				} else {

					// Otherwise the left click menu
					_in_action = true;
				}
			}
		} else if (_system->input->check(WindowInput::SHOW_HIDE_CONSOLE, event))
			_game->toggle_console();
		else if (_system->input->check(WindowInput::MOVE, event)) {
			// Check for Mouse Overs
			sf::Vector2f mouse_pos{static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window))};

			if (std::optional<std::string> left_selected{_left_icon_panel->set_mouse_selected(
					(*_display->layout)["engine_base_ui:left_icon_panel"], mouse_pos)};
				left_selected) {
				_left_icon_panel->selected = left_selected.value();
				if (_right_icon_panel->selected)
					_right_icon_panel->selected = std::nullopt;
				if (_status_bar->selected)
					_status_bar->selected = std::nullopt;
			}

			if (std::optional<std::string> right_selected{
					_right_icon_panel->set_mouse_selected((*_display->layout)["engine_base_ui:right_icon_"
																			  "panel"],
						mouse_pos)};
				right_selected) {
				_right_icon_panel->selected = right_selected.value();
				if (_left_icon_panel->selected)
					_left_icon_panel->selected = std::nullopt;
				if (_status_bar->selected)
					_status_bar->selected = std::nullopt;
			}

			std::optional<unsigned int> status_bar_selected{_status_bar->set_mouse_selected(mouse_pos)};
			if (status_bar_selected) {
				_status_bar->selected = status_bar_selected.value();
				if (_right_icon_panel->selected)
					_right_icon_panel->selected = std::nullopt;
				if (_left_icon_panel->selected)
					_left_icon_panel->selected = std::nullopt;
			}
		}

		/* auto current_loc{_game->state->get_player_pos()};
		if (auto note{(*_game->state->level)(current_loc)}; (note.text.length() > 0) && (note.visible)) {
			_show_tile_note = true;
			_tile_note->update(note);
		} else
			_show_tile_note = false;
		_update_tile_note = false; */
	}

	return std::nullopt;
}

auto Sorcery::Engine::_is_mouse_over(sf::IntRect rect, sf::Vector2f mouse_pos) const -> bool {

	return rect.contains(static_cast<int>(mouse_pos.x), static_cast<int>(mouse_pos.y));
}

// Entering the Maze
auto Sorcery::Engine::start() -> int {

	_generate_display();

	_set_maze_entry_start();

	if (!_tile_explored(_game->state->get_player_pos()))
		_set_tile_explored(_game->state->get_player_pos());

	// Main Event Loops
	sf::Event event{};
	while (_window->isOpen()) {

		// Handle various timers
		_update_timers_and_components();
		_check_for_pending_events();

		if (_system->get_pause()) {

			_do_pause(event);
		} else {

			_pending_chute = false;
			_pending_elevator = false;
			while (_window->pollEvent(event)) {

				// Check for Window Close
				if (event.type == sf::Event::Closed)
					return EXIT_ALL;

				// Handle enabling help overlay
				if (_system->input->check(WindowInput::SHOW_CONTROLS, event)) {
					_display->show_overlay();
					continue;
				} else
					_display->hide_overlay();

				if (_show_confirm_search) {
					const auto do_search_square{_handle_confirm_search(event)};
					_display->set_input_mode(WindowInputMode::IN_GAME);
					if (do_search_square) {

						const auto current_loc{_game->state->get_player_pos()};
						if (_game->state->level->at(current_loc).has_event()) {

							// Find the event and do something with it!
							const auto map_event{_game->state->level->at(current_loc).has_event().value()};
							if (map_event == MapEvent::SILVER_KEY) {
								// do silver key
							}
						}
					}
				} else if (_show_confirm_exit) {
					_handle_confirm_exit(event);
				} else if (_in_character) {
					_handle_in_character(event);
				} else if (_in_map) {
					auto what_to_do{_handle_in_map(event)};
					if (what_to_do) {
						if (what_to_do.value() == CONTINUE) {
							_in_map = false;
							continue;
						}
					}
				} else if (_in_camp) {
					auto what_to_do{_handle_in_camp(event)};
					if (what_to_do) {
						if (what_to_do.value() == CONTINUE)
							continue;
						else if (what_to_do.value() == EXIT_MODULE)
							return EXIT_MODULE;
						else if (what_to_do.value() == EXIT_ALL) {
							return EXIT_ALL;
						}
					}
				} else if (_in_search) {

					auto what_to_do{_handle_in_search(event)};
					if (what_to_do) {
						if (what_to_do.value() == CONTINUE)
							continue;
					}
				} else if (_in_get) {

					auto what_to_do{_handle_in_get(event)};
					if (what_to_do) {
						if (what_to_do.value() == CONTINUE)
							continue;
					}
				} else if (_in_action) {

					auto what_to_do{_handle_in_action(event)};
					if (what_to_do) {
						if (what_to_do.value() == CONTINUE)
							continue;
					}

				} else if (_in_elevator_a_d) {

					auto what_to_do{_handle_elevator_a_d(event)};
					if (what_to_do) {
						if (what_to_do.value() == CONTINUE)
							continue;
					}
				} else if (_in_elevator_a_f) {

					auto what_to_do{_handle_elevator_a_f(event)};
					if (what_to_do) {
						if (what_to_do.value() == CONTINUE)
							continue;
					}
				} else {

					if (_display->get_input_mode() == WindowInputMode::IN_GAME) {

						// Check for a wipe!
						if (_check_for_wipe())
							return _do_wipe();

						auto what_to_do{_handle_in_game(event)};
						if (what_to_do) {
							if (what_to_do.value() == CONTINUE)
								continue;
							else if (what_to_do.value() == EXIT_MODULE)
								return EXIT_MODULE;
							else if (what_to_do.value() == EXIT_ALL) {
								return EXIT_ALL;
							}
						}
					}
				}
			}
		}

		_update_display();
		_refresh_display();
	}

	return EXIT_MODULE;
}

auto Sorcery::Engine::_update_display() -> void {

	if (_update_render) {
		_render->refresh();
		_update_render = false;
	}
	if (_update_automap) {
		_automap->refresh();
		_map->refresh();
		_update_automap = false;
	}
	if (_update_compass) {
		_compass->refresh();
		_update_compass = false;
	}
	if (_update_buffbar) {
		_buffbar->refresh();
		_update_buffbar = false;
	}
	if (_update_search) {
		_search->refresh();
		_update_search = false;
	}
	if (_update_icon_panels) {
		_left_icon_panel->refresh(_in_camp);
		_right_icon_panel->refresh(_in_camp);
		_update_icon_panels = false;
	}
	if (_update_status_bar) {
		_status_bar->refresh();
		_update_status_bar = false;
	}
}

auto Sorcery::Engine::_refresh_display() -> void {

	_window->clear();

	if (_display->layout->refresh_if_needed())
		_generate_display();

	_draw();
	_window->display();
}

// Remember Y is reversed
auto Sorcery::Engine::_move_forward() -> bool {

	using enum Enums::Map::Direction;
	using enum Enums::Tile::Features;

	auto current_loc{_game->state->get_player_pos()};
	auto x_d{current_loc.x};
	auto y_d{current_loc.y};

	switch (_game->state->get_player_facing()) {
	case NORTH:
		++y_d;
		break;
	case SOUTH:
		--y_d;
		break;
	case EAST:
		++x_d;
		break;
	case WEST:
		--x_d;
		break;
	default:
		break;
	}

	if (x_d < 0)
		x_d = MAP_SIZE - 1;
	else if (x_d > MAP_SIZE - 1)
		x_d = 0;
	if (y_d < 0)
		y_d = MAP_SIZE - 1;
	else if (y_d > MAP_SIZE - 1)
		y_d = 0;

	const auto next_loc{Coordinate{x_d, y_d}};

	auto this_tile{_game->state->level->at(current_loc)};
	const auto &next_tile{_game->state->level->at(next_loc)};

	auto this_wall_to_check{_game->state->get_player_facing()};
	if (this_tile.walkable(this_wall_to_check)) {

		_game->state->set_player_pos(next_loc);

		if (!_tile_explored(_game->state->get_player_pos()))
			_set_tile_explored(_game->state->get_player_pos());
		if ((next_tile.is(TileProperty::DARKNESS)) && (_game->state->get_lit()))
			_game->state->set_lit(false);

		if (_game->state->level->stairs_at(next_loc)) {
			const auto current_loc{_game->state->get_player_pos()};
			if (const auto &to_tile{_game->state->level->at(current_loc)}; to_tile.has(LADDER_UP))
				_confirm_stairs->set((*_display->layout)["engine_base_ui:dialog_ladder_up_text"]);
			else if (to_tile.has(LADDER_DOWN))
				_confirm_stairs->set((*_display->layout)["engine_base_ui:dialog_ladder_down_text"]);
			else if (to_tile.has(STAIRS_UP))
				_confirm_stairs->set((*_display->layout)["engine_base_ui:dialog_stairs_up_text"]);
			else if (to_tile.has(STAIRS_DOWN))
				_confirm_stairs->set((*_display->layout)["engine_base_ui:dialog_stairs_down_text"]);
			_show_confirm_stairs = true;
		} else
			_show_confirm_stairs = false;

		// TODO:clunky
		if (_game->state->level->elevator_at(next_loc)) {
			const auto elevator{_game->state->level->at(next_loc).has_elevator()};
			if (elevator.value().bottom_depth == -4) {
				_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
				std::optional<std::vector<MenuEntry>::const_iterator> elevator_option{_elevator_a_d_menu->items.end()};
				_in_elevator_a_d = true;
			} else if (elevator.value().bottom_depth == -9) {
				_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
				std::optional<std::vector<MenuEntry>::const_iterator> elevator_option{_elevator_a_f_menu->items.end()};
				_in_elevator_a_f = true;
			}
		} else {
			_in_elevator_a_d = false;
			_in_elevator_a_f = false;
		}

		_last_movement = NORTH; // Remember this is COMPASS direction (i.e. on screen), not Map direction

		return true;
	} else
		return false;
}

auto Sorcery::Engine::_move_backward() -> bool {

	using enum Enums::Map::Direction;
	using enum Enums::Tile::Features;

	// Work out our new position
	auto current_loc{_game->state->get_player_pos()};
	auto x_d{current_loc.x};
	auto y_d{current_loc.y};

	switch (_game->state->get_player_facing()) {
	case NORTH:
		--y_d;
		break;
	case SOUTH:
		++y_d;
		break;
	case EAST:
		--x_d;
		break;
	case WEST:
		++x_d;
		break;
	default:
		break;
	}

	if (x_d < 0)
		x_d = MAP_SIZE - 1;
	else if (x_d > MAP_SIZE - 1)
		x_d = 0;
	if (y_d < 0)
		y_d = MAP_SIZE - 1;
	else if (y_d > MAP_SIZE - 1)
		y_d = 0;

	const auto next_loc{Coordinate{x_d, y_d}};

	// Check for walls etc between current square and new square
	auto this_tile{_game->state->level->at(current_loc)};
	const auto &next_tile{_game->state->level->at(next_loc)};

	auto this_wall_to_check{NO_DIRECTION};
	switch (_game->state->get_player_facing()) {
	case NORTH:
		this_wall_to_check = SOUTH;
		break;
	case SOUTH:
		this_wall_to_check = NORTH;
		break;
	case EAST:
		this_wall_to_check = WEST;
		break;
	case WEST:
		this_wall_to_check = EAST;
		break;
	default:
		break;
	}

	if (this_tile.walkable(this_wall_to_check)) {

		_game->state->set_player_pos(next_loc);

		if (!_tile_explored(_game->state->get_player_pos()))
			_set_tile_explored(_game->state->get_player_pos());
		if ((next_tile.is(TileProperty::DARKNESS)) && (_game->state->get_lit()))
			_game->state->set_lit(false);

		if (_game->state->level->stairs_at(next_loc)) {
			const auto current_loc{_game->state->get_player_pos()};
			if (const auto &this_tile{_game->state->level->at(current_loc)}; this_tile.has(LADDER_UP))
				_confirm_stairs->set((*_display->layout)["engine_base_ui:dialog_ladder_up_text"]);
			else if (this_tile.has(LADDER_DOWN))
				_confirm_stairs->set((*_display->layout)["engine_base_ui:dialog_ladder_down_text"]);
			else if (this_tile.has(STAIRS_UP))
				_confirm_stairs->set((*_display->layout)["engine_base_ui:dialog_stairs_up_text"]);
			else if (this_tile.has(STAIRS_DOWN))
				_confirm_stairs->set((*_display->layout)["engine_base_ui:dialog_stairs_down_text"]);
			_show_confirm_stairs = true;
		} else
			_show_confirm_stairs = false;

		// TODO:clunky
		if (_game->state->level->elevator_at(next_loc)) {
			const auto elevator{_game->state->level->at(next_loc).has_elevator()};
			if (elevator.value().bottom_depth == -4) {
				_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
				std::optional<std::vector<MenuEntry>::const_iterator> elevator_option{_elevator_a_d_menu->items.end()};
				_in_elevator_a_d = true;
			} else if (elevator.value().bottom_depth == -9) {
				_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
				std::optional<std::vector<MenuEntry>::const_iterator> elevator_option{_elevator_a_f_menu->items.end()};
				_in_elevator_a_f = true;
			}
		} else {
			_in_elevator_a_d = false;
			_in_elevator_a_f = false;
		}

		_last_movement = SOUTH;

		return true;
	} else
		return false;
}

auto Sorcery::Engine::_turn_left() -> void {

	using enum Enums::Map::Direction;

	switch (_game->state->get_player_facing()) {
	case NORTH:
		_game->state->set_player_facing(WEST);
		break;
	case SOUTH:
		_game->state->set_player_facing(EAST);
		break;
	case EAST:
		_game->state->set_player_facing(NORTH);
		break;
	case WEST:
		_game->state->set_player_facing(SOUTH);
		break;
	default:
		break;
	}

	_last_movement = WEST;
}

auto Sorcery::Engine::_turn_right() -> void {

	using enum Enums::Map::Direction;

	switch (_game->state->get_player_facing()) {
	case NORTH:
		_game->state->set_player_facing(EAST);
		break;
	case SOUTH:
		_game->state->set_player_facing(WEST);
		break;
	case EAST:
		_game->state->set_player_facing(SOUTH);
		break;
	case WEST:
		_game->state->set_player_facing(NORTH);
		break;
	default:
		break;
	}

	_last_movement = EAST;
}

auto Sorcery::Engine::_turn_around() -> void {

	using enum Enums::Map::Direction;

	switch (_game->state->get_player_facing()) {
	case NORTH:
		_game->state->set_player_facing(SOUTH);
		break;
	case SOUTH:
		_game->state->set_player_facing(NORTH);
		break;
	case EAST:
		_game->state->set_player_facing(WEST);
		break;
	case WEST:
		_game->state->set_player_facing(EAST);
		break;
	default:
		break;
	}

	_last_movement = SOUTH;
}

// TODO: rock/walkable for all levels/tiles!

auto Sorcery::Engine::_pit_if() -> bool {

	if (const auto tile{_game->state->level->at(_game->state->get_player_pos())}; tile.has(TileFeature::PIT)) {

		_show_pit = true;
		_pit_oops();
		_update_status_bar = true;
		_pit->set_valid(true);
		_pit->reset_timed();
		return true;
	}

	return false;
}

// Example of dice rolling
auto Sorcery::Engine::_pit_oops() -> void {

	std::vector<int> deaths{};
	deaths.clear();

	auto party{_game->state->get_party_characters()};
	for (auto &[character_id, character] : _game->characters) {
		if (std::find(party.begin(), party.end(), character_id) != party.end()) {

			const auto chance{(character.get_cur_attr(CharacterAttribute::AGILITY) - _game->state->get_depth()) * 4};
			const auto roll((*_system->random)[RandomType::D100]);
			_game->state->add_log_dice_roll(
				fmt::format("{:>16} - {}", character.get_name(), "Avoid Pit"), 100, roll, chance);
			if (roll < chance) {

				// Damage is avoided

			} else {

				// Now in the original Apple 2 version, pit damage is based upon 3 extra values stored in the square in
				// the TMaze records - AUX0, AUX1, and AUX2. Thanks to the data extraction by Tommy Ewers, the relevant
				// values for each pit in the game are 0, 8 and depth respectively. This is a long-winded way of saying
				// that the pit damage (calculated in APIT and ROCKWATR) is 0 + (depth * d8), i.e. a d8 for level depth.

				// Inflict damage! (remember depth is negative here and poisitve in original wizardry)
				auto pit_damage{0U};
				const auto dice{std::abs(_game->state->get_depth())};
				for (int i = 1; i <= dice; i++)
					pit_damage += (*_system->random)[RandomType::D8];

				_game->state->add_log_message(
					fmt::format("{} fell into a pit and took {} points of damage!", character.get_name(), pit_damage),
					MessageType::GAME);

				const auto still_alive{character.damage(pit_damage)};
				if (!still_alive) {

					// Oh dear death from a pit!
					_game->state->add_log_message(fmt::format("{} has died!", character.get_name()), MessageType::GAME);
					deaths.emplace_back(character_id);
				}
			}
		}
	}

	if (!deaths.empty()) {

		// need to display a character has died dialog
	}
}

auto Sorcery::Engine::_event_if() -> bool {

	// If we are in-game, and are on something that will happen
	if (const auto current_loc{_game->state->get_player_pos()};
		_game->state->level->at(current_loc).has_event() && _can_run_event) {
		switch (const auto event_type{_game->state->level->at(current_loc).has_event().value()}; event_type) {
		case MapEvent::AREA_OF_OUT_BOUNDS: {
			_show_direction_indicatior = false;
			_display_cursor = false;
			_refresh_display();
			auto event{std::make_unique<Event>(_system, _display, _graphics, _game, event_type)};
			if (auto result{event->start()}; result == MenuItem::ABORT) {
				event->stop();
				_can_run_event = false;
				_display_cursor = true;
				_refresh_display();
				return EXIT_ALL;
			}
		} break;
		case MapEvent::MAN_TELEPORT_CASTLE: {
			_show_direction_indicatior = false;
			_display_cursor = false;
			_refresh_display();
			auto event{std::make_unique<Event>(_system, _display, _graphics, _game, event_type)};
			if (auto result{event->start()}; result == MenuItem::ABORT) {
				event->stop();
				_can_run_event = false;
				_display_cursor = true;
				_refresh_display();
				return EXIT_ALL;
			} else {
				_exit_maze_now = true;
			}
		} break;
		case MapEvent::SILVER_KEY: {
			_show_direction_indicatior = false;
			_display_cursor = false;
			_refresh_display();
			auto event{std::make_unique<Event>(_system, _display, _graphics, _game, event_type)};
			if (auto result{event->start()}; result == MenuItem::ABORT) {
				event->stop();
				_can_run_event = false;
				_display_cursor = true;
				_refresh_display();
				return EXIT_ALL;
			}
			// handle search
			_show_confirm_search = true;
			_display->set_input_mode(WindowInputMode::CONFIRM_QUIT_GAME);
		} break;

		default:
			break;
		}

		_can_run_event = false;
		_display_cursor = true;
		_refresh_display();

		return true;
	}
	return false;
}

auto Sorcery::Engine::_elevator_if() -> bool {

	if (const auto tile{_game->state->level->at(_game->state->get_player_pos())}; tile.has(TileFeature::ELEVATOR)) {

		_show_elevator = true;
		_elevator->set_valid(true);
		_elevator->reset_timed();

		_system->set_pause(2000);
		_pending_elevator = true;

		return true;
	}

	return false;
}

auto Sorcery::Engine::_chute_if() -> bool {

	if (const auto tile{_game->state->level->at(_game->state->get_player_pos())}; tile.has(TileFeature::CHUTE)) {

		_show_chute = true;
		_chute->set_valid(true);
		_chute->reset_timed();

		_system->set_pause(2000);
		_pending_chute = true;

		return true;
	}

	return false;
}

auto Sorcery::Engine::_spinner_if() const -> bool {

	if (const auto tile{_game->state->level->at(_game->state->get_player_pos())}; tile.has(TileFeature::SPINNER)) {

		// Random Direction Change
		auto new_facing{static_cast<MapDirection>((*_system->random)[RandomType::ZERO_TO_3])};
		_game->state->set_player_facing(new_facing);
		return true;
	}

	return false;
}

auto Sorcery::Engine::_stairs_if() -> bool {

	using enum Enums::Tile::Features;

	if (const auto tile{_game->state->level->at(_game->state->get_player_pos())}; tile.has_stairs()) {

		auto destination{tile.has_stairs().value()};
		auto to_level{destination.to_level};

		// Floors are negative
		if (to_level < 0) {
			Level level{((*_game->levelstore)[to_level]).value()};
			_game->state->set_current_level(&level);
			_game->state->set_player_pos(destination.to_loc);
			_game->state->set_depth(to_level);
			_set_tile_explored(_game->state->get_player_pos());

			const auto &next_tile{_game->state->level->at(destination.to_loc)};
			if ((next_tile.is(TileProperty::DARKNESS)) && (_game->state->get_lit()))
				_game->state->set_lit(false);

			if (next_tile.has(LADDER_UP))
				_confirm_stairs->set((*_display->layout)["engine_base_ui:dialog_ladder_up_text"]);
			else if (next_tile.has(LADDER_DOWN))
				_confirm_stairs->set((*_display->layout)["engine_base_ui:dialog_ladder_down_text"]);
			else if (next_tile.has(STAIRS_UP))
				_confirm_stairs->set((*_display->layout)["engine_base_ui:dialog_stairs_up_text"]);
			else if (next_tile.has(STAIRS_DOWN))
				_confirm_stairs->set((*_display->layout)["engine_base_ui:dialog_stairs_down_text"]);

			return true;
		}
	}

	return false;
}

auto Sorcery::Engine::_teleport_if() -> bool {

	using enum Enums::Tile::Features;

	// TODO: handle anti-teleport here in the future
	if (const auto tile{_game->state->level->at(_game->state->get_player_pos())}; tile.has_teleport()) {

		auto destination{tile.has_teleport().value()};
		if (destination.to_level == 0) {

			// Special case of teleporting back to castle
			_exit_maze_now = true;
			return true;
		} else if (destination.to_level == _game->state->get_depth()) {

			const auto &next_tile{_game->state->level->at(destination.to_loc)};
			_game->state->set_player_pos(destination.to_loc);

			if (!_tile_explored(_game->state->get_player_pos()))
				_set_tile_explored(_game->state->get_player_pos());

			if ((next_tile.is(TileProperty::DARKNESS)) && (_game->state->get_lit()))
				_game->state->set_lit(false);

			if (_game->state->level->stairs_at(_game->state->get_player_pos())) {
				const auto current_loc{_game->state->get_player_pos()};
				if (const auto &this_tile{_game->state->level->at(current_loc)}; this_tile.has(TileFeature::LADDER_UP))
					_confirm_stairs->set((*_display->layout)["engine_base_ui:dialog_ladder_up_text"]);
				else if (this_tile.has(LADDER_DOWN))
					_confirm_stairs->set((*_display->layout)["engine_base_ui:dialog_ladder_down_text"]);
				else if (this_tile.has(STAIRS_UP))
					_confirm_stairs->set((*_display->layout)["engine_base_ui:dialog_stairs_up_text"]);
				else if (this_tile.has(STAIRS_DOWN))
					_confirm_stairs->set((*_display->layout)["engine_base_ui:dialog_stairs_down_text"]);
				_show_confirm_stairs = true;
			} else
				_show_confirm_stairs = false;
		} else {

			// TODO: cross teleport levels
		}
	}

	return true;
}

auto Sorcery::Engine::stop() const -> void {

	// TODO
}

auto Sorcery::Engine::_draw() -> void {

	// Scale the Render appropriately
	const auto vf_c{(*_display->layout)["engine_base_ui:view_frame"]};
	const auto wfr_c{(*_display->layout)["engine_base_ui:wireframe_view"]};
	_render->setScale(std::stof(wfr_c["scale_x"].value()), std::stof(wfr_c["scale_y"].value()));

	_graphics->tile_bg(_window);

	// Standard Components
	_display->display("engine_base_ui");

	// Draw the Render
	_render->setPosition(
		wfr_c.x + std::stoi(wfr_c["offset_x"].value()), wfr_c.y + std::stoi(wfr_c["offset_y"].value()));
	_window->draw(*_render);

	if (_show_gui) {

		_window->draw(*_automap);
		_window->draw(*_compass);
		_window->draw(*_buffbar);
		_window->draw(*_search);

		if (_left_icon_panel->selected)
			_left_icon_panel->set_selected_background();
		if (_right_icon_panel->selected)
			_right_icon_panel->set_selected_background();

		_window->draw(*_left_icon_panel);
		_window->draw(*_right_icon_panel);
	}

	if (_show_status) {
		if (_status_bar->selected)
			_status_bar->set_selected_background();
		_window->draw(*_status_bar);
	}

	if (_show_tile_note) {
		_window->draw(*_tile_note);
	}

	if (_in_map) {
		_window->draw(*_map);
	} else if (_in_camp) {
		_window->draw(*_camp_menu_frame);
		_camp_menu->generate((*_display->layout)["engine_base_ui:camp_menu"]);
		_window->draw(*_camp_menu);
	} else if (_in_action) {
		_window->draw(*_action_menu_frame);
		_action_menu->generate((*_display->layout)["engine_base_ui:action_menu"]);
		_window->draw(*_action_menu);
	} else if (_in_search) {
		_window->draw(*_search_menu_frame);
		_search_menu->generate((*_display->layout)["engine_base_ui:search_menu"]);
		_window->draw(*_search_menu);
	} else if (_in_get) {
		_window->draw(*_get_menu_frame);
		_get_menu->generate((*_display->layout)["engine_base_ui:get_menu"]);
		_window->draw(*_get_menu);
	} else if (_in_elevator_a_d) {
		_window->draw(*_elevator_a_d_menu_frame);
		_elevator_a_d_menu->generate((*_display->layout)["engine_base_ui:elevator_a_d_menu"]);
		_window->draw(*_elevator_a_d_menu);

		if (_show_elevator) {
			if (_elevator->get_valid())
				_window->draw(*_elevator);
			else {
				_show_elevator = false;
				_elevator->set_valid(false);
			}
		}
	} else if (_in_elevator_a_f) {
		_window->draw(*_elevator_a_f_menu_frame);
		_elevator_a_f_menu->generate((*_display->layout)["engine_base_ui:elevator_a_f_menu"]);
		_window->draw(*_elevator_a_f_menu);

		if (_show_elevator) {
			if (_elevator->get_valid())
				_window->draw(*_elevator);
			else {
				_show_elevator = false;
				_elevator->set_valid(false);
			}
		}
	} else if (_display->get_input_mode() == WindowInputMode::BROWSE_CHARACTER) {
		if (_cur_char) {

			// If we have a character
			_window->draw(*_cur_char_frame);

			_cur_char.value()->setPosition(
				(*_display->layout)["engine_base_ui:character"].x, (*_display->layout)["engine_base_ui:character"].y);
			_cur_char.value()->update();
			_window->draw(*_cur_char.value());
		}
	} else if (_display->get_input_mode() == WindowInputMode::IN_GAME) {

		if (_show_confirm_stairs) {
			_confirm_stairs->update();
			_window->draw(*_confirm_stairs);
		}

		if (_show_confirm_search) {
			_confirm_search->update();
			_window->draw(*_confirm_search);
		}

		if (_show_ouch) {
			if (_ouch->get_valid())
				_window->draw(*_ouch);
			else {
				_show_ouch = false;
				_ouch->set_valid(false);
			}
		}

		if (_show_found_an_item) {
			if (_found_an_item->get_valid())
				_window->draw(*_found_an_item);
			else {
				_show_found_an_item = false;
				_found_an_item->set_valid(false);
			}
		}

		if (_show_pit) {
			if (_pit->get_valid())
				_window->draw(*_pit);
			else {
				_show_pit = false;
				_pit->set_valid(false);
			}
		}

		if (_show_chute) {
			if (_chute->get_valid())
				_window->draw(*_chute);
			else {
				_show_chute = false;
				_chute->set_valid(false);
			}
		}
	}

	if (_show_direction_indicatior) {
		_display->display_direction_indicator(_last_movement, _monochrome);
	}

	if (_show_confirm_exit) {
		_confirm_exit->update();
		_window->draw(*_confirm_exit);
	}

	// Always draw the following
	_display->display_overlay();
	if (_display_cursor)
		_display->display_cursor();

	if (_game->get_console_status()) {
		_console->refresh();
		_display->window->get_gui()->draw();
	}
}

// Various Debug Functions - can be placed in _handle_in_game and associated with keypresses
auto Sorcery::Engine::_debug_go_to_graveyard() -> std::optional<int> {

	_graveyard->start();
	_graveyard->stop();
	return EXIT_MODULE;
}

auto Sorcery::Engine::_debug_go_down_a_level() -> std::optional<int> {

	auto dest_level{_game->state->get_depth() + 1};
	Level level{((*_game->levelstore)[dest_level]).value()};
	_game->state->set_current_level(&level);
	_game->state->set_depth(dest_level);
	_set_tile_explored(_game->state->get_player_pos());
	_update_automap = true;
	_update_compass = true;
	_update_buffbar = true;
	_update_search = true;
	_update_render = true;

	return CONTINUE;
}

auto Sorcery::Engine::_debug_go_up_a_level() -> std::optional<int> {

	auto dest_level{_game->state->get_depth() - 1};
	Level level{((*_game->levelstore)[dest_level]).value()};
	_game->state->set_current_level(&level);
	_game->state->set_depth(dest_level);
	_set_tile_explored(_game->state->get_player_pos());
	_update_automap = true;
	_update_compass = true;
	_update_buffbar = true;
	_update_search = true;
	_update_render = true;

	return CONTINUE;
}

auto Sorcery::Engine::_debug_heal_party_to_full() -> std::optional<int> {

	const auto party{_game->state->get_party_characters()};
	for (auto &[character_id, character] : _game->characters) {
		if (std::find(party.begin(), party.end(), character_id) != party.end()) {
			character.set_status(CharacterStatus::OK);
			character.set_current_hp(character.get_max_hp());
			character.reset_adjustment_per_turn();
			character.set_poisoned_rate(0);
		}
	}
	_update_automap = true;
	_update_compass = true;
	_update_buffbar = true;
	_update_search = true;
	_update_render = true;
	_update_status_bar = true;

	return CONTINUE;
}

auto Sorcery::Engine::_debug_give_first_character_gold_xp() -> std::optional<int> {

	auto &character{_game->characters[_game->state->get_character_by_position(1).value()]};
	auto next{character.get_next_xp()};
	character.grant_xp(next - 1);
	character.grant_gold(10000);

	_update_automap = true;
	_update_compass = true;
	_update_buffbar = true;
	_update_search = true;
	_update_render = true;
	_update_status_bar = true;

	return CONTINUE;
}

auto Sorcery::Engine::_debug_level_first_character_down() -> std::optional<int> {

	auto &character{_game->characters[_game->state->get_character_by_position(1).value()]};
	character.level_down();

	_update_automap = true;
	_update_compass = true;
	_update_buffbar = true;
	_update_search = true;
	_update_render = true;
	_update_status_bar = true;

	return CONTINUE;
}

auto Sorcery::Engine::_debug_level_first_character_up() -> std::optional<int> {

	auto &character{_game->characters[_game->state->get_character_by_position(1).value()]};
	auto next{character.get_next_xp()};
	character.grant_xp(next + 1);
	for (auto results{character.level_up()}; auto text : results)
		std::cout << text << std::endl;

	_update_automap = true;
	_update_compass = true;
	_update_buffbar = true;
	_update_search = true;
	_update_render = true;
	_update_status_bar = true;

	return CONTINUE;
}

auto Sorcery::Engine::_debug_kill_non_party_characters() -> std::optional<int> {

	for (auto &[character_id, character] : _game->characters) {
		if (character.get_location() != CharacterLocation::PARTY) {
			std::cout << character;
			character.set_current_hp(0);
			character.set_status(CharacterStatus::DEAD);
			character.set_location(CharacterLocation::TEMPLE);
		}
	}

	_update_automap = true;
	_update_compass = true;
	_update_buffbar = true;
	_update_search = true;
	_update_render = true;
	_update_status_bar = true;

	return CONTINUE;
}

auto Sorcery::Engine::_debug_send_non_party_characters_to_tavern() -> std::optional<int> {

	for (auto &[character_id, character] : _game->characters) {
		if (character.get_location() != CharacterLocation::PARTY) {
			std::cout << character;
			character.set_current_hp(character.get_max_hp());
			character.set_status(CharacterStatus::OK);
			character.set_location(CharacterLocation::TAVERN);
		}
	}

	_update_automap = true;
	_update_compass = true;
	_update_buffbar = true;
	_update_search = true;
	_update_render = true;
	_update_status_bar = true;

	return CONTINUE;
}

auto Sorcery::Engine::_debug_give_party_random_hp() -> std::optional<int> {

	auto party{_game->state->get_party_characters()};
	for (auto &[character_id, character] : _game->characters) {
		if (std::find(party.begin(), party.end(), character_id) != party.end()) {
			character.set_current_hp(1);
			if ((*_system->random)[RandomType::ZERO_TO_2] == 0)
				character.set_current_hp(1);
			else
				character.set_current_hp(character.get_max_hp());
		}
	}

	_update_automap = true;
	_update_compass = true;
	_update_buffbar = true;
	_update_search = true;
	_update_render = true;
	_update_status_bar = true;

	return CONTINUE;
}

auto Sorcery::Engine::_debug_give_party_random_status() -> std::optional<int> {

	using enum Enums::Character::CStatus;

	const auto party{_game->state->get_party_characters()};
	for (auto &[character_id, character] : _game->characters) {
		if (std::find(party.begin(), party.end(), character_id) != party.end()) {
			character.set_status(
				magic_enum::enum_cast<CharacterStatus>((*_system->random)[RandomType::ZERO_TO_8]).value());
			if ((character.get_status() == DEAD) || (character.get_status() == ASHES) ||
				(character.get_status() == LOST)) {
				character.set_current_hp(0);
			} else
				character.set_current_hp(character.get_max_hp());
		}
	}

	_update_automap = true;
	_update_compass = true;
	_update_buffbar = true;
	_update_search = true;
	_update_render = true;
	_update_status_bar = true;

	return CONTINUE;
}

auto Sorcery::Engine::_debug_light_on() -> std::optional<int> {

	_game->state->set_lit(true);

	_update_automap = true;
	_update_compass = true;
	_update_buffbar = true;
	_update_search = true;
	_update_render = true;

	return CONTINUE;
}

auto Sorcery::Engine::_debug_light_off() -> std::optional<int> {

	_game->state->set_lit(false);
	_update_automap = true;
	_update_compass = true;
	_update_buffbar = true;
	_update_search = true;
	_update_render = true;

	return CONTINUE;
}

auto Sorcery::Engine::_debug_monochrome_wireframe() -> std::optional<int> {

	_render->set_monochrome(true);
	_update_automap = true;
	_update_compass = true;
	_update_buffbar = true;
	_update_search = true;
	_update_render = true;

	return CONTINUE;
}

auto Sorcery::Engine::_debug_colour_wireframe() -> std::optional<int> {

	_render->set_monochrome(false);
	_update_automap = true;
	_update_compass = true;
	_update_buffbar = true;
	_update_search = true;
	_update_render = true;

	return CONTINUE;
}
