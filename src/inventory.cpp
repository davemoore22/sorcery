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

auto Sorcery::Inventory::operator[](const int slot) -> Item * {
}

auto Sorcery::Inventory::clear() -> void {
}

auto Sorcery::Inventory::size() const -> unsigned int {
}

auto Sorcery::Inventory::is_full() const -> bool {
}

auto Sorcery::Inventory::is_empty() const -> bool {
}

auto Sorcery::Inventory::add_type(ItemTypeID item_type) -> bool {
}

namespace Sorcery {

auto operator<<(std::ostream &out_stream, const Sorcery::Inventory &Inventory) -> std::ostream & {

	auto body{""s};

	return out_stream << body << std::endl;
}
} // namespace Sorcery
