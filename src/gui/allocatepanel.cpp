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

Sorcery::AllocatePanel::AllocatePanel(System *system, Display *display, Graphics *graphics, Character *character)
	: _system{system}, _display{display}, _graphics{graphics}, _character{character} {

	using enum Enums::Menu::Item;

	// Get the standard layout information
	_layout = Component((*_display->layout)["global:allocate_panel"]);
	_c_points_left = Component((*_display->layout)["allocate_panel:to_allocate_number"]);
	_c_points_started = Component((*_display->layout)["allocate_panel:bonus_points_number"]);
	_stat_bar = Component((*_display->layout)["allocate_panel:stat_bar"]);
	_c_allowed_classes = Component((*_display->layout)["allocate_panel:allowed_classes_panel"]);

	// Get the Background Display Components
	_display->generate("allocate_panel", sprites, texts, frames);

	// Colors used
	_green = sf::Color(0x169016ff);
	_red = sf::Color(0xff1a1aff);
	_base = sf::Color(0x133201ff);
	_blue = sf::Color(0x4848ffff);

	// Get and setup Allowed Class Icons
	_class_icons[0] = (*_graphics->icons)[CC_SAMURAI].value();
	_class_icons[1] = (*_graphics->icons)[CC_FIGHTER].value();
	_class_icons[2] = (*_graphics->icons)[CC_LORD].value();
	_class_icons[3] = (*_graphics->icons)[CC_THIEF].value();
	_class_icons[4] = (*_graphics->icons)[CC_NINJA].value();
	_class_icons[5] = (*_graphics->icons)[CC_PRIEST].value();
	_class_icons[6] = (*_graphics->icons)[CC_BISHOP].value();
	_class_icons[7] = (*_graphics->icons)[CC_MAGE].value();
	const sf::Vector2u icon_size{_c_allowed_classes.size, _c_allowed_classes.size};
	constexpr auto texture_size{511.f};
	sf::Vector2f scale{icon_size.x / texture_size, icon_size.y / texture_size};

	auto index{0};
	auto pos_x{_c_allowed_classes.x};
	auto pos_y{_c_allowed_classes.y + 8};
	for (auto &icon : _class_icons) {
		icon.setScale(scale);
		icon.setPosition(pos_x, pos_y);
		if (index == 3) {
			pos_x += _c_allowed_classes.size;
			pos_y = _c_allowed_classes.y + 8;
		} else
			pos_y += _c_allowed_classes.size;
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
		text.setFillColor(sf::Color(_graphics->adjust_colour(value, CharacterAbilityType::STAT)));
		text.setString(fmt::format("{:>2}", value));
		// text.setOrigin(0, text.getLocalBounds().height / 2.0f);
		text.setPosition(x, (y * _display->window->get_ch()));
		if (_display->get_bold())
			text.setStyle(sf::Text::Bold);
		_texts.push_back(text);

		// Get the bars (note drawing order!)
		auto [max_bar, allocated_bar, base_bar] = _get_bar(attribute);
		max_bar.setPosition(x + _layout.size * 2, y * _display->window->get_ch());
		max_bar.setOrigin(0, 0 - max_bar.getLocalBounds().height / 2.0f);
		allocated_bar.setPosition(x + _layout.size * 2, y * _display->window->get_ch());
		allocated_bar.setOrigin(0, 0 - allocated_bar.getLocalBounds().height / 2.0f);
		base_bar.setPosition(x + _layout.size * 2, y * _display->window->get_ch());
		base_bar.setOrigin(0, 0 - base_bar.getLocalBounds().height / 2.0f);
		_bars.push_back(max_bar);
		_bars.push_back(allocated_bar);
		_bars.push_back(base_bar);

		++y;
	}

	sf::Text t_points_left{};
	t_points_left.setFont(_system->resources->fonts[_c_points_left.font]);
	t_points_left.setCharacterSize(_c_points_left.size);
	t_points_left.setFillColor(sf::Color(_c_points_left.colour));
	t_points_left.setString(fmt::format("{:>2}", _character->get_points_left()));
	t_points_left.setPosition(_c_points_left.x - 4, _c_points_left.y);

	_texts.push_back(t_points_left);

	valid = true;

	_set_icons();
}

auto Sorcery::AllocatePanel::_get_bar(
	CharacterAttribute attribute) const -> std::tuple<sf::RectangleShape, sf::RectangleShape, sf::RectangleShape> {

	// Generate three bars which will simply be put on top of each other
	sf::RectangleShape base(sf::Vector2f((_stat_bar.w * _character->get_start_attr(attribute) / 2), _stat_bar.h / 2));
	base.setFillColor(_base);
	base.setOutlineThickness(1);
	sf::RectangleShape allocated(
		sf::Vector2f((_stat_bar.w * _character->get_cur_attr(attribute)) / 2, _stat_bar.h / 2));
	allocated.setFillColor(_green);
	allocated.setOutlineThickness(1);
	sf::RectangleShape max(sf::Vector2f((_stat_bar.w * 18) / 2, _stat_bar.h / 2));
	max.setFillColor(_blue);
	max.setOutlineThickness(1);

	return std::make_tuple(max, allocated, base);
}

auto Sorcery::AllocatePanel::_set_icons() -> void {

	using enum Enums::Character::Class;

	for (auto &icon : _class_icons)
		icon.setColor(_red);

	auto possible_classes = _character->get_pos_class();
	if (possible_classes[SAMURAI])
		_class_icons[0].setColor(_green);
	if (possible_classes[FIGHTER])
		_class_icons[1].setColor(_green);
	if (possible_classes[LORD])
		_class_icons[2].setColor(_green);
	if (possible_classes[THIEF])
		_class_icons[3].setColor(_green);
	if (possible_classes[NINJA])
		_class_icons[4].setColor(_green);
	if (possible_classes[PRIEST])
		_class_icons[5].setColor(_green);
	if (possible_classes[BISHOP])
		_class_icons[6].setColor(_green);
	if (possible_classes[MAGE])
		_class_icons[7].setColor(_green);
}

auto Sorcery::AllocatePanel::draw(sf::RenderTarget &target, sf::RenderStates states) const -> void {

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

	for (const auto &icon : _class_icons)
		target.draw(icon, states);
}
