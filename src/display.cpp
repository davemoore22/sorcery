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

#include "display.hpp"

// Standard Constructor
Sorcery::Display::Display(System *system) : _system{system} {

	string = std::make_unique<String>((*_system->files)[STRINGS_FILE]);
	layout = std::make_unique<Layout>((*_system->files)[LAYOUT_FILE]);
	window = std::make_unique<Window>(_system, string.get(), layout.get(), (*string)["TITLE_AND_VERSION_INFO"]);
	overlay = std::make_unique<ControlOverlay>(_system, this, (*layout)["global:control_overlay"]);
	ui_texture = (*_system->resources).textures[GraphicsTexture::UI];
	_background_movie.openFromFile(_system->files->get_path(MENU_VIDEO));
	auto icon_layout{(*layout)["global:icon"]};
	_icons = std::make_unique<IconStore>(_system, icon_layout, (*_system->files)[ICONS_FILE]);
}

auto Sorcery::Display::generate(std::string_view screen) -> void {

	_sprites.clear();
	_texts.clear();
	_frames.clear();
	generate(screen, _sprites, _texts, _frames);
}

auto Sorcery::Display::shutdown_SFML() -> void {

	window->get_window()->close();
}

auto Sorcery::Display::generate(std::string_view screen, std::map<std::string, sf::Sprite> &sprites,
	std::map<std::string, sf::Text> &texts, std::map<std::string, std::shared_ptr<Frame>> &frames) -> void {

	sprites.clear();
	texts.clear();
	frames.clear();
	const std::optional<std::vector<Component>> components{(*layout)(screen)};
	if (components) {
		for (const auto &component : components.value()) {
			if (component.type == ComponentType::ICON) {

				// use string for the icon key
				auto icon{(*_icons)[component.string_key]};
				if (icon) {
					auto image{icon.value()};

					// Check for Offsets
					const auto offset_x{[&] {
						if (component["offset_x"])
							return std::stoi(component["offset_x"].value());
						else
							return 0;
					}()};
					const auto offset_y{[&] {
						if (component["offset_y"])
							return std::stoi(component["offset_y"].value());
						else
							return 0;
					}()};
					image.setPosition(component.x + offset_x, component.y + offset_y);
					image.setScale(component.scale, component.scale);

					if (component.colour != 0ULL)
						image.setColor(sf::Color(component.colour));

					// Add the image to the components ready to draw
					sprites[component.unique_key] = image;
				}
			} else if (component.type == ComponentType::IMAGE) {

				// Skip in case of an error
				if (component.texture == GraphicsTexture::NO_TEXTURE)
					continue;

				if (component.unique_key.ends_with("background")) {

					// Town Frame Wallpapers
					sf::IntRect bg_rect{};
					bg_rect.width = std::stoi(component["source_w"].value());
					bg_rect.height = std::stoi(component["source_h"].value());
					bg_rect.top = 0;
					bg_rect.left =
						std::stoi(component["source_w"].value()) * std::stoi(component["source_index"].value());
					sf::Sprite image{};
					image.setTexture(_system->resources->textures[GraphicsTexture::TOWN]);
					image.setTextureRect(bg_rect);
					image.setScale(std::stof(component["scale_x"].value()), std::stof(component["scale_y"].value()));
					image.setPosition(window->get_x(image, component.x), window->get_y(image, component.y));

					// Add the image to the components ready to draw
					sprites[component.unique_key] = image;
				} else {

					// Get the texture
					sf::Sprite image;
					image.setTexture(_system->resources->textures[component.texture]);

					// Scale to less than the window size if needed
					if ((component.unique_key.ends_with("banner:banner_image")) ||
						(component.unique_key.ends_with("splash:splash_image")) ||
						(component.unique_key.ends_with("main_menu_attract:logo_image"))) {
						const ImageSize size{static_cast<unsigned int>(image.getLocalBounds().width),
							static_cast<unsigned int>(image.getLocalBounds().height)};
						const ImageSize window_size{
							window->get_window()->getSize().x, window->get_window()->getSize().y};
						auto scale_ratio_needed{1.0f};
						if ((size.w > window_size.w) || (size.h > window_size.h)) {
							auto shrink_width_needed{static_cast<float>(window_size.w) / static_cast<float>(size.w)};
							auto shrink_height_needed{static_cast<float>(window_size.h) / static_cast<float>(size.h)};
							scale_ratio_needed = std::min(shrink_width_needed, shrink_height_needed);
						}
						image.setScale(scale_ratio_needed, scale_ratio_needed);
					} else if (component.unique_key.ends_with("wallpaper")) {

						// Handle background wallpaper tiling
						image.setTextureRect(window->size);
					}

					// Check for Offsets
					const auto offset_x{[&] {
						if (component["offset_x"])
							return std::stoi(component["offset_x"].value());
						else
							return 0;
					}()};
					const auto offset_y{[&] {
						if (component["offset_y"])
							return std::stoi(component["offset_y"].value());
						else
							return 0;
					}()};

					// Set the image position
					const sf::Vector2f image_pos(
						window->get_x(image, component.x + offset_x), window->get_y(image, component.y + offset_y));
					image.setPosition(image_pos);

					// Add the image to the components ready to draw
					sprites[component.unique_key] = image;
				}

			} else if (component.type == ComponentType::FRAME) {

				auto frame = std::make_shared<Frame>(_system->resources->textures[GraphicsTexture::UI], component.w,
					component.h, component.colour, component.background, component.alpha);

				// Check for Offsets
				const auto offset_x{[&] {
					if (component["offset_x"])
						return std::stoi(component["offset_x"].value());
					else
						return 0;
				}()};
				const auto offset_y{[&] {
					if (component["offset_y"])
						return std::stoi(component["offset_y"].value());
					else
						return 0;
				}()};

				frame->setPosition(window->get_x(frame->sprite, component.x) + offset_x,
					window->get_y(frame->sprite, component.y) + offset_y);
				frames.emplace(std::make_pair(component.unique_key, std::move(frame)));
			} else if (component.type == ComponentType::TEXT) {

				sf::Text text{};

				// text.setStyle(sf::Text::Bold);
				text.setFont(_system->resources->fonts[component.font]);
				text.setCharacterSize(component.size);
				auto string_to_print{(*string)[component.string_key]};
				std::transform(string_to_print.begin(), string_to_print.end(), string_to_print.begin(), ::toupper);
				text.setFillColor(sf::Color(component.colour));
				text.setString(string_to_print);
				auto x{component.x == -1 ? window->centre.x : component.x};
				auto y{component.y == -1 ? window->centre.y : component.y};

				// Check for Offsets
				const auto offset_x{[&] {
					if (component["offset_x"])
						return std::stoi(component["offset_x"].value());
					else
						return 0;
				}()};
				const auto offset_y{[&] {
					if (component["offset_y"])
						return std::stoi(component["offset_y"].value());
					else
						return 0;
				}()};

				if (component.justification == Justification::CENTRE) {
					text.setPosition(x + offset_x, y + offset_y);
					text.setOrigin(text.getLocalBounds().width / 2.0f, 0);
				} else if (component.justification == Justification::RIGHT) {
					text.setPosition(x + offset_x, y + offset_y);
					const sf::FloatRect bounds{text.getLocalBounds()};
					text.setPosition(component.x - bounds.width, component.y);
				} else
					text.setPosition(x + offset_x, y + offset_y);

				// Add the image to the components ready to draw
				texts[component.unique_key] = text;
			}
		}
	}
}

