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

#include "resources/levelstore.hpp"
#include "common/enum.hpp"
#include "common/include.hpp"
#include "common/type.hpp"
#include "core/system.hpp"
#include "types/error.hpp"

Sorcery::LevelStore::LevelStore() {
	_levels.clear();
}

Sorcery::LevelStore::LevelStore(System *system) : _system{system} {

	// Prepare the level store
	_levels.clear();
}

// Standard Constructor
Sorcery::LevelStore::LevelStore(System *system, const std::filesystem::path filename) : _system{system} {

	// Prepare the level store
	_levels.clear();

	// Load the levels
	_loaded = _load(filename);
}

// Overload [] Operator(const)
auto Sorcery::LevelStore::operator[](const int depth) const -> std::optional<Level> {

	auto level{_get(depth)};
	return level;
}

// Method called to simulate Normal Constructor with Cereal Constructor
auto Sorcery::LevelStore::set(System *system) -> void {

	_system = system;
}

auto Sorcery::LevelStore::_get(const int depth) const -> std::optional<Level> {

	if (_loaded) {
		auto it{_levels.find(depth)};
		if (it != _levels.end())
			return it->second;
		else
			return std::nullopt;
	} else
		return std::nullopt;
}

auto Sorcery::LevelStore::_load(const std::filesystem::path filename) -> bool {

	try {

		if (std::ifstream file{filename.string(), std::ifstream::binary}; file.good()) {

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
			Json::Reader reader{};
#pragma GCC diagnostic pop
			if (Json::Value layout; reader.parse(file, layout)) {
				Json::Value &regions{layout["regions"]};
				std::string dungeon{regions[0]["name"].asString()};
				Json::Value &layers{regions[0]["floors"]};
				for (auto j = 0u; j < layers.size(); j++) {

					// Top Level Data items
					auto depth{layers[j]["index"].asInt()};
					auto tiles{layers[j]["tiles"]};
					auto notes{layers[j]["notes"]};

					// Second Level Data Items
					auto bounds{layers[j]["tiles"]["bounds"]};
					auto rows{layers[j]["tiles"]["rows"]};

					// Workout Level Metadata
					auto x_origin{bounds["x0"].asInt()};
					auto y_origin{bounds["y0"].asInt()};
					auto width{bounds["width"].asInt()};
					auto height{bounds["height"].asInt()};

					// Create the Level
					Level level{MAT::MAZE, dungeon, depth, Coordinate(x_origin, y_origin), Size(width, height)};
					level.load(rows, notes);

					// Store it
					_levels[depth] = level;
				}
			} else
				return false;
		} else
			return false;

		return true;
	}

	catch (std::exception &e) {
		Error error{SYE::JSON_PARSE_ERROR, e, "error loading levels.json!"};
		std::cout << error;
		exit(EXIT_FAILURE);
	}
}