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

	_floor_sprites.clear();
	_sprites.clear();

	_wall_texture = &_system->resources->textures[GraphicsTexture::WALLS];
	_wall_vertices.setPrimitiveType(sf::Quads);
}

auto Sorcery::TileMap::refresh() -> void {

	_floor_sprites.clear();
	_sprites.clear();
	_refresh_floor();
	_refresh_walls();
}

auto Sorcery::TileMap::_refresh_floor() -> void {

	const auto view_width_radius{
		std::stoi(_layout["view_width_radius"].value())};
	const auto view_height_radius{
		std::stoi(_layout["view_height_radius"].value())};
	const auto tile_size{std::stoi(_layout["tile_size"].value())};
	const auto tile_scaling{tile_size / 400.0f};
	const auto party_scaling{tile_size / 511.0f};

	// Note that width is always greater than height so we can do this
	const auto view_height{view_height_radius * 2 + 1};

	// We need to reverse stuff in the y-direction
	auto reverse_y{((view_height - 1) * tile_size)};

	auto tcx{0};
	auto tcy{0};

	auto player_pos(_game->state->world->player_pos);
	for (auto y = static_cast<int>(player_pos.y - view_height_radius);
		 y <= static_cast<int>(player_pos.y + view_height_radius); y++) {
		for (auto x = static_cast<int>(player_pos.x - view_width_radius);
			 x <= static_cast<int>(player_pos.x) + view_width_radius; x++) {

			// Get the tile
			auto lx{[&] {
				if (x < 0)
					return x + MAP_SIZE;
				else if (x > MAP_SIZE - 1)
					return x - MAP_SIZE;
				return x;
			}()};
			auto ly{[&] {
				if (y < 0)
					return y + MAP_SIZE;
				else if (y > MAP_SIZE - 1)
					return y - MAP_SIZE;
				return y;
			}()};
			auto tile{_game->state->world->current_level->at(lx, ly)};

			auto tile_x{tcx * tile_size};
			auto tile_y{tcy * tile_size};
			_draw_tile(tile, tile_x, reverse_y - tile_y, tile_scaling);

			if ((x == static_cast<int>(player_pos.x)) &&
				(y == static_cast<int>(player_pos.y)))
				_draw_party(_game->state->world->playing_facing, tile_x,
					reverse_y - tile_y, party_scaling);
			++tcx;
		}
		++tcy;
		tcx = 0;
	}
};

auto Sorcery::TileMap::_draw_tile(const Tile &tile, const int x, const int y,
	const float tile_scaling) -> void {

	sf::Sprite bg{_graphics->textures
					  ->get(1, /*tile.floor.gfx,*/ GraphicsTextureType::FLOOR)
					  .value()};
	bg.setPosition(x, y);
	bg.setScale(tile_scaling, tile_scaling);
	_floor_sprites.emplace_back(bg);
};

auto Sorcery::TileMap::_draw_party(
	MapDirection direction, int x, int y, float scaling) -> void {

	auto party{(*_graphics->icons)["party"].value()};
	party.setOrigin(
		party.getLocalBounds().width / 2, party.getLocalBounds().height / 2);
	switch (_game->state->world->playing_facing) {
	case MapDirection::NORTH:
		party.setRotation(0.0f);
		break;
	case MapDirection::SOUTH:
		party.setRotation(180.0f);
		break;
	case MapDirection::EAST:
		party.setRotation(90.0f);
		break;
	case MapDirection::WEST:
		party.setRotation(270.0f);
		break;
	default:
		break;
	}
	party.setPosition(x + (party.getGlobalBounds().width / 2),
		y + (party.getGlobalBounds().height / 2));
	party.setScale(scaling, scaling);
	_sprites.emplace_back(party);
}

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

	// We need to reverse stuff in the y-direction
	auto reverse_y{(view_height * tile_size)};

	// For each square in the viewport
	auto player_pos(_game->state->world->player_pos);
	for (auto y = static_cast<int>(player_pos.y - view_height_radius);
		 y <= static_cast<int>(player_pos.y + view_height_radius); y++) {
		for (auto x = static_cast<int>(player_pos.x - view_width_radius);
			 x <= static_cast<int>(player_pos.x) + view_width_radius; x++) {

			// Get the Tile
			auto lx{[&] {
				if (x < 0)
					return x + MAP_SIZE;
				else if (x > MAP_SIZE - 1)
					return x - MAP_SIZE;
				return x;
			}()};
			auto ly{[&] {
				if (y < 0)
					return y + MAP_SIZE;
				else if (y > MAP_SIZE - 1)
					return y - MAP_SIZE;
				return y;
			}()};
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

			top_left.y = reverse_y - top_left.y;
			top_right.y = reverse_y - top_right.y;
			bottom_left.y = reverse_y - bottom_left.y;
			bottom_right.y = reverse_y - bottom_right.y;

			// North Wall - clockwise from top-left - note north and south are
			// reversed due to origin of map being bottom-left whereas sfml
			// works with origin top-left, which is a right pain
			auto n_wall_thickness{
				n_id != -1
					? std::stoi(_layout["wall_thickness_destination"].value())
					: std::stoi(
						  _layout["no_wall_thickness_destination"].value())};

			/* wall_quad[0].position = sf::Vector2f(top_left.x, top_left.y);
			wall_quad[1].position = sf::Vector2f(top_right.x, top_right.y);
			wall_quad[2].position =
				sf::Vector2f(top_right.x, top_right.y + n_wall_thickness);
			wall_quad[3].position =
				sf::Vector2f(top_left.x, top_right.y + n_wall_thickness); */

			wall_quad[0].position = sf::Vector2f(bottom_left.x, bottom_left.y);
			wall_quad[1].position =
				sf::Vector2f(bottom_right.x, bottom_right.y);
			wall_quad[2].position =
				sf::Vector2f(bottom_right.x, bottom_right.y - n_wall_thickness);
			wall_quad[3].position =
				sf::Vector2f(bottom_left.x, bottom_right.y - n_wall_thickness);

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

			/* wall_quad[4].position = sf::Vector2f(bottom_left.x,
			bottom_left.y); wall_quad[5].position = sf::Vector2f(bottom_right.x,
			bottom_right.y); wall_quad[6].position =
				sf::Vector2f(bottom_right.x, bottom_right.y - s_wall_thickness);
			wall_quad[7].position =
				sf::Vector2f(bottom_left.x, bottom_right.y - s_wall_thickness);
		  */

			wall_quad[4].position = sf::Vector2f(top_left.x, top_left.y);
			wall_quad[5].position = sf::Vector2f(top_right.x, top_right.y);
			wall_quad[6].position =
				sf::Vector2f(top_right.x, top_right.y + s_wall_thickness);
			wall_quad[7].position =
				sf::Vector2f(top_left.x, top_right.y + s_wall_thickness);
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
				sf::Vector2f(w_tile_rect.left + w_texture_wall_thickness,
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

	states.transform *= getTransform();
	for (const auto &fs : _floor_sprites)
		target.draw(fs, states);
	for (const auto &s : _sprites)
		target.draw(s, states);

	states.texture = _wall_texture;
	target.draw(_wall_vertices, states);
}

// Texture Tiles are 400 pixels in size
auto Sorcery::TileMap::_get_rect(unsigned int index) const -> sf::IntRect {

	constexpr auto tile_size{400};
	return sf::IntRect(tile_size * (index % 15), tile_size * (index / 15),
		tile_size, tile_size);
}
