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

#include "resources/componentstore.hpp"
#include "common/enum.hpp"
#include "types/error.hpp"

// This is the Game Layout Handling Class

// Standard Constructor
Sorcery::ComponentStore::ComponentStore(const std::filesystem::path filename) {

	// Defaults for now as _load is called on the constructor here and layout is created before window object retrieves
	// the cell height and width from the config file alas! Solution is to pass the system object into this and get both
	// the layout file name, and the config settings from the system object as we can do
	_cell_width = 24;
	_cell_height = 24; // TODO: argh. remember to change this

	// Load the layout from file
	try {

		_loaded = _load(filename);
		if (_loaded) {
			_last_loaded = std::chrono::file_clock::now();
			_filename = filename;
			_last_modified = std::filesystem::last_write_time(_filename);
		}

	} catch (std::exception &e) {
		Error error{SYE::JSON_PARSE_ERROR, e, "layout.json is not valid JSON!"};
		std::cout << error;
		exit(EXIT_FAILURE);
	}
}

// Overload [] Operator
auto Sorcery::ComponentStore::operator[](std::string_view combined_key) -> Component & {

	try {

		// Else return the requested component
		if (_loaded)
			if (_components.contains(std::string{combined_key}))
				return _components.at(std::string{combined_key});
			else
				return _components[0];
		else
			return _components[0];

	} catch (std::exception &e) {
		Error error{
			SYE::UNKNOWN_COMPONENT, e, fmt::format("Unable to find Component '{}' in layout.json!", combined_key)};
		std::cout << error;
		exit(EXIT_FAILURE);
	}
}

auto Sorcery::ComponentStore::refresh_if_needed() -> bool {

	auto needed{_refresh_needed()};
	if (needed) {
		_loaded = _load(_filename);
		if (_loaded) {
			_last_loaded = std::chrono::file_clock::now();
			_last_modified = std::filesystem::last_write_time(_filename);
		}
	}

	return needed;
}

// Overload () Operator
auto Sorcery::ComponentStore::operator()(std::string_view screen) -> std::optional<std::vector<Component>> {

	// Else return the requested components for the screen
	std::vector<Component> results;
	if (_loaded) {

		for (const auto &[unique_key, component] : _components) {
			if (component.screen == screen && component.drawmode == WDM::AUTOMATIC) {
				results.push_back(component);
			}
		}

		// Sort by priority
		std::sort(results.begin(), results.end(),
			[](const auto &first, const auto &second) { return first.priority < second.priority; });

		return results;
	}

	return std::nullopt;
}

auto Sorcery::ComponentStore::get_error() -> Component & {

	return _components.at("global:error");
}

auto Sorcery::ComponentStore::set_grid(unsigned int cell_width, unsigned int cell_height) -> void {

	_cell_width = cell_width;
	_cell_height = cell_height;
}

