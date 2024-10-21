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
// the resulting work.l

#include "core/display.hpp"
#include "common/define.hpp"
#include "common/enum.hpp"
#include "common/type.hpp"
#include "core/system.hpp"
#include "core/type.hpp"
#include "gui/frame.hpp"
#include "resources/filestore.hpp"
#include "resources/resourcemanager.hpp"

// Standard Constructor
Sorcery::Display::Display(System *system) : _system{system} {

	string = std::make_unique<StringStore>((*_system->files)[STRINGS_FILE]);
	layout = std::make_unique<ComponentStore>((*_system->files)[LAYOUT_FILE]);
	window = std::make_unique<Window>(_system, string.get(), layout.get(),
		(*string)["TITLE_AND_VERSION_INFO"]);
	overlay = std::make_unique<ControlOverlay>(
		_system, this, (*layout)["global:control_overlay"]);
	ui_texture = _system->resources->get_texture(GTX::UI);
	_bg_movie.openFromFile(_system->files->get_path(VIDEO_FILE));
	auto icon_layout{(*layout)["global:icon"]};

	// seperate copy of the icons store since graphics module is unaccessable
	// here
	_icons = std::make_unique<IconStore>(
		_system, icon_layout, (*_system->files)[ICONS_FILE]);

	_bold_text = false;
	_upper_text = false;
	window->set_bold(_bold_text);
	window->set_upper(_upper_text);

	_disc_access = false;
}

auto Sorcery::Display::get_centre_pos(const sf::Vector2f size) const
	-> sf::Vector2f {

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnarrowing"
	return sf::Vector2f{
		window->get_centre_x(size.x), window->get_centre_y(size.y)};
#pragma GCC diagnostic pop
}

auto Sorcery::Display::get_centre_x(const unsigned int width) const
	-> unsigned int {

	return window->get_centre_x(width);
}

auto Sorcery::Display::get_centre_y(const unsigned int height) const
	-> unsigned int {

	return window->get_centre_y(height);
}

auto Sorcery::Display::get_bold() const -> bool {

	return _bold_text;
}

