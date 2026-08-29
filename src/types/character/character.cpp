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

#include "types/character/character.hpp"
#include "core/context.hpp"		   // for Context
#include "core/resources.hpp"	   // for Resources
#include "resources/itemstore.hpp" // for ItemStore
#include "types/item/item.hpp"	   // for Item
#include "types/item/itemtype.hpp" // for ItemType
#include <array>				   // for array
#include <cmath>				   // for abs
#include <compare>				   // for strong_ordering, operator<, opera...
#include <format>				   // for format
#include <functional>			   // for invoke
#include <memory>				   // for unique_ptr
#include <stdlib.h>				   // for abs
#include <string>				   // for basic_string, string, char_traits
#include <utility>				   // for to_underlying

Sorcery::Character::Character() {}

Sorcery::Character::~Character() {}

// Standard Constructor
Sorcery::Character::Character(Context *ctx)
	: _ctx{ctx} {

	create().set_stage(Enums::Character::Stage::NOT_STARTED);

	_hidden = false;
	set_status(Enums::Character::Status::OK);

	_version = 1;

	_legated = false;
	_wiz_1_award = false;

	_location = Enums::Character::Location::TAVERN;

	coordinate = std::nullopt;
	depth = std::nullopt;

	inventory.clear();
}

auto Sorcery::Character::magic() -> CharacterMagic {

	return CharacterMagic{*this};
}

auto Sorcery::Character::magic() const -> ConstCharacterMagic {

	return ConstCharacterMagic{*this};
}

auto Sorcery::Character::create() -> CharacterCreate {

	return CharacterCreate{*this};
}

auto Sorcery::Character::create() const -> ConstCharacterCreate {

	return ConstCharacterCreate{*this};
}

// Overloaded Operator
auto Sorcery::Character::operator[](const Enums::Character::Ability &key)
	-> int & {

	return _abilities[key];
}

// Utility Functions

auto Sorcery::Character::get_location() const -> Enums::Character::Location {

	return _location;
}

auto Sorcery::Character::set_location(const Enums::Character::Location value)
	-> void {

	_location = value;

	using enum Enums::Character::Location;
	if (value == TAVERN || value == TEMPLE || value == TRAINING) {
		coordinate = std::nullopt;
		depth = std::nullopt;
	}
}

auto Sorcery::Character::abilities()
	-> std::map<Enums::Character::Ability, int> & {

	return _abilities;
}

auto Sorcery::Character::abilities() const
	-> const std::map<Enums::Character::Ability, int> & {

	return _abilities;
}

auto Sorcery::Character::attributes()
	-> std::map<Enums::Character::Attribute, int> & {

	return _cur_attr;
}

auto Sorcery::Character::get_name() const -> std::string {

	return _name;
}

auto Sorcery::Character::get_name_and_status() const -> std::string {

	return std::format("{:<16} {:>12}", _name, get_status_string());
}

auto Sorcery::Character::get_name_and_loc() const -> std::string {

	return std::format("{:<16} B{}F {:>2}N/{:>2}E", _name,
					   std::abs(depth.value()), coordinate.value().y,
					   coordinate.value().x);
}

auto Sorcery::Character::get_name_status_and_loc() const -> std::string {

	const auto out{_location == Enums::Character::Location::MAZE ? "(OUT)"
																 : ""};
	const auto status{
		_status != Enums::Character::Status::OK ? get_status_string() : ""};
	const auto desc{full_desc_text()};

	return std::format("{:<24} {:<9} {:^5}", desc, status, out);
}

auto Sorcery::Character::get_race() const -> Enums::Character::Race {

	return _race;
}

auto Sorcery::Character::get_level() const -> int {

	return _abilities.at(Enums::Character::Ability::CURRENT_LEVEL);
}

auto Sorcery::Character::get_alignment() const -> Enums::Character::Align {

	return _alignment;
}

auto Sorcery::Character::get_class() const -> Enums::Character::Class {

	return _class;
}

auto Sorcery::Character::get_attr_ptr(Enums::Character::Attribute attribute)
	-> int * {

	return &_cur_attr.at(attribute);
}

auto Sorcery::Character::get_cur_attr(
	const Enums::Character::Attribute attribute) const -> unsigned int {

	return _cur_attr.at(attribute);
}

auto Sorcery::Character::get_cur_attr() const
	-> std::map<Enums::Character::Attribute, int> {

	return _cur_attr;
}

