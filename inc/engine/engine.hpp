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

#pragma once

#include "common/types.hpp"
#include "core/module.hpp"
#include "engine/enum.hpp"
#include <chrono>
#include <cstdint>
#include <memory>
#include <optional>

namespace Sorcery {

struct Context;
class Options;
class Inspect;
class Reorder;
class Application;
class Automap;
class Graveyard;
class Tile;

struct PendingChute {
		int depth{};
		Coordinate loc{};
		std::chrono::steady_clock::time_point execute_at;
};

struct PendingElevator {
		int depth{};
		std::chrono::steady_clock::time_point execute_at;
};

class Engine final : public Module {

	public:
		// Standard Constructor
		Engine(Context &ctx);
		Engine() = delete;

		// Standard Destructor
		~Engine();

		// Public Members

		// Public Methods
		auto start(const int mode) -> int;
		auto stop() -> int;

	private:
		// Private Methods
		auto _initialise() -> bool;
		auto _go_back_to_town() -> int;
		auto _go_down_a_level() -> void;
		auto _go_up_a_level() -> void;
		auto _go_to_location(const int depth, const Coordinate loc,
							 const Enums::Map::Direction dir) -> void;
		auto _move_backward() -> bool;
		auto _move_forward() -> bool;
		auto _set_tile_explored(const Coordinate loc) -> void;
		auto _start_expedition(const int mode) -> void;
		auto _tile_explored(const Coordinate loc) const -> bool;
		auto _turn_around() -> void;
		auto _turn_left() -> void;
		auto _turn_right() -> void;
		auto _pit_oops() -> void;
		auto _check_for_wipe() const -> bool;

		[[nodiscard]] auto
		_movement_destination(Coordinate origin,
							  Enums::Map::Direction direction) const
			-> Coordinate;
		[[nodiscard]] auto
		_opposite_direction(Enums::Map::Direction direction) const
			-> Enums::Map::Direction;
		auto _move_player_to(Coordinate destination) -> void;

		auto _take_elevator(int depth) -> void;

		auto _handle_completed_tile_event() -> std::optional<int>;

		auto _search_event() -> void;

		[[nodiscard]] auto _check_for_tile_message(const Tile &tile) -> bool;

		auto _show_tile_message(Enums::Map::Event event) -> void;

		// Private Members
		Application *_application;

		std::unique_ptr<Options> _options;
		std::unique_ptr<Reorder> _reorder;
		std::unique_ptr<Inspect> _inspect;
		std::unique_ptr<Automap> _automap;
		std::unique_ptr<Graveyard> _graveyard;

		std::optional<PendingElevator> _pending_elevator;
		std::optional<PendingChute> _pending_chute;
};

}
