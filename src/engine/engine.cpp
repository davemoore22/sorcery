// Copyright (C) 2026 Dave Moore
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
#include "common/sdl2.hpp"
#include "core/application.hpp"
#include "core/audioplayer.hpp"
#include "core/context.hpp"
#include "core/controller.hpp"
#include "core/define.hpp"
#include "core/resources.hpp"
#include "core/ui.hpp"
#include "engine/automap.hpp"
#include "frontend/options.hpp"
#include "gui/define.hpp"
#include "gui/dialog.hpp"
#include "gui/message.hpp"
#include "gui/modal.hpp"
#include "gui/popup.hpp"
#include "modules/inspect.hpp"
#include "modules/reorder.hpp"
#include "resources/levelstore.hpp"
#include "types/game.hpp"
#include "types/state.hpp"

#include <algorithm>
#include <string>

Sorcery::Engine::Engine(Context &ctx)
	: _ctx{ctx} {

	_options = std::make_unique<Options>(_ctx);
	_reorder = std::make_unique<Reorder>(_ctx);
	_inspect = std::make_unique<Inspect>(_ctx);
	_automap = std::make_unique<Automap>(_ctx);

	_initialise();
};

Sorcery::Engine::~Engine() {}

auto Sorcery::Engine::_initialise() -> bool {

	return true;
}

auto Sorcery::Engine::start(const int mode) -> int {

	_ctx.controller->initialise();
	_ctx.controller->set_flag("in_engine");
	_ctx.controller->go_to(Enums::Screen::ENGINE);
	if (_ctx.game->state->get_party_size() > 0)
		_ctx.controller->set_character(
			"inspect", _ctx.game->state->get_party_char(1).value());

	_start_expedition(mode);

	_ctx.audio->set_volume(1.0f);

	// Main loop
	auto done{false};
	while (!done) {

		SDL_Event event;
		while (SDL_PollEvent(&event)) {

			// Check for Quit Events
			ImGui_ImplSDL2_ProcessEvent(&event);
			done = _ctx.controller->check_for_abort(event);

			if (_ctx.controller->want_to_abort())
				return ABORT_GAME;

			if (_ctx.controller->check_for_quicksave(event))
				_application->save_state_to_binary(SAVE_STATE_FILENAME);
			else if (_ctx.controller->check_for_quickload(event))
				_application->load_state_from_binary(SAVE_STATE_FILENAME);

			// Check for Window Resize
			_ctx.controller->check_for_resize(event, _ctx.ui);

			if (_ctx.controller->check_for_automap(event)) {
				_automap->start();
				_automap->stop();
			}

			// Check for Back Event
			if (_ctx.controller->check_for_back(event)) {
				if (_ctx.ui->in_popup()) {
					_ctx.ui->close_all_popups();
					_ctx.controller->clear_modal_flags();
				} else {
					_ctx.ui->modal_camp->show = true;
					_ctx.controller->set_flag("want_camp");
				}
			}

			auto old_monochrome{_ctx.controller->get_monochrome()};
			_ctx.controller->check_for_ui_toggle(event);
			if (old_monochrome != _ctx.controller->get_monochrome())
				_ctx.ui->set_monochrome(_ctx.controller->get_monochrome());

			// Check for Movement
			if (const auto movement{_ctx.controller->check_for_movement(event)};
				movement != MOVE_NONE) {

				switch (movement) {
				case MOVE_FORWARD:
					_ctx.game->pass_turn();
					if (auto has_moved{_move_forward()}; !has_moved) {

						// Can't move forwards
					} else {

						// Move forward
						_ctx.ui->popup_ouch->show = false;
						if (!_tile_explored(_ctx.game->state->get_player_pos()))
							_set_tile_explored(
								_ctx.game->state->get_player_pos());
					}
					break;
				case MOVE_BACKWARD:
					_ctx.game->pass_turn();
					_ctx.ui->popup_ouch->show = false;
					if (auto has_moved{_move_backward()}; !has_moved) {

						// Can't move backwards
					} else {

						// Move backwards
						if (!_tile_explored(_ctx.game->state->get_player_pos()))
							_set_tile_explored(
								_ctx.game->state->get_player_pos());
					}
					break;
				case MOVE_TURN_LEFT:
					_ctx.ui->popup_ouch->show = false;
					_turn_left();
					_ctx.game->pass_turn();
					break;
				case MOVE_TURN_RIGHT:
					_ctx.ui->popup_ouch->show = false;
					_turn_right();
					_ctx.game->pass_turn();
					break;
				case MOVE_TURN_AROUND:
					_ctx.ui->popup_ouch->show = false;
					_turn_around();
					_ctx.game->pass_turn();
					break;
				default:
					break;
				}
			}

			if (_ctx.controller->wants(Enums::Screen::OPTIONS)) {
				_options->start(true);
				_options->stop();
			} else if (_ctx.controller->wants(Enums::Screen::REORDER)) {
				_reorder->start(REORDER_MODE_CAMP);
				_reorder->stop(REORDER_MODE_CAMP);
			} else if (_ctx.controller->wants(Enums::Screen::INSPECT)) {
				_inspect->start(INSPECT_MODE_CAMP,
								_ctx.game->state->get_party_char(1).value());
				_inspect->stop(INSPECT_MODE_CAMP);
			}

			if (_ctx.controller->has_flag("want_take_stairs_up")) {
				if (_ctx.game->state->get_depth() == -1)
					return _go_back_to_town();
				else
					_go_up_a_level();
			} else if (_ctx.controller->has_flag("want_take_stairs_down"))
				_go_down_a_level();

			if (_ctx.controller->has_flag("after_tile_message") &&
				!_ctx.ui->message_tile->show) {
				// Do Event Handling
				_ctx.controller->unset_flag("after_tile_message");
				_ctx.controller->set_last_event(Enums::Map::Event::NO_EVENT);
			}

			if (_ctx.controller->has_flag("want_quit_expedition") &&
				!_ctx.ui->modal_camp->show) {
				// Handle quitting expedition
				auto party{_ctx.game->state->get_party_characters()};
				for (auto &[id, character] : _ctx.game->characters) {
					if (std::find(party.begin(), party.end(), id) !=
						party.end()) {
						character.set_location(
							Enums::Character::Location::MAZE);
					}
				}
				_ctx.controller->set_busy(true);
				_ctx.game->save_game();
				_ctx.controller->set_busy(false);
				_ctx.game->state->clear_party();
				return LEAVE_MAZE;
			}
		}
		_ctx.ui->display_engine();
		_ctx.tick();
	}

	// Exit if we get to here having broken out of the loop
	return LEAVE_MAZE;
}

