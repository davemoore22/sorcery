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

#include "mainmenu.hpp"

// Standard Constructor
Sorcery::MainMenu::MainMenu(System *system, Display *display, Graphics *graphics, Game *game)
	: _system{system}, _display{display}, _graphics{graphics}, _game{game} {

	// Get the Window and Graphics to Display
	_window = _display->window->get_window();

	// Create the Main Menu
	_menu_stage = MainMenuType::ATTRACT_MODE;
	_main_menu = std::make_unique<Menu>(_system, _display, _graphics, _game, MenuType::MAIN);

	// Setup Custom Components
	Component any_key_c{(*_display->layout)["main_menu_attract:press_any_key"]};
	_press_any_key = std::make_unique<Text>(_system, _display, any_key_c,
		magic_enum::enum_integer<ComponentElement>(ComponentElement::COLOUR) |
			magic_enum::enum_integer<ComponentElement>(ComponentElement::FONT) |
			magic_enum::enum_integer<ComponentElement>(ComponentElement::STRING) |
			magic_enum::enum_integer<ComponentElement>(ComponentElement::SIZE) |
			magic_enum::enum_integer<ComponentElement>(ComponentElement::JUSTIFICATION));
	auto x{any_key_c.x == -1 ? _display->window->centre.x : any_key_c.x};
	auto y{any_key_c.y == -1 ? _display->window->centre.y : any_key_c.y};
	_press_any_key->setPosition(x, y);

	// Now set up attract mode data
	_attract_mode =
		std::make_unique<AttractMode>(_graphics, (*_display->layout)["main_menu_attract:attract_creatures"]);
	_attract_mode->data.clear();

	// Create the Confirmation Dialogs
	_dialog_exit =
		std::make_unique<Dialog>(_system, _display, _graphics, (*_display->layout)["main_menu_attract:dialog_exit"],
			(*_display->layout)["main_menu_attract:dialog_exit_text"], WindowDialogType::CONFIRM);
	_dialog_exit->setPosition(
		(*_display->layout)["main_menu_attract:dialog_exit"].x, (*_display->layout)["main_menu_attract:dialog_exit"].y);
	_dialog_new_game =
		std::make_unique<Dialog>(_system, _display, _graphics, (*_display->layout)["main_menu_attract:dialog_new_game"],
			(*_display->layout)["main_menu_attract:dialog_new_game_text"], WindowDialogType::CONFIRM);
	_dialog_new_game->setPosition((*_display->layout)["main_menu_attract:dialog_new_game"].x,
		(*_display->layout)["main_menu_attract:dialog_new_game"].y);

	_error = std::nullopt;
}

// Standard Destructor
Sorcery::MainMenu::~MainMenu() {

	_graphics->animation->stop_attract_threads();
	_display->stop_bg_movie();
}

