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
#include "frame.hpp"
#include "graphics.hpp"
#include "layout.hpp"
#include "main.hpp"
#include "system.hpp"

namespace Sorcery {

	class Display;

	class StatusBar : public sf::Transformable, public sf::Drawable {

	  public:
		// Constructors
		StatusBar(System *system, Display *display, Graphics *graphics);
		StatusBar() = delete;

		// Public Members
		unsigned int width;
		unsigned int height;
		sf::Sprite sprite;

		// Public Methods

	  private:
		// Private Methods
		auto virtual draw(
			sf::RenderTarget &target, sf::RenderStates states) const -> void;
		auto _generate_components() -> void;
		auto _draw_components() -> void;

		// Private Members
		System *_system;
		Display *_display;
		Graphics *_graphics;
		Component _layout;
		Component _frame_c;
		std::unique_ptr<Frame> _frame;
		sf::RenderTexture _rtexture;
		sf::Sprite _fsprite;
		sf::Texture _texture;

		std::map<std::string, sf::Text> _texts;
	};
} // namespace Sorcery
