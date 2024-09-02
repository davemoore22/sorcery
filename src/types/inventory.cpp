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

#include "types/inventory.hpp"
#include "types/itemtype.hpp"

Sorcery::Inventory::Inventory() {

	_items.clear();
}

auto Sorcery::Inventory::operator[](const unsigned int slot) -> std::optional<Item *> {

	if (_items.size() >= (slot - 1))
		return &_items.at(slot - 1);
	else
		return std::nullopt;
}

auto Sorcery::Inventory::get(const unsigned int slot) -> Item {

	return _items.at(slot - 1);
}

auto Sorcery::Inventory::has(const unsigned int slot) const -> bool {

	return _items.size() >= (slot - 1);
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

auto Sorcery::Inventory::get_empty_slots() const -> unsigned int {

	return 8 - _items.size();
}

auto Sorcery::Inventory::is_empty() const -> bool {

	return _items.size() == 8;
}

auto Sorcery::Inventory::add_type(const ItemType &item_type, const bool known) -> bool {

	if (_items.size() != 8) {
		Item item{item_type};
		item.set_known(known);
		item.set_usable(true);
		_items.emplace_back(item);
		return true;
	} else
		return false;
}

auto Sorcery::Inventory::add(Item item) -> void {

	_items.emplace_back(item);
}

auto Sorcery::Inventory::add_type(const ItemType &item_type, const bool usable, const bool known) -> bool {

	if (_items.size() != 8) {
		Item item{item_type};
		item.set_known(known);
		item.set_usable(usable);
		_items.emplace_back(item);
		return true;
	} else
		return false;
}

auto Sorcery::Inventory::add_type(const ItemType &item_type) -> bool {

	if (_items.size() != 8) {
		_items.emplace_back(item_type);
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

	return std::ranges::any_of(
		_items.begin(), _items.end(), [&](const auto &item) { return item.get_known() == false; });
}

auto Sorcery::Inventory::has_cursed_items() const -> bool {

	return std::ranges::any_of(
		_items.begin(), _items.end(), [&](const auto &item) { return item.get_cursed() == false; });
}

auto Sorcery::Inventory::is_equipped_cursed(const unsigned int slot) -> bool {

	if (_items.size() < (slot - 1))
		return false;

	const auto candidate{_items.at(slot - 1)};

	return candidate.get_cursed() && candidate.get_equipped();
}

auto Sorcery::Inventory::_unequip_item_category(const ItemCategory category) -> bool {

	for (auto &item : _items) {
		if (item.get_equipped() && item.get_category() == category) {
			item.set_equipped(false);
			return true;
		}
	}

	return false;
}

// Attempt to identify an Item
auto Sorcery::Inventory::identify_item(const unsigned int slot, const unsigned int roll, const unsigned int id_chance,
	const unsigned int curse_chance) -> ItemIDResult {

	auto cursed{false};
	auto success{false};

	if (_items.size() < (slot - 1))
		return ItemIDResult::NONE;

	auto &candidate{_items.at(slot - 1)};
	if (roll < id_chance) {
		success = true;
		candidate.set_known(true);
	}
	if (roll < curse_chance) {
		if (candidate.get_cursed()) {
			cursed = true;
			candidate.set_equipped(true);
		}
	}

	if (success && !cursed)
		return ItemIDResult::SUCCESS;
	else if (success && cursed)
		return ItemIDResult::CURSED_SUCCESS;
	else if (!success && cursed)
		return ItemIDResult::CURSED_FAIL;
	else
		return ItemIDResult::FAIL;
}

auto Sorcery::Inventory::equip_item(const unsigned int slot) -> bool {

	if (_items.size() < (slot - 1))
		return false;

	auto &candidate{_items.at(slot - 1)};
	const auto item_category{candidate.get_category()};

	if (!candidate.get_usable())
		return false;

	if (_has_cursed_equipped_item_category(item_category))
		return false;

	for (auto &item : _items) {

		if (item.get_category() == item_category && item.get_equipped())
			item.set_equipped(false);
	};

	candidate.set_equipped(true);
	candidate.set_known(true);
	return true;
}

auto Sorcery::Inventory::drop_item(const unsigned int slot) -> bool {

	if (_items.size() < (slot - 1))
		return false;

	auto candidate{_items.at(slot - 1)};
	if (candidate.get_equipped())
		return false;
	candidate.set_known(true);

	// For now, just discard the item
	_items.erase(_items.begin() + (slot - 1));

	return true;
}

auto Sorcery::Inventory::discard_item(const unsigned int slot) -> bool {

	if (_items.size() < (slot - 1))
		return false;

	// For now, just discard the item
	_items.erase(_items.begin() + (slot - 1));

	return true;
}

auto Sorcery::Inventory::_has_equipped_item_category(const ItemCategory category) const -> bool {

	return std::ranges::any_of(_items.begin(), _items.end(),
		[&](const auto &item) { return item.get_category() == category && item.get_equipped(); });
}

auto Sorcery::Inventory::has_cursed_equipped_item_category(const ItemCategory category) const -> bool {

	return _has_cursed_equipped_item_category(category);
}

auto Sorcery::Inventory::_has_cursed_equipped_item_category(const ItemCategory category) const -> bool {

	return std::ranges::any_of(_items.begin(), _items.end(),
		[&](const auto &item) { return item.get_category() == category && item.get_equipped() && item.get_cursed(); });
}

namespace Sorcery {

auto operator<<(std::ostream &out_stream, const Sorcery::Inventory &inventory) -> std::ostream & {

	auto body{"Inventory:\n\n"s};
	int slot{1};

	for (const auto &item : inventory._items) {
		const std::string flag{std::invoke([&] {
			if (!item.get_known())
				return "?";
			else if (!item.get_usable())
				return "#";
			else if (item.get_cursed() && item.get_equipped())
				return "-";
			else if (item.get_equipped())
				return "*";
			else
				return " ";
		})};

		auto line{fmt::format("{}){}{}", slot, flag, item.get_display_name())};
		body.append(line);
		body.append("\n");
		++slot;
	}

	return out_stream << body << std::endl;
}
} // namespace Sorcery
