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

#include "modules/roster.hpp"
#include "common/enum.hpp"
#include "common/type.hpp"
#include "core/display.hpp"
#include "core/game.hpp"
#include "core/graphics.hpp"
#include "core/input.hpp"
#include "core/system.hpp"
#include "core/window.hpp"
#include "gui/characterdisplay.hpp"
#include "gui/characterpanel.hpp"
#include "gui/dialog.hpp"
#include "gui/frame.hpp"
#include "gui/menu.hpp"
#include "modules/edit.hpp"
#include "resources/componentstore.hpp"
#include "resources/resourcemanager.hpp"
#include "types/character.hpp"
#include "types/component.hpp"

// Standard Constructor
Sorcery::Roster::Roster(System *system, Display *display, Graphics *graphics, Game *game, RosterMode mode)
	: _system{system}, _display{display}, _graphics{graphics}, _game{game}, _mode{mode} {

	// Get the Window and Graphics to Display
	_window = _display->window->get_window();
	_cur_char = std::nullopt;

	// Same object can be used in three different modes
	switch (_mode) {
	case RosterMode::INSPECT:
		_screen_key = "roster_inspect";
		break;
	case RosterMode::EDIT:
		_screen_key = "roster_edit";
		break;
	case RosterMode::DELETE:
		_screen_key = "roster_delete";
		break;
	default:
		break;
	}

	_char_panel = std::make_unique<CharacterPanel>(_system, _display, _graphics);
	_character_display = std::make_unique<CharacterDisplay>(_system, _display, _graphics);

	_edit = std::make_unique<Edit>(_system, _display, _graphics, _game);

	_delete = std::make_unique<Dialog>(_system, _display, _graphics,
		(*_display->layout)["roster_delete:dialog_delete_character"],
		(*_display->layout)["roster_delete:dialog_delete_character_text"], WDT::CONFIRM);
	_delete->setPosition(_display->get_centre_pos(_delete->get_size()));
}

// Standard Destructor
Sorcery::Roster::~Roster() {
}

