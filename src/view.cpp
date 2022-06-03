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

	_load_tile_views();
}

// Overload [] Operator
auto Sorcery::View::operator[](ViewNodeKey key) -> ViewNode {

	try {

		return _nodes.at(key);

	} catch (std::exception &e) {
		Error error{
			SystemError::JSON_PARSE_ERROR, e, "view.json is not valid JSON!"};
		std::cout << error;
		exit(EXIT_FAILURE);
	}
}

// Get all used nodes for a depth
auto Sorcery::View::operator[](int z) -> std::vector<ViewNode> {

	try {

		auto matches{_nodes | std::views::filter([&](auto &item) {
			return item.second.coords.z == z && item.second.used;
		})};
		std::vector<ViewNode> results;
		results.clear();
		for (auto &node : matches)
			results.push_back(node.second);

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
			ViewNodeKey coords{ViewNodeLayer::NONE, x, 0, z};
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

						auto tile_type{[&] {
							if (tiles[j].isMember("type")) {
								if (auto tt{tiles[j]["type"].asString()};
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

						// Note that in the Atlas Export, z is from 0 to -6 for
						// 6 tiles in depth, but we reverse that for ease of use
						// and to ensure that z is positive
						int t_z{tile_v["z"].asInt()};
						t_z = 0 - t_z;

						// y is not used as there's only one layer of tile (and
						// floors and ceilings are rendered seperate)
						int t_y{0};
						Coordinate3 t_coords{t_x, t_y, t_z};

						// Destination Location
						Json::Value &screen_v{tiles[j]["screen"]};
						unsigned int d_x{screen_v["x"].asUInt()};
						unsigned int d_y{screen_v["y"].asUInt()};
						Point d_coords{d_x, d_y};

						// Source Graphic
						Json::Value &source{tiles[j]["coords"]};
						unsigned int d_fw{source["fullWidth"].asUInt()};
						unsigned int d_rx{source["x"].asUInt()};
						unsigned int d_ry{source["y"].asUInt()};
						unsigned int d_rw{source["w"].asUInt()};
						unsigned int d_rh{source["h"].asUInt()};
						Rect d_r{d_rx, d_ry, d_rw, d_rh};

						ViewNode view_node{layer_type, tile_type, tile_flipped,
							t_coords, d_coords, d_fw, d_r};
						ViewNodeKey key{
							layer_type, t_coords.x, t_coords.y, t_coords.z};

						_nodes[key] = view_node;
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

auto Sorcery::View::get(
	const ViewNodeLayer layer, const int x, const int z) const -> ViewNode {

	return _get(layer, x, z);
}

auto Sorcery::View::_get(
	const ViewNodeLayer layer, const int x, const int z) const -> ViewNode {

	return _nodes.at(ViewNodeKey{layer, x, 0, z});
}

auto Sorcery::View::get_nodes_at_depth(
	const ViewNodeLayer layer, const int z) const -> std::vector<ViewNode> {

	std::vector<ViewNode> results;
	results.clear();

	auto matches{_nodes | std::views::filter([&](auto &item) {
		return (item.second.layer == layer) && (item.second.coords.z == z) &&
			   item.second.used;
	})};
	for (auto &node : matches)
		results.push_back(node.second);

	return results;
}

auto Sorcery::View::get_nodes_at_depth(const ViewNodeLayer layer,
	const ViewNodeType type, const int x_sgn, const int z) const
	-> std::vector<ViewNode> {

	std::vector<ViewNode> results;
	results.clear();

	// Hack here since View Generator does outside font walls, whereas we want
	// interior walls (side walls are correct())
	auto depth{type == ViewNodeType::FRONT ? z + 1 : z};

	auto matches{_nodes | std::views::filter([&](auto &item) {
		return (item.second.layer == layer) &&
			   (item.second.coords.z == depth) && (item.second.type == type) &&
			   (Sorcery::sgn(x_sgn) == Sorcery::sgn(item.second.coords.x)) &&
			   item.second.used;
	})};
	for (auto &node : matches)
		results.push_back(node.second);

	return results;
}

auto Sorcery::View::get_nodes_at_depth(const ViewNodeLayer layer,
	const ViewNodeType type, const int z) const -> std::vector<ViewNode> {

	std::vector<ViewNode> results;
	results.clear();

	// Hack here since View Generator does outside font walls, whereas we want
	// interior walls (side walls are correct())
	auto depth{type == ViewNodeType::FRONT ? z + 1 : z};

	auto matches{_nodes | std::views::filter([&](auto &item) {
		return (item.second.layer == layer) &&
			   (item.second.coords.z == depth) && (item.second.type == type) &&
			   item.second.used;
	})};
	for (auto &node : matches)
		results.push_back(node.second);

	return results;
}

auto Sorcery::View::width() -> unsigned int {

	return _width;
}

auto Sorcery::View::depth() -> unsigned int {

	return _depth;
}

auto Sorcery::View::get_lit_nodes(const ViewNodeLayer layer, bool lit) const
	-> std::vector<ViewNode> {

	auto depth{lit ? LIGHT_VIEW_DEPTH : DARK_VIEW_DEPTH};
	std::vector<ViewNode> results;
	results.clear();

	for (auto z = 0; z <= depth; z++) {
		auto matches{_nodes | std::views::filter([&](auto &item) {
			return (item.second.layer == layer) &&
				   (item.second.coords.z == z) && item.second.used;
		})};
		for (auto &node : matches)
			results.push_back(node.second);
	}

	std::sort(results.begin(), results.end());

	return results;
}

auto Sorcery::View::_set_texture_coordinates(TileView &tileview) -> void {

	constexpr auto size_panel_x{304u};
	constexpr auto size_panel_y{176u};
	for (auto i = 0; i <= 3; i++) {
		tileview.floor[i].texCoords = tileview.floor[i].position;
		tileview.ceiling[i].texCoords = tileview.ceiling[i].position;
		tileview.darkness[i].texCoords.x =
			tileview.darkness[i].position.x + (0 * size_panel_x);
		tileview.darkness[i].texCoords.y =
			tileview.darkness[i].position.y + (1 * size_panel_y);
		tileview.side_darkness[i].texCoords.x =
			tileview.side_darkness[i].position.x + (1 * size_panel_x);
		tileview.side_darkness[i].texCoords.y =
			tileview.side_darkness[i].position.y + (0 * size_panel_y);
		tileview.left_side_wall[i].texCoords.x =
			tileview.left_side_wall[i].position.x + (1 * size_panel_x);
		tileview.left_side_wall[i].texCoords.y =
			tileview.left_side_wall[i].position.y + (1 * size_panel_y);
		tileview.right_side_wall[i].texCoords.x =
			tileview.right_side_wall[i].position.x + (1 * size_panel_x);
		tileview.right_side_wall[i].texCoords.y =
			tileview.right_side_wall[i].position.y + (1 * size_panel_y);
		tileview.left_side_door[i].texCoords.x =
			tileview.left_side_door[i].position.x + (2 * size_panel_x);
		tileview.left_side_door[i].texCoords.y =
			tileview.left_side_door[i].position.y + (0 * size_panel_y);
		tileview.right_side_door[i].texCoords.x =
			tileview.right_side_door[i].position.x + (2 * size_panel_x);
		tileview.right_side_door[i].texCoords.y =
			tileview.right_side_door[i].position.y + (0 * size_panel_y);

		auto offset_x{0};
		auto offset_y{0};
		if (tileview.offset.z == 0) {
			offset_x = 0;
			offset_y = 3;
		} else if (tileview.offset.z == -1) {
			offset_x = 1;
			offset_y = 3;
		} else if (tileview.offset.z == -2) {
			offset_x = 2;
			offset_y = 3;
		} else if (tileview.offset.z == -3) {
			offset_x = 0;
			offset_y = 4;
		}
		tileview.back_wall[i].texCoords.x =
			tileview.back_wall[i].position.x + (offset_x * size_panel_x);
		tileview.back_wall[i].texCoords.y =
			tileview.back_wall[i].position.y + (offset_y * size_panel_y);

		if (tileview.offset.z == 0) {
			offset_x = 2;
			offset_y = 1;
		} else if (tileview.offset.z == -1) {
			offset_x = 0;
			offset_y = 2;
		} else if (tileview.offset.z == -2) {
			offset_x = 1;
			offset_y = 2;
		} else if (tileview.offset.z == -3) {
			offset_x = 2;
			offset_y = 2;
		}
		tileview.back_door[i].texCoords.x =
			tileview.back_door[i].position.x + (offset_x * size_panel_x);
		tileview.back_door[i].texCoords.y =
			tileview.back_door[i].position.y + (offset_y * size_panel_y);
	}
}

auto Sorcery::View::_set_vertex_array(sf::VertexArray &array, sf::Vector2f p1,
	sf::Vector2f p2, sf::Vector2f p3, sf::Vector2f p4) -> void {

	array[0].position = p1;
	array[1].position = p2;
	array[2].position = p3;
	array[3].position = p4;
}

auto Sorcery::View::_load_tile_views() -> void {

	tileviews.clear();
	for (auto x = -1; x <= 1; x++) {
		for (auto z = 0; z >= -5; z--) {
			Coordinate3 loc{x, 0, z};
			tileviews[loc] = TileView{loc};
		}
	}

	//  FLOORS/CEILINGS				SIDE DARKNESS			SIDE DOORS
	//	FRONT DARKNESS				SIDE WALLS				FRONT DOORS z =
	// 0
	//  FRONT DOORS z = -1			FRONT DOORS z = -2		FRONT DOORS z =
	//  -3
	//  FRONT WALLS z = 0			FRONT WALLS z = -1		FRONT WALLS z =
	//  -2 FRONT WALLS z = -3

	// https://www.sfml-dev.org/tutorials/2.5/graphics-vertex-array.php

	// Tile the player is standing on
	{
		Coordinate3 tile{0, 0, 0};
		auto &tileview{tileviews.at(tile)};

		// Set Vertex Arrays
		_set_vertex_array(tileview.floor, sf::Vector2f{88, 167},
			sf::Vector2f{95, 160}, sf::Vector2f{208, 160},
			sf::Vector2f{215, 167});
		_set_vertex_array(tileview.ceiling, sf::Vector2f{95, 15},
			sf::Vector2f{88, 8}, sf::Vector2f{215, 8}, sf::Vector2f{208, 15});
		_set_vertex_array(tileview.darkness, sf::Vector2f{8, 167},
			sf::Vector2f{8, 8}, sf::Vector2f{295, 8}, sf::Vector2f{295, 167});

		_set_vertex_array(tileview.back_wall, sf::Vector2f{87, 153},
			sf::Vector2f{87, 23}, sf::Vector2f{217, 23},
			sf::Vector2f{217, 153});
		_set_vertex_array(tileview.back_door, sf::Vector2f{87, 153},
			sf::Vector2f{87, 23}, sf::Vector2f{217, 23},
			sf::Vector2f{217, 153});

		/* // Left Wall/Door
		tileview.left_side_wall[0].position = sf::Vector2f(8, 167);
		tileview.left_side_wall[1].position = sf::Vector2f(8, 8);
		tileview.left_side_wall[2].position = sf::Vector2f(87, 8);
		tileview.left_side_wall[3].position = sf::Vector2f(87, 167);
		tileview.left_side_door[0].position = sf::Vector2f(8, 167);
		tileview.left_side_door[1].position = sf::Vector2f(8, 8);
		tileview.left_side_door[2].position = sf::Vector2f(87, 8);
		tileview.left_side_door[3].position = sf::Vector2f(87, 167);

		// Right Wall/Door
		tileview.right_side_wall[0].position = sf::Vector2f(216, 167);
		tileview.right_side_wall[1].position = sf::Vector2f(216, 8);
		tileview.right_side_wall[2].position = sf::Vector2f(295, 8);
		tileview.right_side_wall[3].position = sf::Vector2f(295, 167);
		tileview.right_side_door[0].position = sf::Vector2f(216, 167);
		tileview.right_side_door[1].position = sf::Vector2f(216, 8);
		tileview.right_side_door[2].position = sf::Vector2f(295, 8);
		tileview.right_side_door[3].position = sf::Vector2f(296, 167);

		// Back Wall/Door
		tileview.back_wall[0].position = sf::Vector2f(87, 152);
		tileview.back_wall[1].position = sf::Vector2f(87, 23);
		tileview.back_wall[2].position = sf::Vector2f(216, 23);
		tileview.back_wall[3].position = sf::Vector2f(216, 152);
		tileview.back_door[0].position = sf::Vector2f(87, 152);
		tileview.back_door[1].position = sf::Vector2f(87, 23);
		tileview.back_door[2].position = sf::Vector2f(216, 23);
		tileview.back_door[3].position = sf::Vector2f(216, 152); */

		_set_texture_coordinates(tileview);
	}

	// Tile in Front of the Player
	{
		Coordinate3 tile{0, 0, -1};
		auto &tileview{tileviews.at(tile)};

		// Set Vertex Arrays
		_set_vertex_array(tileview.floor, sf::Vector2f{112, 143},
			sf::Vector2f{127, 128}, sf::Vector2f{176, 128},
			sf::Vector2f{191, 143});
		_set_vertex_array(tileview.ceiling, sf::Vector2f{127, 47},
			sf::Vector2f{112, 32}, sf::Vector2f{191, 32},
			sf::Vector2f{176, 47});
		_set_vertex_array(tileview.darkness, sf::Vector2f{87, 152},
			sf::Vector2f{87, 23}, sf::Vector2f{216, 23},
			sf::Vector2f{216, 152});

		// Left Wall/Door
		tileview.left_side_wall[0].position = sf::Vector2f(87, 152);
		tileview.left_side_wall[1].position = sf::Vector2f(87, 23);
		tileview.left_side_wall[2].position = sf::Vector2f(119, 23);
		tileview.left_side_wall[3].position = sf::Vector2f(119, 152);
		tileview.left_side_door[0].position = sf::Vector2f(87, 152);
		tileview.left_side_door[1].position = sf::Vector2f(87, 23);
		tileview.left_side_door[2].position = sf::Vector2f(119, 23);
		tileview.left_side_door[3].position = sf::Vector2f(119, 152);

		// Right Wall/Door
		tileview.right_side_wall[0].position = sf::Vector2f(188, 152);
		tileview.right_side_wall[1].position = sf::Vector2f(188, 23);
		tileview.right_side_wall[2].position = sf::Vector2f(216, 23);
		tileview.right_side_wall[3].position = sf::Vector2f(216, 152);
		tileview.right_side_door[0].position = sf::Vector2f(188, 152);
		tileview.right_side_door[1].position = sf::Vector2f(188, 23);
		tileview.right_side_door[2].position = sf::Vector2f(216, 23);
		tileview.right_side_door[3].position = sf::Vector2f(216, 152);

		// Back Wall/Door
		tileview.back_wall[0].position = sf::Vector2f(119, 120);
		tileview.back_wall[1].position = sf::Vector2f(119, 55);
		tileview.back_wall[2].position = sf::Vector2f(184, 55);
		tileview.back_wall[3].position = sf::Vector2f(184, 120);
		tileview.back_door[0].position = sf::Vector2f(119, 120);
		tileview.back_door[1].position = sf::Vector2f(119, 55);
		tileview.back_door[2].position = sf::Vector2f(184, 55);
		tileview.back_door[3].position = sf::Vector2f(184, 120);

		_set_texture_coordinates(tileview);
	}

	// Tile 2 in Front of the Player
	{
		Coordinate3 tile{0, 0, -2};
		auto &tileview{tileviews.at(tile)};

		// Set Vertex Arrays
		_set_vertex_array(tileview.darkness, sf::Vector2f{120, 119},
			sf::Vector2f{120, 56}, sf::Vector2f{183, 56},
			sf::Vector2f{183, 119});

		// Left Wall/Door
		tileview.left_side_wall[0].position = sf::Vector2f(120, 119);
		tileview.left_side_wall[1].position = sf::Vector2f(120, 56);
		tileview.left_side_wall[2].position = sf::Vector2f(135, 56);
		tileview.left_side_wall[3].position = sf::Vector2f(135, 119);
		tileview.left_side_door[0].position = sf::Vector2f(120, 119);
		tileview.left_side_door[1].position = sf::Vector2f(120, 56);
		tileview.left_side_door[2].position = sf::Vector2f(135, 56);
		tileview.left_side_door[3].position = sf::Vector2f(135, 119);

		// Right Wall/Door
		tileview.right_side_wall[0].position = sf::Vector2f(168, 119);
		tileview.right_side_wall[1].position = sf::Vector2f(168, 56);
		tileview.right_side_wall[2].position = sf::Vector2f(184, 56);
		tileview.right_side_wall[3].position = sf::Vector2f(184, 119);
		tileview.right_side_door[0].position = sf::Vector2f(168, 119);
		tileview.right_side_door[1].position = sf::Vector2f(168, 56);
		tileview.right_side_door[2].position = sf::Vector2f(184, 56);
		tileview.right_side_door[3].position = sf::Vector2f(184, 119);

		// Back Wall/Door
		tileview.back_wall[0].position = sf::Vector2f(135, 104);
		tileview.back_wall[1].position = sf::Vector2f(135, 71);
		tileview.back_wall[2].position = sf::Vector2f(168, 71);
		tileview.back_wall[3].position = sf::Vector2f(168, 104);
		tileview.back_door[0].position = sf::Vector2f(135, 104);
		tileview.back_door[1].position = sf::Vector2f(135, 71);
		tileview.back_door[2].position = sf::Vector2f(168, 71);
		tileview.back_door[3].position = sf::Vector2f(168, 104);

		_set_texture_coordinates(tileview);
	}

	// Tile 3 in Front of the Player
	{
		Coordinate3 tile{0, 0, -3};
		auto &tileview{tileviews.at(tile)};

		// Set Vertex Arrays
		_set_vertex_array(tileview.darkness, sf::Vector2f{135, 104},
			sf::Vector2f{135, 71}, sf::Vector2f{168, 71},
			sf::Vector2f{168, 104});

		// No Floor
		// No Ceiling

		/* // Left Wall/Door
		tileview.left_side_wall[0].position = sf::Vector2f(120, 119);
		tileview.left_side_wall[1].position = sf::Vector2f(120, 56);
		tileview.left_side_wall[2].position = sf::Vector2f(135, 56);
		tileview.left_side_wall[3].position = sf::Vector2f(135, 119);
		tileview.left_side_door[0].position = sf::Vector2f(120, 119);
		tileview.left_side_door[1].position = sf::Vector2f(120, 56);
		tileview.left_side_door[2].position = sf::Vector2f(135, 56);
		tileview.left_side_door[3].position = sf::Vector2f(135, 119);

		// Right Wall/Door
		tileview.right_side_wall[0].position = sf::Vector2f(168, 119);
		tileview.right_side_wall[1].position = sf::Vector2f(168, 56);
		tileview.right_side_wall[2].position = sf::Vector2f(184, 56);
		tileview.right_side_wall[3].position = sf::Vector2f(184, 119);
		tileview.right_side_door[0].position = sf::Vector2f(168, 119);
		tileview.right_side_door[1].position = sf::Vector2f(168, 56);
		tileview.right_side_door[2].position = sf::Vector2f(184, 56);
		tileview.right_side_door[3].position = sf::Vector2f(184, 119); */

		// Back Wall/Door
		_set_vertex_array(tileview.back_wall, sf::Vector2f{143, 96},
			sf::Vector2f{143, 79}, sf::Vector2f{160, 79},
			sf::Vector2f{160, 96});
		_set_vertex_array(tileview.back_door, sf::Vector2f{143, 96},
			sf::Vector2f{143, 79}, sf::Vector2f{160, 79},
			sf::Vector2f{160, 96});

		_set_texture_coordinates(tileview);
	}

	// Tile 4 in Front of the Player
	{
		Coordinate3 tile{0, 0, -4};
		auto &tileview{tileviews.at(tile)};

		// No Floor
		// No Ceiling

		// Set Vertex Arrays
		_set_vertex_array(tileview.darkness, sf::Vector2f{143, 96},
			sf::Vector2f{143, 79}, sf::Vector2f{160, 79},
			sf::Vector2f{160, 96});

		// No Left Wall/Door
		// No Right Wall/Door
		// No Back Wall/Door

		_set_texture_coordinates(tileview);
	}

	// Tile to the Immediate Left of the Player
	{
		Coordinate3 tile{-1, 0, 0};
		auto &tileview{tileviews.at(tile)};

		// Set Vertex Arrays
		_set_vertex_array(tileview.floor, sf::Vector2f{8, 167},
			sf::Vector2f{8, 160}, sf::Vector2f{63, 160}, sf::Vector2f{56, 167});
		_set_vertex_array(tileview.ceiling, sf::Vector2f{8, 15},
			sf::Vector2f{8, 8}, sf::Vector2f{56, 8}, sf::Vector2f{63, 15});
		_set_vertex_array(tileview.darkness, sf::Vector2f{8, 152},
			sf::Vector2f{8, 24}, sf::Vector2f{87, 24}, sf::Vector2f{87, 152});
		_set_vertex_array(tileview.side_darkness, sf::Vector2f{8, 167},
			sf::Vector2f{8, 8}, sf::Vector2f{87, 8}, sf::Vector2f{87, 167});
		_set_vertex_array(tileview.back_wall, sf::Vector2f{8, 153},
			sf::Vector2f{8, 23}, sf::Vector2f{88, 23}, sf::Vector2f{88, 153});
		_set_vertex_array(tileview.back_door, sf::Vector2f{8, 153},
			sf::Vector2f{8, 23}, sf::Vector2f{88, 23}, sf::Vector2f{88, 153});

		_set_texture_coordinates(tileview);
	}

	// Tile to the Immediate Right of the Player
	{
		Coordinate3 tile{1, 0, 0};
		auto &tileview{tileviews.at(tile)};

		// Set Vertex Arrays
		_set_vertex_array(tileview.floor, sf::Vector2f{239, 167},
			sf::Vector2f{232, 160}, sf::Vector2f{295, 160},
			sf::Vector2f{295, 167});
		_set_vertex_array(tileview.ceiling, sf::Vector2f{232, 15},
			sf::Vector2f{239, 8}, sf::Vector2f{295, 8}, sf::Vector2f{295, 15});
		_set_vertex_array(tileview.darkness, sf::Vector2f{216, 152},
			sf::Vector2f{216, 24}, sf::Vector2f{295, 24},
			sf::Vector2f{295, 152});
		_set_vertex_array(tileview.side_darkness, sf::Vector2f{216, 167},
			sf::Vector2f{216, 8}, sf::Vector2f{295, 8}, sf::Vector2f{296, 167});
		_set_vertex_array(tileview.back_wall, sf::Vector2f{216, 153},
			sf::Vector2f{216, 23}, sf::Vector2f{296, 23},
			sf::Vector2f{296, 153});
		_set_vertex_array(tileview.back_door, sf::Vector2f{216, 153},
			sf::Vector2f{216, 23}, sf::Vector2f{296, 23},
			sf::Vector2f{296, 153});

		_set_texture_coordinates(tileview);
	}

	// Tile to the left and one in front
	{
		Coordinate3 tile{-1, 0, -1};
		auto &tileview{tileviews.at(tile)};

		// Set Vertex Arrays
		_set_vertex_array(tileview.floor, sf::Vector2f{16, 143},
			sf::Vector2f{46, 128}, sf::Vector2f{95, 128},
			sf::Vector2f{80, 143});
		_set_vertex_array(tileview.ceiling, sf::Vector2f{46, 47},
			sf::Vector2f{16, 32}, sf::Vector2f{80, 32}, sf::Vector2f{95, 47});
		_set_vertex_array(tileview.darkness, sf::Vector2f{56, 119},
			sf::Vector2f{56, 56}, sf::Vector2f{119, 56},
			sf::Vector2f{120, 119});
		_set_vertex_array(tileview.side_darkness, sf::Vector2f{88, 151},
			sf::Vector2f{88, 24}, sf::Vector2f{119, 55},
			sf::Vector2f{119, 120});

		_set_texture_coordinates(tileview);
	}

	// Tile to the left and two in front
	{
		Coordinate3 tile{-1, 0, -2};
		auto &tileview{tileviews.at(tile)};

		// Set Vertex Arrays
		_set_vertex_array(tileview.darkness, sf::Vector2f{105, 104},
			sf::Vector2f{104, 72}, sf::Vector2f{135, 72},
			sf::Vector2f{136, 103});
		_set_vertex_array(tileview.side_darkness, sf::Vector2f{120, 119},
			sf::Vector2f{120, 56}, sf::Vector2f{135, 71},
			sf::Vector2f{135, 104});

		_set_texture_coordinates(tileview);
	}

	// Tile to the left and three in front
	{
		Coordinate3 tile{-1, 0, -3};
		auto &tileview{tileviews.at(tile)};

		// Set Vertex Arrays
		_set_vertex_array(tileview.darkness, sf::Vector2f{103, 103},
			sf::Vector2f{104, 72}, sf::Vector2f{136, 72},
			sf::Vector2f{136, 103});
		_set_vertex_array(tileview.side_darkness, sf::Vector2f{136, 103},
			sf::Vector2f{136, 72}, sf::Vector2f{143, 79},
			sf::Vector2f{143, 96});

		_set_texture_coordinates(tileview);
	}

	// Tile to the left and four in front
	{
		Coordinate3 tile{-1, 0, -4};
		auto &tileview{tileviews.at(tile)};

		// Set Vertex Arrays
		_set_vertex_array(tileview.darkness, sf::Vector2f{126, 95},
			sf::Vector2f{126, 80}, sf::Vector2f{143, 80},
			sf::Vector2f{144, 95});

		_set_texture_coordinates(tileview);
	}

	// Tile to the right and one in front
	{
		Coordinate3 tile{1, 0, -1};
		auto &tileview{tileviews.at(tile)};

		// Set Vertex Arrays
		_set_vertex_array(tileview.floor, sf::Vector2f{215, 143},
			sf::Vector2f{200, 128}, sf::Vector2f{257, 128},
			sf::Vector2f{287, 143});
		_set_vertex_array(tileview.ceiling, sf::Vector2f{200, 47},
			sf::Vector2f{215, 32}, sf::Vector2f{287, 32},
			sf::Vector2f{257, 47});
		_set_vertex_array(tileview.darkness, sf::Vector2f{184, 119},
			sf::Vector2f{184, 56}, sf::Vector2f{247, 56},
			sf::Vector2f{247, 119});
		_set_vertex_array(tileview.side_darkness, sf::Vector2f{184, 120},
			sf::Vector2f{184, 55}, sf::Vector2f{215, 24},
			sf::Vector2f{215, 151});

		_set_texture_coordinates(tileview);
	}

	// Tile to the right and two in front
	{
		Coordinate3 tile{1, 0, -2};
		auto &tileview{tileviews.at(tile)};

		// Set Vertex Arrays
		_set_vertex_array(tileview.darkness, sf::Vector2f{168, 103},
			sf::Vector2f{168, 72}, sf::Vector2f{199, 72},
			sf::Vector2f{199, 103});
		_set_vertex_array(tileview.side_darkness, sf::Vector2f{168, 104},
			sf::Vector2f{168, 71}, sf::Vector2f{184, 56},
			sf::Vector2f{184, 119});

		_set_texture_coordinates(tileview);
	}

	// Tile to the right and three in front
	{
		Coordinate3 tile{1, 0, -3};
		auto &tileview{tileviews.at(tile)};

		// Set Vertex Arrays
		_set_vertex_array(tileview.darkness, sf::Vector2f{168, 103},
			sf::Vector2f{168, 72}, sf::Vector2f{199, 72},
			sf::Vector2f{199, 103});
		_set_vertex_array(tileview.side_darkness, sf::Vector2f{160, 96},
			sf::Vector2f{160, 79}, sf::Vector2f{167, 73},
			sf::Vector2f{167, 103});

		_set_texture_coordinates(tileview);
	}

	// Tile to the right and four in front
	{
		Coordinate3 tile{1, 0, -4};
		auto &tileview{tileviews.at(tile)};

		// Set Vertex Arrays
		_set_vertex_array(tileview.darkness, sf::Vector2f{160, 95},
			sf::Vector2f{160, 80}, sf::Vector2f{176, 80},
			sf::Vector2f{176, 95});

		_set_texture_coordinates(tileview);
	}
}
