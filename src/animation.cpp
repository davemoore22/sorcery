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

#include "animation.hpp"

// Standard Constructor
Sorcery::Animation::Animation(System *system, Display *display)
	: _system{system}, _display{display} {

	_finished = false;
	_attract_mode.clear();
	_attract_mode_fade_in = true;

	// Setup Colour Cycling
	_colcyc_dir = false;
	_selected_gradient[0.0f] = sf::Color(std::stoull(
		(*_display->layout)["global:selected_item"]["minimum_background"].value(), 0, 16));
	_selected_gradient[1.0f] = sf::Color(std::stoull(
		(*_display->layout)["global:selected_item"]["maximum_background"].value(), 0, 16));
	_colour_cycling_step = std::stod((*_display->layout)["global:selected_item"]["step"].value());
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

auto Sorcery::Animation::start_attract_mode_animation() -> void {

	_allow_attract_ani = true;
}

auto Sorcery::Animation::start_colour_cycling() -> void {

	_allow_colcyc = true;
	attract_alpha = 0;
}

auto Sorcery::Animation::start_attract_mode_animation_threads() -> void {

	start_attract_mode_animation();
	if (!_attract_thrd.joinable())
		_attract_thrd = std::jthread(&Animation::_animate_attract_mode, this, false);
}

auto Sorcery::Animation::start_colour_cycling_threads() -> void {

	start_colour_cycling();
	if (!_colcyc_thrd.joinable())
		_colcyc_thrd = std::jthread(&Animation::_colour_cycling, this, false);
}

auto Sorcery::Animation::stop_attract_mode_animation() -> void {

	_allow_attract_ani = false;
}

auto Sorcery::Animation::stop_colour_cycling() -> void {

	_allow_colcyc = false;
}

auto Sorcery::Animation::stop_attract_mode_animation_threads() -> void {

	_finished = true;
	stop_attract_mode_animation();
	if (_attract_thrd.joinable())
		_attract_thrd.join();
}

auto Sorcery::Animation::stop_colour_cycling_threads() -> void {

	_finished = true;
	stop_colour_cycling();
	if (_colcyc_thrd.joinable())
		_colcyc_thrd.join();
}

// Generate an attract mode sprite to display
auto Sorcery::Animation::_animate_attract_mode(bool force) -> void {

	if (force)
		_do_attract_mode_animation();
	else {
		do {
			_current_time = std::chrono::system_clock::now();
			const auto time_elapsed{_current_time - _last_attract_ani};
			const auto time_elapsed_sec{
				std::chrono::duration_cast<std::chrono::seconds>(time_elapsed)};
			if (time_elapsed_sec.count() > 5)
				if (_allow_attract_ani)
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
			if (_allow_colcyc)
				_do_colour_cycling();

			std::this_thread::sleep_for(std::chrono::milliseconds(20));
		} while (!_finished);
	}
}

// Note for Thread Safety Purposes, we only generate/update the IDs here
// https://www.sfml-dev.org/tutorials/2.0/graphics-draw.php#drawing-from-threads
auto Sorcery::Animation::_do_attract_mode_animation() -> void {

	std::scoped_lock<std::mutex> _scoped_lock(_attract_mutex);
	unsigned int sprite_index{};
	const auto num{(*_system->random)[RandomType::D4]};
	_attract_mode.clear();
	for (auto i = 0u; i < num; i++) {
		do {
			sprite_index = (*_system->random)[RandomType::ZERO_TO_101];
		} while (std::find(_attract_mode.begin(), _attract_mode.end(), sprite_index) !=
				 _attract_mode.end());
		_attract_mode.push_back(sprite_index);
	}
	attract_alpha = 0;
	_last_attract_ani = std::chrono::system_clock::now();
}

auto Sorcery::Animation::get_attract_mode_data() -> std::vector<unsigned int> {

	std::scoped_lock<std::mutex> _scoped_lock(_attract_mutex);
	return _attract_mode;
}

// Called 50 times a second
auto Sorcery::Animation::_do_colour_cycling() -> void {

	std::scoped_lock<std::mutex> _scoped_lock(_colour_mutex);

	if (_colcyc_dir) {
		if (colour_lerp < 1.0l)
			colour_lerp += _colour_cycling_step;
		else {
			_colcyc_dir = !_colcyc_dir;
			colour_lerp -= _colour_cycling_step;
		}
	} else {
		if (colour_lerp > 0.0l)
			colour_lerp -= _colour_cycling_step;
		else {
			_colcyc_dir = !_colcyc_dir;
			colour_lerp += _colour_cycling_step;
		}
	}

	if (colour_lerp < 0.0l)
		colour_lerp = 0.0l;
	if (colour_lerp > 1.0l)
		colour_lerp = 1.0l;

	selected_colour = _selected_gradient.sampleColor(colour_lerp);

	// Handle Attract Mode Fade In/Out
	if (_attract_mode_fade_in == true) {
		if (attract_alpha < 255)
			attract_alpha += 10;
		else
			attract_alpha = 255;
	}

	if (attract_alpha > 255)
		attract_alpha = 255;

	_attract_mode_fade_in = attract_alpha != 255;
}
