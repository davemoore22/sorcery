// Copyright (C) 2025 Dave Moore
//
// This file is part of Sorcery.
//
// Sorcery is free software: you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software
// Foundation, either version 2 of the License, or (at your option) any later
// version.
//
// Sorcery is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with
// Sorcery.  If not, see <http://www.gnu.org/licenses/>.
//
// If you modify this program, or any covered work, by linking or combining
// it with the libraries referred to in README (or a modified version of
// said libraries), containing parts covered by the terms of said libraries,
// the licensors of this program grant you additional permission to convey
// the resulting work.

#include "engine/engine.hpp"
#include "common/macro.hpp"
#include "core/controller.hpp"
#include "core/display.hpp"
#include "core/system.hpp"
#include "core/ui.hpp"
#include "engine/define.hpp"
#include "gui/define.hpp"
#include "gui/dialog.hpp"
#include "gui/message.hpp"
#include "gui/modal.hpp"
#include "gui/popup.hpp"
#include "resources/levelstore.hpp"
#include "types/game.hpp"
#include "types/state.hpp"

Sorcery::Engine::Engine(System *system, Display *display, UI *ui,
						Controller *controller)
	: _system{system},
	  _display{display},
	  _ui{ui},
	  _controller{controller} {

	_options = std::make_unique<Options>(_system, _display, _ui, _controller);
	_reorder = std::make_unique<Reorder>(_system, _display, _ui, _controller);
	_inspect = std::make_unique<Inspect>(_system, _display, _ui, _controller);

	_initialise();
};

Sorcery::Engine::~Engine() {}

auto Sorcery::Engine::_initialise() -> bool {

	return true;
}

auto Sorcery::Engine::start(Game *game, const int mode) -> int {

	_game = game;

	_controller->initialise("engine");
	_controller->set_flag("show_engine");
	if (_game->state->get_party_size() > 0)
		_controller->set_character("inspect",
								   _game->state->get_party_char(1).value());

	_start_expedition(mode);

	// Main loop
	auto done{false};
	while (!done) {

		SDL_Event event;
		while (SDL_PollEvent(&event)) {

			// Check for Quit Events
			ImGui_ImplSDL2_ProcessEvent(&event);
			done = _controller->check_for_abort(event);

			// Check for Window Resize
			_controller->check_for_resize(event, _ui);

			// Check for Back Event
			_controller->check_for_back(event, _ui->modal_camp->show);

			// Check for Debug
			_controller->check_for_debug(event);

			// Don't do anything else whilst a Dialog or Popup etc is up
			auto in_popup{
				_ui->dialog_leave->show || _ui->modal_camp->show ||
				_ui->message_tile->show || _ui->modal_identify->show ||
				_ui->dialog_stairs_down->show || _ui->dialog_stairs_up->show};
			if (!in_popup) {

				// Check for UI toggles
				_controller->check_for_ui_toggle(event);

				// Check for Movement
				if (const auto movement{_controller->check_for_movement(event)};
					movement != MOVE_NONE) {

					switch (movement) {
					case MOVE_FORWARD:
						_game->pass_turn();
						if (auto has_moved{_move_forward()}; !has_moved) {

							// Can't move forwards
						} else {

							// Move forward
							_ui->popup_ouch->show = false;
							if (!_tile_explored(_game->state->get_player_pos()))
								_set_tile_explored(
									_game->state->get_player_pos());
						}
						break;
					case MOVE_BACKWARD:
						_game->pass_turn();
						_ui->popup_ouch->show = false;
						if (auto has_moved{_move_backward()}; !has_moved) {

							// Can't move backwards
						} else {

							// Move backwards
							if (!_tile_explored(_game->state->get_player_pos()))
								_set_tile_explored(
									_game->state->get_player_pos());
						}
						break;
					case MOVE_TURN_LEFT:
						_ui->popup_ouch->show = false;
						_turn_left();
						_game->pass_turn();
						break;
					case MOVE_TURN_RIGHT:
						_ui->popup_ouch->show = false;
						_turn_right();
						_game->pass_turn();
						break;
					case MOVE_TURN_AROUND:
						_ui->popup_ouch->show = false;
						_turn_around();
						_game->pass_turn();
						break;
					default:
						break;
					}
				}

				if (_controller->has_flag("show_options")) {
					_options->start(false);
					_options->stop();
				} else if (_controller->has_flag("show_reorder")) {
					_reorder->start(_game, REORDER_MODE_CAMP);
					_reorder->stop(_game, REORDER_MODE_CAMP);
				} else if (_controller->has_flag("show_inspect")) {
					_inspect->start(_game, INSPECT_MODE_CAMP,
									_game->state->get_party_char(1).value());
					_inspect->stop(_game, INSPECT_MODE_CAMP);
				}

				if (_controller->has_flag("want_take_stairs_up")) {
					if (_game->state->get_depth() == -1)
						return _go_back_to_town();
					else
						_go_up_a_level();
				} else if (_controller->has_flag("want_take_stairs_down"))
					_go_down_a_level();

				if (_controller->has_flag("after_tile_message") &&
					!_ui->message_tile->show) {

					// Do Event Handling
					_controller->unset_flag("after_tile_message");
					_controller->last_event = Enums::Map::Event::NO_EVENT;
				}

				if (_controller->has_flag("want_quit_expedition") &&
					!_ui->modal_camp->show) {

					// Handle quitting expedition
					auto party{_game->state->get_party_characters()};
					for (auto &[id, character] : _game->characters) {
						if (std::find(party.begin(), party.end(), id) !=
							party.end()) {
							character.set_location(
								Enums::Character::Location::MAZE);
						}
					}
					_controller->busy = true;
					_game->save_game();
					_controller->busy = false;
					_game->state->clear_party();
					return LEAVE_MAZE;
				}

				// check for light, elevator etc
			}
		}
		_ui->display_engine(game);
	}

	// Exit if we get to here having broken out of the loop
	return LEAVE_MAZE;
}

