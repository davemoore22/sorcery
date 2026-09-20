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

enum class SpellType {
	NO_SPELL_TYPE,
	ARCANE,
	DIVINE
};

enum class SpellPointType {
	NO_STATUS,
	CURRENT,
	MAXIMUM
};

enum class SpellCategory {
	NO_CATEGORY,
	HEALING,
	FIELD,
	SUPPORT,
	DISABLE,
	ATTACK
};

// Wizardry 1 to 3 Spells
enum class SpellID {
	NO_SPELL = -1,
	DUMAPIC = 0,	// Mage 1
	HALITO = 1,		// Mage 1
	KATINO = 2,		// Mage 1
	MOGREF = 3,		// Mage 1
	DILTO = 4,		// Mage 2
	SOPIC = 5,		// Mage 2
	MAHALITO = 6,	// Mage 3
	MOLITO = 7,		// Mage 3
	DALTO = 8,		// Mage 4
	LAHALITO = 9,	// Mage 4
	MORLIS = 10,	// Mage 4
	MADALTO = 11,	// Mage 5
	MAKANITO = 12,	// Mage 5
	MAMORLIS = 13,	// Mage 5
	HAMAN = 14,		// Mage 6
	LAKANITO = 15,	// Mage 6
	MASOPIC = 16,	// Mage 6
	ZILWAN = 17,	// Mage 6
	MAHAMAN = 18,	// Mage 7
	MALOR = 19,		// Mage 7
	TILTOWAIT = 20, // Mage 7
	BADIOS = 21,	// Priest 1
	DIOS = 22,		// Priest 1
	KALKI = 23,		// Priest 1
	MILWA = 24,		// Priest 1
	PORFIC = 25,	// Priest 1
	CALFO = 26,		// Priest 2
	MANIFO = 27,	// Priest 2
	MATU = 28,		// Priest 2
	MONTINO = 29,	// Priest 2
	BAMATU = 30,	// Priest 3
	DIALKO = 31,	// Priest 3
	LATUMAPIC = 32, // Priest 3
	LOMILWA = 33,	// Priest 3
	BADIAL = 34,	// Priest 4
	DIAL = 35,		// Priest 4
	LATUMOFIS = 36, // Priest 4
	MAPORFIC = 37,	// Priest 4
	BADI = 38,		// Priest 5
	BADIALMA = 39,	// Priest 5
	DI = 40,		// Priest 5
	DIALMA = 41,	// Priest 5
	KANDI = 42,		// Priest 5
	LITOKAN = 43,	// Priest 5
	LOKTOFEIT = 44, // Priest 6
	LORTO = 45,		// Priest 6
	MABADI = 46,	// Priest 6
	MADI = 47,		// Priest 6
	KADORTO = 48,	// Priest 7
	MALIKTO = 49	// Priest 7
};

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