// Copyright (C) 2025 Dave Moore
//
// This file is part of Sorcery.
//
// Sorcery is free software: you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software
// Foundation, either version 2 of the License, or (at your option) any later
// version.
//
// Sorcery is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with
// Sorcery.  If not, see <http://www.gnu.org/licenses/>.
//
// If you modify this program, or any covered work, by linking or combining
// it with the libraries referred to in README (or a modified version of
// said libraries), containing parts covered by the terms of said libraries,
// the licensors of this program grant you additional permission to convey
// the resulting work.

#include "core/render.hpp"
#include "core/controller.hpp"
#include "core/display.hpp"
#include "core/system.hpp"
#include "core/ui.hpp"
#include "types/game.hpp"
#include "types/state.hpp"

// Standard Constructor
Sorcery::Render::Render(System *system, Display *display, UI *ui,
						Controller *controller)
	: _system{system},
	  _display{display},
	  _ui{ui},
	  _controller{controller} {

	_monochrome = false;
	_source_size = ImVec2{912.0f * 4, 880.0f * 4};
	_pane_size = ImVec2{304 * 4, 176 * 4};

	_load_tile_views();
}

auto Sorcery::Render::get_monochrome() const -> bool {

	return _monochrome;
}

auto Sorcery::Render::set_monochrome(bool value) -> void {

	_monochrome = value;

	_load_tile_views();
}

auto Sorcery::Render::_set_texture_coordinates(TileView &tileview) -> void {

	const auto size_panel_x{_pane_size.x};
	const auto size_panel_y{_pane_size.y};
	for (auto i = 0; i <= 3; i++) {
		tileview.floor[i].tex_coord = tileview.floor[i].position;
		tileview.ceiling[i].tex_coord = tileview.ceiling[i].position;
		tileview.up[i].tex_coord = tileview.up[i].position;
		tileview.down[i].tex_coord = tileview.down[i].position;
		tileview.darkness[i].tex_coord.x =
			tileview.darkness[i].position.x + (0 * size_panel_x);
		tileview.darkness[i].tex_coord.y =
			tileview.darkness[i].position.y + (1 * size_panel_y);
		tileview.side_darkness[i].tex_coord.x =
			tileview.side_darkness[i].position.x + (1 * size_panel_x);
		tileview.side_darkness[i].tex_coord.y =
			tileview.side_darkness[i].position.y + (0 * size_panel_y);
		tileview.left_side_wall[i].tex_coord.x =
			tileview.left_side_wall[i].position.x + (1 * size_panel_x);
		tileview.left_side_wall[i].tex_coord.y =
			tileview.left_side_wall[i].position.y + (1 * size_panel_y);
		tileview.right_side_wall[i].tex_coord.x =
			tileview.right_side_wall[i].position.x + (1 * size_panel_x);
		tileview.right_side_wall[i].tex_coord.y =
			tileview.right_side_wall[i].position.y + (1 * size_panel_y);
		tileview.left_side_door[i].tex_coord.x =
			tileview.left_side_door[i].position.x + (2 * size_panel_x);
		tileview.left_side_door[i].tex_coord.y =
			tileview.left_side_door[i].position.y + (0 * size_panel_y);
		tileview.right_side_door[i].tex_coord.x =
			tileview.right_side_door[i].position.x + (2 * size_panel_x);
		tileview.right_side_door[i].tex_coord.y =
			tileview.right_side_door[i].position.y + (0 * size_panel_y);

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
		tileview.back_wall[i].tex_coord.x =
			tileview.back_wall[i].position.x + (offset_x * size_panel_x);
		tileview.back_wall[i].tex_coord.y =
			tileview.back_wall[i].position.y + (offset_y * size_panel_y);

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
		tileview.back_door[i].tex_coord.x =
			tileview.back_door[i].position.x + (offset_x * size_panel_x);
		tileview.back_door[i].tex_coord.y =
			tileview.back_door[i].position.y + (offset_y * size_panel_y);
	}
}

// Note Texture Coordinates (e.g. the source) is set by _set_texture_coordinates
// above, not here
auto Sorcery::Render::_set_vertex_array(VertexArray &array, ImVec2 p1,
										ImVec2 p2, ImVec2 p3, ImVec2 p4)
	-> void {

	ImVec4 colour{1.0f, 1.0f, 1.0f, _system->animation->fade};

	// As we resized up the view, we resize it here too
	auto scale{4};
	array[0].position = ImVec2{p1.x * scale, p1.y * scale};
	array[1].position = ImVec2{p2.x * scale, p2.y * scale};
	array[2].position = ImVec2{p3.x * scale, p3.y * scale};
	array[3].position = ImVec2{p4.x * scale, p4.y * scale};
	array[0].colour = colour;
	array[1].colour = colour;
	array[2].colour = colour;
	array[3].colour = colour;
}

// Note Texture Coordinates (e.g. the source) is set by _set_texture_coordinates
// above, not here
auto Sorcery::Render::_set_vertex_array(VertexArray &array, ImVec2 p1,
										ImVec2 p2, ImVec2 p3, ImVec2 p4,
										const ImVec4 colour) -> void {

	auto col{_monochrome ? ImVec4{1.0f, 1.0f, 1.0f, _system->animation->fade}
						 : colour};

	// As we resized up the view, we resize it here too
	auto scale{4};
	array[0].position = ImVec2{p1.x * scale, p1.y * scale};
	array[1].position = ImVec2{p2.x * scale, p2.y * scale};
	array[2].position = ImVec2{p3.x * scale, p3.y * scale};
	array[3].position = ImVec2{p4.x * scale, p4.y * scale};
	array[0].colour = col;
	array[1].colour = col;
	array[2].colour = col;
	array[3].colour = col;
}

