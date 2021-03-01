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
#include "camera.hpp"

// Standard Constructor
Sorcery::Camera::Camera() : mDirection(-1.0f, 0), mPlane(0, 0.66f), mPosition(0, 0) {}

auto Sorcery::Camera::getDirection() -> sf::Vector2f {
	return mDirection;
}

auto Sorcery::Camera::setDirection(const sf::Vector2f &direction) -> void {
	mDirection = direction;
}

auto Sorcery::Camera::setDirection(float x, float y) -> void {
	mDirection = sf::Vector2f(x, y);
}

auto Sorcery::Camera::getPlane() -> sf::Vector2f {
	return mPlane;
}

auto Sorcery::Camera::setPlane(const sf::Vector2f &plane) -> void {
	mPlane = plane;
}

auto Sorcery::Camera::setPlane(float x, float y) -> void {
	mPlane = sf::Vector2f(x, y);
}

auto Sorcery::Camera::getPosition() -> sf::Vector2f {
	return mPosition;
}

auto Sorcery::Camera::setPosition(const sf::Vector2f &position) -> void {
	mPosition = position;
}

auto Sorcery::Camera::setPosition(float x, float y) -> void {
	mPosition = sf::Vector2f(x, y);
}

auto Sorcery::Camera::getDirectionInDegrees() -> float {
	return atan2f(mDirection.y, mDirection.x) * (180.0f / std::numbers::pi);
}

auto Sorcery::Camera::setDirectionInDegrees(float degrees) -> void {
	float angle{degrees * (std::numbers::pi / 180.0f)};
	mDirection.x = cosf(angle);
	mDirection.y = sinf(angle);

	float perp{(degrees - 90) * (std::numbers::pi / 180.0f)};
	// float fov = 2 * atan(0.66/1.0);
	mPlane.x = cosf(perp) * 0.66f;
	mPlane.y = sinf(perp) * 0.66f;
}

auto Sorcery::Camera::rotateByDegrees(float degrees) -> void {
	float current = getDirectionInDegrees();
	current += degrees;
	setDirectionInDegrees(current);
}
