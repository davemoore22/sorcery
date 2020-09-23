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

#include "mainmenu.hpp"

// Standard Constructor
Sorcery::MainMenu::MainMenu (System& system, Display& display, Graphics& graphics):  _system {system},
	_display {display}, _graphics {graphics} {

	// Get the Window and Graphics to Display
	_window = _display.window->get_window();

	// Load the Fonts
	_mono_system_font.loadFromFile(_system.files->get_path_as_string(MONO_FONT_FILE));
	_proportional_system_font.loadFromFile(_system.files->get_path_as_string(PROPORTIONAL_FONT_FILE));

	// Load the Background Movie
	_background_movie.openFromFile(_system.files->get_path_as_string(MENU_VIDEO));

	// Set up the Attract Mode data
	_attract_mode_data.clear();
	_creature_sprite_width = 108;
	_creature_sprite_height = 108;
	_creature_sprite_spacing = 8;
	_creature_sprite_scaling =- 2.5f;

	// Clear the window
	_window->clear();
	_menu_status = MainMenuType::ATTRACT_MODE;
}

// Standard Destructor
Sorcery::MainMenu::~MainMenu() {
	_graphics.animation->stop_attract_mode_animation_threads();
	_background_movie.stop();
}

auto Sorcery::MainMenu::start() -> void {

	// Get the Logo and scale it appropriately
	Component logo_c {(*_display.layout)["main_menu_attract:logo_image"]};
	_logo.setTexture(_system.resources->textures[LOGO_TEXTURE]);
	ImageSize logo_size {static_cast<unsigned int>(_logo.getLocalBounds().width),
		static_cast<unsigned int>(_logo.getLocalBounds().height)};
	const ImageSize window_size {_window->getSize().x, _window->getSize().y};
	float scale_ratio_needed {1.0f};
	if ((logo_size.w > window_size.w) || (logo_size.h > window_size.h)) {
		float shrink_width_needed {static_cast<float>(window_size.w) / static_cast<float>(logo_size.w)};
		float shrink_height_needed {static_cast<float>(window_size.h) / static_cast<float>(logo_size.h)};
		scale_ratio_needed = std::min(shrink_width_needed, shrink_height_needed);
	}
	_logo.setScale(scale_ratio_needed, scale_ratio_needed);
	const ImageSize resized {static_cast<unsigned int>(_logo.getGlobalBounds().width),
		static_cast<unsigned int>(_logo.getGlobalBounds().height)};
	_logo.setPosition(_display.window->get_x(_logo, logo_c.x), _display.window->get_y(_logo, logo_c.y));
		//_display.window->centre.x - (resized.w / 2), (resized.h / 2) - 50);

	// Get Constituent Parts for the Main Menu
	Component top_frame_c {(*_display.layout)["main_menu_attract:top_gui_frame"]};
	Component bottom_frame_c {(*_display.layout)["main_menu_attract:bottom_gui_frame"]};
	Component attract_creatures_c {(*_display.layout)["main_menu_attract:attract_creatures"]};

	// Generate the frames
	sf::RenderTexture top_frame_rt;
	sf::Texture top_frame_t;
	sf::Sprite top_frame {_display.window->get_gui_frame(top_frame_rt, top_frame_t, top_frame_c.w, top_frame_c.h)};
	const sf::Vector2f top_pos(_display.window->get_x(top_frame, top_frame_c.x), _display.window->get_y(top_frame,
		top_frame_c.y));
	top_frame.setPosition(top_pos);
	sf::RenderTexture bottom_frame_rt;
	sf::Texture bottom_frame_t;
	sf::Sprite bottom_frame {_display.window->get_gui_frame(bottom_frame_rt, bottom_frame_t, bottom_frame_c.w,
		bottom_frame_c.h)};
	const sf::Vector2f bottom_pos(_display.window->get_x(bottom_frame, bottom_frame_c.x),
		_display.window->get_y(bottom_frame, bottom_frame_c.y));
	bottom_frame.setPosition(bottom_pos);

	// Get the Cursor
	_cursor = _display.window->get_cursor();

	// Scale the Movie
	_background_movie.fit(0, 0, _window->getSize().x, _window->getSize().y);

	// Now set up attract mode data
	std::vector<unsigned int> attract_mode_data;
	attract_mode_data.clear();

	// Start relevant animation worker threads
	_graphics.animation->force_refresh_attract_mode();
	_graphics.animation->start_attract_mode_animation_threads();

	_background_movie.play();
	while (_window->isOpen()) {

		attract_mode_data = _graphics.animation->get_attract_mode_data();
		sf::Event event;
		while (_window->pollEvent(event)) {

			if (event.type == sf::Event::Closed)
				_window->close();
		}

		if (_background_movie.getStatus() == sfe::Stopped) {
			_background_movie.play();
		}
		_background_movie.update();

		_window->clear();
		_window->draw(_background_movie);

		_draw(MainMenuType::ATTRACT_MODE, attract_mode_data, attract_creatures_c, top_frame, bottom_frame);
		_window->display();
	}
}

