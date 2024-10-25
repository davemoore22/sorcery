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
#include "gui/include.hpp"
#include "resources/factory.hpp"
#include "resources/resourcemanager.hpp"
#include "resources/texturestore.hpp"
#include "types/explore.hpp"
#include "types/level.hpp"
#include "types/tile.hpp"

Sorcery::AutoMap::AutoMap(System *system, Display *display, Graphics *graphics,
	Game *game, Component layout)
	: _system{system}, _display{display}, _graphics{graphics}, _game{game},
	  _layout{layout} {

	_sprites.clear();
	_texts.clear();

	// Setup the Factory
	_factory = std::make_unique<Factory>(_system, _display, _graphics, _game);

	_map_radius = std::stoi(_layout["tile_count"].value()) / 2;

	// Make the Top Frame
	_top_frame = _factory->make_comp_frame(_layout, _sprites);

	// Bottom Frame is Custom (TODO: break the coordinates out into new comp)
	const auto cfh{std::stoi(_layout["coordinates_frame_h"].value())};
	const auto cfy{std::stoi(_layout["coordinates_frame_y"].value())};
	_bottom_frame =
		std::make_unique<Frame>(_system->resources->get_texture(GTX::UI),
			_layout.w, cfh, _layout.colour, _layout.background, _layout.alpha);
	auto b_sprite{_bottom_frame->sprite};
	b_sprite.setPosition(0, cfy);
	_sprites.emplace_back(b_sprite);
}

auto Sorcery::AutoMap::is_mouse_over(
	Component &component, sf::Vector2f mouse_pos) const -> bool {

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
	auto pos{_game->state->get_player_pos()};
	for (auto y = static_cast<int>(pos.y - _map_radius);
		y <= static_cast<int>(pos.y + _map_radius); y++) {
		for (auto x = static_cast<int>(pos.x - _map_radius);
			x <= static_cast<int>(pos.x) + _map_radius; x++) {

			auto lx{std::invoke([&] {
				if (x < _game->state->level->wrap_bottom_left().x)
					return x +
						   static_cast<int>(_game->state->level->wrap_size().w);
				else if (x > _game->state->level->wrap_top_right().x)
					return x -
						   static_cast<int>(_game->state->level->wrap_size().w);
				return x;
			})};
			auto ly{std::invoke([&] {
				if (y < _game->state->level->wrap_bottom_left().y)
					return y +
						   static_cast<int>(_game->state->level->wrap_size().h);
				else if (y > _game->state->level->wrap_top_right().y)
					return y -
						   static_cast<int>(_game->state->level->wrap_size().h);
				return y;
			})};

			auto tile{_game->state->level->at(lx, ly)};
			auto tile_x{tx + (tcx * tw) + (tcx * spacing)};
			auto tile_y{ty + (tcy * th) + (tcy * spacing)};
			_draw_tile(
				tile, Coordinate{lx, ly}, tile_x, reverse_y - tile_y, scaling);
			if (x == static_cast<int>(pos.x) && y == static_cast<int>(pos.y))
				_draw_player(_game->state->get_player_facing(), tile_x,
					reverse_y - tile_y, scaling);

			++tcx;
		}
		++tcy;
		tcx = 0;
	}

	// And coordinates
	const auto cts{std::stoi(_layout["coordinates_text_size"].value())};
	const auto ctc{
		std::stoull(_layout["coordinates_text_colour"].value(), 0, 16)};
	const auto ctx{std::stoi(_layout["coordinates_text_x"].value())};
	const auto cty{std::stoi(_layout["coordinates_text_y"].value())};
	const auto cfy{std::stoi(_layout["coordinates_frame_y"].value())};

	const auto depth{_game->state->level->depth()};

	auto coord{std::invoke([&] {
		if (depth < 0)
			return fmt::format(
				"B{}F {:>2}N/{:>2}E", std::abs(depth), pos.y, pos.x);
		else
			return fmt::format(
				"{}F {:>2}/{:>2}E", std::abs(depth), pos.y, pos.x);
	})};

	// Display Coordinate Text
	sf::Text text{};
	text.setFont(_system->resources->fonts[_layout.font]);
	text.setCharacterSize(cts);
	text.setFillColor(sf::Color(ctc));
	if (_display->get_upper())
		std::transform(coord.begin(), coord.end(), coord.begin(), ::toupper);
	text.setString(coord);
	text.setPosition(ctx, cfy + cty);
	if (_display->get_bold())
		text.setStyle(sf::Text::Bold);
	_texts.push_back(text);
}