// Enum to String functions
auto Sorcery::Character::alignment_to_str(
	Enums::Character::Align character_alignment) const -> std::string {

	static const std::array<std::string, 4> alignments{
		"", _ctx->get_string("CHARACTER_ALIGNMENT_GOOD"),
		_ctx->get_string("CHARACTER_ALIGNMENT_NEUTRAL"),
		_ctx->get_string("CHARACTER_ALIGNMENT_EVIL")};

	return alignments[std::to_underlying(character_alignment)];
}

auto Sorcery::Character::race_to_str(
	Enums::Character::Race character_race) const -> std::string {

	static const std::array<std::string, 6> races{
		"",
		_ctx->get_string("CHARACTER_RACE_HUMAN"),
		_ctx->get_string("CHARACTER_RACE_ELF"),
		_ctx->get_string("CHARACTER_RACE_DWARF"),
		_ctx->get_string("CHARACTER_RACE_GNOME"),
		_ctx->get_string("CHARACTER_RACE_HOBBIT")};

	return races[std::to_underlying(character_race)];
}

auto Sorcery::Character::class_to_str(
	Enums::Character::Class character_class) const -> std::string {

	static const std::array<std::string, 10> classes{
		"",
		_ctx->get_string("CHARACTER_CLASS_FIGHTER"),
		_ctx->get_string("CHARACTER_CLASS_MAGE"),
		_ctx->get_string("CHARACTER_CLASS_PRIEST"),
		_ctx->get_string("CHARACTER_CLASS_THIEF"),
		_ctx->get_string("CHARACTER_CLASS_BISHOP"),
		_ctx->get_string("CHARACTER_CLASS_SAMURAI"),
		_ctx->get_string("CHARACTER_CLASS_LORD"),
		_ctx->get_string("CHARACTER_CLASS_NINJA")};

	return classes[std::to_underlying(character_class)];
}

auto Sorcery::Character::get_ress_chance(bool ashes) -> unsigned int {

	return ashes ? _abilities[Enums::Character::Ability::ASHES_RESURRECT]
				 : _abilities[Enums::Character::Ability::DEAD_RESURRECT];
}

auto Sorcery::Character::grant_gold(const int value) -> void {

	_abilities[Enums::Character::Ability::GOLD] =
		_abilities[Enums::Character::Ability::GOLD] + value;
}

auto Sorcery::Character::get_gold() const -> unsigned int {

	return _abilities.at(Enums::Character::Ability::GOLD);
}

auto Sorcery::Character::set_gold(const unsigned int value) -> void {

	_abilities[Enums::Character::Ability::GOLD] = value;
}

auto Sorcery::Character::is_legated() const -> bool {

	return _legated;
}

auto Sorcery::Character::get_version() const -> int {

	return _version;
}

auto Sorcery::Character::get_identify_trap() const -> int {

	return _abilities.at(Enums::Character::Ability::IDENTIFY_TRAP);
}
auto Sorcery::Character::get_disarm_trap() const -> int {

	return _abilities.at(Enums::Character::Ability::BASE_DISARM_TRAP);
}

auto Sorcery::Character::get_activate_trap() const -> int {

	return _abilities.at(Enums::Character::Ability::ACTIVATE_TRAP);
}

auto Sorcery::Character::grant_xp(const int adjustment) -> int {

	using enum Enums::Character::Ability;
	_abilities[CURRENT_XP] = _abilities[CURRENT_XP] + adjustment;

	return _abilities[CURRENT_XP];
}

auto Sorcery::Character::post_construct(Context *ctx) -> void {

	_ctx = ctx;
}

auto Sorcery::Character::get_current_hp() const -> int {

	return _abilities.at(Enums::Character::Ability::CURRENT_HP);
}

auto Sorcery::Character::get_max_hp() const -> int {

	return _abilities.at(Enums::Character::Ability::MAX_HP);
}

auto Sorcery::Character::get_wiz_1_award() const -> bool {

	return _wiz_1_award;
}

auto Sorcery::Character::set_wiz_1_awatd(const bool value) -> void {

	_wiz_1_award = value;
}

auto Sorcery::Character::get_status() const -> Enums::Character::Status {

	return _status;
}

auto Sorcery::Character::get_condition() const -> std::string {

	return _get_condition();
}

auto Sorcery::Character::get_short_cond() const -> std::string {

	if (_status != Enums::Character::Status::OK)
		return std::format("{:>6}", _get_condition());
	else
		return std::format("{:>4}",
						   _abilities.at(Enums::Character::Ability::MAX_HP));
}

