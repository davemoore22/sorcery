// Copyright (C) 2021 Dave Moore
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

#include "display.hpp"
#include "game.hpp"
#include "graphics.hpp"
#include "main.hpp"
#include "system.hpp"

namespace Sorcery {

	class Render : public sf::Transformable, public sf::Drawable {

	  public:
		// Constructors
		Render(
			System *system, Display *display, Graphics *graphics, Game *game);
		Render() = delete;

		// Public Methods
		auto render() -> void;
		auto update() -> void;

	  private:
		// Private Members
		System *_system;
		Display *_display;
		Graphics *_graphics;
		Game *_game;

		double theta;

		double scr_pts[MINIMUM_SCREEN_WIDTH];
		double distortion[MINIMUM_SCREEN_WIDTH];

		const double FOV{1.30899694}; // rad (75 deg)
		const double MIN_DIST{0.3};	  // square sides / s
		const double DARKEST_DIST{
			6.0};					  // any greater distance will not be darker
		const int DARKNESS_ALPHA{40}; // minimal lighting

		const double DEFAULT_SPEED = 3; // sqsides/s
		const double TURN_SPEED = M_PI * 8;
		double tan_FOV;

		double speed; // sqsides/s
		double turn;  // rad/s

		sf::RenderTexture _rtexture;
		sf::Sprite _sprite;
		sf::Texture _texture;

		// Private Methods
		auto virtual draw(
			sf::RenderTarget &target, sf::RenderStates states) const -> void;
	};
} // namespace Sorcery
