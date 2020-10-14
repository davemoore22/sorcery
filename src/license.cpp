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

#include "license.hpp"

// Standard Constructor
Sorcery::License::License (System& system, Display& display, Graphics& graphics):  _system {system},
	_display {display}, _graphics {graphics} {

	// Get the Window and Graphics to Display
	_window = _display.window->get_window();

	// Load the Fonts
	_mono_system_font.loadFromFile(_system.files->get_path_as_string(MONO_FONT_FILE));
	_proportional_system_font.loadFromFile(_system.files->get_path_as_string(PROPORTIONAL_FONT_FILE));

	// Load the Background Movie
	_background_movie.openFromFile(_system.files->get_path_as_string(MENU_VIDEO));

	// setup text file
	_textfile = system.resources->license_file;
	_current_line = 1;
}


// Standard Destructor
Sorcery::License::~License() {
	if (_background_movie.getStatus() == sfe::Playing)
		_background_movie.stop();
}

auto Sorcery::License::start() -> void {

	// Clear the window
	_window->clear();

	// Get Constituent Parts for the Display
	Component frame_c {(*_display.layout)["license:gui_frame"]};
	Component frame_top_c {(*_display.layout)["license:gui_frame_title"]};

	// Generate the frame
	sf::RenderTexture frame_rt;
	sf::Texture frame_t;
	sf::Sprite frame {_display.window->get_gui_frame(frame_rt, frame_t, frame_c.w, frame_c.h, frame_c.alpha)};
	const sf::Vector2f pos(_display.window->get_x(frame, frame_c.x), _display.window->get_y(frame, frame_c.y));
	frame.setPosition(pos);

	sf::RenderTexture frame_top_rt;
	sf::Texture frame_top_t;
	sf::Sprite frame_top {_display.window->get_gui_frame(frame_top_rt, frame_top_t, frame_top_c.w, frame_top_c.h,
		frame_top_c.alpha)};
	const sf::Vector2f pos_top(_display.window->get_x(frame_top, frame_top_c.x),
		_display.window->get_y(frame_top, frame_top_c.y));
	frame_top.setPosition(pos_top);

	// Get the Cursor
	_cursor = _display.window->get_cursor();

	// Scale the Movie
	_background_movie.fit(0, 0, _window->getSize().x, _window->getSize().y);

	// Play the background movie!
	if (_background_movie.getStatus() == sfe::Stopped)
		_background_movie.play();

	_display.window->input_mode = WindowInputMode::DISPLAY_TEXT_FILE;

	// And do the main loop
	sf::Event event {};
	const int lines_to_display {frame_c.h - 9};
	while (_window->isOpen()) {
		while (_window->pollEvent(event)) {

			// Check for Window Close
			if (event.type == sf::Event::Closed)
				return;

			if (_system.input->check_for_event(WindowInput::CANCEL, event)) {
				return;
			} else if (_system.input->check_for_event(WindowInput::DOWN, event)) {
				if (_current_line < _textfile->size())
					++_current_line;
			} else if (_system.input->check_for_event(WindowInput::CONFIRM, event)) {
				if (_current_line < _textfile->size())
					++_current_line;
			} else if (_system.input->check_for_event(WindowInput::UP, event)) {
				if (_current_line > 1)
					_current_line--;
			} else if (_system.input->check_for_event(WindowInput::PAGE_DOWN, event)) {
				if (_current_line < (_textfile->size() - lines_to_display))
					_current_line += lines_to_display;
				else
					_current_line = _textfile->size();
			} else if (_system.input->check_for_event(WindowInput::PAGE_UP, event)) {
				if (_current_line >= lines_to_display)
					_current_line -= lines_to_display;
				else
					_current_line = 1;
				break;
			} else if (_system.input->check_for_event(WindowInput::HOME, event)) {
				_current_line = 1;
			} else if (_system.input->check_for_event(WindowInput::END, event)) {
				_current_line = _textfile->size() - 1;
			}
		}

		if (_background_movie.getStatus() == sfe::Stopped) {
			_background_movie.play();
		}
		_background_movie.update();

		_window->clear();
		_window->draw(_background_movie);

		_draw(frame, frame_top);
		_window->display();
	}
}

auto Sorcery::License::stop() -> void {

	if (_background_movie.getStatus() == sfe::Playing)
		_background_movie.stop();
}

auto Sorcery::License::_draw(sf::Sprite &frame, sf::Sprite &frame_top) -> void {

	_window->draw(frame);
	_window->draw(frame_top);
	sf::Text title_text;
	_display.window->draw_centered_text(title_text, (*_display.layout)["license:gui_frame_title_text"]);
	std::string progress = _textfile->get_reading_progress(_current_line);
	sf::Text progress_text;
	_display.window->draw_right_text(progress_text, (*_display.layout)["license:license_file_progress"], progress);
	_display_file_contents();
}

auto Sorcery::License::_display_file_contents() const -> void {

	const Component frame_c {(*_display.layout)["license:gui_frame"]};
	Component text_c {(*_display.layout)["license:license_file_text"]};
	const int lines_to_display {text_c.h};
	const int top_y {text_c.y};

	// Check for approaching end of file
	const int end_line = [&] {
		if (_current_line < (_textfile->size() - lines_to_display))
			return static_cast<int>(_current_line + lines_to_display);
		else
			return static_cast<int>(_textfile->size());
	}();

	// File Contents
	for (auto y = _current_line; y < end_line; ++y) {
		std::string line_contents {(*_textfile)[y]};
		sf::Text line_text;
		line_text.setString(line_contents);
		text_c.y = top_y + ((y - _current_line) * _display.window->get_cell_height());
		_display.window->draw_left_text(line_text, text_c);
	}
}
