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

#include "splash.hpp"

// Standard Constructor
Sorcery::Splash::Splash(System *system, Display *display, Graphics *graphics)
	: _system{system}, _display{display}, _graphics{graphics} {

	// Get the Window and Graphics to Display
	_window = _display->window->get_window();

	// Set up the Timers
	_alpha = 0;
	_fading_in = true;
	_fading_out = false;
	_finished = false;
}

auto Sorcery::Splash::start() -> int {

	// Get the Background Display Components and load them into Display module
	// storage (not local)
	_display->generate("splash");

	sf::Event event{};
	while (!_finished) {
		_window->pollEvent(event);
		_window->clear();
		_update();
		_draw();
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		_window->display();
		if ((event.type == sf::Event::KeyPressed) || (event.type == sf::Event::MouseButtonPressed))
			_finished = true;
	}

	return EXIT_MODULE;
}

auto Sorcery::Splash::_draw() -> void {

	_display->display("splash", _alpha);
}

auto Sorcery::Splash::_update() -> void {

	if (_fading_in)
		++_alpha;
	if (_fading_out)
		--_alpha;
	if (_alpha == 255) {
		_fading_in = false;
		_fading_out = true;
	}
	if (_alpha == 0)
		_finished = true;
}
