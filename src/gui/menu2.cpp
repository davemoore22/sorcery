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

#include "gui/menu2.hpp"
#include "common/enum.hpp"
#include "common/macro.hpp"
#include "core/animation.hpp"
#include "core/display.hpp"
#include "core/game.hpp"
#include "core/graphics.hpp"
#include "core/state.hpp"
#include "core/system.hpp"
#include "core/window.hpp"
#include "resources/componentstore.hpp"
#include "resources/itemstore.hpp"
#include "resources/monsterstore.hpp"
#include "resources/resourcemanager.hpp"
#include "resources/stringstore.hpp"
#include "types/character.hpp"
#include "types/component.hpp"
#include "types/configfile.hpp"
#include "types/itemtype.hpp"
#include "types/monstertype.hpp"

// Standard Constructor
Sorcery::Menu2::Menu2(System *system, Display *display, Graphics *graphics,
	Game *game, const MTP type, const unsigned int page_size)
	: _system{system}, _display{display}, _graphics{graphics}, _game{game},
	  _type{type}, _page_size{page_size} {

	// Clear the Items
	items.clear();
	bounds.clear();
	_texts.clear();
	selected = std::nullopt;
	_current_page = 0;
}

// Noww generate the item displau; this is optimised so the minimum is done on
// each screen refresh where possible; either just a change to the selected
// items's background bar colour, or, for example, a full refresh to reflect a
// change of selected item

// Reload the Contents of the Menu (for use in Paging)
auto Sorcery::Menu2::refresh_contents() -> void {

	// Rregenerate the displayable items (previous, then current page items,
	// then next, then a space, then go back, so page size + 4 Bounds are
	// generated for each menu item to handle mouse over
	items.clear();
	auto hint{""s};
	auto non_entry_index{_items.size()};

	// Do we enable the Previous Page Entry?
	const auto prev_on{_current_page > 0};
	items.emplace_back(non_entry_index++, MIT::PREVIOUS, MIM::MI_PREVIOUS_PAGE,
		(*_display->string)["MENU_PREVIOUS_PAGE"], prev_on, CFG::NONE,
		hint); // Use _items.size() + 1 as index to avoid overlap

	// Now work out what page we are on, and therefore what items we need to
	// display (a selection from the larger list of entries)
	auto begin_page{(_page_size * _current_page)};
	auto end_page{begin_page + _page_size - 1};
	if (end_page > _items.size() - 1)
		end_page = _items.size() - 1;
	std::span<Sorcery::MenuEntry>(
		_items.begin() + begin_page, _items.begin() + end_page);

	// Add the selected "window" of items to the menu
	for (auto i = 0u; i < _page_size; i++) {
		if (begin_page + i < (_items.size() - 1))
			items.emplace_back(_items.at(begin_page + i));
		else
			items.emplace_back(non_entry_index++, MIT::SPACER, MIM::ITEM_SPACER,
				(*_display->string)["MENU_SPACER"], true, CFG::NONE, hint);
	}

	// And then the previous and back entries
	const auto next_on{_current_page < (_resized_item_count / _page_size)};
	items.emplace_back(non_entry_index++, MIT::NEXT, MIM::MI_NEXT_PAGE,
		(*_display->string)["MENU_NEXT_PAGE"], next_on, CFG::NONE, hint);
	items.emplace_back(non_entry_index++, MIT::CANCEL, MIM::MI_GO_BACK,
		(*_display->string)["MENU_NEXT_PAGE"], true, CFG::NONE, hint);
}

// Load the Menu Items
auto Sorcery::Menu2::load_entries() -> unsigned int {

	// Now depending on the menu type, add the relevant items - note that
	// the Previous, Next, and Go Back entries are loaded if necessary at
	// run time
	switch (_type) {
	case MTP::BESTIARY:
		_add_bestiary_creatures();
		break;
	default:
		break;
	};

	// Work out Page Sizes for Paging (to pad last page)
	_resized_item_count = _items.size();
	if (_resized_item_count % _page_size != 0)
		_resized_item_count =
			(_items.size() + _page_size) - (_items.size() % _page_size);

	selected = _get_first_enabled();
}

// Add the Creatures to the Bestiary
auto Sorcery::Menu2::_add_bestiary_creatures() -> void {

	const auto types{_game->monsterstore->get_all_types()};
	for (auto &monster : types)
		_add_item(unenum(monster.get_type_id()), MIT::ENTRY, MIM::MU_ITEM,
			monster.get_known_name(), unenum(monster.get_type_id()));
}

// Add an item to the Menu
auto Sorcery::Menu2::_add_item(const int index, const MIT item_type,
	const MIM code, std::string key, const bool enabled) -> void {

	// Note passing key by value as we are modifying the key here
	if (key.length() % 2 == 0)
		key.resize(key.length() + 1, 32);

	auto hint{""s};
	_items.emplace_back(static_cast<unsigned int>(index), item_type, code, key,
		enabled, CFG::NONE, hint);
}

// Get the Menu Type
auto Sorcery::Menu2::get_type() const -> MTP {

	return _type;
}

// Get the Page Size of the Menu (not the Count of Items)
auto Sorcery::Menu2::get_page_size() const -> unsigned int {

	return _page_size;
}

// Get the Count of the Items in the Menu (not the Page Size)
auto Sorcery::Menu2::get_item_count() const -> unsigned int {

	return _items.size();
}

// Return the index of the first Entry-type Item that is enabled.
auto Sorcery::Menu2::_get_first_enabled() -> std::optional<unsigned int> {

	auto it{std::ranges::find_if(
		items.begin(), items.end(), [&](const auto &menu_item) {
			return (((menu_item.type == MIT::ENTRY) ||
						(menu_item.type == MIT::SAVE) ||
						(menu_item.type == MIT::CANCEL)) &&
					(menu_item.enabled));
		})};

	if (it != items.end()) {
		return (*it).index;
	} else
		return std::nullopt;
}

// Print the Full Menu Contents
auto Sorcery::Menu2::print() -> void {

	std::string title{magic_enum::enum_name<MTP>(_type)};
	auto body{title + "\n\n"s};
	int index{0};

	for (const auto &item : _items) {

		std::string entry{item.key};
		std::string flag{item.enabled ? "Yes" : " No"};
		auto line{fmt::format("{} ({}) {}", index, flag, entry)};
		body.append(line);
		body.append("\n");
		++index;
	}

	std::cout << body << std::endl;
}

auto Sorcery::Menu2::draw(
	sf::RenderTarget &target, sf::RenderStates states) const -> void {

	states.transform *= getTransform();

	target.draw(_selected_bg, states);
	for (auto &text : _texts)
		target.draw(text, states);
}

namespace Sorcery {

// Additional Print Operator
auto operator<<(std::ostream &out_stream, const Sorcery::Menu2 &menu)
	-> std::ostream & {

	std::string title{magic_enum::enum_name<MTP>(menu._type)};
	auto body{title + "\n\n"s};
	int index{0};

	out_stream << title << std::endl;

	for (const auto &item : menu.items) {

		std::string entry{item.key};
		std::string flag{item.enabled ? "Yes" : " No"};
		auto line{fmt::format("{} ({}) {}", index, flag, entry)};
		body.append(line);
		body.append("\n");
		++index;
	}

	out_stream << body << std::endl;

	return out_stream;
}

}