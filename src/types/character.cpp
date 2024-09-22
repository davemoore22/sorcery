// Copyright (C) 2024 Dave Moore
//
// This file is part of Sorcery: Shadows under Llylgamyn.
//
// Sorcery: Shadows under Llylgamyn is free software: you can redistribute
// it and/or modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation, either version 2 of the License,
// or (at your option) any later version.
//
// Sorcery: Shadows under Llylgamyn is distributed in the hope that it wil
// be useful, but WITHOUT ANY WARRANTY; wiWthout even the implied warranty of
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

#include "types/character.hpp"
#include "common/enum.hpp"
#include "common/macro.hpp"
#include "core/display.hpp"
#include "core/graphics.hpp"
#include "core/random.hpp"
#include "core/system.hpp"
#include "resources/itemstore.hpp"
#include "resources/stringstore.hpp"
#include "types/configfile.hpp"
#include "types/define.hpp"
#include "types/enum.hpp"

Sorcery::Character::Character() {
}

// Standard Constructor
Sorcery::Character::Character(System *system, Display *display, Graphics *graphics, ItemStore *itemstore)
	: _system{system}, _display{display}, _graphics{graphics}, _itemstore{itemstore} {

	set_stage(CHS::CHOOSE_METHOD);

	_hidden = false;
	set_status(CHT::OK);

	_version = CHARACTER_SAVE_VERSION;

	_legated = false;

	_location = CHL::TAVERN;

	coordinate = std::nullopt;
	depth = std::nullopt;

	inventory.clear();
}

// Overloaded Operator
auto Sorcery::Character::operator[](const CAB &key) -> int & {

	return _abilities[key];
}

// Utility Functions

auto Sorcery::Character::get_stage() const -> CHS {

	return _current_stage;
}

auto Sorcery::Character::get_location() const -> CHL {

	return _location;
}

auto Sorcery::Character::set_location(const CHL value) -> void {

	_location = value;

	if (value == CHL::TAVERN || value == CHL::TEMPLE || value == CHL::TRAINING) {
		coordinate = std::nullopt;
		depth = std::nullopt;
	}
}

auto Sorcery::Character::abilities() -> CharacterAbilities & {

	return _abilities;
}

auto Sorcery::Character::spells() -> std::vector<Spell> & {

	return _spells;
}

auto Sorcery::Character::attributes() -> CharacterAttributes & {

	return _cur_attr;
}

auto Sorcery::Character::priest_cur_sp() -> SpellPoints & {

	return _priest_cur_sp;
}

auto Sorcery::Character::mage_cur_sp() -> SpellPoints & {

	return _mage_cur_sp;
}

auto Sorcery::Character::priest_max_sp() -> SpellPoints & {

	return _priest_max_sp;
}

auto Sorcery::Character::mage_max_sp() -> SpellPoints & {

	return _mage_max_sp;
}

// Reset a character back to a particular state
auto Sorcery::Character::set_stage(const CHS stage) -> void {

	_current_stage = stage;
	switch (stage) {
	case CHS::CHOOSE_METHOD:
		_name.clear();
		_race = CHR::NO_RACE;
		_alignment = CAL::NO_ALIGN;
		_start_attr.clear();
		_cur_attr.clear();
		_max_attr.clear();
		_abilities.clear();

		// Used in the display from this point onwards
		_abilities[CAB::CURRENT_LEVEL] = 1;
		_class = CHC::NO_CLASS;
		_points_left = 0;
		_st_points = 0;
		_pos_classes.clear();
		_num_pos_classes = 0;
		_portrait_index = 0;
		_priest_max_sp.clear();
		_priest_cur_sp.clear();
		_mage_max_sp.clear();
		_mage_cur_sp.clear();
		_spells.clear();
		create_spells();
		reset_spells();
		break;
	case CHS::REVIEW_AND_CONFIRM:

		// Handle the generation of the Character Display Here
		//_view = SUMMARY;
		//_generate_display();
		break;
	default:
		break;
	}
}

auto Sorcery::Character::get_name() const -> std::string {

	return _name;
}

auto Sorcery::Character::get_name_and_status() const -> std::string {

	return fmt::format("{:<16} {:>12}", _name, get_status_string());
}

auto Sorcery::Character::get_name_and_loc() const -> std::string {

	return fmt::format(
		"{:<16} B{}F {:>2}N/{:>2}E", _name, std::abs(depth.value()), coordinate.value().y, coordinate.value().x);
}

auto Sorcery::Character::set_name(std::string_view value) -> void {

	_name = value;
}

auto Sorcery::Character::get_race() const -> CHR {

	return _race;
}

auto Sorcery::Character::set_race(const CHR &value) -> void {

	_race = value;
}

auto Sorcery::Character::get_level() const -> int {

	return _abilities.at(CAB::CURRENT_LEVEL);
}

auto Sorcery::Character::set_level(const int &value) -> void {

	_abilities.at(CAB::CURRENT_LEVEL) = value;
}

auto Sorcery::Character::get_alignment() const -> CAL {

	return _alignment;
}

auto Sorcery::Character::set_alignment(const CAL &value) -> void {

	_alignment = value;
}

auto Sorcery::Character::get_num_pos_class() const -> unsigned int {

	return _num_pos_classes;
}

auto Sorcery::Character::get_class() const -> CHC {

	return _class;
}

auto Sorcery::Character::set_class(const CHC &value) -> void {

	_class = value;
}

auto Sorcery::Character::get_points_left() const -> unsigned int {

	return _points_left;
}

auto Sorcery::Character::set_points_left(const unsigned int &value) -> void {

	_points_left = value;
}

auto Sorcery::Character::get_start_points() const -> unsigned int {

	return _st_points;
}

auto Sorcery::Character::set_start_points(const unsigned int &value) -> void {

	_st_points = value;
}

auto Sorcery::Character::get_cur_attr(const CAR attribute) const -> unsigned int {

	return _cur_attr.at(attribute);
}

auto Sorcery::Character::get_start_attr(const CAR attribute) const -> unsigned int {

	return _start_attr.at(attribute);
}

auto Sorcery::Character::set_cur_attr(const CAR attribute, const int adjustment) -> void {

	_cur_attr.at(attribute) += adjustment;
}

auto Sorcery::Character::get_portrait_index() const -> unsigned int {

	return _portrait_index;
}

auto Sorcery::Character::set_portrait_index(const unsigned int value) -> void {

	_portrait_index = value;
}

auto Sorcery::Character::set_start_attr() -> void {

	_start_attr.clear();
	_cur_attr.clear();
	switch (_race) {
	case CHR::HUMAN:
		_start_attr = {
			{CAR::STRENGTH, 8}, {CAR::IQ, 5}, {CAR::PIETY, 5}, {CAR::VITALITY, 8}, {CAR::AGILITY, 8}, {CAR::LUCK, 9}};
		break;
	case CHR::ELF:
		_start_attr = {
			{CAR::STRENGTH, 7}, {CAR::IQ, 10}, {CAR::PIETY, 10}, {CAR::VITALITY, 6}, {CAR::AGILITY, 9}, {CAR::LUCK, 6}};
		break;
	case CHR::DWARF:
		_start_attr = {{CAR::STRENGTH, 10}, {CAR::IQ, 7}, {CAR::PIETY, 10}, {CAR::VITALITY, 10}, {CAR::AGILITY, 5},
			{CAR::LUCK, 6}};
		break;
	case CHR::GNOME:
		_start_attr = {
			{CAR::STRENGTH, 7}, {CAR::IQ, 7}, {CAR::PIETY, 10}, {CAR::VITALITY, 8}, {CAR::AGILITY, 10}, {CAR::LUCK, 7}};
		break;
	case CHR::HOBBIT:
		_start_attr = {
			{CAR::STRENGTH, 5}, {CAR::IQ, 7}, {CAR::PIETY, 7}, {CAR::VITALITY, 6}, {CAR::AGILITY, 10}, {CAR::LUCK, 12}};
		break;
	default:
		break;
	}

	_cur_attr = _start_attr;

	// Formula sourced from http://www.zimlab.com/wizardry/walk/w123calc.htm
	_points_left = (*_system->random)[RNT::ZERO_TO_3];
	const bool chance_of_more{(*_system->random)[RNT::D10] == 1};
	const bool chance_of_more_again{(*_system->random)[RNT::D10] == 1};
	_points_left += 7;
	if (_points_left < 20)
		if (chance_of_more)
			_points_left += 10;
	if (_points_left < 20)
		if (chance_of_more_again)
			_points_left += 10;
	_st_points = _points_left;
}

auto Sorcery::Character::get_cur_attr() const -> CharacterAttributes {

	return _cur_attr;
}

auto Sorcery::Character::get_start_attr() const -> CharacterAttributes {

	return _start_attr;
}

auto Sorcery::Character::get_pos_class() const -> CharacterClassQualified {

	return _pos_classes;
}

// Given a character's current stats and alignment, work out what classes are
// available
auto Sorcery::Character::set_pos_class() -> void {

	_pos_classes.clear();

	// Do the basic classes first (this also sets _num_possible_character_classes); data is from
	// https://strategywiki.org/wiki/Wizardry:_Proving_Grounds_of_the_Mad_Overlord/Trebor%27s_castle#Classes
	if (_cur_attr[CAR::STRENGTH] >= 11)
		_pos_classes[CHC::FIGHTER] = true;
	else
		_pos_classes[CHC::FIGHTER] = false;

	if (_cur_attr[CAR::IQ] >= 11)
		_pos_classes[CHC::MAGE] = true;
	else
		_pos_classes[CHC::MAGE] = false;

	if (_cur_attr[CAR::PIETY] >= 11)
		if (_alignment == CAL::GOOD || _alignment == CAL::EVIL)
			_pos_classes[CHC::PRIEST] = true;
		else
			_pos_classes[CHC::PRIEST] = false;
	else
		_pos_classes[CHC::PRIEST] = false;

	if (_cur_attr[CAR::AGILITY] >= 11)
		if (_alignment == CAL::NEUTRAL || _alignment == CAL::EVIL)
			_pos_classes[CHC::THIEF] = true;
		else
			_pos_classes[CHC::THIEF] = false;
	else
		_pos_classes[CHC::THIEF] = false;

	// Now the elite classes
	if (_cur_attr[CAR::IQ] >= 12 && _cur_attr[CAR::PIETY] >= 12)
		if (_alignment == CAL::GOOD || _alignment == CAL::EVIL)
			_pos_classes[CHC::BISHOP] = true;
		else
			_pos_classes[CHC::BISHOP] = false;
	else
		_pos_classes[CHC::BISHOP] = false;

	if (_cur_attr[CAR::STRENGTH] >= 15 && _cur_attr[CAR::IQ] >= 11 && _cur_attr[CAR::PIETY] >= 10 &&
		_cur_attr[CAR::VITALITY] >= 10 && _cur_attr[CAR::AGILITY] >= 10)
		if (_alignment == CAL::GOOD || _alignment == CAL::NEUTRAL)
			_pos_classes[CHC::SAMURAI] = true;
		else
			_pos_classes[CHC::SAMURAI] = false;
	else
		_pos_classes[CHC::SAMURAI] = false;

	if (_cur_attr[CAR::STRENGTH] >= 15 && _cur_attr[CAR::IQ] >= 12 && _cur_attr[CAR::PIETY] >= 12 &&
		_cur_attr[CAR::VITALITY] >= 15 && _cur_attr[CAR::AGILITY] >= 14 && _cur_attr[CAR::LUCK] >= 15)
		if (_alignment == CAL::GOOD)
			_pos_classes[CHC::LORD] = true;
		else
			_pos_classes[CHC::LORD] = false;
	else
		_pos_classes[CHC::LORD] = false;

	// Using looser Wizardry 5 requirements for Ninja (see https://wizardry.fandom.com/wiki/Ninja)
	if (_cur_attr[CAR::STRENGTH] >= 15 && _cur_attr[CAR::IQ] >= 17 && _cur_attr[CAR::PIETY] >= 15 &&
		_cur_attr[CAR::VITALITY] >= 16 && _cur_attr[CAR::AGILITY] >= 15 && _cur_attr[CAR::LUCK] >= 16)
		if (_alignment != CAL::GOOD)
			_pos_classes[CHC::NINJA] = true;
		else
			_pos_classes[CHC::NINJA] = false;
	else
		_pos_classes[CHC::NINJA] = false;

	// And workout the number of classes
	_num_pos_classes =
		std::count_if(_pos_classes.begin(), _pos_classes.end(), [](auto element) { return element.second; });
}

// Enum to String functions
auto Sorcery::Character::alignment_to_str(CAL character_alignment) const -> std::string {

	static const std::array<std::string, 4> alignments{"", (*_display->string)["CHARACTER_ALIGNMENT_GOOD"],
		(*_display->string)["CHARACTER_ALIGNMENT_NEUTRAL"], (*_display->string)["CHARACTER_ALIGNMENT_EVIL"]};

	return alignments[unenum(character_alignment)];
}

auto Sorcery::Character::race_to_str(CHR character_race) const -> std::string {

	static const std::array<std::string, 6> races{"", (*_display->string)["CHARACTER_RACE_HUMAN"],
		(*_display->string)["CHARACTER_RACE_ELF"], (*_display->string)["CHARACTER_RACE_DWARF"],
		(*_display->string)["CHARACTER_RACE_GNOME"], (*_display->string)["CHARACTER_RACE_HOBBIT"]};

	return races[unenum(character_race)];
}

