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
// said  libraries), containing parts covered by the terms of said libraries,
// the licensors of this program grant you additional permission to convey
// the resulting work.

#pragma once

#include "main.hpp"
#include "layout.hpp"

namespace Sorcery {

	class Display;

	class Frame: public sf::Transformable, public sf::Drawable {

		public:

			// Constructors
			Frame(sf::Texture texture, WindowFrameType _type, const unsigned int width_units,
				const unsigned int height_units, const unsigned int alpha);
			Frame() = delete;

			// Public Members
			unsigned int width;
			unsigned int height;
			sf::Sprite sprite;

			// Public Methods


		private:

			// Private Methods
			auto virtual draw(sf::RenderTarget& target, sf::RenderStates states) const -> void;

			// Private Members
			std::array<sf::IntRect, 8> _frame_parts;
			std::array<sf::Sprite, 8> _frame_sprites;
			sf::RenderTexture _render_texture;
			sf::Texture _texture;
			WindowFrameType _type;
			unsigned int _width_units;
			unsigned int _height_units;
			unsigned int _alpha;
			sf::Sprite _frame;
			sf::RectangleShape _background;
			unsigned int _texture_size_x;
			unsigned int _texture_size_y;
	};
}
