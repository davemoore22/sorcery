// Copyright (C) 2021 Dave Moore
//
// This file is part of Sorcery: Dreams of the Mad Overlord.
//
// Sorcery: Dreams of the Mad Overlord is free software: you can redistribute
// it and/or modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation, either version 2 of the License,
// or (at your option) any later version.
//
// Sorcery: Dreams of the Mad Overlord is distributed in the hope that it wil
// be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
// Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Sorcery: Dreams of the Mad Overlord.  If not,
// see <http://www.gnu.org/licenses/>.
//
// If you modify this program, or any covered work, by linking or combining
// it with the libraries referred to in README (or a modified version of
// said libraries), containing parts covered by the terms of said libraries,
// the licensors of this program grant you additional permission to convey
// the resulting work.

#include "gamemenu.hpp"

// Standard Constructor
Sorcery::GameMenu::GameMenu(System *system, Display *display, Graphics *graphics)
	: _system{system}, _display{display}, _graphics{graphics} {

	// Get the Window and Graphics to Display
	_window = _display->window->get_window();

	// Setup Custom Components
	_castle_menu = std::make_shared<Menu>(_system, _display, _graphics, MenuType::CASTLE);
	_edge_menu = std::make_shared<Menu>(_system, _display, _graphics, MenuType::EDGE_OF_TOWN);

	_menu_stage = GameMenuType::CASTLE;

	_dialog_leave_game = std::make_shared<Dialog>(_system, _display, _graphics,
		(*_display->layout)["castle:dialog_leave_game"],
		(*_display->layout)["castle:dialog_leave_game_text"], WindowDialogType::CONFIRM);
	_dialog_leave_game->setPosition((*_display->layout)["castle:dialog_leave_game"].x,
		(*_display->layout)["castle:dialog_leave_game"].y);

	// Modules
	_status_bar = std::make_unique<StatusBar>(_system, _display, _graphics);
	_training = std::make_shared<Training>(system, display, graphics);
}

// Standard Destructor
Sorcery::GameMenu::~GameMenu() {}

