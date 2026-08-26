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
#include "core/debug.hpp"
#include "core/define.hpp"
#include "core/resources.hpp"
#include "display/ui.hpp"
#include "engine/automap.hpp"
#include "engine/chest.hpp"
#include "engine/graveyard.hpp"
#include "engine/victory.hpp"
#include "frontend/options.hpp"
#include "gui/define.hpp"
#include "gui/dialog.hpp"
#include "gui/message.hpp"
#include "gui/modal.hpp"
#include "gui/popup.hpp"
#include "modules/inspect.hpp"
#include "modules/reorder.hpp"
#include "resources/componentstore.hpp"
#include "resources/define.hpp"
#include "resources/filestore.hpp"
#include "resources/itemstore.hpp"
#include "resources/levelstore.hpp"
#include "types/game.hpp"
#include "types/meta.hpp"
#include "types/state.hpp"

#include <algorithm>
#include <string>

Sorcery::Engine::Engine(Context &ctx)
	: Module{ctx} {

	_options = std::make_unique<Options>(_ctx);
	_reorder = std::make_unique<Reorder>(_ctx);
	_inspect = std::make_unique<Inspect>(_ctx);
	_automap = std::make_unique<Automap>(_ctx);
	_graveyard = std::make_unique<Graveyard>(_ctx);
	_victory = std::make_unique<Victory>(_ctx);
	_chest = std::make_unique<Chest>(_ctx);

	_initialise();
};

Sorcery::Engine::~Engine() {}

auto Sorcery::Engine::_initialise() -> bool {

	return true;
}

