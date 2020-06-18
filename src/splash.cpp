// Copyright (C) 2020 Dave Moore
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

#include "splash.hpp"

// Standard Constructor
Sorcery::Splash::Splash (System& system, Display& display, Graphics& graphics):  _system {system}, _display {display},
	_graphics {graphics} {

	// Get the Window and Graphics to Display
	_window = _display.window->get_window();
	_splash = _display.window->get_splash();

	// Set up the Timers
	_alpha = 0;
	_fading_in = true;
	_fading_out = false;
	finished = false;

	// Get the Splash Image Details
	Component splash_c {(*_display.layout)["splash:splash_image"]};

	// Scale the Splash Image to less than the Window Size
	ImageSize size {static_cast<unsigned int>(_splash.getLocalBounds().width),
		static_cast<unsigned int>(_splash.getLocalBounds().height)};
	const ImageSize window_size {_window->getSize().x, _window->getSize().y};
	float scale_ratio_needed {1.0f};
	if ((size.w > window_size.w) || (size.h > window_size.h)) {
		float shrink_width_needed {static_cast<float>(window_size.w) / static_cast<float>(size.w)};
		float shrink_height_needed {static_cast<float>(window_size.h) / static_cast<float>(size.h)};
		scale_ratio_needed = std::min(shrink_width_needed, shrink_height_needed);
	}
	_splash.setScale(scale_ratio_needed, scale_ratio_needed);
	_splash.setScale(splash_c.scale, splash_c.scale);
	const sf::Vector2f splash_pos(_display.window->get_x(_splash, splash_c.x),
		_display.window->get_y(_splash, splash_c.y));
	_splash.setPosition(splash_pos);
}

// Standard Destructor
Sorcery::Splash::~Splash() {
}

auto Sorcery::Splash::draw() -> void {

	_splash.setColor(sf::Color(255,255,255, _alpha));
	_window->draw(_splash);
}

auto Sorcery::Splash::update() -> void {
	if (_fading_in)
		_alpha++;
	if (_fading_out)
		_alpha--;
	if (_alpha == 255) {
		_fading_in = false;
		_fading_out = true;
	}
	if (_alpha == 0)
		finished = true;
}