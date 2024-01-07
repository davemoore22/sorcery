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

#include "main.hpp"

namespace Sorcery {

class Dice {

	public:

		// Constructors
		Dice();
		Dice(const unsigned int num_, const unsigned int dice_);
		Dice(const std::string dice_);
		Dice(const unsigned int num_, const unsigned int dice_, const int mod_);

		// Public Methods
		auto roll() const -> int;
		auto roll_min() const -> int;
		auto roll_max() const -> int;
		auto set(const unsigned int num_, const unsigned int dice_, const int mod_);
		auto str() const -> std::string;

		// Public Members
		unsigned int num;
		unsigned int dice;
		int mod;

		// Shared RNG
		static std::random_device _device;
		static std::mt19937_64 _random;
};

}