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

#include "frontend/options.hpp"
#include "core/display.hpp"
#include "core/graphics.hpp"
#include "core/input.hpp"
#include "core/system.hpp"
#include "core/window.hpp"
#include "frontend/define.hpp"
#include "gui/dialog.hpp"
#include "gui/infopanel.hpp"
#include "gui/menu.hpp"
#include "resources/componentstore.hpp"
#include "resources/factory.hpp"
#include "types/configfile.hpp"
#include "types/enum.hpp"

// Standard Constructor
Sorcery::Options::Options(System *system, Display *display, Graphics *graphics)
	: _system{system}, _display{display}, _graphics{graphics} {

	// Get the Window and Graphics to Display
	_window = _display->window->get_window();

	// Setup the Factory
	_factory = std::make_unique<Factory>(_system, _display, _graphics, nullptr);

	// Menu and Options
	_menu = _factory->make_menu("options:menu", MTP::OPTIONS);
	_option_on = Component((*_display->layout)["options:on"]);
	_option_off = Component((*_display->layout)["options:off"]);

	// Get the Infopanel
	_ip = std::make_unique<InfoPanel>(_system, _display, _graphics);

	// Create the Confirmation Dialog Boxes
	_confirm_save =
		_factory->make_dialog("options:dialog_confirm_save", WDT::CONFIRM);
	_confirm_cancel =
		_factory->make_dialog("options:dialog_confirm_cancel", WDT::CONFIRM);
	_confirm_strict =
		_factory->make_dialog("options:dialog_confirm_strict_on", WDT::CONFIRM);
}

// Standard Destructor
Sorcery::Options::~Options() {

	_display->stop_bg_movie();
}

