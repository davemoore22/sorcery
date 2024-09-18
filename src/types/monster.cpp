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

#include "types/monster.hpp"
#include "common/macro.hpp"
#include "types/dice.hpp"
#include "types/monstertype.hpp"

// Default Constructor
Sorcery::Monster::Monster() {

	_type = MTI::BUBBLY_SLIME;
	_max_hp = 0;
	_current_hp = 0;
	_alive = false;
	_silenced = false;
	_awake = false;
	_base_ac = 10;
	_current_ac = 10;
	_known = false;
	_name = "";

	_id = s_id++;
}

Sorcery::Monster::Monster(const MonsterType &monster_type) {

	_type = monster_type.get_type_id();
	_max_hp = monster_type.get_hit_dice().roll();
	_current_hp = _max_hp;
	_alive = true;
	_silenced = false;
	_awake = true;
	_base_ac = monster_type.get_armour_class();
	_current_ac = _base_ac;
	_known = false;
	_name = monster_type.get_known_name();

	_id = s_id++;
}

auto Sorcery::Monster::get_type_id() const -> MTI {

	return _type;
}

auto Sorcery::Monster::get_max_hp() const -> int {

	return _max_hp;
}

auto Sorcery::Monster::get_current_hp() const -> int {

	return _current_hp;
}

auto Sorcery::Monster::get_alive() const -> bool {

	return _alive;
}

auto Sorcery::Monster::get_silenced() const -> bool {

	return _silenced;
}

auto Sorcery::Monster::get_awake() const -> bool {

	return _awake;
}

auto Sorcery::Monster::get_base_ac() const -> int {

	return _base_ac;
}

auto Sorcery::Monster::get_current_ac() const -> int {

	return _current_ac;
}

auto Sorcery::Monster::get_known() const -> bool {

	return _known;
}

auto Sorcery::Monster::get_name() const -> std::string {

	return _name;
}

auto Sorcery::Monster::mod_ac(const int value) -> void {

	_current_ac += value;
}

auto Sorcery::Monster::mod_hp(const int value) -> void {

	_current_hp += value;
}

auto Sorcery::Monster::set_known(const bool value) -> void {

	_known = value;
}

auto Sorcery::Monster::set_silenced(const bool value) -> void {

	_silenced = value;
}

auto Sorcery::Monster::set_awake(const bool value) -> void {

	_awake = value;
}

auto Sorcery::Monster::set_alive(const bool value) -> void {

	_alive = value;
}

auto Sorcery::Monster::set_name(const std::string value) -> void {

	_name = value;
}

namespace Sorcery {

auto operator<<(std::ostream &out_stream, const Sorcery::Monster &monster) -> std::ostream & {

	const auto type{unenum(monster.get_type_id())};
	const auto name{monster.get_name()};

	const auto text{fmt::format("{}: {}", type, name)};

	return out_stream << text << std::flush;
}
}