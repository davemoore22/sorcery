// Copyright (C) 2021 Dave Moore
//
// This file is part of Sorcery: Dreams of the Mad Overlord.
//
// Sorcery: Dreams of the Mad Overlord is free software: you can redistribute
// it and/or modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation, either version 2 of the License,
// or (at your option) any later version.
//
// Sorcery: Dreams of the Mad Overlord is distributed in the hope that it wil
// be useful, but WITHOUT ANY WARRANTY; wiWthout even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
// Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Sorcery: Dreams of the Mad Overlord.  If not,
// see <http://www.gnu.org/licenses/>.
//
// If you modify this program, or any covered work, by linking or combining
// it with the libraries referred to in README (or a modified version of
// said libraries), containing parts covered by the terms of said libraries,
// the licensors of this program grant you additional permission to convey
// the resulting work.

#include "character.hpp"

// Standard Constructor
Sorcery::Character::Character(System &system, Display &display, Graphics &graphics)
	: _system{system}, _display{display}, _graphics{graphics} {

	set_stage(CharacterStage::NOT_BEGUN);
	_display.generate_components("character_create_stage_1", sprites, texts, frames);
}

// Note for the copy constuctors we only copy the character data/PODs within
Sorcery::Character::Character(const Character &other)
	: sprites{other.sprites}, texts{other.texts}, frames{other.frames}, _system{other._system},
	  _display{other._display}, _graphics{other._graphics}, _abilities{other._abilities},
	  _cleric_max_sp{other._cleric_max_sp}, _cleric_cur_sp{other._cleric_cur_sp},
	  _mage_max_sp{other._mage_max_sp}, _mage_cur_sp{other._mage_cur_sp}, _spells{other._spells},
	  _current_stage{other._current_stage}, _name{other._name}, _race{other._race},
	  _class{other._class}, _alignment{other._alignment}, _start_attr{other._start_attr},
	  _cur_attr{other._cur_attr}, _max_attr{other._max_attr}, _view{other._view},
	  _points_left{other._points_left}, _st_points{other._st_points},
	  _pos_classes{other._pos_classes}, _class_list{other._class_list},
	  _num_pos_classes{other._num_pos_classes}, _portrait_index{other._portrait_index} {}

auto Sorcery::Character::operator=(const Character &other) -> Character & {

	sprites = other.sprites;
	texts = other.texts;
	frames = other.frames;

	_system = other._system;
	_display = other._display;
	_graphics = other._graphics;

	_abilities = other._abilities;
	_cleric_max_sp = other._cleric_max_sp;
	_cleric_cur_sp = other._cleric_cur_sp;
	_mage_max_sp = other._mage_max_sp;
	_mage_cur_sp = other._mage_cur_sp;
	_spells = other._spells;
	_current_stage = other._current_stage;
	_name = other._name;
	_race = other._race;
	_class = other._class;
	_alignment = other._alignment;
	_start_attr = other._start_attr;
	_cur_attr = other._cur_attr;
	_max_attr = other._max_attr;
	_view = other._view;
	_points_left = other._points_left;
	_st_points = other._st_points;
	_pos_classes = other._pos_classes;
	_class_list = other._class_list;
	_num_pos_classes = other._num_pos_classes;
	_portrait_index = other._portrait_index;

	return *this;
}

auto Sorcery::Character::draw(
	[[maybe_unused]] sf::RenderTarget &target, sf::RenderStates states) const -> void {

	// Note depending on the current character stage, then we draw its repreentation differently
	/*
	1. Select Name
	2. Choose Race
	3. Choose Alignment
	4. Allocate Attributes
	5. Choose Class
	6. Choose Portrait
	7. Review and Confirm

	also we need to draw a summary version too, perhaps a derived class (Character Summary)?

	*/

	states.transform *= getTransform();
	// target.draw(_frame, states);
}

// Overloaded Operator
auto Sorcery::Character::operator[](const CharacterAbility &key) -> int & {

	return _abilities[key];
}

// Utility Functions

auto Sorcery::Character::get_stage() const -> CharacterStage {

	return _current_stage;
}

// Reset a character back to a particular state
auto Sorcery::Character::set_stage(const CharacterStage stage) -> void {

	_current_stage = stage;

	// TODO: assignment/copy operators to backups at each stage so we can go back and undo

	// Also need methods to boost to next stage from start, e.g. random name or race, and going into
	// create immediately sets that

	// As well as a summary display of character (to be displayed where?)

	// Also handle going back to previous stage as well

	switch (stage) {
	case CharacterStage::NOT_BEGUN:
		_name.clear();
		[[fallthrough]];
	case CharacterStage::ENTER_NAME:
		_race = CharacterRace::NONE;

		_alignment = CharacterAlignment::NONE;

		_start_attr.clear();
		_cur_attr.clear();
		_max_attr.clear();
		//_status.clear();
		_abilities.clear();
		_class = CharacterClass::NONE;
		_points_left = 0;
		_st_points = 0;
		_pos_classes.clear();
		_num_pos_classes = 0;
		_portrait_index = 0;
		_cleric_max_sp.clear();
		_cleric_cur_sp.clear();
		_mage_max_sp.clear();
		_mage_cur_sp.clear();
		_spells.clear();
		_create_spell_lists();
		break;
	case CharacterStage::CHOOSE_RACE:
		_race = CharacterRace::NONE;

		_alignment = CharacterAlignment::NONE;

		_start_attr.clear();
		_cur_attr.clear();
		_max_attr.clear();
		//_status.clear();
		_abilities.clear();
		_class = CharacterClass::NONE;
		_points_left = 0;
		_st_points = 0;
		_pos_classes.clear();
		_num_pos_classes = 0;
		_portrait_index = 0;
		_cleric_max_sp.clear();
		_cleric_cur_sp.clear();
		_mage_max_sp.clear();
		_mage_cur_sp.clear();
		_spells.clear();
		_create_spell_lists();
		break;
	case CharacterStage::CHOOSE_ALIGNMENT:
		_alignment = CharacterAlignment::NONE;

		_start_attr.clear();
		_cur_attr.clear();
		_max_attr.clear();
		//_status.clear();
		_abilities.clear();
		_class = CharacterClass::NONE;

		_points_left = 0;
		_st_points = 0;
		_pos_classes.clear();
		_num_pos_classes = 0;
		_portrait_index = 0;
		_cleric_max_sp.clear();
		_cleric_cur_sp.clear();
		_mage_max_sp.clear();
		_mage_cur_sp.clear();
		_spells.clear();
		_create_spell_lists();
		break;
	case CharacterStage::ALLOCATE_STATS:
		_start_attr.clear();
		_cur_attr.clear();
		_max_attr.clear();
		//_status.clear();
		_abilities.clear();
		_class = CharacterClass::NONE;
		_points_left = 0;
		_st_points = 0;
		_pos_classes.clear();
		_num_pos_classes = 0;
		_portrait_index = 0;
		_cleric_max_sp.clear();
		_cleric_cur_sp.clear();
		_mage_max_sp.clear();
		_mage_cur_sp.clear();
		_spells.clear();
		_create_spell_lists();

	default:
		break;
	}

	switch (stage) {
	case CharacterStage::ENTER_NAME:
		_display.generate_components("character_create_stage_1", sprites, texts, frames);
		_display.window->input_mode = WindowInputMode::INPUT_TEXT;
		break;
	case CharacterStage::CHOOSE_RACE:
		_display.generate_components("character_create_stage_2", sprites, texts, frames);
		_display.window->input_mode = WindowInputMode::NORMAL;
		break;
	case CharacterStage::CHOOSE_ALIGNMENT:
		_display.generate_components("character_create_stage_3", sprites, texts, frames);
		_display.window->input_mode = WindowInputMode::NORMAL;
		break;
	case CharacterStage::ALLOCATE_STATS:
		_display.generate_components("character_create_stage_4", sprites, texts, frames);
		_display.window->input_mode = WindowInputMode::ALLOCATE_STATS;
		break;
	default:
		break;
	}
}

auto Sorcery::Character::get_name() const -> std::string {

	return _name;
}

auto Sorcery::Character::set_name(const std::string &value) -> void {

	_name = value;
}

auto Sorcery::Character::get_race() const -> CharacterRace {

	return _race;
}

auto Sorcery::Character::set_race(const CharacterRace &value) -> void {

	_race = value;
}

auto Sorcery::Character::get_alignment() const -> CharacterAlignment {

	return _alignment;
}
auto Sorcery::Character::set_alignment(const CharacterAlignment &value) -> void {

	_alignment = value;
}

auto Sorcery::Character::get_bonus_points_to_allocate() const -> unsigned int {

	return _points_left;
}

auto Sorcery::Character::set_bonus_points_to_allocate(const unsigned int &value) -> void {

	_points_left = value;
}

auto Sorcery::Character::get_starting_bonus_points() const -> unsigned int {

	return _st_points;
}
auto Sorcery::Character::set_starting_bonus_points(const unsigned int &value) -> void {

	_st_points = value;
}

auto Sorcery::Character::set_starting_attributes() -> void {

	_start_attr.clear();
	_cur_attr.clear();
	switch (_race) {
	case CharacterRace::HUMAN:
		_start_attr = {{CharacterAttribute::STRENGTH, 8}, {CharacterAttribute::IQ, 5},
			{CharacterAttribute::PIETY, 5}, {CharacterAttribute::VITALITY, 8},
			{CharacterAttribute::AGILITY, 8}, {CharacterAttribute::LUCK, 9}};
		break;
	case CharacterRace::ELF:
		_start_attr = {{CharacterAttribute::STRENGTH, 7}, {CharacterAttribute::IQ, 10},
			{CharacterAttribute::PIETY, 10}, {CharacterAttribute::VITALITY, 6},
			{CharacterAttribute::AGILITY, 9}, {CharacterAttribute::LUCK, 6}};
		break;
	case CharacterRace::DWARF:
		_start_attr = {{CharacterAttribute::STRENGTH, 10}, {CharacterAttribute::IQ, 7},
			{CharacterAttribute::PIETY, 10}, {CharacterAttribute::VITALITY, 10},
			{CharacterAttribute::AGILITY, 5}, {CharacterAttribute::LUCK, 6}};
		break;
	case CharacterRace::GNOME:
		_start_attr = {{CharacterAttribute::STRENGTH, 7}, {CharacterAttribute::IQ, 7},
			{CharacterAttribute::PIETY, 10}, {CharacterAttribute::VITALITY, 8},
			{CharacterAttribute::AGILITY, 10}, {CharacterAttribute::LUCK, 7}};
		break;
	case CharacterRace::HOBBIT:
		_start_attr = {{CharacterAttribute::STRENGTH, 5}, {CharacterAttribute::IQ, 7},
			{CharacterAttribute::PIETY, 7}, {CharacterAttribute::VITALITY, 6},
			{CharacterAttribute::AGILITY, 10}, {CharacterAttribute::LUCK, 12}};
		break;
	default:
		break;
	};
	_cur_attr = _start_attr;

	// Formula soured from http://www.zimlab.com/wizardry/walk/w123calc.htm
	_points_left = {(*_system.random)[RandomType::ZERO_TO_3]};
	const bool chance_of_more = {(*_system.random)[RandomType::D10] == 1};
	const bool chance_of_more_again = {(*_system.random)[RandomType::D10] == 1};
	_points_left += 7;
	if (_points_left < 20)
		if (chance_of_more)
			_points_left += 10;
	if (_points_left < 20)
		if (chance_of_more_again)
			_points_left += 10;
	_st_points = _points_left;
}

