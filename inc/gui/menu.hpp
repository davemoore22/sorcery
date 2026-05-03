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

#include "common/enum.hpp"
#include "common/imgui.hpp"
#include "common/types.hpp"
#include "gui/define.hpp"
#include "types/enum.hpp"

namespace Sorcery {

struct Context;
class Component;
class Game;

class Menu {

	public:
		Menu() = delete;
		Menu(Context &ctx, Component *component, Game *game);
		~Menu();

		auto draw() -> void;
		auto regenerate() -> void;

		Context &_ctx;
		Component *_component;
		Game *_game;
		std::string _name;
		ImVec2 _pos;
		unsigned int _width;
		unsigned int _height;
		ImU32 _colour;
		ImU32 _bg_colour;
		ImU32 _hi_colour;
		Enums::Layout::Font _font;
		std::vector<std::string> _items;
		std::vector<int> _data;
		bool _reorder;
		bool _across;
};

};