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

#pragma once

#include "main.hpp"
#include "system.hpp"
#include "display.hpp"

namespace Sorcery {

	class Animation {

		public:

			// Standard Constructor
			Animation(System& system, Display& display);

			// Standard Destructor
			virtual ~Animation();

			// Public Members
			double colour_lerp;

			// Public Methods
			auto force_refresh_attract_mode() -> void;
			auto force_refresh_colour_cycling() -> void;
			auto start_attract_mode_animation() ->void;
			auto start_attract_mode_animation_threads() -> void;
			auto start_colour_cycling() ->void;
			auto start_colour_cycling_threads() -> void;
			auto stop_attract_mode_animation() -> void;
			auto stop_attract_mode_animation_threads() -> void;
			auto stop_colour_cycling() -> void;
			auto stop_colour_cycling_threads() -> void;
			auto get_attract_mode_data() -> std::vector<unsigned int>;

		private:

			// Private Members
			System _system;
			Display _display;
			std::thread _attract_mode_animation_thread;
			std::thread _colour_cycling_thread;
			std::atomic<bool> _allow_attract_mode_animations;
			std::atomic<bool> _allow_colour_cycling;
			std::atomic<bool> _colour_cycling_direction;
			std::atomic<bool> _finished;
			std::chrono::time_point<std::chrono::system_clock> _last_attract_mode_animation;
			std::chrono::time_point<std::chrono::system_clock> _current_time;
			std::mutex _attract_mode_mutex;
			std::mutex _colour_cycling_mutex;
			std::vector<unsigned int> _attract_mode;

			// Private Methods
			auto _animate_attract_mode(bool force) -> void;
			auto _calculate_attract_mode_screen_positions(const unsigned int number_to_display) const ->
				std::vector<unsigned int>;
			auto _colour_cycling(bool force) -> void;
			auto _do_attract_mode_animation() -> void;
			auto _do_colour_cycling() -> void;
	};
}