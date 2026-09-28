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

#include "resources/monsterstore.hpp"
#include "resources/json.hpp"			 // for get_string, get_uint, get_enum, get...
#include "types/dice.hpp"				 // for Dice
#include "types/enum.hpp"				 // for Property, Resistance, Breath, Category
#include "types/meta.hpp"				 // for enum_cast
#include "types/monster/monstertype.hpp" // for MonsterType
#include <algorithm>					 // for remove_if
#include <array>						 // for array
#include <cstddef>						 // for size_t
#include <filesystem>					 // for path
#include <fstream>						 // for basic_ifstream, ifstream
#include <json/reader.h>				 // for Reader
#include <json/value.h>					 // for Value
#include <map>							 // for map
#include <optional>						 // for optional
#include <regex>						 // for regex, regex_replace, regex_token_i...
#include <string>						 // for basic_string, string, stoi
#include <string_view>					 // for basic_string_view, string_view
#include <utility>						 // for pair, get, to_underlying, move
#include <vector>						 // for vector

namespace {

template <typename E, std::size_t N, std::size_t M>
auto parse_flags(const std::string_view value, const std::array<std::pair<std::string_view, E>, M> &mapping)
	-> std::array<bool, N> {

	std::array<bool, N> flags{};

	for (const auto &[name, flag] : mapping)
		flags[std::to_underlying(flag)] = value.contains(name);

	return flags;
}

constexpr std::array RESISTANCE_FLAGS{
	std::pair{std::string_view{"Cold"}, Sorcery::Enums::Monsters::Resistance::RESIST_COLD},
	std::pair{std::string_view{"Drain"}, Sorcery::Enums::Monsters::Resistance::RESIST_LEVEL_DRAIN},
	std::pair{std::string_view{"Fire"}, Sorcery::Enums::Monsters::Resistance::RESIST_FIRE},
	std::pair{std::string_view{"Magic"}, Sorcery::Enums::Monsters::Resistance::RESIST_MAGIC},
	std::pair{std::string_view{"Poison"}, Sorcery::Enums::Monsters::Resistance::RESIST_POISON},
	std::pair{std::string_view{"Physical"}, Sorcery::Enums::Monsters::Resistance::RESIST_PHYSICAL},
	std::pair{std::string_view{"Stone"}, Sorcery::Enums::Monsters::Resistance::RESIST_STONING},
};

constexpr std::array PROPERTY_FLAGS{
	std::pair{std::string_view{"Critical"}, Sorcery::Enums::Monsters::Property::CAN_AUTOKILL},
	std::pair{std::string_view{"Sleep"}, Sorcery::Enums::Monsters::Property::CAN_BE_SLEPT},
	std::pair{std::string_view{"Call"}, Sorcery::Enums::Monsters::Property::CAN_CALL_FOR_OTHERS},
	std::pair{std::string_view{"Run"}, Sorcery::Enums::Monsters::Property::CAN_FLEE},
	std::pair{std::string_view{"Paralyse"}, Sorcery::Enums::Monsters::Property::CAN_PARALYSE},
	std::pair{std::string_view{"Stone"}, Sorcery::Enums::Monsters::Property::CAN_PETRIFY},
	std::pair{std::string_view{"Poison"}, Sorcery::Enums::Monsters::Property::CAN_POISON},
};

} // namespace

// Standard Constructor
Sorcery::MonsterStore::MonsterStore(const std::filesystem::path filename) {

	_items.clear();

	// Load the Item Definitions
	_loaded = _load(filename);
}

