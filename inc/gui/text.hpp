// Copyright (C) 2024 Dave Moore
//
// This file is part of Sorcery: Shadows under Llylgamyn.
//
// Sorcery: Shadows under Llylgamyn is free software: you can redistribute
// it and/or modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation, either version 2 of the License,
// or (at your option) any later version.
//
// Sorcery: Shadows under Llylgamyn is distributed in the hope that it wil
// be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
// Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Sorcery: Shadows under Llylgamyn.  If not,
// see <http://www.gnu.org/licenses/>.
//
// If you modify this program, or any covered work, by linking or combining
// it with the libraries referred to in README (or a modified version of
// said libraries), containing parts covered by the terms of said libraries,
// the licensors of this program grant you additional permission to convey
// the resulting work.

#pragma once

#include "common/include.hpp"

namespace Sorcery {

// Forward Declarations
class Component;
class Display;
class System;

class Text: public sf::Transformable, public sf::Drawable {

	public:

		// Constructors
		Text();
		Text(System *system, Display *display);
		Text(System *system, Display *display, const std::optional<Component> component, const int bits);

		// Public Members

		// Public Methods
		auto get_global_bounds() const -> sf::FloatRect;
		auto get_local_bounds() const -> sf::FloatRect;
		auto get_position() const -> sf::Vector2f;
		auto set_character_size(const unsigned int size) -> void;
		auto set_fill_colour(const sf::Color &colour) -> void;
		auto set_font(sf::Font font) -> void;
		auto set_origin(const float x, const float y) -> void;
		auto set_outline_colour(const sf::Color &colour) -> void;
		auto set_outline_thickness(const unsigned int size) -> void;
		auto set_position(const float x, const float y) -> void;
		auto set_string(const std::string value) -> void;

	private:

		// Private Methods
		auto virtual draw(sf::RenderTarget &target, sf::RenderStates states) const -> void;

		// Private Members
		System *_system;
		Display *_display;
		sf::Text _text;
};

}