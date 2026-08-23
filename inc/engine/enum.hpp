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

namespace Sorcery {

namespace Enums {

	namespace Traps {

		enum class Type {
			NONE,
			POISON_NEEDLE,
			GAS_BOMB,
			CROSSBOW_BOLT,
			EXPLODING_BOX,
			SPLINTERS,
			BLADES,
			STUNNER,
			TELEPORTER,
			ANTI_MAGE,
			ANTI_PRIEST,
			ALARM,
		};

	}

	namespace Chests {

		enum class State {
			MENU,
			CHOOSE_OPEN_CHARACTER,
			CHOOSE_INSPECT_CHARACTER,
			CHOOSE_CALFO_CHARACTER,
			CHOOSE_DISARM_CHARACTER,
			CHOOSE_TRAP,
			SHOW_RESULT,
			DONE
		};

		enum class Result {
			NONE,
			ABORT,
			OPENED,
			LEFT,
			PARTY_WIPE
		};
	}

}
};