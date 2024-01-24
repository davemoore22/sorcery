// Copyright (C) 2024 Dave Moore
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

#include "common/include.hpp"
#include "core/define.hpp"
#include "core/include.hpp"

namespace Sorcery {

// Forward Declarations
class Display;
class System;

class Animation {

	public:

		// Constructors
		Animation(System *system, Display *display);
		Animation() = delete;

		// Standard Destructor
		~Animation();

		// Public Members
		double colour_lerp;
		unsigned int attract_alpha;
		sf::Color selected_colour;
		unsigned int wallpaper_idx;

		// Public Methods
		auto refresh_attract() -> void;
		auto refresh_colcyc() -> void;
		auto refresh_wallpaper() -> void;
		auto start_attract() -> void;
		auto start_attract_ani_threads() -> void;
		auto start_colcyc() -> void;
		auto start_colcycl_threads() -> void;
		auto start_wallpaper() -> void;
		auto start_wallpaper_threads() -> void;
		auto stop_attract() -> void;
		auto stop_attract_threads() -> void;
		auto stop_colcyc() -> void;
		auto stop_colcyc_threads() -> void;
		auto stop_wallpaper() -> void;
		auto stop_wallpaper_threads() -> void;
		auto get_attract_data() -> std::vector<unsigned int>;

	private:

		// Private Members
		System *_system;
		Display *_display;
		std::jthread _attract_thread;
		std::jthread _colcyc_thread;
		std::jthread _wallpaper_thread;
		std::atomic<bool> _allow_attract;
		std::atomic<bool> _allow_colcyc;
		std::atomic<bool> _allow_wallpaper;
		std::atomic<bool> _colcyc_dir;
		std::atomic<bool> _finished;
		std::atomic<bool> _attract_fade;
		std::chrono::time_point<std::chrono::system_clock> _last_wallpaper;
		std::chrono::time_point<std::chrono::system_clock> _last_attract;
		std::chrono::time_point<std::chrono::system_clock> _ctime_attract;
		std::chrono::time_point<std::chrono::system_clock> _ctime_wallpaper;
		std::mutex _attract_mutex;
		std::mutex _colour_mutex;
		std::mutex _wallpaper_mutex;
		std::vector<unsigned int> _attract_mode;
		thor::ColorGradient _selected_gradient;
		double _colcyc_step;

		// Private Methods
		auto _change_wallpaper(bool force) -> void;
		auto _animate_attract(bool force) -> void;
		auto _colcyc(bool force) -> void;
		auto _do_attract() -> void;
		auto _do_colcyc() -> void;
		auto _do_wallpaper() -> void;
};

}
