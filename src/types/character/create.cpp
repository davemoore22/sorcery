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
#include "types/templates.hpp"

#include <map>
#include <optional>
#include <ostream>
#include <string>
#include <string_view>
#include <vector>

namespace {

struct BaseAttributes {
		int strength;
		int iq;
		int piety;
		int vitality;
		int agility;
		int luck;
};

static constexpr std::array<BaseAttributes, 6> racial_attributes{{
	{},
	{8, 5, 5, 8, 8, 9},	   // HUMAN
	{7, 10, 10, 6, 9, 6},  // ELF
	{10, 7, 10, 10, 5, 6}, // DWARF
	{7, 7, 10, 8, 10, 7},  // GNOME
	{5, 7, 7, 6, 10, 15},  // HOBBIT
}};

}

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

auto Sorcery::ConstCharacterCreate::can_change_class() const -> bool {

	return std::ranges::any_of(
		_character->_pos_classes, [this](const auto &entry) {
			return entry.first != _character->_class && entry.second;
		});
}

auto Sorcery::ConstCharacterCreate::get_possible_classes() const
	-> std::map<Enums::Character::Class, bool> {

	return _character->_pos_classes;
}

auto Sorcery::ConstCharacterCreate::get_possible_classes_display()
	-> std::string {

	std::string classes{"--------"};

	using enum Enums::Character::Class;

	const auto possible = [&](const auto cls) {
		return cls != _character->_class && _character->_pos_classes.at(cls);
	};

	if (possible(FIGHTER))
		classes[0] = 'F';
	if (possible(MAGE))
		classes[1] = 'M';
	if (possible(PRIEST))
		classes[2] = 'P';
	if (possible(THIEF))
		classes[3] = 'T';
	if (possible(BISHOP))
		classes[4] = 'B';
	if (possible(SAMURAI))
		classes[5] = 'S';
	if (possible(LORD))
		classes[6] = 'L';
	if (possible(NINJA))
		classes[7] = 'N';

	return classes;
}

// In the original code this is from SPLPERLV
auto Sorcery::ConstCharacterCreate::_get_spells_known(
	Enums::Magic::SpellType spell_type, unsigned int spell_level)
	-> unsigned int {

	return std::ranges::count_if(_character->_spells.begin(),
								 _character->_spells.end(), [&](auto spell) {
									 return spell.type == spell_type &&
											spell.level == spell_level &&
											spell.known;
								 });
}

