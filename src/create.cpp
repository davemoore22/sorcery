// Copyright (C) 2021 Dave Moore
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

#include "create.hpp"

// Standard Constructor
Sorcery::Create::Create(System &system, Display &display, Graphics &graphics)
	: _system{system}, _display{display}, _graphics{graphics} {

	// Get the Window and Graphics to Display
	_window = _display.window->get_window();

	// Get the Background Display Components
	_display.generate_components("create");

	// Create the Candidate Character
	_candidate = std::make_shared<Character>(system, display, graphics);

	_name_c = Component((*_display.layout)["character_create_stage_1:name_candidate"]);
}

// Standard Destructor
Sorcery::Create::~Create() {

	_display.stop_background_movie();
}

auto Sorcery::Create::start() -> std::optional<MenuItem> {

	// Get the custom components //TODO: see if this can be done in sprites by extending the
	// component object with the bg_rect?
	const Component bg_c{(*_display.layout)["create:background"]};
	const sf::IntRect bg_rect(1147, 249, 773, 388);
	_bg.setTexture(_system.resources->textures[TOWN_TEXTURE]);
	_bg.setTextureRect(bg_rect);
	_bg.setScale(bg_c.scale, bg_c.scale);
	_bg.setPosition(_display.window->get_x(_bg, bg_c.x), _display.window->get_y(_bg, bg_c.y));

	// Clear Everything"
	const Component name_c{(*_display.layout)["character_create_stage_1:name_candidate"]};
	_candidate->set_stage(CharacterStage::ENTER_NAME);

	// Clear the window
	_window->clear();

	// Play the background movie!
	_display.fit_background_movie();
	_display.start_background_movie();

	// Will need to change this for the seven screens
	_display.window->input_mode = WindowInputMode::INPUT_TEXT;
	std::string candidate_name{};

	sf::Event event{};
	while (_window->isOpen()) {
		while (_window->pollEvent(event)) {

			// Check for Window Close
			if (event.type == sf::Event::Closed)
				_window->close();

			if (_system.input->check_for_event(WindowInput::CANCEL, event))
				return std::nullopt;

			candidate_name = _candidate->name();
			if ((_system.input->check_for_event(WindowInput::ALPHANUMERIC, event)) ||
				(_system.input->check_for_event(WindowInput::SPACE, event))) {
				if (candidate_name.length() < 24) {
					candidate_name += static_cast<char>(event.text.unicode);
					_candidate->name(candidate_name);
				}
			} else if (_system.input->check_for_event(WindowInput::DELETE, event)) {
				if (candidate_name.length() > 0) {
					candidate_name.pop_back();
					_candidate->name(candidate_name);
				}
			} else if (_system.input->check_for_event(WindowInput::CONFIRM_NO_SPACE, event)) {

				if (candidate_name.length() > 0) {

					return std::nullopt;
					//_candidate->set_stage(CharacterStage::CHOOSE_RACE);
				}
			}

			_window->clear();

			_display.start_background_movie();
			_display.update_background_movie();
			_display.draw_background_movie();

			_draw();
			_window->display();
		}
	}

	return std::nullopt;
}

auto Sorcery::Create::stop() -> void {

	_display.stop_background_movie();
}

auto Sorcery::Create::_draw() -> void {

	double lerp{_graphics.animation->colour_lerp};
	sf::Text name_text;
	std::string display_name{};

	// Display Components
	_display.display_components("create");

	// Custom Layering
	_window->draw(_bg);

	// And draw the current state of the character!
	_display.display_components(
		"character_create_stage_1", _candidate->sprites, _candidate->texts, _candidate->frames);

	// TODO: use character-<draw for this!
	display_name = _candidate->name();
	_display.window->draw_text(name_text, _name_c, display_name, lerp);

	// And finally the Cursor
	_display.display_cursor();
}
