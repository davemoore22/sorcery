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

// clang-format off
#include "main.hpp"
#include "system.hpp"
#include "controloverlay.hpp"
#include "frame.hpp"
#include "iconstore.hpp"
#include "layout.hpp"
#include "string.hpp"
#include "window.hpp"
// clang-format on

// Forward Declarations
namespace Sorcery {

	class Frame;
	class System;
	class String;
	class Layout;
	class Window;
	class IconStore;
	class ControlOverlay;

	// Superclass to handle basic display requirements such as Windopw Access,
	// Game Text and so on
	class Display {

	  public:
		// Constructors
		Display(System *system);
		Display() = delete;

		// Copy Constructors
		Display(const Display &other);
		auto operator=(const Display &other) -> Display &;

		// Public Methods
		auto generate(const std::string &screen) -> void;
		auto display(const std::string &screen,
			std::optional<std::any> parameter = std::nullopt) -> void;
		auto display(const std::string &screen,
			std::map<std::string, sf::Sprite> &sprites,
			std::map<std::string, sf::Text> &texts,
			std::map<std::string, std::shared_ptr<Frame>> &frames,
			std::optional<std::any> parameter = std::nullopt) -> void;
		auto generate(const std::string &screen,
			std::map<std::string, sf::Sprite> &sprites,
			std::map<std::string, sf::Text> &texts,
			std::map<std::string, std::shared_ptr<Frame>> &frames) -> void;
		auto display_cursor() -> void;
		auto fit_bg_movie() -> void;
		auto start_bg_movie() -> void;
		auto stop_bg_movie() -> void;
		auto update_bg_movie() -> void;
		auto draw_bg_movie() -> void;
		auto set_input_mode(WindowInputMode input_mode) -> void;
		auto get_input_mode() const -> WindowInputMode;
		auto show_overlay() -> void;
		auto hide_overlay() -> void;
		auto display_overlay() -> void;

		// Public Members
		std::shared_ptr<String> string;
		std::shared_ptr<Window> window;
		std::shared_ptr<Layout> layout;
		std::shared_ptr<ControlOverlay> overlay;
		sf::Texture ui_texture;

	  private:
		// Private Members
		std::map<std::string, sf::Sprite> _sprites;
		std::map<std::string, sf::Text> _texts;
		std::map<std::string, std::shared_ptr<Frame>> _frames;
		System *_system;
		sfe::Movie _background_movie;
		std::map<Component,
			std::variant<sf::Sprite, sf::Text, std::shared_ptr<Frame>>>
			_components;
		std::unique_ptr<IconStore> _icons;
		bool _show_overlay;

		// Private Methods
	};
} // namespace Sorcery