auto Sorcery::Engine::stop() -> void {}

auto Sorcery::Engine::_tile_explored(const Coordinate loc) const -> bool {

	return _game->state->explored[_game->state->get_depth()].at(loc);
}

auto Sorcery::Engine::_set_tile_explored(const Coordinate loc) -> void {

	_game->state->explored[_game->state->get_depth()].set(loc);
}

auto Sorcery::Engine::_go_to_location(const int depth, const Coordinate loc,
									  const Enums::Map::Direction dir) -> void {

	Level level{_game->levels->get(depth).value()};
	_game->state->set_current_level(&level);
	_game->state->set_player_pos(loc);
	_game->state->set_player_prev_depth(_game->state->get_depth());
	_game->state->set_depth(depth);
	_game->state->set_player_facing(dir);
	_set_tile_explored(_game->state->get_player_pos());
}

auto Sorcery::Engine::_start_expedition(const int mode) -> bool {

	_controller->last_dir = Enums::Map::Direction::NO_DIRECTION;
	_controller->last_event = Enums::Map::Event::NO_EVENT;
	_controller->can_undo = false;
	_controller->show_party_panel = true;
	_controller->show_ui = true;
	_controller->show_automap = true;
	//_controller->monochrome = (*_system->config)[18]; // Coloured CGA
	_controller->monochrome = false;
	_ui->set_monochrome(_controller->monochrome);

	if (!_tile_explored(_game->state->get_player_pos()))
		_set_tile_explored(_game->state->get_player_pos());

	if (mode & EXPEDITION_GOTO) {

		// Check we have an override to go to a specific placein the maze
		const auto goto_depth{
			std::stoi((*_system->config).get("Debug", "quick_start_depth"))};
		const auto goto_loc{Coordinate{
			std::stoi((*_system->config).get("Debug", "quick_start_x")),
			std::stoi((*_system->config).get("Debug", "quick_start_y"))}};
		const auto goto_dir{static_cast<Enums::Map::Direction>(
			std::stoi((*_system->config).get("Debug", "quick_start_dir")))};

		_go_to_location(goto_depth, goto_loc, goto_dir);

	} else {

		// Start off in Camp
		_ui->modal_camp->show = true;
		_ui->modal_identify->show = false;
	}
}

