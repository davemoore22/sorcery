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

#include "engine/view.hpp"

// Standard Constructor
Sorcery::View::View(System *system, Display *display, Graphics *graphics, Game *game)
	: _system{system}, _display{display}, _graphics{graphics}, _game{game} {

	_monochrome = false;

	_load_tile_views();
}

auto Sorcery::View::get_monochrome() const -> bool {

	return _monochrome;
}

auto Sorcery::View::set_monochrome(bool value) -> void {

	_monochrome = value;

	_load_tile_views();
}

auto Sorcery::View::_set_texture_coordinates(TileView &tileview) -> void {

	constexpr auto size_panel_x{304u};
	constexpr auto size_panel_y{176u};
	for (auto i = 0; i <= 3; i++) {
		tileview.floor[i].texCoords = tileview.floor[i].position;
		tileview.ceiling[i].texCoords = tileview.ceiling[i].position;
		tileview.up[i].texCoords = tileview.up[i].position;
		tileview.down[i].texCoords = tileview.down[i].position;
		tileview.darkness[i].texCoords.x = tileview.darkness[i].position.x + (0 * size_panel_x);
		tileview.darkness[i].texCoords.y = tileview.darkness[i].position.y + (1 * size_panel_y);
		tileview.side_darkness[i].texCoords.x = tileview.side_darkness[i].position.x + (1 * size_panel_x);
		tileview.side_darkness[i].texCoords.y = tileview.side_darkness[i].position.y + (0 * size_panel_y);
		tileview.left_side_wall[i].texCoords.x = tileview.left_side_wall[i].position.x + (1 * size_panel_x);
		tileview.left_side_wall[i].texCoords.y = tileview.left_side_wall[i].position.y + (1 * size_panel_y);
		tileview.right_side_wall[i].texCoords.x = tileview.right_side_wall[i].position.x + (1 * size_panel_x);
		tileview.right_side_wall[i].texCoords.y = tileview.right_side_wall[i].position.y + (1 * size_panel_y);
		tileview.left_side_door[i].texCoords.x = tileview.left_side_door[i].position.x + (2 * size_panel_x);
		tileview.left_side_door[i].texCoords.y = tileview.left_side_door[i].position.y + (0 * size_panel_y);
		tileview.right_side_door[i].texCoords.x = tileview.right_side_door[i].position.x + (2 * size_panel_x);
		tileview.right_side_door[i].texCoords.y = tileview.right_side_door[i].position.y + (0 * size_panel_y);

		auto offset_x{0};
		auto offset_y{0};
		if (tileview.offset.z == 0) {
			offset_x = 0;
			offset_y = 3;
		} else if (tileview.offset.z == -1) {
			offset_x = 1;
			offset_y = 3;
		} else if (tileview.offset.z == -2) {
			offset_x = 2;
			offset_y = 3;
		} else if (tileview.offset.z == -3) {
			offset_x = 0;
			offset_y = 4;
		}
		tileview.back_wall[i].texCoords.x = tileview.back_wall[i].position.x + (offset_x * size_panel_x);
		tileview.back_wall[i].texCoords.y = tileview.back_wall[i].position.y + (offset_y * size_panel_y);

		if (tileview.offset.z == 0) {
			offset_x = 2;
			offset_y = 1;
		} else if (tileview.offset.z == -1) {
			offset_x = 0;
			offset_y = 2;
		} else if (tileview.offset.z == -2) {
			offset_x = 1;
			offset_y = 2;
		} else if (tileview.offset.z == -3) {
			offset_x = 2;
			offset_y = 2;
		}
		tileview.back_door[i].texCoords.x = tileview.back_door[i].position.x + (offset_x * size_panel_x);
		tileview.back_door[i].texCoords.y = tileview.back_door[i].position.y + (offset_y * size_panel_y);
	}
}

auto Sorcery::View::_set_vertex_array(
	sf::VertexArray &array, sf::Vector2f p1, sf::Vector2f p2, sf::Vector2f p3, sf::Vector2f p4) -> void {

	array[0].position = p1;
	array[1].position = p2;
	array[2].position = p3;
	array[3].position = p4;
}

