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

#include "common/types.hpp"		 // for Spell
#include "magic/castcontext.hpp" // for CastContext
#include "magic/enum.hpp"		 // for CastUsage, has_usage, CastC...

auto Sorcery::Magic::can_cast_in(const Spell &spell, const Enums::Magic::CastContext context) -> bool {

	using enum Enums::Magic::CastContext;
	using enum Enums::Magic::CastUsage;

	switch (context) {

	case FIELD:
		return has_usage(spell.usage, USE_IN_FIELD);

	case COMBAT:
		return has_usage(spell.usage, USE_IN_COMBAT);

	case TRAPS:
		return has_usage(spell.usage, USE_IN_TRAPS);
	default:
		return false;
	}

	return false;
}
