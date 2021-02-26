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

// https://github.com/lxndrdagreat/sfray
#pragma once

#include "main.hpp"

namespace Sorcery {

	class Camera {

	  public:
		// Standard Constructor
		Camera();

		// Public Members
		bool moved;

		// Public Methods
		auto getDirection() -> sf::Vector2f;
		auto setDirection(const sf::Vector2f &direction) -> void;
		auto setDirection(float x, float y) -> void;
		auto getPlane() -> sf::Vector2f;
		auto setPlane(const sf::Vector2f &plane) -> void;
		auto setPlane(float x, float y) -> void;
		auto getPosition() -> sf::Vector2f;
		auto setPosition(const sf::Vector2f &position) -> void;
		auto setPosition(float x, float y) -> void;
		auto setDirectionInDegrees(float degrees) -> void;
		auto getDirectionInDegrees() -> float;
		auto rotateByDegrees(float degrees) -> void;

	  private:
		// Private Members
		sf::Vector2f mDirection;
		sf::Vector2f mPlane;
		sf::Vector2f mPosition;
	};
} // namespace Sorcery