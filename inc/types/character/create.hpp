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

#include "common/enum.hpp" // for Align (ptr only), SpellType (ptr only)
#include <map>			   // for map
#include <string>		   // for string
#include <string_view>	   // for string_view

namespace Sorcery {
class Character;
}

namespace Sorcery {

class ConstCharacterCreate {

	public:
		explicit ConstCharacterCreate(const Character &character) noexcept;

		auto
		get_start_attribute(const Enums::Character::Attribute attribute) const
			-> unsigned int;
		auto get_start_attributes() const
			-> std::map<Enums::Character::Attribute, int>;
		auto get_points_left() const -> unsigned int;
		auto get_start_points() const -> unsigned int;
		auto get_stage() const -> Enums::Character::Stage;
		auto get_possible_classes() const
			-> std::map<Enums::Character::Class, bool>;
		auto get_possible_classes_display() -> std::string;
		auto can_change_class() const -> bool;

	protected:
		const Character *_character;

		auto _get_spells_known(Enums::Magic::SpellType spell_type,
							   unsigned int spell_level) -> unsigned int;
		auto _get_xp_for_level(unsigned int level) const -> int;
};

class CharacterCreate : public ConstCharacterCreate {

	public:
		explicit CharacterCreate(Character &character) noexcept;

		auto change_class(const Enums::Character::Class &value) -> void;
		auto create_random() -> void;
		auto create_quick() -> void;
		auto create_class_alignment(const Enums::Character::Class cclass,
									const Enums::Character::Align alignment)
			-> void;
		auto finalise() -> void;
		auto legate(const Enums::Character::Align &value) -> void;
		auto level_up() -> void;
		auto level_down() -> void;
		auto reset(const Enums::Character::Stage stage) -> void;
		auto set_alignment(const Enums::Character::Align &value) -> void;
		auto set_class(const Enums::Character::Class &value) -> void;
		auto set_name(std::string_view value) -> void;
		auto set_points_left(const unsigned int &value) -> void;
		auto set_possible_classes() -> void;
		auto set_race(const Enums::Character::Race &value) -> void;
		auto set_stage(const Enums::Character::Stage stage) -> void;
		auto set_start_attr() -> void;

	private:
		auto _set_racial_attributes() -> void;
		auto _generate_start_info() -> void;
		auto _regenerate_start_info() -> void;
		auto _legate_start_info() -> void;
		auto _generate_secondary_abil(bool initial, bool change_class,
									  bool legate) -> void;
		auto _set_starting_sp() -> void;
		auto _reset_starting_sp() -> void;
		auto _clear_sp() -> void;
		auto _set_start_spells() -> void;
		auto _reset_start_spells() -> void;
		auto _get_hp_per_level() -> int;
		auto _update_hp_for_level() -> int;
		auto _try_learn_spell(Enums::Magic::SpellType spell_type,
							  unsigned int spell_level) -> bool;
		auto _calculate_sp(Enums::Magic::SpellType spell_type,
						   unsigned int level_mod, unsigned int level_offset)
			-> void;
		auto _set_sp() -> bool;
		auto _update_stat_for_level(Enums::Character::Attribute attribute,
									std::string stat) -> std::string;
		auto _learn_spell(Enums::Magic::SpellID spell_id) -> void;

		Character *_m_character;
};

}