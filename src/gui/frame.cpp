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

#include "gui/frame.hpp"
#include "core/display.hpp"
#include "core/system.hpp"
#include "gui/enum.hpp"

// TODO: need to change this depending on the cell height and cell width

// font is based upon 16x16 so modify this.

// Standard Constructor
Sorcery::Frame::Frame(System *system, Display *display, const Component layout)
	: _system{system}, _display{display}, _layout{layout} {

	// Currently to simplify, we are using the same sized UI grid in the texture as the cell height
	// const auto cw(_display->window->get_cw());
	// const auto ch(_display->window->get_ch());
	const auto cw{24U};
	const auto ch{24U};
	const auto source_top{16U};
	const auto source_size{24U};

	// Define the 8 parts of the Frame based upon the location in the GUI Texture
	_frame_parts[static_cast<unsigned int>(WFP::TOP_LEFT)] = sf::IntRect(0 * source_size, source_top, cw, ch);
	_frame_parts[static_cast<unsigned int>(WFP::TOP)] = sf::IntRect(4 * source_size, source_top, cw, ch);
	_frame_parts[static_cast<unsigned int>(WFP::TOP_RIGHT)] = sf::IntRect(2 * source_size, source_top, cw, ch);
	_frame_parts[static_cast<unsigned int>(WFP::LEFT)] = sf::IntRect(7 * source_size, source_top, cw, ch);
	_frame_parts[static_cast<unsigned int>(WFP::BOTTOM_LEFT)] = sf::IntRect(1 * source_size, source_top, cw, ch);
	_frame_parts[static_cast<unsigned int>(WFP::BOTTOM)] = sf::IntRect(5 * source_size, source_top, cw, ch);
	_frame_parts[static_cast<unsigned int>(WFP::BOTTOM_RIGHT)] = sf::IntRect(3 * source_size, source_top, cw, ch);
	_frame_parts[static_cast<unsigned int>(WFP::RIGHT)] = sf::IntRect(6 * source_size, source_top, cw, ch);

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
	_texture_w = cw * _layout.w;
	_texture_h = ch * _layout.h;
	const sf::Vector2f texture_size(_texture_w, _texture_h);
	_rtexture.create(texture_size.x, texture_size.y);

	// Draw background shape
	sf::RectangleShape rectangle(sf::Vector2f((cw - 2) * _layout.w, (ch - 2) * _layout.h));
	sf::Color fill{sf::Color(_layout.background)};
	rectangle.setFillColor(sf::Color(fill.r, fill.g, fill.b, _layout.alpha));
	rectangle.setPosition(cw, ch);
	_rtexture.draw(rectangle);

	// Draw the Corners of the Frame
	_frame_sprites[static_cast<unsigned int>(WFP::TOP_LEFT)].setPosition(0, 0);
	_rtexture.draw(_frame_sprites[static_cast<unsigned int>(WFP::TOP_LEFT)]);
	_frame_sprites[static_cast<unsigned int>(WFP::TOP_RIGHT)].setPosition(texture_size.x - cw, 0);
	_rtexture.draw(_frame_sprites[static_cast<unsigned int>(WFP::TOP_RIGHT)]);
	_frame_sprites[static_cast<unsigned int>(WFP::BOTTOM_LEFT)].setPosition(0, texture_size.y - ch);
	_rtexture.draw(_frame_sprites[static_cast<unsigned int>(WFP::BOTTOM_LEFT)]);
	_frame_sprites[static_cast<unsigned int>(WFP::BOTTOM_RIGHT)].setPosition(texture_size.x - cw, texture_size.y - ch);
	_rtexture.draw(_frame_sprites[static_cast<unsigned int>(WFP::BOTTOM_RIGHT)]);

	// Draw the Sides of the Frame - note we draw two less since we don't want to overright the corners and we have the
	// offset of cw/ch anyway (so for a frame height of 3 for example, we draw 1 middle side starting at 1)

	// Note in case we want to draw tiny frames in one direction, as a width or height of 1 - 2 gives a
	// massive value and it causes a lockup, so we skip unless we actually have to draw
	if (_layout.w > 2) {
		for (auto x = 0U; x < _layout.w - 2; x++) {
			auto x_pos{cw + (cw * x)};
			_frame_sprites[static_cast<unsigned int>(WFP::TOP)].setPosition(x_pos, 0);
			_rtexture.draw(_frame_sprites[static_cast<unsigned int>(WFP::TOP)]);
			_frame_sprites[static_cast<unsigned int>(WFP::BOTTOM)].setPosition(x_pos, texture_size.y - ch);
			_rtexture.draw(_frame_sprites[static_cast<unsigned int>(WFP::BOTTOM)]);
		}
	}
	if (_layout.h > 2) {
		for (auto y = 0U; y < _layout.h - 2; y++) {
			auto y_pos{ch + (ch * y)};
			_frame_sprites[static_cast<unsigned int>(WFP::LEFT)].setPosition(0, y_pos);
			_rtexture.draw(_frame_sprites[static_cast<unsigned int>(WFP::LEFT)]);
			_frame_sprites[static_cast<unsigned int>(WFP::RIGHT)].setPosition(texture_size.x - cw, y_pos);
			_rtexture.draw(_frame_sprites[static_cast<unsigned int>(WFP::RIGHT)]);
		}
	}

#pragma GCC diagnostic pop

	// And draw
	_rtexture.display();
	_texture = _rtexture.getTexture();
	_frame = sf::Sprite(_texture);

	sprite = _frame;
}

