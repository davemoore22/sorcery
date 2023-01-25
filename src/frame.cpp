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

#include "frame.hpp"

// TODO: need to change this depending on the cell height and cell widtg

// Standard Constructor
Sorcery::Frame::Frame(System *system, Display *display, WindowFrameType type, const Component layout)
	: _system{system}, _display{display}, _type{type}, _layout{layout} {

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
	} else if (_type == WindowFrameType::HINT) {
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
	auto loop{0};
	for (auto &frame_sprite : _frame_sprites) {
		frame_sprite = sf::Sprite(_display->ui_texture);
		frame_sprite.setTextureRect(_frame_parts[loop]);
		if (_colour != 0ULL)
			frame_sprite.setColor(sf::Color(_colour));
		++loop;
	}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"

	// Work out total size of texture needed from units
	_texture_w = [&] {
		if (_type == WindowFrameType::NORMAL)
			return 20 + (20 * _layout.w) + 20;
		else if (_type == WindowFrameType::HINT)
			return 18 + (24 * _layout.w) + 18;
	}();
	_texture_h = [&] {
		if (_type == WindowFrameType::NORMAL)
			return 20 + (20 * _layout.h) + 20;
		else if (_type == WindowFrameType::HINT)
			return 18 + (24 * _layout.h) + 18;
	}();
	const sf::Vector2f texture_size(_texture_w, _texture_h);
	_rtexture.create(texture_size.x, texture_size.y);

	// Render the background
	const auto border{[&] {
		if (_type == WindowFrameType::NORMAL)
			return 10u;
		else if (_type == WindowFrameType::HINT)
			return 5u;
	}()};
	sf::RectangleShape rectangle(sf::Vector2f(texture_size.x - (border * 2), texture_size.y - (border * 2)));
	sf::Color fill{sf::Color(_layout.background)};
	rectangle.setFillColor(sf::Color(fill.r, fill.g, fill.b, _layout.alpha));
	rectangle.setPosition(border, border);
	_rtexture.draw(rectangle);

	// Draw the Corners of the Frame
	const auto offset{[&] {
		if (_type == WindowFrameType::NORMAL)
			return 20u;
		else if (_type == WindowFrameType::HINT)
			return 18u;
	}()};
	_frame_sprites[static_cast<unsigned int>(WindowFrameParts::TOP_LEFT)].setPosition(0, 0);
	_rtexture.draw(_frame_sprites[static_cast<unsigned int>(WindowFrameParts::TOP_LEFT)]);
	_frame_sprites[static_cast<unsigned int>(WindowFrameParts::TOP_RIGHT)].setPosition(texture_size.x - offset, 0);
	_rtexture.draw(_frame_sprites[static_cast<unsigned int>(WindowFrameParts::TOP_RIGHT)]);
	_frame_sprites[static_cast<unsigned int>(WindowFrameParts::BOTTOM_LEFT)].setPosition(0, texture_size.y - offset);
	_rtexture.draw(_frame_sprites[static_cast<unsigned int>(WindowFrameParts::BOTTOM_LEFT)]);
	_frame_sprites[static_cast<unsigned int>(WindowFrameParts::BOTTOM_RIGHT)].setPosition(
		texture_size.x - offset, texture_size.y - offset);
	_rtexture.draw(_frame_sprites[static_cast<unsigned int>(WindowFrameParts::BOTTOM_RIGHT)]);

	// Draw the Sides of the Frame
	for (auto x = 0u; x < _layout.w; x++) {
		auto x_pos{[&] {
			if (_type == WindowFrameType::NORMAL)
				return 20 + (20 * x);
			else if (_type == WindowFrameType::HINT)
				return 18 + (24 * x);
		}()};
		_frame_sprites[static_cast<unsigned int>(WindowFrameParts::TOP)].setPosition(x_pos, 0);
		_rtexture.draw(_frame_sprites[static_cast<unsigned int>(WindowFrameParts::TOP)]);
		_frame_sprites[static_cast<unsigned int>(WindowFrameParts::BOTTOM)].setPosition(
			x_pos, texture_size.y - (border * 2));
		_rtexture.draw(_frame_sprites[static_cast<unsigned int>(WindowFrameParts::BOTTOM)]);
	}
	for (auto y = 0u; y < _layout.h; y++) {
		auto y_pos{[&] {
			if (_type == WindowFrameType::NORMAL)
				return 20 + (20 * y);
			else if (_type == WindowFrameType::HINT)
				return 18 + (24 * y);
		}()};
		_frame_sprites[static_cast<unsigned int>(WindowFrameParts::LEFT)].setPosition(0, y_pos);
		_rtexture.draw(_frame_sprites[static_cast<unsigned int>(WindowFrameParts::LEFT)]);
		_frame_sprites[static_cast<unsigned int>(WindowFrameParts::RIGHT)].setPosition(
			texture_size.x - (border * 2) - 1, y_pos);
		_rtexture.draw(_frame_sprites[static_cast<unsigned int>(WindowFrameParts::RIGHT)]);
	}

#pragma GCC diagnostic pop

	// And draw
	_rtexture.display();
	_texture = _rtexture.getTexture();
	_frame = sf::Sprite(_texture);

	width = _frame.getLocalBounds().width;
	height = _frame.getLocalBounds().height;
	sprite = _frame;
}

