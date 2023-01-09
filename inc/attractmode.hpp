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

#pragma once

#include "component.hpp"
#include "graphics.hpp"
#include "main.hpp"

namespace Sorcery {

class AttractMode: public sf::Transformable, public sf::Drawable {

	public:

		// Constructors
		AttractMode(Graphics *graphics, const Component &component);
		AttractMode() = delete;

		// Public Members
		std::vector<unsigned int> data;
		std::vector<unsigned int> data_temp;
		sf::Sprite sprite;

		// Public Methods
		auto generate() -> void;
		auto set_alpha(unsigned int alpha) -> void;

	private:

		// Private Methods
		auto virtual draw(sf::RenderTarget &target, sf::RenderStates states) const -> void;

		// Private Members
		Graphics *_graphics;
		sf::RenderTexture _rtexture;
		sf::Texture _texture;
		sf::Sprite _sprite;
		Component _component;
};

}