// Start/Continue a new Game
auto Sorcery::GameMenu::start(bool new_game) -> std::optional<MenuItem> {

	_menu_stage = GameMenuType::CASTLE;

	// Get the Background Display Components and load them into Display module storage (not local -
	// and note that due to the way both menus are combined in this class, we need to have the
	// menustage set first in this case and this case only)
	_display->generate_components("castle");

	// Clear the window
	_window->clear();

	// Get the background - complicated since this is dependent upon which screen we are in
	const Component bg_c{(*_display->layout)["castle:background"]};
	sf::IntRect castle_bg_rect{};
	castle_bg_rect.width = std::stoi(bg_c["source_w"].value());
	castle_bg_rect.height = std::stoi(bg_c["source_h"].value());
	castle_bg_rect.top = 0;
	castle_bg_rect.left =
		std::stoi(bg_c["source_w"].value()) * std::stoi(bg_c["source_index_castle"].value());

	sf::IntRect edge_bg_rect{};
	edge_bg_rect.width = std::stoi(bg_c["source_w"].value());
	edge_bg_rect.height = std::stoi(bg_c["source_h"].value());
	edge_bg_rect.top = 0;
	edge_bg_rect.left =
		std::stoi(bg_c["source_w"].value()) * std::stoi(bg_c["source_index_edge"].value());

	// const sf::IntRect castle_bg_rect const sf::IntRect castle_bg_rect(125, 249, 773, 388);
	// const sf::IntRect edge_bg_rect(1147, 249, 773, 388);
	_castle_bg.setTexture(_system->resources->textures[GraphicsTexture::TOWN]);
	_edge_bg.setTexture(_system->resources->textures[GraphicsTexture::TOWN]);

	_castle_bg.setTextureRect(castle_bg_rect);
	_castle_bg.setScale(std::stof(bg_c["scale_x"].value()), std::stof(bg_c["scale_y"].value()));
	//_castle_bg.setScale(bg_c.scale, bg_c.scale);
	_castle_bg.setPosition(
		_display->window->get_x(_castle_bg, bg_c.x), _display->window->get_y(_castle_bg, bg_c.y));
	_castle_bg.setTexture(_system->resources->textures[GraphicsTexture::TOWN]);
	_edge_bg.setTextureRect(edge_bg_rect);
	//_edge_bg.setScale(bg_c.scale, bg_c.scale);
	_edge_bg.setScale(std::stof(bg_c["scale_x"].value()), std::stof(bg_c["scale_y"].value()));
	_edge_bg.setPosition(
		_display->window->get_x(_edge_bg, bg_c.x), _display->window->get_y(_edge_bg, bg_c.y));

	// Generate the custom frames
	const Component castle_mf_c{(*_display->layout)["castle:castle_menu_frame"]};
	const Component edge_mf_c{(*_display->layout)["castle:edge_menu_frame"]};
	_castle_frame =
		std::make_unique<Frame>(_display->ui_texture, WindowFrameType::NORMAL, castle_mf_c.w,
			castle_mf_c.h, castle_mf_c.colour, castle_mf_c.background, castle_mf_c.alpha);
	_edge_frame = std::make_unique<Frame>(_display->ui_texture, WindowFrameType::NORMAL,
		edge_mf_c.w, edge_mf_c.h, edge_mf_c.colour, edge_mf_c.background, edge_mf_c.alpha);
	_castle_frame->setPosition(_display->window->get_x(_castle_frame->sprite, castle_mf_c.x),
		_display->window->get_y(_castle_frame->sprite, castle_mf_c.y));
	_edge_frame->setPosition(_display->window->get_x(_edge_frame->sprite, edge_mf_c.x),
		_display->window->get_y(_edge_frame->sprite, edge_mf_c.y));

	const Component status_bar_c{(*_display->layout)["status_bar:status_bar"]};
	_status_bar->setPosition(_display->window->get_x(_status_bar->sprite, status_bar_c.x),
		_display->window->get_y(_status_bar->sprite, status_bar_c.y));

	// And do the main loop
	_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
	std::optional<std::vector<MenuEntry>::const_iterator> castle_option{
		_castle_menu->items.begin()};
	std::optional<std::vector<MenuEntry>::const_iterator> edge_option{_edge_menu->items.begin()};
	sf::Event event{};
	while (_window->isOpen()) {
		while (_window->pollEvent(event)) {

			// If we are in normal input mode
			if (_display->get_input_mode() == WindowInputMode::NAVIGATE_MENU) {

				// Check for Window Close
				if (event.type == sf::Event::Closed)
					_window->close();

				// Handle enabling help overlay
				if (_system->input->check_for_event(WindowInput::SHOW_CONTROLS, event)) {
					_display->show_overlay();
					continue;
				} else
					_display->hide_overlay();

				// And handle input on the main menu
				if (_menu_stage == GameMenuType::CASTLE) {
					if (_system->input->check_for_event(WindowInput::UP, event))
						castle_option = _castle_menu->choose_previous();
					else if (_system->input->check_for_event(WindowInput::DOWN, event))
						castle_option = _castle_menu->choose_next();
					else if (_system->input->check_for_event(WindowInput::MOVE, event))
						castle_option = _castle_menu->set_mouse_selected(
							static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
					else if (_system->input->check_for_event(WindowInput::CONFIRM, event)) {

						// We have selected something from the menu
						if (castle_option) {
							if (const MenuItem option_chosen{(*castle_option.value()).item};
								option_chosen == MenuItem::CA_EDGE_OF_TOWN) {
								_menu_stage = GameMenuType::EDGE_OF_TOWN;
							}
						}
					} else if ((_system->input->check_for_event(WindowInput::CANCEL, event)) ||
							   ((_system->input->check_for_event(WindowInput::BACK, event)))) {
						_display->set_input_mode(WindowInputMode::CONFIRM_LEAVE_GAME);
						_yes_or_no = WindowConfirm::NO;
					}
				} else if (_menu_stage == GameMenuType::EDGE_OF_TOWN) {
					if (_system->input->check_for_event(WindowInput::UP, event))
						edge_option = _edge_menu->choose_previous();
					else if (_system->input->check_for_event(WindowInput::DOWN, event))
						edge_option = _edge_menu->choose_next();
					else if (_system->input->check_for_event(WindowInput::MOVE, event))
						edge_option = _edge_menu->set_mouse_selected(
							static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
					else if (_system->input->check_for_event(WindowInput::CONFIRM, event)) {

						// We have selected something from the menu
						if (edge_option) {
							if (const MenuItem option_chosen{(*edge_option.value()).item};
								option_chosen == MenuItem::ET_CASTLE) {
								_menu_stage = GameMenuType::CASTLE;
							} else if (option_chosen == MenuItem::ET_LEAVE_GAME) {
								_display->set_input_mode(WindowInputMode::CONFIRM_LEAVE_GAME);
								_yes_or_no = WindowConfirm::NO;
							} else if (option_chosen == MenuItem::ET_MAZE) {
								return MenuItem::ET_MAZE;
							} else if (option_chosen == MenuItem::ET_TRAIN) {
								_training->start();
								_training->stop();
								_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
							}
						}
					} else if ((_system->input->check_for_event(WindowInput::CANCEL, event)) ||
							   ((_system->input->check_for_event(WindowInput::BACK, event)))) {
						_display->set_input_mode(WindowInputMode::CONFIRM_LEAVE_GAME);
						_yes_or_no = WindowConfirm::NO;
					}
				}
			} else if (_display->get_input_mode() == WindowInputMode::CONFIRM_LEAVE_GAME) {

				// Check for Window Close
				if (event.type == sf::Event::Closed)
					_window->close();

				if (_system->input->check_for_event(WindowInput::SHOW_CONTROLS, event)) {
					_display->show_overlay();
					continue;
				} else
					_display->hide_overlay();

				// All we can do is select Y or N
				if (_system->input->check_for_event(WindowInput::LEFT, event))
					_dialog_leave_game->toggle_highlighted();
				else if (_system->input->check_for_event(WindowInput::RIGHT, event))
					_dialog_leave_game->toggle_highlighted();
				else if (_system->input->check_for_event(WindowInput::YES, event))
					_dialog_leave_game->set_selected(WindowDialogButton::YES);
				else if (_system->input->check_for_event(WindowInput::NO, event))
					_dialog_leave_game->set_selected(WindowDialogButton::NO);
				else if (_system->input->check_for_event(WindowInput::CANCEL, event))
					_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
				else if (_system->input->check_for_event(WindowInput::BACK, event))
					_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
				else if (_system->input->check_for_event(WindowInput::MOVE, event))
					_dialog_leave_game->check_for_mouse_move(
						static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
				else if (_system->input->check_for_event(WindowInput::CONFIRM, event)) {
					std::optional<WindowDialogButton> button_chosen{
						_dialog_leave_game->check_if_option_selected(
							static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)))};

					// Mouse click only
					if (button_chosen) {
						if (button_chosen.value() == WindowDialogButton::YES) {
							_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
							return MenuItem::ET_LEAVE_GAME;
						} else if (button_chosen.value() == WindowDialogButton::NO)
							_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);

					} else {

						// Button/Keyboard
						if (_dialog_leave_game->get_selected() == WindowDialogButton::YES) {
							_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
							return MenuItem::ET_LEAVE_GAME;
						} else if (_dialog_leave_game->get_selected() == WindowDialogButton::NO)
							_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
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

auto Sorcery::GameMenu::stop() -> void {}

auto Sorcery::GameMenu::_draw() -> void {

	// Custom Components
	_display->display_components("castle", _menu_stage);
	_window->draw(*_status_bar);

	if (_menu_stage == GameMenuType::CASTLE) {
		_window->draw(*_castle_frame);
	} else if (_menu_stage == GameMenuType::EDGE_OF_TOWN) {
		_window->draw(*_edge_frame);
	}

	// And the Menu
	const double lerp{_graphics->animation->colour_lerp};
	if (_menu_stage == GameMenuType::CASTLE) {
		_castle_menu->generate((*_display->layout)["castle:castle_menu"], lerp);
		const sf::Vector2f menu_pos((*_display->layout)["castle:castle_menu"].x,
			(*_display->layout)["castle:castle_menu"].y);
		_castle_menu->setPosition(menu_pos);
		_window->draw(*_castle_menu);
		if (_display->get_input_mode() == WindowInputMode::CONFIRM_LEAVE_GAME) {
			_dialog_leave_game->update();
			_window->draw(*_dialog_leave_game);
		}
	} else if (_menu_stage == GameMenuType::EDGE_OF_TOWN) {
		_edge_menu->generate((*_display->layout)["castle:edge_menu"], lerp);
		const sf::Vector2f menu_pos(
			(*_display->layout)["castle:edge_menu"].x, (*_display->layout)["castle:edge_menu"].y);
		_edge_menu->setPosition(menu_pos);
		_window->draw(*_edge_menu);
		if (_display->get_input_mode() == WindowInputMode::CONFIRM_LEAVE_GAME) {
			_dialog_leave_game->update();
			_window->draw(*_dialog_leave_game);
		}
	}

	// Always draw the following
	_display->display_overlay();
	_display->display_cursor();
}
