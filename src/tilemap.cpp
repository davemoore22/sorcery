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
	_wall_texture = &_system->resources->textures[GraphicsTexture::WALLS];

	_wall_vertices.setPrimitiveType(sf::Quads);
	_floor_vertices.setPrimitiveType(sf::Quads);
}

auto Sorcery::TileMap::refresh() -> void {

	_refresh_floor();
	//_refresh_walls();
}

auto Sorcery::TileMap::_refresh_floor() -> void {

	_floor_vertices.clear();
	_floor_vertices.resize(_width * _height * 4);

	const auto view_width_radius{
		std::stoi(_layout["view_width_radius"].value())};
	const auto view_height_radius{
		std::stoi(_layout["view_height_radius"].value())};
	const auto tile_size{std::stoi(_layout["tile_size"].value())};
	auto i{0};
	auto j{0};

	// Note that width is always greater than height so we can do this
	const auto view_width{view_width_radius * 2 + 1};

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
			sf::Vertex *floor_quad{&_floor_vertices[(i + j * view_width) * 4]};

			// Define its corners
			floor_quad[0].position = sf::Vector2f(i * tile_size, j * tile_size);
			floor_quad[1].position =
				sf::Vector2f((i + 1) * tile_size, j * tile_size);
			floor_quad[2].position =
				sf::Vector2f((i + 1) * tile_size, (j + 1) * tile_size);
			floor_quad[3].position =
				sf::Vector2f(i * tile_size, (j + 1) * tile_size);

			// Find the origin texture for the Quad from the Floor Tileset
			auto tile_rect{_get_rect(texture_id)};
			floor_quad[0].texCoords =
				sf::Vector2f(tile_rect.left, tile_rect.top);
			floor_quad[1].texCoords =
				sf::Vector2f(tile_rect.left + tile_rect.width, tile_rect.top);
			floor_quad[2].texCoords =
				sf::Vector2f(tile_rect.left + tile_rect.width,
					tile_rect.top + tile_rect.height);
			floor_quad[3].texCoords =
				sf::Vector2f(tile_rect.left, tile_rect.top + tile_rect.height);

			++i;
		}
		i = 0;
		++j;
	}
};