auto Sorcery::Options::start() -> int {

	// Get the Background Display Components and load them into Display module
	// storage (not local)
	_display->generate("options");

	// On entry store the current settings
	_system->config->store();

	// Clear the window
	_window->clear();
	_ip->valid = false;

	// Play the background movie!
	_display->fit_bg_movie();
	_display->start_bg_movie();

	// And select the first option by default;
	_display->set_input_mode(WIM::GAME_OPTIONS);
	_menu->choose_first();
	MenuSelect selected{_menu->items.begin()};
	_set_infopanel(_menu->selected);

	// And do the main loop
	sf::Event event{};
	while (_window->isOpen()) {
		while (_window->pollEvent(event)) {

			// If we are in normal input mode
			if (_display->get_input_mode() == WIM::GAME_OPTIONS) {

				// Check for Window Close
				if (event.type == sf::Event::Closed)
					return EXIT_ALL;

				if (_system->input->check(CIN::CANCEL, event))
					return EXIT_MODULE;

				// Handle enabling help overlay
				if (_system->input->check(CIN::SHOW_CONTROLS, event)) {
					_display->show_overlay();
					continue;
				} else
					_display->hide_overlay();

				// And handle input on the main menu
				if (_system->input->check(CIN::UP, event)) {
					selected = _menu->choose_previous();
				} else if (_system->input->check(CIN::DOWN, event)) {
					selected = _menu->choose_next();
				} else if (_system->input->check(CIN::MOVE, event)) {
					selected = _menu->set_mouse_selected(_display->get_cur());
					if (selected) {
						// No Tooltips anymore
					}

				} else if (_system->input->check(CIN::CONFIRM, event)) {
					if (selected) {
						if ((*_menu->selected).type == MIT::ENTRY) {
							const CFG cfg{(*_menu->selected).config};
							if (cfg == CFG::STRICT_MODE &&
								!(*_system->config)[CFG::STRICT_MODE]) {

								// Ask for confirmation of Strict Mode
								_display->set_input_mode(
									WIM::CONFIRM_STRICT_MODE);
							} else if (cfg == CFG::RECOMMENDED_MODE &&
									   !(*_system->config)
										   [CFG::RECOMMENDED_MODE]) {

								// Handle Recommended Toggling
								_system->config->set_rec_mode();
								(*_system->config)[CFG::RECOMMENDED_MODE] =
									true;

							} else {

								(*_system->config)[cfg] =
									!(*_system->config)[cfg];

								// And toggling off strict mode
								if (!_system->config->is_strict_mode())
									(*_system->config)[CFG::STRICT_MODE] =
										false;

								if (!_system->config->is_rec_mode())
									(*_system->config)[CFG::RECOMMENDED_MODE] =
										false;
							}
						} else if ((*_menu->selected).type == MIT::SAVE) {

							// Ask for confirmation of Save
							_display->set_input_mode(WIM::SAVE_CHANGES);
						} else if ((*_menu->selected).type == MIT::CANCEL) {

							// Ask for confirmation of Cancel
							_display->set_input_mode(WIM::CANCEL_CHANGES);
						}
					}
				}

				_set_infopanel(_menu->selected);

			} else if (_display->get_input_mode() == WIM::CONFIRM_STRICT_MODE ||
					   _display->get_input_mode() == WIM::SAVE_CHANGES ||
					   _display->get_input_mode() == WIM::CANCEL_CHANGES) {

				// Check for Window Close
				if (event.type == sf::Event::Closed)
					return EXIT_ALL;

				if (_system->input->check(CIN::SHOW_CONTROLS, event)) {
					_display->show_overlay();
					continue;
				} else
					_display->hide_overlay();

				// All we can do is select Y or N
				if (_display->get_input_mode() == WIM::CONFIRM_STRICT_MODE) {
					if (auto input{_confirm_strict->handle_input(event)};
						input) {
						if (input.value() == WDB::CLOSE) {
							_display->set_input_mode(WIM::NAVIGATE_MENU);
							return EXIT_MODULE;
						} else if (input.value() == WDB::YES) {
							_system->config->set_strict_mode();
							(*_system->config)[CFG::STRICT_MODE] = true;
							_display->set_input_mode(WIM::GAME_OPTIONS);
						} else if (input.value() == WDB::NO) {
							_display->set_input_mode(WIM::GAME_OPTIONS);
						}
					}
				} else if (_display->get_input_mode() == WIM::SAVE_CHANGES) {
					if (auto input{_confirm_save->handle_input(event)}; input) {
						if (input.value() == WDB::CLOSE) {
							_display->set_input_mode(WIM::NAVIGATE_MENU);
							return EXIT_MODULE;
						} else if (input.value() == WDB::YES) {
							_display->set_input_mode(WIM::NAVIGATE_MENU);
							_system->config->save();
							return true;
						} else if (input.value() == WDB::NO) {
							_display->set_input_mode(WIM::GAME_OPTIONS);
						}
					}
				} else if (_display->get_input_mode() == WIM::CANCEL_CHANGES) {
					if (auto input{_confirm_cancel->handle_input(event)};
						input) {
						if (input.value() == WDB::CLOSE) {
							_display->set_input_mode(WIM::NAVIGATE_MENU);
							return false;
						} else if (input.value() == WDB::YES) {
							_display->set_input_mode(WIM::NAVIGATE_MENU);
							_system->config->load();
							return true;
						} else if (input.value() == WDB::NO) {
							_display->set_input_mode(WIM::GAME_OPTIONS);
						}
					}
				}
			}
		}

		_window->clear();

		_display->start_bg_movie();
		_display->update_bg_movie();
		_display->draw_bg_movie();

		_draw();
		_window->display();
	}

	return false;
}

auto Sorcery::Options::stop() -> void {

	_display->stop_bg_movie();
}

auto Sorcery::Options::_set_infopanel(
	std::vector<Sorcery::MenuEntry>::const_iterator it) -> void {

	// Set the Text
	if ((*it).type == MIT::ENTRY) {
		auto ip_contents{(*it).hint};
		_ip->set_text(ip_contents);
		_ip->valid = true;
	} else
		_ip->valid = false;

	// Set the Icon
	if ((*it).type == MIT::ENTRY)
		_ip->set_icon(MIM::OP_ALL);
}

auto Sorcery::Options::_draw() -> void {

	_display->display("options");

	_menu->generate((*_display->layout)["options:menu"]);
	_window->draw(*_menu);
	if (_display->get_input_mode() == WIM::CONFIRM_STRICT_MODE) {
		_confirm_strict->update();
		_window->draw(*_confirm_strict);
	} else if (_display->get_input_mode() == WIM::SAVE_CHANGES) {
		_confirm_save->update();
		_window->draw(*_confirm_save);
	} else if (_display->get_input_mode() == WIM::CANCEL_CHANGES) {
		_confirm_cancel->update();
		_window->draw(*_confirm_cancel);
	}

	if (_ip->valid) {
		_ip->setPosition((*_display->layout)["options:info_panel"].pos());
		_window->draw(*_ip);
	}

	// Always draw the following
	_display->display_overlay();
	_display->display_cursor();
}