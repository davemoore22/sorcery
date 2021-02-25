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

#include "options.hpp"

// Standard Constructor
Sorcery::Options::Options (System& system, Display& display, Graphics& graphics):  _system {system},
	_display {display}, _graphics {graphics} {

	// Get the Window and Graphics to Display
	_window = _display.window->get_window();

	_options_menu = std::make_shared<Menu>(_system, _display, _graphics, MenuType::OPTIONS);
	_option_on = Component((*_display.layout)["options:option_on"]);
	_option_off = Component((*_display.layout)["options:option_off"]);

	_tooltip = std::make_shared<Tooltip>(_system, _display, _graphics);

	// Get the Display Components
	_display.generate_components("options");

	_confirm_save = std::make_shared<Confirm>(_system, _display, _graphics,
		(*_display.layout)["options:confirm_save_game_options_frame"],
		(*_display.layout)["options:confirm_save_game_options"]);
	_confirm_cancel = std::make_shared<Confirm>(_system, _display, _graphics,
		(*_display.layout)["options:confirm_cancel_changes_options_frame"],
		(*_display.layout)["options:confirm_cancel_changes_options"]);
	_confirm_strict_on = std::make_shared<Confirm>(_system, _display, _graphics,
		(*_display.layout)["options:confirm_strict_mode_options_frame"],
		(*_display.layout)["options:confirm_strict_mode_options"]);
}

// Standard Destructor
Sorcery::Options::~Options() {

	_display.stop_background_movie();
}

auto Sorcery::Options::start() -> void {

	// On entry store the current settings
	_system.config->store_current_settings();

	// Clear the window
	_window->clear();
	_display.window->tooltips.clear();
	_display_tooltip = false;

	// Play the background movie!
	_display.start_background_movie();

	// And select the first option by default;
	_display.window->input_mode = WindowInputMode::GAME_OPTIONS;
	_options_menu->selected = _options_menu->items.begin();
	std::optional<std::vector<MenuEntry>::const_iterator> selected_option {_options_menu->items.begin()};

	// And do the main loop
	sf::Event event {};
	while (_window->isOpen()) {
		while (_window->pollEvent(event)) {

			// Check for Window Close
			if (event.type == sf::Event::Closed)
				return;

			if (_system.input->check_for_event(WindowInput::CANCEL, event))
				return;

			// And handle input on the main menu
			if (_system.input->check_for_event(WindowInput::UP, event)) {
					selected_option = _options_menu->choose_previous();
			} else if (_system.input->check_for_event(WindowInput::DOWN, event)) {
					selected_option = _options_menu->choose_next();
			} else if (_system.input->check_for_event(WindowInput::MOVE, event)) {
					selected_option =
						_options_menu->set_mouse_selected(static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
					if (selected_option) {
						if ((*_options_menu->selected).type == MenuItemType::ENTRY)
							_display_tooltip = _set_tooltip(static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
						else
							_display_tooltip = false;
					}


			} else if (_system.input->check_for_event(WindowInput::CONFIRM, event)) {
				if (selected_option) {
					if ((*_options_menu->selected).type == MenuItemType::ENTRY) {
						ConfigOption config_to_toggle = (*_options_menu->selected).config;
						if ((config_to_toggle == ConfigOption::STRICT_MODE) &&
							(!(*_system.config)[ConfigOption::STRICT_MODE])) {

							// Handle Strict Mode Toggling
							_system.config->set_strict_mode();
							(*_system.config)[ConfigOption::STRICT_MODE] = true;
						} else if ((config_to_toggle == ConfigOption::RECOMMENDED_MODE) &&
							(!(*_system.config)[ConfigOption::RECOMMENDED_MODE])) {

							// Handle Recommended Toggling
							_system.config->set_recommended_mode();
							(*_system.config)[ConfigOption::RECOMMENDED_MODE] = true;

						} else {

							// And toggling off strict mode
							(*_system.config)[config_to_toggle] = !(*_system.config)[config_to_toggle];
						}
					} else if ((*_options_menu->selected).type == MenuItemType::SAVE) {
						_system.config->save();
						return;
					} else if ((*_options_menu->selected).type == MenuItemType::CANCEL) {
						_system.config->load();
						return;
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
}


auto Sorcery::Options::stop() -> void {

	_display.stop_background_movie();
}

auto Sorcery::Options::_draw() -> void {

	double lerp = _graphics.animation->colour_lerp;
	_display.display_components("options");

	_options_menu->generate((*_display.layout)["options:options_menu"], lerp);
	const sf::Vector2f menu_pos((*_display.layout)["options:options_menu"].x,
		(*_display.layout)["options:options_menu"].y);
	_options_menu->setPosition(menu_pos);
	_window->draw(*_options_menu);

	// Always draw the following
	_display.display_cursor();

	if (_display_tooltip) {
		sf::Vector2i tooltip_position = sf::Mouse::getPosition(*_window);
		tooltip_position.x += 10;
		tooltip_position.y += 10;
		_tooltip->setPosition(tooltip_position.x, tooltip_position.y);
		_window->draw(*_tooltip);
	}
}

auto Sorcery::Options::_set_tooltip(sf::Vector2f mouse_position) -> bool {

	if (!_display.window->tooltips.empty()) {
		sf::Vector2f global_pos {_options_menu->getPosition()};
		mouse_position -= global_pos;

		WindowTooltipList::iterator contain = std::find_if(_display.window->tooltips.begin(),
			_display.window->tooltips.end(), [&mouse_position](const auto& entry){
				sf::FloatRect candidate = entry.second;
				return candidate.contains(mouse_position);
			});
		if (contain != _display.window->tooltips.end()) {
				std::string hint = (*contain).first;
			_tooltip->set(hint);
			return true;
		} else
			return false;
	} else
		return false;
}
