// Copyright (C) 2023 Dave Moore
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

#include "event.hpp"

// Standard Constructor
Sorcery::Event::Event(System *system, Display *display, Graphics *graphics, Game *game, MapEvent type)
	: _system{system}, _display{display}, _graphics{graphics}, _game{game}, _type{type} {

	// Get the Window and Graphics to Display
	_window = _display->window->get_window();
	_continue_menu = std::make_unique<Menu>(_system, _display, _graphics, _game, MenuType::CONTINUE);
}

auto Sorcery::Event::start() -> std::optional<MenuItem> {

	using enum Enums::Map::Event;

	_stage = 1;
	_display->window->save_screen();
	std::optional<std::vector<MenuEntry>::const_iterator> option_continue{_continue_menu->items.begin()};

	// Generate the display
	switch (_type) {
	case AREA_OF_OUT_BOUNDS:
		_display->generate("event_area_out_of_bounds", _sprites, _texts, _frames);
		break;
	case MAN_TELEPORT_CASTLE:
		_display->generate("event_man_teleport_castle", _sprites, _texts, _frames);
		break;
	default:

		return std::nullopt;
		break;
	}

	// Clear the window
	_window->clear();

	// Handle Input
	sf::Event event{};
	while (_window->isOpen()) {
		while (_window->pollEvent(event)) {

			if (event.type == sf::Event::Closed)
				return MenuItem::ABORT;

			// Handle enabling help overlay
			if (_system->input->check(WindowInput::SHOW_CONTROLS, event)) {
				_display->show_overlay();
				continue;
			} else
				_display->hide_overlay();

			switch (_type) {
			case AREA_OF_OUT_BOUNDS:
			case MAN_TELEPORT_CASTLE:
				if (_system->input->check(WindowInput::CANCEL, event))
					return MenuItem::CONTINUE;
				else if (_system->input->check(WindowInput::BACK, event))
					return MenuItem::CONTINUE;
				else if (_system->input->check(WindowInput::UP, event))
					option_continue = _continue_menu->choose_previous();
				else if (_system->input->check(WindowInput::DOWN, event))
					option_continue = _continue_menu->choose_next();
				else if (_system->input->check(WindowInput::MOVE, event))
					option_continue =
						_continue_menu->set_mouse_selected(static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
				else if (_system->input->check(WindowInput::CONFIRM, event)) {

					if (option_continue) {
						if (const MenuItem option_chosen{(*option_continue.value()).item};
							option_chosen == MenuItem::CONTINUE) {
							return MenuItem::CONTINUE;
						}
					}
				}
				break;
			default:

				return std::nullopt;
				break;
			}
		}

		_window->clear();

		_draw();
		_window->display();
	}

	return std::nullopt;
}

auto Sorcery::Event::stop() -> void {

	_display->set_input_mode(WindowInputMode::IN_GAME);
	_display->window->restore_screen();
}

auto Sorcery::Event::_draw() -> void {

	using enum Enums::Map::Event;

	_display->window->restore_screen();

	switch (_type) {
	case AREA_OF_OUT_BOUNDS: {
		_display->display("event_area_out_of_bounds", _sprites, _texts, _frames);

		_continue_menu->generate((*_display->layout)["event_area_out_of_bounds:continue_menu"]);
		const sf::Vector2f menu_pos((*_display->layout)["event_area_out_of_bounds:continue_menu"].x,
			(*_display->layout)["event_area_out_of_bounds:continue_menu"].y);
		_continue_menu->setPosition(menu_pos);
		_window->draw(*_continue_menu);
	} break;
	case MAN_TELEPORT_CASTLE: {
		_display->display("event_man_teleport_castle", _sprites, _texts, _frames);

		_continue_menu->generate((*_display->layout)["event_man_teleport_castle:continue_menu"]);
		const sf::Vector2f menu_pos((*_display->layout)["event_man_teleport_castle:continue_menu"].x,
			(*_display->layout)["event_man_teleport_castle:continue_menu"].y);
		_continue_menu->setPosition(menu_pos);
		_window->draw(*_continue_menu);
	} break;
	default:

		break;
	}

	// Always draw the following
	_display->display_overlay();
	_display->display_cursor();
}

//