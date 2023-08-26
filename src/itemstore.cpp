
// Copyright (C) 2023 Dave Moore
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

#include "itemstore.hpp"

// Standard Constructor
Sorcery::ItemStore::ItemStore(System *system, const std::filesystem::path filename) : _system{system} {

	_items.clear();

	// Load the Item Definitions
	_loaded = _load(filename);
}

auto Sorcery::ItemStore::_load(const std::filesystem::path filename) -> bool {

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
			Json::Value &items{data["item"]};

			// Iterate through item file one itemtype at a time
			for (auto i = 0u; i < items.size(); i++) {

				// Some fields are always present
				const auto id{magic_enum::enum_cast<Enums::Items::TypeID>(items[i]["id"].asInt())};
				const auto category{[&] {
					if (items[i].isMember("category")) {
						if (items[i]["category"].asString().length() > 0) {
							auto category{magic_enum::enum_cast<ItemCategory>(items[i]["category"].asString())};
							return category.value_or(NO_ITEM_CATEGORY);
						} else
							return NO_ITEM_CATEGORY;
					} else
						return NO_ITEM_CATEGORY;
				}()};
				const std::string known_name(items[i]["known name"].asString());
				const std::string unknown_name(items[i]["unknown name"].asString());
				const std::string display_name(items[i]["display name"].asString());
				const auto value{static_cast<unsigned int>(std::stoul(items[i]["value"].asString()))};
				const std::string allowed_classes_s{[&] {
					if (items[i].isMember("allowed classes")) {
						return items[i]["allowed classes"].asString().length() > 0
								   ? items[i]["allowed classes"].asString()
								   : "";
					} else
						return std::string{};
				}()};
				const std::string allowed_alignments_s{[&] {
					if (items[i].isMember("allowed alignments"))
						return items[i]["allowed alignments"].asString().length() > 0
								   ? items[i]["allowed alignments"].asString()
								   : "";
					else
						return std::string{};
				}()};
				const auto to_hit{[&] {
					if (items[i].isMember("to hit"))
						return items[i]["to hit"].asInt();
					else
						return 0;
				}()};
				const std::string damage_s{[&] {
					if (items[i].isMember("damage"))
						return items[i]["damage"].asString().length() > 0 ? items[i]["damage"].asString() : "";
					else
						return std::string{};
				}()};
				const auto attacks{[&] {
					if (items[i].isMember("attacks"))
						return items[i]["attacks"].asUInt();
					else
						return 0u;
				}()};
				const auto ac{[&] {
					if (items[i].isMember("ac"))
						return items[i]["ac"].asInt();
					else
						return 0;
				}()};
				const auto use_spell{[&] {
					if (items[i].isMember("use")) {
						if (items[i]["use"].asString().length() > 0) {
							auto use{magic_enum::enum_cast<SpellID>(items[i]["use"].asString())};
							return use.value_or(NO_SPELL);
						} else
							return NO_SPELL;
					} else
						return NO_SPELL;
				}()};
				const auto use_decay{[&] {
					if (items[i].isMember("use decay"))
						return items[i]["use decay"].asUInt();
					else
						return 0u;
				}()};
				const std::string offensive_s{[&] {
					if (items[i].isMember("offensive"))
						return items[i]["offensive"].asString().length() > 0 ? items[i]["offensive"].asString() : "";
					else
						return std::string{};
				}()};
				const std::string defensive_s{[&] {
					if (items[i].isMember("defensive"))
						return items[i]["defensive"].asString().length() > 0 ? items[i]["defensive"].asString() : "";
					else
						return std::string{};
				}()};
				const auto invoke_effect{[&] {
					if (items[i].isMember("invoke")) {
						if (items[i]["invoke"].asString().length() > 0) {
							auto invoke{
								magic_enum::enum_cast<Enums::Items::Effects::Invoke>(items[i]["invoke"].asString())};
							return invoke.value_or(NO_INV_EFFECT);
						} else
							return NO_INV_EFFECT;
					} else
						return NO_INV_EFFECT;
				}()};
				const auto invoke_decay{[&] {
					if (items[i].isMember("invoke decay"))
						return items[i]["invoke decay"].asUInt();
					else
						return 0u;
				}()};
				const auto cursed{[&] {
					if (items[i].isMember("cursed"))
						return items[i]["cursed"].asString() == "Cursed";
					else
						return false;
				}()};

				// Now do extra processing
				ItemUsableClass item_usable{};
				item_usable.fill(false);
				if (allowed_classes_s.find('f') != std::string::npos)
					item_usable[unenum(FIGHTER)] = true;
				if (allowed_classes_s.find('m') != std::string::npos)
					item_usable[unenum(MAGE)] = true;
				if (allowed_classes_s.find('p') != std::string::npos)
					item_usable[unenum(PRIEST)] = true;
				if (allowed_classes_s.find('t') != std::string::npos)
					item_usable[unenum(THIEF)] = true;
				if (allowed_classes_s.find('b') != std::string::npos)
					item_usable[unenum(BISHOP)] = true;
				if (allowed_classes_s.find('s') != std::string::npos)
					item_usable[unenum(SAMURAI)] = true;
				if (allowed_classes_s.find('l') != std::string::npos)
					item_usable[unenum(LORD)] = true;
				if (allowed_classes_s.find('n') != std::string::npos)
					item_usable[unenum(NINJA)] = true;

				ItemUsableAlignment item_alignment{};
				item_alignment.fill(false);
				if (allowed_alignments_s.find('g') != std::string::npos)
					item_alignment[unenum(GOOD)] = true;
				if (allowed_alignments_s.find('n') != std::string::npos)
					item_alignment[unenum(NEUTRAL)] = true;
				if (allowed_alignments_s.find('e') != std::string::npos)
					item_alignment[unenum(EVIL)] = true;

				ItemType item_type{};
				item_type.set_type_id(id.value());
				item_type.set_category(category);
				item_type.set_known_name(known_name);
				item_type.set_unknown_name(unknown_name);
				item_type.set_display_name(display_name);
				item_type.set_value(value);
				item_type.set_usable_class(item_usable);
				item_type.set_usable_alignment(item_alignment);
				item_type.set_to_hit_mod(to_hit);
				item_type.set_damage(damage_s);
				item_type.set_discovered(false);

				// Parse Damage Dice String
				if ((!damage_s.empty()) && (category == ItemCategory::WEAPON)) {

					// Regex for Dice Parsing by "rikkles" on the Grid Cartographer Discord
					const std::regex regex(R"(^(\d+)d(\d+)[+-]?(\d*)$)");
					if (std::smatch match; std::regex_search(damage_s, match, regex)) {
						auto num{std::stoi(match[1])};
						auto type{std::stoi(match[2])};
						auto mod{std::stoi(match[3])};
						if (damage_s.find('-') != std::string::npos)
							mod = -mod;
						item_type.set_damage_dice(num, type, mod);
					} else
						item_type.set_damage_dice(0, 0, 0);
				} else
					item_type.set_damage_dice(0, 0, 0);
				item_type.set_swings(attacks);
				item_type.set_ac_mod(ac);
				item_type.set_eff_use(use_spell);
				item_type.set_eff_use_decay(use_decay);
				item_type.set_eff_def(_get_defensive_effects(defensive_s));
				item_type.set_eff_off(_get_offensive_effects(offensive_s));
				item_type.set_eff_inv(invoke_effect);
				item_type.set_eff_inv_decay(invoke_decay);
				item_type.set_cursed(cursed);

				_items[id.value()] = item_type;
			}

			return true;
		} else
			return false;
	}

	else
		return false;
}

