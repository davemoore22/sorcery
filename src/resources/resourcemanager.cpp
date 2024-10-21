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

	// Load all the Fixed Resources
	try {

		textures.acquire(GTX::AUTOMAP,
			thor::Resources::fromFile<sf::Texture>(_files[AUTOMAP_TEXTURE]));
		textures.acquire(GTX::BACKGROUND,
			thor::Resources::fromFile<sf::Texture>(_files[BACKGROUND_TEXTURE]));
		textures[GTX::BACKGROUND].setRepeated(true);
		textures.acquire(GTX::BANNER,
			thor::Resources::fromFile<sf::Texture>(_files[BANNER_TEXTURE]));
		textures.acquire(GTX::CONTROLS,
			thor::Resources::fromFile<sf::Texture>(_files[CONTROLS_TEXTURE]));
		textures.acquire(
			GTX::CREATURES_KNOWN, thor::Resources::fromFile<sf::Texture>(
									  _files[CREATURES_KNOWN_TEXTURE]));
		textures.acquire(
			GTX::CREATURES_UNKNOWN, thor::Resources::fromFile<sf::Texture>(
										_files[CREATURES_UNKNOWN_TEXTURE]));
		textures.acquire(GTX::DOORS,
			thor::Resources::fromFile<sf::Texture>(_files[DOORS_TEXTURE]));
		textures.acquire(GTX::EVENTS,
			thor::Resources::fromFile<sf::Texture>(_files[EVENTS_TEXTURE]));
		textures.acquire(GTX::FLOORS,
			thor::Resources::fromFile<sf::Texture>(_files[FLOORS_TEXTURE]));
		textures.acquire(GTX::ITEMS,
			thor::Resources::fromFile<sf::Texture>(_files[ITEMS_TEXTURE]));
		textures.acquire(GTX::LOGO,
			thor::Resources::fromFile<sf::Texture>(_files[LOGO_TEXTURE]));
		textures.acquire(GTX::ICONS,
			thor::Resources::fromFile<sf::Texture>(_files[ICONS_TEXTURE]));
		textures.acquire(GTX::PORTRAITS,
			thor::Resources::fromFile<sf::Texture>(_files[PORTRAITS_TEXTURE]));
		textures.acquire(GTX::SPLASH,
			thor::Resources::fromFile<sf::Texture>(_files[SPLASH_TEXTURE]));
		textures.acquire(GTX::TOWN,
			thor::Resources::fromFile<sf::Texture>(_files[TOWN_TEXTURE]));
		textures.acquire(GTX::UI,
			thor::Resources::fromFile<sf::Texture>(_files[UI_TEXTURE]));
		textures.acquire(GTX::VIEW,
			thor::Resources::fromFile<sf::Texture>(_files[VIEW_TEXTURE]));
		textures.acquire(GTX::WALLS,
			thor::Resources::fromFile<sf::Texture>(_files[WALLS_TEXTURE]));
		textures.acquire(GTX::WIREFRAME,
			thor::Resources::fromFile<sf::Texture>(_files[WIREFRAME_TEXTURE]));

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

	return &textures[texture_type];
}

auto Sorcery::ResourceManager::get_texture_ref(const GTX texture_type)
	-> sf::Texture & {

	return textures[texture_type];
}