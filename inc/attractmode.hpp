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

#pragma once

#include "component.hpp"
#include "main.hpp"

namespace Sorcery {

	class Display;

	class AttractMode : public sf::Transformable, public sf::Drawable {

	  public:
		// Constructors
		AttractMode(sf::Texture ctexture, Component component);
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
		auto _get_creature_gfx(const int creature_id, const bool known) -> sf::Sprite;

		// Private Members
		sf::Texture _ctexture;
		sf::RenderTexture _rtexture;
		sf::Texture _texture;
		sf::Sprite _sprite;
		Component _component;
		unsigned int _cs_width;
		unsigned int _cs_height;
		unsigned int _cs_spacing;
	};
} // namespace Sorcery