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

#include "resources/spellstore.hpp"
#include "common/types.hpp" // for Spell
#include "core/context.hpp" // for Context
#include "magic/enum.hpp"	// for CastUsage, SpellCategory, SpellID, Spell...
#include "types/meta.hpp"	// for enum_name
#include <algorithm>		// for find
#include <array>			// for array
#include <cassert>			// for assert
#include <format>			// for format
#include <ranges>			// for filter_view, _Filter, _Partial, _To2
#include <string>			// for basic_string
#include <string_view>		// for string_view
#include <vector>			// for vector

namespace {

using SpellID = Sorcery::Enums::Magic::SpellID;
using SpellType = Sorcery::Enums::Magic::SpellType;
using SpellCategory = Sorcery::Enums::Magic::SpellCategory;
using CastUsage = Sorcery::Enums::Magic::CastUsage;

using enum SpellID;
using enum SpellType;
using enum SpellCategory;

struct SpellDefinition {
		SpellID id;
		SpellType type;
		SpellCategory category;
		unsigned int level;
		CastUsage usage;
};

constexpr std::array SPELL_DEFINITIONS{

	// Arcane - Level 1
	SpellDefinition{DUMAPIC, ARCANE, FIELD, 1, CastUsage::USE_IN_FIELD},
	SpellDefinition{HALITO, ARCANE, ATTACK, 1, CastUsage::USE_IN_COMBAT},
	SpellDefinition{KATINO, ARCANE, DISABLE, 1, CastUsage::USE_IN_COMBAT},
	SpellDefinition{MOGREF, ARCANE, SUPPORT, 1, CastUsage::USE_IN_COMBAT},

	// Arcane - Level 2
	SpellDefinition{DILTO, ARCANE, DISABLE, 2, CastUsage::USE_IN_COMBAT},
	SpellDefinition{SOPIC, ARCANE, SUPPORT, 2, CastUsage::USE_IN_COMBAT},

	// Arcane - Level 3
	SpellDefinition{MAHALITO, ARCANE, ATTACK, 3, CastUsage::USE_IN_COMBAT},
	SpellDefinition{MOLITO, ARCANE, ATTACK, 3, CastUsage::USE_IN_COMBAT},

	// Arcane - Level 4
	SpellDefinition{DALTO, ARCANE, ATTACK, 4, CastUsage::USE_IN_COMBAT},
	SpellDefinition{LAHALITO, ARCANE, ATTACK, 4, CastUsage::USE_IN_COMBAT},
	SpellDefinition{MORLIS, ARCANE, DISABLE, 4, CastUsage::USE_IN_COMBAT},

	// Arcane - Level 5
	SpellDefinition{MADALTO, ARCANE, ATTACK, 5, CastUsage::USE_IN_COMBAT},
	SpellDefinition{MAKANITO, ARCANE, ATTACK, 5, CastUsage::USE_IN_COMBAT},
	SpellDefinition{MAMORLIS, ARCANE, DISABLE, 5, CastUsage::USE_IN_COMBAT},

	// Arcane - Level 6
	SpellDefinition{HAMAN, ARCANE, SUPPORT, 6, CastUsage::USE_IN_FIELD | CastUsage::USE_IN_COMBAT},
	SpellDefinition{LAKANITO, ARCANE, ATTACK, 6, CastUsage::USE_IN_COMBAT},
	SpellDefinition{MASOPIC, ARCANE, ATTACK, 6, CastUsage::USE_IN_COMBAT},
	SpellDefinition{ZILWAN, ARCANE, ATTACK, 6, CastUsage::USE_IN_COMBAT},

	// Arcane - Level 7
	SpellDefinition{MAHAMAN, ARCANE, SUPPORT, 7, CastUsage::USE_IN_FIELD | CastUsage::USE_IN_COMBAT},
	SpellDefinition{MALOR, ARCANE, FIELD, 7, CastUsage::USE_IN_FIELD | CastUsage::USE_IN_COMBAT},
	SpellDefinition{TILTOWAIT, ARCANE, ATTACK, 7, CastUsage::USE_IN_COMBAT},

	// Divine - Level 1
	SpellDefinition{BADIOS, DIVINE, ATTACK, 1, CastUsage::USE_IN_COMBAT},
	SpellDefinition{DIOS, DIVINE, HEALING, 1, CastUsage::USE_IN_FIELD | CastUsage::USE_IN_COMBAT},
	SpellDefinition{KALKI, DIVINE, SUPPORT, 1, CastUsage::USE_IN_COMBAT},
	SpellDefinition{MILWA, DIVINE, FIELD, 1, CastUsage::USE_IN_FIELD},
	SpellDefinition{PORFIC, DIVINE, SUPPORT, 1, CastUsage::USE_IN_COMBAT},

	// Divine - Level 2
	SpellDefinition{CALFO, DIVINE, FIELD, 2, CastUsage::USE_IN_TRAPS},
	SpellDefinition{MANIFO, DIVINE, DISABLE, 2, CastUsage::USE_IN_COMBAT},
	SpellDefinition{MATU, DIVINE, SUPPORT, 2, CastUsage::USE_IN_COMBAT},
	SpellDefinition{MONTINO, DIVINE, DISABLE, 2, CastUsage::USE_IN_COMBAT},

	// Divine - Level 3
	SpellDefinition{BAMATU, DIVINE, SUPPORT, 3, CastUsage::USE_IN_COMBAT},
	SpellDefinition{DIALKO, DIVINE, HEALING, 3, CastUsage::USE_IN_FIELD | CastUsage::USE_IN_COMBAT},
	SpellDefinition{LATUMAPIC, DIVINE, FIELD, 3, CastUsage::USE_IN_FIELD},
	SpellDefinition{LOMILWA, DIVINE, FIELD, 3, CastUsage::USE_IN_FIELD},

	// Divine - Level 4
	SpellDefinition{BADIAL, DIVINE, ATTACK, 4, CastUsage::USE_IN_COMBAT},
	SpellDefinition{DIAL, DIVINE, HEALING, 4, CastUsage::USE_IN_FIELD | CastUsage::USE_IN_COMBAT},
	SpellDefinition{LATUMOFIS, DIVINE, HEALING, 4, CastUsage::USE_IN_FIELD | CastUsage::USE_IN_COMBAT},
	SpellDefinition{MAPORFIC, DIVINE, FIELD, 4, CastUsage::USE_IN_FIELD},

	// Divine - Level 5
	SpellDefinition{BADI, DIVINE, ATTACK, 5, CastUsage::USE_IN_COMBAT},
	SpellDefinition{BADIALMA, DIVINE, ATTACK, 5, CastUsage::USE_IN_COMBAT},
	SpellDefinition{DI, DIVINE, HEALING, 5, CastUsage::USE_IN_FIELD | CastUsage::USE_IN_COMBAT},
	SpellDefinition{DIALMA, DIVINE, HEALING, 5, CastUsage::USE_IN_FIELD | CastUsage::USE_IN_COMBAT},
	SpellDefinition{KANDI, DIVINE, FIELD, 5, CastUsage::USE_IN_FIELD},
	SpellDefinition{LITOKAN, DIVINE, ATTACK, 5, CastUsage::USE_IN_COMBAT},

	// Divine - Level 6
	SpellDefinition{LOKTOFEIT, DIVINE, FIELD, 6, CastUsage::USE_IN_FIELD | CastUsage::USE_IN_COMBAT},
	SpellDefinition{LORTO, DIVINE, ATTACK, 6, CastUsage::USE_IN_COMBAT},
	SpellDefinition{MABADI, DIVINE, ATTACK, 6, CastUsage::USE_IN_COMBAT},
	SpellDefinition{MADI, DIVINE, HEALING, 6, CastUsage::USE_IN_FIELD | CastUsage::USE_IN_COMBAT},

	// Divine - Level 7
	SpellDefinition{KADORTO, DIVINE, HEALING, 7, CastUsage::USE_IN_FIELD | CastUsage::USE_IN_COMBAT},
	SpellDefinition{MALIKTO, DIVINE, ATTACK, 7, CastUsage::USE_IN_COMBAT},
};

} // namespace

