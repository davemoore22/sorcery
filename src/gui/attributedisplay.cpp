// Copyright (C) 2024 Dave Moore
//
// This file is part of Sorcery: Shadows under Llylgamyn.
//
// Sorcery: Shadows under Llylgamyn is free software: you can redistribute it
// and/or modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation, either version 2 of the License,
// or (at your option) any later version.
//
// Sorcery: Shadows under Llylgamyn is distributed in the hope that it will
// be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Sorcery: Shadows under Llylgamyn.  If not, see
// <http://www.gnu.org/licenses/>.
//
// If you modify this Program, or any covered work, by linking or combining it
// with the libraries referred to in README (or a modified version of said
// libraries), containing parts covered by the terms of said libraries, the
// licensors of this Program grant you additional permission to convey the
// resulting work.

#include "gui/attributedisplay.hpp"
#include "core/display.hpp"
#include "core/graphics.hpp"
#include "core/system.hpp"
#include "core/window.hpp"
#include "gui/enum.hpp"
#include "gui/include.hpp"
#include "resources/componentstore.hpp"
#include "resources/iconstore.hpp"
#include "resources/resourcemanager.hpp"
#include "types/character.hpp"

// Standard Constructor
Sorcery::AttributeDisplay::AttributeDisplay(
	System *system, Display *display, Graphics *graphics, Character *character, WAL alignment)
	: _system{system}, _display{display}, _graphics{graphics}, _character{character}, _alignment{alignment} {

	_bars.clear();

	// Get the standard layout information
	if (alignment == WAL::VERTICAL) {
		_bar_c = Component((*_display->layout)["attribute_display:stat_bar_vertical"]);
		_icons_c = Component((*_display->layout)["attribute_display:attribute_icons_vertical"]);
		_text_c = Component((*_display->layout)["attribute_display:stat_text_vertical"]);
	} else {
		_bar_c = Component((*_display->layout)["attribute_display:stat_bar_horizontal"]);
		_icons_c = Component((*_display->layout)["attribute_display:attribute_icons_horizontal"]);
		_text_c = Component((*_display->layout)["attribute_display:stat_text_horizontal"]);
	}

	_attribute_icons[0] = (*_graphics->icons)[MIM::CS_STRENGTH].value();
	_attribute_icons[1] = (*_graphics->icons)[MIM::CS_IQ].value();
	_attribute_icons[2] = (*_graphics->icons)[MIM::CS_PIETY].value();
	_attribute_icons[3] = (*_graphics->icons)[MIM::CS_VITALITY].value();
	_attribute_icons[4] = (*_graphics->icons)[MIM::CS_AGILITY].value();
	_attribute_icons[5] = (*_graphics->icons)[MIM::CS_LUCK].value();

	const sf::Vector2u icon_size{_icons_c.size, _icons_c.size};
	constexpr auto texture_size{511.f};
	sf::Vector2f scale{icon_size.x / texture_size, icon_size.y / texture_size};

	auto index{0};
	for (auto &icon : _attribute_icons) {
		icon.setScale(scale);
		if (_alignment == WAL::VERTICAL)
			icon.setPosition(8 + _icons_c.x + (index * _icons_c.size), _icons_c.y);
		else
			icon.setPosition(_icons_c.x, 8 + _icons_c.y + (index * _icons_c.size));
		++index;
	}

	// Not valid until we call the set command
	valid = false;
}

// Copy Constructors
Sorcery::AttributeDisplay::AttributeDisplay(const AttributeDisplay &other) {

	_system = other._system;
	_display = other._display;
	_graphics = other._graphics;
	_character = other._character;

	_bar_c = other._bar_c;
	_icons_c = other._icons_c;
	_text_c = other._text_c;
	_width = other._width;
	_height = other._height;
	_alignment = other._alignment;

	_bars = other._bars;
	_attribute_icons = other._attribute_icons;
	_texts = other._texts;

	valid = other.valid;
	sprites = other.sprites;
	texts = other.texts;
	frames = other.frames;
}

auto Sorcery::AttributeDisplay::operator=(const AttributeDisplay &other) -> AttributeDisplay & {

	_system = other._system;
	_display = other._display;
	_graphics = other._graphics;
	_character = other._character;
	_bar_c = other._bar_c;
	_icons_c = other._icons_c;
	_text_c = other._text_c;
	_width = other._width;
	_height = other._height;
	_alignment = other._alignment;

	_bars = other._bars;
	_attribute_icons = other._attribute_icons;
	_texts = other._texts;

	valid = other.valid;
	sprites = other.sprites;
	texts = other.texts;
	frames = other.frames;

	return *this;
}

