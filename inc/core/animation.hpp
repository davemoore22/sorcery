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

#pragma once

#include "common/include.hpp"
#include "core/define.hpp"
#include "core/include.hpp"

namespace Sorcery {

class Random;

class Animation {
	public:
		Animation(Random *random);
		Animation() = delete;
		~Animation();

		double lerp;
		double fade;
		unsigned int wp_idx;

		auto refresh_attract() -> void;
		auto refresh_colcyc() -> void;
		auto refresh_wp() -> void;
		auto start_attract() -> void;
		auto start_attract_th() -> void;
		auto start_colcyc() -> void;
		auto start_colcycl_th() -> void;
		auto start_wp() -> void;
		auto start_wp_th() -> void;
		auto stop_attract() -> void;
		auto stop_attract_th() -> void;
		auto stop_colcyc() -> void;
		auto stop_colcyc_th() -> void;
		auto stop_wp() -> void;
		auto stop_wp_th() -> void;
		auto get_attract_data() -> std::vector<unsigned int>;

	private:
		Random *_random;
		std::jthread _attract_th;
		std::jthread _colcyc_th;
		std::jthread _wallpaper_th;
		std::atomic<bool> _allow_attract;
		std::atomic<bool> _allow_colcyc;
		std::atomic<bool> _allow_wp;
		std::atomic<bool> _colcyc_dir;
		std::atomic<bool> _finished;
		std::atomic<bool> _attract_fade;
		std::chrono::time_point<std::chrono::system_clock> _last_wp;
		std::chrono::time_point<std::chrono::system_clock> _last_attract;
		std::chrono::time_point<std::chrono::system_clock> _ctime_attract;
		std::chrono::time_point<std::chrono::system_clock> _ctime_wp;
		std::mutex _attract_mutex;
		std::mutex _colcyc_mutex;
		std::mutex _wp_mutex;
		std::vector<unsigned int> _attract_mode;
		double _colcyc_step;

		auto _change_wp(bool force) -> void;
		auto _animate_attract(bool force) -> void;
		auto _colcyc(bool force) -> void;
		auto _do_attract() -> void;
		auto _do_colcyc() -> void;
		auto _do_wp() -> void;
};

}