auto Sorcery::Character::class_to_str(CHC character_class) const -> std::string {

	static const std::array<std::string, 10> classes{"", (*_display->string)["CHARACTER_CLASS_FIGHTER"],
		(*_display->string)["CHARACTER_CLASS_MAGE"], (*_display->string)["CHARACTER_CLASS_PRIEST"],
		(*_display->string)["CHARACTER_CLASS_THIEF"], (*_display->string)["CHARACTER_CLASS_BISHOP"],
		(*_display->string)["CHARACTER_CLASS_SAMURAI"], (*_display->string)["CHARACTER_CLASS_LORD"],
		(*_display->string)["CHARACTER_CLASS_NINJA"]};

	return classes[unenum(character_class)];
}

// Last step of creating new a character
auto Sorcery::Character::finalise() -> void {

	_generate_start_info();
	_generate_secondary_abil(true, false, false);
	_set_start_spells();
	_set_starting_sp();
}

auto Sorcery::Character::get_ress_chance(bool ashes) -> unsigned int {

	return ashes ? _abilities[CAB::ASHES_RESURRECT] : _abilities[CAB::DEAD_RESURRECT];
}

auto Sorcery::Character::grant_gold(const int value) -> void {

	_abilities[CAB::GOLD] = _abilities[CAB::GOLD] + value;
}

auto Sorcery::Character::get_gold() const -> unsigned int {

	return _abilities.at(CAB::GOLD);
}

auto Sorcery::Character::set_gold(const unsigned int value) -> void {

	_abilities[CAB::GOLD] = value;
}

auto Sorcery::Character::_legate_start_info() -> void {

	// From here: https://datadrivengamer.blogspot.com/2021/08/the-new-mechanics-of-wizardry-iii.html
	_abilities[CAB::CURRENT_LEVEL] = 1;
	_abilities[CAB::CURRENT_XP] = 0;
	_abilities[CAB::NEXT_LEVEL_XP] = _get_xp_for_level(_abilities[CAB::CURRENT_LEVEL]);
	_abilities[CAB::MAX_LEVEL] = _abilities[CAB::CURRENT_LEVEL];
	_abilities[CAB::NEGATIVE_LEVEL] = 0;
	_abilities[CAB::HIT_DICE] = 1;
	if (_abilities[CAB::GOLD] > 500)
		_abilities[CAB::GOLD] = 500;
	_abilities[CAB::AGE] = (20 * 52);
	_abilities[CAB::SWIM] = 1;
	_abilities[CAB::MARKS] = 0;
	_abilities[CAB::DEATHS] = 0;

	// (D7 - 4) is -3 to +3
	_start_attr[CAR::STRENGTH] += ((*_system->random)[RNT::D7] - 4);
	_start_attr[CAR::IQ] += ((*_system->random)[RNT::D7] - 4);
	_start_attr[CAR::PIETY] += ((*_system->random)[RNT::D7] - 4);
	_start_attr[CAR::VITALITY] += ((*_system->random)[RNT::D7] - 4);
	_start_attr[CAR::AGILITY] += ((*_system->random)[RNT::D7] - 4);
	_start_attr[CAR::LUCK] += ((*_system->random)[RNT::D7] - 4);

	auto mage_sp_total{0};
	for (auto level = 1; level <= 7; level++)
		mage_sp_total += _mage_max_sp[level];
	_start_attr[CAR::IQ] += (mage_sp_total / 7);

	auto priest_sp_total{1};
	for (auto level = 1; level <= 7; level++)
		priest_sp_total += _priest_max_sp[level];
	_start_attr[CAR::PIETY] += (priest_sp_total / 10);

	switch (_class) { // NOLINT(clang-diagnostic-switch)
	case CHC::FIGHTER:
	case CHC::LORD:
	case CHC::SAMURAI:
		_start_attr[CAR::STRENGTH] += 2;
		break;
	case CHC::MAGE:
		_start_attr[CAR::IQ] += 2;
		break;
	case CHC::PRIEST:
	case CHC::BISHOP:
		_start_attr[CAR::PIETY] += 2;
		break;
	case CHC::THIEF:
	case CHC::NINJA:
		_start_attr[CAR::AGILITY] += 2;
	default:
		break;
	}

	set_status(CHT::OK);
	_location = CHL::TAVERN;
	_abilities[CAB::CURRENT_HP] = _abilities[CAB::MAX_HP];

	// Clamp Values
	_start_attr[CAR::STRENGTH] = std::min(_start_attr[CAR::STRENGTH], 18);
	_start_attr[CAR::IQ] = std::min(_start_attr[CAR::IQ], 18);
	_start_attr[CAR::PIETY] = std::min(_start_attr[CAR::PIETY], 18);
	_start_attr[CAR::VITALITY] = std::min(_start_attr[CAR::VITALITY], 18);
	_start_attr[CAR::AGILITY] = std::min(_start_attr[CAR::AGILITY], 18);
	_start_attr[CAR::LUCK] = std::min(_start_attr[CAR::LUCK], 18);
	_start_attr[CAR::STRENGTH] = std::max(_start_attr[CAR::STRENGTH], 3);
	_start_attr[CAR::IQ] = std::max(_start_attr[CAR::IQ], 3);
	_start_attr[CAR::PIETY] = std::max(_start_attr[CAR::PIETY], 3);
	_start_attr[CAR::VITALITY] = std::max(_start_attr[CAR::VITALITY], 3);
	_start_attr[CAR::AGILITY] = std::max(_start_attr[CAR::AGILITY], 3);
	_start_attr[CAR::LUCK] = std::max(_start_attr[CAR::LUCK], 3);

	_cur_attr = _start_attr;
	_max_attr = _cur_attr;

	_abilities[CAB::CURRENT_XP] = 0;
	_abilities[CAB::NEXT_LEVEL_XP] = _get_xp_for_level(_abilities[CAB::CURRENT_LEVEL]);
}

auto Sorcery::Character::_regenerate_start_info() -> void {

	_abilities[CAB::MAX_LEVEL] = _abilities[CAB::CURRENT_LEVEL];
	_abilities[CAB::CURRENT_LEVEL] = 1;
	_abilities[CAB::CURRENT_XP] = 0;
	_abilities[CAB::NEXT_LEVEL_XP] = _get_xp_for_level(_abilities[CAB::CURRENT_LEVEL]);

	// https://datadrivengamer.blogspot.com/2019/08/the-not-so-basic-mechanics-of-wizardry.html
	auto age_increment{(52 * (3 + (*_system->random)[RNT::D3])) + 44};
	_abilities[CAB::AGE] += age_increment;

	// Reset attributes to racial minimums
	CharacterAttributes minimum_attr;
	switch (_race) {
	case CHR::HUMAN:
		minimum_attr = {
			{CAR::STRENGTH, 8}, {CAR::IQ, 5}, {CAR::PIETY, 5}, {CAR::VITALITY, 8}, {CAR::AGILITY, 8}, {CAR::LUCK, 9}};
		break;
	case CHR::ELF:
		minimum_attr = {
			{CAR::STRENGTH, 7}, {CAR::IQ, 10}, {CAR::PIETY, 10}, {CAR::VITALITY, 6}, {CAR::AGILITY, 9}, {CAR::LUCK, 6}};
		break;
	case CHR::DWARF:
		minimum_attr = {{CAR::STRENGTH, 10}, {CAR::IQ, 7}, {CAR::PIETY, 10}, {CAR::VITALITY, 10}, {CAR::AGILITY, 5},
			{CAR::LUCK, 6}};
		break;
	case CHR::GNOME:
		minimum_attr = {
			{CAR::STRENGTH, 7}, {CAR::IQ, 7}, {CAR::PIETY, 10}, {CAR::VITALITY, 8}, {CAR::AGILITY, 10}, {CAR::LUCK, 7}};
		break;
	case CHR::HOBBIT:
		minimum_attr = {
			{CAR::STRENGTH, 5}, {CAR::IQ, 7}, {CAR::PIETY, 7}, {CAR::VITALITY, 6}, {CAR::AGILITY, 10}, {CAR::LUCK, 12}};
		break;
	default:
		break;
	}

	_cur_attr = minimum_attr;
}

// Legate
auto Sorcery::Character::legate(const CAL &value) -> void {

	if (_alignment != value)
		_alignment = value;

	_legate_start_info();
	_generate_secondary_abil(true, false, true);
	_set_start_spells();
	_set_starting_sp();
	inventory.clear();
	set_status(CHT::OK);
	_legated = true;

	set_location(CHL::TAVERN);
}

// Change Class
auto Sorcery::Character::change_class(const CHC &value) -> void {

	if (_class != value) {
		_class = value;

		_regenerate_start_info();
		_generate_secondary_abil(false, true, false);
		_reset_start_spells(); // TODO: check this, not sure this works properly
		_reset_starting_sp();
		inventory.unequip_all();

		// Also need to deequip all items!
	}
}

auto Sorcery::Character::is_legated() const -> bool {

	return _legated;
}

auto Sorcery::Character::get_version() const -> int {

	return _version;
}

// Work out all the stuff to do with starting a new character
auto Sorcery::Character::_generate_start_info() -> void {

	// _abilities[CURRENT_LEVEL] = 1; // why is this commented out?
	_abilities[CAB::MAX_LEVEL] = 1;
	_abilities[CAB::NEGATIVE_LEVEL] = 0;
	_abilities[CAB::HIT_DICE] = 1;
	_abilities[CAB::GOLD] = (*_system->random)[RNT::ZERO_TO_99] + 90;
	_abilities[CAB::AGE] = (18 * 52) + (*_system->random)[RNT::ZERO_TO_299];
	_abilities[CAB::SWIM] = 1;
	_abilities[CAB::MARKS] = 0;
	_abilities[CAB::DEATHS] = 0;

	_start_attr = _cur_attr;
	_max_attr = _cur_attr;

	_abilities[CAB::CURRENT_XP] = 0;
	_abilities[CAB::NEXT_LEVEL_XP] = _get_xp_for_level(_abilities[CAB::CURRENT_LEVEL]);
}