auto Sorcery::Render::_load_tile_views() -> void {

	_tileviews.clear();
	for (auto x = -1; x <= 1; x++) {
		for (auto z = 0; z >= -5; z--) {
			Coordinate3 loc{x, 0, z};
			_tileviews[loc] = TileView{loc};
		}
	}

	ImVec4 monochrome_colour{1.0f, 1.0f, 1.0f, _system->animation->fade};
	ImVec4 floor_colour{0.33f, 1.0f, 1.0f, _system->animation->fade};
	ImVec4 ceiling_colour{1.0f, 0.33f, 0.33f, _system->animation->fade};
	ImVec4 stairs_colour{1.0f, 0.33f, 0.33f, _system->animation->fade};
	ImVec4 elevator_colour{1.0f, 0.33f, 0.33f, _system->animation->fade};
	ImVec4 darkness_colour{0.33f, 1.0f, 1.0f, _system->animation->fade};

	//  FLOORS/CEILINGS				SIDE DARKNESS			SIDE DOORS
	//	FRONT DARKNESS				SIDE WALLS				FRONT DOORS z =
	// 0
	//  FRONT DOORS z = -1			FRONT DOORS z = -2		FRONT DOORS z =
	//  -3
	//  FRONT WALLS z = 0			FRONT WALLS z = -1		FRONT WALLS z =
	//  -2 FRONT WALLS z = -3

	// Tile the player is standing on
	{
		Coordinate3 tile{0, 0, 0};
		auto &tileview{_tileviews.at(tile)};

		// Set Vertex Arrays
		_set_vertex_array(tileview.floor, ImVec2{88, 167}, ImVec2{95, 160},
						  ImVec2{208, 160}, ImVec2{215, 167}, floor_colour);
		_set_vertex_array(tileview.ceiling, ImVec2{95, 15}, ImVec2{88, 8},
						  ImVec2{215, 8}, ImVec2{208, 15}, ceiling_colour);
		_set_vertex_array(tileview.down, ImVec2{88, 167}, ImVec2{95, 160},
						  ImVec2{208, 160}, ImVec2{215, 167}, stairs_colour);
		_set_vertex_array(tileview.up, ImVec2{95, 15}, ImVec2{88, 8},
						  ImVec2{215, 8}, ImVec2{208, 15}, stairs_colour);
		_set_vertex_array(tileview.darkness, ImVec2{8, 167}, ImVec2{8, 8},
						  ImVec2{296, 8}, ImVec2{296, 167}, darkness_colour);
		_set_vertex_array(tileview.back_wall, ImVec2{87, 153}, ImVec2{87, 23},
						  ImVec2{217, 23}, ImVec2{217, 153});
		_set_vertex_array(tileview.back_door, ImVec2{87, 153}, ImVec2{87, 23},
						  ImVec2{217, 23}, ImVec2{217, 153});
		_set_vertex_array(tileview.left_side_wall, ImVec2{8, 167}, ImVec2{8, 8},
						  ImVec2{87, 8}, ImVec2{87, 167});
		_set_vertex_array(tileview.left_side_door, ImVec2{8, 167}, ImVec2{8, 8},
						  ImVec2{87, 8}, ImVec2{87, 167});
		_set_vertex_array(tileview.right_side_wall, ImVec2{216, 167},
						  ImVec2{216, 8}, ImVec2{296, 8}, ImVec2{296, 167});
		_set_vertex_array(tileview.right_side_door, ImVec2{216, 167},
						  ImVec2{216, 8}, ImVec2{296, 8}, ImVec2{296, 167});

		_set_texture_coordinates(tileview);
	}

	// Tile in Front of the Player
	{
		Coordinate3 tile{0, 0, -1};
		auto &tileview{_tileviews.at(tile)};

		// Set Vertex Arrays
		_set_vertex_array(tileview.floor, ImVec2{112, 143}, ImVec2{127, 128},
						  ImVec2{176, 128}, ImVec2{191, 143}, floor_colour);
		_set_vertex_array(tileview.ceiling, ImVec2{127, 47}, ImVec2{112, 32},
						  ImVec2{191, 32}, ImVec2{176, 47}, ceiling_colour);
		_set_vertex_array(tileview.down, ImVec2{112, 143}, ImVec2{127, 128},
						  ImVec2{176, 128}, ImVec2{191, 143}, stairs_colour);
		_set_vertex_array(tileview.up, ImVec2{127, 47}, ImVec2{112, 32},
						  ImVec2{191, 32}, ImVec2{176, 47}, stairs_colour);
		_set_vertex_array(tileview.darkness, ImVec2{87, 152}, ImVec2{87, 23},
						  ImVec2{216, 23}, ImVec2{216, 152}, darkness_colour);
		_set_vertex_array(tileview.back_wall, ImVec2{119, 121}, ImVec2{119, 55},
						  ImVec2{185, 55}, ImVec2{185, 121});
		_set_vertex_array(tileview.back_door, ImVec2{119, 121}, ImVec2{119, 55},
						  ImVec2{185, 55}, ImVec2{185, 121});

		_set_vertex_array(tileview.left_side_wall, ImVec2{87, 153},
						  ImVec2{87, 23}, ImVec2{120, 23}, ImVec2{120, 153});
		_set_vertex_array(tileview.left_side_door, ImVec2{87, 153},
						  ImVec2{87, 23}, ImVec2{120, 23}, ImVec2{120, 153});
		_set_vertex_array(tileview.right_side_wall, ImVec2{184, 121},
						  ImVec2{184, 55}, ImVec2{217, 23}, ImVec2{217, 153});
		_set_vertex_array(tileview.right_side_door, ImVec2{184, 121},
						  ImVec2{184, 55}, ImVec2{217, 23}, ImVec2{217, 153});

		_set_texture_coordinates(tileview);
	}

	// Tile 2 in Front of the Player
	{
		Coordinate3 tile{0, 0, -2};
		auto &tileview{_tileviews.at(tile)};

		// Set Vertex Arrays
		_set_vertex_array(tileview.darkness, ImVec2{120, 119}, ImVec2{120, 56},
						  ImVec2{183, 56}, ImVec2{183, 119}, darkness_colour);
		_set_vertex_array(tileview.back_wall, ImVec2{135, 105}, ImVec2{135, 71},
						  ImVec2{169, 71}, ImVec2{169, 105});
		_set_vertex_array(tileview.back_door, ImVec2{135, 105}, ImVec2{139, 71},
						  ImVec2{169, 71}, ImVec2{168, 105});
		_set_vertex_array(tileview.left_side_wall, ImVec2{119, 120},
						  ImVec2{119, 54}, ImVec2{135, 71}, ImVec2{135, 120});
		_set_vertex_array(tileview.left_side_door, ImVec2{119, 120},
						  ImVec2{119, 54}, ImVec2{135, 71}, ImVec2{135, 120});
		_set_vertex_array(tileview.right_side_wall, ImVec2{168, 105},
						  ImVec2{168, 71}, ImVec2{184, 54}, ImVec2{184, 120});
		_set_vertex_array(tileview.right_side_door, ImVec2{168, 105},
						  ImVec2{168, 71}, ImVec2{184, 54}, ImVec2{184, 120});

		_set_texture_coordinates(tileview);
	}

	// Tile 3 in Front of the Player
	{
		Coordinate3 tile{0, 0, -3};
		auto &tileview{_tileviews.at(tile)};

		// Set Vertex Arrays
		_set_vertex_array(tileview.darkness, ImVec2{135, 104}, ImVec2{135, 71},
						  ImVec2{168, 71}, ImVec2{168, 104}, darkness_colour);
		_set_vertex_array(tileview.back_wall, ImVec2{143, 97}, ImVec2{143, 79},
						  ImVec2{161, 79}, ImVec2{161, 97});
		_set_vertex_array(tileview.back_door, ImVec2{143, 97}, ImVec2{143, 79},
						  ImVec2{161, 79}, ImVec2{161, 97});
		_set_vertex_array(tileview.left_side_wall, ImVec2{135, 105},
						  ImVec2{135, 71}, ImVec2{144, 79}, ImVec2{144, 105});
		_set_vertex_array(tileview.left_side_door, ImVec2{135, 105},
						  ImVec2{135, 71}, ImVec2{144, 79}, ImVec2{144, 105});
		_set_vertex_array(tileview.right_side_wall, ImVec2{160, 97},
						  ImVec2{160, 79}, ImVec2{168, 71}, ImVec2{168, 105});
		_set_vertex_array(tileview.right_side_door, ImVec2{160, 97},
						  ImVec2{160, 79}, ImVec2{168, 71}, ImVec2{168, 105});

		_set_texture_coordinates(tileview);
	}

	// Tile 4 in Front of the Player
	{
		Coordinate3 tile{0, 0, -4};
		auto &tileview{_tileviews.at(tile)};

		// Set Vertex Arrays
		_set_vertex_array(tileview.darkness, ImVec2{143, 96}, ImVec2{143, 79},
						  ImVec2{160, 79}, ImVec2{160, 96}, darkness_colour);

		_set_texture_coordinates(tileview);
	}

	// Tile to the Immediate Left of the Player
	{
		Coordinate3 tile{-1, 0, 0};
		auto &tileview{_tileviews.at(tile)};

		// Set Vertex Arrays
		_set_vertex_array(tileview.floor, ImVec2{8, 167}, ImVec2{8, 160},
						  ImVec2{63, 160}, ImVec2{56, 167}, floor_colour);
		_set_vertex_array(tileview.ceiling, ImVec2{8, 15}, ImVec2{8, 8},
						  ImVec2{56, 8}, ImVec2{63, 15}, ceiling_colour);
		_set_vertex_array(tileview.down, ImVec2{8, 167}, ImVec2{8, 160},
						  ImVec2{63, 160}, ImVec2{56, 167}, stairs_colour);
		_set_vertex_array(tileview.up, ImVec2{8, 15}, ImVec2{8, 8},
						  ImVec2{56, 8}, ImVec2{63, 15}, stairs_colour);
		_set_vertex_array(tileview.darkness, ImVec2{8, 152}, ImVec2{8, 24},
						  ImVec2{87, 24}, ImVec2{87, 152}, darkness_colour);
		_set_vertex_array(tileview.side_darkness, ImVec2{8, 167}, ImVec2{8, 8},
						  ImVec2{87, 8}, ImVec2{87, 167}, darkness_colour);
		_set_vertex_array(tileview.back_wall, ImVec2{8, 153}, ImVec2{8, 23},
						  ImVec2{88, 23}, ImVec2{88, 153});
		_set_vertex_array(tileview.back_door, ImVec2{8, 153}, ImVec2{8, 23},
						  ImVec2{88, 23}, ImVec2{88, 153});

		_set_texture_coordinates(tileview);
	}

	// Tile to the Immediate Right of the Player
	{
		Coordinate3 tile{1, 0, 0};
		auto &tileview{_tileviews.at(tile)};

		// Set Vertex Arrays
		_set_vertex_array(tileview.floor, ImVec2{239, 167}, ImVec2{232, 160},
						  ImVec2{295, 160}, ImVec2{295, 167}, floor_colour);
		_set_vertex_array(tileview.ceiling, ImVec2{232, 15}, ImVec2{239, 8},
						  ImVec2{295, 8}, ImVec2{295, 15}, ceiling_colour);
		_set_vertex_array(tileview.down, ImVec2{239, 167}, ImVec2{232, 160},
						  ImVec2{295, 160}, ImVec2{295, 167}, stairs_colour);
		_set_vertex_array(tileview.up, ImVec2{232, 15}, ImVec2{239, 8},
						  ImVec2{295, 8}, ImVec2{295, 15}, stairs_colour);
		_set_vertex_array(tileview.darkness, ImVec2{216, 152}, ImVec2{216, 24},
						  ImVec2{295, 24}, ImVec2{295, 152}, darkness_colour);
		_set_vertex_array(tileview.side_darkness, ImVec2{216, 167},
						  ImVec2{216, 8}, ImVec2{295, 8}, ImVec2{296, 167},
						  darkness_colour);
		_set_vertex_array(tileview.back_wall, ImVec2{216, 153}, ImVec2{216, 23},
						  ImVec2{296, 23}, ImVec2{296, 153});
		_set_vertex_array(tileview.back_door, ImVec2{216, 153}, ImVec2{216, 23},
						  ImVec2{296, 23}, ImVec2{296, 153});

		_set_texture_coordinates(tileview);
	}

	// Tile to the left and one in front
	{
		Coordinate3 tile{-1, 0, -1};
		auto &tileview{_tileviews.at(tile)};

		// Set Vertex Arrays
		_set_vertex_array(tileview.floor, ImVec2{16, 143}, ImVec2{46, 128},
						  ImVec2{95, 128}, ImVec2{80, 143}, floor_colour);
		_set_vertex_array(tileview.ceiling, ImVec2{46, 47}, ImVec2{16, 32},
						  ImVec2{80, 32}, ImVec2{95, 47}, ceiling_colour);
		_set_vertex_array(tileview.down, ImVec2{16, 143}, ImVec2{46, 128},
						  ImVec2{95, 128}, ImVec2{80, 143}, stairs_colour);
		_set_vertex_array(tileview.up, ImVec2{46, 47}, ImVec2{16, 32},
						  ImVec2{80, 32}, ImVec2{95, 47}, stairs_colour);
		_set_vertex_array(tileview.darkness, ImVec2{56, 119}, ImVec2{56, 56},
						  ImVec2{119, 56}, ImVec2{120, 119}, darkness_colour);
		_set_vertex_array(tileview.side_darkness, ImVec2{88, 151},
						  ImVec2{88, 24}, ImVec2{119, 55}, ImVec2{119, 120},
						  darkness_colour);
		_set_vertex_array(tileview.back_wall, ImVec2{55, 121}, ImVec2{55, 55},
						  ImVec2{120, 55}, ImVec2{120, 121});
		_set_vertex_array(tileview.back_door, ImVec2{55, 121}, ImVec2{55, 55},
						  ImVec2{120, 55}, ImVec2{120, 121});

		_set_texture_coordinates(tileview);
	}

	// Tile to the left and two in front
	{
		Coordinate3 tile{-1, 0, -2};
		auto &tileview{_tileviews.at(tile)};

		// Set Vertex Arrays
		_set_vertex_array(tileview.darkness, ImVec2{105, 104}, ImVec2{104, 72},
						  ImVec2{135, 72}, ImVec2{136, 103}, darkness_colour);
		_set_vertex_array(tileview.side_darkness, ImVec2{120, 119},
						  ImVec2{120, 56}, ImVec2{135, 71}, ImVec2{135, 104},
						  darkness_colour);
		_set_vertex_array(tileview.back_wall, ImVec2{103, 105}, ImVec2{103, 71},
						  ImVec2{136, 71}, ImVec2{136, 105});
		_set_vertex_array(tileview.back_door, ImVec2{103, 105}, ImVec2{103, 71},
						  ImVec2{136, 71}, ImVec2{136, 105});

		_set_texture_coordinates(tileview);
	}

	// Tile to the left and three in front
	{
		Coordinate3 tile{-1, 0, -3};
		auto &tileview{_tileviews.at(tile)};

		// Set Vertex Arrays
		_set_vertex_array(tileview.darkness, ImVec2{103, 103}, ImVec2{104, 72},
						  ImVec2{136, 72}, ImVec2{136, 103}, darkness_colour);
		_set_vertex_array(tileview.side_darkness, ImVec2{136, 103},
						  ImVec2{136, 72}, ImVec2{144, 79}, ImVec2{144, 96},
						  darkness_colour);
		_set_vertex_array(tileview.back_wall, ImVec2{127, 97}, ImVec2{127, 79},
						  ImVec2{144, 79}, ImVec2{144, 97});
		_set_vertex_array(tileview.back_door, ImVec2{127, 97}, ImVec2{127, 79},
						  ImVec2{144, 79}, ImVec2{144, 97});

		_set_texture_coordinates(tileview);
	}

	// Tile to the left and four in front
	{
		Coordinate3 tile{-1, 0, -4};
		auto &tileview{_tileviews.at(tile)};

		// Set Vertex Arrays
		_set_vertex_array(tileview.darkness, ImVec2{126, 95}, ImVec2{126, 80},
						  ImVec2{143, 80}, ImVec2{144, 95}, darkness_colour);

		_set_texture_coordinates(tileview);
	}

	// Tile to the right and one in front
	{
		Coordinate3 tile{1, 0, -1};
		auto &tileview{_tileviews.at(tile)};

		// Set Vertex Arrays
		_set_vertex_array(tileview.floor, ImVec2{215, 143}, ImVec2{200, 128},
						  ImVec2{257, 128}, ImVec2{287, 143}, floor_colour);
		_set_vertex_array(tileview.ceiling, ImVec2{200, 47}, ImVec2{215, 32},
						  ImVec2{287, 32}, ImVec2{257, 47}, ceiling_colour);
		_set_vertex_array(tileview.down, ImVec2{215, 143}, ImVec2{200, 128},
						  ImVec2{257, 128}, ImVec2{287, 143}, stairs_colour);
		_set_vertex_array(tileview.up, ImVec2{200, 47}, ImVec2{215, 32},
						  ImVec2{287, 32}, ImVec2{257, 47}, stairs_colour);
		_set_vertex_array(tileview.darkness, ImVec2{184, 119}, ImVec2{184, 56},
						  ImVec2{247, 56}, ImVec2{247, 119}, darkness_colour);
		_set_vertex_array(tileview.side_darkness, ImVec2{185, 120},
						  ImVec2{185, 55}, ImVec2{217, 24}, ImVec2{217, 151},
						  darkness_colour);
		_set_vertex_array(tileview.back_wall, ImVec2{184, 121}, ImVec2{184, 55},
						  ImVec2{249, 55}, ImVec2{249, 121});
		_set_vertex_array(tileview.back_door, ImVec2{184, 121}, ImVec2{184, 55},
						  ImVec2{249, 55}, ImVec2{249, 121});

		_set_texture_coordinates(tileview);
	}

	// Tile to the right and two in front
	{
		Coordinate3 tile{1, 0, -2};
		auto &tileview{_tileviews.at(tile)};

		// Set Vertex Arrays
		_set_vertex_array(tileview.darkness, ImVec2{168, 105}, ImVec2{168, 72},
						  ImVec2{199, 72}, ImVec2{199, 105}, darkness_colour);
		_set_vertex_array(tileview.side_darkness, ImVec2{168, 105},
						  ImVec2{168, 71}, ImVec2{184, 56}, ImVec2{184, 119},
						  darkness_colour);
		_set_vertex_array(tileview.back_wall, ImVec2{168, 105}, ImVec2{168, 71},
						  ImVec2{201, 71}, ImVec2{201, 105});
		_set_vertex_array(tileview.back_door, ImVec2{168, 105}, ImVec2{168, 71},
						  ImVec2{201, 71}, ImVec2{201, 105});

		_set_texture_coordinates(tileview);
	}

	// Tile to the right and three in front
	{
		Coordinate3 tile{1, 0, -3};
		auto &tileview{_tileviews.at(tile)};

		// Set Vertex Arrays
		_set_vertex_array(tileview.darkness, ImVec2{168, 105}, ImVec2{168, 72},
						  ImVec2{199, 72}, ImVec2{199, 105}, darkness_colour);
		_set_vertex_array(tileview.side_darkness, ImVec2{161, 98},
						  ImVec2{161, 79}, ImVec2{168, 72}, ImVec2{168, 105},
						  darkness_colour);
		_set_vertex_array(tileview.back_wall, ImVec2{160, 97}, ImVec2{160, 79},
						  ImVec2{177, 79}, ImVec2{177, 97});
		_set_vertex_array(tileview.back_door, ImVec2{160, 97}, ImVec2{160, 79},
						  ImVec2{177, 79}, ImVec2{177, 97});

		_set_texture_coordinates(tileview);
	}

	// Tile to the right and four in front
	{
		Coordinate3 tile{1, 0, -4};
		auto &tileview{_tileviews.at(tile)};

		// Set Vertex Arrays
		_set_vertex_array(tileview.darkness, ImVec2{160, 95}, ImVec2{160, 80},
						  ImVec2{176, 80}, ImVec2{176, 95}, darkness_colour);

		_set_texture_coordinates(tileview);
	}
}