auto Sorcery::Character::_get_condition() const -> std::string {

	using enum Enums::Character::Status;
	if (is_poisoned() && (_status == OK)) {
		return _ctx->get_string("STATUS_POISONED");
	} else if (_status == OK)
		return _ctx->get_string("STATUS_OK");
	else {
		switch (_status) {
		case AFRAID:
			return _ctx->get_string("STATUS_AFRAID");
			break;
		case ASHES:
			return _ctx->get_string("STATUS_ASHES");
			break;
		case ASLEEP:
			return _ctx->get_string("STATUS_ASLEEP");
			break;
		case DEAD:
			return _ctx->get_string("STATUS_DEAD");
			break;
		case LOST:
			return _ctx->get_string("STATUS_LOST");
			break;
		case HELD:
			return _ctx->get_string("STATUS_PARALYSED");
			break;
		case SILENCED:
			return _ctx->get_string("STATUS_SILENCED");
			break;
		case STONED:
			return _ctx->get_string("STATUS_STONED");
			break;
		default:
			return "";
		}
	}
}

auto Sorcery::Character::get_loc_str() const -> std::string {

	switch (_location) {
		using enum Enums::Character::Location;
	case PARTY:
		return _ctx->get_string("LOCATION_PARTY");
		break;
	case TAVERN:
		return _ctx->get_string("LOCATION_TAVERN");
		break;
	case TRAINING:
		return _ctx->get_string("LOCATION_TRAINING");
		break;
	case TEMPLE:
		return _ctx->get_string("LOCATION_TEMPLE");
		break;
	case MAZE:
		return _ctx->get_string("LOCATION_MAZE");
		break;
	default:
		return "";
		break;
	}

	return "";
}

auto Sorcery::Character::get_status_string() const -> std::string {

	if (!_hidden) {
		return _get_condition();
	} else
		return _ctx->get_string("STATUS_HIDDEN");
}

auto Sorcery::Character::set_status(Enums::Character::Status value) -> void {

	if (value == Enums::Character::Status::OK)
		_status = value;
	else {
		auto candidate{std::to_underlying(value)};
		auto current{std::to_underlying(_status)};
		if (candidate > current)
			_status = value;
	}
}

auto Sorcery::Character::set_current_hp(const int hp) -> void {

	_abilities[Enums::Character::Ability::CURRENT_HP] = hp;
}

auto Sorcery::Character::is_poisoned() const -> bool {

	return _abilities.at(Enums::Character::Ability::POISON_STRENGTH) > 0;
}

auto Sorcery::Character::get_poisoned_rate() const -> int {

	return _abilities.at(Enums::Character::Ability::POISON_STRENGTH);
}

auto Sorcery::Character::get_hp_adjustment() const -> int {

	using enum Enums::Character::Ability;
	return _abilities.at(HP_GAIN_PER_TURN) - _abilities.at(HP_LOSS_PER_TURN) -
		   _abilities.at(POISON_STRENGTH);
}

auto Sorcery::Character::get_hp_adjustment_symbol() const -> char {

	using enum Enums::Character::Ability;
	const auto rate{(_abilities.at(HP_GAIN_PER_TURN) -
					 _abilities.at(HP_LOSS_PER_TURN) -
					 _abilities.at(POISON_STRENGTH)) <=> 0};
	if (rate < 0)
		return '-';
	else if (rate > 0)
		return '+';
	else
		return ' ';
}

auto Sorcery::Character::set_hp_gain_per_turn(const int adjustment) -> void {

	_abilities.at(Enums::Character::Ability::HP_GAIN_PER_TURN) = adjustment;
}

auto Sorcery::Character::set_hp_loss_per_turn(const int adjustment) -> void {

	_abilities.at(Enums::Character::Ability::HP_LOSS_PER_TURN) = adjustment;
}

auto Sorcery::Character::reset_adjustment_per_turn() -> void {

	using enum Enums::Character::Ability;
	_abilities.at(HP_GAIN_PER_TURN) = 0;
	_abilities.at(HP_LOSS_PER_TURN) = 0;
}

auto Sorcery::Character::set_poisoned_rate(int value) -> void {

	using enum Enums::Character::Ability;
	if (value > _abilities.at(POISON_STRENGTH)) {
		_abilities.at(POISON_STRENGTH) = value;
	} else if (value == 0) {
		_abilities.at(POISON_STRENGTH) = 0;
	}
}

auto Sorcery::Character::get_poisoned_string() const -> std::string {

	return _abilities.at(Enums::Character::Ability::POISON_STRENGTH) > 0
			   ? std::format(
					 "{:->2}",
					 _abilities.at(Enums::Character::Ability::POISON_STRENGTH))
			   : "";
}

auto Sorcery::Character::get_short_hp_summary() const -> std::string {

	return std::to_string(_abilities.at(Enums::Character::Ability::CURRENT_HP));
}

