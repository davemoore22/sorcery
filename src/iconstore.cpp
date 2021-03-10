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

#include "iconstore.hpp"

// Standard Constructor
Sorcery::IconStore::IconStore(
	System &system, Display &display, const std::filesystem::path filename)
	: _system{system}, _display{display} {

	// First get the Icon Texture
	_texture = _system.resources->textures[ICONS_TEXTURE];

	// Set the scaling size (note we are using square icons here!) before loading! TODO: get from
	// component
	_size = sf::Vector2f{128, 128};
	float texture_size{static_cast<float>(_texture.getSize().y)};
	_scale = sf::Vector2f(_size.x / texture_size, _size.y / texture_size);

	_loaded = _set_icons();

	_loaded = _load(filename);
}

// Overload [] Operator
auto Sorcery::IconStore::operator[](const std::string &key) -> std::optional<sf::Sprite> {

	if (_loaded)
		return _icons.at(key);
	else
		return std::nullopt;
}

auto Sorcery::IconStore::get(const std::string &key) -> std::optional<sf::Sprite> {

	if (_loaded)
		return _icons.at(key);
	else
		return std::nullopt;
}

auto Sorcery::IconStore::_load(const std::filesystem::path filename) -> bool {

	// Attempt to load Layout File
	if (std::ifstream file{filename.string(), std::ifstream::binary}; file.good()) {

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
		Json::Reader reader{};
#pragma GCC diagnostic pop
		Json::StreamWriterBuilder builder{};
		builder.settings_["indentation"] = "";
		if (Json::Value layout; reader.parse(file, layout)) {
			Json::Value &icons{layout["icon"]};

			// Iterate through icon file one icon at a time
			for (unsigned int i = 0; i < icons.size(); i++) {

				// Get the mappings for each icon (note that all parameters should always be present
				// for each icon entry in the mapping file or else this will error)
				unsigned int index{std::stoul(icons[i]["index"].asString())};
				std::string filename{icons[i]["filename"].asString()};
				std::string menu_item_s{icons[i]["menu_item"].asString()};
				std::string key{icons[i]["key"].asString()};
				std::string colour_hex{icons[i]["colour"].asString()};
				if (colour_hex.length() == 0)
					colour_hex = "0xFFFFFFFF"; // TODO: read from icon component!
				sf::Color colour(std::stoul(colour_hex, 0, 16));
				MenuItem menu_item{MenuItem::NONE};

				// Use Magic Enum Library Reflection to convert the string to the type!
				auto item_t = magic_enum::enum_cast<Sorcery::Enums::Menu::Item>(menu_item_s);
				if (item_t.has_value())
					menu_item = item_t.value();

				// Add the Mapping
				const Icon icon{index, menu_item, key, filename, colour};
				_icon_mapping[menu_item] = icon;
			}
		}
	} else
		return false;

	return true;
}

auto Sorcery::IconStore::_set_icons() -> bool {

	auto color = magic_enum::enum_cast<Sorcery::Enums::Menu::Item>("CR_HUMAN");
	if (color.has_value()) {
		MenuItem wibble = color.value();
		if (wibble == MenuItem::CR_HUMAN) {
			std::cout << "yay!" << std::endl;
		}
	}

	// Icon List load here
	_index = 0;
	_load_icon("samurai");
	_load_icon("fighter");
	_load_icon("lord");
	_load_icon("thief");
	_load_icon("ninja");
	_load_icon("priest");
	_load_icon("bishop");
	_load_icon("mage");
	_load_icon("good");
	_load_icon("neutral");
	_load_icon("evil");
	_load_icon("strength");
	_load_icon("iq");
	_load_icon("piety");
	_load_icon("vitality");
	_load_icon("agility");
	_load_icon("luck");
	_load_icon("human");
	_load_icon("elf");
	_load_icon("dwarf");
	_load_icon("gnome");
	_load_icon("hobbit");

	return true;
}

// Fortunately we are using square icons!
auto Sorcery::IconStore::_get_rect(unsigned int index) const -> sf::IntRect {

	return sf::IntRect(_texture.getSize().y * index, 0, _texture.getSize().y, _texture.getSize().y);
}

auto Sorcery::IconStore::_load_icon(std::string key) -> void {

	sf::Sprite sprite(_texture);
	sprite.setTextureRect(_get_rect(_index));
	sprite.setScale(_scale);
	_icons[key] = sprite;
	++_index;
}
