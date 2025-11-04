// Copyright (C) 2025 Dave Moore
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

#include "types/character.hpp"
#include "common/macro.hpp"
#include "core/resources.hpp"
#include "core/system.hpp"

Sorcery::Character::Character() {}

// Standard Constructor
Sorcery::Character::Character(System *system, Resources *resources)
	: _system{system},
	  _resources{resources} {

	set_stage(Enums::Character::Stage::CHOOSE_METHOD);

	_hidden = false;
	set_status(Enums::Character::Status::OK);

	_version = 1;

	_legated = false;

	_location = Enums::Character::Location::TAVERN;

	coordinate = std::nullopt;
	depth = std::nullopt;

	inventory.clear();
}

auto Sorcery::Character::reset(const Enums::Character::Stage stage) -> void {

	set_stage(stage);
}

// Overloaded Operator
auto Sorcery::Character::operator[](const Enums::Character::Ability &key)
	-> int & {

	return _abilities[key];
}

// Utility Functions

auto Sorcery::Character::get_stage() const -> Enums::Character::Stage {

	return _current_stage;
}

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

auto Sorcery::Character::spells() -> std::vector<Spell> & {

	return _spells;
}

auto Sorcery::Character::attributes()
	-> std::map<Enums::Character::Attribute, int> & {

	return _cur_attr;
}

auto Sorcery::Character::priest_cur_sp()
	-> std::map<unsigned int, unsigned int> & {

	return _priest_cur_sp;
}

auto Sorcery::Character::mage_cur_sp()
	-> std::map<unsigned int, unsigned int> & {

	return _mage_cur_sp;
}

auto Sorcery::Character::priest_max_sp()
	-> std::map<unsigned int, unsigned int> & {

	return _priest_max_sp;
}

auto Sorcery::Character::mage_max_sp()
	-> std::map<unsigned int, unsigned int> & {

	return _mage_max_sp;
}

