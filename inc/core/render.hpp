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

#pragma once

#include "common/include.hpp"
#include "core/types.hpp"
#include "types/tile.hpp"

// Class to handles rendering Wireframe
namespace Sorcery {

// Forward Declarations
class Component;
class Controller;
class Display;
class Game;
class System;
class UI;

class Render {

	public:
		// Constructors
		Render(System *system, Display *display, UI *ui,
			   Controller *controller);
		Render() = delete;

		// Public Methods
		auto get_monochrome() const -> bool;
		auto set_monochrome(bool value) -> void;
		auto draw(Game *game, Component *component) -> void;

	private:
		// Private Members
		System *_system;
		Display *_display;
		UI *_ui;
		Controller *_controller;
		bool _loaded;
		unsigned int _depth;
		unsigned int _width;
		bool _monochrome;
		std::map<Coordinate3, TileView> _tileviews;
		ImVec2 _source_size;
		ImVec2 _pane_size;
		ImVec2 _pos;

		// Private Methods
		auto _get_left_side(const Enums::Map::Direction facing) const
			-> Enums::Map::Direction;
		auto _get_right_side(const Enums::Map::Direction facing) const
			-> Enums::Map::Direction;
		auto _has_normal_door(const Tile &tile,
							  const Enums::Map::Direction direction) const
			-> bool;
		auto _has_secret_door(const Tile &tile,
							  const Enums::Map::Direction direction) const
			-> bool;
		auto _has_wall(const Tile &tile,
					   const Enums::Map::Direction direction) const -> bool;
		auto _load_tile_views() -> void;
		auto _render_wireframe(Game *game, Component *component) -> void;
		auto _set_texture_coordinates(TileView &tileview) -> void;
		auto _set_vertex_array(VertexArray &array, ImVec2 p1, ImVec2 p2,
							   ImVec2 p3, ImVec2 p4) -> void;
		auto _set_vertex_array(VertexArray &array, ImVec2 p1, ImVec2, ImVec2 p3,
							   ImVec2 p4, const ImVec4 colour) -> void;
		auto _draw_vertex_array(const VertexArray &array, const float scale,
								const ImVec2 pos) -> void;
};

}