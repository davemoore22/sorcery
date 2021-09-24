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

#include "engine.hpp"

// Standard Constructor
Sorcery::Engine::Engine(
	System *system, Display *display, Graphics *graphics, Game *game)
	: _system{system}, _display{display}, _graphics{graphics}, _game{game} {

	// Get the Window and Graphics to Display
	_window = _display->window->get_window();

	// Setup Custom Components
	_camp_menu = std::make_unique<Menu>(
		_system, _display, _graphics, _game, MenuType::CAMP);
	const Component menu_fc{
		(*_display->layout)["engine_base_ui:camp_menu_frame"]};
	_camp_menu_frame = std::make_unique<Frame>(_display->ui_texture,
		WindowFrameType::NORMAL, menu_fc.w, menu_fc.h, menu_fc.colour,
		menu_fc.background, menu_fc.alpha);
	_camp_menu_frame->setPosition(
		_display->window->get_x(_camp_menu_frame->sprite, menu_fc.x),
		_display->window->get_y(_camp_menu_frame->sprite, menu_fc.y));

	_confirm_exit = std::make_unique<Dialog>(_system, _display, _graphics,
		(*_display->layout)["engine_base_ui:dialog_exit"],
		(*_display->layout)["engine_base_ui:dialog_exit_text"],
		WindowDialogType::CONFIRM);
	_confirm_exit->setPosition(
		(*_display->layout)["engine_base_ui:dialog_exit"].x,
		(*_display->layout)["engine_base_ui:dialog_exit"].y);

	// Modules
	_status_bar = std::make_unique<StatusBar>(_system, _display, _graphics,
		_game, (*_display->layout)["engine_base_ui:status_bar"],
		(*_display->layout)["engine_base_ui:status_bar_outer_frame"]);
	_reorder = std::make_unique<Reorder>(
		_system, _display, _graphics, _game, MenuMode::CAMP);
	_inspect = std::make_unique<Inspect>(
		_system, _display, _graphics, _game, MenuMode::CAMP);
	_render = std::make_unique<Render>(_system, _display, _graphics, _game);
	_automap = std::make_unique<AutoMap>(_system, _display, _graphics, _game,
		(*_display->layout)["global:automap"]);

	_update_automap = false;
}

// Standard Destructor
Sorcery::Engine::~Engine() {}