auto Sorcery::ItemStore::operator[](ItemTypeID item_type_id) const -> ItemType {

	return _items.at(item_type_id);
}

auto Sorcery::ItemStore::operator[](std::string name) const -> ItemType {

	auto it{std::ranges::find_if(_items.begin(), _items.end(),
		[&](const auto &item_type) { return (item_type.second.get_display_name() == name); })};
	return (*it).second;
}

auto Sorcery::ItemStore::operator()(const ItemCategory category) const -> std::vector<ItemType> {

	std::vector<ItemType> items;
	for (const auto &[key, value] : _items)
		if (value.get_category() == category)
			items.push_back(value);

	return items;
}

// Public methods
auto Sorcery::ItemStore::get_an_item(const ItemTypeID item_type_id) const -> Item {

	return Item{_items.at(item_type_id)};
}

auto Sorcery::ItemStore::is_usable(
	const ItemTypeID item_type_id, const CharacterClass cclass, const CharacterAlignment calign) const -> bool {

	return _items.at(item_type_id).is_class_usable(cclass) && _items.at(item_type_id).is_align_usable(calign);
}

auto Sorcery::ItemStore::get_random_item(const ItemTypeID min_item_type_id, const ItemTypeID max_item_type_id) const
	-> Item {

	// TODO: probably needs some bounds checking on this
	auto item_type_id{_system->random->get(unenum(min_item_type_id), unenum(max_item_type_id))};

	return Item{_items.at(magic_enum::enum_cast<ItemTypeID>(item_type_id).value())};
}

auto Sorcery::ItemStore::get_all_types() const -> std::vector<ItemType> {

	std::vector<ItemType> items;
	for (const auto &[key, value] : _items)
		items.push_back(value);

	return items;
}

auto Sorcery::ItemStore::_get_defensive_effects(const std::string defensive_s) const -> ItemEffDef {

	ItemEffDef effects{};
	effects.fill(false);

	if (defensive_s.length() > 0) {
		std::regex regex{R"([,]+)"};
		std::sregex_token_iterator it{defensive_s.begin(), defensive_s.end(), regex, -1};
		std::vector<std::string> split{it, {}};
		split.erase(
			std::remove_if(split.begin(), split.end(), [](std::string_view s) { return s.size() == 0; }), split.end());

		for (const auto &term : split) {
			if (term == "RESIST_ALL") {
				for (auto i = unenum(ItemDef::RESIST_COLD); i <= unenum(ItemDef::PREVENT_DECAPITATION); i++)
					effects[i] = true;
			}
			if (term == "PROTECT_VS_ALL") {
				for (auto i = unenum(ItemDef::PROTECTION_VS_ANIMAL); i <= unenum(ItemDef::PROTECTION_VS_WERE); i++)
					effects[i] = true;
			};
			auto def{magic_enum::enum_cast<ItemDef>(term)};
			if (def.has_value())
				effects[unenum(def.value())] = term.starts_with('!') ? false : true;
		}
	}

	return effects;
}

auto Sorcery::ItemStore::_get_offensive_effects(const std::string offsensive_s) const -> ItemEffOff {

	ItemEffOff effects{};
	effects.fill(false);

	if (offsensive_s.length() > 0) {
		std::regex regex{R"([,]+)"};
		std::sregex_token_iterator it{offsensive_s.begin(), offsensive_s.end(), regex, -1};
		std::vector<std::string> split{it, {}};
		split.erase(
			std::remove_if(split.begin(), split.end(), [](std::string_view s) { return s.size() == 0; }), split.end());

		for (const auto &term : split) {
			auto off{magic_enum::enum_cast<ItemOff>(term)};
			if (off.has_value())
				effects[unenum(off.value())] = term.starts_with('!') ? false : true;
		}
	}
	return effects;
}
