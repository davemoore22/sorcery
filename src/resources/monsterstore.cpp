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
#include "common/enum.hpp"
#include "common/include.hpp"
#include "common/macro.hpp"
#include "resources/define.hpp"
#include "types/monstertype.hpp"
#include <jsoncpp/json/json.h>

// Standard Constructor
Sorcery::MonsterStore::MonsterStore(const std::filesystem::path filename) {

	_items.clear();

	// Load the Item Definitions
	_loaded = _load(filename);
}

auto Sorcery::MonsterStore::_load(const std::filesystem::path filename)
	-> bool {

	if (std::ifstream file{filename.string(), std::ifstream::binary};
		file.good()) {

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
		Json::Reader reader{};
#pragma GCC diagnostic pop
		Json::StreamWriterBuilder builder{};
		builder.settings_["indentation"] = "";
		if (Json::Value data; reader.parse(file, data)) {
			Json::Value &items{data["monster"]};

			// Iterate through item file one itemtype at a time
			for (auto i = 0u; i < items.size(); i++) {

				// Some fields are always present
				const auto id{magic_enum::enum_cast<Enums::Monsters::TypeID>(
					items[i]["id"].asInt())};
				const std::string known_name{items[i]["known name"].asString()};
				const std::string unknown_name{
					items[i]["unknown name"].asString()};
				const std::string known_name_plural{
					items[i]["known name plural"].asString()};
				const std::string unknown_name_plural{
					items[i]["unknown name plural"].asString()};
				const auto known_gfx{items[i]["gfx known index"].asInt()};
				const auto unknown_gfx{items[i]["gfx unknown index"].asInt()};
				const std::string group_size{items[i]["group size"].asString()};
				const auto level{std::invoke([&] {
					if (items[i].isMember("level"))
						return items[i]["level"].asUInt();
					else
						return 0u;
				})};
				const std::string hit_dice{items[i]["hit dice"].asString()};
				const auto category{std::invoke([&] {
					using enum Enums::Monsters::Category;
					if (items[i].isMember("category")) {
						if (items[i]["category"].asString().length() > 0) {
							auto category{magic_enum::enum_cast<
								Enums::Monsters::Category>(
								items[i]["category"].asString())};
							return category.value_or(HUMANOID);
						} else
							return HUMANOID;
					} else
						return HUMANOID;
				})};
				const auto mclass{std::invoke([&] {
					using enum Enums::Monsters::Class;
					if (category == Enums::Monsters::Category::HUMANOID) {
						auto mclass{
							magic_enum::enum_cast<Enums::Monsters::Class>(
								items[i]["category"].asString())};
						return mclass.value_or(NO_CLASS);
					} else
						return NO_CLASS;
				})};

				const auto ac{std::invoke([&] {
					if (items[i].isMember("ac"))
						return items[i]["ac"].asInt();
					else
						return 10;
				})};
				const std::string atks{items[i]["attacks"].asString()};
				const auto attacks{_parse_attacks(atks)};
				const auto specials{std::invoke([&] {
					if (items[i].isMember("special"))
						return items[i]["special"].asString();
					else
						return std::string{};
				})};
				const auto level_drain{_parse_level_drain(specials)};
				const auto regeneration(_parse_regen(specials));
				const auto breath_weapon{_parse_breath_weapons(specials)};
				const auto rewards_1{std::invoke([&] {
					if (items[i].isMember("reward 1"))
						return items[i]["reward 1"].asUInt();
					else
						return 0u;
				})};
				const auto rewards_2{std::invoke([&] {
					if (items[i].isMember("reward 2"))
						return items[i]["reward 2"].asUInt();
					else
						return 0u;
				})};
				const std::string res{std::invoke([&] {
					if (items[i].isMember("resistances"))
						return items[i]["resistances"].asString();
					else
						return std::string{};
				})};
				const auto resistances{_parse_resistances(res)};
				const std::string props{std::invoke([&] {
					if (items[i].isMember("properties"))
						return items[i]["properties"].asString();
					else
						return std::string{};
				})};
				const auto properties{_parse_properties(props)};
				const auto xp{std::invoke([&] {
					if (items[i].isMember("xp"))
						return items[i]["xp"].asUInt();
					else
						return 0u;
				})};
				const auto partner_type_id{std::invoke([&] {
					if (items[i].isMember("partner id"))
						return items[i]["partner id"].asUInt();
					else
						return 0u;
				})};
				const auto partner_chance{std::invoke([&] {
					if (items[i].isMember("partner chance"))
						return items[i]["partner chance"].asUInt();
					else
						return 0u;
				})};
				const auto mage_level{std::invoke([&] {
					if (items[i].isMember("mage level"))
						return items[i]["mage level"].asUInt();
					else
						return 0u;
				})};
				const auto priest_level{std::invoke([&] {
					if (items[i].isMember("priest level"))
						return items[i]["priest level"].asUInt();
					else
						return 0u;
				})};
				const auto spell_resistance{std::invoke([&] {
					if (items[i].isMember("spell resistance"))
						return items[i]["spell resistance"].asUInt();
					else
						return 0u;
				})};
				const std::string traits{items[i]["traits"].asString()};
				const std::string weaknesses{items[i]["weaknesses"].asString()};

				MonsterType monster_type{};
				monster_type.set_type_id(id.value());
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
				for (auto attack : attacks)
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

				_items[id.value()] = monster_type;
			}

			return true;
		} else
			return false;
	}

	else
		return false;
}