Sorcery::Frame::Frame(sf::Texture texture, const unsigned int width_units, const unsigned int height_units,
	const unsigned long long colour, const unsigned long long bg_colour, const unsigned int alpha)
	: _texture{texture}, _width_units{width_units}, _height_units{height_units}, _colour{colour}, _bg_colour(bg_colour),
	  _alpha{alpha} {

	// Currently to simplify, we are using the same sized UI grid in the texture as the cell height
	// const auto cw(_display->window->get_cw());
	// const auto ch(_display->window->get_ch());
	const auto cw{24U};
	const auto ch{24U};
	const auto source_top{16U};
	const auto source_size{24U};

	// Define the 8 parts of the Frame based upon the location in the GUI Texture
	_frame_parts[static_cast<unsigned int>(WFP::TOP_LEFT)] = sf::IntRect(0 * source_size, source_top, cw, ch);
	_frame_parts[static_cast<unsigned int>(WFP::TOP)] = sf::IntRect(4 * source_size, source_top, cw, ch);
	_frame_parts[static_cast<unsigned int>(WFP::TOP_RIGHT)] = sf::IntRect(2 * source_size, source_top, cw, ch);
	_frame_parts[static_cast<unsigned int>(WFP::LEFT)] = sf::IntRect(7 * source_size, source_top, cw, ch);
	_frame_parts[static_cast<unsigned int>(WFP::BOTTOM_LEFT)] = sf::IntRect(1 * source_size, source_top, cw, ch);
	_frame_parts[static_cast<unsigned int>(WFP::BOTTOM)] = sf::IntRect(5 * source_size, source_top, cw, ch);
	_frame_parts[static_cast<unsigned int>(WFP::BOTTOM_RIGHT)] = sf::IntRect(3 * source_size, source_top, cw, ch);
	_frame_parts[static_cast<unsigned int>(WFP::RIGHT)] = sf::IntRect(6 * source_size, source_top, cw, ch);

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
	_texture_w = cw * _width_units;
	_texture_h = ch * _height_units;
	const sf::Vector2f texture_size(_texture_w, _texture_h);
	_rtexture.create(texture_size.x, texture_size.y);

	// Render the background
	sf::RectangleShape rectangle(sf::Vector2f(texture_size.x, texture_size.y));
	sf::Color fill{sf::Color(bg_colour)};
	rectangle.setFillColor(sf::Color(fill.r, fill.g, fill.b, _alpha));
	rectangle.setPosition(0, 0);
	_rtexture.draw(rectangle);

	// Draw the Corners of the Frame
	_frame_sprites[static_cast<unsigned int>(WFP::TOP_LEFT)].setPosition(0, 0);
	_rtexture.draw(_frame_sprites[static_cast<unsigned int>(WFP::TOP_LEFT)]);
	_frame_sprites[static_cast<unsigned int>(WFP::TOP_RIGHT)].setPosition(texture_size.x - cw, 0);
	_rtexture.draw(_frame_sprites[static_cast<unsigned int>(WFP::TOP_RIGHT)]);
	_frame_sprites[static_cast<unsigned int>(WFP::BOTTOM_LEFT)].setPosition(0, texture_size.y - ch);
	_rtexture.draw(_frame_sprites[static_cast<unsigned int>(WFP::BOTTOM_LEFT)]);
	_frame_sprites[static_cast<unsigned int>(WFP::BOTTOM_RIGHT)].setPosition(texture_size.x - cw, texture_size.y - ch);
	_rtexture.draw(_frame_sprites[static_cast<unsigned int>(WFP::BOTTOM_RIGHT)]);

	// Draw the Sides of the Frame - note we draw two less since we don't want to overright the corners and we have the
	// offset of cw/ch anyway (so for a frame height of 3 for example, we draw 1 middle side starting at 1)

	// Note in case we want to draw tiny frames in one direction, as a width or height of 1 - 2 gives a
	// massive value and it causes a lockup, so we skip unless we actually have to draw
	if (_width_units > 2) {
		for (auto x = 0u; x < _width_units - 2; x++) {
			auto x_pos{cw + (cw * x)};
			_frame_sprites[static_cast<unsigned int>(WFP::TOP)].setPosition(x_pos, 0);
			_rtexture.draw(_frame_sprites[static_cast<unsigned int>(WFP::TOP)]);
			_frame_sprites[static_cast<unsigned int>(WFP::BOTTOM)].setPosition(x_pos, texture_size.y - ch);
			_rtexture.draw(_frame_sprites[static_cast<unsigned int>(WFP::BOTTOM)]);
		}
	}
	if (_height_units > 2) {
		for (auto y = 0u; y < _height_units - 2; y++) {
			auto y_pos{ch + (ch * y)};
			_frame_sprites[static_cast<unsigned int>(WFP::LEFT)].setPosition(0, y_pos);
			_rtexture.draw(_frame_sprites[static_cast<unsigned int>(WFP::LEFT)]);
			_frame_sprites[static_cast<unsigned int>(WFP::RIGHT)].setPosition(texture_size.x - cw, y_pos);
			_rtexture.draw(_frame_sprites[static_cast<unsigned int>(WFP::RIGHT)]);
		}
	}

#pragma GCC diagnostic pop

	// And draw
	_rtexture.display();
	_texture = _rtexture.getTexture();
	_frame = sf::Sprite(_texture);

	sprite = _frame;
}

auto Sorcery::Frame::get_width() const -> unsigned int {

	return _texture_w;
}

auto Sorcery::Frame::get_height() const -> unsigned int {

	return _texture_h;
}

auto Sorcery::Frame::draw(sf::RenderTarget &target, sf::RenderStates states) const -> void {

	states.transform *= getTransform();
	target.draw(_frame, states);
}
