// Copyright (C) 2021 Dave Moore
//
// This file is part of Sorcery: Dreams of the Mad Overlord.
//
// Sorcery: Dreams of the Mad Overlord is free software: you can redistribute
// it and/or modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation, either version 2 of the License,
// or (at your option) any later version.
//
// Sorcery: Dreams of the Mad Overlord is distributed in the hope that it wil
// be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
// Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Sorcery: Dreams of the Mad Overlord.  If not,
// see <http://www.gnu.org/licenses/>.
//
// If you modify this program, or any covered work, by linking or combining
// it with the libraries referred to in README (or a modified version of
// said libraries), containing parts covered by the terms of said libraries,
// the licensors of this program grant you additional permission to convey
// the resulting work.

#include "component.hpp"

Sorcery::Component::Component()
	: screen{""}, name{""}, x{0}, y{0}, w{0}, h{0}, scale{0.0f}, font{0}, size{0}, colour{0},
	  animated{false}, string_key{""}, alpha{255}, background{0},
	  justification{}, type{}, priority{999}, drawmode{}, texture{} {

	unique_key.clear();
}

Sorcery::Component::Component(std::string screen_, std::string name_, int x_, int y_,
	unsigned int w_, unsigned int h_, float scale_, Enums::Internal::FontType font_,
	unsigned int size_, unsigned long long colour_, bool animated_, std::string string_key_,
	unsigned int alpha_, unsigned int width_, unsigned long long background_,
	Enums::Window::Justification justification_, Enums::Window::ComponentType type_,
	unsigned int priority_, Enums::Window::DrawMode drawmode_, Enums::Graphics::Texture texture_)
	: screen{screen_}, name{name_}, x{x_}, y{y_}, w{w_}, h{h_}, scale{scale_}, font{font_},
	  size{size_}, colour{colour_}, animated{animated_}, string_key{string_key_}, alpha{alpha_},
	  width{width_}, background{background_}, justification{justification_}, type{type_},
	  priority{priority_}, drawmode{drawmode_}, texture{texture_} {

	// Unique Key is like this because it is used for runtime component-sorting (std::map is sorted
	// by key) (also needs to be replaced with std::format when available)
	const std::string priority_id{fmt::format("prefix.{:03d}.suffix", priority)};
	unique_key = fmt::format("{}_{}:{}", priority_id, screen, name);
}

// Copy Constructors
Sorcery::Component::Component(const Component &other)
	: screen{other.screen}, name{other.name}, x{other.x}, y{other.y}, w{other.w}, h{other.h},
	  scale{other.scale}, font{other.font}, size{other.size}, colour{other.colour},
	  animated{other.animated}, string_key{other.string_key}, alpha{other.alpha},
	  width{other.width}, background{other.background}, justification{other.justification},
	  type{other.type}, priority{other.priority}, drawmode{other.drawmode}, texture{other.texture} {

	// Not sure why I'm doing this here and not in the initialiser list?
	unique_key = other.unique_key;
}

auto Sorcery::Component::operator=(const Component &other) -> Component & {

	screen = other.screen;
	name = other.name;
	x = other.x;
	y = other.y;
	w = other.w;
	h = other.h;
	scale = other.scale;
	font = other.font;
	size = other.size;
	colour = other.colour;
	animated = other.animated;
	string_key = other.string_key;
	alpha = other.alpha;
	width = other.width;
	background = other.background;
	justification = other.justification;
	type = other.type;
	priority = other.priority;
	drawmode = other.drawmode;
	texture = other.texture;
	unique_key = other.unique_key;

	return *this;
}