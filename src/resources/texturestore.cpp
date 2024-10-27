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

#include "resources/texturestore.hpp"
#include "common/enum.hpp"
#include "core/system.hpp"
#include "resources/define.hpp"
#include "resources/resourcemanager.hpp"

// Standard Constructor
Sorcery::TextureStore::TextureStore(
	System *system, const std::filesystem::path filename)
	: _system{system} {

	// Prepare the icon stores
	_texture_map.clear();

	// Get the Textures
	_automap_t = _system->resources->get_texture(GTX::AUTOMAP);
	_wall_t = _system->resources->get_texture(GTX::WALLS);
	_ceiling_t = _system->resources->get_texture(GTX::FLOORS);
	_floor_t = _system->resources->get_texture(GTX::FLOORS);
	_door_t = _system->resources->get_texture(GTX::DOORS);
	_item_t = _system->resources->get_texture(GTX::ITEMS);
	_creatures_known_t = _system->resources->get_texture(GTX::CREATURES_KNOWN);
	_creatures_unknown_t =
		_system->resources->get_texture(GTX::CREATURES_UNKNOWN);
	_portrait_t = _system->resources->get_texture(GTX::PORTRAITS);
	_view_t = _system->resources->get_texture(GTX::VIEW);
	_events_t = _system->resources->get_texture(GTX::EVENTS);

	// Load the Mapping
	_loaded = _load(filename);
}

// Overload [] Operator(const) -> std::optional<Texture>
auto Sorcery::TextureStore::operator[](unsigned int index) const
	-> std::optional<Texture> {

	auto texture{_get(index)};
	return texture;
}

// Get the indexed texture as an appropriate sprite - note that for
// WALLS/CEILINGS/FLOORS/DOORS, the index refers to the entry in textures.json;
// whereas for all other spritesheets it refers to the actual sprite position
auto Sorcery::TextureStore::get(const unsigned int index,
	GTT texture_type) const -> std::optional<sf::Sprite> {

	std::optional<Sorcery::Texture> texture{std::nullopt};
	sf::Texture *source{nullptr};
	auto idx{0u};
	switch (texture_type) {
	case GTT::FLOOR:
		texture = _get(index);
		source = _floor_t;
		idx = texture.value().floor;
		break;
	case GTT::CEILING:
		texture = _get(index);
		source = _floor_t;
		idx = texture.value().ceiling;
		break;
	case GTT::WALL:
		texture = _get(index);
		idx = texture.value().wall;
		source = _wall_t;
		break;
	case GTT::DOOR:
		texture = _get(index);
		idx = texture.value().door;
		source = _door_t;
		break;
	case GTT::AUTOMAP:
		idx = index;
		source = _automap_t;
		break;
	case GTT::KNOWN_CREATURE:
		idx = index;
		source = _creatures_known_t;
		break;
	case GTT::UNKNOWN_CREATURE:
		source = _creatures_unknown_t;
		idx = index;
		break;
	case GTT::ITEMS:
		idx = index;
		source = _item_t;
		break;
	case GTT::PORTRAIT:
		source = _portrait_t;
		idx = index;
		break;
	case GTT::EVENTS:
		source = _events_t;
		idx = index;
		break;
	case GTT::WALLPAPER:
		source = _wall_t;
		idx = index;
		break;
	default:
		// Doesn't work for View, as we need the exact coordinates for the atlas
		return std::nullopt;
	}

	sf::IntRect tile_r{_get_rect(idx, texture_type)};
	sf::Sprite tile(*source);
	tile.setTextureRect(tile_r);
	return tile;
}

// Get the named texture as an appropriate sprite - WALLS/CEILINGS/FLOORS/DOORS
// only since those are only texture ids stored in the Texture class
auto Sorcery::TextureStore::get(const std::string name, GTT texture_type) const
	-> std::optional<sf::Sprite> {

	std::optional<Sorcery::Texture> texture{std::nullopt};
	sf::Texture *source{nullptr};
	auto idx{0u};
	switch (texture_type) {
	case GTT::FLOOR:
		texture = _get(name);
		source = _floor_t;
		idx = texture.value().floor;
		break;
	case GTT::CEILING:
		texture = _get(name);
		source = _floor_t;
		idx = texture.value().ceiling;
		break;
	case GTT::WALL:
		texture = _get(name);
		idx = texture.value().wall;
		source = _wall_t;
		break;
	case GTT::DOOR:
		texture = _get(name);
		idx = texture.value().door;
		source = _door_t;
		break;
	default:
		return std::nullopt;
	}

	sf::IntRect tile_r{_get_rect(idx, texture_type)};
	sf::Sprite tile(*source);
	tile.setTextureRect(tile_r);

	return tile;
}

