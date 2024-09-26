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

#include "gui/allocatepanel.hpp"
#include "core/display.hpp"
#include "core/graphics.hpp"
#include "core/system.hpp"
#include "core/window.hpp"
#include "gui/frame.hpp"
#include "resources/componentstore.hpp"
#include "resources/iconstore.hpp"
#include "resources/resourcemanager.hpp"
#include "types/character.hpp"

Sorcery::AllocatePanel::AllocatePanel(
	System *system, Display *display, Graphics *graphics, Character *character)
	: _system{system}, _display{display}, _graphics{graphics},
	  _character{character} {

	// Get the standard layout information
	_layout = Component((*_display->layout)["global:allocate_panel"]);
	_c_left =
		Component((*_display->layout)["allocate_panel:to_allocate_number"]);
	_c_start =
		Component((*_display->layout)["allocate_panel:bonus_points_number"]);
	_stat_bar = Component((*_display->layout)["allocate_panel:stat_bar"]);
	_c_allowed =
		Component((*_display->layout)["allocate_panel:allowed_classes_panel"]);

	// Get the Background Display Components
	_display->generate("allocate_panel", sprites, texts, frames);

	// Colors used (TODO: change this)
	_green = sf::Color(0x169016ff);
	_red = sf::Color(0xff1a1aff);
	_base = sf::Color(0x133201ff);
	_blue = sf::Color(0x4848ffff);

	// Get and setup Allowed Class Icons
	_icons[0] = (*_graphics->icons)[MIM::CC_SAMURAI].value();
	_icons[1] = (*_graphics->icons)[MIM::CC_FIGHTER].value();
	_icons[2] = (*_graphics->icons)[MIM::CC_LORD].value();
	_icons[3] = (*_graphics->icons)[MIM::CC_THIEF].value();
	_icons[4] = (*_graphics->icons)[MIM::CC_NINJA].value();
	_icons[5] = (*_graphics->icons)[MIM::CC_PRIEST].value();
	_icons[6] = (*_graphics->icons)[MIM::CC_BISHOP].value();
	_icons[7] = (*_graphics->icons)[MIM::CC_MAGE].value();
	const sf::Vector2u icon_size{_c_allowed.size, _c_allowed.size};
	constexpr auto texture_size{511.f};
	sf::Vector2f scale{icon_size.x / texture_size, icon_size.y / texture_size};

	auto index{0};
	auto pos_x{_c_allowed.x};
	auto pos_y{_c_allowed.y + 8};
	for (auto &icon : _icons) {
		icon.setScale(scale);
		icon.setPosition(pos_x, pos_y);
		if (index == 3) {
			pos_x += _c_allowed.size;
			pos_y = _c_allowed.y + 8;
		} else
			pos_y += _c_allowed.size;
		++index;
	}

	// Not valid until we call the set command
	valid = false;
	_width = 0;
	_height = 0;
}

auto Sorcery::AllocatePanel::set() -> void {

	_texts.clear();
	_bars.clear();

	auto x{14};
	auto y{0};

	for (const auto &[attribute, value] : _character->get_cur_attr()) {

		// Get the current stat
		sf::Text text{};
		text.setFont(_system->resources->fonts[_layout.font]);
		text.setCharacterSize(_layout.size);
		text.setFillColor(
			sf::Color(_graphics->adjust_colour(value, CAT::STAT)));
		text.setString(fmt::format("{:>2}", value));
		// text.setOrigin(0, text.getLocalBounds().height / 2.0f);
		text.setPosition(x, (y * _display->window->get_ch()));
		if (_display->get_bold())
			text.setStyle(sf::Text::Bold);
		_texts.push_back(text);

		// Get the bars (note drawing order!)
		auto [max, alloc, base] = _get_bar(attribute);
		max.setPosition(x + _layout.size * 2, y * _display->window->get_ch());
		max.setOrigin(0, 0 - max.getLocalBounds().height / 2.0f);
		alloc.setPosition(x + _layout.size * 2, y * _display->window->get_ch());
		alloc.setOrigin(0, 0 - alloc.getLocalBounds().height / 2.0f);
		base.setPosition(x + _layout.size * 2, y * _display->window->get_ch());
		base.setOrigin(0, 0 - base.getLocalBounds().height / 2.0f);
		_bars.push_back(max);
		_bars.push_back(alloc);
		_bars.push_back(base);

		++y;
	}

	// Show Points Left
	sf::Text left{};
	left.setFont(_system->resources->fonts[_c_left.font]);
	left.setCharacterSize(_c_left.size);
	left.setFillColor(sf::Color(_c_left.colour));
	left.setString(fmt::format("{:>2}", _character->get_points_left()));
	left.setPosition(_c_left.x - 4, _c_left.y);

	_texts.push_back(left);

	valid = true;

	_set_icons();
}

auto Sorcery::AllocatePanel::_get_bar(CAR attribute) const
	-> std::tuple<sf::RectangleShape, sf::RectangleShape, sf::RectangleShape> {

	// Generate three bars which will simply be put on top of each other
	sf::RectangleShape base(
		sf::Vector2f((_stat_bar.w * _character->get_start_attr(attribute) / 2),
			_stat_bar.h / 2));
	base.setFillColor(_base);
	base.setOutlineThickness(1);
	sf::RectangleShape alloc(
		sf::Vector2f((_stat_bar.w * _character->get_cur_attr(attribute)) / 2,
			_stat_bar.h / 2));
	alloc.setFillColor(_green);
	alloc.setOutlineThickness(1);
	sf::RectangleShape max(
		sf::Vector2f((_stat_bar.w * 18) / 2, _stat_bar.h / 2));
	max.setFillColor(_blue);
	max.setOutlineThickness(1);

	return std::make_tuple(max, alloc, base);
}

auto Sorcery::AllocatePanel::_set_icons() -> void {

	for (auto &icon : _icons)
		icon.setColor(_red);

	auto classes = _character->get_pos_class();
	if (classes[CHC::SAMURAI])
		_icons[0].setColor(_green);
	if (classes[CHC::FIGHTER])
		_icons[1].setColor(_green);
	if (classes[CHC::LORD])
		_icons[2].setColor(_green);
	if (classes[CHC::THIEF])
		_icons[3].setColor(_green);
	if (classes[CHC::NINJA])
		_icons[4].setColor(_green);
	if (classes[CHC::PRIEST])
		_icons[5].setColor(_green);
	if (classes[CHC::BISHOP])
		_icons[6].setColor(_green);
	if (classes[CHC::MAGE])
		_icons[7].setColor(_green);
}

auto Sorcery::AllocatePanel::draw(
	sf::RenderTarget &target, sf::RenderStates states) const -> void {

	states.transform *= getTransform();

	// Draw the standard components
	for (const auto &[unique_key, frame] : frames)
		target.draw(*frame, states);

	for (const auto &[unique_key, sprite] : sprites)
		target.draw(sprite, states);

	for (const auto &[unique_key, text] : texts)
		target.draw(text, states);

	// Draw the custom components
	for (const auto &text : _texts)
		target.draw(text, states);

	for (const auto &bar : _bars)
		target.draw(bar, states);

	for (const auto &icon : _icons)
		target.draw(icon, states);
}
