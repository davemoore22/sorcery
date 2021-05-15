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

#include "training.hpp"

// Standard Constructor
Sorcery::Training::Training(System *system, Display *display, Graphics *graphics, Game *game)
	: _system{system}, _display{display}, _graphics{graphics}, _game{game} {

	// Get the Window and Graphics to Display
	_window = _display->window->get_window();

	_menu = std::make_shared<Menu>(_system, _display, _graphics, MenuType::TRAINING_GROUNDS);
	_status_bar = std::make_unique<StatusBar>(_system, _display, _graphics);
	_create = std::make_unique<Create>(_system, _display, _graphics, _game);
}

// Standard Destructor
Sorcery::Training::~Training() {}

auto Sorcery::Training::start() -> std::optional<MenuItem> {

	// Get the Background Display Components and load them into Display module storage (not local)
	_display->generate_components("training_grounds");

	// Set up the Custom Components
	const Component bg_c{(*_display->layout)["training_grounds:background"]};
	// const sf::IntRect bg_rect(1147, 249, 773, 388);
	sf::IntRect bg_rect{};
	bg_rect.width = std::stoi(bg_c["source_w"].value());
	bg_rect.height = std::stoi(bg_c["source_h"].value());
	bg_rect.top = 0;
	bg_rect.left = std::stoi(bg_c["source_w"].value()) * std::stoi(bg_c["source_index"].value());

	_bg.setTexture(_system->resources->textures[GraphicsTexture::TOWN]);
	_bg.setTextureRect(bg_rect);
	_bg.setScale(std::stof(bg_c["scale_x"].value()), std::stof(bg_c["scale_y"].value()));
	//_bg.setScale(bg_c.scale, bg_c.scale);
	_bg.setPosition(_display->window->get_x(_bg, bg_c.x), _display->window->get_y(_bg, bg_c.y));

	const Component menu_fc{(*_display->layout)["training_grounds:menu_frame"]};
	_menu_frame = std::make_unique<Frame>(_display->ui_texture, WindowFrameType::NORMAL, menu_fc.w,
		menu_fc.h, menu_fc.colour, menu_fc.background, menu_fc.alpha);
	_menu_frame->setPosition(_display->window->get_x(_menu_frame->sprite, menu_fc.x),
		_display->window->get_y(_menu_frame->sprite, menu_fc.y));

	const Component status_bar_c{(*_display->layout)["status_bar:status_bar"]};
	_status_bar->setPosition(_display->window->get_x(_status_bar->sprite, status_bar_c.x),
		_display->window->get_y(_status_bar->sprite, status_bar_c.y));

	// Clear the window
	_window->clear();
	;

	_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
	std::optional<std::vector<MenuEntry>::const_iterator> selected{_menu->items.begin()};

	// And do the main loop
	sf::Event event{};
	while (_window->isOpen()) {
		while (_window->pollEvent(event)) {

			// Check for Window Close
			if (event.type == sf::Event::Closed)
				_window->close();

			// Handle enabling help overlay
			if (_system->input->check_for_event(WindowInput::SHOW_CONTROLS, event)) {
				_display->show_overlay();
				continue;
			} else
				_display->hide_overlay();

			if (_system->input->check_for_event(WindowInput::CANCEL, event))
				return std::nullopt;

			if (_system->input->check_for_event(WindowInput::BACK, event))
				return std::nullopt;

			if (_system->input->check_for_event(WindowInput::UP, event))
				selected = _menu->choose_previous();
			else if (_system->input->check_for_event(WindowInput::DOWN, event))
				selected = _menu->choose_next();
			else if (_system->input->check_for_event(WindowInput::MOVE, event))
				selected = _menu->set_mouse_selected(
					static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
			else if (_system->input->check_for_event(WindowInput::CONFIRM, event)) {

				// We have selected something from the menu
				if (selected) {
					const MenuItem option_chosen{(*selected.value()).item};
					if (option_chosen == MenuItem::TR_EDGE_OF_TOWN) {
						return MenuItem::ET_LEAVE_GAME;
					} else if (option_chosen == MenuItem::TR_CREATE) {
						_create->start();
						_create->stop();
						_display->generate_components("training_grounds");
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

auto Sorcery::Training::stop() -> void {

	_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
}

auto Sorcery::Training::_draw() -> void {

	// Display Components
	_display->display_components("training_grounds");
	_window->draw(*_status_bar);

	// Menu Frame
	_window->draw(*_menu_frame);

	// And the Menu
	_menu->generate(
		(*_display->layout)["training_grounds:menu"], _graphics->animation->colour_lerp);
	const sf::Vector2f menu_pos((*_display->layout)["training_grounds:menu"].x,
		(*_display->layout)["training_grounds:menu"].y);
	_menu->setPosition(menu_pos);
	_window->draw(*_menu);

	// And finally the Cursor
	_display->display_overlay();
	_display->display_cursor();
}
