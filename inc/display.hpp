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

#pragma once

#include "frame.hpp"
#include "layout.hpp"
#include "main.hpp"
#include "string.hpp"
#include "system.hpp"
#include "window.hpp"

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
		Display(System *system);
		Display() = delete;

		// Public Methods
		auto generate_components(const std::string screen) -> void;
		auto display_components(
			const std::string screen, std::optional<std::any> parameter = std::nullopt) -> void;
		auto display_components(const std::string screen,
			std::map<std::string, sf::Sprite> &sprites, std::map<std::string, sf::Text> &texts,
			std::map<std::string, std::shared_ptr<Frame>> &frames,
			std::optional<std::any> parameter = std::nullopt) -> void;

		auto display_cursor() -> void;
		auto fit_background_movie() -> void;
		auto start_background_movie() -> void;
		auto stop_background_movie() -> void;
		auto update_background_movie() -> void;
		auto draw_background_movie() -> void;
		auto generate_components(const std::string screen,
			std::map<std::string, sf::Sprite> &sprites, std::map<std::string, sf::Text> &texts,
			std::map<std::string, std::shared_ptr<Frame>> &frames) -> void;

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
		System *_system;
		sfe::Movie _background_movie;

		// then these can be sorted by component.priority and then we can sort the layering
		std::map<Component, std::variant<sf::Sprite, sf::Text, std::shared_ptr<Frame>>> _components;

		// attract mode to become an object too?

		// Private Methods
	};
} // namespace Sorcery
