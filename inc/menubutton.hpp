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

namespace Sorcery {

	class MenuButton {

		public:

			// Standard Constructor
			MenuButton(sf::Texture& texture, MenuItem item, std::string_view text, Point _position);

			// Standard Destructor
			virtual ~MenuButton();

			// Public Methods

			// Public Members

		private:

			// Private Members
			MenuItem _item;
			MenuButtonState _state;
			sf::Sprite _hovered;
			sf::Sprite _selected;
			sf::Sprite _disabled;
			sf::Sprite _enabled;
			std::string_view _text;
			Point _position;
			sf::Texture& _texture;

			// Private Methods
	};
}