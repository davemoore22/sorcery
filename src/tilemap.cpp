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
// see <http://www.gnu.orbear g/licenses/>.
//
// If you modify this program, or any covered work, by linking or combining
// it with the libraries referred to in README (or a modified version of
// said libraries), containing parts covered by the terms of said libraries,
// the licensors of this program grant you additional permission to convey
// the resulting work.

#include "tilemap.hpp"

Sorcery::TileMap::TileMap(System *system, Display *display, Graphics *graphics,
	Game *game, Component layout)
	: _system{system}, _display{display}, _graphics{graphics}, _game{game},
	  _layout{layout} {

	_height = MAP_SIZE;
	_width = MAP_SIZE;

	_tilemap_texture = &_system->resources->textures[GraphicsTexture::FLOORS];
}

auto Sorcery::TileMap::refresh() -> void {

	_vertices.setPrimitiveType(sf::Quads);
	_vertices.resize(_width * _height * 4);

	const int view_width_radius{
		std::stoi(_layout["view_width_radius"].value())};
	const int view_height_radius{
		std::stoi(_layout["view_height_radius"].value())};
	const int tile_size{std::stoi(_layout["tile_size"].value())};
	int i{0};
	int j{0};
	const int view_width{view_width_radius * 2 + 1};

	// For each square in the viewport
	auto player_pos(_game->state->world->player_pos);
	for (auto y = static_cast<int>(player_pos.y - view_width_radius);
		 y <= static_cast<int>(player_pos.y + view_width_radius); y++) {
		for (auto x = static_cast<int>(player_pos.x - view_height_radius);
			 x <= static_cast<int>(player_pos.x) + view_height_radius; x++) {

			// Get the tile
			auto lx{x < 0 ? x + MAP_SIZE : x};
			auto ly{y < 0 ? y + MAP_SIZE : y};
			auto tile{_game->state->world->current_level->at(lx, ly)};
			auto texture_id{tile.floor.gfx};

			// Display the background

			// Find the appropriate quad
			sf::Vertex *quad{&_vertices[(i + j * view_width) * 4]};

			// Define its corners
			quad[0].position = sf::Vector2f(i * tile_size, j * tile_size);
			quad[1].position = sf::Vector2f((i + 1) * tile_size, j * tile_size);
			quad[2].position =
				sf::Vector2f((i + 1) * tile_size, (j + 1) * tile_size);
			quad[3].position = sf::Vector2f(i * tile_size, (j + 1) * tile_size);

			// Find the origin texture for the Quad from the Floor Tileset
			auto tile_rect{_get_rect(texture_id)};
			quad[0].texCoords = sf::Vector2f(tile_rect.left, tile_rect.top);
			quad[1].texCoords =
				sf::Vector2f(tile_rect.left + tile_rect.width, tile_rect.top);
			quad[2].texCoords = sf::Vector2f(tile_rect.left + tile_rect.width,
				tile_rect.top + tile_rect.height);
			quad[3].texCoords =
				sf::Vector2f(tile_rect.left, tile_rect.top + tile_rect.height);

            // now do the Walls

            // And the Doors

            // And any decorations

            // The Party

			++i;
		}
		i = 0;
		++j;
	}
}

auto Sorcery::TileMap::draw(
	sf::RenderTarget &target, sf::RenderStates states) const -> void {

	states.texture = _tilemap_texture;
	target.draw(_vertices, states);

	states.transform *= getTransform();
}

// Texture Tiles are 400 pixels in size
auto Sorcery::TileMap::_get_rect(unsigned int index) const -> sf::IntRect {

	constexpr auto tile_size{400};
	return sf::IntRect(tile_size * (index % 15), tile_size * (index / 15),
		tile_size, tile_size);
}
