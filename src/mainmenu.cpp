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

	// Create the Main Menu
	_menu_stage = MainMenuType::ATTRACT_MODE;
	_main_menu = std::make_shared<Menu>(_system, _display, _graphics, MenuType::MAIN);

	// Create the Confirmation Exit Dialog
	_confirm_exit = std::make_shared<Confirm>(_system, _display, _graphics,
		(*_display.layout)["main_menu_attract:confirm_exit_gui_frame"],
		(*_display.layout)["main_menu_attract:confirm_exit_game"]);

	// Setup Custom Components
	_press_any_key  = sf::Text();

	// Now set up attract mode data
	_attract_mode = std::make_shared<AttractMode>(_system.resources->textures[CREATURES_TEXTURE],
		(*_display.layout)["main_menu_attract:attract_creatures"]);
	_attract_mode->data.clear();

	// Get the Display Components
	_display.generate_components("main_menu_attract");
}

// Standard Destructor
Sorcery::MainMenu::~MainMenu() {
	_graphics.animation->stop_attract_mode_animation_threads();
	_display.stop_background_movie();
}

auto Sorcery::MainMenu::start(MainMenuType menu_stage) -> std::optional<MenuItem> {

	// Clear the window7
	_window->clear();

	_menu_stage = menu_stage;

	// Now set up attract mode data
	_attract_mode->data_temp.clear();

	// Start relevant animation worker threads
	_graphics.animation->force_refresh_attract_mode();
	_graphics.animation->start_attract_mode_animation_threads();

	// Play the background movie!
	_display.start_background_movie();

	std::optional<std::vector<MenuEntry>::const_iterator> selected_option {_main_menu->items.begin()};
	_display.window->input_mode = WindowInputMode::NORMAL;

	// And do the main loop
	sf::Event event {};
	while (_window->isOpen()) {

		_attract_mode->data_temp = _graphics.animation->get_attract_mode_data();
		while (_window->pollEvent(event)) {

			// If we are in normal input mode
			if (_display.window->input_mode == WindowInputMode::NORMAL) {

				// Check for Window Close
				if (event.type == sf::Event::Closed)
					return std::nullopt;

				// Check for any key being pressed to move onto the main menu
				if (_menu_stage == MainMenuType::ATTRACT_MODE) {
					if (_system.input->check_for_event(WindowInput::ANYTHING, event))
						_menu_stage = MainMenuType::ATTRACT_MENU;
				} else if (_menu_stage == MainMenuType::ATTRACT_MENU) {

					// And handle input on the main menu
					if (_system.input->check_for_event(WindowInput::UP, event))
						selected_option = _main_menu->choose_previous();
					else if (_system.input->check_for_event(WindowInput::DOWN, event))
						selected_option = _main_menu->choose_next();
					else if (_system.input->check_for_event(WindowInput::MOVE, event))
						selected_option =
							_main_menu->set_mouse_selected(static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
					else if (_system.input->check_for_event(WindowInput::CONFIRM, event)) {
						if (selected_option) {

							// We have selected something from the menu
							const MenuItem option_chosen {(*selected_option.value()).item};

							if (option_chosen == MenuItem::MM_NEW_GAME) {
								_display.window->input_mode = WindowInputMode::CASTLE;
								return MenuItem::MM_NEW_GAME;
							}

							if (option_chosen == MenuItem::MM_LICENSE) {
								_display.window->input_mode = WindowInputMode::DISPLAY_TEXT_FILE;
								return MenuItem::MM_LICENSE;
							}

							if (option_chosen == MenuItem::MM_COMPENDIUM) {
								_display.window->input_mode = WindowInputMode::COMPENDIUM;
								return MenuItem::MM_COMPENDIUM;
							}

							if (option_chosen == MenuItem::MM_OPTIONS) {
								_display.window->input_mode = WindowInputMode::GAME_OPTIONS;
								return MenuItem::MM_OPTIONS;
							}

							if (option_chosen == MenuItem::QUIT) {
								_display.window->input_mode = WindowInputMode::CONFIRM;
								_yes_or_no = WindowConfirm::NO;
							}
						}
					} else if (_system.input->check_for_event(WindowInput::CANCEL, event)) {
						_display.window->input_mode = WindowInputMode::CONFIRM;
						_yes_or_no = WindowConfirm::NO;
					}
				}
			} else if (_display.window->input_mode == WindowInputMode::CONFIRM) {

				// Check for Window Close
				if (event.type == sf::Event::Closed)
					return std::nullopt;

				// All we can do is select Y or N
				if (_system.input->check_for_event(WindowInput::LEFT, event))
					_confirm_exit->toggle_highlighted();
				else if (_system.input->check_for_event(WindowInput::RIGHT, event))
					_confirm_exit->toggle_highlighted();
				else if (_system.input->check_for_event(WindowInput::YES, event))
					_confirm_exit->currently_highlighted = WindowConfirm::YES;
				else if (_system.input->check_for_event(WindowInput::NO, event))
					_confirm_exit->currently_highlighted = WindowConfirm::NO;
				else if (_system.input->check_for_event(WindowInput::CANCEL, event))
					_display.window->input_mode = WindowInputMode::NORMAL;
				else if (_system.input->check_for_event(WindowInput::MOVE, event))
					_confirm_exit->check_for_mouse_move(static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
				else if (_system.input->check_for_event(WindowInput::CONFIRM, event)) {
					std::optional<WindowConfirm> option_chosen =
						_confirm_exit->check_if_option_selected(static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));

					// Mouse click only
					if (option_chosen) {
						if (option_chosen.value() == WindowConfirm::YES)
							_window->close();
						if (option_chosen.value() == WindowConfirm::NO)
							_display.window->input_mode = WindowInputMode::NORMAL;
					} else {

							// Button/Keyboard
							if (_confirm_exit->currently_highlighted == WindowConfirm::YES)
								_window->close();
							else if (_confirm_exit->currently_highlighted == WindowConfirm::NO)
								_display.window->input_mode = WindowInputMode::NORMAL;
					}
				}
			}
		}

		_window->clear();
		_display.start_background_movie();
		_display.update_background_movie();
		_display.draw_background_movie();

		_draw();
		_window->display();
	}

	return std::nullopt;
}

auto  Sorcery::MainMenu::stop() -> void {

	// Stop the background movie!
	_display.stop_background_movie();
}

auto Sorcery::MainMenu::_draw() -> void {

	// Only draw the attract mode if we have something to draw (to avoid timing issues)
	if (_attract_mode->data_temp.size() > 0) {

		double lerp = _graphics.animation->colour_lerp;
		_display.display_components("main_menu_attract", _menu_stage);

		// Generate and draw the Attract Mode Graphics
		Component attract_creatures_c {(*_display.layout)["main_menu_attract:attract_creatures"]};
		_attract_mode->generate();
		_attract_mode->setScale(attract_creatures_c.scale, attract_creatures_c.scale);
		_attract_mode->set_alpha(_graphics.animation->attract_mode_alpha);

		// Horrible - but needed since the size of the Attract Mode Graphics are variable
		sf::Vector2u attract_mode_size {_attract_mode->sprite.getGlobalBounds().width * _attract_mode->getScale().x,
			_attract_mode->sprite.getGlobalBounds().height * _attract_mode->getScale().y};
		const sf::Vector2f creature_pos(_display.window->centre.x - (attract_mode_size.x / 2),
			_display.window->get_y(_attract_mode->sprite, attract_creatures_c.y));
		_attract_mode->setPosition(creature_pos);
		_window->draw(*_attract_mode);

		// And either the blurb or the main menu
		if (_menu_stage == MainMenuType::ATTRACT_MODE) {
			_display.window->draw_text(_press_any_key, (*_display.layout)["main_menu_attract:press_any_key"], lerp);
		} else {

			// Draw rhe menu
			_main_menu->generate((*_display.layout)["main_menu_attract:main_menu"], lerp);
			const sf::Vector2f menu_pos((*_display.layout)["main_menu_attract:main_menu"].x,
				(*_display.layout)["main_menu_attract:main_menu"].y);
			_main_menu->setPosition(menu_pos);
			_window->draw(*_main_menu);
			if (_display.window->input_mode == WindowInputMode::CONFIRM) {
				_confirm_exit->draw(lerp);
			}
		}
	}

	// Always draw the following
	_display.display_cursor();
}