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
// be useful, but WITHOUT ANY WARRANTY; wiWthout even the implied warranty of
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

#include "modules/changename.hpp"
#include "common/enum.hpp"
#include "common/include.hpp"
#include "common/macro.hpp"
#include "common/type.hpp"
#include "core/animation.hpp"
#include "core/display.hpp"
#include "core/graphics.hpp"
#include "core/input.hpp"
#include "core/system.hpp"
#include "core/window.hpp"
#include "gui/keyboard.hpp"
#include "gui/text.hpp"
#include "modules/define.hpp"
#include "resources/componentstore.hpp"
#include "types/component.hpp"

// Standard Constructor
Sorcery::ChangeName::ChangeName(System *system, Display *display, Graphics *graphics, std::string old_name)
	: _system{system}, _display{display}, _graphics{graphics}, _old_name{old_name} {

	// Get the Window and Graphics to Display
	_window = _display->window->get_window();

	// Create the On-Screen Keyboard
	_keyboard = std::make_unique<Keyboard>(system, display, graphics);

	// Setup the Name
	_new_name = _old_name;

	Component name_candidate_c{(*_display->layout)["change_name:name_candidate"]};
	_name_candidate = std::make_unique<Text>(_system, _display, name_candidate_c,
		unenum(ComponentElement::COLOUR) | unenum(ComponentElement::FONT) | unenum(ComponentElement::SIZE) |
			unenum(ComponentElement::JUSTIFICATION));
	auto x{(*_display->layout)["change_name:name_candidate"].x == -1
			   ? _display->window->centre.x
			   : (*_display->layout)["change_name:name_candidate"].x};
	auto y{(*_display->layout)["change_name:name_candidate"].y == -1
			   ? _display->window->centre.y
			   : (*_display->layout)["change_name:name_candidate"].y};
	_name_candidate->setPosition(x, y);
}

// Standard Destructor
Sorcery::ChangeName::~ChangeName() {
}

auto Sorcery::ChangeName::start() -> std::optional<std::string> {

	_keyboard->setPosition((*_display->layout)["change_name:keyboard"].pos());

	const Component name_c{(*_display->layout)["change_name:name_candidate"]};

	_display->generate("change_name");
	_display->set_input_mode(WIM::INPUT_NAME);

	// Clear the window
	_window->clear();

	while (_window->isOpen()) {
		sf::Event event{};
		while (_window->pollEvent(event)) {

			// Check for Window Close
			if (event.type == sf::Event::Closed)
				return EXIT_STRING;

			// Handle enabling help overlay
			if (_system->input->check(WIP::SHOW_CONTROLS, event)) {
				_display->show_overlay();
				continue;
			} else
				_display->hide_overlay();

			if (_system->input->check(WIP::CANCEL, event))
				return std::nullopt;

			if (_system->input->check(WIP::BACK, event))
				return std::nullopt;

			auto name_changed = _handle_change_name(event);
			if (name_changed) {
				if (name_changed.value())
					return _new_name;
				else
					return std::nullopt;
			}
		}

		_window->clear();

		_draw();
		_window->display();
	}

	return std::nullopt;
}

auto Sorcery::ChangeName::stop() -> void {

	_display->set_input_mode(WIM::NAVIGATE_MENU);

	// Stop the background movie!
	_display->stop_bg_movie();
}

auto Sorcery::ChangeName::get_new_name() -> std::string {

	return _new_name;
}

auto Sorcery::ChangeName::is_changed() -> bool {

	return _old_name != _new_name;
}

auto Sorcery::ChangeName::_handle_change_name(const sf::Event &event) -> std::optional<bool> {

	auto candidate_name{_new_name};
	if (_system->input->check(WIP::MOVE, event)) {

		sf::Vector2f mouse_pos{static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window))};
		std::optional<std::string> mouse_selected{
			_keyboard->set_mouse_selected((*_display->layout)["character_create_stage_1:keyboard"], mouse_pos)};
		if (mouse_selected)
			_keyboard->selected = mouse_selected.value();
	} else if (_system->input->check(WIP::ALPHANUMERIC, event) || _system->input->check(WIP::SPACE, event)) {
		if (candidate_name.length() < 16) {
			candidate_name += static_cast<char>(event.text.unicode);
			_new_name = candidate_name;
		}
		if (static_cast<char>(event.text.unicode) == ' ') {
			_keyboard->selected = "Spc";
		} else {
			auto key_pressed{""s};
			key_pressed.push_back(static_cast<char>(event.text.unicode));
			_keyboard->selected = key_pressed;
		}
	} else if (_system->input->check(WIP::DELETE, event)) {
		if (candidate_name.length() > 0) {
			candidate_name.pop_back();
			_new_name = candidate_name;
			_keyboard->selected = "Del";
		}
	} else if (_system->input->check(WIP::BACK, event)) {
		if (candidate_name.length() > 0) {
			candidate_name.pop_back();
			_new_name = candidate_name;
			_keyboard->selected = "Del";
		} else {

			// Return if Back Button is selected and no character name is chosen
			return false;
		}
	} else if (_system->input->check(WIP::SELECT, event)) {
		if (_keyboard->selected == "End") {
			if (TRIM_COPY(candidate_name).length() > 0) {
				_new_name = candidate_name;

				// Return if End Icon is selected on the keyboard
				return true;
			}
		} else if (_keyboard->selected == "Spc") {
			if (candidate_name.length() < 16) {
				candidate_name += " ";
				_new_name = candidate_name;
			}
		} else if (_keyboard->selected == "Del") {
			if (candidate_name.length() > 0) {
				candidate_name.pop_back();
				_new_name = candidate_name;
			}
		} else {
			candidate_name += _keyboard->selected;
			_new_name = candidate_name;
		}
	} else if (_system->input->check(WIP::CONFIRM_NO_SPACE, event)) {

		if (_keyboard->selected == "End") {
			if (TRIM_COPY(candidate_name).length() > 0) {
				_new_name = candidate_name;

				// Return if End Icon is highlighted and Confirmed
				return true;
			}
		} else {
			if (TRIM_COPY(candidate_name).length() > 0) {
				_new_name = candidate_name;

				// Return if Confirmation is selected
				return true;
			}
		}
	} else if (_system->input->check(WIP::LEFT, event))
		_keyboard->set_selected(WIP::LEFT);
	else if (_system->input->check(WIP::RIGHT, event))
		_keyboard->set_selected(WIP::RIGHT);
	else if (_system->input->check(WIP::UP, event))
		_keyboard->set_selected(WIP::UP);
	else if (_system->input->check(WIP::DOWN, event))
		_keyboard->set_selected(WIP::DOWN);

	return std::nullopt;
}

auto Sorcery::ChangeName::_draw() -> void {

	// Background Wallpaper
	_graphics->tile_bg(_window);

	// Display Components
	_display->display("change_name");

	// Handle Custom Components
	auto lerp{_graphics->animation->colour_lerp};
	sf::Color adjusted{static_cast<sf::Color>(
		_graphics->adjust_brightness(sf::Color((*_display->layout)["change_name:name_candidate"].colour), lerp))};
	_name_candidate->set_fill_colour(adjusted);

	auto display_name{_new_name + "_"};
	_name_candidate->set_string(display_name);
	_window->draw(*_name_candidate);

	// Draw the On Screen Keyboard
	_keyboard->set_selected_background();
	_window->draw(*_keyboard);

	// And finally the Cursor
	_display->display_overlay();
	_display->display_cursor();
}
