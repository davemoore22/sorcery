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

#pragma once

#include "itemtype.hpp"
#include "main.hpp"
#include "system.hpp"

namespace Sorcery {

class Item {

	public:

		// Constructors
		Item();
		Item(const ItemTypeID item_type);
		Item(const ItemType &item_type);

		// Overloaded Operators
		auto friend operator<<(std::ostream &out_stream, const Item &Item) -> std::ostream &;

		// Serialisation
		template <class Archive> auto serialize(Archive &archive) -> void {
			archive(_type, _known, _equipped, _cursed, _marked, _name, _id, s_id);
		}

		// Public Methods
		auto get_type_id() const -> ItemTypeID;
		auto get_known() const -> bool;
		auto get_equipped() const -> bool;
		auto get_cursed() const -> bool;
		auto get_marked() const -> bool;
		auto get_name() const -> std::string;
		auto set_known(const bool value) -> void;
		auto set_equipped(const bool value) -> void;
		auto set_cursed(const bool value) -> void;
		auto set_marked(const bool value) -> void;
		auto set_name(const std::string &value) -> void;
		auto decay_to(const ItemTypeID value) -> void;

	private:

		// Private Members
		ItemTypeID _type;  // Type
		bool _known;	   // Is known
		bool _equipped;	   // Is currently equipped
		bool _cursed;	   // Is currently cursed
		bool _marked;	   // Marked as undroppable or unsellable
		std::string _name; // Individual Name (or if not set, the ItemTypeID Name)

		long _id;
		static inline long s_id{0};
};

}