Sorcery::SpellStore::SpellStore(Context &ctx)
	: _ctx{ctx} {

	_load();
}

auto Sorcery::SpellStore::get(Enums::Magic::SpellID spell_id) const -> Spell {

	const auto it = std::ranges::find(_spells, spell_id, &Spell::id);

	// Always assumes a spell can be found
	assert(it != _spells.end());

	return *it;
}
auto Sorcery::SpellStore::get(Enums::Magic::SpellCategory category) const -> std::vector<Spell> {

	return _spells | std::views::filter([category](const Spell &spell) {
			   return spell.category == category;
		   }) |
		   std::ranges::to<std::vector>();
}

auto Sorcery::SpellStore::get(std::string_view name) const -> Spell {
	const auto it = std::ranges::find(_spells, name, &Spell::name);

	// Always assumes a spell can be found
	assert(it != _spells.end());

	return *it;
}

auto Sorcery::SpellStore::get_all() -> std::vector<Spell> {

	return _spells;
}

auto Sorcery::SpellStore::_load() -> void {

	_loaded = false;
	_spells.clear();
	_spells.reserve(SPELL_DEFINITIONS.size());

	for (const auto &[id, type, category, level, usage] : SPELL_DEFINITIONS) {

		const auto spell_name{enum_name(id)};
		const auto key{std::format("SPELL_{}", spell_name)};

		_spells.emplace_back(id, type, category, usage, level, _ctx.get_string(std::format("{}_NAME", key)),
							 _ctx.get_string(std::format("{}_TITLE", key)),
							 _ctx.get_string(std::format("{}_DESC", key)));
	}

	_loaded = true;
}