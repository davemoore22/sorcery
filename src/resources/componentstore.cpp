// Copyright (C) 2025 Dave Moore
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

#include "resources/componentstore.hpp"
#include "common/macro.hpp"
#include "core/system.hpp"
#include "types/component.hpp"

// Standard Constructor
Sorcery::ComponentStore::ComponentStore(const std::filesystem::path filename) {

	_grid_w = 16;
	_grid_h = 16;

	_loaded = load(filename);
	if (_loaded) {
		_last_load = std::chrono::file_clock::now();
		_file = filename;
		_last_mod = std::filesystem::last_write_time(_file);
	}
}

auto Sorcery::ComponentStore::get(std::string_view combined_key)
	-> Component & {

	// swap into _load instead and check if refresh needed is called unnecessary
	try {

		// First check if we need to reload if anything has changed!
		if (need_refresh())
			load(_file);

	} catch (std::exception &e) {

		Error error{Enums::System::Error::JSON_PARSE_ERROR, e,
					"layout.json is not valid JSON!"};
		std::cerr << error;
		exit(EXIT_FAILURE);
	}

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
		Error error{Enums::System::Error::UNKNOWN_COMPONENT, e,
					fmt::format("Unable to find Component '{}' in layout.json!",
								combined_key)};
		std::cerr << error;
		exit(EXIT_FAILURE);
	}
}

// Overload [] Operator
auto Sorcery::ComponentStore::operator[](std::string_view combined_key)
	-> Component & {

	return get(combined_key);
}

// Overload () Operator
auto Sorcery::ComponentStore::operator()(std::string_view screen)
	-> std::optional<std::vector<Component>> {

	// First check if we need to reload if anything has changed!
	if (need_refresh())
		load(_file);

	// Else return the requested components for the screen
	std::vector<Component> results;
	if (_loaded) {

		for (const auto &[unique_key, component] : _components) {
			if ((component.form == screen) &&
				(component.drawmode == Enums::Layout::DrawMode::AUTOMATIC)) {
				results.push_back(component);
			}
		}

		// Sort by priority
		std::sort(results.begin(), results.end(),
				  [](const auto &first, const auto &second) {
					  return first.priority < second.priority;
				  });

		return results;
	}

	return std::nullopt;
}

auto Sorcery::ComponentStore::get_custom(std::string_view screen)
	-> std::optional<std::vector<Component>> {

	// First check if we need to reload if anything has changed!
	if (need_refresh())
		load(_file);

	// Else return the requested components for the screen
	std::vector<Component> results;
	if (_loaded) {

		for (const auto &[unique_key, component] : _components) {
			if ((component.form == screen) &&
				(component.drawmode == Enums::Layout::DrawMode::MANUAL)) {
				results.push_back(component);
			}
		}

		// Sort by priority
		std::sort(results.begin(), results.end(),
				  [](const auto &first, const auto &second) {
					  return first.priority < second.priority;
				  });

		return results;
	}

	return std::nullopt;
}

