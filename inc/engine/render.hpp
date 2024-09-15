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
#include "engine/view.hpp"
#include "types/tile.hpp"

namespace Sorcery {

// Forward Declarations
class Display;
class Game;
class Graphics;
class System;

class Render: public sf::Transformable, public sf::Drawable {

	public:

		// Constructors
		Render(System *system, Display *display, Graphics *graphics, Game *game);
		Render() = delete;

		// Public Methods
		auto refresh() -> void;
		auto get_monochrome() const -> bool;
		auto set_monochrome(bool value) -> void;

	private:

		// Private Members
		System *_system;
		Display *_display;
		Graphics *_graphics;
		Game *_game;
		std::unique_ptr<View> _view;

		sf::RenderTexture _rtexture;
		sf::Texture _texture;
		std::vector<sf::Sprite> _sprites;

		// Private Methods
		auto _darken_sprite(const unsigned int depth, bool lit) const -> sf::Color;
		auto _render_floor(bool lit) -> void;
		auto _render_ceiling(bool lit) -> void;
		auto _render_walls(bool lit) -> void;

		auto virtual draw(sf::RenderTarget &target, sf::RenderStates states) const -> void;
		auto _render_wireframe(sf::RenderTarget &target, sf::RenderStates states) const -> void;
		auto _get_left_side(MAD facing) const -> MAD;
		auto _get_right_side(MAD facing) const -> MAD;

		auto _has_wall(const Tile &tile, const Sorcery::MAD direction) const -> bool;
		auto _has_secret_door(const Tile &tile, const Sorcery::MAD direction) const -> bool;
		auto _has_normal_door(const Tile &tile, const Sorcery::MAD direction) const -> bool;
};

}