auto Sorcery::Character::get_hp_summary() const -> std::string {

	return std::format(
		"{}/{}{}",
		std::to_string(_abilities.at(Enums::Character::Ability::CURRENT_HP)),
		std::to_string(_abilities.at(Enums::Character::Ability::MAX_HP)),
		get_hp_adjustment_symbol());
}

// For level draining, optionally keep a track of negative levels unless in
// strict mode Need to also handle character class switching
auto Sorcery::Character::get_summary() -> std::string {

	auto name{_name};
	// if (_display->get_upper())
	//	std::ranges::transform(name.begin(), name.end(), name.begin(),
	//						   ::toupper);
	return std::format("{:<15} L {:>2} {}-{} {}", name,
					   _abilities.at(Enums::Character::Ability::CURRENT_LEVEL),
					   alignment_to_str(_alignment).substr(0, 1),
					   class_to_str(_class).substr(0, 3),
					   race_to_str(_race).substr(0, 3));
}

auto Sorcery::Character::get_summary_and_out() -> std::string {

	auto name{_name};
	// if (_display->get_upper())
	//	std::ranges::transform(name.begin(), name.end(), name.begin(),
	//						   ::toupper);
	auto location{std::invoke([&] {
		if (_location == Enums::Character::Location::MAZE)
			return "  OUT";
		else if (_status == Enums::Character::Status::LOST)
			return " LOST";
		else
			return "    ";
	})};

	return std::format("{:<15} L {:>2} {}-{} {}{:>5}", name,
					   _abilities.at(Enums::Character::Ability::CURRENT_LEVEL),
					   alignment_to_str(_alignment).substr(0, 1),
					   class_to_str(_class).substr(0, 3),
					   race_to_str(_race).substr(0, 3), location);
}

auto Sorcery::Character::can_level() const -> bool {

	return _abilities.at(Enums::Character::Ability::CURRENT_XP) >
		   _abilities.at(Enums::Character::Ability::NEXT_LEVEL_XP);
}

auto Sorcery::Character::get_cure_cost() const -> unsigned int {

	auto cost_per_level{0u};
	switch (_status) {
		using enum Enums::Character::Status;
	case ASHES:
		cost_per_level = 500;
		break;
	case DEAD:
		cost_per_level = 250;
		break;
	case HELD:
		cost_per_level = 100;
		break;
	case STONED:
		cost_per_level = 200;
		break;
	default:
		return 0;
	}

	return cost_per_level *
		   _abilities.at(Enums::Character::Ability::CURRENT_LEVEL);
}

auto Sorcery::Character::get_party_panel_text(const int position)
	-> std::string {

	auto name{_name};
	auto can_level{
		_abilities.at(Enums::Character::Ability::CURRENT_XP) >
				_abilities.at(Enums::Character::Ability::NEXT_LEVEL_XP)
			? "*"
			: " "};
	return std::format(
		"{}{}{:<15} {}-{} {:>2} {:>4}{}{:<6}", position, can_level, name,
		alignment_to_str(_alignment).substr(0, 1),
		class_to_str(_class).substr(0, 3), get_cur_ac(), get_short_hp_summary(),
		get_hp_adjustment_symbol(), get_short_cond());
}

auto Sorcery::Character::get_age() const -> int {

	return _abilities.at(Enums::Character::Ability::AGE);
}

auto Sorcery::Character::set_age(const int adjustment) -> void {

	_abilities.at(Enums::Character::Ability::AGE) =
		_abilities.at(Enums::Character::Ability::AGE) + adjustment;
}

auto Sorcery::Character::full_desc_text() const -> std::string {

	auto name{_name};
	const auto level{_abilities.at(Enums::Character::Ability::CURRENT_LEVEL)};

	return std::format("{:<15} L {:>2} {}-{} {}", name, level,
					   alignment_to_str(_alignment).substr(0, 1),
					   class_to_str(_class).substr(0, 3),
					   race_to_str(_race).substr(0, 3));
};

auto Sorcery::Character::summary_text_with_awards() const -> std::string {

	auto name{_name};
	auto legacy{_legated ? " (D)" : ""};
	const auto level{_abilities.at(Enums::Character::Ability::CURRENT_LEVEL)};
	const auto award{_wiz_1_award ? " \">\"" : ""};

	return std::format("{} L {:>2} {}-{} {}{}{}", name, level,
					   alignment_to_str(_alignment).substr(0, 1),
					   class_to_str(_class).substr(0, 3), race_to_str(_race),
					   legacy, award);
}

