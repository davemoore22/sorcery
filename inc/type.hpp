// Copyright (C) 2020 Dave Moore
//
// This file is part of Sorcery: Dreams of the Mad Overlord.
//
// Sorcery: Dreams of the Mad Overlord is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Sorcery: Dreams of the Mad Overlord is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Sorcery: Dreams of the Mad Overlord.  If not, see <http://www.gnu.org/licenses/>.
//
// If you modify this Program, or any covered work, by linking or combining it
// with the libraries referred to in README (or a modified version of said
// libraries), containing parts covered by the terms of said libraries, the
// licensors of this Program grant you additional permission to convey the
// resulting work.

#pragma once


#include "main.hpp"

namespace Sorcery {

	// Struct to represent a point on the screen (x, y)
	struct Point {
		Point() : x{0}, y{0} {};
		Point(unsigned int x_, unsigned int y_) : x{x_}, y{y_} {};
		Point(const Point &other) : x{other.x}, y{other.y} {};
		auto operator==(const Point &a) const -> bool {
			return (x == a.x && y == a.y);
		}

		unsigned int x;
		unsigned int y;
	};

	// Struct to represent an area of the screen (w, h)
	struct ScreenSize {
		ScreenSize() : w{0}, h{0} {};
		ScreenSize(unsigned int w_, unsigned int h_) : w{w_}, h{h_} {};
		ScreenSize(const ScreenSize &other) : w{other.w}, h{other.h} {};

		unsigned int w;
		unsigned int h;
	};

	// Struct to represent the size of an image
	struct ImageSize {
		ImageSize() : w{0}, h{0} {};
		ImageSize(unsigned int w_, unsigned int h_) : w{w_}, h{h_} {};
		ImageSize(const ImageSize &other) : w{other.w}, h{other.h} {};

		unsigned int w;
		unsigned int h;
	};

	// Struct to represent a rect on the screen
	struct Rect {
		Rect() : x{0}, y{0}, w{0}, h{0} {};
		Rect(unsigned int x_, unsigned int y_, unsigned int w_, unsigned int h_) : x{x_}, y{y_}, w{w_}, h{h_} {};
		Rect(const Rect &other) : x{other.x}, y{other.y}, w{other.w}, h{other.h} {};

		auto menu_contains(unsigned int i, unsigned int j) -> bool {
			// Note the >= etc here - menu rects start at y - if not = then first item in menu would fail this test
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

	struct SpellDetails {
		// Need to use full details in here as Aliases haven't yet been set up in main.hpp when we include enums.hpp
		Enums::Magic::Spell id;
		Enums::Magic::SpellType spelltype;
		std::string name;
		std::string translated_name;
		std::string details;
	};
}