// Given the characters current level, work out all the secondary abilities/stats etc
auto Sorcery::Character::_generate_secondary_abil(bool initial, bool change_class, bool legate) -> void {

	// Formulae used are from here
	// http://www.zimlab.com/wizardry/walk/w123calc.htm and also from
	// https://mirrors.apple2.org.za/ftp.apple.asimov.net/images/games/rpg/wizardry/wizardry_I/Wizardry_i_SourceCode.zip
	const auto current_level{_abilities[CAB::CURRENT_LEVEL]};

	// Bonus Melee to Hit per Attack (num)
	if (_cur_attr[CAR::STRENGTH] > 15)
		_abilities[CAB::ATTACK_MODIFIER] = _cur_attr[CAR::STRENGTH] - 15;
	else if (_cur_attr[CAR::STRENGTH] < 6)
		_abilities[CAB::ATTACK_MODIFIER] = _cur_attr[CAR::STRENGTH] - 6;
	else
		_abilities[CAB::ATTACK_MODIFIER] = 0;

	// Bonus Melee to Hit per Attack (num)
	switch (_class) {
	case CHC::FIGHTER:
	case CHC::SAMURAI:
	case CHC::LORD:
	case CHC::NINJA:
	case CHC::PRIEST:
		_abilities[CAB::HIT_PROBABILITY] = 2 + (current_level / 5);
		break;
	default:
		_abilities[CAB::HIT_PROBABILITY] = current_level / 5;
		break;
	}

	// Bonus Melee Damage per Attack (num)
	if (_cur_attr[CAR::STRENGTH] > 15)
		_abilities[CAB::BONUS_DAMAGE] = _cur_attr[CAR::STRENGTH] - 15;
	else if (_cur_attr[CAR::STRENGTH] < 6)
		_abilities[CAB::BONUS_DAMAGE] = _cur_attr[CAR::STRENGTH] - 6;
	else
		_abilities[CAB::BONUS_DAMAGE] = 0;

	// Unarmed Attack Damage (num)
	_abilities[CAB::UNARMED_DAMAGE] =
		_class == CHC::NINJA ? 8 + _abilities[CAB::BONUS_DAMAGE] : 4 + _abilities[CAB::BONUS_DAMAGE];

	// Number of Melee Attacks (num)
	switch (_class) {
	case CHC::FIGHTER:
	case CHC::SAMURAI:
	case CHC::LORD:
		_abilities[CAB::BASE_NUMBER_OF_ATTACKS] = current_level / 5;
		break;
	case CHC::NINJA:
		_abilities[CAB::BASE_NUMBER_OF_ATTACKS] = (current_level / 5) + 1;
		break;
	default:
		_abilities[CAB::BASE_NUMBER_OF_ATTACKS] = 1;
		break;
	}
	if (_abilities[CAB::BASE_NUMBER_OF_ATTACKS] == 0)
		_abilities[CAB::BASE_NUMBER_OF_ATTACKS] = 1;

	// Chance of learning new Mage Spells (%)
	switch (_class) {
	case CHC::SAMURAI:
	case CHC::BISHOP:
	case CHC::MAGE:
		_abilities[CAB::MAGE_SPELL_LEARN] = (_cur_attr[CAR::IQ] / 29.0) * 100;
		break;
	default:
		_abilities[CAB::MAGE_SPELL_LEARN] = 0;
		break;
	}

	// Chance of Identifying Items (%)
	_abilities[CAB::IDENTIFY_ITEMS] = _class == CHC::BISHOP ? 10 + (5 * current_level) : 0;
	if (_abilities[CAB::IDENTIFY_ITEMS] > 100)
		_abilities[CAB::IDENTIFY_ITEMS] = 100;

	// Chance of getting cursed when Identifying Items (%)
	_abilities[CAB::IDENTIFY_CURSE] = _class == CHC::BISHOP ? 35 - (5 * current_level) : 0;
	if (_abilities[CAB::IDENTIFY_CURSE] < 0)
		_abilities[CAB::IDENTIFY_CURSE] = 0;

	// Chance of identifying unknown Foes per round (%)
	_abilities[CAB::IDENTIFY_FOES] = current_level + _cur_attr[CAR::IQ] + _cur_attr[CAR::PIETY];
	if (_abilities[CAB::IDENTIFY_FOES] > 100)
		_abilities[CAB::IDENTIFY_FOES] = 100;

	// Chance of learning new Priest Spells (%)
	switch (_class) {
	case CHC::PRIEST:
	case CHC::LORD:
	case CHC::BISHOP:
		_abilities[CAB::PRIEST_SPELL_LEARN] = (_cur_attr[CAR::PIETY] / 30.0) * 100;
		break;
	default:
		_abilities[CAB::PRIEST_SPELL_LEARN] = 0;
		break;
	}

	// LOKTOFELT success chance (%)
	_abilities[CAB::LOKTOFELT_SUCCESS] = _class == CHC::PRIEST ? 2 * current_level : 0;

	// Base Dispell chance (affected by monster level) (%)
	switch (_class) {
	case CHC::PRIEST:
		_abilities[CAB::BASE_DISPELL] = 50 + (5 * current_level);
		break;
	case CHC::BISHOP:
		_abilities[CAB::BASE_DISPELL] = current_level >= 4 ? 50 + (5 * current_level) - 20 : 0;
		break;
	case CHC::LORD:
		_abilities[CAB::BASE_DISPELL] = current_level >= 9 ? 50 + (5 * current_level) - 40 : 0;
		break;
	default:
		_abilities[CAB::BASE_DISPELL] = 0;
		break;
	}
	if (_abilities[CAB::BASE_DISPELL] > 100)
		_abilities[CAB::BASE_DISPELL] = 100;

	// Vitality Bonus (num)
	switch (_cur_attr[CAR::VITALITY]) {
	case 3:
		_abilities[CAB::VITALITY_BONUS] = -2;
		break;
	case 4:
	case 5:
		_abilities[CAB::VITALITY_BONUS] = -1;
		break;
	case 16:
		_abilities[CAB::VITALITY_BONUS] = 1;
		break;
	case 17:
		_abilities[CAB::VITALITY_BONUS] = 2;
		break;
	case 18:
	case 19:
	case 20:
		_abilities[CAB::VITALITY_BONUS] = 3;
		break;
	default:
		_abilities[CAB::VITALITY_BONUS] = 0;
		break;
	}

	// Bonus Hit Points per level (num)
	_abilities[CAB::BONUS_HIT_POINTS] = _abilities[CAB::VITALITY_BONUS];

	// Class Change doesn't reset these
	if (!change_class) {

		// Base Hit Points (num) - note initially all non-legated characters get 8 HP as per the PSX versions
		if (initial) {
			const auto bonus{_abilities[CAB::BONUS_HIT_POINTS]};
			switch (auto chance{(*_system->random)[RNT::D100]}; _class) { // NOLINT(clang-diagnostic-switch)
			case CHC::FIGHTER:
			case CHC::LORD:
				_abilities[CAB::MAX_HP] = chance <= 50 ? 10 + bonus : 9 * (10 + bonus) / 10;
				break;
			case CHC::PRIEST:
				_abilities[CAB::MAX_HP] = chance <= 50 ? 8 + bonus : 8 * (10 + bonus) / 10;
				break;
			case CHC::THIEF:
			case CHC::BISHOP:
			case CHC::NINJA:
				_abilities[CAB::MAX_HP] = chance <= 50 ? 6 + bonus : 6 * (10 + bonus) / 10;
				break;
			case CHC::MAGE:
				_abilities[CAB::MAX_HP] = chance <= 50 ? 4 + bonus : 4 * (10 + bonus) / 10;
				break;
			case CHC::SAMURAI:
				_abilities[CAB::MAX_HP] = chance <= 50 ? 16 + bonus : 16 * (10 + bonus) / 10;
				break;
			default:
				break;
			}
			if (_abilities[CAB::MAX_HP] < 1)
				_abilities[CAB::MAX_HP] = 1;

			_abilities[CAB::CURRENT_HP] = _abilities[CAB::MAX_HP];
		}

		if (legate) {
			switch (_class) {
			case CHC::FIGHTER:
				_abilities[CAB::MAX_HP] = 10;
				break;
			case CHC::SAMURAI:
			case CHC::LORD:
				_abilities[CAB::MAX_HP] = 12;
				break;
			case CHC::PRIEST:
				_abilities[CAB::MAX_HP] = 8;
				break;
			case CHC::NINJA:
				_abilities[CAB::MAX_HP] = 7;
				break;
			case CHC::THIEF:
				_abilities[CAB::MAX_HP] = 6;
				break;
			case CHC::MAGE:
				_abilities[CAB::MAX_HP] = 4;
				break;
			default:
				break;
			}

			_abilities[CAB::CURRENT_HP] = _abilities[CAB::MAX_HP];
		}
	}

	// Chance of resurrecting a Dead Character at the Temple (%)
	_abilities[CAB::DEAD_RESURRECT] = 50 + (3 * _cur_attr[CAR::VITALITY]);
	if (_abilities[CAB::DEAD_RESURRECT] > 100)
		_abilities[CAB::DEAD_RESURRECT] = 100;

	// Chance of resurrecting an Ashed Character at the Temple (%)
	_abilities[CAB::ASHES_RESURRECT] = 40 + (3 * _cur_attr[CAR::VITALITY]);
	if (_abilities[CAB::ASHES_RESURRECT] > 100)
		_abilities[CAB::ASHES_RESURRECT] = 100;

	// Chance of resurrecting by a DI or KADORTO spell cast by another Character (%)
	_abilities[CAB::DI_KADORTO_RESURRECT] = 4 * _cur_attr[CAR::VITALITY];

	// Initiative Modifier (num)
	switch (_cur_attr[CAR::AGILITY]) {
	case 3:
		_abilities[CAB::INITIATIVE_MODIFIER] = 3;
		break;
	case 4:
	case 5:
		_abilities[CAB::INITIATIVE_MODIFIER] = 2;
		break;
	case 6:
	case 7:
		_abilities[CAB::INITIATIVE_MODIFIER] = 1;
		break;
	case 15:
		_abilities[CAB::INITIATIVE_MODIFIER] = -1;
		break;
	case 16:
		_abilities[CAB::INITIATIVE_MODIFIER] = -2;
		break;
	case 17:
		_abilities[CAB::INITIATIVE_MODIFIER] = -3;
		break;
	case 18:
	case 19:
	case 20:
		_abilities[CAB::INITIATIVE_MODIFIER] = -4;
		break;
	default:
		_abilities[CAB::INITIATIVE_MODIFIER] = 0;
		break;
	}

	// Armour Class (num) (before equipment)
	_abilities[CAB::BASE_ARMOUR_CLASS] = _class == CHC::NINJA ? 10 - (current_level / 3) - 2 : 10;

	// Don't use this!
	_abilities[CAB::CURRENT_ARMOUR_CLASS] = _abilities[CAB::BASE_ARMOUR_CLASS];

	// Critical Hit Chance (%)
	_abilities[CAB::BASE_CRITICAL_HIT] = _class == CHC::NINJA ? 2 * current_level : 0;
	if (_abilities[CAB::BASE_CRITICAL_HIT] > 50)
		_abilities[CAB::BASE_CRITICAL_HIT] = 50;

	// Chance to identify a Trap (%)
	if (_class == CHC::THIEF)
		_abilities[CAB::IDENTIFY_TRAP] = 6 * _cur_attr[CAR::AGILITY];
	else if (_class == CHC::NINJA)
		_abilities[CAB::IDENTIFY_TRAP] = 4 * _cur_attr[CAR::AGILITY];
	else
		_abilities[CAB::IDENTIFY_TRAP] = _cur_attr[CAR::AGILITY];
	if (_abilities[CAB::IDENTIFY_TRAP] > 95)
		_abilities[CAB::IDENTIFY_TRAP] = 95;

	// Base Chance to Disarm a Trap (modified by Maze Level) (%)
	_abilities[CAB::BASE_DISARM_TRAP] =
		(_class == CHC::NINJA) || (_class == CHC::THIEF) ? ((current_level + 50) / 69.0) * 100 : 0;

	// Chance to activate a Trap if identify fails (%)
	_abilities[CAB::ACTIVATE_TRAP] =
		(_class == CHC::NINJA) || (_class == CHC::THIEF) ? 100 - ((_cur_attr[CAR::AGILITY] / 20.0) * 100) : 100;

	// Base Chance to avoid following into a Pit (modified by Maze Level) (%)
	_abilities[CAB::BASE_AVOID_PIT] = (_cur_attr[CAR::AGILITY] / 25.0) * 100;

	// Base Resist Bonus (d20)
	_abilities[CAB::BASE_RESIST_BONUS] = 1 * (current_level / 5);
	if (_cur_attr[CAR::LUCK] >= 18)
		_abilities[CAB::BASE_RESIST_BONUS] += 3;
	else if (_cur_attr[CAR::LUCK] >= 12)
		_abilities[CAB::BASE_RESIST_BONUS] += 2;
	else if (_cur_attr[CAR::LUCK] >= 6)
		_abilities[CAB::BASE_RESIST_BONUS] += 1;

	// Chance equipment is intact on a corpse TODO: check this is accurate
	_abilities[CAB::EQUIPMENT_INTACT_ON_WIPE] = (_cur_attr[CAR::LUCK] / 21.0f) * 100;

	// Other Resists (d20)
	const auto brb{_abilities[CAB::BASE_RESIST_BONUS]};
	switch (_class) { // NOLINT(clang-diagnostic-switch)
	case CHC::SAMURAI:
		_abilities[CAB::RESISTANCE_VS_POISON_PARALYSIS] = brb + 2;
		_abilities[CAB::RESISTANCE_VS_CRITICAL_HIT] = brb + 2;
		_abilities[CAB::RESISTANCE_VS_STONING] = 0;
		_abilities[CAB::RESISTANCE_VS_BREATH_ATTACKS] = 0;
		_abilities[CAB::RESISTANCE_VS_POISON_GAS_TRAP] = 0;
		_abilities[CAB::RESISTANCE_VS_MAGE_PRIEST_TRAP] = brb + 2;
		_abilities[CAB::RESISTANCE_VS_SILENCE] = brb + 2;
		break;
	case CHC::FIGHTER:
		_abilities[CAB::RESISTANCE_VS_POISON_PARALYSIS] = brb + 3;
		_abilities[CAB::RESISTANCE_VS_CRITICAL_HIT] = brb + 3;
		_abilities[CAB::RESISTANCE_VS_STONING] = 0;
		_abilities[CAB::RESISTANCE_VS_BREATH_ATTACKS] = 0;
		_abilities[CAB::RESISTANCE_VS_POISON_GAS_TRAP] = 0;
		_abilities[CAB::RESISTANCE_VS_MAGE_PRIEST_TRAP] = 0;
		_abilities[CAB::RESISTANCE_VS_SILENCE] = 0;
		break;
	case CHC::PRIEST:
		_abilities[CAB::RESISTANCE_VS_POISON_PARALYSIS] = 0;
		_abilities[CAB::RESISTANCE_VS_CRITICAL_HIT] = 0;
		_abilities[CAB::RESISTANCE_VS_STONING] = brb + 3;
		_abilities[CAB::RESISTANCE_VS_BREATH_ATTACKS] = 0;
		_abilities[CAB::RESISTANCE_VS_POISON_GAS_TRAP] = 0;
		_abilities[CAB::RESISTANCE_VS_MAGE_PRIEST_TRAP] = 0;
		_abilities[CAB::RESISTANCE_VS_SILENCE] = 0;
		break;
	case CHC::LORD:
		_abilities[CAB::RESISTANCE_VS_POISON_PARALYSIS] = brb + 2;
		_abilities[CAB::RESISTANCE_VS_CRITICAL_HIT] = brb + 2;
		_abilities[CAB::RESISTANCE_VS_STONING] = brb + 2;
		_abilities[CAB::RESISTANCE_VS_BREATH_ATTACKS] = 0;
		_abilities[CAB::RESISTANCE_VS_POISON_GAS_TRAP] = 0;
		_abilities[CAB::RESISTANCE_VS_MAGE_PRIEST_TRAP] = 0;
		_abilities[CAB::RESISTANCE_VS_SILENCE] = 0;
		break;
	case CHC::BISHOP:
		_abilities[CAB::RESISTANCE_VS_POISON_PARALYSIS] = 0;
		_abilities[CAB::RESISTANCE_VS_CRITICAL_HIT] = 0;
		_abilities[CAB::RESISTANCE_VS_STONING] = brb + 2;
		_abilities[CAB::RESISTANCE_VS_BREATH_ATTACKS] = brb + 2;
		_abilities[CAB::RESISTANCE_VS_POISON_GAS_TRAP] = 0;
		_abilities[CAB::RESISTANCE_VS_MAGE_PRIEST_TRAP] = brb + 2;
		_abilities[CAB::RESISTANCE_VS_SILENCE] = brb + 2;
		break;
	case CHC::THIEF:
		_abilities[CAB::RESISTANCE_VS_POISON_PARALYSIS] = 0;
		_abilities[CAB::RESISTANCE_VS_CRITICAL_HIT] = 0;
		_abilities[CAB::RESISTANCE_VS_STONING] = 0;
		_abilities[CAB::RESISTANCE_VS_BREATH_ATTACKS] = 0;
		_abilities[CAB::RESISTANCE_VS_POISON_GAS_TRAP] = brb + 3;
		_abilities[CAB::RESISTANCE_VS_MAGE_PRIEST_TRAP] = 0;
		_abilities[CAB::RESISTANCE_VS_SILENCE] = 0;
		break;
	case CHC::NINJA:
		_abilities[CAB::RESISTANCE_VS_POISON_PARALYSIS] = brb + 3;
		_abilities[CAB::RESISTANCE_VS_CRITICAL_HIT] = brb + 3;
		_abilities[CAB::RESISTANCE_VS_STONING] = brb + 2;
		_abilities[CAB::RESISTANCE_VS_BREATH_ATTACKS] = brb + 4;
		_abilities[CAB::RESISTANCE_VS_POISON_GAS_TRAP] = brb + 3;
		_abilities[CAB::RESISTANCE_VS_MAGE_PRIEST_TRAP] = brb + 2;
		_abilities[CAB::RESISTANCE_VS_SILENCE] = brb + 2;
		break;
	case CHC::MAGE:
		_abilities[CAB::RESISTANCE_VS_POISON_PARALYSIS] = 0;
		_abilities[CAB::RESISTANCE_VS_CRITICAL_HIT] = 0;
		_abilities[CAB::RESISTANCE_VS_STONING] = 0;
		_abilities[CAB::RESISTANCE_VS_BREATH_ATTACKS] = 0;
		_abilities[CAB::RESISTANCE_VS_POISON_GAS_TRAP] = 0;
		_abilities[CAB::RESISTANCE_VS_MAGE_PRIEST_TRAP] = brb + 3;
		_abilities[CAB::RESISTANCE_VS_SILENCE] = brb + 3;
		break;
	default:
		break;
	}
	if (_race == CHR::HUMAN)
		_abilities[CAB::RESISTANCE_VS_POISON_PARALYSIS] += 1;
	if (_race == CHR::HUMAN)
		_abilities[CAB::RESISTANCE_VS_CRITICAL_HIT] += 1;
	if (_race == CHR::GNOME)
		_abilities[CAB::RESISTANCE_VS_STONING] += 2;
	if (_race == CHR::ELF)
		_abilities[CAB::RESISTANCE_VS_BREATH_ATTACKS] += 2;
	if (_race == CHR::DWARF)
		_abilities[CAB::RESISTANCE_VS_POISON_GAS_TRAP] += 4;
	if (_race == CHR::HOBBIT) {
		_abilities[CAB::RESISTANCE_VS_MAGE_PRIEST_TRAP] += 2;
		_abilities[CAB::RESISTANCE_VS_SILENCE] += 3;
	}
	if (_abilities[CAB::RESISTANCE_VS_POISON_PARALYSIS] > 19)
		_abilities[CAB::RESISTANCE_VS_POISON_PARALYSIS] = 19;
	if (_abilities[CAB::RESISTANCE_VS_CRITICAL_HIT] > 19)
		_abilities[CAB::RESISTANCE_VS_CRITICAL_HIT] = 19;
	if (_abilities[CAB::RESISTANCE_VS_STONING] > 19)
		_abilities[CAB::RESISTANCE_VS_STONING] = 19;
	if (_abilities[CAB::RESISTANCE_VS_BREATH_ATTACKS] > 19)
		_abilities[CAB::RESISTANCE_VS_BREATH_ATTACKS] = 19;
	if (_abilities[CAB::RESISTANCE_VS_POISON_GAS_TRAP] > 19)
		_abilities[CAB::RESISTANCE_VS_POISON_GAS_TRAP] = 19;
	if (_abilities[CAB::RESISTANCE_VS_MAGE_PRIEST_TRAP] > 19)
		_abilities[CAB::RESISTANCE_VS_MAGE_PRIEST_TRAP] = 19;
	if (_abilities[CAB::RESISTANCE_VS_SILENCE] > 19)
		_abilities[CAB::RESISTANCE_VS_SILENCE] = 19;

	// Resistance to harmful spells (%)
	_abilities[CAB::RESISTANCE_VS_KATINO] = current_level * 10;
	if (_abilities[CAB::RESISTANCE_VS_KATINO] > 100)
		_abilities[CAB::RESISTANCE_VS_KATINO] = 100;
	_abilities[CAB::RESISTANCE_VS_BADI] = current_level * 10;
	if (_abilities[CAB::RESISTANCE_VS_BADI] > 100)
		_abilities[CAB::RESISTANCE_VS_BADI] = 100;
	_abilities[CAB::RESISTANCE_VS_MANIFO] = 50 + (current_level * 10);
	if (_abilities[CAB::RESISTANCE_VS_MANIFO] > 100)
		_abilities[CAB::RESISTANCE_VS_MANIFO] = 100;

	_abilities[CAB::RECOVER_FROM_SLEEP] = current_level * 10;
	if (_abilities[CAB::RECOVER_FROM_SLEEP] > 100)
		_abilities[CAB::RECOVER_FROM_SLEEP] = 100;

	_abilities[CAB::RECOVER_FROM_FEAR] = current_level * 5;
	if (_abilities[CAB::RECOVER_FROM_FEAR] > 100)
		_abilities[CAB::RECOVER_FROM_FEAR] = 100;

	_abilities[CAB::BONUS_MAGE_SPELLS] = 0;
	_abilities[CAB::BONUS_PRIEST_SPELLS] = 0;

	// And set poison/regeneration to default
	if (!change_class) {
		_abilities[CAB::HP_LOSS_PER_TURN] = 0;
		_abilities[CAB::HP_GAIN_PER_TURN] = 0;
		_abilities[CAB::POISON_STRENGTH] = 0;
	}
}

