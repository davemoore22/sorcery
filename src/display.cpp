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

#include "display.hpp"

// Standard Constructor
Sorcery::Display::Display(System *system) : _system{system} {

	//_system = system;
	string = std::make_shared<String>((*_system->files)[STRINGS_FILE]);
	layout = std::make_shared<Layout>((*_system->files)[LAYOUT_FILE]);
	window = std::make_shared<Window>(
		_system, string.get(), layout.get(), (*string)["TITLE_AND_VERSION_INFO"]);
	overlay = std::make_shared<ControlOverlay>(_system, this, (*layout)["global:control_overlay"]);
	ui_texture = (*_system->resources).textures[GraphicsTexture::UI];
	_background_movie.openFromFile(_system->files->get_path_as_string(MENU_VIDEO));
	auto icon_layout{(*layout)["global:icon"]};
	_icons = std::make_unique<IconStore>(_system, icon_layout, (*_system->files)[ICONS_FILE]);
}

Sorcery::Display::Display(const Display &other)
	: string{other.string}, window{other.window}, layout{other.layout},
	  ui_texture{other.ui_texture}, _sprites{other._sprites}, _texts{other._texts},
	  _frames{other._frames}, _system{other._system}, _background_movie{other._background_movie},
	  _components{other._components} {}

auto Sorcery::Display::operator=(const Display &other) -> Display & {

	string = other.string;
	window = other.window;
	layout = other.layout;
	ui_texture = other.ui_texture;

	_sprites = other._sprites;
	_texts = other._texts;
	_frames = other._frames;
	_system = other._system;

	_background_movie = other._background_movie;
	_components = other._components;

	return *this;
}

auto Sorcery::Display::generate_components(const std::string &screen) -> void {

	_sprites.clear();
	_texts.clear();
	_frames.clear();
	generate_components(screen, _sprites, _texts, _frames);
}

auto Sorcery::Display::generate_components(const std::string &screen,
	std::map<std::string, sf::Sprite> &sprites, std::map<std::string, sf::Text> &texts,
	std::map<std::string, std::shared_ptr<Frame>> &frames) -> void {

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
					const int offset_x = [&] {
						if (component["offset_x"])
							return std::stoi(component["offset_x"].value());
						else
							return 0;
					}();
					const int offset_y = [&] {
						if (component["offset_y"])
							return std::stoi(component["offset_y"].value());
						else
							return 0;
					}();
					image.setPosition(component.x + offset_x, component.y + offset_y);
					image.setScale(component.scale, component.scale);

					// Add the image to the components ready to draw
					sprites[component.unique_key] = image;
				}
			} else if (component.type == ComponentType::IMAGE) {

				// Skip in case of an error
				if (component.texture == GraphicsTexture::NONE)
					continue;

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
					float scale_ratio_needed{1.0f};
					if ((size.w > window_size.w) || (size.h > window_size.h)) {
						float shrink_width_needed{
							static_cast<float>(window_size.w) / static_cast<float>(size.w)};
						float shrink_height_needed{
							static_cast<float>(window_size.h) / static_cast<float>(size.h)};
						scale_ratio_needed = std::min(shrink_width_needed, shrink_height_needed);
					}
					image.setScale(scale_ratio_needed, scale_ratio_needed);
				} else if (component.unique_key.ends_with("wallpaper")) {

					// Handle background wallpaper tiling
					image.setTextureRect(window->size);
				}

				// Check for Offsets
				const int offset_x = [&] {
					if (component["offset_x"])
						return std::stoi(component["offset_x"].value());
					else
						return 0;
				}();
				const int offset_y = [&] {
					if (component["offset_y"])
						return std::stoi(component["offset_y"].value());
					else
						return 0;
				}();

				// Set the image position
				const sf::Vector2f image_pos(window->get_x(image, component.x + offset_x),
					window->get_y(image, component.y + offset_y));
				image.setPosition(image_pos);

				// Add the image to the components ready to draw
				sprites[component.unique_key] = image;

			} else if (component.type == ComponentType::FRAME) {

				auto frame = std::make_shared<Frame>(
					_system->resources->textures[GraphicsTexture::UI], WindowFrameType::NORMAL,
					component.w, component.h, component.colour, component.alpha);

				// Check for Offsets
				const int offset_x = [&] {
					if (component["offset_x"])
						return std::stoi(component["offset_x"].value());
					else
						return 0;
				}();
				const int offset_y = [&] {
					if (component["offset_y"])
						return std::stoi(component["offset_y"].value());
					else
						return 0;
				}();

				frame->setPosition(window->get_x(frame->sprite, component.x) + offset_x,
					window->get_y(frame->sprite, component.y) + offset_y);
				frames.emplace(std::make_pair(component.unique_key, std::move(frame)));
			} else if (component.type == ComponentType::TEXT) {

				sf::Text text;
				int x{0};
				int y{0};

				text.setFont(_system->resources->fonts[component.font]);
				text.setCharacterSize(component.size);
				text.setFillColor(sf::Color(component.colour));
				text.setString((*string)[component.string_key]);
				x = component.x == -1 ? window->centre.x : component.x;
				y = component.y == -1 ? window->centre.y : component.y;

				// Check for Offsets
				const int offset_x = [&] {
					if (component["offset_x"])
						return std::stoi(component["offset_x"].value());
					else
						return 0;
				}();
				const int offset_y = [&] {
					if (component["offset_y"])
						return std::stoi(component["offset_y"].value());
					else
						return 0;
				}();

				if (component.justification == Justification::CENTRE) {
					text.setPosition(x + offset_x, y + offset_y);
					text.setOrigin(
						text.getLocalBounds().width / 2.0f, text.getLocalBounds().height / 2.0f);
				} else if (component.justification == Justification::RIGHT) {
					text.setPosition(x + offset_x, y + offset_y);
					const sf::FloatRect bounds{text.getLocalBounds()};
					text.setPosition(component.x - bounds.width, component.y);
				} else {
					text.setPosition(x + offset_x, y + offset_y);
					text.setOrigin(0, text.getLocalBounds().height / 2.0f);
				}

				// Add the image to the components ready to draw
				texts[component.unique_key] = text;
			} else if (component.type == ComponentType::MENU) {

				// Don't do this here - they are all manually displayed
			}
		}
	}
}

