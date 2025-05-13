// Copyright (C) 2025 Dave Moore
//
// This file is part of Sorcery.
//
// Sorcery is free software: you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software
// Foundation, either version 2 of the License, or (at your option) any later
// version.
//
// Sorcery is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with
// Sorcery.  If not, see <http://www.gnu.org/licenses/>.
//
// If you modify this program, or any covered work, by linking or combining
// it with the libraries referred to in README (or a modified version of
// said libraries), containing parts covered by the terms of said libraries,
// the licensors of this program grant you additional permission to convey
// the resulting work.

#include "core/animation.hpp"
#include "core/system.hpp"

// Standard Constructor
Sorcery::Animation::Animation(Random *random)
	: _random{random} {

	_finished = false;
	_attract_mode.clear();
	_attract_fade = true;

	// Setup Colour Cycling
	_colcyc_dir = false;
	_colcyc_step = 0.00625;

	fade = 1.00f;
}

Sorcery::Animation::~Animation() {

	_finished = true;
}

auto Sorcery::Animation::refresh_wp() -> void {

	_change_wp(true);
}

auto Sorcery::Animation::refresh_attract() -> void {

	_animate_attract(true);
}

auto Sorcery::Animation::refresh_colcyc() -> void {

	_colcyc(true);
}

auto Sorcery::Animation::start_wp() -> void {

	_allow_wp = true;
}

auto Sorcery::Animation::start_attract() -> void {

	_allow_attract = true;
}

auto Sorcery::Animation::start_colcyc() -> void {

	_allow_colcyc = true;
}

auto Sorcery::Animation::start_wp_th() -> void {

	start_wp();
	if (!_wallpaper_th.joinable())
		_wallpaper_th = std::jthread(&Animation::_change_wp, this, false);
}

auto Sorcery::Animation::start_attract_th() -> void {

	start_attract();
	if (!_attract_th.joinable())
		_attract_th = std::jthread(&Animation::_animate_attract, this, false);
}

auto Sorcery::Animation::start_colcycl_th() -> void {

	start_colcyc();
	if (!_colcyc_th.joinable())
		_colcyc_th = std::jthread(&Animation::_colcyc, this, false);
}

auto Sorcery::Animation::stop_wp() -> void {

	_allow_wp = false;
}

auto Sorcery::Animation::stop_attract() -> void {

	_allow_attract = false;
}

auto Sorcery::Animation::stop_colcyc() -> void {

	_allow_colcyc = false;
}

auto Sorcery::Animation::stop_wp_th() -> void {

	_finished = true;
	stop_wp();
	if (_wallpaper_th.joinable())
		_wallpaper_th.join();
}

auto Sorcery::Animation::stop_attract_th() -> void {

	_finished = true;
	stop_attract();
	if (_attract_th.joinable())
		_attract_th.join();
}

auto Sorcery::Animation::stop_colcyc_th() -> void {

	_finished = true;
	stop_colcyc();
	if (_colcyc_th.joinable())
		_colcyc_th.join();
}

auto Sorcery::Animation::_change_wp(bool force) -> void {

	// Generate an attract mode sprite to display
	if (force)
		_do_wp();
	else {

		do {
			_ctime_wp = std::chrono::system_clock::now();
			const auto elapsed{_ctime_wp - _last_wp};
			if (const auto elapsed_ms{
					std::chrono::duration_cast<std::chrono::milliseconds>(
						elapsed)};
				elapsed_ms.count() > WALLPAPER_INTERVAL) {
				if (_allow_wp)
					_do_wp();
			}

			std::this_thread::sleep_for(
				std::chrono::milliseconds(DELAY_TSLEEP));
		} while (!_finished);
	}
}

auto Sorcery::Animation::_animate_attract(bool force) -> void {

	// Generate an attract mode sprite to display
	if (force)
		_do_attract();
	else {
		do {
			_ctime_attract = std::chrono::system_clock::now();
			const auto elapsed{_ctime_attract - _last_attract};
			if (const auto elapsed_ms{
					std::chrono::duration_cast<std::chrono::milliseconds>(
						elapsed)};
				elapsed_ms.count() > DELAY_ATTRACT)
				if (_allow_attract)
					_do_attract();

			std::this_thread::sleep_for(
				std::chrono::milliseconds(DELAY_TSLEEP));
		} while (!_finished);
	}
}

auto Sorcery::Animation::_colcyc(bool force) -> void {

	// Calculate Colour Cycling
	if (force)
		_do_colcyc();
	else {
		do {
			if (_allow_colcyc)
				_do_colcyc();

			std::this_thread::sleep_for(std::chrono::milliseconds(20));
		} while (!_finished);
	}
}

auto Sorcery::Animation::_do_wp() -> void {

	std::scoped_lock<std::mutex> scoped_lock(_wp_mutex);

	wp_idx = (*_random)[Enums::System::Random::D165];
	_last_wp = std::chrono::system_clock::now();
}

auto Sorcery::Animation::_do_attract() -> void {

	std::scoped_lock<std::mutex> scoped_lock(_attract_mutex);
	auto index{0u};
	const auto num{(*_random)[Enums::System::Random::D4]};
	_attract_mode.clear();
	for (auto i = 0u; i < num; i++) {
		do {
			index = (*_random)[Enums::System::Random::ZERO_TO_399];
		} while (std::ranges::find(_attract_mode.begin(), _attract_mode.end(),
								   index) != _attract_mode.end());
		_attract_mode.push_back(index);
	}
	_last_attract = std::chrono::system_clock::now();
}

auto Sorcery::Animation::get_attract_data() -> std::vector<unsigned int> {

	std::scoped_lock<std::mutex> scoped_lock(_attract_mutex);

	return _attract_mode;
}

// Called 50 times a second
auto Sorcery::Animation::_do_colcyc() -> void {

	std::scoped_lock<std::mutex> scoped_lock(_colcyc_mutex);

	// Colour Lerp goes back and forth between 0 and 1
	if (_colcyc_dir) {
		if (lerp < 1.0L)
			lerp += _colcyc_step;
		else {
			_colcyc_dir = !_colcyc_dir;
			lerp -= _colcyc_step;
		}
	} else {
		if (lerp > 0.0L)
			lerp -= _colcyc_step;
		else {
			_colcyc_dir = !_colcyc_dir;
			lerp += _colcyc_step;
		}
	}

	if (lerp < 0.0L)
		lerp = 0.0L;
	if (lerp > 1.0L)
		lerp = 1.0L;
}
