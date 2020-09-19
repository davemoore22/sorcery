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

#include "animation.hpp"

// Standard Constructor - to do this does not need system or display, instead put it as a subclass of display
Sorcery::Animation::Animation(System& system, Display& display): _system {system}, _display {display} {
	_finished = false;
	_attract_mode.clear();
	_colour_cycling_direction = false;
}

// Standard Destructor
Sorcery::Animation::~Animation() {
	_finished = true;
}

auto Sorcery::Animation::force_refresh_attract_mode() -> void {
	_animate_attract_mode(true);
}

auto Sorcery::Animation::force_refresh_colour_cycling() -> void {
	_colour_cycling(true);
}

auto Sorcery::Animation::start_attract_mode_animation() ->void {
	_allow_attract_mode_animations = true;
}

auto Sorcery::Animation::start_colour_cycling() -> void {
	_allow_colour_cycling = true;
}

auto Sorcery::Animation::start_attract_mode_animation_threads() -> void {
	start_attract_mode_animation();
	if (!_attract_mode_animation_thread.joinable())
		_attract_mode_animation_thread = std::thread(&Animation::_animate_attract_mode, this, false);
}

auto Sorcery::Animation::start_colour_cycling_threads() -> void {
	start_colour_cycling();
	if (!_colour_cycling_thread.joinable())
		_colour_cycling_thread = std::thread(&Animation::_colour_cycling, this, false);
}

auto Sorcery::Animation::stop_attract_mode_animation() -> void {
	_allow_attract_mode_animations = false;
}

auto Sorcery::Animation::stop_colour_cycling() -> void {
	_allow_colour_cycling = false;
}

auto Sorcery::Animation::stop_attract_mode_animation_threads() -> void {
	_finished = true;
	stop_attract_mode_animation();
	if (_attract_mode_animation_thread.joinable())
		_attract_mode_animation_thread.join();
}

auto Sorcery::Animation::stop_colour_cycling_threads() -> void {
	_finished = true;
	stop_colour_cycling();
	if (_colour_cycling_thread.joinable())
		_colour_cycling_thread.join();
}

// Generate an attract mode sprite to display
auto Sorcery::Animation::_animate_attract_mode(bool force) -> void {
	if (force)
		_do_attract_mode_animation();
	else {
		do {
			_current_time = std::chrono::system_clock::now();
			const auto time_elapsed = _current_time - _last_attract_mode_animation;
			const auto time_elapsed_in_seconds = std::chrono::duration_cast<std::chrono::seconds>(time_elapsed);
			if (time_elapsed_in_seconds.count() > 5)
				if (_allow_attract_mode_animations)
					_do_attract_mode_animation();

			std::this_thread::sleep_for(std::chrono::milliseconds(500));
		} while (!_finished);
	}
}

// Calculate Colour Cycling
auto Sorcery::Animation::_colour_cycling(bool force) -> void {
	if (force)
		_do_colour_cycling();
	else {
		do {
			if (_allow_colour_cycling)
				_do_colour_cycling();

			std::this_thread::sleep_for(std::chrono::milliseconds(25));
		} while (!_finished);
	}
}

// Note for Thread Safety Purposes, we only generate/update the IDs here
// https://www.sfml-dev.org/tutorials/2.0/graphics-draw.php#drawing-from-threads
auto Sorcery::Animation::_do_attract_mode_animation() -> void {
	std::scoped_lock<std::mutex> _scoped_lock(_attract_mode_mutex);
	unsigned int sprite_index {};
	const unsigned int number_to_display {(*_system.random)[RandomType::D4]};
	_attract_mode.clear();
	for (unsigned int i = 0; i < number_to_display; i++) {
		do {
			sprite_index = (*_system.random)[RandomType::ZERO_TO_101];
		} while (std::find(_attract_mode.begin(), _attract_mode.end(), sprite_index) != _attract_mode.end());
		_attract_mode.push_back(sprite_index);
	}
	_last_attract_mode_animation = std::chrono::system_clock::now();
}

auto Sorcery::Animation::get_attract_mode_data() -> std::vector<unsigned int> {
	std::scoped_lock<std::mutex> _scoped_lock(_attract_mode_mutex);
	return _attract_mode;
}

auto Sorcery::Animation::_do_colour_cycling() -> void {
	std::scoped_lock<std::mutex> _scoped_lock(_colour_cycling_mutex);
	if (_colour_cycling_direction) {
		 if (colour_lerp < 1.0l)
			colour_lerp += 0.025l;
		 else {
			_colour_cycling_direction = !_colour_cycling_direction;
			colour_lerp -= 0.025l;
		 }
	} else {
		if (colour_lerp > 0.0l)
			colour_lerp -= 0.025l;
		else {
			_colour_cycling_direction = !_colour_cycling_direction;
			colour_lerp += 0.025l;
		}
	}

	if (colour_lerp < 0.0l)
		colour_lerp = 0.0l;
	if (colour_lerp > 1.0l)
		colour_lerp = 1.0l;
}
