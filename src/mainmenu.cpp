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
	_menu_stage = MainMenuType::ATTRACT_MODE;
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
	sf::Sprite top_frame {_display.window->get_gui_frame(top_frame_rt, top_frame_t, top_frame_c.w, top_frame_c.h,
		top_frame_c.alpha)};
	const sf::Vector2f top_pos(_display.window->get_x(top_frame, top_frame_c.x), _display.window->get_y(top_frame,
		top_frame_c.y));
	top_frame.setPosition(top_pos);
	sf::RenderTexture bottom_frame_rt;
	sf::Texture bottom_frame_t;
	sf::Sprite bottom_frame {_display.window->get_gui_frame(bottom_frame_rt, bottom_frame_t, bottom_frame_c.w,
		bottom_frame_c.h, bottom_frame_c.alpha)};
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

	// Create the Main Menu
	_main_menu = std::make_shared<Menu>( _system, _display, _graphics, MenuType::MAIN);
	_menu_stage = MainMenuType::ATTRACT_MODE;

	// Start relevant animation worker threads
	_graphics.animation->force_refresh_attract_mode();
	_graphics.animation->start_attract_mode_animation_threads();

	// Play the background movie!
	_background_movie.play();

	std::optional<std::vector<MenuEntry>::const_iterator> selected_option {std::nullopt};
	_display.window->input_mode = WindowInputMode::NORMAL;

	// And do the main loop
	while (_window->isOpen()) {

		attract_mode_data = _graphics.animation->get_attract_mode_data();
		sf::Event event;
		while (_window->pollEvent(event)) {

			// If we are in normal input mode
			if (_display.window->input_mode == WindowInputMode::NORMAL) {

				// Check for Window Close
				if (event.type == sf::Event::Closed)
					_window->close();

				// Check for any key being pressed to move onto the main menu
				if (_menu_stage == MainMenuType::ATTRACT_MODE) {
					if ((event.type == sf::Event::KeyPressed) || (event.type == sf::Event::MouseButtonPressed))
						_menu_stage = MainMenuType::ATTRACT_MENU;
				} else if (_menu_stage == MainMenuType::ATTRACT_MENU) {

					// And handle input on the main menu
					if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Up))
						selected_option = _main_menu->choose_previous();
					if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Down))
						selected_option = _main_menu->choose_next();
					if (event.type == sf::Event::MouseMoved)
						selected_option =
							_main_menu->set_mouse_selected(static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
					if ((event.type == sf::Event::MouseButtonReleased) || ((event.type == sf::Event::KeyPressed) &&
						((event.key.code == sf::Keyboard::Space) || (event.key.code == sf::Keyboard::Enter)))) {
						if (selected_option) {
							int option_chosen = std::get<static_cast<int>(MenuField::INDEX)>(*selected_option.value());

							// We have selected something from the menu
							if (option_chosen == 5) {
								_display.window->input_mode = WindowInputMode::CONFIRM_Y_OR_N;
								_yes_or_no = WindowConfirm::NO;
							}
						}
					}
				}
			} else if (_display.window->input_mode == WindowInputMode::CONFIRM_Y_OR_N) {

				// Check for Window Close
				if (event.type == sf::Event::Closed)
					_window->close();

				// All we can do is select Y or N
				if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Left)) {
					if (_yes_or_no == WindowConfirm::YES)
						_yes_or_no = WindowConfirm::NO;
					else if (_yes_or_no == WindowConfirm::NO)
						_yes_or_no = WindowConfirm::YES;
				}

				if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Right)) {
					if (_yes_or_no == WindowConfirm::YES)
						_yes_or_no = WindowConfirm::NO;
					else if (_yes_or_no == WindowConfirm::NO)
						_yes_or_no = WindowConfirm::YES;
				}

				if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Y))
					_yes_or_no = WindowConfirm::YES;

				if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::N))
					_yes_or_no = WindowConfirm::NO;

				// Handle mouse over too and mouse click/enter/space
			}
		}

		if (_background_movie.getStatus() == sfe::Stopped) {
			_background_movie.play();
		}
		_background_movie.update();

		_window->clear();
		_window->draw(_background_movie);

		_draw(attract_mode_data, attract_creatures_c, top_frame, bottom_frame);
		_window->display();
	}
}

