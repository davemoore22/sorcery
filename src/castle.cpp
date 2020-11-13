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

#include "castle.hpp"

// Standard Constructor
Sorcery::Castle::Castle (System& system, Display& display, Graphics& graphics):  _system {system},
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
	_castle_menu = std::make_shared<Menu>(_system, _display, _graphics, MenuType::CASTLE);
}

// Standard Destructor
Sorcery::Castle::~Castle() {
	if (_background_movie.getStatus() == sfe::Playing)
		_background_movie.stop();
}


auto Sorcery::Castle::start() -> void {

	// Clear the window
	_window->clear();

	// Get Constituent Parts for the Display
	Component outside_frame_c {(*_display.layout)["castle:gui_frame"]};
	Component title_frame_c {(*_display.layout)["castle:gui_frame_title"]};
	Component character_frame_c {(*_display.layout)["castle:character_frame"]};
	Component menu_frame_c {(*_display.layout)["castle:menu_frame"]};

	// Get the background
	Component background_c {(*_display.layout)["castle:background_image"]};
	sf::IntRect background_rect(125, 249, 773, 388);
	_background.setTexture(_system.resources->textures[TOWN_TEXTURE]);
	_background.setTextureRect(background_rect);
	_background.setScale(background_c.scale, background_c.scale);
	_background.setPosition(_display.window->get_x(_background, background_c.x),
		_display.window->get_y(_background, background_c.y));

	// Generate the frames
	_outside_frame = std::make_unique<Frame>(_system, _display, _graphics, WindowFrameType::NORMAL, outside_frame_c.w,
		outside_frame_c.h, outside_frame_c.alpha);
	_title_frame = std::make_unique<Frame>(_system, _display, _graphics, WindowFrameType::NORMAL, title_frame_c.w,
		title_frame_c.h, title_frame_c.alpha);
	_character_frame = std::make_unique<Frame>(_system, _display, _graphics, WindowFrameType::NORMAL,
		character_frame_c.w, character_frame_c.h, character_frame_c.alpha);
	_menu_frame = std::make_unique<Frame>(_system, _display, _graphics, WindowFrameType::NORMAL,
		menu_frame_c.w, menu_frame_c.h, menu_frame_c.alpha);
	_outside_frame->setPosition(_display.window->get_x(_outside_frame->sprite, outside_frame_c.x),
		_display.window->get_y(_outside_frame->sprite, outside_frame_c.y));
	_title_frame->setPosition(_display.window->get_x(_title_frame->sprite, title_frame_c.x),
		_display.window->get_y(_title_frame->sprite, title_frame_c.y));
	_character_frame->setPosition(_display.window->get_x(_character_frame->sprite, character_frame_c.x),
		_display.window->get_y(_character_frame->sprite, character_frame_c.y));
	_menu_frame->setPosition(_display.window->get_x(_menu_frame->sprite, menu_frame_c.x),
		_display.window->get_y(_menu_frame->sprite, menu_frame_c.y));

	// Get the character legend
	_character_legend_layout[0] = Component((*_display.layout)["castle:character_legend_name"]);
	_character_legend_layout[1] = Component((*_display.layout)["castle:character_legend_class"]);
	_character_legend_layout[2] = Component((*_display.layout)["castle:character_legend_alignment"]);
	_character_legend_layout[3] = Component((*_display.layout)["castle:character_legend_ac"]);
	_character_legend_layout[4] = Component((*_display.layout)["castle:character_legend_hp"]);
	_character_legend_layout[5] = Component((*_display.layout)["castle:character_legend_status"]);
	int loop {0};
	for (auto& legend_text: _character_legend_text) {
		legend_text = sf::Text();
		legend_text.setFont(_system.resources->fonts[_character_legend_layout[loop].font]);
		legend_text.setCharacterSize(_character_legend_layout[loop].size);
		legend_text.setFillColor(sf::Color(_character_legend_layout[loop].colour));
		legend_text.setString((*_display.string)[_character_legend_layout[loop].string_key]);
		legend_text.setPosition(_character_legend_layout[loop].x, _character_legend_layout[loop].y);
		legend_text.setOrigin(0, legend_text.getLocalBounds().height / 2.0f);
		++loop;
	}

	// Get the Cursor
	_cursor = _display.window->get_cursor();

	// Scale the Movie
	_background_movie.fit(0, 0, _window->getSize().x, _window->getSize().y);

	// Play the background movie!
	if (_background_movie.getStatus() == sfe::Stopped)
		_background_movie.play();

	_display.window->input_mode = WindowInputMode::CASTLE;
	std::optional<std::vector<MenuEntry>::const_iterator> selected_option {_castle_menu->items.begin()};

	// And do the main loop
	sf::Event event {};
	while (_window->isOpen()) {
		while (_window->pollEvent(event)) {

			// Check for Window Close
			if (event.type == sf::Event::Closed)
				return;

			// And handle input on the main menu
			if (_system.input->check_for_event(WindowInput::UP, event))
				selected_option = _castle_menu->choose_previous();
			else if (_system.input->check_for_event(WindowInput::DOWN, event))
				selected_option = _castle_menu->choose_next();
			else if (_system.input->check_for_event(WindowInput::MOVE, event))
				selected_option =
					_castle_menu->set_mouse_selected(static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
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

auto Sorcery::Castle::stop() -> void {

	if (_background_movie.getStatus() == sfe::Playing)
		_background_movie.stop();
}

auto Sorcery::Castle::_draw() -> void {

	// Components
	_window->draw(*_outside_frame);
	_window->draw(*_title_frame);
	_display.window->draw_text(_title_text, (*_display.layout)["castle:gui_frame_title_text"]);
	_window->draw(*_character_frame);
	_window->draw(_background);
	_window->draw(*_menu_frame);

	for (auto& legend_text: _character_legend_text)
		_window->draw(legend_text);

	// And the Menu
	double lerp = _graphics.animation->colour_lerp;
	_castle_menu->generate((*_display.layout)["castle:castle_menu"], lerp);
	const sf::Vector2f menu_pos((*_display.layout)["castle:castle_menu"].x, (*_display.layout)["castle:castle_menu"].y);
	_castle_menu->setPosition(menu_pos);
	_window->draw(*_castle_menu);

	// Always draw the following
	_cursor.setPosition(static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
	_window->draw(_cursor);
}
