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

#include "frame.hpp"

// Standard Constructor
Sorcery::Frame::Frame(System& system, Display& display, Graphics& graphics, WindowFrameType type,
	const unsigned int width_units, const unsigned int height_units, const unsigned int alpha): _system {system},
	_display {display}, _graphics {graphics}, _type{type}, _width_units{width_units}, _height_units{height_units},
	_alpha {alpha} {

	// Get the Window Pointer
	_window = _display.window->get_window();

	// Define the 8 parts of the Frame based upon the location in the GUI Texture
	if (_type == WindowFrameType::NORMAL) {
		_frame_parts[static_cast<unsigned int>(WindowFrameParts::TOP_LEFT)] = sf::IntRect(0, 550, 20, 20);
		_frame_parts[static_cast<unsigned int>(WindowFrameParts::TOP)] = sf::IntRect(20, 550, 20, 20);
		_frame_parts[static_cast<unsigned int>(WindowFrameParts::TOP_RIGHT)] = sf::IntRect(40, 550, 20, 20);
		_frame_parts[static_cast<unsigned int>(WindowFrameParts::LEFT)] = sf::IntRect(0, 570, 20, 20);
		_frame_parts[static_cast<unsigned int>(WindowFrameParts::BOTTOM_LEFT)] = sf::IntRect(0, 590, 20, 20);
		_frame_parts[static_cast<unsigned int>(WindowFrameParts::BOTTOM)] = sf::IntRect(20, 590, 20, 20);
		_frame_parts[static_cast<unsigned int>(WindowFrameParts::BOTTOM_RIGHT)] = sf::IntRect(40, 590, 20, 20);
		_frame_parts[static_cast<unsigned int>(WindowFrameParts::RIGHT)] = sf::IntRect(40, 570, 20, 20);
	} else if (_type == WindowFrameType::HINT){
		_frame_parts[static_cast<unsigned int>(WindowFrameParts::TOP_LEFT)] = sf::IntRect(865, 399, 18, 18);
		_frame_parts[static_cast<unsigned int>(WindowFrameParts::TOP)] = sf::IntRect(899, 399, 24, 10);
		_frame_parts[static_cast<unsigned int>(WindowFrameParts::TOP_RIGHT)] = sf::IntRect(982, 399, 18, 18);
		_frame_parts[static_cast<unsigned int>(WindowFrameParts::LEFT)] = sf::IntRect(865, 428, 10, 24);
		_frame_parts[static_cast<unsigned int>(WindowFrameParts::BOTTOM_LEFT)] = sf::IntRect(865, 498, 18, 18);
		_frame_parts[static_cast<unsigned int>(WindowFrameParts::BOTTOM)] = sf::IntRect(899, 506, 24, 10);
		_frame_parts[static_cast<unsigned int>(WindowFrameParts::BOTTOM_RIGHT)] = sf::IntRect(982, 498, 18, 18);
		_frame_parts[static_cast<unsigned int>(WindowFrameParts::RIGHT)] = sf::IntRect(989, 428, 10, 24);
	}

	// Get the Frame Components
	unsigned int loop = 0;
	for (auto& frame_sprite : _frame_sprites) {
		frame_sprite = sf::Sprite(_system.resources->textures[UI_TEXTURE]);
		frame_sprite.setTextureRect(_frame_parts[loop]);
		++loop;
	}

	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wreturn-type"

	// Work out total size of texture needed from units
	const unsigned int texture_size_x = [&] {
		if (_type == WindowFrameType::NORMAL)
			return 20 + (20 * width_units) + 20;
		else if (_type == WindowFrameType::HINT)
			return 18 + (24 * width_units) + 18;
	}();
	const unsigned int texture_size_y = [&] {
		if (_type == WindowFrameType::NORMAL)
			return 20 + (20 * height_units) + 20;
		else if (_type == WindowFrameType::HINT)
			return 18 + (24 * height_units) + 18;
	}();
	const sf::Vector2f texture_size(texture_size_x, texture_size_y);
	_render_texture.create(texture_size.x, texture_size.y);

	// Render the background
	const unsigned int border = [&] {
		if (_type == WindowFrameType::NORMAL)
			return 10;
		else if (_type == WindowFrameType::HINT)
			return 5;
	}();
	sf::RectangleShape rectangle(sf::Vector2f(texture_size.x - (border * 2), texture_size.y - (border * 2)));
	rectangle.setFillColor(sf::Color(0, 0, 0, _alpha));
	rectangle.setPosition(border, border);
	_render_texture.draw(rectangle);

	// Draw the Corners of the Frame
	const unsigned int offset = [&] {
		if (_type == WindowFrameType::NORMAL)
			return 20;
		else if (_type == WindowFrameType::HINT)
			return 18;
	}();
	_frame_sprites[static_cast<unsigned int>(WindowFrameParts::TOP_LEFT)].setPosition(0, 0);
	_render_texture.draw(_frame_sprites[static_cast<unsigned int>(WindowFrameParts::TOP_LEFT)]);
	_frame_sprites[static_cast<unsigned int>(WindowFrameParts::TOP_RIGHT)].setPosition(texture_size.x - offset, 0);
	_render_texture.draw(_frame_sprites[static_cast<unsigned int>(WindowFrameParts::TOP_RIGHT)]);
	_frame_sprites[static_cast<unsigned int>(WindowFrameParts::BOTTOM_LEFT)].setPosition(0, texture_size.y - offset);
	_render_texture.draw(_frame_sprites[static_cast<unsigned int>(WindowFrameParts::BOTTOM_LEFT)]);
	_frame_sprites[static_cast<unsigned int>(WindowFrameParts::BOTTOM_RIGHT)].setPosition(texture_size.x - offset,
		texture_size.y - offset);
	_render_texture.draw(_frame_sprites[static_cast<unsigned int>(WindowFrameParts::BOTTOM_RIGHT)]);

	// Draw the Sides of the Frame
	for (unsigned int x = 0; x < width_units; x++) {
		unsigned int x_pos = [&] {
			if (_type == WindowFrameType::NORMAL)
				return 20 + (20 * x);
			else if (_type == WindowFrameType::HINT)
				return 18 + (24 * x);
		}();
		_frame_sprites[static_cast<unsigned int>(WindowFrameParts::TOP)].setPosition(x_pos, 0);
		_render_texture.draw(_frame_sprites[static_cast<unsigned int>(WindowFrameParts::TOP)]);
		_frame_sprites[static_cast<unsigned int>(WindowFrameParts::BOTTOM)].setPosition(x_pos,
			texture_size.y -(border * 2));
		_render_texture.draw(_frame_sprites[static_cast<unsigned int>(WindowFrameParts::BOTTOM)]);
	}
	for (unsigned int y = 0; y < height_units; y++) {
		unsigned int y_pos = [&] {
			if (_type == WindowFrameType::NORMAL)
				return 20 + (20 * y);
			else if (_type == WindowFrameType::HINT)
				return 18 + (24 * y);
		}();
		_frame_sprites[static_cast<unsigned int>(WindowFrameParts::LEFT)].setPosition(0, y_pos);
		_render_texture.draw(_frame_sprites[static_cast<unsigned int>(WindowFrameParts::LEFT)]);
		_frame_sprites[static_cast<unsigned int>(WindowFrameParts::RIGHT)].setPosition(texture_size.x - (border * 2) - 1,
			y_pos);
		_render_texture.draw(_frame_sprites[static_cast<unsigned int>(WindowFrameParts::RIGHT)]);
	}

	#pragma GCC diagnostic pop

	// And draw
	_render_texture.display();
	_texture = _render_texture.getTexture();
	_frame = sf::Sprite(_texture);

	width = _frame.getLocalBounds().width;
	height = _frame.getLocalBounds().height;
	sprite = _frame;
}

// Standard Destructor
Sorcery::Frame::~Frame() {
}

auto Sorcery::Frame::draw(sf::RenderTarget& target, sf::RenderStates states) const -> void {

	states.transform *= getTransform();
	target.draw(_frame, states);
}
