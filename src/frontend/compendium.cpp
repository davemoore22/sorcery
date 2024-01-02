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
// the resulting work.

#include "frontend/compendium.hpp"

// Standard Constructor
Sorcery::Compendium::Compendium(System *system, Display *display, Graphics *graphics, Game *game)
	: _system{system}, _display{display}, _graphics{graphics}, _game{game} {
}

// Standard Destructor
Sorcery::Compendium::~Compendium() {

	_display->stop_bg_movie();
}

auto Sorcery::Compendium::start() -> int {

	// Clear the window
	_generate_display();
	_window->clear();

	// Play the background movie!
	_display->fit_bg_movie();
	_display->start_bg_movie();

	_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
	_selected = _menu->items.begin();

	if (auto module_result{_do_event_loop()}; module_result == ModuleResult::EXIT) {

		// Shutdown
		_display->shutdown_SFML();
		return EXIT_ALL;
	}

	return EXIT_MODULE;
}

auto Sorcery::Compendium::stop() -> void {

	_display->stop_bg_movie();
}

auto Sorcery::Compendium::_generate_display() -> void {

	_reset_components();
	_initalise_components();
	_display->generate("compendium");
	_place_components();
}

auto Sorcery::Compendium::_reset_components() -> void {

	if (_menu.get())
		_menu.reset();
	if (_museum.get())
		_museum.reset();
	if (_bestiary.get())
		_bestiary.reset();
}

auto Sorcery::Compendium::_place_components() -> void {

	_menu->setPosition(_display->get_centre_x(_menu->get_width()), (*_display->layout)["compendium:menu"].y);
}

auto Sorcery::Compendium::_initalise_components() -> void {

	// Get the Window and Graphics to Display
	_window = _display->window->get_window();

	_menu = std::make_unique<Menu>(_system, _display, _graphics, _game, MenuType::COMPENDIUM);
	_menu->generate((*_display->layout)["compendium:menu"]);

	_museum = std::make_unique<Museum>(_system, _display, _graphics, _game);
	_bestiary = std::make_unique<Bestiary>(_system, _display, _graphics, _game);
}

auto Sorcery::Compendium::_refresh_display() -> void {

	_window->clear();

	if (_display->layout->refresh_if_needed())
		_generate_display();

	_display->start_bg_movie();
	_display->update_bg_movie();
	_display->draw_bg_movie();

	_draw();
	_window->display();
}

auto Sorcery::Compendium::_draw() -> void {

	_display->display("compendium");

	_menu->generate((*_display->layout)["compendium:menu"]);
	_window->draw(*_menu);

	_display->display_overlay();
	_display->display_cursor();
}

auto Sorcery::Compendium::_do_event_loop() -> std::optional<ModuleResult> {

	// And do the main loop
	while (_window->isOpen()) {
		sf::Event event{};
		while (_window->pollEvent(event)) {
			auto const module_result = _handle_input(event);
			if (module_result) {
				if (module_result.value() == ModuleResult::CLOSE)
					return ModuleResult::CLOSE;
				if (module_result.value() == ModuleResult::BACK)
					return ModuleResult::BACK;
				if (module_result.value() == ModuleResult::EXIT)
					return ModuleResult::EXIT;
			}
		}

		_window->clear();

		_refresh_display();
		_window->display();
	}

	return std::nullopt;
}

auto Sorcery::Compendium::_handle_input(const sf::Event &event) -> std::optional<ModuleResult> {

	// Check for Window Close
	if (event.type == sf::Event::Closed)
		return ModuleResult::EXIT;

	if (_system->input->check(WindowInput::CANCEL, event))
		return ModuleResult::CLOSE;

	if (_system->input->check(WindowInput::BACK, event))
		return ModuleResult::BACK;

	// Handle enabling help overlay
	if (_system->input->check(WindowInput::SHOW_CONTROLS, event)) {
		_display->show_overlay();
		return std::nullopt;
	} else
		_display->hide_overlay();

	if (_system->input->check(WindowInput::UP, event))
		_selected = _menu->choose_previous();
	else if (_system->input->check(WindowInput::DOWN, event))
		_selected = _menu->choose_next();
	else if (_system->input->check(WindowInput::MOVE, event))
		_selected = _menu->set_mouse_selected(static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
	else if (_system->input->check(WindowInput::CONFIRM, event)) {

		// We have selected something from the menu
		if (_selected) {
			const MenuItem option_chosen{(*_selected.value()).item};
			if (option_chosen == MenuItem::ITEM_RETURN)
				return ModuleResult::BACK;
			else if (option_chosen == MenuItem::CO_ITEMS) {
				if (auto result{_museum->start()}; result && result == EXIT_ALL) {
					_museum->stop();
					return ModuleResult::EXIT;
				}
				_museum->stop();
				_generate_display();
				_refresh_display();
			} else if (option_chosen == MenuItem::CO_MONSTERS) {
				if (auto result{_bestiary->start()}; result && result == EXIT_ALL) {
					_bestiary->stop();
					return ModuleResult::EXIT;
				}
				_bestiary->stop();
				_generate_display();
				_refresh_display();
			}
		}
	}

	return std::nullopt;
}