auto Sorcery::Engine::start() -> int {

	_display->generate("engine_base_ui");

	// Refresh the Party characters
	_status_bar->refresh();
	_automap->refresh();

	// Generate the Custom Components
	const Component status_bar_c{
		(*_display->layout)["engine_base_ui:status_bar"]};
	_status_bar->setPosition(
		_display->window->get_x(_status_bar->sprite, status_bar_c.x),
		_display->window->get_y(_status_bar->sprite, status_bar_c.y));

	const Component automap_c{(*_display->layout)["global:automap"]};
	_automap->setPosition(automap_c.x, automap_c.y);

	// Start in camp as is tradition
	_in_camp = true;
	_show_confirm_exit = false;
	_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
	std::optional<std::vector<MenuEntry>::const_iterator> camp_option{
		_camp_menu->items.begin()};

	sf::Event event{};
	while (_window->isOpen()) {
		while (_window->pollEvent(event)) {

			// Check for Window Close
			if (event.type == sf::Event::Closed)
				return EXIT_ALL;

			// Handle enabling help overlay
			if (_system->input->check(WindowInput::SHOW_CONTROLS, event)) {
				_display->show_overlay();
				continue;
			} else
				_display->hide_overlay();

			if (_show_confirm_exit) {

				auto dialog_input{_confirm_exit->handle_input(event)};
				if (dialog_input) {
					if (dialog_input.value() == WindowDialogButton::CLOSE) {
						_display->set_input_mode(
							WindowInputMode::NAVIGATE_MENU);
						_show_confirm_exit = false;
					} else if (dialog_input.value() ==
							   WindowDialogButton::YES) {
						_window->close();
					} else if (dialog_input.value() == WindowDialogButton::NO) {
						_display->set_input_mode(
							WindowInputMode::NAVIGATE_MENU);
						_show_confirm_exit = false;
					}
				}
			} else if (_in_camp) {

				if (_system->input->check(WindowInput::CANCEL, event))
					_in_camp = false;

				if (_system->input->check(WindowInput::BACK, event))
					_in_camp = false;

				if (_system->input->check(WindowInput::UP, event))
					camp_option = _camp_menu->choose_previous();
				else if (_system->input->check(WindowInput::DOWN, event))
					camp_option = _camp_menu->choose_next();
				else if (_system->input->check(WindowInput::MOVE, event))
					camp_option = _camp_menu->set_mouse_selected(
						static_cast<sf::Vector2f>(
							sf::Mouse::getPosition(*_window)));
				else if (_system->input->check(WindowInput::CONFIRM, event)) {

					// We have selected something from the menu
					if (camp_option) {

						if (const MenuItem option_chosen{
								(*camp_option.value()).item};
							option_chosen == MenuItem::CP_LEAVE) {

							_game->save_game();
							_status_bar->refresh();
							_in_camp = false;
							_display->generate("engine_base_ui");
							_display->set_input_mode(WindowInputMode::IN_GAME);
							continue;
						} else if (option_chosen == MenuItem::CP_SAVE) {
							_game->save_game();
							return EXIT_MODULE;
						} else if (option_chosen == MenuItem::QUIT) {
							_show_confirm_exit = true;
							continue;
						} else if (option_chosen == MenuItem::CP_OPTIONS) {

							auto options{std::make_unique<Options>(
								_system, _display, _graphics)};
							auto result{options->start()};
							if (result == EXIT_ALL) {
								options->stop();
								return EXIT_ALL;
							}
							options->stop();
							_status_bar->refresh();
							_display->generate("engine_base_ui");
						} else if (option_chosen == MenuItem::CP_INSPECT) {
							_status_bar->refresh();
							auto result{_inspect->start()};
							if (result == MenuItem::ABORT) {
								_inspect->stop();
								return EXIT_ALL;
							}
							_inspect->stop();
							_status_bar->refresh();
							_display->generate("engine_base_ui");
						} else if (option_chosen == MenuItem::CP_REORDER) {
							_status_bar->refresh();
							auto new_party{_reorder->start()};
							if (new_party) {

								// TODO: handle aborts here too
								_game->state->set_party(new_party.value());
								_game->save_game();
								_game->load_game();
								_status_bar->refresh();
							}
							_reorder->stop();
							_status_bar->refresh();
							_display->generate("engine_base_ui");
						}
					}
				}
			} else {

				if (_display->get_input_mode() == WindowInputMode::IN_GAME) {

					if (_system->input->check(WindowInput::SPACE, event)) {
						_game->state->world->create();
						_update_automap = true;
					}

					if (_system->input->check(WindowInput::CANCEL, event))
						_in_camp = true;

					if (_system->input->check(WindowInput::BACK, event))
						_in_camp = true;
				}
			}

			_render->update();
			_render->render();
			if (_update_automap) {
				_automap->refresh();
				_update_automap = false;
			}
			_window->clear();
			_draw();
			_window->display();
		}
	}

	return EXIT_MODULE;
}

auto Sorcery::Engine::stop() -> void {}

auto Sorcery::Engine::_draw() -> void {

	// Draw the Render
	_render->setPosition(0, 0);
	_window->draw(*_render);

	// Custom Components
	_display->display("engine_base_ui");
	_window->draw(*_status_bar);
	_window->draw(*_automap);

	// And the Menu
	if (_in_camp) {
		_window->draw(*_camp_menu_frame);
		_camp_menu->generate((*_display->layout)["engine_base_ui:camp_menu"]);
		const sf::Vector2f menu_pos(
			(*_display->layout)["engine_base_ui:camp_menu"].x,
			(*_display->layout)["engine_base_ui:camp_menu"].y);
		_camp_menu->setPosition(menu_pos);
		_window->draw(*_camp_menu);
	}

	if (_show_confirm_exit) {
		_confirm_exit->update();
		_window->draw(*_confirm_exit);
	}

	// Always draw the following
	_display->display_overlay();
	_display->display_cursor();
}
