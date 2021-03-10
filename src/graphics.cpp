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

#include "graphics.hpp"

// Standard Constructor
Sorcery::Graphics::Graphics(System *system, Display *display) {

	animation = std::make_shared<Animation>(*system, *display);
	icons = std::make_shared<IconStore>(*system, *display, (*system->files)[ICONS_FILE]);
}

Sorcery::Graphics::Graphics(const Graphics &other)
	: animation{other.animation}, icons{other.icons} {}

auto Sorcery::Graphics::operator=(const Graphics &other) -> Graphics & {

	animation = other.animation;
	icons = other.icons;

	return *this;
}