auto Sorcery::Character::grant_xp(const int adjustment) -> int {

	_abilities[CAB::CURRENT_XP] = _abilities[CAB::CURRENT_XP] + adjustment;

	return _abilities[CAB::CURRENT_XP];
}

// Now work out spellpoints!
auto Sorcery::Character::_reset_starting_sp() -> void {

	_set_starting_sp();

	// And add in 1 spell point for each known spell NOT of the type
	for (auto spell_level = 1u; spell_level <= 7u; spell_level++) {

		// Handle Priest Spells
		auto priest_known{static_cast<unsigned int>(std::count_if(_spells.begin(), _spells.end(),
			[=](auto spell) { return (spell.type == SPT::PRIEST) && (spell.level == spell_level) && (spell.known); }))};

		if (_priest_max_sp[spell_level] < priest_known) {
			_priest_max_sp[spell_level] = priest_known;
			_priest_cur_sp[spell_level] = _priest_max_sp[spell_level];
		}

		// Handle Mage Spells
		auto mage_known{static_cast<unsigned int>(std::count_if(_spells.begin(), _spells.end(),
			[=](auto spell) { return (spell.type == SPT::MAGE) && (spell.level == spell_level) && (spell.known); }))};
		if (_mage_max_sp[spell_level] < mage_known) {
			_mage_max_sp[spell_level] = mage_known;
			_mage_cur_sp[spell_level] = _mage_max_sp[spell_level];
		}
	}
}

// Set the starting spellpoints
auto Sorcery::Character::_set_starting_sp() -> void {

	// By default clear all spells
	_clear_sp();

	// In the original code this is handled in "SETSPELS"/"SPLPERLV"/"NWMAGE"/"NWPRIEST"
	switch (_class) { // NOLINT(clang-diagnostic-switch)
	case CHC::PRIEST:
		_priest_max_sp[1] = (*_system->config)[CFG::STRICT_MODE] ? 2 : 2 + _abilities[CAB::BONUS_PRIEST_SPELLS];
		break;
	case CHC::BISHOP:
		_mage_max_sp[1] = 2;
		break;
	case CHC::MAGE:
		_mage_max_sp[1] = (*_system->config)[CFG::STRICT_MODE] ? 2 : 2 + _abilities[CAB::BONUS_MAGE_SPELLS];
		break;
	default:
		break;
	}

	_priest_cur_sp[1] = _priest_max_sp[1];
	_mage_cur_sp[1] = _mage_max_sp[1];
}

// Clear all spell-points to begin with
auto Sorcery::Character::_clear_sp() -> void {

	for (auto spell_level = 1; spell_level <= 7; spell_level++) {
		_priest_max_sp[spell_level] = 0;
		_priest_cur_sp[spell_level] = 0;
		_mage_max_sp[spell_level] = 0;
		_mage_cur_sp[spell_level] = 0;
	}
}

// Reset Starting Spells on Class Change
auto Sorcery::Character::_reset_start_spells() -> void {

	// All known spells are kept, and new ones added as per the starting class guildlines
	_set_start_spells();
}

auto Sorcery::Character::_learn_spell(SPI spell_id) -> void {

	std::vector<Spell>::iterator it;
	it = std::find_if(_spells.begin(), _spells.end(), [&](auto item) { return item.id == spell_id; });
	if (it != _spells.end()) {
		(*it).known = true;
		_spells_known[(*it).id] = true;
	} // NOLINT(clang-di
}

// Set starting spells
auto Sorcery::Character::_set_start_spells() -> void {

	// This is taken from "KEEPCHYN" which hard codes the spells known to beginning characters!
	switch (_class) { // NOLINT(clang-diagnostic-switch)
	case CHC::BISHOP:
	case CHC::MAGE:
		_learn_spell(SPI::KATINO);
		_learn_spell(SPI::HALITO);
		break;
	case CHC::PRIEST:
		_learn_spell(SPI::DIOS);
		_learn_spell(SPI::BADIOS);
		break;
	default:
		break;
	}
}

// Get HP gained for all levels apart from the first
auto Sorcery::Character::_get_hp_per_level() -> int {

	// In the original code ("MOREHP"), Samurai get 2d8
	auto extra_hp{0};
	switch (_class) { // NOLINT(clang-diagnostic-switch)
	case CHC::FIGHTER:
	case CHC::LORD:
		extra_hp += (*_system->random)[RNT::D10];
		break;
	case CHC::PRIEST:
		extra_hp += (*_system->random)[RNT::D8];
		break;
	case CHC::THIEF:
	case CHC::BISHOP:
	case CHC::NINJA:
		extra_hp += (*_system->random)[RNT::D6];
		break;
	case CHC::MAGE:
		extra_hp += (*_system->random)[RNT::D4];
		break;
	case CHC::SAMURAI:
		extra_hp += (*_system->random)[RNT::D8];
		extra_hp += (*_system->random)[RNT::D8];
		break;
	default:
		break;
	}

	extra_hp += _abilities[CAB::BONUS_HIT_POINTS];
	if (extra_hp < 0)
		extra_hp = 1;

	// Though this could be an unsigned int as it will always be greater than 0, just return int as everything else is
	return extra_hp;
}

// Add hit points on level gain (but note the strict mode limitation mentioned below)
auto Sorcery::Character::_update_hp_for_level() -> int {

	// Note the rerolling of all HP ("MADELEV") when levelling - and using MaxLevel achieved when in strict mode
	auto hp_gained{0};
	if ((*_system->config)[CFG::REROLL_HIT_POINTS_ON_LEVEL_GAIN]) {
		auto hp_total{0};
		for (auto level = 1; level < _abilities[CAB::CURRENT_LEVEL]; level++)
			hp_total += _get_hp_per_level();
		if (hp_total < _abilities[CAB::MAX_HP])
			hp_total = _abilities[CAB::MAX_HP] + 1;
		hp_gained = hp_total - _abilities[CAB::MAX_HP];
		_abilities[CAB::MAX_HP] += hp_gained;
		_abilities[CAB::CURRENT_HP] += hp_gained;
	} else {
		hp_gained = _get_hp_per_level();
		_abilities[CAB::MAX_HP] += hp_gained;
		_abilities[CAB::CURRENT_HP] += hp_gained;
	}

	return hp_gained;
}

auto Sorcery::Character::get_current_hp() const -> int {

	return _abilities.at(CAB::CURRENT_HP);
}

auto Sorcery::Character::get_max_hp() const -> int {

	return _abilities.at(CAB::MAX_HP);
}

auto Sorcery::Character::_update_stat_for_level(CAR attribute, std::string stat) -> std::string {

	auto message{""s};

	if ((*_system->random)[RNT::D100] < 75) {
		const auto chance{_abilities.at(CAB::AGE) / 130.f};
		if ((*_system->random)[RNT::D100] < chance) {

			// Decrease
			bool proceed{true};
			if (_cur_attr.at(attribute) == 18 && (*_system->random)[RNT::D6] > 1)
				proceed = false;

			if (proceed) {
				_cur_attr.at(attribute) = _cur_attr.at(attribute) - 1;
				message = fmt::format("{} {}", (*_display->string)["LEVEL_LOSS"], stat);
				if (_cur_attr.at(attribute) < 1)
					_cur_attr.at(attribute) = 1;
			}
		} else {
			if (_cur_attr.at(attribute) < 18) {
				_cur_attr.at(attribute) = _cur_attr.at(attribute) + 1;
				if (_cur_attr.at(attribute) > _max_attr.at(attribute))
					_max_attr.at(attribute) = _cur_attr.at(attribute);
				message = fmt::format("{} {}", (*_display->string)["LEVEL_GAIN"], stat);
			}
		}
	}

	if (_cur_attr.at(attribute) > _max_attr.at(attribute))
		_max_attr.at(attribute) = _cur_attr.at(attribute);

	return message;
}