auto Sorcery::Character::get_current_attributes() const -> CharacterAttributes {

	return _cur_attr;
}

auto Sorcery::Character::get_starting_attributes() const -> CharacterAttributes {

	return _start_attr;
}

// Given a character's current stats and alignment, work out what classes are available
auto Sorcery::Character::set_possible_classes() -> void {

	_pos_classes.clear();

	// Do the basic classes first (this also sets _num_possible_character_classes); data is from
	// https://strategywiki.org/wiki/Wizardry:_Proving_Grounds_of_the_Mad_Overlord/Trebor%27s_castle#Classes
	if (_cur_attr[CharacterAttribute::STRENGTH] >= 11)
		_pos_classes[CharacterClass::FIGHTER] = true;
	else
		_pos_classes[CharacterClass::FIGHTER] = false;

	if (_cur_attr[CharacterAttribute::IQ] >= 11)
		_pos_classes[CharacterClass::MAGE] = true;
	else
		_pos_classes[CharacterClass::MAGE] = false;

	if (_cur_attr[CharacterAttribute::PIETY] >= 11)
		if (_alignment == CharacterAlignment::GOOD || _alignment == CharacterAlignment::EVIL)
			_pos_classes[CharacterClass::PRIEST] = true;
		else
			_pos_classes[CharacterClass::PRIEST] = false;
	else
		_pos_classes[CharacterClass::PRIEST] = false;

	if (_cur_attr[CharacterAttribute::AGILITY] >= 11)
		if (_alignment == CharacterAlignment::NEUTRAL || _alignment == CharacterAlignment::EVIL)
			_pos_classes[CharacterClass::THIEF] = true;
		else
			_pos_classes[CharacterClass::THIEF] = false;
	else
		_pos_classes[CharacterClass::THIEF] = false;

	// Now the elite classes
	if (_cur_attr[CharacterAttribute::IQ] >= 12 && _cur_attr[CharacterAttribute::PIETY] >= 12)
		if (_alignment == CharacterAlignment::GOOD || _alignment == CharacterAlignment::EVIL)
			_pos_classes[CharacterClass::BISHOP] = true;
		else
			_pos_classes[CharacterClass::BISHOP] = false;
	else
		_pos_classes[CharacterClass::BISHOP] = false;

	if (_cur_attr[CharacterAttribute::STRENGTH] >= 15 && _cur_attr[CharacterAttribute::IQ] >= 11 &&
		_cur_attr[CharacterAttribute::PIETY] >= 10 &&
		_cur_attr[CharacterAttribute::VITALITY] >= 10 &&
		_cur_attr[CharacterAttribute::AGILITY] >= 10)
		if (_alignment == CharacterAlignment::GOOD || _alignment == CharacterAlignment::NEUTRAL)
			_pos_classes[CharacterClass::SAMURAI] = true;
		else
			_pos_classes[CharacterClass::SAMURAI] = false;
	else
		_pos_classes[CharacterClass::SAMURAI] = false;

	if (_cur_attr[CharacterAttribute::STRENGTH] >= 15 && _cur_attr[CharacterAttribute::IQ] >= 12 &&
		_cur_attr[CharacterAttribute::PIETY] >= 12 &&
		_cur_attr[CharacterAttribute::VITALITY] >= 15 &&
		_cur_attr[CharacterAttribute::AGILITY] >= 14 && _cur_attr[CharacterAttribute::LUCK] >= 15)
		if (_alignment == CharacterAlignment::GOOD)
			_pos_classes[CharacterClass::LORD] = true;
		else
			_pos_classes[CharacterClass::LORD] = false;
	else
		_pos_classes[CharacterClass::LORD] = false;

	if (_cur_attr[CharacterAttribute::STRENGTH] >= 17 && _cur_attr[CharacterAttribute::IQ] >= 17 &&
		_cur_attr[CharacterAttribute::PIETY] >= 17 &&
		_cur_attr[CharacterAttribute::VITALITY] >= 17 &&
		_cur_attr[CharacterAttribute::AGILITY] >= 17 && _cur_attr[CharacterAttribute::LUCK] >= 17)
		if (_alignment == CharacterAlignment::GOOD)
			_pos_classes[CharacterClass::NINJA] = true;
		else
			_pos_classes[CharacterClass::NINJA] = false;
	else
		_pos_classes[CharacterClass::NINJA] = false;

	// And workout the number of classes
	_num_pos_classes = std::count_if(_pos_classes.begin(), _pos_classes.end(), [](auto element) {
		return element.second;
	});
}

// Enum to String functions
auto Sorcery::Character::get_alignment(CharacterAlignment character_alignment) const
	-> std::string {

	switch (character_alignment) {
	case CharacterAlignment::GOOD:
		return (*_display.string)["CHARACTER_ALIGNMENT_GOOD"];
		break;
	case CharacterAlignment::NEUTRAL:
		return (*_display.string)["CHARACTER_ALIGNMENT_NEUTRAL"];
		break;
	case CharacterAlignment::EVIL:
		return (*_display.string)["CHARACTER_ALIGNMENT_EVIL"];
		break;
	default:
		return "";
		break;
	}
}

auto Sorcery::Character::get_race(CharacterRace character_race) const -> std::string {

	switch (character_race) {
	case CharacterRace::HUMAN:
		return (*_display.string)["CHARACTER_RACE_HUMAN"];
		break;
	case CharacterRace::ELF:
		return (*_display.string)["CHARACTER_RACE_ELF"];
		break;
	case CharacterRace::DWARF:
		return (*_display.string)["CHARACTER_RACE_DWARF"];
		break;
	case CharacterRace::GNOME:
		return (*_display.string)["CHARACTER_RACE_GNOME"];
		break;
	case CharacterRace::HOBBIT:
		return (*_display.string)["CHARACTER_RACE_HOBBIT"];
		break;
	default:
		return "";
		break;
	}
}

auto Sorcery::Character::get_class(CharacterClass character_class) const -> std::string {

	switch (character_class) {
	case CharacterClass::FIGHTER:
		return (*_display.string)["CHARACTER_CLASS_FIGHTER"];
		break;
	case CharacterClass::MAGE:
		return (*_display.string)["CHARACTER_CLASS_MAGE"];
		break;
	case CharacterClass::PRIEST:
		return (*_display.string)["CHARACTER_CLASS_PRIEST"];
		break;
	case CharacterClass::THIEF:
		return (*_display.string)["CHARACTER_CLASS_THIEF"];
		break;
	case CharacterClass::BISHOP:
		return (*_display.string)["CHARACTER_CLASS_BISHOP"];
		break;
	case CharacterClass::SAMURAI:
		return (*_display.string)["CHARACTER_CLASS_SAMURAI"];
		break;
	case CharacterClass::LORD:
		return (*_display.string)["CHARACTER_CLASS_LORD"];
		break;
	case CharacterClass::NINJA:
		return (*_display.string)["CHARACTER_CLASS_NINJA"];
		break;
	default:
		return "";
		break;
	}
}

// Save from DB
auto Sorcery::Character::_save() -> unsigned int {

	// TODO:
	return 1;
}

// Load from DB
auto Sorcery::Character::_load([[maybe_unused]] unsigned int character_id) -> void {

	// TODO:
}

// Last step of creating new a character
auto Sorcery::Character::finalise() -> void {

	_generate_starting_information();
	_generate_secondary_abilities();
	_set_starting_spells();
	_set_starting_sp();
}

// Work out all the stuff to do with starting a new character
auto Sorcery::Character::_generate_starting_information() -> void {

	_abilities[CharacterAbility::CURRENT_LEVEL] = 1;
	_abilities[CharacterAbility::MAX_LEVEL] = 1;
	_abilities[CharacterAbility::NEGATIVE_LEVEL] = 0;
	_abilities[CharacterAbility::HIT_DICE] = 1;
	_abilities[CharacterAbility::GOLD] = (*_system.random)[RandomType::ZERO_TO_99] + 90;
	_abilities[CharacterAbility::AGE] = (18 * 52) + (*_system.random)[RandomType::ZERO_TO_299];
	_abilities[CharacterAbility::SWIM] = 1;
	_abilities[CharacterAbility::MARKS] = 0;
	_abilities[CharacterAbility::DEATHS] = 0;

	_start_attr = _cur_attr;
	_max_attr = _cur_attr;

	_abilities[CharacterAbility::NEXT_LEVEL_XP] =
		_get_xp_for_level(_abilities[CharacterAbility::CURRENT_LEVEL]);
}

