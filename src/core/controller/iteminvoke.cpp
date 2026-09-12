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
#include "types/game.hpp"

auto Sorcery::apply_invoke(Game *game, Character &character, const Enums::Items::Effects::Invoke effect) -> bool {

	using enum Enums::Items::Effects::Invoke;

	switch (effect) {

	case BECOME_NINJA:
		character.create().set_class(Enums::Character::Class::NINJA);
		return true;

	case HEAL_ALL:
		for (const auto character_id : game->state->get_party_characters()) {
			auto &member{game->characters.at(character_id)};
			member.set_current_hp(member.get_max_hp());
		}
		return true;

	case INC_HP:
		character.adjust_max_hp(1);
		return true;

	case INC_STRENGTH:
		if (character.get_cur_attr(Enums::Character::Attribute::STRENGTH) < 18) {
			character.adjust_attribute(Enums::Character::Attribute::STRENGTH, 1);
		}
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