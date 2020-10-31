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

#pragma once

#include "main.hpp"
#include "system.hpp"
#include "display.hpp"
#include "graphics.hpp"
#include "layout.hpp"


namespace Sorcery {

	class Display;

	class Frame: public sf::Transformable, public sf::Drawable {

	public:

			// Standard Constructor
			Frame(System& system, Display& display, Graphics& graphics, WindowFrameType _type,
				const unsigned int width_units, const unsigned int height_units, const unsigned int alpha);

			// Standard Destructor
			virtual ~Frame();

			// Public Members

			// Public Methods


		private:

			// Private Methods
			auto virtual draw(sf::RenderTarget& target, sf::RenderStates states) const -> void;

			// Private Members
			System _system;
			Display _display;
			Graphics _graphics;
			WindowFrameType _type;
			unsigned int _width_units;
			unsigned int _height_units;
			unsigned int _alpha;
			unsigned int _width;
			unsigned int _height;
			sf::RenderWindow* _window;
			std::array<sf::IntRect, 8> _frame_parts;
			std::array<sf::Sprite, 8> _frame_sprites;
			sf::RenderTexture _render_texture;
			sf::Texture _texture;
			sf::Sprite _frame;
			sf::RectangleShape _background;
	};
}