Sorcery::Frame::Frame(sf::Texture texture, WindowFrameType type, const unsigned int width_units,
	const unsigned int height_units, const unsigned long long colour, const unsigned long long bg_colour,
	const unsigned int alpha)
	: _texture{texture}, _type{type}, _width_units{width_units}, _height_units{height_units}, _colour{colour},
	  _bg_colour(bg_colour), _alpha{alpha} {

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
	} else if (_type == WindowFrameType::HINT) {
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
	auto loop{0};
	for (auto &frame_sprite : _frame_sprites) {
		frame_sprite = sf::Sprite(_texture);
		frame_sprite.setTextureRect(_frame_parts[loop]);
		if (_colour != 0ULL)
			frame_sprite.setColor(sf::Color(_colour));
		++loop;
	}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-type"

	// Work out total size of texture needed from units
	_texture_w = [&] {
		if (_type == WindowFrameType::NORMAL)
			return 20 + (20 * width_units) + 20;
		else if (_type == WindowFrameType::HINT)
			return 18 + (24 * width_units) + 18;
	}();
	_texture_h = [&] {
		if (_type == WindowFrameType::NORMAL)
			return 20 + (20 * height_units) + 20;
		else if (_type == WindowFrameType::HINT)
			return 18 + (24 * height_units) + 18;
	}();
	const sf::Vector2f texture_size(_texture_w, _texture_h);
	_rtexture.create(texture_size.x, texture_size.y);

	// Render the background
	const auto border{[&] {
		if (_type == WindowFrameType::NORMAL)
			return 10u;
		else if (_type == WindowFrameType::HINT)
			return 5u;
	}()};
	sf::RectangleShape rectangle(sf::Vector2f(texture_size.x - (border * 2), texture_size.y - (border * 2)));
	sf::Color fill{sf::Color(bg_colour)};
	rectangle.setFillColor(sf::Color(fill.r, fill.g, fill.b, _alpha));
	rectangle.setPosition(border, border);
	_rtexture.draw(rectangle);

	// Draw the Corners of the Frame
	const auto offset{[&] {
		if (_type == WindowFrameType::NORMAL)
			return 20u;
		else if (_type == WindowFrameType::HINT)
			return 18u;
	}()};
	_frame_sprites[static_cast<unsigned int>(WindowFrameParts::TOP_LEFT)].setPosition(0, 0);
	_rtexture.draw(_frame_sprites[static_cast<unsigned int>(WindowFrameParts::TOP_LEFT)]);
	_frame_sprites[static_cast<unsigned int>(WindowFrameParts::TOP_RIGHT)].setPosition(texture_size.x - offset, 0);
	_rtexture.draw(_frame_sprites[static_cast<unsigned int>(WindowFrameParts::TOP_RIGHT)]);
	_frame_sprites[static_cast<unsigned int>(WindowFrameParts::BOTTOM_LEFT)].setPosition(0, texture_size.y - offset);
	_rtexture.draw(_frame_sprites[static_cast<unsigned int>(WindowFrameParts::BOTTOM_LEFT)]);
	_frame_sprites[static_cast<unsigned int>(WindowFrameParts::BOTTOM_RIGHT)].setPosition(
		texture_size.x - offset, texture_size.y - offset);
	_rtexture.draw(_frame_sprites[static_cast<unsigned int>(WindowFrameParts::BOTTOM_RIGHT)]);

	// Draw the Sides of the Frame
	for (auto x = 0u; x < width_units; x++) {
		auto x_pos{[&] {
			if (_type == WindowFrameType::NORMAL)
				return 20 + (20 * x);
			else if (_type == WindowFrameType::HINT)
				return 18 + (24 * x);
		}()};
		_frame_sprites[static_cast<unsigned int>(WindowFrameParts::TOP)].setPosition(x_pos, 0);
		_rtexture.draw(_frame_sprites[static_cast<unsigned int>(WindowFrameParts::TOP)]);
		_frame_sprites[static_cast<unsigned int>(WindowFrameParts::BOTTOM)].setPosition(
			x_pos, texture_size.y - (border * 2));
		_rtexture.draw(_frame_sprites[static_cast<unsigned int>(WindowFrameParts::BOTTOM)]);
	}
	for (auto y = 0u; y < height_units; y++) {
		auto y_pos{[&] {
			if (_type == WindowFrameType::NORMAL)
				return 20 + (20 * y);
			else if (_type == WindowFrameType::HINT)
				return 18 + (24 * y);
		}()};
		_frame_sprites[static_cast<unsigned int>(WindowFrameParts::LEFT)].setPosition(0, y_pos);
		_rtexture.draw(_frame_sprites[static_cast<unsigned int>(WindowFrameParts::LEFT)]);
		_frame_sprites[static_cast<unsigned int>(WindowFrameParts::RIGHT)].setPosition(
			texture_size.x - (border * 2) - 1, y_pos);
		_rtexture.draw(_frame_sprites[static_cast<unsigned int>(WindowFrameParts::RIGHT)]);
	}

#pragma GCC diagnostic pop

	// And draw
	_rtexture.display();
	_texture = _rtexture.getTexture();
	_frame = sf::Sprite(_texture);

	width = _frame.getLocalBounds().width;
	height = _frame.getLocalBounds().height;
	sprite = _frame;
}

auto Sorcery::Frame::draw(sf::RenderTarget &target, sf::RenderStates states) const -> void {

	states.transform *= getTransform();
	target.draw(_frame, states);
}
