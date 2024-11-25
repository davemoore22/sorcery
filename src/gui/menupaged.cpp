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

#include "gui/menupaged.hpp"
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
Sorcery::MenuPaged::MenuPaged(System *system, Display *display,
	Graphics *graphics, Game *game, const MTP type,
	const unsigned int page_size)
	: _system{system}, _display{display}, _graphics{graphics}, _game{game},
	  _type{type}, _page_size{page_size} {

	// Clear the Items
	items.clear();
	bounds.clear();
	_texts.clear();
	selected = std::nullopt;
	_current_page = 0;

	_load_entries();
}

// Noww generate the item displau; this is optimised so the minimum is done on
// each screen refresh where possible; either just a change to the selected
// items's background bar colour, or, for example, a full refresh to reflect a
// change of selected item
auto Sorcery::MenuPaged::generate(
	const Component &component, bool force_refresh) -> void {

	auto do_refresh{force_refresh || _texts.empty()};
	if (do_refresh) {

		// In case we are generating the Options Menu
		// Work out total size of texture needed
		const auto tx{component.w * _display->window->get_cw()};
		const auto ty{component.h * _display->window->get_ch()};
		const sf::Vector2f sz(tx, ty);
		_render_tx.create(sz.x, sz.y);

		_width = sz.x;
		_height = sz.y;

		// Bounds are generated for each menu item to handle mouse over
		_texts.clear();
		bounds.clear();
		auto i{0u};
		auto entry_y{0};

		// In case we do not have any item selected (such as the first time)
		if (!selected)
			selected = 0;

		for (const auto &item : items) {

			if (item.type == MIT::TEXT || item.type == MIT::ENTRY ||
				item.type == MIT::SAVE || item.type == MIT::CANCEL ||
				item.type == MIT::PREVIOUS || item.type == MIT::NEXT) {
				auto text_string{item.key};
				sf::Text text{};
				text.setFont(_system->resources->fonts[component.font]);
				text.setCharacterSize(component.size);
				if (_display->get_bold())
					text.setStyle(sf::Text::Bold);
				if (item.enabled)
					text.setFillColor(sf::Color(component.colour));
				else
					text.setFillColor(
						sf::Color(0x606060ff)); // TODO change this to be
												// located in the layout file
				if (_display->get_upper())
					std::transform(text_string.begin(), text_string.end(),
						text_string.begin(), ::toupper);
				text.setString(text_string);

				// Check for alignment and set location appropriately
				auto entry_x{
					(component.justification == JUS::CENTRE) ? tx / 2 : 0};
				text.setPosition(entry_x - text.getLocalBounds().left,
					entry_y - text.getLocalBounds().top);

				// If we have a selected entry, change the background colour
				if (selected.value() == i) {

					// See
					// https://stackoverflow.com/questions/14505571/centering-text-on-the-screen-with-sfml/15253837#15253837
					sf::RectangleShape bg(
						sf::Vector2f(component.w * _display->window->get_cw(),
							_system->resources->get_font_height(
								component.font, component.size)));
					bg.setPosition(0, entry_y);
					if (component.animated)
						bg.setFillColor(_graphics->animation->select_col);
					else
						bg.setFillColor(sf::Color(component.background));

					text.setFillColor(sf::Color(component.colour));
					text.setOutlineColor(sf::Color(0, 0, 0));
					text.setOutlineThickness(1);

					_selected_bg = bg;
				}

				// Handle Justification
				if (component.justification == JUS::CENTRE)
					text.setOrigin(text.getLocalBounds().width / 2.0f, 0);

				_texts.emplace_back(text);

				// Now handle the mouse move/select!
				if (item.type == MIT::ENTRY || item.type == MIT::SAVE ||
					item.type == MIT::CANCEL || item.type == MIT::PREVIOUS ||
					item.type == MIT::NEXT) {
					const sf::FloatRect actual_rect{text.getGlobalBounds()};
					bounds.push_back(actual_rect);
				} else {
					const sf::FloatRect actual_rect;
					bounds.push_back(actual_rect);
				}

				entry_y += _display->window->get_ch();
			} else {
				sf::Text empty_text{};
				const sf::FloatRect actual_rect{};
				bounds.push_back(actual_rect);
				_texts.emplace_back(empty_text);
				entry_y += _display->window->get_ch();
			}
			++i;
		}
	} else {

		// Only change what needs to be changed
		auto entry_y{0};
		auto i{0u};
		for (const auto &item : items) {
			if (item.type == MIT::TEXT || item.type == MIT::ENTRY ||
				item.type == MIT::SAVE || item.type == MIT::CANCEL) {
				if (selected.value() == i) {
					const sf::FloatRect bg_rect{_texts.at(i).getGlobalBounds()};
					sf::RectangleShape bg(
						sf::Vector2f(component.w * _display->window->get_cw(),
							bg_rect.height));
					bg.setPosition(0, entry_y);
					if (component.animated)
						bg.setFillColor(_graphics->animation->select_col);
					else
						bg.setFillColor(sf::Color(component.background));

					if (item.enabled)
						_texts.at(i).setFillColor(sf::Color(component.colour));
					else
						_texts.at(i).setFillColor(sf::Color(0x606060ff));
					_texts.at(i).setOutlineColor(sf::Color(0, 0, 0));
					_texts.at(i).setOutlineThickness(1);

					_selected_bg = bg;
				}
			}
			++i;
			entry_y += _display->window->get_ch();
		}
	}
}

