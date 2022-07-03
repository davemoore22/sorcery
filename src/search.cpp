// Copyright (C) 2021 Dave Moore
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

#include "search.hpp"

Sorcery::Search::Search(System *system, Display *display, Graphics *graphics, Game *game, Component layout)
	: _system{system}, _display{display}, _graphics{graphics}, _game{game}, _layout{layout} {

	_sprites.clear();
	_texts.clear();
	_icons.clear();

	// Set up Frame
	if (_frame.get()) {
		_frame.release();
		_frame.reset();
	}
	_frame = std::make_unique<Frame>(_display->ui_texture, WindowFrameType::NORMAL, _layout.w, _layout.h,
		_layout.colour, _layout.background, _layout.alpha);
	auto fsprite{_frame->sprite};
	fsprite.setPosition(0, 0);
	_sprites.emplace_back(fsprite);
	width = fsprite.getGlobalBounds().width;
	height = fsprite.getGlobalBounds().height;
}

auto Sorcery::Search::refresh() -> void {

	_sprites.resize(1);
	_icons.clear();

	auto other_characters{_game->get_characters_at_loc()};
	if (other_characters.size() > 0) {

		auto characters_here{(*_graphics->icons)["age"].value()};
		const auto initial_x{[&] {
			if (_layout["offset_x"])
				return std::stoi(_layout["offset_x"].value());
			else
				return 0;
		}()};
		const auto initial_y{[&] {
			if (_layout["offset_y"])
				return std::stoi(_layout["offset_y"].value());
			else
				return 0;
		}()};

		auto x{initial_x};
		auto y{initial_y};

		characters_here.setScale(_layout.scale, _layout.scale);
		characters_here.setColor(sf::Color(_layout.colour));
		characters_here.setPosition(x, y);
		_icons.emplace_back(std::make_pair("characters_here", characters_here));
	}

	characters_here = other_characters.size() > 0;
}

auto Sorcery::Search::draw(sf::RenderTarget &target, sf::RenderStates states) const -> void {

	states.transform *= getTransform();

	// Draw the standard components
	for (const auto &sprite : _sprites)
		target.draw(sprite, states);

	for (const auto &[key, icon] : _icons)
		target.draw(icon, states);

	for (const auto &text : _texts)
		target.draw(text, states);
}
