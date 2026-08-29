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

#include "types/character/magic.hpp"
#include "common/enum.hpp"				 // for SpellType, SpellType::ARCANE
#include "common/types.hpp"				 // for Spell
#include "core/context.hpp"				 // for Context
#include "core/resources.hpp"			 // for Resources
#include "resources/spellstore.hpp"		 // for SpellStore
#include "types/character/character.hpp" // for Character
#include <algorithm>					 // for find_if
#include <map>							 // for map
#include <memory>						 // for unique_ptr
#include <utility>						 // for pair
#include <vector>						 // for vector

Sorcery::ConstCharacterMagic::ConstCharacterMagic(
	const Character &character) noexcept
	: _character{&character} {}

auto Sorcery::ConstCharacterMagic::can_cast(
	const Enums::Magic::SpellType spell_type, const int spell_level) const
	-> bool {

	using enum Enums::Magic::SpellType;

	if (spell_type == ARCANE)
		return _character->_mage_cur_sp.at(spell_level) > 0;

	if (spell_type == DIVINE)
		return _character->_priest_cur_sp.at(spell_level) > 0;

	return false;
}

auto Sorcery::ConstCharacterMagic::get_calfo_uses_left() const -> int {

	// CALFO is a 2nd Level Priest Spell
	return _character->_priest_cur_sp.at(2);
}

auto Sorcery::ConstCharacterMagic::get_spells() const
	-> const std::vector<Spell> & {

	return _character->_spells;
}

auto Sorcery::ConstCharacterMagic::priest_current_spellpoints() const
	-> const std::map<unsigned int, unsigned int> & {

	return _character->_priest_cur_sp;
}

auto Sorcery::ConstCharacterMagic::mage_current_spellpoints() const
	-> const std::map<unsigned int, unsigned int> & {

	return _character->_mage_cur_sp;
}

auto Sorcery::ConstCharacterMagic::priest_max_spellpoints() const
	-> const std::map<unsigned int, unsigned int> & {

	return _character->_priest_max_sp;
}

auto Sorcery::ConstCharacterMagic::mage_max_spellpoints() const
	-> const std::map<unsigned int, unsigned int> & {

	return _character->_mage_max_sp;
}

//

Sorcery::CharacterMagic::CharacterMagic(Character &character) noexcept
	: ConstCharacterMagic{character},
	  _m_character{&character} {}

// Wizardry 1 - 3 Spells
auto Sorcery::CharacterMagic::create_spells() -> void {

	_m_character->_spells.clear();
	_m_character->_spells = _m_character->_ctx->resources->spells->get_all();
}

auto Sorcery::CharacterMagic::reset_spells() -> void {

	for (auto &spell : _m_character->_spells)
		_m_character->_spells_known[spell.id] = spell.known;
}

auto Sorcery::CharacterMagic::replenish_spells() -> void {

	for (auto level = 1; level <= 7; level++) {
		_m_character->_mage_cur_sp.at(level) =
			_m_character->_mage_max_sp.at(level);
		_m_character->_priest_cur_sp.at(level) =
			_m_character->_priest_max_sp.at(level);
	}
}

auto Sorcery::CharacterMagic::set_spells() -> void {

	// Now for each spell known set the appropriate entry in the spells table
	for (auto &spell_known : _m_character->_spells_known) {

		std::vector<Spell>::iterator it;
		it = std::find_if(_m_character->_spells.begin(),
						  _m_character->_spells.end(), [&](auto item) {
							  return item.id == spell_known.first;
						  });
		if (it != _m_character->_spells.end())
			(*it).known = spell_known.second;
	}
}