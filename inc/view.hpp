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

// Class to handles managing screen layouts
namespace Sorcery {

	class View {

	  public:
		// Constructors
		View(System *system, Display *display, Graphics *graphics, Game *game);
		View() = delete;

		// Public Methods
		auto get(const ViewNodeLayer layer, const int x, const int z) const
			-> ViewNode;
		auto get_lit_nodes(const ViewNodeLayer layer, bool lit = true) const
			-> std::vector<ViewNode>;
		auto get_nodes_at_depth(const ViewNodeLayer layer, const int z) const
			-> std::vector<ViewNode>;
		auto get_nodes_at_depth(const ViewNodeLayer layer,
			const ViewNodeType type, const int z) const
			-> std::vector<ViewNode>;
		auto get_nodes_at_depth(const ViewNodeLayer layer,
			const ViewNodeType type, const int x_sgn, const int z) const
			-> std::vector<ViewNode>;
		auto width() -> unsigned int;
		auto depth() -> unsigned int;

		// Operator Overloading
		auto operator[](ViewNodeKey key) -> ViewNode;
		auto operator[](int z) -> std::vector<ViewNode>;

		std::map<Coordinate3, TileView> tileviews;

	  private:
		// Private Members
		System *_system;
		Display *_display;
		Graphics *_graphics;
		Game *_game;
		std::map<ViewNodeKey, ViewNode> _nodes;
		bool _loaded;
		unsigned int _depth;
		unsigned int _width;

		// Private Methods
		auto _get(ViewNodeLayer layer, int x, int z) const -> ViewNode;
		auto _load(const std::filesystem::path filename) -> bool;
		auto _preload(const int depth, const int width) -> void;

		auto _load_tile_views() -> void;
		auto _set_texture_coordinates(TileView &tileview) -> void;
		auto _set_vertex_array(sf::VertexArray &array, sf::Vector2f p1,
			sf::Vector2f p2, sf::Vector2f p3, sf::Vector2f p4) -> void;
	};
} // namespace Sorcery
