// Copyright (C) 2021 Dave Moore
//
// This file is part of Sorcery: Dreams of the Mad Overlord.
//
// Sorcery: Dreams of the Mad Overlord is free software: you can redistribute
// it and/or modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation, either version 2 of the License,
// or (at your option) any later version.
//
// Sorcery: Dreams of the Mad Overlord is distributed in the hope that it wil
// be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
// Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Sorcery: Dreams of the Mad Overlord.  If not,
// see <http://www.gnu.org/licenses/>.
//
// If you modify this program, or any covered work, by linking or combining
// it with the libraries referred to in README (or a modified version of
// said libraries), containing parts covered by the terms of said libraries,
// the licensors of this program grant you additional permission to convey
// the resulting work.

#include "options.hpp"

// Standard Constructor
Sorcery::Options::Options(System &system, Display &display, Graphics &graphics)
	: _system{system}, _display{display}, _graphics{graphics} {

	// Get the Window and Graphics to Display
	_window = _display.window->get_window();

	// Menu and Options
	_menu = std::make_shared<Menu>(&_system, &_display, &_graphics, MenuType::OPTIONS);
	_option_on = Component((*_display.layout)["options:on"]);
	_option_off = Component((*_display.layout)["options:off"]);

	// Get the Tooltip
	_tt = std::make_shared<Tooltip>(_system, _display, _graphics);

	// Get the Display Components
	_display.generate_components("options");

	// Create the Confirmation Boxes
	_confirm_save = std::make_shared<Confirm>(_system, _display, _graphics,
		(*_display.layout)["options:save_frame"], (*_display.layout)["options:save_text"]);
	_confirm_cancel = std::make_shared<Confirm>(_system, _display, _graphics,
		(*_display.layout)["options:cancel_frame"], (*_display.layout)["options:cancel_text"]);
	_confirm_strict_on = std::make_shared<Confirm>(_system, _display, _graphics,
		(*_display.layout)["options:strict_frame"], (*_display.layout)["options:strict_text"]);
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
	_display_tt = false;

	// Play the background movie!
	_display.fit_background_movie();
	_display.start_background_movie();

	// And select the first option by default;
	_display.window->input_mode = WindowInputMode::GAME_OPTIONS;
	_menu->selected = _menu->items.begin();
	std::optional<std::vector<MenuEntry>::const_iterator> selected{_menu->items.begin()};

	// And do the main loop
	sf::Event event{};
	while (_window->isOpen()) {
		while (_window->pollEvent(event)) {

			// If we are in normal input mode
			if (_display.window->input_mode == WindowInputMode::GAME_OPTIONS) {

				// Check for Window Close
				if (event.type == sf::Event::Closed)
					return;

				if (_system.input->check_for_event(WindowInput::CANCEL, event))
					return;

				// And handle input on the main menu
				if (_system.input->check_for_event(WindowInput::UP, event)) {
					selected = _menu->choose_previous();
				} else if (_system.input->check_for_event(WindowInput::DOWN, event)) {
					selected = _menu->choose_next();
				} else if (_system.input->check_for_event(WindowInput::MOVE, event)) {
					selected = _menu->set_mouse_selected(
						static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
					if (selected) {
						if ((*_menu->selected).type == MenuItemType::ENTRY)
							_display_tt = _set_tooltip(
								static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
						else
							_display_tt = false;
					}

				} else if (_system.input->check_for_event(WindowInput::CONFIRM, event)) {
					if (selected) {
						if ((*_menu->selected).type == MenuItemType::ENTRY) {
							const ConfigOption config_to_toggle{(*_menu->selected).config};
							if ((config_to_toggle == ConfigOption::STRICT_MODE) &&
								(!(*_system.config)[ConfigOption::STRICT_MODE])) {

								// Ask for confirmation of Strict Mode
								_display.window->input_mode =
									WindowInputMode::SWITCH_ON_STRICT_MODE;
								_yes_or_no = WindowConfirm::NO;

							} else if ((config_to_toggle == ConfigOption::RECOMMENDED_MODE) &&
									   (!(*_system.config)[ConfigOption::RECOMMENDED_MODE])) {

								// Handle Recommended Toggling
								_system.config->set_recommended_mode();
								(*_system.config)[ConfigOption::RECOMMENDED_MODE] = true;

							} else {

								// And toggling off strict mode
								(*_system.config)[config_to_toggle] =
									!(*_system.config)[config_to_toggle];
							}
						} else if ((*_menu->selected).type == MenuItemType::SAVE) {

							// Ask for confirmation of Save
							_display.window->input_mode = WindowInputMode::SAVE_CHANGES;
							_yes_or_no = WindowConfirm::NO;
						} else if ((*_menu->selected).type == MenuItemType::CANCEL) {

							// Ask for confirmation of Cancel
							_display.window->input_mode = WindowInputMode::CANCEL_CHANGES;
							_yes_or_no = WindowConfirm::NO;
						}
					}
				}

			} else if ((_display.window->input_mode == WindowInputMode::SWITCH_ON_STRICT_MODE) ||
					   (_display.window->input_mode == WindowInputMode::SAVE_CHANGES) ||
					   (_display.window->input_mode == WindowInputMode::CANCEL_CHANGES)) {

				// Check for Window Close
				if (event.type == sf::Event::Closed)
					return;

				// All we can do is select Y or N
				if (_system.input->check_for_event(WindowInput::LEFT, event))
					_confirm_strict_on->toggle_highlighted();
				else if (_system.input->check_for_event(WindowInput::RIGHT, event))
					_confirm_strict_on->toggle_highlighted();
				else if (_system.input->check_for_event(WindowInput::YES, event))
					_confirm_strict_on->highlighted = WindowConfirm::YES;
				else if (_system.input->check_for_event(WindowInput::NO, event))
					_confirm_strict_on->highlighted = WindowConfirm::NO;
				else if (_system.input->check_for_event(WindowInput::CANCEL, event))
					_display.window->input_mode = WindowInputMode::GAME_OPTIONS;
				else if (_system.input->check_for_event(WindowInput::BACK, event))
					_display.window->input_mode = WindowInputMode::GAME_OPTIONS;
				else if (_system.input->check_for_event(WindowInput::MOVE, event)) {
					if (_display.window->input_mode == WindowInputMode::SWITCH_ON_STRICT_MODE) {
						_confirm_strict_on->check_for_mouse_move(
							static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
					} else if (_display.window->input_mode == WindowInputMode::SAVE_CHANGES) {
						_confirm_save->check_for_mouse_move(
							static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
					} else if (_display.window->input_mode == WindowInputMode::CANCEL_CHANGES) {
						_confirm_cancel->check_for_mouse_move(
							static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
					}
				} else if (_system.input->check_for_event(WindowInput::CONFIRM, event)) {

					std::optional<WindowConfirm> option{};
					if (_display.window->input_mode == WindowInputMode::SWITCH_ON_STRICT_MODE) {
						option = _confirm_strict_on->check_if_option_selected(
							static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
					} else if (_display.window->input_mode == WindowInputMode::SAVE_CHANGES) {
						option = _confirm_save->check_if_option_selected(
							static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
					} else if (_display.window->input_mode == WindowInputMode::CANCEL_CHANGES) {
						option = _confirm_cancel->check_if_option_selected(
							static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
					}

					// Mouse click only
					if (option) {
						if (option.value() == WindowConfirm::YES) {

							// Switch on Strict Mode Here!
							if (_display.window->input_mode ==
								WindowInputMode::SWITCH_ON_STRICT_MODE) {
								_system.config->set_strict_mode();
								(*_system.config)[ConfigOption::STRICT_MODE] = true;
								_display.window->input_mode = WindowInputMode::GAME_OPTIONS;
							} else if (_display.window->input_mode ==
									   WindowInputMode::CANCEL_CHANGES) {
								_system.config->load();
								return;
							} else if (_display.window->input_mode ==
									   WindowInputMode::SAVE_CHANGES) {
								_system.config->save();
								return;
							}
						}
						if (option.value() == WindowConfirm::NO)
							_display.window->input_mode = WindowInputMode::GAME_OPTIONS;
					} else {

						// Button/Keyboard
						if (_confirm_strict_on->highlighted == WindowConfirm::YES) {

							// Switch on Strict Mode Here!
							if (_display.window->input_mode ==
								WindowInputMode::SWITCH_ON_STRICT_MODE) {
								_system.config->set_strict_mode();
								(*_system.config)[ConfigOption::STRICT_MODE] = true;
								_display.window->input_mode = WindowInputMode::GAME_OPTIONS;
							} else if (_display.window->input_mode ==
									   WindowInputMode::CANCEL_CHANGES) {
								_system.config->load();
								return;
							} else if (_display.window->input_mode ==
									   WindowInputMode::SAVE_CHANGES) {
								_system.config->save();
								return;
							}
						} else if (_confirm_strict_on->highlighted == WindowConfirm::NO)
							_display.window->input_mode = WindowInputMode::GAME_OPTIONS;
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

	const double lerp{_graphics.animation->colour_lerp};
	_display.display_components("options");

	_menu->generate((*_display.layout)["options:menu"], lerp);
	const sf::Vector2f menu_pos(
		(*_display.layout)["options:menu"].x, (*_display.layout)["options:menu"].y);
	_menu->setPosition(menu_pos);
	_window->draw(*_menu);
	if (_display.window->input_mode == WindowInputMode::SWITCH_ON_STRICT_MODE) {
		_confirm_strict_on->draw(lerp);
	} else if (_display.window->input_mode == WindowInputMode::SAVE_CHANGES) {
		_confirm_save->draw(lerp);
	} else if (_display.window->input_mode == WindowInputMode::CANCEL_CHANGES) {
		_confirm_cancel->draw(lerp);
	} else if (_display_tt) {
		sf::Vector2i tooltip_position{sf::Mouse::getPosition(*_window)};
		tooltip_position.x += 10;
		tooltip_position.y += 10;
		_tt->setPosition(tooltip_position.x, tooltip_position.y);
		_window->draw(*_tt);
	}

	// Always draw the following
	_display.display_cursor();
}

auto Sorcery::Options::_set_tooltip(sf::Vector2f mouse_pos) -> bool {

	if (!_display.window->tooltips.empty()) {
		const sf::Vector2f global_pos{_menu->getPosition()};
		mouse_pos -= global_pos;

		auto contain{std::find_if(_display.window->tooltips.begin(),
			_display.window->tooltips.end(), [&mouse_pos](const auto &entry) {
				const sf::FloatRect candidate{entry.second};
				return candidate.contains(mouse_pos);
			})};
		if (contain != _display.window->tooltips.end()) {
			const std::string hint{(*contain).first};
			_tt->set(hint);
			return true;
		} else
			return false;
	} else
		return false;
}