// Level a character up
auto Sorcery::Character::level_up() -> std::string {

	std::string results{};
	results.append((*_display->string)["LEVEL_DING"]);
	results.append("@");

	// Increase level
	_abilities.at(CAB::CURRENT_LEVEL) = _abilities.at(CAB::CURRENT_LEVEL) + 1;
	_abilities.at(CAB::HIT_DICE) = _abilities.at(CAB::HIT_DICE) + 1;
	if (_abilities.at(CAB::CURRENT_LEVEL) > _abilities.at(CAB::MAX_LEVEL))
		_abilities.at(CAB::MAX_LEVEL) = _abilities.at(CAB::CURRENT_LEVEL);

	// Handle learning spells
	if (_set_sp()) {
		results.append((*_display->string)["LEVEL_SPELLS"]);
		results.append("@");
	}

	// Work out new xp needed
	_abilities[CAB::NEXT_LEVEL_XP] = _get_xp_for_level(_abilities[CAB::CURRENT_LEVEL]);

	// Handle stat changing
	auto stat_message{""s};
	stat_message = _update_stat_for_level(CAR::STRENGTH, (*_display->string)["CHARACTER_STAT_STRENGTH"]);
	if (!stat_message.empty()) {
		results.append(stat_message);
		results.append("@");
	}
	stat_message = _update_stat_for_level(CAR::IQ, (*_display->string)["CHARACTER_STAT_INTELLIGENCE"]);
	if (!stat_message.empty()) {

		results.append(stat_message);
		results.append("@");
	}
	stat_message = _update_stat_for_level(CAR::PIETY, (*_display->string)["CHARACTER_STAT_PIETY"]);
	if (!stat_message.empty()) {
		results.append("@");
	}
	stat_message = _update_stat_for_level(CAR::VITALITY, (*_display->string)["CHARACTER_STAT_VITALITY"]);
	if (!stat_message.empty()) {
		results.append(stat_message);
		results.append("@");
	}
	stat_message = _update_stat_for_level(CAR::AGILITY, (*_display->string)["CHARACTER_STAT_AGILITY"]);
	if (!stat_message.empty()) {
		results.append("@");
	}
	stat_message = _update_stat_for_level(CAR::LUCK, (*_display->string)["CHARACTER_STAT_LUCK"]);
	if (!stat_message.empty()) {
		results.append(stat_message);
		results.append("@");
	}

	// Level everything else up
	_generate_secondary_abil(false, false, false);

	// handle hp
	const auto hp_gained{_update_hp_for_level()};
	const auto hp_message{fmt::format(
		"{} {} {}", (*_display->string)["LEVEL_HP_PREFIX"], hp_gained, (*_display->string)["LEVEL_HP_SUFFIX"])};
	results.append(hp_message);

	if (_cur_attr.at(CAR::VITALITY) < 3) {
		results.append("@");
		results.append((*_display->string)["LEVEL_DIE"]);
		_status = CHT::LOST;
		_location = CHL::TRAINING;
	}

	return results;
}

// Level a character down (e.g. drain levels or give/increase negative levels_
auto Sorcery::Character::level_down() -> void {

	if (_abilities.at(CAB::CURRENT_LEVEL) == 1) {
		_status = CHT::LOST;
		_location = CHL::TRAINING;
		return;
	}

	// TODO: What to do about negative level ability?
	const auto old_level{_abilities.at(CAB::CURRENT_LEVEL)};
	const auto diff_hp{_abilities.at(CAB::MAX_HP) - _abilities.at(CAB::CURRENT_HP)};

	_abilities.at(CAB::CURRENT_LEVEL) = _abilities.at(CAB::CURRENT_LEVEL) - 1;
	_abilities.at(CAB::HIT_DICE) = _abilities.at(CAB::HIT_DICE) - 1;
	if (_abilities.at(CAB::MAX_LEVEL) > _abilities.at(CAB::CURRENT_LEVEL))
		_abilities.at(CAB::MAX_LEVEL) = _abilities.at(CAB::CURRENT_LEVEL);

	_set_sp();

	// When drained XP is set to beginning of current level (for example, draining to level 9 means that youre xp is set
	// to enough for level 9 plus 1 - which is vety harsh)
	_abilities[CAB::NEXT_LEVEL_XP] = _get_xp_for_level(_abilities.at(CAB::CURRENT_LEVEL));
	_abilities[CAB::CURRENT_XP] = _get_xp_for_level(_abilities.at(CAB::CURRENT_LEVEL) - 1) + 1;

	_generate_secondary_abil(false, false, false);
	_abilities[CAB::MAX_HP] = _abilities.at(CAB::MAX_HP) * (_abilities.at(CAB::CURRENT_LEVEL) / (old_level * 1.f));
	_abilities[CAB::CURRENT_HP] = _abilities[CAB::MAX_HP] - diff_hp;
	if (_abilities[CAB::CURRENT_HP] < 0)
		_abilities[CAB::CURRENT_HP] = 0;
}

// For each spell level, try to learn spells - called before set_spellpoints
auto Sorcery::Character::_try_learn_spell(SPT spell_type, unsigned int spell_level) -> bool {

	bool new_spell_learnt{false};

	// Only do spells if a character has available spell points in this spell level
	if (spell_type == SPT::PRIEST)
		if (_priest_max_sp[spell_level] == 0)
			return false;
	if (spell_type == SPT::MAGE)
		if (_mage_max_sp[spell_level] == 0)
			return false;

	// Get all unknown spells of this spell level and type
	auto spells{_spells | std::views::filter([&](Spell spell) {
		return (spell.type == spell_type) && (spell.level == spell_level) && (spell.known == false);
	})};
	for (auto &spell : spells) {

		const auto dice{(*_system->random)[RNT::ZERO_TO_29]};

		// Check the Spell Type against the relevant stat (see SPLPERLV//TRYLEARN)
		if (spell_type == SPT::PRIEST) {
			if (dice <= static_cast<unsigned int>(_cur_attr[CAR::PIETY])) {
				spell.known = true;
				_spells_known[spell.id] = true;
				new_spell_learnt = true;
			}
		}
		if (spell_type == SPT::MAGE) {
			if (dice <= static_cast<unsigned int>(_cur_attr[CAR::IQ])) {
				spell.known = true;
				_spells_known[spell.id] = true;
				new_spell_learnt = true;
			}
		}
	}

	return new_spell_learnt;
}

// Reimplementation of SPLPERLV - note this will reset spell points!
auto Sorcery::Character::_calculate_sp(SPT spell_type, unsigned int level_mod, unsigned int level_offset) -> void {

	SpellPoints *spells{spell_type == SPT::PRIEST ? &_priest_max_sp : &_mage_max_sp};
	for (auto spell_level = 1; spell_level <= 7; spell_level++)
		(*spells)[spell_level] = 0;

	// https://datadrivengamer.blogspot.com/2019/08/the-not-so-basic-mechanics-of-wizardry.html
	for (auto spell_level = 1; spell_level <= 7; spell_level++) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnarrowing"
		int spell_count{_abilities[CAB::CURRENT_LEVEL] - level_mod + level_offset - (level_offset * spell_level)};
#pragma GCC diagnostic pop
		spell_count = std::max(0, std::min(spell_count, 9));
		(*spells)[spell_level] = spell_count;
	}
}

// Copied and rewritten from the original code from MINMAG/MINPRI/NWPRIEST/NWMAGE
auto Sorcery::Character::_set_sp() -> bool {

	bool new_spells_learnt{false};

	for (auto spell_level = 1; spell_level <= 7; spell_level++) {
		_priest_max_sp[spell_level] = 0;
		_mage_max_sp[spell_level] = 0;
	}

	// Generate spell points according to current level and class (this does not change any spells known but will reset
	// spell points)
	switch (_class) { // NOLINT(clang-diagnostic-switch)
	case CHC::FIGHTER:
	case CHC::THIEF:
	case CHC::NINJA:
		break;
	case CHC::MAGE:
		_calculate_sp(SPT::MAGE, 0, 2);
		break;
	case CHC::PRIEST:
		_calculate_sp(SPT::PRIEST, 0, 2);
		break;
	case CHC::BISHOP:
		_calculate_sp(SPT::PRIEST, 3, 4);
		_calculate_sp(SPT::MAGE, 0, 4);
		break;
	case CHC::SAMURAI:
		_calculate_sp(SPT::MAGE, 3, 3);
		break;
	case CHC::LORD:
		_calculate_sp(SPT::PRIEST, 3, 2);
		break;
	default:
		break;
	}

	// Now try and learn any additional spells
	for (auto spell_level = 1; spell_level <= 7; spell_level++) {

		// If we know at least one spell in this level, we can always try and learn more no matter what even if we are
		// currently a non-spellcasting class
		if (_priest_max_sp[spell_level] > 0) {
			if (_try_learn_spell(SPT::PRIEST, spell_level))
				new_spells_learnt = true;
		}
		if (_mage_max_sp[spell_level] > 0) {
			if (_try_learn_spell(SPT::MAGE, spell_level))
				new_spells_learnt = true;
		}
	}

	// Now make sure that if the above fails, we always learn a spell of each circle just in case (GETMAGSP/GETPRISP -
	// though note that the orignal is bugged and selects the wrong level spells sometimes) - note this is for creatures
	// so it might not be applicable?
	for (auto spell_level = 1; spell_level <= 7; spell_level++) {

		if (_priest_max_sp[spell_level] > 0 && _get_spells_known(SPT::PRIEST, spell_level) == 0) {
			switch (spell_level) {
			case 1:
				_learn_spell(SPI::BADIOS);
				break;
			case 2:
				_learn_spell(SPI::MONTINO);
				new_spells_learnt = true;
				break;
			case 3:
				if ((*_system->random)[RNT::D100] > 33)
					_learn_spell(SPI::DIALKO);
				else
					_learn_spell(SPI::LOMILWA);
				new_spells_learnt = true;
				break;
			case 4:
				_learn_spell(SPI::BADIAL);
				new_spells_learnt = true;
				break;
			case 5:
				if ((*_system->random)[RNT::D100] > 33)
					_learn_spell(SPI::BADIALMA);
				else
					_learn_spell(SPI::BADI);
				new_spells_learnt = true;
				break;
			case 6:
				if ((*_system->random)[RNT::D100] > 33)
					_learn_spell(SPI::LORTO);
				else
					_learn_spell(SPI::MABADI);
				new_spells_learnt = true;
				break;
			case 7:
				_learn_spell(SPI::MALIKTO);
				new_spells_learnt = true;
				break;
			default:
				break;
			}
		}
		if (_mage_max_sp[spell_level] > 0 && _get_spells_known(SPT::MAGE, spell_level) == 0) {
			switch (spell_level) {
			case 1:
				if ((*_system->random)[RNT::D100] > 33)
					_learn_spell(SPI::KATINO);
				else
					_learn_spell(SPI::HALITO);
				new_spells_learnt = true;
				break;
			case 2:
				if ((*_system->random)[RNT::D100] > 33)
					_learn_spell(SPI::DILTO);
				else
					_learn_spell(SPI::SOPIC);
				new_spells_learnt = true;
				break;
			case 3:
				if ((*_system->random)[RNT::D100] > 33)
					_learn_spell(SPI::MOLITO);
				else
					_learn_spell(SPI::MAHALITO);
				new_spells_learnt = true;
				break;
			case 4:
				if ((*_system->random)[RNT::D100] > 33)
					_learn_spell(SPI::DALTO);
				else
					_learn_spell(SPI::LAHALITO);
				new_spells_learnt = true;
				break;
			case 5:
				if ((*_system->random)[RNT::D100] > 33)
					_learn_spell(SPI::MAMORLIS);
				else
					_learn_spell(SPI::MADALTO);
				new_spells_learnt = true;
				break;
			case 6:
				if ((*_system->random)[RNT::D100] > 33)
					_learn_spell(SPI::LAKANITO);
				else
					_learn_spell(SPI::ZILWAN);
				new_spells_learnt = true;
				break;
			case 7:
				_learn_spell(SPI::MALOR);
				new_spells_learnt = true;
				break;
			default:
				break;
			}
		}
	}

	// And work out spells known and boost sp accordingly if we have to but note that we can't go above maxlevel/2 (for
	// the case of level drain)
	for (auto spell_level = 1; spell_level <= 7; spell_level++) {

		if ((spell_level * 2) - 1 > _abilities.at(CAB::MAX_LEVEL))
			continue;

		const auto priest_known{_get_spells_known(SPT::PRIEST, spell_level)};
		const auto mage_known{_get_spells_known(SPT::MAGE, spell_level)};

		if (priest_known > _priest_max_sp[spell_level])
			_priest_max_sp[spell_level] = priest_known;
		if (mage_known > _mage_max_sp[spell_level])
			_mage_max_sp[spell_level] = mage_known;
	}

	// level up only takes place at the inn, so reset spells
	for (auto spell_level = 1; spell_level <= 7; spell_level++) {
		_priest_cur_sp[spell_level] = _priest_max_sp[spell_level];
		_mage_cur_sp[spell_level] = _mage_max_sp[spell_level];
	}

	return new_spells_learnt;
}