auto Sorcery::ComponentStore::_load(const std::filesystem::path filename) -> bool {

	_components.clear();

	// Attempt to load Layout File
	if (std::ifstream file{filename.string(), std::ifstream::binary}; file.good()) {

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
		Json::Reader reader{};
#pragma GCC diagnostic pop
		Json::StreamWriterBuilder builder{};
		builder.settings_["indentation"] = "";
		if (Json::Value layout; reader.parse(file, layout)) {
			Json::Value &screens{layout["screen"]};

			// Iterate through layout file one screen at a time
			for (auto i = 0u; i < screens.size(); i++) {

				// Each screen will always have a name and one or more
				// components
				auto screen_name{screens[i]["name"].asString()};
				Json::Value &components{screens[i]["component"]};

				// For every component on that screen read in their properties
				for (auto j = 0u; j < components.size(); j++) {

					// Always Present
					auto name{components[j]["name"].asString()};

					// Not always present
					auto x{std::invoke([&] {
						if (components[j].isMember("x")) {
							if (components[j]["x"].asString() == "centre")
								return -1;
							else if (components[j]["x"].asString().length() > 0)
								return (std::stoi(components[j]["x"].asString()) * static_cast<int>(_cell_width));
							else
								return 0;
						} else
							return 0;
					})};
					auto y{std::invoke([&] {
						if (components[j].isMember("y")) {
							if (components[j]["y"].asString() == "centre")
								return -1;
							else if (components[j]["y"].asString().length() > 0)
								return (std::stoi(components[j]["y"].asString()) * static_cast<int>(_cell_height));
							else
								return 0;
						} else
							return 0;
					})};
					auto w{std::invoke([&] {
						if (components[j].isMember("w")) {
							if (components[j]["w"].asString().length() > 0)

								return static_cast<unsigned int>(std::stoi(components[j]["w"].asString()));
							else
								return 0u;
						} else
							return 0u;
					})};
					auto h{std::invoke([&] {
						if (components[j].isMember("h")) {
							if (components[j]["h"].asString().length() > 0)
								return static_cast<unsigned int>(std::stoi(components[j]["h"].asString()));
							else
								return 0u;
						} else
							return 0u;
					})};
					auto scale{std::invoke([&] {
						if (components[j].isMember("scale")) {
							if (components[j]["scale"].asString().length() > 0)
								return std::stof(components[j]["scale"].asString());
							else
								return 0.0f;
						} else
							return 0.0f;
					})};
					FTT font_type{std::invoke([&] {
						if (components[j].isMember("font")) {
							if (components[j]["font"].asString().length() > 0) {
								if (components[j]["font"].asString() == "monospace")
									return FTT::MONOSPACE;
								else if (components[j]["font"].asString() == "text")
									return FTT::TEXT;
								else if (components[j]["font"].asString() == "proportional")
									return FTT::PROPORTIONAL;
								else if (components[j]["font"].asString() == "input")
									return FTT::INPUT;
								else
									return FTT::NO_FONT;
							} else
								return FTT::NO_FONT;
						} else
							return FTT::NO_FONT;
					})};
					auto size{std::invoke([&] {
						if (components[j].isMember("size")) {
							if (components[j]["size"].asString().length() > 0)
								return static_cast<unsigned int>(std::stoi(components[j]["size"].asString()));
							else
								return 0u;
						} else
							return 0u;
					})};
					auto colour{std::invoke([&] {
						if (components[j].isMember("colour")) {
							if (components[j]["colour"].asString().length() > 0)
								return std::stoull(components[j]["colour"].asString(), 0, 16);
							else
								return static_cast<unsigned long long>(0ULL);
						} else
							return static_cast<unsigned long long>(0ULL);
					})};
					auto animated{std::invoke([&] {
						if (components[j].isMember("animated")) {
							if (components[j]["animated"].asString().length() > 0)
								return components[j]["animated"].asString() == "true";
							else
								return false;
						} else
							return false;
					})};
					auto string_key{std::invoke([&] {
						if (components[j].isMember("string"))
							return components[j]["string"].asString();
						else
							return std::string{};
					})};
					auto alpha{std::invoke([&] {
						if (components[j].isMember("alpha")) {
							if (components[j]["alpha"].asString().length() > 0)
								return static_cast<unsigned int>(std::stoi(components[j]["alpha"].asString()));
							else
								return 0u;
						} else
							return 0u;
					})};
					auto width{std::invoke([&] {
						if (components[j].isMember("width")) {
							if (components[j]["width"].asString().length() > 0)
								return static_cast<unsigned int>(std::stoi(components[j]["width"].asString()));
							else
								return 0u;
						} else
							return 0u;
					})};
					auto background{std::invoke([&] {
						if (components[j].isMember("background")) {
							if (components[j]["background"].asString().length() > 0)
								return std::stoull(components[j]["background"].asString(), 0, 16);
							else
								return static_cast<unsigned long long>(0ULL);
						} else
							return static_cast<unsigned long long>(0ULL);
					})};
					JUS justification{std::invoke([&] {
						if (components[j].isMember("justification")) {
							if (components[j]["justification"].asString().length() > 0) {
								if (components[j]["justification"].asString() == "left")
									return JUS::LEFT;
								else if (components[j]["justification"].asString() == "centre")
									return JUS::CENTRE;
								else if (components[j]["justification"].asString() == "right")
									return JUS::RIGHT;
								else
									return JUS::LEFT;
							} else
								return JUS::LEFT;
						} else
							return JUS::LEFT;
					})};
					CPT component_type{std::invoke([&] {
						if (components[j].isMember("type")) {
							if (components[j]["type"].asString().length() > 0) {
								if (components[j]["type"].asString() == "frame")
									return CPT::FRAME;
								else if (components[j]["type"].asString() == "text")
									return CPT::TEXT;
								else if (components[j]["type"].asString() == "image")
									return CPT::IMAGE;
								else if (components[j]["type"].asString() == "icon")
									return CPT::ICON;
								else if (components[j]["type"].asString() == "menu")
									return CPT::MENU;
								else if (components[j]["type"].asString() == "dialog")
									return CPT::DIALOG;
								else
									return CPT::UNKNOWN;
							} else
								return CPT::UNKNOWN;
						} else
							return CPT::UNKNOWN;
					})};
					auto priority{std::invoke([&] {
						if (components[j].isMember("priority")) {
							if (components[j]["priority"].asString().length() > 0)
								return static_cast<unsigned int>(std::stoi(components[j]["priority"].asString()));
							else
								return 999u;
						} else
							return 999u;
					})};
					WDM drawmode{std::invoke([&] {
						if (components[j].isMember("drawmode")) {
							if (components[j]["drawmode"].asString().length() > 0) {
								if (components[j]["drawmode"].asString() == "manual")
									return WDM::MANUAL;
								else if (components[j]["drawmode"].asString() == "automatic")
									return WDM::AUTOMATIC;
								else
									return WDM::AUTOMATIC;
							} else
								return WDM::AUTOMATIC;
						} else
							return WDM::AUTOMATIC;
					})};
					GTX texture{std::invoke([&] {
						if (components[j].isMember("texture")) {
							if (components[j]["texture"].asString().length() > 0) {
								if (components[j]["texture"].asString() == "background")
									return GTX::BACKGROUND;
								else if (components[j]["texture"].asString() == "banner")
									return GTX::BANNER;
								else if (components[j]["texture"].asString() == "creature")
									return GTX::CREATURES_KNOWN;
								else if (components[j]["texture"].asString() == "logo")
									return GTX::LOGO;
								else if (components[j]["texture"].asString() == "splash")
									return GTX::SPLASH;
								else if (components[j]["texture"].asString() == "town")
									return GTX::TOWN;
								else if (components[j]["texture"].asString() == "ui")
									return GTX::UI;
								else
									return GTX::NO_TEXTURE;
							} else
								return GTX::NO_TEXTURE;
						} else
							return GTX::NO_TEXTURE;
					})};

					// Add the Component
					const auto key{fmt::format("{}:{}", screen_name, name)};
					Component component{screen_name, name, x, y, w, h, scale, font_type, size, colour, animated,
						string_key, alpha, width, background, justification, component_type, priority, drawmode,
						texture};

					// Now look for any extra data
					if (components[j].isMember("data")) {
						auto &extra_data{components[j]["data"][0]};
						auto data_keys{extra_data.getMemberNames()};
						for (auto &data_key : data_keys) {
							auto data_value{extra_data[data_key].asString()};
							component.set(data_key, data_value);
						}
					}

					_components[key] = component;
				}
			}
		} else
			return false;
	} else
		return false;

	return true;
}

auto Sorcery::ComponentStore::_refresh_needed() -> bool {

	_last_modified = std::filesystem::last_write_time(_filename);
	return _last_modified > _last_loaded;
}
