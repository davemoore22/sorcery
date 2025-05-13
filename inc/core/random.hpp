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

namespace Sorcery {

class Random {

	public:
		Random();

		auto operator[](const Enums::System::Random random_type)
			-> unsigned int;

		auto get_random_name() -> std::string;
		auto get_type(const int num) const -> Enums::System::Random;
		auto get(const unsigned int min, const unsigned int max)
			-> unsigned int;

	private:
		std::random_device _device;
		std::mt19937_64 _random;
		std::map<Enums::System::Random, std::tuple<unsigned int, unsigned int>>
			_range;
		std::vector<std::string> _sy1;
		std::vector<std::string> _sy2;
		std::vector<std::string> _sy3;

		auto _get(const Enums::System::Random random_type) -> unsigned int;
};

}