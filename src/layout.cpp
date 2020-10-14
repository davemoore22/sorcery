// Copyright (C) 2020 Dave Moore
//
// This file is part of Sorcery: Dreams of the Mad Overlord.
//
// Sorcery: Dreams of the Mad Overlord is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Sorcery: Dreams of the Mad Overlord is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Sorcery: Dreams of the Mad Overlord.  If not, see <http://www.gnu.org/licenses/>.
//
// If you modify this Program, or any covered work, by linking or combining it
// with the libraries referred to in README (or a modified version of said
// libraries), containing parts covered by the terms of said libraries, the
// licensors of this Program grant you additional permission to convey the
// resulting work.

#include "layout.hpp"

// This is the Game Layout Handling Class

// Standard Constructor
Sorcery::Layout::Layout(const std::filesystem::path filename) {

	// Defaults for now as _load is called on the constructor here and layout is created before window object retrieves
	// the cell height and width from the config file alas! Solution is to pass the system object into this and get both
	// the layout file name, and the config settings from the system object as we can do
	_cell_width = 20;
	_cell_height = 25;

	// Load the layout from file
	_loaded = _load(filename);
	if (_loaded) {
		_last_loaded = std::chrono::file_clock::now();
		_filename = filename;
		_last_modified = std::filesystem::last_write_time(_filename);
	}
}

// Standard Destructor
Sorcery::Layout::~Layout() {
}

// Overload [] Operator
auto Sorcery::Layout::operator [] (const std::string& combined_key) -> Component& {

	// First check if we need to reload if anything has changed!
	if (_refresh_needed())
		_load(_filename);

	// Else return the requested component
	if (_loaded)
		if (_components.find(combined_key) != _components.end())
			return _components.at(combined_key);
		else
			return _components[0];
	else
		return _components[0];
}

auto Sorcery::Layout::set_grid(unsigned int cell_width, unsigned int cell_height) -> void {
	_cell_width = cell_width;
	_cell_height = cell_height;
}

auto Sorcery::Layout::_load(const std::filesystem::path filename) -> bool {
	_components.clear();

	// Attempt to load Layout File
	if (std::ifstream layout_file {filename.string(), std::ifstream::binary}; layout_file.good()) {

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
		Json::Reader reader {};
#pragma GCC diagnostic pop
		Json::StreamWriterBuilder builder {};
		builder.settings_["indentation"] = "";
		Json::Value layout;
		if (reader.parse(layout_file, layout)) {
			Json::Value& screens {layout["screen"]};

			// Iterate through layout file one screen at a time
			for (unsigned int i = 0; i < screens.size(); i++) {

				// Each screen will always have a name and one or more components
				std::string screen_name {screens[i]["name"].asString()};
				Json::Value& components {screens[i]["component"]};

				// For every component on that screen read in their properties
				for (unsigned int j = 0; j < components.size(); j++) {

					// Always Present
					std::string name {components[j]["name"].asString()};

					// Not always present
					int x = [&] {
						if (components[j].isMember("x")) {
							if (components[j]["x"].asString() == "centre")
								return -1;
							else if (components[j]["x"].asString().length() > 0)
								return (std::stoi(components[j]["x"].asString()) * static_cast<int>(_cell_width));
							else
								return 0;
						} else
							return 0;
					}();
					int y = [&] {
						if (components[j].isMember("y")) {
							if (components[j]["y"].asString() == "centre")
								return -1;
							else if (components[j]["y"].asString().length() > 0)
								return (std::stoi(components[j]["y"].asString()) * static_cast<int>(_cell_height));
							else
								return 0;
						} else
							return 0;
					}();
					unsigned int w = [&] {
						if (components[j].isMember("w")) {
							if (components[j]["w"].asString().length() > 0)
								return std::stoi(components[j]["w"].asString());
							else
								return 0;
						} else
							return 0;
					}();
					unsigned int h = [&] {
						if (components[j].isMember("h")) {
							if (components[j]["h"].asString().length() > 0)
								return std::stoi(components[j]["h"].asString());
							else
								return 0;
						} else
							return 0;
					}();
					float scale = [&] {
						if (components[j].isMember("scale")) {
							if (components[j]["scale"].asString().length() > 0)
								return std::stof(components[j]["scale"].asString());
							else
								return 0.0f;
						} else
							return 0.0f;
					}();
					FontType font_type = [&] {
						if (components[j].isMember("font")) {
							if (components[j]["font"].asString().length() > 0) {
								if (components[j]["font"].asString() == "monospace")
									return FontType::MONOSPACE;
								else if (components[j]["font"].asString() == "text")
									return FontType::TEXT;
								else if (components[j]["font"].asString() == "proportional")
									return FontType::PROPORTIONAL;
								else
									return FontType::NONE;
							} else
								return FontType::NONE;
						} else
							return FontType::NONE;
					}();
					unsigned int size = [&] {
						if (components[j].isMember("size")) {
							if (components[j]["size"].asString().length() > 0)
								return std::stoi(components[j]["size"].asString());
							else
								return 0;
						} else
							return 0;
					}();
					unsigned long long colour = [&] {
						if (components[j].isMember("colour")) {
							if (components[j]["colour"].asString().length() > 0)
								return std::stoull(components[j]["colour"].asString(), 0, 16);
							else
								return static_cast<unsigned long long>(0ull);
						} else
							return static_cast<unsigned long long>(0ull);
					}();
					bool animated = [&] {
						if (components[j].isMember("animated")) {
							if (components[j]["animated"].asString().length() > 0)
								return components[j]["animated"].asString() == "true";
							else
								return false;
						} else
							return false;
					}();
					std::string string_key = [&] {
						if (components[j].isMember("string"))
							return components[j]["string"].asString();
						else
							return std::string();
					}();
					unsigned int alpha = [&] {
						if (components[j].isMember("alpha")) {
							if (components[j]["alpha"].asString().length() > 0)
								return std::stoi(components[j]["alpha"].asString());
							else
								return 0;
						} else
							return 0;
					}();
					unsigned int width = [&] {
						if (components[j].isMember("width")) {
							if (components[j]["width"].asString().length() > 0)
								return std::stoi(components[j]["width"].asString());
							else
								return 0;
						} else
							return 0;
					}();
					unsigned long long background = [&] {
						if (components[j].isMember("background")) {
							if (components[j]["background"].asString().length() > 0)
								return std::stoull(components[j]["background"].asString(), 0, 16);
							else
								return static_cast<unsigned long long>(0ull);
						} else
							return static_cast<unsigned long long>(0ull);
					}();

					// Add the Component
					std::string key = screen_name + ":" + name;
					Component component(screen_name, name, x, y, w, h, scale, font_type, size, colour, animated,
						string_key, alpha, width, background);
					_components[key] = component;
				}
			 }
		} else
			return false;
	} else
		return false;

	return true;
}

auto Sorcery::Layout::_refresh_needed() -> bool {
	_last_modified = std::filesystem::last_write_time(_filename);
	return _last_modified > _last_loaded;
}
