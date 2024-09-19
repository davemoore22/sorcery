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

#include "engine/event.hpp"
#include "common/enum.hpp"
#include "core/display.hpp"
#include "core/game.hpp"
#include "core/graphics.hpp"
#include "core/input.hpp"
#include "core/system.hpp"
#include "core/window.hpp"
#include "resources/componentstore.hpp"
#include "resources/iconstore.hpp"

// Standard Constructor
Sorcery::Event::Event(System *system, Display *display, Graphics *graphics, Game *game, MAV type, unsigned int stage)
	: _system{system}, _display{display}, _graphics{graphics}, _game{game}, _type{type}, _stage{stage} {

	// Get the Window and Graphics to Display
	_window = _display->window->get_window();
	_dungeon_event = _game->get_event(_type);

	// some events are passed through without input directly to an encounter
	switch (_type) {
	case MAV::FIRE_DRAGONS_COMBAT:
		[[fallthrough]];
	case MAV::WERDNA_COMBAT:
		[[fallthrough]];
	case MAV::DEADLY_RING_COMBAT:
		[[fallthrough]];
	case MAV::GUARANTEED_COMBAT:
		break;
	case MAV::TREBOR_VOICE:
		[[fallthrough]];
	case MAV::WERDNA_BOAST: {

		// Others are multistage
		_cont_menu = std::make_unique<Menu>(_system, _display, _graphics, _game, MTP::CONTINUE);
		const auto menu_stage_key{std::invoke([&] {
			if (_stage == 1)
				return _dungeon_event.component_key + "_1:continue_menu";
			else if (_stage == 2)
				return _dungeon_event.component_key + "_2:continue_menu";
			else if (_stage == 3)
				return _dungeon_event.component_key + "_3:continue_menu";
			else
				return std::string{};
		})};
		_cont_menu->generate((*_display->layout)[menu_stage_key]);
		_cont_menu->setPosition(_display->get_centre_x(_cont_menu->get_width()), (*_display->layout)[menu_stage_key].y);
	} break;
	default: {
		_cont_menu = std::make_unique<Menu>(_system, _display, _graphics, _game, MTP::CONTINUE);
		const auto menu_key{_dungeon_event.component_key + ":continue_menu"};
		_cont_menu->generate((*_display->layout)[menu_key]);
		_cont_menu->setPosition(_display->get_centre_x(_cont_menu->get_width()), (*_display->layout)[menu_key].y);
	} break;
	}

	_event_layout = (*_display->layout)["engine_base_ui:event"];
	_icon = _graphics->icons->get_event_icon(_type);

	if (_icon) {
		_icon.value().setScale(
			std::stof(_event_layout["icon_scale_x"].value()), std::stof(_event_layout["icon_scale_y"].value()));
		_icon.value().setPosition(_event_layout.pos());
	}
}

auto Sorcery::Event::start() -> std::optional<MIM> {

	switch (_type) {
	case MAV::FIRE_DRAGONS_COMBAT:
		[[fallthrough]];
	case MAV::WERDNA_COMBAT:
		[[fallthrough]];
	case MAV::DEADLY_RING_COMBAT:
		[[fallthrough]];
	case MAV::GUARANTEED_COMBAT:
		return std::nullopt;
		break;
	default: {
		_display->window->save_screen();
		std::optional<std::vector<MenuEntry>::const_iterator> opt{_cont_menu->items.begin()};

		// Generate the display
		std::string screen_key{};
		if (_type == MAV::TREBOR_VOICE || _type == MAV::WERDNA_BOAST) {
			screen_key = std::invoke([&] {
				if (_stage == 1)
					return _dungeon_event.component_key + "_1";
				else if (_stage == 2)
					return _dungeon_event.component_key + "_2";
				else if (_stage == 3)
					return _dungeon_event.component_key + "_3";
				else
					return std::string{};
			});
		} else
			screen_key = _dungeon_event.component_key;

		_display->generate(screen_key, _sprites, _texts, _frames);

		// Clear the window
		_window->clear();

		// Handle Input
		sf::Event event{};
		while (_window->isOpen()) {
			while (_window->pollEvent(event)) {

				if (event.type == sf::Event::Closed)
					return MIM::ITEM_ABORT;

				// Handle enabling help overlay
				if (_system->input->check(CIN::SHOW_CONTROLS, event)) {
					_display->show_overlay();
					continue;
				} else
					_display->hide_overlay();

				// We have all continue menus to begin with
				if (_system->input->check(CIN::CANCEL, event))
					return MIM::ITEM_CONTINUE;
				else if (_system->input->check(CIN::BACK, event))
					return MIM::ITEM_CONTINUE;
				else if (_system->input->check(CIN::UP, event))
					opt = _cont_menu->choose_previous();
				else if (_system->input->check(CIN::DOWN, event))
					opt = _cont_menu->choose_next();
				else if (_system->input->check(CIN::MOVE, event))
					opt = _cont_menu->set_mouse_selected(static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
				else if (_system->input->check(CIN::CONFIRM, event)) {

					if (opt) {
						if (const MIM opt_cont{(*opt.value()).item}; opt_cont == MIM::ITEM_CONTINUE) {
							return MIM::ITEM_CONTINUE;
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

	_display->set_input_mode(WIM::IN_GAME);
	_display->window->restore_screen();
}

auto Sorcery::Event::_draw() -> void {

	_display->window->restore_screen();

	switch (_type) {
	case MAV::FIRE_DRAGONS_COMBAT:
		[[fallthrough]];
	case MAV::WERDNA_COMBAT:
		[[fallthrough]];
	case MAV::DEADLY_RING_COMBAT:
		[[fallthrough]];
	case MAV::GUARANTEED_COMBAT:
		break;
	case MAV::TREBOR_VOICE:
		[[fallthrough]];
	case MAV::WERDNA_BOAST: {

		// Others are multistage
		const auto screen_key{std::invoke([&] {
			if (_stage == 1)
				return _dungeon_event.component_key + "_1";
			else if (_stage == 2)
				return _dungeon_event.component_key + "_2";
			else if (_stage == 3)
				return _dungeon_event.component_key + "_3";
			else
				return std::string{};
		})};
		const auto menu_key{screen_key + ":continue_menu"};
		_display->display(screen_key, _sprites, _texts, _frames);
		_cont_menu->generate((*_display->layout)[menu_key]);
		_window->draw(*_cont_menu);

		if (_icon)
			_window->draw(_icon.value());

		// Always draw the following
		_display->display_overlay();
		_display->display_cursor();
	} break;
	default: {
		const auto screen_key{_dungeon_event.component_key};
		const auto menu_key{_dungeon_event.component_key + ":continue_menu"};
		_display->display(screen_key, _sprites, _texts, _frames);
		_cont_menu->generate((*_display->layout)[menu_key]);
		_window->draw(*_cont_menu);

		if (_icon)
			_window->draw(_icon.value());

		// Always draw the following
		_display->display_overlay();
		_display->display_cursor();
	} break;
	}
}