// Given a level, get the XP needed for it
auto Sorcery::ConstCharacterCreate::_get_xp_for_level(unsigned int level) const
	-> int {

	// XP values are obtained directly from original Apple2 Scenario Disc (look
	// for E8 03 00 00 00 00 near &00020106 though due to what I think is the
	// way USCD pascal stores large numbers, they are stored in 16 bit LSB
	// "chunks", for example, 134586 is stored at &0002013C as EA 11 0D, or 4586
	// - to get the actual value for the level we add this to 0D in decimal (13)
	// times 10000, to get 134586.
	static constexpr Grid<int, 8, 14> levels{std::array<std::array<int, 14>, 8>{
		{{0, 1000, 1724, 2972, 5124, 8834, 15231, 26260, 45275, 78060, 134586,
		  232044, 400075, 289709},

		 {0, 1100, 1896, 3268, 5124, 9713, 16746, 28872, 49779, 85825, 147974,
		  255127, 439874, 318529},

		 {0, 1050, 1810, 3120, 5379, 9274, 15989, 27567, 47529, 81946, 141286,
		  243596, 419993, 304132},

		 {0, 900, 1551, 2574, 4610, 7948, 13703, 23625, 40732, 70187, 121081,
		  208750, 359931, 260639},

		 {0, 1000, 2105, 3692, 6477, 11363, 19935, 34973, 61136, 107642, 188845,
		  331370, 481240, 438479},

		 {0, 1250, 2192, 3845, 6745, 11833, 20759, 36419, 63892, 112091, 196650,
		  345000, 605263, 456601},

		 {0, 1300, 2280, 4000, 7017, 12310, 21596, 37887, 66468, 116610, 204578,
		  358908, 629663, 475008},

		 {0, 1450, 2543, 4461, 7826, 13729, 24085, 42254, 74129, 130050, 228157,
		  400275, 702236, 529756}}}};

	// Also found here:
	// http://www.the-spoiler.com/RPG/Sir-Tech/wizardry.1.2.html

	auto xp_needed{0};
	auto c_index{std::to_underlying(_character->_class) - 1};
	if (level <= 13)
		xp_needed = levels[c_index, level];
	else
		xp_needed = levels[c_index, 12] + ((level - 13) * levels[c_index, 13]);

	return xp_needed;
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

	_generate_start_info();
	_generate_secondary_abil(true, false, false);
	_set_start_spells();
	_set_starting_sp();
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
	_set_racial_attributes();
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

	_set_racial_attributes();

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
	_set_racial_attributes();

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

// Level a character up
auto Sorcery::CharacterCreate::level_up() -> void {

	_m_character->level_up_results.clear();
	_m_character->level_up_results.emplace_back(
		_m_character->_ctx->get_string("LEVEL_DING"));

	// Increase level
	using enum Enums::Character::Ability;
	_m_character->_abilities.at(CURRENT_LEVEL) =
		_m_character->_abilities.at(CURRENT_LEVEL) + 1;
	_m_character->_abilities.at(HIT_DICE) =
		_m_character->_abilities.at(HIT_DICE) + 1;
	if (_m_character->_abilities.at(CURRENT_LEVEL) >
		_m_character->_abilities.at(MAX_LEVEL))
		_m_character->_abilities.at(MAX_LEVEL) =
			_m_character->_abilities.at(CURRENT_LEVEL);

	// Handle learning spells
	if (_set_sp())
		_m_character->level_up_results.emplace_back(
			_m_character->_ctx->get_string("LEVEL_SPELLS"));

	// Work out new xp needed
	_m_character->_abilities[NEXT_LEVEL_XP] =
		_get_xp_for_level(_m_character->_abilities[CURRENT_LEVEL]);

	// Handle stat changing
	auto stat_message{""s};
	using enum Enums::Character::Attribute;
	stat_message = _update_stat_for_level(
		STRENGTH, _m_character->_ctx->get_string("CHARACTER_STAT_STRENGTH"));
	if (!stat_message.empty())
		_m_character->level_up_results.emplace_back(stat_message);
	stat_message = _update_stat_for_level(
		IQ, _m_character->_ctx->get_string("CHARACTER_STAT_INTELLIGENCE"));
	if (!stat_message.empty())
		_m_character->level_up_results.emplace_back(stat_message);
	stat_message = _update_stat_for_level(
		PIETY, _m_character->_ctx->get_string("CHARACTER_STAT_PIETY"));
	if (!stat_message.empty())
		_m_character->level_up_results.emplace_back(stat_message);
	stat_message = _update_stat_for_level(
		VITALITY, _m_character->_ctx->get_string("CHARACTER_STAT_VITALITY"));
	if (!stat_message.empty())
		_m_character->level_up_results.emplace_back(stat_message);
	stat_message = _update_stat_for_level(
		AGILITY, _m_character->_ctx->get_string("CHARACTER_STAT_AGILITY"));
	if (!stat_message.empty())
		_m_character->level_up_results.emplace_back(stat_message);
	stat_message = _update_stat_for_level(
		LUCK, _m_character->_ctx->get_string("CHARACTER_STAT_LUCK"));
	if (!stat_message.empty())
		_m_character->level_up_results.emplace_back(stat_message);

	// Level everything else up
	_generate_secondary_abil(false, false, false);

	// handle hp
	const auto hp_gained{_update_hp_for_level()};
	const auto hp_message{std::format(
		"{} {} {}", _m_character->_ctx->get_string("LEVEL_HP_PREFIX"),
		hp_gained, _m_character->_ctx->get_string("LEVEL_HP_SUFFIX"))};
	_m_character->level_up_results.emplace_back(hp_message);

	if (_m_character->_cur_attr.at(VITALITY) < 3) {
		_m_character->level_up_results.emplace_back(
			_m_character->_ctx->get_string("LEVEL_DIE"));
		_m_character->_status = Enums::Character::Status::LOST;
		_m_character->_location = Enums::Character::Location::TRAINING;
	}
}

// Level a character down (e.g. drain levels or give/increase negative levels_
auto Sorcery::CharacterCreate::level_down() -> void {

	using enum Enums::Character::Ability;
	if (_m_character->_abilities.at(CURRENT_LEVEL) == 1) {
		_m_character->_status = Enums::Character::Status::LOST;
		_m_character->_location = Enums::Character::Location::TRAINING;
		return;
	}

	// TODO: What to do about negative level ability?
	const auto old_level{_m_character->_abilities.at(CURRENT_LEVEL)};
	const auto diff_hp{_m_character->_abilities.at(MAX_HP) -
					   _m_character->_abilities.at(CURRENT_HP)};

	_m_character->_abilities.at(CURRENT_LEVEL) =
		_m_character->_abilities.at(CURRENT_LEVEL) - 1;
	_m_character->_abilities.at(HIT_DICE) =
		_m_character->_abilities.at(HIT_DICE) - 1;
	if (_m_character->_abilities.at(MAX_LEVEL) >
		_m_character->_abilities.at(CURRENT_LEVEL))
		_m_character->_abilities.at(MAX_LEVEL) =
			_m_character->_abilities.at(CURRENT_LEVEL);

	_set_sp();

	// When drained XP is set to beginning of current level (for example,
	// draining to level 9 means that your xp is set to enough for level 9 plus
	// 1 - which is vety harsh)
	_m_character->_abilities[NEXT_LEVEL_XP] =
		_get_xp_for_level(_m_character->_abilities.at(CURRENT_LEVEL));
	_m_character->_abilities[CURRENT_XP] =
		_get_xp_for_level(_m_character->_abilities.at(CURRENT_LEVEL) - 1) + 1;

	_generate_secondary_abil(false, false, false);
	_m_character->_abilities[MAX_HP] =
		_m_character->_abilities.at(MAX_HP) *
		(_m_character->_abilities.at(CURRENT_LEVEL) / (old_level * 1.f));
	_m_character->_abilities[CURRENT_HP] =
		_m_character->_abilities[MAX_HP] - diff_hp;
	if (_m_character->_abilities[CURRENT_HP] < 0)
		_m_character->_abilities[CURRENT_HP] = 0;
}

auto Sorcery::CharacterCreate::set_name(std::string_view value) -> void {

	_m_character->_name = value;
}

auto Sorcery::CharacterCreate::set_race(const Enums::Character::Race &value)
	-> void {

	_m_character->_race = value;
}

auto Sorcery::CharacterCreate::set_alignment(
	const Enums::Character::Align &value) -> void {

	_m_character->_alignment = value;
}

auto Sorcery::CharacterCreate::set_class(const Enums::Character::Class &value)
	-> void {

	_m_character->_class = value;
}

// Legate
auto Sorcery::CharacterCreate::legate(const Enums::Character::Align &value)
	-> void {

	if (_m_character->_alignment != value)
		_m_character->_alignment = value;

	_legate_start_info();
	_generate_secondary_abil(true, false, true);
	_set_start_spells();
	_set_starting_sp();
	_m_character->inventory.clear();
	_m_character->set_status(Enums::Character::Status::OK);
	_m_character->_legated = true;

	_m_character->set_location(Enums::Character::Location::TAVERN);
}

// Change Class
auto Sorcery::CharacterCreate::change_class(
	const Enums::Character::Class &value) -> void {

	if (_m_character->_class != value) {
		_m_character->_class = value;

		_regenerate_start_info();
		_generate_secondary_abil(false, true, false);
		_reset_start_spells(); // TODO: check this, not sure this works
		_reset_starting_sp();
		_m_character->inventory.unequip_all();
	}
}

// Work out all the stuff to do with starting a new character
auto Sorcery::CharacterCreate::_generate_start_info() -> void {

	// _abilities[CURRENT_LEVEL] = 1; // why is this commented out?
	using enum Enums::Character::Ability;
	using enum Enums::System::Random;
	_m_character->_abilities[MAX_LEVEL] = 1;
	_m_character->_abilities[NEGATIVE_LEVEL] = 0;
	_m_character->_abilities[HIT_DICE] = 1;
	_m_character->_abilities[GOLD] =
		_m_character->_ctx->get_random(ZERO_TO_99) + 90;
	_m_character->_abilities[AGE] =
		(18 * 52) + _m_character->_ctx->get_random(ZERO_TO_299);
	_m_character->_abilities[SWIM] = 1;
	_m_character->_abilities[MARKS] = 0;
	_m_character->_abilities[DEATHS] = 0;

	_m_character->_start_attr = _m_character->_cur_attr;
	_m_character->_max_attr = _m_character->_cur_attr;

	_m_character->_abilities[CURRENT_XP] = 0;
	_m_character->_abilities[NEXT_LEVEL_XP] =
		_get_xp_for_level(_m_character->_abilities[CURRENT_LEVEL]);
}

auto Sorcery::CharacterCreate::_generate_secondary_abil(bool initial,
														bool change_class,
														bool legate) -> void {

	// Formulae used are from here
	// http://www.zimlab.com/wizardry/walk/w123calc.htm and also from
	// https://mirrors.apple2.org.za/ftp.apple.asimov.net/images/games/rpg/wizardry/wizardry_I/Wizardry_i_SourceCode.zip
	using enum Enums::Character::Ability;
	const auto current_level{_m_character->_abilities[CURRENT_LEVEL]};

	// Bonus Melee to Hit per Attack (num)
	using enum Enums::Character::Attribute;
	if (_m_character->_cur_attr[STRENGTH] > 15)
		_m_character->_abilities[ATTACK_MODIFIER] =
			_m_character->_cur_attr[STRENGTH] - 15;
	else if (_m_character->_cur_attr[STRENGTH] < 6)
		_m_character->_abilities[ATTACK_MODIFIER] =
			_m_character->_cur_attr[STRENGTH] - 6;
	else
		_m_character->_abilities[ATTACK_MODIFIER] = 0;

	// Bonus Melee to Hit per Attack (num)
	using enum Enums::Character::Class;
	switch (_m_character->_class) {
	case FIGHTER:
	case SAMURAI:
	case LORD:
	case NINJA:
	case PRIEST:
		_m_character->_abilities[HIT_PROBABILITY] = 2 + (current_level / 5);
		break;
	default:
		_m_character->_abilities[HIT_PROBABILITY] = current_level / 5;
		break;
	}

	// Bonus Melee Damage per Attack (num)
	if (_m_character->_cur_attr[STRENGTH] > 15)
		_m_character->_abilities[BONUS_DAMAGE] =
			_m_character->_cur_attr[STRENGTH] - 15;
	else if (_m_character->_cur_attr[STRENGTH] < 6)
		_m_character->_abilities[BONUS_DAMAGE] =
			_m_character->_cur_attr[STRENGTH] - 6;
	else
		_m_character->_abilities[BONUS_DAMAGE] = 0;

	// Unarmed Attack Damage (num)
	_m_character->_abilities[UNARMED_DAMAGE] =
		_m_character->_class == NINJA
			? 8 + _m_character->_abilities[BONUS_DAMAGE]
			: 4 + _m_character->_abilities[BONUS_DAMAGE];

	// Number of Melee Attacks (num)
	switch (_m_character->_class) {
	case FIGHTER:
	case SAMURAI:
	case LORD:
		_m_character->_abilities[BASE_NUMBER_OF_ATTACKS] = current_level / 5;
		break;
	case NINJA:
		_m_character->_abilities[BASE_NUMBER_OF_ATTACKS] =
			(current_level / 5) + 1;
		break;
	default:
		_m_character->_abilities[BASE_NUMBER_OF_ATTACKS] = 1;
		break;
	}
	if (_m_character->_abilities[BASE_NUMBER_OF_ATTACKS] == 0)
		_m_character->_abilities[BASE_NUMBER_OF_ATTACKS] = 1;

	// Chance of learning new Mage Spells (%)
	switch (_m_character->_class) {
	case SAMURAI:
	case BISHOP:
	case MAGE:
		_m_character->_abilities[MAGE_SPELL_LEARN] =
			(_m_character->_cur_attr[IQ] / 29.0) * 100;
		break;
	default:
		_m_character->_abilities[MAGE_SPELL_LEARN] = 0;
		break;
	}

	// Chance of Identifying Items (%)
	_m_character->_abilities[IDENTIFY_ITEMS] =
		_m_character->_class == BISHOP ? 10 + (5 * current_level) : 0;
	if (_m_character->_abilities[IDENTIFY_ITEMS] > 100)
		_m_character->_abilities[IDENTIFY_ITEMS] = 100;

	// Chance of getting cursed when Identifying Items (%)
	_m_character->_abilities[IDENTIFY_CURSE] =
		_m_character->_class == BISHOP ? 35 - (5 * current_level) : 0;
	if (_m_character->_abilities[IDENTIFY_CURSE] < 0)
		_m_character->_abilities[IDENTIFY_CURSE] = 0;

	// Chance of identifying unknown Foes per round (%)
	_m_character->_abilities[IDENTIFY_FOES] = current_level +
											  _m_character->_cur_attr[IQ] +
											  _m_character->_cur_attr[PIETY];
	if (_m_character->_abilities[IDENTIFY_FOES] > 100)
		_m_character->_abilities[IDENTIFY_FOES] = 100;

	// Chance of learning new Priest Spells (%)
	switch (_m_character->_class) {
	case PRIEST:
	case LORD:
	case BISHOP:
		_m_character->_abilities[PRIEST_SPELL_LEARN] =
			(_m_character->_cur_attr[PIETY] / 30.0) * 100;
		break;
	default:
		_m_character->_abilities[PRIEST_SPELL_LEARN] = 0;
		break;
	}

	// LOKTOFELT success chance (%)
	_m_character->_abilities[LOKTOFELT_SUCCESS] =
		_m_character->_class == PRIEST ? 2 * current_level : 0;

	// Base Dispell chance (affected by monster level) (%)
	switch (_m_character->_class) {
	case PRIEST:
		_m_character->_abilities[BASE_DISPELL] = 50 + (5 * current_level);
		break;
	case BISHOP:
		_m_character->_abilities[BASE_DISPELL] =
			current_level >= 4 ? 50 + (5 * current_level) - 20 : 0;
		break;
	case LORD:
		_m_character->_abilities[BASE_DISPELL] =
			current_level >= 9 ? 50 + (5 * current_level) - 40 : 0;
		break;
	default:
		_m_character->_abilities[BASE_DISPELL] = 0;
		break;
	}
	if (_m_character->_abilities[BASE_DISPELL] > 100)
		_m_character->_abilities[BASE_DISPELL] = 100;

	// Vitality Bonus (num)
	switch (_m_character->_cur_attr[VITALITY]) {
	case 3:
		_m_character->_abilities[VITALITY_BONUS] = -2;
		break;
	case 4:
	case 5:
		_m_character->_abilities[VITALITY_BONUS] = -1;
		break;
	case 16:
		_m_character->_abilities[VITALITY_BONUS] = 1;
		break;
	case 17:
		_m_character->_abilities[VITALITY_BONUS] = 2;
		break;
	case 18:
	case 19:
	case 20:
		_m_character->_abilities[VITALITY_BONUS] = 3;
		break;
	default:
		_m_character->_abilities[VITALITY_BONUS] = 0;
		break;
	}

	// Bonus Hit Points per level (num)
	_m_character->_abilities[BONUS_HIT_POINTS] =
		_m_character->_abilities[VITALITY_BONUS];

	// Class Change doesn't reset these
	if (!change_class) {

		// Base Hit Points (num) - note initially all non-legated characters get
		// 8 HP as per the PSX versions
		if (initial) {
			const auto bonus{_m_character->_abilities[BONUS_HIT_POINTS]};
			using enum Enums::System::Random;
			switch (auto chance{_m_character->_ctx->get_random(D100)};
					_m_character->_class) { // NOLINT(clang-diagnostic-switch)
			case FIGHTER:
			case LORD:
				_m_character->_abilities[MAX_HP] =
					chance <= 50 ? 10 + bonus : 9 * (10 + bonus) / 10;
				break;
			case PRIEST:
				_m_character->_abilities[MAX_HP] =
					chance <= 50 ? 8 + bonus : 8 * (10 + bonus) / 10;
				break;
			case THIEF:
			case BISHOP:
			case NINJA:
				_m_character->_abilities[MAX_HP] =
					chance <= 50 ? 6 + bonus : 6 * (10 + bonus) / 10;
				break;
			case MAGE:
				_m_character->_abilities[MAX_HP] =
					chance <= 50 ? 4 + bonus : 4 * (10 + bonus) / 10;
				break;
			case SAMURAI:
				_m_character->_abilities[MAX_HP] =
					chance <= 50 ? 16 + bonus : 16 * (10 + bonus) / 10;
				break;
			default:
				break;
			}
			if (_m_character->_abilities[MAX_HP] < 1)
				_m_character->_abilities[MAX_HP] = 1;

			_m_character->_abilities[CURRENT_HP] =
				_m_character->_abilities[MAX_HP];
		}

		if (legate) {
			switch (_m_character->_class) {
			case FIGHTER:
				_m_character->_abilities[MAX_HP] = 10;
				break;
			case SAMURAI:
			case LORD:
				_m_character->_abilities[MAX_HP] = 12;
				break;
			case PRIEST:
				_m_character->_abilities[MAX_HP] = 8;
				break;
			case NINJA:
				_m_character->_abilities[MAX_HP] = 7;
				break;
			case THIEF:
				_m_character->_abilities[MAX_HP] = 6;
				break;
			case MAGE:
				_m_character->_abilities[MAX_HP] = 4;
				break;
			default:
				break;
			}

			_m_character->_abilities[CURRENT_HP] =
				_m_character->_abilities[MAX_HP];
		}
	}

	// Chance of resurrecting a Dead Character at the Temple (%)
	_m_character->_abilities[DEAD_RESURRECT] =
		50 + (3 * _m_character->_cur_attr[VITALITY]);
	_m_character->_abilities[DEAD_RESURRECT] =
		std::min(_m_character->_abilities[DEAD_RESURRECT], 100);

	// Chance of resurrecting an Ashed Character at the Temple (%)
	_m_character->_abilities[ASHES_RESURRECT] =
		40 + (3 * _m_character->_cur_attr[VITALITY]);
	_m_character->_abilities[ASHES_RESURRECT] =
		std::min(_m_character->_abilities[ASHES_RESURRECT], 100);

	// Chance of resurrecting by a DI or KADORTO spell cast by another Character
	// (%)
	_m_character->_abilities[DI_KADORTO_RESURRECT] =
		4 * _m_character->_cur_attr[VITALITY];

	// Initiative Modifier (num)
	switch (_m_character->_cur_attr[AGILITY]) {
	case 3:
		_m_character->_abilities[INITIATIVE_MODIFIER] = 3;
		break;
	case 4:
	case 5:
		_m_character->_abilities[INITIATIVE_MODIFIER] = 2;
		break;
	case 6:
	case 7:
		_m_character->_abilities[INITIATIVE_MODIFIER] = 1;
		break;
	case 15:
		_m_character->_abilities[INITIATIVE_MODIFIER] = -1;
		break;
	case 16:
		_m_character->_abilities[INITIATIVE_MODIFIER] = -2;
		break;
	case 17:
		_m_character->_abilities[INITIATIVE_MODIFIER] = -3;
		break;
	case 18:
	case 19:
	case 20:
		_m_character->_abilities[INITIATIVE_MODIFIER] = -4;
		break;
	default:
		_m_character->_abilities[INITIATIVE_MODIFIER] = 0;
		break;
	}

	// Armour Class (num) (before equipment)
	_m_character->_abilities[BASE_ARMOUR_CLASS] =
		_m_character->_class == NINJA ? 10 - (current_level / 3) - 2 : 10;

	// Don't use this!
	_m_character->_abilities[CURRENT_ARMOUR_CLASS] =
		_m_character->_abilities[BASE_ARMOUR_CLASS];

	// Critical Hit Chance (%)
	_m_character->_abilities[BASE_CRITICAL_HIT] =
		_m_character->_class == NINJA ? 2 * current_level : 0;
	if (_m_character->_abilities[BASE_CRITICAL_HIT] > 50)
		_m_character->_abilities[BASE_CRITICAL_HIT] = 50;

	// Chance to identify a Trap (%)
	if (_m_character->_class == THIEF)
		_m_character->_abilities[IDENTIFY_TRAP] =
			6 * _m_character->_cur_attr[AGILITY];
	else if (_m_character->_class == NINJA)
		_m_character->_abilities[IDENTIFY_TRAP] =
			4 * _m_character->_cur_attr[AGILITY];
	else
		_m_character->_abilities[IDENTIFY_TRAP] =
			_m_character->_cur_attr[AGILITY];
	if (_m_character->_abilities[IDENTIFY_TRAP] > 95)
		_m_character->_abilities[IDENTIFY_TRAP] = 95;

	// Base Chance to Disarm a Trap (modified by Maze Level) (%)
	_m_character->_abilities[BASE_DISARM_TRAP] =
		(_m_character->_class == NINJA) || (_m_character->_class == THIEF)
			? ((current_level + 50) / 69.0) * 100
			: 0;

	// Chance to activate a Trap if identify fails (%)
	_m_character->_abilities[ACTIVATE_TRAP] =
		(_m_character->_class == NINJA) || (_m_character->_class == THIEF)
			? 100 - ((_m_character->_cur_attr[AGILITY] / 20.0) * 100)
			: 100;

	// Base Chance to avoid following into a Pit (modified by Maze Level) (%)
	_m_character->_abilities[BASE_AVOID_PIT] =
		(_m_character->_cur_attr[AGILITY] / 25.0) * 100;

	// Base Resist Bonus (d20)
	_m_character->_abilities[BASE_RESIST_BONUS] = 1 * (current_level / 5);
	if (_m_character->_cur_attr[LUCK] >= 18)
		_m_character->_abilities[BASE_RESIST_BONUS] += 3;
	else if (_m_character->_cur_attr[LUCK] >= 12)
		_m_character->_abilities[BASE_RESIST_BONUS] += 2;
	else if (_m_character->_cur_attr[LUCK] >= 6)
		_m_character->_abilities[BASE_RESIST_BONUS] += 1;

	// Chance equipment is intact on a corpse TODO: check this is accurate
	_m_character->_abilities[EQUIPMENT_INTACT_ON_WIPE] =
		(_m_character->_cur_attr[LUCK] / 21.0f) * 100;

	// Other Resists (d20)

	// TODO: rewrite using a Constexpr table
	const auto brb{_m_character->_abilities[BASE_RESIST_BONUS]};
	switch (_m_character->_class) { // NOLINT(clang-diagnostic-switch)
	case SAMURAI:
		_m_character->_abilities[RESISTANCE_VS_POISON_PARALYSIS] = brb + 2;
		_m_character->_abilities[RESISTANCE_VS_CRITICAL_HIT] = brb + 2;
		_m_character->_abilities[RESISTANCE_VS_STONING] = 0;
		_m_character->_abilities[RESISTANCE_VS_BREATH_ATTACKS] = 0;
		_m_character->_abilities[RESISTANCE_VS_POISON_GAS_TRAP] = 0;
		_m_character->_abilities[RESISTANCE_VS_MAGE_PRIEST_TRAP] = brb + 2;
		_m_character->_abilities[RESISTANCE_VS_SILENCE] = brb + 2;
		break;
	case FIGHTER:
		_m_character->_abilities[RESISTANCE_VS_POISON_PARALYSIS] = brb + 3;
		_m_character->_abilities[RESISTANCE_VS_CRITICAL_HIT] = brb + 3;
		_m_character->_abilities[RESISTANCE_VS_STONING] = 0;
		_m_character->_abilities[RESISTANCE_VS_BREATH_ATTACKS] = 0;
		_m_character->_abilities[RESISTANCE_VS_POISON_GAS_TRAP] = 0;
		_m_character->_abilities[RESISTANCE_VS_MAGE_PRIEST_TRAP] = 0;
		_m_character->_abilities[RESISTANCE_VS_SILENCE] = 0;
		break;
	case PRIEST:
		_m_character->_abilities[RESISTANCE_VS_POISON_PARALYSIS] = 0;
		_m_character->_abilities[RESISTANCE_VS_CRITICAL_HIT] = 0;
		_m_character->_abilities[RESISTANCE_VS_STONING] = brb + 3;
		_m_character->_abilities[RESISTANCE_VS_BREATH_ATTACKS] = 0;
		_m_character->_abilities[RESISTANCE_VS_POISON_GAS_TRAP] = 0;
		_m_character->_abilities[RESISTANCE_VS_MAGE_PRIEST_TRAP] = 0;
		_m_character->_abilities[RESISTANCE_VS_SILENCE] = 0;
		break;
	case LORD:
		_m_character->_abilities[RESISTANCE_VS_POISON_PARALYSIS] = brb + 2;
		_m_character->_abilities[RESISTANCE_VS_CRITICAL_HIT] = brb + 2;
		_m_character->_abilities[RESISTANCE_VS_STONING] = brb + 2;
		_m_character->_abilities[RESISTANCE_VS_BREATH_ATTACKS] = 0;
		_m_character->_abilities[RESISTANCE_VS_POISON_GAS_TRAP] = 0;
		_m_character->_abilities[RESISTANCE_VS_MAGE_PRIEST_TRAP] = 0;
		_m_character->_abilities[RESISTANCE_VS_SILENCE] = 0;
		break;
	case BISHOP:
		_m_character->_abilities[RESISTANCE_VS_POISON_PARALYSIS] = 0;
		_m_character->_abilities[RESISTANCE_VS_CRITICAL_HIT] = 0;
		_m_character->_abilities[RESISTANCE_VS_STONING] = brb + 2;
		_m_character->_abilities[RESISTANCE_VS_BREATH_ATTACKS] = brb + 2;
		_m_character->_abilities[RESISTANCE_VS_POISON_GAS_TRAP] = 0;
		_m_character->_abilities[RESISTANCE_VS_MAGE_PRIEST_TRAP] = brb + 2;
		_m_character->_abilities[RESISTANCE_VS_SILENCE] = brb + 2;
		break;
	case THIEF:
		_m_character->_abilities[RESISTANCE_VS_POISON_PARALYSIS] = 0;
		_m_character->_abilities[RESISTANCE_VS_CRITICAL_HIT] = 0;
		_m_character->_abilities[RESISTANCE_VS_STONING] = 0;
		_m_character->_abilities[RESISTANCE_VS_BREATH_ATTACKS] = 0;
		_m_character->_abilities[RESISTANCE_VS_POISON_GAS_TRAP] = brb + 3;
		_m_character->_abilities[RESISTANCE_VS_MAGE_PRIEST_TRAP] = 0;
		_m_character->_abilities[RESISTANCE_VS_SILENCE] = 0;
		break;
	case NINJA:
		_m_character->_abilities[RESISTANCE_VS_POISON_PARALYSIS] = brb + 3;
		_m_character->_abilities[RESISTANCE_VS_CRITICAL_HIT] = brb + 3;
		_m_character->_abilities[RESISTANCE_VS_STONING] = brb + 2;
		_m_character->_abilities[RESISTANCE_VS_BREATH_ATTACKS] = brb + 4;
		_m_character->_abilities[RESISTANCE_VS_POISON_GAS_TRAP] = brb + 3;
		_m_character->_abilities[RESISTANCE_VS_MAGE_PRIEST_TRAP] = brb + 2;
		_m_character->_abilities[RESISTANCE_VS_SILENCE] = brb + 2;
		break;
	case MAGE:
		_m_character->_abilities[RESISTANCE_VS_POISON_PARALYSIS] = 0;
		_m_character->_abilities[RESISTANCE_VS_CRITICAL_HIT] = 0;
		_m_character->_abilities[RESISTANCE_VS_STONING] = 0;
		_m_character->_abilities[RESISTANCE_VS_BREATH_ATTACKS] = 0;
		_m_character->_abilities[RESISTANCE_VS_POISON_GAS_TRAP] = 0;
		_m_character->_abilities[RESISTANCE_VS_MAGE_PRIEST_TRAP] = brb + 3;
		_m_character->_abilities[RESISTANCE_VS_SILENCE] = brb + 3;
		break;
	default:
		break;
	}

	using enum Enums::Character::Race;
	if (_m_character->_race == HUMAN)
		_m_character->_abilities[RESISTANCE_VS_POISON_PARALYSIS] += 1;
	if (_m_character->_race == HUMAN)
		_m_character->_abilities[RESISTANCE_VS_CRITICAL_HIT] += 1;
	if (_m_character->_race == GNOME)
		_m_character->_abilities[RESISTANCE_VS_STONING] += 2;
	if (_m_character->_race == ELF)
		_m_character->_abilities[RESISTANCE_VS_BREATH_ATTACKS] += 2;
	if (_m_character->_race == DWARF)
		_m_character->_abilities[RESISTANCE_VS_POISON_GAS_TRAP] += 4;
	if (_m_character->_race == HOBBIT) {
		_m_character->_abilities[RESISTANCE_VS_MAGE_PRIEST_TRAP] += 2;
		_m_character->_abilities[RESISTANCE_VS_SILENCE] += 3;
	}
	if (_m_character->_abilities[RESISTANCE_VS_POISON_PARALYSIS] > 19)
		_m_character->_abilities[RESISTANCE_VS_POISON_PARALYSIS] = 19;
	if (_m_character->_abilities[RESISTANCE_VS_CRITICAL_HIT] > 19)
		_m_character->_abilities[RESISTANCE_VS_CRITICAL_HIT] = 19;
	if (_m_character->_abilities[RESISTANCE_VS_STONING] > 19)
		_m_character->_abilities[RESISTANCE_VS_STONING] = 19;
	if (_m_character->_abilities[RESISTANCE_VS_BREATH_ATTACKS] > 19)
		_m_character->_abilities[RESISTANCE_VS_BREATH_ATTACKS] = 19;
	if (_m_character->_abilities[RESISTANCE_VS_POISON_GAS_TRAP] > 19)
		_m_character->_abilities[RESISTANCE_VS_POISON_GAS_TRAP] = 19;
	if (_m_character->_abilities[RESISTANCE_VS_MAGE_PRIEST_TRAP] > 19)
		_m_character->_abilities[RESISTANCE_VS_MAGE_PRIEST_TRAP] = 19;
	if (_m_character->_abilities[RESISTANCE_VS_SILENCE] > 19)
		_m_character->_abilities[RESISTANCE_VS_SILENCE] = 19;

	// Resistance to harmful spells (%)
	_m_character->_abilities[RESISTANCE_VS_KATINO] = current_level * 10;
	if (_m_character->_abilities[RESISTANCE_VS_KATINO] > 100)
		_m_character->_abilities[RESISTANCE_VS_KATINO] = 100;
	_m_character->_abilities[RESISTANCE_VS_BADI] = current_level * 10;
	if (_m_character->_abilities[RESISTANCE_VS_BADI] > 100)
		_m_character->_abilities[RESISTANCE_VS_BADI] = 100;
	_m_character->_abilities[RESISTANCE_VS_MANIFO] = 50 + (current_level * 10);
	if (_m_character->_abilities[RESISTANCE_VS_MANIFO] > 100)
		_m_character->_abilities[RESISTANCE_VS_MANIFO] = 100;

	_m_character->_abilities[RECOVER_FROM_SLEEP] = current_level * 10;
	if (_m_character->_abilities[RECOVER_FROM_SLEEP] > 100)
		_m_character->_abilities[RECOVER_FROM_SLEEP] = 100;

	_m_character->_abilities[RECOVER_FROM_FEAR] = current_level * 5;
	if (_m_character->_abilities[RECOVER_FROM_FEAR] > 100)
		_m_character->_abilities[RECOVER_FROM_FEAR] = 100;

	_m_character->_abilities[BONUS_MAGE_SPELLS] = 0;
	_m_character->_abilities[BONUS_PRIEST_SPELLS] = 0;

	// And set poison/regeneration to default
	if (!change_class) {
		_m_character->_abilities[HP_LOSS_PER_TURN] = 0;
		_m_character->_abilities[HP_GAIN_PER_TURN] = 0;
		_m_character->_abilities[POISON_STRENGTH] = 0;
	}
}

// Now work out spellpoints!
auto Sorcery::CharacterCreate::_reset_starting_sp() -> void {

	_set_starting_sp();

	// And add in 1 spell point for each known spell NOT of the type
	for (auto spell_level = 1u; spell_level <= 7u; spell_level++) {

		// Handle Priest Spells
		auto priest_known{static_cast<unsigned int>(std::count_if(
			_m_character->_spells.begin(), _m_character->_spells.end(),
			[=](auto spell) {
				return (spell.type == Enums::Magic::SpellType::DIVINE) &&
					   (spell.level == spell_level) && (spell.known);
			}))};

		if (_m_character->_priest_max_sp[spell_level] < priest_known) {
			_m_character->_priest_max_sp[spell_level] = priest_known;
			_m_character->_priest_cur_sp[spell_level] =
				_m_character->_priest_max_sp[spell_level];
		}

		// Handle Mage Spells
		auto mage_known{static_cast<unsigned int>(std::count_if(
			_m_character->_spells.begin(), _m_character->_spells.end(),
			[=](auto spell) {
				return (spell.type == Enums::Magic::SpellType::ARCANE) &&
					   (spell.level == spell_level) && (spell.known);
			}))};
		if (_m_character->_mage_max_sp[spell_level] < mage_known) {
			_m_character->_mage_max_sp[spell_level] = mage_known;
			_m_character->_mage_cur_sp[spell_level] =
				_m_character->_mage_max_sp[spell_level];
		}
	}
}

// Set the starting spellpoints
auto Sorcery::CharacterCreate::_set_starting_sp() -> void {

	// By default clear all spells
	_clear_sp();

	// In the original code this is handled in
	// "SETSPELS"/"SPLPERLV"/"NWMAGE"/"NWPRIEST"
	switch (_m_character->_class) { // NOLINT(clang-diagnostic-switch)
		using enum Enums::Character::Ability;
		using enum Enums::Character::Class;
		using enum Enums::Config::Options;
	case PRIEST:
		_m_character->_priest_max_sp[1] =
			_m_character->_ctx->get_config(STRICT_MODE)
				? 2
				: 2 + _m_character->_abilities[BONUS_PRIEST_SPELLS];
		break;
	case BISHOP:
		_m_character->_mage_max_sp[1] = 2;
		break;
	case MAGE:
		_m_character->_mage_max_sp[1] =
			_m_character->_ctx->get_config(STRICT_MODE)
				? 2
				: 2 + _m_character->_abilities[BONUS_MAGE_SPELLS];
		break;
	default:
		break;
	}

	_m_character->_priest_cur_sp[1] = _m_character->_priest_max_sp[1];
	_m_character->_mage_cur_sp[1] = _m_character->_mage_max_sp[1];
}

// Clear all spell-points to begin with
auto Sorcery::CharacterCreate::_clear_sp() -> void {

	for (auto spell_level = 1; spell_level <= 7; spell_level++) {
		_m_character->_priest_max_sp[spell_level] = 0;
		_m_character->_priest_cur_sp[spell_level] = 0;
		_m_character->_mage_max_sp[spell_level] = 0;
		_m_character->_mage_cur_sp[spell_level] = 0;
	}
}

// Reset Starting Spells on Class Change
auto Sorcery::CharacterCreate::_reset_start_spells() -> void {

	// All known spells are kept, and starting class ones added if needed
	_set_start_spells();
}

auto Sorcery::CharacterCreate::_learn_spell(Enums::Magic::SpellID spell_id)
	-> void {

	std::vector<Spell>::iterator it;
	it = std::find_if(_m_character->_spells.begin(),
					  _m_character->_spells.end(), [&](auto item) {
						  return item.id == spell_id;
					  });
	if (it != _m_character->_spells.end()) {
		(*it).known = true;
		_m_character->_spells_known[(*it).id] = true;
	} // NOLINT(clang-di
}

// Set starting spells
auto Sorcery::CharacterCreate::_set_start_spells() -> void {

	// This is taken from "KEEPCHYN" which hard codes the spells known to
	// beginning characters!
	switch (_m_character->_class) { // NOLINT(clang-diagnostic-switch)
		using enum Enums::Character::Class;
		using enum Enums::Magic::SpellID;
	case BISHOP:
	case MAGE:
		_learn_spell(KATINO);
		_learn_spell(HALITO);
		break;
	case PRIEST:
		_learn_spell(DIOS);
		_learn_spell(BADIOS);
		break;
	default:
		break;
	}
}

// Get HP gained for all levels apart from the first
auto Sorcery::CharacterCreate::_get_hp_per_level() -> int {

	// In the original code ("MOREHP"), Samurai get 2d8
	auto extra_hp{0};
	switch (_m_character->_class) { // NOLINT(clang-diagnostic-switch)
		using enum Enums::Character::Class;
		using enum Enums::System::Random;
	case FIGHTER:
	case LORD:
		extra_hp += _m_character->_ctx->get_random(D10);
		break;
	case PRIEST:
		extra_hp += _m_character->_ctx->get_random(D8);
		break;
	case THIEF:
	case BISHOP:
	case NINJA:
		extra_hp += _m_character->_ctx->get_random(D6);
		break;
	case MAGE:
		extra_hp += _m_character->_ctx->get_random(D4);
		break;
	case SAMURAI:
		extra_hp += _m_character->_ctx->get_random(D8);
		extra_hp += _m_character->_ctx->get_random(D8);
		break;
	default:
		break;
	}

	extra_hp +=
		_m_character->_abilities[Enums::Character::Ability::BONUS_HIT_POINTS];
	if (extra_hp < 0)
		extra_hp = 1;

	// Though this could be an unsigned int as it will always be greater than 0,
	// just return int as everything else is
	return extra_hp;
}

// Add hit points on level gain (but note the strict mode limitation mentioned
// below)
auto Sorcery::CharacterCreate::_update_hp_for_level() -> int {

	// Note the rerolling of all HP ("MADELEV") when levelling - and using
	// MaxLevel achieved when in strict mode
	using enum Enums::Character::Ability;
	auto hp_gained{0};
	if (_m_character->_ctx->get_config(Enums::Config::LEVEL_REROLL_HP)) {
		auto hp_total{0};
		for (auto level = 1; level < _m_character->_abilities[CURRENT_LEVEL];
			 level++)
			hp_total += _get_hp_per_level();
		if (hp_total < _m_character->_abilities[MAX_HP])
			hp_total = _m_character->_abilities[MAX_HP] + 1;
		hp_gained = hp_total - _m_character->_abilities[MAX_HP];
		_m_character->_abilities[MAX_HP] += hp_gained;
		_m_character->_abilities[CURRENT_HP] += hp_gained;
	} else {
		hp_gained = _get_hp_per_level();
		_m_character->_abilities[MAX_HP] += hp_gained;
		_m_character->_abilities[CURRENT_HP] += hp_gained;
	}

	return hp_gained;
}

auto Sorcery::CharacterCreate::_update_stat_for_level(
	Enums::Character::Attribute attribute, std::string stat) -> std::string {

	auto message{""s};
	using enum Enums::System::Random;
	if (_m_character->_ctx->get_random(D100) < 75) {
		const auto chance{
			_m_character->_abilities.at(Enums::Character::Ability::AGE) /
			130.f};
		if (_m_character->_ctx->get_random(D100) < chance) {
			// Decrease
			bool proceed{true};
			if (_m_character->_cur_attr.at(attribute) == 18 &&
				_m_character->_ctx->get_random(D6) > 1)
				proceed = false;

			if (proceed) {
				_m_character->_cur_attr.at(attribute) =
					_m_character->_cur_attr.at(attribute) - 1;
				message = std::format(
					"{} {}", _m_character->_ctx->get_string("LEVEL_LOSS"),
					stat);
				if (_m_character->_cur_attr.at(attribute) < 1)
					_m_character->_cur_attr.at(attribute) = 1;
			}
		} else {
			if (_m_character->_cur_attr.at(attribute) < 18) {
				_m_character->_cur_attr.at(attribute) =
					_m_character->_cur_attr.at(attribute) + 1;
				if (_m_character->_cur_attr.at(attribute) >
					_m_character->_max_attr.at(attribute))
					_m_character->_max_attr.at(attribute) =
						_m_character->_cur_attr.at(attribute);
				message = std::format(
					"{} {}", _m_character->_ctx->get_string("LEVEL_GAIN"),
					stat);
			}
		}
	}

	if (_m_character->_cur_attr.at(attribute) >
		_m_character->_max_attr.at(attribute))
		_m_character->_max_attr.at(attribute) =
			_m_character->_cur_attr.at(attribute);

	return message;
}

// For each spell level, try to learn spells - called before set_spellpoints
auto Sorcery::CharacterCreate::_try_learn_spell(
	Enums::Magic::SpellType spell_type, unsigned int spell_level) -> bool {

	bool new_spell_learnt{false};

	// Only do spells if a character has available spell points in this spell
	// level
	using enum Enums::Magic::SpellType;
	if (spell_type == DIVINE)
		if (_m_character->_priest_max_sp[spell_level] == 0)
			return false;
	if (spell_type == ARCANE)
		if (_m_character->_mage_max_sp[spell_level] == 0)
			return false;

	// Get all unknown spells of this spell level and type
	for (auto spells{
			 _m_character->_spells | std::views::filter([&](Spell spell) {
				 return (spell.type == spell_type) &&
						(spell.level == spell_level) && (spell.known == false);
			 })};
		 auto &spell : spells) {

		const auto dice{
			_m_character->_ctx->get_random(Enums::System::Random::ZERO_TO_29)};

		// Check the Spell Type against the relevant stat (see
		// SPLPERLV//TRYLEARN)
		using enum Enums::Character::Attribute;
		if (spell_type == DIVINE) {
			if (dice <=
				static_cast<unsigned int>(_m_character->_cur_attr[PIETY])) {
				spell.known = true;
				_m_character->_spells_known[spell.id] = true;
				new_spell_learnt = true;
			}
		}
		if (spell_type == ARCANE) {
			if (dice <=
				static_cast<unsigned int>(_m_character->_cur_attr[IQ])) {
				spell.known = true;
				_m_character->_spells_known[spell.id] = true;
				new_spell_learnt = true;
			}
		}
	}

	return new_spell_learnt;
}

// Reimplementation of SPLPERLV - note this will reset spell points!
auto Sorcery::CharacterCreate::_calculate_sp(Enums::Magic::SpellType spell_type,
											 unsigned int level_mod,
											 unsigned int level_offset)
	-> void {

	std::map<unsigned int, unsigned int> *spells{
		spell_type == Enums::Magic::SpellType::DIVINE
			? &_m_character->_priest_max_sp
			: &_m_character->_mage_max_sp};
	for (auto spell_level = 1; spell_level <= 7; spell_level++)
		(*spells)[spell_level] = 0;

	// https://datadrivengamer.blogspot.com/2019/08/the-not-so-basic-mechanics-of-wizardry.html
	for (auto spell_level = 1; spell_level <= 7; spell_level++) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnarrowing"
		int spell_count{
			_m_character->_abilities[Enums::Character::Ability::CURRENT_LEVEL] -
			level_mod + level_offset - (level_offset * spell_level)};
#pragma GCC diagnostic pop
		spell_count = std::max(0, std::min(spell_count, 9));
		(*spells)[spell_level] = spell_count;
	}
}

