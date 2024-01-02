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

// clang-format off
#include "main.hpp"
#include "types/item.hpp"
#include "types/itemtype.hpp"

// clang-format on

namespace Sorcery {

class Inventory {

	public:

		// Constructors
		Inventory();

		// Destructor
		~Inventory() = default;

		// Overloaded Operators
		auto operator[](const unsigned int slot) -> std::optional<Item *>;
		auto friend operator<<(std::ostream &out_stream, const Inventory &inventory) -> std::ostream &;

		// Serialisation
		template <class Archive> auto serialize(Archive &archive) -> void {
			archive(_items);
		}

		// Public Methods
		auto clear() -> void;
		auto size() const -> unsigned int;
		auto is_full() const -> bool;
		auto is_empty() const -> bool;
		auto add_type(ItemType item_type) -> bool;
		auto add_type(ItemType item_type, const bool known) -> bool;
		auto unequip_all() -> void;
		auto items() const -> std::vector<Item>;
		auto has_unidentified_items() const -> bool;
		auto has_cursed_items() const -> bool;

	private:

		// Private Methods

		// Private Members
		std::vector<Item> _items;
};

}
