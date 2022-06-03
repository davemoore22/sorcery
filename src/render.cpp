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
	_rtexture.create(VIEW_WIDTH, VIEW_HEIGHT);
	_rtexture.setSmooth(true);
	_rtexture.clear(sf::Color(0, 0, 0, 255));

	// Load the Tile Config
	_view = std::make_unique<View>(_system, _display, _graphics, _game);

	_sprites.clear();
}

auto Sorcery::Render::refresh() -> void {

	// Clear the View
	_rtexture.clear(sf::Color(0, 0, 0, 255));
	_rtexture.display();
	/* _texture = _rtexture.getTexture();
	auto bg{sf::Sprite{_texture}};
	bg.setPosition(0, 0);
	_sprites.emplace_back(bg);

	// Render the View Components
	_render_floor(false);
	_render_ceiling(false);
	_render_walls(false); */
}

auto Sorcery::Render::_darken_sprite(
	const unsigned int depth, bool lit = true) const -> sf::Color {

	const auto max_depth{lit ? LIGHT_VIEW_DEPTH : DARK_VIEW_DEPTH};
	const auto step{255 / max_depth};

	const auto darkened{sf::Color{
		255 - (depth * step), 255 - (depth * step), 255 - (depth * step)}};

	return darkened;
}

auto Sorcery::Render::draw(
	sf::RenderTarget &target, sf::RenderStates states) const -> void {

	states.transform *= getTransform();

	bool lit = true;

	_render_wireframe(target, states, lit);

	for (const auto &sprite : _sprites)
		target.draw(sprite, states);
}

auto Sorcery::Render::_get_left_side(MapDirection facing) const
	-> MapDirection {

	switch (facing) {
	case MapDirection::NORTH:
		return MapDirection::WEST;
		break;
	case MapDirection::SOUTH:
		return MapDirection::EAST;
		break;
	case MapDirection::EAST:
		return MapDirection::NORTH;
		break;
	case MapDirection::WEST:
		return MapDirection::SOUTH;
		break;
	default:
		return facing;
		break;
	}
}
auto Sorcery::Render::_get_right_side(MapDirection facing) const
	-> MapDirection {

	switch (facing) {
	case MapDirection::NORTH:
		return MapDirection::EAST;
		break;
	case MapDirection::SOUTH:
		return MapDirection::WEST;
		break;
	case MapDirection::EAST:
		return MapDirection::SOUTH;
		break;
	case MapDirection::WEST:
		return MapDirection::NORTH;
		break;
	default:
		return facing;
		break;
	}
}

