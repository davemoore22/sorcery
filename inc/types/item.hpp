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

namespace Sorcery {

class ItemType;

class Item {

	public:
		// Constructors
		Item();
		Item(const ItemType &item_type);
		Item(const ItemType &item_type, const bool usable);

		// Copy Constructor
		Item(const Item &item);

		// Overloaded Operators
		auto friend operator<<(std::ostream &out_stream, const Item &item)
			-> std::ostream &;

		// Serialisation
		template <class Archive> auto serialize(Archive &archive) -> void {
			archive(_type, _category, _known, _equipped, _cursed, _marked,
					_usable, _name, _id, s_id);
		}

		// Public Methods
		auto get_type_id() const -> Enums::Items::TypeID;
		auto get_category() const -> Enums::Items::Category;
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
		auto decay_to(const Enums::Items::TypeID value) -> void;
		auto set_usable(const bool value) -> void;
		auto get_usable() const -> bool;
		auto get_display_name() const -> std::string;

	private:
		Enums::Items::TypeID _type;		  // Type
		Enums::Items::Category _category; // Category
		bool _known;					  // Is known
		bool _equipped;					  // Is currently equipped
		bool _cursed;					  // Is currently cursed
		bool _marked;					  // Marked as undroppable or unsellable
		bool _usable;					  // Is usable
		std::string _name; // Individual Name (or if not set, the ITT Name)
		std::string _uname;

		long _id;
		static inline long s_id{0};
};

}
