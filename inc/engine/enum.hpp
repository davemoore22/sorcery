// Copyright (C) 2024 Dave Moore
//
// This file is part of Sorcery: Shadows under Llylgamyn.
//
// Sorcery: Shadows under Llylgamyn is free software: you can redistribute
// it and/or modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation, either version 2 of the License,
// or (at your option) any later version.
//
// Sorcery: Shadows under Llylgamyn is distributed in the hope that it wil
// be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
// Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Sorcery: Shadows under Llylgamyn.  If not,
// see <http://www.gnu.org/licenses/>.
//
// If you modify this program, or any covered work, by linking or combining
// it with the libraries referred to in README (or a modified version of
// said libraries), containing parts covered by the terms of said libraries,
// the licensors of this program grant you additional permission to convey
// the resulting work.

#pragma once

// Enums
namespace Sorcery {

namespace Enums::Window {}

namespace Enums::Graphics {

	enum class Event {
		NO_IMAGE = -1,
		TREASURE = 0,
		GRAVESTONE = 1,
		CHEST = 2
	};
}

namespace Enums::Items {

	enum class Quest {
		NO_QUEST_ITEM = 0,
		AMULET_OF_WERDNA = 1,
		BEAR_STATUE = 2,
		BLUE_RIBBON = 3,
		BRONZE_KEY = 4,
		FROG_STATUE = 5,
		GOLD_KEY = 6,
		SILVER_KEY = 7
	};

}

namespace Enums::Combat {

	enum class Type {
		NO_COMBAT = -1,
		RANDOM = 0,
		ROOM = 1,
		MURPHYS_GHOSTS = 2,
		RING_PARTY = 3,
		DRAGONS = 4,
		WERDNA = 5
	};

}

using CombatType = Enums::Combat::Type;
using EventGraphic = Enums::Graphics::Event;
using ItemQuest = Enums::Items::Quest;

}