auto Sorcery::MainMenu::_draw(MainMenuType stage, std::vector<unsigned int> attract_mode_data,
	Component& attract_creatures_c, sf::Sprite &top_frame, sf::Sprite &bottom_frame) -> void {

	// Only draw the attract mode if we have something to draw (to avoid timing issues)
	if (attract_mode_data.size() > 0) {

		sf::Sprite creatures {_get_attract_mode(attract_mode_data)};
		creatures.setColor(sf::Color(255, 255, 255, _graphics.animation->attract_mode_alpha));
		creatures.setScale(attract_creatures_c.scale, attract_creatures_c.scale);
		const sf::Vector2f creature_pos(_display.window->get_x(creatures, attract_creatures_c.x),
			_display.window->get_y(creatures, attract_creatures_c.y));
		creatures.setPosition(creature_pos);

		_window->draw(top_frame);
		_window->draw(bottom_frame);
		_window->draw(creatures);
		_window->draw(_logo);

		// Draw Attract Mode Text
		double lerp = _graphics.animation->colour_lerp;
		sf::Text text;
		_display.window->draw_centered_text(text, (*_display.layout)["main_menu_attract:title"]);
		_display.window->draw_centered_text(text, (*_display.layout)["main_menu_attract:subtitle_1"]);
		_display.window->draw_centered_text(text, (*_display.layout)["main_menu_attract:subtitle_2"]);
		_display.window->draw_centered_text(text, (*_display.layout)["main_menu_attract:copyright"]);

		if (_menu_status == MainMenuType::ATTRACT_MODE)
			_display.window->draw_centered_text(text, (*_display.layout)["main_menu_attract:press_any_key"], lerp);
	}

	// Always draw the following
	_cursor.setPosition(static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
}

// We generate the attract mode graphic in the main thread, though we generate the IDs in the animation threads
// https://en.sfml-dev.org/forums/index.php?topic=18672.0
auto Sorcery::MainMenu::_get_attract_mode(std::vector<unsigned int> attract_mode_data) -> sf::Sprite {

	// Only regenerate if we have a change
	if (_attract_mode_data != attract_mode_data) {
		_attract_mode_data = attract_mode_data;
		const unsigned int number_to_display {attract_mode_data.size()};
		const sf::Vector2f texture_size(_creature_sprite_width * number_to_display + (_creature_sprite_spacing *
			(number_to_display - 1)), _creature_sprite_height);
		sf::RenderTexture attract_texture;
		attract_texture.create(texture_size.x, texture_size.y);
		attract_texture.setSmooth(true);
		attract_texture.clear();

		// Work out their Indexes and Positions
		unsigned int sprite_index {};
		unsigned int sprite_x {0};
		for (auto i: _attract_mode_data) {
			sf::Sprite sprite = _get_creature_gfx(i, true);
			sprite.setPosition(sprite_x, 0);
			attract_texture.draw(sprite);
			sprite_x += (_creature_sprite_width + _creature_sprite_spacing);
		}

		attract_texture.display();
		_attract_mode_texture = attract_texture.getTexture();
		sf::Sprite attract_sprite(_attract_mode_texture);
		return attract_sprite;
	} else {
		sf::Sprite attract_sprite(_attract_mode_texture);
		return attract_sprite;
	}
}

auto Sorcery::MainMenu::_get_creature_gfx(const int creature_id, const bool known) -> sf::Sprite {
	sf::IntRect creature_rect {};
	sf::Sprite creature(_system.resources->textures[CREATURES_TEXTURE]);
	creature_rect.left = (creature_id - 1) * _creature_sprite_width;
	creature_rect.width = _creature_sprite_width;
	creature_rect.top = known ? 0 : _creature_sprite_height;
	creature_rect.height = _creature_sprite_height;
	creature.setTextureRect(creature_rect);
	return creature;
}
