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

#include "gui/automap.hpp"
#include "common/type.hpp"
#include "core/display.hpp"
#include "core/game.hpp"
#include "core/graphics.hpp"
#include "core/state.hpp"
#include "core/system.hpp"
#include "gui/enum.hpp"
#include "resources/resourcemanager.hpp"
#include "resources/texturestore.hpp"
#include "types/explore.hpp"
#include "types/level.hpp"
#include "types/tile.hpp"

Sorcery::AutoMap::AutoMap(System *system, Display *display, Graphics *graphics, Game *game, Component layout)
	: _system{system}, _display{display}, _graphics{graphics}, _game{game}, _layout{layout} {

	_sprites.clear();
	_texts.clear();

	_map_radius = std::stoi(_layout["tile_count"].value()) / 2;

	if (_top_frame.get()) {
		_top_frame.release();
		_top_frame.reset();
	}

	if (_bottom_frame.get()) {
		_bottom_frame.release();
		_bottom_frame.reset();
	}
	_top_frame = std::make_unique<Frame>(
		_display->ui_texture, _layout.w, _layout.h, _layout.colour, _layout.background, _layout.alpha);
	auto cfh{std::stoi(_layout["coordinates_frame_h"].value())};
	auto cfy{std::stoi(_layout["coordinates_frame_y"].value())};

	_bottom_frame = std::make_unique<Frame>(
		_display->ui_texture, _layout.w, cfh, _layout.colour, _layout.background, _layout.alpha);
	auto t_sprite{_top_frame->sprite};
	auto b_sprite{_bottom_frame->sprite};
	t_sprite.setPosition(0, 0);
	b_sprite.setPosition(0, cfy);
	_sprites.emplace_back(t_sprite);
	_sprites.emplace_back(b_sprite);
}

auto Sorcery::AutoMap::is_mouse_over(Component &component, sf::Vector2f mouse_pos) const -> bool {

	for (auto sprite : _sprites) {
		sf::Rect sprite_area{sprite.getGlobalBounds()};
		sprite_area.left += component.x;
		sprite_area.top += component.y;
		if (sprite_area.contains(mouse_pos))
			return true;
	}

	return false;
}

auto Sorcery::AutoMap::refresh() -> void {

	_sprites.resize(2);
	_texts.clear();

	// need to take into account explored etc
	auto tc{std::stoi(_layout["tile_count"].value())};
	auto scaling{std::stof(_layout["tile_scaling"].value())};
	auto tx{std::stoi(_layout["tile_offset_x"].value())};
	auto ty{std::stoi(_layout["tile_offset_y"].value())};
	auto tw{std::stoi(_layout["tile_width"].value())};
	auto th{std::stoi(_layout["tile_height"].value())};
	auto spacing{std::stoi(_layout["tile_spacing"].value())};
	tw *= scaling;
	th *= scaling;
	auto reverse_y{(th * tc) + ((tc - 1) * spacing) + 2};

	auto tcx{0};
	auto tcy{0};
	auto player_pos{_game->state->get_player_pos()};
	for (auto y = static_cast<int>(player_pos.y - _map_radius); y <= static_cast<int>(player_pos.y + _map_radius);
		 y++) {
		for (auto x = static_cast<int>(player_pos.x - _map_radius); x <= static_cast<int>(player_pos.x) + _map_radius;
			 x++) {

			auto lx{std::invoke([&] {
				if (x < _game->state->level->wrap_bottom_left().x)
					return x + static_cast<int>(_game->state->level->wrap_size().w);
				else if (x > _game->state->level->wrap_top_right().x)
					return x - static_cast<int>(_game->state->level->wrap_size().w);
				return x;
			})};
			auto ly{std::invoke([&] {
				if (y < _game->state->level->wrap_bottom_left().y)
					return y + static_cast<int>(_game->state->level->wrap_size().h);
				else if (y > _game->state->level->wrap_top_right().y)
					return y - static_cast<int>(_game->state->level->wrap_size().h);
				return y;
			})};

			auto tile{_game->state->level->at(lx, ly)};
			auto tile_x{tx + (tcx * tw) + (tcx * spacing)};
			auto tile_y{ty + (tcy * th) + (tcy * spacing)};
			_draw_tile(tile, Coordinate{lx, ly}, tile_x, reverse_y - tile_y, scaling);
			if (x == static_cast<int>(player_pos.x) && y == static_cast<int>(player_pos.y))
				_draw_player(_game->state->get_player_facing(), tile_x, reverse_y - tile_y, scaling);

			++tcx;
		}
		++tcy;
		tcx = 0;
	}

	// And coordinates
	const auto cts{std::stoi(_layout["coordinates_text_size"].value())};
	const auto ctc{std::stoull(_layout["coordinates_text_colour"].value(), 0, 16)};
	const auto ctx{std::stoi(_layout["coordinates_text_x"].value())};
	const auto cty{std::stoi(_layout["coordinates_text_y"].value())};
	const auto cfy{std::stoi(_layout["coordinates_frame_y"].value())};

	const auto player_depth{_game->state->level->depth()};

	auto coord{std::invoke([&] {
		if (player_depth < 0)
			return fmt::format("B{}F {:>2}N/{:>2}E", std::abs(player_depth), player_pos.y, player_pos.x);
		else
			return fmt::format("{}F {:>2}/{:>2}E", std::abs(player_depth), player_pos.y, player_pos.x);
	})};

	sf::Text coord_text{};
	coord_text.setFont(_system->resources->fonts[_layout.font]);
	coord_text.setCharacterSize(cts);
	coord_text.setFillColor(sf::Color(ctc));
	if (_display->get_upper())
		std::transform(coord.begin(), coord.end(), coord.begin(), ::toupper);
	coord_text.setString(coord);
	coord_text.setPosition(ctx, cfy + cty);
	if (_display->get_bold())
		coord_text.setStyle(sf::Text::Bold);
	_texts.push_back(coord_text);
}