auto Sorcery::Engine::stop() -> void {

	_ctx.controller->unset_flag("in_engine");
}

auto Sorcery::Engine::_tile_explored(const Coordinate loc) const -> bool {

	const auto depth{_ctx.game->state->get_depth()};
	const auto it{_ctx.game->state->explored.find(depth)};

	if (it == _ctx.game->state->explored.end())
		return false;

	return it->second.at(loc);
}

auto Sorcery::Engine::_set_tile_explored(const Coordinate loc) -> void {

	const auto depth{_ctx.game->state->get_depth()};
	_ctx.game->state->explored[depth].set(loc);
}

auto Sorcery::Engine::_go_to_location(const int depth, const Coordinate loc,
									  const Enums::Map::Direction dir) -> void {

	Level level{_ctx.resources->levels->get(depth).value()};
	_ctx.game->state->set_current_level(&level);
	_ctx.game->state->set_player_pos(loc);
	_ctx.game->state->set_player_prev_depth(_ctx.game->state->get_depth());
	_ctx.game->state->set_depth(depth);
	_ctx.game->state->set_player_facing(dir);
	_set_tile_explored(_ctx.game->state->get_player_pos());
}

auto Sorcery::Engine::_start_expedition(const int mode) -> void {

	_ctx.controller->set_last_dir(Enums::Map::Direction::NO_DIRECTION);
	_ctx.controller->set_last_event(Enums::Map::Event::NO_EVENT);
	_ctx.controller->set_can_undo(false);
	_ctx.controller->set_monochrome(
		_ctx.get_config(Enums::Config::COLOURED_WIREFRAME));
	_ctx.ui->set_monochrome(_ctx.get_config(Enums::Config::COLOURED_WIREFRAME));

	//_ctx.controller->set_flag("show_automap");
	_ctx.controller->set_flag("interface_party_panel");
	_ctx.controller->set_flag("interface_ui");

	if (!_tile_explored(_ctx.game->state->get_player_pos()))
		_set_tile_explored(_ctx.game->state->get_player_pos());

	if (mode & EXPEDITION_GOTO) {
		// Check we have an override to go to a specific placein the maze
		const auto goto_depth{
			std::stoi(_ctx.get_config("Debug", "quick_start_depth"))};
		const auto goto_loc{
			Coordinate{std::stoi(_ctx.get_config("Debug", "quick_start_x")),
					   std::stoi(_ctx.get_config("Debug", "quick_start_y"))}};
		const auto goto_dir{static_cast<Enums::Map::Direction>(
			std::stoi(_ctx.get_config("Debug", "quick_start_dir")))};

		_go_to_location(goto_depth, goto_loc, goto_dir);

		_ctx.ui->modal_camp->regenerate();
		_ctx.ui->modal_camp->show = true;

		_ctx.ui->modal_identify->show = false;
		_ctx.ui->modal_drop->show = false;
		_ctx.ui->modal_use->show = false;
		_ctx.ui->modal_invoke->show = false;
		_ctx.ui->modal_trade->show = false;
		_ctx.ui->modal_give->show = false;

	} else {
		// Start off in Camp
		_ctx.ui->modal_camp->regenerate();
		_ctx.ui->modal_camp->show = true;

		// Hide any other modals that might be showing
		_ctx.ui->modal_identify->show = false;
		_ctx.ui->modal_drop->show = false;
		_ctx.ui->modal_use->show = false;
		_ctx.ui->modal_invoke->show = false;
		_ctx.ui->modal_trade->show = false;
		_ctx.ui->modal_give->show = false;
	}
}

