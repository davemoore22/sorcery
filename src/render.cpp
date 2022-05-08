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

#include "render.hpp"

// Standard Constructor
Sorcery::Render::Render(
	System *system, Display *display, Graphics *graphics, Game *game)
	: _system{system}, _display{display}, _graphics{graphics}, _game{game} {

	// Setup the Draw Surface
	_rtexture.create(vIEW_WIDTH, vIEW_HEIGHT);
	_rtexture.setSmooth(true);
	_rtexture.clear(sf::Color(0, 0, 0, 255));

	// Load the Tile Config
	_view = std::make_unique<View>(_system, _display, _graphics, _game);

	_sprites.clear();
}

auto Sorcery::Render::refresh() -> void {

	_rtexture.clear(sf::Color(40, 40, 40, 255));
	_rtexture.display();
	_texture = _rtexture.getTexture();
	auto bg = sf::Sprite{_texture};
	bg.setPosition(0, 0);
	_sprites.emplace_back(bg);

	// Get the source graphics for the visible tiles - floor first
	_visible.clear();
	_visible = _view->get_to_depth(ViewNodeLayer::FLOOR, false);

	// For each visible tile, get the level cell
	const auto player_pos{_game->state->get_player_pos()};
	const auto player_facing{_game->state->get_player_facing()};
	for (auto node : _visible) {

		const auto x{node.coords.x};
		const auto z{node.coords.z};
		const auto tile{
			_game->state->level->at(player_pos, player_facing, x, z)};
		sf::Sprite floor_sprite{
			_graphics->textures->get_atlas(node.source_rect, false)};
		floor_sprite.setPosition(sf::Vector2f{node.dest.x, node.dest.y});
		if (node.flipped)
			floor_sprite.setScale(-1.0f, 1.0);

		_sprites.emplace_back(floor_sprite);
	}

	_visible.clear();
	_visible = _view->get_to_depth(ViewNodeLayer::CEILING, false);
	for (auto node : _visible) {

		const auto x{node.coords.x};
		const auto z{node.coords.z};
		const auto tile{
			_game->state->level->at(player_pos, player_facing, x, z)};
		sf::Sprite ceiling_sprite{
			_graphics->textures->get_atlas(node.source_rect, false)};
		ceiling_sprite.setPosition(sf::Vector2f{node.dest.x, node.dest.y});
		if (node.flipped)
			ceiling_sprite.setScale(-1.0f, 1.0);

		_sprites.emplace_back(ceiling_sprite);
	}
}

auto Sorcery::Render::draw(
	sf::RenderTarget &target, sf::RenderStates states) const -> void {

	states.transform *= getTransform();

	for (const auto &sprite : _sprites)
		target.draw(sprite, states);
}