// Remember Y is reversed
auto Sorcery::Engine::_move_forward() -> bool {

	using Enums::Map::Direction;
	using Enums::Tile::Features;
	using Enums::Tile::Properties;

	auto at_loc{_game->state->get_player_pos()};
	auto x_d{at_loc.x};
	auto y_d{at_loc.y};

	switch (_game->state->get_player_facing()) {
	case Direction::NORTH:
		++y_d;
		break;
	case Direction::SOUTH:
		--y_d;
		break;
	case Direction::EAST:
		++x_d;
		break;
	case Direction::WEST:
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
	auto this_tile{_game->state->level->at(at_loc)};
	const auto &next_tile{_game->state->level->at(next_loc)};

	if (const auto next_wall{_game->state->get_player_facing()};
		this_tile.walkable(next_wall)) {

		_game->state->set_player_prev_depth(_game->state->get_depth());
		_game->state->set_depth(_game->state->get_depth());
		_game->state->set_player_pos(next_loc);
		_controller->can_undo = true;

		// Check for Darkness
		if (!_tile_explored(_game->state->get_player_pos()))
			_set_tile_explored(_game->state->get_player_pos());
		if ((next_tile.is(Properties::DARKNESS)) && (_game->state->get_lit()))
			_game->state->set_lit(false);

		// Check for Stairs
		if (_game->state->level->stairs_at(next_loc)) {
			const auto at_loc{_game->state->get_player_pos()};
			if (const auto &to_tile{_game->state->level->at(at_loc)};
				to_tile.has(Features::LADDER_UP)) {
				_ui->dialog_stairs_up->show = true;
			} else if (to_tile.has(Features::STAIRS_UP)) {
				_ui->dialog_stairs_up->show = true;
			} else if (to_tile.has(Features::LADDER_DOWN)) {
				_ui->dialog_stairs_down->show = true;
			} else if (to_tile.has(Features::STAIRS_DOWN)) {
				_ui->dialog_stairs_down->show = true;
			}
		}

		// Check for Event
		if (const auto at{_game->state->get_player_pos()};
			_game->state->level->at(at).has_event()) {

			const auto event_type{
				_game->state->level->at(at).has_event().value()};
			// const auto dungeon_event{_game->get_event(event_type)};

			if (event_type == Enums::Map::Event::NO_EVENT) {

			} else {
				_ui->message_tile->set(_ui->load_message(event_type),
									   event_type);
				_controller->set_flag("after_tile_message");
				_controller->last_event = event_type;
				_ui->message_tile->show = true;
			}
		}

		// Remember this is COMPASS direction (i.e. on screen), not Map
		// direction!
		_controller->last_dir = Direction::NORTH;

		return true;
	} else {
		_start_popup_ouch();
	}
}

auto Sorcery::Engine::_callback_stop_popup_ouch(Uint32, void *param) -> Uint32 {

	((Engine *)param)->_ui->popup_ouch->show = false;

	return 0;
}

auto Sorcery::Engine::_start_popup_ouch() -> void {

	_ui->popup_ouch->show = true;
	SDL_AddTimer(2000, &Engine::_callback_stop_popup_ouch, this);
}

auto Sorcery::Engine::_go_back_to_town() -> int {

	_controller->busy = true;
	_game->save_game();
	_controller->busy = false;

	return RETURN_TO_TOWN;
}
auto Sorcery::Engine::_go_down_a_level() -> void {

	if (const auto tile{
			_game->state->level->at(_game->state->get_player_pos())};
		tile.has_stairs()) {

		auto destination{tile.has_stairs().value()};
		auto to_level{destination.to_level};

		// Floors are negative
		if (to_level < 0) {
			Level level{_game->levels->get(to_level).value()};
			_game->state->set_current_level(&level);
			_game->state->set_player_pos(destination.to_loc);
			_game->state->set_player_prev_depth(_game->state->get_depth());
			_game->state->set_depth(to_level);
			_set_tile_explored(_game->state->get_player_pos());

			_controller->unset_flag("want_take_stairs_down");
		}
	}
}

auto Sorcery::Engine::_go_up_a_level() -> void {

	if (const auto tile{
			_game->state->level->at(_game->state->get_player_pos())};
		tile.has_stairs()) {

		auto destination{tile.has_stairs().value()};
		auto to_level{destination.to_level};

		// Floors are negative
		if (to_level < 0) {
			Level level{_game->levels->get(to_level).value()};
			_game->state->set_current_level(&level);
			_game->state->set_player_pos(destination.to_loc);
			_game->state->set_player_prev_depth(_game->state->get_depth());
			_game->state->set_depth(to_level);
			_set_tile_explored(_game->state->get_player_pos());

			_controller->unset_flag("want_take_stairs_down");
		}
	}

	_controller->unset_flag("want_take_stairs_up");
}

auto Sorcery::Engine::_move_backward() -> bool {

	using Enums::Map::Direction;
	using Enums::Tile::Features;
	using Enums::Tile::Properties;

	// Work out our new position
	auto at_loc{_game->state->get_player_pos()};
	auto x_d{at_loc.x};
	auto y_d{at_loc.y};

	switch (_game->state->get_player_facing()) {
	case Direction::NORTH:
		--y_d;
		break;
	case Direction::SOUTH:
		++y_d;
		break;
	case Direction::EAST:
		--x_d;
		break;
	case Direction::WEST:
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
	auto this_tile{_game->state->level->at(at_loc)};
	const auto &next_tile{_game->state->level->at(next_loc)};

	auto this_wall_to_check{Direction::NO_DIRECTION};
	switch (_game->state->get_player_facing()) {
	case Direction::NORTH:
		this_wall_to_check = Direction::SOUTH;
		break;
	case Direction::SOUTH:
		this_wall_to_check = Direction::NORTH;
		break;
	case Direction::EAST:
		this_wall_to_check = Direction::WEST;
		break;
	case Direction::WEST:
		this_wall_to_check = Direction::EAST;
		break;
	default:
		break;
	}

	if (this_tile.walkable(this_wall_to_check)) {

		_game->state->set_player_prev_depth(_game->state->get_depth());
		_game->state->set_depth(_game->state->get_depth());
		_game->state->set_player_pos(next_loc);
		_controller->can_undo = true;

		if (!_tile_explored(_game->state->get_player_pos()))
			_set_tile_explored(_game->state->get_player_pos());
		if ((next_tile.is(Properties::DARKNESS)) && (_game->state->get_lit()))
			_game->state->set_lit(false);

		if (_game->state->level->stairs_at(next_loc)) {
			const auto at_loc{_game->state->get_player_pos()};
			if (const auto &to_tile{_game->state->level->at(at_loc)};
				to_tile.has(Features::LADDER_UP)) {
				_ui->dialog_stairs_up->show = true;
			} else if (to_tile.has(Features::STAIRS_UP)) {
				_ui->dialog_stairs_up->show = true;
			} else if (to_tile.has(Features::LADDER_DOWN)) {
				_ui->dialog_stairs_down->show = true;
			} else if (to_tile.has(Features::STAIRS_DOWN)) {
				_ui->dialog_stairs_down->show = true;
			}
		}

		// Check for Event
		if (const auto at{_game->state->get_player_pos()};
			_game->state->level->at(at).has_event()) {

			const auto event_type{
				_game->state->level->at(at).has_event().value()};

			if (event_type == Enums::Map::Event::NO_EVENT) {

			} else {
				_ui->message_tile->set(_ui->load_message(event_type),
									   event_type);
				_controller->set_flag("after_tile_message");
				_controller->last_event = event_type;
				_ui->message_tile->show = true;
			}
		}

		_controller->last_dir = Direction::SOUTH;

		return true;
	} else
		return false;
}

auto Sorcery::Engine::_turn_left() -> void {

	using Enums::Map::Direction;

	switch (_game->state->get_player_facing()) {
	case Direction::NORTH:
		_game->state->set_player_facing(Direction::WEST);
		break;
	case Direction::SOUTH:
		_game->state->set_player_facing(Direction::EAST);
		break;
	case Direction::EAST:
		_game->state->set_player_facing(Direction::NORTH);
		break;
	case Direction::WEST:
		_game->state->set_player_facing(Direction::SOUTH);
		break;
	default:
		break;
	}

	_controller->last_dir = Direction::WEST;
	_controller->can_undo = false;
}

auto Sorcery::Engine::_turn_right() -> void {

	using Enums::Map::Direction;

	switch (_game->state->get_player_facing()) {
	case Direction::NORTH:
		_game->state->set_player_facing(Direction::EAST);
		break;
	case Direction::SOUTH:
		_game->state->set_player_facing(Direction::WEST);
		break;
	case Direction::EAST:
		_game->state->set_player_facing(Direction::SOUTH);
		break;
	case Direction::WEST:
		_game->state->set_player_facing(Direction::NORTH);
		break;
	default:
		break;
	}

	_controller->last_dir = Direction::EAST;
	_controller->can_undo = false;
}

auto Sorcery::Engine::_turn_around() -> void {

	using Enums::Map::Direction;

	switch (_game->state->get_player_facing()) {
	case Direction::NORTH:
		_game->state->set_player_facing(Direction::SOUTH);
		break;
	case Direction::SOUTH:
		_game->state->set_player_facing(Direction::NORTH);
		break;
	case Direction::EAST:
		_game->state->set_player_facing(Direction::WEST);
		break;
	case Direction::WEST:
		_game->state->set_player_facing(Direction::EAST);
		break;
	default:
		break;
	}

	_controller->last_dir = Direction::SOUTH;
	_controller->can_undo = false;
}