auto Sorcery::AutoMap::_draw_player(MAD direction, int x, int y, float scaling)
	-> void {

	auto icon{AMF::NO_FEATURE};
	switch (direction) {
	case MAD::NORTH:
		icon = AMF::PLAYER_NORTH;
		break;
	case MAD::SOUTH:
		icon = AMF::PLAYER_SOUTH;
		break;
	case MAD::EAST:
		icon = AMF::PLAYER_EAST;
		break;
	case MAD::WEST:
		icon = AMF::PLAYER_WEST;
		break;
	default:
		break;
	}

	sf::Sprite player{
		_graphics->textures->get(unenum(icon), GTT::AUTOMAP).value()};
	player.setPosition(x, y);
	player.setScale(scaling, scaling);
	_sprites.emplace_back(player);
}

// TODO
auto Sorcery::AutoMap::_draw_tile(
	Tile &tile, Coordinate loc, int x, int y, float scaling) -> void {

	sf::Sprite bg{
		_graphics->textures->get(unenum(AMF::FLOOR), GTT::AUTOMAP).value()};
	bg.setPosition(x, y);
	bg.setScale(scaling, scaling);
	_sprites.emplace_back(bg);

	if (_game->state->explored[_game->state->get_depth()].at(loc)) {

		sf::Sprite bg{
			_graphics->textures->get(unenum(AMF::FLOOR), GTT::AUTOMAP).value()};
		bg.setPosition(x, y);
		bg.setScale(scaling, scaling);
		bg.setColor(sf::Color(0x000000ff));
		_sprites.emplace_back(bg);

		if (tile.is(TLP::DARKNESS)) {
			sf::Sprite property{
				_graphics->textures->get(unenum(AMF::DARKNESS), GTT::AUTOMAP)
					.value()};
			property.setPosition(x, y);
			property.setScale(scaling, scaling);
			_sprites.emplace_back(property);
		}

		if (tile.has(MAD::NORTH, TLE::SECRET_DOOR) ||
			tile.has(MAD::NORTH, TLE::ONE_WAY_HIDDEN_DOOR)) {
			sf::Sprite wall{_graphics->textures
					->get(unenum(AMF::NORTH_SECRET), GTT::AUTOMAP)
					.value()};
			wall.setPosition(x, y);
			wall.setScale(scaling, scaling);
			_sprites.emplace_back(wall);
		} else if (tile.has(MAD::NORTH, TLE::UNLOCKED_DOOR) ||
				   tile.has(MAD::NORTH, TLE::ONE_WAY_DOOR)) {
			sf::Sprite wall{
				_graphics->textures->get(unenum(AMF::NORTH_DOOR), GTT::AUTOMAP)
					.value()};
			wall.setPosition(x, y);
			wall.setScale(scaling, scaling);
			_sprites.emplace_back(wall);
		} else if (tile.has(MAD::NORTH, TLE::ONE_WAY_WALL)) {
			sf::Sprite wall{_graphics->textures
					->get(unenum(AMF::NORTH_ONE_WAY_WALL), GTT::AUTOMAP)
					.value()};
			wall.setPosition(x, y);
			wall.setScale(scaling, scaling);
			_sprites.emplace_back(wall);
		} else if (tile.has(MAD::NORTH)) {
			sf::Sprite wall{
				_graphics->textures->get(unenum(AMF::NORTH_WALL), GTT::AUTOMAP)
					.value()};
			wall.setPosition(x, y);
			wall.setScale(scaling, scaling);
			_sprites.emplace_back(wall);
		}

		if (tile.has(MAD::SOUTH, TLE::SECRET_DOOR) ||
			tile.has(MAD::SOUTH, TLE::ONE_WAY_HIDDEN_DOOR)) {
			sf::Sprite wall{_graphics->textures
					->get(unenum(AMF::SOUTH_SECRET), GTT::AUTOMAP)
					.value()};
			wall.setPosition(x, y);
			wall.setScale(scaling, scaling);
			_sprites.emplace_back(wall);
		} else if (tile.has(MAD::SOUTH, TLE::UNLOCKED_DOOR) ||
				   tile.has(MAD::SOUTH, TLE::ONE_WAY_DOOR)) {
			sf::Sprite wall{
				_graphics->textures->get(unenum(AMF::SOUTH_DOOR), GTT::AUTOMAP)
					.value()};
			wall.setPosition(x, y);
			wall.setScale(scaling, scaling);
			_sprites.emplace_back(wall);
		} else if (tile.has(MAD::SOUTH, TLE::ONE_WAY_WALL)) {
			sf::Sprite wall{_graphics->textures
					->get(unenum(AMF::SOUTH_ONE_WAY_WALL), GTT::AUTOMAP)
					.value()};
			wall.setPosition(x, y);
			wall.setScale(scaling, scaling);
			_sprites.emplace_back(wall);

		} else if (tile.has(MAD::SOUTH)) {
			sf::Sprite wall{
				_graphics->textures->get(unenum(AMF::SOUTH_WALL), GTT::AUTOMAP)
					.value()};
			wall.setPosition(x, y);
			wall.setScale(scaling, scaling);
			_sprites.emplace_back(wall);
		}
		if (tile.has(MAD::EAST, TLE::SECRET_DOOR) ||
			tile.has(MAD::EAST, TLE::ONE_WAY_HIDDEN_DOOR)) {
			sf::Sprite wall{
				_graphics->textures->get(unenum(AMF::EAST_SECRET), GTT::AUTOMAP)
					.value()};
			wall.setPosition(x, y);
			wall.setScale(scaling, scaling);
			_sprites.emplace_back(wall);
		} else if (tile.has(MAD::EAST, TLE::UNLOCKED_DOOR) ||
				   tile.has(MAD::EAST, TLE::ONE_WAY_DOOR)) {
			sf::Sprite wall{
				_graphics->textures->get(unenum(AMF::EAST_DOOR), GTT::AUTOMAP)
					.value()};
			wall.setPosition(x, y);
			wall.setScale(scaling, scaling);
			_sprites.emplace_back(wall);
		} else if (tile.has(MAD::EAST, TLE::ONE_WAY_WALL)) {
			sf::Sprite wall{_graphics->textures
					->get(unenum(AMF::EAST_ONE_WAY_WALL), GTT::AUTOMAP)
					.value()};
			wall.setPosition(x, y);
			wall.setScale(scaling, scaling);
			_sprites.emplace_back(wall);
		} else if (tile.has(MAD::EAST)) {
			sf::Sprite wall{
				_graphics->textures->get(unenum(AMF::EAST_WALL), GTT::AUTOMAP)
					.value()};
			wall.setPosition(x, y);
			wall.setScale(scaling, scaling);
			_sprites.emplace_back(wall);
		}

		if (tile.has(MAD::WEST, TLE::SECRET_DOOR) ||
			tile.has(MAD::WEST, TLE::ONE_WAY_HIDDEN_DOOR)) {
			sf::Sprite wall{
				_graphics->textures->get(unenum(AMF::WEST_SECRET), GTT::AUTOMAP)
					.value()};
			wall.setPosition(x, y);
			wall.setScale(scaling, scaling);
			_sprites.emplace_back(wall);
		} else if (tile.has(MAD::WEST, TLE::ONE_WAY_WALL)) {
			sf::Sprite wall{_graphics->textures
					->get(unenum(AMF::WEST_ONE_WAY_WALL), GTT::AUTOMAP)
					.value()};
			wall.setPosition(x, y);
			wall.setScale(scaling, scaling);
			_sprites.emplace_back(wall);
		} else if (tile.has(MAD::WEST, TLE::UNLOCKED_DOOR) ||
				   tile.has(MAD::WEST, TLE::ONE_WAY_DOOR)) {
			sf::Sprite wall{
				_graphics->textures->get(unenum(AMF::WEST_DOOR), GTT::AUTOMAP)
					.value()};
			wall.setPosition(x, y);
			wall.setScale(scaling, scaling);
			_sprites.emplace_back(wall);

		} else if (tile.has(MAD::WEST)) {
			sf::Sprite wall{
				_graphics->textures->get(unenum(AMF::WEST_WALL), GTT::AUTOMAP)
					.value()};
			wall.setPosition(x, y);
			wall.setScale(scaling, scaling);
			_sprites.emplace_back(wall);
		}

		if (tile.has(TLF::STAIRS_UP) || tile.has(TLF::LADDER_UP)) {
			sf::Sprite stairs{
				_graphics->textures->get(unenum(AMF::STAIRS_UP), GTT::AUTOMAP)
					.value()};
			stairs.setPosition(x, y);
			stairs.setScale(scaling, scaling);
			_sprites.emplace_back(stairs);
		} else if (tile.has(TLF::STAIRS_DOWN) || tile.has(TLF::LADDER_DOWN)) {
			sf::Sprite stairs{
				_graphics->textures->get(unenum(AMF::STAIRS_DOWN), GTT::AUTOMAP)
					.value()};
			stairs.setPosition(x, y);
			stairs.setScale(scaling, scaling);
			_sprites.emplace_back(stairs);
		} else if (tile.has(TLF::ELEVATOR)) {
			sf::Sprite stairs{
				_graphics->textures->get(unenum(AMF::ELEVATOR), GTT::AUTOMAP)
					.value()};
			stairs.setPosition(x, y);
			stairs.setScale(scaling, scaling);
			_sprites.emplace_back(stairs);
		} else if (tile.has(TLF::SPINNER)) {
			sf::Sprite message{
				_graphics->textures->get(unenum(AMF::SPINNER), GTT::AUTOMAP)
					.value()};
			message.setPosition(x, y);
			message.setScale(scaling, scaling);
			_sprites.emplace_back(message);
		} else if (tile.has(TLF::PIT)) {
			sf::Sprite message{
				_graphics->textures->get(unenum(AMF::PIT), GTT::AUTOMAP)
					.value()};
			message.setPosition(x, y);
			message.setScale(scaling, scaling);
			_sprites.emplace_back(message);
		} else if (tile.has(TLF::CHUTE)) {
			sf::Sprite message{
				_graphics->textures->get(unenum(AMF::CHUTE), GTT::AUTOMAP)
					.value()};
			message.setPosition(x, y);
			message.setScale(scaling, scaling);
			_sprites.emplace_back(message);
		} else if (tile.has(TLF::TELEPORT_TO)) {
			sf::Sprite stairs{
				_graphics->textures->get(unenum(AMF::TELEPORT_TO), GTT::AUTOMAP)
					.value()};
			stairs.setPosition(x, y);
			stairs.setScale(scaling, scaling);
			_sprites.emplace_back(stairs);
		} else {
			// TODO: change this to use events instead
			if (tile.has(TLF::MESSAGE) || tile.has(TLF::NOTICE)) {
				sf::Sprite message{_graphics->textures
						->get(unenum(AMF::EXCLAMATION), GTT::AUTOMAP)
						.value()};
				message.setPosition(x, y);
				message.setScale(scaling, scaling);
				_sprites.emplace_back(message);
			}
		}
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
