// Copyright (C) 2021 Dave Moore
//
// This file is part of Sorcery: Dreams of the Mad Overlord.
//
// Sorcery: Dreams of the Mad Overlord is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Sorcery: Dreams of the Mad Overlord is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Sorcery: Dreams of the Mad Overlord.  If not, see <http://www.gnu.org/licenses/>.
//
// If you modify this Program, or any covered work, by linking or combining it
// with the libraries referred to in README (or a modified version of said
// libraries), containing parts covered by the terms of said libraries, the
// licensors of this Program grant you additional permission to convey the
// resulting work.

#include "attributedisplay.hpp"

// Standard Constructor
Sorcery::AttributeDisplay::AttributeDisplay(
	System *system, Display *display, Graphics *graphics, Character *character, Alignment alignment)
	: _system{system}, _display{display}, _graphics{graphics}, _character{character},
	  _alignment{alignment} {

	_bars.clear();

	// Get the standard layout information
	if (alignment == Alignment::VERTICAL) {
		_bar_c = Component((*_display->layout)["attribute_display:stat_bar_vertical"]);
		_icons_c = Component((*_display->layout)["attribute_display:attribute_icons_vertical"]);
		_text_c = Component((*_display->layout)["attribute_display:stat_text_vertical"]);
	} else {
		_bar_c = Component((*_display->layout)["attribute_display:stat_bar_horizontal"]);
		_icons_c = Component((*_display->layout)["attribute_display:attribute_icons_horizontal"]);
		_text_c = Component((*_display->layout)["attribute_display:stat_text_horizontal"]);
	}

	_attribute_icons[0] = (*_graphics->icons)[MenuItem::CS_STRENGTH].value();
	_attribute_icons[1] = (*_graphics->icons)[MenuItem::CS_IQ].value();
	_attribute_icons[2] = (*_graphics->icons)[MenuItem::CS_PIETY].value();
	_attribute_icons[3] = (*_graphics->icons)[MenuItem::CS_VITALITY].value();
	_attribute_icons[4] = (*_graphics->icons)[MenuItem::CS_AGILITY].value();
	_attribute_icons[5] = (*_graphics->icons)[MenuItem::CS_LUCK].value();

	const sf::Vector2u icon_size{_icons_c.size, _icons_c.size};
	constexpr float texture_size{511};
	sf::Vector2f scale{icon_size.x / texture_size, icon_size.y / texture_size};

	int index{0};
	for (auto &icon : _attribute_icons) {
		icon.setScale(scale);
		if (_alignment == Alignment::VERTICAL)
			icon.setPosition(8 + _icons_c.x + (index * _icons_c.size), _icons_c.y);
		else
			icon.setPosition(_icons_c.x, 8 + _icons_c.y + (index * _icons_c.size));
		++index;
	}

	// Not valid until we call the set command
	valid = false;
}

auto Sorcery::AttributeDisplay::set() -> void {

	_bars.clear();

	int x{0};
	int y{0};
	for (const auto &[attribute, value] : _character->get_current_attributes()) {

		auto bar{_get_attribute_bar(attribute)};
		if (_alignment == Alignment::VERTICAL) {
			bar.setPosition(
				(_bar_c.w / 3) + _bar_c.x + (x * _display->window->get_cell_width() * _bar_c.scale),
				_bar_c.y + _bar_c.size * 2);
			bar.scale(1.0f, -1.0f);
		} else {
			auto text{_get_attribute_text(attribute)};
			text.setPosition(
				_text_c.x + std::stoi(_text_c["offset_x"].value()), _text_c.y + (y * _text_c.size));
			bar.setPosition((_bar_c.x + std::stoi(_bar_c["offset_x"].value())),
				(_bar_c.h / 3) + _bar_c.y +
					(y * _display->window->get_cell_height() * _bar_c.scale));
			_texts.push_back(text);
		};

		_bars.push_back(bar);

		++x;
		++y;
	}

	valid = true;
}

auto Sorcery::AttributeDisplay::_get_attribute_text(CharacterAttribute attribute) -> sf::Text {

	if (_alignment == Alignment::VERTICAL) {

		sf::Text text;

		// TODO
		return text;

	} else {
		sf::Text text;
		std::string formatted_value{fmt::format("{:>2}", _character->get_attribute(attribute))};
		text.setFont(_system->resources->fonts[_text_c.font]);
		text.setCharacterSize(_text_c.size);
		text.setFillColor(sf::Color(_text_c.colour));
		text.setString(formatted_value);

		return text;
	}
}

auto Sorcery::AttributeDisplay::_get_attribute_bar(CharacterAttribute attribute)
	-> sf::RectangleShape {

	// Generate three bars which will simply be put on top of each other
	if (_alignment == Alignment::VERTICAL) {
		sf::RectangleShape attr(
			sf::Vector2f(_bar_c.w / 2, (_bar_c.h * _character->get_attribute(attribute) / 2)));
		attr.setFillColor(sf::Color(_bar_c.colour));
		attr.setOutlineThickness(1);
		return attr;
	} else {
		sf::RectangleShape attr(
			sf::Vector2f(_bar_c.w * _character->get_attribute(attribute) / 2, _bar_c.h / 2));
		attr.setFillColor(sf::Color(_bar_c.colour));
		attr.setOutlineThickness(1);
		return attr;
	}
}

auto Sorcery::AttributeDisplay::draw(sf::RenderTarget &target, sf::RenderStates states) const
	-> void {

	states.transform *= getTransform();

	for (const auto &bar : _bars)
		target.draw(bar, states);

	for (const auto &icon : _attribute_icons)
		target.draw(icon, states);

	for (const auto &text : _texts)
		target.draw(text, states);
}
