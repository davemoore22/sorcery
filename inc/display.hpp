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
#include "system.hpp"
#include "string.hpp"
#include "layout.hpp"
#include "window.hpp"
#include "frame.hpp"

// Forward Declarations
namespace Sorcery {

	class System;
	class String;
	class Layout;
	class Window;

	// Superclass to handle basic display requirements such as Windopw Access, Game Text and so on
	class Display {

		public:

			// Constructors
			Display(System* system);
			Display() = delete;

			// Public Methods
			auto generate_components(const std::string screen) -> void;
			auto display_components(const std::string screen, std::optional<std::any> parameter = std::nullopt) -> void;

			// Public Members
			std::shared_ptr<String> string;
			std::shared_ptr<Window> window;
			std::shared_ptr<Layout> layout;
			sf::Texture ui_texture;

		private:

			// Private Members
			std::map<std::string, sf::Sprite> _sprites;
			std::map<std::string, sf::Text> _texts;
			std::map<std::string, std::shared_ptr<Frame>> _frames;
			System* _system;
			std::vector<std::pair<std::string, std::shared_ptr<Frame>>> __frames;

			// Private Methods
	};
}
