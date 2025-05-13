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
#include "common/types.hpp"

namespace Sorcery {

class Component;
class UI;
class System;

class Frame {

	public:
		Frame() = delete;
		Frame(System *system, UI *ui, Component *component);
		Frame(System *system, UI *ui, std::string_view name, const ImVec2 pos,
			  const Size size, const ImU32 colour, const ImU32 bg_colour);

	private:
		auto _draw(const bool foreground) -> void;

		System *_system;
		UI *_ui;
		Component *_component;
		std::string _name;
		ImVec2 _pos;
		Size _size;
		ImU32 _colour;
		ImU32 _bg_colour;
		std::optional<std::string> _title;
		std::optional<std::string> _bg_image;
};

};