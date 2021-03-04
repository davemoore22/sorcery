// Copyright (C) 2021 Dave Moore
//
// This file is part of Sorcery: Dreams of the Mad Overlord.
//
// Sorcery: Dreams of the Mad Overlord is free software: you can redistribute
// it and/or modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation, either version 2 of the License,
// or (at your option) any later version.
//
// Sorcery: Dreams of the Mad Overlord is distributed in the hope that it wil
// be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
// Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Sorcery: Dreams of the Mad Overlord.  If not,
// see <http://www.gnu.org/licenses/>.
//
// If you modify this program, or any covered work, by linking or combining
// it with the libraries referred to in README (or a modified version of
// said libraries), containing parts covered by the terms of said libraries,
// the licensors of this program grant you additional permission to convey
// the resulting work.

#include "attractmode.hpp"

// Standard Constructor
Sorcery::AttractMode::AttractMode(sf::Texture ctexture, Component component)
	: _ctexture{ctexture}, _component{component} {

	_cs_width = 108;
	_cs_height = 108;
	_cs_spacing = 8;
}

// We generate the attract mode graphic in the main thread, though we generate the IDs in the
// animation threads https://en.sfml-dev.org/forums/index.php?topic=18672.0
auto Sorcery::AttractMode::generate() -> void {

	// Only regenerate if we have a change
	if (data != data_temp) {
		data = data_temp;

		// Work out the new size
		const unsigned int number_to_display{static_cast<unsigned int>(data_temp.size())};
		const sf::Vector2f texture_size(
			_cs_width * number_to_display + (_cs_spacing * (number_to_display - 1)), _cs_height);

		// Don't worry about previous contents of this
		_rtexture.create(texture_size.x, texture_size.y);
		_rtexture.setSmooth(true);
		_rtexture.clear();

		// Get the Required Sprites
		unsigned int sprite_x{0};
		for (auto i : data) {
			sf::Sprite sprite{_get_creature_gfx(i, true)};
			sprite.setPosition(sprite_x, 0);
			_rtexture.draw(sprite, sf::BlendAlpha);
			sprite_x += (_cs_width + _cs_spacing);
		}

		// Draw them to the Texture
		_rtexture.display();
		_texture = _rtexture.getTexture();
		_sprite = sf::Sprite(_texture);
		_sprite.setColor(sf::Color(0, 0, 0, 175));
		sprite = _sprite;
	}
}

// Get the Sprite from the Creatures Texture
auto Sorcery::AttractMode::_get_creature_gfx(const int creature_id, const bool known)
	-> sf::Sprite {

	sf::IntRect crect{};
	sf::Sprite creature(_ctexture);
	crect.left = (creature_id - 1) * _cs_width;
	crect.width = _cs_width;
	crect.top = known ? 0 : _cs_height;
	crect.height = _cs_height;
	creature.setTextureRect(crect);
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
