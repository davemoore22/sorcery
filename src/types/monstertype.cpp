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
#include "common/macro.hpp"

std::random_device Sorcery::MonsterType::_device;
std::mt19937_64 Sorcery::MonsterType::_random(_device());

auto Sorcery::MonsterType::get_type_id() const -> MonsterTypeID {

	return _type;
}

auto Sorcery::MonsterType::get_known_name() const -> std::string {

	return _known_name;
}

auto Sorcery::MonsterType::get_unknown_name() const -> std::string {

	return _unknown_name;
}

auto Sorcery::MonsterType::get_known_name_plural() const -> std::string {

	return _known_name_plural;
}

auto Sorcery::MonsterType::get_unknown_name_plural() const -> std::string {

	return _unknown_name_plural;
}

auto Sorcery::MonsterType::set_known_gfx(const unsigned int value) -> void {

	_known_gfx = value;
}

auto Sorcery::MonsterType::get_known_gfx() const -> unsigned int {

	return _unknown_gfx;
}

auto Sorcery::MonsterType::set_unknown_gfx(const unsigned int value) -> void {

	_unknown_gfx = value;
}

auto Sorcery::MonsterType::get_unknown_gfx() const -> unsigned int {

	return _unknown_gfx;
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

auto Sorcery::MonsterType::set_level(const unsigned int value) -> void {

	_level = value;
}

auto Sorcery::MonsterType::set_armour_class(const int value) -> void {

	_armour_class = value;
}

auto Sorcery::MonsterType::get_level() const -> unsigned int {

	return _level;
}

auto Sorcery::MonsterType::get_armour_class() const -> int {

	return _armour_class;
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

auto Sorcery::MonsterType::get_attacks_str() const -> std::string {

	std::string result;
	for (const auto atk : _attacks) {
		result.append(atk.str());
		result.append(" ");
	}

	TRIM(result);

	return result;
}

auto Sorcery::MonsterType::get_weaknesses() const -> std::string {

	return _weaknesses;
}

auto Sorcery::MonsterType::get_group_size() const -> Dice {

	return _group_size;
}

auto Sorcery::MonsterType::get_class() const -> MonsterClass {

	return _class;
}

auto Sorcery::MonsterType::set_group_size(const std::string value) -> void {

	_group_size = Dice{value};
}

auto Sorcery::MonsterType::get_hit_dice() const -> Dice {

	return _hit_dice;
}

auto Sorcery::MonsterType::set_hit_dice(const std::string value) -> void {

	_hit_dice = Dice{value};
}

auto Sorcery::MonsterType::get_category() const -> MonsterCategory {

	return _category;
}

auto Sorcery::MonsterType::set_category(const MonsterCategory value) -> void {

	_category = value;
}

auto Sorcery::MonsterType::set_class(const MonsterClass value) -> void {

	_class = value;
}

auto Sorcery::MonsterType::get_attacks() const -> std::vector<Dice> {

	return _attacks;
}

auto Sorcery::MonsterType::get_number_attacks() const -> unsigned int {

	return _attacks.size();
}

auto Sorcery::MonsterType::clear_attacks() -> void {

	_attacks.clear();
}

auto Sorcery::MonsterType::set_attack(Dice value) -> void {

	_attacks.emplace_back(value);
}

auto Sorcery::MonsterType::get_attack(const unsigned int attack) const -> Dice {

	if (attack < _attacks.size())
		return _attacks.at(attack);
	else
		return Dice{};
}

auto Sorcery::MonsterType::get_breath_weapon() const -> MonsterBreath {

	return _breath_weapon;
}

auto Sorcery::MonsterType::set_breath_weapon(const MonsterBreath value) -> void {

	_breath_weapon = value;
}

auto Sorcery::MonsterType::get_level_drain() const -> unsigned int {

	return _level_drain;
}

auto Sorcery::MonsterType::get_regeneration() const -> unsigned int {

	return _regeneration;
}

auto Sorcery::MonsterType::set_level_drain(const unsigned int value) -> void {

	_level_drain = value;
}

auto Sorcery::MonsterType::set_regeneration(const unsigned int value) -> void {

	_regeneration = value;
}

auto Sorcery::MonsterType::get_rewards() const -> std::tuple<unsigned int, unsigned int> {

	return std::make_tuple(_reward_1, _reward_2);
}

auto Sorcery::MonsterType::set_rewards(const unsigned int value_1, const unsigned int value_2) -> void {

	_reward_1 = value_1;
	_reward_2 = value_2;
}

auto Sorcery::MonsterType::set_resistances(MonsterResistances value) -> void {

	_resistances = value;
}

auto Sorcery::MonsterType::set_properties(MonsterProperties value) -> void {

	_properties = value;
}

auto Sorcery::MonsterType::has_resistance(MonsterResist value) -> bool {

	const auto res{unenum(value)};

	return _resistances[res];
}

auto Sorcery::MonsterType::has_property(MonsterProperty value) -> bool {

	const auto prop{unenum(value)};

	return _properties[prop];
}

auto Sorcery::MonsterType::get_properties() const -> MonsterProperties {

	return _properties;
}

auto Sorcery::MonsterType::get_resistances() const -> MonsterResistances {

	return _resistances;
}

auto Sorcery::MonsterType::get_xp() const -> unsigned int {

	return _xp;
}

auto Sorcery::MonsterType::set_xp(const unsigned int value) -> void {

	_xp = value;
}

auto Sorcery::MonsterType::get_partners() const -> std::tuple<unsigned int, unsigned int> {

	return std::make_tuple(_partner_type_id, _partner_chance);
}

auto Sorcery::MonsterType::set_partners(const unsigned int value_1, const unsigned int value_2) -> void {

	_partner_type_id = value_1;
	_partner_chance = value_2;
}

auto Sorcery::MonsterType::set_priest_level(const unsigned int value) -> void {

	_priest_level = value;
}

auto Sorcery::MonsterType::set_mage_level(const unsigned int value) -> void {

	_mage_level = value;
}

auto Sorcery::MonsterType::get_priest_level() const -> unsigned int {

	return _mage_level;
}

auto Sorcery::MonsterType::get_mage_level() const -> unsigned int {

	return _priest_level;
}

auto Sorcery::MonsterType::get_spell_resistance() const -> unsigned int {

	return _mage_level;
}

auto Sorcery::MonsterType::set_spell_resistance(const unsigned int value) -> void {

	_spell_resistance = value;
}

namespace Sorcery {

auto operator<<(std::ostream &out_stream, const Sorcery::MonsterType &monstertype) -> std::ostream & {

	const auto type{unenum(monstertype.get_type_id())};
	const auto name{monstertype.get_known_name()};

	const auto text{fmt::format("{}: {}", type, name)};

	return out_stream << text << std::flush;
}
} // namest