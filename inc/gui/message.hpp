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
#include "types/component.hpp"
#include "types/enum.hpp"

namespace Sorcery {

class UI;
class System;

class Message {

	public:
		Message(System *system, UI *ui, Component &component);

		auto display(bool &is_yes) -> void;
		auto id() const -> std::string;
		auto set(std::vector<std::string> strings,
				 const Enums::Map::Event event_id) -> void;
		auto name() const -> std::string;

		bool show;

	private:
		System *_system;
		UI *_ui;
		Component _component;
		std::string _id;
		std::string _name;
		std::string _str;
		std::vector<std::string> _strings;
		Enums::Map::Event _event_id;
};

}