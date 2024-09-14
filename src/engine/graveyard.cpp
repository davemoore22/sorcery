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

#include "engine/graveyard.hpp"
#include "common/enum.hpp"
#include "common/macro.hpp"
#include "common/type.hpp"
#include "core/display.hpp"
#include "core/game.hpp"
#include "core/graphics.hpp"
#include "core/input.hpp"
#include "core/state.hpp"
#include "core/system.hpp"
#include "core/window.hpp"
#include "engine/enum.hpp"
#include "gui/frame.hpp"
#include "gui/menu.hpp"
#include "resources/componentstore.hpp"
#include "resources/resourcemanager.hpp"
#include "resources/stringstore.hpp"
#include "resources/texturestore.hpp"
#include "types/character.hpp"
#include "types/component.hpp"

// Standard Constructor
Sorcery::Graveyard::Graveyard(System *system, Display *display, Graphics *graphics, Game *game)
	: _system{system}, _display{display}, _graphics{graphics}, _game{game} {

	// Get the Window and Graphics to Display
	_window = _display->window->get_window();

	_menu = std::make_unique<Menu>(_system, _display, _graphics, _game, MenuType::GRAVEYARD);
	_menu->generate((*_display->layout)["graveyard:menu"]);
	_menu->setPosition(_display->get_centre_x(_menu->get_width()), (*_display->layout)["graveyard:menu"].y);

	const Component menu_fc{(*_display->layout)["graveyard:menu_frame"]};
	_menu_frame = std::make_unique<Frame>(
		_display->ui_texture, menu_fc.w, menu_fc.h, menu_fc.colour, menu_fc.background, menu_fc.alpha);
	_menu_frame->setPosition(_display->window->get_x(_menu_frame->sprite, menu_fc.x),
		_display->window->get_y(_menu_frame->sprite, menu_fc.y));

	_sprites.clear();
	_texts.clear();

	_wipe_message = sf::Text();
};

// Standard Destructor
Sorcery::Graveyard::~Graveyard() {
}

auto Sorcery::Graveyard::start() -> std::optional<MI> {

	// Get the Background Display Components and load them into Display module storage (not local)
	_display->generate("graveyard");

	// Draw the gravestones
	const auto gs_c{(*_display->layout)["graveyard:gravestone"]};
	const auto num_gs{std::stoi(gs_c["number"].value())};
	auto gs_x{gs_c.x};
	auto gs_y{gs_c.y};
	auto inc_x{std::stoi(gs_c["offset_x"].value()) * _display->window->get_cw()};
	auto inc_y{std::stoi(gs_c["offset_y"].value()) * _display->window->get_cw()};

	for (int i = 0; i < num_gs; i++) {
		auto gravestone{
			_graphics->textures->get(unenum(Enums::Graphics::Event::GRAVESTONE), GraphicsTextureType::EVENTS).value()};
		gravestone.setPosition(gs_x + (inc_x * i), gs_y + (inc_y * i));
		gravestone.setScale(gs_c.scl());
		_sprites.emplace_back(gravestone);
	}

	// Get party names
	std::vector<std::string> names;
	names.clear();
	const auto party{_game->state->get_party_characters()};
	for (const auto &[character_id, character] : _game->characters) {
		if (std::find(party.begin(), party.end(), character_id) != party.end()) {
			names.emplace_back(character.get_name());
		}
	}
	if (names.size() < 6)
		names.resize(6);

	auto c_c{(*_display->layout)["graveyard:party_members"]};
	auto text_x{c_c.x};
	auto text_y{c_c.y};
	auto text_spacing_y{std::stoi(c_c["spacing_y"].value()) * _display->window->get_ch()};
	for (int i = 0; i < 6; i += 2) {
		auto characters{fmt::format("{:^18}{:^18}", names.at(i), names.at(i + 1))};
		sf::Text text{};
		if (_display->get_bold())
			text.setStyle(sf::Text::Bold);
		text.setFont(_system->resources->fonts[c_c.font]);
		text.setCharacterSize(c_c.size);
		text.setFillColor(sf::Color(c_c.colour));
		if (_display->get_upper())
			std::transform(characters.begin(), characters.end(), characters.begin(), ::toupper);
		text.setString(characters);
		text.setPosition(text_x, text_y);
		_texts.emplace_back(text);
		text_y += text_spacing_y;
	}

	// Clear the window
	_window->clear();

	_display->set_input_mode(WIM::NAVIGATE_MENU);
	std::optional<std::vector<MenuEntry>::const_iterator> selected{_menu->items.begin()};

	// And do the main loop
	sf::Event event{};
	while (_window->isOpen()) {
		while (_window->pollEvent(event)) {

			// Check for Window Close
			if (event.type == sf::Event::Closed)
				_window->close();

			// Handle enabling help overlay
			if (_system->input->check(CIN::SHOW_CONTROLS, event)) {
				_display->show_overlay();
				continue;
			} else
				_display->hide_overlay();

			if (_system->input->check(CIN::CANCEL, event))
				return MI::ITEM_CONTINUE;

			if (_system->input->check(CIN::BACK, event))
				return MI::ITEM_CONTINUE;

			if (_system->input->check(CIN::UP, event))
				selected = _menu->choose_previous();
			else if (_system->input->check(CIN::DOWN, event))
				selected = _menu->choose_next();
			else if (_system->input->check(CIN::MOVE, event))
				selected = _menu->set_mouse_selected(static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
			else if (_system->input->check(CIN::CONFIRM, event)) {

				// We have selected something from the menu
				if (selected) {
					const MI option_chosen{(*selected.value()).item};
					if (option_chosen == MI::ITEM_CONTINUE) {
						return MI::ITEM_CONTINUE;
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

auto Sorcery::Graveyard::stop() -> void {

	_display->set_input_mode(WIM::NAVIGATE_MENU);
}

auto Sorcery::Graveyard::_draw() -> void {

	// Play the background movie!
	_graphics->tile_bg(_window);

	// Display Components
	_display->display("graveyard");

	// Draw the standard components
	for (const auto &sprite : _sprites)
		_window->draw(sprite);

	for (const auto &text : _texts)
		_window->draw(text);

	// Menu Frame
	_window->draw(*_menu_frame);

	// And the Menu
	_menu->generate((*_display->layout)["graveyard:menu"]);
	_window->draw(*_menu);

	_display->window->draw_text(
		_wipe_message, (*_display->layout)["graveyard:wipe_text"], (*_display->string)["GRAVEYARD_MESSAGE"]);

	// And finally the Cursor
	_display->display_overlay();
	_display->display_cursor();
}