auto Sorcery::Display::get_upper() const -> bool {

	return _upper_text;
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

auto Sorcery::Display::generate(std::string_view screen, SpriteMap &sprites,
	TextMap &texts, FrameMap &frames) -> void {

	sprites.clear();
	texts.clear();
	frames.clear();
	const std::optional<std::vector<Component>> components{(*layout)(screen)};
	if (components) {
		for (const auto &comp : components.value()) {
			if (comp.type == CPT::ICON) {

				// use string for the icon key
				auto icon{(*_icons)[comp.string_key]};
				if (icon) {
					auto image{icon.value()};

					// Check for Offsets
					const auto off_c{std::invoke([&] {
						if (comp["offset_x"])
							return std::stoi(comp["offset_x"].value());
						else
							return 0;
					})};
					const auto off_y{std::invoke([&] {
						if (comp["offset_y"])
							return std::stoi(comp["offset_y"].value());
						else
							return 0;
					})};
					image.setPosition(comp.x + off_c, comp.y + off_y);
					image.setScale(comp.scl());

					if (comp.colour != 0ULL)
						image.setColor(sf::Color(comp.colour));

					// Add the image to the components ready to draw
					sprites[comp.unique_key] = image;
				}
			} else if (comp.type == CPT::IMAGE) {

				// Skip in case of an error
				if (comp.texture == GTX::NO_TEXTURE)
					continue;

				if (comp.unique_key.ends_with("background")) {

					// Town Frame Wallpapers
					sf::IntRect bg_rect{};
					bg_rect.width = std::stoi(comp["source_w"].value());
					bg_rect.height = std::stoi(comp["source_h"].value());
					bg_rect.top = 0;
					bg_rect.left = std::stoi(comp["source_w"].value()) *
								   std::stoi(comp["source_index"].value());
					sf::Sprite image{};
					image.setTexture(
						*_system->resources->get_texture(GTX::TOWN));
					image.setTextureRect(bg_rect);
					image.setScale(std::stof(comp["scale_x"].value()),
						std::stof(comp["scale_y"].value()));
					image.setPosition(window->get_x(image, comp.x),
						window->get_y(image, comp.y));

					// Add the image to the components ready to draw
					sprites[comp.unique_key] = image;
				} else {

					// Get the texture
					sf::Sprite image;
					image.setTexture(
						_system->resources->textures[comp.texture]);

					// Scale to less than the window size if needed
					if (comp.unique_key.ends_with("banner:banner_image") ||
						comp.unique_key.ends_with("splash:splash_image") ||
						comp.unique_key.ends_with(
							"main_menu_attract:logo_image")) {
						const ImageSize size{static_cast<unsigned int>(
												 image.getLocalBounds().width),
							static_cast<unsigned int>(
								image.getLocalBounds().height)};
						const ImageSize window_size{
							window->get_window()->getSize().x,
							window->get_window()->getSize().y};
						auto scale{1.0f};
						if (size.w > window_size.w || size.h > window_size.h) {
							const auto shrink_w{
								static_cast<float>(window_size.w) /
								static_cast<float>(size.w)};
							const auto shrink_h{
								static_cast<float>(window_size.h) /
								static_cast<float>(size.h)};
							scale = std::min(shrink_w, shrink_h);
						}
						image.setScale(scale, scale);
					} else if (comp.unique_key.ends_with("wallpaper")) {

						// Handle background wallpaper tiling
						image.setTextureRect(window->size);
					}

					// Check for Offsets
					const auto off_x{std::invoke([&] {
						if (comp["offset_x"])
							return std::stoi(comp["offset_x"].value());
						else
							return 0;
					})};
					const auto off_y{std::invoke([&] {
						if (comp["offset_y"])
							return std::stoi(comp["offset_y"].value());
						else
							return 0;
					})};

					// Set the image position
					const sf::Vector2f pos(window->get_x(image, comp.x + off_x),
						window->get_y(image, comp.y + off_y));
					image.setPosition(pos);

					// Add the image to the components ready to draw
					sprites[comp.unique_key] = image;
				}

			} else if (comp.type == CPT::FRAME) {

				auto frame = std::make_shared<Frame>(
					_system->resources->get_texture(GTX::UI), comp.w, comp.h,
					comp.colour, comp.background, comp.alpha);

				// Check for Offsets
				const auto off_x{std::invoke([&] {
					if (comp["offset_x"])
						return std::stoi(comp["offset_x"].value());
					else
						return 0;
				})};
				const auto off_y{std::invoke([&] {
					if (comp["offset_y"])
						return std::stoi(comp["offset_y"].value());
					else
						return 0;
				})};

				frame->setPosition(window->get_x(frame->sprite, comp.x) + off_x,
					window->get_y(frame->sprite, comp.y) + off_y);
				frames.emplace(
					std::make_pair(comp.unique_key, std::move(frame)));
			} else if (comp.type == CPT::TEXT) {

				sf::Text text{};

				if (_bold_text)
					text.setStyle(sf::Text::Bold);
				text.setFont(_system->resources->fonts[comp.font]);
				text.setCharacterSize(comp.size);
				auto str{(*string)[comp.string_key]};
				if (_upper_text)
					std::transform(
						str.begin(), str.end(), str.begin(), ::toupper);
				text.setFillColor(sf::Color(comp.colour));
				text.setString(str);
				auto x{comp.x == -1 ? window->centre.x : comp.x};
				auto y{comp.y == -1 ? window->centre.y : comp.y};

				// Check for Offsets
				const auto off_x{std::invoke([&] {
					if (comp["offset_x"])
						return std::stoi(comp["offset_x"].value());
					else
						return 0;
				})};
				const auto off_y{std::invoke([&] {
					if (comp["offset_y"])
						return std::stoi(comp["offset_y"].value());
					else
						return 0;
				})};

				// And for Shoves
				const auto shove_x{std::invoke([&] {
					if (comp["shove_x"])
						return std::stoi(comp["shove_x"].value()) *
							   window->get_cw();
					else
						return 0u;
				})};
				const auto shove_y{std::invoke([&] {
					if (comp["shove_y"])
						return std::stoi(comp["shove_y"].value()) *
							   window->get_ch();
					else
						return 0u;
				})};

				if (comp.justification == JUS::CENTRE) {
					text.setPosition(x + off_x + shove_x, y + off_y + shove_y);
					text.setOrigin(text.getLocalBounds().width / 2.0f, 0);
				} else if (comp.justification == JUS::RIGHT) {
					text.setPosition(x + off_x + shove_x, y + off_y + shove_y);
					const sf::FloatRect bounds{text.getLocalBounds()};
					text.setPosition(comp.x - bounds.width, comp.y);
				} else
					text.setPosition(x + off_x + shove_x, y + off_y + shove_x);

				// Add the image to the components ready to draw
				texts[comp.unique_key] = text;
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

auto Sorcery::Display::display(
	std::string_view screen, std::optional<std::any> param) -> void {

	display(screen, _sprites, _texts, _frames, param);
}

auto Sorcery::Display::display_overlay() -> void {

	const sf::Vector2f pos(
		window->get_x(overlay->width, (*layout)["global:control_overlay"].x),
		window->get_y(overlay->height, (*layout)["global:control_overlay"].y));

	if (_show_overlay && overlay->valid) {
		overlay->setPosition(pos);
		window->get_window()->draw(*overlay);
	}
}

auto Sorcery::Display::display(std::string_view screen, SpriteMap &sprites,
	TextMap &texts, FrameMap &frames, std::optional<std::any> param) -> void {

	// first draw anything with wallpaper/background
	for (auto &[key, sprite] : sprites) {
		if (key.ends_with("wallpaper"))
			window->get_window()->draw(sprite);
		if (key.ends_with("background"))
			window->get_window()->draw(sprite);
	}

	// Handle the different sized window frames in the town menu
	for (auto &[key, frame] : frames) {
		if (screen == "create") {
			if (param) {
				if (const CHS stage{std::any_cast<CHS>(param.value())};
					stage == CHS::CHOOSE_METHOD ||
					stage == CHS::REVIEW_AND_CONFIRM) {
					if (key.ends_with("_frame_progress") ||
						key.ends_with("_summary_progres"))
						continue;
				}
			}
		}

		window->get_window()->draw(*frame);
	}

	// Display all other sprites, but not the background wallpaper
	for (auto &[key, sprite] : sprites) {
		if (key.ends_with("banner:banner_image") ||
			key.ends_with("splash:splash_image")) {
			if (param) {
				sprite.setColor(sf::Color(
					255, 255, 255, std::any_cast<unsigned int>(param.value())));
			}
		}

		if (key.ends_with("wallpaper"))
			continue;
		else if (key.ends_with("background"))
			continue;
		else
			window->get_window()->draw(sprite);
	}

	for (const auto &[key, text] : texts) {

		if (screen == "main_menu_attract") {
			if (param) {
				if (const MMT stage{std::any_cast<MMT>(param.value())};
					stage == MMT::ATTRACT_MENU) {
					if (key.ends_with("main_menu_attract:press_any_key") ||
						key.ends_with("main_menu_attract:subtitle_1") ||
						key.ends_with("main_menu_attract:subtitle_2") ||
						key.ends_with("main_menu_attract:copyright"))
						continue;
				}
			}
		} else if (screen == "create") {
			if (param) {
				if (const CHS stage{std::any_cast<CHS>(param.value())};
					stage == CHS::CHOOSE_METHOD ||
					stage == CHS::REVIEW_AND_CONFIRM) {
					if (key.ends_with("_frame_progress") ||
						key.ends_with("_summary_progress"))
						continue;
				}
			}
		}

		window->get_window()->draw(text);
	}
}

auto Sorcery::Display::set_input_mode(WIM input_mode) -> void {

	window->set_input_mode(input_mode);
	overlay->set_input_mode(input_mode);
}

auto Sorcery::Display::get_input_mode() const -> WIM {

	return window->get_input_mode();
}

auto Sorcery::Display::display_direction_indicator(MAD dir, bool mono = false)
	-> void {

	const auto di_layout{(*layout)["engine_base_ui:direction_indicator"]};
	const auto di_type{std::invoke([&] {
		return mono ? di_layout["mono"].value() : di_layout["not_mono"].value();
	})};
	auto di_icon{(*_icons)[di_type]};
	if (di_icon) {
		auto ind{di_icon.value()};
		ind.setOrigin(
			ind.getLocalBounds().width / 2, ind.getLocalBounds().height / 2);
		switch (dir) {
		case MAD::NORTH:
			ind.setRotation(180.0f);
			break;
		case MAD::SOUTH:
			ind.setRotation(0.0f);
			break;
		case MAD::EAST:
			ind.setRotation(270.0f);
			break;
		case MAD::WEST:
			ind.setRotation(90.0f);
			break;
		default:
			break;
		}

		const auto off_x{std::invoke([&] {
			if (di_layout["offset_x"])
				return std::stoi(di_layout["offset_x"].value());
			else
				return 0;
		})};
		const auto off_y{std::invoke([&] {
			if (di_layout["offset_y"])
				return std::stoi(di_layout["offset_y"].value());
			else
				return 0;
		})};

		sf::RectangleShape bg{sf::Vector2f(512, 512)};
		bg.setFillColor(sf::Color(0, 0, 0));
		bg.setOrigin(
			bg.getLocalBounds().width / 2, bg.getLocalBounds().height / 2);

		ind.setPosition((ind.getGlobalBounds().width / 2) + di_layout.x + off_x,
			(ind.getGlobalBounds().height / 2) + di_layout.y + off_y);
		bg.setPosition((ind.getGlobalBounds().width / 2) + di_layout.x + off_x,
			(ind.getGlobalBounds().height / 2) + di_layout.y + off_y);
		ind.setScale(di_layout.scl());
		bg.setScale(di_layout.scl());

		window->get_window()->draw(bg);
		window->get_window()->draw(ind);
	}
}

auto Sorcery::Display::display_cursor() -> void {

	auto cursor{std::invoke([&] {
		if (_disc_access)
			return window->get_disc();
		else
			return window->get_cursor();
	})};
	cursor.setPosition(window->get_cur());
	window->get_window()->draw(cursor);
}

auto Sorcery::Display::get_cur() const -> sf::Vector2f {

	return window->get_cur();
}

auto Sorcery::Display::fit_bg_movie() -> void {

	_bg_movie.fit(0, 0, window->get_window()->getSize().x,
		window->get_window()->getSize().y);
}

auto Sorcery::Display::start_bg_movie() -> void {

	if (_bg_movie.getStatus() == sfe::Stopped)
		_bg_movie.play();
}

auto Sorcery::Display::get_disc() const -> bool {

	return _disc_access;
}

auto Sorcery::Display::set_disc(const bool value) -> void {

	_disc_access = value;
}

auto Sorcery::Display::stop_bg_movie() -> void {

	if (_bg_movie.getStatus() == sfe::Playing)
		_bg_movie.stop();
}

auto Sorcery::Display::update_bg_movie() -> void {

	_bg_movie.update();
}

auto Sorcery::Display::draw_bg_movie() -> void {

	window->get_window()->draw(_bg_movie);
}
