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

#pragma once

#include "core/system.hpp"
#include "main.hpp"
#include "types/item.hpp"
#include "types/itemtype.hpp"

namespace Sorcery {

class ItemStore {

	public:

		ItemStore(System *system, const std::filesystem::path filename);
		ItemStore() = delete;

		// Overload operators
		auto operator[](ItemTypeID item_type_id) const -> ItemType;
		auto operator()(ItemCategory category) const -> std::vector<ItemType>;
		auto operator[](std::string name) const -> ItemType;

		// Public methods
		auto get_an_item(const ItemTypeID item_type_id) const -> Item;
		auto get_random_item(const ItemTypeID min_item_type_id, const ItemTypeID max_item_type_id) const -> Item;
		auto get_all_types() const -> std::vector<ItemType>;
		auto is_usable(
			const ItemTypeID item_type_id, const CharacterClass cclass, const CharacterAlignment calign) const -> bool;

	private:

		// Private members
		System *_system;
		std::map<ItemTypeID, ItemType> _items;
		bool _loaded;

		// Private methods
		auto _load(const std::filesystem::path filename) -> bool;
		auto _get_defensive_effects(const std::string defensive_s) const -> ItemEffDef;
		auto _get_offensive_effects(const std::string offsensive_s) const -> ItemEffOff;
};

}
