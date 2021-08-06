// Copyright (C) 2021 Dave Moore
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

#include "edit.hpp"

// Standard Constructor
Sorcery::Edit::Edit(
	System *system, Display *display, Graphics *graphics, Game *game)
	: _system{system}, _display{display}, _graphics{graphics}, _game{game} {

	// Get the Window and Graphics to Display
	_window = _display->window->get_window();
	_cur_char = std::nullopt;

	_char_panel = std::make_unique<CharPanel>(_system, _display, _graphics);
	_menu = std::make_shared<Menu>(
		_system, _display, _graphics, _game, MenuType::EDIT_CHARACTER);

	_changed = std::make_unique<Dialog>(_system, _display, _graphics,
		(*_display->layout)["character_edit:dialog_class_changed"],
		(*_display->layout)["character_edit:dialog_class_changed_text"],
		WindowDialogType::OK);
	_changed->setPosition(
		(*_display->layout)["character_edit:dialog_class_changed"].x,
		(*_display->layout)["character_edit:dialog_class_changed"].y);
	_legated = std::make_unique<Dialog>(_system, _display, _graphics,
		(*_display->layout)["character_edit:dialog_legated"],
		(*_display->layout)["character_edit:dialog_legated_text"],
		WindowDialogType::OK);
	_legated->setPosition(
		(*_display->layout)["character_edit:dialog_legated"].x,
		(*_display->layout)["character_edit:dialog_legated"].y);
}

// Standard Destructor
Sorcery::Edit::~Edit() {}

