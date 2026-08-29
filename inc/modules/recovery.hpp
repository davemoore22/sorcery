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

#include "core/module.hpp" // for Module
#include <SDL_timer.h>	   // for SDL_TimerID
#include <atomic>		   // for atomic_bool
#include <cstdint>		   // for uint32_t

namespace Sorcery {
class Character;
}
namespace Sorcery {
struct Context;
}

namespace Sorcery {

class Recovery final : public Module {

	public:
		Recovery(Context &ctx);

		auto start(int mode) -> int;
		auto stop() -> int;

	private:
		auto _initialise() -> bool;

		static auto _callback_napping(std::uint32_t, void *param)
			-> std::uint32_t;
		static auto _callback_recuperating(std::uint32_t, void *param)
			-> std::uint32_t;

		SDL_TimerID _rest_tick{};
		Character *_character{nullptr};

		int _mode{};
		std::atomic_bool _finished{false};
};

} // namespace Sorcery