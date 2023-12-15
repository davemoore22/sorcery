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
// be useful, but WITHOUT ANY WARRANTY; wiWthout even the implied warranty of
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

#include "inventory.hpp"

Sorcery::Inventory::Inventory() {

	_items.clear();
}

auto Sorcery::Inventory::operator[](const int slot) -> std::optional<Item *> {

	if (_items.size() >= (slot - 1))
		return &_items.at(slot - 1);
	else
		return std::nullopt;
}

auto Sorcery::Inventory::items() const -> std::vector<Item> {

	return _items;
}

auto Sorcery::Inventory::clear() -> void {

	_items.clear();
}

auto Sorcery::Inventory::size() const -> unsigned int {

	return _items.size();
}

auto Sorcery::Inventory::is_full() const -> bool {

	return _items.size() == 8;
}

auto Sorcery::Inventory::is_empty() const -> bool {

	return _items.size() == 8;
}

auto Sorcery::Inventory::add_type(ItemType item_type, const bool known) -> bool {

	if (_items.size() != 8) {
		Item item{item_type};
		item.set_known(known);
		_items.emplace_back(item);
		return true;
	} else
		return false;
}

auto Sorcery::Inventory::add_type(ItemType item_type) -> bool {

	if (_items.size() != 8) {
		Item item{item_type};
		_items.emplace_back(item);
		return true;
	} else
		return false;
}

auto Sorcery::Inventory::unequip_all() -> void {

	for (auto &item : _items) {
		if (item.get_equipped())
			item.set_equipped(false);
	}
}

auto Sorcery::Inventory::has_unidentified_items() const -> bool {

	for (const auto &item : _items) {
		if (item.get_cursed())
			return true;
	}

	return false;
}

auto Sorcery::Inventory::has_cursed_items() const -> bool {

	for (const auto &item : _items) {
		if (!item.get_known())
			return true;
	}

	return false;
}

namespace Sorcery {

auto operator<<(std::ostream &out_stream, const Sorcery::Inventory &inventory) -> std::ostream & {

	auto body{"Inventory:\n\n"s};
	int slot{1};

	for (const auto &item : inventory._items) {

		std::string flag{!item.get_usable() ? "#" : (item.get_equipped() ? "*" : " ")};
		auto line{fmt::format("{}){}{}", slot, flag, item.get_name())};
		body.append(line);
		body.append("\n");
		++slot;
	}

	return out_stream << body << std::endl;
}
} // namespace Sorcery
