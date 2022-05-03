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

#include "view.hpp"

// Standard Constructor
Sorcery::View::View(
	System *system, Display *display, Graphics *graphics, Game *game)
	: _system{system}, _display{display}, _graphics{graphics}, _game{game} {

	// Load the layout from file
	_loaded = _load((*_system->files)[VIEW_FILE]);
}

// Overload [] Operator
auto Sorcery::View::operator[](Coordinate3 point) -> ViewNode & {

	try {

		return _nodes.at(point);

	} catch (std::exception &e) {
		Error error{
			SystemError::JSON_PARSE_ERROR, e, "view.json is not valid JSON!"};
		std::cout << error;
		exit(EXIT_FAILURE);
	}
}

// Get all used nodes for a depth
auto Sorcery::View::operator[](int z) -> std::vector<ViewNode *> {

	try {

		auto matches{_nodes | std::views::filter([&](auto &item) {
			return item.second.coords.z == z && item.second.used;
		})};
		std::vector<ViewNode *> results;
		results.clear();
		for (auto &node : matches)
			results.emplace_back(&node.second);

		return results;

	} catch (std::exception &e) {
		Error error{
			SystemError::JSON_PARSE_ERROR, e, "view.json is not valid JSON!"};
		std::cout << error;
		exit(EXIT_FAILURE);
	}
}

auto Sorcery::View::_preload(const int depth, const int width) -> void {

	_nodes.clear();
	for (auto x = 0 - width; x <= width; x++) {
		for (auto z = 0; z <= depth; z++) {

			ViewNode empty_node{};
			Coordinate3 coords{x, 0, z};
			_nodes[coords] = empty_node;
		}
	}
}

auto Sorcery::View::_load(const std::filesystem::path filename) -> bool {

	try {
		if (std::ifstream file{filename.string(), std::ifstream::binary};
			file.good()) {

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
			Json::Reader reader{};
#pragma GCC diagnostic pop
			Json::StreamWriterBuilder builder{};
			builder.settings_["indentation"] = "";
			if (Json::Value layout; reader.parse(file, layout)) {

				const auto depth{static_cast<int>(layout["depth"].asInt())};
				const auto width{static_cast<int>(layout["width"].asInt())};

				_preload(depth, width);

				Json::Value &layers{layout["layers"]};

				// Iterate through view file one layer at a time
				for (auto i = 0u; i < layers.size(); i++) {
					auto layer_type{[&] {
						if (layers[i].isMember("type")) {
							if (auto lt{layers[i]["type"].asString()};
								lt.length() > 0) {
								if (lt == "Floor")
									return ViewNodeLayer::FLOOR;
								else if (lt == "Ceiling")
									return ViewNodeLayer::CEILING;
								else if (lt == "Walls")
									return ViewNodeLayer::WALLS;
								else if (lt == "Object")
									return ViewNodeLayer::OBJECT;
								else if (lt == "Decal")
									return ViewNodeLayer::DECAL;
								else
									return ViewNodeLayer::NONE;
							} else
								return ViewNodeLayer::NONE;
						} else
							return ViewNodeLayer::NONE;
					}()};
					Json::Value &tiles{layers[i]["tiles"]};
					for (auto j = 0u; j < tiles.size(); j++) {

						// Tile
						// Screen
						// coords/fullwidth
						auto tile_type{[&] {
							if (tiles[j].isMember("type")) {
								if (auto tt{layers[i]["type"].asString()};
									tt.length() > 0) {
									if (tt == "floor")
										return ViewNodeType::FLOOR;
									else if (tt == "ceiling")
										return ViewNodeType::CEILING;
									else if (tt == "front")
										return ViewNodeType::FRONT;
									else if (tt == "side")
										return ViewNodeType::SIDE;
									else if (tt == "object")
										return ViewNodeType::OBJECT;
									else
										return ViewNodeType::NONE;

								} else
									return ViewNodeType::NONE;
							} else
								return ViewNodeType::NONE;
						}()};
						auto tile_flipped{[&] {
							if (tiles[j].isMember("flipped"))
								return tiles[j]["flipped"].asBool();
							else
								return false;
						}()};
						Json::Value &tile_v{tiles[j]["tile"]};
						int t_x{tile_v["x"].asInt()};
						int t_z{tile_v["z"].asInt()};
						int t_y{
							0}; // Not used as there's only one layer of tile
						Coordinate3 t_coords{t_x, t_y, t_z};

						Json::Value &screen_v{tiles[j]["screen"]};
						unsigned int d_x{screen_v["x"].asUInt()};
						unsigned int d_y{screen_v["y"].asUInt()};
						Point d_coords{d_x, d_y};

						Json::Value &coords{tiles[j]["coords"]};
						unsigned int d_fw{coords["fullWidth"].asUInt()};

						ViewNode view_node{layer_type, tile_type, tile_flipped,
							t_coords, d_coords, d_fw};

						_nodes[t_coords] = view_node;
					}
				}

				_depth = depth;
				_width = width;
			} else
				return false;
		} else
			return true;

		return true;

	} catch (std::exception &e) {
		Error error{
			SystemError::JSON_PARSE_ERROR, e, "view.json is not valid JSON!"};
		std::cout << error;
		exit(EXIT_FAILURE);
	}
}

auto Sorcery::View::get(const int x, const int z) -> ViewNode & {

	return _get(x, z);
}

auto Sorcery::View::_get(const int x, const int z) -> ViewNode & {

	return _nodes.at(Coordinate3{x, 0, z});
}

auto Sorcery::View::get_to_depth(bool lit) -> std::vector<ViewNode *> {

	auto depth{lit ? LIGHT_VIEW_DEPTH : DARK_VIEW_DEPTH};
	std::vector<ViewNode *> results;
	results.clear();

	for (auto z = 0; z <= depth; z++) {
		auto matches{_nodes | std::views::filter([&](auto &item) {
			return item.second.coords.z == z && item.second.used;
		})};
		for (auto &node : matches)
			results.emplace_back(&node.second);
	}

	return results;
}