auto Sorcery::Render::_render_wireframe(
	sf::RenderTarget &target, sf::RenderStates states, bool lit) const -> void {

	sf::Texture wireframe_t{
		_system->resources->textures[GraphicsTexture::WIREFRAME]};

	states.texture = &wireframe_t;

	const auto player_pos{_game->state->get_player_pos()};
	const auto player_facing{_game->state->get_player_facing()};

	// todo - change level at to use z- into the screen
	const auto tl0{_game->state->level->at(player_pos, player_facing, -1, 0)};
	auto vl0{_view->tileviews.at(Coordinate3{-1, 0, 0})};
	const auto tm0{_game->state->level->at(player_pos, player_facing, 0, 0)};
	auto vm0{_view->tileviews.at(Coordinate3{0, 0, 0})};
	const auto tr0{_game->state->level->at(player_pos, player_facing, 1, 0)};
	auto vr0{_view->tileviews.at(Coordinate3{1, 0, 0})};

	const auto tl1{_game->state->level->at(player_pos, player_facing, -1, 1)};
	auto vl1{_view->tileviews.at(Coordinate3{-1, 0, -1})};
	const auto tm1{_game->state->level->at(player_pos, player_facing, 0, 1)};
	auto vm1{_view->tileviews.at(Coordinate3{0, 0, -1})};
	const auto tr1{_game->state->level->at(player_pos, player_facing, 1, 1)};
	auto vr1{_view->tileviews.at(Coordinate3{1, 0, -1})};

	const auto tl2{_game->state->level->at(player_pos, player_facing, -1, 2)};
	auto vl2{_view->tileviews.at(Coordinate3{-1, 0, -2})};
	const auto tm2{_game->state->level->at(player_pos, player_facing, 0, 2)};
	auto vm2{_view->tileviews.at(Coordinate3{0, 0, -2})};
	const auto tr2{_game->state->level->at(player_pos, player_facing, 1, 2)};
	auto vr2{_view->tileviews.at(Coordinate3{1, 0, -2})};

	const auto tl3{_game->state->level->at(player_pos, player_facing, -1, 3)};
	auto vl3{_view->tileviews.at(Coordinate3{-1, 0, -3})};
	const auto tm3{_game->state->level->at(player_pos, player_facing, 0, 3)};
	auto vm3{_view->tileviews.at(Coordinate3{0, 0, -3})};
	const auto tr3{_game->state->level->at(player_pos, player_facing, 1, 3)};
	auto vr3{_view->tileviews.at(Coordinate3{1, 0, -3})};

	const auto tl4{_game->state->level->at(player_pos, player_facing, -1, 4)};
	auto vl4{_view->tileviews.at(Coordinate3{-1, 0, -4})};
	const auto tm4{_game->state->level->at(player_pos, player_facing, 0, 4)};
	auto vm4{_view->tileviews.at(Coordinate3{0, 0, -4})};
	const auto tr4{_game->state->level->at(player_pos, player_facing, 1, 4)};
	auto vr4{_view->tileviews.at(Coordinate3{1, 0, -4})};

	// If we are in darkness, only draw that!
	if (tm0.is(TileProperty::DARKNESS)) {
		target.draw(vm0.darkness, states);
	} else {

		if (lit) {

			// Row 4
			if (tl4.is(TileProperty::DARKNESS))
				target.draw(vl4.darkness, states);
			if (tm4.is(TileProperty::DARKNESS))
				target.draw(vm4.darkness, states);
			if (tr4.is(TileProperty::DARKNESS))
				target.draw(vr4.darkness, states);

			// Row 3
			if (tl3.is(TileProperty::DARKNESS)) {
				target.draw(vl2.darkness, states);
				target.draw(vl3.side_darkness, states);
			} else {
				if (tl3.has(player_facing, TileEdge::WALL))
					target.draw(vl3.back_wall, states);
				if (tl3.has(player_facing, TileEdge::UNLOCKED_DOOR)) {
					target.draw(vl3.back_wall, states);
					target.draw(vl3.back_door, states);
				}
			}
			if (tm3.is(TileProperty::DARKNESS))
				target.draw(vm3.darkness, states);
			else {
				if (tm3.has(player_facing, TileEdge::WALL))
					target.draw(vm3.back_wall, states);
				if (tm3.has(player_facing, TileEdge::UNLOCKED_DOOR)) {
					target.draw(vm3.back_wall, states);
					target.draw(vm3.back_door, states);
				}
			}
			if (tr3.is(TileProperty::DARKNESS)) {
				target.draw(vr2.darkness, states);
				target.draw(vr3.side_darkness, states);
			} else {
				if (tr3.has(player_facing, TileEdge::WALL))
					target.draw(vr3.back_wall, states);
				if (tr3.has(player_facing, TileEdge::UNLOCKED_DOOR)) {
					target.draw(vr3.back_wall, states);
					target.draw(vr3.back_door, states);
				}
			}

			if (!tm3.is(TileProperty::DARKNESS)) {
				if (tm3.has(_get_left_side(player_facing), TileEdge::WALL))
					target.draw(vm3.left_side_wall, states);
				if (tm3.has(_get_left_side(player_facing),
						TileEdge::UNLOCKED_DOOR)) {
					target.draw(vm3.left_side_wall, states);
					target.draw(vm3.left_side_door, states);
				}
				if (tm3.has(_get_right_side(player_facing), TileEdge::WALL))
					target.draw(vm3.right_side_wall, states);
				if (tm3.has(_get_right_side(player_facing),
						TileEdge::UNLOCKED_DOOR)) {
					target.draw(vm3.right_side_wall, states);
					target.draw(vm3.right_side_door, states);
				}
			}

			// Row 2
			if (tl2.is(TileProperty::DARKNESS)) {
				target.draw(vl1.darkness, states);
				target.draw(vl2.side_darkness, states);
			} else {
				if (tl2.has(player_facing, TileEdge::WALL))
					target.draw(vl2.back_wall, states);
				if (tl2.has(player_facing, TileEdge::UNLOCKED_DOOR)) {
					target.draw(vl2.back_wall, states);
					target.draw(vl2.back_door, states);
				}
			}
			if (tm2.is(TileProperty::DARKNESS))
				target.draw(vm2.darkness, states);
			else {
				if (tm2.has(player_facing, TileEdge::WALL))
					target.draw(vm2.back_wall, states);
				if (tm2.has(player_facing, TileEdge::UNLOCKED_DOOR)) {
					target.draw(vm2.back_wall, states);
					target.draw(vm2.back_door, states);
				}
			}
			if (tr2.is(TileProperty::DARKNESS)) {
				target.draw(vr1.darkness, states);
				target.draw(vr2.side_darkness, states);
			} else {
				if (tr2.has(player_facing, TileEdge::WALL))
					target.draw(vr2.back_wall, states);
				if (tr2.has(player_facing, TileEdge::UNLOCKED_DOOR)) {
					target.draw(vr2.back_wall, states);
					target.draw(vr2.back_door, states);
				}
			}

			if (!tm2.is(TileProperty::DARKNESS)) {
				if (tm2.has(_get_left_side(player_facing), TileEdge::WALL))
					target.draw(vm2.left_side_wall, states);
				if (tm2.has(_get_left_side(player_facing),
						TileEdge::UNLOCKED_DOOR)) {
					target.draw(vm2.left_side_wall, states);
					target.draw(vm2.left_side_door, states);
				}
				if (tm2.has(_get_right_side(player_facing), TileEdge::WALL))
					target.draw(vm2.right_side_wall, states);
				if (tm2.has(_get_right_side(player_facing),
						TileEdge::UNLOCKED_DOOR)) {
					target.draw(vm2.right_side_wall, states);
					target.draw(vm2.right_side_door, states);
				}
			}
		}

		// Row 1
		if (tl1.is(TileProperty::DARKNESS)) {
			target.draw(vl0.darkness, states);
			target.draw(vl1.side_darkness, states);
		} else {
			if (tl1.has(player_facing, TileEdge::WALL))
				target.draw(vl1.back_wall, states);
			if (tl1.has(player_facing, TileEdge::UNLOCKED_DOOR)) {
				target.draw(vl1.back_wall, states);
				target.draw(vl1.back_door, states);
			}
			if (tl1.has(TileFeature::MESSAGE))
				target.draw(vl1.floor, states);
			if (tl1.has(TileFeature::STAIRS_DOWN))
				target.draw(vl1.floor, states);
			if (tl1.has(TileFeature::STAIRS_UP))
				target.draw(vl1.ceiling, states);
		}
		if (tm1.is(TileProperty::DARKNESS))
			target.draw(vm1.darkness, states);
		else {
			if (tm1.has(player_facing, TileEdge::WALL))
				target.draw(vm1.back_wall, states);
			if (tm1.has(player_facing, TileEdge::UNLOCKED_DOOR)) {
				target.draw(vm1.back_wall, states);
				target.draw(vm1.back_door, states);
			}
			if (tm1.has(TileFeature::MESSAGE))
				target.draw(vm1.floor, states);
			if (tm1.has(TileFeature::STAIRS_DOWN))
				target.draw(vm1.floor, states);
			if (tm1.has(TileFeature::STAIRS_UP))
				target.draw(vm1.ceiling, states);
		}
		if (tr1.is(TileProperty::DARKNESS)) {
			target.draw(vr0.darkness, states);
			target.draw(vr1.side_darkness, states);
		} else {
			if (tr1.has(player_facing, TileEdge::WALL))
				target.draw(vr1.back_wall, states);
			if (tr1.has(player_facing, TileEdge::UNLOCKED_DOOR)) {
				target.draw(vr1.back_wall, states);
				target.draw(vr1.back_door, states);
			}
			if (tr1.has(TileFeature::MESSAGE))
				target.draw(vr1.floor, states);
			if (tr1.has(TileFeature::STAIRS_DOWN))
				target.draw(vr1.floor, states);
			if (tr1.has(TileFeature::STAIRS_UP))
				target.draw(vr1.ceiling, states);
		}

		if (!tm1.is(TileProperty::DARKNESS)) {
			if (tm1.has(_get_left_side(player_facing), TileEdge::WALL))
				target.draw(vm1.left_side_wall, states);
			if (tm1.has(
					_get_left_side(player_facing), TileEdge::UNLOCKED_DOOR)) {
				target.draw(vm1.left_side_wall, states);
				target.draw(vm1.left_side_door, states);
			}
			if (tm1.has(_get_right_side(player_facing), TileEdge::WALL))
				target.draw(vm1.right_side_wall, states);
			if (tm1.has(
					_get_right_side(player_facing), TileEdge::UNLOCKED_DOOR)) {
				target.draw(vm1.right_side_wall, states);
				target.draw(vm1.right_side_door, states);
			}
		}

		// Row 0
		if (tl0.has(player_facing, TileEdge::WALL))
			target.draw(vl0.back_wall, states);
		if (tl0.is(TileProperty::DARKNESS)) {
			target.draw(vl0.darkness, states);
			target.draw(vl0.side_darkness, states);
		} else {
			if (tl0.has(player_facing, TileEdge::UNLOCKED_DOOR)) {
				target.draw(vl0.back_wall, states);
				target.draw(vl0.back_door, states);
			}
			if (tl0.has(TileFeature::MESSAGE))
				target.draw(vl0.floor, states);
			if (tl0.has(TileFeature::STAIRS_DOWN))
				target.draw(vl0.floor, states);
			if (tl0.has(TileFeature::STAIRS_UP))
				target.draw(vl0.ceiling, states);
		}

		// If we have reached here, we aren't standing in darkness
		if (tm0.has(player_facing, TileEdge::WALL))
			target.draw(vm0.back_wall, states);
		if (tm0.has(player_facing, TileEdge::UNLOCKED_DOOR)) {
			target.draw(vm0.back_wall, states);
			target.draw(vm0.back_door, states);
		}
		if (tm0.has(TileFeature::MESSAGE))
			target.draw(vm0.floor, states);
		if (tm0.has(TileFeature::STAIRS_DOWN))
			target.draw(vm0.floor, states);
		if (tm0.has(TileFeature::STAIRS_UP))
			target.draw(vm0.ceiling, states);

		if (tr0.has(player_facing, TileEdge::WALL))
			target.draw(vr0.back_wall, states);
		if (tr0.is(TileProperty::DARKNESS)) {
			target.draw(vr0.darkness, states);
			target.draw(vr0.side_darkness, states);
		} else {
			if (tr0.has(player_facing, TileEdge::UNLOCKED_DOOR)) {
				target.draw(vr0.back_wall, states);
				target.draw(vr0.back_door, states);
			}
			if (tr0.has(TileFeature::MESSAGE))
				target.draw(vr0.floor, states);
			if (tr0.has(TileFeature::STAIRS_DOWN))
				target.draw(vr0.floor, states);
			if (tr0.has(TileFeature::STAIRS_UP))
				target.draw(vr0.ceiling, states);
		}

		if (!tm0.is(TileProperty::DARKNESS)) {
			if (tm0.has(_get_left_side(player_facing), TileEdge::WALL))
				target.draw(vm0.left_side_wall, states);
			if (tm0.has(
					_get_left_side(player_facing), TileEdge::UNLOCKED_DOOR)) {
				target.draw(vm0.left_side_wall, states);
				target.draw(vm0.left_side_door, states);
			}
			if (tm0.has(_get_right_side(player_facing), TileEdge::WALL))
				target.draw(vm0.right_side_wall, states);
			if (tm0.has(
					_get_right_side(player_facing), TileEdge::UNLOCKED_DOOR)) {
				target.draw(vm0.right_side_wall, states);
				target.draw(vm0.right_side_door, states);
			}
		}
	}
}

