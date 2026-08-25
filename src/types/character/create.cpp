// src/types/character/magic.cpp
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

#include "types/character/create.hpp"
#include "core/context.hpp"
#include "core/random.hpp"
#include "core/resources.hpp"
#include "resources/spellstore.hpp"
#include "types/character.hpp"

#include <map>
#include <optional>
#include <ostream>
#include <string>
#include <string_view>
#include <vector>

Sorcery::ConstCharacterCreate::ConstCharacterCreate(
	const Character &character) noexcept
	: _character{&character} {}

auto Sorcery::ConstCharacterCreate::get_stage() const
	-> Enums::Character::Stage {

	return _character->_current_stage;
}

auto Sorcery::ConstCharacterCreate::get_points_left() const -> unsigned int {

	return _character->_points_left;
}

auto Sorcery::ConstCharacterCreate::get_start_points() const -> unsigned int {

	return _character->_st_points;
}

auto Sorcery::ConstCharacterCreate::get_start_attribute(
	const Enums::Character::Attribute attribute) const -> unsigned int {

	return _character->_start_attr.at(attribute);
}

auto Sorcery::ConstCharacterCreate::get_start_attributes() const
	-> std::map<Enums::Character::Attribute, int> {

	return _character->_start_attr;
}

////////////////////////////////////////////////////////////////////////////////

Sorcery::CharacterCreate::CharacterCreate(Character &character) noexcept
	: ConstCharacterCreate{character},
	  _m_character{&character} {}

// Reset a character back to a particular state
auto Sorcery::CharacterCreate::set_stage(const Enums::Character::Stage stage)
	-> void {

	_m_character->_current_stage = stage;
	switch (stage) {
		using enum Enums::Character::Stage;
	case NOT_STARTED:
		_m_character->_name.clear();
		_m_character->_race = Enums::Character::Race::NO_RACE;
		_m_character->_alignment = Enums::Character::Align::NO_ALIGN;
		_m_character->_start_attr.clear();
		_m_character->_cur_attr.clear();
		_m_character->_max_attr.clear();
		_m_character->_abilities.clear();

		// Used in the display from this point onwards
		_m_character->_abilities[Enums::Character::Ability::CURRENT_LEVEL] = 1;
		_m_character->_class = Enums::Character::Class::NO_CLASS;
		_m_character->_points_left = 0;
		_m_character->_st_points = 0;
		_m_character->_pos_classes.clear();
		_m_character->_num_pos_classes = 0;
		_m_character->_priest_max_sp.clear();
		_m_character->_priest_cur_sp.clear();
		_m_character->_mage_max_sp.clear();
		_m_character->_mage_cur_sp.clear();
		_m_character->_spells.clear();
		_m_character->magic().create_spells();
		_m_character->magic().reset_spells();
		break;
	case REVIEW_AND_CONFIRM:

		// Handle the generation of the Character Display Here
		//_view = SUMMARY;
		//_generate_display();
		break;
	default:
		break;
	}
}

// Last step of creating new a character
auto Sorcery::CharacterCreate::finalise() -> void {

	_m_character->_generate_start_info();
	_m_character->_generate_secondary_abil(true, false, false);
	_m_character->_set_start_spells();
	_m_character->_set_starting_sp();
}

auto Sorcery::CharacterCreate::set_points_left(const unsigned int &value)
	-> void {

	_m_character->_points_left = value;
}

auto Sorcery::CharacterCreate::reset(const Enums::Character::Stage stage)
	-> void {

	set_stage(stage);
}

