// Copyright (C) 2021 Dave Moore
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

#include "display.hpp"
#include "game.hpp"
#include "graphics.hpp"
#include "main.hpp"
#include "system.hpp"

namespace Sorcery {

	class TileMap : public sf::Transformable, public sf::Drawable {

	  public:
		// Constructors
		TileMap(System *system, Display *display, Graphics *graphics,
			Game *game, Component layout);
		TileMap() = delete;

		// Public Members

		// Public Methods
		auto refresh() -> void;

	  private:
		// Private Methods
		auto _draw_party(MapDirection direction, int x, int y, float scaling)
			-> void;
		auto _draw_tile(const Tile &tile, const int x, const int y,
			const float tile_scaling) -> void;
		auto _draw_tile(const Tile_ &tile, const int x, const int y,
			const float tile_scaling) -> void;
		auto _get_rect(unsigned int index) const -> sf::IntRect;
		auto _refresh_floor() -> void;
		auto _refresh_walls() -> void;
		auto virtual draw(
			sf::RenderTarget &target, sf::RenderStates states) const -> void;

		// Private Members
		System *_system;
		Display *_display;
		Graphics *_graphics;
		Game *_game;
		Component _layout;
		unsigned int _height;
		unsigned int _width;
		sf::VertexArray _wall_vertices;
		sf::Texture *_wall_texture;
		std::vector<sf::Sprite> _sprites;
		std::vector<sf::Sprite> _floor_sprites;
	};
} // namespace Sorcery