auto Sorcery::TextureStore::get_atlas(
	const Rect rect, bool feature = false) const -> sf::Sprite {

	sf::Sprite view(*_view_t);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnarrowing"
	sf::IntRect tile_r{rect.x, rect.y, rect.w, rect.h};
#pragma GCC diagnostic pop
	if (feature)
		tile_r.left += VIEW_OFFSET_SIZE;
	view.setTextureRect(tile_r);

	return view;
}

auto Sorcery::TextureStore::_get(const std::string name) const
	-> std::optional<Texture> {

	if (_loaded) {
		auto it = std::find_if(_texture_map.begin(), _texture_map.end(),
			[&](const auto &item) { return item.second.name == name; });

		if (it != _texture_map.end())
			return _texture_map.at((*it).first);
		else
			return std::nullopt;
	} else
		return std::nullopt;
}

auto Sorcery::TextureStore::_get(const unsigned int index) const
	-> std::optional<Texture> {

	if (_loaded)
		return _texture_map.at(index);
	else
		return std::nullopt;
}

auto Sorcery::TextureStore::_get_rect(
	unsigned int index, GTT texture_type) const -> sf::IntRect {

	int tile_size{std::invoke([&] {
		switch (texture_type) {
		case GTT::AUTOMAP:
			return AUTOMAP_TILE_SIZE;
			break;
		case GTT::KNOWN_CREATURE:
		case GTT::UNKNOWN_CREATURE:
			return CREATURE_TILE_SIZE;
			break;
		case GTT::PORTRAIT:
			return PORTRAIT_TILE_SIZE;
			break;
		case GTT::ITEMS:
			return ITEM_TILE_SIZE;
			break;
		case GTT::EVENTS:
			return EVENT_TILE_SIZE;
			break;
		case GTT::WALLPAPER:
			return WALLPAPER_TILE_SIZE;
			break;
		default:
			return DUNGEON_TILE_SIZE;
			break;
		}
	})};
	int tile_row_count{std::invoke([&] {
		switch (texture_type) {
		case GTT::AUTOMAP:
			return AUTOMAP_TILE_ROW_COUNT;
			break;
		case GTT::KNOWN_CREATURE:
		case GTT::UNKNOWN_CREATURE:
			return CREATURE_TILE_ROW_COUNT;
			break;
		case GTT::PORTRAIT:
			return PORTRAIT_TILE_ROW_COUNT;
			break;
		case GTT::ITEMS:
			return ITEM_TILE_ROW_COUNT;
			break;
		case GTT::EVENTS:
			return EVENT_TILE_ROW_COUNT;
			break;
		case GTT::WALLPAPER:
			return WALLPAPER_TILE_ROW_COUNT;
			break;
		default:
			return DUNGEON_TILE_ROW_COUNT;
			break;
		}
	})};

	return sf::IntRect(tile_size * (index % tile_row_count),
		tile_size * (index / tile_row_count), tile_size, tile_size);
}

// For those textures that are mapped
auto Sorcery::TextureStore::_load(const std::filesystem::path filename)
	-> bool {

	if (std::ifstream file{filename.string(), std::ifstream::binary};
		file.good()) {

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
		Json::Reader reader{};
#pragma GCC diagnostic pop
		Json::StreamWriterBuilder builder{};
		builder.settings_["indentation"] = "";
		if (Json::Value layout; reader.parse(file, layout)) {
			Json::Value &textures{layout["texture"]};

			// Iterate through texture file one texture at a time
			for (auto i = 0u; i < textures.size(); i++) {

				// Get the mappings for each texture (note that all parameters
				// should always be present for each texture entry in the
				// mapping file or else this will error)
				auto index{static_cast<unsigned int>(
					std::stoul(textures[i]["index"].asString()))};
				auto wall{static_cast<unsigned int>(
					std::stoul(textures[i]["wall"].asString()))};
				auto ceiling{static_cast<unsigned int>(
					std::stoul(textures[i]["ceiling"].asString()))};
				auto floor{static_cast<unsigned int>(
					std::stoul(textures[i]["floor"].asString()))};
				auto door{static_cast<unsigned int>(
					std::stoul(textures[i]["door"].asString()))};
				auto source{textures[i]["source"].asString()};
				auto comment{textures[i]["comment"].asString()};
				auto name{textures[i]["name"].asString()};
				const Texture texture{
					name, index, wall, floor, ceiling, door, source, comment};

				_texture_map[index] = texture;
			}
		}
	} else
		return false;

	return true;
}