auto Sorcery::Edit::start(int current_character_idx)
	-> std::optional<MenuItem> {

	// Get the Background Display Components and load them into Display module
	// storage (not local)
	_display->generate("character_edit");

	_show_changed = false;
	_show_legated = false;

	// Get the Current Character
	_cur_char = &_game->characters.at(current_character_idx);
	_char_panel->set(_cur_char.value());

	// Set up the Custom Components
	const Component bg_c{(*_display->layout)["character_edit:background"]};
	sf::IntRect bg_rect{};
	bg_rect.width = std::stoi(bg_c["source_w"].value());
	bg_rect.height = std::stoi(bg_c["source_h"].value());
	bg_rect.top = 0;
	bg_rect.left = std::stoi(bg_c["source_w"].value()) *
				   std::stoi(bg_c["source_index"].value());

	_bg.setTexture(_system->resources->textures[GraphicsTexture::TOWN]);
	_bg.setTextureRect(bg_rect);
	_bg.setScale(
		std::stof(bg_c["scale_x"].value()), std::stof(bg_c["scale_y"].value()));
	_bg.setPosition(_display->window->get_x(_bg, bg_c.x),
		_display->window->get_y(_bg, bg_c.y));

	const Component menu_fc{(*_display->layout)["character_edit:menu_frame"]};
	_menu_frame = std::make_unique<Frame>(_display->ui_texture,
		WindowFrameType::NORMAL, menu_fc.w, menu_fc.h, menu_fc.colour,
		menu_fc.background, menu_fc.alpha);
	_menu_frame->setPosition(
		_display->window->get_x(_menu_frame->sprite, menu_fc.x),
		_display->window->get_y(_menu_frame->sprite, menu_fc.y));

	const Component p_fc{(*_display->layout)["roster:preview_frame"]};
	_preview_frame =
		std::make_unique<Frame>(_display->ui_texture, WindowFrameType::NORMAL,
			p_fc.w, p_fc.h, p_fc.colour, p_fc.background, p_fc.alpha);
	_preview_frame->setPosition(
		_display->window->get_x(_preview_frame->sprite, p_fc.x),
		_display->window->get_y(_preview_frame->sprite, p_fc.y));

	// Clear the window
	_window->clear();

	// Play the background movie!
	_display->fit_bg_movie();
	_display->start_bg_movie();

	_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
	std::optional<std::vector<MenuEntry>::const_iterator> selected{
		_menu->items.begin()};

	// And do the main loop
	sf::Event event{};
	while (_window->isOpen()) {
		while (_window->pollEvent(event)) {

			// Check for Window Close
			if (event.type == sf::Event::Closed)
				_window->close();

			// Handle enabling help overlay
			if (_system->input->check(WindowInput::SHOW_CONTROLS, event)) {
				_display->show_overlay();
				continue;
			} else
				_display->hide_overlay();

			if (_show_changed) {
				auto dialog_input{_changed->handle_input(event)};
				if (dialog_input) {
					if (dialog_input.value() == WindowDialogButton::CLOSE) {
						_show_changed = false;
						_display->set_input_mode(
							WindowInputMode::NAVIGATE_MENU);
						return std::nullopt;
					} else if (dialog_input.value() == WindowDialogButton::OK) {
						_show_changed = false;
						_display->set_input_mode(
							WindowInputMode::NAVIGATE_MENU);
						return std::nullopt;
					}
				};
			} else if (_show_legated) {
				auto dialog_input{_legated->handle_input(event)};
				if (dialog_input) {
					if (dialog_input.value() == WindowDialogButton::CLOSE) {
						_show_legated = false;
						_display->set_input_mode(
							WindowInputMode::NAVIGATE_MENU);
						return std::nullopt;
					} else if (dialog_input.value() == WindowDialogButton::OK) {
						_show_legated = false;
						_display->set_input_mode(
							WindowInputMode::NAVIGATE_MENU);
						return std::nullopt;
					}
				};
			} else {

				if (_system->input->check(WindowInput::CANCEL, event))
					return std::nullopt;

				if (_system->input->check(WindowInput::BACK, event))
					return std::nullopt;

				if (_system->input->check(WindowInput::UP, event))
					selected = _menu->choose_previous();
				else if (_system->input->check(WindowInput::DOWN, event))
					selected = _menu->choose_next();
				else if (_system->input->check(WindowInput::MOVE, event))
					selected =
						_menu->set_mouse_selected(static_cast<sf::Vector2f>(
							sf::Mouse::getPosition(*_window)));
				else if (_system->input->check(WindowInput::CONFIRM, event)) {

					// We have selected something from the menu
					if (selected) {
						const MenuItem option_chosen{(*selected.value()).item};
						if (option_chosen == MenuItem::EC_RETURN_EDIT) {
							return MenuItem::EC_RETURN_EDIT;
						} else if (option_chosen == MenuItem::EC_CHANGE_NAME) {

							auto change_name{
								std::make_unique<ChangeName>(_system, _display,
									_graphics, _cur_char.value()->get_name())};
							auto new_name{change_name->start()};
							if (new_name) {

								// Update character name and resave the
								// character!
								std::string changed_name{new_name.value()};
								_cur_char.value()->set_name(changed_name);
								auto character{*_cur_char.value()};
								_game->update_char(_game->get_id(),
									current_character_idx, character);
								_game->reload_char(current_character_idx);
							};
							change_name->stop();
						} else if (option_chosen == MenuItem::EC_CHANGE_CLASS) {
							auto character{*_cur_char.value()};
							auto change_class{std::make_unique<ChangeClass>(
								_system, _display, _graphics, &character)};
							auto new_class{change_class->start()};
							if (new_class) {

								// Can't select same class in the change_class
								// module - it returns nullopt if you do
								character.change_class(new_class.value());
								_game->update_char(_game->get_id(),
									current_character_idx, character);
								_game->reload_char(current_character_idx);

								_show_changed = true;
								_display->set_input_mode(
									WindowInputMode::NAVIGATE_MENU);
							}
							change_class->stop();
						} else if (option_chosen ==
								   MenuItem::EC_LEGATE_CHARACTER) {
							auto character{*_cur_char.value()};
							auto legate{std::make_unique<Legate>(
								_system, _display, _graphics, &character)};
							auto legated{legate->start()};
							if (legated) {

								character.legate(legated.value());
								_game->update_char(_game->get_id(),
									current_character_idx, character);
								_game->reload_char(current_character_idx);

								_show_legated = true;
								_display->set_input_mode(
									WindowInputMode::NAVIGATE_MENU);
							}
							legate->stop();
						}

						_display->generate("character_edit");
						_display->set_input_mode(
							WindowInputMode::NAVIGATE_MENU);
					};
				}
			}
		}

		_window->clear();

		// Update Background Movie
		_display->start_bg_movie();
		_display->update_bg_movie();
		_display->draw_bg_movie();

		_draw();
		_window->display();
	}

	return std::nullopt;
}

auto Sorcery::Edit::stop() -> void {

	_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);

	// Stop the background movie!
	_display->stop_bg_movie();
}

auto Sorcery::Edit::_draw() -> void {

	// Display Components
	_display->display("character_edit");

	// Menu Frame
	_window->draw(*_menu_frame);

	// And the Menu
	_menu->generate((*_display->layout)["character_edit:menu"]);
	const sf::Vector2f menu_pos((*_display->layout)["character_edit:menu"].x,
		(*_display->layout)["character_edit:menu"].y);
	_menu->setPosition(menu_pos);
	_window->draw(*_menu);

	// Character Preview
	_window->draw(*_preview_frame);
	if (_char_panel->valid) {
		_char_panel->setPosition(
			(*_display->layout)["character_edit:info_panel"].x,
			(*_display->layout)["character_edit:info_panel"].y);
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