// Copied and rewritten from the original code from
// MINMAG/MINPRI/NWPRIEST/NWMAGE
auto Sorcery::CharacterCreate::_set_sp() -> bool {

	bool new_spells_learnt{false};

	for (auto spell_level = 1; spell_level <= 7; spell_level++) {
		_m_character->_priest_max_sp[spell_level] = 0;
		_m_character->_mage_max_sp[spell_level] = 0;
	}

	// Generate spell points according to current level and class (this does not
	// change any spells known but will reset spell points)
	switch (_m_character->_class) { // NOLINT(clang-diagnostic-switch)
		using enum Enums::Magic::SpellType;
		using enum Enums::Character::Class;
	case FIGHTER:
	case THIEF:
	case NINJA:
		break;
	case MAGE:
		_calculate_sp(ARCANE, 0, 2);
		break;
	case PRIEST:
		_calculate_sp(DIVINE, 0, 2);
		break;
	case BISHOP:
		_calculate_sp(DIVINE, 3, 4);
		_calculate_sp(ARCANE, 0, 4);
		break;
	case SAMURAI:
		_calculate_sp(ARCANE, 3, 3);
		break;
	case LORD:
		_calculate_sp(DIVINE, 3, 2);
		break;
	default:
		break;
	}

	// Now try and learn any additional spells
	for (auto spell_level = 1; spell_level <= 7; spell_level++) {

		// If we know at least one spell in this level, we can always try and
		// learn more no matter what even if we are currently a non-spellcasting
		// class
		using enum Enums::Magic::SpellType;
		if (_m_character->_priest_max_sp[spell_level] > 0) {
			if (_try_learn_spell(DIVINE, spell_level))
				new_spells_learnt = true;
		}
		if (_m_character->_mage_max_sp[spell_level] > 0) {
			if (_try_learn_spell(ARCANE, spell_level))
				new_spells_learnt = true;
		}
	}

	// Now make sure that if the above fails, we always learn a spell of each
	// circle just in case (GETMAGSP/GETPRISP - though note that the orignal is
	// bugged and selects the wrong level spells sometimes) - note this is for
	// creatures so it might not be applicable?
	for (auto spell_level = 1; spell_level <= 7; spell_level++) {

		using enum Enums::Magic::SpellType;
		using enum Enums::Magic::SpellID;
		using enum Enums::System::Random;
		if (_m_character->_priest_max_sp[spell_level] > 0 &&
			_get_spells_known(DIVINE, spell_level) == 0) {
			switch (spell_level) {
			case 1:
				_learn_spell(BADIOS);
				break;
			case 2:
				_learn_spell(MONTINO);
				new_spells_learnt = true;
				break;
			case 3:
				if (_m_character->_ctx->get_random(D100) > 33)
					_learn_spell(DIALKO);
				else
					_learn_spell(LOMILWA);
				new_spells_learnt = true;
				break;
			case 4:
				_learn_spell(BADIAL);
				new_spells_learnt = true;
				break;
			case 5:
				if (_m_character->_ctx->get_random(D100) > 33)
					_learn_spell(BADIALMA);
				else
					_learn_spell(BADI);
				new_spells_learnt = true;
				break;
			case 6:
				if (_m_character->_ctx->get_random(D100) > 33)
					_learn_spell(LORTO);
				else
					_learn_spell(MABADI);
				new_spells_learnt = true;
				break;
			case 7:
				_learn_spell(MALIKTO);
				new_spells_learnt = true;
				break;
			default:
				break;
			}
		}
		if (_m_character->_mage_max_sp[spell_level] > 0 &&
			_get_spells_known(ARCANE, spell_level) == 0) {
			switch (spell_level) {
			case 1:
				if (_m_character->_ctx->get_random(D100) > 33)
					_learn_spell(KATINO);
				else
					_learn_spell(HALITO);
				new_spells_learnt = true;
				break;
			case 2:
				if (_m_character->_ctx->get_random(D100) > 33)
					_learn_spell(DILTO);
				else
					_learn_spell(SOPIC);
				new_spells_learnt = true;
				break;
			case 3:
				if (_m_character->_ctx->get_random(D100) > 33)
					_learn_spell(MOLITO);
				else
					_learn_spell(MAHALITO);
				new_spells_learnt = true;
				break;
			case 4:
				if (_m_character->_ctx->get_random(D100) > 33)
					_learn_spell(DALTO);
				else
					_learn_spell(LAHALITO);
				new_spells_learnt = true;
				break;
			case 5:
				if (_m_character->_ctx->get_random(D100) > 33)
					_learn_spell(MAMORLIS);
				else
					_learn_spell(MADALTO);
				new_spells_learnt = true;
				break;
			case 6:
				if (_m_character->_ctx->get_random(D100) > 33)
					_learn_spell(LAKANITO);
				else
					_learn_spell(ZILWAN);
				new_spells_learnt = true;
				break;
			case 7:
				_learn_spell(MALOR);
				new_spells_learnt = true;
				break;
			default:
				break;
			}
		}
	}

	// And work out spells known and boost sp accordingly if we have to but note
	// that we can't go above maxlevel/2 (for the case of level drain)
	for (auto spell_level = 1; spell_level <= 7; spell_level++) {

		using enum Enums::Character::Ability;
		if ((spell_level * 2) - 1 > _m_character->_abilities.at(MAX_LEVEL))
			continue;

		using enum Enums::Magic::SpellType;
		const auto priest_known{_get_spells_known(DIVINE, spell_level)};
		const auto mage_known{_get_spells_known(ARCANE, spell_level)};

		if (priest_known > _m_character->_priest_max_sp[spell_level])
			_m_character->_priest_max_sp[spell_level] = priest_known;
		if (mage_known > _m_character->_mage_max_sp[spell_level])
			_m_character->_mage_max_sp[spell_level] = mage_known;
	}

	// level up only takes place at the inn, so reset spells
	for (auto spell_level = 1; spell_level <= 7; spell_level++) {
		_m_character->_priest_cur_sp[spell_level] =
			_m_character->_priest_max_sp[spell_level];
		_m_character->_mage_cur_sp[spell_level] =
			_m_character->_mage_max_sp[spell_level];
	}

	return new_spells_learnt;
}

