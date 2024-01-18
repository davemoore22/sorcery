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

namespace Enums::Automap {

	enum class Feature {
		NO_FEATURE = -1,
		FLOOR = 0,
		NORTH_WALL = 1,
		SOUTH_WALL = 2,
		EAST_WALL = 3,
		WEST_WALL = 4,
		NORTH_DOOR = 5,
		SOUTH_DOOR = 6,
		EAST_DOOR = 7,
		WEST_DOOR = 8,
		NORTH_SECRET = 9,
		SOUTH_SECRET = 10,
		EAST_SECRET = 11,
		WEST_SECRET = 12,
		DARKNESS = 13,
		STAIRS_DOWN = 14,
		STAIRS_UP = 15,
		CHUTE = 16,
		ELEVATOR = 17,
		ROCK = 18,
		PIT = 19,
		SPINNER = 20,
		TELEPORT_FROM = 21,
		ENCOUNTER = 22,
		ANTI_MAGIC = 23,
		LAIR = 24,
		FOUNTAIN = 25,
		POOL = 26,
		LETTER_C = 27,
		LETTER_T = 28,
		EXCLAMATION = 29,
		QUESTION_MARK = 30,
		NORTH_FORCED = 31,
		SOUTH_FORCED = 32,
		EAST_FORCED = 33,
		WEST_FORCED = 34,
		STAR = 35,
		MINUS_HP = 36,
		PLUS_HP = 37,
		PLAYER_NORTH = 38,
		PLAYER_SOUTH = 39,
		PLAYER_EAST = 40,
		PLAYER_WEST = 41,
		NORTH_ONE_WAY_WALL = 42,
		SOUTH_ONE_WAY_WALL = 43,
		EAST_ONE_WAY_WALL = 44,
		WEST_ONE_WAY_WALL = 45,
		TELEPORT_TO = 46
	};
}

namespace Enums::Window {

	enum class Alignment {
		NO_ALIGNMENT,
		VERTICAL,
		HORIZONTAL
	};

	enum class InputCategory {
		MOUSE_MOVE = 0,
		UP = 1,
		DOWN = 2,
		LEFT = 3,
		RIGHT = 4,
		CONFIRM = 5,
		YES_NO = 6,
		ALPHANUMERIC = 7,
		PAGE_UP = 8,
		PAGE_DOWN = 9,
		HOME = 10,
		END = 11,
		BACK_DELETE_CANCEL = 12,
		ESCAPE = 13,
		HELP = 14,
		SHOW_CHARACTER = 15,
		MAZE_FORWARD = 16,
		MAZE_BACKWARD = 17,
		MAZE_LEFT = 18,
		MAZE_RIGHT = 19,
		MAZE_TURN_AROUND = 20,
		MAZE_CAMP = 21,
		MAZE_ACTION = 22,
		MAZE_SEARCH = 23,
		MAZE_INSPECT = 24,
		MAZE_GUI_TOGGLE = 25,
		MAZE_STATUSBAR_TOGGLE = 26,
		MAZE_QUIT = 27
	};

	enum class FrameParts {
		NO_PART = -1,
		TOP_LEFT = 0,
		TOP = 1,
		TOP_RIGHT = 2,
		LEFT = 3,
		BOTTOM_LEFT = 4,
		BOTTOM = 5,
		BOTTOM_RIGHT = 6,
		RIGHT = 7
	};
};

using AutoMapFeature = Enums::Automap::Feature;
using Alignment = Enums::Window::Alignment;
using WindowInputCategory = Enums::Window::InputCategory;
using WindowFrameParts = Enums::Window::FrameParts;

}