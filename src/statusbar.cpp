// Copyright (C) 2021 Dave Moore
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

#include "statusbar.hpp"

// Standard Constructor
Sorcery::StatusBar::StatusBar(
	System *system, Display *display, Graphics *graphics, Game *game)
	: _system{system}, _display{display}, _graphics{graphics}, _game{game} {

	_texts.clear();

	// Get any Layout Information
	_layout = Component((*_display->layout)["status_bar:status_bar"]);
	_frame_c = Component((*_display->layout)["status_bar:outer_frame"]);

	_rtexture.create(_layout.w * _display->window->get_cw(),
		_layout.h * _display->window->get_ch());
	_rtexture.setSmooth(true);
	_rtexture.clear();

	// Create the Outside Fram
	_frame = std::make_unique<Frame>(_display->ui_texture,
		WindowFrameType::NORMAL, _frame_c.w, _frame_c.h, _frame_c.colour,
		_frame_c.background, _frame_c.alpha);

	// Render the background (inset by the frame)
	sf::RectangleShape rect(
		sf::Vector2f((_display->window->get_cw() * (_layout.w)) - 20,
			(_display->window->get_ch() * (_layout.h)) - 20));
	rect.setFillColor(sf::Color(0, 0, 0, _layout.alpha));
	rect.setPosition(10, 10);
	_rtexture.draw(rect);

	_fsprite = _frame->sprite;
	_fsprite.setPosition(0, 0);
	_rtexture.draw(_fsprite);

	_generate();
	_draw();

	// Normally we'd be passed in a vector of character shared ptrs, and draw
	// each one (in a seperate update method?) but for now, just do this
	_rtexture.display();
	_texture = _rtexture.getTexture();
	sprite = sf::Sprite(_texture);

	width = sprite.getLocalBounds().width;
	height = sprite.getLocalBounds().height;
}

auto Sorcery::StatusBar::refresh() -> void {

	auto party{_game->state->get_party_characters()};
	for (auto _id : party) {

		std::cout << _game->characters[_id].get_name() << std::endl;
	}
}

auto Sorcery::StatusBar::draw(
	sf::RenderTarget &target, sf::RenderStates states) const -> void {

	states.transform *= getTransform();
	target.draw(sprite, states);
}

auto Sorcery::StatusBar::_generate() -> void {

	// For now - this is copied from display.cpp
	auto components{(*_display->layout)("status_bar")};
	if (components) {
		for (const auto &component : components.value()) {
			if (component.type == ComponentType::TEXT) {
				sf::Text text{};
				text.setFont(_system->resources->fonts[component.font]);
				text.setCharacterSize(component.size);
				text.setFillColor(sf::Color(component.colour));
				text.setString((*_display->string)[component.string_key]);
				const auto x{component.x == -1 ? _display->window->centre.x
											   : component.x};
				const auto y{component.y == -1 ? _display->window->centre.y
											   : component.y};
				if (component.justification == Justification::CENTRE) {
					text.setPosition(x, y);
					text.setOrigin(text.getLocalBounds().width / 2.0f,
						text.getLocalBounds().height / 2.0f);
				} else if (component.justification == Justification::RIGHT) {
					text.setPosition(x, y);
					const sf::FloatRect bounds{text.getLocalBounds()};
					text.setPosition(component.x - bounds.width, component.y);
				} else {
					text.setPosition(x, y);
					text.setOrigin(0, text.getLocalBounds().height / 2.0f);
				}

				_texts[component.unique_key] = text;
			}
		}
	}
}

auto Sorcery::StatusBar::_draw() -> void {

	for (auto &[unique_key, text] : _texts) {
		_rtexture.draw(text);
	}
}