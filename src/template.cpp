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

// Template File for new Game Screen/Modules

#include "template.hpp"

// Standard Constructor
Sorcery::Template::Template(System *system, Display *display, Graphics *graphics)
	: _system{system}, _display{display}, _graphics{graphics} {

	// Get the Window and Graphics to Display
	_window = _display->window->get_window();

	// Setup Components
	_title_text = sf::Text();
}

// Standard Destructor
Sorcery::Template::~Template() {

	_display->stop_background_movie();
}

auto Sorcery::Template::start() -> void {

	// Clear the window
	_window->clear();

	// Get Constituent Parts for the Display
	Component outside_frame_c{(*_display->layout)["compendium:gui_frame"]};
	Component title_frame_c{(*_display->layout)["compendium:gui_frame_title"]};

	// Generate the frame
	_outside_frame = std::make_unique<Frame>(_display->ui_texture, WindowFrameType::NORMAL,
		outside_frame_c.w, outside_frame_c.h, outside_frame_c.colour, outside_frame_c.alpha);
	_title_frame = std::make_unique<Frame>(_display->ui_texture, WindowFrameType::NORMAL,
		title_frame_c.w, title_frame_c.h, title_frame_c.colour, title_frame_c.alpha);
	_outside_frame->setPosition(_display->window->get_x(_outside_frame->sprite, outside_frame_c.x),
		_display->window->get_y(_outside_frame->sprite, outside_frame_c.y));
	_title_frame->setPosition(_display->window->get_x(_title_frame->sprite, title_frame_c.x),
		_display->window->get_y(_title_frame->sprite, title_frame_c.y));

	// Play the background movie!
	_display->start_background_movie();

	_display->set_input_mode(WindowInputMode::DISPLAY_TEXT_FILE);

	// And do the main loop
	sf::Event event{};
	while (_window->isOpen()) {
		while (_window->pollEvent(event)) {

			// Check for Window Close
			if (event.type == sf::Event::Closed)
				return;

			if (_system->input->check_for_event(WindowInput::CANCEL, event)) {
				return;
			}
		}

		_window->clear();

		_display->start_background_movie();
		_display->update_background_movie();
		_display->draw_background_movie();

		_draw();
		_window->display();
	}
}

auto Sorcery::Template::stop() -> void {
	_display->stop_background_movie();
}

auto Sorcery::Template::_draw() -> void {
	_window->draw(*_outside_frame);
	_window->draw(*_title_frame);
	_display->window->draw_text(
		_title_text, (*_display->layout)["compendium:gui_frame_title_text"]);

	// Always draw the following
	_display->display_cursor();
}
