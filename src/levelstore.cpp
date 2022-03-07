// Copyright (C) 2021 Dave Moore
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

#include "levelstore.hpp"

// Standard Constructor
Sorcery::LevelStore::LevelStore(
	System *system, const std::filesystem::path filename)
	: _system{system} {

	// Prepare the level store
	_levels.clear();

	// Load the levels
	_loaded = _load(filename);
}

// Overload [] Operator(const)
auto Sorcery::LevelStore::operator[](const std::string name) const
	-> std::optional<Level> {

	auto level{_get(name)};
	return level;
}

auto Sorcery::LevelStore::_get(const std::string name) const
	-> std::optional<Level> {

	if (_loaded) {
		auto it{_levels.find(name)};
		if (it != _levels.end())
			return it->second;
		else
			return std::nullopt;
	} else
		return std::nullopt;
}

auto Sorcery::LevelStore::_load(const std::filesystem::path filename) -> bool {

	if (std::ifstream file{filename.string(), std::ifstream::binary};
		file.good()) {

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
		Json::Reader reader{};
#pragma GCC diagnostic pop
	} else
		return false;

	return true;
}
