// Copyright (C) 2023 Dave Moore
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

#include "reorder.hpp"

// Standard Constructor
Sorcery::Reorder::Reorder(System *system, Display *display, Graphics *graphics, Game *game, MenuMode mode)
	: _system{system}, _display{display}, _graphics{graphics}, _game{game}, _mode{mode} {

	// Get the Window and Graphics to Display
	_window = _display->window->get_window();

	// Setup Custom Components
	_menu = std::make_unique<Menu>(_system, _display, _graphics, _game, MenuType::PARTY_CHARACTER_NAMES);
	_candidate_c = Component{(*_display->layout)["reorder:candidate_party"]};

	// Modules
	_status_bar = std::make_unique<StatusBar>(_system, _display, _graphics, _game);
}

// Standard Destructor
Sorcery::Reorder::~Reorder() {
}

// Visit the Tavern
auto Sorcery::Reorder::start() -> std::optional<std::vector<unsigned int>> {

	_candidate_party.clear();
	_texts.clear();

	if (_mode == MenuMode::CAMP) {
		_display->window->save_screen();
		_display->generate("reorder_camp");
	} else
		_display->generate("reorder");

	// Clear the window
	_window->clear();

	// Refresh the Party characters
	_status_bar->refresh();

	// Generate the Custom Components
	const Component status_bar_c{(*_display->layout)["status_bar:status_bar"]};
	_status_bar->setPosition(_display->window->get_x(_status_bar->sprite, status_bar_c.x),
		_display->window->get_y(_status_bar->sprite, status_bar_c.y));

	// And do the main loop
	_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
	std::optional<std::vector<MenuEntry>::const_iterator> option{_menu->items.begin()};
	sf::Event event{};
	while (_window->isOpen()) {
		while (_window->pollEvent(event)) {

			// not idea, need to figure out how to return something else to
			// abort everything

			// Check for Window Close
			if (event.type == sf::Event::Closed)
				return std::nullopt;

			// Handle enabling help overlay
			if (_system->input->check(WindowInput::SHOW_CONTROLS, event)) {
				_display->show_overlay();
				continue;
			} else
				_display->hide_overlay();

			if (_system->input->check(WindowInput::CANCEL, event))
				return std::nullopt;

			if (_system->input->check(WindowInput::BACK, event))
				return std::nullopt;

			// And handle input on the main menu
			if (_system->input->check(WindowInput::UP, event))
				option = _menu->choose_previous();
			else if (_system->input->check(WindowInput::DOWN, event))
				option = _menu->choose_next();
			else if (_system->input->check(WindowInput::MOVE, event))
				option = _menu->set_mouse_selected(static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
			else if (_system->input->check(WindowInput::CONFIRM, event)) {

				if (option) {

					const auto character_chosen{(*option.value()).index};
					_candidate_party.emplace_back(character_chosen);
					_populate_candidate();
					auto added{_menu->get_by_index((*option.value()).index)};
					if (added) {
						(*added)->enabled = false;
						if (_menu->num_enabled() == 0)
							return _candidate_party;
					}
					continue;
				}
			}
		}

		_window->clear();

		_draw();
		_window->display();
	}

	return std::nullopt;
}

auto Sorcery::Reorder::_populate_candidate() -> void {

	_texts.clear();

	auto x{_candidate_c.x};
	auto y{_candidate_c.y};
	auto index{0};
	for (const auto character_id : _candidate_party) {
		sf::Text text{};

		text.setFont(_system->resources->fonts[_candidate_c.font]);
		text.setCharacterSize(_candidate_c.size);
		text.setFillColor(sf::Color(_candidate_c.colour));
		auto name{_game->characters[character_id].get_name()};
		std::transform(name.begin(), name.end(), name.begin(), ::toupper);
		text.setString(fmt::format("{}. {}", index + 1, name));
		text.setPosition(x, y + (index * _display->window->get_ch()));
		++index;
		_texts.push_back(text);
	}
}

auto Sorcery::Reorder::_draw() -> void {

	// Play the background movie!
	_graphics->tile_bg(_window);

	// Custom Components
	if (_mode == MenuMode::CAMP) {
		_display->window->restore_screen();
		_display->display("reorder_camp");
	} else {
		_display->display("reorder");
		_window->draw(*_status_bar);
	}

	_menu->generate((*_display->layout)["reorder:menu"]);
	const sf::Vector2f menu_pos((*_display->layout)["reorder:menu"].x, (*_display->layout)["reorder:menu"].y);
	_menu->setPosition(menu_pos);
	_window->draw(*_menu);

	// Draw the character names selected
	for (const auto &text : _texts)
		_window->draw(text);

	// Always draw the following
	_display->display_overlay();
	_display->display_cursor();
}

auto Sorcery::Reorder::stop() -> void {
}
