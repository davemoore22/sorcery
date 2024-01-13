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

// Default Constructor
Sorcery::Item::Item() {

	using enum Enums::Items::TypeID;

	_type = BROKEN_ITEM;
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
	_usable = usable;
	_known = false;
	_equipped = false;
	_cursed = false;
	_marked = false;
	_name = item_type.get_known_name();
	_uname = item_type.get_unknown_name();

	_id = s_id++;
}

Sorcery::Item::Item(const ItemType &item_type) {

	_type = item_type.get_type_id();

	_known = false;
	_equipped = false;
	_cursed = false;
	_marked = false;
	_usable = true;
	_name = item_type.get_known_name();
	_uname = item_type.get_unknown_name();

	_id = s_id++;
}

auto Sorcery::Item::get_type_id() const -> ItemTypeID {

	return _type;
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

	std::string prefix{_equipped ? "*" : (!_usable ? "#" : " ")};
	std::string name{_known ? _name : _uname};
	return fmt::format("{}{}", prefix, name);
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

auto Sorcery::Item::decay_to(const ItemTypeID value) -> void {

	_type = value;
}

namespace Sorcery {

auto operator<<(std::ostream &out_stream, const Sorcery::Item &item) -> std::ostream & {

	const auto type{unenum(item.get_type_id())};
	const auto name{item.get_name()};

	const auto text{fmt::format("{}: {}", type, name)};

	return out_stream << text << std::flush;
}
} // namespace Sorcery