auto Sorcery::Render::_get_left_side(const Enums::Map::Direction facing) const
	-> Enums::Map::Direction {

	using Enums::Map::Direction;

	switch (facing) {
	case Direction::NORTH:
		return Direction::WEST;
		break;
	case Direction::SOUTH:
		return Direction::EAST;
		break;
	case Direction::EAST:
		return Direction::NORTH;
		break;
	case Direction::WEST:
		return Direction::SOUTH;
		break;
	default:
		return facing;
		break;
	}
}

auto Sorcery::Render::_get_right_side(const Enums::Map::Direction facing) const
	-> Enums::Map::Direction {

	using Enums::Map::Direction;

	switch (facing) {
	case Direction::NORTH:
		return Direction::EAST;
		break;
	case Direction::SOUTH:
		return Direction::WEST;
		break;
	case Direction::EAST:
		return Direction::SOUTH;
		break;
	case Direction::WEST:
		return Direction::NORTH;
		break;
	default:
		return facing;
		break;
	}
}

auto Sorcery::Render::_has_secret_door(
	const Tile &tile, const Sorcery::Enums::Map::Direction direction) const
	-> bool {

	using Enums::Tile::Edge;

	return ((tile.has(direction, Edge::ONE_WAY_HIDDEN_DOOR)) ||
			(tile.has(direction, Edge::SECRET_DOOR)));
}

