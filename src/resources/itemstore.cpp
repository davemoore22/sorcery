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

#include "resources/itemstore.hpp"
#include "common/enum.hpp"		   // for Class, Align
#include "core/context.hpp"		   // for Context
#include "core/random.hpp"		   // for Random
#include "magic/enum.hpp"		   // for SpellID, SpellID::NO_SPELL
#include "resources/json.hpp"	   // for get_string, get_int, get_enum
#include "types/enum.hpp"		   // for TypeID (ptr only), Defensive, Cat...
#include "types/item/item.hpp"	   // for Item
#include "types/item/itemtype.hpp" // for ItemType
#include "types/meta.hpp"		   // for enum_cast
#include <algorithm>			   // for find_if, remove_if
#include <array>				   // for array
#include <cstddef>				   // for size_t
#include <filesystem>			   // for path
#include <fstream>				   // for basic_ifstream, ifstream
#include <json/reader.h>		   // for Reader
#include <json/value.h>			   // for Value
#include <json/writer.h>		   // for StreamWriterBuilder
#include <map>					   // for map, operator==
#include <optional>				   // for optional
#include <ranges>				   // for __find_if_fn
#include <regex>				   // for regex, regex_token_iterator, sreg...
#include <stdexcept>			   // for out_of_range
#include <string>				   // for basic_string, string, char_traits
#include <string_view>			   // for string_view, operator==
#include <utility>				   // for pair, to_underlying, get
#include <vector>				   // for vector

template <typename E, std::size_t N, std::size_t M>
auto parse_allowed(const std::string_view allowed, const std::array<std::pair<char, E>, M> &mapping)
	-> std::array<bool, N> {

	std::array<bool, N> result{};

	for (const auto &[code, value] : mapping)
		result[std::to_underlying(value)] = allowed.contains(code);

	return result;
}

constexpr std::array CLASS_CODES{
	std::pair{'f', Sorcery::Enums::Character::Class::FIGHTER},
	std::pair{'m', Sorcery::Enums::Character::Class::MAGE},
	std::pair{'p', Sorcery::Enums::Character::Class::PRIEST},
	std::pair{'t', Sorcery::Enums::Character::Class::THIEF},
	std::pair{'b', Sorcery::Enums::Character::Class::BISHOP},
	std::pair{'s', Sorcery::Enums::Character::Class::SAMURAI},
	std::pair{'l', Sorcery::Enums::Character::Class::LORD},
	std::pair{'n', Sorcery::Enums::Character::Class::NINJA},
};

constexpr std::array ALIGNMENT_CODES{
	std::pair{'g', Sorcery::Enums::Character::Align::GOOD},
	std::pair{'n', Sorcery::Enums::Character::Align::NEUTRAL},
	std::pair{'e', Sorcery::Enums::Character::Align::EVIL},
};

// Standard Constructor
Sorcery::ItemStore::ItemStore(Context &ctx, const std::filesystem::path filename)
	: _ctx{ctx} {

	_items.clear();

	// Load the Item Definitions
	_loaded = _load(filename);
}