auto Sorcery::MainMenu::_draw(std::vector<unsigned int> attract_mode_data,
	Component& attract_creatures_c, sf::Sprite &top_frame, sf::Sprite &bottom_frame) -> void {

	// Only draw the attract mode if we have something to draw (to avoid timing issues)
	if (attract_mode_data.size() > 0) {

		sf::Sprite creatures {_get_attract_mode(attract_mode_data)};
		//creatures.setColor(sf::Color(0, 0, 0, attract_creatures_c.alpha));
		creatures.setColor(sf::Color(255, 255, 255, _graphics.animation->attract_mode_alpha));
		creatures.setScale(attract_creatures_c.scale, attract_creatures_c.scale);
		const sf::Vector2f creature_pos(_display.window->get_x(creatures, attract_creatures_c.x),
			_display.window->get_y(creatures, attract_creatures_c.y));
		creatures.setPosition(creature_pos);

		_window->draw(top_frame);
		_window->draw(bottom_frame);
		_window->draw(creatures, sf::BlendAlpha);
		_window->draw(_logo);

		// Draw Attract Mode Text
		double lerp = _graphics.animation->colour_lerp;
		sf::Text text;
		_display.window->draw_centered_text(text, (*_display.layout)["main_menu_attract:title"]);

		if (_menu_stage == MainMenuType::ATTRACT_MODE) {
			_display.window->draw_centered_text(text, (*_display.layout)["main_menu_attract:press_any_key"], lerp);
			_display.window->draw_centered_text(text, (*_display.layout)["main_menu_attract:subtitle_1"]);
			_display.window->draw_centered_text(text, (*_display.layout)["main_menu_attract:subtitle_2"]);
			_display.window->draw_centered_text(text, (*_display.layout)["main_menu_attract:copyright"]);
		} else {
			_display.window->draw_centered_menu(_main_menu->items, _main_menu->bounds, _main_menu->selected,
				(*_display.layout)["main_menu_attract:main_menu"], lerp);
			if (_display.window->input_mode == WindowInputMode::CONFIRM_Y_OR_N) {
				_display.window->draw_confirm((*_display.layout)["main_menu_attract:confirm_exit_gui_frame"],
				(*_display.layout)["main_menu_attract:confirm_exit_game"],
					static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)), _yes_or_no, lerp);
			}
		}
	}

	// Always draw the following
	_cursor.setPosition(static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
	_window->draw(_cursor);
}

// We generate the attract mode graphic in the main thread, though we generate the IDs in the animation threads
// https://en.sfml-dev.org/forums/index.php?topic=18672.0
auto Sorcery::MainMenu::_get_attract_mode(std::vector<unsigned int> attract_mode_data) -> sf::Sprite {

	// Only regenerate if we have a change
	if (_attract_mode_data != attract_mode_data) {
		_attract_mode_data = attract_mode_data;
		const unsigned int number_to_display {static_cast<unsigned int>(attract_mode_data.size())};
		const sf::Vector2f texture_size(_creature_sprite_width * number_to_display + (_creature_sprite_spacing *
			(number_to_display - 1)), _creature_sprite_height);
		sf::RenderTexture attract_texture;
		attract_texture.create(texture_size.x, texture_size.y);
		attract_texture.setSmooth(true);
		attract_texture.clear();

		// Work out their Indexes and Positions
		unsigned int sprite_x {0};
		for (auto i: _attract_mode_data) {
			sf::Sprite sprite = _get_creature_gfx(i, true);
			sprite.setPosition(sprite_x, 0);
			attract_texture.draw(sprite, sf::BlendAlpha);
			sprite_x += (_creature_sprite_width + _creature_sprite_spacing);
		}

		attract_texture.display();
		_attract_mode_texture = attract_texture.getTexture();
		sf::Sprite attract_sprite(_attract_mode_texture);
		attract_sprite.setColor(sf::Color(0, 0, 0, 175));
		//attract_sprite.setColor(sf::Color(0, 0, 0, 0));
		return attract_sprite;
	} else {
		sf::Sprite attract_sprite(_attract_mode_texture);
		attract_sprite.setColor(sf::Color(0, 0, 0, 175));
		//attract_sprite.setColor(sf::Color(0, 0, 0, 0));
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