auto Sorcery::Character::summary_text() const -> std::string {

	auto name{_name};
	auto legacy{_legated ? " (D)" : ""};
	const auto level{_abilities.at(Enums::Character::Ability::CURRENT_LEVEL)};
	switch (_current_stage) {
		using enum Enums::Character::Stage;

	case ENTER_NAME:
		return std::format("{:<15} L ?? ?-??? ???", "???");
		break;
	case CHOOSE_RACE:
		return std::format("{:<15} L {:>2} ?-??? ???", name, level);
		break;
	case CHOOSE_ALIGNMENT:
		return std::format("{:<15} L {:>2} ?-??? {}", name, level,
						   race_to_str(_race));
		break;

	case CHOOSE_CLASS:
		return std::format("{:<15} L {:>2} {}-??? {}", name, level,
						   alignment_to_str(_alignment).substr(0, 1),
						   race_to_str(_race));
		break;
	case REVIEW_AND_CONFIRM:
		[[fallthrough]];
	case COMPLETED:
		return std::format("{} L {:>2} {}-{} {}{}", name, level,
						   alignment_to_str(_alignment).substr(0, 1),
						   class_to_str(_class).substr(0, 3),
						   race_to_str(_race), legacy);
		break;
	default:
		return "";
		break;
	}
}

auto Sorcery::Character::damage(const unsigned int adjustment) -> bool {

	return _damage(adjustment);
}

auto Sorcery::Character::heal(const unsigned int adjustment) -> void {

	_heal(adjustment);
}

auto Sorcery::Character::_damage(const unsigned int adjustment) -> bool {

	using enum Enums::Character::Ability;
	_abilities[CURRENT_HP] = _abilities[CURRENT_HP] - adjustment;
	if (_abilities[CURRENT_HP] < 0) {
		_abilities[CURRENT_HP] = 0;
		_status = Enums::Character::Status::DEAD;
		return false;
	} else
		return true;
}

auto Sorcery::Character::_heal(const unsigned int adjustment) -> void {

	using enum Enums::Character::Ability;
	_abilities[CURRENT_HP] = _abilities[CURRENT_HP] + adjustment;

	if (_abilities[CURRENT_HP] > _abilities[MAX_HP])
		_abilities[CURRENT_HP] = _abilities[MAX_HP];
}

auto Sorcery::Character::get_cur_xp() const -> int {

	return _abilities.at(Enums::Character::Ability::CURRENT_XP);
}

auto Sorcery::Character::get_next_xp() const -> int {

	return _abilities.at(Enums::Character::Ability::NEXT_LEVEL_XP);
}

auto Sorcery::Character::get_cur_ac() const -> int {

	auto ac{_abilities.at(Enums::Character::Ability::BASE_ARMOUR_CLASS)};

	for (const auto &item : inventory.items()) {
		if (item.get_equipped() && item.get_cursed())
			ac = ac +
				 _ctx->resources->items->get(item.get_type_id()).get_ac_mod();
		else if (item.get_equipped())
			ac = ac -
				 _ctx->resources->items->get(item.get_type_id()).get_ac_mod();
	}

	return ac;
}

auto Sorcery::Character::get_cur_to_hit() const -> int {

	auto to_hit{_abilities.at(Enums::Character::Ability::HIT_PROBABILITY)};

	for (const auto &item : inventory.items())
		to_hit =
			to_hit +
			_ctx->resources->items->get(item.get_type_id()).get_to_hit_mod();

	return to_hit;
}

auto Sorcery::Character::get_cur_num_attacks() const -> int {

	auto base_attacks{
		_abilities.at(Enums::Character::Ability::BASE_NUMBER_OF_ATTACKS)};
	auto extra_attacks{0};

	for (const auto &item : inventory.items())
		extra_attacks =
			extra_attacks +
			_ctx->resources->items->get(item.get_type_id()).get_swings();

	return extra_attacks > base_attacks ? extra_attacks : base_attacks;
}

namespace Sorcery {

auto operator<<(std::ostream &out_stream, const Sorcery::Character &character)
	-> std::ostream & {

	auto name{character.get_name()};
	auto cclass{character.get_class()};
	auto alignment{character.get_alignment()};
	auto hp{character.get_hp_summary()};

	auto body{std::format(
		"{:<15} {:>2} {}-{} {:>3} {:>6} {:^10}", name, character.get_level(),
		character.alignment_to_str(alignment).substr(0, 1),
		character.class_to_str(cclass).substr(0, 3), character.get_cur_ac(),
		character.get_hp_summary(), character.get_condition())};

	return out_stream << body << std::endl;
}
} // namespace Sorcery
