// Copyright (C) 2024 Dave Moore
//
// This file is part of Sorcery: Shadows under Llylgamyn.
//
// Sorcery: Shadows under Llylgamyn is free software: you can redistribute it
// and/or modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation, either version 2 of the License,
// or (at your option) any later version.
//
// Sorcery: Shadows under Llylgamyn is distributed in the hope that it will
// be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Sorcery: Shadows under Llylgamyn.  If not, see
// <http://www.gnu.org/licenses/>.
//
// If you modify this Program, or any covered work, by linking or combining it
// with the libraries referred to in README (or a modified version of said
// libraries), containing parts covered by the terms of said libraries, the
// licensors of this Program grant you additional permission to convey the
// resulting work.

#include "gui/debuffbar.hpp"
#include "core/display.hpp"
#include "core/game.hpp"
#include "core/graphics.hpp"
#include "core/system.hpp"
#include "resources/factory.hpp"

Sorcery::DebuffBar::DebuffBar(System *system, Display *display,
	Graphics *graphics, Game *game, Component layout)
	: _system{system}, _display{display}, _graphics{graphics}, _game{game},
	  _layout{layout} {

	_sprites.clear();
	_texts.clear();
	_icons.clear();

	// Setup the Factory
	_factory = std::make_unique<Factory>(_system, _display, _graphics, _game);

	// Make the Frame
	_frame = _factory->make_comp_frame(_layout, _sprites);
}

auto Sorcery::DebuffBar::refresh() -> void {

	_sprites.resize(1);
	_icons.clear();

	/* auto lomilwa{(*_graphics->icons)["light-buff"].value()};
	auto torch{(*_graphics->icons)["no-light-buff"].value()};
	auto maporfic{(*_graphics->icons)["shield-buff"].value()};
	auto latumpaic{(*_graphics->icons)["knowledge-buff"].value()};

	const auto initial_x{std::invoke([&] {
		if (_layout["offset_x"])
			return std::stoi(_layout["offset_x"].value());
		else
			return 0;
	})};
	const auto initial_y{std::invoke([&] {
		if (_layout["offset_y"])
			return std::stoi(_layout["offset_y"].value());
		else
			return 0;
	})};

	auto x{initial_x};
	auto y{initial_y};

	if (_game->state->get_lit()) {
		lomilwa.setScale(_layout.scale, _layout.scale);
		lomilwa.setColor(sf::Color(_layout.colour));
		lomilwa.setPosition(x, y);
		_icons.emplace_back(std::make_pair("light-buff", lomilwa));
	} else {
		torch.setScale(_layout.scale, _layout.scale);
		torch.setColor(sf::Color(_layout.colour));
		torch.setPosition(x, y);
		_icons.emplace_back(std::make_pair("no-light-buff", torch));
	}
	*/
}

auto Sorcery::DebuffBar::draw(
	sf::RenderTarget &target, sf::RenderStates states) const -> void {

	states.transform *= getTransform();

	// Draw the standard components
	for (const auto &sprite : _sprites)
		target.draw(sprite, states);

	for (const auto &[key, icon] : _icons)
		target.draw(icon, states);
	for (const auto &text : _texts)
		target.draw(text, states);
}
