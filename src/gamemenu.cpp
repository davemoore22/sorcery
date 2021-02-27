// Copyright (C) 2020 Dave Moore
//
// This file is part of Sorcery: Dreams of the Mad Overlord.
//
// Sorcery: Dreams of the Mad Overlord is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Sorcery: Dreams of the Mad Overlord is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Sorcery: Dreams of the Mad Overlord.  If not, see <http://www.gnu.org/licenses/>.
//
// If you modify this Program, or any covered work, by linking or combining it
// with the libraries referred to in README (or a modified version of said
// libraries), containing parts covered by the terms of said libraries, the
// licensors of this Program grant you additional permission to convey the
// resulting work.

#include "gamemenu.hpp"

// Standard Constructor
Sorcery::GameMenu::GameMenu(System &system, Display &display, Graphics &graphics)
	: _system{system}, _display{display}, _graphics{graphics} {

	// Get the Window and Graphics to Display
	_window = _display.window->get_window();

	// Setup Components
	_castle_menu = std::make_shared<Menu>(_system, _display, _graphics, MenuType::CASTLE);
	_edge_of_town_menu = std::make_shared<Menu>(_system, _display, _graphics, MenuType::EDGE_OF_TOWN);

	_menu_stage = GameMenuType::CASTLE;

	// Get the Display Components
	_display.generate_components("castle");

	_confirm_leave_game = std::make_shared<Confirm>(_system, _display, _graphics,
		(*_display.layout)["castle:confirm_leave_game_frame"], (*_display.layout)["castle:confirm_leave_game"]);

	// Modules
	_training = std::make_shared<Training>(system, display, graphics);
}

// Standard Destructor
Sorcery::GameMenu::~GameMenu() {

	_display.stop_background_movie();
}

