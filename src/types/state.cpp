// Copyright (C) 2026 Dave Moore
//
// This file is part of Sorcery.
//
// Sorcery is free software: you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software
// Foundation, either version 2 of the License, or (at your option) any later
// version.
//
// Sorcery is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with
// Sorcery.  If not, see <http://www.gnu.org/licenses/>.
//
// If you modify this program, or any covered work, by linking or combining
// it with the libraries referred to in README (or a modified version of
// said libraries), containing parts covered by the terms of said libraries,
// the licensors of this program grant you additional permission to convey
// the resulting work.

#include "types/state.hpp"
#include "common/enum.hpp"		   // for MessageType, Direction
#include "core/context.hpp"		   // for Context
#include "core/system.hpp"		   // for System
#include "resources/itemstore.hpp" // for ItemStore
#include "types/item/itemtype.hpp" // for ItemType
#include "types/meta.hpp"		   // for enum_cast
#include <algorithm>			   // for find_if, remove_if
#include <format>				   // for format
#include <functional>			   // for invoke
#include <iterator>				   // for distance, end
#include <print>				   // for println
#include <string_view>			   // for literals
#include <utility>				   // for to_underlying

namespace Sorcery {
namespace Enums {
	namespace Items { enum class TypeID; }
}
}

using namespace std::literals;

// Constructor used by Cereal to serialise this item
Sorcery::State::State() {}

// Normal Constructor
Sorcery::State::State(Context *ctx)
	: _ctx{ctx} {

	_clear();
	_restart_expedition();
}

/// @brief
/// @param itemstore
/// @return
auto Sorcery::State::reset_shop(ItemStore *itemstore) -> void {

	for (int id = 0; id < 101; id++) {
		const auto item_type{itemstore->get(enum_cast<Enums::Items::TypeID>(id).value())};
		_shop[id] = {item_type.get_shop_inital_stock(), item_type.get_shop_inital_stock(), item_type.get_can_be_sold(),
					 item_type.get_can_be_bought()};
	}
}

/// @brief
/// @return
auto Sorcery::State::print() -> void {

	auto text{"State:\n\n"s};

	text.append(std::format("{}\n", _version));
	text.append(std::format("{}\n", _player_depth));
	text.append(std::format("{}\\{}\n", _player_pos.x, _player_pos.y));
	text.append(std::format("{}\n", (int)_playing_facing));
	text.append(std::format("{}\n", _light_turns));
	text.append(std::format("{}\n\n[", _turns));

	for (const auto id : _party) {
		auto line{std::format("{},", id)};
		text.append(line);
	}
	text.append("]\n");
	std::println("{}", text);
}

/// @brief
/// @return
auto Sorcery::State::_clear() -> void {

	_party.clear();

	level.reset();
	level = std::make_unique<Level>();
	_clear_explored();
	_version = 1;
	_turns = 0;

	_log.clear();
}

/// @brief
/// @return
auto Sorcery::State::get_turns() const -> unsigned int {

	return _turns;
}

/// @brief
/// @param turns
/// @return
auto Sorcery::State::pass_turn(unsigned int turns) -> void {

	_turns += turns;
	if (_light_turns > 0)
		--_light_turns;
}

/// @brief
/// @return
auto Sorcery::State::_clear_explored() -> void {

	explored.clear();

	for (auto depth = -1; depth >= -10; --depth)
		explored.emplace(depth, Explore{});
}

/// @brief /

/// @param char_id
/// @return
auto Sorcery::State::add_character_by_id(unsigned int char_id) -> bool {

	return add_character_to_party(char_id);
}

/// @brief
/// @param new_order
/// @return
auto Sorcery::State::reorder_party(std::vector<unsigned int> &new_order) -> void {

	_party.clear();
	for (auto char_id : new_order)
		_party.push_back(char_id);
}

/// @brief
/// @return
auto Sorcery::State::restart_expedition() -> void {

	_restart_expedition();
}

/// @brief
/// @param turns
/// @return
auto Sorcery::State::set_lit(const int turns) -> void {

	_light_turns = turns;
}

/// @brief
/// @return
auto Sorcery::State::get_lit() const -> bool {

	return _light_turns > 0;
}

/// @brief
/// @return
auto Sorcery::State::get_lit_turns() const -> int {

	return _light_turns;
}

/// @brief
/// @param value
/// @return
auto Sorcery::State::set_maporfic(const bool value) -> void {

	_maporfic = value;
}

/// @brief
/// @return
auto Sorcery::State::get_maporifc() const -> bool {

	return _maporfic;
}

