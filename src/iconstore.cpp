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
	System *system, Component layout, const std::filesystem::path filename)
	: _system{system}, _layout{layout} {

	// Prepare the icon stores
	_icon_store.clear();
	_menu_icon_map.clear();

	// First get the Icon Texture and load it into the Spritesheet
	_texture = _system->resources->textures[GraphicsTexture::ICONS];

	// Set the Icon scaling (remember we are using square icons)
	_size = sf::Vector2f(_layout.size, _layout.size);
	constexpr float texture_size{511};
	_scale = sf::Vector2f(_size.x / texture_size, _size.y / texture_size);

	// Load the Icons
	_loaded = _load(filename);
}

// Overload [] Operator
auto Sorcery::IconStore::operator[](const std::string &key) -> std::optional<sf::Sprite> {

	auto sprite{get(key)};
	auto copy{sprite};
	return copy;
}

auto Sorcery::IconStore::operator[](const MenuItem key) -> std::optional<sf::Sprite> {

	auto sprite{get(key)};
	auto copy{sprite};
	return copy;
}

// Find the corresponding item in the map by String
auto Sorcery::IconStore::get(const std::string &key) -> std::optional<sf::Sprite> {

	if (_loaded)
		return _icon_store.at(key);
	else
		return std::nullopt;
}

// Find the corresponding item in the map by Menu Item
auto Sorcery::IconStore::get(const MenuItem key) -> std::optional<sf::Sprite> {

	if (_loaded) {
		auto it =
			std::find_if(_menu_icon_map.begin(), _menu_icon_map.end(), [&key](const auto &item) {
				return item.second.item == key;
			});

		if (it != _menu_icon_map.end())
			return _icon_store.at((*it).second.key);
		else
			return std::nullopt;
	} else
		return std::nullopt;
}

// Attempt to load Icons from Layout File
auto Sorcery::IconStore::_load(const std::filesystem::path filename) -> bool {

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
				unsigned int index{
					static_cast<unsigned int>(std::stoul(icons[i]["index"].asString()))};
				std::string filename{icons[i]["filename"].asString()};
				std::string menu_item_s{icons[i]["menu_item"].asString()};
				std::string key{icons[i]["key"].asString()};
				std::string colour_hex{icons[i]["colour"].asString()};
				sf::Color colour{[&] {
					if (colour_hex.length() == 0)
						return sf::Color(_layout.colour);
					else
						return sf::Color(std::stoul(colour_hex, 0, 16));
				}()};
				MenuItem menu_item{MenuItem::NONE};

				// Use Magic Enum Library Reflection to convert the string to the type if we have an
				// associated menu item for the icon (which is used in an info panel beneath a menu)
				auto item_t{magic_enum::enum_cast<Sorcery::Enums::Menu::Item>(menu_item_s)};
				if (item_t.has_value())
					menu_item = item_t.value();

				// Add the Mapping for Icons
				const Icon icon{index, menu_item, key, filename, colour};

				// First for Menu Items
				if (menu_item != MenuItem::NONE) {
					_menu_icon_map[menu_item] = icon;
				}

				// Now work out the Corresponding Sprite in all cases and add it
				sf::IntRect sprite_r{_get_rect(icon.index)};
				sf::Sprite sprite(_texture);
				sprite.setTextureRect(sprite_r);
				sprite.setScale(_scale);
				sprite.setColor(colour);
				_icon_store[key] = sprite;
			}
		}
	} else
		return false;

	return true;
}

// Fortunately we are using square icons so we can easily find the appropriate icon from the
// spritesheet - note that we have 15 icons per row, and each icon is 511x511 pixels
auto Sorcery::IconStore::_get_rect(unsigned int index) const -> sf::IntRect {

	constexpr int icon_size{511};
	return sf::IntRect(icon_size * (index % 15), icon_size * (index / 15), icon_size, icon_size);
}