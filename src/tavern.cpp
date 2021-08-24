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

#include "tavern.hpp"

// Standard Constructor
Sorcery::Tavern::Tavern(
	System *system, Display *display, Graphics *graphics, Game *game)
	: _system{system}, _display{display}, _graphics{graphics}, _game{game} {

	// Get the Window and Graphics to Display
	_window = _display->window->get_window();

	// Setup Custom Components
	_menu = std::make_unique<Menu>(
		_system, _display, _graphics, _game, MenuType::TAVERN);

	// Modules
	_status_bar =
		std::make_unique<StatusBar>(_system, _display, _graphics, _game);

	_stage = TavernStage::MENU;
}

// Standard Destructor
Sorcery::Tavern::~Tavern() {}

// Visit the Tavern
auto Sorcery::Tavern::start() -> std::optional<MenuItem> {

	// Do the Menus here when we has access to the game characters
	_add.reset();
	_remove.reset();
	_inspect.reset();
	_add = std::make_unique<Menu>(
		_system, _display, _graphics, _game, MenuType::AVAILABLE_CHARACTERS);
	_inspect = std::make_unique<Menu>(
		_system, _display, _graphics, _game, MenuType::CHARACTER_ROSTER);
	_remove = std::make_unique<Menu>(
		_system, _display, _graphics, _game, MenuType::PARTY_CHARACTERS);

	switch (_stage) {
	case TavernStage::MENU:
		_screen_key = "tavern";
		break;
	case TavernStage::INSPECT:
		_screen_key = "tavern_inspect";
		break;
	case TavernStage::ADD:
		_screen_key = "tavern_add";
		break;
	case TavernStage::REMOVE:
		_screen_key = "tavern_remove";
		break;
	default:
		break;
	}

	_display->generate(_screen_key);

	// Clear the window
	_window->clear();

	// Generate the Custom Components
	const Component status_bar_c{(*_display->layout)["status_bar:status_bar"]};
	_status_bar->setPosition(
		_display->window->get_x(_status_bar->sprite, status_bar_c.x),
		_display->window->get_y(_status_bar->sprite, status_bar_c.y));

	// Play the background movie!
	_display->fit_bg_movie();
	_display->start_bg_movie();

	// And do the main loop
	_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
	std::optional<std::vector<MenuEntry>::const_iterator> option{
		_menu->items.begin()};
	sf::Event event{};
	while (_window->isOpen()) {
		while (_window->pollEvent(event)) {

			_update_menus();

			// Check for Window Close
			if (event.type == sf::Event::Closed)
				_window->close();

			// Handle enabling help overlay
			if (_system->input->check(WindowInput::SHOW_CONTROLS, event)) {
				_display->show_overlay();
				continue;
			} else
				_display->hide_overlay();

			if (_stage == TavernStage::MENU) {

				// If we are in normal input mode
				if (_display->get_input_mode() ==
					WindowInputMode::NAVIGATE_MENU) {

					if (_system->input->check(WindowInput::CANCEL, event))
						return std::nullopt;

					if (_system->input->check(WindowInput::BACK, event))
						return std::nullopt;

					// And handle input on the main menu
					if (_system->input->check(WindowInput::UP, event))
						option = _menu->choose_previous();
					else if (_system->input->check(WindowInput::DOWN, event))
						option = _menu->choose_next();
					else if (_system->input->check(WindowInput::MOVE, event))
						option =
							_menu->set_mouse_selected(static_cast<sf::Vector2f>(
								sf::Mouse::getPosition(*_window)));
					else if (_system->input->check(
								 WindowInput::CONFIRM, event)) {

						// We have selected something from the menu
						if (option) {
							if (const MenuItem option_chosen{
									(*option.value()).item};
								option_chosen == MenuItem::TA_CASTLE) {
								return MenuItem::TA_CASTLE;
							} else if (option_chosen ==
									   MenuItem::TA_ADD_TO_PARTY) {

								_add->reload();
								_stage = TavernStage::ADD;
								_screen_key = "tavern_add";
								_display->generate(_screen_key);
								continue;
							}

							/* else if (option_chosen ==
							MenuItem::ET_LEAVE_GAME) { _display->set_input_mode(
									WindowInputMode::CONFIRM_LEAVE_GAME);
								_yes_or_no = WindowConfirm::NO;
							} else if (option_chosen == MenuItem::ET_MAZE) {
								return MenuItem::ET_MAZE;
							} else if (option_chosen == MenuItem::ET_TRAIN) {
								_training->start();
								_training->stop();
								_display->generate("edge_of_town");
								_display->set_input_mode(
									WindowInputMode::NAVIGATE_MENU);
							} */
						}
					}
				}

			} else if (_stage == TavernStage::ADD) {

				if (_system->input->check(WindowInput::CANCEL, event)) {
					_stage = TavernStage::MENU;
					_screen_key = "tavern";
					_display->generate(_screen_key);
					continue;
				}

				if (_system->input->check(WindowInput::BACK, event)) {
					_stage = TavernStage::MENU;
					_screen_key = "tavern";
					_display->generate(_screen_key);
					continue;
				}

				// And handle input on the main menu
				if (_system->input->check(WindowInput::UP, event))
					option = _add->choose_previous();
				else if (_system->input->check(WindowInput::DOWN, event))
					option = _add->choose_next();
				else if (_system->input->check(WindowInput::MOVE, event))
					option = _add->set_mouse_selected(static_cast<sf::Vector2f>(
						sf::Mouse::getPosition(*_window)));
				else if (_system->input->check(WindowInput::CONFIRM, event)) {

					// We have selected something from the menu
					if (option) {
						if (const MenuItem option_chosen{
								(*option.value()).item};
							option_chosen == MenuItem::CA_TAVERN) {

							_stage = TavernStage::MENU;
							_screen_key = "tavern";
							_display->generate(_screen_key);
							continue;
						} else {

							// Add a character
							const auto character_chosen{
								(*option.value()).index};
							_game->state->add_character_by_id(character_chosen);
							_game->save_game();
							_status_bar->refresh();
							_stage = TavernStage::MENU;
							_screen_key = "tavern";
							_display->generate(_screen_key);
						}
					}
				}
			}
		}

		_window->clear();

		// Update Background Movie
		_display->start_bg_movie();
		_display->update_bg_movie();
		_display->draw_bg_movie();

		_draw();
		_window->display();
	}

	return std::nullopt;
}

