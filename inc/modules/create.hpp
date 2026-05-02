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

#include "common/enum.hpp"
#include "common/include.hpp"
#include "common/types.hpp"
#include "engine/define.hpp"
#include "engine/enum.hpp"

namespace Sorcery {

// Forward Declarations
struct Context;
class Character;

class Create {

	public:
		// Standard Constructor
		Create(Context &ctx);

		// Standard Destructor
		~Create();

		// Public Members

		// Public Methods
		auto start() -> int;
		auto stop() -> int;

	private:
		// Private Methods
		auto _initialise() -> bool;

		// Private Members
		Context &_ctx;
		Enums::Character::Stage _stage;
		std::vector<Character> _stages;
};

}