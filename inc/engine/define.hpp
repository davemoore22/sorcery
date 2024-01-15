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

#include "common/include.hpp"

namespace Sorcery {

using namespace std::literals;

inline constexpr auto CONTINUE{-1};
inline constexpr auto STOP_ENGINE{1};
inline constexpr auto STOP_ALL{0};

inline constexpr auto MAP_SIZE{20};

// View Depth
inline constexpr auto DARK_VIEW_DEPTH{3u};
inline constexpr auto LIGHT_VIEW_DEPTH{6u};

// Timed Settings (Milliseconds)
inline constexpr auto DELAY_OUCH{1000u};
inline constexpr auto DELAY_PIT{2000u};
inline constexpr auto DELAY_CHUTE{2000u};
inline constexpr auto DELAY_FIND_AN_ITEM{2000u};
inline constexpr auto DELAY_ENCOUNTER{2000u};
inline constexpr auto DELAY_ELEVATOR{2000u};
inline constexpr auto DELAY_RESTING{2000u};
inline constexpr auto DELAY_DIRECTION{150u};

// View settings (before Scaling is applied)
inline constexpr auto VIEW_WIDTH{304};
inline constexpr auto VIEW_HEIGHT{176};

}