auto Sorcery::MainMenu::start(MainMenuType menu_stage) -> std::optional<MenuItem> {

	// Get the Background Display Components and load them into Display module
	// storage (not local)
	_display->generate("main_menu_attract");

	// Clear the window
	_window->clear();

	_menu_stage = menu_stage;

	// Now set up attract mode data
	_attract_mode->data_temp.clear();

	// Start relevant animation worker threads
	_graphics->animation->refresh_attract();
	_graphics->animation->start_attract_ani_threads();

	// Play the background movie!
	_display->fit_bg_movie();
	_display->start_bg_movie();

	std::optional<std::vector<MenuEntry>::const_iterator> selected_option{_main_menu->items.begin()};
	if (_menu_stage == MainMenuType::ATTRACT_MODE)
		_display->set_input_mode(WindowInputMode::ATTRACT_MODE);
	else
		_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);

	// See if we have a Game to Continue;
	_set();

	// And do the main loop
	sf::Event event{};
	while (_window->isOpen()) {

		try {

			_attract_mode->data_temp = _graphics->animation->get_attract_data();
			while (_window->pollEvent(event)) {
				if (_error)
					_display->window->get_gui()->handleEvent(event);

				else {

					// If we are in normal input mode
					if (_display->get_input_mode() == WindowInputMode::ATTRACT_MODE) {

						// Check for Window Close
						if (event.type == sf::Event::Closed)
							return MenuItem::ABORT;

						// Check for any key being pressed to move onto the main
						// menu
						if (_menu_stage == MainMenuType::ATTRACT_MODE) {

							// Check for Window Close
							if (event.type == sf::Event::Closed)
								return std::nullopt;

							// Check for any key being pressed to move onto the
							// main menu
							if (_system->input->check(WindowInput::ANYTHING, event)) {

								_menu_stage = MainMenuType::ATTRACT_MENU;
								_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
							}
						}
					} else if (_display->get_input_mode() == WindowInputMode::NAVIGATE_MENU) {

						// Check for Window Close
						if (event.type == sf::Event::Closed)
							return std::nullopt;

						// Handle enabling help overlay
						if (_system->input->check(WindowInput::SHOW_CONTROLS, event)) {
							_display->show_overlay();
							continue;
						} else
							_display->hide_overlay();

						// And handle input on the main menu
						if (_system->input->check(WindowInput::UP, event))
							selected_option = _main_menu->choose_previous();
						else if (_system->input->check(WindowInput::DOWN, event))
							selected_option = _main_menu->choose_next();
						else if (_system->input->check(WindowInput::MOVE, event))
							selected_option = _main_menu->set_mouse_selected(
								static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
						else if (_system->input->check(WindowInput::CONFIRM, event)) {
							if (selected_option) {

								// We have selected something from the menu
								if (const MenuItem option_chosen{(*selected_option.value()).item};
									option_chosen == MenuItem::MM_NEW_GAME) {
									_display->set_input_mode(WindowInputMode::CONFIRM_NEW_GAME);
								} else if (option_chosen == MenuItem::MM_CONTINUE_GAME) {
									_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
									return MenuItem::MM_CONTINUE_GAME;
								} else if (option_chosen == MenuItem::MM_LICENSE) {
									_display->set_input_mode(WindowInputMode::DISPLAY_TEXT_FILE);
									return MenuItem::MM_LICENSE;
								} else if (option_chosen == MenuItem::MM_COMPENDIUM) {
									_display->set_input_mode(WindowInputMode::COMPENDIUM);
									return MenuItem::MM_COMPENDIUM;
								} else if (option_chosen == MenuItem::MM_OPTIONS) {
									_display->set_input_mode(WindowInputMode::GAME_OPTIONS);
									return MenuItem::MM_OPTIONS;
								} else if (option_chosen == MenuItem::QUIT) {
									_display->set_input_mode(WindowInputMode::CONFIRM_QUIT_GAME);
								}
							}
						} else if ((_system->input->check(WindowInput::CANCEL, event)) ||
								   ((_system->input->check(WindowInput::BACK, event)))) {
							_display->set_input_mode(WindowInputMode::CONFIRM_QUIT_GAME);
						}

					} else if (_display->get_input_mode() == WindowInputMode::CONFIRM_NEW_GAME) {

						auto dialog_input{_dialog_new_game->handle_input(event)};
						if (dialog_input) {
							if (dialog_input.value() == WindowDialogButton::CLOSE) {
								_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
								return std::nullopt;
							} else if (dialog_input.value() == WindowDialogButton::YES) {
								_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
								return MenuItem::MM_NEW_GAME;
							} else if (dialog_input.value() == WindowDialogButton::NO) {
								_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
							}
						}
					} else if (_display->get_input_mode() == WindowInputMode::CONFIRM_QUIT_GAME) {

						auto dialog_input{_dialog_exit->handle_input(event)};
						if (dialog_input) {
							if (dialog_input.value() == WindowDialogButton::CLOSE) {
								_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
								return std::nullopt;
							} else if (dialog_input.value() == WindowDialogButton::YES) {
								_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
								return MenuItem::QUIT;
							} else if (dialog_input.value() == WindowDialogButton::NO) {
								_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
							}
						}
					}
				}
			}

		} catch (std::exception &e) {

			_error = std::make_unique<Error>(_display->window->get_gui(), SystemError::UNHANDLED_EXCEPTION, e);
		}

		_window->clear();
		_display->start_bg_movie();
		_display->update_bg_movie();
		_display->draw_bg_movie();

		_draw();
		_display->display_overlay();
		_display->window->get_gui()->draw();
		_display->display_cursor();
		_window->display();
	}

	return std::nullopt;
}

auto Sorcery::MainMenu::stop() -> void {

	// Stop the background movie!
	_display->stop_bg_movie();
}

auto Sorcery::MainMenu::_set() -> void {

	(*_main_menu)[1].enabled = _game->valid;
	if ((*_main_menu)[1].enabled) {
		auto it{_main_menu->choose(1)};
		_main_menu->selected = it.value();
	}
}

auto Sorcery::MainMenu::_draw() -> void {

	// Only draw the attract mode if we have something to draw (to avoid timing
	// issues)
	if (_attract_mode->data_temp.size() > 0) {

		const auto lerp{_graphics->animation->colour_lerp};
		_display->display("main_menu_attract", _menu_stage);

		// Generate and draw the Attract Mode Graphics
		Component attract_creatures_c{(*_display->layout)["main_menu_attract:attract_creatures"]};
		_attract_mode->generate();
		_attract_mode->setScale(attract_creatures_c.scale, attract_creatures_c.scale);
		_attract_mode->set_alpha(_graphics->animation->attract_alpha);

		// Horrible - but needed since the size of the Attract Mode Graphics are
		// variable
		const sf::Vector2f attract_mode_size{
			_attract_mode->sprite.getGlobalBounds().width * _attract_mode->getScale().x,
			_attract_mode->sprite.getGlobalBounds().height * _attract_mode->getScale().y};
		const sf::Vector2f creature_pos(_display->window->centre.x - (attract_mode_size.x / 2),
			_display->window->get_y(_attract_mode->sprite, attract_creatures_c.y));
		_attract_mode->setPosition(creature_pos);
		_window->draw(*_attract_mode);

		// And either the blurb or the main menu
		if (_menu_stage == MainMenuType::ATTRACT_MODE) {
			sf::Color adjusted{_graphics->adjust_brightness(
				sf::Color((*_display->layout)["main_menu_attract:press_any_key"].colour), lerp)};
			_press_any_key->set_fill_colour(adjusted);
			_window->draw(*_press_any_key);
		} else {

			// Draw the menu
			_main_menu->generate((*_display->layout)["main_menu_attract:main_menu"]);
			const sf::Vector2f menu_pos((*_display->layout)["main_menu_attract:main_menu"].x,
				(*_display->layout)["main_menu_attract:main_menu"].y);
			_main_menu->setPosition(menu_pos);
			_window->draw(*_main_menu);
			if (_display->get_input_mode() == WindowInputMode::CONFIRM_QUIT_GAME) {
				_dialog_exit->update();
				_window->draw(*_dialog_exit);
			} else if (_display->get_input_mode() == WindowInputMode::CONFIRM_NEW_GAME) {
				_dialog_new_game->update();
				_window->draw(*_dialog_new_game);
			}
		}
	}
}