// Given the characters current level, work out all the secondary abilities/stats etc
auto Sorcery::Character::_generate_secondary_abilities() -> void {

	// Formulae used are from here http://www.zimlab.com/wizardry/walk/w123calc.htm and also from
	// https://mirrors.apple2.org.za/ftp.apple.asimov.net/images/games/rpg/wizardry/wizardry_I/Wizardry_i_SourceCode.zip
	const int current_level{_abilities[CharacterAbility::CURRENT_LEVEL]};

	// Bonus Melee to Hit per Attack (num)
	if (_cur_attr[CharacterAttribute::STRENGTH] > 15)
		_abilities[CharacterAbility::ATTACK_MODIFIER] =
			_cur_attr[CharacterAttribute::STRENGTH] - 15;
	else if (_cur_attr[CharacterAttribute::STRENGTH] < 6)
		_abilities[CharacterAbility::ATTACK_MODIFIER] = _cur_attr[CharacterAttribute::STRENGTH] - 6;
	else
		_abilities[CharacterAbility::ATTACK_MODIFIER] = 0;

	// Bonus Melee to Hit per Attack (num)
	switch (_class) {
	case CharacterClass::FIGHTER:
	case CharacterClass::SAMURAI:
	case CharacterClass::LORD:
	case CharacterClass::NINJA:
	case CharacterClass::PRIEST:
		_abilities[CharacterAbility::HIT_PROBABILITY] = 2 + (current_level / 5);
		break;
	default:
		_abilities[CharacterAbility::HIT_PROBABILITY] = current_level / 5;
		break;
	}

	// Bonus Melee Damage per Attack (num)
	if (_cur_attr[CharacterAttribute::STRENGTH] > 15)
		_abilities[CharacterAbility::BONUS_DAMAGE] = _cur_attr[CharacterAttribute::STRENGTH] - 15;
	else if (_cur_attr[CharacterAttribute::STRENGTH] < 6)
		_abilities[CharacterAbility::BONUS_DAMAGE] = _cur_attr[CharacterAttribute::STRENGTH] - 6;
	else
		_abilities[CharacterAbility::BONUS_DAMAGE] = 0;

	// Unarmed Attack Damage (num)
	_abilities[CharacterAbility::UNARMED_DAMAGE] =
		_class == CharacterClass::NINJA ? 8 + _abilities[CharacterAbility::BONUS_DAMAGE]
										: 4 + _abilities[CharacterAbility::BONUS_DAMAGE];

	// Number of Melee Attacks (num)
	switch (_class) {
	case CharacterClass::FIGHTER:
	case CharacterClass::SAMURAI:
	case CharacterClass::LORD:
		_abilities[CharacterAbility::BASE_NUMBER_OF_ATTACKS] = current_level / 5;
		break;
	case CharacterClass::NINJA:
		_abilities[CharacterAbility::BASE_NUMBER_OF_ATTACKS] = (current_level / 5) + 1;
		break;
	default:
		_abilities[CharacterAbility::BASE_NUMBER_OF_ATTACKS] = 1;
		break;
	}

	// Chance of learning new Mage Spells (%)
	switch (_class) {
	case CharacterClass::SAMURAI:
	case CharacterClass::BISHOP:
	case CharacterClass::MAGE:
		_abilities[CharacterAbility::MAGE_SPELL_LEARN] =
			(_cur_attr[CharacterAttribute::IQ] / 29.0) * 100;
		break;
	default:
		_abilities[CharacterAbility::MAGE_SPELL_LEARN] = 0;
		break;
	}

	// Chance of Identifying Items (%)
	_abilities[CharacterAbility::IDENTIFY_ITEMS] =
		_class == CharacterClass::BISHOP ? 10 + (5 * current_level) : 0;
	if (_abilities[CharacterAbility::IDENTIFY_ITEMS] > 100)
		_abilities[CharacterAbility::IDENTIFY_ITEMS] = 100;

	// Chance of getting cursed when Identifying Items (%)
	_abilities[CharacterAbility::IDENTIFY_CURSE] =
		_class == CharacterClass::BISHOP ? 35 - (5 * current_level) : 0;
	if (_abilities[CharacterAbility::IDENTIFY_CURSE] < 0)
		_abilities[CharacterAbility::IDENTIFY_CURSE] = 0;

	// Chance of identifying unknown Foes per round (%)
	_abilities[CharacterAbility::IDENTIFY_FOES] =
		current_level + _cur_attr[CharacterAttribute::IQ] + _cur_attr[CharacterAttribute::PIETY];
	if (_abilities[CharacterAbility::IDENTIFY_FOES] > 100)
		_abilities[CharacterAbility::IDENTIFY_FOES] = 100;

	// Chance of learning new Priest Spells (%)
	switch (_class) {
	case CharacterClass::PRIEST:
	case CharacterClass::LORD:
	case CharacterClass::BISHOP:
		_abilities[CharacterAbility::PRIEST_SPELL_LEARN] =
			(_cur_attr[CharacterAttribute::PIETY] / 30.0) * 100;
		break;
	default:
		_abilities[CharacterAbility::PRIEST_SPELL_LEARN] = 0;
		break;
	}

	// LOKTOFELT success chance (%)
	_abilities[CharacterAbility::LOKTOFELT_SUCCESS] =
		_class == CharacterClass::PRIEST ? 2 * current_level : 0;

	// Base Dispell chance (affected by monster level) (%)
	switch (_class) {
	case CharacterClass::PRIEST:
		_abilities[CharacterAbility::BASE_DISPELL] = 50 + (5 * current_level);
		break;
	case CharacterClass::BISHOP:
		_abilities[CharacterAbility::BASE_DISPELL] =
			current_level >= 4 ? 50 + (5 * current_level) - 20 : 0;
		break;
	case CharacterClass::LORD:
		_abilities[CharacterAbility::BASE_DISPELL] =
			current_level >= 9 ? 50 + (5 * current_level) - 40 : 0;
		break;
	default:
		_abilities[CharacterAbility::BASE_DISPELL] = 0;
		break;
	}
	if (_abilities[CharacterAbility::BASE_DISPELL] > 100)
		_abilities[CharacterAbility::BASE_DISPELL] = 100;

	// Vitality Bonus (num)
	switch (_cur_attr[CharacterAttribute::VITALITY]) {
	case 3:
		_abilities[CharacterAbility::VITALITY_BONUS] = -2;
		break;
	case 4:
	case 5:
		_abilities[CharacterAbility::VITALITY_BONUS] = -1;
		break;
	case 16:
		_abilities[CharacterAbility::VITALITY_BONUS] = 1;
		break;
	case 17:
		_abilities[CharacterAbility::VITALITY_BONUS] = 2;
		break;
	case 18:
	case 19:
	case 20:
		_abilities[CharacterAbility::VITALITY_BONUS] = 3;
		break;
	default:
		_abilities[CharacterAbility::VITALITY_BONUS] = 0;
		break;
	}

	// Bonus Hit Points per level (num)
	_abilities[CharacterAbility::BONUS_HIT_POINTS] = _abilities[CharacterAbility::VITALITY_BONUS];

	// Base Hit Points (num)
	switch (unsigned int chance{(*_system.random)[RandomType::D100]};
			_class) { // NOLINT(clang-diagnostic-switch)
	case CharacterClass::FIGHTER:
	case CharacterClass::LORD:
		_abilities[CharacterAbility::MAX_HP] =
			chance <= 50 ? 10 + _abilities[CharacterAbility::BONUS_HIT_POINTS]
						 : 9 * (10 + _abilities[CharacterAbility::BONUS_HIT_POINTS]) / 10;
		break;
	case CharacterClass::PRIEST:
		_abilities[CharacterAbility::MAX_HP] =
			chance <= 50 ? 8 + _abilities[CharacterAbility::BONUS_HIT_POINTS]
						 : 8 * (10 + _abilities[CharacterAbility::BONUS_HIT_POINTS]) / 10;
		break;
	case CharacterClass::THIEF:
	case CharacterClass::BISHOP:
	case CharacterClass::NINJA:
		_abilities[CharacterAbility::MAX_HP] =
			chance <= 50 ? 6 + _abilities[CharacterAbility::BONUS_HIT_POINTS]
						 : 6 * (10 + _abilities[CharacterAbility::BONUS_HIT_POINTS]) / 10;
		break;
	case CharacterClass::MAGE:
		_abilities[CharacterAbility::MAX_HP] =
			chance <= 50 ? 4 + _abilities[CharacterAbility::BONUS_HIT_POINTS]
						 : 4 * (10 + _abilities[CharacterAbility::BONUS_HIT_POINTS]) / 10;
		break;
	case CharacterClass::SAMURAI:
		_abilities[CharacterAbility::MAX_HP] =
			chance <= 50 ? 16 + _abilities[CharacterAbility::BONUS_HIT_POINTS]
						 : 16 * (10 + _abilities[CharacterAbility::BONUS_HIT_POINTS]) / 10;
		break;
	default:
		break;
	}
	if (_abilities[CharacterAbility::MAX_HP] < 1)
		_abilities[CharacterAbility::MAX_HP] = 1;
	_abilities[CharacterAbility::CURRENT_HP] = _abilities[CharacterAbility::MAX_HP];

	// Chance of resurrecting a Dead Character at the Temple (%)
	_abilities[CharacterAbility::DEAD_RESURRECT] =
		50 + (3 * _cur_attr[CharacterAttribute::VITALITY]);
	if (_abilities[CharacterAbility::DEAD_RESURRECT] > 100)
		_abilities[CharacterAbility::DEAD_RESURRECT] = 100;

	// Chance of resurrecting an Ashed Character at the Temple (%)
	_abilities[CharacterAbility::ASHES_RESURRECT] =
		40 + (3 * _cur_attr[CharacterAttribute::VITALITY]);
	if (_abilities[CharacterAbility::ASHES_RESURRECT] > 100)
		_abilities[CharacterAbility::ASHES_RESURRECT] = 100;

	// Chance of resurrecting by a DI or KADORTO spell cast by another Character (%)
	_abilities[CharacterAbility::DI_KADORTO_RESURRECT] =
		4 * _cur_attr[CharacterAttribute::VITALITY];

	// Initiative Modifier (num)
	switch (_cur_attr[CharacterAttribute::AGILITY]) {
	case 3:
		_abilities[CharacterAbility::INITIATIVE_MODIFIER] = 3;
		break;
	case 4:
	case 5:
		_abilities[CharacterAbility::INITIATIVE_MODIFIER] = 2;
		break;
	case 6:
	case 7:
		_abilities[CharacterAbility::INITIATIVE_MODIFIER] = 1;
		break;
	case 15:
		_abilities[CharacterAbility::INITIATIVE_MODIFIER] = -1;
		break;
	case 16:
		_abilities[CharacterAbility::INITIATIVE_MODIFIER] = -2;
		break;
	case 17:
		_abilities[CharacterAbility::INITIATIVE_MODIFIER] = -3;
		break;
	case 18:
	case 19:
	case 20:
		_abilities[CharacterAbility::INITIATIVE_MODIFIER] = -4;
		break;
	default:
		_abilities[CharacterAbility::INITIATIVE_MODIFIER] = 0;
		break;
	}

	// Armour Class (num) (before equipment)
	_abilities[CharacterAbility::BASE_ARMOUR_CLASS] =
		_class == CharacterClass::NINJA ? 10 - (current_level / 3) - 2 : 10;
	_abilities[CharacterAbility::CURRENT_ARMOUR_CLASS] =
		_abilities[CharacterAbility::BASE_ARMOUR_CLASS];

	// Critical Hit Chance (%)
	_abilities[CharacterAbility::BASE_CRITICAL_HIT] =
		_class == CharacterClass::NINJA ? 2 * current_level : 0;
	if (_abilities[CharacterAbility::BASE_CRITICAL_HIT] > 50)
		_abilities[CharacterAbility::BASE_CRITICAL_HIT] = 50;

	// Chance to identify a Trap (%)
	if (_class == CharacterClass::THIEF)
		_abilities[CharacterAbility::IDENTIFY_TRAP] = 6 * _cur_attr[CharacterAttribute::AGILITY];
	else if (_class == CharacterClass::NINJA)
		_abilities[CharacterAbility::IDENTIFY_TRAP] = 4 * _cur_attr[CharacterAttribute::AGILITY];
	else
		_abilities[CharacterAbility::IDENTIFY_TRAP] = 0;
	if (_abilities[CharacterAbility::IDENTIFY_TRAP] > 95)
		_abilities[CharacterAbility::IDENTIFY_TRAP] = 95;

	// Base Chance to Disarm a Trap (modified by Maze Level) (%)
	_abilities[CharacterAbility::BASE_DISARM_TRAP] =
		(_class == CharacterClass::NINJA) || (_class == CharacterClass::THIEF)
			? ((current_level + 50) / 69.0) * 100
			: 0;

	// Chance to activate a Trap if identify fails (%)
	_abilities[CharacterAbility::ACTIVATE_TRAP] =
		(_class == CharacterClass::NINJA) || (_class == CharacterClass::THIEF)
			? 100 - ((_cur_attr[CharacterAttribute::AGILITY] / 20.0) * 100)
			: 100;

	// Base Chance to avoid following into a Pit (modified by Maze Level) (%)
	_abilities[CharacterAbility::BASE_AVOID_PIT] =
		(_cur_attr[CharacterAttribute::AGILITY] / 25.0) * 100;

	// Base Resist Bonus (d20)
	_abilities[CharacterAbility::BASE_RESIST_BONUS] = 1 * (current_level / 5);
	if (_cur_attr[CharacterAttribute::LUCK] >= 18)
		_abilities[CharacterAbility::BASE_RESIST_BONUS] += 3;
	else if (_cur_attr[CharacterAttribute::LUCK] >= 12)
		_abilities[CharacterAbility::BASE_RESIST_BONUS] += 2;
	else if (_cur_attr[CharacterAttribute::LUCK] >= 6)
		_abilities[CharacterAbility::BASE_RESIST_BONUS] += 1;

	// Other Resists (d20)
	switch (_class) { // NOLINT(clang-diagnostic-switch)
	case CharacterClass::SAMURAI:
		_abilities[CharacterAbility::RESISTANCE_VS_POISON_PARALYSIS] =
			_abilities[CharacterAbility::BASE_RESIST_BONUS] + 2;
		_abilities[CharacterAbility::RESISTANCE_VS_CRITICAL_HIT] =
			_abilities[CharacterAbility::BASE_RESIST_BONUS] + 2;
		_abilities[CharacterAbility::RESISTANCE_VS_STONING] = 0;
		_abilities[CharacterAbility::RESISTANCE_VS_BREATH_ATTACKS] = 0;
		_abilities[CharacterAbility::RESISTANCE_VS_POISON_GAS_TRAP] = 0;
		_abilities[CharacterAbility::RESISTANCE_VS_MAGE_PRIEST_TRAP] =
			_abilities[CharacterAbility::BASE_RESIST_BONUS] + 2;
		_abilities[CharacterAbility::RESISTANCE_VS_SILENCE] =
			_abilities[CharacterAbility::BASE_RESIST_BONUS] + 2;
		break;
	case CharacterClass::FIGHTER:
		_abilities[CharacterAbility::RESISTANCE_VS_POISON_PARALYSIS] =
			_abilities[CharacterAbility::BASE_RESIST_BONUS] + 3;
		_abilities[CharacterAbility::RESISTANCE_VS_CRITICAL_HIT] =
			_abilities[CharacterAbility::BASE_RESIST_BONUS] + 3;
		_abilities[CharacterAbility::RESISTANCE_VS_STONING] = 0;
		_abilities[CharacterAbility::RESISTANCE_VS_BREATH_ATTACKS] = 0;
		_abilities[CharacterAbility::RESISTANCE_VS_POISON_GAS_TRAP] = 0;
		_abilities[CharacterAbility::RESISTANCE_VS_MAGE_PRIEST_TRAP] = 0;
		_abilities[CharacterAbility::RESISTANCE_VS_SILENCE] = 0;
		break;
	case CharacterClass::PRIEST:
		_abilities[CharacterAbility::RESISTANCE_VS_POISON_PARALYSIS] = 0;
		_abilities[CharacterAbility::RESISTANCE_VS_CRITICAL_HIT] = 0;
		_abilities[CharacterAbility::RESISTANCE_VS_STONING] =
			_abilities[CharacterAbility::BASE_RESIST_BONUS] + 3;
		_abilities[CharacterAbility::RESISTANCE_VS_BREATH_ATTACKS] = 0;
		_abilities[CharacterAbility::RESISTANCE_VS_POISON_GAS_TRAP] = 0;
		_abilities[CharacterAbility::RESISTANCE_VS_MAGE_PRIEST_TRAP] = 0;
		_abilities[CharacterAbility::RESISTANCE_VS_SILENCE] = 0;
		break;
	case CharacterClass::LORD:
		_abilities[CharacterAbility::RESISTANCE_VS_POISON_PARALYSIS] =
			_abilities[CharacterAbility::BASE_RESIST_BONUS] + 2;
		_abilities[CharacterAbility::RESISTANCE_VS_CRITICAL_HIT] =
			_abilities[CharacterAbility::BASE_RESIST_BONUS] + 2;
		_abilities[CharacterAbility::RESISTANCE_VS_STONING] =
			_abilities[CharacterAbility::BASE_RESIST_BONUS] + 2;
		_abilities[CharacterAbility::RESISTANCE_VS_BREATH_ATTACKS] = 0;
		_abilities[CharacterAbility::RESISTANCE_VS_POISON_GAS_TRAP] = 0;
		_abilities[CharacterAbility::RESISTANCE_VS_MAGE_PRIEST_TRAP] = 0;
		_abilities[CharacterAbility::RESISTANCE_VS_SILENCE] = 0;
		break;
	case CharacterClass::BISHOP:
		_abilities[CharacterAbility::RESISTANCE_VS_POISON_PARALYSIS] = 0;
		_abilities[CharacterAbility::RESISTANCE_VS_CRITICAL_HIT] = 0;
		_abilities[CharacterAbility::RESISTANCE_VS_STONING] =
			_abilities[CharacterAbility::BASE_RESIST_BONUS] + 2;
		_abilities[CharacterAbility::RESISTANCE_VS_BREATH_ATTACKS] =
			_abilities[CharacterAbility::BASE_RESIST_BONUS] + 2;
		_abilities[CharacterAbility::RESISTANCE_VS_POISON_GAS_TRAP] = 0;
		_abilities[CharacterAbility::RESISTANCE_VS_MAGE_PRIEST_TRAP] =
			_abilities[CharacterAbility::BASE_RESIST_BONUS] + 2;
		_abilities[CharacterAbility::RESISTANCE_VS_SILENCE] =
			_abilities[CharacterAbility::BASE_RESIST_BONUS] + 2;
		break;
	case CharacterClass::THIEF:
		_abilities[CharacterAbility::RESISTANCE_VS_POISON_PARALYSIS] = 0;
		_abilities[CharacterAbility::RESISTANCE_VS_CRITICAL_HIT] = 0;
		_abilities[CharacterAbility::RESISTANCE_VS_STONING] = 0;
		_abilities[CharacterAbility::RESISTANCE_VS_BREATH_ATTACKS] = 0;
		_abilities[CharacterAbility::RESISTANCE_VS_POISON_GAS_TRAP] =
			_abilities[CharacterAbility::BASE_RESIST_BONUS] + 3;
		_abilities[CharacterAbility::RESISTANCE_VS_MAGE_PRIEST_TRAP] = 0;
		_abilities[CharacterAbility::RESISTANCE_VS_SILENCE] = 0;
		break;
	case CharacterClass::NINJA:
		_abilities[CharacterAbility::RESISTANCE_VS_POISON_PARALYSIS] =
			_abilities[CharacterAbility::BASE_RESIST_BONUS] + 3;
		_abilities[CharacterAbility::RESISTANCE_VS_CRITICAL_HIT] =
			_abilities[CharacterAbility::BASE_RESIST_BONUS] + 3;
		_abilities[CharacterAbility::RESISTANCE_VS_STONING] =
			_abilities[CharacterAbility::BASE_RESIST_BONUS] + 2;
		_abilities[CharacterAbility::RESISTANCE_VS_BREATH_ATTACKS] =
			_abilities[CharacterAbility::BASE_RESIST_BONUS] + 4;
		_abilities[CharacterAbility::RESISTANCE_VS_POISON_GAS_TRAP] =
			_abilities[CharacterAbility::BASE_RESIST_BONUS] + 3;
		_abilities[CharacterAbility::RESISTANCE_VS_MAGE_PRIEST_TRAP] =
			_abilities[CharacterAbility::BASE_RESIST_BONUS] + 2;
		_abilities[CharacterAbility::RESISTANCE_VS_SILENCE] =
			_abilities[CharacterAbility::BASE_RESIST_BONUS] + 2;
		break;
	case CharacterClass::MAGE:
		_abilities[CharacterAbility::RESISTANCE_VS_POISON_PARALYSIS] = 0;
		_abilities[CharacterAbility::RESISTANCE_VS_CRITICAL_HIT] = 0;
		_abilities[CharacterAbility::RESISTANCE_VS_STONING] = 0;
		_abilities[CharacterAbility::RESISTANCE_VS_BREATH_ATTACKS] = 0;
		_abilities[CharacterAbility::RESISTANCE_VS_POISON_GAS_TRAP] = 0;
		_abilities[CharacterAbility::RESISTANCE_VS_MAGE_PRIEST_TRAP] =
			_abilities[CharacterAbility::BASE_RESIST_BONUS] + 3;
		_abilities[CharacterAbility::RESISTANCE_VS_SILENCE] =
			_abilities[CharacterAbility::BASE_RESIST_BONUS] + 3;
		break;
	default:
		break;
	}
	if (_race == CharacterRace::HUMAN)
		_abilities[CharacterAbility::RESISTANCE_VS_POISON_PARALYSIS] += 1;
	if (_race == CharacterRace::HUMAN)
		_abilities[CharacterAbility::RESISTANCE_VS_CRITICAL_HIT] += 1;
	if (_race == CharacterRace::GNOME)
		_abilities[CharacterAbility::RESISTANCE_VS_STONING] += 2;
	if (_race == CharacterRace::ELF)
		_abilities[CharacterAbility::RESISTANCE_VS_BREATH_ATTACKS] += 2;
	if (_race == CharacterRace::DWARF)
		_abilities[CharacterAbility::RESISTANCE_VS_POISON_GAS_TRAP] += 4;
	if (_race == CharacterRace::HOBBIT) {
		_abilities[CharacterAbility::RESISTANCE_VS_MAGE_PRIEST_TRAP] += 2;
		_abilities[CharacterAbility::RESISTANCE_VS_SILENCE] += 3;
	}
	if (_abilities[CharacterAbility::RESISTANCE_VS_POISON_PARALYSIS] > 19)
		_abilities[CharacterAbility::RESISTANCE_VS_POISON_PARALYSIS] = 19;
	if (_abilities[CharacterAbility::RESISTANCE_VS_CRITICAL_HIT] > 19)
		_abilities[CharacterAbility::RESISTANCE_VS_CRITICAL_HIT] = 19;
	if (_abilities[CharacterAbility::RESISTANCE_VS_STONING] > 19)
		_abilities[CharacterAbility::RESISTANCE_VS_STONING] = 19;
	if (_abilities[CharacterAbility::RESISTANCE_VS_BREATH_ATTACKS] > 19)
		_abilities[CharacterAbility::RESISTANCE_VS_BREATH_ATTACKS] = 19;
	if (_abilities[CharacterAbility::RESISTANCE_VS_POISON_GAS_TRAP] > 19)
		_abilities[CharacterAbility::RESISTANCE_VS_POISON_GAS_TRAP] = 19;
	if (_abilities[CharacterAbility::RESISTANCE_VS_MAGE_PRIEST_TRAP] > 19)
		_abilities[CharacterAbility::RESISTANCE_VS_MAGE_PRIEST_TRAP] = 19;
	if (_abilities[CharacterAbility::RESISTANCE_VS_SILENCE] > 19)
		_abilities[CharacterAbility::RESISTANCE_VS_SILENCE] = 19;

	// Resistance to harmful spells (%)
	_abilities[CharacterAbility::RESISTANCE_VS_KATINO] = current_level * 10;
	if (_abilities[CharacterAbility::RESISTANCE_VS_KATINO] > 100)
		_abilities[CharacterAbility::RESISTANCE_VS_KATINO] = 100;
	_abilities[CharacterAbility::RESISTANCE_VS_BADI] = current_level * 10;
	if (_abilities[CharacterAbility::RESISTANCE_VS_BADI] > 100)
		_abilities[CharacterAbility::RESISTANCE_VS_BADI] = 100;
	_abilities[CharacterAbility::RESISTANCE_VS_MANIFO] = 50 + (current_level * 10);
	if (_abilities[CharacterAbility::RESISTANCE_VS_MANIFO] > 100)
		_abilities[CharacterAbility::RESISTANCE_VS_MANIFO] = 100;

	// If we are not in strict mode, add some bonus spellpoints to first level priests/,ages
	// depending on their associated stats
	_abilities[CharacterAbility::BONUS_MAGE_SPELLS] = 0;
	_abilities[CharacterAbility::BONUS_PRIEST_SPELLS] = 0;
	if (!(*_system.config)[ConfigOption::STRICT_MODE]) {
		switch (_class) { // NOLINT(clang-diagnostic-switch)
		case CharacterClass::PRIEST:
			switch (_cur_attr[CharacterAttribute::PIETY]) {
			case 16:
				_abilities[CharacterAbility::BONUS_PRIEST_SPELLS] = 1;
				break;
			case 17:
				_abilities[CharacterAbility::BONUS_PRIEST_SPELLS] = 2;
				break;
			case 18:
			case 19:
			case 20:
				_abilities[CharacterAbility::BONUS_PRIEST_SPELLS] = 3;
				break;
			default:
				break;
			}
			break;
		case CharacterClass::MAGE:
			switch (_cur_attr[CharacterAttribute::IQ]) {
			case 16:
				_abilities[CharacterAbility::BONUS_MAGE_SPELLS] = 1;
				break;
			case 17:
				_abilities[CharacterAbility::BONUS_MAGE_SPELLS] = 2;
				break;
			case 18:
			case 19:
			case 20:
				_abilities[CharacterAbility::BONUS_MAGE_SPELLS] = 3;
				break;
			default:
				break;
			}
			break;
		default:
			break;
		}
	}
}

