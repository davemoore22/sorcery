// Copyright (C) 2025 Dave Moore
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

#include "common/include.hpp"
#include "types/enum.hpp"
#include "types/item.hpp"
#include "types/itemtype.hpp"

namespace Sorcery {

class System;

class ItemStore {

	public:
		ItemStore(System *system, const std::filesystem::path filename);
		ItemStore() = delete;

		auto operator[](Enums::Items::TypeID item_type_id) const -> ItemType;
		auto operator[](unsigned int item_type_id) const -> ItemType;
		auto operator()(Enums::Items::Category category) const
			-> std::vector<ItemType>;
		auto operator[](std::string name) const -> ItemType;

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
		System *_system;
		std::map<Enums::Items::TypeID, ItemType> _items;
		bool _loaded;

		auto _load(const std::filesystem::path filename) -> bool;
		auto _get_defensive_effects(const std::string defensive_s) const
			-> std::array<bool, 22>;
		auto _get_offensive_effects(const std::string offsensive_s) const
			-> std::array<bool, 15>;
};

}