auto Sorcery::ItemStore::_load(const std::filesystem::path filename) -> bool {

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

				using enum Enums::Items::Category;
				using enum Enums::Magic::SpellID;
				using enum Enums::Items::Effects::Invoke;

				auto item{items[i]};
				const auto type_id{JsonHelper::get_required_enum<Enums::Items::TypeID>(item, "id")};
				const auto category{JsonHelper::get_enum(item, "category", NO_ITEM_CATEGORY)};
				const auto known_name{JsonHelper::get_string(item, "known name")};
				const auto unknown_name{JsonHelper::get_string(item, "unknown name")};
				const auto display_name{JsonHelper::get_string(item, "display name")};
				const auto value{JsonHelper::get_uint64(item, "value")};
				const auto allowed_classes{JsonHelper::get_string(item, "allowed classes")};
				const auto allowed_alignments{JsonHelper::get_string(item, "allowed alignments")};
				const auto to_hit{JsonHelper::get_int(item, "to hit")};
				const auto damage{JsonHelper::get_string(item, "damage")};
				const auto attacks{JsonHelper::get_uint(item, "attacks")};
				const auto ac{JsonHelper::get_int(item, "ac")};
				const auto regeneration{JsonHelper::get_int(item, "regeneration")};
				const auto use_spell{JsonHelper::get_enum(item, "use", NO_SPELL)};
				const auto use_decay{JsonHelper::get_uint(item, "use decay")};
				const auto offensive{JsonHelper::get_string(item, "offensive")};
				const auto defensive{JsonHelper::get_string(item, "defensive")};
				const auto invoke_effect{JsonHelper::get_enum(item, "invoke", NO_INV_EFFECT)};
				const auto invoke_decay{JsonHelper::get_uint(item, "invoke decay")};
				const auto cursed{JsonHelper::get_flag(item, "cursed", "Cursed")};
				const auto shop_stock{JsonHelper::get_int(item, "shop stock")};
				const auto shop_will_buy{JsonHelper::get_flag(item, "shop_will_buy", "yes")};
				const auto shop_will_sell{JsonHelper::get_flag(item, "shop_will_sell", "yes")};
				const auto effects{JsonHelper::get_string(item, "effects")};
				const auto usage{JsonHelper::get_string(item, "usage")};
				const auto invokage{JsonHelper::get_string(item, "invokage")};

				const auto item_usable{parse_allowed<Enums::Character::Class, 9>(allowed_classes, CLASS_CODES)};
				const auto item_alignment{
					parse_allowed<Enums::Character::Align, 4>(allowed_alignments, ALIGNMENT_CODES)};

				ItemType item_type{};
				item_type.set_type_id(type_id);
				item_type.set_category(category);
				item_type.set_known_name(known_name);
				item_type.set_unknown_name(unknown_name);
				item_type.set_display_name(display_name);
				item_type.set_value(value);
				item_type.set_usable_class(item_usable);
				item_type.set_usable_alignment(item_alignment);
				item_type.set_to_hit_mod(to_hit);
				item_type.set_damage(damage);
				item_type.set_discovered(false);
				item_type.set_shop_inital_stock(shop_stock);
				item_type.set_effects(effects);
				item_type.set_invokage(invokage);
				item_type.set_usage(usage);

				// Parse Damage Dice String
				if (!damage.empty() && category == Enums::Items::Category::WEAPON)
					item_type.set_damage_dice(damage);
				else
					item_type.set_damage_dice("");
				item_type.set_swings(attacks);
				item_type.set_ac_mod(ac);
				item_type.set_regen(regeneration);
				item_type.set_eff_use(use_spell);
				item_type.set_eff_use_decay(use_decay);
				item_type.set_eff_def(_get_defensive_effects(defensive));
				item_type.set_eff_off(_get_offensive_effects(offensive));
				item_type.set_eff_inv(invoke_effect);
				item_type.set_eff_inv_decay(invoke_decay);
				item_type.set_cursed(cursed);
				item_type.set_can_be_bought(shop_will_sell);
				item_type.set_can_be_sold(shop_will_buy);
				_items[type_id] = item_type;
			}

			return true;
		} else
			return false;
	}

	else
		return false;
}

auto Sorcery::ItemStore::get(Enums::Items::TypeID item_type_id) const -> ItemType {

	return _items.at(item_type_id);
}

auto Sorcery::ItemStore::get(unsigned int item_type_id) const -> ItemType {

	return _items.at(enum_cast<Enums::Items::TypeID>(item_type_id).value());
}

auto Sorcery::ItemStore::get_item_type(const Enums::Items::TypeID item_type_id) const -> ItemType {

	return _items.at(item_type_id);
}

auto Sorcery::ItemStore::get(std::string_view name) const -> ItemType {

	const auto it{std::ranges::find_if(_items, [name](const auto &item) {
		return item.second.get_display_name() == name;
	})};

	if (it == _items.end())
		throw std::out_of_range{"Item not found: " + std::string{name}};

	return it->second;
}

auto Sorcery::ItemStore::get(const Enums::Items::Category category) const -> std::vector<ItemType> {

	std::vector<ItemType> items;
	for (const auto &[key, value] : _items)
		if (value.get_category() == category)
			items.push_back(value);

	return items;
}

