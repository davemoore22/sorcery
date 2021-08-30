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

#include "engine.hpp"

// Standard Constructor
Sorcery::Engine::Engine(
	System *system, Display *display, Graphics *graphics, Game *game)
	: _system{system}, _display{display}, _graphics{graphics}, _game{game} {

	// Get the Window and Graphics to Display
	_window = _display->window->get_window();

	// Setup Custom Components
	_camp_menu = std::make_unique<Menu>(
		_system, _display, _graphics, _game, MenuType::CAMP);

	// Modules
	_status_bar =
		std::make_unique<StatusBar>(_system, _display, _graphics, _game);
}

// Standard Destructor
Sorcery::Engine::~Engine() {}

auto Sorcery::Engine::start() -> void {

	_display->generate("engine_base_ui");

	// Clear the window
	_window->clear();

	// Refresh the Party characters
	_status_bar->refresh();

	// Generate the Custom Components
	const Component status_bar_c{
		(*_display->layout)["engine_base_ui:status_bar"]};
	_status_bar->setPosition(
		_display->window->get_x(_status_bar->sprite, status_bar_c.x),
		_display->window->get_y(_status_bar->sprite, status_bar_c.y));

	// Start in camp as is tradition
	_in_camp = true;
	_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
	std::optional<std::vector<MenuEntry>::const_iterator> camp_option{
		_camp_menu->items.begin()};

	sf::Event event{};
	while (_window->isOpen()) {
		while (_window->pollEvent(event)) {

			// Check for Window Close
			if (event.type == sf::Event::Closed)
				_window->close();

			// Handle enabling help overlay
			if (_system->input->check(WindowInput::SHOW_CONTROLS, event)) {
				_display->show_overlay();
				continue;
			} else
				_display->hide_overlay();

			if (_in_camp) {

				if (_system->input->check(WindowInput::CANCEL, event))
					_in_camp = false;

				if (_system->input->check(WindowInput::BACK, event))
					_in_camp = false;

				if (_system->input->check(WindowInput::UP, event))
					camp_option = _camp_menu->choose_previous();
				else if (_system->input->check(WindowInput::DOWN, event))
					camp_option = _camp_menu->choose_next();
				else if (_system->input->check(WindowInput::MOVE, event))
					camp_option = _camp_menu->set_mouse_selected(
						static_cast<sf::Vector2f>(
							sf::Mouse::getPosition(*_window)));
				else if (_system->input->check(WindowInput::CONFIRM, event)) {

					// We have selected something from the menu
					if (camp_option) {

						if (const MenuItem option_chosen{
								(*camp_option.value()).item};
							option_chosen == MenuItem::CP_LEAVE) {

							_game->save_game();
							_status_bar->refresh();
							_in_camp = false;
							_display->generate("engine_base_ui");
							_display->set_input_mode(WindowInputMode::IN_GAME);
							continue;
						} else if (option_chosen == MenuItem::CP_SAVE) {
							_game->save_game();
							return;
						}
					}
				}
			} else {

				if (_display->get_input_mode() == WindowInputMode::IN_GAME) {

					if (_system->input->check(WindowInput::CANCEL, event))
						_in_camp = true;

					if (_system->input->check(WindowInput::BACK, event))
						_in_camp = true;
				}
			}

			_window->clear();

			_draw();
			_window->display();
		}
	}
}

auto Sorcery::Engine::stop() -> void {}

auto Sorcery::Engine::_draw() -> void {

	// Custom Components
	_display->display("engine_base_ui");
	_window->draw(*_status_bar);

	// And the Menu
	if (_in_camp) {
		_camp_menu->generate((*_display->layout)["engine_base_ui:menu"]);
		const sf::Vector2f menu_pos(
			(*_display->layout)["engine_base_ui:menu"].x,
			(*_display->layout)["engine_base_ui:menu"].y);
		_camp_menu->setPosition(menu_pos);
		_window->draw(*_camp_menu);
	}

	// Always draw the following
	_display->display_overlay();
	_display->display_cursor();
}