auto Sorcery::Engine::start(const int mode) -> int {

	using namespace std::chrono_literals;

	_ctx.controller->initialise();
	_ctx.controller->set_flag("in_engine");
	_ctx.controller->go_to(Enums::Screen::ENGINE);

	if (_ctx.game->state->get_party_size() > 0)
		_ctx.controller->set_character(
			Enums::CharacterSlot::INSPECT,
			_ctx.game->state->get_party_char(1).value());

	_start_expedition(mode);

	_ctx.audio->set_volume(1.0f);

	fade_in(
		[this] {
			_ctx.ui->display_engine();
		},
		QUICK_FADE);

	// Main loop
	auto done{false};
	while (!done) {

		SDL_Event event{};

		while (SDL_PollEvent(&event)) {

			switch (process_event(event)) {

			case ModuleEvent::ABORT:
				done = true;
				break;

			case ModuleEvent::QUICKLOAD:
				continue;

			case ModuleEvent::NONE:
				break;
			}

			if (done)
				break;

			// Check for Debug
			_ctx.controller->check_for_debug(event);

			// Back closes popup first, otherwise opens camp.
			if (_ctx.controller->check_for_back(event)) {

				if (_ctx.ui->in_popup()) {

					_ctx.ui->close_all_popups();
					_ctx.controller->clear_modal_flags();

				} else {

					_ctx.ui->modal_camp->show = true;
					_ctx.controller->set_flag("want_camp");
				}

				continue;
			}

			// From here down, no gameplay input while an active
			// popup/modal/dialog is displayed.
			if (_ctx.ui->in_popup())
				continue;

			// Some transient messages deliberately block gameplay until
			// their duration has expired.
			if (_ctx.ui->transient_blocks_input())
				continue;

			// Check for Automap
			if (_ctx.controller->check_for_automap(event)) {

				const auto result{_automap->start()};

				_automap->stop();

				if (result == ABORT_GAME)
					return ABORT_GAME;

				fade_in(
					[this] {
						_ctx.ui->display_engine();
					},
					QUICK_FADE);

				continue;
			}

			// TODO: Remove Debug for Chest
			if (_ctx.controller->has_flag("debug_start_chest")) {

				_ctx.controller->unset_flag("debug_start_chest");

				if (_start_chest() == ABORT_GAME)
					return ABORT_GAME;

				fade_in(
					[this] {
						_ctx.ui->display_engine();
					},
					QUICK_FADE);

				continue;
			}

			// Check for UI toggle
			const auto old_monochrome{_ctx.controller->get_monochrome()};

			_ctx.controller->check_for_ui_toggle(event);

			if (old_monochrome != _ctx.controller->get_monochrome())
				_ctx.ui->set_monochrome(_ctx.controller->get_monochrome());

			// Check for movement
			if (const auto movement{_ctx.controller->check_for_movement(event)};
				movement != MOVE_NONE) {

				_ctx.ui->clear_transient_on_action();

				switch (movement) {

				case MOVE_FORWARD:

					_ctx.game->pass_turn();

					if (const auto has_moved{_move_forward()}; has_moved) {

						if (!_tile_explored(_ctx.game->state->get_player_pos()))
							_set_tile_explored(
								_ctx.game->state->get_player_pos());
					}

					break;

				case MOVE_BACKWARD:

					_ctx.game->pass_turn();

					if (const auto has_moved{_move_backward()}; has_moved) {

						if (!_tile_explored(_ctx.game->state->get_player_pos()))
							_set_tile_explored(
								_ctx.game->state->get_player_pos());
					}

					break;

				case MOVE_TURN_LEFT:

					_turn_left();
					_ctx.game->pass_turn();

					break;

				case MOVE_TURN_RIGHT:

					_turn_right();
					_ctx.game->pass_turn();

					break;

				case MOVE_TURN_AROUND:

					_turn_around();
					_ctx.game->pass_turn();

					break;

				default:
					break;
				}
			}
		}

		if (done)
			break;

		//
		// Frame/game-state processing
		//

		//
		// Complete pending timed transitions
		//
		if (_pending_elevator &&
			std::chrono::steady_clock::now() >= _pending_elevator->execute_at) {

			const auto depth{_pending_elevator->depth};

			_pending_elevator.reset();

			_ctx.ui->clear_transient();

			_take_elevator(depth);
		}

		if (_pending_chute &&
			std::chrono::steady_clock::now() >= _pending_chute->execute_at) {

			const auto depth{_pending_chute->depth};
			const auto loc{_pending_chute->loc};

			_pending_chute.reset();

			_ctx.ui->clear_transient();

			// Now actually fall through the chute.
			_go_to_location(depth, loc, Enums::Map::Direction::NORTH);

			_ctx.controller->set_can_undo(false);

			// And only now process the destination tile message.
			const auto &destination_tile{_ctx.game->state->level->at(loc)};

			(void)_check_for_tile_message(destination_tile);
		}

		// Popups block gameplay and module transitions, but they MUST NOT
		// block rendering/ticking.
		//
		if (!_ctx.ui->in_popup()) {

			// Check for return-to-town teleport
			if (_ctx.controller->has_flag("want_return_to_town")) {
				_ctx.controller->unset_flag("want_return_to_town");

				return _go_back_to_town();
			}

			// Check for party wipe
			if (_check_for_wipe()) {

				const auto result{_graveyard->start()};

				_graveyard->stop();

				if (result == ABORT_GAME)
					return ABORT_GAME;

				fade_in(
					[this] {
						_ctx.ui->display_engine();
					},
					QUICK_FADE);

				const auto party{_ctx.game->state->get_party_characters()};

				for (auto &[id, character] : _ctx.game->characters) {

					if (std::find(party.begin(), party.end(), id) !=
						party.end()) {

						character.set_location(
							Enums::Character::Location::MAZE);

						character.set_current_hp(0);
					}
				}

				_ctx.game->state->clear_party();
				_ctx.game->save_game();

				return LEAVE_MAZE;
			}

			// Check for child modules
			if (_ctx.controller->wants(Enums::Screen::OPTIONS)) {

				const auto result{_options->start(true)};
				_options->stop();

				if (result == ABORT_GAME)
					return ABORT_GAME;

				fade_in(
					[this] {
						_ctx.ui->display_engine();
					},
					QUICK_FADE);

			} else if (_ctx.controller->wants(Enums::Screen::REORDER)) {

				const auto result{_reorder->start(REORDER_MODE_CAMP)};

				_reorder->stop(REORDER_MODE_CAMP);

				if (result == ABORT_GAME)
					return ABORT_GAME;

			} else if (_ctx.controller->wants(Enums::Screen::INSPECT)) {

				const auto result{_inspect->start(
					INSPECT_MODE_BASE | INSPECT_MODE_ACTIONS,
					_ctx.game->state->get_party_char(1).value())};

				if (result == ABORT_GAME)
					return ABORT_GAME;

				_inspect->stop(INSPECT_MODE_BASE | INSPECT_MODE_ACTIONS);
			}

			// Check for stairs
			if (_ctx.controller->has_flag("want_take_stairs_up")) {

				if (_ctx.game->state->get_depth() == -1)
					return _go_back_to_town();

				_go_up_a_level();

			} else if (_ctx.controller->has_flag("want_take_stairs_down")) {

				_go_down_a_level();
			}

			// Check for Elevator
			if (_ctx.controller->has_flag("want_take_elevator")) {

				const auto depth{
					_ctx.controller->get_selected("elevator_selected")};

				_ctx.controller->unset_flag("want_take_elevator");

				_ctx.ui->show_transient(_ctx.get_string("POP_UP_ELEVATOR"), 1s,
										TransientWidth::FIT_TEXT,
										TransientMode::UNTIL_EXPIRY);

				_pending_elevator = PendingElevator{
					.depth = depth,
					.execute_at = std::chrono::steady_clock::now() + 1s};
			}

			if (_ctx.controller->has_flag("after_event_search") &&
				!_ctx.ui->dialog_search->show) {

				_ctx.controller->unset_flag("after_event_search");

				if (_ctx.controller->has_flag("want_search")) {

					_ctx.controller->unset_flag("want_search");

					if (_search_event()) {

						// Start Murphy's Ghost encounter here (only encounter
						// to happen after a search).
						DEBUG_LOG("MURPHY'S GHOSTS!");
					}

				} else {

					// Player selected No: no search, no encounter.
					_ctx.controller->set_last_event(
						Enums::Map::Event::NO_EVENT);
				}
			}

			// Handle quitting expedition
			if (_ctx.controller->has_flag("want_quit_expedition")) {

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

		// Clear completed tile message state
		if (_ctx.controller->has_flag("after_tile_message") &&
			!_ctx.ui->message_tile->show) {

			_ctx.controller->unset_flag("after_tile_message");

			if (const auto result{_handle_completed_tile_event()})
				return *result;
		}

		//
		// ALWAYS render/tick, including while a popup/modal/dialog
		// is active.
		//
		_ctx.ui->display_engine();
		_ctx.tick();
	}

	return LEAVE_MAZE;
}

auto Sorcery::Engine::stop() -> int {

	_ctx.controller->unset_flag("in_engine");

	return 0;
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

		// REMOVED Start off in Camp
		_ctx.ui->modal_camp->regenerate();
		_ctx.ui->modal_elevator_bottom->regenerate();
		_ctx.ui->modal_elevator_top->regenerate();

		//_ctx.ui->modal_camp->show = true;

		_ctx.ui->modal_identify->show = false;
		_ctx.ui->modal_chest->show = false;
		_ctx.ui->modal_equip->show = false;
		_ctx.ui->modal_remove->show = false;
		_ctx.ui->modal_spell->show = false;
		_ctx.ui->modal_drop->show = false;
		_ctx.ui->modal_use->show = false;
		_ctx.ui->modal_invoke->show = false;
		_ctx.ui->modal_trade->show = false;
		_ctx.ui->modal_give->show = false;
		_ctx.ui->modal_elevator_top->show = false;
		_ctx.ui->modal_elevator_bottom->show = false;

	} else {
		// REMOVED Start off in Camp
		_ctx.ui->modal_camp->regenerate();
		_ctx.ui->modal_elevator_bottom->regenerate();
		_ctx.ui->modal_elevator_top->regenerate();

		//_ctx.ui->modal_camp->show = true;

		// Hide any other modals that might be showing
		_ctx.ui->modal_identify->show = false;
		_ctx.ui->modal_chest->show = false;
		_ctx.ui->modal_spell->show = false;
		_ctx.ui->modal_equip->show = false;
		_ctx.ui->modal_remove->show = false;
		_ctx.ui->modal_drop->show = false;
		_ctx.ui->modal_use->show = false;
		_ctx.ui->modal_invoke->show = false;
		_ctx.ui->modal_trade->show = false;
		_ctx.ui->modal_give->show = false;
		_ctx.ui->modal_elevator_top->show = false;
		_ctx.ui->modal_elevator_bottom->show = false;

		(void)_process_current_tile();
	}
}

// Remember Y is reversed
auto Sorcery::Engine::_move_forward() -> bool {

	const auto from{_ctx.game->state->get_player_pos()};
	const auto direction{_ctx.game->state->get_player_facing()};
	const auto to{_movement_destination(from, direction)};

	const auto &this_tile{_ctx.game->state->level->at(from)};

	if (!this_tile.walkable(direction)) {

		_ctx.ui->show_transient(_ctx.get_string("POPUP_OUCH"));

		return false;
	}

	_move_player_to(to);

	_ctx.controller->set_last_dir(Enums::Map::Direction::NORTH);

	return _process_tile_entry(from, to);
}

auto Sorcery::Engine::_check_for_tile_message(const Tile &tile) -> bool {

	auto event{tile.has_event()};

	if (!event || *event == Enums::Map::Event::NO_EVENT)
		return false;

	using enum Enums::Map::Event;
	using enum Enums::Items::TypeID;

	if (*event == WERDNA_SIGN_IN &&
		_ctx.game->party_has_item(AMULET_OF_WERDNA)) {

		event = WERDNA_SIGN_OUT;
	}

	if (_skip_tile_event(*event))
		return false;

	_show_tile_message(*event);

	return true;
}

auto Sorcery::Engine::_go_back_to_town() -> int {

	using enum Enums::Items::TypeID;

	if (!_check_for_wipe() && _ctx.game->party_has_item(AMULET_OF_WERDNA)) {

		const auto result{_victory->start()};

		_victory->stop();

		if (result == ABORT_GAME)
			return ABORT_GAME;

		// Remove Amulet
		_ctx.game->remove_party_item(AMULET_OF_WERDNA);

		// Grant Gold and XP
		_ctx.game->grant_party_members_gold(50000);
		_ctx.game->grant_party_members_xp(50000);

		// Grant Chevron Award
		_ctx.game->set_party_members_wiz_1_award(true);
	}

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

	const auto from{_ctx.game->state->get_player_pos()};
	const auto facing{_ctx.game->state->get_player_facing()};
	const auto direction{_opposite_direction(facing)};
	const auto to{_movement_destination(from, direction)};

	const auto &this_tile{_ctx.game->state->level->at(from)};

	if (!this_tile.walkable(direction))
		return false;

	_move_player_to(to);

	_ctx.controller->set_last_dir(Enums::Map::Direction::SOUTH);

	return _process_tile_entry(from, to);
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

	const auto at{_ctx.game->state->get_player_pos()};
	if (_ctx.game->state->level->at(at).has_spinner()) {

		auto new_facing{static_cast<Enums::Map::Direction>(
			_ctx.get_random(Enums::System::Random::ZERO_TO_3))};
		_ctx.game->state->set_player_facing(new_facing);

		DEBUG_LOG("Player triggered spinner");
	}
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

	const auto at{_ctx.game->state->get_player_pos()};
	if (_ctx.game->state->level->at(at).has_spinner()) {

		auto new_facing{static_cast<Enums::Map::Direction>(
			_ctx.get_random(Enums::System::Random::ZERO_TO_3))};
		_ctx.game->state->set_player_facing(new_facing);

		DEBUG_LOG("Player triggered spinner");
	}
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

	const auto at{_ctx.game->state->get_player_pos()};
	if (_ctx.game->state->level->at(at).has_spinner()) {

		auto new_facing{static_cast<Enums::Map::Direction>(
			_ctx.get_random(Enums::System::Random::ZERO_TO_3))};
		_ctx.game->state->set_player_facing(new_facing);

		DEBUG_LOG("Player triggered spinner");
	}
}

auto Sorcery::Engine::_pit_oops() -> void {

	std::vector<int> deaths{};
	deaths.clear();

	auto party{_ctx.game->state->get_party_characters()};
	for (auto &[id, character] : _ctx.game->characters) {
		if (std::find(party.begin(), party.end(), id) != party.end()) {

			const auto chance{
				(character.get_cur_attr(Enums::Character::Attribute::AGILITY) -
				 _ctx.game->state->get_depth()) *
				4};
			const auto roll(_ctx.get_random(Enums::System::Random::D100));
			//_ctx.game->state->add_log_dice_roll(
			//	fmt::format("{:>16} - {}", character.get_name(), "Avoid Pit"),
			//	100, roll, chance);
			if (roll < chance) {

				// Damage is avoided

			} else {

				// Now in the original Apple 2 version, pit damage is based upon
				// 3 extra values stored in the square in the TMaze records -
				// AUX0, AUX1, and AUX2. Thanks to the data extraction by Tommy
				// Ewers, the relevant values for each pit in the game are 0, 8
				// and depth respectively. This is a long-winded way of saying
				// that the pit damage (calculated in APIT and ROCKWATR) is 0 +
				// (depth * d8), i.e. a d8 for level depth.

				// Inflict damage! (remember depth is negative here and positve
				// in original wizardry)
				auto pit_damage{0U};
				const auto dice{std::abs(_ctx.game->state->get_depth())};
				for (int i = 1; i <= dice; i++)
					pit_damage += _ctx.get_random(Enums::System::Random::D8);

				//_ctx.game->state->add_log_message(
				//	fmt::format(
				//		"{} fell into a pit and took {} points of damage!",
				//		character.get_name(), pit_damage),
				//	IMT::GAME);

				if (const auto alive{character.damage(pit_damage)}; !alive) {

					// Oh dear, death from a pit!
					//_game->state->add_log_message(
					//	fmt::format("{} has died!", character.get_name()),
					//	IMT::GAME);
					deaths.emplace_back(id);
				}
			}
		}
	}

	if (!deaths.empty()) {

		// need to display a character has died dialog
	}
}

// Will return true in the event of a wipe
auto Sorcery::Engine::_check_for_wipe() const -> bool {

	const auto party{_ctx.game->state->get_party_characters()};
	for (auto &[id, character] : _ctx.game->characters) {
		if (std::find(party.begin(), party.end(), id) != party.end()) {
			using enum Enums::Character::Status;
			if ((character.get_status() == OK) ||
				(character.get_status() == AFRAID) ||
				(character.get_status() == SILENCED))
				return false;
		}
	}

	return true;
}

auto Sorcery::Engine::_opposite_direction(
	const Enums::Map::Direction direction) const -> Enums::Map::Direction {

	using enum Enums::Map::Direction;

	switch (direction) {
	case NORTH:
		return SOUTH;
	case SOUTH:
		return NORTH;
	case EAST:
		return WEST;
	case WEST:
		return EAST;
	default:
		return NO_DIRECTION;
	}
}

auto Sorcery::Engine::_movement_destination(
	const Coordinate origin, const Enums::Map::Direction direction) const
	-> Coordinate {

	auto destination{origin};

	using enum Enums::Map::Direction;

	switch (direction) {
	case NORTH:
		++destination.y;
		break;

	case SOUTH:
		--destination.y;
		break;

	case EAST:
		++destination.x;
		break;

	case WEST:
		--destination.x;
		break;

	default:
		break;
	}

	if (destination.x < 0)
		destination.x = MAP_SIZE - 1;
	else if (destination.x >= MAP_SIZE)
		destination.x = 0;

	if (destination.y < 0)
		destination.y = MAP_SIZE - 1;
	else if (destination.y >= MAP_SIZE)
		destination.y = 0;

	return destination;
}

auto Sorcery::Engine::_move_player_to(const Coordinate destination) -> void {

	const auto depth{_ctx.game->state->get_depth()};

	_ctx.game->state->set_player_prev_depth(depth);
	_ctx.game->state->set_depth(depth);
	_ctx.game->state->set_player_pos(destination);

	_ctx.controller->set_can_undo(true);
}

auto Sorcery::Engine::_take_elevator(const int depth) -> void {

	const auto current_depth{_ctx.game->state->get_depth()};

	if (depth >= 0 || depth < -9)
		return;

	if (depth == current_depth)
		return;

	const auto loc{_ctx.game->state->get_player_pos()};
	const auto facing{_ctx.game->state->get_player_facing()};

	DEBUG_LOGF("Taking elevator from depth {} to depth {}", current_depth,
			   depth);

	_go_to_location(depth, loc, facing);

	_ctx.controller->set_can_undo(false);
}
auto Sorcery::Engine::_handle_completed_tile_event() -> std::optional<int> {

	const auto event_type{_ctx.controller->get_last_event()};

	if (event_type == Enums::Map::Event::NO_EVENT)
		return std::nullopt;

	const auto event{_ctx.game->get_event(event_type)};

	if (event.go_town_after) {

		_ctx.controller->set_last_event(Enums::Map::Event::NO_EVENT);

		return _go_back_to_town();
	}

	if (event.go_back_after) {

		_move_backward();

		_ctx.controller->set_last_event(Enums::Map::Event::NO_EVENT);

		return std::nullopt;
	}

	if (event.search_after) {

		_ctx.ui->dialog_search->show = true;
		_ctx.controller->set_flag("after_event_search");

		return std::nullopt;
	}

	if (event.combat_after) {

		// Normal combat-after-message event.
		return std::nullopt;
	}

	if (event.give_item_after) {

		using enum Enums::Items::TypeID;

		switch (event_type) {

		case Enums::Map::Event::TREBOR_VOICE:

			// TODO: change this to give back the name of the character given an
			// item
			if (_ctx.game->give_party_item(BLUE_RIBBON, false)) {
				_ctx.ui->show_transient(
					_ctx.get_string("POP_UP_PARTY_FOUND_AN_ITEM"));
			}

			break;

		default:
			break;
		}

		_ctx.controller->set_last_event(Enums::Map::Event::NO_EVENT);

		return std::nullopt;
	}

	// Plain message-only event is now finished.
	_ctx.controller->set_last_event(Enums::Map::Event::NO_EVENT);

	return std::nullopt;
}

auto Sorcery::Engine::_search_event() -> bool {

	const auto event_type{_ctx.controller->get_last_event()};

	const auto event{_ctx.game->get_event(event_type)};

	using enum Enums::Map::Event;
	using enum Enums::Items::TypeID;

	const auto item_type = [&]() -> std::optional<Enums::Items::TypeID> {
		switch (event_type) {

		case OBTAIN_SILVER_KEY:
			return KEY_OF_SILVER;

		case OBTAIN_BRONZE_KEY:
			return KEY_OF_BRONZE;

		case OBTAIN_GOLD_KEY:
			return KEY_OF_GOLD;

		case OBTAIN_BEAR_STATUE:
			return STATUE_OF_BEAR;

		case OBTAIN_FROG_STATUE:
			return STATUE_OF_FROG;

		default:
			return std::nullopt;
		}
	}();

	if (item_type) {

		for (const auto char_id : _ctx.game->state->get_party_characters()) {

			auto &character{_ctx.game->characters.at(char_id)};

			if (character.inventory.get_empty_slots() == 0)
				continue;

			character.inventory.add_type(_ctx.resources->items->get(*item_type),
										 false);

			_ctx.ui->show_transient(
				std::format("{} {}", character.get_name(),
							_ctx.get_string("POP_UP_FOUND_AN_ITEM")));

			break;
		}
	}

	const auto combat_after{event.combat_after};

	_ctx.controller->set_last_event(Enums::Map::Event::NO_EVENT);

	return combat_after;
}

auto Sorcery::Engine::_show_tile_message(const Enums::Map::Event event)
	-> void {

	_ctx.ui->message_tile->set(_ctx.ui->load_message(event), event);

	_ctx.controller->set_flag("after_tile_message");
	_ctx.controller->set_last_event(event);
	_ctx.ui->message_tile->show = true;
}

auto Sorcery::Engine::_skip_tile_event(const Enums::Map::Event event) const
	-> bool {

	using enum Enums::Map::Event;
	using enum Enums::Items::TypeID;

	switch (event) {

	case NEED_SILVER_KEY:
		return _ctx.game->party_has_item(KEY_OF_SILVER);
	case NEED_BRONZE_KEY:
		return _ctx.game->party_has_item(KEY_OF_BRONZE);
	case NEED_GOLD_KEY:
		return _ctx.game->party_has_item(KEY_OF_GOLD);
	case NEED_BEAR_STATUE:
		return _ctx.game->party_has_item(STATUE_OF_BEAR);
	case NEED_FROG_STATUE:
		return _ctx.game->party_has_item(STATUE_OF_FROG);
	case NEED_BLUE_RIBBON:
		return _ctx.game->party_has_item(BLUE_RIBBON);
	case LARGE_DESK:
		return _ctx.game->party_has_item(BLUE_RIBBON);
	case TREBOR_VOICE:
		return _ctx.game->party_has_item(BLUE_RIBBON);

	default:
		return false;
	}
}

// Handle the guaranteed combats on Level 4!
auto Sorcery::Engine::_triggers_guaranteed_encounter(
	const int depth, const Coordinate from,
	[[maybe_unused]] const Coordinate to) const -> bool {

	return depth == -4 && from == Coordinate{10, 15};
}

auto Sorcery::Engine::_process_current_tile() -> bool {

	const auto loc{_ctx.game->state->get_player_pos()};
	const auto &tile{_ctx.game->state->level->at(loc)};

	// Once-per-delve / special combat events
	if (const auto event{tile.has_event()}; event) {

		using enum Enums::Map::Event;
		using enum Enums::Items::TypeID;

		switch (*event) {

		case DEADLY_RING_COMBAT:

			if (_ctx.game->party_has_item(BLUE_RIBBON))
				return true;

			DEBUG_LOG("Player triggered deadly ring combat");

			_ctx.game->state->level->clear_event(loc);

			// Start specific Deadly Ring combat here.

			return true;

		case FIRE_DRAGONS_COMBAT:

			DEBUG_LOG("Player triggered fire dragons combat");

			_ctx.game->state->level->clear_event(loc);

			// Start specific Fire Dragons combat here.

			return true;

		case WERDNA_COMBAT:

			if (_ctx.game->party_has_item(AMULET_OF_WERDNA))
				return true;

			DEBUG_LOG("Player triggered Werdna combat");

			// Do NOT clear this event: possession of the amulet
			// suppresses repeat combat.

			// Start specific Werdna combat here.

			return true;

		default:
			break;
		}
	}

	// Darkness
	using enum Enums::Tile::Properties;

	if (!_tile_explored(loc))
		_set_tile_explored(loc);

	if (tile.is(DARKNESS) && _ctx.game->state->get_lit())
		_ctx.game->state->set_lit(false);

	// Stairs
	using enum Enums::Tile::Features;

	if (_ctx.game->state->level->stairs_at(loc)) {

		if (tile.has(LADDER_UP) || tile.has(STAIRS_UP))
			_ctx.ui->dialog_stairs_up->show = true;
		else if (tile.has(LADDER_DOWN) || tile.has(STAIRS_DOWN))
			_ctx.ui->dialog_stairs_down->show = true;
	}

	// Elevators / chute / teleport / spinner / pit / message
	if (const auto elevator{tile.has_elevator()}) {

		const auto top_elevator{elevator->top_depth == -1};

		if (top_elevator) {

			_ctx.ui->modal_elevator_top->show = true;

			DEBUG_LOG("Player triggered top elevator");

		} else {

			_ctx.ui->modal_elevator_bottom->show = true;

			DEBUG_LOG("Player triggered bottom elevator");
		}

	} else if (const auto destination{tile.has_chute()}) {

		DEBUG_LOG("Player triggered chute");

		_ctx.ui->show_transient(
			_ctx.get_string("DIALOG_CHUTE"), std::chrono::seconds{2},
			TransientWidth::FIT_TEXT, TransientMode::UNTIL_EXPIRY);

		_pending_chute =
			PendingChute{.depth = destination->to_level,
						 .loc = destination->to_loc,
						 .execute_at = std::chrono::steady_clock::now() +
									   std::chrono::seconds{2}};

		return true;

	} else if (const auto destination{tile.has_teleport()}) {

		DEBUG_LOG("Player triggered teleporter");

		if (destination->to_level == 0) {

			_ctx.controller->set_last_event(Enums::Map::Event::NO_EVENT);
			_ctx.controller->set_flag("want_return_to_town");

			return true;

		} else if (destination->to_level == _ctx.game->state->get_depth()) {

			_ctx.game->state->set_player_pos(destination->to_loc);
			_ctx.controller->set_can_undo(false);
			const auto &destination_tile{
				_ctx.game->state->level->at(destination->to_loc)};
			if (_check_for_tile_message(destination_tile))
				_ctx.ui->clear_transient();

			return true;
		} else {

			_ctx.game->state->set_player_prev_depth(
				_ctx.game->state->get_depth());
			_ctx.game->state->set_depth(destination->to_level);
			_ctx.game->state->set_player_pos(destination->to_loc);
			_ctx.controller->set_can_undo(false);

			return true;
		}

	} else if (tile.has_spinner()) {

		const auto new_facing{static_cast<Enums::Map::Direction>(
			_ctx.get_random(Enums::System::Random::ZERO_TO_3))};

		_ctx.game->state->set_player_facing(new_facing);

		DEBUG_LOG("Player triggered spinner");

		return true;

	} else if (tile.has_pit()) {

		_ctx.ui->show_transient(_ctx.get_string("POPUP_PIT"));

		DEBUG_LOG("Player triggered pit");

		_pit_oops();

		return true;

	} else {

		(void)_check_for_tile_message(tile);
	}

	return true;
}

auto Sorcery::Engine::_process_tile_entry(const Coordinate from,
										  const Coordinate to) -> bool {

	const auto depth{_ctx.game->state->get_depth()};

	if (_triggers_guaranteed_encounter(depth, from, to)) {

		DEBUG_LOG("Player triggered guaranteed encounter");

		// Start/schedule encounter here.

		return true;
	}

	return _process_current_tile();
}

auto Sorcery::Engine::_start_chest() -> int {

	//_ctx.ui->create_dynamic_modal("modal_chest");
	//_ctx.ui->modal_chest->regenerate();

	const auto result{_chest->start()};
	_chest->stop();

	switch (result) {

	case Enums::Chests::Result::OPENED:
		return 0; // later: continue reward handling

	case Enums::Chests::Result::LEFT:
		return 0;

	case Enums::Chests::Result::ABORT:
		return ABORT_GAME;
	}

	std::unreachable();
}