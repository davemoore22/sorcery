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
#include "gui/enum.hpp"
#include "resources/componentstore.hpp"

namespace Sorcery {

class Frame;
class Display;
class System;

class ControlOverlay: public sf::Transformable, public sf::Drawable {

	public:

		// Constructors
		ControlOverlay(System *system, Display *display, Component layout);
		ControlOverlay() = delete;

		// Public Members
		unsigned int width;
		unsigned int height;
		bool valid;

		// Public Methods
		auto set_input_mode(WIM input_mode) -> void;

	private:

		// Private Methods
		auto _get_control_gfx(WIC input) -> sf::Sprite;
		auto virtual draw(sf::RenderTarget &target, sf::RenderStates states) const -> void;

		// Private Members
		System *_system;
		Display *_display;
		std::vector<sf::Sprite> _sprites;
		std::vector<sf::Text> _texts;
		Component _layout;
		WIM _input_mode;
		std::vector<std::pair<std::string, sf::Sprite>> _controls;
		sf::Texture _control_texture;
		std::unique_ptr<Frame> _frame;
};
} // namespace Sorcery
