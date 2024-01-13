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

#include "main.hpp"
#include "types/monstertype.hpp"

namespace Sorcery {

class Monster {

	public:

		// Constructors
		Monster();
		Monster(const MonsterType &monster_type);

		// Overloaded Operators
		auto friend operator<<(std::ostream &out_stream, const Monster &monster) -> std::ostream &;

		// Public Methods
		auto get_type_id() const -> MonsterTypeID;
		auto get_max_hp() const -> int;
		auto get_current_hp() const -> int;
		auto get_alive() const -> bool;
		auto get_silenced() const -> bool;
		auto get_awake() const -> bool;
		auto get_base_ac() const -> int;
		auto get_current_ac() const -> int;
		auto get_known() const -> bool;
		auto get_name() const -> std::string;

		auto mod_ac(const int value) -> void;
		auto mod_hp(const int value) -> void;

		auto set_known(const bool value) -> void;
		auto set_silenced(const bool value) -> void;
		auto set_awake(const bool value) -> void;
		auto set_alive(const bool value) -> void;
		auto set_name(const std::string value) -> void;

	private:

		// Private Members
		MonsterTypeID _type;
		int _max_hp;
		int _current_hp;
		bool _alive;
		bool _silenced;
		bool _awake;
		int _base_ac;
		int _current_ac;
		bool _known;
		std::string _name;

		long _id;
		static inline long s_id{0};
};

}