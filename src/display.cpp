// Copyright (C) 2020 Dave Moore
//
// This file is part of Sorcery: Dreams of the Mad Overlord.
//
// Sorcery: Dreams of the Mad Overlord is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Sorcery: Dreams of the Mad Overlord is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Sorcery: Dreams of the Mad Overlord.  If not, see <http://www.gnu.org/licenses/>.
//
// If you modify this Program, or any covered work, by linking or combining it
// with the libraries referred to in README (or a modified version of said
// libraries), containing parts covered by the terms of said libraries, the
// licensors of this Program grant you additional permission to convey the
// resulting work.

#include "display.hpp"

// Standard Constructor
Sorcery::Display::Display(System* system) {

	string = std::make_shared<String>((*system->files)[STRINGS_FILE], (*system->files)[EXPLAIN_FILE]);
	std::string window_title {string->get("TITLE_AND_VERSION_INFO")};
	layout = std::make_shared<Layout>((*system->files)[LAYOUT_FILE]);
	window = std::make_shared<Window>(window_title, *system, *string, *layout);
	ui_texture = (*system->resources).textures[UI_TEXTURE];

	_system = system;
}

auto Sorcery::Display::generate_components(const std::string screen) -> void {

	_sprites.clear();
	_texts.clear();
	std::optional<std::vector<Component>> components {(*layout)(screen)};
	if (components) {
		for (const auto& component: components.value()) {
			if (component.type == ComponentType::IMAGE) {

				// Skip in case of an error
				if (component.texture == GraphicsTexture::NONE)
					continue;

				// Get the texture
				sf::Sprite image;
				//image.setTexture(_system.resources->textures[BANNER_TEXTURE]);
				image.setTexture(_system->resources->texture[component.texture]);

				// Scale to less than the window size if needed
				if (component.unique_key == "banner:banner_image") {
					ImageSize size {static_cast<unsigned int>(image.getLocalBounds().width),
						static_cast<unsigned int>(image.getLocalBounds().height)};
					const ImageSize window_size {window->get_window()->getSize().x, window->get_window()->getSize().y};

					float scale_ratio_needed {1.0f};
					if ((size.w > window_size.w) || (size.h > window_size.h)) {
						float shrink_width_needed {static_cast<float>(window_size.w) / static_cast<float>(size.w)};
						float shrink_height_needed {static_cast<float>(window_size.h) / static_cast<float>(size.h)};
						scale_ratio_needed = std::min(shrink_width_needed, shrink_height_needed);
					}
					image.setScale(scale_ratio_needed, scale_ratio_needed);
				}

				// Set the image position
				const sf::Vector2f image_pos(window->get_x(image, component.x), window->get_y(image, component.y));
				image.setPosition(image_pos);

				// Add the image to the components ready to draw
				_sprites[component.unique_key] = image;

			} else if (component.type == ComponentType::FRAME) {


			} else if (component.type == ComponentType::TEXT) {

			} else if (component.type == ComponentType::MENU) {

			}
		}
	}
}

auto Sorcery::Display::display_components(std::optional<std::any> parameter) -> void {

	for (auto& [unique_key, sprite]: _sprites) {
		if (unique_key == "banner:banner_image") {
			if (parameter) {
				sprite.setColor(sf::Color(255,255,255, std::any_cast<unsigned int>(parameter.value())));
			}
		}

		window->get_window()->draw(sprite);
	}
}
