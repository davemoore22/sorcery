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

#pragma once

#include "common/enum.hpp"		   // for Class, SpellID, SpellID::NO_SPELL
#include "core/context.hpp"		   // for Context
#include "core/random.hpp"		   // for Random
#include "resources/itemstore.hpp" // for ItemStore
#include "types/enum.hpp"		   // for TypeID (ptr only), Category, Cate...
#include "types/item/item.hpp"	   // for Item
#include "types/item/itemtype.hpp" // for ItemType
#include "types/meta.hpp"		   // for enum_cast
#include <algorithm>			   // for find_if, remove_if
#include <array>				   // for array
#include <filesystem>			   // for path
#include <fstream>				   // for basic_ifstream, char_traits, ifst...
#include <functional>			   // for invoke
#include <jsoncpp/json/reader.h>   // for Reader
#include <jsoncpp/json/value.h>	   // for Value
#include <jsoncpp/json/writer.h>   // for StreamWriterBuilder
#include <map>					   // for map, operator==
#include <optional>				   // for optional
#include <ranges>				   // for __find_if_fn
#include <regex>				   // for regex, regex_token_iterator, sreg...
#include <stdexcept>			   // for out_of_range
#include <string>				   // for basic_string, string, operator==
#include <string_view>			   // for string_view, operator==
#include <utility>				   // for to_underlying, get, pair
#include <vector>				   // for vector

namespace Sorcery {

class ItemStore {

	public:
		ItemStore(Context &ctx, const std::filesystem::path filename);
		ItemStore() = delete;

		auto get(Enums::Items::TypeID item_type_id) const -> ItemType;
		auto get(unsigned int item_type_id) const -> ItemType;
		auto get(Enums::Items::Category category) const
			-> std::vector<ItemType>;
		auto get(std::string_view name) const -> ItemType;

		auto get_item_type(const Enums::Items::TypeID item_type_id) const
			-> ItemType;
		auto get_an_item(const Enums::Items::TypeID item_type_id) const -> Item;
		auto get_random_item(const Enums::Items::TypeID min_item_type_id,
							 const Enums::Items::TypeID max_item_type_id) const
			-> Item;
		auto get_all_types() const -> std::vector<ItemType>;
		auto is_usable(const Enums::Items::TypeID item_type_id,
					   const Enums::Character::Class cclass,
					   const Enums::Character::Align calign) const -> bool;
		auto has_usable(const Enums::Items::TypeID item_type_id) const -> bool;
		auto has_invokable(const Enums::Items::TypeID item_type_id) const
			-> bool;
		auto sellable_to_shop(const Enums::Items::TypeID item_type_id) const
			-> bool;
		auto sellable_price(const Enums::Items::TypeID item_type_id) const
			-> unsigned int;

	private:
		Context &_ctx;
		std::map<Enums::Items::TypeID, ItemType> _items;
		bool _loaded;

		auto _load(const std::filesystem::path filename) -> bool;
		auto _get_defensive_effects(const std::string defensive_s) const
			-> std::array<bool, 22>;
		auto _get_offensive_effects(const std::string offsensive_s) const
			-> std::array<bool, 15>;
};

}