auto Sorcery::Roster::start() -> std::optional<MIM> {

	_game->load_game();

	// Do the menu here when it has access to the game characters
	_menu.reset();
	_menu = std::make_unique<Menu>(_system, _display, _graphics, _game, MTP::CHARACTER_ROSTER, MMD::TRAINING);
	_menu->generate((*_display->layout)[_screen_key + ":menu"]);
	_menu->setPosition(_display->get_centre_x(_menu->get_width()), (*_display->layout)[_screen_key + ":menu"].y);

	_cur_char_id = -1;

	// Get the Background Display Components and load them into Display module
	// storage (not local)
	switch (_mode) {
	case RosterMode::INSPECT:
		_display->generate("roster_inspect");
		break;
	case RosterMode::EDIT:
		_display->generate("roster_edit");
		break;
	case RosterMode::DELETE:
		_display->generate("roster_delete");
		break;
	default:
		break;
	}

	// Set up the Custom Components
	const Component bg_c{(*_display->layout)[_screen_key + ":background"]};
	sf::IntRect bg_rect{};
	bg_rect.width = std::stoi(bg_c["source_w"].value());
	bg_rect.height = std::stoi(bg_c["source_h"].value());
	bg_rect.top = 0;
	bg_rect.left = std::stoi(bg_c["source_w"].value()) * std::stoi(bg_c["source_index"].value());

	_bg.setTexture(_system->resources->textures[GTX::TOWN]);
	_bg.setTextureRect(bg_rect);
	_bg.setScale(std::stof(bg_c["scale_x"].value()), std::stof(bg_c["scale_y"].value()));
	_bg.setPosition(_display->window->get_x(_bg, bg_c.x), _display->window->get_y(_bg, bg_c.y));

	const Component menu_fc{(*_display->layout)[_screen_key + ":menu_frame"]};
	_menu_frame = std::make_unique<Frame>(
		_display->ui_texture, menu_fc.w, menu_fc.h, menu_fc.colour, menu_fc.background, menu_fc.alpha);
	_menu_frame->setPosition(_display->window->get_x(_menu_frame->sprite, menu_fc.x),
		_display->window->get_y(_menu_frame->sprite, menu_fc.y));

	const Component cc_fc{(*_display->layout)[_screen_key + ":character_frame"]};
	_cur_char_frame =
		std::make_unique<Frame>(_display->ui_texture, cc_fc.w, cc_fc.h, cc_fc.colour, cc_fc.background, cc_fc.alpha);
	_cur_char_frame->setPosition(_display->window->get_x(_cur_char_frame->sprite, cc_fc.x),
		_display->window->get_y(_cur_char_frame->sprite, cc_fc.y));

	const Component p_fc{(*_display->layout)["roster:preview_frame"]};
	_preview_frame =
		std::make_unique<Frame>(_display->ui_texture, p_fc.w, p_fc.h, p_fc.colour, p_fc.background, p_fc.alpha);
	_preview_frame->setPosition(_display->window->get_x(_preview_frame->sprite, p_fc.x),
		_display->window->get_y(_preview_frame->sprite, p_fc.y));

	// Clear the window
	_window->clear();

	_display->set_input_mode(WIM::NAVIGATE_MENU);
	std::optional<std::vector<MenuEntry>::const_iterator> selected{_menu->items.begin()};

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

			if (_display->get_input_mode() == WIM::NAVIGATE_MENU) {

				if (_system->input->check(CIN::CANCEL, event))
					return std::nullopt;

				if (_system->input->check(CIN::BACK, event))
					return std::nullopt;

				if (_system->input->check(CIN::UP, event))
					selected = _menu->choose_previous();
				else if (_system->input->check(CIN::DOWN, event))
					selected = _menu->choose_next();
				else if (_system->input->check(CIN::MOVE, event))
					selected = _menu->set_mouse_selected(static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
				else if (_system->input->check(CIN::CONFIRM, event)) {

					// We have selected something from the menu
					if (selected) {
						const MIM option_chosen{(*selected.value()).item};
						if (option_chosen == MIM::ET_TRAIN) {
							_display->set_input_mode(WIM::NAVIGATE_MENU);
							_cur_char = std::nullopt;
							return std::nullopt;
						} else {

							if (_mode == RosterMode::INSPECT) {

								const auto character_chosen{(*selected.value()).index};
								_cur_char = &_game->characters[character_chosen];
								_character_display->set(_cur_char.value());
								if (_cur_char) {
									_character_display->set_mode(CHM::AT_TRAINING);
									_display->set_input_mode(WIM::BROWSE_CHARACTER);
									_character_display->set_view(CHV::SUMMARY);
								}
							} else if (_mode == RosterMode::DELETE) {

								const auto character_chosen{(*selected.value()).index};
								_cur_char = &_game->characters[character_chosen];
								_character_display->set(_cur_char.value());
								if (_cur_char) {
									_display->set_input_mode(WIM::CONFIRM_DELETE_CHARACTER);
								}
							} else if (_mode == RosterMode::EDIT) {

								const auto character_chosen{(*selected.value()).index};
								auto result{_edit->start(character_chosen)};
								if (result && result.value() == MIM::ITEM_ABORT) {
									_game->save_game();
									_edit->stop();
									_display->shutdown_SFML();
									return MIM::ITEM_ABORT;
								}
								_edit->stop();
								_menu->reload();
								_cur_char = &_game->characters[character_chosen];
								_char_panel->set(_cur_char.value());
								_character_display->set(_cur_char.value());
								_display->generate("character_edit");
								_display->set_input_mode(WIM::NAVIGATE_MENU);
							}
						}
					}
				}

				if (selected) {
					if ((*selected.value()).item != MIM::ET_TRAIN) {
						const auto character_chosen{static_cast<int>((*selected.value()).index)};
						if (character_chosen != _cur_char_id) {
							auto character{&_game->characters[character_chosen]};
							_char_panel->set(character);
							_character_display->set(character);
							_cur_char_id = character_chosen;
						}
					} else {
						_char_panel->valid = false;
						_cur_char_id = -1;
					}
				}
			} else if (_display->get_input_mode() == WIM::CONFIRM_DELETE_CHARACTER) {

				auto dialog_input{_delete->handle_input(event)};
				if (dialog_input) {
					if (dialog_input.value() == WDB::CLOSE) {
						_display->set_input_mode(WIM::NAVIGATE_MENU);
						return std::nullopt;
					} else if (dialog_input.value() == WDB::YES) {

						// Delete a character!
						_game->delete_character(_cur_char_id);
						_game->save_game();
						_game->load_game();

						// Need to reload the menu!
						_menu->reload();

						// And select the first one in the list after one is
						// deleted
						_menu->choose_first();
						_display->set_input_mode(WIM::NAVIGATE_MENU);
						continue;
					} else if (dialog_input.value() == WDB::NO) {
						_display->set_input_mode(WIM::NAVIGATE_MENU);
					}
				}

			} else {

				if (_system->input->check(CIN::LEFT, event))
					_character_display->left_view();
				else if (_system->input->check(CIN::RIGHT, event))
					_character_display->right_view();
				else if (_system->input->check(CIN::CANCEL, event)) {
					_display->set_input_mode(WIM::NAVIGATE_MENU);
					_cur_char = std::nullopt;
				} else if (_system->input->check(CIN::BACK, event)) {
					_display->set_input_mode(WIM::NAVIGATE_MENU);
					_cur_char = std::nullopt;
				} else if (_system->input->check(CIN::CONFIRM, event)) {
					_character_display->right_view();
				} else if (_system->input->check(CIN::UP, event)) {
					if (_character_display->get_view() == CHV::MAGE_SPELLS)
						_character_display->dec_hl_spell(SPT::MAGE);
					else if (_character_display->get_view() == CHV::PRIEST_SPELLS)
						_character_display->dec_hl_spell(SPT::PRIEST);

				} else if (_system->input->check(CIN::DOWN, event)) {
					if (_character_display->get_view() == CHV::MAGE_SPELLS)
						_character_display->inc_hl_spell(SPT::MAGE);
					else if (_character_display->get_view() == CHV::PRIEST_SPELLS)
						_character_display->inc_hl_spell(SPT::PRIEST);
				} else if (_system->input->check(CIN::MOVE, event)) {
					if (_character_display->check_for_mouse_move(
							sf::Vector2f(static_cast<float>(sf::Mouse::getPosition(*_window).x),
								static_cast<float>(sf::Mouse::getPosition(*_window).y)))) {
						_character_display->set_view(_character_display->get_view());
					} else if (_character_display->check_for_action_mouse_move(
								   sf::Vector2f(static_cast<float>(sf::Mouse::getPosition(*_window).x),
									   static_cast<float>(sf::Mouse::getPosition(*_window).y)))) {
						_character_display->generate_display();
					}
				}
			}
		}

		_window->clear();

		_draw();
		_window->display();
	}

	_game->save_game();

	return std::nullopt;
}