// Set the starting spellpoints
auto Sorcery::Character::_set_starting_sp() -> void {

	// By default clear all spells
	_clear_sp();

	// In the original code this is handled in "SETSPELS"/"SPLPERLV"/"NWMAGE"/"NWPRIEST" - but we
	// are just setting them straight for now but adding in extra slots for casters to make things
	// easier if we're not in strict mode
	switch (_class) { // NOLINT(clang-diagnostic-switch)
	case CharacterClass::PRIEST:
		_cleric_max_sp[1] = (*_system.config)[ConfigOption::STRICT_MODE]
								? 2
								: 2 + _abilities[CharacterAbility::BONUS_PRIEST_SPELLS];
		break;
	case CharacterClass::BISHOP:
		_mage_max_sp[1] = 2;
		break;
	case CharacterClass::MAGE:
		_mage_max_sp[1] = (*_system.config)[ConfigOption::STRICT_MODE]
							  ? 2
							  : 2 + _abilities[CharacterAbility::BONUS_MAGE_SPELLS];
		break;
	default:
		break;
	}

	_cleric_cur_sp[1] = _cleric_max_sp[1];
	_mage_cur_sp[1] = _mage_max_sp[1];
}

// Clear all spell-points to begin with
auto Sorcery::Character::_clear_sp() -> void {

	for (auto spell_level = 1; spell_level <= 7; spell_level++) {
		_cleric_max_sp[spell_level] = 0;
		_cleric_cur_sp[spell_level] = 0;
		_mage_max_sp[spell_level] = 0;
		_mage_cur_sp[spell_level] = 0;
	}
}