// Remember Y is reversed
auto Sorcery::Engine::_move_forward() -> bool {

	auto at_loc{_ctx.game->state->get_player_pos()};
	auto x_d{at_loc.x};
	auto y_d{at_loc.y};

	switch (_ctx.game->state->get_player_facing()) {
		using enum Enums::Map::Direction;
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
	auto this_tile{_ctx.game->state->level->at(at_loc)};
	const auto &next_tile{_ctx.game->state->level->at(next_loc)};

	if (const auto next_wall{_ctx.game->state->get_player_facing()};
		this_tile.walkable(next_wall)) {

		_ctx.game->state->set_player_prev_depth(_ctx.game->state->get_depth());
		_ctx.game->state->set_depth(_ctx.game->state->get_depth());
		_ctx.game->state->set_player_pos(next_loc);
		_ctx.controller->set_can_undo(true);

		// Check for Darkness
		using enum Enums::Tile::Properties;
		if (!_tile_explored(_ctx.game->state->get_player_pos()))
			_set_tile_explored(_ctx.game->state->get_player_pos());
		if ((next_tile.is(DARKNESS)) && (_ctx.game->state->get_lit()))
			_ctx.game->state->set_lit(false);

		// Check for Stairs
		using enum Enums::Tile::Features;
		if (_ctx.game->state->level->stairs_at(next_loc)) {
			const auto at_loc{_ctx.game->state->get_player_pos()};
			if (const auto &to_tile{_ctx.game->state->level->at(at_loc)};
				to_tile.has(LADDER_UP)) {
				_ctx.ui->dialog_stairs_up->show = true;
			} else if (to_tile.has(STAIRS_UP)) {
				_ctx.ui->dialog_stairs_up->show = true;
			} else if (to_tile.has(LADDER_DOWN)) {
				_ctx.ui->dialog_stairs_down->show = true;
			} else if (to_tile.has(STAIRS_DOWN)) {
				_ctx.ui->dialog_stairs_down->show = true;
			}
		}

		// Check for Event
		if (const auto at{_ctx.game->state->get_player_pos()};
			_ctx.game->state->level->at(at).has_event()) {

			const auto event_type{
				_ctx.game->state->level->at(at).has_event().value()};
			// const auto dungeon_event{_ctx.game->get_event(event_type)};

			if (event_type == Enums::Map::Event::NO_EVENT) {

			} else {
				_ctx.ui->message_tile->set(_ctx.ui->load_message(event_type),
										   event_type);
				_ctx.controller->set_flag("after_tile_message");
				_ctx.controller->set_last_event(event_type);
				_ctx.ui->message_tile->show = true;
			}
		}

		// Remember this is COMPASS (on screen) direction, not map direction!
		_ctx.controller->set_last_dir(Enums::Map::Direction::NORTH);

		return true;
	} else {
		_start_popup_ouch();
		return false;
	}
}

auto Sorcery::Engine::_callback_stop_popup_ouch(std::uint32_t, void *param)
	-> std::uint32_t {

	((Engine *)param)->_ctx.ui->popup_ouch->show = false;

	return 0;
}

auto Sorcery::Engine::_start_popup_ouch() -> void {

	_ctx.ui->popup_ouch->show = true;
	SDL_AddTimer(2000, &Engine::_callback_stop_popup_ouch, this);
}

auto Sorcery::Engine::_go_back_to_town() -> int {

	_ctx.controller->set_busy(true);
	_ctx.game->save_game();
	_ctx.controller->set_busy(false);

	return RETURN_TO_TOWN;
}
auto Sorcery::Engine::_go_down_a_level() -> void {

	if (const auto tile{
			_ctx.game->state->level->at(_ctx.game->state->get_player_pos())};
		tile.has_stairs()) {

		auto destination{tile.has_stairs().value()};
		auto to_level{destination.to_level};

		// Floors are negative
		if (to_level < 0) {
			Level level{_ctx.resources->levels->get(to_level).value()};
			_ctx.game->state->set_current_level(&level);
			_ctx.game->state->set_player_pos(destination.to_loc);
			_ctx.game->state->set_player_prev_depth(
				_ctx.game->state->get_depth());
			_ctx.game->state->set_depth(to_level);
			_set_tile_explored(_ctx.game->state->get_player_pos());

			_ctx.controller->unset_flag("want_take_stairs_down");
		}
	}
}

auto Sorcery::Engine::_go_up_a_level() -> void {

	if (const auto tile{
			_ctx.game->state->level->at(_ctx.game->state->get_player_pos())};
		tile.has_stairs()) {

		auto destination{tile.has_stairs().value()};
		auto to_level{destination.to_level};

		// Floors are negative
		if (to_level < 0) {
			Level level{_ctx.resources->levels->get(to_level).value()};
			_ctx.game->state->set_current_level(&level);
			_ctx.game->state->set_player_pos(destination.to_loc);
			_ctx.game->state->set_player_prev_depth(
				_ctx.game->state->get_depth());
			_ctx.game->state->set_depth(to_level);
			_set_tile_explored(_ctx.game->state->get_player_pos());

			_ctx.controller->unset_flag("want_take_stairs_down");
		}
	}

	_ctx.controller->unset_flag("want_take_stairs_up");
}

auto Sorcery::Engine::_move_backward() -> bool {

	// Work out our new position
	auto at_loc{_ctx.game->state->get_player_pos()};
	auto x_d{at_loc.x};
	auto y_d{at_loc.y};

	switch (_ctx.game->state->get_player_facing()) {
		using enum Enums::Map::Direction;
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
	auto this_tile{_ctx.game->state->level->at(at_loc)};
	const auto &next_tile{_ctx.game->state->level->at(next_loc)};

	auto this_wall_to_check{Enums::Map::Direction::NO_DIRECTION};
	switch (_ctx.game->state->get_player_facing()) {
		using enum Enums::Map::Direction;
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

		_ctx.game->state->set_player_prev_depth(_ctx.game->state->get_depth());
		_ctx.game->state->set_depth(_ctx.game->state->get_depth());
		_ctx.game->state->set_player_pos(next_loc);
		_ctx.controller->set_can_undo(true);

		if (!_tile_explored(_ctx.game->state->get_player_pos()))
			_set_tile_explored(_ctx.game->state->get_player_pos());
		if ((next_tile.is(Enums::Tile::Properties::DARKNESS)) &&
			(_ctx.game->state->get_lit()))
			_ctx.game->state->set_lit(false);

		if (_ctx.game->state->level->stairs_at(next_loc)) {
			using enum Enums::Tile::Features;
			const auto at_loc{_ctx.game->state->get_player_pos()};
			if (const auto &to_tile{_ctx.game->state->level->at(at_loc)};
				to_tile.has(LADDER_UP)) {
				_ctx.ui->dialog_stairs_up->show = true;
			} else if (to_tile.has(STAIRS_UP)) {
				_ctx.ui->dialog_stairs_up->show = true;
			} else if (to_tile.has(LADDER_DOWN)) {
				_ctx.ui->dialog_stairs_down->show = true;
			} else if (to_tile.has(STAIRS_DOWN)) {
				_ctx.ui->dialog_stairs_down->show = true;
			}
		}

		// Check for Event
		if (const auto at{_ctx.game->state->get_player_pos()};
			_ctx.game->state->level->at(at).has_event()) {

			const auto event_type{
				_ctx.game->state->level->at(at).has_event().value()};

			if (event_type == Enums::Map::Event::NO_EVENT) {

			} else {
				_ctx.ui->message_tile->set(_ctx.ui->load_message(event_type),
										   event_type);
				_ctx.controller->set_flag("after_tile_message");
				_ctx.controller->set_last_event(event_type);
				_ctx.ui->message_tile->show = true;
			}
		}

		_ctx.controller->set_last_dir(Enums::Map::Direction::SOUTH);

		return true;
	} else
		return false;
}

auto Sorcery::Engine::_turn_left() -> void {

	switch (_ctx.game->state->get_player_facing()) {
		using enum Enums::Map::Direction;
	case NORTH:
		_ctx.game->state->set_player_facing(WEST);
		break;
	case SOUTH:
		_ctx.game->state->set_player_facing(EAST);
		break;
	case EAST:
		_ctx.game->state->set_player_facing(NORTH);
		break;
	case WEST:
		_ctx.game->state->set_player_facing(SOUTH);
		break;
	default:
		break;
	}

	_ctx.controller->set_last_dir(Enums::Map::Direction::WEST);
	_ctx.controller->set_can_undo(false);
}

auto Sorcery::Engine::_turn_right() -> void {

	switch (_ctx.game->state->get_player_facing()) {
		using enum Enums::Map::Direction;
	case NORTH:
		_ctx.game->state->set_player_facing(EAST);
		break;
	case SOUTH:
		_ctx.game->state->set_player_facing(WEST);
		break;
	case EAST:
		_ctx.game->state->set_player_facing(SOUTH);
		break;
	case WEST:
		_ctx.game->state->set_player_facing(NORTH);
		break;
	default:
		break;
	}

	_ctx.controller->set_last_dir(Enums::Map::Direction::EAST);
	_ctx.controller->set_can_undo(false);
}

auto Sorcery::Engine::_turn_around() -> void {

	switch (_ctx.game->state->get_player_facing()) {
		using enum Enums::Map::Direction;
	case NORTH:
		_ctx.game->state->set_player_facing(SOUTH);
		break;
	case SOUTH:
		_ctx.game->state->set_player_facing(NORTH);
		break;
	case EAST:
		_ctx.game->state->set_player_facing(WEST);
		break;
	case WEST:
		_ctx.game->state->set_player_facing(EAST);
		break;
	default:
		break;
	}

	_ctx.controller->set_last_dir(Enums::Map::Direction::SOUTH);
	_ctx.controller->set_can_undo(false);
}