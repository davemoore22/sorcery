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

#include "gui/text.hpp"
#include "common/enum.hpp"
#include "common/include.hpp"
#include "common/macro.hpp"
#include "core/display.hpp"
#include "core/system.hpp"
#include "resources/resourcemanager.hpp"
#include "resources/stringstore.hpp"
#include "types/component.hpp"

Sorcery::Text::Text() {

	_system = nullptr;
	_display = nullptr;
	_text = sf::Text();
}

Sorcery::Text::Text(System *system, Display *display) : _system{system}, _display{display} {

	_text = sf::Text();
}

Sorcery::Text::Text(
	System *system, Display *display, const std::optional<Component> component = std::nullopt, const int bits = -1)
	: _system{system}, _display{display} {

	_text = sf::Text();
	if (component) {

		Component layout{component.value()};

		if (bits | unenum(ComponentElement::FONT))
			_text.setFont(_system->resources->fonts[layout.font]);
		if (bits | unenum(ComponentElement::SIZE))
			_text.setCharacterSize(layout.size);
		if (bits | unenum(ComponentElement::COLOUR))
			_text.setFillColor(sf::Color(layout.colour));
		if (bits | unenum(ComponentElement::STRING))
			_text.setString((*_display->string)[layout.string_key]);
		if (bits | unenum(ComponentElement::OFFSET)) {
			const auto offset_x{std::invoke([&] {
				if (layout["offset_x"])
					return std::stoi(layout["offset_x"].value());
				else
					return 0;
			})};
			const auto offset_y{std::invoke([&] {
				if (layout["offset_y"])
					return std::stoi(layout["offset_y"].value());
				else
					return 0;
			})};
			_text.setPosition(offset_x, offset_y);
		}
		if (bits | unenum(ComponentElement::ORIGIN)) {
			const auto origin_x{std::invoke([&] {
				if (layout["origin_x"])
					return std::stoi(layout["origin_x"].value());
				else
					return 0;
			})};
			const auto origin_y{std::invoke([&] {
				if (layout["origin_y"])
					return std::stoi(layout["origin_y"].value());
				else
					return 0;
			})};
			_text.setPosition(origin_x, origin_y);
		}
		if (bits | unenum(ComponentElement::O_COLOUR)) {
			const auto outline_colour{std::invoke([&] {
				if (layout["outline_colour"])
					return sf::Color(std::stoull(layout["outline_colour"].value(), 0, 16));
				else
					return sf::Color(sf::Color::Black);
			})};
		}
		if (bits | unenum(ComponentElement::O_THICKNESS)) {
			const auto outline_thickness{std::invoke([&] {
				if (layout["origin_y"])
					return std::stoi(layout["outline_thickness"].value());
				else
					return 0;
			})};
			_text.setOutlineThickness(outline_thickness);
		}
		if (bits | unenum(ComponentElement::JUSTIFICATION)) {

			if (layout.justification == JUS::CENTRE) {
				_text.setPosition(0, 0);
				_text.setOrigin(_text.getLocalBounds().width / 2.0f, 0);
			} else if (layout.justification == JUS::RIGHT) {
				_text.setPosition(0, 0);
				const sf::FloatRect bounds{_text.getLocalBounds()};
				_text.setPosition(layout.x - bounds.width, layout.y);
			} else {
				_text.setPosition(0, 0);
				_text.setOrigin(0, 0);
			}

			// Handle varying height of proportional fonts
			if (layout.font == FontType::PROPORTIONAL)
				_text.setPosition(
					_text.getPosition().x, _text.getPosition().y - ((layout.size - _text.getLocalBounds().height) / 2));
		}
	}
}

auto Sorcery::Text::get_global_bounds() const -> sf::FloatRect {

	return _text.getGlobalBounds();
}

auto Sorcery::Text::get_local_bounds() const -> sf::FloatRect {

	return _text.getLocalBounds();
}

auto Sorcery::Text::get_position() const -> sf::Vector2f {

	return _text.getPosition();
}

auto Sorcery::Text::set_character_size(const unsigned int size) -> void {

	_text.setCharacterSize(size);
}

auto Sorcery::Text::set_fill_colour(const sf::Color &colour) -> void {

	_text.setFillColor(colour);
}

auto Sorcery::Text::set_font(sf::Font font) -> void {

	_text.setFont(font);
}

auto Sorcery::Text::set_origin(const float x, const float y) -> void {

	_text.setOrigin(x, y);
}

auto Sorcery::Text::set_outline_colour(const sf::Color &colour) -> void {

	_text.setOutlineColor(colour);
}

auto Sorcery::Text::set_outline_thickness(const unsigned int size) -> void {

	_text.setOutlineThickness(size);
}

auto Sorcery::Text::set_position(const float x, const float y) -> void {

	_text.setPosition(x, y);
}

auto Sorcery::Text::set_string(const std::string value) -> void {

	_text.setString(value);
}

auto Sorcery::Text::draw(sf::RenderTarget &target, sf::RenderStates states) const -> void {

	states.transform *= getTransform();
	target.draw(_text, states);
}