// Set starting spells
auto Sorcery::Character::_set_starting_spells() -> void {

	// TODO: see if get can be replaced bv named parameter (i.e. use struct not tuple?)

	// This is taken from "KEEPCHYN" which hard codes the spells known to beginning characters!
	std::vector<SpellEntry>::iterator it;
	switch (_class) { // NOLINT(clang-diagnostic-switch)
	case CharacterClass::BISHOP:
		it = std::find_if(_spells.begin(), _spells.end(), [=](auto item) {
			return std::get<0>(item) == Spell::KATINO;
		});
		if (it != _spells.end())
			std::get<4>(*it) = true;
		it = std::find_if(_spells.begin(), _spells.end(), [=](auto item) {
			return std::get<0>(item) == Spell::HALITO;
		});
		if (it != _spells.end())
			std::get<4>(*it) = true;
		break;
	case CharacterClass::MAGE:
		it = std::find_if(_spells.begin(), _spells.end(), [=](auto item) {
			return std::get<0>(item) == Spell::DUMAPIC;
		});
		if (it != _spells.end())
			std::get<4>(*it) = true;
		it = std::find_if(_spells.begin(), _spells.end(), [=](auto item) {
			return std::get<0>(item) == Spell::MOGREF;
		});
		if (it != _spells.end())
			std::get<4>(*it) = true;
		break;
	case CharacterClass::PRIEST:
		it = std::find_if(_spells.begin(), _spells.end(), [=](auto item) {
			return std::get<0>(item) == Spell::DIOS;
		});
		if (it != _spells.end())
			std::get<4>(*it) = true;
		it = std::find_if(_spells.begin(), _spells.end(), [=](auto item) {
			return std::get<0>(item) == Spell::BADIOS;
		});
		if (it != _spells.end())
			std::get<4>(*it) = true;
		break;
	default:
		break;
	}
}

// Level Gain/Loss Functions

// Get HP gained for all levels apart from the first
auto Sorcery::Character::_get_hp_gained_per_level() -> int {

	// In the original code ("MOREHP"), Samurai get 2d8
	int extra_hp{0};
	switch (_class) { // NOLINT(clang-diagnostic-switch)
	case CharacterClass::FIGHTER:
	case CharacterClass::LORD:
		extra_hp += (*_system.random)[RandomType::D10];
		break;
	case CharacterClass::PRIEST:
		extra_hp += (*_system.random)[RandomType::D8];
		break;
	case CharacterClass::THIEF:
	case CharacterClass::BISHOP:
	case CharacterClass::NINJA:
		extra_hp += (*_system.random)[RandomType::D6];
		break;
	case CharacterClass::MAGE:
		extra_hp += (*_system.random)[RandomType::D4];
		break;
	case CharacterClass::SAMURAI:
		extra_hp += (*_system.random)[RandomType::D8];
		extra_hp += (*_system.random)[RandomType::D8];
		break;
	default:
		break;
	}

	extra_hp += _abilities[CharacterAbility::BONUS_HIT_POINTS];
	if (extra_hp < 0)
		extra_hp = 1;

	// Though this could be an unsigned int as it will always be greater than 0, just return int as
	// everything else is
	return extra_hp;
}

// Add hit points on level gain (but note the strict mode limitation mentioned below)
auto Sorcery::Character::_update_hp_for_level() -> void {

	// Note the annoying thing in the original Wizardry ("MADELEV") and reproduced here in strict
	// mode where it recalculates all HP and thus often you end up with gaining only one HP - this
	// also reproduces the equally annoying thing where if you have changed class it uses your
	// *current* class level for recalculation, hence until you get back to where you were before
	// changing class you will probably only ever gain 1 hp each time unless the random dice rolls
	// are really in your favour!
	if ((*_system.config)[ConfigOption::REROLL_HIT_POINTS_ON_LEVEL_GAIN]) {
		int hp_total{0};
		for (auto level = 1; level < _abilities[CharacterAbility::CURRENT_LEVEL]; level++)
			hp_total += _get_hp_gained_per_level();
		if (hp_total < _abilities[CharacterAbility::MAX_HP])
			hp_total = _abilities[CharacterAbility::MAX_HP] + 1;
		int hp_gained{hp_total - _abilities[CharacterAbility::MAX_HP]};
		_abilities[CharacterAbility::MAX_HP] += hp_gained;
		_abilities[CharacterAbility::CURRENT_HP] += hp_gained;
	} else {
		int hp_gained{_get_hp_gained_per_level()};
		_abilities[CharacterAbility::MAX_HP] += hp_gained;
		_abilities[CharacterAbility::CURRENT_HP] += hp_gained;
	}
}

// Level a character up
auto Sorcery::Character::level_up() -> void {

	// For now
}

// Level a character down (e.g. drain levels or give/increase negative levels_
auto Sorcery::Character::level_down() -> void {

	// For now
}

// For each spell level, try to learn spells - called before set_spellpoints
auto Sorcery::Character::_try_to_learn_spells(SpellType spell_type, unsigned int spell_level)
	-> void {

	// Only do spells if a character can learn them
	if (spell_type == SpellType::PRIEST)
		if (_cleric_max_sp[spell_level] == 0)
			return;
	if (spell_type == SpellType::MAGE)
		if (_mage_max_sp[spell_level] == 0)
			return;

	// First, get the spells themselves
	std::vector<SpellEntry>::iterator it;
	it = std::find_if(_spells.begin(), _spells.end(), [=](auto item) {
		return std::get<1>(item) == spell_type && std::get<2>(item) == spell_level;
	});
	while (it != _spells.end()) {
		it = std::find_if(++it, _spells.end(), [=](auto item) {
			return std::get<1>(item) == spell_type && std::get<2>(item) == spell_level;
		});

		// TODO: same comment about struct instead of tuple

		// If spell already known, skip
		if (std::get<4>(*it))
			continue;

		// Check the Spell Type against the relevant stat (see SPLPERLV//TRYLEARN)
		if (spell_type == SpellType::PRIEST)
			if ((*_system.random)[RandomType::ZERO_TO_29] <=
				static_cast<unsigned int>(_cur_attr[CharacterAttribute::PIETY]))
				std::get<4>(*it) = true;
		if (spell_type == SpellType::MAGE)
			if ((*_system.random)[RandomType::ZERO_TO_29] <=
				static_cast<unsigned int>(_cur_attr[CharacterAttribute::IQ]))
				std::get<4>(*it) = true;
	}
}

// Reimplementation of SPLPERLV
auto Sorcery::Character::_calculate_sp(
	SpellType spell_type, unsigned int level_mod, unsigned int level_offset) -> void {

	// No ownership granted by use of raw pointer here - is the only reason we use it!
	SpellPoints *spells{spell_type == SpellType::PRIEST ? &_cleric_max_sp : &_mage_max_sp};

	int spell_count{static_cast<int>(_abilities[CharacterAbility::CURRENT_LEVEL] - level_mod)};
	if (spell_count <= 0)
		return;

	unsigned int spell_level = {1};
	while (spell_level >= 1 && spell_level <= 7 && spell_count > 0) {
		if (static_cast<unsigned int>(spell_count) > (*spells)[spell_level])
			(*spells)[spell_level] = spell_count;
		++spell_level;
		spell_count -= level_offset;
	}
	for (spell_level = 1; spell_level <= 7; spell_level++)
		if ((*spells)[spell_level] > 9)
			(*spells)[spell_level] = 9;
}

// Copied and rewritten from the original code from MINMAG/MINPRI/NWPRIEST/NWMAGE
auto Sorcery::Character::_set_sp() -> void {

	// First work out the number of spells known at each level (not this deliberately does not alter
	// spells learned in a previous class to allow those to remain the same (see MINMAG/MINPRI in
	// the code)
	for (auto spell_level = 1; spell_level <= 7; spell_level++) {
		_cleric_max_sp[spell_level] = _get_spells_known(SpellType::PRIEST, spell_level);
		_mage_max_sp[spell_level] = _get_spells_known(SpellType::MAGE, spell_level);
	}

	// Then bump up spells according to level
	switch (_class) { // NOLINT(clang-diagnostic-switch)
	case CharacterClass::FIGHTER:
	case CharacterClass::THIEF:
	case CharacterClass::NINJA:
		return;
		break;
	case CharacterClass::MAGE:
		_calculate_sp(SpellType::MAGE, 0, 2);
		break;
	case CharacterClass::PRIEST:
		_calculate_sp(SpellType::PRIEST, 0, 2);
		break;
	case CharacterClass::BISHOP:
		_calculate_sp(SpellType::PRIEST, 3, 4);
		_calculate_sp(SpellType::MAGE, 0, 4);
		break;
	case CharacterClass::SAMURAI:
		_calculate_sp(SpellType::MAGE, 3, 3);
		break;
	case CharacterClass::LORD:
		_calculate_sp(SpellType::PRIEST, 3, 2);
		break;
	default:
		break;
	}
}

