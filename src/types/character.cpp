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
#include "resources/stringstore.hpp"
#include "types/configfile.hpp"
#include "types/define.hpp"
#include "types/enum.hpp"

Sorcery::Character::Character() {
}

// Standard Constructor
Sorcery::Character::Character(System *system, Display *display, Graphics *graphics)
	: _system{system}, _display{display}, _graphics{graphics} {

	set_stage(CharacterStage::CHOOSE_METHOD);

	_hidden = false;
	set_status(CharacterStatus::OK);

	_version = CHARACTER_SAVE_VERSION;

	_legated = false;

	_location = CharacterLocation::TAVERN;

	coordinate = std::nullopt;
	depth = std::nullopt;

	inventory.clear();
}

// Overloaded Operator
auto Sorcery::Character::operator[](const CharacterAbility &key) -> int & {

	return _abilities[key];
}

// Utility Functions

auto Sorcery::Character::get_stage() const -> CharacterStage {

	return _current_stage;
}

auto Sorcery::Character::get_location() const -> CharacterLocation {

	return _location;
}

auto Sorcery::Character::set_location(const CharacterLocation value) -> void {

	using enum Enums::Character::Location;

	_location = value;

	if ((value == TAVERN) || (value == TEMPLE) || (value == TRAINING)) {
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
auto Sorcery::Character::set_stage(const CharacterStage stage) -> void {

	using enum Enums::Character::Ability;
	using enum Enums::Character::Align;
	using enum Enums::Character::Class;
	using enum Enums::Character::Race;
	using enum Enums::Character::Stage;
	using enum Enums::Character::View;

	_current_stage = stage;
	switch (stage) {
	case CHOOSE_METHOD:
		_name.clear();
		_race = NO_RACE;
		_alignment = NO_ALIGN;
		_start_attr.clear();
		_cur_attr.clear();
		_max_attr.clear();
		_abilities.clear();

		// Used in the display from this point onwards
		_abilities[CURRENT_LEVEL] = 1;
		_class = NO_CLASS;
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
	case REVIEW_AND_CONFIRM:

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

auto Sorcery::Character::get_race() const -> CharacterRace {

	return _race;
}

auto Sorcery::Character::set_race(const CharacterRace &value) -> void {

	_race = value;
}

auto Sorcery::Character::get_level() const -> int {

	using enum Enums::Character::Ability;

	return _abilities.at(CURRENT_LEVEL);
}

auto Sorcery::Character::set_level(const int &value) -> void {

	using enum Enums::Character::Ability;

	_abilities.at(CURRENT_LEVEL) = value;
}

auto Sorcery::Character::get_alignment() const -> CharacterAlignment {

	return _alignment;
}

auto Sorcery::Character::set_alignment(const CharacterAlignment &value) -> void {

	_alignment = value;
}

auto Sorcery::Character::get_num_pos_class() const -> unsigned int {

	return _num_pos_classes;
}

auto Sorcery::Character::get_class() const -> CharacterClass {

	return _class;
}

auto Sorcery::Character::set_class(const CharacterClass &value) -> void {

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

auto Sorcery::Character::get_cur_attr(const CharacterAttribute attribute) const -> unsigned int {

	return _cur_attr.at(attribute);
}

auto Sorcery::Character::get_start_attr(const CharacterAttribute attribute) const -> unsigned int {

	return _start_attr.at(attribute);
}

auto Sorcery::Character::set_cur_attr(const CharacterAttribute attribute, const int adjustment) -> void {

	_cur_attr.at(attribute) += adjustment;
}

auto Sorcery::Character::get_portrait_index() const -> unsigned int {

	return _portrait_index;
}

auto Sorcery::Character::set_portrait_index(const unsigned int value) -> void {

	_portrait_index = value;
}

auto Sorcery::Character::set_start_attr() -> void {

	using enum Enums::Character::Attribute;
	using enum Enums::Character::Race;

	_start_attr.clear();
	_cur_attr.clear();
	switch (_race) {
	case HUMAN:
		_start_attr = {{STRENGTH, 8}, {IQ, 5}, {PIETY, 5}, {VITALITY, 8}, {AGILITY, 8}, {LUCK, 9}};
		break;
	case ELF:
		_start_attr = {{STRENGTH, 7}, {IQ, 10}, {PIETY, 10}, {VITALITY, 6}, {AGILITY, 9}, {LUCK, 6}};
		break;
	case DWARF:
		_start_attr = {{STRENGTH, 10}, {IQ, 7}, {PIETY, 10}, {VITALITY, 10}, {AGILITY, 5}, {LUCK, 6}};
		break;
	case GNOME:
		_start_attr = {{STRENGTH, 7}, {IQ, 7}, {PIETY, 10}, {VITALITY, 8}, {AGILITY, 10}, {LUCK, 7}};
		break;
	case HOBBIT:
		_start_attr = {{STRENGTH, 5}, {IQ, 7}, {PIETY, 7}, {VITALITY, 6}, {AGILITY, 10}, {LUCK, 12}};
		break;
	default:
		break;
	}

	_cur_attr = _start_attr;

	// Formula sourced from http://www.zimlab.com/wizardry/walk/w123calc.htm
	_points_left = (*_system->random)[RandomType::ZERO_TO_3];
	const bool chance_of_more{(*_system->random)[RandomType::D10] == 1};
	const bool chance_of_more_again{(*_system->random)[RandomType::D10] == 1};
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

	using enum Enums::Character::Align;
	using enum Enums::Character::Attribute;
	using enum Enums::Character::Class;

	_pos_classes.clear();

	// Do the basic classes first (this also sets _num_possible_character_classes); data is from
	// https://strategywiki.org/wiki/Wizardry:_Proving_Grounds_of_the_Mad_Overlord/Trebor%27s_castle#Classes
	if (_cur_attr[STRENGTH] >= 11)
		_pos_classes[FIGHTER] = true;
	else
		_pos_classes[FIGHTER] = false;

	if (_cur_attr[IQ] >= 11)
		_pos_classes[MAGE] = true;
	else
		_pos_classes[MAGE] = false;

	if (_cur_attr[PIETY] >= 11)
		if (_alignment == GOOD || _alignment == EVIL)
			_pos_classes[PRIEST] = true;
		else
			_pos_classes[PRIEST] = false;
	else
		_pos_classes[PRIEST] = false;

	if (_cur_attr[AGILITY] >= 11)
		if (_alignment == NEUTRAL || _alignment == EVIL)
			_pos_classes[THIEF] = true;
		else
			_pos_classes[THIEF] = false;
	else
		_pos_classes[THIEF] = false;

	// Now the elite classes
	if (_cur_attr[IQ] >= 12 && _cur_attr[PIETY] >= 12)
		if (_alignment == GOOD || _alignment == EVIL)
			_pos_classes[BISHOP] = true;
		else
			_pos_classes[BISHOP] = false;
	else
		_pos_classes[BISHOP] = false;

	if (_cur_attr[STRENGTH] >= 15 && _cur_attr[IQ] >= 11 && _cur_attr[PIETY] >= 10 && _cur_attr[VITALITY] >= 10 &&
		_cur_attr[AGILITY] >= 10)
		if (_alignment == GOOD || _alignment == NEUTRAL)
			_pos_classes[SAMURAI] = true;
		else
			_pos_classes[SAMURAI] = false;
	else
		_pos_classes[SAMURAI] = false;

	if (_cur_attr[STRENGTH] >= 15 && _cur_attr[IQ] >= 12 && _cur_attr[PIETY] >= 12 && _cur_attr[VITALITY] >= 15 &&
		_cur_attr[AGILITY] >= 14 && _cur_attr[LUCK] >= 15)
		if (_alignment == GOOD)
			_pos_classes[LORD] = true;
		else
			_pos_classes[LORD] = false;
	else
		_pos_classes[LORD] = false;

	// Using Wizardry 5 requirements for Ninja (see https://wizardry.fandom.com/wiki/Ninja)
	if (_cur_attr[STRENGTH] >= 15 && _cur_attr[IQ] >= 17 && _cur_attr[PIETY] >= 15 && _cur_attr[VITALITY] >= 16 &&
		_cur_attr[AGILITY] >= 15 && _cur_attr[LUCK] >= 16)
		if (_alignment != GOOD)
			_pos_classes[NINJA] = true;
		else
			_pos_classes[NINJA] = false;
	else
		_pos_classes[NINJA] = false;

	// And workout the number of classes
	_num_pos_classes =
		std::count_if(_pos_classes.begin(), _pos_classes.end(), [](auto element) { return element.second; });
}

// Enum to String functions
auto Sorcery::Character::alignment_to_str(CharacterAlignment character_alignment) const -> std::string {

	static const std::array<std::string, 4> alignments{"", (*_display->string)["CHARACTER_ALIGNMENT_GOOD"],
		(*_display->string)["CHARACTER_ALIGNMENT_NEUTRAL"], (*_display->string)["CHARACTER_ALIGNMENT_EVIL"]};

	return alignments[unenum(character_alignment)];
}

auto Sorcery::Character::race_to_str(CharacterRace character_race) const -> std::string {

	static const std::array<std::string, 6> races{"", (*_display->string)["CHARACTER_RACE_HUMAN"],
		(*_display->string)["CHARACTER_RACE_ELF"], (*_display->string)["CHARACTER_RACE_DWARF"],
		(*_display->string)["CHARACTER_RACE_GNOME"], (*_display->string)["CHARACTER_RACE_HOBBIT"]};

	return races[unenum(character_race)];
}

auto Sorcery::Character::class_to_str(CharacterClass character_class) const -> std::string {

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

	using enum Enums::Character::Ability;

	return ashes ? _abilities[ASHES_RESURRECT] : _abilities[DEAD_RESURRECT];
}

auto Sorcery::Character::grant_gold(const int value) -> void {

	using enum Enums::Character::Ability;

	_abilities[GOLD] = _abilities[GOLD] + value;
}

auto Sorcery::Character::get_gold() const -> unsigned int {

	using enum Enums::Character::Ability;

	return _abilities.at(GOLD);
}

auto Sorcery::Character::set_gold(const unsigned int value) -> void {

	using enum Enums::Character::Ability;

	_abilities[GOLD] = value;
}

auto Sorcery::Character::_legate_start_info() -> void {

	using enum Enums::Character::Attribute;
	using enum Enums::Character::Ability;
	using enum Enums::Character::Class;

	// From here: https://datadrivengamer.blogspot.com/2021/08/the-new-mechanics-of-wizardry-iii.html
	_abilities[CURRENT_LEVEL] = 1;
	_abilities[CURRENT_XP] = 0;
	_abilities[NEXT_LEVEL_XP] = _get_xp_for_level(_abilities[CURRENT_LEVEL]);
	_abilities[MAX_LEVEL] = _abilities[CURRENT_LEVEL];
	_abilities[NEGATIVE_LEVEL] = 0;
	_abilities[HIT_DICE] = 1;
	if (_abilities[GOLD] > 500)
		_abilities[GOLD] = 500;
	_abilities[AGE] = (20 * 52);
	_abilities[SWIM] = 1;
	_abilities[MARKS] = 0;
	_abilities[DEATHS] = 0;

	// (D7 - 4) is -3 to +3
	_start_attr[STRENGTH] += ((*_system->random)[RandomType::D7] - 4);
	_start_attr[IQ] += ((*_system->random)[RandomType::D7] - 4);
	_start_attr[PIETY] += ((*_system->random)[RandomType::D7] - 4);
	_start_attr[VITALITY] += ((*_system->random)[RandomType::D7] - 4);
	_start_attr[AGILITY] += ((*_system->random)[RandomType::D7] - 4);
	_start_attr[LUCK] += ((*_system->random)[RandomType::D7] - 4);

	auto mage_sp_total{0};
	for (auto level = 1; level <= 7; level++)
		mage_sp_total += _mage_max_sp[level];
	_start_attr[IQ] += (mage_sp_total / 7);

	auto priest_sp_total{1};
	for (auto level = 1; level <= 7; level++)
		priest_sp_total += _priest_max_sp[level];
	_start_attr[PIETY] += (priest_sp_total / 10);

	switch (_class) { // NOLINT(clang-diagnostic-switch)
	case FIGHTER:
	case LORD:
	case SAMURAI:
		_start_attr[STRENGTH] += 2;
		break;
	case MAGE:
		_start_attr[IQ] += 2;
		break;
	case PRIEST:
	case BISHOP:
		_start_attr[PIETY] += 2;
		break;
	case THIEF:
	case NINJA:
		_start_attr[AGILITY] += 2;
	default:
		break;
	}

	set_status(CharacterStatus::OK);
	_location = CharacterLocation::TAVERN;
	_abilities[CURRENT_HP] = _abilities[MAX_HP];

	// Clamp Values
	_start_attr[STRENGTH] = std::min(_start_attr[STRENGTH], 18);
	_start_attr[IQ] = std::min(_start_attr[IQ], 18);
	_start_attr[PIETY] = std::min(_start_attr[PIETY], 18);
	_start_attr[VITALITY] = std::min(_start_attr[VITALITY], 18);
	_start_attr[AGILITY] = std::min(_start_attr[AGILITY], 18);
	_start_attr[LUCK] = std::min(_start_attr[LUCK], 18);
	_start_attr[STRENGTH] = std::max(_start_attr[STRENGTH], 3);
	_start_attr[IQ] = std::max(_start_attr[IQ], 3);
	_start_attr[PIETY] = std::max(_start_attr[PIETY], 3);
	_start_attr[VITALITY] = std::max(_start_attr[VITALITY], 3);
	_start_attr[AGILITY] = std::max(_start_attr[AGILITY], 3);
	_start_attr[LUCK] = std::max(_start_attr[LUCK], 3);

	_cur_attr = _start_attr;
	_max_attr = _cur_attr;

	_abilities[CURRENT_XP] = 0;
	_abilities[NEXT_LEVEL_XP] = _get_xp_for_level(_abilities[CURRENT_LEVEL]);
}

auto Sorcery::Character::_regenerate_start_info() -> void {

	using enum Enums::Character::Ability;
	using enum Enums::Character::Attribute;
	using enum Enums::Character::Race;

	_abilities[MAX_LEVEL] = _abilities[CURRENT_LEVEL];
	_abilities[CURRENT_LEVEL] = 1;
	_abilities[CURRENT_XP] = 0;
	_abilities[NEXT_LEVEL_XP] = _get_xp_for_level(_abilities[CURRENT_LEVEL]);

	// https://datadrivengamer.blogspot.com/2019/08/the-not-so-basic-mechanics-of-wizardry.html
	auto age_increment{(52 * (3 + (*_system->random)[RandomType::D3])) + 44};
	_abilities[AGE] += age_increment;

	// Reset attributes to racial minimums
	CharacterAttributes minimum_attr;
	switch (_race) {
	case HUMAN:
		minimum_attr = {{STRENGTH, 8}, {IQ, 5}, {PIETY, 5}, {VITALITY, 8}, {AGILITY, 8}, {LUCK, 9}};
		break;
	case ELF:
		minimum_attr = {{STRENGTH, 7}, {IQ, 10}, {PIETY, 10}, {VITALITY, 6}, {AGILITY, 9}, {LUCK, 6}};
		break;
	case DWARF:
		minimum_attr = {{STRENGTH, 10}, {IQ, 7}, {PIETY, 10}, {VITALITY, 10}, {AGILITY, 5}, {LUCK, 6}};
		break;
	case GNOME:
		minimum_attr = {{STRENGTH, 7}, {IQ, 7}, {PIETY, 10}, {VITALITY, 8}, {AGILITY, 10}, {LUCK, 7}};
		break;
	case HOBBIT:
		minimum_attr = {{STRENGTH, 5}, {IQ, 7}, {PIETY, 7}, {VITALITY, 6}, {AGILITY, 10}, {LUCK, 12}};
		break;
	default:
		break;
	}

	_cur_attr = minimum_attr;
}

// Legate
auto Sorcery::Character::legate(const CharacterAlignment &value) -> void {

	if (_alignment != value)
		_alignment = value;

	_legate_start_info();
	_generate_secondary_abil(true, false, true);
	_set_start_spells();
	_set_starting_sp();
	inventory.clear();
	set_status(CharacterStatus::OK);
	_legated = true;

	set_location(CharacterLocation::TAVERN);
}

// Change Class
auto Sorcery::Character::change_class(const CharacterClass &value) -> void {

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

	using enum Enums::Character::Ability;

	// _abilities[CURRENT_LEVEL] = 1; // why is this commented out?
	_abilities[MAX_LEVEL] = 1;
	_abilities[NEGATIVE_LEVEL] = 0;
	_abilities[HIT_DICE] = 1;
	_abilities[GOLD] = (*_system->random)[RandomType::ZERO_TO_99] + 90;
	_abilities[AGE] = (18 * 52) + (*_system->random)[RandomType::ZERO_TO_299];
	_abilities[SWIM] = 1;
	_abilities[MARKS] = 0;
	_abilities[DEATHS] = 0;

	_start_attr = _cur_attr;
	_max_attr = _cur_attr;

	_abilities[CURRENT_XP] = 0;
	_abilities[NEXT_LEVEL_XP] = _get_xp_for_level(_abilities[CURRENT_LEVEL]);
}

// Given the characters current level, work out all the secondary abilities/stats etc
auto Sorcery::Character::_generate_secondary_abil(bool initial, bool change_class, bool legate) -> void {

	using enum Enums::Character::Ability;
	using enum Enums::Character::Attribute;
	using enum Enums::Character::Class;
	using enum Enums::Character::Race;

	// Formulae used are from here
	// http://www.zimlab.com/wizardry/walk/w123calc.htm and also from
	// https://mirrors.apple2.org.za/ftp.apple.asimov.net/images/games/rpg/wizardry/wizardry_I/Wizardry_i_SourceCode.zip
	const auto current_level{_abilities[CURRENT_LEVEL]};

	// Bonus Melee to Hit per Attack (num)
	if (_cur_attr[STRENGTH] > 15)
		_abilities[ATTACK_MODIFIER] = _cur_attr[STRENGTH] - 15;
	else if (_cur_attr[STRENGTH] < 6)
		_abilities[ATTACK_MODIFIER] = _cur_attr[STRENGTH] - 6;
	else
		_abilities[ATTACK_MODIFIER] = 0;

	// Bonus Melee to Hit per Attack (num)
	switch (_class) {
	case FIGHTER:
	case SAMURAI:
	case LORD:
	case NINJA:
	case PRIEST:
		_abilities[HIT_PROBABILITY] = 2 + (current_level / 5);
		break;
	default:
		_abilities[HIT_PROBABILITY] = current_level / 5;
		break;
	}

	// Bonus Melee Damage per Attack (num)
	if (_cur_attr[STRENGTH] > 15)
		_abilities[BONUS_DAMAGE] = _cur_attr[STRENGTH] - 15;
	else if (_cur_attr[STRENGTH] < 6)
		_abilities[BONUS_DAMAGE] = _cur_attr[STRENGTH] - 6;
	else
		_abilities[BONUS_DAMAGE] = 0;

	// Unarmed Attack Damage (num)
	_abilities[UNARMED_DAMAGE] = _class == NINJA ? 8 + _abilities[BONUS_DAMAGE] : 4 + _abilities[BONUS_DAMAGE];

	// Number of Melee Attacks (num)
	switch (_class) {
	case FIGHTER:
	case SAMURAI:
	case LORD:
		_abilities[BASE_NUMBER_OF_ATTACKS] = current_level / 5;
		break;
	case NINJA:
		_abilities[BASE_NUMBER_OF_ATTACKS] = (current_level / 5) + 1;
		break;
	default:
		_abilities[BASE_NUMBER_OF_ATTACKS] = 1;
		break;
	}
	if (_abilities[BASE_NUMBER_OF_ATTACKS] == 0)
		_abilities[BASE_NUMBER_OF_ATTACKS] = 1;

	// Chance of learning new Mage Spells (%)
	switch (_class) {
	case SAMURAI:
	case BISHOP:
	case MAGE:
		_abilities[MAGE_SPELL_LEARN] = (_cur_attr[IQ] / 29.0) * 100;
		break;
	default:
		_abilities[MAGE_SPELL_LEARN] = 0;
		break;
	}

	// Chance of Identifying Items (%)
	_abilities[IDENTIFY_ITEMS] = _class == BISHOP ? 10 + (5 * current_level) : 0;
	if (_abilities[IDENTIFY_ITEMS] > 100)
		_abilities[IDENTIFY_ITEMS] = 100;

	// Chance of getting cursed when Identifying Items (%)
	_abilities[IDENTIFY_CURSE] = _class == BISHOP ? 35 - (5 * current_level) : 0;
	if (_abilities[IDENTIFY_CURSE] < 0)
		_abilities[IDENTIFY_CURSE] = 0;

	// Chance of identifying unknown Foes per round (%)
	_abilities[IDENTIFY_FOES] = current_level + _cur_attr[IQ] + _cur_attr[PIETY];
	if (_abilities[IDENTIFY_FOES] > 100)
		_abilities[IDENTIFY_FOES] = 100;

	// Chance of learning new Priest Spells (%)
	switch (_class) {
	case PRIEST:
	case LORD:
	case BISHOP:
		_abilities[PRIEST_SPELL_LEARN] = (_cur_attr[PIETY] / 30.0) * 100;
		break;
	default:
		_abilities[PRIEST_SPELL_LEARN] = 0;
		break;
	}

	// LOKTOFELT success chance (%)
	_abilities[LOKTOFELT_SUCCESS] = _class == PRIEST ? 2 * current_level : 0;

	// Base Dispell chance (affected by monster level) (%)
	switch (_class) {
	case CharacterClass::PRIEST:
		_abilities[BASE_DISPELL] = 50 + (5 * current_level);
		break;
	case CharacterClass::BISHOP:
		_abilities[BASE_DISPELL] = current_level >= 4 ? 50 + (5 * current_level) - 20 : 0;
		break;
	case CharacterClass::LORD:
		_abilities[BASE_DISPELL] = current_level >= 9 ? 50 + (5 * current_level) - 40 : 0;
		break;
	default:
		_abilities[BASE_DISPELL] = 0;
		break;
	}
	if (_abilities[BASE_DISPELL] > 100)
		_abilities[BASE_DISPELL] = 100;

	// Vitality Bonus (num)
	switch (_cur_attr[VITALITY]) {
	case 3:
		_abilities[VITALITY_BONUS] = -2;
		break;
	case 4:
	case 5:
		_abilities[VITALITY_BONUS] = -1;
		break;
	case 16:
		_abilities[VITALITY_BONUS] = 1;
		break;
	case 17:
		_abilities[VITALITY_BONUS] = 2;
		break;
	case 18:
	case 19:
	case 20:
		_abilities[VITALITY_BONUS] = 3;
		break;
	default:
		_abilities[VITALITY_BONUS] = 0;
		break;
	}

	// Bonus Hit Points per level (num)
	_abilities[BONUS_HIT_POINTS] = _abilities[VITALITY_BONUS];

	// Class Change doesn't reset these
	if (!change_class) {

		// Base Hit Points (num) - note initially all non-legated characters get 8 HP as per the PSX versions
		if (initial) {

			switch (auto chance{(*_system->random)[RandomType::D100]}; _class) { // NOLINT(clang-diagnostic-switch)
			case FIGHTER:
			case LORD:
				_abilities[MAX_HP] =
					chance <= 50 ? 10 + _abilities[BONUS_HIT_POINTS] : 9 * (10 + _abilities[BONUS_HIT_POINTS]) / 10;
				break;
			case PRIEST:
				_abilities[MAX_HP] =
					chance <= 50 ? 8 + _abilities[BONUS_HIT_POINTS] : 8 * (10 + _abilities[BONUS_HIT_POINTS]) / 10;
				break;
			case THIEF:
			case BISHOP:
			case NINJA:
				_abilities[MAX_HP] =
					chance <= 50 ? 6 + _abilities[BONUS_HIT_POINTS] : 6 * (10 + _abilities[BONUS_HIT_POINTS]) / 10;
				break;
			case MAGE:
				_abilities[MAX_HP] =
					chance <= 50 ? 4 + _abilities[BONUS_HIT_POINTS] : 4 * (10 + _abilities[BONUS_HIT_POINTS]) / 10;
				break;
			case SAMURAI:
				_abilities[MAX_HP] =
					chance <= 50 ? 16 + _abilities[BONUS_HIT_POINTS] : 16 * (10 + _abilities[BONUS_HIT_POINTS]) / 10;
				break;
			default:
				break;
			}
			if (_abilities[MAX_HP] < 1)
				_abilities[MAX_HP] = 1;

			_abilities[CURRENT_HP] = _abilities[MAX_HP];
		}

		if (legate) {
			switch (_class) {
			case FIGHTER:
				_abilities[MAX_HP] = 10;
				break;
			case SAMURAI:
			case LORD:
				_abilities[MAX_HP] = 12;
				break;
			case PRIEST:
				_abilities[MAX_HP] = 8;
				break;
			case NINJA:
				_abilities[MAX_HP] = 7;
				break;
			case THIEF:
				_abilities[MAX_HP] = 6;
				break;
			case MAGE:
				_abilities[MAX_HP] = 4;
				break;
			default:
				break;
			}

			_abilities[CURRENT_HP] = _abilities[MAX_HP];
		}
	}

	// Chance of resurrecting a Dead Character at the Temple (%)
	_abilities[DEAD_RESURRECT] = 50 + (3 * _cur_attr[VITALITY]);
	if (_abilities[DEAD_RESURRECT] > 100)
		_abilities[DEAD_RESURRECT] = 100;

	// Chance of resurrecting an Ashed Character at the Temple (%)
	_abilities[ASHES_RESURRECT] = 40 + (3 * _cur_attr[VITALITY]);
	if (_abilities[ASHES_RESURRECT] > 100)
		_abilities[ASHES_RESURRECT] = 100;

	// Chance of resurrecting by a DI or KADORTO spell cast by another Character (%)
	_abilities[DI_KADORTO_RESURRECT] = 4 * _cur_attr[VITALITY];

	// Initiative Modifier (num)
	switch (_cur_attr[AGILITY]) {
	case 3:
		_abilities[INITIATIVE_MODIFIER] = 3;
		break;
	case 4:
	case 5:
		_abilities[INITIATIVE_MODIFIER] = 2;
		break;
	case 6:
	case 7:
		_abilities[INITIATIVE_MODIFIER] = 1;
		break;
	case 15:
		_abilities[INITIATIVE_MODIFIER] = -1;
		break;
	case 16:
		_abilities[INITIATIVE_MODIFIER] = -2;
		break;
	case 17:
		_abilities[INITIATIVE_MODIFIER] = -3;
		break;
	case 18:
	case 19:
	case 20:
		_abilities[INITIATIVE_MODIFIER] = -4;
		break;
	default:
		_abilities[INITIATIVE_MODIFIER] = 0;
		break;
	}

	// Armour Class (num) (before equipment)
	_abilities[BASE_ARMOUR_CLASS] = _class == NINJA ? 10 - (current_level / 3) - 2 : 10;
	_abilities[CURRENT_ARMOUR_CLASS] = _abilities[BASE_ARMOUR_CLASS];

	// Critical Hit Chance (%)
	_abilities[BASE_CRITICAL_HIT] = _class == NINJA ? 2 * current_level : 0;
	if (_abilities[BASE_CRITICAL_HIT] > 50)
		_abilities[BASE_CRITICAL_HIT] = 50;

	// Chance to identify a Trap (%)
	if (_class == THIEF)
		_abilities[IDENTIFY_TRAP] = 6 * _cur_attr[AGILITY];
	else if (_class == NINJA)
		_abilities[IDENTIFY_TRAP] = 4 * _cur_attr[AGILITY];
	else
		_abilities[IDENTIFY_TRAP] = _cur_attr[AGILITY];
	if (_abilities[IDENTIFY_TRAP] > 95)
		_abilities[IDENTIFY_TRAP] = 95;

	// Base Chance to Disarm a Trap (modified by Maze Level) (%)
	_abilities[BASE_DISARM_TRAP] = (_class == NINJA) || (_class == THIEF) ? ((current_level + 50) / 69.0) * 100 : 0;

	// Chance to activate a Trap if identify fails (%)
	_abilities[ACTIVATE_TRAP] =
		(_class == NINJA) || (_class == THIEF) ? 100 - ((_cur_attr[AGILITY] / 20.0) * 100) : 100;

	// Base Chance to avoid following into a Pit (modified by Maze Level) (%)
	_abilities[BASE_AVOID_PIT] = (_cur_attr[AGILITY] / 25.0) * 100;

	// Base Resist Bonus (d20)
	_abilities[BASE_RESIST_BONUS] = 1 * (current_level / 5);
	if (_cur_attr[LUCK] >= 18)
		_abilities[BASE_RESIST_BONUS] += 3;
	else if (_cur_attr[LUCK] >= 12)
		_abilities[BASE_RESIST_BONUS] += 2;
	else if (_cur_attr[LUCK] >= 6)
		_abilities[BASE_RESIST_BONUS] += 1;

	// Chance equipment is intact on a corpse TODO: check this is accurate
	_abilities[EQUIPMENT_INTACT_ON_WIPE] = (_cur_attr[LUCK] / 21.0f) * 100;

	// Other Resists (d20)
	switch (_class) { // NOLINT(clang-diagnostic-switch)
	case SAMURAI:
		_abilities[RESISTANCE_VS_POISON_PARALYSIS] = _abilities[BASE_RESIST_BONUS] + 2;
		_abilities[RESISTANCE_VS_CRITICAL_HIT] = _abilities[BASE_RESIST_BONUS] + 2;
		_abilities[RESISTANCE_VS_STONING] = 0;
		_abilities[RESISTANCE_VS_BREATH_ATTACKS] = 0;
		_abilities[RESISTANCE_VS_POISON_GAS_TRAP] = 0;
		_abilities[RESISTANCE_VS_MAGE_PRIEST_TRAP] = _abilities[BASE_RESIST_BONUS] + 2;
		_abilities[RESISTANCE_VS_SILENCE] = _abilities[BASE_RESIST_BONUS] + 2;
		break;
	case FIGHTER:
		_abilities[RESISTANCE_VS_POISON_PARALYSIS] = _abilities[BASE_RESIST_BONUS] + 3;
		_abilities[RESISTANCE_VS_CRITICAL_HIT] = _abilities[BASE_RESIST_BONUS] + 3;
		_abilities[RESISTANCE_VS_STONING] = 0;
		_abilities[RESISTANCE_VS_BREATH_ATTACKS] = 0;
		_abilities[RESISTANCE_VS_POISON_GAS_TRAP] = 0;
		_abilities[RESISTANCE_VS_MAGE_PRIEST_TRAP] = 0;
		_abilities[RESISTANCE_VS_SILENCE] = 0;
		break;
	case PRIEST:
		_abilities[RESISTANCE_VS_POISON_PARALYSIS] = 0;
		_abilities[RESISTANCE_VS_CRITICAL_HIT] = 0;
		_abilities[RESISTANCE_VS_STONING] = _abilities[BASE_RESIST_BONUS] + 3;
		_abilities[RESISTANCE_VS_BREATH_ATTACKS] = 0;
		_abilities[RESISTANCE_VS_POISON_GAS_TRAP] = 0;
		_abilities[RESISTANCE_VS_MAGE_PRIEST_TRAP] = 0;
		_abilities[RESISTANCE_VS_SILENCE] = 0;
		break;
	case LORD:
		_abilities[RESISTANCE_VS_POISON_PARALYSIS] = _abilities[BASE_RESIST_BONUS] + 2;
		_abilities[RESISTANCE_VS_CRITICAL_HIT] = _abilities[BASE_RESIST_BONUS] + 2;
		_abilities[RESISTANCE_VS_STONING] = _abilities[BASE_RESIST_BONUS] + 2;
		_abilities[RESISTANCE_VS_BREATH_ATTACKS] = 0;
		_abilities[RESISTANCE_VS_POISON_GAS_TRAP] = 0;
		_abilities[RESISTANCE_VS_MAGE_PRIEST_TRAP] = 0;
		_abilities[RESISTANCE_VS_SILENCE] = 0;
		break;
	case BISHOP:
		_abilities[RESISTANCE_VS_POISON_PARALYSIS] = 0;
		_abilities[RESISTANCE_VS_CRITICAL_HIT] = 0;
		_abilities[RESISTANCE_VS_STONING] = _abilities[BASE_RESIST_BONUS] + 2;
		_abilities[RESISTANCE_VS_BREATH_ATTACKS] = _abilities[BASE_RESIST_BONUS] + 2;
		_abilities[RESISTANCE_VS_POISON_GAS_TRAP] = 0;
		_abilities[RESISTANCE_VS_MAGE_PRIEST_TRAP] = _abilities[BASE_RESIST_BONUS] + 2;
		_abilities[RESISTANCE_VS_SILENCE] = _abilities[BASE_RESIST_BONUS] + 2;
		break;
	case THIEF:
		_abilities[RESISTANCE_VS_POISON_PARALYSIS] = 0;
		_abilities[RESISTANCE_VS_CRITICAL_HIT] = 0;
		_abilities[RESISTANCE_VS_STONING] = 0;
		_abilities[RESISTANCE_VS_BREATH_ATTACKS] = 0;
		_abilities[RESISTANCE_VS_POISON_GAS_TRAP] = _abilities[BASE_RESIST_BONUS] + 3;
		_abilities[RESISTANCE_VS_MAGE_PRIEST_TRAP] = 0;
		_abilities[RESISTANCE_VS_SILENCE] = 0;
		break;
	case NINJA:
		_abilities[RESISTANCE_VS_POISON_PARALYSIS] = _abilities[BASE_RESIST_BONUS] + 3;
		_abilities[RESISTANCE_VS_CRITICAL_HIT] = _abilities[BASE_RESIST_BONUS] + 3;
		_abilities[RESISTANCE_VS_STONING] = _abilities[BASE_RESIST_BONUS] + 2;
		_abilities[RESISTANCE_VS_BREATH_ATTACKS] = _abilities[BASE_RESIST_BONUS] + 4;
		_abilities[RESISTANCE_VS_POISON_GAS_TRAP] = _abilities[BASE_RESIST_BONUS] + 3;
		_abilities[RESISTANCE_VS_MAGE_PRIEST_TRAP] = _abilities[BASE_RESIST_BONUS] + 2;
		_abilities[RESISTANCE_VS_SILENCE] = _abilities[BASE_RESIST_BONUS] + 2;
		break;
	case MAGE:
		_abilities[RESISTANCE_VS_POISON_PARALYSIS] = 0;
		_abilities[RESISTANCE_VS_CRITICAL_HIT] = 0;
		_abilities[RESISTANCE_VS_STONING] = 0;
		_abilities[RESISTANCE_VS_BREATH_ATTACKS] = 0;
		_abilities[RESISTANCE_VS_POISON_GAS_TRAP] = 0;
		_abilities[RESISTANCE_VS_MAGE_PRIEST_TRAP] = _abilities[BASE_RESIST_BONUS] + 3;
		_abilities[RESISTANCE_VS_SILENCE] = _abilities[BASE_RESIST_BONUS] + 3;
		break;
	default:
		break;
	}
	if (_race == HUMAN)
		_abilities[RESISTANCE_VS_POISON_PARALYSIS] += 1;
	if (_race == HUMAN)
		_abilities[RESISTANCE_VS_CRITICAL_HIT] += 1;
	if (_race == GNOME)
		_abilities[RESISTANCE_VS_STONING] += 2;
	if (_race == ELF)
		_abilities[RESISTANCE_VS_BREATH_ATTACKS] += 2;
	if (_race == DWARF)
		_abilities[RESISTANCE_VS_POISON_GAS_TRAP] += 4;
	if (_race == HOBBIT) {
		_abilities[RESISTANCE_VS_MAGE_PRIEST_TRAP] += 2;
		_abilities[RESISTANCE_VS_SILENCE] += 3;
	}
	if (_abilities[RESISTANCE_VS_POISON_PARALYSIS] > 19)
		_abilities[RESISTANCE_VS_POISON_PARALYSIS] = 19;
	if (_abilities[RESISTANCE_VS_CRITICAL_HIT] > 19)
		_abilities[RESISTANCE_VS_CRITICAL_HIT] = 19;
	if (_abilities[RESISTANCE_VS_STONING] > 19)
		_abilities[RESISTANCE_VS_STONING] = 19;
	if (_abilities[RESISTANCE_VS_BREATH_ATTACKS] > 19)
		_abilities[RESISTANCE_VS_BREATH_ATTACKS] = 19;
	if (_abilities[RESISTANCE_VS_POISON_GAS_TRAP] > 19)
		_abilities[RESISTANCE_VS_POISON_GAS_TRAP] = 19;
	if (_abilities[RESISTANCE_VS_MAGE_PRIEST_TRAP] > 19)
		_abilities[RESISTANCE_VS_MAGE_PRIEST_TRAP] = 19;
	if (_abilities[RESISTANCE_VS_SILENCE] > 19)
		_abilities[RESISTANCE_VS_SILENCE] = 19;

	// Resistance to harmful spells (%)
	_abilities[RESISTANCE_VS_KATINO] = current_level * 10;
	if (_abilities[RESISTANCE_VS_KATINO] > 100)
		_abilities[RESISTANCE_VS_KATINO] = 100;
	_abilities[RESISTANCE_VS_BADI] = current_level * 10;
	if (_abilities[RESISTANCE_VS_BADI] > 100)
		_abilities[RESISTANCE_VS_BADI] = 100;
	_abilities[RESISTANCE_VS_MANIFO] = 50 + (current_level * 10);
	if (_abilities[RESISTANCE_VS_MANIFO] > 100)
		_abilities[RESISTANCE_VS_MANIFO] = 100;

	_abilities[RECOVER_FROM_SLEEP] = current_level * 10;
	if (_abilities[RECOVER_FROM_SLEEP] > 100)
		_abilities[RECOVER_FROM_SLEEP] = 100;

	_abilities[RECOVER_FROM_FEAR] = current_level * 5;
	if (_abilities[RECOVER_FROM_FEAR] > 100)
		_abilities[RECOVER_FROM_FEAR] = 100;

	_abilities[BONUS_MAGE_SPELLS] = 0;
	_abilities[BONUS_PRIEST_SPELLS] = 0;

	// And set poison/regeneration to default
	if (!change_class) {
		_abilities[HP_LOSS_PER_TURN] = 0;
		_abilities[HP_GAIN_PER_TURN] = 0;
		_abilities[POISON_STRENGTH] = 0;
	}
}

auto Sorcery::Character::grant_xp(const int adjustment) -> int {

	using enum Enums::Character::Ability;

	_abilities[CURRENT_XP] = _abilities[CURRENT_XP] + adjustment;

	return _abilities[CURRENT_XP];
}

// Now work out spellpoints!
auto Sorcery::Character::_reset_starting_sp() -> void {

	_set_starting_sp();

	// And add in 1 spell point for each known spell NOT of the type
	for (auto spell_level = 1u; spell_level <= 7u; spell_level++) {

		// Handle Priest Spells
		auto priest_known{static_cast<unsigned int>(std::count_if(_spells.begin(), _spells.end(), [=](auto spell) {
			return (spell.type == SpellType::PRIEST) && (spell.level == spell_level) && (spell.known);
		}))};

		if (_priest_max_sp[spell_level] < priest_known) {
			_priest_max_sp[spell_level] = priest_known;
			_priest_cur_sp[spell_level] = _priest_max_sp[spell_level];
		}

		// Handle Mage Spells
		auto mage_known{static_cast<unsigned int>(std::count_if(_spells.begin(), _spells.end(), [=](auto spell) {
			return (spell.type == SpellType::MAGE) && (spell.level == spell_level) && (spell.known);
		}))};
		if (_mage_max_sp[spell_level] < mage_known) {
			_mage_max_sp[spell_level] = mage_known;
			_mage_cur_sp[spell_level] = _mage_max_sp[spell_level];
		}
	}
}

// Set the starting spellpoints
auto Sorcery::Character::_set_starting_sp() -> void {

	using enum Enums::Character::Ability;
	using enum Enums::Character::Class;

	// By default clear all spells
	_clear_sp();

	// In the original code this is handled in "SETSPELS"/"SPLPERLV"/"NWMAGE"/"NWPRIEST"
	switch (_class) { // NOLINT(clang-diagnostic-switch)
	case PRIEST:
		_priest_max_sp[1] = (*_system->config)[ConfigOption::STRICT_MODE] ? 2 : 2 + _abilities[BONUS_PRIEST_SPELLS];
		break;
	case BISHOP:
		_mage_max_sp[1] = 2;
		break;
	case MAGE:
		_mage_max_sp[1] = (*_system->config)[ConfigOption::STRICT_MODE] ? 2 : 2 + _abilities[BONUS_MAGE_SPELLS];
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

auto Sorcery::Character::_learn_spell(SpellID spell_id) -> void {

	std::vector<Spell>::iterator it;
	it = std::find_if(_spells.begin(), _spells.end(), [&](auto item) { return item.id == spell_id; });
	if (it != _spells.end()) {
		(*it).known = true;
		_spells_known[(*it).id] = true;
	} // NOLINT(clang-di
}

// Set starting spells
auto Sorcery::Character::_set_start_spells() -> void {

	using enum Enums::Character::Class;
	using enum Enums::Magic::SpellID;

	// This is taken from "KEEPCHYN" which hard codes the spells known to beginning characters!
	switch (_class) { // NOLINT(clang-diagnostic-switch)
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
auto Sorcery::Character::_get_hp_per_level() -> int {

	using enum Enums::Character::Ability;
	using enum Enums::Character::Class;

	// In the original code ("MOREHP"), Samurai get 2d8
	auto extra_hp{0};
	switch (_class) { // NOLINT(clang-diagnostic-switch)
	case FIGHTER:
	case LORD:
		extra_hp += (*_system->random)[RandomType::D10];
		break;
	case PRIEST:
		extra_hp += (*_system->random)[RandomType::D8];
		break;
	case THIEF:
	case BISHOP:
	case NINJA:
		extra_hp += (*_system->random)[RandomType::D6];
		break;
	case MAGE:
		extra_hp += (*_system->random)[RandomType::D4];
		break;
	case SAMURAI:
		extra_hp += (*_system->random)[RandomType::D8];
		extra_hp += (*_system->random)[RandomType::D8];
		break;
	default:
		break;
	}

	extra_hp += _abilities[BONUS_HIT_POINTS];
	if (extra_hp < 0)
		extra_hp = 1;

	// Though this could be an unsigned int as it will always be greater than 0, just return int as everything else is
	return extra_hp;
}

// Add hit points on level gain (but note the strict mode limitation mentioned below)
auto Sorcery::Character::_update_hp_for_level() -> int {

	using enum Enums::Character::Ability;

	// Note the rerolling of all HP ("MADELEV") when levelling - and using MaxLevel achieved when in strict mode
	auto hp_gained{0};
	if ((*_system->config)[ConfigOption::REROLL_HIT_POINTS_ON_LEVEL_GAIN]) {
		auto hp_total{0};
		for (auto level = 1; level < _abilities[CURRENT_LEVEL]; level++)
			hp_total += _get_hp_per_level();
		if (hp_total < _abilities[MAX_HP])
			hp_total = _abilities[MAX_HP] + 1;
		hp_gained = hp_total - _abilities[MAX_HP];
		_abilities[MAX_HP] += hp_gained;
		_abilities[CURRENT_HP] += hp_gained;
	} else {
		hp_gained = _get_hp_per_level();
		_abilities[MAX_HP] += hp_gained;
		_abilities[CURRENT_HP] += hp_gained;
	}

	return hp_gained;
}

auto Sorcery::Character::get_current_hp() const -> int {

	using enum Enums::Character::Ability;

	return _abilities.at(CURRENT_HP);
}

auto Sorcery::Character::get_max_hp() const -> int {

	using enum Enums::Character::Ability;

	return _abilities.at(MAX_HP);
}

auto Sorcery::Character::_update_stat_for_level(CharacterAttribute attribute, std::string stat) -> std::string {

	using enum Enums::Character::Ability;
	using enum Enums::Character::Attribute;

	auto message{""s};

	if ((*_system->random)[RandomType::D100] < 75) {
		const auto chance{_abilities.at(AGE) / 130.f};
		if ((*_system->random)[RandomType::D100] < chance) {

			// Decrease
			bool proceed{true};
			if ((_cur_attr.at(attribute) == 18) && ((*_system->random)[RandomType::D6] > 1))
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

	using enum Enums::Character::Ability;
	using enum Enums::Character::Attribute;

	std::string results{};
	results.append((*_display->string)["LEVEL_DING"]);
	results.append("@");

	// increase level
	_abilities.at(CURRENT_LEVEL) = _abilities.at(CURRENT_LEVEL) + 1;
	_abilities.at(HIT_DICE) = _abilities.at(HIT_DICE) + 1;
	if (_abilities.at(CURRENT_LEVEL) > _abilities.at(MAX_LEVEL))
		_abilities.at(MAX_LEVEL) = _abilities.at(CURRENT_LEVEL);

	// handle learning spells
	if (_set_sp()) {
		results.append((*_display->string)["LEVEL_SPELLS"]);
		results.append("@");
	}

	// work out new xp needed
	_abilities[NEXT_LEVEL_XP] = _get_xp_for_level(_abilities[CURRENT_LEVEL]);

	// handle stat changing
	auto stat_message{""s};
	stat_message = _update_stat_for_level(STRENGTH, (*_display->string)["CHARACTER_STAT_STRENGTH"]);
	if (!stat_message.empty()) {
		results.append(stat_message);
		results.append("@");
	}
	stat_message = _update_stat_for_level(IQ, (*_display->string)["CHARACTER_STAT_INTELLIGENCE"]);
	if (!stat_message.empty()) {

		results.append(stat_message);
		results.append("@");
	}
	stat_message = _update_stat_for_level(PIETY, (*_display->string)["CHARACTER_STAT_PIETY"]);
	if (!stat_message.empty()) {
		results.append("@");
	}
	stat_message = _update_stat_for_level(VITALITY, (*_display->string)["CHARACTER_STAT_VITALITY"]);
	if (!stat_message.empty()) {
		results.append(stat_message);
		results.append("@");
	}
	stat_message = _update_stat_for_level(AGILITY, (*_display->string)["CHARACTER_STAT_AGILITY"]);
	if (!stat_message.empty()) {
		results.append("@");
	}
	stat_message = _update_stat_for_level(LUCK, (*_display->string)["CHARACTER_STAT_LUCK"]);
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

	if (_cur_attr.at(VITALITY) < 3) {
		results.append("@");
		results.append((*_display->string)["LEVEL_DIE"]);
		_status = CharacterStatus::LOST;
		_location = CharacterLocation::TRAINING;
	}

	return results;
}

// Level a character down (e.g. drain levels or give/increase negative levels_
auto Sorcery::Character::level_down() -> void {

	using enum Enums::Character::Ability;

	if (_abilities.at(CURRENT_LEVEL) == 1) {
		_status = CharacterStatus::LOST;
		_location = CharacterLocation::TRAINING;
		return;
	}

	// what to do about negative level ability?
	const auto old_level{_abilities.at(CURRENT_LEVEL)};
	const auto diff_hp{_abilities.at(MAX_HP) - _abilities.at(CURRENT_HP)};

	_abilities.at(CURRENT_LEVEL) = _abilities.at(CURRENT_LEVEL) - 1;
	_abilities.at(HIT_DICE) = _abilities.at(HIT_DICE) - 1;
	if (_abilities.at(MAX_LEVEL) > _abilities.at(CURRENT_LEVEL))
		_abilities.at(MAX_LEVEL) = _abilities.at(CURRENT_LEVEL);

	_set_sp();

	// When drained XP is set to beginning of current level (for example, draining to level 9 means that youre xp is set
	// to enough for level 9 plus 1 - which is vety harsh)
	_abilities[NEXT_LEVEL_XP] = _get_xp_for_level(_abilities.at(CURRENT_LEVEL));
	_abilities[CURRENT_XP] = _get_xp_for_level(_abilities.at(CURRENT_LEVEL) - 1) + 1;

	_generate_secondary_abil(false, false, false);
	_abilities[MAX_HP] = _abilities.at(MAX_HP) * (_abilities.at(CURRENT_LEVEL) / (old_level * 1.f));
	_abilities[CURRENT_HP] = _abilities[MAX_HP] - diff_hp;
	if (_abilities[CURRENT_HP] < 0)
		_abilities[CURRENT_HP] = 0;
}

// For each spell level, try to learn spells - called before set_spellpoints
auto Sorcery::Character::_try_learn_spell(SpellType spell_type, unsigned int spell_level) -> bool {

	bool new_spell_learnt{false};

	// Only do spells if a character has available spell points in this spell level
	if (spell_type == SpellType::PRIEST)
		if (_priest_max_sp[spell_level] == 0)
			return false;
	if (spell_type == SpellType::MAGE)
		if (_mage_max_sp[spell_level] == 0)
			return false;

	// Get all unknown spells of this spell level and type
	auto spells{_spells | std::views::filter([&](Spell spell) {
		return (spell.type == spell_type) && (spell.level == spell_level) && (spell.known == false);
	})};
	for (auto &spell : spells) {

		const auto dice{(*_system->random)[RandomType::ZERO_TO_29]};

		// Check the Spell Type against the relevant stat (see SPLPERLV//TRYLEARN)
		if (spell_type == SpellType::PRIEST) {
			if (dice <= static_cast<unsigned int>(_cur_attr[CharacterAttribute::PIETY])) {
				spell.known = true;
				_spells_known[spell.id] = true;
				new_spell_learnt = true;
			}
		}
		if (spell_type == SpellType::MAGE) {
			if (dice <= static_cast<unsigned int>(_cur_attr[CharacterAttribute::IQ])) {
				spell.known = true;
				_spells_known[spell.id] = true;
				new_spell_learnt = true;
			}
		}
	}

	return new_spell_learnt;
}

// Reimplementation of SPLPERLV - note this will reset spell points!
auto Sorcery::Character::_calculate_sp(
	SpellType spell_type, unsigned int level_mod, unsigned int level_offset) -> void {

	using enum Enums::Character::Ability;

	SpellPoints *spells{spell_type == SpellType::PRIEST ? &_priest_max_sp : &_mage_max_sp};
	for (auto spell_level = 1; spell_level <= 7; spell_level++)
		(*spells)[spell_level] = 0;

	// https://datadrivengamer.blogspot.com/2019/08/the-not-so-basic-mechanics-of-wizardry.html
	for (auto spell_level = 1; spell_level <= 7; spell_level++) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnarrowing"
		int spell_count{_abilities[CURRENT_LEVEL] - level_mod + level_offset - (level_offset * spell_level)};
#pragma GCC diagnostic pop
		spell_count = std::max(0, std::min(spell_count, 9));
		(*spells)[spell_level] = spell_count;
	}
}

// Copied and rewritten from the original code from MINMAG/MINPRI/NWPRIEST/NWMAGE
auto Sorcery::Character::_set_sp() -> bool {

	using enum Enums::Character::Ability;
	using enum Enums::Character::Class;
	using enum Enums::Magic::SpellID;

	bool new_spells_learnt{false};

	for (auto spell_level = 1; spell_level <= 7; spell_level++) {
		_priest_max_sp[spell_level] = 0;
		_mage_max_sp[spell_level] = 0;
	}

	// Generate spell points according to current level and class (this does not change any spells known but will reset
	// spell points)
	switch (_class) { // NOLINT(clang-diagnostic-switch)
	case FIGHTER:
	case THIEF:
	case NINJA:
		break;
	case MAGE:
		_calculate_sp(SpellType::MAGE, 0, 2);
		break;
	case PRIEST:
		_calculate_sp(SpellType::PRIEST, 0, 2);
		break;
	case BISHOP:
		_calculate_sp(SpellType::PRIEST, 3, 4);
		_calculate_sp(SpellType::MAGE, 0, 4);
		break;
	case SAMURAI:
		_calculate_sp(SpellType::MAGE, 3, 3);
		break;
	case LORD:
		_calculate_sp(SpellType::PRIEST, 3, 2);
		break;
	default:
		break;
	}

	// Now try and learn any additional spells
	for (auto spell_level = 1; spell_level <= 7; spell_level++) {

		// If we know at least one spell in this level, we can always try and learn more no matter what even if we are
		// currently a non-spellcasting class
		if (_priest_max_sp[spell_level] > 0) {
			if (_try_learn_spell(SpellType::PRIEST, spell_level))
				new_spells_learnt = true;
		}
		if (_mage_max_sp[spell_level] > 0) {
			if (_try_learn_spell(SpellType::MAGE, spell_level))
				new_spells_learnt = true;
		}
	}

	// Now make sure that if the above fails, we always learn a spell of each circle just in case (GETMAGSP/GETPRISP -
	// though note that the orignal is bugged and selects the wrong level spells sometimes) - note this is for creatures
	// so it might not be applicable?
	for (auto spell_level = 1; spell_level <= 7; spell_level++) {

		if ((_priest_max_sp[spell_level] > 0) && (_get_spells_known(SpellType::PRIEST, spell_level) == 0)) {
			switch (spell_level) {
			case 1:
				_learn_spell(BADIOS);
				break;
			case 2:
				_learn_spell(MONTINO);
				new_spells_learnt = true;
				break;
			case 3:
				if ((*_system->random)[RandomType::D100] > 33)
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
				if ((*_system->random)[RandomType::D100] > 33)
					_learn_spell(BADIALMA);
				else
					_learn_spell(BADI);
				new_spells_learnt = true;
				break;
			case 6:
				if ((*_system->random)[RandomType::D100] > 33)
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
		if ((_mage_max_sp[spell_level] > 0) && (_get_spells_known(SpellType::MAGE, spell_level) == 0)) {
			switch (spell_level) {
			case 1:
				if ((*_system->random)[RandomType::D100] > 33)
					_learn_spell(KATINO);
				else
					_learn_spell(HALITO);
				new_spells_learnt = true;
				break;
			case 2:
				if ((*_system->random)[RandomType::D100] > 33)
					_learn_spell(DILTO);
				else
					_learn_spell(SOPIC);
				new_spells_learnt = true;
				break;
			case 3:
				if ((*_system->random)[RandomType::D100] > 33)
					_learn_spell(MOLITO);
				else
					_learn_spell(MAHALITO);
				new_spells_learnt = true;
				break;
			case 4:
				if ((*_system->random)[RandomType::D100] > 33)
					_learn_spell(DALTO);
				else
					_learn_spell(LAHALITO);
				new_spells_learnt = true;
				break;
			case 5:
				if ((*_system->random)[RandomType::D100] > 33)
					_learn_spell(MAMORLIS);
				else
					_learn_spell(MADALTO);
				new_spells_learnt = true;
				break;
			case 6:
				if ((*_system->random)[RandomType::D100] > 33)
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

	// And work out spells known and boost sp accordingly if we have to but note that we can't go above maxlevel/2 (for
	// the case of level drain)
	for (auto spell_level = 1; spell_level <= 7; spell_level++) {

		if (((spell_level * 2) - 1) > (_abilities.at(MAX_LEVEL)))
			continue;

		const auto priest_known{_get_spells_known(SpellType::PRIEST, spell_level)};
		const auto mage_known{_get_spells_known(SpellType::MAGE, spell_level)};

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
auto Sorcery::Character::_get_spells_known(SpellType spell_type, unsigned int spell_level) -> unsigned int {

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

	using enum Enums::Magic::SpellCategory;
	using enum Enums::Magic::SpellID;
	using enum Enums::Magic::SpellType;

	// Mage Spells (grouped by level)

	// TODO: get the autotranslated Japanese text from the PSX versions, and use these, but also put this into the
	// game strings file

	// Level 1
	auto level{1};
	_spells.emplace_back(DUMAPIC, MAGE, FIELD, level, false, (*_display->string)["SPELL_DUMAPIC_NAME"],
		(*_display->string)["SPELL_DUMAPIC_TITLE"], (*_display->string)["SPELL_DUMAPIC_DESC"]);
	_spells.emplace_back(HALITO, MAGE, ATTACK, level, false, (*_display->string)["SPELL_HALITO_NAME"],
		(*_display->string)["SPELL_HALITO_TITLE"], (*_display->string)["SPELL_HALITO_DESC"]);
	_spells.emplace_back(KATINO, MAGE, DISABLE, level, false, (*_display->string)["SPELL_KATINO_NAME"],
		(*_display->string)["SPELL_KATINO_TITLE"], (*_display->string)["SPELL_KATINO_DESC"]);
	_spells.emplace_back(MOGREF, MAGE, SUPPORT, level, false, (*_display->string)["SPELL_MOGREF_NAME"],
		(*_display->string)["SPELL_MOGREF_TITLE"], (*_display->string)["SPELL_MOGREF_DESC"]);

	// Level 2
	++level;
	_spells.emplace_back(DILTO, MAGE, DISABLE, level, false, (*_display->string)["SPELL_DILTO_NAME"],
		(*_display->string)["SPELL_DILTO_TITLE"], (*_display->string)["SPELL_DILTO_DESC"]);
	_spells.emplace_back(SOPIC, MAGE, SUPPORT, level, false, (*_display->string)["SPELL_SOPIC_NAME"],
		(*_display->string)["SPELL_SOPIC_TITLE"], (*_display->string)["SPELL_SOPIC_DESC"]);

	// Level 3
	++level;
	_spells.emplace_back(MAHALITO, MAGE, ATTACK, level, false, (*_display->string)["SPELL_MAHALITO_NAME"],
		(*_display->string)["SPELL_MAHALITO_TITLE"], (*_display->string)["SPELL_MAHALITO_DESC"]);
	_spells.emplace_back(MOLITO, MAGE, ATTACK, level, false, (*_display->string)["SPELL_MOLITO_NAME"],
		(*_display->string)["SPELL_MOLITO_TITLE"], (*_display->string)["SPELL_MOLITO_DESC"]);

	// Level 4
	++level;
	_spells.emplace_back(DALTO, MAGE, ATTACK, level, false, (*_display->string)["SPELL_DALTO_NAME"],
		(*_display->string)["SPELL_DALTO_TITLE"], (*_display->string)["SPELL_DALTO_DESC"]);
	_spells.emplace_back(LAHALITO, MAGE, ATTACK, level, false, (*_display->string)["SPELL_LAHALITO_NAME"],
		(*_display->string)["SPELL_LAHALITO_TITLE"], (*_display->string)["SPELL_LAHALITO_DESC"]);
	_spells.emplace_back(MORLIS, MAGE, DISABLE, level, false, (*_display->string)["SPELL_MORLIS_NAME"],
		(*_display->string)["SPELL_MORLIS_TITLE"], (*_display->string)["SPELL_MORLIS_DESC"]);

	// Level 5
	++level;
	_spells.emplace_back(MADALTO, MAGE, ATTACK, level, false, (*_display->string)["SPELL_MADALTO_NAME"],
		(*_display->string)["SPELL_MADALTO_TITLE"], (*_display->string)["SPELL_MADALTO_DESC"]);
	_spells.emplace_back(MAKANITO, MAGE, ATTACK, level, false, (*_display->string)["SPELL_MAKANITO_NAME"],
		(*_display->string)["SPELL_MAKANITO_TITLE"], (*_display->string)["SPELL_MAKANITO_DESC"]);
	_spells.emplace_back(MAMORLIS, MAGE, DISABLE, level, false, (*_display->string)["SPELL_MAMORLIS_NAME"],
		(*_display->string)["SPELL_MAMORLIS_TITLE"], (*_display->string)["SPELL_MAMORLIS_DESC"]);

	// Level 6
	++level;
	_spells.emplace_back(HAMAN, MAGE, SUPPORT, level, false, (*_display->string)["SPELL_HAMAN_NAME"],
		(*_display->string)["SPELL_HAMAN_TITLE"], (*_display->string)["SPELL_HAMAN_DESC"]);
	_spells.emplace_back(LAKANITO, MAGE, ATTACK, level, false, (*_display->string)["SPELL_LAKANITO_NAME"],
		(*_display->string)["SPELL_LAKANITO_TITLE"], (*_display->string)["SPELL_LAKANITO_DESC"]);
	_spells.emplace_back(MASOPIC, MAGE, ATTACK, level, false, (*_display->string)["SPELL_MASOPIC_NAME"],
		(*_display->string)["SPELL_MASOPIC_TITLE"], (*_display->string)["SPELL_MASOPIC_DESC"]);
	_spells.emplace_back(ZILWAN, MAGE, ATTACK, level, false, (*_display->string)["SPELL_ZILWAN_NAME"],
		(*_display->string)["SPELL_ZILWAN_TITLE"], (*_display->string)["SPELL_ZILWAN_DESC"]);

	// Level 7
	++level;
	_spells.emplace_back(MAHAMAN, MAGE, SUPPORT, level, false, (*_display->string)["SPELL_MAHAMAN_NAME"],
		(*_display->string)["SPELL_MAHAMAN_TITLE"], (*_display->string)["SPELL_MAHAMAN_DESC"]);
	_spells.emplace_back(MALOR, MAGE, FIELD, level, false, (*_display->string)["SPELL_MALOR_NAME"],
		(*_display->string)["SPELL_MALOR_TITLE"], (*_display->string)["SPELL_MALOR_DESC"]);
	_spells.emplace_back(TILTOWAIT, MAGE, ATTACK, level, false, (*_display->string)["SPELL_TILTOWAIT_NAME"],
		(*_display->string)["SPELL_TILTOWAIT_TITLE"], (*_display->string)["SPELL_TILTOWAIT_DESC"]);

	// Priest Spells (grouped by level)

	// Level 1
	level = 1u;
	_spells.emplace_back(BADIOS, PRIEST, ATTACK, level, false, (*_display->string)["SPELL_BADIOS_NAME"],
		(*_display->string)["SPELL_BADIOS_TITLE"], (*_display->string)["SPELL_BADIOS_DESC"]);
	_spells.emplace_back(DIOS, PRIEST, HEALING, level, false, (*_display->string)["SPELL_DIOS_NAME"],
		(*_display->string)["SPELL_DIOS_TITLE"], (*_display->string)["SPELL_DIOS_DESC"]);
	_spells.emplace_back(KALKI, PRIEST, SUPPORT, level, false, (*_display->string)["SPELL_KALKI_NAME"],
		(*_display->string)["SPELL_KALKI_TITLE"], (*_display->string)["SPELL_KALKI_DESC"]);
	_spells.emplace_back(MILWA, PRIEST, FIELD, level, false, (*_display->string)["SPELL_MILWA_NAME"],
		(*_display->string)["SPELL_MILWA_TITLE"], (*_display->string)["SPELL_MILWA_DESC"]);
	_spells.emplace_back(PORFIC, PRIEST, SUPPORT, level, false, (*_display->string)["SPELL_PORFIC_NAME"],
		(*_display->string)["SPELL_PORFIC_TITLE"], (*_display->string)["SPELL_PORFIC_DESC"]);

	// Level 2
	++level;
	_spells.emplace_back(CALFO, PRIEST, FIELD, level, false, (*_display->string)["SPELL_CALFO_NAME"],
		(*_display->string)["SPELL_CALFO_TITLE"], (*_display->string)["SPELL_CALFO_DESC"]);
	_spells.emplace_back(MANIFO, PRIEST, DISABLE, level, false, (*_display->string)["SPELL_MANIFO_NAME"],
		(*_display->string)["SPELL_MANIFO_TITLE"], (*_display->string)["SPELL_MANIFO_DESC"]);
	_spells.emplace_back(MATU, PRIEST, SUPPORT, level, false, (*_display->string)["SPELL_MATU_NAME"],
		(*_display->string)["SPELL_MATU_TITLE"], (*_display->string)["SPELL_MATU_DESC"]);
	_spells.emplace_back(MONTINO, PRIEST, DISABLE, level, false, (*_display->string)["SPELL_MONTINO_NAME"],
		(*_display->string)["SPELL_MONTINO_TITLE"], (*_display->string)["SPELL_MONTINO_DESC"]);

	// Level 3
	++level;
	_spells.emplace_back(BAMATU, PRIEST, SUPPORT, level, false, (*_display->string)["SPELL_BAMATU_NAME"],
		(*_display->string)["SPELL_BAMATU_TITLE"], (*_display->string)["SPELL_BAMATU_DESC"]);
	_spells.emplace_back(DIALKO, PRIEST, HEALING, level, false, (*_display->string)["SPELL_DIALKO_NAME"],
		(*_display->string)["SPELL_DIALKO_TITLE"], (*_display->string)["SPELL_DIALKO_DESC"]);
	_spells.emplace_back(LATUMAPIC, PRIEST, FIELD, level, false, (*_display->string)["SPELL_LATUMAPIC_NAME"],
		(*_display->string)["SPELL_LATUMAPIC_TITLE"], (*_display->string)["SPELL_LATUMAPIC_DESC"]);
	_spells.emplace_back(LOMILWA, PRIEST, FIELD, level, false, (*_display->string)["SPELL_LOMILWA_NAME"],
		(*_display->string)["SPELL_LOMILWA_TITLE"], (*_display->string)["SPELL_LOMILWA_DESC"]);

	// Level 4
	++level;
	_spells.emplace_back(BADIAL, PRIEST, ATTACK, level, false, (*_display->string)["SPELL_BADIAL_NAME"],
		(*_display->string)["SPELL_BADIAL_TITLE"], (*_display->string)["SPELL_BADIAL_DESC"]);
	_spells.emplace_back(DIAL, PRIEST, HEALING, level, false, (*_display->string)["SPELL_DIAL_NAME"],
		(*_display->string)["SPELL_DIAL_TITLE"], (*_display->string)["SPELL_DIAL_DESC"]);
	_spells.emplace_back(LATUMOFIS, PRIEST, HEALING, level, false, (*_display->string)["SPELL_LATUMOFIS_NAME"],
		(*_display->string)["SPELL_LATUMOFIS_TITLE"], (*_display->string)["SPELL_LATUMOFIS_DESC"]);
	_spells.emplace_back(MAPORFIC, PRIEST, FIELD, level, false, (*_display->string)["SPELL_MAPORFIC_NAME"],
		(*_display->string)["SPELL_MAPORFIC_TITLE"], (*_display->string)["SPELL_MAPORFIC_DESC"]);

	// Level 5
	++level;
	_spells.emplace_back(BADI, PRIEST, ATTACK, level, false, (*_display->string)["SPELL_BADI_NAME"],
		(*_display->string)["SPELL_BADI_TITLE"], (*_display->string)["SPELL_BADI_DESC"]);
	_spells.emplace_back(BADIALMA, PRIEST, ATTACK, level, false, (*_display->string)["SPELL_BADIALMA_NAME"],
		(*_display->string)["SPELL_BADIALMA_TITLE"], (*_display->string)["SPELL_BADIALMA_DESC"]);
	_spells.emplace_back(DI, PRIEST, HEALING, level, false, (*_display->string)["SPELL_DI_NAME"],
		(*_display->string)["SPELL_DI_TITLE"], (*_display->string)["SPELL_DI_DESC"]);
	_spells.emplace_back(DIALMA, PRIEST, HEALING, level, false, (*_display->string)["SPELL_DIALMA_NAME"],
		(*_display->string)["SPELL_DIALMA_TITLE"], (*_display->string)["SPELL_DIALMA_DESC"]);
	_spells.emplace_back(KANDI, PRIEST, FIELD, level, false, (*_display->string)["SPELL_KANDI_NAME"],
		(*_display->string)["SPELL_KANDI_TITLE"], (*_display->string)["SPELL_KANDI_DESC"]);

	// Level 6
	++level;
	_spells.emplace_back(LOKTOFEIT, PRIEST, FIELD, level, false, (*_display->string)["SPELL_LOKTOFEIT_NAME"],
		(*_display->string)["SPELL_LOKTOFEIT_TITLE"], (*_display->string)["SPELL_LOKTOFEIT_DESC"]);
	_spells.emplace_back(LORTO, PRIEST, ATTACK, level, false, (*_display->string)["SPELL_LORTO_NAME"],
		(*_display->string)["SPELL_LORTO_TITLE"], (*_display->string)["SPELL_LORTO_DESC"]);
	_spells.emplace_back(MABADI, PRIEST, ATTACK, level, false, (*_display->string)["SPELL_MABADI_NAME"],
		(*_display->string)["SPELL_MABADI_TITLE"], (*_display->string)["SPELL_MABADI_DESC"]);
	_spells.emplace_back(MADI, PRIEST, HEALING, level, false, (*_display->string)["SPELL_MADI_NAME"],
		(*_display->string)["SPELL_MADI_TITLE"], (*_display->string)["SPELL_MADI_DESC"]);

	// Level 7
	++level;
	_spells.emplace_back(KADORTO, PRIEST, HEALING, level, false, (*_display->string)["SPELL_KADORTO_NAME"],
		(*_display->string)["SPELL_KADORTO_TITLE"], (*_display->string)["SPELL_KADORTO_DESC"]);
	_spells.emplace_back(MALIKTO, PRIEST, ATTACK, level, false, (*_display->string)["SPELL_MALIKTO_NAME"],
		(*_display->string)["SPELL_MALIKTO_TITLE"], (*_display->string)["SPELL_MALIKTO_DESC"]);
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
auto Sorcery::Character::create_class_alignment(
	const CharacterClass cclass, const CharacterAlignment alignment) -> void {

	using enum Enums::Character::Align;
	using enum Enums::Character::Attribute;
	using enum Enums::Character::Class;
	using enum Enums::Character::Race;

	_class = cclass;
	_race = static_cast<CharacterRace>((*_system->random)[RandomType::D5]);
	_alignment = alignment;

	switch (_race) { // NOLINT(clang-diagnostic-switch)
	case HUMAN:
		_start_attr = {{STRENGTH, 8}, {IQ, 5}, {PIETY, 5}, {VITALITY, 8}, {AGILITY, 8}, {LUCK, 9}};
		break;
	case ELF:
		_start_attr = {{STRENGTH, 7}, {IQ, 10}, {PIETY, 10}, {VITALITY, 6}, {AGILITY, 9}, {LUCK, 6}};
		break;
	case DWARF:
		_start_attr = {{STRENGTH, 10}, {IQ, 7}, {PIETY, 10}, {VITALITY, 10}, {AGILITY, 5}, {LUCK, 6}};
		break;
	case GNOME:
		_start_attr = {{STRENGTH, 7}, {IQ, 7}, {PIETY, 10}, {VITALITY, 8}, {AGILITY, 10}, {LUCK, 7}};
		break;
	case HOBBIT:
		_start_attr = {{STRENGTH, 5}, {IQ, 7}, {PIETY, 7}, {VITALITY, 6}, {AGILITY, 10}, {LUCK, 12}};
		break;
	default:
		break;
	}

	// Put most of the points into the main attribute (note that 10 points means a Human Priest and Dwarf Thief have
	// allocated all points to their main attribute with no points left over)
	_points_left = 10;
	_st_points = _points_left;
	switch (_class) { // NOLINT(clang-diagnostic-switch)
	case FIGHTER:
		[[fallthrough]];
	case LORD:
		[[fallthrough]];
	case SAMURAI:
		_points_left -= (15 - _start_attr[STRENGTH]);
		_start_attr[STRENGTH] = 15;
		break;
	case MAGE:
		[[fallthrough]];
	case BISHOP:
		_points_left -= (15 - _start_attr[IQ]);
		_start_attr[IQ] = 15;
		break;
	case PRIEST:
		_points_left -= (15 - _start_attr[PIETY]);
		_start_attr[PIETY] = 15;
		break;
	case THIEF:
		[[fallthrough]];
	case NINJA:
		_points_left -= (15 - _start_attr[AGILITY]);
		_start_attr[AGILITY] = 15;
		break;
	default:
		break;
	}

	// Pump any points left into the Vitality attribute
	if (_points_left > 0)
		_start_attr[VITALITY] += _points_left;

	_cur_attr = _start_attr;

	_name = _system->random->get_random_name();
	_portrait_index = (*_system->random)[RandomType::ZERO_TO_29];
}

// Enter Name and Portrait, rest is random
auto Sorcery::Character::create_quick() -> void {

	using enum Enums::Character::Align;
	using enum Enums::Character::Attribute;
	using enum Enums::Character::Class;
	using enum Enums::Character::Race;

	// Exclude Samurai/Lord/Ninja/Bishop from this method of character creation
	_class = static_cast<CharacterClass>((*_system->random)[RandomType::D4]);
	_race = static_cast<CharacterRace>((*_system->random)[RandomType::D5]);
	switch (_class) { // NOLINT(clang-diagnostic-switch)
	case FIGHTER:
	case MAGE:
		_alignment = static_cast<CharacterAlignment>((*_system->random)[RandomType::D3]);
		break;
	case PRIEST:
		_alignment = (*_system->random)[RandomType::D2] == 1 ? GOOD : EVIL;
		break;
	case THIEF:
		_alignment = (*_system->random)[RandomType::D2] == 1 ? NEUTRAL : EVIL;
		break;
	default:
		break;
	}

	// Now get minimum attributes for race/class combo (note as we are only allowing creation of some classes, it
	// will be as if we had a maximum of 10 bonus points to spend - in order to incentivise full blown character
	// creation! see table IV (A) at
	// https://gamefaqs.gamespot.com/pc/946844-the-ultimate-wizardry-archives/faqs/45726 for info
	switch (_race) { // NOLINT(clang-diagnostic-switch)
	case HUMAN:
		_start_attr = {{STRENGTH, 8}, {IQ, 5}, {PIETY, 5}, {VITALITY, 8}, {AGILITY, 8}, {LUCK, 9}};
		break;
	case ELF:
		_start_attr = {{STRENGTH, 7}, {IQ, 10}, {PIETY, 10}, {VITALITY, 6}, {AGILITY, 9}, {LUCK, 6}};
		break;
	case DWARF:
		_start_attr = {{STRENGTH, 10}, {IQ, 7}, {PIETY, 10}, {VITALITY, 10}, {AGILITY, 5}, {LUCK, 6}};
		break;
	case GNOME:
		_start_attr = {{STRENGTH, 7}, {IQ, 7}, {PIETY, 10}, {VITALITY, 8}, {AGILITY, 10}, {LUCK, 7}};
		break;
	case HOBBIT:
		_start_attr = {{STRENGTH, 5}, {IQ, 7}, {PIETY, 7}, {VITALITY, 6}, {AGILITY, 10}, {LUCK, 12}};
		break;
	default:
		break;
	}

	// Put most of the points into the main attribute (note that 10 points means a Human Priest and Dwarf Thief have
	// allocated all points to their main attribute with no points left over)
	_points_left = 10;
	_st_points = _points_left;
	switch (_class) { // NOLINT(clang-diagnostic-switch)
	case FIGHTER:
		_points_left -= (15 - _start_attr[STRENGTH]);
		_start_attr[STRENGTH] = 15;
		break;
	case MAGE:
		_points_left -= (15 - _start_attr[IQ]);
		_start_attr[IQ] = 15;
		break;
	case PRIEST:
		_points_left -= (15 - _start_attr[PIETY]);
		_start_attr[PIETY] = 15;
		break;
	case THIEF:
		_points_left -= (15 - _start_attr[AGILITY]);
		_start_attr[AGILITY] = 15;
		break;
	default:
		break;
	}

	// Pump any points left into the Vitality attribute
	if (_points_left > 0)
		_start_attr[VITALITY] += _points_left;

	_cur_attr = _start_attr;
}

// Create a (semi) random character
auto Sorcery::Character::create_random() -> void {

	// Random Name and Portrait
	create_quick();
	_name = _system->random->get_random_name();
	_portrait_index = (*_system->random)[RandomType::ZERO_TO_29];
}

auto Sorcery::Character::get_status() const -> CharacterStatus {

	return _status;
}

auto Sorcery::Character::get_condition() const -> std::string {

	return _get_condition();
}

auto Sorcery::Character::get_short_cond() const -> std::string {

	if (_status != CharacterStatus::OK)
		return fmt::format("{:>6}", _get_condition());
	else
		return fmt::format("{:>4}", _abilities.at(CharacterAbility::MAX_HP));
}

auto Sorcery::Character::_get_condition() const -> std::string {

	using enum Enums::Character::CStatus;

	if (is_poisoned() && (_status == OK)) {
		return (*_display->string)["STATUS_POISONED"];
	} else if (_status == OK)
		return (*_display->string)["STATUS_OK"];
	else {
		switch (_status) {
		case AFRAID:
			return (*_display->string)["STATUS_AFRAID"];
			break;
		case ASHES:
			return (*_display->string)["STATUS_ASHES"];
			break;
		case ASLEEP:
			return (*_display->string)["STATUS_ASLEEP"];
			break;
		case DEAD:
			return (*_display->string)["STATUS_DEAD"];
			break;
		case LOST:
			return (*_display->string)["STATUS_LOST"];
			break;
		case HELD:
			return (*_display->string)["STATUS_PARALYSED"];
			break;
		case SILENCED:
			return (*_display->string)["STATUS_SILENCED"];
			break;
		case STONED:
			return (*_display->string)["STATUS_STONED"];
			break;
		default:
			return "";
		}
	}
}

auto Sorcery::Character::get_loc_str() const -> std::string {

	using enum Enums::Character::Location;

	switch (_location) {
	case PARTY:
		return (*_display->string)["LOCATION_PARTY"];
		break;
	case TAVERN:
		return (*_display->string)["LOCATION_TAVERN"];
		break;
	case TRAINING:
		return (*_display->string)["LOCATION_TRAINING"];
		break;
	case TEMPLE:
		return (*_display->string)["LOCATION_TEMPLE"];
		break;
	case MAZE:
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

auto Sorcery::Character::set_status(CharacterStatus value) -> void {

	if (value == CharacterStatus::OK)
		_status = value;
	else {
		auto candidate{unenum(value)};
		auto current{unenum(_status)};
		if (candidate > current)
			_status = value;
	}
}

auto Sorcery::Character::set_current_hp(const int hp) -> void {

	using enum Enums::Character::Ability;

	_abilities[CURRENT_HP] = hp;
}

auto Sorcery::Character::is_poisoned() const -> bool {

	using enum Enums::Character::Ability;

	return _abilities.at(POISON_STRENGTH) > 0;
}

auto Sorcery::Character::get_poisoned_rate() const -> int {

	using enum Enums::Character::Ability;

	return _abilities.at(POISON_STRENGTH);
}

auto Sorcery::Character::get_hp_adjustment() const -> int {

	using enum Enums::Character::Ability;

	return _abilities.at(HP_GAIN_PER_TURN) - _abilities.at(HP_LOSS_PER_TURN) - _abilities.at(POISON_STRENGTH);
}

auto Sorcery::Character::get_hp_adjustment_symbol() const -> char {

	using enum Enums::Character::Ability;

	const auto rate{
		(_abilities.at(HP_GAIN_PER_TURN) - _abilities.at(HP_LOSS_PER_TURN) - _abilities.at(POISON_STRENGTH)) <=> 0};
	if (rate < 0)
		return '-';
	else if (rate > 0)
		return '+';
	else
		return ' ';
}

auto Sorcery::Character::set_hp_gain_per_turn(const int adjustment) -> void {

	using enum Enums::Character::Ability;

	_abilities.at(HP_GAIN_PER_TURN) = adjustment;
}

auto Sorcery::Character::set_hp_loss_per_turn(const int adjustment) -> void {

	using enum Enums::Character::Ability;

	_abilities.at(HP_LOSS_PER_TURN) = adjustment;
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

	using enum Enums::Character::Ability;

	return _abilities.at(POISON_STRENGTH) > 0 ? fmt::format("{:->2}", _abilities.at(POISON_STRENGTH)) : "";
}

auto Sorcery::Character::get_short_hp_summary() const -> std::string {

	using enum Enums::Character::Ability;

	return std::to_string(_abilities.at(CURRENT_HP));
}

auto Sorcery::Character::get_hp_summary() const -> std::string {

	using enum Enums::Character::Ability;

	return fmt::format("{}/{}{}", std::to_string(_abilities.at(CURRENT_HP)), std::to_string(_abilities.at(MAX_HP)),
		get_hp_adjustment_symbol());
}

auto Sorcery::Character::get_spell_points(
	const SpellType type, const SpellPointStatus status) const -> std::optional<SpellPoints> {

	using enum Enums::Magic::SpellPointStatus;
	using enum Enums::Magic::SpellType;

	if ((type == MAGE) && (status == CURRENT))
		return _mage_cur_sp;
	else if ((type == MAGE) && (status == MAXIMUM))
		return _mage_max_sp;
	else if ((type == PRIEST) && (status == CURRENT))
		return _priest_cur_sp;
	else if ((type == PRIEST) && (status == MAXIMUM))
		return _priest_max_sp;
	else
		return std::nullopt;
}

auto Sorcery::Character::_get_sp_per_level(const SpellType type, int level) -> std::string {

	if (type == SpellType::MAGE)
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

	using enum Enums::Character::Ability;

	auto name{_name};
	if (_display->get_upper())
		std::ranges::transform(name.begin(), name.end(), name.begin(), ::toupper);
	return fmt::format("{:<15} L {:>2} {}-{} {}", name, _abilities.at(CURRENT_LEVEL),
		alignment_to_str(_alignment).substr(0, 1), class_to_str(_class).substr(0, 3), race_to_str(_race).substr(0, 3));
}

auto Sorcery::Character::get_summary_and_out() -> std::string {

	using enum Enums::Character::Ability;

	auto name{_name};
	if (_display->get_upper())
		std::ranges::transform(name.begin(), name.end(), name.begin(), ::toupper);
	auto location{[&] {
		if (_location == CharacterLocation::MAZE)
			return "  OUT";
		else if (_status == CharacterStatus::LOST)
			return " LOST";
		else
			return "    ";
	}()};

	return fmt::format("{:<15} L {:>2} {}-{} {}{:>5}", name, _abilities.at(CURRENT_LEVEL),
		alignment_to_str(_alignment).substr(0, 1), class_to_str(_class).substr(0, 3), race_to_str(_race).substr(0, 3),
		location);
}

auto Sorcery::Character::can_level() const -> bool {

	using enum Enums::Character::Ability;

	return _abilities.at(CURRENT_XP) > _abilities.at(NEXT_LEVEL_XP);
}

auto Sorcery::Character::get_cure_cost() const -> unsigned int {

	using enum Enums::Character::Ability;
	using enum Enums::Character::CStatus;

	auto cost_per_level{0u};
	switch (_status) {
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

	return cost_per_level * _abilities.at(CURRENT_LEVEL);
}

auto Sorcery::Character::get_sb_text(const int position) -> std::string {

	using enum Enums::Character::Ability;

	auto name{_name};
	if (_display->get_upper())
		std::ranges::transform(name.begin(), name.end(), name.begin(), ::toupper);
	return fmt::format("{} {:<15} {}-{} {:>2} {:>4}{}{:<6}", position, name, alignment_to_str(_alignment).substr(0, 1),
		class_to_str(_class).substr(0, 3), _abilities.at(CURRENT_ARMOUR_CLASS), get_short_hp_summary(),
		get_hp_adjustment_symbol(), get_short_cond());
}

auto Sorcery::Character::get_age() const -> int {

	using enum Enums::Character::Ability;

	return _abilities.at(AGE);
}

auto Sorcery::Character::set_age(const int adjustment) -> void {

	using enum Enums::Character::Ability;

	_abilities.at(AGE) = _abilities.at(AGE) + adjustment;
}

auto Sorcery::Character::summary_text() -> std::string {

	using enum Enums::Character::Ability;
	using enum Enums::Character::Stage;

	auto name{_name};
	if (_display->get_upper())
		std::ranges::transform(name.begin(), name.end(), name.begin(), ::toupper);
	auto legacy{_legated ? " (D)" : ""};
	switch (_current_stage) {
	case CHOOSE_METHOD:
		[[fallthrough]];
	case ENTER_NAME:
		return fmt::format("{:<15} L ?? ?-??? ???", "???");
		break;
	case CHOOSE_RACE:
		return fmt::format("{:<15} L {:>2} ?-??? ???", name, _abilities.at(CURRENT_LEVEL));
		break;
	case CHOOSE_ALIGNMENT:
		return fmt::format("{:<15} L {:>2} ?-??? {}", name, _abilities.at(CURRENT_LEVEL), race_to_str(_race));
		break;
	case ALLOCATE_STATS:
		return fmt::format("{:<15} L {:>2} {}-??? {}", name, _abilities.at(CURRENT_LEVEL),
			alignment_to_str(_alignment).substr(0, 1), race_to_str(_race));
		break;
	case CHOOSE_CLASS:
		return fmt::format("{:<15} L {:>2} {}-??? {}", name, _abilities.at(CURRENT_LEVEL),
			alignment_to_str(_alignment).substr(0, 1), race_to_str(_race));
		break;
	case CHOOSE_PORTRAIT:
		return fmt::format("{:<15} L {:>2} {}-{} {}", name, _abilities.at(CURRENT_LEVEL),
			alignment_to_str(_alignment).substr(0, 1), class_to_str(_class).substr(0, 3), race_to_str(_race));
		break;
	case REVIEW_AND_CONFIRM:
		[[fallthrough]];
	case COMPLETED:
		return fmt::format("{} L {:>2} {}-{} {}{}", name, _abilities.at(CURRENT_LEVEL),
			alignment_to_str(_alignment).substr(0, 1), class_to_str(_class).substr(0, 3), race_to_str(_race), legacy);
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
		_status = CharacterStatus::DEAD;
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

auto Sorcery::Character::get_method() const -> CreateMethod {

	return _method;
}

auto Sorcery::Character::set_method(const CreateMethod value) -> void {

	_method = value;
}

auto Sorcery::Character::get_cur_xp() const -> int {

	using enum Enums::Character::Ability;

	return _abilities.at(CURRENT_XP);
}

auto Sorcery::Character::get_next_xp() const -> int {

	using enum Enums::Character::Ability;

	return _abilities.at(NEXT_LEVEL_XP);
}

auto Sorcery::Character::get_cur_ac() const -> int {

	using enum Enums::Character::Ability;

	return _abilities.at(CURRENT_ARMOUR_CLASS);
}

/*
auto Sorcery::Character::add_starting_equipment(Game *game) -> void {

	using enum Enums::Character::Class;
	using enum Enums::Items::TypeID;

	inventory.clear();

	switch (get_class()) { // NOLINT(clang-diagnostic-switch)
	case FIGHTER:
	case LORD:
	case SAMURAI:
		inventory.add_type((*game->itemstore)[LEATHER_ARMOR], true);
		inventory.add_type((*game->itemstore)[LONG_SWORD], true);
		break;
	case MAGE:
		inventory.add_type((*game->itemstore)[ROBES], true);
		inventory.add_type((*game->itemstore)[DAGGER], true);
		break;
	case PRIEST:
	case BISHOP:
		inventory.add_type((*game->itemstore)[ROBES], true);
		inventory.add_type((*game->itemstore)[STAFF], true);
		break;
	case THIEF:
	case NINJA:
		inventory.add_type((*game->itemstore)[LEATHER_ARMOR], true);
		inventory.add_type((*game->itemstore)[SHORT_SWORD], true);
	default:
		break;
	}
} */

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
