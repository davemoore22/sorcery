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

#pragma once

#include "core/display.hpp"
#include "core/game.hpp"
#include "core/graphics.hpp"
#include "core/system.hpp"
#include "main.hpp"

// Class to handles managing screen layouts
namespace Sorcery {

class View {

	public:

		// Constructors
		View(System *system, Display *display, Graphics *graphics, Game *game);
		View() = delete;

		// Public Methods
		auto get_monochrome() const -> bool;
		auto set_monochrome(bool value) -> void;

		// Public Members
		std::map<Coordinate3, TileView> tileviews;

	private:

		// Private Members
		System *_system;
		Display *_display;
		Graphics *_graphics;
		Game *_game;
		bool _loaded;
		unsigned int _depth;
		unsigned int _width;
		bool _monochrome;

		// Private Methods
		auto _load_tile_views() -> void;
		auto _set_texture_coordinates(TileView &tileview) -> void;
		auto _set_vertex_array(
			sf::VertexArray &array, sf::Vector2f p1, sf::Vector2f p2, sf::Vector2f p3, sf::Vector2f p4) -> void;
		auto _set_vertex_array(sf::VertexArray &array, sf::Vector2f p1, sf::Vector2f p2, sf::Vector2f p3,
			sf::Vector2f p4, const sf::Color colour) -> void;
};

}