// In the original code this is from SPLPERLV
auto Sorcery::Character::_get_spells_known(SpellType spell_type, unsigned int spell_level)
	-> unsigned int {

	unsigned int spells_known = {0};
	std::vector<SpellEntry>::iterator it;
	it = std::find_if(_spells.begin(), _spells.end(), [=](auto item) {
		return std::get<1>(item) == spell_type && std::get<2>(item) == spell_level;
	});
	while (it != _spells.end()) {
		it = std::find_if(++it, _spells.end(), [=](auto item) {
			return std::get<1>(item) == spell_type && std::get<2>(item) == spell_level;
		});

		if (std::get<4>(*it))
			++spells_known;
	}

	return spells_known;
}

// Given a level, get the XP needed for it
auto Sorcery::Character::_get_xp_for_level(unsigned int level) const -> int {

	// Values obtained from http://www.the-spoiler.com/RPG/Sir-Tech/wizardry.1.2.html
	float base = {};
	float coefficient_2_to_3 = {};
	float coefficient_3_to_13 = {};
	float coefficient_13_plus = {};
	switch (_class) { // NOLINT(clang-diagnostic-switch)
	case CharacterClass::FIGHTER:
		base = 1000;
		coefficient_2_to_3 = 1.724;
		coefficient_3_to_13 = 1.724;
		coefficient_13_plus = 1.381;
		break;
	case CharacterClass::MAGE:
		base = 1100;
		coefficient_2_to_3 = 1.724;
		coefficient_3_to_13 = 1.724;
		coefficient_13_plus = 1.381;
		break;
	case CharacterClass::PRIEST:
		coefficient_2_to_3 = 1.724;
		coefficient_3_to_13 = 1.724;
		coefficient_13_plus = 1.381;
		base = 1050;
		break;
	case CharacterClass::THIEF:
		coefficient_2_to_3 = 1.724;
		coefficient_3_to_13 = 1.724;
		coefficient_13_plus = 1.381;
		base = 900;
		break;
	case CharacterClass::BISHOP:
		base = 1000;
		coefficient_2_to_3 = 2.105;
		coefficient_3_to_13 = 1.724;
		coefficient_13_plus = 1.326;
		break;
	case CharacterClass::SAMURAI:
		coefficient_2_to_3 = 1.754;
		coefficient_3_to_13 = 1.754;
		coefficient_13_plus = 1.326;
		base = 1250;
		break;
	case CharacterClass::LORD:
		coefficient_2_to_3 = 1.754;
		coefficient_3_to_13 = 1.754;
		coefficient_13_plus = 1.326;
		base = 1300;
		break;
	case CharacterClass::NINJA:
		coefficient_2_to_3 = 1.754;
		coefficient_3_to_13 = 1.754;
		coefficient_13_plus = 1.326;
		base = 1450;
		break;
	default:
		break;
	}

	switch (level) {
	case 1:
		return std::floor(base);
		break;
	case 2:
		return std::floor(base * coefficient_2_to_3);
		break;
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
	case 12:
	case 13:
		return std::floor((base * coefficient_2_to_3) * std::pow(coefficient_3_to_13, level - 2));
	default:
		return std::floor(((base * coefficient_2_to_3) * std::pow(coefficient_3_to_13, level - 2)) *
						  std::pow(coefficient_13_plus, level - 13));
	}
}

// Other Functions

// Note a few extra spells taken from Wizardry V, such as DESTO, CALIFIC, LITOFEIT, and LABADI
auto Sorcery::Character::_create_spell_lists() -> void {
	_spells.clear();

	// Mage Spells (grouped by level)
	_spells.push_back(std::tuple<Enums::Magic::Spell, Enums::Magic::SpellType, unsigned int,
		Enums::Magic::SpellCategory, bool>(
		Spell::DUMAPIC, SpellType::MAGE, 1, SpellCategory::FIELD, false));
	_spells.push_back(std::tuple<Enums::Magic::Spell, Enums::Magic::SpellType, unsigned int,
		Enums::Magic::SpellCategory, bool>(
		Spell::HALITO, SpellType::MAGE, 1, SpellCategory::ATTACK, false));
	_spells.push_back(std::tuple<Enums::Magic::Spell, Enums::Magic::SpellType, unsigned int,
		Enums::Magic::SpellCategory, bool>(
		Spell::KATINO, SpellType::MAGE, 1, SpellCategory::DISABLE, false));
	_spells.push_back(std::tuple<Enums::Magic::Spell, Enums::Magic::SpellType, unsigned int,
		Enums::Magic::SpellCategory, bool>(
		Spell::MOGREF, SpellType::MAGE, 1, SpellCategory::SUPPORT, false));

	_spells.push_back(std::tuple<Enums::Magic::Spell, Enums::Magic::SpellType, unsigned int,
		Enums::Magic::SpellCategory, bool>(
		Spell::DESTO, SpellType::MAGE, 2, SpellCategory::FIELD, false));
	_spells.push_back(std::tuple<Enums::Magic::Spell, Enums::Magic::SpellType, unsigned int,
		Enums::Magic::SpellCategory, bool>(
		Spell::DILTO, SpellType::MAGE, 2, SpellCategory::DISABLE, false));
	_spells.push_back(std::tuple<Enums::Magic::Spell, Enums::Magic::SpellType, unsigned int,
		Enums::Magic::SpellCategory, bool>(
		Spell::SOPIC, SpellType::MAGE, 2, SpellCategory::SUPPORT, false));

	_spells.push_back(std::tuple<Enums::Magic::Spell, Enums::Magic::SpellType, unsigned int,
		Enums::Magic::SpellCategory, bool>(
		Spell::CALIFIC, SpellType::MAGE, 3, SpellCategory::FIELD, false));
	_spells.push_back(std::tuple<Enums::Magic::Spell, Enums::Magic::SpellType, unsigned int,
		Enums::Magic::SpellCategory, bool>(
		Spell::MAHALITO, SpellType::MAGE, 3, SpellCategory::ATTACK, false));
	_spells.push_back(std::tuple<Enums::Magic::Spell, Enums::Magic::SpellType, unsigned int,
		Enums::Magic::SpellCategory, bool>(
		Spell::MOLITO, SpellType::MAGE, 3, SpellCategory::ATTACK, false));

	_spells.push_back(std::tuple<Enums::Magic::Spell, Enums::Magic::SpellType, unsigned int,
		Enums::Magic::SpellCategory, bool>(
		Spell::DALTO, SpellType::MAGE, 4, SpellCategory::ATTACK, false));
	_spells.push_back(std::tuple<Enums::Magic::Spell, Enums::Magic::SpellType, unsigned int,
		Enums::Magic::SpellCategory, bool>(
		Spell::LAHALITO, SpellType::MAGE, 4, SpellCategory::ATTACK, false));
	_spells.push_back(std::tuple<Enums::Magic::Spell, Enums::Magic::SpellType, unsigned int,
		Enums::Magic::SpellCategory, bool>(
		Spell::LITOFEIT, SpellType::MAGE, 4, SpellCategory::FIELD, false));
	_spells.push_back(std::tuple<Enums::Magic::Spell, Enums::Magic::SpellType, unsigned int,
		Enums::Magic::SpellCategory, bool>(
		Spell::MORLIS, SpellType::MAGE, 4, SpellCategory::DISABLE, false));

	_spells.push_back(std::tuple<Enums::Magic::Spell, Enums::Magic::SpellType, unsigned int,
		Enums::Magic::SpellCategory, bool>(
		Spell::MADALTO, SpellType::MAGE, 5, SpellCategory::ATTACK, false));
	_spells.push_back(std::tuple<Enums::Magic::Spell, Enums::Magic::SpellType, unsigned int,
		Enums::Magic::SpellCategory, bool>(
		Spell::MAKANITO, SpellType::MAGE, 5, SpellCategory::ATTACK, false));
	_spells.push_back(std::tuple<Enums::Magic::Spell, Enums::Magic::SpellType, unsigned int,
		Enums::Magic::SpellCategory, bool>(
		Spell::MAMORLIS, SpellType::MAGE, 5, SpellCategory::DISABLE, false));

	_spells.push_back(std::tuple<Enums::Magic::Spell, Enums::Magic::SpellType, unsigned int,
		Enums::Magic::SpellCategory, bool>(
		Spell::HAMAN, SpellType::MAGE, 6, SpellCategory::SUPPORT, false));
	_spells.push_back(std::tuple<Enums::Magic::Spell, Enums::Magic::SpellType, unsigned int,
		Enums::Magic::SpellCategory, bool>(
		Spell::LAKANITO, SpellType::MAGE, 6, SpellCategory::ATTACK, false));
	_spells.push_back(std::tuple<Enums::Magic::Spell, Enums::Magic::SpellType, unsigned int,
		Enums::Magic::SpellCategory, bool>(
		Spell::MASOPIC, SpellType::MAGE, 6, SpellCategory::SUPPORT, false));
	_spells.push_back(std::tuple<Enums::Magic::Spell, Enums::Magic::SpellType, unsigned int,
		Enums::Magic::SpellCategory, bool>(
		Spell::ZILWAN, SpellType::MAGE, 6, SpellCategory::ATTACK, false));

	_spells.push_back(std::tuple<Enums::Magic::Spell, Enums::Magic::SpellType, unsigned int,
		Enums::Magic::SpellCategory, bool>(
		Spell::MAHAMAN, SpellType::MAGE, 7, SpellCategory::SUPPORT, false));
	_spells.push_back(std::tuple<Enums::Magic::Spell, Enums::Magic::SpellType, unsigned int,
		Enums::Magic::SpellCategory, bool>(
		Spell::MALOR, SpellType::MAGE, 7, SpellCategory::FIELD, false));
	_spells.push_back(std::tuple<Enums::Magic::Spell, Enums::Magic::SpellType, unsigned int,
		Enums::Magic::SpellCategory, bool>(
		Spell::TILTOWAIT, SpellType::MAGE, 7, SpellCategory::ATTACK, false));

	// Cleric Spells (grouped by level)
	_spells.push_back(std::tuple<Enums::Magic::Spell, Enums::Magic::SpellType, unsigned int,
		Enums::Magic::SpellCategory, bool>(
		Spell::BADIOS, SpellType::PRIEST, 1, SpellCategory::ATTACK, false));
	_spells.push_back(std::tuple<Enums::Magic::Spell, Enums::Magic::SpellType, unsigned int,
		Enums::Magic::SpellCategory, bool>(
		Spell::DIOS, SpellType::PRIEST, 1, SpellCategory::HEALING, false));
	_spells.push_back(std::tuple<Enums::Magic::Spell, Enums::Magic::SpellType, unsigned int,
		Enums::Magic::SpellCategory, bool>(
		Spell::KALKI, SpellType::PRIEST, 1, SpellCategory::SUPPORT, false));
	_spells.push_back(std::tuple<Enums::Magic::Spell, Enums::Magic::SpellType, unsigned int,
		Enums::Magic::SpellCategory, bool>(
		Spell::MILWA, SpellType::PRIEST, 1, SpellCategory::FIELD, false));
	_spells.push_back(std::tuple<Enums::Magic::Spell, Enums::Magic::SpellType, unsigned int,
		Enums::Magic::SpellCategory, bool>(
		Spell::PORFIC, SpellType::PRIEST, 1, SpellCategory::SUPPORT, false));

	_spells.push_back(std::tuple<Enums::Magic::Spell, Enums::Magic::SpellType, unsigned int,
		Enums::Magic::SpellCategory, bool>(
		Spell::CALFO, SpellType::PRIEST, 2, SpellCategory::FIELD, false));
	_spells.push_back(std::tuple<Enums::Magic::Spell, Enums::Magic::SpellType, unsigned int,
		Enums::Magic::SpellCategory, bool>(
		Spell::MANIFO, SpellType::PRIEST, 2, SpellCategory::DISABLE, false));
	_spells.push_back(std::tuple<Enums::Magic::Spell, Enums::Magic::SpellType, unsigned int,
		Enums::Magic::SpellCategory, bool>(
		Spell::MATU, SpellType::PRIEST, 2, SpellCategory::SUPPORT, false));
	_spells.push_back(std::tuple<Enums::Magic::Spell, Enums::Magic::SpellType, unsigned int,
		Enums::Magic::SpellCategory, bool>(
		Spell::MONTINO, SpellType::PRIEST, 2, SpellCategory::DISABLE, false));

	_spells.push_back(std::tuple<Enums::Magic::Spell, Enums::Magic::SpellType, unsigned int,
		Enums::Magic::SpellCategory, bool>(
		Spell::BAMATU, SpellType::PRIEST, 3, SpellCategory::SUPPORT, false));
	_spells.push_back(std::tuple<Enums::Magic::Spell, Enums::Magic::SpellType, unsigned int,
		Enums::Magic::SpellCategory, bool>(
		Spell::DIALKO, SpellType::PRIEST, 3, SpellCategory::HEALING, false));
	_spells.push_back(std::tuple<Enums::Magic::Spell, Enums::Magic::SpellType, unsigned int,
		Enums::Magic::SpellCategory, bool>(
		Spell::LATUMAPIC, SpellType::PRIEST, 3, SpellCategory::FIELD, false));
	_spells.push_back(std::tuple<Enums::Magic::Spell, Enums::Magic::SpellType, unsigned int,
		Enums::Magic::SpellCategory, bool>(
		Spell::LOMILWA, SpellType::PRIEST, 3, SpellCategory::FIELD, false));

	_spells.push_back(std::tuple<Enums::Magic::Spell, Enums::Magic::SpellType, unsigned int,
		Enums::Magic::SpellCategory, bool>(
		Spell::BADIAL, SpellType::PRIEST, 4, SpellCategory::ATTACK, false));
	_spells.push_back(std::tuple<Enums::Magic::Spell, Enums::Magic::SpellType, unsigned int,
		Enums::Magic::SpellCategory, bool>(
		Spell::DIAL, SpellType::PRIEST, 4, SpellCategory::HEALING, false));
	_spells.push_back(std::tuple<Enums::Magic::Spell, Enums::Magic::SpellType, unsigned int,
		Enums::Magic::SpellCategory, bool>(
		Spell::LATUMOFIS, SpellType::PRIEST, 4, SpellCategory::HEALING, false));
	_spells.push_back(std::tuple<Enums::Magic::Spell, Enums::Magic::SpellType, unsigned int,
		Enums::Magic::SpellCategory, bool>(
		Spell::MAPORFIC, SpellType::PRIEST, 4, SpellCategory::FIELD, false));

	_spells.push_back(std::tuple<Enums::Magic::Spell, Enums::Magic::SpellType, unsigned int,
		Enums::Magic::SpellCategory, bool>(
		Spell::BADI, SpellType::PRIEST, 5, SpellCategory::ATTACK, false));
	_spells.push_back(std::tuple<Enums::Magic::Spell, Enums::Magic::SpellType, unsigned int,
		Enums::Magic::SpellCategory, bool>(
		Spell::BADIALMA, SpellType::PRIEST, 5, SpellCategory::ATTACK, false));
	_spells.push_back(std::tuple<Enums::Magic::Spell, Enums::Magic::SpellType, unsigned int,
		Enums::Magic::SpellCategory, bool>(
		Spell::DI, SpellType::PRIEST, 5, SpellCategory::HEALING, false));
	_spells.push_back(std::tuple<Enums::Magic::Spell, Enums::Magic::SpellType, unsigned int,
		Enums::Magic::SpellCategory, bool>(
		Spell::DIALMA, SpellType::PRIEST, 5, SpellCategory::HEALING, false));
	_spells.push_back(std::tuple<Enums::Magic::Spell, Enums::Magic::SpellType, unsigned int,
		Enums::Magic::SpellCategory, bool>(
		Spell::KANDI, SpellType::PRIEST, 5, SpellCategory::FIELD, false));
	_spells.push_back(std::tuple<Enums::Magic::Spell, Enums::Magic::SpellType, unsigned int,
		Enums::Magic::SpellCategory, bool>(
		Spell::LITOKAN, SpellType::PRIEST, 5, SpellCategory::ATTACK, false));

	_spells.push_back(std::tuple<Enums::Magic::Spell, Enums::Magic::SpellType, unsigned int,
		Enums::Magic::SpellCategory, bool>(
		Spell::LABADI, SpellType::PRIEST, 6, SpellCategory::ATTACK, false));
	_spells.push_back(std::tuple<Enums::Magic::Spell, Enums::Magic::SpellType, unsigned int,
		Enums::Magic::SpellCategory, bool>(
		Spell::LOKTOFEIT, SpellType::PRIEST, 6, SpellCategory::FIELD, false));
	_spells.push_back(std::tuple<Enums::Magic::Spell, Enums::Magic::SpellType, unsigned int,
		Enums::Magic::SpellCategory, bool>(
		Spell::LORTO, SpellType::PRIEST, 6, SpellCategory::ATTACK, false));
	_spells.push_back(std::tuple<Enums::Magic::Spell, Enums::Magic::SpellType, unsigned int,
		Enums::Magic::SpellCategory, bool>(
		Spell::MABADI, SpellType::PRIEST, 6, SpellCategory::ATTACK, false));
	_spells.push_back(std::tuple<Enums::Magic::Spell, Enums::Magic::SpellType, unsigned int,
		Enums::Magic::SpellCategory, bool>(
		Spell::MADI, SpellType::PRIEST, 6, SpellCategory::HEALING, false));

	_spells.push_back(std::tuple<Enums::Magic::Spell, Enums::Magic::SpellType, unsigned int,
		Enums::Magic::SpellCategory, bool>(
		Spell::MALIKTO, SpellType::PRIEST, 7, SpellCategory::ATTACK, false));
	_spells.push_back(std::tuple<Enums::Magic::Spell, Enums::Magic::SpellType, unsigned int,
		Enums::Magic::SpellCategory, bool>(
		Spell::KADORTO, SpellType::PRIEST, 7, SpellCategory::HEALING, false));
}

