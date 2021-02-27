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

#include "attractmode.hpp"

// Standard Constructor
Sorcery::AttractMode::AttractMode(sf::Texture creatures_texture, Component component)
	: _creatures_texture{creatures_texture}, _component{component} {

	_creature_sprite_width = 108;
	_creature_sprite_height = 108;
	_creature_sprite_spacing = 8;
}

// We generate the attract mode graphic in the main thread, though we generate the IDs in the animation threads
// https://en.sfml-dev.org/forums/index.php?topic=18672.0
auto Sorcery::AttractMode::generate() -> void {

	// Only regenerate if we have a change
	if (data != data_temp) {
		data = data_temp;

		// Work out the new size
		const unsigned int number_to_display{static_cast<unsigned int>(data_temp.size())};
		const sf::Vector2f texture_size(
			_creature_sprite_width * number_to_display + (_creature_sprite_spacing * (number_to_display - 1)),
			_creature_sprite_height);

		// Don't worry about previous contents of this
		_render_texture.create(texture_size.x, texture_size.y);
		_render_texture.setSmooth(true);
		_render_texture.clear();

		// Get the Required Sprites
		unsigned int sprite_x{0};
		for (auto i : data) {
			sf::Sprite sprite = _get_creature_gfx(i, true);
			sprite.setPosition(sprite_x, 0);
			_render_texture.draw(sprite, sf::BlendAlpha);
			sprite_x += (_creature_sprite_width + _creature_sprite_spacing);
		}

		// Draw them to the Texture
		_render_texture.display();
		_texture = _render_texture.getTexture();
		_sprite = sf::Sprite(_texture);
		_sprite.setColor(sf::Color(0, 0, 0, 175));
		sprite = _sprite;
	}
}

// Get the Sprite from the Creatures Texture
auto Sorcery::AttractMode::_get_creature_gfx(const int creature_id, const bool known) -> sf::Sprite {

	sf::IntRect creature_rect{};
	sf::Sprite creature(_creatures_texture);
	creature_rect.left = (creature_id - 1) * _creature_sprite_width;
	creature_rect.width = _creature_sprite_width;
	creature_rect.top = known ? 0 : _creature_sprite_height;
	creature_rect.height = _creature_sprite_height;
	creature.setTextureRect(creature_rect);
	return creature;
}

// Adjust the Alpha
auto Sorcery::AttractMode::set_alpha(unsigned int alpha) -> void {

	if (data_temp.size() > 0)
		_sprite.setColor(sf::Color(255, 255, 255, alpha));
}

auto Sorcery::AttractMode::draw(sf::RenderTarget &target, sf::RenderStates states) const -> void {

	if (data_temp.size() > 0) {
		states.transform *= getTransform();
		target.draw(_sprite, states);
	}
}
