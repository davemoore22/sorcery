// Copyright (C) 2020 Dave Moore
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

#include "confirm.hpp"

// Standard Constructor
Sorcery::Confirm::Confirm(
	System &system, Display &display, Graphics &graphics, Component &gui_component, Component &text_component)
	: _system{system}, _display{display}, _graphics{graphics}, _gui_component{gui_component}, _text_component{
																								  text_component} {

	_window = _display.window->get_window();
	currently_highlighted = WindowConfirm::NO;
}

auto Sorcery::Confirm::check_for_mouse_move(sf::Vector2f mouse_position) -> std::optional<WindowConfirm> {

	if (_yes_background_rect.contains(mouse_position)) {
		currently_highlighted = WindowConfirm::YES;
		return currently_highlighted;
	} else if (_no_background_rect.contains(mouse_position)) {
		currently_highlighted = WindowConfirm::NO;
		return currently_highlighted;
	} else
		return std::nullopt;
}

// Only works for the Mouse
auto Sorcery::Confirm::check_if_option_selected(sf::Vector2f mouse_position) -> std::optional<WindowConfirm> {

	if (_yes_background_rect.contains(mouse_position)) {
		currently_highlighted = WindowConfirm::YES;
		return currently_highlighted;
	} else if (_no_background_rect.contains(mouse_position)) {
		currently_highlighted = WindowConfirm::NO;
		return currently_highlighted;
	} else
		return std::nullopt;
}

auto Sorcery::Confirm::toggle_highlighted() -> WindowConfirm {

	if (currently_highlighted == WindowConfirm::YES)
		currently_highlighted = WindowConfirm::NO;
	else if (currently_highlighted == WindowConfirm::NO)
		currently_highlighted = WindowConfirm::YES;
	return currently_highlighted;
}

auto Sorcery::Confirm::draw(double lerp) -> void {

	// Generate back frame
	_frame = std::make_unique<Frame>(
		_display.ui_texture, WindowFrameType::NORMAL, _gui_component.w, _gui_component.h, _gui_component.alpha);
	_frame->setPosition(_display.window->get_x(_frame->sprite, _gui_component.x),
		_display.window->get_y(_frame->sprite, _gui_component.y));
	_window->draw(*_frame);

	// Display Confirmation Message
	sf::Text text{};
	_display.window->draw_text(text, _text_component);

	// Draw Yes / No (and highlight them depending on which one chosen)
	unsigned int yes_no_y{_text_component.y + (_display.window->get_cell_height() * 2)};
	unsigned int yes_x{_display.window->centre.x - (_display.window->get_cell_width() * 4)};
	unsigned int no_x{_display.window->centre.x + (_display.window->get_cell_width() * 2)};

	_yes_text.setFont(_system.resources->fonts[_text_component.font]);
	_yes_text.setCharacterSize(_text_component.size);
	_yes_text.setFillColor(sf::Color(_text_component.colour));
	_yes_text.setString("YES");
	_yes_text.setPosition(yes_x, yes_no_y);
	//yes_text.setOrigin(yes_text.getLocalBounds().width / 2.0f, yes_text.getLocalBounds().height / 2.0f);
	_no_text.setFont(_system.resources->fonts[_text_component.font]);
	_no_text.setCharacterSize(_text_component.size);
	_no_text.setFillColor(sf::Color(_text_component.colour));
	_no_text.setString("NO");
	_no_text.setPosition(no_x, yes_no_y);
	//no_text.setOrigin(no_text.getLocalBounds().width / 2.0f, no_text.getLocalBounds().height / 2.0f);

	// Draw backgrounds
	if (currently_highlighted == WindowConfirm::YES) {
		sf::RectangleShape yes_background{_display.window->highlight_text(_yes_text, _text_component, lerp)};
		_window->draw(yes_background, _yes_text.getTransform());
	} else if (currently_highlighted == WindowConfirm::NO) {
		sf::RectangleShape no_background{_display.window->highlight_text(_no_text, _text_component, lerp)};
		_window->draw(no_background, _no_text.getTransform());
	}

	_yes_background_rect = _yes_text.getGlobalBounds();
	_no_background_rect = _no_text.getGlobalBounds();
	_window->draw(_yes_text);
	_window->draw(_no_text);
}