auto Sorcery::ComponentStore::load(const std::filesystem::path filename)
	-> bool {

	_components.clear();

	// Attempt to load Layout File
	if (std::ifstream file{filename.string(), std::ifstream::binary};
		file.good()) {

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
		Json::Reader reader{};
#pragma GCC diagnostic pop
		Json::StreamWriterBuilder builder{};
		builder.settings_["indentation"] = "";
		if (Json::Value layout; reader.parse(file, layout)) {
			Json::Value &forms{layout["form"]};

			// Iterate through layout file one screen at a time
			for (auto i = 0u; i < forms.size(); i++) {

				// Each form will always have a name and one or more
				// components
				std::string form_name{forms[i]["name"].asString()};
				Json::Value &components{forms[i]["component"]};

				// For every component on that screen read in their properties
				for (auto j = 0u; j < components.size(); j++) {

					using Enums::Layout::ComponentType;

					// Always Present
					std::string name{components[j]["name"].asString()};
					ComponentType component_type{[&] {
						if (components[j].isMember("type")) {
							if (components[j]["type"].asString().length() > 0) {
								if (components[j]["type"].asString() == "text")
									return ComponentType::TEXT;
								else if (components[j]["type"].asString() ==
										 "frame")
									return ComponentType::FRAME;
								else if (components[j]["type"].asString() ==
										 "button")
									return ComponentType::BUTTON;
								else if (components[j]["type"].asString() ==
										 "image_fg")
									return ComponentType::IMAGE_FG;
								else if (components[j]["type"].asString() ==
										 "image_bg")
									return ComponentType::IMAGE_BG;
								else if (components[j]["type"].asString() ==
										 "menu")
									return ComponentType::MENU;
								else if (components[j]["type"].asString() ==
										 "text")
									return ComponentType::TEXT;
								else if (components[j]["type"].asString() ==
										 "paragraph")
									return ComponentType::PARAGRAPH;
								else if (components[j]["type"].asString() ==
										 "other")
									return ComponentType::OTHER;
								else
									return ComponentType::NO_CT;
							} else
								return ComponentType::NO_CT;
						} else
							return ComponentType::NO_CT;
					}()};

					// Not always present
					auto x{[&] {
						if (components[j].isMember("x")) {
							if (components[j]["x"].asString() == "centre")
								return -1;
							else if (components[j]["x"].asString().length() > 0)
								return (
									std::stoi(components[j]["x"].asString()));
							else
								return 0;
						} else
							return 0;
					}()};
					auto y{[&] {
						if (components[j].isMember("y")) {
							if (components[j]["y"].asString() == "centre")
								return -1;
							else if (components[j]["y"].asString().length() > 0)
								return (
									std::stoi(components[j]["y"].asString()));
							else
								return 0;
						} else
							return 0;
					}()};
					auto w{[&] {
						if (components[j].isMember("w")) {
							if (components[j]["w"].asString().length() > 0) {
								return static_cast<unsigned int>(
									std::stoi(components[j]["w"].asString()));
							} else
								return 0u;
						} else
							return 0u;
					}()};
					auto h{[&] {
						if (components[j].isMember("h")) {
							if (components[j]["w"].asString().length() > 0) {
								return static_cast<unsigned int>(
									std::stoi(components[j]["h"].asString()));
							} else
								return 0u;
						} else
							return 0u;
					}()};
					auto font{[&] {
						using Enums::Layout::Font;

						if (components[j].isMember("font")) {
							if (components[j]["font"].asString().length() > 0) {
								if (components[j]["font"].asString() ==
									"monospace")
									return Font::MONOSPACE;
								else if (components[j]["font"].asString() ==
										 "proportional")
									return Font::PROPORTIONAL;
								else if (components[j]["font"].asString() ==
										 "text")
									return Font::TEXT;
								else if (components[j]["font"].asString() ==
										 "default")
									return Font::DEFAULT;
								else
									return Font::NO_FONT;
							} else
								return Font::NO_FONT;
						} else
							return Font::NO_FONT;
					}()};
					auto colour{[&] {
						if (components[j].isMember("colour")) {
							if (components[j]["colour"].asString().length() > 0)
								return COL2NUM(
									components[j]["colour"].asString());
							else
								return COL2NUM("0");
						} else
							return COL2NUM("0");
					}()};
					auto animated{[&] {
						if (components[j].isMember("animated")) {
							if (components[j]["animated"].asString().length() >
								0)
								return components[j]["animated"].asString() ==
									   "true";
							else
								return false;
						} else
							return false;
					}()};
					auto string_key{[&] {
						if (components[j].isMember("string"))
							return components[j]["string"].asString();
						else
							return std::string{};
					}()};
					auto alpha{[&] {
						if (components[j].isMember("alpha")) {
							if (components[j]["alpha"].asString().length() > 0)
								return std::stof(
									components[j]["alpha"].asString());
							else
								return 0.0f;
						} else
							return 0.0f;
					}()};
					auto background{[&] {
						if (components[j].isMember("background")) {
							if (components[j]["background"]
									.asString()
									.length() > 0)
								return COL2NUM(
									components[j]["background"].asString());
							else
								return COL2NUM("0");
						} else
							return COL2NUM("0");
					}()};
					Enums::Layout::Justification justification{[&] {
						using Enums::Layout::Justification;

						if (components[j].isMember("justification")) {
							if (components[j]["justification"]
									.asString()
									.length() > 0) {
								if (components[j]["justification"].asString() ==
									"left")
									return Justification::LEFT;
								else if (components[j]["justificati"
													   "on"]
											 .asString() == "centre")
									return Justification::CENTRE;
								else if (components[j]["justificati"
													   "on"]
											 .asString() == "right")
									return Justification::RIGHT;
								else
									return Justification::LEFT;
							} else
								return Justification::LEFT;
						} else
							return Justification::LEFT;
					}()};
					auto priority{[&] {
						if (components[j].isMember("priority")) {
							if (components[j]["priority"].asString().length() >
								0)
								return static_cast<unsigned int>(std::stoi(
									components[j]["priority"].asString()));
							else
								return 999u;
						} else
							return 999u;
					}()};
					Enums::Layout::DrawMode drawmode{[&] {
						using Enums::Layout::DrawMode;

						if (components[j].isMember("drawmode")) {
							if (components[j]["drawmode"].asString().length() >
								0) {
								if (components[j]["drawmode"].asString() ==
									"manual")
									return DrawMode::MANUAL;
								else if (components[j]["drawmode"].asString() ==
										 "automatic")
									return DrawMode::AUTOMATIC;
								else
									return DrawMode::AUTOMATIC;
							} else
								return DrawMode::AUTOMATIC;
						} else
							return DrawMode::AUTOMATIC;
					}()};

					// Add the Component
					const auto key{fmt::format("{}:{}", form_name, name)};
					Component component{form_name,
										name,
										x,
										y,
										w,
										h,
										font,
										colour,
										animated,
										string_key,
										alpha,
										background,
										justification,
										component_type,
										priority,
										drawmode};

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

auto Sorcery::ComponentStore::need_refresh() -> bool {

	_last_mod = std::filesystem::last_write_time(_file);
	return _last_mod > _last_load;
}
