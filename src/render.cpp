// Copyright (C) 2021 Dave Moore
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

#include "render.hpp"

// Standard Constructor
Sorcery::Render::Render(
	System *system, Display *display, Graphics *graphics, Game *game)
	: _system{system}, _display{display}, _graphics{graphics}, _game{game} {

	theta = 0.0f;
	speed = 0.0f;
	turn = 0.0f;

	// Precalculatw the Rays
	tan_FOV = tan(FOV / 2);

	// A ray will be casted for every horizontal pixel
	for (int i = 0; i < MINIMUM_SCREEN_WIDTH; i++) {
		scr_pts[i] = tan_FOV - (2 * tan_FOV * (i + 1)) / MINIMUM_SCREEN_WIDTH;
		distortion[i] = 1.0 / sqrt(1 + scr_pts[i] * scr_pts[i]);
	}

	// Setup the Draw Surface
	_rtexture.create(MINIMUM_SCREEN_WIDTH, MINIMUM_SCREEN_HEIGHT);
	_rtexture.setSmooth(true);
	_rtexture.clear(sf::Color(255, 0, 0, 255));
}
auto Sorcery::Render::update() -> void {

	_rtexture.clear(sf::Color(255, 0, 0, 255));
	_rtexture.display();
	_texture = _rtexture.getTexture();
	_sprite = sf::Sprite(_texture);

	// scale the sprite
	auto current_size{_display->window->size};
	float scale_x{(current_size.width * 1.0f) / _sprite.getLocalBounds().width};
	float scale_y{
		(current_size.height * 1.0f) / _sprite.getLocalBounds().height};
	_sprite.setScale(scale_x, scale_y);
}

auto Sorcery::Render::draw(
	sf::RenderTarget &target, sf::RenderStates states) const -> void {

	states.transform *= getTransform();

	target.draw(_sprite, states);
}