// Choose the previous selected item
auto Sorcery::MenuPaged::choose_previous() -> std::optional<unsigned int> {

	if (selected > 0) {

		// Iterate backwards until we find the first previous enabled menu
		bool found{false};
		int i{static_cast<int>(selected.value())};
		do {
			--i;
			const auto &item{items.at(i)};
			found = item.enabled &&
					((item.type == MIT::ENTRY) || (item.type == MIT::SAVE) ||
						(item.type == MIT::CANCEL));
		} while (i >= 0 && !found);
		if (found) {
			selected = i;
			return selected;
		}
	} else
		return selected;

	return std::nullopt;
}

// Check if the mouse cursor is on a menu item, and if so set it
auto Sorcery::MenuPaged::set_mouse_selected(sf::Vector2f mouse_pos)
	-> std::optional<unsigned int> {

	if (!bounds.empty()) {

		// Look for the bounds the mouse cursor is in, but select and return
		// the associated item with the same index, since both containers
		// track each other
		const sf::Vector2f global_pos{this->getPosition()};
		mouse_pos -= global_pos;
		auto it{std::ranges::find_if(
			bounds.begin(), bounds.end(), [&mouse_pos](const auto &item) {
				return item.contains(mouse_pos);
			})};
		if (it != bounds.end()) {
			auto dist{std::distance(bounds.begin(), it)};
			if (items.at(dist).enabled)
				selected = dist;

			return selected;
		} else
			return std::nullopt;
	} else
		return std::nullopt;

	// If we reach here it means that bounds (which requites a draw to take
	// place, hasn't been populated yet)
	return std::nullopt;
}

auto Sorcery::MenuPaged::go_to_next_page() -> std::optional<unsigned int> {

	const auto next_on{
		_current_page <
		(std::floor(static_cast<float>(_resized_item_count) / _page_size)) - 1};
	if (next_on) {
		++_current_page;
		_refresh_contents();
	}

	return selected;
}

auto Sorcery::MenuPaged::go_to_previous_page() -> std::optional<unsigned int> {

	const auto prev_on{_current_page > 0};
	if (prev_on) {
		--_current_page;
		_refresh_contents();
	}

	return selected;
}

auto Sorcery::MenuPaged::choose_first() -> std::optional<unsigned int> {

	selected = _get_first_enabled();

	return selected;
}

auto Sorcery::MenuPaged::choose_last() -> std::optional<unsigned int> {

	selected = _get_last_enabled();

	return selected;
}

// Choose the next selected item
auto Sorcery::MenuPaged::choose_next() -> std::optional<unsigned int> {

	if (selected < (items.size())) {

		// Go forwards until we find the first next enabled item
		bool found{false};
		unsigned int i{selected.value()};
		do {
			++i;
			const auto &item{items.at(i)};
			found = item.enabled &&
					(item.type == MIT::ENTRY || item.type == MIT::SAVE ||
						item.type == MIT::CANCEL);
		} while (i < items.size() && !found);
		if (found) {

			selected = i;
			return selected.value();
		}
	} else
		return selected.value();

	return std::nullopt;
}

