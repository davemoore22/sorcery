// Copyright (C) 2026 Dave Moore
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

#include "resources/levelstore.hpp" // for LevelStore
#include "common/enum.hpp"			// for Error, Type
#include "common/types.hpp"			// for Coordinate, Size
#include "resources/json.hpp"
#include "types/error.hpp"		 // for Error, operator<<
#include "types/world/level.hpp" // for Level
#include <cstdlib>				 // for exit, EXIT_FAILURE
#include <exception>			 // for exception
#include <filesystem>			 // for path
#include <fstream>				 // for basic_ifstream, basic_ostream
#include <iostream>				 // for cerr
#include <json/reader.h>		 // for Reader
#include <json/value.h>			 // for Value
#include <map>					 // for map, operator==
#include <optional>				 // for optional, nullopt, nullopt_t
#include <string>				 // for basic_string, string
#include <utility>				 // for pair

Sorcery::LevelStore::LevelStore() {

	_levels.clear();
}

// Standard Constructor
Sorcery::LevelStore::LevelStore(const std::filesystem::path filename) {

	// Prepare the level store
	_levels.clear();

	// Load the levels
	_loaded = _load(filename);
}

auto Sorcery::LevelStore::get(const int depth) const -> std::optional<Level> {

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

		std::ifstream file{filename};

		if (!file)
			return false;

		Json::CharReaderBuilder builder{};
		Json::Value layout{};
		std::string errors{};

		if (!Json::parseFromStream(builder, file, &layout, &errors))
			return false;

		const auto &regions{layout["regions"]};

		if (!regions.isArray() || regions.empty())
			return false;

		const auto &region{regions[0]};

		const auto dungeon{JsonHelper::get_string(region, "name")};

		const auto &layers{region["floors"]};

		if (!layers.isArray())
			return false;

		_levels.clear();

		for (const auto &layer : layers) {

			const auto depth{JsonHelper::get_int(layer, "index")};

			const auto &tiles{layer["tiles"]};
			const auto &bounds{tiles["bounds"]};

			const auto x_origin{JsonHelper::get_int(bounds, "x0")};
			const auto y_origin{JsonHelper::get_int(bounds, "y0")};
			const auto width{JsonHelper::get_int(bounds, "width")};
			const auto height{JsonHelper::get_int(bounds, "height")};

			// Keep these as values for now because Level::load()
			// currently owns the Grid Cartographer parsing step.
			auto rows{tiles["rows"]};
			auto notes{layer["notes"]};

			Level level{Enums::Map::Type::MAZE, dungeon, depth, Coordinate{x_origin, y_origin}, Size{width, height}};

			level.load(rows, notes);

			_levels.emplace(depth, std::move(level));
		}

		return true;
	}

	catch (const std::exception &e) {

		Error error{Enums::System::Error::JSON_PARSE_ERROR, e, "error loading levels.json!"};

		std::cerr << error;
		std::exit(EXIT_FAILURE);
	}
}