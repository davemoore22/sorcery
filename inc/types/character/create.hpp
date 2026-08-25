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

#include "common/cereal.hpp"
#include "common/define.hpp"
#include "common/enum.hpp"
#include "common/types.hpp"
#include "types/define.hpp"
#include "types/enum.hpp"
#include "types/inventory.hpp"

#pragma once

#include "types/enum.hpp"

namespace Sorcery {

class Character;

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

	protected:
		const Character *_character;
};

class CharacterCreate : public ConstCharacterCreate {

	public:
		explicit CharacterCreate(Character &character) noexcept;

		auto create_random() -> void;
		auto create_quick() -> void;
		auto create_class_alignment(const Enums::Character::Class cclass,
									const Enums::Character::Align alignment)
			-> void;
		auto finalise() -> void;
		auto reset(const Enums::Character::Stage stage) -> void;
		auto set_points_left(const unsigned int &value) -> void;
		auto set_possible_classes() -> void;
		auto set_stage(const Enums::Character::Stage stage) -> void;
		auto set_start_attr() -> void;

	private:
		Character *_m_character;
};

}