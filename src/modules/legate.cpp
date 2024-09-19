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

#include "modules/legate.hpp"
#include "common/enum.hpp"
#include "common/type.hpp"
#include "core/display.hpp"
#include "core/graphics.hpp"
#include "core/input.hpp"
#include "core/system.hpp"
#include "core/window.hpp"
#include "gui/dialog.hpp"
#include "gui/frame.hpp"
#include "gui/menu.hpp"
#include "resources/componentstore.hpp"
#include "resources/resourcemanager.hpp"
#include "resources/stringstore.hpp"
#include "types/character.hpp"
#include "types/component.hpp"

// Standard Constructor
Sorcery::Legate::Legate(System *system, Display *display, Graphics *graphics, Character *character)
	: _system{system}, _display{display}, _graphics{graphics}, _character{character} {

	// Get the Window and Graphics to Display
	_window = _display->window->get_window();

	// Dialog
	_proceed =
		std::make_unique<Dialog>(_system, _display, _graphics, (*_display->layout)["legate:dialog_confirm_legate"],
			(*_display->layout)["legate:dialog_confirm_legate_text"], WDT::CONFIRM);
	_proceed->setPosition(_display->get_centre_pos(_proceed->get_size()));

	// Menu
	_menu = std::make_unique<Menu>(_system, _display, _graphics, nullptr, MTP::CHOOSE_CHARACTER_ALIGNMENT);
	_menu->generate((*_display->layout)["legate:menu"]);
	_menu->setPosition(_display->get_centre_x(_menu->get_width()), (*_display->layout)["legate:menu"].y);

	_set_alignment_menu();

	// Frame
	const Component menu_fc{(*_display->layout)["legate:menu_frame"]};
	_frame = std::make_unique<Frame>(
		_display->ui_texture, menu_fc.w, menu_fc.h, menu_fc.colour, menu_fc.background, menu_fc.alpha);
	_frame->setPosition(
		_display->window->get_x(_frame->sprite, menu_fc.x), _display->window->get_y(_frame->sprite, menu_fc.y));

	// Text
	const Component text_c{(*_display->layout)["legate:choose_alignment_text"]};
	_choose_alignment = sf::Text();
	_choose_alignment.setFont(_system->resources->fonts[text_c.font]);
	_choose_alignment.setCharacterSize(text_c.size);
	_choose_alignment.setFillColor(sf::Color(text_c.colour));
	auto alignment{(*_display->string)["LEGATE_CHARACTER_ALIGNMENT"]};
	if (_display->get_upper())
		std::transform(alignment.begin(), alignment.end(), alignment.begin(), ::toupper);

	_choose_alignment.setString((*_display->string)["LEGATE_CHARACTER_ALIGNMENT"]);
	const auto offset_x{std::invoke([&] {
		if (text_c["offset_x"])
			return std::stoi(text_c["offset_x"].value());
		else
			return 0;
	})};
	const auto offset_y{std::invoke([&] {
		if (text_c["offset_y"])
			return std::stoi(text_c["offset_y"].value());
		else
			return 0;
	})};
	_choose_alignment.setPosition(text_c.x + offset_x, text_c.y + offset_y);

	// Initial Stage
	_stage = CHG::NO_STAGE;
}

// Standard Destructor
Sorcery::Legate::~Legate() {
}

