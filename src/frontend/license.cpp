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

#include "frontend/license.hpp"
#include "core/display.hpp"
#include "core/graphics.hpp"
#include "core/input.hpp"
#include "core/system.hpp"
#include "core/window.hpp"
#include "frontend/define.hpp"
#include "resources/componentstore.hpp"
#include "resources/resourcemanager.hpp"
#include "types/component.hpp"
#include "types/textfile.hpp"

// Standard Constructor
Sorcery::License::License(System *system, Display *display, Graphics *graphics)
	: _system{system}, _display{display}, _graphics{graphics} {

	// Get the Window and Graphics to Display
	_window = _display->window->get_window();

	// Setup text file
	_textfile = system->resources->license_file;
	_current_line = 1;

	// Setup Components
	_progress_text = sf::Text();
	_line_text = sf::Text();
}

// Standard Destructor
Sorcery::License::~License() {

	_display->stop_bg_movie();
}

auto Sorcery::License::start() -> int {

	// Get the Background Display Components and load them into Display module
	// storage (not local)
	_display->generate("license");

	// Clear the window
	_window->clear();

	// Play the background movie!
	_display->fit_bg_movie();
	_display->start_bg_movie();

	// And do the main loop
	_display->set_input_mode(WindowInputMode::DISPLAY_TEXT_FILE);
	sf::Event event{};
	const auto lines_to_display{21U};
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

			if ((_system->input->check(WindowInput::CANCEL, event)) ||
				(_system->input->check(WindowInput::BACK, event))) {
				return EXIT_MODULE;
			} else if (_system->input->check(WindowInput::DOWN, event)) {
				if (_current_line < _textfile->size())
					++_current_line;
			} else if (_system->input->check(WindowInput::CONFIRM, event)) {
				if (_current_line < _textfile->size())
					++_current_line;
			} else if (_system->input->check(WindowInput::UP, event)) {
				if (_current_line > 1)
					_current_line--;
			} else if (_system->input->check(WindowInput::PAGE_DOWN, event)) {
				if (_current_line < (_textfile->size() - lines_to_display))
					_current_line += lines_to_display;
				else
					_current_line = _textfile->size();
			} else if (_system->input->check(WindowInput::PAGE_UP, event)) {
				if (_current_line >= lines_to_display)
					_current_line -= lines_to_display;
				else
					_current_line = 1;
				break;
			} else if (_system->input->check(WindowInput::HOME, event)) {
				_current_line = 1;
			} else if (_system->input->check(WindowInput::END, event)) {
				_current_line = _textfile->size() - 1;
			}
		}

		_window->clear();

		_display->start_bg_movie();
		_display->update_bg_movie();
		_display->draw_bg_movie();

		_draw();
		_window->display();
	}

	return EXIT_MODULE;
}

auto Sorcery::License::stop() -> void {

	_display->stop_bg_movie();
}

auto Sorcery::License::_draw() -> void {

	_display->display("license");

	const auto progress{_textfile->get_progress(_current_line)};
	_display->window->draw_text(_progress_text, (*_display->layout)["license:progress"], progress);
	_display_file();

	// Always draw the following
	_display->display_overlay();
	_display->display_cursor();
}

auto Sorcery::License::_display_file() -> void {

	Component text_c{(*_display->layout)["license:file_text"]};
	const auto lines_to_display{text_c.h};
	const auto top_y{text_c.y};

	// Check for approaching end of file
	const auto end_line{std::invoke([&] {
		if (_current_line < (_textfile->size() - lines_to_display))
			return static_cast<unsigned int>(_current_line + lines_to_display);
		else
			return static_cast<unsigned int>(_textfile->size());
	})};

	// File Contents
	for (auto y = _current_line; y < end_line; ++y) {
		const auto line_contents{(*_textfile)[y]};
		_line_text.setString(line_contents);
		text_c.y = top_y + ((y - _current_line) * _display->window->get_ch());
		_display->window->draw_text(_line_text, text_c, std::string{line_contents});
	}
}
