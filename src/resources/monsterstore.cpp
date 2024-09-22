
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

#include "resources/monsterstore.hpp"
#include "common/enum.hpp"
#include "common/include.hpp"
#include "common/macro.hpp"
#include "core/system.hpp"

// Standard Constructor
Sorcery::MonsterStore::MonsterStore(System *system, const std::filesystem::path filename) : _system{system} {

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
		Json::StreamWriterBuilder builder{};
		builder.settings_["indentation"] = "";
		if (Json::Value data; reader.parse(file, data)) {
			Json::Value &items{data["monster"]};

			// Iterate through item file one itemtype at a time
			for (auto i = 0u; i < items.size(); i++) {

				// Some fields are always present
				const auto id{magic_enum::enum_cast<Enums::Monsters::TypeID>(items[i]["id"].asInt())};
				const std::string known_name{items[i]["known name"].asString()};
				const std::string unknown_name{items[i]["unknown name"].asString()};
				const std::string known_name_plural{items[i]["known name plural"].asString()};
				const std::string unknown_name_plural{items[i]["unknown name plural"].asString()};
				const auto known_gfx{items[i]["gfx known index"].asInt()};
				const auto unknown_gfx{items[i]["gfx known index"].asInt()};
				const std::string group_size{items[i]["group size"].asString()};
				const auto level{std::invoke([&] {
					if (items[i].isMember("level"))
						return items[i]["level"].asUInt();
					else
						return 0u;
				})};
				const std::string hit_dice{items[i]["hit dice"].asString()};
				const auto category{std::invoke([&] {
					if (items[i].isMember("category")) {
						if (items[i]["category"].asString().length() > 0) {
							auto category{magic_enum::enum_cast<MCT>(items[i]["category"].asString())};
							return category.value_or(MCT::HUMANOID);
						} else
							return MCT::HUMANOID;
					} else
						return MCT::HUMANOID;
				})};
				const auto mclass{std::invoke([&] {
					if (category == MCT::HUMANOID) {
						auto mclass{magic_enum::enum_cast<MCL>(items[i]["category"].asString())};
						return mclass.value_or(MCL::NO_CLASS);
					} else
						return MCL::NO_CLASS;
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

auto Sorcery::MonsterStore::operator[](MTI monster_type_id) const -> MonsterType {

	return _items.at(monster_type_id);
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
	split.erase(
		std::remove_if(split.begin(), split.end(), [](std::string_view s) { return s.size() == 0; }), split.end());

	for (const auto &each : split) {

		Dice atk{each};
		attacks.emplace_back(atk);
	}

	return attacks;
}

auto Sorcery::MonsterStore::_parse_breath_weapons(const std::string value) const -> MBR {

	if (value.find("Drain Breath") != std::string::npos)
		return MBR::DRAIN_BREATH;
	else if (value.find("Flame Breath") != std::string::npos)
		return MBR::FLAME_BREATH;
	else if (value.find("Poison Breath") != std::string::npos)
		return MBR::POISON_BREATH;
	else if (value.find("Cold Breath") != std::string::npos)
		return MBR::COLD_BREATH;
	else if (value.find("Stone Breath") != std::string::npos)
		return MBR::STONE_BREATH;
	else
		return MBR::NO_BREATH_WEAPON;
}

auto Sorcery::MonsterStore::_parse_level_drain(const std::string value) const -> unsigned int {

	if (value.find("Level Drain") != std::string::npos) {
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

auto Sorcery::MonsterStore::_parse_resistances(const std::string value) const -> MonsterResistances {

	MonsterResistances res;
	if (value.find("Cold") != std::string::npos)
		res[unenum(MRE::RESIST_COLD)] = true;
	if (value.find("Drain") != std::string::npos)
		res[unenum(MRE::RESIST_LEVEL_DRAIN)] = true;
	if (value.find("Fire") != std::string::npos)
		res[unenum(MRE::RESIST_FIRE)] = true;
	if (value.find("Magic") != std::string::npos)
		res[unenum(MRE::RESIST_MAGIC)] = true;
	if (value.find("Poison") != std::string::npos)
		res[unenum(MRE::RESIST_POISON)] = true;
	if (value.find("Physical") != std::string::npos)
		res[unenum(MRE::RESIST_PHYSICAL)] = true;
	if (value.find("Stone") != std::string::npos)
		res[unenum(MRE::RESIST_STONING)] = true;

	return res;
}

auto Sorcery::MonsterStore::_parse_properties(const std::string value) const -> MonsterProperties {

	MonsterProperties props;
	if (value.find("Critical") != std::string::npos)
		props[unenum(MPR::CAN_AUTOKILL)] = true;
	if (value.find("Sleep") != std::string::npos)
		props[unenum(MPR::CAN_BE_SLEPT)] = true;
	if (value.find("Call") != std::string::npos)
		props[unenum(MPR::CAN_CALL_FOR_OTHERS)] = true;
	if (value.find("Run") != std::string::npos)
		props[unenum(MPR::CAN_FLEE)] = true;
	if (value.find("Paralyse") != std::string::npos)
		props[unenum(MPR::CAN_PARALYSE)] = true;
	if (value.find("Stone") != std::string::npos)
		props[unenum(MPR::CAN_PETRIFY)] = true;
	if (value.find("Poison") != std::string::npos)
		props[unenum(MPR::CAN_POISON)] = true;

	return props;
}