// Move Constructors
Sorcery::AttributeDisplay::AttributeDisplay(AttributeDisplay &&other) noexcept {

	_system = other._system;
	_display = other._display;
	_graphics = other._graphics;
	_character = other._character;

	_bar_c = other._bar_c;
	_icons_c = other._icons_c;
	_text_c = other._text_c;
	_width = other._width;
	_height = other._height;
	_alignment = other._alignment;

	_bars = std::move(other._bars);
	_attribute_icons = std::move(other._attribute_icons);
	_texts = std::move(other._texts);

	valid = other.valid;
	sprites = other.sprites;
	texts = other.texts;
	frames = other.frames;

	other._system = nullptr;
	other._display = nullptr;
	other._graphics = nullptr;
	other._character = nullptr;

	other._bar_c = Component();
	other._icons_c = Component();
	other._text_c = Component();
	other._width = 0;
	other._height = 0;
	other._alignment = WAL::NO_ALIGNMENT;

	other._bars.clear();
	other._attribute_icons = {};
	other._texts.clear();

	other.valid = false;
	other.sprites.clear();
	other.texts.clear();
	other.frames.clear();
}

auto Sorcery::AttributeDisplay::operator=(AttributeDisplay &&other) noexcept -> AttributeDisplay & {

	if (this != &other) {

		_system = other._system;
		_display = other._display;
		_graphics = other._graphics;
		_character = other._character;

		_bar_c = other._bar_c;
		_icons_c = other._icons_c;
		_text_c = other._text_c;
		_width = other._width;
		_height = other._height;
		_alignment = other._alignment;

		_bars = std::move(other._bars);
		_attribute_icons = std::move(other._attribute_icons);
		_texts = std::move(other._texts);

		valid = other.valid;
		sprites = other.sprites;
		texts = other.texts;
		frames = other.frames;

		other._system = nullptr;
		other._display = nullptr;
		other._graphics = nullptr;
		other._character = nullptr;

		other._bar_c = Component();
		other._icons_c = Component();
		other._text_c = Component();
		other._width = 0;
		other._height = 0;
		other._alignment = WAL::NO_ALIGNMENT;

		other._bars.clear();
		other._attribute_icons = {};
		other._texts.clear();

		other.valid = false;
		other.sprites.clear();
		other.texts.clear();
		other.frames.clear();
	}
	return *this;
}

auto Sorcery::AttributeDisplay::set() -> void {

	_bars.clear();

	auto x{0};
	auto y{0};
	for (const auto &[attribute, value] : _character->get_cur_attr()) {

		auto bar{_get_bar(attribute)};
		if (_alignment == WAL::VERTICAL) {
			bar.setPosition((_bar_c.w / 3) + _bar_c.x + (x * _display->window->get_cw() * _bar_c.scale),
				_bar_c.y + _bar_c.size * 2);
			bar.scale(1.0f, -1.0f);
		} else {
			auto text{_get_text(attribute)};
			text.setPosition(_text_c.x + std::stoi(_text_c["offset_x"].value()), _text_c.y + (y * _text_c.size));
			bar.setPosition((_bar_c.x + std::stoi(_bar_c["offset_x"].value())),
				(_bar_c.h / 3) + _bar_c.y + (y * _display->window->get_ch() * _bar_c.scale));
			_texts.push_back(text);
		};

		_bars.push_back(bar);

		++x;
		++y;
	}

	valid = true;
}

auto Sorcery::AttributeDisplay::_get_text(CAR attribute) -> sf::Text {

	if (_alignment == WAL::VERTICAL) {

		sf::Text text{};

		// TODO
		return text;

	} else {
		sf::Text text{};
		const auto formatted_value{fmt::format("{:>2}", _character->get_cur_attr(attribute))};
		text.setFont(_system->resources->fonts[_text_c.font]);
		text.setCharacterSize(_text_c.size);
		text.setFillColor(sf::Color(_text_c.colour));
		text.setString(formatted_value);
		if (_display->get_bold())
			text.setStyle(sf::Text::Bold);

		return text;
	}
}

auto Sorcery::AttributeDisplay::_get_bar(CAR attribute) -> sf::RectangleShape {

	// Generate three bars which will simply be put on top of each other
	if (_alignment == WAL::VERTICAL) {
		sf::RectangleShape attr(sf::Vector2f(_bar_c.w / 2, (_bar_c.h * _character->get_cur_attr(attribute) / 2)));
		attr.setFillColor(sf::Color(_bar_c.colour));
		attr.setOutlineThickness(1);
		return attr;
	} else {
		sf::RectangleShape attr(sf::Vector2f(_bar_c.w * _character->get_cur_attr(attribute) / 2, _bar_c.h / 2));
		attr.setFillColor(sf::Color(_bar_c.colour));
		attr.setOutlineThickness(1);
		return attr;
	}
}

auto Sorcery::AttributeDisplay::draw(sf::RenderTarget &target, sf::RenderStates states) const -> void {

	states.transform *= getTransform();

	for (const auto &bar : _bars)
		target.draw(bar, states);

	for (const auto &icon : _attribute_icons)
		target.draw(icon, states);

	for (const auto &text : _texts)
		target.draw(text, states);
}