// In the original code this is from SPLPERLV
auto Sorcery::Character::_get_spells_known(SPT spell_type, unsigned int spell_level) -> unsigned int {

	return std::ranges::count_if(_spells.begin(), _spells.end(),
		[&](auto spell) { return spell.type == spell_type && spell.level == spell_level && spell.known; });
}

// Given a level, get the XP needed for it
auto Sorcery::Character::_get_xp_for_level(unsigned int level) const -> int {

	// XP values are obtained directly from original Apple2 Scenario Disc (look for E8 03 00 00 00 00 near &00020106
	// though due to what I think is the way USCD pascal stores large numbers, they are stored in 16 bit LSB "chunks",
	// for example, 134586 is stored at &0002013C as EA 11 0D, or 4586 - to get the actual value for the level we add
	// this to 0D in decimal (13) times 10000, to get 134586.
	static const std::array<std::array<int, 14>, 8> levels{
		{{0, 1000, 1724, 2972, 5124, 8834, 15231, 26260, 45275, 78060, 134586, 232044, 400075, 289709},		 // FIGHTER
			{0, 1100, 1896, 3268, 5124, 9713, 16746, 28872, 49779, 85825, 147974, 255127, 439874, 318529},	 // MAGE
			{0, 1050, 1810, 3120, 5379, 9274, 15989, 27567, 47529, 81946, 141286, 243596, 419993, 304132},	 // PRIEST
			{0, 900, 1551, 2574, 4610, 7948, 13703, 23625, 40732, 70187, 121081, 208750, 359931, 260639},	 // THIEF
			{0, 1000, 2105, 3692, 6477, 11363, 19935, 34973, 61136, 107642, 188845, 331370, 481240, 438479}, // BISHOP
			{0, 1250, 2192, 3845, 6745, 11833, 20759, 36419, 63892, 112091, 196650, 345000, 605263, 456601}, // SAMURAI
			{0, 1300, 2280, 4000, 7017, 12310, 21596, 37887, 66468, 116610, 204578, 358908, 629663, 475008}, // LORD
			{0, 1450, 2543, 4461, 7826, 13729, 24085, 42254, 74129, 130050, 228157, 400275, 702236, 529756}}}; // NINJA

	// Also found here: http://www.the-spoiler.com/RPG/Sir-Tech/wizardry.1.2.html

	auto xp_needed{0};
	auto c_index{unenum(_class) - 1};
	if (level <= 13)
		xp_needed = levels[c_index][level];
	else
		xp_needed = levels[c_index][12] + ((level - 13) * levels[c_index][13]);

	return xp_needed;
}

auto Sorcery::Character::set_spells() -> void {

	// Now for each spell known set the appropriate entry in the spells table
	for (auto &spell_known : _spells_known) {

		std::vector<Spell>::iterator it;
		it = std::find_if(_spells.begin(), _spells.end(), [&](auto item) { return item.id == spell_known.first; });
		if (it != _spells.end())
			(*it).known = spell_known.second;
	}
}

// Wizardry 1 - 3 Spells
auto Sorcery::Character::create_spells() -> void {
	_spells.clear();

	// Mage Spells (grouped by level)

	// Level 1
	auto level{1};
	_spells.emplace_back(SPI::DUMAPIC, SPT::MAGE, SPC::FIELD, level, false, (*_display->string)["SPELL_DUMAPIC_NAME"],
		(*_display->string)["SPELL_DUMAPIC_TITLE"], (*_display->string)["SPELL_DUMAPIC_DESC"]);
	_spells.emplace_back(SPI::HALITO, SPT::MAGE, SPC::ATTACK, level, false, (*_display->string)["SPELL_HALITO_NAME"],
		(*_display->string)["SPELL_HALITO_TITLE"], (*_display->string)["SPELL_HALITO_DESC"]);
	_spells.emplace_back(SPI::KATINO, SPT::MAGE, SPC::DISABLE, level, false, (*_display->string)["SPELL_KATINO_NAME"],
		(*_display->string)["SPELL_KATINO_TITLE"], (*_display->string)["SPELL_KATINO_DESC"]);
	_spells.emplace_back(SPI::MOGREF, SPT::MAGE, SPC::SUPPORT, level, false, (*_display->string)["SPELL_MOGREF_NAME"],
		(*_display->string)["SPELL_MOGREF_TITLE"], (*_display->string)["SPELL_MOGREF_DESC"]);

	// Level 2
	++level;
	_spells.emplace_back(SPI::DILTO, SPT::MAGE, SPC::DISABLE, level, false, (*_display->string)["SPELL_DILTO_NAME"],
		(*_display->string)["SPELL_DILTO_TITLE"], (*_display->string)["SPELL_DILTO_DESC"]);
	_spells.emplace_back(SPI::SOPIC, SPT::MAGE, SPC::SUPPORT, level, false, (*_display->string)["SPELL_SOPIC_NAME"],
		(*_display->string)["SPELL_SOPIC_TITLE"], (*_display->string)["SPELL_SOPIC_DESC"]);

	// Level 3
	++level;
	_spells.emplace_back(SPI::MAHALITO, SPT::MAGE, SPC::ATTACK, level, false,
		(*_display->string)["SPELL_MAHALITO_NAME"], (*_display->string)["SPELL_MAHALITO_TITLE"],
		(*_display->string)["SPELL_MAHALITO_DESC"]);
	_spells.emplace_back(SPI::MOLITO, SPT::MAGE, SPC::ATTACK, level, false, (*_display->string)["SPELL_MOLITO_NAME"],
		(*_display->string)["SPELL_MOLITO_TITLE"], (*_display->string)["SPELL_MOLITO_DESC"]);

	// Level 4
	++level;
	_spells.emplace_back(SPI::DALTO, SPT::MAGE, SPC::ATTACK, level, false, (*_display->string)["SPELL_DALTO_NAME"],
		(*_display->string)["SPELL_DALTO_TITLE"], (*_display->string)["SPELL_DALTO_DESC"]);
	_spells.emplace_back(SPI::LAHALITO, SPT::MAGE, SPC::ATTACK, level, false,
		(*_display->string)["SPELL_LAHALITO_NAME"], (*_display->string)["SPELL_LAHALITO_TITLE"],
		(*_display->string)["SPELL_LAHALITO_DESC"]);
	_spells.emplace_back(SPI::MORLIS, SPT::MAGE, SPC::DISABLE, level, false, (*_display->string)["SPELL_MORLIS_NAME"],
		(*_display->string)["SPELL_MORLIS_TITLE"], (*_display->string)["SPELL_MORLIS_DESC"]);

	// Level 5
	++level;
	_spells.emplace_back(SPI::MADALTO, SPT::MAGE, SPC::ATTACK, level, false, (*_display->string)["SPELL_MADALTO_NAME"],
		(*_display->string)["SPELL_MADALTO_TITLE"], (*_display->string)["SPELL_MADALTO_DESC"]);
	_spells.emplace_back(SPI::MAKANITO, SPT::MAGE, SPC::ATTACK, level, false,
		(*_display->string)["SPELL_MAKANITO_NAME"], (*_display->string)["SPELL_MAKANITO_TITLE"],
		(*_display->string)["SPELL_MAKANITO_DESC"]);
	_spells.emplace_back(SPI::MAMORLIS, SPT::MAGE, SPC::DISABLE, level, false,
		(*_display->string)["SPELL_MAMORLIS_NAME"], (*_display->string)["SPELL_MAMORLIS_TITLE"],
		(*_display->string)["SPELL_MAMORLIS_DESC"]);

	// Level 6
	++level;
	_spells.emplace_back(SPI::HAMAN, SPT::MAGE, SPC::SUPPORT, level, false, (*_display->string)["SPELL_HAMAN_NAME"],
		(*_display->string)["SPELL_HAMAN_TITLE"], (*_display->string)["SPELL_HAMAN_DESC"]);
	_spells.emplace_back(SPI::LAKANITO, SPT::MAGE, SPC::ATTACK, level, false,
		(*_display->string)["SPELL_LAKANITO_NAME"], (*_display->string)["SPELL_LAKANITO_TITLE"],
		(*_display->string)["SPELL_LAKANITO_DESC"]);
	_spells.emplace_back(SPI::MASOPIC, SPT::MAGE, SPC::ATTACK, level, false, (*_display->string)["SPELL_MASOPIC_NAME"],
		(*_display->string)["SPELL_MASOPIC_TITLE"], (*_display->string)["SPELL_MASOPIC_DESC"]);
	_spells.emplace_back(SPI::ZILWAN, SPT::MAGE, SPC::ATTACK, level, false, (*_display->string)["SPELL_ZILWAN_NAME"],
		(*_display->string)["SPELL_ZILWAN_TITLE"], (*_display->string)["SPELL_ZILWAN_DESC"]);

	// Level 7
	++level;
	_spells.emplace_back(SPI::MAHAMAN, SPT::MAGE, SPC::SUPPORT, level, false, (*_display->string)["SPELL_MAHAMAN_NAME"],
		(*_display->string)["SPELL_MAHAMAN_TITLE"], (*_display->string)["SPELL_MAHAMAN_DESC"]);
	_spells.emplace_back(SPI::MALOR, SPT::MAGE, SPC::FIELD, level, false, (*_display->string)["SPELL_MALOR_NAME"],
		(*_display->string)["SPELL_MALOR_TITLE"], (*_display->string)["SPELL_MALOR_DESC"]);
	_spells.emplace_back(SPI::TILTOWAIT, SPT::MAGE, SPC::ATTACK, level, false,
		(*_display->string)["SPELL_TILTOWAIT_NAME"], (*_display->string)["SPELL_TILTOWAIT_TITLE"],
		(*_display->string)["SPELL_TILTOWAIT_DESC"]);

	// Priest Spells (grouped by level)

	// Level 1
	level = 1u;
	_spells.emplace_back(SPI::BADIOS, SPT::PRIEST, SPC::ATTACK, level, false, (*_display->string)["SPELL_BADIOS_NAME"],
		(*_display->string)["SPELL_BADIOS_TITLE"], (*_display->string)["SPELL_BADIOS_DESC"]);
	_spells.emplace_back(SPI::DIOS, SPT::PRIEST, SPC::HEALING, level, false, (*_display->string)["SPELL_DIOS_NAME"],
		(*_display->string)["SPELL_DIOS_TITLE"], (*_display->string)["SPELL_DIOS_DESC"]);
	_spells.emplace_back(SPI::KALKI, SPT::PRIEST, SPC::SUPPORT, level, false, (*_display->string)["SPELL_KALKI_NAME"],
		(*_display->string)["SPELL_KALKI_TITLE"], (*_display->string)["SPELL_KALKI_DESC"]);
	_spells.emplace_back(SPI::MILWA, SPT::PRIEST, SPC::FIELD, level, false, (*_display->string)["SPELL_MILWA_NAME"],
		(*_display->string)["SPELL_MILWA_TITLE"], (*_display->string)["SPELL_MILWA_DESC"]);
	_spells.emplace_back(SPI::PORFIC, SPT::PRIEST, SPC::SUPPORT, level, false, (*_display->string)["SPELL_PORFIC_NAME"],
		(*_display->string)["SPELL_PORFIC_TITLE"], (*_display->string)["SPELL_PORFIC_DESC"]);

	// Level 2
	++level;
	_spells.emplace_back(SPI::CALFO, SPT::PRIEST, SPC::FIELD, level, false, (*_display->string)["SPELL_CALFO_NAME"],
		(*_display->string)["SPELL_CALFO_TITLE"], (*_display->string)["SPELL_CALFO_DESC"]);
	_spells.emplace_back(SPI::MANIFO, SPT::PRIEST, SPC::DISABLE, level, false, (*_display->string)["SPELL_MANIFO_NAME"],
		(*_display->string)["SPELL_MANIFO_TITLE"], (*_display->string)["SPELL_MANIFO_DESC"]);
	_spells.emplace_back(SPI::MATU, SPT::PRIEST, SPC::SUPPORT, level, false, (*_display->string)["SPELL_MATU_NAME"],
		(*_display->string)["SPELL_MATU_TITLE"], (*_display->string)["SPELL_MATU_DESC"]);
	_spells.emplace_back(SPI::MONTINO, SPT::PRIEST, SPC::DISABLE, level, false,
		(*_display->string)["SPELL_MONTINO_NAME"], (*_display->string)["SPELL_MONTINO_TITLE"],
		(*_display->string)["SPELL_MONTINO_DESC"]);

	// Level 3
	++level;
	_spells.emplace_back(SPI::MATU, SPT::PRIEST, SPC::SUPPORT, level, false, (*_display->string)["SPELL_BAMATU_NAME"],
		(*_display->string)["SPELL_BAMATU_TITLE"], (*_display->string)["SPELL_BAMATU_DESC"]);
	_spells.emplace_back(SPI::DIALKO, SPT::PRIEST, SPC::HEALING, level, false, (*_display->string)["SPELL_DIALKO_NAME"],
		(*_display->string)["SPELL_DIALKO_TITLE"], (*_display->string)["SPELL_DIALKO_DESC"]);
	_spells.emplace_back(SPI::LATUMAPIC, SPT::PRIEST, SPC::FIELD, level, false,
		(*_display->string)["SPELL_LATUMAPIC_NAME"], (*_display->string)["SPELL_LATUMAPIC_TITLE"],
		(*_display->string)["SPELL_LATUMAPIC_DESC"]);
	_spells.emplace_back(SPI::LOMILWA, SPT::PRIEST, SPC::FIELD, level, false, (*_display->string)["SPELL_LOMILWA_NAME"],
		(*_display->string)["SPELL_LOMILWA_TITLE"], (*_display->string)["SPELL_LOMILWA_DESC"]);

	// Level 4
	++level;
	_spells.emplace_back(SPI::BADIAL, SPT::PRIEST, SPC::ATTACK, level, false, (*_display->string)["SPELL_BADIAL_NAME"],
		(*_display->string)["SPELL_BADIAL_TITLE"], (*_display->string)["SPELL_BADIAL_DESC"]);
	_spells.emplace_back(SPI::DIAL, SPT::PRIEST, SPC::HEALING, level, false, (*_display->string)["SPELL_DIAL_NAME"],
		(*_display->string)["SPELL_DIAL_TITLE"], (*_display->string)["SPELL_DIAL_DESC"]);
	_spells.emplace_back(SPI::LATUMOFIS, SPT::PRIEST, SPC::HEALING, level, false,
		(*_display->string)["SPELL_LATUMOFIS_NAME"], (*_display->string)["SPELL_LATUMOFIS_TITLE"],
		(*_display->string)["SPELL_LATUMOFIS_DESC"]);
	_spells.emplace_back(SPI::MAPORFIC, SPT::PRIEST, SPC::FIELD, level, false,
		(*_display->string)["SPELL_MAPORFIC_NAME"], (*_display->string)["SPELL_MAPORFIC_TITLE"],
		(*_display->string)["SPELL_MAPORFIC_DESC"]);

	// Level 5
	++level;
	_spells.emplace_back(SPI::BADI, SPT::PRIEST, SPC::ATTACK, level, false, (*_display->string)["SPELL_BADI_NAME"],
		(*_display->string)["SPELL_BADI_TITLE"], (*_display->string)["SPELL_BADI_DESC"]);
	_spells.emplace_back(SPI::BADIALMA, SPT::PRIEST, SPC::ATTACK, level, false,
		(*_display->string)["SPELL_BADIALMA_NAME"], (*_display->string)["SPELL_BADIALMA_TITLE"],
		(*_display->string)["SPELL_BADIALMA_DESC"]);
	_spells.emplace_back(SPI::DI, SPT::PRIEST, SPC::HEALING, level, false, (*_display->string)["SPELL_DI_NAME"],
		(*_display->string)["SPELL_DI_TITLE"], (*_display->string)["SPELL_DI_DESC"]);
	_spells.emplace_back(SPI::DIALMA, SPT::PRIEST, SPC::HEALING, level, false, (*_display->string)["SPELL_DIALMA_NAME"],
		(*_display->string)["SPELL_DIALMA_TITLE"], (*_display->string)["SPELL_DIALMA_DESC"]);
	_spells.emplace_back(SPI::KANDI, SPT::PRIEST, SPC::FIELD, level, false, (*_display->string)["SPELL_KANDI_NAME"],
		(*_display->string)["SPELL_KANDI_TITLE"], (*_display->string)["SPELL_KANDI_DESC"]);

	// Level 6
	++level;
	_spells.emplace_back(SPI::LOKTOFEIT, SPT::PRIEST, SPC::FIELD, level, false,
		(*_display->string)["SPELL_LOKTOFEIT_NAME"], (*_display->string)["SPELL_LOKTOFEIT_TITLE"],
		(*_display->string)["SPELL_LOKTOFEIT_DESC"]);
	_spells.emplace_back(SPI::LORTO, SPT::PRIEST, SPC::ATTACK, level, false, (*_display->string)["SPELL_LORTO_NAME"],
		(*_display->string)["SPELL_LORTO_TITLE"], (*_display->string)["SPELL_LORTO_DESC"]);
	_spells.emplace_back(SPI::MABADI, SPT::PRIEST, SPC::ATTACK, level, false, (*_display->string)["SPELL_MABADI_NAME"],
		(*_display->string)["SPELL_MABADI_TITLE"], (*_display->string)["SPELL_MABADI_DESC"]);
	_spells.emplace_back(SPI::MADI, SPT::PRIEST, SPC::HEALING, level, false, (*_display->string)["SPELL_MADI_NAME"],
		(*_display->string)["SPELL_MADI_TITLE"], (*_display->string)["SPELL_MADI_DESC"]);

	// Level 7
	++level;
	_spells.emplace_back(SPI::KADORTO, SPT::PRIEST, SPC::HEALING, level, false,
		(*_display->string)["SPELL_KADORTO_NAME"], (*_display->string)["SPELL_KADORTO_TITLE"],
		(*_display->string)["SPELL_KADORTO_DESC"]);
	_spells.emplace_back(SPI::MALIKTO, SPT::PRIEST, SPC::ATTACK, level, false,
		(*_display->string)["SPELL_MALIKTO_NAME"], (*_display->string)["SPELL_MALIKTO_TITLE"],
		(*_display->string)["SPELL_MALIKTO_DESC"]);
}