// Render the character
// auto Sorcery::Character::render(unsigned int y_position) -> void {

/* // Character Templates are 79x39
switch (_view) {
case CharacterView::MAIN:
	_character_bg =
		std::make_unique<TCODConsole>(CSTR((*_system.files)[CHARACTER_TEMPLATE_FILE_MAIN]));
	break;
case CharacterView::DETAILED:
	_character_bg =
		std::make_unique<TCODConsole>(CSTR((*_system.files)[CHARACTER_TEMPLATE_FILE_DETAILED]));
	break;
};

// And always update the console with the basic character info
_print(Point(21, 3), _name, Colour::CHARACTER_SUMMARY_INFORMATION);
_print(Point(40, 3), "L" + std::to_string(_abilities[CharacterAbility::CURRENT_LEVEL]));
_print(Point(47, 3), get_character_class_alignment(_alignment).substr(0, 1) + "-" +
						 get_character_class_string(_class));
_print(Point(60, 3), get_character_race_string(_race));

const Point portrait_top_left = {3, 3};
const int portrait_size = {32};
const int portrait_x_index = {_portrait_index % 7}; // Number of Portraits in Graphic by Column
const int portrait_y_index = {
	std::floor(_portrait_index / 7)}; // Number of Portraits in Graphic by Row

switch (_view) {
case CharacterView::MAIN:

	// Current/Max HP
	_print(Point(46, 7),
		std::to_string(_abilities[CharacterAbility::CURRENT_HP]) + "/" +
			std::to_string(_abilities[CharacterAbility::MAX_HP]),
		Colour::CHARACTER_SUMMARY_HP);

	// Character Portrait
	_display.screen->portraits->blit2x(_character_bg.get(), portrait_top_left.x,
		portrait_top_left.y, portrait_x_index * portrait_size, portrait_y_index * portrait_size,
		portrait_size, portrait_size);

	// Attributes
	_print(Point(31, 7), _max_attributes[CharacterAttribute::STRENGTH],
		Colour::CHARACTER_SUMMARY_ATTRIBUTES_MAX);
	if (_current_attributes[CharacterAttribute::STRENGTH] !=
		_max_attributes[CharacterAttribute::STRENGTH])
		_print(Point(34, 7), _current_attributes[CharacterAttribute::STRENGTH]);
	_print(Point(31, 9), _max_attributes[CharacterAttribute::IQ]);
	if (_current_attributes[CharacterAttribute::IQ] != _max_attributes[CharacterAttribute::IQ])
		_print(Point(34, 9), _current_attributes[CharacterAttribute::IQ]);
	_print(Point(31, 11), _max_attributes[CharacterAttribute::PIETY]);
	if (_current_attributes[CharacterAttribute::PIETY] !=
		_max_attributes[CharacterAttribute::PIETY])
		_print(Point(34, 11), _current_attributes[CharacterAttribute::PIETY]);
	_print(Point(31, 13), _max_attributes[CharacterAttribute::VITALITY]);
	if (_current_attributes[CharacterAttribute::VITALITY] !=
		_max_attributes[CharacterAttribute::VITALITY])
		_print(Point(34, 13), _current_attributes[CharacterAttribute::VITALITY]);
	_print(Point(31, 15), _max_attributes[CharacterAttribute::AGILITY],
		Colour::CHARACTER_SUMMARY_ATTRIBUTES_MAX);
	if (_current_attributes[CharacterAttribute::AGILITY] !=
		_max_attributes[CharacterAttribute::AGILITY])
		_print(Point(34, 15), _current_attributes[CharacterAttribute::AGILITY]);
	_print(Point(31, 17), _max_attributes[CharacterAttribute::LUCK],
		Colour::CHARACTER_SUMMARY_ATTRIBUTES_MAX);
	if (_current_attributes[CharacterAttribute::LUCK] !=
		_max_attributes[CharacterAttribute::LUCK])
		_print(Point(34, 17), _current_attributes[CharacterAttribute::LUCK]);

	// Other character stuff
	_print(
		Point(66, 11), _abilities[CharacterAbility::GOLD], Colour::CHARACTER_SUMMARY_ABILITIES);
	_print(Point(46, 9), _abilities[CharacterAbility::CURRENT_ARMOUR_CLASS]);
	_print(Point(46, 11), _abilities[CharacterAbility::AGE] / 52);
	_print(Point(46, 13), _abilities[CharacterAbility::SWIM]);
	_print(Point(66, 7), _abilities[CharacterAbility::CURRENT_XP]);
	_print(Point(66, 9), _abilities[CharacterAbility::NEXT_LEVEL_XP]);
	_print(Point(66, 15), _abilities[CharacterAbility::MARKS]);
	_print(Point(66, 17), _abilities[CharacterAbility::DEATHS]);

	// Spell points
	for (auto level = 1; level <= 7; level++) {
		_print(Point(13 + ((level - 1) * 8), 21), _mage_current_spell_points[level],
			Colour::CHARACTER_SUMMARY_SPELLS_MIN);
		_print(Point(16 + ((level - 1) * 8), 21), _mage_max_spell_points[level],
			Colour::CHARACTER_SUMMARY_SPELLS_MAX);
		_print(Point(13 + ((level - 1) * 8), 23), _cleric_current_spell_points[level],
			Colour::CHARACTER_SUMMARY_SPELLS_MIN);
		_print(Point(16 + ((level - 1) * 8), 23), _cleric_max_spell_points[level],
			Colour::CHARACTER_SUMMARY_SPELLS_MAX);
	}
	break;
case CharacterView::DETAILED:

	_print(Point(23, 6), _current_attributes[CharacterAttribute::STRENGTH],
		Colour::CHARACTER_DETAILED_ATTRIBUTES);
	_print(Point(23, 14), _current_attributes[CharacterAttribute::IQ]);
	_print(Point(49, 6), _current_attributes[CharacterAttribute::PIETY]);
	_print(Point(49, 14), _current_attributes[CharacterAttribute::VITALITY]);
	_print(Point(74, 6), _current_attributes[CharacterAttribute::AGILITY]);
	_print(Point(74, 17), _current_attributes[CharacterAttribute::LUCK]);

	_print(Point(23, 8), _abilities[CharacterAbility::ATTACK_MODIFIER],
		Colour::CHARACTER_DETAILED_ABILITIES);
	_print(Point(23, 9), _abilities[CharacterAbility::HIT_PROBABILITY]);
	_print(Point(23, 10), _abilities[CharacterAbility::BONUS_DAMAGE]);
	_print(Point(23, 11), _abilities[CharacterAbility::BASE_NUMBER_OF_ATTACKS]);
	_print(Point(23, 12), _abilities[CharacterAbility::UNARMED_DAMAGE]);

	_print(Point(23, 16), _abilities[CharacterAbility::MAGE_SPELL_LEARN]);
	_print(Point(23, 17), _abilities[CharacterAbility::IDENTIFY_ITEMS]);
	_print(Point(23, 18), _abilities[CharacterAbility::IDENTIFY_CURSE]);
	_print(Point(23, 19), _abilities[CharacterAbility::IDENTIFY_FOES]);
	_print(Point(23, 20), _abilities[CharacterAbility::BONUS_MAGE_SPELLS]);

	_print(Point(49, 8), _abilities[CharacterAbility::PRIEST_SPELL_LEARN]);
	_print(Point(49, 9), _abilities[CharacterAbility::LOKTOFELT_SUCCESS]);
	_print(Point(49, 10), _abilities[CharacterAbility::BASE_DISPELL]);
	_print(Point(49, 11), _abilities[CharacterAbility::BONUS_PRIEST_SPELLS]);

	_print(Point(49, 16), _abilities[CharacterAbility::VITALITY_BONUS]);
	_print(Point(49, 17), _abilities[CharacterAbility::BONUS_HIT_POINTS]);
	_print(Point(49, 18), _abilities[CharacterAbility::DEAD_RESURRECT]);
	_print(Point(49, 19), _abilities[CharacterAbility::ASHES_RESURRECT]);
	_print(Point(49, 20), _abilities[CharacterAbility::DI_KADORTO_RESURRECT]);

	_print(Point(74, 8), _abilities[CharacterAbility::INITIATIVE_MODIFIER]);
	_print(Point(74, 9), _abilities[CharacterAbility::BASE_CRITICAL_HIT]);
	_print(Point(74, 10), _abilities[CharacterAbility::IDENTIFY_TRAP]);
	_print(Point(74, 11), _abilities[CharacterAbility::BASE_DISARM_TRAP]);
	_print(Point(74, 12), _abilities[CharacterAbility::ACTIVATE_TRAP]);
	_print(Point(74, 13), _abilities[CharacterAbility::BASE_AVOID_PIT]);
	_print(Point(74, 14), _abilities[CharacterAbility::BASE_ARMOUR_CLASS]);

	_print(Point(74, 19), _abilities[CharacterAbility::BASE_RESIST_BONUS]);
	_print(Point(74, 20), _abilities[CharacterAbility::EQUIPMENT_INTACT_ON_WIPE]);

	_print(Point(23, 26), _abilities[CharacterAbility::RESISTANCE_VS_CRITICAL_HIT]);
	_print(Point(23, 27), _abilities[CharacterAbility::RESISTANCE_VS_POISON_PARALYSIS]);
	_print(Point(23, 28), _abilities[CharacterAbility::RESISTANCE_VS_STONING]);
	_print(Point(23, 29), _abilities[CharacterAbility::RESISTANCE_VS_BREATH_ATTACKS]);
	_print(Point(23, 30), _abilities[CharacterAbility::RESISTANCE_VS_POISON_GAS_TRAP]);
	_print(Point(23, 31), _abilities[CharacterAbility::RESISTANCE_VS_MAGE_PRIEST_TRAP]);

	_print(Point(49, 26), _abilities[CharacterAbility::RECOVER_FROM_SLEEP]);
	_print(Point(49, 27), _abilities[CharacterAbility::RECOVER_FROM_FEAR]);
	_print(Point(49, 28), _abilities[CharacterAbility::RESISTANCE_VS_SILENCE]);
	_print(Point(49, 29), _abilities[CharacterAbility::RESISTANCE_VS_KATINO]);
	_print(Point(49, 30), _abilities[CharacterAbility::RESISTANCE_VS_BADI]);
	_print(Point(49, 31), _abilities[CharacterAbility::RESISTANCE_VS_MANIFO]);

	break;
};

// And display
const Point summary_loc = {(_display.screen->width() - 79) / 2, y_position};
_display.screen->load_from_offscreen(_character_bg, summary_loc);
*/
//}

