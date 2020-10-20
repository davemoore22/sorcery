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

#include "options.hpp"

// Standard Constructor
Sorcery::Options::Options (System& system, Display& display, Graphics& graphics):  _system {system},
	_display {display}, _graphics {graphics} {

	// Get the Window and Graphics to Display
	_window = _display.window->get_window();

	// Load the Fonts
	_mono_system_font.loadFromFile(_system.files->get_path_as_string(MONO_FONT_FILE));
	_proportional_system_font.loadFromFile(_system.files->get_path_as_string(PROPORTIONAL_FONT_FILE));

	// Load the Background Movie
	_background_movie.openFromFile(_system.files->get_path_as_string(MENU_VIDEO));

	// Setup Components
	_title_text = sf::Text();

	_options_menu = std::make_shared<Menu>(_system, _display, _graphics, MenuType::OPTIONS);
	_option_on = Component((*_display.layout)["options:option_on"]);
	_option_off = Component((*_display.layout)["options:option_off"]);
}

// Standard Destructor
Sorcery::Options::~Options() {
	if (_background_movie.getStatus() == sfe::Playing)
		_background_movie.stop();
}


auto Sorcery::Options::start() -> void {

	// On entry store the current settings
	_system.config->store_current_settings();

	// Clear the window
	_window->clear();

	// Get Constituent Parts for the Display
	Component frame_c {(*_display.layout)["options:gui_frame"]};
	Component frame_top_c {(*_display.layout)["options:gui_frame_title"]};

	// Generate the frame
	sf::RenderTexture frame_rt;
	sf::Texture frame_t;
	sf::RenderTexture frame_top_rt;
	sf::Texture frame_top_t;
	_frame = sf::Sprite(_display.window->get_gui_frame(frame_rt, frame_t, frame_c.w, frame_c.h, frame_c.alpha));
	const sf::Vector2f pos(_display.window->get_x(_frame, frame_c.x), _display.window->get_y(_frame, frame_c.y));
	_frame.setPosition(pos);
	_frame_top = sf::Sprite(_display.window->get_gui_frame(frame_top_rt, frame_top_t, frame_top_c.w, frame_top_c.h,
		frame_top_c.alpha));
	const sf::Vector2f pos_top(_display.window->get_x(_frame_top, frame_top_c.x),
		_display.window->get_y(_frame_top, frame_top_c.y));
	_frame_top.setPosition(pos_top);

	// Get the Cursor
	_cursor = _display.window->get_cursor();

	// Scale the Movie
	_background_movie.fit(0, 0, _window->getSize().x, _window->getSize().y);

	// Play the background movie!
	if (_background_movie.getStatus() == sfe::Stopped)
		_background_movie.play();

	// And select the first option by default;
	_display.window->input_mode = WindowInputMode::GAME_OPTIONS;
	_options_menu->selected = _options_menu->items.begin();
	std::optional<std::vector<MenuEntry>::const_iterator> selected_option {_options_menu->items.begin()};

	// And do the main loop
	sf::Event event {};
	while (_window->isOpen()) {
		while (_window->pollEvent(event)) {

			// Check for Window Close
			if (event.type == sf::Event::Closed)
				return;

			if (_system.input->check_for_event(WindowInput::CANCEL, event))
				return;

			// And handle input on the main menu
			if (_system.input->check_for_event(WindowInput::UP, event)) {
					selected_option = _options_menu->choose_previous();
			} else if (_system.input->check_for_event(WindowInput::DOWN, event)) {
					selected_option = _options_menu->choose_next();
			} else if (_system.input->check_for_event(WindowInput::MOVE, event)) {
					selected_option =
						_options_menu->set_mouse_selected(static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
			} else if (_system.input->check_for_event(WindowInput::CONFIRM, event)) {
				if (selected_option) {
					if ((*_options_menu->selected).type == MenuItemType::ENTRY) {
						ConfigOption config_to_toggle = (*_options_menu->selected).config;
						if ((config_to_toggle == ConfigOption::STRICT_MODE) &&
							(!(*_system.config)[ConfigOption::STRICT_MODE])) {

							// Handle Strict Mode Toggling
							_system.config->set_strict_mode();
							(*_system.config)[ConfigOption::STRICT_MODE] = true;

						} if ((config_to_toggle == ConfigOption::RECOMMENDED_MODE) &&
							(!(*_system.config)[ConfigOption::RECOMMENDED_MODE])) {

							// Handle Recommended Toggling
							_system.config->set_recommended_mode();
							(*_system.config)[ConfigOption::RECOMMENDED_MODE] = true;

						} else {

							// And toggling off strict mode
							(*_system.config)[config_to_toggle] = !(*_system.config)[config_to_toggle];
						}
					} else if ((*_options_menu->selected).type == MenuItemType::SAVE) {
						_system.config->save();
						return;
					} else if ((*_options_menu->selected).type == MenuItemType::CANCEL) {
						_system.config->load();
						return;
					}
				}
			}
		}

		if (_background_movie.getStatus() == sfe::Stopped) {
			_background_movie.play();
		}
		_background_movie.update();

		_window->clear();
		_window->draw(_background_movie);

		_draw();
		_window->display();
	}
}


auto Sorcery::Options::stop() -> void {

	if (_background_movie.getStatus() == sfe::Playing)
		_background_movie.stop();
}

auto Sorcery::Options::_draw() -> void {

	_window->draw(_frame);
	_window->draw(_frame_top);
	_display.window->draw_text(_title_text, (*_display.layout)["options:gui_frame_title_text"]);

	double lerp = _graphics.animation->colour_lerp;

	_display.window->draw_menu(_options_menu->items, _options_menu->bounds, _options_menu->selected,
		(*_display.layout)["options:options_menu"], _options_menu->get_type(), lerp);

	_display.window->draw_text(_gameplay_text, (*_display.layout)["options:subtitle_gameplay"]);
	_display.window->draw_text(_graphics_text, (*_display.layout)["options:subtitle_graphics"]);

	// Always draw the following
	_cursor.setPosition(static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
	_window->draw(_cursor);
}