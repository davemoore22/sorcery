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

#include "resources/iconstore.hpp"
#include "common/enum.hpp"
#include "core/system.hpp"
#include "resources/resourcemanager.hpp"

// Standard Constructor
Sorcery::IconStore::IconStore(System *system, Component layout, const std::filesystem::path filename)
	: _system{system}, _layout{layout} {

	using enum Enums::Map::Event;

	// Prepare the icon stores
	_icon_store.clear();
	_menu_icon_map.clear();
	_event_icon_mapping.clear();

	// First get the Icon Texture and load it into the Spritesheet
	_texture = _system->resources->textures[GraphicsTexture::ICONS];

	// Set the Icon scaling (remember we are using square icons)
	_size = sf::Vector2f(_layout.size, _layout.size);
	constexpr auto texture_size{511.f};
	_scale = sf::Vector2f(_size.x / texture_size, _size.y / texture_size);

	// Load the Icons
	_loaded = _load(filename);

	// Set the Event->Icon Mapping
	_event_icon_mapping[AREA_OF_OUT_BOUNDS] = "event-sign";
	_event_icon_mapping[MAN_TELEPORT_CASTLE] = "event-man-teleport";
	_event_icon_mapping[SILVER_KEY] = "event-monstrous-statue";
	_event_icon_mapping[BRONZE_KEY] = "event-monstrous-statue";
	_event_icon_mapping[MURPHYS_GHOSTS] = "event-humanoid-statue";
	_event_icon_mapping[BEAR_STATUE] = "event-monstrous-statue";
	_event_icon_mapping[FROG_STATUE] = "event-monstrous-statue";
	_event_icon_mapping[GOLD_KEY] = "event-monstrous-statue";
	_event_icon_mapping[NEED_SILVER_KEY] = "event-fog";
	_event_icon_mapping[NEED_BRONZE_KEY] = "event-fog";
	_event_icon_mapping[NEED_BEAR_STATUE] = "event-door";
	_event_icon_mapping[NEED_FROG_STATUE] = "event-door";
	_event_icon_mapping[PLACARD_PIT_1] = "event-sign";
	_event_icon_mapping[PLACARD_PIT_2] = "event-sign";
	_event_icon_mapping[PLACARD_PIT_3] = "event-sign";
	_event_icon_mapping[TURN_AROUND] = "event-sign";
	_event_icon_mapping[TURN_LEFT] = "event-sign";
	_event_icon_mapping[TURN_RIGHT] = "event-sign";
	_event_icon_mapping[NEED_BEAR_STATUE_2] = "event-wall";
	_event_icon_mapping[TESTING_GROUNDS] = "event-sign";
	_event_icon_mapping[ALARM_BELLS] = "event-alarm";
	_event_icon_mapping[TREASURE_REPOSITORY] = "event-sign";
	_event_icon_mapping[MONSTER_ALLOCATION_CENTRE] = "event-sign";
	_event_icon_mapping[LARGE_DESK] = "event-voice";
	_event_icon_mapping[TREBOR_VOICE] = "event-voice";
	_event_icon_mapping[SERVICE_ELEVATOR] = "event-sign";
	_event_icon_mapping[WERDNA_BOAST] = "event-sign";
	_event_icon_mapping[TURN_BACK] = "event-voice";
	_event_icon_mapping[WERDNA_SIGN] = "event-sign";
	_event_icon_mapping[THREE_HUMANOIDS] = "party";
	_event_icon_mapping[GETTING_WARM_1] = "event-warm";
	_event_icon_mapping[GETTING_WARM_2] = "event-warm";
	_event_icon_mapping[GETTING_WARM_3] = "event-warm";
	_event_icon_mapping[NEED_BLUE_RIBBON] = "event-wall";
}

// Overload [] Operator
auto Sorcery::IconStore::operator[](std::string_view key) -> std::optional<sf::Sprite> {

	auto sprite{get(key)};
	auto copy{sprite};
	return copy;
}

auto Sorcery::IconStore::operator[](const MIM key) -> std::optional<sf::Sprite> {

	auto sprite{get(key)};
	auto copy{sprite};
	return copy;
}

// Find the corresponding item in the map by String
auto Sorcery::IconStore::get(std::string_view key) -> std::optional<sf::Sprite> {

	if (_loaded)
		return _icon_store.at(std::string{key});
	else
		return std::nullopt;
}

// Find the corresponding item in the map by Menu Item
auto Sorcery::IconStore::get(const MIM key) -> std::optional<sf::Sprite> {

	if (_loaded) {
		auto it = std::find_if(
			_menu_icon_map.begin(), _menu_icon_map.end(), [&key](const auto &item) { return item.second.item == key; });

		if (it != _menu_icon_map.end())
			return _icon_store.at((*it).second.key);
		else
			return std::nullopt;
	} else
		return std::nullopt;
}

auto Sorcery::IconStore::get_event_icon(MAV type) -> std::optional<sf::Sprite> {

	if (_event_icon_mapping.find(type) != _event_icon_mapping.end()) {
		auto sprite{get(_event_icon_mapping.at(type))};
		auto copy{sprite};
		return copy;
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
			for (auto i = 0u; i < icons.size(); i++) {

				// Get the mappings for each icon (note that all parameters should always be present for each icon entry
				// in the mapping file or else this will error)
				auto index{static_cast<unsigned int>(std::stoul(icons[i]["index"].asString()))};
				auto filename{icons[i]["filename"].asString()};
				auto menu_item_s{icons[i]["menu_item"].asString()};
				auto key{icons[i]["key"].asString()};
				auto colour_hex{icons[i]["colour"].asString()};
				sf::Color colour{std::invoke([&] {
					if (colour_hex.length() == 0)
						return sf::Color(_layout.colour);
					else
						return sf::Color(std::stoul(colour_hex, 0, 16));
				})};
				MIM menu_item{MIM::NO_MENU_ITEM};

				// Use Magic Enum Library Reflection to convert the string to the type if we have an associated menu
				// item for the icon (which is used in an info panel beneath a menu)
				auto item_t{magic_enum::enum_cast<Sorcery::Enums::Menu::Item>(menu_item_s)};
				if (item_t.has_value())
					menu_item = item_t.value();

				// Add the Mapping for Icons
				const Icon icon{index, menu_item, key, filename, colour};

				// First for Menu Items
				if (menu_item != MIM::NO_MENU_ITEM) {
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

// Fortunately we are using square icons so we can easily find the appropriate icon from the spritesheet - note that we
// have 15 icons per row, and each icon is 511x511 pixels
auto Sorcery::IconStore::_get_rect(unsigned int index) const -> sf::IntRect {

	constexpr auto icon_size{511};
	return sf::IntRect(icon_size * (index % 15), icon_size * (index / 15), icon_size, icon_size);
}