// Reset a character back to a particular state
auto Sorcery::Character::set_stage(const Enums::Character::Stage stage)
	-> void {

	_current_stage = stage;
	switch (stage) {
		using enum Enums::Character::Stage;
	case CHOOSE_METHOD:
		_name.clear();
		_race = Enums::Character::Race::NO_RACE;
		_alignment = Enums::Character::Align::NO_ALIGN;
		_start_attr.clear();
		_cur_attr.clear();
		_max_attr.clear();
		_abilities.clear();

		// Used in the display from this point onwards
		_abilities[Enums::Character::Ability::CURRENT_LEVEL] = 1;
		_class = Enums::Character::Class::NO_CLASS;
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

auto Sorcery::Character::get_name_ref() -> std::string * {

	return &_name;
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

auto Sorcery::Character::set_name(std::string_view value) -> void {

	_name = value;
}

auto Sorcery::Character::get_race() const -> Enums::Character::Race {

	return _race;
}

auto Sorcery::Character::set_race(const Enums::Character::Race &value) -> void {

	_race = value;
}

auto Sorcery::Character::get_level() const -> int {

	return _abilities.at(Enums::Character::Ability::CURRENT_LEVEL);
}

auto Sorcery::Character::set_level(const int &value) -> void {

	_abilities.at(Enums::Character::Ability::CURRENT_LEVEL) = value;
}

auto Sorcery::Character::get_alignment() const -> Enums::Character::Align {

	return _alignment;
}

auto Sorcery::Character::set_alignment(const Enums::Character::Align &value)
	-> void {

	_alignment = value;
}

auto Sorcery::Character::get_num_pos_class() const -> unsigned int {

	return _num_pos_classes;
}

auto Sorcery::Character::get_class() const -> Enums::Character::Class {

	return _class;
}

auto Sorcery::Character::set_class(const Enums::Character::Class &value)
	-> void {

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

auto Sorcery::Character::get_cur_attr(
	const Enums::Character::Attribute attribute) const -> unsigned int {

	return _cur_attr.at(attribute);
}

auto Sorcery::Character::get_start_attr(
	const Enums::Character::Attribute attribute) const -> unsigned int {

	return _start_attr.at(attribute);
}

auto Sorcery::Character::set_cur_attr(
	const Enums::Character::Attribute attribute, const int adjustment) -> void {

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
		using enum Enums::Character::Attribute;
		using enum Enums::Character::Race;
	case HUMAN:
		_start_attr = {{STRENGTH, 8}, {IQ, 5},		{PIETY, 5},
					   {VITALITY, 8}, {AGILITY, 8}, {LUCK, 9}};
		break;
	case ELF:
		_start_attr = {{STRENGTH, 7}, {IQ, 10},		{PIETY, 10},
					   {VITALITY, 6}, {AGILITY, 9}, {LUCK, 6}};
		break;
	case DWARF:
		_start_attr = {{STRENGTH, 10}, {IQ, 7},		 {PIETY, 10},
					   {VITALITY, 10}, {AGILITY, 5}, {LUCK, 6}};
		break;
	case GNOME:
		_start_attr = {{STRENGTH, 7}, {IQ, 7},		 {PIETY, 10},
					   {VITALITY, 8}, {AGILITY, 10}, {LUCK, 7}};
		break;
	case HOBBIT:
		_start_attr = {{STRENGTH, 5}, {IQ, 7},		 {PIETY, 7},
					   {VITALITY, 6}, {AGILITY, 10}, {LUCK, 12}};
		break;
	default:
		break;
	}

	_cur_attr = _start_attr;

	// Formula sourced from http://www.zimlab.com/wizardry/walk/w123calc.htm
	using enum Enums::System::Random;
	_points_left = (*_system->random)[ZERO_TO_3];
	const bool chance_of_more{(*_system->random)[D10] == 1};
	const bool chance_of_more_again{(*_system->random)[D10] == 1};
	_points_left += 7;
	if (_points_left < 20)
		if (chance_of_more)
			_points_left += 10;
	if (_points_left < 20)
		if (chance_of_more_again)
			_points_left += 10;
	_st_points = _points_left;
}

auto Sorcery::Character::get_cur_attr() const
	-> std::map<Enums::Character::Attribute, int> {

	return _cur_attr;
}

auto Sorcery::Character::get_start_attr() const
	-> std::map<Enums::Character::Attribute, int> {

	return _start_attr;
}

auto Sorcery::Character::get_pos_class() const
	-> std::map<Enums::Character::Class, bool> {

	return _pos_classes;
}

// Given a character's current stats and alignment, work out what classes are
// available
auto Sorcery::Character::set_pos_class() -> void {

	_pos_classes.clear();

	// Do the basic classes first (this also sets
	// _num_possible_character_classes); data is from
	// https://strategywiki.org/wiki/Wizardry:_Proving_Grounds_of_the_Mad_Overlord/Trebor%27s_castle#Classes
	using enum Enums::Character::Align;
	using enum Enums::Character::Class;
	using enum Enums::Character::Attribute;
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

	if (_cur_attr[STRENGTH] >= 15 && _cur_attr[IQ] >= 11 &&
		_cur_attr[PIETY] >= 10 && _cur_attr[VITALITY] >= 10 &&
		_cur_attr[AGILITY] >= 10)
		if (_alignment == GOOD || _alignment == NEUTRAL)
			_pos_classes[SAMURAI] = true;
		else
			_pos_classes[SAMURAI] = false;
	else
		_pos_classes[SAMURAI] = false;

	if (_cur_attr[STRENGTH] >= 15 && _cur_attr[IQ] >= 12 &&
		_cur_attr[PIETY] >= 12 && _cur_attr[VITALITY] >= 15 &&
		_cur_attr[AGILITY] >= 14 && _cur_attr[LUCK] >= 15)
		if (_alignment == GOOD)
			_pos_classes[LORD] = true;
		else
			_pos_classes[LORD] = false;
	else
		_pos_classes[LORD] = false;

	// Using looser Wizardry 5 requirements for Ninja (see
	// https://wizardry.fandom.com/wiki/Ninja)
	if (_cur_attr[STRENGTH] >= 15 && _cur_attr[IQ] >= 17 &&
		_cur_attr[PIETY] >= 15 && _cur_attr[VITALITY] >= 16 &&
		_cur_attr[AGILITY] >= 15 && _cur_attr[LUCK] >= 16)
		if (_alignment != GOOD)
			_pos_classes[NINJA] = true;
		else
			_pos_classes[NINJA] = false;
	else
		_pos_classes[NINJA] = false;

	// And workout the number of classes
	_num_pos_classes = std::count_if(_pos_classes.begin(), _pos_classes.end(),
									 [](auto element) {
										 return element.second;
									 });
}

// Enum to String functions
auto Sorcery::Character::alignment_to_str(
	Enums::Character::Align character_alignment) const -> std::string {

	static const std::array<std::string, 4> alignments{
		"", (*_system->strings)["CHARACTER_ALIGNMENT_GOOD"],
		(*_system->strings)["CHARACTER_ALIGNMENT_NEUTRAL"],
		(*_system->strings)["CHARACTER_ALIGNMENT_EVIL"]};

	return alignments[unenum(character_alignment)];
}

auto Sorcery::Character::race_to_str(
	Enums::Character::Race character_race) const -> std::string {

	static const std::array<std::string, 6> races{
		"",
		(*_system->strings)["CHARACTER_RACE_HUMAN"],
		(*_system->strings)["CHARACTER_RACE_ELF"],
		(*_system->strings)["CHARACTER_RACE_DWARF"],
		(*_system->strings)["CHARACTER_RACE_GNOME"],
		(*_system->strings)["CHARACTER_RACE_HOBBIT"]};

	return races[unenum(character_race)];
}

auto Sorcery::Character::class_to_str(
	Enums::Character::Class character_class) const -> std::string {

	static const std::array<std::string, 10> classes{
		"",
		(*_system->strings)["CHARACTER_CLASS_FIGHTER"],
		(*_system->strings)["CHARACTER_CLASS_MAGE"],
		(*_system->strings)["CHARACTER_CLASS_PRIEST"],
		(*_system->strings)["CHARACTER_CLASS_THIEF"],
		(*_system->strings)["CHARACTER_CLASS_BISHOP"],
		(*_system->strings)["CHARACTER_CLASS_SAMURAI"],
		(*_system->strings)["CHARACTER_CLASS_LORD"],
		(*_system->strings)["CHARACTER_CLASS_NINJA"]};

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

auto Sorcery::Character::_legate_start_info() -> void {

	// From here:
	// https://datadrivengamer.blogspot.com/2021/08/the-new-mechanics-of-wizardry-iii.html
	using enum Enums::Character::Ability;
	_abilities[CURRENT_LEVEL] = 1;
	_abilities[CURRENT_XP] = 0;
	_abilities[NEXT_LEVEL_XP] = _get_xp_for_level(_abilities[CURRENT_LEVEL]);
	_abilities[MAX_LEVEL] = _abilities[CURRENT_LEVEL];
	_abilities[NEGATIVE_LEVEL] = 0;
	_abilities[HIT_DICE] = 1;
	if (_abilities[GOLD] > 500)
		_abilities[GOLD] = 500;
	_abilities[AGE] = 20 * 52;
	_abilities[SWIM] = 1;
	_abilities[MARKS] = 0;
	_abilities[DEATHS] = 0;

	// (D7 - 4) is -3 to +3
	using enum Enums::Character::Attribute;
	using enum Enums::System::Random;
	_start_attr[STRENGTH] += ((*_system->random)[D7] - 4);
	_start_attr[IQ] += ((*_system->random)[D7] - 4);
	_start_attr[PIETY] += ((*_system->random)[D7] - 4);
	_start_attr[VITALITY] += ((*_system->random)[D7] - 4);
	_start_attr[AGILITY] += ((*_system->random)[D7] - 4);
	_start_attr[LUCK] += ((*_system->random)[D7] - 4);

	auto mage_sp_total{0};
	for (auto level = 1; level <= 7; level++)
		mage_sp_total += _mage_max_sp[level];
	_start_attr[IQ] += (mage_sp_total / 7);

	auto priest_sp_total{1};
	for (auto level = 1; level <= 7; level++)
		priest_sp_total += _priest_max_sp[level];
	_start_attr[PIETY] += (priest_sp_total / 10);

	switch (_class) { // NOLINT(clang-diagnostic-switch)
		using enum Enums::Character::Class;
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

	set_status(Enums::Character::Status::OK);
	_location = Enums::Character::Location::TAVERN;
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
	_abilities[MAX_LEVEL] = _abilities[CURRENT_LEVEL];
	_abilities[CURRENT_LEVEL] = 1;
	_abilities[CURRENT_XP] = 0;
	_abilities[NEXT_LEVEL_XP] = _get_xp_for_level(_abilities[CURRENT_LEVEL]);

	// https://datadrivengamer.blogspot.com/2019/08/the-not-so-basic-mechanics-of-wizardry.html
	auto age_increment{
		(52 * (3 + (*_system->random)[Enums::System::Random::D3])) + 44};
	_abilities[AGE] += age_increment;

	// Reset attributes to racial minimums
	std::map<Enums::Character::Attribute, int> minimum_attr;
	switch (_race) {
		using enum Enums::Character::Race;
		using enum Enums::Character::Attribute;
	case HUMAN:

		minimum_attr = {{STRENGTH, 8}, {IQ, 5},		 {PIETY, 5},
						{VITALITY, 8}, {AGILITY, 8}, {LUCK, 9}};
		break;
	case ELF:
		minimum_attr = {{STRENGTH, 7}, {IQ, 10},	 {PIETY, 10},
						{VITALITY, 6}, {AGILITY, 9}, {LUCK, 6}};
		break;
	case DWARF:
		minimum_attr = {{STRENGTH, 10}, {IQ, 7},	  {PIETY, 10},
						{VITALITY, 10}, {AGILITY, 5}, {LUCK, 6}};
		break;
	case GNOME:
		minimum_attr = {{STRENGTH, 7}, {IQ, 7},		  {PIETY, 10},
						{VITALITY, 8}, {AGILITY, 10}, {LUCK, 7}};
		break;
	case HOBBIT:
		minimum_attr = {{STRENGTH, 5}, {IQ, 7},		  {PIETY, 7},
						{VITALITY, 6}, {AGILITY, 10}, {LUCK, 12}};
		break;
	default:
		break;
	}

	_cur_attr = minimum_attr;
}

// Legate
auto Sorcery::Character::legate(const Enums::Character::Align &value) -> void {

	if (_alignment != value)
		_alignment = value;

	_legate_start_info();
	_generate_secondary_abil(true, false, true);
	_set_start_spells();
	_set_starting_sp();
	inventory.clear();
	set_status(Enums::Character::Status::OK);
	_legated = true;

	set_location(Enums::Character::Location::TAVERN);
}

// Change Class
auto Sorcery::Character::change_class(const Enums::Character::Class &value)
	-> void {

	if (_class != value) {
		_class = value;

		_regenerate_start_info();
		_generate_secondary_abil(false, true, false);
		_reset_start_spells(); // TODO: check this, not sure this works properly
		_reset_starting_sp();
		inventory.unequip_all();
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
	using enum Enums::Character::Ability;
	using enum Enums::System::Random;
	_abilities[MAX_LEVEL] = 1;
	_abilities[NEGATIVE_LEVEL] = 0;
	_abilities[HIT_DICE] = 1;
	_abilities[GOLD] = (*_system->random)[ZERO_TO_99] + 90;
	_abilities[AGE] = (18 * 52) + (*_system->random)[ZERO_TO_299];
	_abilities[SWIM] = 1;
	_abilities[MARKS] = 0;
	_abilities[DEATHS] = 0;

	_start_attr = _cur_attr;
	_max_attr = _cur_attr;

	_abilities[CURRENT_XP] = 0;
	_abilities[NEXT_LEVEL_XP] = _get_xp_for_level(_abilities[CURRENT_LEVEL]);
}

// Given the characters current level, work out all the secondary
// abilities/stats etc
auto Sorcery::Character::_generate_secondary_abil(bool initial,
												  bool change_class,
												  bool legate) -> void {

	// Formulae used are from here
	// http://www.zimlab.com/wizardry/walk/w123calc.htm and also from
	// https://mirrors.apple2.org.za/ftp.apple.asimov.net/images/games/rpg/wizardry/wizardry_I/Wizardry_i_SourceCode.zip
	using enum Enums::Character::Ability;
	const auto current_level{_abilities[CURRENT_LEVEL]};

	// Bonus Melee to Hit per Attack (num)
	using enum Enums::Character::Attribute;
	if (_cur_attr[STRENGTH] > 15)
		_abilities[ATTACK_MODIFIER] = _cur_attr[STRENGTH] - 15;
	else if (_cur_attr[STRENGTH] < 6)
		_abilities[ATTACK_MODIFIER] = _cur_attr[STRENGTH] - 6;
	else
		_abilities[ATTACK_MODIFIER] = 0;

	// Bonus Melee to Hit per Attack (num)
	using enum Enums::Character::Class;
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
	_abilities[UNARMED_DAMAGE] = _class == NINJA ? 8 + _abilities[BONUS_DAMAGE]
												 : 4 + _abilities[BONUS_DAMAGE];

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
	_abilities[IDENTIFY_ITEMS] =
		_class == BISHOP ? 10 + (5 * current_level) : 0;
	if (_abilities[IDENTIFY_ITEMS] > 100)
		_abilities[IDENTIFY_ITEMS] = 100;

	// Chance of getting cursed when Identifying Items (%)
	_abilities[IDENTIFY_CURSE] =
		_class == BISHOP ? 35 - (5 * current_level) : 0;
	if (_abilities[IDENTIFY_CURSE] < 0)
		_abilities[IDENTIFY_CURSE] = 0;

	// Chance of identifying unknown Foes per round (%)
	_abilities[IDENTIFY_FOES] =
		current_level + _cur_attr[IQ] + _cur_attr[PIETY];
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
	case PRIEST:
		_abilities[BASE_DISPELL] = 50 + (5 * current_level);
		break;
	case BISHOP:
		_abilities[BASE_DISPELL] =
			current_level >= 4 ? 50 + (5 * current_level) - 20 : 0;
		break;
	case LORD:
		_abilities[BASE_DISPELL] =
			current_level >= 9 ? 50 + (5 * current_level) - 40 : 0;
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

		// Base Hit Points (num) - note initially all non-legated characters get
		// 8 HP as per the PSX versions
		if (initial) {
			const auto bonus{_abilities[BONUS_HIT_POINTS]};
			using enum Enums::System::Random;
			switch (auto chance{(*_system->random)[D100]};
					_class) { // NOLINT(clang-diagnostic-switch)
			case FIGHTER:
			case LORD:
				_abilities[MAX_HP] =
					chance <= 50 ? 10 + bonus : 9 * (10 + bonus) / 10;
				break;
			case PRIEST:
				_abilities[MAX_HP] =
					chance <= 50 ? 8 + bonus : 8 * (10 + bonus) / 10;
				break;
			case THIEF:
			case BISHOP:
			case NINJA:
				_abilities[MAX_HP] =
					chance <= 50 ? 6 + bonus : 6 * (10 + bonus) / 10;
				break;
			case MAGE:
				_abilities[MAX_HP] =
					chance <= 50 ? 4 + bonus : 4 * (10 + bonus) / 10;
				break;
			case SAMURAI:
				_abilities[MAX_HP] =
					chance <= 50 ? 16 + bonus : 16 * (10 + bonus) / 10;
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

	// Chance of resurrecting by a DI or KADORTO spell cast by another Character
	// (%)
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
	_abilities[BASE_ARMOUR_CLASS] =
		_class == NINJA ? 10 - (current_level / 3) - 2 : 10;

	// Don't use this!
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
	_abilities[BASE_DISARM_TRAP] = (_class == NINJA) || (_class == THIEF)
									   ? ((current_level + 50) / 69.0) * 100
									   : 0;

	// Chance to activate a Trap if identify fails (%)
	_abilities[ACTIVATE_TRAP] = (_class == NINJA) || (_class == THIEF)
									? 100 - ((_cur_attr[AGILITY] / 20.0) * 100)
									: 100;

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
	const auto brb{_abilities[BASE_RESIST_BONUS]};
	switch (_class) { // NOLINT(clang-diagnostic-switch)
	case SAMURAI:
		_abilities[RESISTANCE_VS_POISON_PARALYSIS] = brb + 2;
		_abilities[RESISTANCE_VS_CRITICAL_HIT] = brb + 2;
		_abilities[RESISTANCE_VS_STONING] = 0;
		_abilities[RESISTANCE_VS_BREATH_ATTACKS] = 0;
		_abilities[RESISTANCE_VS_POISON_GAS_TRAP] = 0;
		_abilities[RESISTANCE_VS_MAGE_PRIEST_TRAP] = brb + 2;
		_abilities[RESISTANCE_VS_SILENCE] = brb + 2;
		break;
	case FIGHTER:
		_abilities[RESISTANCE_VS_POISON_PARALYSIS] = brb + 3;
		_abilities[RESISTANCE_VS_CRITICAL_HIT] = brb + 3;
		_abilities[RESISTANCE_VS_STONING] = 0;
		_abilities[RESISTANCE_VS_BREATH_ATTACKS] = 0;
		_abilities[RESISTANCE_VS_POISON_GAS_TRAP] = 0;
		_abilities[RESISTANCE_VS_MAGE_PRIEST_TRAP] = 0;
		_abilities[RESISTANCE_VS_SILENCE] = 0;
		break;
	case PRIEST:
		_abilities[RESISTANCE_VS_POISON_PARALYSIS] = 0;
		_abilities[RESISTANCE_VS_CRITICAL_HIT] = 0;
		_abilities[RESISTANCE_VS_STONING] = brb + 3;
		_abilities[RESISTANCE_VS_BREATH_ATTACKS] = 0;
		_abilities[RESISTANCE_VS_POISON_GAS_TRAP] = 0;
		_abilities[RESISTANCE_VS_MAGE_PRIEST_TRAP] = 0;
		_abilities[RESISTANCE_VS_SILENCE] = 0;
		break;
	case LORD:
		_abilities[RESISTANCE_VS_POISON_PARALYSIS] = brb + 2;
		_abilities[RESISTANCE_VS_CRITICAL_HIT] = brb + 2;
		_abilities[RESISTANCE_VS_STONING] = brb + 2;
		_abilities[RESISTANCE_VS_BREATH_ATTACKS] = 0;
		_abilities[RESISTANCE_VS_POISON_GAS_TRAP] = 0;
		_abilities[RESISTANCE_VS_MAGE_PRIEST_TRAP] = 0;
		_abilities[RESISTANCE_VS_SILENCE] = 0;
		break;
	case BISHOP:
		_abilities[RESISTANCE_VS_POISON_PARALYSIS] = 0;
		_abilities[RESISTANCE_VS_CRITICAL_HIT] = 0;
		_abilities[RESISTANCE_VS_STONING] = brb + 2;
		_abilities[RESISTANCE_VS_BREATH_ATTACKS] = brb + 2;
		_abilities[RESISTANCE_VS_POISON_GAS_TRAP] = 0;
		_abilities[RESISTANCE_VS_MAGE_PRIEST_TRAP] = brb + 2;
		_abilities[RESISTANCE_VS_SILENCE] = brb + 2;
		break;
	case THIEF:
		_abilities[RESISTANCE_VS_POISON_PARALYSIS] = 0;
		_abilities[RESISTANCE_VS_CRITICAL_HIT] = 0;
		_abilities[RESISTANCE_VS_STONING] = 0;
		_abilities[RESISTANCE_VS_BREATH_ATTACKS] = 0;
		_abilities[RESISTANCE_VS_POISON_GAS_TRAP] = brb + 3;
		_abilities[RESISTANCE_VS_MAGE_PRIEST_TRAP] = 0;
		_abilities[RESISTANCE_VS_SILENCE] = 0;
		break;
	case NINJA:
		_abilities[RESISTANCE_VS_POISON_PARALYSIS] = brb + 3;
		_abilities[RESISTANCE_VS_CRITICAL_HIT] = brb + 3;
		_abilities[RESISTANCE_VS_STONING] = brb + 2;
		_abilities[RESISTANCE_VS_BREATH_ATTACKS] = brb + 4;
		_abilities[RESISTANCE_VS_POISON_GAS_TRAP] = brb + 3;
		_abilities[RESISTANCE_VS_MAGE_PRIEST_TRAP] = brb + 2;
		_abilities[RESISTANCE_VS_SILENCE] = brb + 2;
		break;
	case MAGE:
		_abilities[RESISTANCE_VS_POISON_PARALYSIS] = 0;
		_abilities[RESISTANCE_VS_CRITICAL_HIT] = 0;
		_abilities[RESISTANCE_VS_STONING] = 0;
		_abilities[RESISTANCE_VS_BREATH_ATTACKS] = 0;
		_abilities[RESISTANCE_VS_POISON_GAS_TRAP] = 0;
		_abilities[RESISTANCE_VS_MAGE_PRIEST_TRAP] = brb + 3;
		_abilities[RESISTANCE_VS_SILENCE] = brb + 3;
		break;
	default:
		break;
	}

	using enum Enums::Character::Race;
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
		auto priest_known{static_cast<unsigned int>(
			std::count_if(_spells.begin(), _spells.end(), [=](auto spell) {
				return (spell.type == Enums::Magic::SpellType::DIVINE) &&
					   (spell.level == spell_level) && (spell.known);
			}))};

		if (_priest_max_sp[spell_level] < priest_known) {
			_priest_max_sp[spell_level] = priest_known;
			_priest_cur_sp[spell_level] = _priest_max_sp[spell_level];
		}

		// Handle Mage Spells
		auto mage_known{static_cast<unsigned int>(
			std::count_if(_spells.begin(), _spells.end(), [=](auto spell) {
				return (spell.type == Enums::Magic::SpellType::ARCANE) &&
					   (spell.level == spell_level) && (spell.known);
			}))};
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

	// In the original code this is handled in
	// "SETSPELS"/"SPLPERLV"/"NWMAGE"/"NWPRIEST"
	switch (_class) { // NOLINT(clang-diagnostic-switch)
		using enum Enums::Character::Ability;
		using enum Enums::Character::Class;
		using enum Enums::Config::Options;
	case PRIEST:
		_priest_max_sp[1] = (*_system->config)[STRICT_MODE]
								? 2
								: 2 + _abilities[BONUS_PRIEST_SPELLS];
		break;
	case BISHOP:
		_mage_max_sp[1] = 2;
		break;
	case MAGE:
		_mage_max_sp[1] = (*_system->config)[STRICT_MODE]
							  ? 2
							  : 2 + _abilities[BONUS_MAGE_SPELLS];
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

	// All known spells are kept, and new ones added as per the starting class
	// guildlines
	_set_start_spells();
}

auto Sorcery::Character::_learn_spell(Enums::Magic::SpellID spell_id) -> void {

	std::vector<Spell>::iterator it;
	it = std::find_if(_spells.begin(), _spells.end(), [&](auto item) {
		return item.id == spell_id;
	});
	if (it != _spells.end()) {
		(*it).known = true;
		_spells_known[(*it).id] = true;
	} // NOLINT(clang-di
}

// Set starting spells
auto Sorcery::Character::_set_start_spells() -> void {

	// This is taken from "KEEPCHYN" which hard codes the spells known to
	// beginning characters!
	switch (_class) { // NOLINT(clang-diagnostic-switch)
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
auto Sorcery::Character::_get_hp_per_level() -> int {

	// In the original code ("MOREHP"), Samurai get 2d8
	auto extra_hp{0};
	switch (_class) { // NOLINT(clang-diagnostic-switch)
		using enum Enums::Character::Class;
		using enum Enums::System::Random;
	case FIGHTER:
	case LORD:
		extra_hp += (*_system->random)[D10];
		break;
	case PRIEST:
		extra_hp += (*_system->random)[D8];
		break;
	case THIEF:
	case BISHOP:
	case NINJA:
		extra_hp += (*_system->random)[D6];
		break;
	case MAGE:
		extra_hp += (*_system->random)[D4];
		break;
	case SAMURAI:
		extra_hp += (*_system->random)[D8];
		extra_hp += (*_system->random)[D8];
		break;
	default:
		break;
	}

	extra_hp += _abilities[Enums::Character::Ability::BONUS_HIT_POINTS];
	if (extra_hp < 0)
		extra_hp = 1;

	// Though this could be an unsigned int as it will always be greater than 0,
	// just return int as everything else is
	return extra_hp;
}

auto Sorcery::Character::post_construct(System *system, Resources *resources)
	-> void {
	_system = system;
	_resources = resources;
}

// Add hit points on level gain (but note the strict mode limitation mentioned
// below)
auto Sorcery::Character::_update_hp_for_level() -> int {

	// Note the rerolling of all HP ("MADELEV") when levelling - and using
	// MaxLevel achieved when in strict mode
	using enum Enums::Character::Ability;
	auto hp_gained{0};
	if ((*_system->config)[Enums::Config::LEVEL_REROLL_HP]) {
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

	return _abilities.at(Enums::Character::Ability::CURRENT_HP);
}

auto Sorcery::Character::get_max_hp() const -> int {

	return _abilities.at(Enums::Character::Ability::MAX_HP);
}

auto Sorcery::Character::_update_stat_for_level(
	Enums::Character::Attribute attribute, std::string stat) -> std::string {

	auto message{""s};
	using enum Enums::System::Random;
	if ((*_system->random)[D100] < 75) {
		const auto chance{_abilities.at(Enums::Character::Ability::AGE) /
						  130.f};
		if ((*_system->random)[D100] < chance) {

			// Decrease
			bool proceed{true};
			if (_cur_attr.at(attribute) == 18 && (*_system->random)[D6] > 1)
				proceed = false;

			if (proceed) {
				_cur_attr.at(attribute) = _cur_attr.at(attribute) - 1;
				message = std::format("{} {}",
									  (*_system->strings)["LEVEL_LOSS"], stat);
				if (_cur_attr.at(attribute) < 1)
					_cur_attr.at(attribute) = 1;
			}
		} else {
			if (_cur_attr.at(attribute) < 18) {
				_cur_attr.at(attribute) = _cur_attr.at(attribute) + 1;
				if (_cur_attr.at(attribute) > _max_attr.at(attribute))
					_max_attr.at(attribute) = _cur_attr.at(attribute);
				message = std::format("{} {}",
									  (*_system->strings)["LEVEL_GAIN"], stat);
			}
		}
	}

	if (_cur_attr.at(attribute) > _max_attr.at(attribute))
		_max_attr.at(attribute) = _cur_attr.at(attribute);

	return message;
}

// Level a character up
auto Sorcery::Character::level_up() -> void {

	level_up_results.clear();
	level_up_results.emplace_back((*_system->strings)["LEVEL_DING"]);

	// Increase level
	using enum Enums::Character::Ability;
	_abilities.at(CURRENT_LEVEL) = _abilities.at(CURRENT_LEVEL) + 1;
	_abilities.at(HIT_DICE) = _abilities.at(HIT_DICE) + 1;
	if (_abilities.at(CURRENT_LEVEL) > _abilities.at(MAX_LEVEL))
		_abilities.at(MAX_LEVEL) = _abilities.at(CURRENT_LEVEL);

	// Handle learning spells
	if (_set_sp())
		level_up_results.emplace_back((*_system->strings)["LEVEL_SPELLS"]);

	// Work out new xp needed
	_abilities[NEXT_LEVEL_XP] = _get_xp_for_level(_abilities[CURRENT_LEVEL]);

	// Handle stat changing
	auto stat_message{""s};
	using enum Enums::Character::Attribute;
	stat_message = _update_stat_for_level(
		STRENGTH, (*_system->strings)["CHARACTER_STAT_STRENGTH"]);
	if (!stat_message.empty())
		level_up_results.emplace_back(stat_message);
	stat_message = _update_stat_for_level(
		IQ, (*_system->strings)["CHARACTER_STAT_INTELLIGENCE"]);
	if (!stat_message.empty())
		level_up_results.emplace_back(stat_message);
	stat_message = _update_stat_for_level(
		PIETY, (*_system->strings)["CHARACTER_STAT_PIETY"]);
	if (!stat_message.empty())
		level_up_results.emplace_back(stat_message);
	stat_message = _update_stat_for_level(
		VITALITY, (*_system->strings)["CHARACTER_STAT_VITALITY"]);
	if (!stat_message.empty())
		level_up_results.emplace_back(stat_message);
	stat_message = _update_stat_for_level(
		AGILITY, (*_system->strings)["CHARACTER_STAT_AGILITY"]);
	if (!stat_message.empty())
		level_up_results.emplace_back(stat_message);
	stat_message = _update_stat_for_level(
		LUCK, (*_system->strings)["CHARACTER_STAT_LUCK"]);
	if (!stat_message.empty())
		level_up_results.emplace_back(stat_message);

	// Level everything else up
	_generate_secondary_abil(false, false, false);

	// handle hp
	const auto hp_gained{_update_hp_for_level()};
	const auto hp_message{
		std::format("{} {} {}", (*_system->strings)["LEVEL_HP_PREFIX"],
					hp_gained, (*_system->strings)["LEVEL_HP_SUFFIX"])};
	level_up_results.emplace_back(hp_message);

	if (_cur_attr.at(VITALITY) < 3) {
		level_up_results.emplace_back((*_system->strings)["LEVEL_DIE"]);
		_status = Enums::Character::Status::LOST;
		_location = Enums::Character::Location::TRAINING;
	}
}

// Level a character down (e.g. drain levels or give/increase negative levels_
auto Sorcery::Character::level_down() -> void {

	using enum Enums::Character::Ability;
	if (_abilities.at(CURRENT_LEVEL) == 1) {
		_status = Enums::Character::Status::LOST;
		_location = Enums::Character::Location::TRAINING;
		return;
	}

	// TODO: What to do about negative level ability?
	const auto old_level{_abilities.at(CURRENT_LEVEL)};
	const auto diff_hp{_abilities.at(MAX_HP) - _abilities.at(CURRENT_HP)};

	_abilities.at(CURRENT_LEVEL) = _abilities.at(CURRENT_LEVEL) - 1;
	_abilities.at(HIT_DICE) = _abilities.at(HIT_DICE) - 1;
	if (_abilities.at(MAX_LEVEL) > _abilities.at(CURRENT_LEVEL))
		_abilities.at(MAX_LEVEL) = _abilities.at(CURRENT_LEVEL);

	_set_sp();

	// When drained XP is set to beginning of current level (for example,
	// draining to level 9 means that youre xp is set to enough for level 9 plus
	// 1 - which is vety harsh)
	_abilities[NEXT_LEVEL_XP] = _get_xp_for_level(_abilities.at(CURRENT_LEVEL));
	_abilities[CURRENT_XP] =
		_get_xp_for_level(_abilities.at(CURRENT_LEVEL) - 1) + 1;

	_generate_secondary_abil(false, false, false);
	_abilities[MAX_HP] = _abilities.at(MAX_HP) *
						 (_abilities.at(CURRENT_LEVEL) / (old_level * 1.f));
	_abilities[CURRENT_HP] = _abilities[MAX_HP] - diff_hp;
	if (_abilities[CURRENT_HP] < 0)
		_abilities[CURRENT_HP] = 0;
}

// For each spell level, try to learn spells - called before set_spellpoints
auto Sorcery::Character::_try_learn_spell(Enums::Magic::SpellType spell_type,
										  unsigned int spell_level) -> bool {

	bool new_spell_learnt{false};

	// Only do spells if a character has available spell points in this spell
	// level
	using enum Enums::Magic::SpellType;
	if (spell_type == DIVINE)
		if (_priest_max_sp[spell_level] == 0)
			return false;
	if (spell_type == ARCANE)
		if (_mage_max_sp[spell_level] == 0)
			return false;

	// Get all unknown spells of this spell level and type
	for (auto spells{_spells | std::views::filter([&](Spell spell) {
						 return (spell.type == spell_type) &&
								(spell.level == spell_level) &&
								(spell.known == false);
					 })};
		 auto &spell : spells) {

		const auto dice{(*_system->random)[Enums::System::Random::ZERO_TO_29]};

		// Check the Spell Type against the relevant stat (see
		// SPLPERLV//TRYLEARN)
		using enum Enums::Character::Attribute;
		if (spell_type == DIVINE) {
			if (dice <= static_cast<unsigned int>(_cur_attr[PIETY])) {
				spell.known = true;
				_spells_known[spell.id] = true;
				new_spell_learnt = true;
			}
		}
		if (spell_type == ARCANE) {
			if (dice <= static_cast<unsigned int>(_cur_attr[IQ])) {
				spell.known = true;
				_spells_known[spell.id] = true;
				new_spell_learnt = true;
			}
		}
	}

	return new_spell_learnt;
}

// Reimplementation of SPLPERLV - note this will reset spell points!
auto Sorcery::Character::_calculate_sp(Enums::Magic::SpellType spell_type,
									   unsigned int level_mod,
									   unsigned int level_offset) -> void {

	std::map<unsigned int, unsigned int> *spells{
		spell_type == Enums::Magic::SpellType::DIVINE ? &_priest_max_sp
													  : &_mage_max_sp};
	for (auto spell_level = 1; spell_level <= 7; spell_level++)
		(*spells)[spell_level] = 0;

	// https://datadrivengamer.blogspot.com/2019/08/the-not-so-basic-mechanics-of-wizardry.html
	for (auto spell_level = 1; spell_level <= 7; spell_level++) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnarrowing"
		int spell_count{_abilities[Enums::Character::Ability::CURRENT_LEVEL] -
						level_mod + level_offset -
						(level_offset * spell_level)};
#pragma GCC diagnostic pop
		spell_count = std::max(0, std::min(spell_count, 9));
		(*spells)[spell_level] = spell_count;
	}
}

// Copied and rewritten from the original code from
// MINMAG/MINPRI/NWPRIEST/NWMAGE
auto Sorcery::Character::_set_sp() -> bool {

	bool new_spells_learnt{false};

	for (auto spell_level = 1; spell_level <= 7; spell_level++) {
		_priest_max_sp[spell_level] = 0;
		_mage_max_sp[spell_level] = 0;
	}

	// Generate spell points according to current level and class (this does not
	// change any spells known but will reset spell points)
	switch (_class) { // NOLINT(clang-diagnostic-switch)
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
		if (_priest_max_sp[spell_level] > 0) {
			if (_try_learn_spell(DIVINE, spell_level))
				new_spells_learnt = true;
		}
		if (_mage_max_sp[spell_level] > 0) {
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
		if (_priest_max_sp[spell_level] > 0 &&
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
				if ((*_system->random)[D100] > 33)
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
				if ((*_system->random)[D100] > 33)
					_learn_spell(BADIALMA);
				else
					_learn_spell(BADI);
				new_spells_learnt = true;
				break;
			case 6:
				if ((*_system->random)[D100] > 33)
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
		if (_mage_max_sp[spell_level] > 0 &&
			_get_spells_known(ARCANE, spell_level) == 0) {
			switch (spell_level) {
			case 1:
				if ((*_system->random)[D100] > 33)
					_learn_spell(KATINO);
				else
					_learn_spell(HALITO);
				new_spells_learnt = true;
				break;
			case 2:
				if ((*_system->random)[D100] > 33)
					_learn_spell(DILTO);
				else
					_learn_spell(SOPIC);
				new_spells_learnt = true;
				break;
			case 3:
				if ((*_system->random)[D100] > 33)
					_learn_spell(MOLITO);
				else
					_learn_spell(MAHALITO);
				new_spells_learnt = true;
				break;
			case 4:
				if ((*_system->random)[D100] > 33)
					_learn_spell(DALTO);
				else
					_learn_spell(LAHALITO);
				new_spells_learnt = true;
				break;
			case 5:
				if ((*_system->random)[D100] > 33)
					_learn_spell(MAMORLIS);
				else
					_learn_spell(MADALTO);
				new_spells_learnt = true;
				break;
			case 6:
				if ((*_system->random)[D100] > 33)
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
		if ((spell_level * 2) - 1 > _abilities.at(MAX_LEVEL))
			continue;

		using enum Enums::Magic::SpellType;
		const auto priest_known{_get_spells_known(DIVINE, spell_level)};
		const auto mage_known{_get_spells_known(ARCANE, spell_level)};

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
auto Sorcery::Character::_get_spells_known(Enums::Magic::SpellType spell_type,
										   unsigned int spell_level)
	-> unsigned int {

	return std::ranges::count_if(
		_spells.begin(), _spells.end(), [&](auto spell) {
			return spell.type == spell_type && spell.level == spell_level &&
				   spell.known;
		});
}

// Given a level, get the XP needed for it
auto Sorcery::Character::_get_xp_for_level(unsigned int level) const -> int {

	// XP values are obtained directly from original Apple2 Scenario Disc (look
	// for E8 03 00 00 00 00 near &00020106 though due to what I think is the
	// way USCD pascal stores large numbers, they are stored in 16 bit LSB
	// "chunks", for example, 134586 is stored at &0002013C as EA 11 0D, or 4586
	// - to get the actual value for the level we add this to 0D in decimal (13)
	// times 10000, to get 134586.
	static const std::array<std::array<int, 14>, 8> levels{
		{{0, 1000, 1724, 2972, 5124, 8834, 15231, 26260, 45275, 78060, 134586,
		  232044, 400075, 289709}, // FIGHTER
		 {0, 1100, 1896, 3268, 5124, 9713, 16746, 28872, 49779, 85825, 147974,
		  255127, 439874, 318529}, // MAGE
		 {0, 1050, 1810, 3120, 5379, 9274, 15989, 27567, 47529, 81946, 141286,
		  243596, 419993, 304132}, // PRIEST
		 {0, 900, 1551, 2574, 4610, 7948, 13703, 23625, 40732, 70187, 121081,
		  208750, 359931, 260639}, // THIEF
		 {0, 1000, 2105, 3692, 6477, 11363, 19935, 34973, 61136, 107642, 188845,
		  331370, 481240, 438479}, // BISHOP
		 {0, 1250, 2192, 3845, 6745, 11833, 20759, 36419, 63892, 112091, 196650,
		  345000, 605263, 456601}, // SAMURAI
		 {0, 1300, 2280, 4000, 7017, 12310, 21596, 37887, 66468, 116610, 204578,
		  358908, 629663, 475008}, // LORD
		 {0, 1450, 2543, 4461, 7826, 13729, 24085, 42254, 74129, 130050, 228157,
		  400275, 702236, 529756}}}; // NINJA

	// Also found here:
	// http://www.the-spoiler.com/RPG/Sir-Tech/wizardry.1.2.html

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
		it = std::find_if(_spells.begin(), _spells.end(), [&](auto item) {
			return item.id == spell_known.first;
		});
		if (it != _spells.end())
			(*it).known = spell_known.second;
	}
}

// Wizardry 1 - 3 Spells
auto Sorcery::Character::create_spells() -> void {

	_spells.clear();
	_spells = _resources->spells->get_all();
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
	const Enums::Character::Class cclass,
	const Enums::Character::Align alignment) -> void {

	_class = cclass;
	_race = static_cast<Enums::Character::Race>(
		(*_system->random)[Enums::System::Random::D5]);
	_alignment = alignment;

	switch (_race) { // NOLINT(clang-diagnostic-switch)
		using enum Enums::Character::Attribute;
		using enum Enums::Character::Race;
	case HUMAN:
		_start_attr = {{STRENGTH, 8}, {IQ, 5},		{PIETY, 5},
					   {VITALITY, 8}, {AGILITY, 8}, {LUCK, 9}};
		break;
	case ELF:
		_start_attr = {{STRENGTH, 7}, {IQ, 10},		{PIETY, 10},
					   {VITALITY, 6}, {AGILITY, 9}, {LUCK, 6}};
		break;
	case DWARF:
		_start_attr = {{STRENGTH, 10}, {IQ, 7},		 {PIETY, 10},
					   {VITALITY, 10}, {AGILITY, 5}, {LUCK, 6}};
		break;
	case GNOME:
		_start_attr = {{STRENGTH, 7}, {IQ, 7},		 {PIETY, 10},
					   {VITALITY, 8}, {AGILITY, 10}, {LUCK, 7}};
		break;
	case HOBBIT:
		_start_attr = {{STRENGTH, 5}, {IQ, 7},		 {PIETY, 7},
					   {VITALITY, 6}, {AGILITY, 10}, {LUCK, 12}};
		break;
	default:
		break;
	}

	// Put most of the points into the main attribute (note that 10 points means
	// a Human Priest and Dwarf Thief have allocated all points to their main
	// attribute with no points left over)
	_points_left = 10;
	_st_points = _points_left;

	switch (_class) { // NOLINT(clang-diagnostic-switch)
		using enum Enums::Character::Attribute;
		using enum Enums::Character::Class;
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
		_start_attr[Enums::Character::Attribute::VITALITY] += _points_left;

	_cur_attr = _start_attr;

	_name = _system->random->get_random_name();
	_portrait_index = (*_system->random)[Enums::System::Random::ZERO_TO_29];
}

// Enter Name and Portrait, rest is random
auto Sorcery::Character::create_quick() -> void {

	// Exclude Samurai/Lord/Ninja/Bishop from this method of character creation
	using enum Enums::System::Random;
	_class = static_cast<Enums::Character::Class>((*_system->random)[D4]);
	_race = static_cast<Enums::Character::Race>((*_system->random)[D5]);
	switch (_class) { // NOLINT(clang-diagnostic-switch)#
		using enum Enums::Character::Align;
		using enum Enums::Character::Class;
	case FIGHTER:
	case MAGE:
		_alignment =
			static_cast<Enums::Character::Align>((*_system->random)[D3]);
		break;
	case PRIEST:
		_alignment = (*_system->random)[D2] == 1 ? GOOD : EVIL;
		break;
	case THIEF:
		_alignment = (*_system->random)[D2] == 1 ? NEUTRAL : EVIL;
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
	switch (_race) { // NOLINT(clang-diagnostic-switch)
		using enum Enums::Character::Attribute;
		using enum Enums::Character::Race;
	case HUMAN:
		_start_attr = {{STRENGTH, 8}, {IQ, 5},		{PIETY, 5},
					   {VITALITY, 8}, {AGILITY, 8}, {LUCK, 9}};
		break;
	case ELF:
		_start_attr = {{STRENGTH, 7}, {IQ, 10},		{PIETY, 10},
					   {VITALITY, 6}, {AGILITY, 9}, {LUCK, 6}};
		break;
	case DWARF:
		_start_attr = {{STRENGTH, 10}, {IQ, 7},		 {PIETY, 10},
					   {VITALITY, 10}, {AGILITY, 5}, {LUCK, 6}};
		break;
	case GNOME:
		_start_attr = {{STRENGTH, 7}, {IQ, 7},		 {PIETY, 10},
					   {VITALITY, 8}, {AGILITY, 10}, {LUCK, 7}};
		break;
	case HOBBIT:
		_start_attr = {{STRENGTH, 5}, {IQ, 7},		 {PIETY, 7},
					   {VITALITY, 6}, {AGILITY, 10}, {LUCK, 12}};
		break;
	default:
		break;
	}

	// Put most of the points into the main attribute (note that 10 points means
	// a Human Priest and Dwarf Thief have allocated all points to their main
	// attribute with no points left over)
	_points_left = 10;
	_st_points = _points_left;
	switch (_class) { // NOLINT(clang-diagnostic-switch)
		using enum Enums::Character::Attribute;
		using enum Enums::Character::Class;
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
		_start_attr[Enums::Character::Attribute::VITALITY] += _points_left;

	_cur_attr = _start_attr;
}

// Create a (semi) random character
auto Sorcery::Character::create_random() -> void {

	// Random Name and Portrait
	create_quick();
	_name = _system->random->get_random_name();
	_portrait_index = (*_system->random)[Enums::System::Random::ZERO_TO_29];
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
		return (*_system->strings)["STATUS_POISONED"];
	} else if (_status == OK)
		return (*_system->strings)["STATUS_OK"];
	else {
		switch (_status) {
		case AFRAID:
			return (*_system->strings)["STATUS_AFRAID"];
			break;
		case ASHES:
			return (*_system->strings)["STATUS_ASHES"];
			break;
		case ASLEEP:
			return (*_system->strings)["STATUS_ASLEEP"];
			break;
		case DEAD:
			return (*_system->strings)["STATUS_DEAD"];
			break;
		case LOST:
			return (*_system->strings)["STATUS_LOST"];
			break;
		case HELD:
			return (*_system->strings)["STATUS_PARALYSED"];
			break;
		case SILENCED:
			return (*_system->strings)["STATUS_SILENCED"];
			break;
		case STONED:
			return (*_system->strings)["STATUS_STONED"];
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
		return (*_system->strings)["LOCATION_PARTY"];
		break;
	case TAVERN:
		return (*_system->strings)["LOCATION_TAVERN"];
		break;
	case TRAINING:
		return (*_system->strings)["LOCATION_TRAINING"];
		break;
	case TEMPLE:
		return (*_system->strings)["LOCATION_TEMPLE"];
		break;
	case MAZE:
		return (*_system->strings)["LOCATION_MAZE"];
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
		return (*_system->strings)["STATUS_HIDDEN"];
}

auto Sorcery::Character::set_status(Enums::Character::Status value) -> void {

	if (value == Enums::Character::Status::OK)
		_status = value;
	else {
		auto candidate{unenum(value)};
		auto current{unenum(_status)};
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

auto Sorcery::Character::get_spell_points(
	const Enums::Magic::SpellType type,
	const Enums::Magic::SpellPointType status) const
	-> std::optional<std::map<unsigned int, unsigned int>> {

	using enum Enums::Magic::SpellPointType;
	using enum Enums::Magic::SpellType;
	if (type == ARCANE && status == CURRENT)
		return _mage_cur_sp;
	else if (type == ARCANE && status == MAXIMUM)
		return _mage_max_sp;
	else if (type == DIVINE && status == CURRENT)
		return _priest_cur_sp;
	else if (type == DIVINE && status == MAXIMUM)
		return _priest_max_sp;
	else
		return std::nullopt;
}

auto Sorcery::Character::_get_sp_per_level(const Enums::Magic::SpellType type,
										   int level) -> std::string {

	if (type == Enums::Magic::SpellType::ARCANE)
		return std::format("{}/{}", std::to_string(_mage_cur_sp[level]),
						   std::to_string(_mage_max_sp[level]));
	else
		return std::format("{}/{}", std::to_string(_priest_cur_sp[level]),
						   std::to_string(_priest_max_sp[level]));
}

auto Sorcery::Character::_get_mage_status(bool current) -> std::string {

	auto value{""s};
	for (auto level = 1; level <= 7; level++)
		if (current)
			value.append(std::format(" {} ", _mage_cur_sp[level]));
		else
			value.append(std::format("({})", _mage_max_sp[level]));

	RTRIM(value);
	return value;
}

auto Sorcery::Character::_get_priest_status(bool current) -> std::string {

	auto value{""s};
	for (auto level = 1; level <= 7; level++)
		if (current)
			value.append(std::format(" {} ", _priest_cur_sp[level]));
		else
			value.append(std::format("({})", _priest_max_sp[level]));

	RTRIM(value);
	return value;
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

auto Sorcery::Character::summary_text() -> std::string {

	auto name{_name};
	// if (_display->get_upper())
	//	std::ranges::transform(name.begin(), name.end(), name.begin(),
	//						   ::toupper);
	auto legacy{_legated ? " (D)" : ""};
	const auto level{_abilities.at(Enums::Character::Ability::CURRENT_LEVEL)};
	switch (_current_stage) {
		using enum Enums::Character::Stage;

	case CHOOSE_METHOD:
		[[fallthrough]];
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
	case ALLOCATE_STATS:
		return std::format("{:<15} L {:>2} {}-??? {}", name, level,
						   alignment_to_str(_alignment).substr(0, 1),
						   race_to_str(_race));
		break;
	case CHOOSE_CLASS:
		return std::format("{:<15} L {:>2} {}-??? {}", name, level,
						   alignment_to_str(_alignment).substr(0, 1),
						   race_to_str(_race));
		break;
	case CHOOSE_PORTRAIT:
		return std::format("{:<15} L {:>2} {}-{} {}", name, level,
						   alignment_to_str(_alignment).substr(0, 1),
						   class_to_str(_class).substr(0, 3),
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

auto Sorcery::Character::get_method() const -> Enums::Manage::Method {

	return _method;
}

auto Sorcery::Character::set_method(const Enums::Manage::Method value) -> void {

	_method = value;
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
			ac = ac + (*_resources->items)[item.get_type_id()].get_ac_mod();
		else if (item.get_equipped())
			ac = ac - (*_resources->items)[item.get_type_id()].get_ac_mod();
	}

	return ac;
}

auto Sorcery::Character::get_cur_to_hit() const -> int {

	auto to_hit{_abilities.at(Enums::Character::Ability::HIT_PROBABILITY)};

	for (const auto &item : inventory.items())
		to_hit =
			to_hit + (*_resources->items)[item.get_type_id()].get_to_hit_mod();

	return to_hit;
}

auto Sorcery::Character::get_cur_num_attacks() const -> int {

	auto base_attacks{
		_abilities.at(Enums::Character::Ability::BASE_NUMBER_OF_ATTACKS)};
	auto extra_attacks{0};

	for (const auto &item : inventory.items())
		extra_attacks = extra_attacks +
						(*_resources->items)[item.get_type_id()].get_swings();

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