/// @brief
/// @param value
/// @return
auto Sorcery::State::set_latumapic(const bool value) -> void {

	_latumapic = value;
}

/// @brief
/// @return
auto Sorcery::State::get_latumapic() const -> bool {

	return _latumapic;
}

/// @brief
/// @param depth
/// @return
auto Sorcery::State::set_player_prev_depth(int depth) -> void {

	_previous_depth = depth;
}

/// @brief
/// @param depth
/// @return
auto Sorcery::State::set_depth(int depth) -> void {

	_player_depth = depth;
}

/// @brief
/// @return
auto Sorcery::State::get_depth() const -> int {

	return _player_depth;
}

/// @brief
/// @return
auto Sorcery::State::get_player_prev_depth() const -> int {

	return _previous_depth;
}

/// @brief
/// @return
auto Sorcery::State::_restart_expedition() -> void {

	_playing_facing = Enums::Map::Direction::NORTH;
	_player_pos = Coordinate{0, 0};
	_player_depth = -1;
	_light_turns = 0;
}

/// @brief
/// @param ctx
/// @return
auto Sorcery::State::post_construct(Context *ctx) -> void {

	_ctx = ctx;
}

/// @brief  Method called to simulate Normal Constructor with Cereal Constructor
/// @param ctx
/// @return
auto Sorcery::State::set(Context *ctx) -> void {

	_ctx = ctx;
}

/// @brief
/// @param candidate_party
/// @return

auto Sorcery::State::set_party(std::vector<unsigned int> candidate_party) -> void {

	_party = candidate_party;
}

/// @brief
/// @return
auto Sorcery::State::party_has_members() const -> bool {

	return _party.size() > 0;
}

/// @brief
/// @return
auto Sorcery::State::get_party_size() const -> unsigned int {

	return _party.size();
}

/// @brief
/// @return
auto Sorcery::State::get_party_characters() const -> std::vector<unsigned int> {

	return _party;
}

/// @brief
/// @return
auto Sorcery::State::clear_party() -> void {

	_party.clear();
}

/// @brief
/// @param char_id
/// @return
auto Sorcery::State::add_character_to_party(unsigned int char_id) -> bool {

	if (_party.size() < 6) {
		_party.push_back(char_id);
		return true;
	} else
		return false;
}

/// @brief
/// @param char_id
/// @return
auto Sorcery::State::check_character_in_party(unsigned int char_id) -> bool {

	if (_party.size() > 0) {
		const auto found{std::find_if(_party.begin(), _party.end(), [&](unsigned int id) {
			return id == char_id;
		})};
		return found != std::end(_party);
	} else
		return false;
}

/// @brief
/// @param other
/// @return
auto Sorcery::State::set_current_level(Level *other) -> void {

	level->set(other);
}

/// @brief Return the *slot* of a character in the party (note this is 1-indexed!)
/// @param char_id
/// @return
auto Sorcery::State::get_char_slot(unsigned int char_id) -> std::optional<unsigned int> {

	if (_party.size() > 0) {

		const auto distance{std::find_if(_party.begin(), _party.end(), [&](unsigned int id) {
			return id == char_id;
		})};
		if (distance != _party.end())
			return (std::distance(_party.begin(), distance)) + 1;
		else
			return std::nullopt;
	} else
		return std::nullopt;
}

/// @brief
/// @return
auto Sorcery::State::get_player_facing() const -> Enums::Map::Direction {

	return _playing_facing;
}

/// @brief
/// @return
auto Sorcery::State::get_player_pos() const -> Coordinate {

	return _player_pos;
}

/// @brief
/// @return
auto Sorcery::State::get_player_prev_pos() const -> Coordinate {

	return _previous_pos;
}

/// @brief
/// @param direction
/// @return
auto Sorcery::State::set_player_facing(const Enums::Map::Direction direction) -> void {

	_playing_facing = direction;
}

/// @brief
/// @param position
/// @return
auto Sorcery::State::set_player_pos(const Coordinate position) -> void {

	_previous_pos = _player_pos;
	_player_pos = position;
}

/// @brief
/// @param char_id
/// @return
auto Sorcery::State::remove_character_by_id(unsigned int char_id) -> bool {

	if (_party.size() > 0) {
		_party.erase(std::remove_if(_party.begin(), _party.end(),
									[&](unsigned int id) {
										return id == char_id;
									}),
					 _party.end());
		return true;
	} else
		return false;
}

