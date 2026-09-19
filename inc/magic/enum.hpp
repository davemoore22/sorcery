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

#include <cstdint>

namespace Sorcery::Enums::Magic {

enum class CastContext {
	FIELD,
	COMBAT,
	TRAPS,
};

enum class CastUsage : std::uint8_t {
	NONE = 0,
	USE_IN_FIELD = 1 << 0,
	USE_IN_COMBAT = 1 << 1,
	USE_IN_TRAPS = 1 << 2
};

constexpr auto operator|(const CastUsage lhs, const CastUsage rhs) -> CastUsage {

	return static_cast<CastUsage>(static_cast<std::uint8_t>(lhs) | static_cast<std::uint8_t>(rhs));
}

constexpr auto operator&(const CastUsage lhs, const CastUsage rhs) -> CastUsage {

	return static_cast<CastUsage>(static_cast<std::uint8_t>(lhs) & static_cast<std::uint8_t>(rhs));
}

[[nodiscard]]
constexpr auto has_usage(const CastUsage usage, const CastUsage flag) -> bool {

	return (usage & flag) != CastUsage::NONE;
}

}