auto Sorcery::Legate::start() -> std::optional<CAL> {

	_stage = CHG::CONFIRM;
	_display->generate("legate");
	_display->set_input_mode(WIM::CONFIRM_LEGATE);

	// Clear the window
	_window->clear();

	while (_window->isOpen()) {
		sf::Event event{};
		while (_window->pollEvent(event)) {

			// Check for Window Close
			if (event.type == sf::Event::Closed)
				return std::nullopt;

			// Handle enabling help overlay
			if (_system->input->check(CIN::SHOW_CONTROLS, event)) {
				_display->show_overlay();
				continue;
			} else
				_display->hide_overlay();

			if (_system->input->check(CIN::CANCEL, event))
				return std::nullopt;

			if (_system->input->check(CIN::BACK, event))
				return std::nullopt;

			if (_stage == CHG::CONFIRM) {

				auto dialog_input{_proceed->handle_input(event)};
				if (dialog_input) {
					if (dialog_input.value() == WDB::CLOSE) {
						_display->set_input_mode(WIM::NAVIGATE_MENU);
						return std::nullopt;
					} else if (dialog_input.value() == WDB::YES) {
						_display->set_input_mode(WIM::NAVIGATE_MENU);
						_stage = CHG::CHANGE_ALIGNMENT;
						continue;
					} else if (dialog_input.value() == WDB::NO) {
						_display->set_input_mode(WIM::NAVIGATE_MENU);
						return std::nullopt;
					}
				}

			} else if (_stage == CHG::CHANGE_ALIGNMENT) {

				std::optional<std::vector<MenuEntry>::const_iterator> selected{_menu->selected};
				if (_system->input->check(CIN::UP, event))
					selected = _menu->choose_previous();
				else if (_system->input->check(CIN::DOWN, event))
					selected = _menu->choose_next();
				else if (_system->input->check(CIN::MOVE, event))
					selected = _menu->set_mouse_selected(_display->get_cur());
				else if (_system->input->check(CIN::BACK, event)) {
					_stage = CHG::CONFIRM;
					_display->set_input_mode(WIM::CONFIRM_LEGATE);
					continue;
				} else if (_system->input->check(CIN::DELETE, event)) {
					_stage = CHG::CONFIRM;
					_display->set_input_mode(WIM::CONFIRM_LEGATE);
					continue;
				} else if (_system->input->check(CIN::CONFIRM, event)) {

					switch ((*selected.value()).item) {
					case MIM::CA_GOOD:
						return CAL::GOOD;
						break;
					case MIM::CA_NEUTRAL:
						return CAL::NEUTRAL;
						break;
					case MIM::CA_EVIL:
						return CAL::EVIL;
						break;
					default:
						return std::nullopt;
					}

					continue;
				}
			}
		}

		_window->clear();

		_draw();
		_window->display();
	}

	return std::nullopt;
}

auto Sorcery::Legate::stop() -> void {

	_display->set_input_mode(WIM::NAVIGATE_MENU);
}

auto Sorcery::Legate::_set_alignment_menu() -> void {

	switch (_character->get_class()) {
	case CHC::FIGHTER:
		(*_menu)[0].enabled = true;
		(*_menu)[1].enabled = true;
		(*_menu)[2].enabled = true;
		break;
	case CHC::MAGE:
		(*_menu)[0].enabled = true;
		(*_menu)[1].enabled = true;
		(*_menu)[2].enabled = true;
		break;
	case CHC::PRIEST:
		(*_menu)[0].enabled = true;
		(*_menu)[1].enabled = false;
		(*_menu)[2].enabled = true;
		break;
	case CHC::THIEF:
		(*_menu)[0].enabled = false;
		(*_menu)[1].enabled = true;
		(*_menu)[2].enabled = true;
		break;
	case CHC::BISHOP:
		(*_menu)[0].enabled = true;
		(*_menu)[1].enabled = false;
		(*_menu)[2].enabled = true;
		break;
	case CHC::SAMURAI:
		(*_menu)[0].enabled = true;
		(*_menu)[1].enabled = true;
		(*_menu)[2].enabled = false;
		break;
	case CHC::LORD:
		(*_menu)[0].enabled = true;
		(*_menu)[1].enabled = false;
		(*_menu)[2].enabled = false;
		break;
	case CHC::NINJA:
		(*_menu)[0].enabled = false;
		(*_menu)[1].enabled = false;
		(*_menu)[2].enabled = true;
		break;
	default:
		break;
	}

	// And select the first enabled
	_menu->choose_first();
}

auto Sorcery::Legate::_draw() -> void {

	// Play the background movie!
	_graphics->tile_bg(_window);

	// Display Components
	_display->display("legate");

	if (_stage == CHG::CONFIRM) {

		_proceed->update();
		_window->draw(*_proceed);

	} else if (_stage == CHG::CHANGE_ALIGNMENT) {

		// And the Menu
		_window->draw(*_frame);
		_menu->generate((*_display->layout)["legate:menu"]);
		_window->draw(*_menu);
		_window->draw(_choose_alignment);
	}

	// And finally the Cursor
	_display->display_overlay();
	_display->display_cursor();
}
