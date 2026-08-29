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

#include <map>	  // for map
#include <vector> // for vector
namespace Sorcery {
class Character;
} // lines 38-38
namespace Sorcery {
namespace Enums {
	namespace Magic {
		enum class SpellType;
	}
}
}
namespace Sorcery {
struct Spell;
}

namespace Sorcery {

class ConstCharacterMagic {

	public:
		explicit ConstCharacterMagic(const Character &character) noexcept;

		auto can_cast(Enums::Magic::SpellType spell_type, int spell_level) const
			-> bool;
		auto get_calfo_uses_left() const -> int;
		auto get_spells() const -> const std::vector<Spell> &;
		auto priest_current_spellpoints() const
			-> const std::map<unsigned int, unsigned int> &;
		auto mage_current_spellpoints() const
			-> const std::map<unsigned int, unsigned int> &;
		auto priest_max_spellpoints() const
			-> const std::map<unsigned int, unsigned int> &;
		auto mage_max_spellpoints() const
			-> const std::map<unsigned int, unsigned int> &;

	protected:
		const Character *_character;
};

class CharacterMagic : public ConstCharacterMagic {

	public:
		explicit CharacterMagic(Character &character) noexcept;

		auto create_spells() -> void;
		auto reset_spells() -> void;
		auto replenish_spells() -> void;
		auto set_spells() -> void;

	private:
		Character *_m_character;
};

}