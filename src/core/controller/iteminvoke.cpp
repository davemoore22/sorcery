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

#include "core/controller/iteminvoke.hpp"
#include "core/debug.hpp"
#include "types/character/character.hpp"
#include "types/enum.hpp"

auto Sorcery::apply_invoke(Character &character,
						   const Enums::Items::Effects::Invoke effect) -> bool {

	using enum Enums::Items::Effects::Invoke;

	switch (effect) {

	case BECOME_NINJA:

		DEBUG_LOG("BECOME_NINJA INVOKE");
		// perform class change
		return true;

	case HEAL_ALL:
		DEBUG_LOG("HEAL_ALL INVOKE");
		// character.set_current_hp(character.get_max_hp());
		return true;

	case INC_HP:
		// increase max HP according to original rule
		DEBUG_LOG("INC_HP INVOKE");
		return true;

	case INC_STRENGTH:
		DEBUG_LOG("INC_STRENGTH INVOKE");
		// increase strength according to original rule
		return true;

	case AGE_BY_A_YEAR:
	case BECOME_LORD:
	case BECOME_SAMURAI:
	case GRANT_50000_EXP:
	case GRANT_50000_GOLD:
	case STATUS_TO_LOST:
		// Implementable, but unused in Wiz 1.
		DEBUG_LOG("NOT IMPLEMENTED IN WIZ 1 INVOKE");
		return true;

	case NO_INV_EFFECT:
	default:
		return false;
	}
}