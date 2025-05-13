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
	set_status(Enums::Character::CStatus::OK);

	_version = 1;

	_legated = false;

	_location = Enums::Character::Location::TAVERN;

	coordinate = std::nullopt;
	depth = std::nullopt;

	inventory.clear();
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

	using Enums::Character::Location;

	_location = value;

	if (value == Location::TAVERN || value == Location::TEMPLE ||
		value == Location::TRAINING) {
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

	using Enums::Character::Stage;

	_current_stage = stage;
	switch (stage) {
	case Stage::CHOOSE_METHOD:
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
	case Stage::REVIEW_AND_CONFIRM:

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

	return fmt::format("{:<16} B{}F {:>2}N/{:>2}E", _name,
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

	using Enums::Character::Attribute;
	using Enums::Character::Race;
	using Enums::System::Random;

	_start_attr.clear();
	_cur_attr.clear();
	switch (_race) {
	case Race::HUMAN:
		_start_attr = {{Attribute::STRENGTH, 8}, {Attribute::IQ, 5},
					   {Attribute::PIETY, 5},	 {Attribute::VITALITY, 8},
					   {Attribute::AGILITY, 8},	 {Attribute::LUCK, 9}};
		break;
	case Race::ELF:
		_start_attr = {{Attribute::STRENGTH, 7}, {Attribute::IQ, 10},
					   {Attribute::PIETY, 10},	 {Attribute::VITALITY, 6},
					   {Attribute::AGILITY, 9},	 {Attribute::LUCK, 6}};
		break;
	case Race::DWARF:
		_start_attr = {{Attribute::STRENGTH, 10}, {Attribute::IQ, 7},
					   {Attribute::PIETY, 10},	  {Attribute::VITALITY, 10},
					   {Attribute::AGILITY, 5},	  {Attribute::LUCK, 6}};
		break;
	case Race::GNOME:
		_start_attr = {{Attribute::STRENGTH, 7}, {Attribute::IQ, 7},
					   {Attribute::PIETY, 10},	 {Attribute::VITALITY, 8},
					   {Attribute::AGILITY, 10}, {Attribute::LUCK, 7}};
		break;
	case Race::HOBBIT:
		_start_attr = {{Attribute::STRENGTH, 5}, {Attribute::IQ, 7},
					   {Attribute::PIETY, 7},	 {Attribute::VITALITY, 6},
					   {Attribute::AGILITY, 10}, {Attribute::LUCK, 12}};
		break;
	default:
		break;
	}

	_cur_attr = _start_attr;

	// Formula sourced from http://www.zimlab.com/wizardry/walk/w123calc.htm
	_points_left = (*_system->random)[Random::ZERO_TO_3];
	const bool chance_of_more{(*_system->random)[Random::D10] == 1};
	const bool chance_of_more_again{(*_system->random)[Random::D10] == 1};
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

	using Enums::Character::Align;
	using Enums::Character::Attribute;
	using Enums::Character::Class;

	_pos_classes.clear();

	// Do the basic classes first (this also sets
	// _num_possible_character_classes); data is from
	// https://strategywiki.org/wiki/Wizardry:_Proving_Grounds_of_the_Mad_Overlord/Trebor%27s_castle#Classes
	if (_cur_attr[Attribute::STRENGTH] >= 11)
		_pos_classes[Class::FIGHTER] = true;
	else
		_pos_classes[Class::FIGHTER] = false;

	if (_cur_attr[Attribute::IQ] >= 11)
		_pos_classes[Class::MAGE] = true;
	else
		_pos_classes[Class::MAGE] = false;

	if (_cur_attr[Attribute::PIETY] >= 11)
		if (_alignment == Align::GOOD || _alignment == Align::EVIL)
			_pos_classes[Class::PRIEST] = true;
		else
			_pos_classes[Class::PRIEST] = false;
	else
		_pos_classes[Class::PRIEST] = false;

	if (_cur_attr[Attribute::AGILITY] >= 11)
		if (_alignment == Align::NEUTRAL || _alignment == Align::EVIL)
			_pos_classes[Class::THIEF] = true;
		else
			_pos_classes[Class::THIEF] = false;
	else
		_pos_classes[Class::THIEF] = false;

	// Now the elite classes
	if (_cur_attr[Attribute::IQ] >= 12 && _cur_attr[Attribute::PIETY] >= 12)
		if (_alignment == Align::GOOD || _alignment == Align::EVIL)
			_pos_classes[Class::BISHOP] = true;
		else
			_pos_classes[Class::BISHOP] = false;
	else
		_pos_classes[Class::BISHOP] = false;

	if (_cur_attr[Attribute::STRENGTH] >= 15 &&
		_cur_attr[Attribute::IQ] >= 11 && _cur_attr[Attribute::PIETY] >= 10 &&
		_cur_attr[Attribute::VITALITY] >= 10 &&
		_cur_attr[Attribute::AGILITY] >= 10)
		if (_alignment == Align::GOOD || _alignment == Align::NEUTRAL)
			_pos_classes[Class::SAMURAI] = true;
		else
			_pos_classes[Class::SAMURAI] = false;
	else
		_pos_classes[Class::SAMURAI] = false;

	if (_cur_attr[Attribute::STRENGTH] >= 15 &&
		_cur_attr[Attribute::IQ] >= 12 && _cur_attr[Attribute::PIETY] >= 12 &&
		_cur_attr[Attribute::VITALITY] >= 15 &&
		_cur_attr[Attribute::AGILITY] >= 14 && _cur_attr[Attribute::LUCK] >= 15)
		if (_alignment == Align::GOOD)
			_pos_classes[Class::LORD] = true;
		else
			_pos_classes[Class::LORD] = false;
	else
		_pos_classes[Class::LORD] = false;

	// Using looser Wizardry 5 requirements for Ninja (see
	// https://wizardry.fandom.com/wiki/Ninja)
	if (_cur_attr[Attribute::STRENGTH] >= 15 &&
		_cur_attr[Attribute::IQ] >= 17 && _cur_attr[Attribute::PIETY] >= 15 &&
		_cur_attr[Attribute::VITALITY] >= 16 &&
		_cur_attr[Attribute::AGILITY] >= 15 && _cur_attr[Attribute::LUCK] >= 16)
		if (_alignment != Align::GOOD)
			_pos_classes[Class::NINJA] = true;
		else
			_pos_classes[Class::NINJA] = false;
	else
		_pos_classes[Class::NINJA] = false;

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

	using Enums::Character::Ability;
	using Enums::Character::Attribute;
	using Enums::Character::Class;
	using Enums::Character::CStatus;
	using Enums::Character::Location;
	using Enums::Character::Race;
	using Enums::System::Random;

	// From here:
	// https://datadrivengamer.blogspot.com/2021/08/the-new-mechanics-of-wizardry-iii.html
	_abilities[Ability::CURRENT_LEVEL] = 1;
	_abilities[Ability::CURRENT_XP] = 0;
	_abilities[Ability::NEXT_LEVEL_XP] =
		_get_xp_for_level(_abilities[Ability::CURRENT_LEVEL]);
	_abilities[Ability::MAX_LEVEL] = _abilities[Ability::CURRENT_LEVEL];
	_abilities[Ability::NEGATIVE_LEVEL] = 0;
	_abilities[Ability::HIT_DICE] = 1;
	if (_abilities[Ability::GOLD] > 500)
		_abilities[Ability::GOLD] = 500;
	_abilities[Ability::AGE] = 20 * 52;
	_abilities[Ability::SWIM] = 1;
	_abilities[Ability::MARKS] = 0;
	_abilities[Ability::DEATHS] = 0;

	// (D7 - 4) is -3 to +3
	_start_attr[Attribute::STRENGTH] += ((*_system->random)[Random::D7] - 4);
	_start_attr[Attribute::IQ] += ((*_system->random)[Random::D7] - 4);
	_start_attr[Attribute::PIETY] += ((*_system->random)[Random::D7] - 4);
	_start_attr[Attribute::VITALITY] += ((*_system->random)[Random::D7] - 4);
	_start_attr[Attribute::AGILITY] += ((*_system->random)[Random::D7] - 4);
	_start_attr[Attribute::LUCK] += ((*_system->random)[Random::D7] - 4);

	auto mage_sp_total{0};
	for (auto level = 1; level <= 7; level++)
		mage_sp_total += _mage_max_sp[level];
	_start_attr[Attribute::IQ] += (mage_sp_total / 7);

	auto priest_sp_total{1};
	for (auto level = 1; level <= 7; level++)
		priest_sp_total += _priest_max_sp[level];
	_start_attr[Attribute::PIETY] += (priest_sp_total / 10);

	switch (_class) { // NOLINT(clang-diagnostic-switch)
	case Class::FIGHTER:
	case Class::LORD:
	case Class::SAMURAI:
		_start_attr[Attribute::STRENGTH] += 2;
		break;
	case Class::MAGE:
		_start_attr[Attribute::IQ] += 2;
		break;
	case Class::PRIEST:
	case Class::BISHOP:
		_start_attr[Attribute::PIETY] += 2;
		break;
	case Class::THIEF:
	case Class::NINJA:
		_start_attr[Attribute::AGILITY] += 2;
	default:
		break;
	}

	set_status(CStatus::OK);
	_location = Location::TAVERN;
	_abilities[Ability::CURRENT_HP] = _abilities[Ability::MAX_HP];

	// Clamp Values
	_start_attr[Attribute::STRENGTH] =
		std::min(_start_attr[Attribute::STRENGTH], 18);
	_start_attr[Attribute::IQ] = std::min(_start_attr[Attribute::IQ], 18);
	_start_attr[Attribute::PIETY] = std::min(_start_attr[Attribute::PIETY], 18);
	_start_attr[Attribute::VITALITY] =
		std::min(_start_attr[Attribute::VITALITY], 18);
	_start_attr[Attribute::AGILITY] =
		std::min(_start_attr[Attribute::AGILITY], 18);
	_start_attr[Attribute::LUCK] = std::min(_start_attr[Attribute::LUCK], 18);
	_start_attr[Attribute::STRENGTH] =
		std::max(_start_attr[Attribute::STRENGTH], 3);
	_start_attr[Attribute::IQ] = std::max(_start_attr[Attribute::IQ], 3);
	_start_attr[Attribute::PIETY] = std::max(_start_attr[Attribute::PIETY], 3);
	_start_attr[Attribute::VITALITY] =
		std::max(_start_attr[Attribute::VITALITY], 3);
	_start_attr[Attribute::AGILITY] =
		std::max(_start_attr[Attribute::AGILITY], 3);
	_start_attr[Attribute::LUCK] = std::max(_start_attr[Attribute::LUCK], 3);

	_cur_attr = _start_attr;
	_max_attr = _cur_attr;

	_abilities[Ability::CURRENT_XP] = 0;
	_abilities[Ability::NEXT_LEVEL_XP] =
		_get_xp_for_level(_abilities[Ability::CURRENT_LEVEL]);
}

auto Sorcery::Character::_regenerate_start_info() -> void {

	using Enums::Character::Ability;
	using Enums::Character::Attribute;
	using Enums::Character::Class;
	using Enums::Character::CStatus;
	using Enums::Character::Location;
	using Enums::Character::Race;
	using Enums::System::Random;

	_abilities[Ability::MAX_LEVEL] = _abilities[Ability::CURRENT_LEVEL];
	_abilities[Ability::CURRENT_LEVEL] = 1;
	_abilities[Ability::CURRENT_XP] = 0;
	_abilities[Ability::NEXT_LEVEL_XP] =
		_get_xp_for_level(_abilities[Ability::CURRENT_LEVEL]);

	// https://datadrivengamer.blogspot.com/2019/08/the-not-so-basic-mechanics-of-wizardry.html
	auto age_increment{(52 * (3 + (*_system->random)[Random::D3])) + 44};
	_abilities[Ability::AGE] += age_increment;

	// Reset attributes to racial minimums
	std::map<Enums::Character::Attribute, int> minimum_attr;
	switch (_race) {
	case Race::HUMAN:
		minimum_attr = {{Attribute::STRENGTH, 8}, {Attribute::IQ, 5},
						{Attribute::PIETY, 5},	  {Attribute::VITALITY, 8},
						{Attribute::AGILITY, 8},  {Attribute::LUCK, 9}};
		break;
	case Race::ELF:
		minimum_attr = {{Attribute::STRENGTH, 7}, {Attribute::IQ, 10},
						{Attribute::PIETY, 10},	  {Attribute::VITALITY, 6},
						{Attribute::AGILITY, 9},  {Attribute::LUCK, 6}};
		break;
	case Race::DWARF:
		minimum_attr = {{Attribute::STRENGTH, 10}, {Attribute::IQ, 7},
						{Attribute::PIETY, 10},	   {Attribute::VITALITY, 10},
						{Attribute::AGILITY, 5},   {Attribute::LUCK, 6}};
		break;
	case Race::GNOME:
		minimum_attr = {{Attribute::STRENGTH, 7}, {Attribute::IQ, 7},
						{Attribute::PIETY, 10},	  {Attribute::VITALITY, 8},
						{Attribute::AGILITY, 10}, {Attribute::LUCK, 7}};
		break;
	case Race::HOBBIT:
		minimum_attr = {{Attribute::STRENGTH, 5}, {Attribute::IQ, 7},
						{Attribute::PIETY, 7},	  {Attribute::VITALITY, 6},
						{Attribute::AGILITY, 10}, {Attribute::LUCK, 12}};
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
	set_status(Enums::Character::CStatus::OK);
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

	using Enums::Character::Ability;
	using Enums::System::Random;

	// _abilities[CURRENT_LEVEL] = 1; // why is this commented out?
	_abilities[Ability::MAX_LEVEL] = 1;
	_abilities[Ability::NEGATIVE_LEVEL] = 0;
	_abilities[Ability::HIT_DICE] = 1;
	_abilities[Ability::GOLD] = (*_system->random)[Random::ZERO_TO_99] + 90;
	_abilities[Ability::AGE] =
		(18 * 52) + (*_system->random)[Random::ZERO_TO_299];
	_abilities[Ability::SWIM] = 1;
	_abilities[Ability::MARKS] = 0;
	_abilities[Ability::DEATHS] = 0;

	_start_attr = _cur_attr;
	_max_attr = _cur_attr;

	_abilities[Ability::CURRENT_XP] = 0;
	_abilities[Ability::NEXT_LEVEL_XP] =
		_get_xp_for_level(_abilities[Ability::CURRENT_LEVEL]);
}

// Given the characters current level, work out all the secondary
// abilities/stats etc
auto Sorcery::Character::_generate_secondary_abil(bool initial,
												  bool change_class,
												  bool legate) -> void {

	using Enums::Character::Ability;
	using Enums::Character::Attribute;
	using Enums::Character::Class;
	using Enums::Character::Race;
	using Enums::System::Random;

	// Formulae used are from here
	// http://www.zimlab.com/wizardry/walk/w123calc.htm and also from
	// https://mirrors.apple2.org.za/ftp.apple.asimov.net/images/games/rpg/wizardry/wizardry_I/Wizardry_i_SourceCode.zip
	const auto current_level{_abilities[Ability::CURRENT_LEVEL]};

	// Bonus Melee to Hit per Attack (num)
	if (_cur_attr[Attribute::STRENGTH] > 15)
		_abilities[Ability::ATTACK_MODIFIER] =
			_cur_attr[Attribute::STRENGTH] - 15;
	else if (_cur_attr[Attribute::STRENGTH] < 6)
		_abilities[Ability::ATTACK_MODIFIER] =
			_cur_attr[Attribute::STRENGTH] - 6;
	else
		_abilities[Ability::ATTACK_MODIFIER] = 0;

	// Bonus Melee to Hit per Attack (num)
	switch (_class) {
	case Class::FIGHTER:
	case Class::SAMURAI:
	case Class::LORD:
	case Class::NINJA:
	case Class::PRIEST:
		_abilities[Ability::HIT_PROBABILITY] = 2 + (current_level / 5);
		break;
	default:
		_abilities[Ability::HIT_PROBABILITY] = current_level / 5;
		break;
	}

	// Bonus Melee Damage per Attack (num)
	if (_cur_attr[Attribute::STRENGTH] > 15)
		_abilities[Ability::BONUS_DAMAGE] = _cur_attr[Attribute::STRENGTH] - 15;
	else if (_cur_attr[Attribute::STRENGTH] < 6)
		_abilities[Ability::BONUS_DAMAGE] = _cur_attr[Attribute::STRENGTH] - 6;
	else
		_abilities[Ability::BONUS_DAMAGE] = 0;

	// Unarmed Attack Damage (num)
	_abilities[Ability::UNARMED_DAMAGE] =
		_class == Class::NINJA ? 8 + _abilities[Ability::BONUS_DAMAGE]
							   : 4 + _abilities[Ability::BONUS_DAMAGE];

	// Number of Melee Attacks (num)
	switch (_class) {
	case Class::FIGHTER:
	case Class::SAMURAI:
	case Class::LORD:
		_abilities[Ability::BASE_NUMBER_OF_ATTACKS] = current_level / 5;
		break;
	case Class::NINJA:
		_abilities[Ability::BASE_NUMBER_OF_ATTACKS] = (current_level / 5) + 1;
		break;
	default:
		_abilities[Ability::BASE_NUMBER_OF_ATTACKS] = 1;
		break;
	}
	if (_abilities[Ability::BASE_NUMBER_OF_ATTACKS] == 0)
		_abilities[Ability::BASE_NUMBER_OF_ATTACKS] = 1;

	// Chance of learning new Mage Spells (%)
	switch (_class) {
	case Class::SAMURAI:
	case Class::BISHOP:
	case Class::MAGE:
		_abilities[Ability::MAGE_SPELL_LEARN] =
			(_cur_attr[Attribute::IQ] / 29.0) * 100;
		break;
	default:
		_abilities[Ability::MAGE_SPELL_LEARN] = 0;
		break;
	}

	// Chance of Identifying Items (%)
	_abilities[Ability::IDENTIFY_ITEMS] =
		_class == Class::BISHOP ? 10 + (5 * current_level) : 0;
	if (_abilities[Ability::IDENTIFY_ITEMS] > 100)
		_abilities[Ability::IDENTIFY_ITEMS] = 100;

	// Chance of getting cursed when Identifying Items (%)
	_abilities[Ability::IDENTIFY_CURSE] =
		_class == Class::BISHOP ? 35 - (5 * current_level) : 0;
	if (_abilities[Ability::IDENTIFY_CURSE] < 0)
		_abilities[Ability::IDENTIFY_CURSE] = 0;

	// Chance of identifying unknown Foes per round (%)
	_abilities[Ability::IDENTIFY_FOES] =
		current_level + _cur_attr[Attribute::IQ] + _cur_attr[Attribute::PIETY];
	if (_abilities[Ability::IDENTIFY_FOES] > 100)
		_abilities[Ability::IDENTIFY_FOES] = 100;

	// Chance of learning new Priest Spells (%)
	switch (_class) {
	case Class::PRIEST:
	case Class::LORD:
	case Class::BISHOP:
		_abilities[Ability::PRIEST_SPELL_LEARN] =
			(_cur_attr[Attribute::PIETY] / 30.0) * 100;
		break;
	default:
		_abilities[Ability::PRIEST_SPELL_LEARN] = 0;
		break;
	}

	// LOKTOFELT success chance (%)
	_abilities[Ability::LOKTOFELT_SUCCESS] =
		_class == Class::PRIEST ? 2 * current_level : 0;

	// Base Dispell chance (affected by monster level) (%)
	switch (_class) {
	case Class::PRIEST:
		_abilities[Ability::BASE_DISPELL] = 50 + (5 * current_level);
		break;
	case Class::BISHOP:
		_abilities[Ability::BASE_DISPELL] =
			current_level >= 4 ? 50 + (5 * current_level) - 20 : 0;
		break;
	case Class::LORD:
		_abilities[Ability::BASE_DISPELL] =
			current_level >= 9 ? 50 + (5 * current_level) - 40 : 0;
		break;
	default:
		_abilities[Ability::BASE_DISPELL] = 0;
		break;
	}
	if (_abilities[Ability::BASE_DISPELL] > 100)
		_abilities[Ability::BASE_DISPELL] = 100;

	// Vitality Bonus (num)
	switch (_cur_attr[Attribute::VITALITY]) {
	case 3:
		_abilities[Ability::VITALITY_BONUS] = -2;
		break;
	case 4:
	case 5:
		_abilities[Ability::VITALITY_BONUS] = -1;
		break;
	case 16:
		_abilities[Ability::VITALITY_BONUS] = 1;
		break;
	case 17:
		_abilities[Ability::VITALITY_BONUS] = 2;
		break;
	case 18:
	case 19:
	case 20:
		_abilities[Ability::VITALITY_BONUS] = 3;
		break;
	default:
		_abilities[Ability::VITALITY_BONUS] = 0;
		break;
	}

	// Bonus Hit Points per level (num)
	_abilities[Ability::BONUS_HIT_POINTS] = _abilities[Ability::VITALITY_BONUS];

	// Class Change doesn't reset these
	if (!change_class) {

		// Base Hit Points (num) - note initially all non-legated characters get
		// 8 HP as per the PSX versions
		if (initial) {
			const auto bonus{_abilities[Ability::BONUS_HIT_POINTS]};
			switch (auto chance{(*_system->random)[Random::D100]};
					_class) { // NOLINT(clang-diagnostic-switch)
			case Class::FIGHTER:
			case Class::LORD:
				_abilities[Ability::MAX_HP] =
					chance <= 50 ? 10 + bonus : 9 * (10 + bonus) / 10;
				break;
			case Class::PRIEST:
				_abilities[Ability::MAX_HP] =
					chance <= 50 ? 8 + bonus : 8 * (10 + bonus) / 10;
				break;
			case Class::THIEF:
			case Class::BISHOP:
			case Class::NINJA:
				_abilities[Ability::MAX_HP] =
					chance <= 50 ? 6 + bonus : 6 * (10 + bonus) / 10;
				break;
			case Class::MAGE:
				_abilities[Ability::MAX_HP] =
					chance <= 50 ? 4 + bonus : 4 * (10 + bonus) / 10;
				break;
			case Class::SAMURAI:
				_abilities[Ability::MAX_HP] =
					chance <= 50 ? 16 + bonus : 16 * (10 + bonus) / 10;
				break;
			default:
				break;
			}
			if (_abilities[Ability::MAX_HP] < 1)
				_abilities[Ability::MAX_HP] = 1;

			_abilities[Ability::CURRENT_HP] = _abilities[Ability::MAX_HP];
		}

		if (legate) {
			switch (_class) {
			case Class::FIGHTER:
				_abilities[Ability::MAX_HP] = 10;
				break;
			case Class::SAMURAI:
			case Class::LORD:
				_abilities[Ability::MAX_HP] = 12;
				break;
			case Class::PRIEST:
				_abilities[Ability::MAX_HP] = 8;
				break;
			case Class::NINJA:
				_abilities[Ability::MAX_HP] = 7;
				break;
			case Class::THIEF:
				_abilities[Ability::MAX_HP] = 6;
				break;
			case Class::MAGE:
				_abilities[Ability::MAX_HP] = 4;
				break;
			default:
				break;
			}

			_abilities[Ability::CURRENT_HP] = _abilities[Ability::MAX_HP];
		}
	}

	// Chance of resurrecting a Dead Character at the Temple (%)
	_abilities[Ability::DEAD_RESURRECT] =
		50 + (3 * _cur_attr[Attribute::VITALITY]);
	if (_abilities[Ability::DEAD_RESURRECT] > 100)
		_abilities[Ability::DEAD_RESURRECT] = 100;

	// Chance of resurrecting an Ashed Character at the Temple (%)
	_abilities[Ability::ASHES_RESURRECT] =
		40 + (3 * _cur_attr[Attribute::VITALITY]);
	if (_abilities[Ability::ASHES_RESURRECT] > 100)
		_abilities[Ability::ASHES_RESURRECT] = 100;

	// Chance of resurrecting by a DI or KADORTO spell cast by another Character
	// (%)
	_abilities[Ability::DI_KADORTO_RESURRECT] =
		4 * _cur_attr[Attribute::VITALITY];

	// Initiative Modifier (num)
	switch (_cur_attr[Attribute::AGILITY]) {
	case 3:
		_abilities[Ability::INITIATIVE_MODIFIER] = 3;
		break;
	case 4:
	case 5:
		_abilities[Ability::INITIATIVE_MODIFIER] = 2;
		break;
	case 6:
	case 7:
		_abilities[Ability::INITIATIVE_MODIFIER] = 1;
		break;
	case 15:
		_abilities[Ability::INITIATIVE_MODIFIER] = -1;
		break;
	case 16:
		_abilities[Ability::INITIATIVE_MODIFIER] = -2;
		break;
	case 17:
		_abilities[Ability::INITIATIVE_MODIFIER] = -3;
		break;
	case 18:
	case 19:
	case 20:
		_abilities[Ability::INITIATIVE_MODIFIER] = -4;
		break;
	default:
		_abilities[Ability::INITIATIVE_MODIFIER] = 0;
		break;
	}

	// Armour Class (num) (before equipment)
	_abilities[Ability::BASE_ARMOUR_CLASS] =
		_class == Class::NINJA ? 10 - (current_level / 3) - 2 : 10;

	// Don't use this!
	_abilities[Ability::CURRENT_ARMOUR_CLASS] =
		_abilities[Ability::BASE_ARMOUR_CLASS];

	// Critical Hit Chance (%)
	_abilities[Ability::BASE_CRITICAL_HIT] =
		_class == Class::NINJA ? 2 * current_level : 0;
	if (_abilities[Ability::BASE_CRITICAL_HIT] > 50)
		_abilities[Ability::BASE_CRITICAL_HIT] = 50;

	// Chance to identify a Trap (%)
	if (_class == Class::THIEF)
		_abilities[Ability::IDENTIFY_TRAP] = 6 * _cur_attr[Attribute::AGILITY];
	else if (_class == Class::NINJA)
		_abilities[Ability::IDENTIFY_TRAP] = 4 * _cur_attr[Attribute::AGILITY];
	else
		_abilities[Ability::IDENTIFY_TRAP] = _cur_attr[Attribute::AGILITY];
	if (_abilities[Ability::IDENTIFY_TRAP] > 95)
		_abilities[Ability::IDENTIFY_TRAP] = 95;

	// Base Chance to Disarm a Trap (modified by Maze Level) (%)
	_abilities[Ability::BASE_DISARM_TRAP] =
		(_class == Class::NINJA) || (_class == Class::THIEF)
			? ((current_level + 50) / 69.0) * 100
			: 0;

	// Chance to activate a Trap if identify fails (%)
	_abilities[Ability::ACTIVATE_TRAP] =
		(_class == Class::NINJA) || (_class == Class::THIEF)
			? 100 - ((_cur_attr[Attribute::AGILITY] / 20.0) * 100)
			: 100;

	// Base Chance to avoid following into a Pit (modified by Maze Level) (%)
	_abilities[Ability::BASE_AVOID_PIT] =
		(_cur_attr[Attribute::AGILITY] / 25.0) * 100;

	// Base Resist Bonus (d20)
	_abilities[Ability::BASE_RESIST_BONUS] = 1 * (current_level / 5);
	if (_cur_attr[Attribute::LUCK] >= 18)
		_abilities[Ability::BASE_RESIST_BONUS] += 3;
	else if (_cur_attr[Attribute::LUCK] >= 12)
		_abilities[Ability::BASE_RESIST_BONUS] += 2;
	else if (_cur_attr[Attribute::LUCK] >= 6)
		_abilities[Ability::BASE_RESIST_BONUS] += 1;

	// Chance equipment is intact on a corpse TODO: check this is accurate
	_abilities[Ability::EQUIPMENT_INTACT_ON_WIPE] =
		(_cur_attr[Attribute::LUCK] / 21.0f) * 100;

	// Other Resists (d20)
	const auto brb{_abilities[Ability::BASE_RESIST_BONUS]};
	switch (_class) { // NOLINT(clang-diagnostic-switch)
	case Class::SAMURAI:
		_abilities[Ability::RESISTANCE_VS_POISON_PARALYSIS] = brb + 2;
		_abilities[Ability::RESISTANCE_VS_CRITICAL_HIT] = brb + 2;
		_abilities[Ability::RESISTANCE_VS_STONING] = 0;
		_abilities[Ability::RESISTANCE_VS_BREATH_ATTACKS] = 0;
		_abilities[Ability::RESISTANCE_VS_POISON_GAS_TRAP] = 0;
		_abilities[Ability::RESISTANCE_VS_MAGE_PRIEST_TRAP] = brb + 2;
		_abilities[Ability::RESISTANCE_VS_SILENCE] = brb + 2;
		break;
	case Class::FIGHTER:
		_abilities[Ability::RESISTANCE_VS_POISON_PARALYSIS] = brb + 3;
		_abilities[Ability::RESISTANCE_VS_CRITICAL_HIT] = brb + 3;
		_abilities[Ability::RESISTANCE_VS_STONING] = 0;
		_abilities[Ability::RESISTANCE_VS_BREATH_ATTACKS] = 0;
		_abilities[Ability::RESISTANCE_VS_POISON_GAS_TRAP] = 0;
		_abilities[Ability::RESISTANCE_VS_MAGE_PRIEST_TRAP] = 0;
		_abilities[Ability::RESISTANCE_VS_SILENCE] = 0;
		break;
	case Class::PRIEST:
		_abilities[Ability::RESISTANCE_VS_POISON_PARALYSIS] = 0;
		_abilities[Ability::RESISTANCE_VS_CRITICAL_HIT] = 0;
		_abilities[Ability::RESISTANCE_VS_STONING] = brb + 3;
		_abilities[Ability::RESISTANCE_VS_BREATH_ATTACKS] = 0;
		_abilities[Ability::RESISTANCE_VS_POISON_GAS_TRAP] = 0;
		_abilities[Ability::RESISTANCE_VS_MAGE_PRIEST_TRAP] = 0;
		_abilities[Ability::RESISTANCE_VS_SILENCE] = 0;
		break;
	case Class::LORD:
		_abilities[Ability::RESISTANCE_VS_POISON_PARALYSIS] = brb + 2;
		_abilities[Ability::RESISTANCE_VS_CRITICAL_HIT] = brb + 2;
		_abilities[Ability::RESISTANCE_VS_STONING] = brb + 2;
		_abilities[Ability::RESISTANCE_VS_BREATH_ATTACKS] = 0;
		_abilities[Ability::RESISTANCE_VS_POISON_GAS_TRAP] = 0;
		_abilities[Ability::RESISTANCE_VS_MAGE_PRIEST_TRAP] = 0;
		_abilities[Ability::RESISTANCE_VS_SILENCE] = 0;
		break;
	case Class::BISHOP:
		_abilities[Ability::RESISTANCE_VS_POISON_PARALYSIS] = 0;
		_abilities[Ability::RESISTANCE_VS_CRITICAL_HIT] = 0;
		_abilities[Ability::RESISTANCE_VS_STONING] = brb + 2;
		_abilities[Ability::RESISTANCE_VS_BREATH_ATTACKS] = brb + 2;
		_abilities[Ability::RESISTANCE_VS_POISON_GAS_TRAP] = 0;
		_abilities[Ability::RESISTANCE_VS_MAGE_PRIEST_TRAP] = brb + 2;
		_abilities[Ability::RESISTANCE_VS_SILENCE] = brb + 2;
		break;
	case Class::THIEF:
		_abilities[Ability::RESISTANCE_VS_POISON_PARALYSIS] = 0;
		_abilities[Ability::RESISTANCE_VS_CRITICAL_HIT] = 0;
		_abilities[Ability::RESISTANCE_VS_STONING] = 0;
		_abilities[Ability::RESISTANCE_VS_BREATH_ATTACKS] = 0;
		_abilities[Ability::RESISTANCE_VS_POISON_GAS_TRAP] = brb + 3;
		_abilities[Ability::RESISTANCE_VS_MAGE_PRIEST_TRAP] = 0;
		_abilities[Ability::RESISTANCE_VS_SILENCE] = 0;
		break;
	case Class::NINJA:
		_abilities[Ability::RESISTANCE_VS_POISON_PARALYSIS] = brb + 3;
		_abilities[Ability::RESISTANCE_VS_CRITICAL_HIT] = brb + 3;
		_abilities[Ability::RESISTANCE_VS_STONING] = brb + 2;
		_abilities[Ability::RESISTANCE_VS_BREATH_ATTACKS] = brb + 4;
		_abilities[Ability::RESISTANCE_VS_POISON_GAS_TRAP] = brb + 3;
		_abilities[Ability::RESISTANCE_VS_MAGE_PRIEST_TRAP] = brb + 2;
		_abilities[Ability::RESISTANCE_VS_SILENCE] = brb + 2;
		break;
	case Class::MAGE:
		_abilities[Ability::RESISTANCE_VS_POISON_PARALYSIS] = 0;
		_abilities[Ability::RESISTANCE_VS_CRITICAL_HIT] = 0;
		_abilities[Ability::RESISTANCE_VS_STONING] = 0;
		_abilities[Ability::RESISTANCE_VS_BREATH_ATTACKS] = 0;
		_abilities[Ability::RESISTANCE_VS_POISON_GAS_TRAP] = 0;
		_abilities[Ability::RESISTANCE_VS_MAGE_PRIEST_TRAP] = brb + 3;
		_abilities[Ability::RESISTANCE_VS_SILENCE] = brb + 3;
		break;
	default:
		break;
	}
	if (_race == Race::HUMAN)
		_abilities[Ability::RESISTANCE_VS_POISON_PARALYSIS] += 1;
	if (_race == Race::HUMAN)
		_abilities[Ability::RESISTANCE_VS_CRITICAL_HIT] += 1;
	if (_race == Race::GNOME)
		_abilities[Ability::RESISTANCE_VS_STONING] += 2;
	if (_race == Race::ELF)
		_abilities[Ability::RESISTANCE_VS_BREATH_ATTACKS] += 2;
	if (_race == Race::DWARF)
		_abilities[Ability::RESISTANCE_VS_POISON_GAS_TRAP] += 4;
	if (_race == Race::HOBBIT) {
		_abilities[Ability::RESISTANCE_VS_MAGE_PRIEST_TRAP] += 2;
		_abilities[Ability::RESISTANCE_VS_SILENCE] += 3;
	}
	if (_abilities[Ability::RESISTANCE_VS_POISON_PARALYSIS] > 19)
		_abilities[Ability::RESISTANCE_VS_POISON_PARALYSIS] = 19;
	if (_abilities[Ability::RESISTANCE_VS_CRITICAL_HIT] > 19)
		_abilities[Ability::RESISTANCE_VS_CRITICAL_HIT] = 19;
	if (_abilities[Ability::RESISTANCE_VS_STONING] > 19)
		_abilities[Ability::RESISTANCE_VS_STONING] = 19;
	if (_abilities[Ability::RESISTANCE_VS_BREATH_ATTACKS] > 19)
		_abilities[Ability::RESISTANCE_VS_BREATH_ATTACKS] = 19;
	if (_abilities[Ability::RESISTANCE_VS_POISON_GAS_TRAP] > 19)
		_abilities[Ability::RESISTANCE_VS_POISON_GAS_TRAP] = 19;
	if (_abilities[Ability::RESISTANCE_VS_MAGE_PRIEST_TRAP] > 19)
		_abilities[Ability::RESISTANCE_VS_MAGE_PRIEST_TRAP] = 19;
	if (_abilities[Ability::RESISTANCE_VS_SILENCE] > 19)
		_abilities[Ability::RESISTANCE_VS_SILENCE] = 19;

	// Resistance to harmful spells (%)
	_abilities[Ability::RESISTANCE_VS_KATINO] = current_level * 10;
	if (_abilities[Ability::RESISTANCE_VS_KATINO] > 100)
		_abilities[Ability::RESISTANCE_VS_KATINO] = 100;
	_abilities[Ability::RESISTANCE_VS_BADI] = current_level * 10;
	if (_abilities[Ability::RESISTANCE_VS_BADI] > 100)
		_abilities[Ability::RESISTANCE_VS_BADI] = 100;
	_abilities[Ability::RESISTANCE_VS_MANIFO] = 50 + (current_level * 10);
	if (_abilities[Ability::RESISTANCE_VS_MANIFO] > 100)
		_abilities[Ability::RESISTANCE_VS_MANIFO] = 100;

	_abilities[Ability::RECOVER_FROM_SLEEP] = current_level * 10;
	if (_abilities[Ability::RECOVER_FROM_SLEEP] > 100)
		_abilities[Ability::RECOVER_FROM_SLEEP] = 100;

	_abilities[Ability::RECOVER_FROM_FEAR] = current_level * 5;
	if (_abilities[Ability::RECOVER_FROM_FEAR] > 100)
		_abilities[Ability::RECOVER_FROM_FEAR] = 100;

	_abilities[Ability::BONUS_MAGE_SPELLS] = 0;
	_abilities[Ability::BONUS_PRIEST_SPELLS] = 0;

	// And set poison/regeneration to default
	if (!change_class) {
		_abilities[Ability::HP_LOSS_PER_TURN] = 0;
		_abilities[Ability::HP_GAIN_PER_TURN] = 0;
		_abilities[Ability::POISON_STRENGTH] = 0;
	}
}

auto Sorcery::Character::grant_xp(const int adjustment) -> int {

	using Enums::Character::Ability;

	_abilities[Ability::CURRENT_XP] =
		_abilities[Ability::CURRENT_XP] + adjustment;

	return _abilities[Ability::CURRENT_XP];
}

// Now work out spellpoints!
auto Sorcery::Character::_reset_starting_sp() -> void {

	_set_starting_sp();

	// And add in 1 spell point for each known spell NOT of the type
	for (auto spell_level = 1u; spell_level <= 7u; spell_level++) {

		// Handle Priest Spells
		auto priest_known{static_cast<unsigned int>(
			std::count_if(_spells.begin(), _spells.end(), [=](auto spell) {
				return (spell.type == Enums::Magic::SpellType::PRIEST) &&
					   (spell.level == spell_level) && (spell.known);
			}))};

		if (_priest_max_sp[spell_level] < priest_known) {
			_priest_max_sp[spell_level] = priest_known;
			_priest_cur_sp[spell_level] = _priest_max_sp[spell_level];
		}

		// Handle Mage Spells
		auto mage_known{static_cast<unsigned int>(
			std::count_if(_spells.begin(), _spells.end(), [=](auto spell) {
				return (spell.type == Enums::Magic::SpellType::MAGE) &&
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

	using Enums::Character::Ability;
	using Enums::Character::Class;
	using Enums::Config::Options;

	// By default clear all spells
	_clear_sp();

	// In the original code this is handled in
	// "SETSPELS"/"SPLPERLV"/"NWMAGE"/"NWPRIEST"
	switch (_class) { // NOLINT(clang-diagnostic-switch)
	case Class::PRIEST:
		_priest_max_sp[1] = (*_system->config)[Options::STRICT_MODE]
								? 2
								: 2 + _abilities[Ability::BONUS_PRIEST_SPELLS];
		break;
	case Class::BISHOP:
		_mage_max_sp[1] = 2;
		break;
	case Class::MAGE:
		_mage_max_sp[1] = (*_system->config)[Options::STRICT_MODE]
							  ? 2
							  : 2 + _abilities[Ability::BONUS_MAGE_SPELLS];
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

	using Enums::Character::Class;
	using Enums::Magic::SpellID;

	// This is taken from "KEEPCHYN" which hard codes the spells known to
	// beginning characters!
	switch (_class) { // NOLINT(clang-diagnostic-switch)
	case Class::BISHOP:
	case Class::MAGE:
		_learn_spell(SpellID::KATINO);
		_learn_spell(SpellID::HALITO);
		break;
	case Class::PRIEST:
		_learn_spell(SpellID::DIOS);
		_learn_spell(SpellID::BADIOS);
		break;
	default:
		break;
	}
}

// Get HP gained for all levels apart from the first
auto Sorcery::Character::_get_hp_per_level() -> int {

	using Enums::Character::Ability;
	using Enums::Character::Class;
	using Enums::System::Random;

	// In the original code ("MOREHP"), Samurai get 2d8
	auto extra_hp{0};
	switch (_class) { // NOLINT(clang-diagnostic-switch)
	case Class::FIGHTER:
	case Class::LORD:
		extra_hp += (*_system->random)[Random::D10];
		break;
	case Class::PRIEST:
		extra_hp += (*_system->random)[Random::D8];
		break;
	case Class::THIEF:
	case Class::BISHOP:
	case Class::NINJA:
		extra_hp += (*_system->random)[Random::D6];
		break;
	case Class::MAGE:
		extra_hp += (*_system->random)[Random::D4];
		break;
	case Class::SAMURAI:
		extra_hp += (*_system->random)[Random::D8];
		extra_hp += (*_system->random)[Random::D8];
		break;
	default:
		break;
	}

	extra_hp += _abilities[Ability::BONUS_HIT_POINTS];
	if (extra_hp < 0)
		extra_hp = 1;

	// Though this could be an unsigned int as it will always be greater than 0,
	// just return int as everything else is
	return extra_hp;
}

// Add hit points on level gain (but note the strict mode limitation mentioned
// below)
auto Sorcery::Character::_update_hp_for_level() -> int {

	using Enums::Character::Ability;
	using Enums::Character::Class;

	// Note the rerolling of all HP ("MADELEV") when levelling - and using
	// MaxLevel achieved when in strict mode
	auto hp_gained{0};
	if ((*_system->config)[Enums::Config::LEVEL_REROLL_HP]) {
		auto hp_total{0};
		for (auto level = 1; level < _abilities[Ability::CURRENT_LEVEL];
			 level++)
			hp_total += _get_hp_per_level();
		if (hp_total < _abilities[Ability::MAX_HP])
			hp_total = _abilities[Ability::MAX_HP] + 1;
		hp_gained = hp_total - _abilities[Ability::MAX_HP];
		_abilities[Ability::MAX_HP] += hp_gained;
		_abilities[Ability::CURRENT_HP] += hp_gained;
	} else {
		hp_gained = _get_hp_per_level();
		_abilities[Ability::MAX_HP] += hp_gained;
		_abilities[Ability::CURRENT_HP] += hp_gained;
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

	using Enums::Character::Ability;
	using Enums::System::Random;

	auto message{""s};

	if ((*_system->random)[Random::D100] < 75) {
		const auto chance{_abilities.at(Ability::AGE) / 130.f};
		if ((*_system->random)[Random::D100] < chance) {

			// Decrease
			bool proceed{true};
			if (_cur_attr.at(attribute) == 18 &&
				(*_system->random)[Random::D6] > 1)
				proceed = false;

			if (proceed) {
				_cur_attr.at(attribute) = _cur_attr.at(attribute) - 1;
				message = fmt::format("{} {}",
									  (*_system->strings)["LEVEL_LOSS"], stat);
				if (_cur_attr.at(attribute) < 1)
					_cur_attr.at(attribute) = 1;
			}
		} else {
			if (_cur_attr.at(attribute) < 18) {
				_cur_attr.at(attribute) = _cur_attr.at(attribute) + 1;
				if (_cur_attr.at(attribute) > _max_attr.at(attribute))
					_max_attr.at(attribute) = _cur_attr.at(attribute);
				message = fmt::format("{} {}",
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

	using Enums::Character::Ability;
	using Enums::Character::Attribute;

	level_up_results.clear();
	level_up_results.emplace_back((*_system->strings)["LEVEL_DING"]);

	// Increase level
	_abilities.at(Ability::CURRENT_LEVEL) =
		_abilities.at(Ability::CURRENT_LEVEL) + 1;
	_abilities.at(Ability::HIT_DICE) = _abilities.at(Ability::HIT_DICE) + 1;
	if (_abilities.at(Ability::CURRENT_LEVEL) >
		_abilities.at(Ability::MAX_LEVEL))
		_abilities.at(Ability::MAX_LEVEL) =
			_abilities.at(Ability::CURRENT_LEVEL);

	// Handle learning spells
	if (_set_sp())
		level_up_results.emplace_back((*_system->strings)["LEVEL_SPELLS"]);

	// Work out new xp needed
	_abilities[Ability::NEXT_LEVEL_XP] =
		_get_xp_for_level(_abilities[Ability::CURRENT_LEVEL]);

	// Handle stat changing
	auto stat_message{""s};
	stat_message = _update_stat_for_level(
		Attribute::STRENGTH, (*_system->strings)["CHARACTER_STAT_STRENGTH"]);
	if (!stat_message.empty())
		level_up_results.emplace_back(stat_message);
	stat_message = _update_stat_for_level(
		Attribute::IQ, (*_system->strings)["CHARACTER_STAT_INTELLIGENCE"]);
	if (!stat_message.empty())
		level_up_results.emplace_back(stat_message);
	stat_message = _update_stat_for_level(
		Attribute::PIETY, (*_system->strings)["CHARACTER_STAT_PIETY"]);
	if (!stat_message.empty())
		level_up_results.emplace_back(stat_message);
	stat_message = _update_stat_for_level(
		Attribute::VITALITY, (*_system->strings)["CHARACTER_STAT_VITALITY"]);
	if (!stat_message.empty())
		level_up_results.emplace_back(stat_message);
	stat_message = _update_stat_for_level(
		Attribute::AGILITY, (*_system->strings)["CHARACTER_STAT_AGILITY"]);
	if (!stat_message.empty())
		level_up_results.emplace_back(stat_message);
	stat_message = _update_stat_for_level(
		Attribute::LUCK, (*_system->strings)["CHARACTER_STAT_LUCK"]);
	if (!stat_message.empty())
		level_up_results.emplace_back(stat_message);

	// Level everything else up
	_generate_secondary_abil(false, false, false);

	// handle hp
	const auto hp_gained{_update_hp_for_level()};
	const auto hp_message{
		fmt::format("{} {} {}", (*_system->strings)["LEVEL_HP_PREFIX"],
					hp_gained, (*_system->strings)["LEVEL_HP_SUFFIX"])};
	level_up_results.emplace_back(hp_message);

	if (_cur_attr.at(Attribute::VITALITY) < 3) {
		level_up_results.emplace_back((*_system->strings)["LEVEL_DIE"]);
		_status = Enums::Character::CStatus::LOST;
		_location = Enums::Character::Location::TRAINING;
	}
}

// Level a character down (e.g. drain levels or give/increase negative levels_
auto Sorcery::Character::level_down() -> void {

	using Enums::Character::Ability;

	if (_abilities.at(Ability::CURRENT_LEVEL) == 1) {
		_status = Enums::Character::CStatus::LOST;
		_location = Enums::Character::Location::TRAINING;
		return;
	}

	// TODO: What to do about negative level ability?
	const auto old_level{_abilities.at(Ability::CURRENT_LEVEL)};
	const auto diff_hp{_abilities.at(Ability::MAX_HP) -
					   _abilities.at(Ability::CURRENT_HP)};

	_abilities.at(Ability::CURRENT_LEVEL) =
		_abilities.at(Ability::CURRENT_LEVEL) - 1;
	_abilities.at(Ability::HIT_DICE) = _abilities.at(Ability::HIT_DICE) - 1;
	if (_abilities.at(Ability::MAX_LEVEL) >
		_abilities.at(Ability::CURRENT_LEVEL))
		_abilities.at(Ability::MAX_LEVEL) =
			_abilities.at(Ability::CURRENT_LEVEL);

	_set_sp();

	// When drained XP is set to beginning of current level (for example,
	// draining to level 9 means that youre xp is set to enough for level 9 plus
	// 1 - which is vety harsh)
	_abilities[Ability::NEXT_LEVEL_XP] =
		_get_xp_for_level(_abilities.at(Ability::CURRENT_LEVEL));
	_abilities[Ability::CURRENT_XP] =
		_get_xp_for_level(_abilities.at(Ability::CURRENT_LEVEL) - 1) + 1;

	_generate_secondary_abil(false, false, false);
	_abilities[Ability::MAX_HP] =
		_abilities.at(Ability::MAX_HP) *
		(_abilities.at(Ability::CURRENT_LEVEL) / (old_level * 1.f));
	_abilities[Ability::CURRENT_HP] = _abilities[Ability::MAX_HP] - diff_hp;
	if (_abilities[Ability::CURRENT_HP] < 0)
		_abilities[Ability::CURRENT_HP] = 0;
}

// For each spell level, try to learn spells - called before set_spellpoints
auto Sorcery::Character::_try_learn_spell(Enums::Magic::SpellType spell_type,
										  unsigned int spell_level) -> bool {

	using Enums::Character::Attribute;
	using Enums::Magic::SpellType;
	using Enums::System::Random;

	bool new_spell_learnt{false};

	// Only do spells if a character has available spell points in this spell
	// level
	if (spell_type == SpellType::PRIEST)
		if (_priest_max_sp[spell_level] == 0)
			return false;
	if (spell_type == SpellType::MAGE)
		if (_mage_max_sp[spell_level] == 0)
			return false;

	// Get all unknown spells of this spell level and type
	auto spells{_spells | std::views::filter([&](Spell spell) {
					return (spell.type == spell_type) &&
						   (spell.level == spell_level) &&
						   (spell.known == false);
				})};
	for (auto &spell : spells) {

		const auto dice{(*_system->random)[Random::ZERO_TO_29]};

		// Check the Spell Type against the relevant stat (see
		// SPLPERLV//TRYLEARN)
		if (spell_type == SpellType::PRIEST) {
			if (dice <=
				static_cast<unsigned int>(_cur_attr[Attribute::PIETY])) {
				spell.known = true;
				_spells_known[spell.id] = true;
				new_spell_learnt = true;
			}
		}
		if (spell_type == SpellType::MAGE) {
			if (dice <= static_cast<unsigned int>(_cur_attr[Attribute::IQ])) {
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
		spell_type == Enums::Magic::SpellType::PRIEST ? &_priest_max_sp
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

	using Enums::Character::Ability;
	using Enums::Character::Class;
	using Enums::Magic::SpellID;
	using Enums::Magic::SpellType;
	using Enums::System::Random;

	bool new_spells_learnt{false};

	for (auto spell_level = 1; spell_level <= 7; spell_level++) {
		_priest_max_sp[spell_level] = 0;
		_mage_max_sp[spell_level] = 0;
	}

	// Generate spell points according to current level and class (this does not
	// change any spells known but will reset spell points)
	switch (_class) { // NOLINT(clang-diagnostic-switch)
	case Class::FIGHTER:
	case Class::THIEF:
	case Class::NINJA:
		break;
	case Class::MAGE:
		_calculate_sp(SpellType::MAGE, 0, 2);
		break;
	case Class::PRIEST:
		_calculate_sp(SpellType::PRIEST, 0, 2);
		break;
	case Class::BISHOP:
		_calculate_sp(SpellType::PRIEST, 3, 4);
		_calculate_sp(SpellType::MAGE, 0, 4);
		break;
	case Class::SAMURAI:
		_calculate_sp(SpellType::MAGE, 3, 3);
		break;
	case Class::LORD:
		_calculate_sp(SpellType::PRIEST, 3, 2);
		break;
	default:
		break;
	}

	// Now try and learn any additional spells
	for (auto spell_level = 1; spell_level <= 7; spell_level++) {

		// If we know at least one spell in this level, we can always try and
		// learn more no matter what even if we are currently a non-spellcasting
		// class
		if (_priest_max_sp[spell_level] > 0) {
			if (_try_learn_spell(SpellType::PRIEST, spell_level))
				new_spells_learnt = true;
		}
		if (_mage_max_sp[spell_level] > 0) {
			if (_try_learn_spell(SpellType::MAGE, spell_level))
				new_spells_learnt = true;
		}
	}

	// Now make sure that if the above fails, we always learn a spell of each
	// circle just in case (GETMAGSP/GETPRISP - though note that the orignal is
	// bugged and selects the wrong level spells sometimes) - note this is for
	// creatures so it might not be applicable?
	for (auto spell_level = 1; spell_level <= 7; spell_level++) {

		if (_priest_max_sp[spell_level] > 0 &&
			_get_spells_known(SpellType::PRIEST, spell_level) == 0) {
			switch (spell_level) {
			case 1:
				_learn_spell(SpellID::BADIOS);
				break;
			case 2:
				_learn_spell(SpellID::MONTINO);
				new_spells_learnt = true;
				break;
			case 3:
				if ((*_system->random)[Random::D100] > 33)
					_learn_spell(SpellID::DIALKO);
				else
					_learn_spell(SpellID::LOMILWA);
				new_spells_learnt = true;
				break;
			case 4:
				_learn_spell(SpellID::BADIAL);
				new_spells_learnt = true;
				break;
			case 5:
				if ((*_system->random)[Random::D100] > 33)
					_learn_spell(SpellID::BADIALMA);
				else
					_learn_spell(SpellID::BADI);
				new_spells_learnt = true;
				break;
			case 6:
				if ((*_system->random)[Random::D100] > 33)
					_learn_spell(SpellID::LORTO);
				else
					_learn_spell(SpellID::MABADI);
				new_spells_learnt = true;
				break;
			case 7:
				_learn_spell(SpellID::MALIKTO);
				new_spells_learnt = true;
				break;
			default:
				break;
			}
		}
		if (_mage_max_sp[spell_level] > 0 &&
			_get_spells_known(SpellType::MAGE, spell_level) == 0) {
			switch (spell_level) {
			case 1:
				if ((*_system->random)[Random::D100] > 33)
					_learn_spell(SpellID::KATINO);
				else
					_learn_spell(SpellID::HALITO);
				new_spells_learnt = true;
				break;
			case 2:
				if ((*_system->random)[Random::D100] > 33)
					_learn_spell(SpellID::DILTO);
				else
					_learn_spell(SpellID::SOPIC);
				new_spells_learnt = true;
				break;
			case 3:
				if ((*_system->random)[Random::D100] > 33)
					_learn_spell(SpellID::MOLITO);
				else
					_learn_spell(SpellID::MAHALITO);
				new_spells_learnt = true;
				break;
			case 4:
				if ((*_system->random)[Random::D100] > 33)
					_learn_spell(SpellID::DALTO);
				else
					_learn_spell(SpellID::LAHALITO);
				new_spells_learnt = true;
				break;
			case 5:
				if ((*_system->random)[Random::D100] > 33)
					_learn_spell(SpellID::MAMORLIS);
				else
					_learn_spell(SpellID::MADALTO);
				new_spells_learnt = true;
				break;
			case 6:
				if ((*_system->random)[Random::D100] > 33)
					_learn_spell(SpellID::LAKANITO);
				else
					_learn_spell(SpellID::ZILWAN);
				new_spells_learnt = true;
				break;
			case 7:
				_learn_spell(SpellID::MALOR);
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

		if ((spell_level * 2) - 1 > _abilities.at(Ability::MAX_LEVEL))
			continue;

		const auto priest_known{
			_get_spells_known(SpellType::PRIEST, spell_level)};
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

	using Enums::Character::Attribute;
	using Enums::Character::Class;
	using Enums::Character::Race;
	using Enums::System::Random;

	_class = cclass;
	_race = static_cast<Race>((*_system->random)[Random::D5]);
	_alignment = alignment;

	switch (_race) { // NOLINT(clang-diagnostic-switch)
	case Race::HUMAN:
		_start_attr = {{Attribute::STRENGTH, 8}, {Attribute::IQ, 5},
					   {Attribute::PIETY, 5},	 {Attribute::VITALITY, 8},
					   {Attribute::AGILITY, 8},	 {Attribute::LUCK, 9}};
		break;
	case Race::ELF:
		_start_attr = {{Attribute::STRENGTH, 7}, {Attribute::IQ, 10},
					   {Attribute::PIETY, 10},	 {Attribute::VITALITY, 6},
					   {Attribute::AGILITY, 9},	 {Attribute::LUCK, 6}};
		break;
	case Race::DWARF:
		_start_attr = {{Attribute::STRENGTH, 10}, {Attribute::IQ, 7},
					   {Attribute::PIETY, 10},	  {Attribute::VITALITY, 10},
					   {Attribute::AGILITY, 5},	  {Attribute::LUCK, 6}};
		break;
	case Race::GNOME:
		_start_attr = {{Attribute::STRENGTH, 7}, {Attribute::IQ, 7},
					   {Attribute::PIETY, 10},	 {Attribute::VITALITY, 8},
					   {Attribute::AGILITY, 10}, {Attribute::LUCK, 7}};
		break;
	case Race::HOBBIT:
		_start_attr = {{Attribute::STRENGTH, 5}, {Attribute::IQ, 7},
					   {Attribute::PIETY, 7},	 {Attribute::VITALITY, 6},
					   {Attribute::AGILITY, 10}, {Attribute::LUCK, 12}};
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
	case Class::FIGHTER:
		[[fallthrough]];
	case Class::LORD:
		[[fallthrough]];
	case Class::SAMURAI:
		_points_left -= (15 - _start_attr[Attribute::STRENGTH]);
		_start_attr[Attribute::STRENGTH] = 15;
		break;
	case Class::MAGE:
		[[fallthrough]];
	case Class::BISHOP:
		_points_left -= (15 - _start_attr[Attribute::IQ]);
		_start_attr[Attribute::IQ] = 15;
		break;
	case Class::PRIEST:
		_points_left -= (15 - _start_attr[Attribute::PIETY]);
		_start_attr[Attribute::PIETY] = 15;
		break;
	case Class::THIEF:
		[[fallthrough]];
	case Class::NINJA:
		_points_left -= (15 - _start_attr[Attribute::AGILITY]);
		_start_attr[Attribute::AGILITY] = 15;
		break;
	default:
		break;
	}

	// Pump any points left into the Vitality attribute
	if (_points_left > 0)
		_start_attr[Attribute::VITALITY] += _points_left;

	_cur_attr = _start_attr;

	_name = _system->random->get_random_name();
	_portrait_index = (*_system->random)[Random::ZERO_TO_29];
}

// Enter Name and Portrait, rest is random
auto Sorcery::Character::create_quick() -> void {

	using Enums::Character::Align;
	using Enums::Character::Attribute;
	using Enums::Character::Class;
	using Enums::Character::Race;
	using Enums::System::Random;

	// Exclude Samurai/Lord/Ninja/Bishop from this method of character creation
	_class = static_cast<Class>((*_system->random)[Random::D4]);
	_race = static_cast<Race>((*_system->random)[Random::D5]);
	switch (_class) { // NOLINT(clang-diagnostic-switch)
	case Class::FIGHTER:
	case Class::MAGE:
		_alignment = static_cast<Align>((*_system->random)[Random::D3]);
		break;
	case Class::PRIEST:
		_alignment =
			(*_system->random)[Random::D2] == 1 ? Align::GOOD : Align::EVIL;
		break;
	case Class::THIEF:
		_alignment =
			(*_system->random)[Random::D2] == 1 ? Align::NEUTRAL : Align::EVIL;
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
	case Race::HUMAN:
		_start_attr = {{Attribute::STRENGTH, 8}, {Attribute::IQ, 5},
					   {Attribute::PIETY, 5},	 {Attribute::VITALITY, 8},
					   {Attribute::AGILITY, 8},	 {Attribute::LUCK, 9}};
		break;
	case Race::ELF:
		_start_attr = {{Attribute::STRENGTH, 7}, {Attribute::IQ, 10},
					   {Attribute::PIETY, 10},	 {Attribute::VITALITY, 6},
					   {Attribute::AGILITY, 9},	 {Attribute::LUCK, 6}};
		break;
	case Race::DWARF:
		_start_attr = {{Attribute::STRENGTH, 10}, {Attribute::IQ, 7},
					   {Attribute::PIETY, 10},	  {Attribute::VITALITY, 10},
					   {Attribute::AGILITY, 5},	  {Attribute::LUCK, 6}};
		break;
	case Race::GNOME:
		_start_attr = {{Attribute::STRENGTH, 7}, {Attribute::IQ, 7},
					   {Attribute::PIETY, 10},	 {Attribute::VITALITY, 8},
					   {Attribute::AGILITY, 10}, {Attribute::LUCK, 7}};
		break;
	case Race::HOBBIT:
		_start_attr = {{Attribute::STRENGTH, 5}, {Attribute::IQ, 7},
					   {Attribute::PIETY, 7},	 {Attribute::VITALITY, 6},
					   {Attribute::AGILITY, 10}, {Attribute::LUCK, 12}};
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
	case Class::FIGHTER:
		_points_left -= (15 - _start_attr[Attribute::STRENGTH]);
		_start_attr[Attribute::STRENGTH] = 15;
		break;
	case Class::MAGE:
		_points_left -= (15 - _start_attr[Attribute::IQ]);
		_start_attr[Attribute::IQ] = 15;
		break;
	case Class::PRIEST:
		_points_left -= (15 - _start_attr[Attribute::PIETY]);
		_start_attr[Attribute::PIETY] = 15;
		break;
	case Class::THIEF:
		_points_left -= (15 - _start_attr[Attribute::AGILITY]);
		_start_attr[Attribute::AGILITY] = 15;
		break;
	default:
		break;
	}

	// Pump any points left into the Vitality attribute
	if (_points_left > 0)
		_start_attr[Attribute::VITALITY] += _points_left;

	_cur_attr = _start_attr;
}

// Create a (semi) random character
auto Sorcery::Character::create_random() -> void {

	// Random Name and Portrait
	create_quick();
	_name = _system->random->get_random_name();
	_portrait_index = (*_system->random)[Enums::System::Random::ZERO_TO_29];
}

auto Sorcery::Character::get_status() const -> Enums::Character::CStatus {

	return _status;
}

auto Sorcery::Character::get_condition() const -> std::string {

	return _get_condition();
}

auto Sorcery::Character::get_short_cond() const -> std::string {

	if (_status != Enums::Character::CStatus::OK)
		return fmt::format("{:>6}", _get_condition());
	else
		return fmt::format("{:>4}",
						   _abilities.at(Enums::Character::Ability::MAX_HP));
}

auto Sorcery::Character::_get_condition() const -> std::string {

	using Enums::Character::CStatus;

	if (is_poisoned() && (_status == CStatus::OK)) {
		return (*_system->strings)["STATUS_POISONED"];
	} else if (_status == CStatus::OK)
		return (*_system->strings)["STATUS_OK"];
	else {
		switch (_status) {
		case CStatus::AFRAID:
			return (*_system->strings)["STATUS_AFRAID"];
			break;
		case CStatus::ASHES:
			return (*_system->strings)["STATUS_ASHES"];
			break;
		case CStatus::ASLEEP:
			return (*_system->strings)["STATUS_ASLEEP"];
			break;
		case CStatus::DEAD:
			return (*_system->strings)["STATUS_DEAD"];
			break;
		case CStatus::LOST:
			return (*_system->strings)["STATUS_LOST"];
			break;
		case CStatus::HELD:
			return (*_system->strings)["STATUS_PARALYSED"];
			break;
		case CStatus::SILENCED:
			return (*_system->strings)["STATUS_SILENCED"];
			break;
		case CStatus::STONED:
			return (*_system->strings)["STATUS_STONED"];
			break;
		default:
			return "";
		}
	}
}

auto Sorcery::Character::get_loc_str() const -> std::string {

	using Enums::Character::Location;

	switch (_location) {
	case Location::PARTY:
		return (*_system->strings)["LOCATION_PARTY"];
		break;
	case Location::TAVERN:
		return (*_system->strings)["LOCATION_TAVERN"];
		break;
	case Location::TRAINING:
		return (*_system->strings)["LOCATION_TRAINING"];
		break;
	case Location::TEMPLE:
		return (*_system->strings)["LOCATION_TEMPLE"];
		break;
	case Location::MAZE:
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

auto Sorcery::Character::set_status(Enums::Character::CStatus value) -> void {

	if (value == Enums::Character::CStatus::OK)
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

	using Enums::Character::Ability;

	return _abilities.at(Ability::HP_GAIN_PER_TURN) -
		   _abilities.at(Ability::HP_LOSS_PER_TURN) -
		   _abilities.at(Ability::POISON_STRENGTH);
}

auto Sorcery::Character::get_hp_adjustment_symbol() const -> char {

	using Enums::Character::Ability;

	const auto rate{(_abilities.at(Ability::HP_GAIN_PER_TURN) -
					 _abilities.at(Ability::HP_LOSS_PER_TURN) -
					 _abilities.at(Ability::POISON_STRENGTH)) <=> 0};
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

	_abilities.at(Enums::Character::Ability::HP_GAIN_PER_TURN) = 0;
	_abilities.at(Enums::Character::Ability::HP_LOSS_PER_TURN) = 0;
}

auto Sorcery::Character::set_poisoned_rate(int value) -> void {

	using Enums::Character::Ability;

	if (value > _abilities.at(Ability::POISON_STRENGTH)) {
		_abilities.at(Ability::POISON_STRENGTH) = value;
	} else if (value == 0) {
		_abilities.at(Ability::POISON_STRENGTH) = 0;
	}
}

auto Sorcery::Character::get_poisoned_string() const -> std::string {

	return _abilities.at(Enums::Character::Ability::POISON_STRENGTH) > 0
			   ? fmt::format(
					 "{:->2}",
					 _abilities.at(Enums::Character::Ability::POISON_STRENGTH))
			   : "";
}

auto Sorcery::Character::get_short_hp_summary() const -> std::string {

	return std::to_string(_abilities.at(Enums::Character::Ability::CURRENT_HP));
}

auto Sorcery::Character::get_hp_summary() const -> std::string {

	return fmt::format(
		"{}/{}{}",
		std::to_string(_abilities.at(Enums::Character::Ability::CURRENT_HP)),
		std::to_string(_abilities.at(Enums::Character::Ability::MAX_HP)),
		get_hp_adjustment_symbol());
}

auto Sorcery::Character::get_spell_points(
	const Enums::Magic::SpellType type,
	const Enums::Magic::SpellPointType status) const
	-> std::optional<std::map<unsigned int, unsigned int>> {

	using Enums::Magic::SpellPointType;
	using Enums::Magic::SpellType;

	if (type == SpellType::MAGE && status == SpellPointType::CURRENT)
		return _mage_cur_sp;
	else if (type == SpellType::MAGE && status == SpellPointType::MAXIMUM)
		return _mage_max_sp;
	else if (type == SpellType::PRIEST && status == SpellPointType::CURRENT)
		return _priest_cur_sp;
	else if (type == SpellType::PRIEST && status == SpellPointType::MAXIMUM)
		return _priest_max_sp;
	else
		return std::nullopt;
}

auto Sorcery::Character::_get_sp_per_level(const Enums::Magic::SpellType type,
										   int level) -> std::string {

	if (type == Enums::Magic::SpellType::MAGE)
		return fmt::format("{}/{}", std::to_string(_mage_cur_sp[level]),
						   std::to_string(_mage_max_sp[level]));
	else
		return fmt::format("{}/{}", std::to_string(_priest_cur_sp[level]),
						   std::to_string(_priest_max_sp[level]));
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

// For level draining, optionally keep a track of negative levels unless in
// strict mode Need to also handle character class switching

auto Sorcery::Character::get_summary() -> std::string {

	auto name{_name};
	// if (_display->get_upper())
	//	std::ranges::transform(name.begin(), name.end(), name.begin(),
	//						   ::toupper);
	return fmt::format("{:<15} L {:>2} {}-{} {}", name,
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
		else if (_status == Enums::Character::CStatus::LOST)
			return " LOST";
		else
			return "    ";
	})};

	return fmt::format("{:<15} L {:>2} {}-{} {}{:>5}", name,
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

	using Enums::Character::CStatus;

	auto cost_per_level{0u};
	switch (_status) {
	case CStatus::ASHES:
		cost_per_level = 500;
		break;
	case CStatus::DEAD:
		cost_per_level = 250;
		break;
	case CStatus::HELD:
		cost_per_level = 100;
		break;
	case CStatus::STONED:
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
	return fmt::format(
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

	using Enums::Character::Stage;

	auto name{_name};
	// if (_display->get_upper())
	//	std::ranges::transform(name.begin(), name.end(), name.begin(),
	//						   ::toupper);
	auto legacy{_legated ? " (D)" : ""};
	const auto level{_abilities.at(Enums::Character::Ability::CURRENT_LEVEL)};
	switch (_current_stage) {
	case Stage::CHOOSE_METHOD:
		[[fallthrough]];
	case Stage::ENTER_NAME:
		return fmt::format("{:<15} L ?? ?-??? ???", "???");
		break;
	case Stage::CHOOSE_RACE:
		return fmt::format("{:<15} L {:>2} ?-??? ???", name, level);
		break;
	case Stage::CHOOSE_ALIGNMENT:
		return fmt::format("{:<15} L {:>2} ?-??? {}", name, level,
						   race_to_str(_race));
		break;
	case Stage::ALLOCATE_STATS:
		return fmt::format("{:<15} L {:>2} {}-??? {}", name, level,
						   alignment_to_str(_alignment).substr(0, 1),
						   race_to_str(_race));
		break;
	case Stage::CHOOSE_CLASS:
		return fmt::format("{:<15} L {:>2} {}-??? {}", name, level,
						   alignment_to_str(_alignment).substr(0, 1),
						   race_to_str(_race));
		break;
	case Stage::CHOOSE_PORTRAIT:
		return fmt::format("{:<15} L {:>2} {}-{} {}", name, level,
						   alignment_to_str(_alignment).substr(0, 1),
						   class_to_str(_class).substr(0, 3),
						   race_to_str(_race));
		break;
	case Stage::REVIEW_AND_CONFIRM:
		[[fallthrough]];
	case Stage::COMPLETED:
		return fmt::format("{} L {:>2} {}-{} {}{}", name, level,
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

	using Enums::Character::Ability;

	_abilities[Ability::CURRENT_HP] =
		_abilities[Ability::CURRENT_HP] - adjustment;
	if (_abilities[Ability::CURRENT_HP] < 0) {
		_abilities[Ability::CURRENT_HP] = 0;
		_status = Enums::Character::CStatus::DEAD;
		return false;
	} else
		return true;
}

auto Sorcery::Character::_heal(const unsigned int adjustment) -> void {

	using Enums::Character::Ability;

	_abilities[Ability::CURRENT_HP] =
		_abilities[Ability::CURRENT_HP] + adjustment;

	if (_abilities[Ability::CURRENT_HP] > _abilities[Ability::MAX_HP])
		_abilities[Ability::CURRENT_HP] = _abilities[Ability::MAX_HP];
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

	auto body{fmt::format(
		"{:<15} {:>2} {}-{} {:>3} {:>6} {:^10}", name, character.get_level(),
		character.alignment_to_str(alignment).substr(0, 1),
		character.class_to_str(cclass).substr(0, 3), character.get_cur_ac(),
		character.get_hp_summary(), character.get_condition())};

	return out_stream << body << std::endl;
}
} // namespace Sorcery