auto Sorcery::GameMenu::start() -> std::optional<MenuItem> {

	_menu_stage = GameMenuType::CASTLE;

	// Clear the window
	_window->clear();

	// Get the background
	Component background_c{(*_display.layout)["castle:background_image"]};
	sf::IntRect castle_background_rect(125, 249, 773, 388);
	sf::IntRect edge_of_town_background_rect(1147, 249, 773, 388);
	_castle_background.setTexture(_system.resources->textures[TOWN_TEXTURE]);
	_castle_background.setTextureRect(castle_background_rect);
	_castle_background.setScale(background_c.scale, background_c.scale);
	_castle_background.setPosition(_display.window->get_x(_castle_background, background_c.x),
		_display.window->get_y(_castle_background, background_c.y));
	_edge_of_town_background.setTexture(_system.resources->textures[TOWN_TEXTURE]);
	_edge_of_town_background.setTextureRect(edge_of_town_background_rect);
	_edge_of_town_background.setScale(background_c.scale, background_c.scale);
	_edge_of_town_background.setPosition(_display.window->get_x(_edge_of_town_background, background_c.x),
		_display.window->get_y(_edge_of_town_background, background_c.y));

	// Generate the custom frames
	Component castle_menu_frame_c{(*_display.layout)["castle:castle_menu_frame"]};
	Component edge_of_town_menu_frame_c{(*_display.layout)["castle:edge_of_town_menu_frame"]};
	_castle_menu_frame = std::make_unique<Frame>(_display.ui_texture, WindowFrameType::NORMAL, castle_menu_frame_c.w,
		castle_menu_frame_c.h, castle_menu_frame_c.alpha);
	_edge_of_town_menu_frame = std::make_unique<Frame>(_display.ui_texture, WindowFrameType::NORMAL,
		edge_of_town_menu_frame_c.w, edge_of_town_menu_frame_c.h, edge_of_town_menu_frame_c.alpha);
	_castle_menu_frame->setPosition(_display.window->get_x(_castle_menu_frame->sprite, castle_menu_frame_c.x),
		_display.window->get_y(_castle_menu_frame->sprite, castle_menu_frame_c.y));
	_edge_of_town_menu_frame->setPosition(
		_display.window->get_x(_edge_of_town_menu_frame->sprite, edge_of_town_menu_frame_c.x),
		_display.window->get_y(_edge_of_town_menu_frame->sprite, edge_of_town_menu_frame_c.y));

	// Play the background movie!
	_display.start_background_movie();

	_display.window->input_mode = WindowInputMode::CASTLE;
	std::optional<std::vector<MenuEntry>::const_iterator> selected_castle_option{_castle_menu->items.begin()};
	std::optional<std::vector<MenuEntry>::const_iterator> selected_edge_of_town_option{
		_edge_of_town_menu->items.begin()};

	// And do the main loop
	sf::Event event{};
	while (_window->isOpen()) {
		while (_window->pollEvent(event)) {

			// If we are in normal input mode
			if (_display.window->input_mode == WindowInputMode::CASTLE) {

				// Check for Window Close
				if (event.type == sf::Event::Closed)
					return std::nullopt;

				// And handle input on the main menu
				if (_menu_stage == GameMenuType::CASTLE) {
					if (_system.input->check_for_event(WindowInput::UP, event))
						selected_castle_option = _castle_menu->choose_previous();
					else if (_system.input->check_for_event(WindowInput::DOWN, event))
						selected_castle_option = _castle_menu->choose_next();
					else if (_system.input->check_for_event(WindowInput::MOVE, event))
						selected_castle_option = _castle_menu->set_mouse_selected(
							static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
					else if (_system.input->check_for_event(WindowInput::CONFIRM, event)) {

						// We have selected something from the menu
						if (selected_castle_option) {
							const MenuItem option_chosen{(*selected_castle_option.value()).item};
							if (option_chosen == MenuItem::CA_EDGE_OF_TOWN) {
								_menu_stage = GameMenuType::EDGE_OF_TOWN;
							}
						}
					}

				} else if (_menu_stage == GameMenuType::EDGE_OF_TOWN) {
					if (_system.input->check_for_event(WindowInput::UP, event))
						selected_edge_of_town_option = _edge_of_town_menu->choose_previous();
					else if (_system.input->check_for_event(WindowInput::DOWN, event))
						selected_edge_of_town_option = _edge_of_town_menu->choose_next();
					else if (_system.input->check_for_event(WindowInput::MOVE, event))
						selected_edge_of_town_option = _edge_of_town_menu->set_mouse_selected(
							static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
					else if (_system.input->check_for_event(WindowInput::CONFIRM, event)) {

						// We have selected something from the menu
						if (selected_edge_of_town_option) {
							const MenuItem option_chosen{(*selected_edge_of_town_option.value()).item};
							if (option_chosen == MenuItem::ET_CASTLE) {
								_menu_stage = GameMenuType::CASTLE;
							} else if (option_chosen == MenuItem::ET_LEAVE_GAME) {
								_display.window->input_mode = WindowInputMode::LEAVE_GAME;
								_yes_or_no = WindowConfirm::NO;
								//return MenuItem::ET_LEAVE_GAME;
							} else if (option_chosen == MenuItem::ET_MAZE) {
								return MenuItem::ET_MAZE;
							} else if (option_chosen == MenuItem::ET_TRAIN) {
								_training->start();
								_training->stop();
								_display.window->input_mode = WindowInputMode::CASTLE;
							}
						}
					} else if (_system.input->check_for_event(WindowInput::CANCEL, event)) {
						_display.window->input_mode = WindowInputMode::LEAVE_GAME;
						_yes_or_no = WindowConfirm::NO;
					}
				}
			} else if (_display.window->input_mode == WindowInputMode::LEAVE_GAME) {

				// Check for Window Close
				if (event.type == sf::Event::Closed)
					return std::nullopt;

				// All we can do is select Y or N
				if (_system.input->check_for_event(WindowInput::LEFT, event))
					_confirm_leave_game->toggle_highlighted();
				else if (_system.input->check_for_event(WindowInput::RIGHT, event))
					_confirm_leave_game->toggle_highlighted();
				else if (_system.input->check_for_event(WindowInput::YES, event))
					_confirm_leave_game->currently_highlighted = WindowConfirm::YES;
				else if (_system.input->check_for_event(WindowInput::NO, event))
					_confirm_leave_game->currently_highlighted = WindowConfirm::NO;
				else if (_system.input->check_for_event(WindowInput::CANCEL, event))
					_display.window->input_mode = WindowInputMode::CASTLE;
				else if (_system.input->check_for_event(WindowInput::MOVE, event))
					_confirm_leave_game->check_for_mouse_move(
						static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
				else if (_system.input->check_for_event(WindowInput::CONFIRM, event)) {
					std::optional<WindowConfirm> option_chosen = _confirm_leave_game->check_if_option_selected(
						static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));

					// Mouse click only
					if (option_chosen) {
						if (option_chosen.value() == WindowConfirm::YES)
							return MenuItem::ET_LEAVE_GAME;
						if (option_chosen.value() == WindowConfirm::NO)
							_display.window->input_mode = WindowInputMode::CASTLE;
					} else {

						// Button/Keyboard
						if (_confirm_leave_game->currently_highlighted == WindowConfirm::YES)
							return MenuItem::ET_LEAVE_GAME;
						else if (_confirm_leave_game->currently_highlighted == WindowConfirm::NO)
							_display.window->input_mode = WindowInputMode::CASTLE;
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

	return std::nullopt;
}

auto Sorcery::GameMenu::stop() -> void {

	_display.stop_background_movie();
}

auto Sorcery::GameMenu::_draw() -> void {

	// Custom Components
	_display.display_components("castle", _menu_stage);
	if (_menu_stage == GameMenuType::CASTLE) {
		_window->draw(_castle_background);
		_window->draw(*_castle_menu_frame);
	} else if (_menu_stage == GameMenuType::EDGE_OF_TOWN) {
		_window->draw(_edge_of_town_background);
		_window->draw(*_edge_of_town_menu_frame);
	}

	// And the Menu
	double lerp = _graphics.animation->colour_lerp;
	if (_menu_stage == GameMenuType::CASTLE) {
		_castle_menu->generate((*_display.layout)["castle:castle_menu"], lerp);
		const sf::Vector2f menu_pos(
			(*_display.layout)["castle:castle_menu"].x, (*_display.layout)["castle:castle_menu"].y);
		_castle_menu->setPosition(menu_pos);
		_window->draw(*_castle_menu);
	} else if (_menu_stage == GameMenuType::EDGE_OF_TOWN) {
		_edge_of_town_menu->generate((*_display.layout)["castle:edge_of_town_menu"], lerp);
		const sf::Vector2f menu_pos(
			(*_display.layout)["castle:edge_of_town_menu"].x, (*_display.layout)["castle:edge_of_town_menu"].y);
		_edge_of_town_menu->setPosition(menu_pos);
		_window->draw(*_edge_of_town_menu);
		if (_display.window->input_mode == WindowInputMode::LEAVE_GAME)
			_confirm_leave_game->draw(lerp);
	}

	// Always draw the following
	_display.display_cursor();
}