auto Sorcery::Display::show_overlay() -> void {
	_show_overlay = true;
}

auto Sorcery::Display::hide_overlay() -> void {
	_show_overlay = false;
}

auto Sorcery::Display::display(std::string_view screen, std::optional<std::any> parameter) -> void {

	display(screen, _sprites, _texts, _frames, parameter);
}

auto Sorcery::Display::display_overlay() -> void {

	const sf::Vector2f pos(window->get_x(overlay->width, (*layout)["global:control_overlay"].x),
		window->get_y(overlay->height, (*layout)["global:control_overlay"].y));

	if (_show_overlay && overlay->valid) {
		overlay->setPosition(pos);
		window->get_window()->draw(*overlay);
	}
}

auto Sorcery::Display::display(std::string_view screen, std::map<std::string, sf::Sprite> &sprites,
	std::map<std::string, sf::Text> &texts, std::map<std::string, std::shared_ptr<Frame>> &frames,
	std::optional<std::any> parameter) -> void {

	// first draw anything with wallpaper/background
	for (auto &[unique_key, sprite] : sprites) {
		if (unique_key.ends_with("wallpaper"))
			window->get_window()->draw(sprite);
		if (unique_key.ends_with("background"))
			window->get_window()->draw(sprite);
	}

	// Handle the different sized window frames in the town menu
	for (auto &[unique_key, frame] : frames) {
		if (screen == "create") {
			if (parameter) {
				if (const CharacterStage character_stage{std::any_cast<CharacterStage>(parameter.value())};
					character_stage == CharacterStage::CHOOSE_METHOD ||
					character_stage == CharacterStage::REVIEW_AND_CONFIRM) {
					if ((unique_key.ends_with("_frame_progress")) || (unique_key.ends_with("_summary_progres")))
						continue;
				}
			}
		}

		window->get_window()->draw(*frame);
	}

	// Display all other sprites, but not the background wallpaper
	for (auto &[unique_key, sprite] : sprites) {
		if ((unique_key.ends_with("banner:banner_image")) || (unique_key.ends_with("splash:splash_image"))) {
			if (parameter) {
				sprite.setColor(sf::Color(255, 255, 255, std::any_cast<unsigned int>(parameter.value())));
			}
		}

		if (unique_key.ends_with("wallpaper"))
			continue;
		else if (unique_key.ends_with("background"))
			continue;
		else
			window->get_window()->draw(sprite);
	}

	for (const auto &[unique_key, text] : texts) {

		if (screen == "main_menu_attract") {
			if (parameter) {
				if (const MainMenuType menu_stage{std::any_cast<MainMenuType>(parameter.value())};
					menu_stage == MainMenuType::ATTRACT_MENU) {
					if ((unique_key.ends_with("main_menu_attract:press_any_key")) ||
						(unique_key.ends_with("main_menu_attract:subtitle_1")) ||
						(unique_key.ends_with("main_menu_attract:subtitle_2")) ||
						(unique_key.ends_with("main_menu_attract:copyright")))
						continue;
				}
			}
		} else if (screen == "create") {
			if (parameter) {
				if (const CharacterStage character_stage{std::any_cast<CharacterStage>(parameter.value())};
					character_stage == CharacterStage::CHOOSE_METHOD ||
					character_stage == CharacterStage::REVIEW_AND_CONFIRM) {
					if ((unique_key.ends_with("_frame_progress")) || (unique_key.ends_with("_summary_progress")))
						continue;
				}
			}
		}

		window->get_window()->draw(text);
	}
}