auto Sorcery::CharacterCreate::set_start_attr() -> void {

	_m_character->_start_attr.clear();
	_m_character->_cur_attr.clear();
	switch (_m_character->_race) {
		using enum Enums::Character::Attribute;
		using enum Enums::Character::Race;
	case HUMAN:
		_m_character->_start_attr = {{STRENGTH, 8}, {IQ, 5},	  {PIETY, 5},
									 {VITALITY, 8}, {AGILITY, 8}, {LUCK, 9}};
		break;
	case ELF:
		_m_character->_start_attr = {{STRENGTH, 7}, {IQ, 10},	  {PIETY, 10},
									 {VITALITY, 6}, {AGILITY, 9}, {LUCK, 6}};
		break;
	case DWARF:
		_m_character->_start_attr = {{STRENGTH, 10}, {IQ, 7},	   {PIETY, 10},
									 {VITALITY, 10}, {AGILITY, 5}, {LUCK, 6}};
		break;
	case GNOME:
		_m_character->_start_attr = {{STRENGTH, 7}, {IQ, 7},	   {PIETY, 10},
									 {VITALITY, 8}, {AGILITY, 10}, {LUCK, 7}};
		break;
	case HOBBIT:
		_m_character->_start_attr = {{STRENGTH, 5}, {IQ, 7},	   {PIETY, 7},
									 {VITALITY, 6}, {AGILITY, 10}, {LUCK, 15}};
		break;
	default:
		break;
	}

	_m_character->_cur_attr = _m_character->_start_attr;

	// Formula sourced from http://www.zimlab.com/wizardry/walk/w123calc.htm
	using enum Enums::System::Random;
	_m_character->_points_left = _m_character->_ctx->get_random(ZERO_TO_3);
	const bool chance_of_more{_m_character->_ctx->get_random(D10) == 1};
	const bool chance_of_more_again{_m_character->_ctx->get_random(D10) == 1};
	_m_character->_points_left += 7;
	if (_m_character->_points_left < 20)
		if (chance_of_more)
			_m_character->_points_left += 10;
	if (_m_character->_points_left < 20)
		if (chance_of_more_again)
			_m_character->_points_left += 10;
	_m_character->_st_points = _m_character->_points_left;
}

// Given an Alignment and a Class, create a character
auto Sorcery::CharacterCreate::create_class_alignment(
	const Enums::Character::Class cclass,
	const Enums::Character::Align alignment) -> void {

	_m_character->_class = cclass;
	_m_character->_race = static_cast<Enums::Character::Race>(
		_m_character->_ctx->get_random(Enums::System::Random::D5));
	_m_character->_alignment = alignment;

	switch (_m_character->_race) { // NOLINT(clang-diagnostic-switch)
		using enum Enums::Character::Attribute;
		using enum Enums::Character::Race;
	case HUMAN:
		_m_character->_start_attr = {{STRENGTH, 8}, {IQ, 5},	  {PIETY, 5},
									 {VITALITY, 8}, {AGILITY, 8}, {LUCK, 9}};
		break;
	case ELF:
		_m_character->_start_attr = {{STRENGTH, 7}, {IQ, 10},	  {PIETY, 10},
									 {VITALITY, 6}, {AGILITY, 9}, {LUCK, 6}};
		break;
	case DWARF:
		_m_character->_start_attr = {{STRENGTH, 10}, {IQ, 7},	   {PIETY, 10},
									 {VITALITY, 10}, {AGILITY, 5}, {LUCK, 6}};
		break;
	case GNOME:
		_m_character->_start_attr = {{STRENGTH, 7}, {IQ, 7},	   {PIETY, 10},
									 {VITALITY, 8}, {AGILITY, 10}, {LUCK, 7}};
		break;
	case HOBBIT:
		_m_character->_start_attr = {{STRENGTH, 5}, {IQ, 7},	   {PIETY, 7},
									 {VITALITY, 6}, {AGILITY, 10}, {LUCK, 15}};
		break;
	default:
		break;
	}

	// Put most of the points into the main attribute (note that 10 points means
	// a Human Priest and Dwarf Thief have allocated all points to their main
	// attribute with no points left over)
	_m_character->_points_left = 10;
	_m_character->_st_points = _m_character->_points_left;

	switch (_m_character->_class) { // NOLINT(clang-diagnostic-switch)
		using enum Enums::Character::Attribute;
		using enum Enums::Character::Class;
	case FIGHTER:
		[[fallthrough]];
	case LORD:
		[[fallthrough]];
	case SAMURAI:
		_m_character->_points_left -=
			(15 - _m_character->_start_attr[STRENGTH]);
		_m_character->_start_attr[STRENGTH] = 15;
		break;
	case MAGE:
		[[fallthrough]];
	case BISHOP:
		_m_character->_points_left -= (15 - _m_character->_start_attr[IQ]);
		_m_character->_start_attr[IQ] = 15;
		break;
	case PRIEST:
		_m_character->_points_left -= (15 - _m_character->_start_attr[PIETY]);
		_m_character->_start_attr[PIETY] = 15;
		break;
	case THIEF:
		[[fallthrough]];
	case NINJA:
		_m_character->_points_left -= (15 - _m_character->_start_attr[AGILITY]);
		_m_character->_start_attr[AGILITY] = 15;
		break;
	default:
		break;
	}

	// Pump any points left into the Vitality attribute
	if (_m_character->_points_left > 0)
		_m_character->_start_attr[Enums::Character::Attribute::VITALITY] +=
			_m_character->_points_left;

	_m_character->_cur_attr = _m_character->_start_attr;

	_m_character->_name = _m_character->_ctx->random->get_random_name();
}

