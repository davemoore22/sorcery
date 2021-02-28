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

#include "statusbar.hpp"

// Standard Constructor
Sorcery::StatusBar::StatusBar(System &system, Display &display, Graphics &graphics)
	: _system{system}, _display{display}, _graphics{graphics} {

	// Get any Layout Information
	_layout = Component((*_display.layout)["status_bar:status_bar"]);
	_frame_c = Component((*_display.layout)["status_bar:outer_frame"]);

	_render_texture.create(
		_layout.w * _display.window->get_cell_width(), _layout.h * _display.window->get_cell_height());
	_render_texture.setSmooth(true);
	_render_texture.clear();

	// Create the Outside Fram
	_frame =
		std::make_unique<Frame>(_display.ui_texture, WindowFrameType::NORMAL, _frame_c.w, _frame_c.h, _frame_c.alpha);

	// Render the background (inset by the frame)
	sf::RectangleShape rectangle(sf::Vector2f((_display.window->get_cell_width() * (_layout.w)) - 20,
		(_display.window->get_cell_height() * (_layout.h)) - 20));
	rectangle.setFillColor(sf::Color(0, 0, 0, _layout.alpha));
	rectangle.setPosition(10, 10);
	_render_texture.draw(rectangle);

	_frame_sprite = _frame->sprite;
	_frame_sprite.setPosition(0, 0);
	_render_texture.draw(_frame_sprite);

	// Normally we'd be passed in a vector of character shared ptrs, and draw each one (in a seperate update method?)
	// but for now, just do this
	_texture = _render_texture.getTexture();
	sprite = sf::Sprite(_texture);

	width = sprite.getLocalBounds().width;
	height = sprite.getLocalBounds().height;
}

auto Sorcery::StatusBar::draw(sf::RenderTarget &target, sf::RenderStates states) const -> void {

	states.transform *= getTransform();
	target.draw(sprite, states);
}