auto Sorcery::Render::_has_normal_door(
	const Tile &tile, const Sorcery::Enums::Map::Direction direction) const
	-> bool {

	using Enums::Tile::Edge;

	return ((tile.has(direction, Edge::ONE_WAY_DOOR)) ||
			(tile.has(direction, Edge::UNLOCKED_DOOR)) ||
			(tile.has(direction, Edge::LOCKED_DOOR)));
}

auto Sorcery::Render::_has_wall(
	const Tile &tile, const Sorcery::Enums::Map::Direction direction) const
	-> bool {

	using Enums::Tile::Edge;

	return ((tile.has(direction, Edge::WALL)) ||
			(tile.has(direction, Edge::ONE_WAY_WALL)));
}

auto Sorcery::Render::draw(Game *game, Component *component) -> void {

	_render_wireframe(game, component);
}

auto Sorcery::Render::_render_wireframe(Game *game, Component *component)
	-> void {

	using Enums::Tile::Features;
	using Enums::Tile::Properties;

	const auto player_pos{game->state->get_player_pos()};
	const auto player_facing{game->state->get_player_facing()};
	// const auto scale{std::stof((*component)["scale"].value())};
	const auto scale{1.0f};
	const auto width{scale * _pane_size.x};
	const auto height{scale * _pane_size.y};
	const auto x{std::invoke([&] {
		if (component->x == -1) {
			const auto viewport{ImGui::GetMainViewport()};
			return (viewport->Size.x - width) / 2;
		} else
			return static_cast<float>(width);
	})};
	const auto y{std::invoke([&] {
		if (component->y == -1) {
			const auto viewport{ImGui::GetMainViewport()};
			return (viewport->Size.y - height) / 2;
		} else
			return static_cast<float>(height);
	})};
	const ImVec2 pos{x, y};

	// TODO - change level at to use z- into the screen
	const auto tl0{game->state->level->at(player_pos, player_facing, -1, 0)};
	auto vl0{_tileviews.at(Coordinate3{-1, 0, 0})};
	const auto tm0{game->state->level->at(player_pos, player_facing, 0, 0)};
	auto vm0{_tileviews.at(Coordinate3{0, 0, 0})};
	const auto tr0{game->state->level->at(player_pos, player_facing, 1, 0)};
	auto vr0{_tileviews.at(Coordinate3{1, 0, 0})};

	const auto tl1{game->state->level->at(player_pos, player_facing, -1, 1)};
	auto vl1{_tileviews.at(Coordinate3{-1, 0, -1})};
	const auto tm1{game->state->level->at(player_pos, player_facing, 0, 1)};
	auto vm1{_tileviews.at(Coordinate3{0, 0, -1})};
	const auto tr1{game->state->level->at(player_pos, player_facing, 1, 1)};
	auto vr1{_tileviews.at(Coordinate3{1, 0, -1})};

	const auto tl2{game->state->level->at(player_pos, player_facing, -1, 2)};
	auto vl2{_tileviews.at(Coordinate3{-1, 0, -2})};
	const auto tm2{game->state->level->at(player_pos, player_facing, 0, 2)};
	auto vm2{_tileviews.at(Coordinate3{0, 0, -2})};
	const auto tr2{game->state->level->at(player_pos, player_facing, 1, 2)};
	auto vr2{_tileviews.at(Coordinate3{1, 0, -2})};

	const auto tl3{game->state->level->at(player_pos, player_facing, -1, 3)};
	auto vl3{_tileviews.at(Coordinate3{-1, 0, -3})};
	const auto tm3{game->state->level->at(player_pos, player_facing, 0, 3)};
	auto vm3{_tileviews.at(Coordinate3{0, 0, -3})};
	const auto tr3{game->state->level->at(player_pos, player_facing, 1, 3)};
	auto vr3{_tileviews.at(Coordinate3{1, 0, -3})};

	const auto tl4{game->state->level->at(player_pos, player_facing, -1, 4)};
	auto vl4{_tileviews.at(Coordinate3{-1, 0, -4})};
	const auto tm4{game->state->level->at(player_pos, player_facing, 0, 4)};
	auto vm4{_tileviews.at(Coordinate3{0, 0, -4})};
	const auto tr4{game->state->level->at(player_pos, player_facing, 1, 4)};
	auto vr4{_tileviews.at(Coordinate3{1, 0, -4})};

	// If we are in darkness, only draw that!
	if (tm0.is(Properties::DARKNESS)) {
		_draw_vertex_array(vm0.darkness, scale, pos);
	} else {

		if (game->state->get_lit()) {

			// Row 4
			if (tl4.is(Properties::DARKNESS))
				_draw_vertex_array(vl4.darkness, scale, pos);
			if (tm4.is(Properties::DARKNESS))
				_draw_vertex_array(vm4.darkness, scale, pos);
			if (tr4.is(Properties::DARKNESS))
				_draw_vertex_array(vr4.darkness, scale, pos);

			// Row 3
			if (tl3.is(Properties::DARKNESS)) {
				_draw_vertex_array(vl2.darkness, scale, pos);
				_draw_vertex_array(vl3.side_darkness, scale, pos);
			} else {
				if (!tm3.is(Properties::DARKNESS)) {
					if (_has_wall(tl3, player_facing))
						_draw_vertex_array(vl3.back_wall, scale, pos);
					if (_has_normal_door(tl3, player_facing)) {
						_draw_vertex_array(vl3.back_wall, scale, pos);
						_draw_vertex_array(vl3.back_door, scale, pos);
					}
					if (_has_secret_door(tl3, player_facing)) {
						_draw_vertex_array(vl3.back_wall, scale, pos);
						if (game->state->get_lit())
							_draw_vertex_array(vl3.back_door, scale, pos);
					}
				}
			}
			if (tr3.is(Properties::DARKNESS)) {
				_draw_vertex_array(vr2.darkness, scale, pos);
				_draw_vertex_array(vr3.side_darkness, scale, pos);
			} else {
				if (!tm3.is(Properties::DARKNESS)) {
					if (_has_wall(tr3, player_facing))
						_draw_vertex_array(vr3.back_wall, scale, pos);
					if (_has_normal_door(tr3, player_facing)) {
						_draw_vertex_array(vr3.back_wall, scale, pos);
						_draw_vertex_array(vr3.back_door, scale, pos);
					}
					if (_has_secret_door(tr3, player_facing)) {
						_draw_vertex_array(vr3.back_wall, scale, pos);
						if (game->state->get_lit())
							_draw_vertex_array(vr3.back_door, scale, pos);
					}
				}
			}
			if (tm3.is(Properties::DARKNESS))
				_draw_vertex_array(vm3.darkness, scale, pos);
			else {
				if (_has_wall(tm3, player_facing))
					_draw_vertex_array(vm3.back_wall, scale, pos);
				if (_has_normal_door(tm3, player_facing)) {
					_draw_vertex_array(vm3.back_wall, scale, pos);
					_draw_vertex_array(vm3.back_door, scale, pos);
				}
				if (_has_secret_door(tm3, player_facing)) {
					_draw_vertex_array(vm3.back_wall, scale, pos);
					if (game->state->get_lit())
						_draw_vertex_array(vm3.back_door, scale, pos);
				}
			}

			if (!tm3.is(Properties::DARKNESS)) {
				if (_has_wall(tm3, _get_left_side(player_facing)))
					_draw_vertex_array(vm3.left_side_wall, scale, pos);
				if (_has_normal_door(tm3, _get_left_side(player_facing))) {
					_draw_vertex_array(vm3.left_side_wall, scale, pos);
					_draw_vertex_array(vm3.left_side_door, scale, pos);
				}
				if (_has_secret_door(tm3, _get_left_side(player_facing))) {
					_draw_vertex_array(vm3.left_side_wall, scale, pos);
					if (game->state->get_lit())
						_draw_vertex_array(vm3.left_side_door, scale, pos);
				}

				if (_has_wall(tm3, _get_right_side(player_facing)))
					_draw_vertex_array(vm3.right_side_wall, scale, pos);
				if (_has_normal_door(tm3, _get_right_side(player_facing))) {
					_draw_vertex_array(vm3.right_side_wall, scale, pos);
					if (game->state->get_lit())
						_draw_vertex_array(vm3.right_side_door, scale, pos);
				}
				if (_has_secret_door(tm3, _get_right_side(player_facing))) {
					_draw_vertex_array(vm3.right_side_wall, scale, pos);
					if (game->state->get_lit())
						_draw_vertex_array(vm3.right_side_door, scale, pos);
				}
			}

			// Row 2
			if (tl2.is(Properties::DARKNESS)) {
				_draw_vertex_array(vl1.darkness, scale, pos);
				_draw_vertex_array(vl2.side_darkness, scale, pos);
			} else {
				if (_has_wall(tl2, player_facing))
					_draw_vertex_array(vl2.back_wall, scale, pos);
				if (_has_normal_door(tl2, player_facing)) {
					_draw_vertex_array(vl2.back_wall, scale, pos);
					_draw_vertex_array(vl2.back_door, scale, pos);
				}
				if (_has_secret_door(tl2, player_facing)) {
					_draw_vertex_array(vl2.back_wall, scale, pos);
					if (game->state->get_lit())
						_draw_vertex_array(vl2.back_door, scale, pos);
				}
			}
			if (tr2.is(Properties::DARKNESS)) {
				_draw_vertex_array(vr1.darkness, scale, pos);
				_draw_vertex_array(vr2.side_darkness, scale, pos);
			} else {
				if (_has_wall(tr2, player_facing))
					_draw_vertex_array(vr2.back_wall, scale, pos);
				if (_has_normal_door(tr2, player_facing)) {
					_draw_vertex_array(vr2.back_wall, scale, pos);
					_draw_vertex_array(vr2.back_door, scale, pos);
				}
				if (_has_secret_door(tr2, player_facing)) {
					_draw_vertex_array(vr2.back_wall, scale, pos);
					if (game->state->get_lit())
						_draw_vertex_array(vr2.back_door, scale, pos);
				}
			}
			if (tm2.is(Properties::DARKNESS))
				_draw_vertex_array(vm2.darkness, scale, pos);
			else {
				if (_has_wall(tm2, player_facing))
					_draw_vertex_array(vm2.back_wall, scale, pos);
				if (_has_normal_door(tm2, player_facing)) {
					_draw_vertex_array(vm2.back_wall, scale, pos);
					_draw_vertex_array(vm2.back_door, scale, pos);
				}
				if (_has_secret_door(tm2, player_facing)) {
					_draw_vertex_array(vm2.back_wall, scale, pos);
					if (game->state->get_lit())
						_draw_vertex_array(vm2.back_door, scale, pos);
				}
			}

			if (!tm2.is(Properties::DARKNESS)) {
				if (_has_wall(tm2, _get_left_side(player_facing)))
					_draw_vertex_array(vm2.left_side_wall, scale, pos);
				if (_has_normal_door(tm2, _get_left_side(player_facing))) {
					_draw_vertex_array(vm2.left_side_wall, scale, pos);
					_draw_vertex_array(vm2.left_side_door, scale, pos);
				}
				if (_has_secret_door(tm2, _get_left_side(player_facing))) {
					_draw_vertex_array(vm2.left_side_wall, scale, pos);
					if (game->state->get_lit())
						_draw_vertex_array(vm2.left_side_door, scale, pos);
				}

				if (_has_wall(tm2, _get_right_side(player_facing)))
					_draw_vertex_array(vm2.right_side_wall, scale, pos);
				if (_has_normal_door(tm2, _get_right_side(player_facing))) {
					_draw_vertex_array(vm2.right_side_wall, scale, pos);
					_draw_vertex_array(vm2.right_side_door, scale, pos);
				}
				if (_has_secret_door(tm2, _get_right_side(player_facing))) {
					_draw_vertex_array(vm2.right_side_wall, scale, pos);
					if (game->state->get_lit())
						_draw_vertex_array(vm2.right_side_door, scale, pos);
				}
			}
		} else {

			if (tl2.is(Properties::DARKNESS))
				_draw_vertex_array(vl1.darkness, scale, pos);
			if (tm2.is(Properties::DARKNESS))
				_draw_vertex_array(vm2.darkness, scale, pos);
			if (tr2.is(Properties::DARKNESS))
				_draw_vertex_array(vr1.darkness, scale, pos);
		}

		// Row 1
		if (tl1.is(Properties::DARKNESS)) {
			_draw_vertex_array(vl0.darkness, scale, pos);
			_draw_vertex_array(vl1.side_darkness, scale, pos);
		} else {
			if (_has_wall(tl1, player_facing))
				_draw_vertex_array(vl1.back_wall, scale, pos);
			if (_has_normal_door(tl1, player_facing)) {
				_draw_vertex_array(vl1.back_wall, scale, pos);
				_draw_vertex_array(vl1.back_door, scale, pos);
			}
			if (_has_secret_door(tl1, player_facing)) {
				_draw_vertex_array(vl1.back_wall, scale, pos);
				if (game->state->get_lit())
					_draw_vertex_array(vl1.back_door, scale, pos);
			}

			if (tl1.has(Features::MESSAGE) || tl1.has(Features::NOTICE))
				_draw_vertex_array(vl1.floor, scale, pos);
			if (tl1.has(Features::STAIRS_DOWN) ||
				tl1.has(Features::LADDER_DOWN) ||
				tl1.has(Features::ELEVATOR_DOWN))
				_draw_vertex_array(vl1.down, scale, pos);
			if (tl1.has(Features::STAIRS_UP) || tl1.has(Features::LADDER_UP) ||
				tl1.has(Features::ELEVATOR_UP))
				_draw_vertex_array(vl1.up, scale, pos);
		}

		if (tr1.is(Properties::DARKNESS)) {
			_draw_vertex_array(vr0.darkness, scale, pos);
			_draw_vertex_array(vr1.side_darkness, scale, pos);
		} else {
			if (_has_wall(tr1, player_facing))
				_draw_vertex_array(vr1.back_wall, scale, pos);
			if (_has_normal_door(tr1, player_facing)) {
				_draw_vertex_array(vr1.back_wall, scale, pos);
				_draw_vertex_array(vr1.back_door, scale, pos);
			}
			if (_has_secret_door(tr1, player_facing)) {
				_draw_vertex_array(vr1.back_wall, scale, pos);
				if (game->state->get_lit())
					_draw_vertex_array(vr1.back_door, scale, pos);
			}
			if (tr1.has(Features::MESSAGE) || tr1.has(Features::NOTICE))
				_draw_vertex_array(vr1.floor, scale, pos);
			if (tr1.has(Features::STAIRS_DOWN) ||
				tr1.has(Features::LADDER_DOWN) ||
				tr1.has(Features::ELEVATOR_DOWN))
				_draw_vertex_array(vr1.down, scale, pos);
			if (tr1.has(Features::STAIRS_UP) || tr1.has(Features::LADDER_UP) ||
				tr1.has(Features::ELEVATOR_UP))
				_draw_vertex_array(vr1.up, scale, pos);
		}
		if (tm1.is(Properties::DARKNESS))
			_draw_vertex_array(vm1.darkness, scale, pos);
		else {
			if (_has_wall(tm1, player_facing))
				_draw_vertex_array(vm1.back_wall, scale, pos);
			if (_has_normal_door(tm1, player_facing)) {
				_draw_vertex_array(vm1.back_wall, scale, pos);
				_draw_vertex_array(vm1.back_door, scale, pos);
			}
			if (_has_secret_door(tm1, player_facing)) {
				_draw_vertex_array(vm1.back_wall, scale, pos);
				if (game->state->get_lit())
					_draw_vertex_array(vm1.back_door, scale, pos);
			}

			if (tm1.has(Features::MESSAGE) || tm1.has(Features::NOTICE))
				_draw_vertex_array(vm1.floor, scale, pos);
			if (tm1.has(Features::STAIRS_DOWN) ||
				tm1.has(Features::LADDER_DOWN) ||
				tm1.has(Features::ELEVATOR_DOWN))
				_draw_vertex_array(vm1.down, scale, pos);
			if (tm1.has(Features::STAIRS_UP) || tm1.has(Features::LADDER_UP) ||
				tm1.has(Features::ELEVATOR_UP))
				_draw_vertex_array(vm1.up, scale, pos);
		}

		if (!tm1.is(Properties::DARKNESS)) {
			if (_has_wall(tm1, _get_left_side(player_facing)))
				_draw_vertex_array(vm1.left_side_wall, scale, pos);
			if (_has_normal_door(tm1, _get_left_side(player_facing))) {
				_draw_vertex_array(vm1.left_side_wall, scale, pos);
				_draw_vertex_array(vm1.left_side_door, scale, pos);
			}
			if (_has_secret_door(tm1, _get_left_side(player_facing))) {
				_draw_vertex_array(vm1.left_side_wall, scale, pos);
				if (game->state->get_lit())
					_draw_vertex_array(vm1.left_side_door, scale, pos);
			}

			if (_has_wall(tm1, _get_right_side(player_facing)))
				_draw_vertex_array(vm1.right_side_wall, scale, pos);
			if (_has_normal_door(tm1, _get_right_side(player_facing))) {
				_draw_vertex_array(vm1.right_side_wall, scale, pos);
				_draw_vertex_array(vm1.right_side_door, scale, pos);
			}
			if (_has_secret_door(tm1, _get_right_side(player_facing))) {
				_draw_vertex_array(vm1.right_side_wall, scale, pos);
				if (game->state->get_lit())
					_draw_vertex_array(vm1.right_side_door, scale, pos);
			}
		}

		// Row 0
		if (_has_wall(tl0, player_facing))
			_draw_vertex_array(vl0.back_wall, scale, pos);
		if (tl0.is(Properties::DARKNESS)) {
			_draw_vertex_array(vl0.darkness, scale, pos);
			_draw_vertex_array(vl0.side_darkness, scale, pos);
		} else {
			if (_has_normal_door(tl0, player_facing)) {
				_draw_vertex_array(vl0.back_wall, scale, pos);
				_draw_vertex_array(vl0.back_door, scale, pos);
			}
			if (_has_secret_door(tl0, player_facing)) {
				_draw_vertex_array(vl0.back_wall, scale, pos);
				if (game->state->get_lit())
					_draw_vertex_array(vl0.back_door, scale, pos);
			}

			if (tl0.has(Features::MESSAGE) || tl0.has(Features::NOTICE))
				_draw_vertex_array(vl0.floor, scale, pos);
			if (tl0.has(Features::STAIRS_DOWN) ||
				tl0.has(Features::LADDER_DOWN) ||
				tl0.has(Features::ELEVATOR_DOWN))
				_draw_vertex_array(vl0.down, scale, pos);
			if (tl0.has(Features::STAIRS_UP) || tl0.has(Features::LADDER_UP) ||
				tl0.has(Features::ELEVATOR_UP))
				_draw_vertex_array(vl0.up, scale, pos);
		}

		// If we have reached here, we aren't standing in darkness
		if (_has_wall(tm0, player_facing))
			_draw_vertex_array(vm0.back_wall, scale, pos);
		if (_has_normal_door(tm0, player_facing)) {
			_draw_vertex_array(vm0.back_wall, scale, pos);
			_draw_vertex_array(vm0.back_door, scale, pos);
		}
		if (_has_secret_door(tm0, player_facing)) {
			_draw_vertex_array(vm0.back_wall, scale, pos);
			if (game->state->get_lit())
				_draw_vertex_array(vm0.back_door, scale, pos);
		}

		if (tm0.has(Features::MESSAGE) || tm0.has(Features::NOTICE))
			_draw_vertex_array(vm0.floor, scale, pos);
		if (tm0.has(Features::STAIRS_DOWN) || tm0.has(Features::LADDER_DOWN) ||
			tm0.has(Features::ELEVATOR_DOWN))
			_draw_vertex_array(vm0.down, scale, pos);
		if (tm0.has(Features::STAIRS_UP) || tm0.has(Features::LADDER_UP) ||
			tm0.has(Features::ELEVATOR_UP))
			_draw_vertex_array(vm0.up, scale, pos);

		if (_has_wall(tr0, player_facing))
			_draw_vertex_array(vr0.back_wall, scale, pos);
		if (tr0.is(Properties::DARKNESS)) {
			_draw_vertex_array(vr0.darkness, scale, pos);
			_draw_vertex_array(vr0.side_darkness, scale, pos);
		} else {
			if (_has_normal_door(tr0, player_facing)) {
				_draw_vertex_array(vr0.back_wall, scale, pos);
				_draw_vertex_array(vr0.back_door, scale, pos);
			}
			if (_has_secret_door(tr0, player_facing)) {
				_draw_vertex_array(vr0.back_wall, scale, pos);
				if (game->state->get_lit())
					_draw_vertex_array(vr0.back_door, scale, pos);
			}

			if (tr0.has(Features::MESSAGE) || tr0.has(Features::NOTICE))
				_draw_vertex_array(vr0.floor, scale, pos);
			if (tr0.has(Features::STAIRS_DOWN) ||
				tr0.has(Features::LADDER_DOWN) ||
				tr0.has(Features::ELEVATOR_DOWN))
				_draw_vertex_array(vr0.down, scale, pos);
			if (tr0.has(Features::STAIRS_UP) || tr0.has(Features::LADDER_UP) ||
				tr0.has(Features::ELEVATOR_UP))
				_draw_vertex_array(vr0.up, scale, pos);
		}

		if (!tm0.is(Properties::DARKNESS)) {
			if (_has_wall(tm0, _get_left_side(player_facing)))
				_draw_vertex_array(vm0.left_side_wall, scale, pos);
			if (_has_normal_door(tm0, _get_left_side(player_facing))) {
				_draw_vertex_array(vm0.left_side_wall, scale, pos);
				_draw_vertex_array(vm0.left_side_door, scale, pos);
			}
			if (_has_secret_door(tm0, _get_left_side(player_facing))) {
				_draw_vertex_array(vm0.left_side_wall, scale, pos);
				if (game->state->get_lit())
					_draw_vertex_array(vm0.left_side_door, scale, pos);
			}

			if (_has_wall(tm0, _get_right_side(player_facing)))
				_draw_vertex_array(vm0.right_side_wall, scale, pos);
			if (_has_normal_door(tm0, _get_right_side(player_facing))) {
				_draw_vertex_array(vm0.right_side_wall, scale, pos);
				_draw_vertex_array(vm0.right_side_door, scale, pos);
			}
			if (_has_secret_door(tm0, _get_right_side(player_facing))) {
				_draw_vertex_array(vm0.right_side_wall, scale, pos);
				if (game->state->get_lit())
					_draw_vertex_array(vm0.right_side_door, scale, pos);
			}
		}
	}
}