auto Sorcery::Roster::stop() -> void {

	_display->set_input_mode(WIM::NAVIGATE_MENU);
}

auto Sorcery::Roster::_draw() -> void {

	// Play the background movie!
	_graphics->tile_bg(_window);

	// Display Components
	_display->display("roster");
	if (_display->get_input_mode() == WIM::BROWSE_CHARACTER) {
		if (_cur_char) {

			// Character Preview
			_window->draw(*_preview_frame);
			if (_char_panel->valid) {
				_char_panel->setPosition((*_display->layout)["roster:info_panel"].pos());
				_window->draw(*_char_panel);
			}

			// If we have a character
			_window->draw(*_cur_char_frame);
			_character_display->setPosition((*_display->layout)[_screen_key + ":character"].pos());
			_character_display->update();
			_window->draw(*_character_display);
		}
	} else if (_display->get_input_mode() == WIM::CONFIRM_DELETE_CHARACTER) {

		// Menu Frame
		_window->draw(*_menu_frame);
		_window->draw(*_preview_frame);

		// And the Menu
		_menu->generate((*_display->layout)[_screen_key + ":menu"]);
		const sf::Vector2f menu_pos((*_display->layout)[_screen_key + ":menu"].pos());
		_menu->setPosition(menu_pos);
		_window->draw(*_menu);

		// Character Preview
		if (_char_panel->valid) {
			_char_panel->setPosition((*_display->layout)["roster:info_panel"].pos());
			_window->draw(*_char_panel);
		}

		if (_cur_char) {
			_delete->update();
			_window->draw(*_delete);
		}
	} else {

		// Menu Frame
		_window->draw(*_menu_frame);
		_window->draw(*_preview_frame);

		// And the Menu
		_menu->generate((*_display->layout)[_screen_key + ":menu"]);
		_window->draw(*_menu);

		// Character Preview
		if (_char_panel->valid) {
			_char_panel->setPosition((*_display->layout)["roster:info_panel"].pos());
			_window->draw(*_char_panel);
		}
	}

	// And finally the Cursor
	_display->display_overlay();
	_display->display_cursor();
}
