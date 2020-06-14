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
// If you modify this Program, or any covered work, by linking or combining it
// with the libraries referred to in README (or a modified version of said
// libraries), containing parts covered by the terms of said libraries, the
// licensors of this Program grant you additional permission to convey the
// resulting work.

#include "component.hpp"

Sorcery::Component::Component(): screen{""}, name{""}, x{0}, y{0}, w{0}, h{0}, scale{0.0f}, font{0},
	size{0}, colour{0}, string_key{""} {
}

Sorcery::Component::Component(std::string screen_, std::string name_, int x_, int y_, unsigned int w_, unsigned int h_,
	float scale_, Enums::Internal::FontType font_, unsigned int size_, unsigned int colour_, std::string string_key_):
	screen{screen_}, name{name_} , x{x_}, y{y_}, w{w_}, h{h_}, scale{scale_}, font{font_}, size{size_},
	colour{colour_}, string_key{string_key_} {
}

// Copy Constructor
Sorcery::Component::Component(const Component &other): screen{other.screen}, name{other.name}, x{other.x}, y{other.y},
	w{other.w}, h{other.h}, scale{other.scale}, font{other.font}, size{other.size}, colour{other.colour},
	string_key{other.string_key} {
}

// Standard Destructor
Sorcery::Component::~Component() {
}