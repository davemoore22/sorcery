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

#pragma once

#include "common/include.hpp"
#include "common/macro.hpp"
#include "core/animation.hpp"
#include "core/define.hpp"
#include "resources/iconstore.hpp"
#include "resources/texturestore.hpp"

namespace Sorcery {

// Forward Declarations
class Display;
class System;

// Superclass to handle graphics specific functionality such as animation
class Graphics {
	public:

		// Constructors
		Graphics(System *system, Display *display);
		Graphics() = default;

		// Public Methods
		auto adjust_colour(int value, CAT ability_type) -> unsigned long long;
		auto adjust_status_colour(CHT value, bool poisoned)
			-> unsigned long long;
		auto adjust_brightness(sf::Color colour, double colour_lerp)
			-> unsigned long long;
		auto get_background_sprite() -> sf::Sprite;
		auto tile_bg(sf::RenderWindow *window) -> void;

		// Public Members
		std::unique_ptr<Animation> animation;
		std::unique_ptr<IconStore> icons;
		std::unique_ptr<TextureStore> textures;

	private:

		// Private Members
		System *_system;
		Display *_display;
		unsigned int _wp_idx;
		sf::Sprite _wp;

		// Private Methods
};

}