auto Sorcery::Character::reset_spells() -> void {
	for (auto &spell : _spells)
		_spells_known[spell.id] = spell.known;
}

auto Sorcery::Character::replenish_spells() -> void {

	for (auto level = 1; level <= 7; level++) {
		_mage_cur_sp[level] = _mage_max_sp[level];
		_priest_cur_sp[level] = _priest_max_sp[level];
	}
}

// Given an Alignment and a Class, create a character
auto Sorcery::Character::create_class_alignment(const CHC cclass, const CAL alignment) -> void {

	_class = cclass;
	_race = static_cast<CHR>((*_system->random)[RNT::D5]);
	_alignment = alignment;

	switch (_race) { // NOLINT(clang-diagnostic-switch)
	case CHR::HUMAN:
		_start_attr = {
			{CAR::STRENGTH, 8}, {CAR::IQ, 5}, {CAR::PIETY, 5}, {CAR::VITALITY, 8}, {CAR::AGILITY, 8}, {CAR::LUCK, 9}};
		break;
	case CHR::ELF:
		_start_attr = {
			{CAR::STRENGTH, 7}, {CAR::IQ, 10}, {CAR::PIETY, 10}, {CAR::VITALITY, 6}, {CAR::AGILITY, 9}, {CAR::LUCK, 6}};
		break;
	case CHR::DWARF:
		_start_attr = {{CAR::STRENGTH, 10}, {CAR::IQ, 7}, {CAR::PIETY, 10}, {CAR::VITALITY, 10}, {CAR::AGILITY, 5},
			{CAR::LUCK, 6}};
		break;
	case CHR::GNOME:
		_start_attr = {
			{CAR::STRENGTH, 7}, {CAR::IQ, 7}, {CAR::PIETY, 10}, {CAR::VITALITY, 8}, {CAR::AGILITY, 10}, {CAR::LUCK, 7}};
		break;
	case CHR::HOBBIT:
		_start_attr = {
			{CAR::STRENGTH, 5}, {CAR::IQ, 7}, {CAR::PIETY, 7}, {CAR::VITALITY, 6}, {CAR::AGILITY, 10}, {CAR::LUCK, 12}};
		break;
	default:
		break;
	}

	// Put most of the points into the main attribute (note that 10 points means a Human Priest and Dwarf Thief have
	// allocated all points to their main attribute with no points left over)
	_points_left = 10;
	_st_points = _points_left;

	switch (_class) { // NOLINT(clang-diagnostic-switch)
	case CHC::FIGHTER:
		[[fallthrough]];
	case CHC::LORD:
		[[fallthrough]];
	case CHC::SAMURAI:
		_points_left -= (15 - _start_attr[CAR::STRENGTH]);
		_start_attr[CAR::STRENGTH] = 15;
		break;
	case CHC::MAGE:
		[[fallthrough]];
	case CHC::BISHOP:
		_points_left -= (15 - _start_attr[CAR::IQ]);
		_start_attr[CAR::IQ] = 15;
		break;
	case CHC::PRIEST:
		_points_left -= (15 - _start_attr[CAR::PIETY]);
		_start_attr[CAR::PIETY] = 15;
		break;
	case CHC::THIEF:
		[[fallthrough]];
	case CHC::NINJA:
		_points_left -= (15 - _start_attr[CAR::AGILITY]);
		_start_attr[CAR::AGILITY] = 15;
		break;
	default:
		break;
	}

	// Pump any points left into the Vitality attribute
	if (_points_left > 0)
		_start_attr[CAR::VITALITY] += _points_left;

	_cur_attr = _start_attr;

	_name = _system->random->get_random_name();
	_portrait_index = (*_system->random)[RNT::ZERO_TO_29];
}

// Enter Name and Portrait, rest is random
auto Sorcery::Character::create_quick() -> void {

	// Exclude Samurai/Lord/Ninja/Bishop from this method of character creation
	_class = static_cast<CHC>((*_system->random)[RNT::D4]);
	_race = static_cast<CHR>((*_system->random)[RNT::D5]);
	switch (_class) { // NOLINT(clang-diagnostic-switch)
	case CHC::FIGHTER:
	case CHC::MAGE:
		_alignment = static_cast<CAL>((*_system->random)[RNT::D3]);
		break;
	case CHC::PRIEST:
		_alignment = (*_system->random)[RNT::D2] == 1 ? CAL::GOOD : CAL::EVIL;
		break;
	case CHC::THIEF:
		_alignment = (*_system->random)[RNT::D2] == 1 ? CAL::NEUTRAL : CAL::EVIL;
		break;
	default:
		break;
	}

	// Now get minimum attributes for race/class combo (note as we are only allowing creation of some classes, it
	// will be as if we had a maximum of 10 bonus points to spend - in order to incentivise full blown character
	// creation! see table IV (A) at
	// https://gamefaqs.gamespot.com/pc/946844-the-ultimate-wizardry-archives/faqs/45726 for info
	switch (_race) { // NOLINT(clang-diagnostic-switch)
	case CHR::HUMAN:
		_start_attr = {
			{CAR::STRENGTH, 8}, {CAR::IQ, 5}, {CAR::PIETY, 5}, {CAR::VITALITY, 8}, {CAR::AGILITY, 8}, {CAR::LUCK, 9}};
		break;
	case CHR::ELF:
		_start_attr = {
			{CAR::STRENGTH, 7}, {CAR::IQ, 10}, {CAR::PIETY, 10}, {CAR::VITALITY, 6}, {CAR::AGILITY, 9}, {CAR::LUCK, 6}};
		break;
	case CHR::DWARF:
		_start_attr = {{CAR::STRENGTH, 10}, {CAR::IQ, 7}, {CAR::PIETY, 10}, {CAR::VITALITY, 10}, {CAR::AGILITY, 5},
			{CAR::LUCK, 6}};
		break;
	case CHR::GNOME:
		_start_attr = {
			{CAR::STRENGTH, 7}, {CAR::IQ, 7}, {CAR::PIETY, 10}, {CAR::VITALITY, 8}, {CAR::AGILITY, 10}, {CAR::LUCK, 7}};
		break;
	case CHR::HOBBIT:
		_start_attr = {
			{CAR::STRENGTH, 5}, {CAR::IQ, 7}, {CAR::PIETY, 7}, {CAR::VITALITY, 6}, {CAR::AGILITY, 10}, {CAR::LUCK, 12}};
		break;
	default:
		break;
	}

	// Put most of the points into the main attribute (note that 10 points means a Human Priest and Dwarf Thief have
	// allocated all points to their main attribute with no points left over)
	_points_left = 10;
	_st_points = _points_left;
	switch (_class) { // NOLINT(clang-diagnostic-switch)
	case CHC::FIGHTER:
		_points_left -= (15 - _start_attr[CAR::STRENGTH]);
		_start_attr[CAR::STRENGTH] = 15;
		break;
	case CHC::MAGE:
		_points_left -= (15 - _start_attr[CAR::IQ]);
		_start_attr[CAR::IQ] = 15;
		break;
	case CHC::PRIEST:
		_points_left -= (15 - _start_attr[CAR::PIETY]);
		_start_attr[CAR::PIETY] = 15;
		break;
	case CHC::THIEF:
		_points_left -= (15 - _start_attr[CAR::AGILITY]);
		_start_attr[CAR::AGILITY] = 15;
		break;
	default:
		break;
	}

	// Pump any points left into the Vitality attribute
	if (_points_left > 0)
		_start_attr[CAR::VITALITY] += _points_left;

	_cur_attr = _start_attr;
}

// Create a (semi) random character
auto Sorcery::Character::create_random() -> void {

	// Random Name and Portrait
	create_quick();
	_name = _system->random->get_random_name();
	_portrait_index = (*_system->random)[RNT::ZERO_TO_29];
}

auto Sorcery::Character::get_status() const -> CHT {

	return _status;
}

auto Sorcery::Character::get_condition() const -> std::string {

	return _get_condition();
}

auto Sorcery::Character::get_short_cond() const -> std::string {

	if (_status != CHT::OK)
		return fmt::format("{:>6}", _get_condition());
	else
		return fmt::format("{:>4}", _abilities.at(CAB::MAX_HP));
}

