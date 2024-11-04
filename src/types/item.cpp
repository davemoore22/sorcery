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

#include "types/item.hpp"
#include "common/macro.hpp"
#include "types/itemtype.hpp"

// Default Constructor
Sorcery::Item::Item() {

	_type = ITT::BROKEN_ITEM;
	_category = ITC::NO_ITEM_CATEGORY;
	_known = false;
	_equipped = false;
	_cursed = false;
	_marked = false;
	_usable = true;
	_name = "";
	_uname = "";

	_id = s_id++;
}

Sorcery::Item::Item(const ItemType &item_type, const bool usable) {

	_type = item_type.get_type_id();
	_category = item_type.get_category();

	_usable = usable;
	_known = false;
	_equipped = false;
	_cursed = item_type.get_cursed();
	_marked = false;
	_name = item_type.get_known_name();
	_uname = item_type.get_unknown_name();

	_id = s_id++;
}

Sorcery::Item::Item(const ItemType &item_type) {

	_type = item_type.get_type_id();
	_category = item_type.get_category();

	_known = false;
	_equipped = false;
	_cursed = item_type.get_cursed();
	_marked = false;
	_usable = true;
	_name = item_type.get_known_name();
	_uname = item_type.get_unknown_name();

	_id = s_id++;
}

Sorcery::Item::Item(const Item &item) {

	_type = item._type;
	_category = item._category;
	_known = item._known;
	_equipped = item._equipped;
	_cursed = item._cursed;
	_marked = item._marked;
	_usable = item._usable;
	_name = item._name;
	_uname = item._uname;
}

auto Sorcery::Item::get_type_id() const -> ITT {

	return _type;
}

auto Sorcery::Item::get_category() const -> ITC {

	return _category;
}

auto Sorcery::Item::get_known() const -> bool {

	return _known;
}

auto Sorcery::Item::set_usable(const bool value) -> void {

	_usable = value;
}

auto Sorcery::Item::get_usable() const -> bool {

	return _usable;
}

auto Sorcery::Item::get_equipped() const -> bool {

	return _equipped;
}

auto Sorcery::Item::get_cursed() const -> bool {

	return _cursed;
}

auto Sorcery::Item::get_marked() const -> bool {

	return _marked;
}

auto Sorcery::Item::get_name() const -> std::string {

	return _name;
}

auto Sorcery::Item::get_display_name() const -> std::string {

	return _known ? _name : _uname;
}

auto Sorcery::Item::set_known(const bool value) -> void {

	_known = value;
}

auto Sorcery::Item::set_equipped(const bool value) -> void {

	_equipped = value;
}

auto Sorcery::Item::set_cursed(const bool value) -> void {

	_cursed = value;
}

auto Sorcery::Item::set_marked(const bool value) -> void {

	_marked = value;
}

auto Sorcery::Item::set_name(const std::string &value) -> void {

	_name = value;
}

auto Sorcery::Item::decay_to(const ITT value) -> void {

	_type = value;
}

namespace Sorcery {

auto operator<<(std::ostream &out_stream, const Sorcery::Item &item)
	-> std::ostream & {

	const auto type{unenum(item.get_type_id())};
	const auto name{item.get_name()};

	const auto text{fmt::format("{}: {}", type, name)};

	return out_stream << text << std::flush;
}
} // namespace Sorcery
