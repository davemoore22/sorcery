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

#include "attractmode.hpp"

// Standard Constructor
Sorcery::AttractMode::AttractMode(Graphics *graphics, const Component &component)
	: _graphics{graphics}, _component{component} {
}

// We generate the attract mode graphic in the main thread, though we generate
// the IDs in the animation threads - see
// https://en.sfml-dev.org/forums/index.php?topic=18672.0 for why it is done
// this way
auto Sorcery::AttractMode::generate() -> void {

	// Only regenerate if we have a change
	if (data != data_temp) {
		data = data_temp;

		// Work out the new size
		const auto number_to_display{static_cast<unsigned int>(data_temp.size())};
		const sf::Vector2f texture_size(
			CREATURE_TILE_SIZE * number_to_display + (ATTRACT_MODE_TILE_SPACING * (number_to_display - 1)),
			CREATURE_TILE_SIZE);

		// Don't worry about previous contents of this
		_rtexture.create(texture_size.x, texture_size.y);
		_rtexture.setSmooth(true);

		// Get the Required Sprites
		auto sprite_x{0u};
		for (auto i : data) {
			sf::Sprite sprite{_graphics->textures->get(i, GraphicsTextureType::KNOWN_CREATURE).value()};
			sprite.setPosition(sprite_x, 0);
			_rtexture.draw(sprite, sf::BlendAlpha);
			sprite_x += (CREATURE_TILE_SIZE + ATTRACT_MODE_TILE_SPACING);
		}

		// Draw them to the Texture
		_rtexture.display();
		_texture = _rtexture.getTexture();
		_sprite = sf::Sprite(_texture);
		sprite = _sprite;
	}
}

// Adjust the Alpha
auto Sorcery::AttractMode::set_alpha(unsigned int alpha) -> void {

	if (!data_temp.empty())
		_sprite.setColor(sf::Color(255, 255, 255, alpha));
}

auto Sorcery::AttractMode::draw(sf::RenderTarget &target, sf::RenderStates states) const -> void {

	if (!data_temp.empty()) {
		states.transform *= getTransform();
		target.draw(_sprite, states);
	}
}