auto Sorcery::TileMap::_refresh_walls() -> void {

	_wall_vertices.clear();
	_wall_vertices.resize(_width * _height * 4 * 4);

	const auto view_width_radius{
		std::stoi(_layout["view_width_radius"].value())};
	const auto view_height_radius{
		std::stoi(_layout["view_height_radius"].value())};
	const auto tile_size{std::stoi(_layout["tile_size"].value())};
	auto i{0};
	auto j{0};
	const auto view_width{view_width_radius * 2 + 1};
	const auto view_height{view_height_radius * 2 + 1};

	// For each square in the viewport
	auto player_pos(_game->state->world->player_pos);
	for (auto y = static_cast<int>(player_pos.y - view_width_radius);
		 y <= static_cast<int>(player_pos.y + view_width_radius); y++) {
		for (auto x = static_cast<int>(player_pos.x - view_height_radius);
			 x <= static_cast<int>(player_pos.x) + view_height_radius; x++) {

			// Get the Tile
			auto lx{x < 0 ? x + MAP_SIZE : x};
			auto ly{y < 0 ? y + MAP_SIZE : y};
			auto tile{_game->state->world->current_level->at(lx, ly)};

			// Get the Walls (note texture 0 is the white square only)
			auto n_id{tile.check_wall(TileWall::NORTH)
						  ? tile.walls.at(TileWall::NORTH).gfx
						  : -1}; // TODO: hack
			auto s_id{tile.check_wall(TileWall::SOUTH)
						  ? tile.walls.at(TileWall::SOUTH).gfx
						  : -1};
			auto e_id{tile.check_wall(TileWall::EAST)
						  ? tile.walls.at(TileWall::EAST).gfx
						  : -1};
			auto w_id{tile.check_wall(TileWall::WEST)
						  ? tile.walls.at(TileWall::WEST).gfx
						  : -1};

			// Find the appropriate quad
			sf::Vertex *wall_quad{
				&_wall_vertices[(i + j * view_width) * 4 * 4]};

			// Work out the tile corners for sake of convenience
			auto top_left{sf::Vector2f(i * tile_size, j * tile_size)};
			auto top_right{sf::Vector2f((i + 1) * tile_size, j * tile_size)};
			auto bottom_right{
				sf::Vector2f((i + 1) * tile_size, (j + 1) * tile_size)};
			auto bottom_left{sf::Vector2f(i * tile_size, (j + 1) * tile_size)};

			// North Wall - clockwise from top-left
			auto n_wall_thickness{
				n_id != -1
					? std::stoi(_layout["wall_thickness_destination"].value())
					: std::stoi(
						  _layout["no_wall_thickness_destination"].value())};
			wall_quad[0].position = sf::Vector2f(top_left.x, top_left.y);
			wall_quad[1].position = sf::Vector2f(top_right.x, top_right.y);
			wall_quad[2].position =
				sf::Vector2f(top_right.x, top_right.y + n_wall_thickness);
			wall_quad[3].position =
				sf::Vector2f(top_left.x, top_right.y + n_wall_thickness);
			auto n_tile_rect{_get_rect(n_id)};
			auto n_texture_wall_thickness{
				n_id != -1
					? std::stoi(_layout["wall_thickness_source"].value())
					: std::stoi(_layout["no_wall_thickness_source"].value())};
			wall_quad[0].texCoords =
				sf::Vector2f(n_tile_rect.left, n_tile_rect.top);
			wall_quad[1].texCoords = sf::Vector2f(
				n_tile_rect.left + n_tile_rect.width, n_tile_rect.top);
			wall_quad[2].texCoords =
				sf::Vector2f(n_tile_rect.left + n_tile_rect.width,
					n_tile_rect.top + n_texture_wall_thickness);
			wall_quad[3].texCoords = sf::Vector2f(
				n_tile_rect.left, n_tile_rect.top + n_texture_wall_thickness);

			// South Wall - anti-clockwise from bottom-left
			auto s_wall_thickness{
				s_id != -1
					? std::stoi(_layout["wall_thickness_destination"].value())
					: std::stoi(
						  _layout["no_wall_thickness_destination"].value())};
			wall_quad[4].position = sf::Vector2f(bottom_left.x, bottom_left.y);
			wall_quad[5].position =
				sf::Vector2f(bottom_right.x, bottom_right.y);
			wall_quad[6].position =
				sf::Vector2f(bottom_right.x, bottom_right.y - s_wall_thickness);
			wall_quad[7].position =
				sf::Vector2f(bottom_left.x, bottom_right.y - s_wall_thickness);
			auto s_tile_rect{_get_rect(s_id)};
			auto s_texture_wall_thickness{
				s_id != -1
					? std::stoi(_layout["wall_thickness_source"].value())
					: std::stoi(_layout["no_wall_thickness_source"].value())};
			wall_quad[4].texCoords = sf::Vector2f(
				s_tile_rect.left, s_tile_rect.top + s_tile_rect.height);
			wall_quad[5].texCoords =
				sf::Vector2f(s_tile_rect.left + s_tile_rect.width,
					s_tile_rect.top + s_tile_rect.height);
			wall_quad[6].texCoords =
				sf::Vector2f(s_tile_rect.left + s_tile_rect.width,
					s_tile_rect.top + s_tile_rect.height -
						s_texture_wall_thickness);
			wall_quad[7].texCoords = sf::Vector2f(
				s_tile_rect.left, s_tile_rect.top + s_tile_rect.height -
									  s_texture_wall_thickness);

			// East Wall - clockwise from top-right
			auto e_wall_thickness{
				e_id != -1
					? std::stoi(_layout["wall_thickness_destination"].value())
					: std::stoi(
						  _layout["no_wall_thickness_destination"].value())};
			wall_quad[8].position = sf::Vector2f(top_right.x, top_right.y);
			wall_quad[9].position =
				sf::Vector2f(bottom_right.x, bottom_right.y);
			wall_quad[10].position =
				sf::Vector2f(bottom_right.x - e_wall_thickness, bottom_right.y);
			wall_quad[11].position =
				sf::Vector2f(top_right.x - e_wall_thickness, top_right.y);
			auto e_tile_rect{_get_rect(e_id)};
			auto e_texture_wall_thickness{
				e_id != -1
					? std::stoi(_layout["wall_thickness_source"].value())
					: std::stoi(_layout["no_wall_thickness_source"].value())};
			wall_quad[8].texCoords = sf::Vector2f(
				e_tile_rect.left + e_tile_rect.width, e_tile_rect.top);
			wall_quad[9].texCoords =
				sf::Vector2f(e_tile_rect.left + e_tile_rect.width,
					e_tile_rect.top + e_tile_rect.height);
			wall_quad[10].texCoords = sf::Vector2f(
				e_tile_rect.left + e_tile_rect.width - e_texture_wall_thickness,
				e_tile_rect.top + e_tile_rect.height);
			wall_quad[11].texCoords = sf::Vector2f(
				e_tile_rect.left + e_tile_rect.width - e_texture_wall_thickness,
				e_tile_rect.top);

			// West Wall - anti-clockwise from top-left
			auto w_wall_thickness{
				w_id != -1
					? std::stoi(_layout["wall_thickness_destination"].value())
					: std::stoi(
						  _layout["no_wall_thickness_destination"].value())};
			wall_quad[12].position = sf::Vector2f(top_left.x, top_left.y);
			wall_quad[13].position = sf::Vector2f(top_left.x, bottom_left.y);
			wall_quad[14].position =
				sf::Vector2f(bottom_left.x + w_wall_thickness, bottom_left.y);
			wall_quad[15].position =
				sf::Vector2f(bottom_left.x + w_wall_thickness, top_left.y);
			auto w_tile_rect{_get_rect(w_id)};
			auto w_texture_wall_thickness{
				w_id != -1
					? std::stoi(_layout["wall_thickness_source"].value())
					: std::stoi(_layout["no_wall_thickness_source"].value())};

			wall_quad[12].texCoords =
				sf::Vector2f(w_tile_rect.left, w_tile_rect.top);
			wall_quad[13].texCoords = sf::Vector2f(
				w_tile_rect.left, w_tile_rect.top + w_tile_rect.height);
			wall_quad[14].texCoords =
				sf::Vector2f(e_tile_rect.left + w_texture_wall_thickness,
					w_tile_rect.top + w_tile_rect.height);
			wall_quad[15].texCoords = sf::Vector2f(
				w_tile_rect.left + w_texture_wall_thickness, w_tile_rect.top);

			++i;
		}
		i = 0;
		++j;
	}
};

auto Sorcery::TileMap::draw(
	sf::RenderTarget &target, sf::RenderStates states) const -> void {

	states.texture = _tilemap_texture;
	target.draw(_floor_vertices, states);
	states.texture = _wall_texture;
	target.draw(_wall_vertices, states);

	states.transform *= getTransform();
}

// Texture Tiles are 400 pixels in size
auto Sorcery::TileMap::_get_rect(unsigned int index) const -> sf::IntRect {

	constexpr auto tile_size{400};
	return sf::IntRect(tile_size * (index % 15), tile_size * (index / 15),
		tile_size, tile_size);
}