auto Sorcery::AutoMap::_draw_player(MapDirection direction, int x, int y, float scaling) -> void {

	using enum Sorcery::Enums::Automap::Feature;
	using enum Sorcery::Enums::Map::Direction;

	auto icon{NO_FEATURE};
	switch (direction) {
	case NORTH:
		icon = PLAYER_NORTH;
		break;
	case SOUTH:
		icon = PLAYER_SOUTH;
		break;
	case EAST:
		icon = PLAYER_EAST;
		break;
	case WEST:
		icon = PLAYER_WEST;
		break;
	default:
		break;
	}

	sf::Sprite player{_graphics->textures->get(unenum(icon), GraphicsTextureType::AUTOMAP).value()};
	player.setPosition(x, y);
	player.setScale(scaling, scaling);
	_sprites.emplace_back(player);
}

// TODO
auto Sorcery::AutoMap::_draw_tile(Tile &tile, Coordinate loc, int x, int y, float scaling) -> void {

	using enum Sorcery::Enums::Graphics::TextureType;
	using enum Sorcery::Enums::Map::Direction;

	sf::Sprite bg{_graphics->textures->get(unenum(AutoMapFeature::FLOOR), AUTOMAP).value()};
	bg.setPosition(x, y);
	bg.setScale(scaling, scaling);
	_sprites.emplace_back(bg);

	if (_game->state->explored[_game->state->get_depth()].at(loc)) {

		sf::Sprite bg_explored{_graphics->textures->get(unenum(AutoMapFeature::FLOOR), AUTOMAP).value()};
		bg_explored.setPosition(x, y);
		bg_explored.setScale(scaling, scaling);
		bg_explored.setColor(sf::Color(0x000000ff));
		_sprites.emplace_back(bg_explored);

		if (tile.is(TileProperty::DARKNESS)) {
			sf::Sprite property{_graphics->textures->get(unenum(AutoMapFeature::DARKNESS), AUTOMAP).value()};
			property.setPosition(x, y);
			property.setScale(scaling, scaling);
			_sprites.emplace_back(property);
		}

		if (tile.has(NORTH, TileEdge::SECRET_DOOR) || tile.has(NORTH, TileEdge::ONE_WAY_HIDDEN_DOOR)) {
			sf::Sprite wall{_graphics->textures->get(unenum(AutoMapFeature::NORTH_SECRET), AUTOMAP).value()};
			wall.setPosition(x, y);
			wall.setScale(scaling, scaling);
			_sprites.emplace_back(wall);
		} else if (tile.has(NORTH, TileEdge::UNLOCKED_DOOR) || tile.has(NORTH, TileEdge::ONE_WAY_DOOR)) {
			sf::Sprite wall{_graphics->textures->get(unenum(AutoMapFeature::NORTH_DOOR), AUTOMAP).value()};
			wall.setPosition(x, y);
			wall.setScale(scaling, scaling);
			_sprites.emplace_back(wall);
		} else if (tile.has(NORTH, TileEdge::ONE_WAY_WALL)) {
			sf::Sprite wall{_graphics->textures->get(unenum(AutoMapFeature::NORTH_ONE_WAY_WALL), AUTOMAP).value()};
			wall.setPosition(x, y);
			wall.setScale(scaling, scaling);
			_sprites.emplace_back(wall);
		} else if (tile.has(NORTH)) {
			sf::Sprite wall{_graphics->textures->get(unenum(AutoMapFeature::NORTH_WALL), AUTOMAP).value()};
			wall.setPosition(x, y);
			wall.setScale(scaling, scaling);
			_sprites.emplace_back(wall);
		}

		if (tile.has(SOUTH, TileEdge::SECRET_DOOR) || tile.has(SOUTH, TileEdge::ONE_WAY_HIDDEN_DOOR)) {
			sf::Sprite wall{_graphics->textures->get(unenum(AutoMapFeature::SOUTH_SECRET), AUTOMAP).value()};
			wall.setPosition(x, y);
			wall.setScale(scaling, scaling);
			_sprites.emplace_back(wall);
		} else if (tile.has(SOUTH, TileEdge::UNLOCKED_DOOR) || tile.has(SOUTH, TileEdge::ONE_WAY_DOOR)) {
			sf::Sprite wall{_graphics->textures->get(unenum(AutoMapFeature::SOUTH_DOOR), AUTOMAP).value()};
			wall.setPosition(x, y);
			wall.setScale(scaling, scaling);
			_sprites.emplace_back(wall);
		} else if (tile.has(SOUTH, TileEdge::ONE_WAY_WALL)) {
			sf::Sprite wall{_graphics->textures->get(unenum(AutoMapFeature::SOUTH_ONE_WAY_WALL), AUTOMAP).value()};
			wall.setPosition(x, y);
			wall.setScale(scaling, scaling);
			_sprites.emplace_back(wall);

		} else if (tile.has(SOUTH)) {
			sf::Sprite wall{_graphics->textures->get(unenum(AutoMapFeature::SOUTH_WALL), AUTOMAP).value()};
			wall.setPosition(x, y);
			wall.setScale(scaling, scaling);
			_sprites.emplace_back(wall);
		}
		if (tile.has(EAST, TileEdge::SECRET_DOOR) || tile.has(EAST, TileEdge::ONE_WAY_HIDDEN_DOOR)) {
			sf::Sprite wall{_graphics->textures->get(unenum(AutoMapFeature::EAST_SECRET), AUTOMAP).value()};
			wall.setPosition(x, y);
			wall.setScale(scaling, scaling);
			_sprites.emplace_back(wall);
		} else if (tile.has(EAST, TileEdge::UNLOCKED_DOOR) || tile.has(EAST, TileEdge::ONE_WAY_DOOR)) {
			sf::Sprite wall{_graphics->textures->get(unenum(AutoMapFeature::EAST_DOOR), AUTOMAP).value()};
			wall.setPosition(x, y);
			wall.setScale(scaling, scaling);
			_sprites.emplace_back(wall);
		} else if (tile.has(EAST, TileEdge::ONE_WAY_WALL)) {
			sf::Sprite wall{_graphics->textures->get(unenum(AutoMapFeature::EAST_ONE_WAY_WALL), AUTOMAP).value()};
			wall.setPosition(x, y);
			wall.setScale(scaling, scaling);
			_sprites.emplace_back(wall);
		} else if (tile.has(EAST)) {
			sf::Sprite wall{_graphics->textures->get(unenum(AutoMapFeature::EAST_WALL), AUTOMAP).value()};
			wall.setPosition(x, y);
			wall.setScale(scaling, scaling);
			_sprites.emplace_back(wall);
		}

		if (tile.has(WEST, TileEdge::SECRET_DOOR) || tile.has(WEST, TileEdge::ONE_WAY_HIDDEN_DOOR)) {
			sf::Sprite wall{_graphics->textures->get(unenum(AutoMapFeature::WEST_SECRET), AUTOMAP).value()};
			wall.setPosition(x, y);
			wall.setScale(scaling, scaling);
			_sprites.emplace_back(wall);
		} else if (tile.has(WEST, TileEdge::ONE_WAY_WALL)) {
			sf::Sprite wall{_graphics->textures->get(unenum(AutoMapFeature::WEST_ONE_WAY_WALL), AUTOMAP).value()};
			wall.setPosition(x, y);
			wall.setScale(scaling, scaling);
			_sprites.emplace_back(wall);
		} else if (tile.has(WEST, TileEdge::UNLOCKED_DOOR) || tile.has(WEST, TileEdge::ONE_WAY_DOOR)) {
			sf::Sprite wall{_graphics->textures->get(unenum(AutoMapFeature::WEST_DOOR), AUTOMAP).value()};
			wall.setPosition(x, y);
			wall.setScale(scaling, scaling);
			_sprites.emplace_back(wall);

		} else if (tile.has(WEST)) {
			sf::Sprite wall{_graphics->textures->get(unenum(AutoMapFeature::WEST_WALL), AUTOMAP).value()};
			wall.setPosition(x, y);
			wall.setScale(scaling, scaling);
			_sprites.emplace_back(wall);
		}

		if (tile.has(TileFeature::STAIRS_UP) || tile.has(TileFeature::LADDER_UP)) {
			sf::Sprite stairs{_graphics->textures->get(unenum(AutoMapFeature::STAIRS_UP), AUTOMAP).value()};
			stairs.setPosition(x, y);
			stairs.setScale(scaling, scaling);
			_sprites.emplace_back(stairs);
		} else if (tile.has(TileFeature::STAIRS_DOWN) || tile.has(TileFeature::LADDER_DOWN)) {
			sf::Sprite stairs{_graphics->textures->get(unenum(AutoMapFeature::STAIRS_DOWN), AUTOMAP).value()};
			stairs.setPosition(x, y);
			stairs.setScale(scaling, scaling);
			_sprites.emplace_back(stairs);
		} else if (tile.has(TileFeature::ELEVATOR)) {
			sf::Sprite stairs{_graphics->textures->get(unenum(AutoMapFeature::ELEVATOR), AUTOMAP).value()};
			stairs.setPosition(x, y);
			stairs.setScale(scaling, scaling);
			_sprites.emplace_back(stairs);
		} else if (tile.has(TileFeature::SPINNER)) {
			sf::Sprite message{_graphics->textures->get(unenum(AutoMapFeature::SPINNER), AUTOMAP).value()};
			message.setPosition(x, y);
			message.setScale(scaling, scaling);
			_sprites.emplace_back(message);
		} else if (tile.has(TileFeature::PIT)) {
			sf::Sprite message{_graphics->textures->get(unenum(AutoMapFeature::PIT), AUTOMAP).value()};
			message.setPosition(x, y);
			message.setScale(scaling, scaling);
			_sprites.emplace_back(message);
		} else if (tile.has(TileFeature::CHUTE)) {
			sf::Sprite message{_graphics->textures->get(unenum(AutoMapFeature::CHUTE), AUTOMAP).value()};
			message.setPosition(x, y);
			message.setScale(scaling, scaling);
			_sprites.emplace_back(message);
		} else if (tile.has(TileFeature::TELEPORT_TO)) {
			sf::Sprite stairs{_graphics->textures->get(unenum(AutoMapFeature::TELEPORT_TO), AUTOMAP).value()};
			stairs.setPosition(x, y);
			stairs.setScale(scaling, scaling);
			_sprites.emplace_back(stairs);
		} else {
			// TODO: change this to use events instead
			if (tile.has(TileFeature::MESSAGE) || tile.has(TileFeature::NOTICE)) {
				sf::Sprite message{_graphics->textures->get(unenum(AutoMapFeature::EXCLAMATION), AUTOMAP).value()};
				message.setPosition(x, y);
				message.setScale(scaling, scaling);
				_sprites.emplace_back(message);
			}
		}
	}
}

auto Sorcery::AutoMap::draw(sf::RenderTarget &target, sf::RenderStates states) const -> void {

	states.transform *= getTransform();

	// Draw the standard components
	for (const auto &sprite : _sprites)
		target.draw(sprite, states);

	for (const auto &text : _texts)
		target.draw(text, states);
}