auto Sorcery::Display::display_components(
	const std::string &screen, std::optional<std::any> parameter) -> void {

	display_components(screen, _sprites, _texts, _frames, parameter);
}

auto Sorcery::Display::display_components(const std::string &screen,
	std::map<std::string, sf::Sprite> &sprites, std::map<std::string, sf::Text> &texts,
	std::map<std::string, std::shared_ptr<Frame>> &frames, std::optional<std::any> parameter)
	-> void {

	// first draw anything with wallpaper
	for (auto &[unique_key, sprite] : sprites) {
		if (unique_key.ends_with("wallpaper"))
			window->get_window()->draw(sprite);
	}

	// Handle the different sized window frames in the town menu
	for (auto &[unique_key, frame] : frames) {
		if (screen == "castle") {
			if (parameter) {
				if (const GameMenuType menu_stage{std::any_cast<GameMenuType>(parameter.value())};
					menu_stage == GameMenuType::CASTLE) {
					if (unique_key.ends_with("castle:edge_menu_frame"))
						continue;
				} else if (menu_stage == GameMenuType::EDGE_OF_TOWN) {
					if (unique_key.ends_with("castle:castle_menu_frame"))
						continue;
				}
			}
		} else if (screen == "create") {
			if (parameter) {
				if (const CharacterStage character_stage{
						std::any_cast<CharacterStage>(parameter.value())};
					character_stage == CharacterStage::CHOOSE_METHOD ||
					character_stage == CharacterStage::REVIEW_AND_CONFIRM) {
					if ((unique_key.ends_with("_frame_progress")) ||
						(unique_key.ends_with("_summary_progres")))
						continue;
				}
			}
		}

		window->get_window()->draw(*frame);
	}

	// Display all other sprites, but not the background wallpaper
	for (auto &[unique_key, sprite] : sprites) {
		if ((unique_key.ends_with("banner:banner_image")) ||
			(unique_key.ends_with("splash:splash_image"))) {
			if (parameter) {
				sprite.setColor(
					sf::Color(255, 255, 255, std::any_cast<unsigned int>(parameter.value())));
			}
		}

		if (unique_key.ends_with("wallpaper"))
			continue;
		else
			window->get_window()->draw(sprite);
	}

	for (auto &[unique_key, text] : texts) {

		if (screen == "main_menu_attract") {
			if (parameter) {

				// TODO: replace with IF-INIT?
				if (const MainMenuType menu_stage{std::any_cast<MainMenuType>(parameter.value())};
					menu_stage == MainMenuType::ATTRACT_MENU) {
					if ((unique_key.ends_with("main_menu_attract:press_any_key")) ||
						(unique_key.ends_with("main_menu_attract:subtitle_1")) ||
						(unique_key.ends_with("main_menu_attract:subtitle_2")) ||
						(unique_key.ends_with("main_menu_attract:copyright")))
						continue;
				} else if (menu_stage == MainMenuType::ATTRACT_MODE) {
				}
			}
		} else if (screen == "castle") {
			if (parameter) {

				// TODO: replace with IF-INIT?
				const GameMenuType menu_stage{std::any_cast<GameMenuType>(parameter.value())};
				if (menu_stage == GameMenuType::CASTLE) {
					if ((unique_key.ends_with("castle:edge_title_frame")) ||
						(unique_key.ends_with("castle:edge_title_text")))
						continue;
				} else if (menu_stage == GameMenuType::EDGE_OF_TOWN) {
					if ((unique_key.ends_with("castle:castle_title_frame")) ||
						(unique_key.ends_with("castle:castle_title_text")))
						continue;
				}
			}
		} else if (screen == "create") {
			if (parameter) {
				if (const CharacterStage character_stage{
						std::any_cast<CharacterStage>(parameter.value())};
					character_stage == CharacterStage::CHOOSE_METHOD ||
					character_stage == CharacterStage::REVIEW_AND_CONFIRM) {
					if ((unique_key.ends_with("_frame_progress")) ||
						(unique_key.ends_with("_summary_progress")))
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

auto Sorcery::Display::display_cursor() -> void {

	sf::Sprite cursor{window->get_cursor()};
	cursor.setPosition(static_cast<sf::Vector2f>(sf::Mouse::getPosition(*window->get_window())));
	window->get_window()->draw(cursor);
}

auto Sorcery::Display::fit_background_movie() -> void {
	_background_movie.fit(
		0, 0, window->get_window()->getSize().x, window->get_window()->getSize().y);
}

auto Sorcery::Display::start_background_movie() -> void {

	if (_background_movie.getStatus() == sfe::Stopped)
		_background_movie.play();
}

auto Sorcery::Display::stop_background_movie() -> void {

	if (_background_movie.getStatus() == sfe::Playing)
		_background_movie.stop();
}

auto Sorcery::Display::update_background_movie() -> void {

	_background_movie.update();
}

auto Sorcery::Display::draw_background_movie() -> void {

	window->get_window()->draw(_background_movie);
}