/*
auto Sorcery::Render::_render_floor(bool lit = true) -> void {

	const auto player_pos{_game->state->get_player_pos()};
	const auto player_facing{_game->state->get_player_facing()};

	_visible.clear();
	_visible = _view->get_lit_nodes(ViewNodeLayer::FLOOR, lit);

	for (const auto &node : _visible) {

		const auto x{node.coords.x};
		const auto z{node.coords.z};
		const auto tile{
			_game->state->level->at(player_pos, player_facing, x, z)};
		auto offset{false};
		if (tile.has(TileFeature::STAIRS_DOWN))
			offset = true;
		sf::Sprite floor_sprite{
			_graphics->textures->get_atlas(node.source_rect, offset)};
		floor_sprite.setColor(_darken_sprite(z, lit));
		floor_sprite.setPosition(sf::Vector2f{
			static_cast<float>(node.dest.x),
static_cast<float>(node.dest.y)}); if (node.flipped)
			floor_sprite.setScale(-1.0f, 1.0);

		_sprites.emplace_back(floor_sprite);
	}
}

auto Sorcery::Render::_render_walls(bool lit = true) -> void {

	// OK for every "z" row
	const auto player_pos{_game->state->get_player_pos()};
	const auto player_facing{_game->state->get_player_facing()};
	const auto depth{lit ? LIGHT_VIEW_DEPTH : DARK_VIEW_DEPTH};

	for (int z = depth; z >= 0; --z) {

		// Get back walls
		_visible.clear();
		_visible = _view->get_nodes_at_depth(
			ViewNodeLayer::WALLS, ViewNodeType::FRONT, z);
		std::sort(_visible.begin(), _visible.end(),
			[](const ViewNode &a, const ViewNode &b) -> bool {
				return a.coords.x < b.coords.x;
			});

		for (const auto &node : _visible) {

			// Front Walls directly in front
			auto render{true};
			const auto l_x{node.coords.x};
			const auto l_z{node.coords.z - 1};
			const auto tile{
				_game->state->level->at(player_pos, player_facing, l_x,
l_z)}; auto offset{false}; sf::Sprite front_sprite{
				_graphics->textures->get_atlas(node.source_rect, offset)};
			front_sprite.setColor(_darken_sprite(z - 1, lit));
			const auto offset_width{front_sprite.getLocalBounds().width};

			if (player_facing == MapDirection::NORTH) {
				if (tile.wall(MapDirection::NORTH) != TileEdge::WALL)
					render = false;
			} else if (player_facing == MapDirection::SOUTH) {
				if (tile.wall(MapDirection::SOUTH) != TileEdge::WALL)
					render = false;
			} else if (player_facing == MapDirection::EAST) {
				if (tile.wall(MapDirection::EAST) != TileEdge::WALL)
					render = false;
			} else if (player_facing == MapDirection::WEST) {
				if (tile.wall(MapDirection::WEST) != TileEdge::WALL)
					render = false;
			}
			if (render) {
				front_sprite.setPosition(
					sf::Vector2f{static_cast<float>(node.dest.x),
						static_cast<float>(node.dest.y)});
				if (node.flipped)
					front_sprite.setScale(-1.0f, 1.0);
				_sprites.emplace_back(front_sprite);
			}

			// Now we need to extend on each side if necessary (remember
that in
			// our view -x is to the left but on our map +x is to the right,
as
			// seen from the player's perspective)
			for (int left_x = 1; left_x <= depth; left_x++) {
				auto shall_render{true};
				const auto dest_offset_x{std::abs(left_x * offset_width)};
				const auto n_x{l_x - left_x};
				const auto tile_right{_game->state->level->at(
					player_pos, player_facing, n_x, l_z)};
				if (player_facing == MapDirection::NORTH) {
					if (tile_right.wall(MapDirection::NORTH) !=
TileEdge::WALL) shall_render = false; } else if (player_facing ==
MapDirection::SOUTH) { if (tile_right.wall(MapDirection::SOUTH) !=
TileEdge::WALL) shall_render = false; } else if (player_facing ==
MapDirection::EAST) { if (tile_right.wall(MapDirection::EAST) !=
TileEdge::WALL) shall_render = false; } else if (player_facing ==
MapDirection::WEST) { if (tile_right.wall(MapDirection::WEST) !=
TileEdge::WALL) shall_render = false;
				}
				if (shall_render) {
					auto offset_texture{false};
					sf::Sprite
left_front_sprite{_graphics->textures->get_atlas( node.source_rect,
offset_texture)}; left_front_sprite.setColor(_darken_sprite(z - 1, lit));
					left_front_sprite.setPosition(sf::Vector2f{
						static_cast<float>(node.dest.x - dest_offset_x),
						static_cast<float>(node.dest.y)});
					_sprites.emplace_back(left_front_sprite);
				}
			}

			for (int right_x = 1; right_x <= depth; right_x++) {
				auto shall_render{true};
				const auto dest_offset_x{right_x * offset_width};
				const auto n_x{l_x + right_x};
				const auto tile_left{_game->state->level->at(
					player_pos, player_facing, n_x, l_z)};
				if (player_facing == MapDirection::NORTH) {
					if (tile_left.wall(MapDirection::NORTH) !=
TileEdge::WALL) shall_render = false; } else if (player_facing ==
MapDirection::SOUTH) { if (tile_left.wall(MapDirection::SOUTH) !=
TileEdge::WALL) shall_render = false; } else if (player_facing ==
MapDirection::EAST) { if (tile_left.wall(MapDirection::EAST) !=
TileEdge::WALL) shall_render = false; } else if (player_facing ==
MapDirection::WEST) { if (tile_left.wall(MapDirection::WEST) !=
TileEdge::WALL) shall_render = false;
				}
				if (shall_render) {
					auto offset_texture{false};
					sf::Sprite right_front_sprite{
						_graphics->textures->get_atlas(
							node.source_rect, offset_texture)};
					right_front_sprite.setColor(_darken_sprite(z - 1, lit));
					right_front_sprite.setPosition(sf::Vector2f{
						static_cast<float>(node.dest.x + dest_offset_x),
						static_cast<float>(node.dest.y)});
					_sprites.emplace_back(right_front_sprite);
				}
			}
		}

		// Now we do side walls to the right at this depth (unlike the front
		// walls, the view atlas returns the correct depth for side walls);
but
		// we also have to mirror them
		_visible.clear();
		_visible = _view->get_nodes_at_depth(
			ViewNodeLayer::WALLS, ViewNodeType::SIDE, 1, z);
		std::sort(_visible.begin(), _visible.end(),
			[](const ViewNode &a, const ViewNode &b) -> bool {
				return a.coords.x > b.coords.x;
			});

		for (const auto &node : _visible) {

			auto render{true};
			const auto l_x{node.coords.x - 1};
			const auto l_z{node.coords.z};
			const auto tile{
				_game->state->level->at(player_pos, player_facing, l_x,
l_z)}; auto offset{false}; sf::Sprite right_side_sprite{
				_graphics->textures->get_atlas(node.source_rect, offset)};

			if (player_facing == MapDirection::NORTH) {
				if (tile.wall(MapDirection::EAST) != TileEdge::WALL)
					render = false;
			} else if (player_facing == MapDirection::SOUTH) {
				if (tile.wall(MapDirection::WEST) != TileEdge::WALL)
					render = false;
			} else if (player_facing == MapDirection::EAST) {
				if (tile.wall(MapDirection::SOUTH) != TileEdge::WALL)
					render = false;
			} else if (player_facing == MapDirection::WEST) {
				if (tile.wall(MapDirection::WEST) != TileEdge::WALL)
					render = false;
			}
			if (render) {
				right_side_sprite.setColor(_darken_sprite(z, lit));
				right_side_sprite.setPosition(
					sf::Vector2f{static_cast<float>(node.dest.x),
						static_cast<float>(node.dest.y)});
				if (node.flipped)
					right_side_sprite.setScale(-1.0f, 1.0);
				_sprites.emplace_back(right_side_sprite);
			}
		}

		// Now we do side walls to the left at this depth (unlike the front
		// walls, the view atlas returns the correct depth for side walls)
		_visible.clear();
		_visible = _view->get_nodes_at_depth(
			ViewNodeLayer::WALLS, ViewNodeType::SIDE, -1, z);
		std::sort(_visible.begin(), _visible.end(),
			[](const ViewNode &a, const ViewNode &b) -> bool {
				return a.coords.x < b.coords.x;
			});

		for (const auto &node : _visible) {

			auto render{true};

			// another hack here due to outside walls
			const auto l_x{node.coords.x + 1};
			const auto l_z{node.coords.z};
			const auto tile{
				_game->state->level->at(player_pos, player_facing, l_x,
l_z)}; auto offset{false}; sf::Sprite left_side_sprite{
				_graphics->textures->get_atlas(node.source_rect, offset)};

			if (player_facing == MapDirection::NORTH) {
				if (tile.wall(MapDirection::WEST) != TileEdge::WALL)
					render = false;
			} else if (player_facing == MapDirection::SOUTH) {
				if (tile.wall(MapDirection::EAST) != TileEdge::WALL)
					render = false;
			} else if (player_facing == MapDirection::EAST) {
				if (tile.wall(MapDirection::NORTH) != TileEdge::WALL)
					render = false;
			} else if (player_facing == MapDirection::WEST) {
				if (tile.wall(MapDirection::SOUTH) != TileEdge::WALL)
					render = false;
			}
			if (render) {
				left_side_sprite.setColor(_darken_sprite(z, lit));
				left_side_sprite.setPosition(
					sf::Vector2f{static_cast<float>(node.dest.x),
						static_cast<float>(node.dest.y)});
				if (node.flipped)
					left_side_sprite.setScale(-1.0f, 1.0);
				_sprites.emplace_back(left_side_sprite);
			}
		}
	}
}

auto Sorcery::Render::_render_ceiling(bool lit = true) -> void {

	const auto player_pos{_game->state->get_player_pos()};
	const auto player_facing{_game->state->get_player_facing()};

	_visible.clear();
	_visible = _view->get_lit_nodes(ViewNodeLayer::CEILING, lit);
	for (const auto &node : _visible) {

		const auto x{node.coords.x};
		const auto z{node.coords.z};
		const auto tile{
			_game->state->level->at(player_pos, player_facing, x, z)};
		auto offset{false};
		if (tile.has(TileFeature::STAIRS_UP))
			offset = true;
		sf::Sprite ceiling_sprite{
			_graphics->textures->get_atlas(node.source_rect, offset)};
		ceiling_sprite.setColor(_darken_sprite(z, lit));
		ceiling_sprite.setPosition(sf::Vector2f{
			static_cast<float>(node.dest.x),
static_cast<float>(node.dest.y)}); if (node.flipped)
			ceiling_sprite.setScale(-1.0f, 1.0);
		_sprites.emplace_back(ceiling_sprite);
	}
}

*/