/// @brief
/// @param index
/// @return
auto Sorcery::State::get_party_char(unsigned int index) -> std::optional<unsigned int> {

	// 1-indexed
	if (_party.size() < index)
		return std::nullopt;
	else
		return _party.at(index - 1);
}

/// @brief
/// @param character_id
/// @return
auto Sorcery::State::get_next_party_character(unsigned int character_id) -> std::optional<unsigned int> {

	auto index{get_char_slot(character_id)};
	if (!index)
		return std::nullopt;

	auto slot{index.value()}; // 1-based
	auto next_slot{slot == _party.size() ? 1u : slot + 1u};

	return _party.at(next_slot - 1); // return character ID
}

/// @brief
/// @param character_id
/// @return
auto Sorcery::State::get_previous_party_character(unsigned int character_id) -> std::optional<unsigned int> {

	auto index{get_char_slot(character_id)};
	if (!index)
		return std::nullopt;

	auto slot{index.value()}; // 1-based
	auto prev_slot{slot == 1u ? _party.size() : slot - 1u};

	return _party.at(prev_slot - 1); // return character ID
}

/// @brief
/// @param index
/// @return
auto Sorcery::State::remove_character_by_position(unsigned int index) -> bool {

	if (_party.size() > index) {
		_party.erase(_party.begin() + index);
		return true;
	} else
		return false;
}

/// @brief
/// @param text
/// @param type
/// @return
auto Sorcery::State::add_log_message(std::string text,
									 Enums::Internal::MessageType type = Enums::Internal::MessageType::STANDARD)
	-> void {

	_log.emplace_back(ConsoleMessage{type, text});
}

/// @brief
/// @return
auto Sorcery::State::clear_log_messages() -> void {

	_log.clear();
}

/// @brief
/// @param message
/// @param dice
/// @param roll
/// @param needed
/// @return
auto Sorcery::State::add_log_dice_roll(const std::string &message, const int dice, const int roll, const int needed)
	-> void {

	if (dice != -1 || roll != -1 || needed != -1) {
		const auto success{roll < needed ? "SUCCESS" : "FAILURE"};
		const auto string{std::format("{} ({})", message, success)};
		add_log_message(_ctx->system->dice_roll_to_str(string, dice, roll, needed), Enums::Internal::MessageType::ROLL);
	} else
		add_log_message(message, Enums::Internal::MessageType::GAME);
}

/// @brief
/// @param last
/// @return
auto Sorcery::State::get_log_messages(unsigned int last) const -> std::vector<ConsoleMessage> {

	if (last == 0)
		return _log;
	else {
		if (_log.size() < last)
			last = _log.size();

		std::vector<ConsoleMessage> results{_log.cend() - last, _log.cend()};
		return results;
	}
}

/// @brief
/// @param item_type
/// @return
auto Sorcery::State::check_shop_stock(const Enums::Items::TypeID item_type) const -> int {

	return _shop[std::to_underlying(item_type)].current_stock;
}

/// @brief
/// @param item_type
/// @return
auto Sorcery::State::check_shop_will_sell(const Enums::Items::TypeID item_type) const -> bool {

	return _shop[std::to_underlying(item_type)].shop_will_sell;
}

/// @brief
/// @param item_type
/// @return
auto Sorcery::State::check_shop_will_buy(const Enums::Items::TypeID item_type) const -> bool {

	return _shop[std::to_underlying(item_type)].shop_will_buy;
}

/// @brief
/// @param item_type
/// @return
auto Sorcery::State::sell_to_shop(const Enums::Items::TypeID item_type) -> void {

	auto &stock{_shop[std::to_underlying(item_type)]};

	if (stock.current_stock != -1)
		++stock.current_stock;
}
/// @brief
/// @param item_type
/// @return
auto Sorcery::State::buy_from_shop(const Enums::Items::TypeID item_type) -> void {

	auto &stock{_shop[std::to_underlying(item_type)]};

	// -1 means unlimited stock.
	if (stock.current_stock != -1)
		--stock.current_stock;
}

/// @brief
/// @param itemstore
/// @param item_type
/// @return
auto Sorcery::State::get_shop_display(ItemStore *itemstore, const Enums::Items::TypeID item_type) -> std::string {

	const auto item{itemstore->get(item_type)};
	const std::string flag{std::invoke([&] {
		if (_shop[std::to_underlying(item_type)].current_stock == -1)
			return std::string{"(*)"};
		else
			return std::format("({})", _shop[std::to_underlying(item_type)].current_stock);
	})};

	const std::string line{std::format("{:>16} {:<5} {:>7} GP", item.get_display_name(), flag, item.get_value())};

	return line;
}
