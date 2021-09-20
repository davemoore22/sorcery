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

#include "automap.hpp"

Sorcery::AutoMap::AutoMap(System *system, Display *display, Graphics *graphics,
	Game *game, Component layout)
	: _system{system}, _display{display}, _graphics{graphics}, _game{game},
	  _layout{layout} {

	_sprites.clear();
	_texts.clear();

	_map_radius = std::stoi(_layout["tile_count"].value()) / 2;

	if (_frame.get()) {
		_frame.release();
		_frame.reset();
	}
	_frame = std::make_unique<Frame>(_display->ui_texture,
		WindowFrameType::NORMAL, _layout.w, _layout.h, _layout.colour,
		_layout.background, _layout.alpha);
	auto fsprite{_frame->sprite};
	fsprite.setPosition(0, 0);
	_sprites.emplace_back(fsprite);
}

auto Sorcery::AutoMap::refresh() -> void {

	_sprites.resize(1);

	auto tx{std::stoi(_layout["tile_offset_x"].value())};
	auto ty{std::stoi(_layout["tile_offset_y"].value())};
	auto tcx{0};
	auto tcy{0};
	auto player_pos(_game->state->world->player_pos);
	for (auto x = static_cast<int>(player_pos.x - _map_radius);
		 x <= static_cast<int>(player_pos.x) + _map_radius; x++) {
		for (auto y = static_cast<int>(player_pos.y - _map_radius);
			 y <= static_cast<int>(player_pos.y + _map_radius); y++) {

			auto lx{x < 0 ? x + 19 : x};
			auto ly{y < 0 ? y + 19 : y};
			auto tile{_game->state->world->current_level->at(lx, ly)};
			auto tile_x{tx + (tcx * std::stoi(_layout["tile_width"].value())) +
						(tcx * std::stoi(_layout["tile_spacing"].value()))};
			auto tile_y{ty + (tcy * std::stoi(_layout["tile_height"].value())) +
						(tcy * std::stoi(_layout["tile_spacing"].value()))};

			++tcx;
		}
		++tcy;
		tcx = 0;
	}
}

auto Sorcery::AutoMap::draw(
	sf::RenderTarget &target, sf::RenderStates states) const -> void {

	states.transform *= getTransform();

	// Draw the standard components
	for (const auto &sprite : _sprites)
		target.draw(sprite, states);

	for (const auto &text : _texts)
		target.draw(text, states);
}
