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
Sorcery::Render::Render(System *system, Display *display, Graphics *graphics, Game *game)
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
}

auto Sorcery::Render::_darken_sprite(const unsigned int depth, bool lit = true) const -> sf::Color {

	const auto max_depth{lit ? LIGHT_VIEW_DEPTH : DARK_VIEW_DEPTH};
	const auto step{255 / max_depth};

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnarrowing"
	const auto darkened{sf::Color{255 - (depth * step), 255 - (depth * step), 255 - (depth * step)}};
#pragma GCC diagnostic pop

	return darkened;
}

auto Sorcery::Render::draw(sf::RenderTarget &target, sf::RenderStates states) const -> void {

	states.transform *= getTransform();

	_render_wireframe(target, states);

	for (const auto &sprite : _sprites)
		target.draw(sprite, states);
}

auto Sorcery::Render::_get_left_side(MapDirection facing) const -> MapDirection {

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
auto Sorcery::Render::_get_right_side(MapDirection facing) const -> MapDirection {

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

auto Sorcery::Render::_has_secret_door(const Tile &tile, const Sorcery::MapDirection direction) const -> bool {

	return ((tile.has(direction, TileEdge::ONE_WAY_HIDDEN_DOOR)) || (tile.has(direction, TileEdge::SECRET_DOOR)));
}

auto Sorcery::Render::_has_normal_door(const Tile &tile, const Sorcery::MapDirection direction) const -> bool {

	return ((tile.has(direction, TileEdge::ONE_WAY_DOOR)) || (tile.has(direction, TileEdge::UNLOCKED_DOOR)) ||
			(tile.has(direction, TileEdge::LOCKED_DOOR)));
}

auto Sorcery::Render::_has_wall(const Tile &tile, const Sorcery::MapDirection direction) const -> bool {

	return ((tile.has(direction, TileEdge::WALL)) || (tile.has(direction, TileEdge::ONE_WAY_WALL)));
}

auto Sorcery::Render::_render_wireframe(sf::RenderTarget &target, sf::RenderStates states) const -> void {

	sf::Texture wireframe_t{_system->resources->textures[GraphicsTexture::WIREFRAME]};

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

		if (_game->state->get_lit()) {

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
				if (!tm3.is(TileProperty::DARKNESS)) {
					if (_has_wall(tl3, player_facing))
						target.draw(vl3.back_wall, states);
					if (_has_normal_door(tl3, player_facing)) {
						target.draw(vl3.back_wall, states);
						target.draw(vl3.back_door, states);
					}
					if (_has_secret_door(tl3, player_facing)) {
						target.draw(vl3.back_wall, states);
						if (_game->state->get_lit())
							target.draw(vl3.back_door, states);
					}
				}
			}
			if (tr3.is(TileProperty::DARKNESS)) {
				target.draw(vr2.darkness, states);
				target.draw(vr3.side_darkness, states);
			} else {
				if (!tm3.is(TileProperty::DARKNESS)) {
					if (_has_wall(tr3, player_facing))
						target.draw(vr3.back_wall, states);
					if (_has_normal_door(tr3, player_facing)) {
						target.draw(vr3.back_wall, states);
						target.draw(vr3.back_door, states);
					}
					if (_has_secret_door(tr3, player_facing)) {
						target.draw(vr3.back_wall, states);
						if (_game->state->get_lit())
							target.draw(vr3.back_door, states);
					}
				}
			}
			if (tm3.is(TileProperty::DARKNESS))
				target.draw(vm3.darkness, states);
			else {
				if (_has_wall(tm3, player_facing))
					target.draw(vm3.back_wall, states);
				if (_has_normal_door(tm3, player_facing)) {
					target.draw(vm3.back_wall, states);
					target.draw(vm3.back_door, states);
				}
				if (_has_secret_door(tm3, player_facing)) {
					target.draw(vm3.back_wall, states);
					if (_game->state->get_lit())
						target.draw(vm3.back_door, states);
				}
			}

			if (!tm3.is(TileProperty::DARKNESS)) {
				if (_has_wall(tm3, _get_left_side(player_facing)))
					target.draw(vm3.left_side_wall, states);
				if (_has_normal_door(tm3, _get_left_side(player_facing))) {
					target.draw(vm3.left_side_wall, states);
					target.draw(vm3.left_side_door, states);
				}
				if (_has_secret_door(tm3, _get_left_side(player_facing))) {
					target.draw(vm3.left_side_wall, states);
					if (_game->state->get_lit())
						target.draw(vm3.left_side_door, states);
				}

				if (_has_wall(tm3, _get_right_side(player_facing)))
					target.draw(vm3.right_side_wall, states);
				if (_has_normal_door(tm3, _get_right_side(player_facing))) {
					target.draw(vm3.right_side_wall, states);
					if (_game->state->get_lit())
						target.draw(vm3.right_side_door, states);
				}
				if (_has_secret_door(tm3, _get_right_side(player_facing))) {
					target.draw(vm3.right_side_wall, states);
					if (_game->state->get_lit())
						target.draw(vm3.right_side_door, states);
				}
			}

			// Row 2
			if (tl2.is(TileProperty::DARKNESS)) {
				target.draw(vl1.darkness, states);
				target.draw(vl2.side_darkness, states);
			} else {
				if (_has_wall(tl2, player_facing))
					target.draw(vl2.back_wall, states);
				if (_has_normal_door(tl2, player_facing)) {
					target.draw(vl2.back_wall, states);
					target.draw(vl2.back_door, states);
				}
				if (_has_secret_door(tl2, player_facing)) {
					target.draw(vl2.back_wall, states);
					if (_game->state->get_lit())
						target.draw(vl2.back_door, states);
				}
			}
			if (tr2.is(TileProperty::DARKNESS)) {
				target.draw(vr1.darkness, states);
				target.draw(vr2.side_darkness, states);
			} else {
				if (_has_wall(tr2, player_facing))
					target.draw(vr2.back_wall, states);
				if (_has_normal_door(tr2, player_facing)) {
					target.draw(vr2.back_wall, states);
					target.draw(vr2.back_door, states);
				}
				if (_has_secret_door(tr2, player_facing)) {
					target.draw(vr2.back_wall, states);
					if (_game->state->get_lit())
						target.draw(vr2.back_door, states);
				}
			}
			if (tm2.is(TileProperty::DARKNESS))
				target.draw(vm2.darkness, states);
			else {
				if (_has_wall(tm2, player_facing))
					target.draw(vm2.back_wall, states);
				if (_has_normal_door(tm2, player_facing)) {
					target.draw(vm2.back_wall, states);
					target.draw(vm2.back_door, states);
				}
				if (_has_secret_door(tm2, player_facing)) {
					target.draw(vm2.back_wall, states);
					if (_game->state->get_lit())
						target.draw(vm2.back_door, states);
				}
			}

			if (!tm2.is(TileProperty::DARKNESS)) {
				if (_has_wall(tm2, _get_left_side(player_facing)))
					target.draw(vm2.left_side_wall, states);
				if (_has_normal_door(tm2, _get_left_side(player_facing))) {
					target.draw(vm2.left_side_wall, states);
					target.draw(vm2.left_side_door, states);
				}
				if (_has_secret_door(tm2, _get_left_side(player_facing))) {
					target.draw(vm2.left_side_wall, states);
					if (_game->state->get_lit())
						target.draw(vm2.left_side_door, states);
				}

				if (_has_wall(tm2, _get_right_side(player_facing)))
					target.draw(vm2.right_side_wall, states);
				if (_has_normal_door(tm2, _get_right_side(player_facing))) {
					target.draw(vm2.right_side_wall, states);
					target.draw(vm2.right_side_door, states);
				}
				if (_has_secret_door(tm2, _get_right_side(player_facing))) {
					target.draw(vm2.right_side_wall, states);
					if (_game->state->get_lit())
						target.draw(vm2.right_side_door, states);
				}
			}
		} else {

			if (tl2.is(TileProperty::DARKNESS))
				target.draw(vl1.darkness, states);
			if (tm2.is(TileProperty::DARKNESS))
				target.draw(vm2.darkness, states);
			if (tr2.is(TileProperty::DARKNESS))
				target.draw(vr1.darkness, states);
		}

		// Row 1
		if (tl1.is(TileProperty::DARKNESS)) {
			target.draw(vl0.darkness, states);
			target.draw(vl1.side_darkness, states);
		} else {
			if (_has_wall(tl1, player_facing))
				target.draw(vl1.back_wall, states);
			if (_has_normal_door(tl1, player_facing)) {
				target.draw(vl1.back_wall, states);
				target.draw(vl1.back_door, states);
			}
			if (_has_secret_door(tl1, player_facing)) {
				target.draw(vl1.back_wall, states);
				if (_game->state->get_lit())
					target.draw(vl1.back_door, states);
			}

			if ((tl1.has(TileFeature::MESSAGE)) || (tl1.has(TileFeature::NOTICE)))
				target.draw(vl1.floor, states);
			if ((tl1.has(TileFeature::STAIRS_DOWN)) || (tl1.has(TileFeature::LADDER_DOWN)) ||
				(tl1.has(TileFeature::ELEVATOR_DOWN)))
				target.draw(vl1.down, states);
			if ((tl1.has(TileFeature::STAIRS_UP)) || (tl1.has(TileFeature::LADDER_UP)) ||
				(tl1.has(TileFeature::ELEVATOR_UP)))
				target.draw(vl1.up, states);
			//}
		}

		if (tr1.is(TileProperty::DARKNESS)) {
			target.draw(vr0.darkness, states);
			target.draw(vr1.side_darkness, states);
		} else {
			if (_has_wall(tr1, player_facing))
				target.draw(vr1.back_wall, states);
			if (_has_normal_door(tr1, player_facing)) {
				target.draw(vr1.back_wall, states);
				target.draw(vr1.back_door, states);
			}
			if (_has_secret_door(tr1, player_facing)) {
				target.draw(vr1.back_wall, states);
				if (_game->state->get_lit())
					target.draw(vr1.back_door, states);
			}
			if ((tr1.has(TileFeature::MESSAGE)) || (tr1.has(TileFeature::NOTICE)))
				target.draw(vr1.floor, states);
			if ((tr1.has(TileFeature::STAIRS_DOWN)) || (tr1.has(TileFeature::LADDER_DOWN)) ||
				(tr1.has(TileFeature::ELEVATOR_DOWN)))
				target.draw(vr1.down, states);
			if ((tr1.has(TileFeature::STAIRS_UP)) || (tr1.has(TileFeature::LADDER_UP)) ||
				(tr1.has(TileFeature::ELEVATOR_UP)))
				target.draw(vr1.up, states);
			//}
		}
		if (tm1.is(TileProperty::DARKNESS))
			target.draw(vm1.darkness, states);
		else {
			if (_has_wall(tm1, player_facing))
				target.draw(vm1.back_wall, states);
			if (_has_normal_door(tm1, player_facing)) {
				target.draw(vm1.back_wall, states);
				target.draw(vm1.back_door, states);
			}
			if (_has_secret_door(tm1, player_facing)) {
				target.draw(vm1.back_wall, states);
				if (_game->state->get_lit())
					target.draw(vm1.back_door, states);
			}

			if ((tm1.has(TileFeature::MESSAGE)) || (tm1.has(TileFeature::NOTICE)))
				target.draw(vm1.floor, states);
			if ((tm1.has(TileFeature::STAIRS_DOWN)) || (tm1.has(TileFeature::LADDER_DOWN)) ||
				(tm1.has(TileFeature::ELEVATOR_DOWN)))
				target.draw(vm1.down, states);
			if ((tm1.has(TileFeature::STAIRS_UP)) || (tm1.has(TileFeature::LADDER_UP)) ||
				(tm1.has(TileFeature::ELEVATOR_UP)))
				target.draw(vm1.up, states);
		}

		if (!tm1.is(TileProperty::DARKNESS)) {
			if (_has_wall(tm1, _get_left_side(player_facing)))
				target.draw(vm1.left_side_wall, states);
			if (_has_normal_door(tm1, _get_left_side(player_facing))) {
				target.draw(vm1.left_side_wall, states);
				target.draw(vm1.left_side_door, states);
			}
			if (_has_secret_door(tm1, _get_left_side(player_facing))) {
				target.draw(vm1.left_side_wall, states);
				if (_game->state->get_lit())
					target.draw(vm1.left_side_door, states);
			}

			if (_has_wall(tm1, _get_right_side(player_facing)))
				target.draw(vm1.right_side_wall, states);
			if (_has_normal_door(tm1, _get_right_side(player_facing))) {
				target.draw(vm1.right_side_wall, states);
				target.draw(vm1.right_side_door, states);
			}
			if (_has_secret_door(tm1, _get_right_side(player_facing))) {
				target.draw(vm1.right_side_wall, states);
				if (_game->state->get_lit())
					target.draw(vm1.right_side_door, states);
			}
		}

		// Row 0
		if (_has_wall(tl0, player_facing))
			target.draw(vl0.back_wall, states);
		if (tl0.is(TileProperty::DARKNESS)) {
			target.draw(vl0.darkness, states);
			target.draw(vl0.side_darkness, states);
		} else {
			if (_has_normal_door(tl0, player_facing)) {
				target.draw(vl0.back_wall, states);
				target.draw(vl0.back_door, states);
			}
			if (_has_secret_door(tl0, player_facing)) {
				target.draw(vl0.back_wall, states);
				if (_game->state->get_lit())
					target.draw(vl0.back_door, states);
			}

			if ((tl0.has(TileFeature::MESSAGE)) || (tl0.has(TileFeature::NOTICE)))
				target.draw(vl0.floor, states);
			if ((tl0.has(TileFeature::STAIRS_DOWN)) || (tl0.has(TileFeature::LADDER_DOWN)) ||
				(tl0.has(TileFeature::ELEVATOR_DOWN)))
				target.draw(vl0.down, states);
			if ((tl0.has(TileFeature::STAIRS_UP)) || (tl0.has(TileFeature::LADDER_UP)) ||
				(tl0.has(TileFeature::ELEVATOR_UP)))
				target.draw(vl0.up, states);
		}

		// If we have reached here, we aren't standing in darkness
		if (_has_wall(tm0, player_facing))
			target.draw(vm0.back_wall, states);
		if (_has_normal_door(tm0, player_facing)) {
			target.draw(vm0.back_wall, states);
			target.draw(vm0.back_door, states);
		}
		if (_has_secret_door(tm0, player_facing)) {
			target.draw(vm0.back_wall, states);
			if (_game->state->get_lit())
				target.draw(vm0.back_door, states);
		}

		if ((tm0.has(TileFeature::MESSAGE)) || (tm0.has(TileFeature::NOTICE)))
			target.draw(vm0.floor, states);
		if ((tm0.has(TileFeature::STAIRS_DOWN)) || (tm0.has(TileFeature::LADDER_DOWN)) ||
			(tm0.has(TileFeature::ELEVATOR_DOWN)))
			target.draw(vm0.down, states);
		if ((tm0.has(TileFeature::STAIRS_UP)) || (tm0.has(TileFeature::LADDER_UP)) ||
			(tm0.has(TileFeature::ELEVATOR_UP)))
			target.draw(vm0.up, states);

		if (_has_wall(tr0, player_facing))
			target.draw(vr0.back_wall, states);
		if (tr0.is(TileProperty::DARKNESS)) {
			target.draw(vr0.darkness, states);
			target.draw(vr0.side_darkness, states);
		} else {
			if (_has_normal_door(tr0, player_facing)) {
				target.draw(vr0.back_wall, states);
				target.draw(vr0.back_door, states);
			}
			if (_has_secret_door(tr0, player_facing)) {
				target.draw(vr0.back_wall, states);
				if (_game->state->get_lit())
					target.draw(vr0.back_door, states);
			}

			if ((tr0.has(TileFeature::MESSAGE)) || (tr0.has(TileFeature::NOTICE)))
				target.draw(vr0.floor, states);
			if ((tr0.has(TileFeature::STAIRS_DOWN)) || (tr0.has(TileFeature::LADDER_DOWN)) ||
				(tr0.has(TileFeature::ELEVATOR_DOWN)))
				target.draw(vr0.down, states);
			if ((tr0.has(TileFeature::STAIRS_UP)) || (tr0.has(TileFeature::LADDER_UP)) ||
				(tr0.has(TileFeature::ELEVATOR_UP)))
				target.draw(vr0.up, states);
		}

		if (!tm0.is(TileProperty::DARKNESS)) {

			if (_has_wall(tm0, _get_left_side(player_facing)))
				target.draw(vm0.left_side_wall, states);
			if (_has_normal_door(tm0, _get_left_side(player_facing))) {
				target.draw(vm0.left_side_wall, states);
				target.draw(vm0.left_side_door, states);
			}
			if (_has_secret_door(tm0, _get_left_side(player_facing))) {
				target.draw(vm0.left_side_wall, states);
				if (_game->state->get_lit())
					target.draw(vm0.left_side_door, states);
			}

			if (_has_wall(tm0, _get_right_side(player_facing)))
				target.draw(vm0.right_side_wall, states);
			if (_has_normal_door(tm0, _get_right_side(player_facing))) {
				target.draw(vm0.right_side_wall, states);
				target.draw(vm0.right_side_door, states);
			}
			if (_has_secret_door(tm0, _get_right_side(player_facing))) {
				target.draw(vm0.right_side_wall, states);
				if (_game->state->get_lit())
					target.draw(vm0.right_side_door, states);
			}
		}
	}
}