// Public methods
auto Sorcery::ItemStore::get_an_item(const Enums::Items::TypeID item_type_id) const -> Item {

	return Item{_items.at(item_type_id)};
}

auto Sorcery::ItemStore::is_usable(const Enums::Items::TypeID item_type_id, const Enums::Character::Class cclass,
								   const Enums::Character::Align calign) const -> bool {

	return _items.at(item_type_id).is_class_usable(cclass) && _items.at(item_type_id).is_align_usable(calign);
}

auto Sorcery::ItemStore::has_usable(const Enums::Items::TypeID item_type_id) const -> bool {

	return _items.at(item_type_id).has_usable();
}

auto Sorcery::ItemStore::sell_price(const Enums::Items::TypeID item_type_id) const -> std::uint64_t {

	return _items.at(item_type_id).get_value() / 2;
}

auto Sorcery::ItemStore::can_be_sold(const Enums::Items::TypeID item_type_id) const -> bool {

	return _items.at(item_type_id).get_can_be_sold();
}

auto Sorcery::ItemStore::has_invokable(const Enums::Items::TypeID item_type_id) const -> bool {

	return _items.at(item_type_id).has_invokable();
}

auto Sorcery::ItemStore::get_random_item(const Enums::Items::TypeID min_item_type_id,
										 const Enums::Items::TypeID max_item_type_id) const -> Item {

	auto item_type_id{_ctx.random->get(std::to_underlying(min_item_type_id), std::to_underlying(max_item_type_id))};

	return Item{_items.at(enum_cast<Enums::Items::TypeID>(item_type_id).value())};
}

auto Sorcery::ItemStore::get_all_types() const -> std::vector<ItemType> {

	std::vector<ItemType> items;
	for (const auto &[key, value] : _items)
		items.push_back(value);

	return items;
}

auto Sorcery::ItemStore::_get_defensive_effects(const std::string defensive_s) const -> std::array<bool, 22> {

	std::array<bool, 22> effects{};
	effects.fill(false);

	if (defensive_s.length() > 0) {
		std::regex regex{R"([,]+)"};
		std::sregex_token_iterator it{defensive_s.begin(), defensive_s.end(), regex, -1};
		std::vector<std::string> split{it, {}};
		split.erase(std::remove_if(split.begin(), split.end(),
								   [](std::string_view s) {
									   return s.size() == 0;
								   }),
					split.end());

		for (const auto &term : split) {
			using enum Enums::Items::Effects::Defensive;
			if (term == "RESIST_ALL") {
				for (auto i = std::to_underlying(RESIST_COLD); i <= std::to_underlying(PREVENT_DECAPITATION); i++)
					effects[i] = true;
			}
			if (term == "PROTECT_VS_ALL") {
				for (auto i = std::to_underlying(PROTECTION_VS_ANIMAL); i <= std::to_underlying(PROTECTION_VS_WERE);
					 i++)
					effects[i] = true;
			};
			auto def{enum_cast<Enums::Items::Effects::Defensive>(term)};
			if (def.has_value())
				effects[std::to_underlying(def.value())] = term.starts_with('!') ? false : true;
		}
	}

	return effects;
}

auto Sorcery::ItemStore::_get_offensive_effects(const std::string offsensive_s) const -> std::array<bool, 15> {

	std::array<bool, 15> effects{};
	effects.fill(false);

	if (offsensive_s.length() > 0) {
		std::regex regex{R"([,]+)"};
		std::sregex_token_iterator it{offsensive_s.begin(), offsensive_s.end(), regex, -1};
		std::vector<std::string> split{it, {}};
		split.erase(std::remove_if(split.begin(), split.end(),
								   [](std::string_view s) {
									   return s.size() == 0;
								   }),
					split.end());

		for (const auto &term : split) {
			auto off{enum_cast<Enums::Items::Effects::Offensive>(term)};
			if (off.has_value())
				effects[std::to_underlying(off.value())] = term.starts_with('!') ? false : true;
		}
	}
	return effects;
}
