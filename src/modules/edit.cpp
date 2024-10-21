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

#include "modules/edit.hpp"
#include "common/enum.hpp"
#include "common/type.hpp"
#include "core/display.hpp"
#include "core/game.hpp"
#include "core/graphics.hpp"
#include "core/input.hpp"
#include "core/system.hpp"
#include "core/window.hpp"
#include "modules/changeclass.hpp"
#include "modules/changename.hpp"
#include "modules/define.hpp"
#include "modules/legate.hpp"
#include "resources/componentstore.hpp"
#include "resources/factory.hpp"
#include "resources/resourcemanager.hpp"
#include "types/character.hpp"
#include "types/component.hpp"

// Standard Constructor
Sorcery::Edit::Edit(
	System *system, Display *display, Graphics *graphics, Game *game)
	: _system{system}, _display{display}, _graphics{graphics}, _game{game} {

	// Get the Window and Graphics to Display
	_window = _display->window->get_window();

	// Setup the Factory
	_factory = std::make_unique<Factory>(_system, _display, _graphics, _game);

	_cur_char = std::nullopt;

	_char_panel =
		std::make_unique<CharacterPanel>(_system, _display, _graphics);

	_menu = _factory->make_menu("character_edit:menu", MTP::EDIT_CHARACTER);
	_changed = _factory->make_dialog("character_edit:dialog_class_changed");
	_legated = _factory->make_dialog("character_edit:dialog_legated");
	_menu_frame = _factory->make_menu_frame("character_edit:menu_frame");
	_preview_frame = _factory->make_frame("roster:preview_frame");
}

