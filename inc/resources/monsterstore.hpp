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

#include "common/include.hpp"
#include "common/macro.hpp"

#include "core/system.hpp"
#include "resources/define.hpp"
#include "types/monster.hpp"
#include "types/monstertype.hpp"

namespace Sorcery {

class MonsterStore {

	public:

		MonsterStore(System *system, const std::filesystem::path filename);
		MonsterStore() = delete;

		// Overload operators
		auto operator[](MonsterTypeID monster_type_id) const -> MonsterType;
		// auto operator()(ItemCategory category) const -> std::vector<ItemType>;
		auto operator[](std::string name) const -> MonsterType;

		// Public methods
		// auto get_a_monster(const MonsterTypeID monster_type_id) const -> Monster;
		// auto get_random_monster(
		//	const MonsterTypeID min_monster_type_id, const MonsterTypeID max_monster_type_id) const -> Monster;
		auto get_all_types() const -> std::vector<MonsterType>;

	private:

		// Private members
		System *_system;
		std::map<MonsterTypeID, MonsterType> _items;
		bool _loaded;

		// Private methods
		auto _load(const std::filesystem::path filename) -> bool;
		auto _parse_attacks(const std::string value) const -> std::vector<Dice>;
		auto _parse_breath_weapons(const std::string value) const -> MonsterBreath;
		auto _parse_regen(const std::string value) const -> unsigned int;
		auto _parse_level_drain(const std::string value) const -> unsigned int;
		auto _parse_resistances(const std::string value) const -> MonsterResistances;
		auto _parse_properties(const std::string value) const -> MonsterProperties;
};

}