auto Sorcery::MonsterStore::_load(const std::filesystem::path filename) -> bool {

	if (std::ifstream file{filename.string(), std::ifstream::binary}; file.good()) {

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
		Json::Reader reader{};
#pragma GCC diagnostic pop

		if (Json::Value data; reader.parse(file, data)) {

			const auto &items{data["monster"]};

			for (auto i = 0u; i < items.size(); ++i) {

				using enum Enums::Monsters::Category;
				using enum Enums::Monsters::Class;

				const auto &item{items[i]};

				const auto type_id{JsonHelper::get_enum<Enums::Monsters::TypeID>(item, "id").value()};
				const auto known_name{JsonHelper::get_string(item, "known name")};
				const auto unknown_name{JsonHelper::get_string(item, "unknown name")};
				const auto known_name_plural{JsonHelper::get_string(item, "known name plural")};
				const auto unknown_name_plural{JsonHelper::get_string(item, "unknown name plural")};
				const auto known_gfx{JsonHelper::get_int(item, "gfx known index")};
				const auto unknown_gfx{JsonHelper::get_int(item, "gfx unknown index")};
				const auto group_size{JsonHelper::get_string(item, "group size")};
				const auto level{JsonHelper::get_uint(item, "level")};
				const auto hit_dice{JsonHelper::get_string(item, "hit dice")};
				const auto category{JsonHelper::get_enum(item, "category", HUMANOID)};
				const auto mclass{category == HUMANOID ? JsonHelper::get_enum(item, "category", NO_CLASS) : NO_CLASS};
				const auto ac{JsonHelper::get_int(item, "ac", 10)};
				const auto attacks{_parse_attacks(JsonHelper::get_string(item, "attacks"))};
				const auto special{JsonHelper::get_string(item, "special")};
				const auto level_drain{_parse_level_drain(special)};
				const auto regeneration{_parse_regen(special)};
				const auto breath_weapon{_parse_breath_weapons(special)};
				const auto rewards_1{JsonHelper::get_uint(item, "reward 1")};
				const auto rewards_2{JsonHelper::get_uint(item, "reward 2")};
				const auto resistances{_parse_resistances(JsonHelper::get_string(item, "resistances"))};
				const auto properties{_parse_properties(JsonHelper::get_string(item, "properties"))};
				const auto xp{JsonHelper::get_uint(item, "xp")};
				const auto partner_type_id{JsonHelper::get_uint(item, "partner id")};
				const auto partner_chance{JsonHelper::get_uint(item, "partner chance")};
				const auto mage_level{JsonHelper::get_uint(item, "mage level")};
				const auto priest_level{JsonHelper::get_uint(item, "priest level")};
				const auto spell_resistance{JsonHelper::get_uint(item, "spell resistance")};
				const auto traits{JsonHelper::get_string(item, "traits")};
				const auto weaknesses{JsonHelper::get_string(item, "weaknesses")};

				MonsterType monster_type{};

				monster_type.set_type_id(type_id);
				monster_type.set_known_name(known_name);
				monster_type.set_unknown_name(unknown_name);
				monster_type.set_known_name_plural(known_name_plural);
				monster_type.set_unknown_name_plural(unknown_name_plural);
				monster_type.set_known_gfx(known_gfx);
				monster_type.set_unknown_gfx(unknown_gfx);
				monster_type.set_group_size(group_size);
				monster_type.set_level(level);
				monster_type.set_hit_dice(hit_dice);
				monster_type.set_category(category);
				monster_type.set_class(mclass);
				monster_type.set_armour_class(ac);

				monster_type.clear_attacks();

				for (const auto &attack : attacks)
					monster_type.set_attack(attack);

				monster_type.set_breath_weapon(breath_weapon);
				monster_type.set_level_drain(level_drain);
				monster_type.set_regeneration(regeneration);
				monster_type.set_rewards(rewards_1, rewards_2);
				monster_type.set_resistances(resistances);
				monster_type.set_properties(properties);
				monster_type.set_xp(xp);
				monster_type.set_partners(partner_type_id, partner_chance);
				monster_type.set_priest_level(priest_level);
				monster_type.set_mage_level(mage_level);
				monster_type.set_spell_resistance(spell_resistance);
				monster_type.set_traits(traits);
				monster_type.set_weaknesses(weaknesses);

				_items[type_id] = std::move(monster_type);
			}

			return true;
		}
	}

	return false;
}

auto Sorcery::MonsterStore::get(Enums::Monsters::TypeID monster_type_id) const -> MonsterType {

	return _items.at(monster_type_id);
}

auto Sorcery::MonsterStore::get(int monster_type_id) const -> MonsterType {

	return _items.at(enum_cast<Enums::Monsters::TypeID>(monster_type_id).value());
}

auto Sorcery::MonsterStore::get_all_types() const -> std::vector<MonsterType> {

	std::vector<MonsterType> items;
	for (const auto &[key, value] : _items)
		items.push_back(value);

	return items;
}

auto Sorcery::MonsterStore::_parse_attacks(const std::string value) const -> std::vector<Dice> {

	std::vector<Dice> attacks;
	attacks.clear();

	const std::regex regex(R"([,]+)");
	std::sregex_token_iterator it{value.begin(), value.end(), regex, -1};
	std::vector<std::string> split{it, {}};
	split.erase(std::remove_if(split.begin(), split.end(),
							   [](std::string_view s) {
								   return s.size() == 0;
							   }),
				split.end());

	for (const auto &each : split) {

		Dice atk{each};
		attacks.emplace_back(atk);
	}

	return attacks;
}

auto Sorcery::MonsterStore::_parse_breath_weapons(const std::string value) const -> Enums::Monsters::Breath {

	using enum Enums::Monsters::Breath;
	if (value.contains("Drain Breath"))
		return DRAIN_BREATH;
	else if (value.contains("Flame Breath"))
		return FLAME_BREATH;
	else if (value.contains("Poison Breath"))
		return POISON_BREATH;
	else if (value.contains("Cold Breath"))
		return COLD_BREATH;
	else if (value.contains("Stone Breath"))
		return STONE_BREATH;
	else
		return NO_BREATH_WEAPON;
}

auto Sorcery::MonsterStore::_parse_level_drain(const std::string value) const -> unsigned int {

	if (value.contains("Level Drain")) {
		std::string output{std::regex_replace(value, std::regex("[^0-9]*([0-9]+).*"), std::string("$1"))};
		return std::stoi(output);
	} else
		return 0;
}

auto Sorcery::MonsterStore::_parse_regen(const std::string value) const -> unsigned int {

	auto pos{value.find("Heal")};
	if (pos != std::string::npos) {
		auto heal_portion{value.substr(pos)};
		std::string output{std::regex_replace(heal_portion, std::regex("[^0-9]*([0-9]+).*"), std::string("$1"))};
		return std::stoi(output);
	} else
		return 0;
}

auto Sorcery::MonsterStore::_parse_resistances(const std::string_view value) const -> std::array<bool, 7> {

	return parse_flags<Enums::Monsters::Resistance, 7>(value, RESISTANCE_FLAGS);
}

auto Sorcery::MonsterStore::_parse_properties(const std::string_view value) const -> std::array<bool, 7> {

	return parse_flags<Enums::Monsters::Property, 7>(value, PROPERTY_FLAGS);
}