auto Sorcery::Display::set_input_mode(WindowInputMode input_mode) -> void {

	window->set_input_mode(input_mode);
	overlay->set_input_mode(input_mode);
}

auto Sorcery::Display::get_input_mode() const -> WindowInputMode {

	return window->get_input_mode();
}

auto Sorcery::Display::display_direction_indicator(MapDirection direction, bool monochrome = false) -> void {

	auto di_layout{(*layout)["engine_base_ui:direction_indicator"]};
	auto di_type{[&] { return monochrome ? di_layout["mono"].value() : di_layout["not_mono"].value(); }()};
	auto di_icon{(*_icons)[di_type]};
	if (di_icon) {
		auto indicator{di_icon.value()};
		indicator.setOrigin(indicator.getLocalBounds().width / 2, indicator.getLocalBounds().height / 2);
		switch (direction) {
		case MapDirection::NORTH:
			indicator.setRotation(180.0f);
			break;
		case MapDirection::SOUTH:
			indicator.setRotation(0.0f);
			break;
		case MapDirection::EAST:
			indicator.setRotation(270.0f);
			break;
		case MapDirection::WEST:
			indicator.setRotation(90.0f);
			break;
		default:
			break;
		}

		const auto offset_x{[&] {
			if (di_layout["offset_x"])
				return std::stoi(di_layout["offset_x"].value());
			else
				return 0;
		}()};
		const auto offset_y{[&] {
			if (di_layout["offset_y"])
				return std::stoi(di_layout["offset_y"].value());
			else
				return 0;
		}()};

		sf::RectangleShape backdrop{sf::Vector2f(512, 512)};
		backdrop.setFillColor(sf::Color(0, 0, 0));
		backdrop.setOrigin(backdrop.getLocalBounds().width / 2, backdrop.getLocalBounds().height / 2);

		indicator.setPosition((indicator.getGlobalBounds().width / 2) + di_layout.x + offset_x,
			(indicator.getGlobalBounds().height / 2) + di_layout.y + offset_y);
		backdrop.setPosition((indicator.getGlobalBounds().width / 2) + di_layout.x + offset_x,
			(indicator.getGlobalBounds().height / 2) + di_layout.y + offset_y);
		indicator.setScale(di_layout.scale, di_layout.scale);
		backdrop.setScale(di_layout.scale, di_layout.scale);

		window->get_window()->draw(backdrop);
		window->get_window()->draw(indicator);
	}
}

auto Sorcery::Display::display_cursor() -> void {

	sf::Sprite cursor{window->get_cursor()};
	cursor.setPosition(static_cast<sf::Vector2f>(sf::Mouse::getPosition(*window->get_window())));
	window->get_window()->draw(cursor);
}

auto Sorcery::Display::fit_bg_movie() -> void {
	_background_movie.fit(0, 0, window->get_window()->getSize().x, window->get_window()->getSize().y);
}

auto Sorcery::Display::start_bg_movie() -> void {

	if (_background_movie.getStatus() == sfe::Stopped)
		_background_movie.play();
}

auto Sorcery::Display::stop_bg_movie() -> void {

	if (_background_movie.getStatus() == sfe::Playing)
		_background_movie.stop();
}

auto Sorcery::Display::update_bg_movie() -> void {

	_background_movie.update();
}

auto Sorcery::Display::draw_bg_movie() -> void {

	window->get_window()->draw(_background_movie);
}
