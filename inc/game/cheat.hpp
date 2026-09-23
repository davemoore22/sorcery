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

#include <span>
#include <string_view>

namespace Sorcery {

class Game;
struct Context;

class Cheat {

	public:
		using Handler = void (Cheat::*)();

		struct Action {

				std::string_view label;
				Handler handler;
		};

		explicit Cheat(Context &ctx, Game &game);

		[[nodiscard]]
		auto actions() const -> std::span<const Action>;

		auto execute(const Action &action) -> void;

	private:
		Context &_ctx;
		Game &_game;

		auto create_random_party() -> void;
		auto fill_party_unid_items() -> void;
		auto give_party_gold() -> void;
		auto give_party_random_items() -> void;
		auto give_party_random_status() -> void;
		auto give_party_xp() -> void;
		auto heal_party_to_full() -> void;
		auto harm_party_to_min() -> void;
		auto kill_party() -> void;
		auto toggle_light() -> void;
		auto toggle_shield() -> void;
		auto toggle_identify() -> void;
		auto give_party_quest_items() -> void;
		auto start_chest_event() -> void;
		auto show_debug() -> void;
		auto level_up_party() -> void;

		static const Action _actions[];
};

} // namespace Sorcery