auto Sorcery::Render::_draw_vertex_array(const VertexArray &array,
										 const float scale, const ImVec2 pos)
	-> void {

	// Work out the Quad (Rect) on the Screen where we are going to draw!
	const ImVec2 p1{pos.x + (array.data[0].position.x * scale),
					pos.y + (array.data[0].position.y * scale)};
	const ImVec2 p2{pos.x + (array.data[1].position.x * scale),
					pos.y + (array.data[1].position.y * scale)};
	const ImVec2 p3{pos.x + (array.data[2].position.x * scale),
					pos.y + (array.data[2].position.y * scale)};
	const ImVec2 p4{pos.x + (array.data[3].position.x * scale),
					pos.y + (array.data[3].position.y * scale)};

	// Then work out the Source Quad (Rect)
	const ImVec2 uv_0{array.data[0].tex_coord.x / _source_size.x,
					  array.data[0].tex_coord.y / _source_size.y};
	const ImVec2 uv_1{array.data[1].tex_coord.x / _source_size.x,
					  array.data[1].tex_coord.y / _source_size.y};
	const ImVec2 uv_2{array.data[2].tex_coord.x / _source_size.x,
					  array.data[2].tex_coord.y / _source_size.y};
	const ImVec2 uv_3{array.data[3].tex_coord.x / _source_size.x,
					  array.data[3].tex_coord.y / _source_size.y};

	VertexArray adjusted;
	adjusted.data[0].position = p1;
	adjusted.data[1].position = p2;
	adjusted.data[2].position = p3;
	adjusted.data[3].position = p4;
	adjusted.data[0].tex_coord = uv_0;
	adjusted.data[1].tex_coord = uv_1;
	adjusted.data[2].tex_coord = uv_2;
	adjusted.data[3].tex_coord = uv_3;
	adjusted.data[0].colour = array.data[0].colour;
	adjusted.data[1].colour = array.data[1].colour;
	adjusted.data[2].colour = array.data[2].colour;
	adjusted.data[3].colour = array.data[3].colour;
	_ui->draw_view_image(WIREFRAME_TEXTURE, adjusted);
}