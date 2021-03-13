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

#include "display.hpp"
#include "main.hpp"
#include "system.hpp"

namespace Sorcery {

	class IconStore {

	  public:
		// Constructors
		IconStore(System *system, Display *display, const std::filesystem::path filename);
		IconStore() = delete;

		// Overload [] operator
		auto operator[](const std::string &key) -> std::optional<sf::Sprite>;
		auto operator[](const MenuItem key) -> std::optional<sf::Sprite>;

		// Public Methods
		auto get(const std::string &key) -> std::optional<sf::Sprite>;
		auto get(const MenuItem key) -> std::optional<sf::Sprite>;

	  private:
		// Private Members
		System *_system;
		Display *_display;
		IconLibrary _icons;
		bool _loaded;
		sf::Texture _texture;
		sf::Vector2f _size;
		sf::Vector2f _scale;
		Component _layout;
		std::map<MenuItem, Icon> _menu_icon_map;
		std::map<std::string, sf::Sprite> _icon_store;

		// Private Methods
		auto _get_rect(unsigned int index) const -> sf::IntRect;
		auto _load(const std::filesystem::path filename) -> bool;
	};
} // namespace Sorcery