auto Sorcery::Edit::start(int current_character_idx) -> std::optional<MIM> {

	// Get the Background Display Components and load them into Display module
	// storage (not local)
	_display->generate("character_edit");

	_show_changed = false;
	_show_legated = false;

	// Get the Current Character
	_cur_char = &_game->characters[current_character_idx];
	_char_panel->set(_cur_char.value());

	// Set up the Custom Components
	const Component bg_c{(*_display->layout)["character_edit:background"]};
	sf::IntRect bg_rect{};
	bg_rect.width = std::stoi(bg_c["source_w"].value());
	bg_rect.height = std::stoi(bg_c["source_h"].value());
	bg_rect.top = 0;
	bg_rect.left = std::stoi(bg_c["source_w"].value()) *
				   std::stoi(bg_c["source_index"].value());

	_bg.setTexture(*_system->resources->get_texture(GTX::TOWN));
	_bg.setTextureRect(bg_rect);
	_bg.setScale(
		std::stof(bg_c["scale_x"].value()), std::stof(bg_c["scale_y"].value()));
	_bg.setPosition(_display->window->get_x(_bg, bg_c.x),
		_display->window->get_y(_bg, bg_c.y));

	// Clear the window
	_window->clear();

	_display->set_input_mode(WIM::NAVIGATE_MENU);
	MenuSelect selected{_menu->items.begin()};

	// And do the main loop
	sf::Event event{};
	while (_window->isOpen()) {
		while (_window->pollEvent(event)) {

			// Check for Window Close
			if (event.type == sf::Event::Closed)
				return MIM::ITEM_ABORT;

			// Handle enabling help overlay
			if (_system->input->check(CIN::SHOW_CONTROLS, event)) {
				_display->show_overlay();
				continue;
			} else
				_display->hide_overlay();

			if (_show_changed) {
				auto input{_changed->handle_input(event)};
				if (input) {
					if (input.value() == WDB::CLOSE ||
						input.value() == WDB::OK) {
						_show_changed = false;
						_display->set_input_mode(WIM::NAVIGATE_MENU);
						return std::nullopt;
					}
				}
			} else if (_show_legated) {
				auto input{_legated->handle_input(event)};
				if (input) {
					if (input.value() == WDB::CLOSE ||
						input.value() == WDB::OK) {
						_show_legated = false;
						_display->set_input_mode(WIM::NAVIGATE_MENU);
						return std::nullopt;
					}
				}
			} else {
				if (_system->input->check(CIN::CANCEL, event))
					return std::nullopt;

				if (_system->input->check(CIN::BACK, event))
					return std::nullopt;

				if (_system->input->check(CIN::UP, event))
					selected = _menu->choose_previous();
				else if (_system->input->check(CIN::DOWN, event))
					selected = _menu->choose_next();
				else if (_system->input->check(CIN::MOVE, event))
					selected = _menu->set_mouse_selected(_display->get_cur());
				else if (_system->input->check(CIN::CONFIRM, event)) {

					// We have selected something from the menu
					if (selected) {
						if (const MIM opt{(*selected.value()).item};
							opt == MIM::EC_RETURN_EDIT) {
							return MIM::EC_RETURN_EDIT;
						} else if (opt == MIM::EC_CHANGE_NAME) {

							auto change_name{
								std::make_unique<ChangeName>(_system, _display,
									_graphics, _cur_char.value()->get_name())};
							if (auto new_name{change_name->start()}; new_name) {

								if (new_name.value() == EXIT_STRING)
									return MIM::ITEM_ABORT;

								// Update character name and resave the
								// character!
								auto changed_name{new_name.value()};
								_cur_char.value()->set_name(changed_name);
								auto &character{*_cur_char.value()};
								_game->update_character(_game->get_id(),
									current_character_idx, character);
								_game->save_game();
							}
							change_name->stop();
						} else if (opt == MIM::EC_CHANGE_CLASS) {
							auto &character{*_cur_char.value()};
							auto change_class{std::make_unique<ChangeClass>(
								_system, _display, _graphics, &character)};

							if (auto new_class{change_class->start()};
								new_class) {

								// Can't select same class in the change_class
								// module - it returns nullopt if you do
								character.change_class(new_class.value());
								_game->update_character(_game->get_id(),
									current_character_idx, character);
								_game->save_game();

								_show_changed = true;
								_display->set_input_mode(WIM::NAVIGATE_MENU);
							}
							change_class->stop();
						} else if (opt == MIM::EC_LEGATE_CHARACTER) {
							auto &character{*_cur_char.value()};
							auto legate{std::make_unique<Legate>(
								_system, _display, _graphics, &character)};
							if (auto legated{legate->start()}; legated) {

								// How to exit from legated module?
								character.legate(legated.value());
								_game->update_character(_game->get_id(),
									current_character_idx, character);

								_game->save_game();
								auto cur_char{
									&_game->characters[current_character_idx]};
								_char_panel->set(cur_char);

								_show_legated = true;
								_display->set_input_mode(WIM::NAVIGATE_MENU);
							}
							legate->stop();
						}

						_display->generate("character_edit");
						_char_panel->set(_cur_char.value());
						_display->set_input_mode(WIM::NAVIGATE_MENU);
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

auto Sorcery::Edit::stop() -> void {

	_display->set_input_mode(WIM::NAVIGATE_MENU);
}

auto Sorcery::Edit::_draw() -> void {

	// Background Wallpaper
	_graphics->tile_bg(_window);

	// Display Components
	_display->display("character_edit");

	// Menu Frame
	_window->draw(*_menu_frame);

	// And the Menu
	_menu->generate((*_display->layout)["character_edit:menu"]);
	_window->draw(*_menu);

	// Character Preview
	_window->draw(*_preview_frame);
	if (_char_panel->valid) {
		_char_panel->setPosition(
			(*_display->layout)["character_edit:info_panel"].pos());
		_window->draw(*_char_panel);
	}

	if (_show_changed) {
		_changed->update();
		_window->draw(*_changed);
	}

	if (_show_legated) {
		_legated->update();
		_window->draw(*_legated);
	}

	// And finally the Cursor
	_display->display_overlay();
	_display->display_cursor();
}
