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

#include "map.hpp"

Sorcery::Map::Map(System *system, Display *display, Graphics *graphics, Game *game, Component layout)
	: _system{system}, _display{display}, _graphics{graphics}, _game{game}, _layout{layout} {

	_sprites.clear();
	_texts.clear();

	if (_outside_frame.get()) {
		_outside_frame.release();
		_outside_frame.reset();
	}

	_outside_frame = std::make_unique<Frame>(_display->ui_texture, WindowFrameType::NORMAL, _layout.w, _layout.h,
		_layout.colour, _layout.background, _layout.alpha);
	auto f_sprite{_outside_frame->sprite};
	f_sprite.setPosition(0, 0);
	_sprites.emplace_back(f_sprite);
}

auto Sorcery::Map::refresh() -> void {

	_sprites.resize(2);
	_texts.clear();

	auto tc{20};
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
	for (auto y = 0; y <= 19; y++) {
		for (auto x = 0; x <= 19; x++) {

			auto lx{[&] {
				if (x < _game->state->level->wrap_bottom_left().x)
					return x + static_cast<int>(_game->state->level->wrap_size().w);
				else if (x > _game->state->level->wrap_top_right().x)
					return x - static_cast<int>(_game->state->level->wrap_size().w);
				return x;
			}()};
			auto ly{[&] {
				if (y < _game->state->level->wrap_bottom_left().y)
					return y + static_cast<int>(_game->state->level->wrap_size().h);
				else if (y > _game->state->level->wrap_top_right().y)
					return y - static_cast<int>(_game->state->level->wrap_size().h);
				return y;
			}()};

			auto tile{_game->state->level->at(lx, ly)};
			auto tile_x{tx + (tcx * tw) + (tcx * spacing)};
			auto tile_y{ty + (tcy * th) + (tcy * spacing)};
			_draw_tile(tile, Coordinate{lx, ly}, tile_x, reverse_y - tile_y, scaling);
			if ((x == static_cast<int>(player_pos.x)) && (y == static_cast<int>(player_pos.y)))
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

	const auto player_depth{_game->state->level->depth()};

	const auto coord{[&] {
		if (player_depth < 0)
			return fmt::format("B{}F {:>2}N/{:>2}E", std::abs(player_depth), player_pos.y, player_pos.x);
		else
			return fmt::format("{}F {:>2}/{:>2}E", std::abs(player_depth), player_pos.y, player_pos.x);
	}()};

	sf::Text coord_text{};
	coord_text.setFont(_system->resources->fonts[_layout.font]);
	coord_text.setCharacterSize(cts);
	coord_text.setFillColor(sf::Color(ctc));
	coord_text.setString(coord);
	coord_text.setPosition(ctx, cty);
	_texts.push_back(coord_text);
}

auto Sorcery::Map::_draw_player(MapDirection direction, int x, int y, float scaling) -> void {

	auto icon{AutoMapFeature::NONE};
	switch (direction) {
	case MapDirection::NORTH:
		icon = AutoMapFeature::PLAYER_NORTH;
		break;
	case MapDirection::SOUTH:
		icon = AutoMapFeature::PLAYER_SOUTH;
		break;
	case MapDirection::EAST:
		icon = AutoMapFeature::PLAYER_EAST;
		break;
	case MapDirection::WEST:
		icon = AutoMapFeature::PLAYER_WEST;
		break;
	default:
		break;
	}

	sf::Sprite player{
		_graphics->textures->get(magic_enum::enum_integer<AutoMapFeature>(icon), GraphicsTextureType::AUTOMAP).value()};
	player.setPosition(x, y);
	player.setScale(scaling, scaling);
	_sprites.emplace_back(player);
}

// TODO
auto Sorcery::Map::_draw_tile(Tile &tile, Coordinate loc, int x, int y, float scaling) -> void {

	{
		sf::Sprite bg{
			_graphics->textures
				->get(magic_enum::enum_integer<AutoMapFeature>(AutoMapFeature::FLOOR), GraphicsTextureType::AUTOMAP)
				.value()};
		bg.setPosition(x, y);
		bg.setScale(scaling, scaling);
		_sprites.emplace_back(bg);
	}

	if (_game->state->explored[_game->state->get_depth()].at(loc)) {

		sf::Sprite bg_explored{
			_graphics->textures
				->get(magic_enum::enum_integer<AutoMapFeature>(AutoMapFeature::FLOOR), GraphicsTextureType::AUTOMAP)
				.value()};
		bg_explored.setPosition(x, y);
		bg_explored.setScale(scaling, scaling);
		bg_explored.setColor(sf::Color(0x000000ff));
		_sprites.emplace_back(bg_explored);

		if (tile.is(TileProperty::DARKNESS)) {
			sf::Sprite property{_graphics->textures
									->get(magic_enum::enum_integer<AutoMapFeature>(AutoMapFeature::DARKNESS),
										GraphicsTextureType::AUTOMAP)
									.value()};
			property.setPosition(x, y);
			property.setScale(scaling, scaling);
			_sprites.emplace_back(property);
		}

		if ((tile.has(MapDirection::NORTH, TileEdge::SECRET_DOOR)) ||
			(tile.has(MapDirection::NORTH, TileEdge::ONE_WAY_HIDDEN_DOOR))) {
			sf::Sprite wall{_graphics->textures
								->get(magic_enum::enum_integer<AutoMapFeature>(AutoMapFeature::NORTH_SECRET),
									GraphicsTextureType::AUTOMAP)
								.value()};
			wall.setPosition(x, y);
			wall.setScale(scaling, scaling);
			_sprites.emplace_back(wall);
		} else if ((tile.has(MapDirection::NORTH, TileEdge::UNLOCKED_DOOR)) ||
				   (tile.has(MapDirection::NORTH, TileEdge::ONE_WAY_DOOR))) {
			sf::Sprite wall{_graphics->textures
								->get(magic_enum::enum_integer<AutoMapFeature>(AutoMapFeature::NORTH_DOOR),
									GraphicsTextureType::AUTOMAP)
								.value()};
			wall.setPosition(x, y);
			wall.setScale(scaling, scaling);
			_sprites.emplace_back(wall);
		} else if (tile.has(MapDirection::NORTH, TileEdge::ONE_WAY_WALL)) {
			sf::Sprite wall{_graphics->textures
								->get(magic_enum::enum_integer<AutoMapFeature>(AutoMapFeature::NORTH_ONE_WAY_WALL),
									GraphicsTextureType::AUTOMAP)
								.value()};
			wall.setPosition(x, y);
			wall.setScale(scaling, scaling);
			_sprites.emplace_back(wall);
		} else if (tile.has(MapDirection::NORTH)) {
			sf::Sprite wall{_graphics->textures
								->get(magic_enum::enum_integer<AutoMapFeature>(AutoMapFeature::NORTH_WALL),
									GraphicsTextureType::AUTOMAP)
								.value()};
			wall.setPosition(x, y);
			wall.setScale(scaling, scaling);
			_sprites.emplace_back(wall);
		}

		if ((tile.has(MapDirection::SOUTH, TileEdge::SECRET_DOOR)) ||
			(tile.has(MapDirection::SOUTH, TileEdge::ONE_WAY_HIDDEN_DOOR))) {
			sf::Sprite wall{_graphics->textures
								->get(magic_enum::enum_integer<AutoMapFeature>(AutoMapFeature::SOUTH_SECRET),
									GraphicsTextureType::AUTOMAP)
								.value()};
			wall.setPosition(x, y);
			wall.setScale(scaling, scaling);
			_sprites.emplace_back(wall);
		} else if ((tile.has(MapDirection::SOUTH, TileEdge::UNLOCKED_DOOR)) ||
				   (tile.has(MapDirection::SOUTH, TileEdge::ONE_WAY_DOOR))) {
			sf::Sprite wall{_graphics->textures
								->get(magic_enum::enum_integer<AutoMapFeature>(AutoMapFeature::SOUTH_DOOR),
									GraphicsTextureType::AUTOMAP)
								.value()};
			wall.setPosition(x, y);
			wall.setScale(scaling, scaling);
			_sprites.emplace_back(wall);
		} else if (tile.has(MapDirection::SOUTH, TileEdge::ONE_WAY_WALL)) {
			sf::Sprite wall{_graphics->textures
								->get(magic_enum::enum_integer<AutoMapFeature>(AutoMapFeature::SOUTH_ONE_WAY_WALL),
									GraphicsTextureType::AUTOMAP)
								.value()};
			wall.setPosition(x, y);
			wall.setScale(scaling, scaling);
			_sprites.emplace_back(wall);

		} else if (tile.has(MapDirection::SOUTH)) {
			sf::Sprite wall{_graphics->textures
								->get(magic_enum::enum_integer<AutoMapFeature>(AutoMapFeature::SOUTH_WALL),
									GraphicsTextureType::AUTOMAP)
								.value()};
			wall.setPosition(x, y);
			wall.setScale(scaling, scaling);
			_sprites.emplace_back(wall);
		}
		if ((tile.has(MapDirection::EAST, TileEdge::SECRET_DOOR)) ||
			(tile.has(MapDirection::EAST, TileEdge::ONE_WAY_HIDDEN_DOOR))) {
			sf::Sprite wall{_graphics->textures
								->get(magic_enum::enum_integer<AutoMapFeature>(AutoMapFeature::EAST_SECRET),
									GraphicsTextureType::AUTOMAP)
								.value()};
			wall.setPosition(x, y);
			wall.setScale(scaling, scaling);
			_sprites.emplace_back(wall);
		} else if ((tile.has(MapDirection::EAST, TileEdge::UNLOCKED_DOOR)) ||
				   (tile.has(MapDirection::EAST, TileEdge::ONE_WAY_DOOR))) {
			sf::Sprite wall{_graphics->textures
								->get(magic_enum::enum_integer<AutoMapFeature>(AutoMapFeature::EAST_DOOR),
									GraphicsTextureType::AUTOMAP)
								.value()};
			wall.setPosition(x, y);
			wall.setScale(scaling, scaling);
			_sprites.emplace_back(wall);
		} else if (tile.has(MapDirection::EAST, TileEdge::ONE_WAY_WALL)) {
			sf::Sprite wall{_graphics->textures
								->get(magic_enum::enum_integer<AutoMapFeature>(AutoMapFeature::EAST_ONE_WAY_WALL),
									GraphicsTextureType::AUTOMAP)
								.value()};
			wall.setPosition(x, y);
			wall.setScale(scaling, scaling);
			_sprites.emplace_back(wall);
		} else if (tile.has(MapDirection::EAST)) {
			sf::Sprite wall{_graphics->textures
								->get(magic_enum::enum_integer<AutoMapFeature>(AutoMapFeature::EAST_WALL),
									GraphicsTextureType::AUTOMAP)
								.value()};
			wall.setPosition(x, y);
			wall.setScale(scaling, scaling);
			_sprites.emplace_back(wall);
		}

		if ((tile.has(MapDirection::WEST, TileEdge::SECRET_DOOR)) ||
			(tile.has(MapDirection::WEST, TileEdge::ONE_WAY_HIDDEN_DOOR))) {
			sf::Sprite wall{_graphics->textures
								->get(magic_enum::enum_integer<AutoMapFeature>(AutoMapFeature::WEST_SECRET),
									GraphicsTextureType::AUTOMAP)
								.value()};
			wall.setPosition(x, y);
			wall.setScale(scaling, scaling);
			_sprites.emplace_back(wall);
		} else if (tile.has(MapDirection::WEST, TileEdge::ONE_WAY_WALL)) {
			sf::Sprite wall{_graphics->textures
								->get(magic_enum::enum_integer<AutoMapFeature>(AutoMapFeature::WEST_ONE_WAY_WALL),
									GraphicsTextureType::AUTOMAP)
								.value()};
			wall.setPosition(x, y);
			wall.setScale(scaling, scaling);
			_sprites.emplace_back(wall);
		} else if ((tile.has(MapDirection::WEST, TileEdge::UNLOCKED_DOOR)) ||
				   (tile.has(MapDirection::WEST, TileEdge::ONE_WAY_DOOR))) {
			sf::Sprite wall{_graphics->textures
								->get(magic_enum::enum_integer<AutoMapFeature>(AutoMapFeature::WEST_DOOR),
									GraphicsTextureType::AUTOMAP)
								.value()};
			wall.setPosition(x, y);
			wall.setScale(scaling, scaling);
			_sprites.emplace_back(wall);

		} else if (tile.has(MapDirection::WEST)) {
			sf::Sprite wall{_graphics->textures
								->get(magic_enum::enum_integer<AutoMapFeature>(AutoMapFeature::WEST_WALL),
									GraphicsTextureType::AUTOMAP)
								.value()};
			wall.setPosition(x, y);
			wall.setScale(scaling, scaling);
			_sprites.emplace_back(wall);
		}

		if ((tile.has(TileFeature::STAIRS_UP)) || (tile.has(TileFeature::LADDER_UP))) {
			sf::Sprite stairs{_graphics->textures
								  ->get(magic_enum::enum_integer<AutoMapFeature>(AutoMapFeature::STAIRS_UP),
									  GraphicsTextureType::AUTOMAP)
								  .value()};
			stairs.setPosition(x, y);
			stairs.setScale(scaling, scaling);
			_sprites.emplace_back(stairs);
		} else if ((tile.has(TileFeature::STAIRS_DOWN)) || (tile.has(TileFeature::LADDER_DOWN))) {
			sf::Sprite stairs{_graphics->textures
								  ->get(magic_enum::enum_integer<AutoMapFeature>(AutoMapFeature::STAIRS_DOWN),
									  GraphicsTextureType::AUTOMAP)
								  .value()};
			stairs.setPosition(x, y);
			stairs.setScale(scaling, scaling);
			_sprites.emplace_back(stairs);
		} else if (tile.has(TileFeature::ELEVATOR)) {
			sf::Sprite stairs{_graphics->textures
								  ->get(magic_enum::enum_integer<AutoMapFeature>(AutoMapFeature::ELEVATOR),
									  GraphicsTextureType::AUTOMAP)
								  .value()};
			stairs.setPosition(x, y);
			stairs.setScale(scaling, scaling);
			_sprites.emplace_back(stairs);
		} else if (tile.has(TileFeature::SPINNER)) {
			sf::Sprite message{_graphics->textures
								   ->get(magic_enum::enum_integer<AutoMapFeature>(AutoMapFeature::SPINNER),
									   GraphicsTextureType::AUTOMAP)
								   .value()};
			message.setPosition(x, y);
			message.setScale(scaling, scaling);
			_sprites.emplace_back(message);
		} else if (tile.has(TileFeature::PIT)) {
			sf::Sprite message{
				_graphics->textures
					->get(magic_enum::enum_integer<AutoMapFeature>(AutoMapFeature::PIT), GraphicsTextureType::AUTOMAP)
					.value()};
			message.setPosition(x, y);
			message.setScale(scaling, scaling);
			_sprites.emplace_back(message);
		} else if (tile.has(TileFeature::CHUTE)) {
			sf::Sprite message{
				_graphics->textures
					->get(magic_enum::enum_integer<AutoMapFeature>(AutoMapFeature::CHUTE), GraphicsTextureType::AUTOMAP)
					.value()};
			message.setPosition(x, y);
			message.setScale(scaling, scaling);
			_sprites.emplace_back(message);
		} else if (tile.has(TileFeature::TELEPORT_TO)) {
			sf::Sprite stairs{_graphics->textures
								  ->get(magic_enum::enum_integer<AutoMapFeature>(AutoMapFeature::TELEPORT_TO),
									  GraphicsTextureType::AUTOMAP)
								  .value()};
			stairs.setPosition(x, y);
			stairs.setScale(scaling, scaling);
			_sprites.emplace_back(stairs);
		} else {
			if ((tile.has(TileFeature::MESSAGE)) || (tile.has(TileFeature::NOTICE))) {
				sf::Sprite message{_graphics->textures
									   ->get(magic_enum::enum_integer<AutoMapFeature>(AutoMapFeature::EXCLAMATION),
										   GraphicsTextureType::AUTOMAP)
									   .value()};
				message.setPosition(x, y);
				message.setScale(scaling, scaling);
				_sprites.emplace_back(message);
			}
		}
	}
}

auto Sorcery::Map::draw(sf::RenderTarget &target, sf::RenderStates states) const -> void {

	states.transform *= getTransform();

	// Draw the standard components
	for (const auto &sprite : _sprites)
		target.draw(sprite, states);

	for (const auto &text : _texts)
		target.draw(text, states);
}
