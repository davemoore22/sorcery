
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

// Standard Constructor
Sorcery::MonsterStore::MonsterStore(System *system, const std::filesystem::path filename) : _system{system} {

	_items.clear();

	// Load the Item Definitions
	_loaded = _load(filename);
}

auto Sorcery::MonsterStore::_load(const std::filesystem::path filename) -> bool {

	using enum Enums::Character::Align;
	using enum Enums::Character::Class;
	using enum Enums::Items::Category;
	using enum Enums::Items::Effects::Invoke;
	using enum Enums::Items::TypeID;
	using enum Enums::Magic::SpellID;

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
				const std::string traits{items[i]["traits"].asString()};
				const std::string weaknesses{items[i]["weaknesses"].asString()};
				const std::string group_size{items[i]["group size"].asString()};
				const auto level{[&] {
					if (items[i].isMember("level"))
						return items[i]["level"].asUInt();
					else
						return 0u;
				}()};
				const std::string hit_dice{items[i]["hit dice"].asString()};
				const auto category{[&] {
					if (items[i].isMember("category")) {
						if (items[i]["category"].asString().length() > 0) {
							auto category{magic_enum::enum_cast<MonsterCategory>(items[i]["category"].asString())};
							return category.value_or(MonsterCategory::HUMANOID);
						} else
							return MonsterCategory::HUMANOID;
					} else
						return MonsterCategory::HUMANOID;
				}()};

				MonsterType monster_type{};
				monster_type.set_type_id(id.value());
				monster_type.set_known_name(known_name);
				monster_type.set_unknown_name(unknown_name);
				monster_type.set_known_name_plural(known_name_plural);
				monster_type.set_unknown_name_plural(unknown_name_plural);
				monster_type.set_known_gfx(known_gfx);
				monster_type.set_traits(traits);
				monster_type.set_weaknesses(weaknesses);
				monster_type.set_group_size(group_size);
				monster_type.set_level(level);
				monster_type.set_hit_dice(hit_dice);

				_items[id.value()] = monster_type;
			}

			return true;
		} else
			return false;
	}

	else
		return false;
}

auto Sorcery::MonsterStore::operator[](MonsterTypeID monster_type_id) const -> MonsterType {

	return _items.at(monster_type_id);
}

auto Sorcery::MonsterStore::get_all_types() const -> std::vector<MonsterType> {

	std::vector<MonsterType> items;
	for (const auto &[key, value] : _items)
		items.push_back(value);

	return items;
}
