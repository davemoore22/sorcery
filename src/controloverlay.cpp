// Copyright (C) 2021 Dave Moore
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

#include "controloverlay.hpp"

// Standard Constructor
Sorcery::ControlOverlay::ControlOverlay(System *system, Display *display, Component layout)
	: _system{system}, _display{display}, _layout{layout} {

	_sprites.clear();
	_texts.clear();

	_input_mode = WindowInputMode::NONE;
}

auto Sorcery::ControlOverlay::set_input_mode(WindowInputMode input_mode) -> void {

	_input_mode = input_mode;
}

auto Sorcery::ControlOverlay::draw(sf::RenderTarget &target, sf::RenderStates states) const
	-> void {

	states.transform *= getTransform();

	for (const auto &sprite : _sprites)
		target.draw(sprite, states);

	for (const auto &text : _texts)
		target.draw(text, states);
}