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

#include "allocatepanel.hpp"

// create a classbar graphic object as part of this? (get the texture of the relevant section of the
// icons texture for all the classes, then set parts of the image - will need a component for this)

Sorcery::AllocatePanel::AllocatePanel(
	System *system, Display *display, Graphics *graphics, Character *character)
	: _system{system}, _display{display}, _graphics{graphics}, _character{character} {

	// Get the standard layout information
	_layout = Component((*_display->layout)["global:allocate_panel"]);
	_c_points_left = Component((*_display->layout)["allocate_panel:to_allocate_number"]);
	_c_points_started = Component((*_display->layout)["allocate_panel:bonus_points_number"]);
	_stat_bar = Component((*_display->layout)["allocate_panel:stat_bar"]);
	_c_allowed_classes = Component((*_display->layout)["allocate_panel:allowed_classes_panel"]);

	// Get the Background Display Components
	_display->generate_components("allocate_panel", sprites, texts, frames);

	// Colors used
	_green = sf::Color(0x169016ff);
	_red = sf::Color(0xff1a1aff);
	_base = sf::Color(0x133201ff);
	_blue = sf::Color(0x4848ffff);
	_outline = sf::Color(0xffffffff);

	// Get and setup Allowed Class Icons
	_class_icons[0] = (*_graphics->icons)[MenuItem::CC_SAMURAI].value();
	_class_icons[1] = (*_graphics->icons)[MenuItem::CC_FIGHTER].value();
	_class_icons[2] = (*_graphics->icons)[MenuItem::CC_LORD].value();
	_class_icons[3] = (*_graphics->icons)[MenuItem::CC_THIEF].value();
	_class_icons[4] = (*_graphics->icons)[MenuItem::CC_NINJA].value();
	_class_icons[5] = (*_graphics->icons)[MenuItem::CC_PRIEST].value();
	_class_icons[6] = (*_graphics->icons)[MenuItem::CC_BISHOP].value();
	_class_icons[7] = (*_graphics->icons)[MenuItem::CC_MAGE].value();
	const sf::Vector2u icon_size{_c_allowed_classes.size, _c_allowed_classes.size};
	float texture_size{
		static_cast<float>(system->resources->textures[GraphicsTexture::ICONS].getSize().y)};
	sf::Vector2f scale{icon_size.x / texture_size, icon_size.y / texture_size};

	int index{0};
	for (auto &icon : _class_icons) {
		icon.setScale(scale);
		icon.setPosition(
			_c_allowed_classes.x + (index * _c_allowed_classes.size), _c_allowed_classes.y);
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

	int x{18};
	int y{0};

	for (const auto &[attribute, value] : _character->get_current_attributes()) {

		// Get the current stat
		sf::Text text;
		text.setFont(_system->resources->fonts[_layout.font]);
		text.setCharacterSize(_layout.size);
		if (value > static_cast<int>(_character->get_starting_attribute(attribute)))
			text.setFillColor(_green);
		else
			text.setFillColor(sf::Color(_layout.colour));
		text.setString(fmt::format("{:>2}", value));
		text.setOrigin(0, text.getLocalBounds().height / 2.0f);
		text.setPosition(x, y * _display->window->get_cell_height());
		_texts.push_back(text);

		// Get the bars (note drawing order!)
		auto [max_bar, allocated_bar, base_bar] = _get_attribute_bar(attribute);
		max_bar.setPosition(x + _layout.size * 2, y * _display->window->get_cell_height());
		max_bar.setOrigin(0, 0 - max_bar.getLocalBounds().height / 2.0f);
		allocated_bar.setPosition(x + _layout.size * 2, y * _display->window->get_cell_height());
		allocated_bar.setOrigin(0, 0 - allocated_bar.getLocalBounds().height / 2.0f);
		base_bar.setPosition(x + _layout.size * 2, y * _display->window->get_cell_height());
		base_bar.setOrigin(0, 0 - base_bar.getLocalBounds().height / 2.0f);
		_bars.push_back(max_bar);
		_bars.push_back(allocated_bar);
		_bars.push_back(base_bar);

		++y;
	}

	sf::Text t_points_left;
	t_points_left.setFont(_system->resources->fonts[_c_points_left.font]);
	t_points_left.setCharacterSize(_c_points_left.size);
	if (_character->get_bonus_points_to_allocate() == _character->get_starting_bonus_points())
		t_points_left.setFillColor(_green);
	else if (_character->get_bonus_points_to_allocate() == 0)
		t_points_left.setFillColor(_red);
	else
		t_points_left.setFillColor(sf::Color(_c_points_left.colour));
	t_points_left.setString(fmt::format("{:>2}", _character->get_bonus_points_to_allocate()));
	t_points_left.setPosition(
		_c_points_left.x, _c_points_left.y + _display->window->get_cell_height());

	_texts.push_back(t_points_left);

	sf::Text t_points_started;
	t_points_started.setFont(_system->resources->fonts[_c_points_started.font]);
	t_points_started.setCharacterSize(_c_points_started.size);
	t_points_started.setFillColor(sf::Color(_c_points_started.colour));
	t_points_started.setString(fmt::format("{:>2}", _character->get_starting_bonus_points()));
	t_points_started.setPosition(
		_c_points_started.x, _c_points_started.y + _display->window->get_cell_height());
	_texts.push_back(t_points_started);

	valid = true;

	_set_allowed_class_icons();
}

auto Sorcery::AllocatePanel::_get_attribute_bar(CharacterAttribute attribute)
	-> std::tuple<sf::RectangleShape, sf::RectangleShape, sf::RectangleShape> {

	// Generate three bars which will simply be put on top of each other
	sf::RectangleShape base(sf::Vector2f(
		(_stat_bar.w * _character->get_starting_attribute(attribute) / 2), _stat_bar.h / 2));
	base.setFillColor(_base);
	base.setOutlineThickness(1);
	sf::RectangleShape allocated(
		sf::Vector2f((_stat_bar.w * _character->get_attribute(attribute)) / 2, _stat_bar.h / 2));
	allocated.setFillColor(_green);
	allocated.setOutlineThickness(1);
	sf::RectangleShape max(sf::Vector2f((_stat_bar.w * 18) / 2, _stat_bar.h / 2));
	max.setFillColor(_blue);
	max.setOutlineThickness(1);

	return std::make_tuple(max, allocated, base);
}

auto Sorcery::AllocatePanel::_set_allowed_class_icons() -> void {

	for (auto &icon : _class_icons)
		icon.setColor(_red);

	auto possible_classes = _character->get_possible_classes();
	if (possible_classes[CharacterClass::SAMURAI])
		_class_icons[0].setColor(_green);
	if (possible_classes[CharacterClass::FIGHTER])
		_class_icons[1].setColor(_green);
	if (possible_classes[CharacterClass::LORD])
		_class_icons[2].setColor(_green);
	if (possible_classes[CharacterClass::THIEF])
		_class_icons[3].setColor(_green);
	if (possible_classes[CharacterClass::NINJA])
		_class_icons[4].setColor(_green);
	if (possible_classes[CharacterClass::PRIEST])
		_class_icons[5].setColor(_green);
	if (possible_classes[CharacterClass::BISHOP])
		_class_icons[6].setColor(_green);
	if (possible_classes[CharacterClass::MAGE])
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