auto Sorcery::Tavern::stop() -> void {

	// Stop the background movie!
	_display->stop_bg_movie();
}

auto Sorcery::Tavern::_update_menus() -> void {

	if (_game->state->get_party_characters().size() == MAX_PARTY_SIZE) {
		if ((*_menu)[0].enabled)
			(*_menu)[0].enabled = false;
	} else if (_game->state->get_party_characters().size() == 0) {
		if ((*_menu)[1].enabled)
			(*_menu)[1].enabled = false;
	} else {
		if (!(*_menu)[0].enabled)
			(*_menu)[0].enabled = true;
		if (!(*_menu)[1].enabled)
			(*_menu)[1].enabled = true;
	}
}

auto Sorcery::Tavern::_draw() -> void {

	// Custom Components
	_display->display(_screen_key);
	_window->draw(*_status_bar);

	// And the Menu
	if (_stage == TavernStage::MENU) {
		_menu->generate((*_display->layout)["tavern:menu"]);
		const sf::Vector2f menu_pos((*_display->layout)["tavern:menu"].x,
			(*_display->layout)["tavern:menu"].y);
		_menu->setPosition(menu_pos);
		_window->draw(*_menu);
	} else if (_stage == TavernStage::ADD) {
		_add->generate((*_display->layout)["tavern_add:menu"]);
		const sf::Vector2f menu_pos((*_display->layout)["tavern_add:menu"].x,
			(*_display->layout)["tavern_add:menu"].y);
		_add->setPosition(menu_pos);
		_window->draw(*_add);
	}

	// Always draw the following
	_display->display_overlay();
	_display->display_cursor();
}
