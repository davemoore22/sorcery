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

#include "resources/resourcemanager.hpp"
#include "types/textfile.hpp"

// Standard Constructor
Sorcery::ResourceManager::ResourceManager(FileStore &files) : _files{files} {

	_loaded.clear();
	_map.clear();
	_map.insert({GTX::AUTOMAP, _files[AUTOMAP_TEXTURE]});
	_map.insert({GTX::BACKGROUND, _files[BACKGROUND_TEXTURE]});
	_map.insert({GTX::BANNER, _files[BANNER_TEXTURE]});
	_map.insert({GTX::CONTROLS, _files[CONTROLS_TEXTURE]});
	_map.insert({GTX::CREATURES_KNOWN, _files[CREATURES_KNOWN_TEXTURE]});
	_map.insert({GTX::CREATURES_UNKNOWN, _files[CREATURES_UNKNOWN_TEXTURE]});
	_map.insert({GTX::DOORS, _files[DOORS_TEXTURE]});
	_map.insert({GTX::EVENTS, _files[EVENTS_TEXTURE]});
	_map.insert({GTX::FLOORS, _files[FLOORS_TEXTURE]});
	_map.insert({GTX::ITEMS, _files[ITEMS_TEXTURE]});
	_map.insert({GTX::LOGO, _files[LOGO_TEXTURE]});
	_map.insert({GTX::ICONS, _files[ICONS_TEXTURE]});
	_map.insert({GTX::PORTRAITS, _files[PORTRAITS_TEXTURE]});
	_map.insert({GTX::SPLASH, _files[SPLASH_TEXTURE]});
	_map.insert({GTX::TOWN, _files[TOWN_TEXTURE]});
	_map.insert({GTX::UI, _files[UI_TEXTURE]});
	_map.insert({GTX::VIEW, _files[VIEW_TEXTURE]});
	_map.insert({GTX::WALLS, _files[WALLS_TEXTURE]});
	_map.insert({GTX::WIREFRAME, _files[WIREFRAME_TEXTURE]});

	// Load all the Fixed Resources
	try {

		// Note that Textures are only loaded on demand
		fonts.acquire(FTT::INPUT,
			thor::Resources::fromFile<sf::Font>(_files[INPUT_FONT_FILE]));
		fonts.acquire(FTT::MONOSPACE,
			thor::Resources::fromFile<sf::Font>(_files[MONO_FONT_FILE]));
		fonts.acquire(FTT::PROPORTIONAL, thor::Resources::fromFile<sf::Font>(
											 _files[PROPORTIONAL_FONT_FILE]));
		fonts.acquire(FTT::TEXT,
			thor::Resources::fromFile<sf::Font>(_files[TEXT_FONT_FILE]));

		license_file = std::make_shared<TextFile>(_files[LICENSE_FILE]);

	} catch (thor::ResourceLoadingException &e) {
		std::cout << e.what() << std::endl;
	}
}

auto Sorcery::ResourceManager::get_font_height(const FTT font_type,
	const unsigned int size, bool bold) const -> unsigned int {

	sf::Font font{};
	switch (font_type) {
	case FTT::INPUT:
		font = fonts[FTT::INPUT];
		break;
	case FTT::MONOSPACE:
		font = fonts[FTT::MONOSPACE];
		break;
	case FTT::PROPORTIONAL:
		font = fonts[FTT::PROPORTIONAL];
		break;
	case FTT::TEXT:
		font = fonts[FTT::TEXT];
		break;
	default:
		return 0;
	}

	auto max_h{0u};
	auto current_h{0u};
	for (size_t char_index = 32; char_index < 128; ++char_index) {
		current_h = font.getGlyph(char_index, size, bold).bounds.height;
		if (current_h > max_h) {
			max_h = current_h;
		}
	}

	return max_h;
}

auto Sorcery::ResourceManager::get_texture(const GTX texture_type)
	-> sf::Texture * {

	if (!_loaded.contains(texture_type)) {
		_textures.acquire(texture_type,
			thor::Resources::fromFile<sf::Texture>(_map.at(texture_type)));
		if (texture_type == GTX::BACKGROUND)
			_textures[texture_type].setRepeated(true);
		_loaded.insert({texture_type, true});
	}

	return &_textures[texture_type];
}

auto Sorcery::ResourceManager::get_texture_ref(const GTX texture_type)
	-> sf::Texture & {

	return _textures[texture_type];
}
