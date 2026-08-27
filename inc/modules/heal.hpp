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

#include "common/sdl2.hpp"
#include "core/module.hpp"

#include <atomic>
#include <cstdint>

namespace Sorcery {

// Forward Declarations
struct Context;
class Character;

class Heal final : public Module {

	public:
		// Standard Constructor
		Heal(Context &ctx);

		// Public Methods
		auto start() -> int;
		auto stop() -> int;

	private:
		// Private Methods
		static auto _callback_heal_tick(std::uint32_t, void *param)
			-> std::uint32_t;

		auto _try_heal(int heal_char_id, int pay_char_id) -> bool;

		// Private Members
		std::atomic_int _stage{4};
		SDL_TimerID _heal_tick{};
		bool _healing_done{false};
};

} // namespace Sorcery