// Create a (semi) random character
auto Sorcery::Character::create_random() -> void {

	// Exclude Samurai/Lord/Ninja/Bishop from this method of character creation
	_class = {static_cast<CharacterClass>((*_system.random)[RandomType::D4])};
	_race = {static_cast<CharacterRace>((*_system.random)[RandomType::D5])};
	switch (_class) { // NOLINT(clang-diagnostic-switch)
	case CharacterClass::FIGHTER:
	case CharacterClass::MAGE:
		_alignment = {static_cast<CharacterAlignment>((*_system.random)[RandomType::D3])};
		break;
	case CharacterClass::PRIEST:
		_alignment = (*_system.random)[RandomType::D2] == 1 ? CharacterAlignment::GOOD
															: CharacterAlignment::EVIL;
		break;
	case CharacterClass::THIEF:
		_alignment = (*_system.random)[RandomType::D2] == 1 ? CharacterAlignment::NEUTRAL
															: CharacterAlignment::EVIL;
		break;
	default:
		break;
	}

	// Now get minimum attributes for race/class combo (note as we are only allowing creation of
	// some classes, it will be as if we had a maximum of 10 bonus points to spend - in order to
	// incentivise full blown character creation! see table IV (A) at
	// https://gamefaqs.gamespot.com/pc/946844-the-ultimate-wizardry-archives/faqs/45726 for info
	switch (_race) { // NOLINT(clang-diagnostic-switch)
	case CharacterRace::HUMAN:
		_start_attr = {{CharacterAttribute::STRENGTH, 8}, {CharacterAttribute::IQ, 5},
			{CharacterAttribute::PIETY, 5}, {CharacterAttribute::VITALITY, 8},
			{CharacterAttribute::AGILITY, 8}, {CharacterAttribute::LUCK, 9}};
		break;
	case CharacterRace::ELF:
		_start_attr = {{CharacterAttribute::STRENGTH, 7}, {CharacterAttribute::IQ, 10},
			{CharacterAttribute::PIETY, 10}, {CharacterAttribute::VITALITY, 6},
			{CharacterAttribute::AGILITY, 9}, {CharacterAttribute::LUCK, 6}};
		break;
	case CharacterRace::DWARF:
		_start_attr = {{CharacterAttribute::STRENGTH, 10}, {CharacterAttribute::IQ, 7},
			{CharacterAttribute::PIETY, 10}, {CharacterAttribute::VITALITY, 10},
			{CharacterAttribute::AGILITY, 5}, {CharacterAttribute::LUCK, 6}};
		break;
	case CharacterRace::GNOME:
		_start_attr = {{CharacterAttribute::STRENGTH, 7}, {CharacterAttribute::IQ, 7},
			{CharacterAttribute::PIETY, 10}, {CharacterAttribute::VITALITY, 8},
			{CharacterAttribute::AGILITY, 10}, {CharacterAttribute::LUCK, 7}};
		break;
	case CharacterRace::HOBBIT:
		_start_attr = {{CharacterAttribute::STRENGTH, 5}, {CharacterAttribute::IQ, 7},
			{CharacterAttribute::PIETY, 7}, {CharacterAttribute::VITALITY, 6},
			{CharacterAttribute::AGILITY, 10}, {CharacterAttribute::LUCK, 12}};
		break;
	default:
		break;
	};

	// Put most of the points into the main attribute (note that 10 points means a Human Priest and
	// Dwarf Thief have allocated all points to their main attribute with no points left over)
	_points_left = 10;
	_st_points = _points_left;
	switch (_class) { // NOLINT(clang-diagnostic-switch)
	case CharacterClass::FIGHTER:
		_points_left -= (15 - _start_attr[CharacterAttribute::STRENGTH]);
		_start_attr[CharacterAttribute::STRENGTH] = 15;
		_portrait_index = 12;
		break;
	case CharacterClass::MAGE:
		_points_left -= (15 - _start_attr[CharacterAttribute::IQ]);
		_start_attr[CharacterAttribute::IQ] = 15;
		_portrait_index = 35;
		break;
	case CharacterClass::PRIEST:
		_points_left -= (15 - _start_attr[CharacterAttribute::PIETY]);
		_start_attr[CharacterAttribute::PIETY] = 15;
		_portrait_index = 31;
		break;
	case CharacterClass::THIEF:
		_points_left -= (15 - _start_attr[CharacterAttribute::AGILITY]);
		_start_attr[CharacterAttribute::AGILITY] = 15;
		_portrait_index = 1;
		break;
	default:
		break;
	}

	// Pump any points left into the Vitality attribute
	if (_points_left > 0)
		_start_attr[CharacterAttribute::VITALITY] += _points_left;

	_cur_attr = _start_attr;
}

// For level draining, optionally keep a track of negative levels unless in strict mode

// Need to also handle character class switching

/* switch (_class) {

		case CharacterClass::FIGHTER:
		break;
	case CharacterClass::MAGE:
		break;
	case CharacterClass::PRIEST:
		break;
	case CharacterClass::THIEF:
		break;
	case CharacterClass::BISHOP:
		break
	case CharacterClass::SAMURAI:
		break;
	case CharacterClass::LORD:
		break;
	case CharacterClass::NINJA:
		break;
	}
*/