auto Sorcery::CharacterCreate::_legate_start_info() -> void {

	// From here:
	// https://datadrivengamer.blogspot.com/2021/08/the-new-mechanics-of-wizardry-iii.html
	using enum Enums::Character::Ability;
	_m_character->_abilities[CURRENT_LEVEL] = 1;
	_m_character->_abilities[CURRENT_XP] = 0;
	_m_character->_abilities[NEXT_LEVEL_XP] =
		_get_xp_for_level(_m_character->_abilities[CURRENT_LEVEL]);
	_m_character->_abilities[MAX_LEVEL] =
		_m_character->_abilities[CURRENT_LEVEL];
	_m_character->_abilities[NEGATIVE_LEVEL] = 0;
	_m_character->_abilities[HIT_DICE] = 1;
	if (_m_character->_abilities[GOLD] > 500)
		_m_character->_abilities[GOLD] = 500;
	_m_character->_abilities[AGE] = 20 * 52;
	_m_character->_abilities[SWIM] = 1;
	_m_character->_abilities[MARKS] = 0;
	_m_character->_abilities[DEATHS] = 0;

	// (D7 - 4) is -3 to +3
	using enum Enums::Character::Attribute;
	using enum Enums::System::Random;
	_m_character->_start_attr[STRENGTH] +=
		(_m_character->_ctx->get_random(D7) - 4);
	_m_character->_start_attr[IQ] += (_m_character->_ctx->get_random(D7) - 4);
	_m_character->_start_attr[PIETY] +=
		(_m_character->_ctx->get_random(D7) - 4);
	_m_character->_start_attr[VITALITY] +=
		(_m_character->_ctx->get_random(D7) - 4);
	_m_character->_start_attr[AGILITY] +=
		(_m_character->_ctx->get_random(D7) - 4);
	_m_character->_start_attr[LUCK] += (_m_character->_ctx->get_random(D7) - 4);

	auto mage_sp_total{0};
	for (auto level = 1; level <= 7; level++)
		mage_sp_total += _m_character->_mage_max_sp[level];
	_m_character->_start_attr[IQ] += (mage_sp_total / 7);

	auto priest_sp_total{1};
	for (auto level = 1; level <= 7; level++)
		priest_sp_total += _m_character->_priest_max_sp[level];
	_m_character->_start_attr[PIETY] += (priest_sp_total / 10);

	switch (_m_character->_class) { // NOLINT(clang-diagnostic-switch)
		using enum Enums::Character::Class;
	case FIGHTER:
	case LORD:
	case SAMURAI:
		_m_character->_start_attr[STRENGTH] += 2;
		break;
	case MAGE:
		_m_character->_start_attr[IQ] += 2;
		break;
	case PRIEST:
	case BISHOP:
		_m_character->_start_attr[PIETY] += 2;
		break;
	case THIEF:
	case NINJA:
		_m_character->_start_attr[AGILITY] += 2;
	default:
		break;
	}

	_m_character->set_status(Enums::Character::Status::OK);
	_m_character->_location = Enums::Character::Location::TAVERN;
	_m_character->_abilities[CURRENT_HP] = _m_character->_abilities[MAX_HP];

	// Clamp Values
	_m_character->_start_attr[STRENGTH] =
		std::min(_m_character->_start_attr[STRENGTH], 18);
	_m_character->_start_attr[IQ] = std::min(_m_character->_start_attr[IQ], 18);
	_m_character->_start_attr[PIETY] =
		std::min(_m_character->_start_attr[PIETY], 18);
	_m_character->_start_attr[VITALITY] =
		std::min(_m_character->_start_attr[VITALITY], 18);
	_m_character->_start_attr[AGILITY] =
		std::min(_m_character->_start_attr[AGILITY], 18);
	_m_character->_start_attr[LUCK] =
		std::min(_m_character->_start_attr[LUCK], 18);
	_m_character->_start_attr[STRENGTH] =
		std::max(_m_character->_start_attr[STRENGTH], 3);
	_m_character->_start_attr[IQ] = std::max(_m_character->_start_attr[IQ], 3);
	_m_character->_start_attr[PIETY] =
		std::max(_m_character->_start_attr[PIETY], 3);
	_m_character->_start_attr[VITALITY] =
		std::max(_m_character->_start_attr[VITALITY], 3);
	_m_character->_start_attr[AGILITY] =
		std::max(_m_character->_start_attr[AGILITY], 3);
	_m_character->_start_attr[LUCK] =
		std::max(_m_character->_start_attr[LUCK], 3);

	_m_character->_cur_attr = _m_character->_start_attr;
	_m_character->_max_attr = _m_character->_cur_attr;

	_m_character->_abilities[CURRENT_XP] = 0;
	_m_character->_abilities[NEXT_LEVEL_XP] =
		_get_xp_for_level(_m_character->_abilities[CURRENT_LEVEL]);
}

