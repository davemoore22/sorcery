// Copyright (C) 2023 Dave Moore
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

// Standard Constructor
Sorcery::ResourceManager::ResourceManager(File &files) : _files{files} {

	// Load all the Fixed Resources
	try {

		using enum Enums::Graphics::Texture;

		textures.acquire(AUTOMAP, thor::Resources::fromFile<sf::Texture>(_files[AUTOMAP_TEXTURE]));
		textures.acquire(BACKGROUND, thor::Resources::fromFile<sf::Texture>(_files[BACKGROUND_TEXTURE]));
		textures[BACKGROUND].setRepeated(true);
		textures.acquire(BANNER, thor::Resources::fromFile<sf::Texture>(_files[BANNER_TEXTURE]));
		textures.acquire(CONTROLS, thor::Resources::fromFile<sf::Texture>(_files[CONTROLS_TEXTURE]));
		textures.acquire(CREATURES_KNOWN, thor::Resources::fromFile<sf::Texture>(_files[CREATURES_KNOWN_TEXTURE]));
		textures.acquire(CREATURES_UNKNOWN, thor::Resources::fromFile<sf::Texture>(_files[CREATURES_UNKNOWN_TEXTURE]));
		textures.acquire(DOORS, thor::Resources::fromFile<sf::Texture>(_files[DOORS_TEXTURE]));
		textures.acquire(EVENTS, thor::Resources::fromFile<sf::Texture>(_files[EVENTS_TEXTURE]));
		textures.acquire(FLOORS, thor::Resources::fromFile<sf::Texture>(_files[FLOORS_TEXTURE]));
		textures.acquire(ITEMS, thor::Resources::fromFile<sf::Texture>(_files[ITEMS_TEXTURE]));
		textures.acquire(LOGO, thor::Resources::fromFile<sf::Texture>(_files[LOGO_TEXTURE]));
		textures.acquire(ICONS, thor::Resources::fromFile<sf::Texture>(_files[ICONS_TEXTURE]));
		textures.acquire(PORTRAITS, thor::Resources::fromFile<sf::Texture>(_files[PORTRAITS_TEXTURE]));
		textures.acquire(SPLASH, thor::Resources::fromFile<sf::Texture>(_files[SPLASH_TEXTURE]));
		textures.acquire(TOWN, thor::Resources::fromFile<sf::Texture>(_files[TOWN_TEXTURE]));
		textures.acquire(UI, thor::Resources::fromFile<sf::Texture>(_files[UI_TEXTURE]));
		textures.acquire(VIEW, thor::Resources::fromFile<sf::Texture>(_files[VIEW_TEXTURE]));
		textures.acquire(WALLS, thor::Resources::fromFile<sf::Texture>(_files[WALLS_TEXTURE]));
		textures.acquire(WIREFRAME, thor::Resources::fromFile<sf::Texture>(_files[WIREFRAME_TEXTURE]));

		fonts.acquire(FontType::INPUT, thor::Resources::fromFile<sf::Font>(_files[INPUT_FONT_FILE]));
		fonts.acquire(FontType::MONOSPACE, thor::Resources::fromFile<sf::Font>(_files[MONO_FONT_FILE]));
		fonts.acquire(FontType::PROPORTIONAL, thor::Resources::fromFile<sf::Font>(_files[PROPORTIONAL_FONT_FILE]));
		fonts.acquire(FontType::TEXT, thor::Resources::fromFile<sf::Font>(_files[TEXT_FONT_FILE]));

		license_file = std::make_shared<TextFile>(_files[LICENSE_FILE]);

	} catch (thor::ResourceLoadingException &e) {
		std::cout << e.what() << std::endl;
	}
}

auto Sorcery::ResourceManager::get_font_height(
	const FontType font_type, const unsigned int size, bool bold) const -> unsigned int {

	using enum Enums::Internal::FontType;

	sf::Font font{};
	switch (font_type) {
	case INPUT:
		font = fonts[INPUT];
		break;
	case MONOSPACE:
		font = fonts[MONOSPACE];
		break;
	case PROPORTIONAL:
		font = fonts[PROPORTIONAL];
		break;
	case TEXT:
		font = fonts[TEXT];
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