auto Sorcery::Character::_get_condition() const -> std::string {

	if (is_poisoned() && (_status == CHT::OK)) {
		return (*_display->string)["STATUS_POISONED"];
	} else if (_status == CHT::OK)
		return (*_display->string)["STATUS_OK"];
	else {
		switch (_status) {
		case CHT::AFRAID:
			return (*_display->string)["STATUS_AFRAID"];
			break;
		case CHT::ASHES:
			return (*_display->string)["STATUS_ASHES"];
			break;
		case CHT::ASLEEP:
			return (*_display->string)["STATUS_ASLEEP"];
			break;
		case CHT::DEAD:
			return (*_display->string)["STATUS_DEAD"];
			break;
		case CHT::LOST:
			return (*_display->string)["STATUS_LOST"];
			break;
		case CHT::HELD:
			return (*_display->string)["STATUS_PARALYSED"];
			break;
		case CHT::SILENCED:
			return (*_display->string)["STATUS_SILENCED"];
			break;
		case CHT::STONED:
			return (*_display->string)["STATUS_STONED"];
			break;
		default:
			return "";
		}
	}
}

auto Sorcery::Character::get_loc_str() const -> std::string {

	switch (_location) {
	case CHL::PARTY:
		return (*_display->string)["LOCATION_PARTY"];
		break;
	case CHL::TAVERN:
		return (*_display->string)["LOCATION_TAVERN"];
		break;
	case CHL::TRAINING:
		return (*_display->string)["LOCATION_TRAINING"];
		break;
	case CHL::TEMPLE:
		return (*_display->string)["LOCATION_TEMPLE"];
		break;
	case CHL::MAZE:
		return (*_display->string)["LOCATION_MAZE"];
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
		return (*_display->string)["STATUS_HIDDEN"];
}

auto Sorcery::Character::set_status(CHT value) -> void {

	if (value == CHT::OK)
		_status = value;
	else {
		auto candidate{unenum(value)};
		auto current{unenum(_status)};
		if (candidate > current)
			_status = value;
	}
}

auto Sorcery::Character::set_current_hp(const int hp) -> void {

	_abilities[CAB::CURRENT_HP] = hp;
}

auto Sorcery::Character::is_poisoned() const -> bool {

	return _abilities.at(CAB::POISON_STRENGTH) > 0;
}

auto Sorcery::Character::get_poisoned_rate() const -> int {

	return _abilities.at(CAB::POISON_STRENGTH);
}

auto Sorcery::Character::get_hp_adjustment() const -> int {

	return _abilities.at(CAB::HP_GAIN_PER_TURN) - _abilities.at(CAB::HP_LOSS_PER_TURN) -
		   _abilities.at(CAB::POISON_STRENGTH);
}

auto Sorcery::Character::get_hp_adjustment_symbol() const -> char {

	const auto rate{(_abilities.at(CAB::HP_GAIN_PER_TURN) - _abilities.at(CAB::HP_LOSS_PER_TURN) -
						_abilities.at(CAB::POISON_STRENGTH)) <=> 0};
	if (rate < 0)
		return '-';
	else if (rate > 0)
		return '+';
	else
		return ' ';
}

auto Sorcery::Character::set_hp_gain_per_turn(const int adjustment) -> void {

	_abilities.at(CAB::HP_GAIN_PER_TURN) = adjustment;
}

auto Sorcery::Character::set_hp_loss_per_turn(const int adjustment) -> void {

	_abilities.at(CAB::HP_LOSS_PER_TURN) = adjustment;
}

auto Sorcery::Character::reset_adjustment_per_turn() -> void {

	_abilities.at(CAB::HP_GAIN_PER_TURN) = 0;
	_abilities.at(CAB::HP_LOSS_PER_TURN) = 0;
}

auto Sorcery::Character::set_poisoned_rate(int value) -> void {

	if (value > _abilities.at(CAB::POISON_STRENGTH)) {
		_abilities.at(CAB::POISON_STRENGTH) = value;
	} else if (value == 0) {
		_abilities.at(CAB::POISON_STRENGTH) = 0;
	}
}

auto Sorcery::Character::get_poisoned_string() const -> std::string {

	return _abilities.at(CAB::POISON_STRENGTH) > 0 ? fmt::format("{:->2}", _abilities.at(CAB::POISON_STRENGTH)) : "";
}

auto Sorcery::Character::get_short_hp_summary() const -> std::string {

	return std::to_string(_abilities.at(CAB::CURRENT_HP));
}

auto Sorcery::Character::get_hp_summary() const -> std::string {

	return fmt::format("{}/{}{}", std::to_string(_abilities.at(CAB::CURRENT_HP)),
		std::to_string(_abilities.at(CAB::MAX_HP)), get_hp_adjustment_symbol());
}

auto Sorcery::Character::get_spell_points(const SPT type, const SPS status) const -> std::optional<SpellPoints> {

	if (type == SPT::MAGE && status == SPS::CURRENT)
		return _mage_cur_sp;
	else if (type == SPT::MAGE && status == SPS::MAXIMUM)
		return _mage_max_sp;
	else if (type == SPT::PRIEST && status == SPS::CURRENT)
		return _priest_cur_sp;
	else if (type == SPT::PRIEST && status == SPS::MAXIMUM)
		return _priest_max_sp;
	else
		return std::nullopt;
}

auto Sorcery::Character::_get_sp_per_level(const SPT type, int level) -> std::string {

	if (type == SPT::MAGE)
		return fmt::format("{}/{}", std::to_string(_mage_cur_sp[level]), std::to_string(_mage_max_sp[level]));
	else
		return fmt::format("{}/{}", std::to_string(_priest_cur_sp[level]), std::to_string(_priest_max_sp[level]));
}

auto Sorcery::Character::_get_mage_status(bool current) -> std::string {

	auto value{""s};
	for (auto level = 1; level <= 7; level++)
		if (current)
			value.append(fmt::format(" {} ", _mage_cur_sp[level]));
		else
			value.append(fmt::format("({})", _mage_max_sp[level]));

	RTRIM(value);
	return value;
}

auto Sorcery::Character::_get_priest_status(bool current) -> std::string {

	auto value{""s};
	for (auto level = 1; level <= 7; level++)
		if (current)
			value.append(fmt::format(" {} ", _priest_cur_sp[level]));
		else
			value.append(fmt::format("({})", _priest_max_sp[level]));

	RTRIM(value);
	return value;
}

// For level draining, optionally keep a track of negative levels unless in strict mode
// Need to also handle character class switching

auto Sorcery::Character::get_summary() -> std::string {

	auto name{_name};
	if (_display->get_upper())
		std::ranges::transform(name.begin(), name.end(), name.begin(), ::toupper);
	return fmt::format("{:<15} L {:>2} {}-{} {}", name, _abilities.at(CAB::CURRENT_LEVEL),
		alignment_to_str(_alignment).substr(0, 1), class_to_str(_class).substr(0, 3), race_to_str(_race).substr(0, 3));
}

auto Sorcery::Character::get_summary_and_out() -> std::string {

	auto name{_name};
	if (_display->get_upper())
		std::ranges::transform(name.begin(), name.end(), name.begin(), ::toupper);
	auto location{std::invoke([&] {
		if (_location == CHL::MAZE)
			return "  OUT";
		else if (_status == CHT::LOST)
			return " LOST";
		else
			return "    ";
	})};

	return fmt::format("{:<15} L {:>2} {}-{} {}{:>5}", name, _abilities.at(CAB::CURRENT_LEVEL),
		alignment_to_str(_alignment).substr(0, 1), class_to_str(_class).substr(0, 3), race_to_str(_race).substr(0, 3),
		location);
}

auto Sorcery::Character::can_level() const -> bool {

	return _abilities.at(CAB::CURRENT_XP) > _abilities.at(CAB::NEXT_LEVEL_XP);
}

auto Sorcery::Character::get_cure_cost() const -> unsigned int {

	auto cost_per_level{0u};
	switch (_status) {
	case CHT::ASHES:
		cost_per_level = 500;
		break;
	case CHT::DEAD:
		cost_per_level = 250;
		break;
	case CHT::HELD:
		cost_per_level = 100;
		break;
	case CHT::STONED:
		cost_per_level = 200;
		break;
	default:
		return 0;
	}

	return cost_per_level * _abilities.at(CAB::CURRENT_LEVEL);
}

auto Sorcery::Character::get_sb_text(const int position) -> std::string {

	auto name{_name};
	if (_display->get_upper())
		std::ranges::transform(name.begin(), name.end(), name.begin(), ::toupper);
	return fmt::format("{} {:<15} {}-{} {:>2} {:>4}{}{:<6}", position, name, alignment_to_str(_alignment).substr(0, 1),
		class_to_str(_class).substr(0, 3), get_cur_ac(), get_short_hp_summary(), get_hp_adjustment_symbol(),
		get_short_cond());
}

auto Sorcery::Character::get_age() const -> int {

	return _abilities.at(CAB::AGE);
}

auto Sorcery::Character::set_age(const int adjustment) -> void {

	_abilities.at(CAB::AGE) = _abilities.at(CAB::AGE) + adjustment;
}

auto Sorcery::Character::summary_text() -> std::string {

	auto name{_name};
	if (_display->get_upper())
		std::ranges::transform(name.begin(), name.end(), name.begin(), ::toupper);
	auto legacy{_legated ? " (D)" : ""};
	const auto level{_abilities.at(CAB::CURRENT_LEVEL)};
	switch (_current_stage) {
	case CHS::CHOOSE_METHOD:
		[[fallthrough]];
	case CHS::ENTER_NAME:
		return fmt::format("{:<15} L ?? ?-??? ???", "???");
		break;
	case CHS::CHOOSE_RACE:
		return fmt::format("{:<15} L {:>2} ?-??? ???", name, level);
		break;
	case CHS::CHOOSE_ALIGNMENT:
		return fmt::format("{:<15} L {:>2} ?-??? {}", name, level, race_to_str(_race));
		break;
	case CHS::ALLOCATE_STATS:
		return fmt::format(
			"{:<15} L {:>2} {}-??? {}", name, level, alignment_to_str(_alignment).substr(0, 1), race_to_str(_race));
		break;
	case CHS::CHOOSE_CLASS:
		return fmt::format(
			"{:<15} L {:>2} {}-??? {}", name, level, alignment_to_str(_alignment).substr(0, 1), race_to_str(_race));
		break;
	case CHS::CHOOSE_PORTRAIT:
		return fmt::format("{:<15} L {:>2} {}-{} {}", name, level, alignment_to_str(_alignment).substr(0, 1),
			class_to_str(_class).substr(0, 3), race_to_str(_race));
		break;
	case CHS::REVIEW_AND_CONFIRM:
		[[fallthrough]];
	case CHS::COMPLETED:
		return fmt::format("{} L {:>2} {}-{} {}{}", name, level, alignment_to_str(_alignment).substr(0, 1),
			class_to_str(_class).substr(0, 3), race_to_str(_race), legacy);
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

	_abilities[CAB::CURRENT_HP] = _abilities[CAB::CURRENT_HP] - adjustment;
	if (_abilities[CAB::CURRENT_HP] < 0) {

		_abilities[CAB::CURRENT_HP] = 0;
		_status = CHT::DEAD;
		return false;
	} else
		return true;
}

auto Sorcery::Character::_heal(const unsigned int adjustment) -> void {

	_abilities[CAB::CURRENT_HP] = _abilities[CAB::CURRENT_HP] + adjustment;

	if (_abilities[CAB::CURRENT_HP] > _abilities[CAB::MAX_HP])
		_abilities[CAB::CURRENT_HP] = _abilities[CAB::MAX_HP];
}

auto Sorcery::Character::get_method() const -> CRM {

	return _method;
}

auto Sorcery::Character::set_method(const CRM value) -> void {

	_method = value;
}

auto Sorcery::Character::get_cur_xp() const -> int {

	return _abilities.at(CAB::CURRENT_XP);
}

auto Sorcery::Character::get_next_xp() const -> int {

	return _abilities.at(CAB::NEXT_LEVEL_XP);
}

auto Sorcery::Character::get_cur_ac() const -> int {

	auto ac{_abilities.at(CAB::BASE_ARMOUR_CLASS)};

	for (const auto &item : inventory.items()) {
		if (item.get_equipped() && item.get_cursed())
			ac = ac + (*_itemstore)[item.get_type_id()].get_ac_mod();
		else if (item.get_equipped())
			ac = ac - (*_itemstore)[item.get_type_id()].get_ac_mod();
	}

	return ac;
}

auto Sorcery::Character::get_cur_to_hit() const -> int {

	auto to_hit{_abilities.at(CAB::HIT_PROBABILITY)};

	for (const auto &item : inventory.items())
		to_hit = to_hit + (*_itemstore)[item.get_type_id()].get_to_hit_mod();

	return to_hit;
}

auto Sorcery::Character::get_cur_num_attacks() const -> int {

	auto base_attacks{_abilities.at(CAB::BASE_NUMBER_OF_ATTACKS)};
	auto extra_attacks{0};

	for (const auto &item : inventory.items())
		extra_attacks = extra_attacks + (*_itemstore)[item.get_type_id()].get_swings();

	return extra_attacks > base_attacks ? extra_attacks : base_attacks;
}

namespace Sorcery {

auto operator<<(std::ostream &out_stream, const Sorcery::Character &character) -> std::ostream & {

	auto name{character.get_name()};
	auto cclass{character.get_class()};
	auto alignment{character.get_alignment()};
	auto hp{character.get_hp_summary()};

	auto body{fmt::format("{:<15} {:>2} {}-{} {:>3} {:>6} {:^10}", name, character.get_level(),
		character.alignment_to_str(alignment).substr(0, 1), character.class_to_str(cclass).substr(0, 3),
		character.get_cur_ac(), character.get_hp_summary(), character.get_condition())};

	return out_stream << body << std::endl;
}
} // namespace Sorcery
