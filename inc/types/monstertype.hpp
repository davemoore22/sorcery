// Copyright (C) 2024 Dave Moore
//
// This file is part of Sorcery: Shadows under Llylgamyn.
//
// Sorcery: Shadows under Llylgamyn is free software: you can redistribute
// it and/or modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation, either version 2 of the License,
// or (at your option) any later version.
//
// Sorcery: Shadows under Llylgamyn is distributed in the hope that it wil
// be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
// Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Sorcery: Shadows under Llylgamyn.  If not,
// see <http://www.gnu.org/licenses/>.
//
// If you modify this program, or any covered work, by linking or combining
// it with the libraries referred to in README (or a modified version of
// said libraries), containing parts covered by the terms of said libraries,
// the licensors of this program grant you additional permission to convey
// the resulting work.

#pragma once

#include "core/system.hpp"
#include "main.hpp"

namespace Sorcery {

class MonsterType {

	public:

		// Default Constructor
		MonsterType() = default;

		// Overloaded Operators
		auto friend operator<<(std::ostream &out_stream, const MonsterType &ItemType) -> std::ostream &;

		// Public Methods
		auto get_type_id() const -> MonsterTypeID;
		auto get_known_name() const -> std::string;
		auto get_unknown_name() const -> std::string;
		auto get_known_name_plural() const -> std::string;
		auto get_unknown_name_plural() const -> std::string;
		auto get_group_size() const -> Dice;
		auto get_traits() const -> std::string;
		auto get_weaknesses() const -> std::string;
		auto get_level() const -> unsigned int;
		auto get_known_gfx() const -> unsigned int;
		auto get_hit_dice() const -> Dice;
		auto get_category() const -> MonsterCategory;

		auto set_type_id(const MonsterTypeID value) -> void;
		auto set_known_name(const std::string value) -> void;
		auto set_unknown_name(const std::string value) -> void;
		auto set_known_name_plural(const std::string value) -> void;
		auto set_unknown_name_plural(const std::string value) -> void;
		auto set_group_size(const std::string value) -> void;
		auto set_level(const unsigned int value) -> void;
		auto set_known_gfx(const unsigned int value) -> void;
		auto set_traits(const std::string value) -> void;
		auto set_weaknesses(const std::string value) -> void;
		auto set_hit_dice(const std::string value) -> void;
		auto set_category(const MonsterCategory value) -> void;

	private:

		// Private Members
		MonsterTypeID _type;
		std::string _known_name;
		std::string _unknown_name;
		std::string _known_name_plural;
		std::string _unknown_name_plural;
		Dice _group_size;
		unsigned int _level;
		Dice _hit_dice;
		unsigned int _known_gfx;
		MonsterCategory _category;
		MonsterClass _class;

		std::string _weaknesses;
		std::string _traits;

		static std::random_device _device; // Shared RNG
		static std::mt19937_64 _random;
};

}
