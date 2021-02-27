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

#include "training.hpp"

// Standard Constructor
Sorcery::Training::Training(System &system, Display &display, Graphics &graphics)
	: _system{system}, _display{display}, _graphics{graphics} {

	// Get the Window and Graphics to Display
	_window = _display.window->get_window();

	// Get the Display Components
	_display.generate_components("training_grounds");

	_menu = std::make_shared<Menu>(_system, _display, _graphics, MenuType::TRAINING_GROUNDS);
}

// Standard Destructor
Sorcery::Training::~Training() {

	_display.stop_background_movie();
}

auto Sorcery::Training::start() -> std::optional<MenuItem> {

	Component background_c{(*_display.layout)["training_grounds:background_image"]};
	sf::IntRect background_rect(1147, 249, 773, 388);
	_background.setTexture(_system.resources->textures[TOWN_TEXTURE]);
	_background.setTextureRect(background_rect);
	_background.setScale(background_c.scale, background_c.scale);
	_background.setPosition(
		_display.window->get_x(_background, background_c.x), _display.window->get_y(_background, background_c.y));

	Component menu_frame_c{(*_display.layout)["training_grounds:menu_frame"]};
	_menu_frame = std::make_unique<Frame>(
		_display.ui_texture, WindowFrameType::NORMAL, menu_frame_c.w, menu_frame_c.h, menu_frame_c.alpha);
	_menu_frame->setPosition(_display.window->get_x(_menu_frame->sprite, menu_frame_c.x),
		_display.window->get_y(_menu_frame->sprite, menu_frame_c.y));

	// Clear the window
	_window->clear();

	// Play the background movie!
	_display.start_background_movie();

	_display.window->input_mode = WindowInputMode::NORMAL;
	std::optional<std::vector<MenuEntry>::const_iterator> selected_option{_menu->items.begin()};

	// And do the main loop
	sf::Event event{};
	while (_window->isOpen()) {
		while (_window->pollEvent(event)) {

			// Check for Window Close
			if (event.type == sf::Event::Closed)
				return std::nullopt;

			if (_system.input->check_for_event(WindowInput::CANCEL, event))
				return std::nullopt;

			if (_system.input->check_for_event(WindowInput::UP, event))
				selected_option = _menu->choose_previous();
			else if (_system.input->check_for_event(WindowInput::DOWN, event))
				selected_option = _menu->choose_next();
			else if (_system.input->check_for_event(WindowInput::MOVE, event))
				selected_option =
					_menu->set_mouse_selected(static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
			else if (_system.input->check_for_event(WindowInput::CONFIRM, event)) {

				// We have selected something from the menu
				if (selected_option) {
					const MenuItem option_chosen{(*selected_option.value()).item};
					if (option_chosen == MenuItem::TR_EDGE_OF_TOWN) {
						return MenuItem::ET_LEAVE_GAME;
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

auto Sorcery::Training::stop() -> void {

	_display.window->input_mode = WindowInputMode::CASTLE;
	_display.stop_background_movie();
}

auto Sorcery::Training::_draw() -> void {

	// Display Components
	_display.display_components("training_grounds");

	// Custom Layering
	_window->draw(_background);
	_window->draw(*_menu_frame);

	// And the Menu
	double lerp = _graphics.animation->colour_lerp;
	_menu->generate((*_display.layout)["training_grounds:menu"], lerp);
	const sf::Vector2f menu_pos(
		(*_display.layout)["training_grounds:menu"].x, (*_display.layout)["training_grounds:menu"].y);
	_menu->setPosition(menu_pos);
	_window->draw(*_menu);

	// And finally the Cursor
	_display.display_cursor();
}