auto Sorcery::MonsterStore::get(Enums::Monsters::TypeID monster_type_id) const
	-> MonsterType {

	return _items.at(monster_type_id);
}

auto Sorcery::MonsterStore::get(int monster_type_id) const -> MonsterType {

	return _items.at(
		magic_enum::enum_cast<Enums::Monsters::TypeID>(monster_type_id)
			.value());
}

auto Sorcery::MonsterStore::get_all_types() const -> std::vector<MonsterType> {

	std::vector<MonsterType> items;
	for (const auto &[key, value] : _items)
		items.push_back(value);

	return items;
}

auto Sorcery::MonsterStore::_parse_attacks(const std::string value) const
	-> std::vector<Dice> {

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

auto Sorcery::MonsterStore::_parse_breath_weapons(const std::string value) const
	-> Enums::Monsters::Breath {

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

auto Sorcery::MonsterStore::_parse_level_drain(const std::string value) const
	-> unsigned int {

	if (value.contains("Level Drain")) {
		std::string output{std::regex_replace(
			value, std::regex("[^0-9]*([0-9]+).*"), std::string("$1"))};
		return std::stoi(output);
	} else
		return 0;
}

auto Sorcery::MonsterStore::_parse_regen(const std::string value) const
	-> unsigned int {

	auto pos{value.find("Heal")};
	if (pos != std::string::npos) {
		auto heal_portion{value.substr(pos)};
		std::string output{std::regex_replace(
			heal_portion, std::regex("[^0-9]*([0-9]+).*"), std::string("$1"))};
		return std::stoi(output);
	} else
		return 0;
}

auto Sorcery::MonsterStore::_parse_resistances(const std::string value) const
	-> std::array<bool, 7> {

	using enum Enums::Monsters::Resistance;
	std::array<bool, 7> res;
	if (value.contains("Cold"))
		res[unenum(RESIST_COLD)] = true;
	if (value.contains("Drain"))
		res[unenum(RESIST_LEVEL_DRAIN)] = true;
	if (value.contains("Fire"))
		res[unenum(RESIST_FIRE)] = true;
	if (value.contains("Magic"))
		res[unenum(RESIST_MAGIC)] = true;
	if (value.contains("Poison"))
		res[unenum(RESIST_POISON)] = true;
	if (value.contains("Physical"))
		res[unenum(RESIST_PHYSICAL)] = true;
	if (value.contains("Stone"))
		res[unenum(RESIST_STONING)] = true;

	return res;
}

auto Sorcery::MonsterStore::_parse_properties(const std::string value) const
	-> std::array<bool, 7> {

	using enum Enums::Monsters::Property;
	std::array<bool, 7> props;
	if (value.contains("Critical"))
		props[unenum(CAN_AUTOKILL)] = true;
	if (value.contains("Sleep"))
		props[unenum(CAN_BE_SLEPT)] = true;
	if (value.contains("Call"))
		props[unenum(CAN_CALL_FOR_OTHERS)] = true;
	if (value.contains("Run"))
		props[unenum(CAN_FLEE)] = true;
	if (value.contains("Paralyse"))
		props[unenum(CAN_PARALYSE)] = true;
	if (value.contains("Stone"))
		props[unenum(CAN_PETRIFY)] = true;
	if (value.contains("Poison"))
		props[unenum(CAN_POISON)] = true;

	return props;
}