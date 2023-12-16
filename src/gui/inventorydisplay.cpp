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

#include "gui/inventorydisplay.hpp"

Sorcery::InventoryDisplay::InventoryDisplay(System *system, Display *display, Graphics *graphics, Inventory *inventory)
	: _system{system}, _display{display}, _graphics{graphics}, _inventory{inventory} {

	_layout = Component((*_display->layout)["global:inventory_display"]);
}

auto Sorcery::InventoryDisplay::generate() -> void {

	_sprites.clear();
	_texts.clear();

	auto slot{1u};
	auto x{std::stoi(_layout["offset_x"].value())};
	const auto start_x{x};
	auto y{std::stoi(_layout["offset_y"].value())};
	const auto start_y{y};
	const auto row_y{std::stoi(_layout["row_y"].value())};
	const auto column_x{std::stoi(_layout["column_x"].value())};

	for (const auto &item : _inventory->items()) {

		std::string flag{!item.get_usable() ? "#" : (item.get_equipped() ? "*" : " ")};
		auto line{fmt::format("{}){}{}", slot, flag, item.get_name())};

		if (slot % 2 == 1) {
			x = start_x;
			y += row_y;
		} else {
			x = start_x + column_x;
		}

		sf::Text text{};
		text.setFont(_system->resources->fonts[_layout.font]);
		text.setCharacterSize(_layout.size);
		text.setFillColor(sf::Color(_layout.colour));

		text.setString(line);
		if (_display->get_bold())
			text.setStyle(sf::Text::Bold);
		text.setPosition(x, y);

		auto new_unique_key{GUID()};
		_texts.try_emplace(new_unique_key, text);

		++slot;
	}
}

auto Sorcery::InventoryDisplay::_add_text(Component &component, std::string format, std::string value) -> sf::Text * {

	sf::Text text{};

	// Note that Format v8 needs the format string wrapped in fmt::runtime - this isn't available in < v8 - see
	// https://github.com/fmtlib/fmt/issues/2438 - check FMT_VERSION version in fmt/core.h
	auto formatted_value{fmt::format(fmt::runtime(format), value)};
	if (_display->get_upper())
		std::transform(formatted_value.begin(), formatted_value.end(), formatted_value.begin(), ::toupper);
	text.setFont(_system->resources->fonts[component.font]);
	text.setCharacterSize(component.size);
	text.setFillColor(sf::Color(component.colour));

	text.setString(formatted_value);
	if (_display->get_bold())
		text.setStyle(sf::Text::Bold);
	const auto offset_x{[&] {
		if (component["offset_x"])
			return std::stoi(component["offset_x"].value());
		else
			return 0;
	}()};
	const auto offset_y{[&] {
		if (component["offset_y"])
			return std::stoi(component["offset_y"].value());
		else
			return 0;
	}()};
	text.setPosition(component.x + offset_x, component.y + offset_y);

	// Generate a new key as this is a map, and we might call this with the same base component
	auto new_unique_key{GUID()};
	_texts.try_emplace(new_unique_key, text);
	return &_texts.at(new_unique_key);
}

auto Sorcery::InventoryDisplay::draw(sf::RenderTarget &target, sf::RenderStates states) const -> void {

	states.transform *= getTransform();

	// Draw the standard components
	for (const auto &[unique_key, frame] : _frames)
		target.draw(*frame, states);

	for (const auto &[unique_key, sprite] : _sprites)
		target.draw(sprite, states);

	for (const auto &[unique_key, text] : _texts)
		target.draw(text, states);
}
