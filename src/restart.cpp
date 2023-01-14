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

#include "restart.hpp"

// Standard Constructor
Sorcery::Restart::Restart(System *system, Display *display, Graphics *graphics, Game *game)
	: _system{system}, _display{display}, _graphics{graphics}, _game{game} {

	// Get the Window and Graphics to Display
	_window = _display->window->get_window();
}

// Standard Destructor
Sorcery::Restart::~Restart() {
}

auto Sorcery::Restart::start() -> std::optional<MenuItem> {

	// Get the Background Display Components and load them into Display module
	// storage (not local)
	_display->generate("restart_expedition");
	_menu = std::make_unique<Menu>(_system, _display, _graphics, _game, MenuType::RESTART_EXPEDITION);

	_update_menus();

	// Set up the Custom Components
	const Component bg_c{(*_display->layout)["restart_expedition:background"]};
	sf::IntRect bg_rect{};
	bg_rect.width = std::stoi(bg_c["source_w"].value());
	bg_rect.height = std::stoi(bg_c["source_h"].value());
	bg_rect.top = 0;
	bg_rect.left = std::stoi(bg_c["source_w"].value()) * std::stoi(bg_c["source_index"].value());

	_bg.setTexture(_system->resources->textures[GraphicsTexture::TOWN]);
	_bg.setTextureRect(bg_rect);
	_bg.setScale(std::stof(bg_c["scale_x"].value()), std::stof(bg_c["scale_y"].value()));
	_bg.setPosition(_display->window->get_x(_bg, bg_c.x), _display->window->get_y(_bg, bg_c.y));

	const Component menu_fc{(*_display->layout)["restart_expedition:menu_frame"]};
	_menu_frame = std::make_unique<Frame>(_display->ui_texture, WindowFrameType::NORMAL, menu_fc.w, menu_fc.h,
		menu_fc.colour, menu_fc.background, menu_fc.alpha);
	_menu_frame->setPosition(_display->window->get_x(_menu_frame->sprite, menu_fc.x),
		_display->window->get_y(_menu_frame->sprite, menu_fc.y));

	// Clear the window
	_window->clear();

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
			if (_system->input->check(WindowInput::SHOW_CONTROLS, event)) {
				_display->show_overlay();
				continue;
			} else
				_display->hide_overlay();

			if (_system->input->check(WindowInput::CANCEL, event))
				return std::nullopt;

			if (_system->input->check(WindowInput::BACK, event))
				return std::nullopt;

			if (_system->input->check(WindowInput::UP, event))
				selected = _menu->choose_previous();
			else if (_system->input->check(WindowInput::DOWN, event))
				selected = _menu->choose_next();
			else if (_system->input->check(WindowInput::MOVE, event))
				selected = _menu->set_mouse_selected(static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
			else if (_system->input->check(WindowInput::CONFIRM, event)) {

				if (selected) {
					const MenuItem option_chosen{(*selected.value()).item};
					if (option_chosen == MenuItem::TR_EDGE_OF_TOWN) {
						return MenuItem::TR_EDGE_OF_TOWN;
					} else {

						// Find the location and floor of the character pointed to, and reload the maze, repopulate the
						// party and restart the game from there
						const auto character_chosen{(*selected.value()).index};
						auto &character{_game->characters[character_chosen]};
						auto to_depth{character.depth.value()};
						auto to_loc{character.coordinate.value()};
						_game->state->clear_party();
						for (auto &[character_id, character] : _game->characters) {
							if (character.get_location() == CharacterLocation::MAZE) {
								if ((character.depth.value() == to_depth) && (character.coordinate.value() == to_loc)) {
									character.set_location(CharacterLocation::PARTY);
									_game->state->add_character_by_id(character_id);
								}
							}
						}

						_game->state->set_depth(to_depth);
						_game->state->set_player_pos(to_loc);
						auto engine{std::make_unique<Engine>(_system, _display, _graphics, _game)};
						if (auto result{engine->start()}; result == EXIT_ALL) {
							_game->save_game();
							_game->state->set_depth(to_depth);
							_game->state->set_player_pos(to_loc);
							engine->stop();
							_display->shutdown_SFML();
							return MenuItem::ABORT;
						}

						engine->stop();
						_update_menus();
						return MenuItem::CANCEL;
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

auto Sorcery::Restart::_update_menus() -> void {

	if (_menu.get()) {
		_menu.release();
		_menu.reset();
	}

	_menu = std::make_unique<Menu>(_system, _display, _graphics, _game, MenuType::RESTART_EXPEDITION);
}

auto Sorcery::Restart::stop() -> void {

	_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
}

auto Sorcery::Restart::_draw() -> void {

	// Play the background movie!
	_graphics->tile_bg(_window);

	// Display Components
	_display->display("restart_expedition");

	// Menu Frame
	_window->draw(*_menu_frame);

	// And the Menu
	_menu->generate((*_display->layout)["restart_expedition:menu"]);
	const sf::Vector2f menu_pos(
		(*_display->layout)["restart_expedition:menu"].x, (*_display->layout)["restart_expedition:menu"].y);
	_menu->setPosition(menu_pos);
	_window->draw(*_menu);

	// And finally the Cursor
	_display->display_overlay();
	_display->display_cursor();
}
