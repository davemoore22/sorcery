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

#include "compendium.hpp"

// Standard Constructor
Sorcery::Compendium::Compendium(System *system, Display *display, Graphics *graphics)
	: _system{system}, _display{display}, _graphics{graphics} {

	// Get the Window and Graphics to Display
	_window = _display->window->get_window();
}

// Standard Destructor
Sorcery::Compendium::~Compendium() {

	_display->stop_bg_movie();
}

auto Sorcery::Compendium::start() -> int {

	// Get the Background Display Components and load them into Display module
	// storage (not local)
	_display->generate("compendium");

	// Clear the window
	_window->clear();

	// Play the background movie!
	_display->fit_bg_movie();
	_display->start_bg_movie();

	_display->set_input_mode(WindowInputMode::COMPENDIUM);

	auto module_result{_do_event_loop()};
	if (module_result == ModuleResult::EXIT) {

		// Shutdown
		_display->shutdown_SFML();
		return EXIT_ALL;
	}

	return EXIT_MODULE;
}

auto Sorcery::Compendium::stop() -> void {

	_display->stop_bg_movie();
}

auto Sorcery::Compendium::_draw() -> void {

	_display->display("compendium");
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

		_display->start_bg_movie();
		_display->update_bg_movie();
		_display->draw_bg_movie();

		_draw();
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

	return std::nullopt;
}
