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

#include "types/monstertype.hpp"

std::random_device Sorcery::MonsterType::_device;
std::mt19937_64 Sorcery::MonsterType::_random(_device());

auto Sorcery::MonsterType::get_type_id() const -> MonsterTypeID {

	return _type;
}

auto Sorcery::MonsterType::get_known_name() const -> std::string {

	return _known_name;
}

auto Sorcery::MonsterType::set_known_gfx(const unsigned int value) -> void {

	_known_gfx = value;
}

auto Sorcery::MonsterType::get_known_gfx() const -> unsigned int {

	return _known_gfx;
}

auto Sorcery::MonsterType::set_type_id(const MonsterTypeID value) -> void {

	_type = value;
}

auto Sorcery::MonsterType::set_known_name(const std::string value) -> void {

	_known_name = value;
}

auto Sorcery::MonsterType::set_unknown_name(const std::string value) -> void {

	_unknown_name = value;
}

auto Sorcery::MonsterType::set_known_name_plural(const std::string value) -> void {

	_known_name_plural = value;
}

auto Sorcery::MonsterType::set_unknown_name_plural(const std::string value) -> void {

	_unknown_name_plural = value;
}

auto Sorcery::MonsterType::set_traits(const std::string value) -> void {

	_traits = value;
}

auto Sorcery::MonsterType::set_weaknesses(const std::string value) -> void {

	_weaknesses = value;
}

auto Sorcery::MonsterType::get_traits() const -> std::string {

	return _traits;
}

auto Sorcery::MonsterType::get_weaknesses() const -> std::string {

	return _weaknesses;
}

namespace Sorcery {

auto operator<<(std::ostream &out_stream, const Sorcery::MonsterType &monstertype) -> std::ostream & {

	const auto type{unenum(monstertype.get_type_id())};
	const auto name{monstertype.get_known_name()};

	const auto text{fmt::format("{}: {}", type, name)};

	return out_stream << text << std::flush;
}
} // names