// Enter Name, rest is random
auto Sorcery::CharacterCreate::create_quick() -> void {

	// Exclude Samurai/Lord/Ninja/Bishop from this method of character creation
	using enum Enums::System::Random;
	_m_character->_class = static_cast<Enums::Character::Class>(
		_m_character->_ctx->get_random(D4));
	_m_character->_race =
		static_cast<Enums::Character::Race>(_m_character->_ctx->get_random(D5));
	switch (_m_character->_class) { // NOLINT(clang-diagnostic-switch)#
		using enum Enums::Character::Align;
		using enum Enums::Character::Class;
	case FIGHTER:
	case MAGE:
		_m_character->_alignment = static_cast<Enums::Character::Align>(
			_m_character->_ctx->get_random(D3));
		break;
	case PRIEST:
		_m_character->_alignment =
			_m_character->_ctx->get_random(D2) == 1 ? GOOD : EVIL;
		break;
	case THIEF:
		_m_character->_alignment =
			_m_character->_ctx->get_random(D2) == 1 ? NEUTRAL : EVIL;
		break;
	default:
		break;
	}

	// Now get minimum attributes for race/class combo (note as we are only
	// allowing creation of some classes, it will be as if we had a maximum of
	// 10 bonus points to spend - in order to incentivise full blown character
	// creation! see table IV (A) at
	// https://gamefaqs.gamespot.com/pc/946844-the-ultimate-wizardry-archives/faqs/45726
	// for info
	switch (_m_character->_race) { // NOLINT(clang-diagnostic-switch)
		using enum Enums::Character::Attribute;
		using enum Enums::Character::Race;
	case HUMAN:
		_m_character->_start_attr = {{STRENGTH, 8}, {IQ, 5},	  {PIETY, 5},
									 {VITALITY, 8}, {AGILITY, 8}, {LUCK, 9}};
		break;
	case ELF:
		_m_character->_start_attr = {{STRENGTH, 7}, {IQ, 10},	  {PIETY, 10},
									 {VITALITY, 6}, {AGILITY, 9}, {LUCK, 6}};
		break;
	case DWARF:
		_m_character->_start_attr = {{STRENGTH, 10}, {IQ, 7},	   {PIETY, 10},
									 {VITALITY, 10}, {AGILITY, 5}, {LUCK, 6}};
		break;
	case GNOME:
		_m_character->_start_attr = {{STRENGTH, 7}, {IQ, 7},	   {PIETY, 10},
									 {VITALITY, 8}, {AGILITY, 10}, {LUCK, 7}};
		break;
	case HOBBIT:
		_m_character->_start_attr = {{STRENGTH, 5}, {IQ, 7},	   {PIETY, 7},
									 {VITALITY, 6}, {AGILITY, 10}, {LUCK, 15}};
		break;
	default:
		break;
	}

	// Put most of the points into the main attribute (note that 10 points means
	// a Human Priest and Dwarf Thief have allocated all points to their main
	// attribute with no points left over)
	_m_character->_points_left = 10;
	_m_character->_st_points = _m_character->_points_left;
	switch (_m_character->_class) { // NOLINT(clang-diagnostic-switch)
		using enum Enums::Character::Attribute;
		using enum Enums::Character::Class;
	case FIGHTER:
		_m_character->_points_left -=
			(15 - _m_character->_start_attr[STRENGTH]);
		_m_character->_start_attr[STRENGTH] = 15;
		break;
	case MAGE:
		_m_character->_points_left -= (15 - _m_character->_start_attr[IQ]);
		_m_character->_start_attr[IQ] = 15;
		break;
	case PRIEST:
		_m_character->_points_left -= (15 - _m_character->_start_attr[PIETY]);
		_m_character->_start_attr[PIETY] = 15;
		break;
	case THIEF:
		_m_character->_points_left -= (15 - _m_character->_start_attr[AGILITY]);
		_m_character->_start_attr[AGILITY] = 15;
		break;
	default:
		break;
	}

	// Pump any points left into the Vitality attribute
	if (_m_character->_points_left > 0)
		_m_character->_start_attr[Enums::Character::Attribute::VITALITY] +=
			_m_character->_points_left;

	_m_character->_cur_attr = _m_character->_start_attr;
}

