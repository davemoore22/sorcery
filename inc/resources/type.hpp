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

#include "common/enum.hpp"
#include "common/include.hpp"
#include "common/type.hpp"

namespace Sorcery {

// Struct to represent an icon
struct Icon {
		Icon() : index{0}, item{Enums::Menu::Item::NO_MENU_ITEM}, key{}, filename{}, colour{} {};
		Icon(unsigned int index_, Enums::Menu::Item item_, std::string key_, std::string filename_, sf::Color colour_)
			: index{index_}, item{item_}, key{key_}, filename{filename_}, colour{colour_} {};

		auto operator==(const Icon &a) const -> bool {
			return ((index == a.index) && (item == a.item) && (key == a.key) && (filename == a.filename) &&
					(colour == a.colour));
		}

		unsigned int index;
		Enums::Menu::Item item;
		std::string key;
		std::string filename;
		sf::Color colour;
};

// Dungeon Graphics
struct Texture {
		Texture() : name{0}, index{0}, wall{0}, floor{0}, ceiling{0}, door{0}, source{""}, comment{""} {};
		Texture(std::string name, unsigned int index_, unsigned int wall_, unsigned int floor_, unsigned int ceiling_,
			unsigned int door_, std::string source_, std::string comment_ = "")
			: name{name}, index{index_}, wall{wall_}, floor{floor_}, ceiling{ceiling_}, door{door_}, source{source_},
			  comment{comment_} {};

		auto operator==(const Texture &a) const -> bool {
			return (name == a.name && index == a.index && wall == a.wall && floor == a.floor && ceiling == a.ceiling &&
					door == a.door && source == a.source);
		}

		std::string name;
		unsigned int index;
		unsigned int wall;
		unsigned int floor;
		unsigned int ceiling;
		unsigned int door;
		std::string source;
		std::string comment;
};

// Struct to represent a rect on the screen
struct Rect {
		Rect() : x{0}, y{0}, w{0}, h{0} {};
		Rect(unsigned int x_, unsigned int y_, unsigned int w_, unsigned int h_) : x{x_}, y{y_}, w{w_}, h{h_} {};
		Rect(const Rect &other) : x{other.x}, y{other.y}, w{other.w}, h{other.h} {};

		auto operator=(const Rect &other) -> Rect & {

			x = other.x;
			y = other.y;
			w = other.w;
			h = other.h;

			return *this;
		};

		auto menu_contains(unsigned int i, unsigned int j) -> bool {
			// Note the >= etc here - menu rects start at y - if not = then
			// first item in menu would fail this test
			return ((i >= x) && (j >= y) && (i < (x + w - 1)) && (j < (y + h)));
		}

		auto highlight_contains(unsigned int i, unsigned int j) -> bool {
			return ((i >= x) && (j >= y) && (i < (x + w)) && (j <= (y + h)));
		}

		auto debug() const -> void {
			std::cout << x << "/" << y << " " << w << ":" << h << std::endl;
		}

		auto scale(int change) -> void {
			x -= change;
			y -= change;
			w += (change * 2);
			h += (change * 2);
		}

		unsigned int x;
		unsigned int y;
		unsigned int w;
		unsigned int h;
};

}