auto Sorcery::View::_set_vertex_array(sf::VertexArray &array, sf::Vector2f p1, sf::Vector2f p2, sf::Vector2f p3,
	sf::Vector2f p4, const sf::Color colour) -> void {
	array[0].position = p1;
	array[1].position = p2;
	array[2].position = p3;
	array[3].position = p4;
	array[0].color = colour;
	array[1].color = colour;
	array[2].color = colour;
	array[3].color = colour;
}

auto Sorcery::View::_load_tile_views() -> void {

	tileviews.clear();
	for (auto x = -1; x <= 1; x++) {
		for (auto z = 0; z >= -5; z--) {
			Coordinate3 loc{x, 0, z};
			tileviews[loc] = TileView{loc};
		}
	}

	const auto floor_c{(*_display->layout)["engine_base_ui:wireframe_view_small"]};

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnarrowing"
	auto floor_colour{sf::Color{std::stoull(floor_c["monochrome_colour"].value(), nullptr, 16)}};
	auto ceiling_colour{sf::Color{std::stoull(floor_c["monochrome_colour"].value(), nullptr, 16)}};
	auto stairs_colour{sf::Color{std::stoull(floor_c["monochrome_colour"].value(), nullptr, 16)}};
	auto elevator_colour{sf::Color{std::stoull(floor_c["monochrome_colour"].value(), nullptr, 16)}};
	auto darkness_colour{sf::Color{std::stoull(floor_c["monochrome_colour"].value(), nullptr, 16)}};

	if (!_monochrome) {
		floor_colour = sf::Color{std::stoull(floor_c["floor_colour"].value(), nullptr, 16)};
		ceiling_colour = sf::Color{std::stoull(floor_c["ceiling_colour"].value(), nullptr, 16)};
		stairs_colour = sf::Color{std::stoull(floor_c["stairs_colour"].value(), nullptr, 16)};
		elevator_colour = sf::Color{std::stoull(floor_c["elevator_colour"].value(), nullptr, 16)};
		darkness_colour = sf::Color{std::stoull(floor_c["darkness_colour"].value(), nullptr, 16)};
	}
#pragma GCC diagnostic pop

	//  FLOORS/CEILINGS				SIDE DARKNESS			SIDE DOORS
	//	FRONT DARKNESS				SIDE WALLS				FRONT DOORS z =
	// 0
	//  FRONT DOORS z = -1			FRONT DOORS z = -2		FRONT DOORS z =
	//  -3
	//  FRONT WALLS z = 0			FRONT WALLS z = -1		FRONT WALLS z =
	//  -2 FRONT WALLS z = -3

	// https://www.sfml-dev.org/tutorials/2.5/graphics-vertex-array.php

	// Tile the player is standing on
	{
		Coordinate3 tile{0, 0, 0};
		auto &tileview{tileviews.at(tile)};

		// Set Vertex Arrays
		_set_vertex_array(tileview.floor, sf::Vector2f{88, 167}, sf::Vector2f{95, 160}, sf::Vector2f{208, 160},
			sf::Vector2f{215, 167}, floor_colour);
		_set_vertex_array(tileview.ceiling, sf::Vector2f{95, 15}, sf::Vector2f{88, 8}, sf::Vector2f{215, 8},
			sf::Vector2f{208, 15}, ceiling_colour);
		_set_vertex_array(tileview.down, sf::Vector2f{88, 167}, sf::Vector2f{95, 160}, sf::Vector2f{208, 160},
			sf::Vector2f{215, 167}, stairs_colour);
		_set_vertex_array(tileview.up, sf::Vector2f{95, 15}, sf::Vector2f{88, 8}, sf::Vector2f{215, 8},
			sf::Vector2f{208, 15}, stairs_colour);
		_set_vertex_array(tileview.darkness, sf::Vector2f{8, 167}, sf::Vector2f{8, 8}, sf::Vector2f{296, 8},
			sf::Vector2f{296, 167}, darkness_colour);
		_set_vertex_array(tileview.back_wall, sf::Vector2f{87, 153}, sf::Vector2f{87, 23}, sf::Vector2f{217, 23},
			sf::Vector2f{217, 153});
		_set_vertex_array(tileview.back_door, sf::Vector2f{87, 153}, sf::Vector2f{87, 23}, sf::Vector2f{217, 23},
			sf::Vector2f{217, 153});
		_set_vertex_array(tileview.left_side_wall, sf::Vector2f{8, 167}, sf::Vector2f{8, 8}, sf::Vector2f{87, 8},
			sf::Vector2f{87, 167});
		_set_vertex_array(tileview.left_side_door, sf::Vector2f{8, 167}, sf::Vector2f{8, 8}, sf::Vector2f{87, 8},
			sf::Vector2f{87, 167});
		_set_vertex_array(tileview.right_side_wall, sf::Vector2f{216, 167}, sf::Vector2f{216, 8}, sf::Vector2f{296, 8},
			sf::Vector2f{296, 167});
		_set_vertex_array(tileview.right_side_door, sf::Vector2f{216, 167}, sf::Vector2f{216, 8}, sf::Vector2f{296, 8},
			sf::Vector2f{296, 167});

		_set_texture_coordinates(tileview);
	}

	// Tile in Front of the Player
	{
		Coordinate3 tile{0, 0, -1};
		auto &tileview{tileviews.at(tile)};

		// Set Vertex Arrays
		_set_vertex_array(tileview.floor, sf::Vector2f{112, 143}, sf::Vector2f{127, 128}, sf::Vector2f{176, 128},
			sf::Vector2f{191, 143}, floor_colour);
		_set_vertex_array(tileview.ceiling, sf::Vector2f{127, 47}, sf::Vector2f{112, 32}, sf::Vector2f{191, 32},
			sf::Vector2f{176, 47}, ceiling_colour);
		_set_vertex_array(tileview.down, sf::Vector2f{112, 143}, sf::Vector2f{127, 128}, sf::Vector2f{176, 128},
			sf::Vector2f{191, 143}, stairs_colour);
		_set_vertex_array(tileview.up, sf::Vector2f{127, 47}, sf::Vector2f{112, 32}, sf::Vector2f{191, 32},
			sf::Vector2f{176, 47}, stairs_colour);
		_set_vertex_array(tileview.darkness, sf::Vector2f{87, 152}, sf::Vector2f{87, 23}, sf::Vector2f{216, 23},
			sf::Vector2f{216, 152}, darkness_colour);
		_set_vertex_array(tileview.back_wall, sf::Vector2f{119, 121}, sf::Vector2f{119, 55}, sf::Vector2f{185, 55},
			sf::Vector2f{185, 121});
		_set_vertex_array(tileview.back_door, sf::Vector2f{119, 121}, sf::Vector2f{119, 55}, sf::Vector2f{185, 55},
			sf::Vector2f{185, 121});

		_set_vertex_array(tileview.left_side_wall, sf::Vector2f{87, 153}, sf::Vector2f{87, 23}, sf::Vector2f{120, 23},
			sf::Vector2f{120, 153});
		_set_vertex_array(tileview.left_side_door, sf::Vector2f{87, 153}, sf::Vector2f{87, 23}, sf::Vector2f{120, 23},
			sf::Vector2f{120, 153});
		_set_vertex_array(tileview.right_side_wall, sf::Vector2f{184, 121}, sf::Vector2f{184, 55},
			sf::Vector2f{217, 23}, sf::Vector2f{217, 153});
		_set_vertex_array(tileview.right_side_door, sf::Vector2f{184, 121}, sf::Vector2f{184, 55},
			sf::Vector2f{217, 23}, sf::Vector2f{217, 153});

		_set_texture_coordinates(tileview);
	}

	// Tile 2 in Front of the Player
	{
		Coordinate3 tile{0, 0, -2};
		auto &tileview{tileviews.at(tile)};

		// Set Vertex Arrays
		_set_vertex_array(tileview.darkness, sf::Vector2f{120, 119}, sf::Vector2f{120, 56}, sf::Vector2f{183, 56},
			sf::Vector2f{183, 119}, darkness_colour);
		_set_vertex_array(tileview.back_wall, sf::Vector2f{135, 105}, sf::Vector2f{135, 71}, sf::Vector2f{169, 71},
			sf::Vector2f{169, 105});
		_set_vertex_array(tileview.back_door, sf::Vector2f{135, 105}, sf::Vector2f{139, 71}, sf::Vector2f{169, 71},
			sf::Vector2f{168, 105});
		_set_vertex_array(tileview.left_side_wall, sf::Vector2f{119, 120}, sf::Vector2f{119, 54}, sf::Vector2f{135, 71},
			sf::Vector2f{135, 120});
		_set_vertex_array(tileview.left_side_door, sf::Vector2f{119, 120}, sf::Vector2f{119, 54}, sf::Vector2f{135, 71},
			sf::Vector2f{135, 120});
		_set_vertex_array(tileview.right_side_wall, sf::Vector2f{168, 105}, sf::Vector2f{168, 71},
			sf::Vector2f{184, 54}, sf::Vector2f{184, 120});
		_set_vertex_array(tileview.right_side_door, sf::Vector2f{168, 105}, sf::Vector2f{168, 71},
			sf::Vector2f{184, 54}, sf::Vector2f{184, 120});

		_set_texture_coordinates(tileview);
	}

	// Tile 3 in Front of the Player
	{
		Coordinate3 tile{0, 0, -3};
		auto &tileview{tileviews.at(tile)};

		// Set Vertex Arrays
		_set_vertex_array(tileview.darkness, sf::Vector2f{135, 104}, sf::Vector2f{135, 71}, sf::Vector2f{168, 71},
			sf::Vector2f{168, 104}, darkness_colour);
		_set_vertex_array(tileview.back_wall, sf::Vector2f{143, 97}, sf::Vector2f{143, 79}, sf::Vector2f{161, 79},
			sf::Vector2f{161, 97});
		_set_vertex_array(tileview.back_door, sf::Vector2f{143, 97}, sf::Vector2f{143, 79}, sf::Vector2f{161, 79},
			sf::Vector2f{161, 97});
		_set_vertex_array(tileview.left_side_wall, sf::Vector2f{135, 105}, sf::Vector2f{135, 71}, sf::Vector2f{144, 79},
			sf::Vector2f{144, 105});
		_set_vertex_array(tileview.left_side_door, sf::Vector2f{135, 105}, sf::Vector2f{135, 71}, sf::Vector2f{144, 79},
			sf::Vector2f{144, 105});
		_set_vertex_array(tileview.right_side_wall, sf::Vector2f{160, 97}, sf::Vector2f{160, 79}, sf::Vector2f{168, 71},
			sf::Vector2f{168, 105});
		_set_vertex_array(tileview.right_side_door, sf::Vector2f{160, 97}, sf::Vector2f{160, 79}, sf::Vector2f{168, 71},
			sf::Vector2f{168, 105});

		_set_texture_coordinates(tileview);
	}

	// Tile 4 in Front of the Player
	{
		Coordinate3 tile{0, 0, -4};
		auto &tileview{tileviews.at(tile)};

		// Set Vertex Arrays
		_set_vertex_array(tileview.darkness, sf::Vector2f{143, 96}, sf::Vector2f{143, 79}, sf::Vector2f{160, 79},
			sf::Vector2f{160, 96}, darkness_colour);

		_set_texture_coordinates(tileview);
	}

	// Tile to the Immediate Left of the Player
	{
		Coordinate3 tile{-1, 0, 0};
		auto &tileview{tileviews.at(tile)};

		// Set Vertex Arrays
		_set_vertex_array(tileview.floor, sf::Vector2f{8, 167}, sf::Vector2f{8, 160}, sf::Vector2f{63, 160},
			sf::Vector2f{56, 167}, floor_colour);
		_set_vertex_array(tileview.ceiling, sf::Vector2f{8, 15}, sf::Vector2f{8, 8}, sf::Vector2f{56, 8},
			sf::Vector2f{63, 15}, ceiling_colour);
		_set_vertex_array(tileview.down, sf::Vector2f{8, 167}, sf::Vector2f{8, 160}, sf::Vector2f{63, 160},
			sf::Vector2f{56, 167}, stairs_colour);
		_set_vertex_array(tileview.up, sf::Vector2f{8, 15}, sf::Vector2f{8, 8}, sf::Vector2f{56, 8},
			sf::Vector2f{63, 15}, stairs_colour);
		_set_vertex_array(tileview.darkness, sf::Vector2f{8, 152}, sf::Vector2f{8, 24}, sf::Vector2f{87, 24},
			sf::Vector2f{87, 152}, darkness_colour);
		_set_vertex_array(tileview.side_darkness, sf::Vector2f{8, 167}, sf::Vector2f{8, 8}, sf::Vector2f{87, 8},
			sf::Vector2f{87, 167}, darkness_colour);
		_set_vertex_array(
			tileview.back_wall, sf::Vector2f{8, 153}, sf::Vector2f{8, 23}, sf::Vector2f{88, 23}, sf::Vector2f{88, 153});
		_set_vertex_array(
			tileview.back_door, sf::Vector2f{8, 153}, sf::Vector2f{8, 23}, sf::Vector2f{88, 23}, sf::Vector2f{88, 153});

		_set_texture_coordinates(tileview);
	}

	// Tile to the Immediate Right of the Player
	{
		Coordinate3 tile{1, 0, 0};
		auto &tileview{tileviews.at(tile)};

		// Set Vertex Arrays
		_set_vertex_array(tileview.floor, sf::Vector2f{239, 167}, sf::Vector2f{232, 160}, sf::Vector2f{295, 160},
			sf::Vector2f{295, 167}, floor_colour);
		_set_vertex_array(tileview.ceiling, sf::Vector2f{232, 15}, sf::Vector2f{239, 8}, sf::Vector2f{295, 8},
			sf::Vector2f{295, 15}, ceiling_colour);
		_set_vertex_array(tileview.down, sf::Vector2f{239, 167}, sf::Vector2f{232, 160}, sf::Vector2f{295, 160},
			sf::Vector2f{295, 167}, stairs_colour);
		_set_vertex_array(tileview.up, sf::Vector2f{232, 15}, sf::Vector2f{239, 8}, sf::Vector2f{295, 8},
			sf::Vector2f{295, 15}, stairs_colour);
		_set_vertex_array(tileview.darkness, sf::Vector2f{216, 152}, sf::Vector2f{216, 24}, sf::Vector2f{295, 24},
			sf::Vector2f{295, 152}, darkness_colour);
		_set_vertex_array(tileview.side_darkness, sf::Vector2f{216, 167}, sf::Vector2f{216, 8}, sf::Vector2f{295, 8},
			sf::Vector2f{296, 167}, darkness_colour);
		_set_vertex_array(tileview.back_wall, sf::Vector2f{216, 153}, sf::Vector2f{216, 23}, sf::Vector2f{296, 23},
			sf::Vector2f{296, 153});
		_set_vertex_array(tileview.back_door, sf::Vector2f{216, 153}, sf::Vector2f{216, 23}, sf::Vector2f{296, 23},
			sf::Vector2f{296, 153});

		_set_texture_coordinates(tileview);
	}

	// Tile to the left and one in front
	{
		Coordinate3 tile{-1, 0, -1};
		auto &tileview{tileviews.at(tile)};

		// Set Vertex Arrays
		_set_vertex_array(tileview.floor, sf::Vector2f{16, 143}, sf::Vector2f{46, 128}, sf::Vector2f{95, 128},
			sf::Vector2f{80, 143}, floor_colour);
		_set_vertex_array(tileview.ceiling, sf::Vector2f{46, 47}, sf::Vector2f{16, 32}, sf::Vector2f{80, 32},
			sf::Vector2f{95, 47}, ceiling_colour);
		_set_vertex_array(tileview.down, sf::Vector2f{16, 143}, sf::Vector2f{46, 128}, sf::Vector2f{95, 128},
			sf::Vector2f{80, 143}, stairs_colour);
		_set_vertex_array(tileview.up, sf::Vector2f{46, 47}, sf::Vector2f{16, 32}, sf::Vector2f{80, 32},
			sf::Vector2f{95, 47}, stairs_colour);
		_set_vertex_array(tileview.darkness, sf::Vector2f{56, 119}, sf::Vector2f{56, 56}, sf::Vector2f{119, 56},
			sf::Vector2f{120, 119}, darkness_colour);
		_set_vertex_array(tileview.side_darkness, sf::Vector2f{88, 151}, sf::Vector2f{88, 24}, sf::Vector2f{119, 55},
			sf::Vector2f{119, 120}, darkness_colour);
		_set_vertex_array(tileview.back_wall, sf::Vector2f{55, 121}, sf::Vector2f{55, 55}, sf::Vector2f{120, 55},
			sf::Vector2f{120, 121});
		_set_vertex_array(tileview.back_door, sf::Vector2f{55, 121}, sf::Vector2f{55, 55}, sf::Vector2f{120, 55},
			sf::Vector2f{120, 121});

		_set_texture_coordinates(tileview);
	}

	// Tile to the left and two in front
	{
		Coordinate3 tile{-1, 0, -2};
		auto &tileview{tileviews.at(tile)};

		// Set Vertex Arrays
		_set_vertex_array(tileview.darkness, sf::Vector2f{105, 104}, sf::Vector2f{104, 72}, sf::Vector2f{135, 72},
			sf::Vector2f{136, 103}, darkness_colour);
		_set_vertex_array(tileview.side_darkness, sf::Vector2f{120, 119}, sf::Vector2f{120, 56}, sf::Vector2f{135, 71},
			sf::Vector2f{135, 104}, darkness_colour);
		_set_vertex_array(tileview.back_wall, sf::Vector2f{103, 105}, sf::Vector2f{103, 71}, sf::Vector2f{136, 71},
			sf::Vector2f{136, 105});
		_set_vertex_array(tileview.back_door, sf::Vector2f{103, 105}, sf::Vector2f{103, 71}, sf::Vector2f{136, 71},
			sf::Vector2f{136, 105});

		_set_texture_coordinates(tileview);
	}

	// Tile to the left and three in front
	{
		Coordinate3 tile{-1, 0, -3};
		auto &tileview{tileviews.at(tile)};

		// Set Vertex Arrays
		_set_vertex_array(tileview.darkness, sf::Vector2f{103, 103}, sf::Vector2f{104, 72}, sf::Vector2f{136, 72},
			sf::Vector2f{136, 103}, darkness_colour);
		_set_vertex_array(tileview.side_darkness, sf::Vector2f{136, 103}, sf::Vector2f{136, 72}, sf::Vector2f{144, 79},
			sf::Vector2f{144, 96}, darkness_colour);
		_set_vertex_array(tileview.back_wall, sf::Vector2f{127, 97}, sf::Vector2f{127, 79}, sf::Vector2f{144, 79},
			sf::Vector2f{144, 97});
		_set_vertex_array(tileview.back_door, sf::Vector2f{127, 97}, sf::Vector2f{127, 79}, sf::Vector2f{144, 79},
			sf::Vector2f{144, 97});

		_set_texture_coordinates(tileview);
	}

	// Tile to the left and four in front
	{
		Coordinate3 tile{-1, 0, -4};
		auto &tileview{tileviews.at(tile)};

		// Set Vertex Arrays
		_set_vertex_array(tileview.darkness, sf::Vector2f{126, 95}, sf::Vector2f{126, 80}, sf::Vector2f{143, 80},
			sf::Vector2f{144, 95}, darkness_colour);

		_set_texture_coordinates(tileview);
	}

	// Tile to the right and one in front
	{
		Coordinate3 tile{1, 0, -1};
		auto &tileview{tileviews.at(tile)};

		// Set Vertex Arrays
		_set_vertex_array(tileview.floor, sf::Vector2f{215, 143}, sf::Vector2f{200, 128}, sf::Vector2f{257, 128},
			sf::Vector2f{287, 143}, floor_colour);
		_set_vertex_array(tileview.ceiling, sf::Vector2f{200, 47}, sf::Vector2f{215, 32}, sf::Vector2f{287, 32},
			sf::Vector2f{257, 47}, ceiling_colour);
		_set_vertex_array(tileview.down, sf::Vector2f{215, 143}, sf::Vector2f{200, 128}, sf::Vector2f{257, 128},
			sf::Vector2f{287, 143}, stairs_colour);
		_set_vertex_array(tileview.up, sf::Vector2f{200, 47}, sf::Vector2f{215, 32}, sf::Vector2f{287, 32},
			sf::Vector2f{257, 47}, stairs_colour);
		_set_vertex_array(tileview.darkness, sf::Vector2f{184, 119}, sf::Vector2f{184, 56}, sf::Vector2f{247, 56},
			sf::Vector2f{247, 119}, darkness_colour);
		_set_vertex_array(tileview.side_darkness, sf::Vector2f{185, 120}, sf::Vector2f{185, 55}, sf::Vector2f{217, 24},
			sf::Vector2f{217, 151}, darkness_colour);
		_set_vertex_array(tileview.back_wall, sf::Vector2f{184, 121}, sf::Vector2f{184, 55}, sf::Vector2f{249, 55},
			sf::Vector2f{249, 121});
		_set_vertex_array(tileview.back_door, sf::Vector2f{184, 121}, sf::Vector2f{184, 55}, sf::Vector2f{249, 55},
			sf::Vector2f{249, 121});

		_set_texture_coordinates(tileview);
	}

	// Tile to the right and two in front
	{
		Coordinate3 tile{1, 0, -2};
		auto &tileview{tileviews.at(tile)};

		// Set Vertex Arrays
		_set_vertex_array(tileview.darkness, sf::Vector2f{168, 105}, sf::Vector2f{168, 72}, sf::Vector2f{199, 72},
			sf::Vector2f{199, 105}, darkness_colour);
		_set_vertex_array(tileview.side_darkness, sf::Vector2f{168, 105}, sf::Vector2f{168, 71}, sf::Vector2f{184, 56},
			sf::Vector2f{184, 119}, darkness_colour);
		_set_vertex_array(tileview.back_wall, sf::Vector2f{168, 105}, sf::Vector2f{168, 71}, sf::Vector2f{201, 71},
			sf::Vector2f{201, 105});
		_set_vertex_array(tileview.back_door, sf::Vector2f{168, 105}, sf::Vector2f{168, 71}, sf::Vector2f{201, 71},
			sf::Vector2f{201, 105});

		_set_texture_coordinates(tileview);
	}

	// Tile to the right and three in front
	{
		Coordinate3 tile{1, 0, -3};
		auto &tileview{tileviews.at(tile)};

		// Set Vertex Arrays
		_set_vertex_array(tileview.darkness, sf::Vector2f{168, 105}, sf::Vector2f{168, 72}, sf::Vector2f{199, 72},
			sf::Vector2f{199, 105}, darkness_colour);
		_set_vertex_array(tileview.side_darkness, sf::Vector2f{161, 98}, sf::Vector2f{161, 79}, sf::Vector2f{168, 72},
			sf::Vector2f{168, 105}, darkness_colour);
		_set_vertex_array(tileview.back_wall, sf::Vector2f{160, 97}, sf::Vector2f{160, 79}, sf::Vector2f{177, 79},
			sf::Vector2f{177, 97});
		_set_vertex_array(tileview.back_door, sf::Vector2f{160, 97}, sf::Vector2f{160, 79}, sf::Vector2f{177, 79},
			sf::Vector2f{177, 97});

		_set_texture_coordinates(tileview);
	}

	// Tile to the right and four in front
	{
		Coordinate3 tile{1, 0, -4};
		auto &tileview{tileviews.at(tile)};

		// Set Vertex Arrays
		_set_vertex_array(tileview.darkness, sf::Vector2f{160, 95}, sf::Vector2f{160, 80}, sf::Vector2f{176, 80},
			sf::Vector2f{176, 95}, darkness_colour);

		_set_texture_coordinates(tileview);
	}
}
