// Copyright (C) 2021 Dave Moore
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

#include "resourcemanager.hpp"

// Standard Constructor
Sorcery::ResourceManager::ResourceManager(File &files) : _files{files} {

	// Load all the Fixed Resources
	try {

		textures.acquire(Enums::Graphics::Texture::AUTOMAP,
			thor::Resources::fromFile<sf::Texture>(_files[AUTOMAP_TEXTURE]));
		textures.acquire(Enums::Graphics::Texture::BACKGROUND,
			thor::Resources::fromFile<sf::Texture>(_files[BACKGROUND_TEXTURE]));
		textures[Enums::Graphics::Texture::BACKGROUND].setRepeated(true);
		textures.acquire(Enums::Graphics::Texture::BANNER,
			thor::Resources::fromFile<sf::Texture>(_files[BANNER_TEXTURE]));
		textures.acquire(Enums::Graphics::Texture::CONTROLS,
			thor::Resources::fromFile<sf::Texture>(_files[CONTROLS_TEXTURE]));
		textures.acquire(Enums::Graphics::Texture::CREATURES_KNOWN,
			thor::Resources::fromFile<sf::Texture>(
				_files[CREATURES_KNOWN_TEXTURE]));
		textures.acquire(Enums::Graphics::Texture::CREATURES_UNKNOWN,
			thor::Resources::fromFile<sf::Texture>(
				_files[CREATURES_UNKNOWN_TEXTURE]));
		textures.acquire(Enums::Graphics::Texture::DOORS,
			thor::Resources::fromFile<sf::Texture>(_files[DOORS_TEXTURE]));
		textures.acquire(Enums::Graphics::Texture::FLOORS,
			thor::Resources::fromFile<sf::Texture>(_files[FLOORS_TEXTURE]));
		textures.acquire(Enums::Graphics::Texture::LOGO,
			thor::Resources::fromFile<sf::Texture>(_files[LOGO_TEXTURE]));
		textures.acquire(Enums::Graphics::Texture::ICONS,
			thor::Resources::fromFile<sf::Texture>(_files[ICONS_TEXTURE]));
		textures.acquire(Enums::Graphics::Texture::PORTRAITS,
			thor::Resources::fromFile<sf::Texture>(_files[PORTRAITS_TEXTURE]));
		textures.acquire(Enums::Graphics::Texture::SPLASH,
			thor::Resources::fromFile<sf::Texture>(_files[SPLASH_TEXTURE]));
		textures.acquire(Enums::Graphics::Texture::TOWN,
			thor::Resources::fromFile<sf::Texture>(_files[TOWN_TEXTURE]));
		textures.acquire(Enums::Graphics::Texture::UI,
			thor::Resources::fromFile<sf::Texture>(_files[UI_TEXTURE]));
		textures.acquire(Enums::Graphics::Texture::WALLS,
			thor::Resources::fromFile<sf::Texture>(_files[WALLS_TEXTURE]));

		fonts.acquire(FontType::INPUT,
			thor::Resources::fromFile<sf::Font>(_files[INPUT_FONT_FILE]));
		fonts.acquire(FontType::MONOSPACE,
			thor::Resources::fromFile<sf::Font>(_files[MONO_FONT_FILE]));
		fonts.acquire(
			FontType::PROPORTIONAL, thor::Resources::fromFile<sf::Font>(
										_files[PROPORTIONAL_FONT_FILE]));
		fonts.acquire(FontType::TEXT,
			thor::Resources::fromFile<sf::Font>(_files[TEXT_FONT_FILE]));

		license_file = std::make_shared<TextFile>(_files[LICENSE_FILE]);

	} catch (thor::ResourceLoadingException &e) {
		std::cout << e.what() << std::endl;
	}
}