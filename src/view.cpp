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
Sorcery::View::View(const std::filesystem::path filename) {

	// Defaults for now as _load is called on the constructor here and layout is
	// created before window object retrieves the cell height and width from the
	// config file alas! Solution is to pass the system object into this and get
	// both the layout file name, and the config settings from the system object
	// as we can do

	// Load the layout from file
	_loaded = _load(filename);
	if (_loaded)
		_filename = filename;
}

// Overload [] Operator
auto Sorcery::View::operator[](Point3) -> ViewNode & {
	try {
	} catch (std::exception &e) {
		Error error{
			SystemError::JSON_PARSE_ERROR, e, "view.json is not valid JSON!"};
		std::cout << error;
		exit(EXIT_FAILURE);
	}
}
auto Sorcery::View::operator[](int z) -> std::vector<ViewNode *> {
	try {
	} catch (std::exception &e) {
		Error error{
			SystemError::JSON_PARSE_ERROR, e, "view.json is not valid JSON!"};
		std::cout << error;
		exit(EXIT_FAILURE);
	}
}

auto Sorcery::View::_load(const std::filesystem::path filename) -> bool {

	_nodes.clear();
	if (std::ifstream file{filename.string(), std::ifstream::binary};
		file.good()) {

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
		Json::Reader reader{};
#pragma GCC diagnostic pop
		Json::StreamWriterBuilder builder{};
		builder.settings_["indentation"] = "";
		if (Json::Value layout; reader.parse(file, layout)) {
			Json::Value &layers{layout["layers"]};

			// Iterate through view file one layer at a time
			for (auto i = 0u; i < layers.size(); i++) {

				auto layer_type{layers[i]["type"].asString()};
				Json::Value &tiles{layers[i]["tiles"]};

				// For every component on that screen read in their
				// properties
				for (auto j = 0u; j < tiles.size(); j++) {

					// Type
					// Flipped
					// Tile
					// Screen
					// coords/fullwidth
					std::cout << layer_type << " " << tiles << std::endl;

					/* // Add the Component
					const auto key{fmt::format("{}:{}", screen_name, name)};
					Component component(screen_name, name, x, y, w, h, scale,
						font_type, size, colour, animated, string_key, alpha,
						width, background, justification, component_type,
						priority, drawmode, texture);

					// Now look for any extra data
					if (components[j].isMember("data")) {
						auto &extra_data{components[j]["data"][0]};
						auto data_keys{extra_data.getMemberNames()};
						for (auto &data_key : data_keys) {
							auto data_value{extra_data[data_key].asString()};
							component.set(data_key, data_value);
						}
					} */

					// _components[key] = component;
				}
			}
		} else
			return false;

	} else
		return false;

	return true;
}