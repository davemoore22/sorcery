// Copyright (C) 2026 Dave Moore
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

#include "common/imgui.hpp"

namespace Sorcery {

// offset
// 		Screen-space displacement from the main image.
//
// brightness
//		Multiplier applied to the RGB components of the main tint.
// 		1.0 = same brightness, 0.0 = black.
//
// alpha
//		Opacity multiplier for the offset image.
// 		1.0 = fully opaque, 0.0 = invisible.
struct ImageOffsetEffect {

		ImVec2 offset{0.0f, 0.0f};
		float brightness{0.25f};
		float alpha{1.0f};
		std::optional<ImVec4> colour{};
};

}