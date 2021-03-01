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

#include "random.hpp"

// Standard Constructor
Sorcery::Random::Random() {

	// Random Seeding Device
	_random = std::mt19937_64(_device());

	// Populate Ranges
	_range.clear();
	_range[RandomType::D2] = std::make_tuple(1, 2);
	_range[RandomType::D3] = std::make_tuple(1, 3);
	_range[RandomType::D4] = std::make_tuple(1, 4);
	_range[RandomType::D5] = std::make_tuple(1, 5);
	_range[RandomType::D6] = std::make_tuple(1, 6);
	_range[RandomType::D8] = std::make_tuple(1, 8);
	_range[RandomType::D10] = std::make_tuple(1, 10);
	_range[RandomType::D15] = std::make_tuple(1, 15);
	_range[RandomType::D16] = std::make_tuple(1, 16);
	_range[RandomType::D52] = std::make_tuple(1, 52);
	_range[RandomType::D100] = std::make_tuple(1, 100);
	_range[RandomType::D300] = std::make_tuple(1, 300);
	_range[RandomType::ZERO_TO_2] = std::make_tuple(0, 2);
	_range[RandomType::ZERO_TO_3] = std::make_tuple(0, 3);
	_range[RandomType::ZERO_TO_4] = std::make_tuple(0, 4);
	_range[RandomType::ZERO_TO_7] = std::make_tuple(0, 7);
	_range[RandomType::ZERO_TO_8] = std::make_tuple(0, 8);
	_range[RandomType::ZERO_TO_14] = std::make_tuple(0, 14);
	_range[RandomType::ZERO_TO_19] = std::make_tuple(0, 19);
	_range[RandomType::ZERO_TO_29] = std::make_tuple(0, 29);
	_range[RandomType::ZERO_TO_34] = std::make_tuple(0, 34);
	_range[RandomType::ZERO_TO_69] = std::make_tuple(0, 69);
	_range[RandomType::ZERO_TO_99] = std::make_tuple(0, 99);
	_range[RandomType::ZERO_TO_100] = std::make_tuple(0, 100);
	_range[RandomType::ZERO_TO_101] = std::make_tuple(0, 101);
	_range[RandomType::ZERO_TO_129] = std::make_tuple(0, 129);
	_range[RandomType::ZERO_TO_299] = std::make_tuple(0, 299);
}

// Overload [] operator
auto Sorcery::Random::operator[](const RandomType random_type) -> unsigned int {

	return _get(random_type);
}

auto Sorcery::Random::_get(const RandomType random_type) -> unsigned int {

	// Generate a random number of a specified type
	const auto [min, max] = _range[random_type];
	auto dist = std::uniform_int_distribution<unsigned int>(min, max);
	return dist(_random);
}