// Reload the Contents of the Menu (for use in Paging)
auto Sorcery::MenuPaged::_refresh_contents() -> void {

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
				(*_display->string)["MENU_SPACER"], false, CFG::NONE, hint);
	}

	// And then the previous and back entries
	const auto next_on{
		_current_page <
		(std::floor(static_cast<float>(_resized_item_count) / _page_size)) - 1};
	items.emplace_back(non_entry_index++, MIT::NEXT, MIM::MI_NEXT_PAGE,
		(*_display->string)["MENU_NEXT_PAGE"], next_on, CFG::NONE, hint);
	items.emplace_back(non_entry_index++, MIT::CANCEL, MIM::MI_GO_BACK,
		(*_display->string)["MENU_GO_BACK"], true, CFG::NONE, hint);
}

// Load the Menu Items
auto Sorcery::MenuPaged::_load_entries() -> unsigned int {

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
auto Sorcery::MenuPaged::_add_bestiary_creatures() -> void {

	const auto types{_game->monsterstore->get_all_types()};
	for (auto &monster : types) {
		if (monster.get_type_id() <= MTI::WERDNA)
			_add_item(unenum(monster.get_type_id()), MIT::ENTRY, MIM::MU_ITEM,
				monster.get_known_name(), true);
	}
}

// Add an item to the Menu
auto Sorcery::MenuPaged::_add_item(const int index, const MIT item_type,
	const MIM code, std::string key, const bool enabled) -> void {

	// Note passing key by value as we are modifying the key here
	if (key.length() % 2 == 0)
		key.resize(key.length() + 1, 32);

	auto hint{""s};
	_items.emplace_back(static_cast<unsigned int>(index), item_type, code, key,
		enabled, CFG::NONE, hint);
}

// Get the Menu Type
auto Sorcery::MenuPaged::get_type() const -> MTP {

	return _type;
}

// Get the Page Size of the Menu (not the Count of Items)
auto Sorcery::MenuPaged::get_page_size() const -> unsigned int {

	return _page_size;
}

// Get the Count of the Items in the Menu (not the Page Size)
auto Sorcery::MenuPaged::get_item_count() const -> unsigned int {

	return _items.size();
}

// Return the index of the firstventry Item that is enabled.
auto Sorcery::MenuPaged::_get_first_enabled() -> std::optional<unsigned int> {

	for (auto i = 0u; i < items.size(); i++) {
		if (((items.at(i).type == MIT::ENTRY) ||
				(items.at(i).type == MIT::SAVE) ||
				(items.at(i).type == MIT::PREVIOUS) ||
				(items.at(i).type == MIT::NEXT) ||
				(items.at(i).type == MIT::CANCEL)) &&
			(items.at(i).enabled))
			return i;
	}

	return std::nullopt;
}

// Return the index of the last entry Item that is enabled.
auto Sorcery::MenuPaged::_get_last_enabled() -> std::optional<unsigned int> {

	for (int i = items.size() - 1; i >= 0; i--) {
		if (((items.at(i).type == MIT::ENTRY) ||
				(items.at(i).type == MIT::SAVE) ||
				(items.at(i).type == MIT::PREVIOUS) ||
				(items.at(i).type == MIT::NEXT) ||
				(items.at(i).type == MIT::CANCEL)) &&
			(items.at(i).enabled))
			return i;
	}

	return std::nullopt;
};

auto Sorcery::MenuPaged::get_current_page() const -> unsigned int {

	return _current_page;
}

auto Sorcery::MenuPaged::set_current_page(const unsigned int value) -> void {

	_current_page = value;
	_refresh_contents();
}

// Print the Menu Contents
auto Sorcery::MenuPaged::print() -> void {

	std::string title{magic_enum::enum_name<MTP>(_type)};
	auto body{title + "\n\n"s};
	int index{0};

	for (const auto &item : items) {

		std::string entry{item.key};
		std::string flag{item.enabled ? "Yes" : " No"};
		auto line{fmt::format("{} ({}) {}", index, flag, entry)};
		body.append(line);
		body.append("\n");
		++index;
	}

	std::cout << body << std::endl;
}

auto Sorcery::MenuPaged::draw(
	sf::RenderTarget &target, sf::RenderStates states) const -> void {

	states.transform *= getTransform();

	target.draw(_selected_bg, states);
	for (auto &text : _texts)
		target.draw(text, states);
}

namespace Sorcery {

// Additional Print Operator
auto operator<<(std::ostream &out_stream, const Sorcery::MenuPaged &menu)
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

auto Sorcery::MenuPaged::get_width() const -> unsigned int {

	return _width;
}

auto Sorcery::MenuPaged::get_height() const -> unsigned int {

	return _height;
}

}