auto Sorcery::CharacterCreate::_regenerate_start_info() -> void {

	using enum Enums::Character::Ability;
	_m_character->_abilities[MAX_LEVEL] =
		_m_character->_abilities[CURRENT_LEVEL];
	_m_character->_abilities[CURRENT_LEVEL] = 1;
	_m_character->_abilities[CURRENT_XP] = 0;
	_m_character->_abilities[NEXT_LEVEL_XP] =
		_get_xp_for_level(_m_character->_abilities[CURRENT_LEVEL]);

	// https://datadrivengamer.blogspot.com/2019/08/the-not-so-basic-mechanics-of-wizardry.html
	auto age_increment{
		(52 * (3 + _m_character->_ctx->get_random(Enums::System::Random::D3))) +
		44};
	_m_character->_abilities[AGE] += age_increment;

	// Reset attributes to racial minimums
	_set_racial_attributes();
	_m_character->_cur_attr = _m_character->_start_attr;
}

auto Sorcery::CharacterCreate::_set_racial_attributes() -> void {

	using enum Enums::Character::Attribute;

	const auto &attributes{
		racial_attributes[std::to_underlying(_m_character->_race)]};

	_m_character->_start_attr = {
		{STRENGTH, attributes.strength}, {IQ, attributes.iq},
		{PIETY, attributes.piety},		 {VITALITY, attributes.vitality},
		{AGILITY, attributes.agility},	 {LUCK, attributes.luck},
	};
}