// Create a (semi) random character
auto Sorcery::CharacterCreate::create_random() -> void {

	// Random Name
	create_quick();
	_m_character->_name = _m_character->_ctx->random->get_random_name();
}

// Given a character's current stats and alignment, work out what classes are
// available
auto Sorcery::CharacterCreate::set_possible_classes() -> void {

	_m_character->_pos_classes.clear();

	// Do the basic classes first (this also sets
	// _num_possible_character_classes); data is from
	// https://strategywiki.org/wiki/Wizardry:_Proving_Grounds_of_the_Mad_Overlord/Trebor%27s_castle#Classes
	using enum Enums::Character::Align;
	using enum Enums::Character::Class;
	using enum Enums::Character::Attribute;
	if (_m_character->_cur_attr[STRENGTH] >= 11)
		_m_character->_pos_classes[FIGHTER] = true;
	else
		_m_character->_pos_classes[FIGHTER] = false;

	if (_m_character->_cur_attr[IQ] >= 11)
		_m_character->_pos_classes[MAGE] = true;
	else
		_m_character->_pos_classes[MAGE] = false;

	if (_m_character->_cur_attr[PIETY] >= 11)
		if (_m_character->_alignment == GOOD ||
			_m_character->_alignment == EVIL)
			_m_character->_pos_classes[PRIEST] = true;
		else
			_m_character->_pos_classes[PRIEST] = false;
	else
		_m_character->_pos_classes[PRIEST] = false;

	if (_m_character->_cur_attr[AGILITY] >= 11)
		if (_m_character->_alignment == NEUTRAL ||
			_m_character->_alignment == EVIL)
			_m_character->_pos_classes[THIEF] = true;
		else
			_m_character->_pos_classes[THIEF] = false;
	else
		_m_character->_pos_classes[THIEF] = false;

	// Now the elite classes
	if (_m_character->_cur_attr[IQ] >= 12 &&
		_m_character->_cur_attr[PIETY] >= 12)
		if (_m_character->_alignment == GOOD ||
			_m_character->_alignment == EVIL)
			_m_character->_pos_classes[BISHOP] = true;
		else
			_m_character->_pos_classes[BISHOP] = false;
	else
		_m_character->_pos_classes[BISHOP] = false;

	if (_m_character->_cur_attr[STRENGTH] >= 15 &&
		_m_character->_cur_attr[IQ] >= 11 &&
		_m_character->_cur_attr[PIETY] >= 10 &&
		_m_character->_cur_attr[VITALITY] >= 10 &&
		_m_character->_cur_attr[AGILITY] >= 10)
		if (_m_character->_alignment == GOOD ||
			_m_character->_alignment == NEUTRAL)
			_m_character->_pos_classes[SAMURAI] = true;
		else
			_m_character->_pos_classes[SAMURAI] = false;
	else
		_m_character->_pos_classes[SAMURAI] = false;

	if (_m_character->_cur_attr[STRENGTH] >= 15 &&
		_m_character->_cur_attr[IQ] >= 12 &&
		_m_character->_cur_attr[PIETY] >= 12 &&
		_m_character->_cur_attr[VITALITY] >= 15 &&
		_m_character->_cur_attr[AGILITY] >= 14 &&
		_m_character->_cur_attr[LUCK] >= 15)
		if (_m_character->_alignment == GOOD)
			_m_character->_pos_classes[LORD] = true;
		else
			_m_character->_pos_classes[LORD] = false;
	else
		_m_character->_pos_classes[LORD] = false;

	// Using looser Wizardry 5 requirements for Ninja (see
	// https://wizardry.fandom.com/wiki/Ninja)
	if (_m_character->_cur_attr[STRENGTH] >= 15 &&
		_m_character->_cur_attr[IQ] >= 17 &&
		_m_character->_cur_attr[PIETY] >= 15 &&
		_m_character->_cur_attr[VITALITY] >= 16 &&
		_m_character->_cur_attr[AGILITY] >= 15 &&
		_m_character->_cur_attr[LUCK] >= 16)
		if (_m_character->_alignment != GOOD)
			_m_character->_pos_classes[NINJA] = true;
		else
			_m_character->_pos_classes[NINJA] = false;
	else
		_m_character->_pos_classes[NINJA] = false;

	// And workout the number of classes
	_m_character->_num_pos_classes =
		std::count_if(_m_character->_pos_classes.begin(),
					  _m_character->_pos_classes.end(), [](auto element) {
						  return element.second;
					  });
}