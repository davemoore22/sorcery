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
#include <atomic>		   // for atomic_bool, atomic_int
#include <cstdint>		   // for uint32_t
#include <memory>		   // for unique_ptr

namespace Sorcery { class Inspect; }
namespace Sorcery { struct Context; }

namespace Sorcery {

class Rite final : public Module {

	public:
		// Standard Constructor
		Rite(Context &ctx);

		~Rite();

		// Public Methods
		auto start() -> int;
		auto stop() -> int;

	private:
		// Private Methods
		static auto _callback_rite_tick(std::uint32_t interval, void *param)
			-> std::uint32_t;

		auto _initialise() -> bool;

		// Private Members
		std::atomic_int _stage{1};
		std::atomic_bool _stage_visible{true};
		std::atomic_bool _rite_ready{false};
		SDL_TimerID _rite_tick{};

		std::unique_ptr<Inspect> _inspect;
};
};