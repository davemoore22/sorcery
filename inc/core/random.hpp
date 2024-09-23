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

// Class to handle all Random Number generation
namespace Sorcery {

class Random {

	public:

		// Constructor
		Random();

		// Overload [] operator
		auto operator[](const RNT random_type) -> unsigned int;

		// Public Methods
		auto get_random_name() -> std::string;
		auto get_type(const int num) const -> RNT;
		auto get(const unsigned int min, const unsigned int max)
			-> unsigned int;

		// Public Members

	private:

		// Private Members
		std::random_device _device;
		std::mt19937_64 _random;
		std::map<RNT, Range> _range;
		std::vector<std::string> _sy1;
		std::vector<std::string> _sy2;
		std::vector<std::string> _sy3;

		// Private Methods
		auto _get(const RNT random_type) -> unsigned int;
};

}