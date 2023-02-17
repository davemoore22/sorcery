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
Sorcery::Event::Event(
	System *system, Display *display, Graphics *graphics, Game *game, MapEvent type, unsigned int stage)
	: _system{system}, _display{display}, _graphics{graphics}, _game{game}, _type{type}, _stage{stage} {

	using enum Enums::Map::Event;

	// Get the Window and Graphics to Display
	_window = _display->window->get_window();
	_dungeon_event = _game->get_event(_type);

	// some events are passed through without input directly to an encounter
	switch (_type) {
	case FIRE_DRAGONS_COMBAT:
		[[fallthrough]];
	case WERDNA_COMBAT:
		[[fallthrough]];
	case DEADLY_RING_COMBAT:
		[[fallthrough]];
	case GUARANTEED_COMBAT:
		break;
	case TREBOR_VOICE: {

		// Others are multistage
		_continue_menu = std::make_unique<Menu>(_system, _display, _graphics, _game, MenuType::CONTINUE);
		const auto menu_stage_key{[&] {
			if (_stage == 1)
				return _dungeon_event.component_key + "_1:continue_menu";
			else
				return _dungeon_event.component_key + "_2:continue_menu";
		}()};
		_continue_menu->generate((*_display->layout)[menu_stage_key]);
		_continue_menu->setPosition(
			_display->get_centre_x(_continue_menu->get_width()), (*_display->layout)[menu_stage_key].y);
	} break;
	default: {
		_continue_menu = std::make_unique<Menu>(_system, _display, _graphics, _game, MenuType::CONTINUE);
		const auto menu_key{_dungeon_event.component_key + ":continue_menu"};
		_continue_menu->generate((*_display->layout)[menu_key]);
		_continue_menu->setPosition(
			_display->get_centre_x(_continue_menu->get_width()), (*_display->layout)[menu_key].y);
	} break;
	}
}

auto Sorcery::Event::start() -> std::optional<MenuItem> {

	using enum Enums::Map::Event;

	switch (_type) {
	case FIRE_DRAGONS_COMBAT:
		[[fallthrough]];
	case WERDNA_COMBAT:
		[[fallthrough]];
	case DEADLY_RING_COMBAT:
		[[fallthrough]];
	case GUARANTEED_COMBAT:
		return std::nullopt;
		break;
	default: {
		_display->window->save_screen();
		std::optional<std::vector<MenuEntry>::const_iterator> option_continue{_continue_menu->items.begin()};

		// Generate the display
		std::string screen_key{};
		if (_type == TREBOR_VOICE) {
			screen_key = {[&] {
				if (_stage == 1)
					return _dungeon_event.component_key + "_1";
				else
					return _dungeon_event.component_key + "_2";
			}()};
		} else
			screen_key = _dungeon_event.component_key;

		// const auto screen_key{_dungeon_event.component_key};
		_display->generate(screen_key, _sprites, _texts, _frames);

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

				// We have all continue menus to begin with
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
			}

			_window->clear();

			_draw();
			_window->display();
		}

		return std::nullopt;
	}
	}
}

auto Sorcery::Event::stop() -> void {

	_display->set_input_mode(WindowInputMode::IN_GAME);
	_display->window->restore_screen();
}

auto Sorcery::Event::_draw() -> void {

	using enum Enums::Map::Event;

	_display->window->restore_screen();

	switch (_type) {
	case FIRE_DRAGONS_COMBAT:
		[[fallthrough]];
	case WERDNA_COMBAT:
		[[fallthrough]];
	case DEADLY_RING_COMBAT:
		[[fallthrough]];
	case GUARANTEED_COMBAT:
		break;
	case TREBOR_VOICE: {

		// Others are multistage
		const auto screen_key{[&] {
			if (_stage == 1)
				return _dungeon_event.component_key + "_1";
			else
				return _dungeon_event.component_key + "_2";
		}()};
		const auto menu_key{screen_key + ":continue_menu"};
		_display->display(screen_key, _sprites, _texts, _frames);
		_continue_menu->generate((*_display->layout)[menu_key]);
		_window->draw(*_continue_menu);

		// Always draw the following
		_display->display_overlay();
		_display->display_cursor();
	} break;
	default: {
		const auto screen_key{_dungeon_event.component_key};
		const auto menu_key{_dungeon_event.component_key + ":continue_menu"};
		_display->display(screen_key, _sprites, _texts, _frames);
		_continue_menu->generate((*_display->layout)[menu_key]);
		_window->draw(*_continue_menu);

		// Always draw the following
		_display->display_overlay();
		_display->display_cursor();
	} break;
	}
}
