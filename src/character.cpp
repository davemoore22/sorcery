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

Sorcery::Character::Character() {}

// Standard Constructor
Sorcery::Character::Character(System *system, Display *display, Graphics *graphics)
	: _system{system}, _display{display}, _graphics{graphics} {

	set_stage(CharacterStage::CHOOSE_METHOD);
	_texts.clear();
	_sprites.clear();
	_frames.clear();
	_v_texts.clear();
	_v_sprites.clear();
	_v_frames.clear();
	_view = CharacterView::NONE;
}

// Note for the copy constuctors we only copy the character data/PODs within
Sorcery::Character::Character(const Character &other)
	: _system{other._system}, _display{other._display}, _graphics{other._graphics},
	  _abilities{other._abilities}, _priest_max_sp{other._priest_max_sp},
	  _priest_cur_sp{other._priest_cur_sp}, _mage_max_sp{other._mage_max_sp},
	  _mage_cur_sp{other._mage_cur_sp}, _spells{other._spells}, _spells_known{other._spells_known},
	  _current_stage{other._current_stage}, _name{other._name}, _race{other._race},
	  _class{other._class}, _alignment{other._alignment}, _start_attr{other._start_attr},
	  _cur_attr{other._cur_attr}, _max_attr{other._max_attr}, _view{other._view},
	  _points_left{other._points_left}, _st_points{other._st_points},
	  _pos_classes{other._pos_classes}, _class_list{other._class_list},
	  _num_pos_classes{other._num_pos_classes}, _portrait_index{other._portrait_index} {

	_sprites = other._sprites;
	_texts = other._texts;
	_frames = other._frames;
	_v_sprites = other._v_sprites;
	_v_texts = other._v_texts;
	_v_frames = other._v_frames;
	_ad = other._ad;
	_ss = other._ss;
	_ad_c = other._ad_c;
	_ss_c = other._ss_c;
}

auto Sorcery::Character::operator=(const Character &other) -> Character & {

	_system = other._system;
	_display = other._display;
	_graphics = other._graphics;

	_abilities = other._abilities;
	_priest_max_sp = other._priest_max_sp;
	_priest_cur_sp = other._priest_cur_sp;
	_mage_max_sp = other._mage_max_sp;
	_mage_cur_sp = other._mage_cur_sp;
	_spells = other._spells;
	_spells_known = other._spells_known;
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

	_sprites = other._sprites;
	_texts = other._texts;
	_frames = other._frames;
	_v_sprites = other._v_sprites;
	_v_texts = other._v_texts;
	_v_frames = other._v_frames;
	_ad = other._ad;
	_ad_c = other._ad_c;
	_ss = other._ss;
	_ss_c = other._ss_c;

	return *this;
}

// Move Constructors
Sorcery::Character::Character(Character &&other) noexcept {

	if (this != &other) {
		_system = other._system;
		_display = other._display;
		_graphics = other._graphics;

		_abilities = other._abilities;
		_priest_max_sp = other._priest_max_sp;
		_priest_cur_sp = other._priest_cur_sp;
		_mage_max_sp = other._mage_max_sp;
		_mage_cur_sp = other._mage_cur_sp;
		_spells = other._spells;
		_spells_known = other._spells_known;
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

		_sprites = std::move(other._sprites);
		_texts = std::move(other._texts);
		_frames = std::move(other._frames);
		_v_sprites = std::move(other._v_sprites);
		_v_texts = std::move(other._v_texts);
		_v_frames = std::move(other._v_frames);
		_ad = std::move(other._ad);
		_ad_c = std::move(other._ad_c);
		_ss = std::move(other._ss);
		_ss_c = std::move(other._ss_c);

		other._system = nullptr;
		other._display = nullptr;
		other._graphics = nullptr;

		other._abilities.clear();
		other._priest_max_sp.clear();
		other._priest_cur_sp.clear();
		other._mage_max_sp.clear();
		other._mage_cur_sp.clear();
		other._spells.clear();
		other._spells_known.clear();
		other._current_stage = CharacterStage::NONE;
		other._name.clear();
		other._race = CharacterRace::NONE;
		other._class = CharacterClass::NONE;
		other._alignment = CharacterAlignment::NONE;
		other._start_attr.clear();
		other._cur_attr.clear();
		other._max_attr.clear();
		other._view = CharacterView::NONE;
		other._points_left = 0;
		other._st_points = 0;
		other._pos_classes.clear();
		other._class_list.clear();
		other._num_pos_classes = 0;
		other._portrait_index = 0;

		other._sprites.clear();
		other._texts.clear();
		other._frames.clear();
		other._v_sprites.clear();
		other._v_texts.clear();
		other._v_frames.clear();
		other._ad_c = Component();
		other._ss_c = Component();
	}
}

auto Sorcery::Character::operator=(Character &&other) noexcept -> Character & {

	if (this != &other) {
		_system = other._system;
		_display = other._display;
		_graphics = other._graphics;

		_abilities = other._abilities;
		_priest_max_sp = other._priest_max_sp;
		_priest_cur_sp = other._priest_cur_sp;
		_mage_max_sp = other._mage_max_sp;
		_mage_cur_sp = other._mage_cur_sp;
		_spells = other._spells;
		_spells_known = other._spells_known;
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

		_sprites = std::move(other._sprites);
		_texts = std::move(other._texts);
		_frames = std::move(other._frames);
		_v_sprites = std::move(other._v_sprites);
		_v_texts = std::move(other._v_texts);
		_v_frames = std::move(other._v_frames);
		_ad = std::move(other._ad);
		_ad_c = std::move(other._ad_c);
		_ss = std::move(other._ss);
		_ss_c = std::move(other._ss_c);

		other._system = nullptr;
		other._display = nullptr;
		other._graphics = nullptr;

		other._abilities.clear();
		other._priest_max_sp.clear();
		other._priest_cur_sp.clear();
		other._mage_max_sp.clear();
		other._mage_cur_sp.clear();
		other._spells.clear();
		other._spells_known.clear();
		other._current_stage = CharacterStage::NONE;
		other._name.clear();
		other._race = CharacterRace::NONE;
		other._class = CharacterClass::NONE;
		other._alignment = CharacterAlignment::NONE;
		other._start_attr.clear();
		other._cur_attr.clear();
		other._max_attr.clear();
		other._view = CharacterView::NONE;
		other._points_left = 0;
		other._st_points = 0;
		other._pos_classes.clear();
		other._class_list.clear();
		other._num_pos_classes = 0;
		other._portrait_index = 0;

		other._sprites.clear();
		other._texts.clear();
		other._frames.clear();
		other._v_sprites.clear();
		other._v_texts.clear();
		other._v_frames.clear();
		other._ad_c = Component();
		other._ss_c = Component();
	}
	return *this;
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

	// Also need methods to boost to next stage from start, e.g. random name or race, and going into
	// create immediately sets that

	// As well as a summary display of character (to be displayed where?)
	switch (stage) {
	case CharacterStage::CHOOSE_METHOD:
		_name.clear();
		_race = CharacterRace::NONE;
		_alignment = CharacterAlignment::NONE;
		_start_attr.clear();
		_cur_attr.clear();
		_max_attr.clear();
		//_status.clear();
		_abilities.clear();

		// Used in the display from this point onwards
		_abilities[CharacterAbility::CURRENT_LEVEL] = 1;
		_class = CharacterClass::NONE;
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
		create_spell_lists();
		reset_spells();
		_view = CharacterView::NONE;
		break;
	case CharacterStage::REVIEW_AND_CONFIRM:

		// Handle the generation of the Character Display Here
		_view = CharacterView::SUMMARY;
		_generate_display();
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

auto Sorcery::Character::get_level() const -> int {

	return _abilities.at(CharacterAbility::CURRENT_LEVEL);
}

auto Sorcery::Character::set_level(const int &value) -> void {

	_abilities.at(CharacterAbility::CURRENT_LEVEL) = value;
}

auto Sorcery::Character::get_alignment() const -> CharacterAlignment {

	return _alignment;
}
auto Sorcery::Character::set_alignment(const CharacterAlignment &value) -> void {

	_alignment = value;
}

auto Sorcery::Character::get_number_possible_classes() const -> unsigned int {
	return _num_pos_classes;
}

auto Sorcery::Character::get_class() const -> CharacterClass {

	return _class;
}
auto Sorcery::Character::set_class(const CharacterClass &value) -> void {

	_class = value;
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

auto Sorcery::Character::get_attribute(const CharacterAttribute attribute) const -> unsigned int {

	return _cur_attr.at(attribute);
}

auto Sorcery::Character::get_starting_attribute(const CharacterAttribute attribute) const
	-> unsigned int {

	return _start_attr.at(attribute);
}

auto Sorcery::Character::set_attribute(const CharacterAttribute attribute, const int adjustment)
	-> void {

	_cur_attr.at(attribute) += adjustment;
}

auto Sorcery::Character::get_portrait_index() const -> unsigned int {

	return _portrait_index;
}

auto Sorcery::Character::set_portrait_index(const unsigned int value) -> void {

	_portrait_index = value;
}

// Setting the view will regenerate the display components
auto Sorcery::Character::left_view() -> void {

	int view_index = magic_enum::enum_integer<CharacterView>(_view);
	if (view_index == magic_enum::enum_integer<CharacterView>(CharacterView::SUMMARY))
		view_index = magic_enum::enum_integer<CharacterView>(CharacterView::INVENTORY);
	else
		--view_index;
	_view = magic_enum::enum_cast<CharacterView>(view_index).value();

	_generate_display();
}
// Setting the view will regenerate the display components
auto Sorcery::Character::right_view() -> void {

	int view_index = magic_enum::enum_integer<CharacterView>(_view);
	if (view_index == magic_enum::enum_integer<CharacterView>(CharacterView::INVENTORY))
		view_index = magic_enum::enum_integer<CharacterView>(CharacterView::SUMMARY);
	else
		++view_index;
	_view = magic_enum::enum_cast<CharacterView>(view_index).value();

	_generate_display();
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
	_points_left = {(*_system->random)[RandomType::ZERO_TO_3]};
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

auto Sorcery::Character::get_current_attributes() const -> CharacterAttributes {

	return _cur_attr;
}

auto Sorcery::Character::get_starting_attributes() const -> CharacterAttributes {

	return _start_attr;
}

auto Sorcery::Character::get_possible_classes() const -> CharacterClassQualified {

	return _pos_classes;
}

auto Sorcery::Character::get_icon(CharacterStage type) -> std::optional<sf::Sprite> {

	switch (type) {
	case CharacterStage::CHOOSE_ALIGNMENT: {
		std::string alignment{get_alignment(_alignment)};
		std::transform(alignment.begin(), alignment.end(), alignment.begin(), ::tolower);
		return (*_graphics->icons)[alignment].value();
	} break;
	case CharacterStage::CHOOSE_RACE: {
		std::string race{get_race(_race)};
		std::transform(race.begin(), race.end(), race.begin(), ::tolower);
		return (*_graphics->icons)[race].value();
	} break;
	case CharacterStage::CHOOSE_CLASS: {
		std::string cclass{get_class(_class)};
		std::transform(cclass.begin(), cclass.end(), cclass.begin(), ::tolower);
		return (*_graphics->icons)[cclass].value();
	} break;
	default:
		break;
	}

	return std::nullopt;
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
		return (*_display->string)["CHARACTER_ALIGNMENT_GOOD"];
		break;
	case CharacterAlignment::NEUTRAL:
		return (*_display->string)["CHARACTER_ALIGNMENT_NEUTRAL"];
		break;
	case CharacterAlignment::EVIL:
		return (*_display->string)["CHARACTER_ALIGNMENT_EVIL"];
		break;
	default:
		return "";
		break;
	}
}

auto Sorcery::Character::get_race(CharacterRace character_race) const -> std::string {

	switch (character_race) {
	case CharacterRace::HUMAN:
		return (*_display->string)["CHARACTER_RACE_HUMAN"];
		break;
	case CharacterRace::ELF:
		return (*_display->string)["CHARACTER_RACE_ELF"];
		break;
	case CharacterRace::DWARF:
		return (*_display->string)["CHARACTER_RACE_DWARF"];
		break;
	case CharacterRace::GNOME:
		return (*_display->string)["CHARACTER_RACE_GNOME"];
		break;
	case CharacterRace::HOBBIT:
		return (*_display->string)["CHARACTER_RACE_HOBBIT"];
		break;
	default:
		return "";
		break;
	}
}

auto Sorcery::Character::get_class(CharacterClass character_class) const -> std::string {

	switch (character_class) {
	case CharacterClass::FIGHTER:
		return (*_display->string)["CHARACTER_CLASS_FIGHTER"];
		break;
	case CharacterClass::MAGE:
		return (*_display->string)["CHARACTER_CLASS_MAGE"];
		break;
	case CharacterClass::PRIEST:
		return (*_display->string)["CHARACTER_CLASS_PRIEST"];
		break;
	case CharacterClass::THIEF:
		return (*_display->string)["CHARACTER_CLASS_THIEF"];
		break;
	case CharacterClass::BISHOP:
		return (*_display->string)["CHARACTER_CLASS_BISHOP"];
		break;
	case CharacterClass::SAMURAI:
		return (*_display->string)["CHARACTER_CLASS_SAMURAI"];
		break;
	case CharacterClass::LORD:
		return (*_display->string)["CHARACTER_CLASS_LORD"];
		break;
	case CharacterClass::NINJA:
		return (*_display->string)["CHARACTER_CLASS_NINJA"];
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

	// _abilities[CharacterAbility::CURRENT_LEVEL] = 1;
	_abilities[CharacterAbility::MAX_LEVEL] = 1;
	_abilities[CharacterAbility::NEGATIVE_LEVEL] = 0;
	_abilities[CharacterAbility::HIT_DICE] = 1;
	_abilities[CharacterAbility::GOLD] = (*_system->random)[RandomType::ZERO_TO_99] + 90;
	_abilities[CharacterAbility::AGE] = (18 * 52) + (*_system->random)[RandomType::ZERO_TO_299];
	_abilities[CharacterAbility::SWIM] = 1;
	_abilities[CharacterAbility::MARKS] = 0;
	_abilities[CharacterAbility::DEATHS] = 0;

	_start_attr = _cur_attr;
	_max_attr = _cur_attr;

	_abilities[CharacterAbility::CURRENT_XP] = 0;
	_abilities[CharacterAbility::NEXT_LEVEL_XP] =
		_get_xp_for_level(_abilities[CharacterAbility::CURRENT_LEVEL]);
}

// Given the characters current level, work out all the secondary abilities/stats etc
auto Sorcery::Character::_generate_secondary_abilities() -> void {

	// Formulae used are from here http://www.zimlab.com/wizardry/walk/w123calc.htm and also
	// from
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
	switch (unsigned int chance{(*_system->random)[RandomType::D100]};
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

	// Chance equipment is intact on a corpse TODO: check this is accurate
	_abilities[CharacterAbility::EQUIPMENT_INTACT_ON_WIPE] =
		(_cur_attr[CharacterAttribute::LUCK] / 21.0) * 100;

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

	_abilities[CharacterAbility::RECOVER_FROM_SLEEP] = current_level * 10;
	_abilities[CharacterAbility::RECOVER_FROM_FEAR] = current_level * 5;

	// If we are not in strict mode, add some bonus spellpoints to first level priests/,ages
	// depending on their associated stats
	_abilities[CharacterAbility::BONUS_MAGE_SPELLS] = 0;
	_abilities[CharacterAbility::BONUS_PRIEST_SPELLS] = 0;
	if (!(*_system->config)[ConfigOption::STRICT_MODE]) {
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

	// In the original code this is handled in "SETSPELS"/"SPLPERLV"/"NWMAGE"/"NWPRIEST" - but
	// we are just setting them straight for now but adding in extra slots for casters to make
	// things easier if we're not in strict mode
	switch (_class) { // NOLINT(clang-diagnostic-switch)
	case CharacterClass::PRIEST:
		_priest_max_sp[1] = (*_system->config)[ConfigOption::STRICT_MODE]
								? 2
								: 2 + _abilities[CharacterAbility::BONUS_PRIEST_SPELLS];
		break;
	case CharacterClass::BISHOP:
		_mage_max_sp[1] = 2;
		break;
	case CharacterClass::MAGE:
		_mage_max_sp[1] = (*_system->config)[ConfigOption::STRICT_MODE]
							  ? 2
							  : 2 + _abilities[CharacterAbility::BONUS_MAGE_SPELLS];
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

// Spells known aren't set on serialisation restore? are they written?

// Set starting spells
auto Sorcery::Character::_set_starting_spells() -> void {

	// This is taken from "KEEPCHYN" which hard codes the spells known to beginning characters!
	std::vector<Spell>::iterator it;
	switch (_class) { // NOLINT(clang-diagnostic-switch)
	case CharacterClass::BISHOP:
		it = std::find_if(_spells.begin(), _spells.end(), [&](auto item) {
			return item.id == SpellID::KATINO;
		});
		if (it != _spells.end()) {
			(*it).known = true;
			_spells_known[(*it).id] = true;
		}
		it = std::find_if(_spells.begin(), _spells.end(), [&](auto item) {
			return item.id == SpellID::HALITO;
		});
		if (it != _spells.end()) {
			(*it).known = true;
			_spells_known[(*it).id] = true;
		}
		break;
	case CharacterClass::MAGE:
		it = std::find_if(_spells.begin(), _spells.end(), [&](auto item) {
			return item.id == SpellID::DUMAPIC;
		});
		if (it != _spells.end()) {
			(*it).known = true;
			_spells_known[(*it).id] = true;
		}
		it = std::find_if(_spells.begin(), _spells.end(), [&](auto item) {
			return item.id == SpellID::MOGREF;
		});
		if (it != _spells.end()) {
			(*it).known = true;
			_spells_known[(*it).id] = true;
		}
		break;
	case CharacterClass::PRIEST:
		it = std::find_if(_spells.begin(), _spells.end(), [&](auto item) {
			return item.id == SpellID::DIOS;
		});
		if (it != _spells.end()) {
			(*it).known = true;
			_spells_known[(*it).id] = true;
		}
		it = std::find_if(_spells.begin(), _spells.end(), [&](auto item) {
			return item.id == SpellID::BADIOS;
		});
		if (it != _spells.end()) {
			(*it).known = true;
			_spells_known[(*it).id] = true;
		}
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
		extra_hp += (*_system->random)[RandomType::D10];
		break;
	case CharacterClass::PRIEST:
		extra_hp += (*_system->random)[RandomType::D8];
		break;
	case CharacterClass::THIEF:
	case CharacterClass::BISHOP:
	case CharacterClass::NINJA:
		extra_hp += (*_system->random)[RandomType::D6];
		break;
	case CharacterClass::MAGE:
		extra_hp += (*_system->random)[RandomType::D4];
		break;
	case CharacterClass::SAMURAI:
		extra_hp += (*_system->random)[RandomType::D8];
		extra_hp += (*_system->random)[RandomType::D8];
		break;
	default:
		break;
	}

	extra_hp += _abilities[CharacterAbility::BONUS_HIT_POINTS];
	if (extra_hp < 0)
		extra_hp = 1;

	// Though this could be an unsigned int as it will always be greater than 0, just return int
	// as everything else is
	return extra_hp;
}

// Add hit points on level gain (but note the strict mode limitation mentioned below)
auto Sorcery::Character::_update_hp_for_level() -> void {

	// Note the annoying thing in the original Wizardry ("MADELEV") and reproduced here in
	// strict mode where it recalculates all HP and thus often you end up with gaining only one
	// HP - this also reproduces the equally annoying thing where if you have changed class it
	// uses your *current* class level for recalculation, hence until you get back to where you
	// were before changing class you will probably only ever gain 1 hp each time unless the
	// random dice rolls are really in your favour!
	if ((*_system->config)[ConfigOption::REROLL_HIT_POINTS_ON_LEVEL_GAIN]) {
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
		if (_priest_max_sp[spell_level] == 0)
			return;
	if (spell_type == SpellType::MAGE)
		if (_mage_max_sp[spell_level] == 0)
			return;

	// First, get the spells themselves
	std::vector<Spell>::iterator it;
	it = std::find_if(_spells.begin(), _spells.end(), [&](auto item) {
		return item.type == spell_type && item.level == spell_level;
	});
	while (it != _spells.end()) {
		it = std::find_if(++it, _spells.end(), [&](auto item) {
			return item.type == spell_type && item.level == spell_level;
		});

		// If spell already known, skip
		if ((*it).known)
			continue;

		// Check the Spell Type against the relevant stat (see SPLPERLV//TRYLEARN)
		if (spell_type == SpellType::PRIEST)
			if ((*_system->random)[RandomType::ZERO_TO_29] <=
				static_cast<unsigned int>(_cur_attr[CharacterAttribute::PIETY])) {
				(*it).known = true;
				_spells_known[(*it).id] = true;
			}
		if (spell_type == SpellType::MAGE)
			if ((*_system->random)[RandomType::ZERO_TO_29] <=
				static_cast<unsigned int>(_cur_attr[CharacterAttribute::IQ])) {
				(*it).known = true;
				_spells_known[(*it).id] = true;
			}
	}
}

// Reimplementation of SPLPERLV
auto Sorcery::Character::_calculate_sp(
	SpellType spell_type, unsigned int level_mod, unsigned int level_offset) -> void {

	// No ownership granted by use of raw pointer here
	SpellPoints *spells{spell_type == SpellType::PRIEST ? &_priest_max_sp : &_mage_max_sp};

	int spell_count{static_cast<int>(_abilities[CharacterAbility::CURRENT_LEVEL] - level_mod)};
	if (spell_count <= 0)
		return;

	unsigned int spell_level{1};
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

	// First work out the number of spells known at each level (not this deliberately does not
	// alter spells learned in a previous class to allow those to remain the same (see
	// MINMAG/MINPRI in the code)
	for (auto spell_level = 1; spell_level <= 7; spell_level++) {
		_priest_max_sp[spell_level] = _get_spells_known(SpellType::PRIEST, spell_level);
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

	unsigned int spells_known{0};
	std::vector<Spell>::iterator it;
	it = std::find_if(_spells.begin(), _spells.end(), [&](auto item) {
		return item.type == spell_type && item.level == spell_level;
	});
	while (it != _spells.end()) {
		it = std::find_if(++it, _spells.end(), [=](auto item) {
			return item.type == spell_type && item.level == spell_level;
		});

		if ((*it).known)
			++spells_known;
	}

	return spells_known;
}

// Given a level, get the XP needed for it
auto Sorcery::Character::_get_xp_for_level(unsigned int level) const -> int {

	// Values obtained from http://www.the-spoiler.com/RPG/Sir-Tech/wizardry.1.2.html
	float base{};
	float coefficient_2_to_3{};
	float coefficient_3_to_13{};
	float coefficient_13_plus{};
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

auto Sorcery::Character::set_spells() -> void {

	// Now for each spell known set the appropriate entry in the spells table (TODO: this is
	// terribly inefficient)
	for (auto &spell_known : _spells_known) {

		std::vector<Spell>::iterator it;
		it = std::find_if(_spells.begin(), _spells.end(), [&](auto item) {
			return item.id == spell_known.first;
		});
		if (it != _spells.end())
			(*it).known = spell_known.second;
	}
}

// Note a few extra spells taken from Wizardry V, such as DESTO, CALIFIC, LITOFEIT, and LABADI
auto Sorcery::Character::create_spell_lists() -> void {
	_spells.clear();

	// Mage Spells (grouped by level)

	// Level 1
	unsigned int level{1};
	_spells.emplace_back(SpellID::DUMAPIC, SpellType::MAGE, SpellCategory::FIELD, level, false,
		"DUMAPIC", "Clarity",
		"Restablishes the party's bearings and shows their location in the maze.");
	_spells.emplace_back(SpellID::HALITO, SpellType::MAGE, SpellCategory::ATTACK, level, false,
		"HALITO", "Little Fire",
		"Flings a a ball of fire at one foe inflicting 1d8 points of fire damage.");
	_spells.emplace_back(SpellID::KATINO, SpellType::MAGE, SpellCategory::DISABLE, level, false,
		"KATINO", "Bad Air", "Temporarily puts to sleep one group of foes.");
	_spells.emplace_back(SpellID::MOGREF, SpellType::MAGE, SpellCategory::SUPPORT, level, false,
		"MOGREF", "Body Iron",
		"Grants a -2 bonus to armour class to the caster for the duration of combat.");

	// Level 2
	++level;
	_spells.emplace_back(SpellID::BOLATU, SpellType::MAGE, SpellCategory::DISABLE, level, false,
		"BOLATU", "Heart of Stone", "Attempts to turn one foe to stone.");
	_spells.emplace_back(SpellID::DESTO, SpellType::MAGE, SpellCategory::FIELD, level, false,
		"DESTO", "Unlock",
		"Attempts to magically unlock one door as if the caster were a thief of the same level.");
	_spells.emplace_back(SpellID::MELITO, SpellType::MAGE, SpellCategory::ATTACK, level, false,
		"MELITO", "Little Sparks", "Inflicts 1d8 points of electric damage to one group of foes.");
	_spells.emplace_back(SpellID::MORLIS, SpellType::MAGE, SpellCategory::DISABLE, level, false,
		"MORLIS", "Fear",
		"Causes one group of foes to fear the party, making them less effective in combat.");
	_spells.emplace_back(SpellID::PONTI, SpellType::MAGE, SpellCategory::SUPPORT, level, false,
		"PONTI", "Speed",
		"Grants a -1 bonus to armour class to a party member and improves their speed for the "
		"duration of combat.");

	// Level 3
	++level;
	_spells.emplace_back(SpellID::CALIFIC, SpellType::MAGE, SpellCategory::FIELD, level, false,
		"CALIFIC", "Reveal", "Reveals nearby secret doors.");
	_spells.emplace_back(SpellID::CORTU, SpellType::MAGE, SpellCategory::SUPPORT, level, false,
		"CORTU", "Magic Screen",
		"Erects a protective barrier around the party giving partial protection from breath "
		"attacks during combat. Stacks with multiple casts.");
	_spells.emplace_back(SpellID::KANTIOS, SpellType::MAGE, SpellCategory::DISABLE, level, false,
		"KANTIOS", "Discrption",
		"Disrupts the senses of a group of foes, preventing them from casting spells, using breath "
		"attacks and calling for help.");
	_spells.emplace_back(SpellID::MAHALITO, SpellType::MAGE, SpellCategory::ATTACK, level, false,
		"MAHALITO", "Big Fire",
		"Causes an explosion in a group of foes, inflicting 3d8 points of fire damage.");

	// Level 4
	++level;
	_spells.emplace_back(SpellID::LAHALITO, SpellType::MAGE, SpellCategory::ATTACK, level, false,
		"LAHALITO", "Torch", "Inflicts 6d6 points of fire damage to a group of foes.");
	_spells.emplace_back(SpellID::LITOFEIT, SpellType::MAGE, SpellCategory::FIELD, level, false,
		"LITOFEIT", "Levitate",
		"Causes the party to levitate, preventing the triggering of floor traps and greatly "
		"reduces the chances of ambushes.");
	_spells.emplace_back(SpellID::ROKDO, SpellType::MAGE, SpellCategory::DISABLE, level, false,
		"ROKDO", "Stun", "Stuns one group of foes, rendering them helpless in combat.");
	_spells.emplace_back(SpellID::TZALIK, SpellType::MAGE, SpellCategory::ATTACK, level, false,
		"TZALIK", "Fist of the Gods", "Inflicts 20d3 points of force damage on one foe.");

	// Level 5
	++level;
	_spells.emplace_back(SpellID::BACORTU, SpellType::MAGE, SpellCategory::DISABLE, level, false,
		"BACORTU", "Fizzle Field", "Erects a spell dampening field around a group of foes.");
	_spells.emplace_back(SpellID::MADALTO, SpellType::MAGE, SpellCategory::ATTACK, level, false,
		"MADALTO", "Frost", "Inflicts 8d8 points of cold damage to a group of foes.");
	_spells.emplace_back(SpellID::PALIOS, SpellType::MAGE, SpellCategory::DISABLE, level, false,
		"PALIOS", "Anti-Magic", "Destroys spell-dampening fields.");
	_spells.emplace_back(SpellID::SOCORDI, SpellType::MAGE, SpellCategory::SUPPORT, level, false,
		"SOCORDI", "Terror",
		"Gates in a powerful extra-dimnensional creature to aid the party during combat.");
	_spells.emplace_back(SpellID::VASKYRE, SpellType::MAGE, SpellCategory::DISABLE, level, false,
		"VASKYRE", "Rainbow Rays", "Causes random damaging effects to a group of foes.");

	// Level 6
	++level;
	_spells.emplace_back(SpellID::HAMAN, SpellType::MAGE, SpellCategory::SUPPORT, level, false,
		"HAMAN", "Change",
		"Causes random but beneficial effects to the entire party but the caster loses one level "
		"of experience.");
	_spells.emplace_back(SpellID::LADALTO, SpellType::MAGE, SpellCategory::ATTACK, level, false,
		"LADALTO", "Ice Storm",
		"Freezes one group of foes, inflicting 10d10 points of cold damage.");
	_spells.emplace_back(SpellID::LOKARA, SpellType::MAGE, SpellCategory::ATTACK, level, false,
		"LOKARA", "Big Glass",
		"Opens the earth underneath all foes, attempting to swallow them whole.");
	_spells.emplace_back(SpellID::ZILWAN, SpellType::MAGE, SpellCategory::ATTACK, level, false,
		"ZILWAN", "Dispel",
		"Inflicts 200d10 points of positive energy damage to one undead creature.");

	// Level 7
	++level;
	_spells.emplace_back(SpellID::MAHAMAN, SpellType::MAGE, SpellCategory::SUPPORT, level, false,
		"MAHAMAN", "Great Change",
		"Causes random but beneficial major effects to the entire party but the caster loses one "
		"level and the spell is forgotten.");
	_spells.emplace_back(SpellID::MALOR, SpellType::MAGE, SpellCategory::FIELD, level, false,
		"MALOR", "Apport",
		"Teleports the party to a random nearby location when cast in combat, but to a specified "
		"location when cast outside of combat.");
	_spells.emplace_back(SpellID::MAWXIWTZ, SpellType::MAGE, SpellCategory::DISABLE, level, false,
		"MAWXIWTZ", "MadHouse", "Causes random major damaging effects to all foes.");
	_spells.emplace_back(SpellID::TILTOWAIT, SpellType::MAGE, SpellCategory::ATTACK, level, false,
		"TILTOWAIT", "Explosion", "Inflicts 10d15 points of fire and force damage to all foes.");

	// Priest Spells (grouped by level)

	// Level 1
	level = 1u;
	_spells.emplace_back(SpellID::BADIOS, SpellType::PRIEST, SpellCategory::ATTACK, level, false,
		"BADIOS", "Harm", "Inflicts 1d8 points of negative energy damage to one foe.");
	_spells.emplace_back(SpellID::DIOS, SpellType::PRIEST, SpellCategory::HEALING, level, false,
		"DIOS", "Heal", "Restores 1d8 hp to a party member.");
	_spells.emplace_back(SpellID::KALKI, SpellType::PRIEST, SpellCategory::SUPPORT, level, false,
		"KALKI", "Blessings",
		"Grants a -1 bonus to armour class to the entire party for the duration of combat.");
	_spells.emplace_back(SpellID::MILWA, SpellType::PRIEST, SpellCategory::FIELD, level, false,
		"MILWA", "Light",
		"Causes a softly glowing light to follow the party, increasing vision and revealing some "
		"secret doors for 15d2 turns.");
	_spells.emplace_back(SpellID::PORFIC, SpellType::PRIEST, SpellCategory::SUPPORT, level, false,
		"PORFIC", "Shield",
		"Grants a -4 bonus to armour class to the caster for the duration of combat.");

	// Level 2
	++level;
	_spells.emplace_back(SpellID::CALFO, SpellType::PRIEST, SpellCategory::FIELD, level, false,
		"CALFO", "X-Ray Vision",
		"Allows the caster to identify the trap on a chest with 95% accuracy.");
	_spells.emplace_back(SpellID::KANDI, SpellType::PRIEST, SpellCategory::FIELD, level, false,
		"KANDI", "Locate Soul",
		"Gives the direction of the creature the party is attempting to locate; the location is "
		"relative to the position of the caster.");
	_spells.emplace_back(SpellID::KATU, SpellType::PRIEST, SpellCategory::FIELD, level, false,
		"KATU", "Charm",
		"Attempts to charm one creature or foe, making it more friendly to the party.");
	_spells.emplace_back(SpellID::MONTINO, SpellType::PRIEST, SpellCategory::DISABLE, level, false,
		"MONTINO", "Still Air",
		"Stills the air around a group of foes, making it impossible for them to cast spells.");

	// Level 3
	++level;
	_spells.emplace_back(SpellID::BAMATU, SpellType::PRIEST, SpellCategory::SUPPORT, level, false,
		"BAMATU", "Prayer",
		"Grants a -4 bonus to armour class to the entire party for the duration of combat.");
	_spells.emplace_back(SpellID::DIALKO, SpellType::PRIEST, SpellCategory::HEALING, level, false,
		"DIALKO", "Softness", "Cures one party member of paralysis, silence, or sleep.");
	_spells.emplace_back(SpellID::HAKANIDO, SpellType::PRIEST, SpellCategory::DISABLE, level, false,
		"HAKANIDO", "Magic Drain", "Drains one foe of spell points.");
	_spells.emplace_back(SpellID::LATUMAPIC, SpellType::PRIEST, SpellCategory::FIELD, level, false,
		"LATUMAPIC", "Identify", "Full identifies unknown foes.");
	_spells.emplace_back(SpellID::LOMILWA, SpellType::PRIEST, SpellCategory::FIELD, level, false,
		"LOMILWA", "More Light",
		" Extend the party's field of vision and reveals most secret doors. Lasts until leaving "
		"the maze or entering an area of magical darkness.");

	// Level 4
	++level;
	_spells.emplace_back(SpellID::BADIAL, SpellType::PRIEST, SpellCategory::ATTACK, level, false,
		"BADIAL", "More Hurt", "Inflicts 2d8 points of negative energy damage to one foe.");
	_spells.emplace_back(SpellID::BARIKO, SpellType::PRIEST, SpellCategory::ATTACK, level, false,
		"BARIKO", "Razor Wind", "Inflicts 5d3 points of force damage to a group of foes.");
	_spells.emplace_back(SpellID::DIAL, SpellType::PRIEST, SpellCategory::HEALING, level, false,
		"DIAL", "More Heal", "Restores 2d8 hp to a party member.");
	_spells.emplace_back(SpellID::LATUMOFIS, SpellType::PRIEST, SpellCategory::HEALING, level,
		false, "LATUMOFIS", "Cure Poison", "Cures a party member of poisoning.");
	_spells.emplace_back(SpellID::MAPORFIC, SpellType::PRIEST, SpellCategory::FIELD, level, false,
		"MAPORFIC", "Big Shield",
		"Grants a -2 bonus to armour class to the entire party. Lasts until leaving the maze or "
		"dispelled");

	// Level 5
	++level;
	_spells.emplace_back(SpellID::BADI, SpellType::PRIEST, SpellCategory::ATTACK, level, false,
		"BADI", "Death", "Attempts to slay one foe outright.");
	_spells.emplace_back(SpellID::BAMORDI, SpellType::PRIEST, SpellCategory::SUPPORT, level, false,
		"BAMORDI", "Summoning",
		"Gates in a group of powerful extra-dimnensional creature to aid the party during combat.");
	_spells.emplace_back(SpellID::DI, SpellType::PRIEST, SpellCategory::HEALING, level, false, "DI",
		"Life",
		"Attempts to resurrect a dead party member. If it succeeds, the party members has 1 hp, "
		"and loses 1 point of vitality. If it fails, the dead member is turned to ashes.");
	_spells.emplace_back(SpellID::DIALMA, SpellType::PRIEST, SpellCategory::HEALING, level, false,
		"DIALMA", "Great Heal", "Restores 3d8 hp to a party member.");
	_spells.emplace_back(SpellID::MOGATO, SpellType::PRIEST, SpellCategory::DISABLE, level, false,
		"MOGATO", "Astral Gate", "Attempts to banish a devil or demon back to its home plane.");

	// Level 6
	++level;
	_spells.emplace_back(SpellID::LABADI, SpellType::PRIEST, SpellCategory::ATTACK, level, false,
		"LABADI", "Life Steal",
		"Attempts to drain all but 1d8 hp from a foe, and transfers the lifeforce to heal the "
		"caster.");
	_spells.emplace_back(SpellID::LOKTOFEIT, SpellType::PRIEST, SpellCategory::FIELD, level, false,
		"LOKTOFEIT", "Recall",
		"Causes all party members to be transported back to the castle, all of their equipment and "
		"gold, but the spell is forgotten after casting and must be relearned, and there is a "
		"chance the spell will not work.");
	_spells.emplace_back(SpellID::KAKAMEN, SpellType::PRIEST, SpellCategory::ATTACK, level, false,
		"KAKAMEN", "Fire Wind", "Inflicts 18d2 points of fire damage to a group of foes.");
	_spells.emplace_back(SpellID::MADI, SpellType::PRIEST, SpellCategory::HEALING, level, false,
		"MADI", "Healing",
		"Fills a party member with positive energy, causeing all hp to be restored to a party "
		"member and curing any condition except death.");

	// Level 7
	++level;
	_spells.emplace_back(SpellID::BAKADI, SpellType::PRIEST, SpellCategory::ATTACK, level, false,
		"BAKADI", "Death Wind",
		"Unleashes a massive blast of negative energy at one group of foes, attempting to wipe "
		"them from existance.");
	_spells.emplace_back(SpellID::IHALON, SpellType::PRIEST, SpellCategory::SUPPORT, level, false,
		"IHALON", "Wish",
		"Grants a special favor to a party member, but is forgotten after being cast");
	_spells.emplace_back(SpellID::KADORTO, SpellType::PRIEST, SpellCategory::HEALING, level, false,
		"KADORTO", "Resurrection",
		"Restores the dead to life, and restores all hp and cures all conditions, even if the "
		"party member is ashes. If the attempt fails, the character is lost forever.");
	_spells.emplace_back(SpellID::MABARIKO, SpellType::PRIEST, SpellCategory::ATTACK, level, false,
		"MABARIKO", "Meteor Winds", "Inflicts 18d3 points of fire damage to all foes.");
}

auto Sorcery::Character::reset_spells() -> void {
	for (auto &spell : _spells) {
		_spells_known[spell.id] = spell.known;
	}
}

auto Sorcery::Character::create_quick() -> void {

	// Enter Name and Portrait, rest is random
	// Exclude Samurai/Lord/Ninja/Bishop from this method of character creation
	_class = {static_cast<CharacterClass>((*_system->random)[RandomType::D4])};
	_race = {static_cast<CharacterRace>((*_system->random)[RandomType::D5])};
	switch (_class) { // NOLINT(clang-diagnostic-switch)
	case CharacterClass::FIGHTER:
	case CharacterClass::MAGE:
		_alignment = {static_cast<CharacterAlignment>((*_system->random)[RandomType::D3])};
		break;
	case CharacterClass::PRIEST:
		_alignment = (*_system->random)[RandomType::D2] == 1 ? CharacterAlignment::GOOD
															 : CharacterAlignment::EVIL;
		break;
	case CharacterClass::THIEF:
		_alignment = (*_system->random)[RandomType::D2] == 1 ? CharacterAlignment::NEUTRAL
															 : CharacterAlignment::EVIL;
		break;
	default:
		break;
	}

	// Now get minimum attributes for race/class combo (note as we are only allowing creation of
	// some classes, it will be as if we had a maximum of 10 bonus points to spend - in order to
	// incentivise full blown character creation! see table IV (A) at
	// https://gamefaqs.gamespot.com/pc/946844-the-ultimate-wizardry-archives/faqs/45726 for
	// info
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

	// Put most of the points into the main attribute (note that 10 points means a Human Priest
	// and Dwarf Thief have allocated all points to their main attribute with no points left
	// over)
	_points_left = 10;
	_st_points = _points_left;
	switch (_class) { // NOLINT(clang-diagnostic-switch)
	case CharacterClass::FIGHTER:
		_points_left -= (15 - _start_attr[CharacterAttribute::STRENGTH]);
		_start_attr[CharacterAttribute::STRENGTH] = 15;
		break;
	case CharacterClass::MAGE:
		_points_left -= (15 - _start_attr[CharacterAttribute::IQ]);
		_start_attr[CharacterAttribute::IQ] = 15;
		break;
	case CharacterClass::PRIEST:
		_points_left -= (15 - _start_attr[CharacterAttribute::PIETY]);
		_start_attr[CharacterAttribute::PIETY] = 15;
		break;
	case CharacterClass::THIEF:
		_points_left -= (15 - _start_attr[CharacterAttribute::AGILITY]);
		_start_attr[CharacterAttribute::AGILITY] = 15;
		break;
	default:
		break;
	}

	// Pump any points left into the Vitality attribute
	if (_points_left > 0)
		_start_attr[CharacterAttribute::VITALITY] += _points_left;

	_cur_attr = _start_attr;
}

// Create a (semi) random character
auto Sorcery::Character::create_random() -> void {

	// Random Name and Portrait
	create_quick();
	_name = _system->random->get_random_name();
	_portrait_index = (*_system->random)[RandomType::ZERO_TO_29];
}

auto Sorcery::Character::_get_character_portrait() -> sf::Sprite {

	// Workout the location of the potrait on the texture, noting that the potraits are all
	// square and are 600x600 pixels in size arranged in a grid of 6 by 5
	constexpr int portrait_size{600};
	sf::Vector2u top_left{
		(_portrait_index % 6) * portrait_size, (_portrait_index / 6) * portrait_size};
	sf::IntRect rect{sf::IntRect(top_left.x, top_left.y, portrait_size, portrait_size)};

	// Grab the associated part of the texture and return it
	sf::Sprite portrait(_system->resources->textures[GraphicsTexture::PORTRAITS]);
	portrait.setTextureRect(rect);

	return portrait;
}

auto Sorcery::Character::get_status() -> std::string {

	// TODO: for now just return ok
	return "OK";
}
auto Sorcery::Character::get_hp_summary() -> std::string {

	return fmt::format("{}/{}", std::to_string(_abilities.at(CharacterAbility::CURRENT_HP)),
		std::to_string(_abilities.at(CharacterAbility::MAX_HP)));
}

auto Sorcery::Character::get_spell_points(const SpellType type, const SpellPointStatus status) const
	-> std::optional<SpellPoints> {

	if ((type == SpellType::MAGE) && (status == SpellPointStatus::CURRENT))
		return _mage_cur_sp;
	else if ((type == SpellType::MAGE) && (status == SpellPointStatus::MAXIMUM))
		return _mage_max_sp;
	else if ((type == SpellType::PRIEST) && (status == SpellPointStatus::CURRENT))
		return _priest_cur_sp;
	else if ((type == SpellType::PRIEST) && (status == SpellPointStatus::MAXIMUM))
		return _priest_max_sp;
	else
		return std::nullopt;
}

auto Sorcery::Character::_get_spell_points_per_level(const SpellType type, int level)
	-> std::string {

	if (type == SpellType::MAGE) {
		return fmt::format(
			"{}/{}", std::to_string(_mage_cur_sp[level]), std::to_string(_mage_max_sp[level]));
	} else {
		return fmt::format(
			"{}/{}", std::to_string(_priest_cur_sp[level]), std::to_string(_priest_max_sp[level]));
	}
}

auto Sorcery::Character::_get_mage_magic_status(bool current) -> std::string {

	std::string value{};
	for (auto level = 1; level <= 7; level++)
		if (current)
			value.append(fmt::format(" {} ", _mage_cur_sp[level]));
		else
			value.append(fmt::format("({})", _mage_max_sp[level]));

	RTRIM(value);
	return value;
}

auto Sorcery::Character::_get_priest_magic_status(bool current) -> std::string {

	std::string value{};
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

auto Sorcery::Character::_generate_summary_icons() -> void {
	auto class_icon = get_icon(CharacterStage::CHOOSE_CLASS).value();
	class_icon.setPosition((*_display->layout)["character:class_icon"].x,
		(*_display->layout)["character:class_icon"].y);
	class_icon.setScale((*_display->layout)["character:class_icon"].scale,
		(*_display->layout)["character:class_icon"].scale);
	_v_sprites.emplace((*_display->layout)["character:class_icon"].unique_key, class_icon);

	auto race_icon = get_icon(CharacterStage::CHOOSE_RACE).value();
	race_icon.setPosition(
		(*_display->layout)["character:race_icon"].x, (*_display->layout)["character:race_icon"].y);
	race_icon.setScale((*_display->layout)["character:race_icon"].scale,
		(*_display->layout)["character:race_icon"].scale);
	_v_sprites.emplace((*_display->layout)["character:race_icon"].unique_key, race_icon);

	auto alignment_icon = get_icon(CharacterStage::CHOOSE_ALIGNMENT).value();
	alignment_icon.setPosition((*_display->layout)["character:alignment_icon"].x,
		(*_display->layout)["character:alignment_icon"].y);
	alignment_icon.setScale((*_display->layout)["character:alignment_icon"].scale,
		(*_display->layout)["character:alignment_icon"].scale);
	_v_sprites.emplace((*_display->layout)["character:alignment_icon"].unique_key, alignment_icon);

	auto level_icon = (*_graphics->icons)["level"].value();
	level_icon.setPosition((*_display->layout)["character:level_icon"].x,
		(*_display->layout)["character:level_icon"].y);
	level_icon.setScale((*_display->layout)["character:level_icon"].scale,
		(*_display->layout)["character:level_icon"].scale);
	_v_sprites.emplace((*_display->layout)["character:level_icon"].unique_key, level_icon);

	_add_text((*_display->layout)["character:level_text"], "{}",
		std::to_string(_abilities.at(CharacterAbility::CURRENT_LEVEL)), true);
}

auto Sorcery::Character::_adjust_ability_colour(int value, CharacterAbilityType ability_type)
	-> unsigned long long {

	// Colours "borrowed" from
	// https://github.com/angband/angband/blob/master/src/ui-player.c
	// https://github.com/angband/angband/blob/master/src/z-color.h
	std::string colour{};
	switch (ability_type) {
	case CharacterAbilityType::NUMBER:
		if (value < 0)
			colour = "negative";
		else if (value == 0)
			colour = "zero";
		else
			colour = "positive";
		break;
	case CharacterAbilityType::AC:
		if (value == 10)
			colour = "negative";
		else if (value > 6)
			colour = "zero";
		else
			colour = "positive";
		break;
	case CharacterAbilityType::STAT:
		if (value < 6)
			colour = "bad";
		else if (value < 8)
			colour = "poor";
		else if (value < 10)
			colour = "fair";
		else if (value < 12)
			colour = "good";
		else if (value < 14)
			colour = "very good";
		else if (value < 15)
			colour = "excellent";
		else if (value < 16)
			colour = "superb";
		else if (value < 18)
			colour = "heroic";
		else
			colour = "legendary";
		break;
	case CharacterAbilityType::PERCENTAGE:
		if (value < 20)
			colour = "bad";
		else if (value < 30)
			colour = "poor";
		else if (value < 40)
			colour = "fair";
		else if (value < 50)
			colour = "good";
		else if (value < 60)
			colour = "very good";
		else if (value < 70)
			colour = "excellent";
		else if (value < 80)
			colour = "superb";
		else if (value < 90)
			colour = "heroic";
		else
			colour = "legendary";
		break;
	case CharacterAbilityType::MODIFIER:
		if (value < 0)
			colour = "negative";
		else if (value == 0)
			colour = "zero";
		else
			colour = "positive";
		break;
	default:
		colour = "label";
		break;
	};

	Component colours{(*_display->layout)["character:character_colours"]};
	return std::stoull(colours[colour].value(), 0, 16);
}

auto Sorcery::Character::get_summary() -> std::string {
	return fmt::format("{:<15} L {:>2} {}-{} {}", _name,
		_abilities.at(CharacterAbility::CURRENT_LEVEL), get_alignment(_alignment).substr(0, 1),
		get_class(_class).substr(0, 3), get_race(_race).substr(0, 3));
}

auto Sorcery::Character::summary_text() -> std::string {

	// CHOOSE NAME: 			"???             L  1 ?-??? ???"
	// CHOOSE RACE: 			"NAMENAMENAMENAM L  1 ?-??? ???"
	// CHOOSE ALIGNMENT: 		"NAMENAMENAMENAM L  1 ?-??? RAC"
	// ALLOCATE STATS:			"NAMENAMENAMENAM L  1 A-??? RAC"
	// CHOOSE PORTRAIT:			"NAMENAMENAMENAM L  1 A-CLA RAC"
	// REVIEW AND CONFIRM:		"NAMENAMENAMENAM L  1 A-CLA RAC"
	switch (_current_stage) {
	case CharacterStage::CHOOSE_METHOD:
	case CharacterStage::ENTER_NAME:
		return fmt::format("{:<15} L ?? ?-??? ???", "???");
		break;
	case CharacterStage::CHOOSE_RACE:
		return fmt::format(
			"{:<15} L {:>2} ?-??? ???", _name, _abilities.at(CharacterAbility::CURRENT_LEVEL));
		break;
	case CharacterStage::CHOOSE_ALIGNMENT:
		return fmt::format("{:<15} L {:>2} ?-??? {}", _name,
			_abilities.at(CharacterAbility::CURRENT_LEVEL), get_race(_race));
		break;
	case CharacterStage::ALLOCATE_STATS:
		return fmt::format("{:<15} L {:>2} {}-??? {}", _name,
			_abilities.at(CharacterAbility::CURRENT_LEVEL), get_alignment(_alignment).substr(0, 1),
			get_race(_race));
		break;
	case CharacterStage::CHOOSE_CLASS:
		return fmt::format("{:<15} L {:>2} {}-??? {}", _name,
			_abilities.at(CharacterAbility::CURRENT_LEVEL), get_alignment(_alignment).substr(0, 1),
			get_race(_race));
		break;
	case CharacterStage::CHOOSE_PORTRAIT:
		return fmt::format("{:<15} L {:>2} {}-{} {}", _name,
			_abilities.at(CharacterAbility::CURRENT_LEVEL), get_alignment(_alignment).substr(0, 1),
			get_class(_class).substr(0, 3), get_race(_race));
		break;
	case CharacterStage::REVIEW_AND_CONFIRM:
	case CharacterStage::COMPLETED:
		return fmt::format("{} L {:>2} {}-{} {}", _name,
			_abilities.at(CharacterAbility::CURRENT_LEVEL), get_alignment(_alignment).substr(0, 1),
			get_class(_class).substr(0, 3), get_race(_race));
		break;
	default:
		return "";
		break;
	}

	// Character Sheet version has no 15 name padding
	// Roster version/character creation has 15 padding

	// Names 15 characters

	// starting equipment

	// priest, bishop: staff and robes
	// fighter: longsword leather armour
	// bishop: staff and robes
	// mage: dagger robes
	// thief: short sword leather armour
	// samurai, lord, ninja?
}

auto Sorcery::Character::_generate_display() -> void {

	_sprites.clear();
	_texts.clear();
	_frames.clear();
	_v_sprites.clear();
	_v_texts.clear();
	_v_frames.clear();

	_display->generate_components("character", _sprites, _texts, _frames);

	if (_view == CharacterView::SUMMARY) {

		_display->generate_components("character_strict", _v_sprites, _v_texts, _v_frames);

		_add_text(
			(*_display->layout)["character_strict:name_and_summary_text"], "{}", summary_text());

		auto portrait{_get_character_portrait()};
		Component portrait_c{(*_display->layout)["character_strict:portrait"]};
		portrait.setScale(portrait_c.scale, portrait_c.scale);
		const int offset_x = [&] {
			if (portrait_c["offset_x"])
				return std::stoi(portrait_c["offset_x"].value());
			else
				return 0;
		}();
		const int offset_y = [&] {
			if (portrait_c["offset_y"])
				return std::stoi(portrait_c["offset_y"].value());
			else
				return 0;
		}();
		portrait.setPosition(portrait_c.x + offset_x, portrait_c.y + offset_y);
		_v_sprites.emplace(portrait_c.unique_key, portrait);

		Component s_c{(*_display->layout)["character_strict:strength_value"]};
		s_c.colour = _adjust_ability_colour(
			_cur_attr.at(CharacterAttribute::STRENGTH), CharacterAbilityType::STAT);
		_add_text(s_c, "{:>2}", std::to_string(_cur_attr.at(CharacterAttribute::STRENGTH)));

		Component i_c{(*_display->layout)["character_strict:iq_value"]};
		i_c.colour = _adjust_ability_colour(
			_cur_attr.at(CharacterAttribute::IQ), CharacterAbilityType::STAT);
		_add_text(i_c, "{:>2}", std::to_string(_cur_attr.at(CharacterAttribute::IQ)));

		Component p_c{(*_display->layout)["character_strict:piety_value"]};
		p_c.colour = _adjust_ability_colour(
			_cur_attr.at(CharacterAttribute::PIETY), CharacterAbilityType::STAT);
		_add_text(p_c, "{:>2}", std::to_string(_cur_attr.at(CharacterAttribute::PIETY)));

		Component a_c{(*_display->layout)["character_strict:agility_value"]};
		a_c.colour = _adjust_ability_colour(
			_cur_attr.at(CharacterAttribute::AGILITY), CharacterAbilityType::STAT);
		_add_text(a_c, "{:>2}", std::to_string(_cur_attr.at(CharacterAttribute::AGILITY)));

		Component v_c{(*_display->layout)["character_strict:vitality_value"]};
		v_c.colour = _adjust_ability_colour(
			_cur_attr.at(CharacterAttribute::VITALITY), CharacterAbilityType::STAT);
		_add_text(v_c, "{:>2}", std::to_string(_cur_attr.at(CharacterAttribute::VITALITY)));

		Component l_c{(*_display->layout)["character_strict:luck_value"]};
		l_c.colour = _adjust_ability_colour(
			_cur_attr.at(CharacterAttribute::LUCK), CharacterAbilityType::STAT);
		_add_text(l_c, "{:>2}", std::to_string(_cur_attr.at(CharacterAttribute::LUCK)));

		/* _add_text((*_display->layout)["character_strict:strength_value"], "{:>2}",
			std::to_string(_cur_attr.at(CharacterAttribute::STRENGTH)));
		_add_text((*_display->layout)["character_strict:iq_value"], "{:>2}",
			std::to_string(_cur_attr.at(CharacterAttribute::IQ)));
		_add_text((*_display->layout)["character_strict:piety_value"], "{:>2}",
			std::to_string(_cur_attr.at(CharacterAttribute::PIETY)));
		_add_text((*_display->layout)["character_strict:agility_value"], "{:>2}",
			std::to_string(_cur_attr.at(CharacterAttribute::AGILITY)));
		_add_text((*_display->layout)["character_strict:vitality_value"], "{:>2}",
			std::to_string(_cur_attr.at(CharacterAttribute::VITALITY)));
		_add_text((*_display->layout)["character_strict:luck_value"], "{:>2}",
			std::to_string(_cur_attr.at(CharacterAttribute::LUCK))); */

		_add_text((*_display->layout)["character_strict:hp_value"], "{}",
			fmt::format("{}/{}", std::to_string(_abilities.at(CharacterAbility::CURRENT_HP)),
				std::to_string(_abilities.at(CharacterAbility::MAX_HP))));
		_add_text((*_display->layout)["character_strict:ac_value"], "{}",
			std::to_string(_abilities.at(CharacterAbility::CURRENT_ARMOUR_CLASS)));
		_add_text((*_display->layout)["character_strict:age_value"], "{}",
			std::to_string(static_cast<int>(_abilities.at(CharacterAbility::AGE) / 52)));
		_add_text((*_display->layout)["character_strict:swim_value"], "{}",
			std::to_string(_abilities.at(CharacterAbility::SWIM)));
		_add_text((*_display->layout)["character_strict:status_value"], "{}",
			"OK"); // TODO

		//_add_text((*_display->layout)["character_strict:lev_value"], "{}",
		//		std::to_string(_abilities.at(CharacterAbility::CURRENT_LEVEL)));
		_add_text((*_display->layout)["character_strict:exp_value"], "{}",
			std::to_string(_abilities.at(CharacterAbility::CURRENT_XP)));
		_add_text((*_display->layout)["character_strict:next_value"], "{}",
			std::to_string(_abilities.at(CharacterAbility::NEXT_LEVEL_XP)));
		_add_text((*_display->layout)["character_strict:gold_value"], "{}",
			std::to_string(_abilities.at(CharacterAbility::GOLD)));
		_add_text((*_display->layout)["character_strict:marks_value"], "{}",
			std::to_string(_abilities.at(CharacterAbility::MARKS)));
		_add_text((*_display->layout)["character_strict:deaths_value"], "{}",
			std::to_string(_abilities.at(CharacterAbility::DEATHS)));

		std::string mage_spells{fmt::format("{}/{}/{}/{}/{}/{}/{}", _mage_cur_sp.at(1),
			_mage_cur_sp.at(2), _mage_cur_sp.at(3), _mage_cur_sp.at(4), _mage_cur_sp.at(5),
			_mage_cur_sp.at(6), _mage_cur_sp.at(7))};
		std::string priest_spells{fmt::format("{}/{}/{}/{}/{}/{}/{}", _priest_cur_sp.at(1),
			_priest_cur_sp.at(2), _priest_cur_sp.at(3), _priest_cur_sp.at(4), _priest_cur_sp.at(5),
			_priest_cur_sp.at(6), _priest_cur_sp.at(7))};

		_add_text((*_display->layout)["character_strict:mage_spells"], "{}", mage_spells);
		_add_text((*_display->layout)["character_strict:priest_spells"], "{}", priest_spells);

		/* } else if (_view == CharacterView::MAIN) {

			_display->generate_components("character_main", _v_sprites, _v_texts, _v_frames);

			_generate_summary_icons();

			auto portrait{_get_character_portrait()};
			Component portrait_c{(*_display->layout)["character_main:portrait"]};
			portrait.setPosition(portrait_c.x, portrait_c.y);
			portrait.setScale(portrait_c.scale, portrait_c.scale);
			_v_sprites.emplace(portrait_c.unique_key, portrait);

			_add_text((*_display->layout)["character_main:name_text"], "{}", _name);

			_ad = std::make_shared<AttributeDisplay>(
				_system, _display, _graphics, this, Alignment::HORIZONTAL);

			_ss = std::make_shared<SpellSummary>(_system, _display, _graphics, this);

			_ad_c = Component((*_display->layout)["character_main:attribute_display"]);
			_ad->set();
			_ad->setPosition(_ad_c.x + std::stoi(_ad_c["offset_x"].value()),
				_ad_c.y + std::stoi(_ad_c["offset_y"].value()));

			_ss_c = Component((*_display->layout)["character_main:spellsummary"]);
			_ss->set();
			_ss->setPosition(_ss_c.x + std::stoi(_ss_c["offset_x"].value()),
				_ss_c.y + std::stoi(_ss_c["offset_y"].value()));

			_add_text((*_display->layout)["character_main:hp_value"], "{}",
				fmt::format("{}/{}", std::to_string(_abilities.at(CharacterAbility::CURRENT_HP)),
					std::to_string(_abilities.at(CharacterAbility::MAX_HP))));
			_add_text((*_display->layout)["character_main:ac_value"], "{}",
				std::to_string(_abilities.at(CharacterAbility::CURRENT_ARMOUR_CLASS)));
			_add_text((*_display->layout)["character_main:age_value"], "{}",
				std::to_string(static_cast<int>(_abilities.at(CharacterAbility::AGE) / 52)));
			_add_text((*_display->layout)["character_main:swim_value"], "{}",
				std::to_string(_abilities.at(CharacterAbility::SWIM)));
			_add_text((*_display->layout)["character_main:exp_value"], "{}",
				std::to_string(_abilities.at(CharacterAbility::CURRENT_XP)));
			_add_text((*_display->layout)["character_main:next_value"], "{}",
				std::to_string(_abilities.at(CharacterAbility::NEXT_LEVEL_XP)));
			_add_text((*_display->layout)["character_main:gold_value"], "{}",
				std::to_string(_abilities.at(CharacterAbility::GOLD)));
			_add_text((*_display->layout)["character_main:marks_value"], "{}",
				std::to_string(_abilities.at(CharacterAbility::MARKS)));
			_add_text((*_display->layout)["character_main:deaths_value"], "{}",
				std::to_string(_abilities.at(CharacterAbility::DEATHS))); */
	} else if (_view == CharacterView::DETAILED) {

		_display->generate_components("character_detailed", _v_sprites, _v_texts, _v_frames);

		_add_text(
			(*_display->layout)["character_detailed:name_and_summary_text"], "{}", summary_text());

		auto portrait{_get_character_portrait()};
		Component portrait_c{(*_display->layout)["character_strict:portrait"]};
		portrait.setScale(portrait_c.scale, portrait_c.scale);
		const int offset_x = [&] {
			if (portrait_c["offset_x"])
				return std::stoi(portrait_c["offset_x"].value());
			else
				return 0;
		}();
		const int offset_y = [&] {
			if (portrait_c["offset_y"])
				return std::stoi(portrait_c["offset_y"].value());
			else
				return 0;
		}();
		portrait.setPosition(portrait_c.x + offset_x, portrait_c.y + offset_y);
		_v_sprites.emplace(portrait_c.unique_key, portrait);

		Component s_c{(*_display->layout)["character_detailed:strength_value"]};
		s_c.colour = _adjust_ability_colour(
			_cur_attr.at(CharacterAttribute::STRENGTH), CharacterAbilityType::STAT);
		_add_text(s_c, "{:>2}", std::to_string(_cur_attr.at(CharacterAttribute::STRENGTH)));

		Component i_c{(*_display->layout)["character_detailed:iq_value"]};
		i_c.colour = _adjust_ability_colour(
			_cur_attr.at(CharacterAttribute::IQ), CharacterAbilityType::STAT);
		_add_text(i_c, "{:>2}", std::to_string(_cur_attr.at(CharacterAttribute::IQ)));

		Component p_c{(*_display->layout)["character_detailed:piety_value"]};
		p_c.colour = _adjust_ability_colour(
			_cur_attr.at(CharacterAttribute::PIETY), CharacterAbilityType::STAT);
		_add_text(p_c, "{:>2}", std::to_string(_cur_attr.at(CharacterAttribute::PIETY)));

		Component a_c{(*_display->layout)["character_detailed:agility_value"]};
		a_c.colour = _adjust_ability_colour(
			_cur_attr.at(CharacterAttribute::AGILITY), CharacterAbilityType::STAT);
		_add_text(a_c, "{:>2}", std::to_string(_cur_attr.at(CharacterAttribute::AGILITY)));

		Component v_c{(*_display->layout)["character_detailed:vitality_value"]};
		v_c.colour = _adjust_ability_colour(
			_cur_attr.at(CharacterAttribute::VITALITY), CharacterAbilityType::STAT);
		_add_text(v_c, "{:>2}", std::to_string(_cur_attr.at(CharacterAttribute::VITALITY)));

		Component l_c{(*_display->layout)["character_detailed:luck_value"]};
		l_c.colour = _adjust_ability_colour(
			_cur_attr.at(CharacterAttribute::LUCK), CharacterAbilityType::STAT);
		_add_text(l_c, "{:>2}", std::to_string(_cur_attr.at(CharacterAttribute::LUCK)));

		Component strength_c((*_display->layout)["character_detailed:strength_detailed_values"]);

		strength_c.colour = _adjust_ability_colour(
			_abilities.at(CharacterAbility::ATTACK_MODIFIER), CharacterAbilityType::MODIFIER);
		_add_text(
			strength_c, "{:+>2}", std::to_string(_abilities.at(CharacterAbility::ATTACK_MODIFIER)));

		strength_c.y += _display->window->get_cell_height();
		strength_c.colour = _adjust_ability_colour(
			_abilities.at(CharacterAbility::HIT_PROBABILITY), CharacterAbilityType::MODIFIER);
		_add_text(
			strength_c, "{:+>2}", std::to_string(_abilities.at(CharacterAbility::HIT_PROBABILITY)));

		strength_c.y += _display->window->get_cell_height();
		strength_c.colour = _adjust_ability_colour(
			_abilities.at(CharacterAbility::BONUS_DAMAGE), CharacterAbilityType::MODIFIER);
		_add_text(
			strength_c, "{:+>2}", std::to_string(_abilities.at(CharacterAbility::BONUS_DAMAGE)));

		strength_c.y += _display->window->get_cell_height();
		strength_c.colour = _adjust_ability_colour(
			_abilities.at(CharacterAbility::BASE_NUMBER_OF_ATTACKS), CharacterAbilityType::NUMBER);
		_add_text(strength_c, "{:>2}",
			std::to_string(_abilities.at(CharacterAbility::BASE_NUMBER_OF_ATTACKS)));

		strength_c.y += _display->window->get_cell_height();
		strength_c.colour = _adjust_ability_colour(
			_abilities.at(CharacterAbility::UNARMED_DAMAGE), CharacterAbilityType::NUMBER);
		_add_text(
			strength_c, "{:>2}", std::to_string(_abilities.at(CharacterAbility::UNARMED_DAMAGE)));

		Component iq_c((*_display->layout)["character_detailed:iq_detailed_values"]);

		iq_c.colour = _adjust_ability_colour(
			_abilities.at(CharacterAbility::MAGE_SPELL_LEARN), CharacterAbilityType::PERCENTAGE);
		_add_text(
			iq_c, "{:>2}%", std::to_string(_abilities.at(CharacterAbility::MAGE_SPELL_LEARN)));

		iq_c.y += _display->window->get_cell_height();
		iq_c.colour = _adjust_ability_colour(
			_abilities.at(CharacterAbility::IDENTIFY_ITEMS), CharacterAbilityType::PERCENTAGE);
		_add_text(iq_c, "{:>2}%", std::to_string(_abilities.at(CharacterAbility::IDENTIFY_ITEMS)));

		iq_c.y += _display->window->get_cell_height();
		iq_c.colour = _adjust_ability_colour(
			_abilities.at(CharacterAbility::IDENTIFY_CURSE), CharacterAbilityType::PERCENTAGE);
		_add_text(iq_c, "{:>2}%", std::to_string(_abilities.at(CharacterAbility::IDENTIFY_CURSE)));

		iq_c.y += _display->window->get_cell_height();
		iq_c.colour = _adjust_ability_colour(
			_abilities.at(CharacterAbility::IDENTIFY_FOES), CharacterAbilityType::PERCENTAGE);
		_add_text(iq_c, "{:>2}%", std::to_string(_abilities.at(CharacterAbility::IDENTIFY_FOES)));

		iq_c.y += _display->window->get_cell_height();
		iq_c.colour = _adjust_ability_colour(
			_abilities.at(CharacterAbility::BONUS_MAGE_SPELLS), CharacterAbilityType::NUMBER);
		_add_text(
			iq_c, "{:>2}", std::to_string(_abilities.at(CharacterAbility::BONUS_MAGE_SPELLS)));

		Component piety_c((*_display->layout)["character_detailed:piety_detailed_values"]);
		piety_c.colour = _adjust_ability_colour(
			_abilities.at(CharacterAbility::PRIEST_SPELL_LEARN), CharacterAbilityType::PERCENTAGE);
		_add_text(
			piety_c, "{:>2}%", std::to_string(_abilities.at(CharacterAbility::PRIEST_SPELL_LEARN)));

		piety_c.y += _display->window->get_cell_height();
		piety_c.colour = _adjust_ability_colour(
			_abilities.at(CharacterAbility::LOKTOFELT_SUCCESS), CharacterAbilityType::PERCENTAGE);
		_add_text(
			piety_c, "{:>2}%", std::to_string(_abilities.at(CharacterAbility::LOKTOFELT_SUCCESS)));

		piety_c.y += _display->window->get_cell_height();
		piety_c.colour = _adjust_ability_colour(
			_abilities.at(CharacterAbility::BASE_DISPELL), CharacterAbilityType::PERCENTAGE);
		_add_text(piety_c, "{:>2}%", std::to_string(_abilities.at(CharacterAbility::BASE_DISPELL)));

		piety_c.y += _display->window->get_cell_height();
		piety_c.colour = _adjust_ability_colour(
			_abilities.at(CharacterAbility::BONUS_PRIEST_SPELLS), CharacterAbilityType::NUMBER);
		_add_text(
			piety_c, "{:>2}", std::to_string(_abilities.at(CharacterAbility::BONUS_PRIEST_SPELLS)));

		Component vitality_c((*_display->layout)["character_detailed:vitality_detailed_values"]);

		_add_text(
			vitality_c, "{:+>2}", std::to_string(_abilities.at(CharacterAbility::VITALITY_BONUS)));
		vitality_c.colour = _adjust_ability_colour(
			_abilities.at(CharacterAbility::VITALITY_BONUS), CharacterAbilityType::MODIFIER);

		vitality_c.y += _display->window->get_cell_height();
		vitality_c.colour = _adjust_ability_colour(
			_abilities.at(CharacterAbility::BONUS_HIT_POINTS), CharacterAbilityType::MODIFIER);
		_add_text(vitality_c, "{:+>2}",
			std::to_string(_abilities.at(CharacterAbility::BONUS_HIT_POINTS)));

		vitality_c.y += _display->window->get_cell_height();
		vitality_c.colour = _adjust_ability_colour(
			_abilities.at(CharacterAbility::DEAD_RESURRECT), CharacterAbilityType::PERCENTAGE);
		_add_text(
			vitality_c, "{:>2}%", std::to_string(_abilities.at(CharacterAbility::DEAD_RESURRECT)));

		vitality_c.y += _display->window->get_cell_height();
		vitality_c.colour = _adjust_ability_colour(
			_abilities.at(CharacterAbility::ASHES_RESURRECT), CharacterAbilityType::PERCENTAGE);
		_add_text(
			vitality_c, "{:>2}%", std::to_string(_abilities.at(CharacterAbility::ASHES_RESURRECT)));

		vitality_c.y += _display->window->get_cell_height();
		vitality_c.colour =
			_adjust_ability_colour(_abilities.at(CharacterAbility::DI_KADORTO_RESURRECT),
				CharacterAbilityType::PERCENTAGE);
		_add_text(vitality_c, "{:>2}%",
			std::to_string(_abilities.at(CharacterAbility::DI_KADORTO_RESURRECT)));

		Component agility_c((*_display->layout)["character_detailed:agility_detailed_values"]);

		agility_c.colour = _adjust_ability_colour(
			_abilities.at(CharacterAbility::INITIATIVE_MODIFIER), CharacterAbilityType::MODIFIER);
		_add_text(agility_c, "{:+>2}",
			std::to_string(_abilities.at(CharacterAbility::INITIATIVE_MODIFIER)));

		agility_c.y += _display->window->get_cell_height();
		agility_c.colour = _adjust_ability_colour(
			_abilities.at(CharacterAbility::BASE_CRITICAL_HIT), CharacterAbilityType::PERCENTAGE);
		_add_text(agility_c, "{:>2}%",
			std::to_string(_abilities.at(CharacterAbility::BASE_CRITICAL_HIT)));

		agility_c.y += _display->window->get_cell_height();
		agility_c.colour = _adjust_ability_colour(
			_abilities.at(CharacterAbility::IDENTIFY_TRAP), CharacterAbilityType::PERCENTAGE);
		_add_text(
			agility_c, "{:>2}%", std::to_string(_abilities.at(CharacterAbility::IDENTIFY_TRAP)));

		agility_c.y += _display->window->get_cell_height();
		agility_c.colour = _adjust_ability_colour(
			_abilities.at(CharacterAbility::BASE_DISARM_TRAP), CharacterAbilityType::PERCENTAGE);
		_add_text(
			agility_c, "{:>2}%", std::to_string(_abilities.at(CharacterAbility::BASE_DISARM_TRAP)));

		agility_c.y += _display->window->get_cell_height();
		agility_c.colour =
			_adjust_ability_colour((100 - _abilities.at(CharacterAbility::ACTIVATE_TRAP)),
				CharacterAbilityType::PERCENTAGE);
		_add_text(agility_c, "{:>2}%",
			std::to_string(100 - _abilities.at(CharacterAbility::ACTIVATE_TRAP)));

		agility_c.y += _display->window->get_cell_height();
		agility_c.colour = _adjust_ability_colour(
			_abilities.at(CharacterAbility::BASE_AVOID_PIT), CharacterAbilityType::PERCENTAGE);
		_add_text(
			agility_c, "{:>2}%", std::to_string(_abilities.at(CharacterAbility::BASE_AVOID_PIT)));

		agility_c.y += _display->window->get_cell_height();
		agility_c.colour = _adjust_ability_colour(
			_abilities.at(CharacterAbility::BASE_ARMOUR_CLASS), CharacterAbilityType::AC);
		_add_text(
			agility_c, "{:>2}", std::to_string(_abilities.at(CharacterAbility::BASE_ARMOUR_CLASS)));

		Component luck_c((*_display->layout)["character_detailed:luck_detailed_values"]);

		luck_c.colour = _adjust_ability_colour(
			_abilities.at(CharacterAbility::BASE_RESIST_BONUS), CharacterAbilityType::PERCENTAGE);
		_add_text(
			luck_c, "{:>2}%", std::to_string(_abilities.at(CharacterAbility::BASE_RESIST_BONUS)));

		luck_c.y += _display->window->get_cell_height();
		luck_c.colour =
			_adjust_ability_colour(_abilities.at(CharacterAbility::EQUIPMENT_INTACT_ON_WIPE),
				CharacterAbilityType::PERCENTAGE);
		_add_text(luck_c, "{:>2}%",
			std::to_string(_abilities.at(CharacterAbility::EQUIPMENT_INTACT_ON_WIPE)));

		Component resistances_c(
			(*_display->layout)["character_detailed:resistances_detailed_values"]);
		int pos_x{resistances_c.x};
		int pos_y{resistances_c.y};
		int offset_columns{std::stoi(resistances_c["offset_columns"].value())};

		resistances_c.colour =
			_adjust_ability_colour(_abilities.at(CharacterAbility::RESISTANCE_VS_CRITICAL_HIT),
				CharacterAbilityType::PERCENTAGE);
		_add_text(resistances_c, "{:>2}%",
			std::to_string(_abilities.at(CharacterAbility::RESISTANCE_VS_CRITICAL_HIT)));

		resistances_c.y += _display->window->get_cell_height();
		resistances_c.colour =
			_adjust_ability_colour(_abilities.at(CharacterAbility::RESISTANCE_VS_POISON_PARALYSIS),
				CharacterAbilityType::PERCENTAGE);
		_add_text(resistances_c, "{:>2}%",
			std::to_string(_abilities.at(CharacterAbility::RESISTANCE_VS_POISON_PARALYSIS)));

		resistances_c.y += _display->window->get_cell_height();
		resistances_c.colour =
			_adjust_ability_colour(_abilities.at(CharacterAbility::RESISTANCE_VS_STONING),
				CharacterAbilityType::PERCENTAGE);
		_add_text(resistances_c, "{:>2}%",
			std::to_string(_abilities.at(CharacterAbility::RESISTANCE_VS_STONING)));

		resistances_c.y += _display->window->get_cell_height();
		resistances_c.colour =
			_adjust_ability_colour(_abilities.at(CharacterAbility::RESISTANCE_VS_BREATH_ATTACKS),
				CharacterAbilityType::PERCENTAGE);
		_add_text(resistances_c, "{:>2}%",
			std::to_string(_abilities.at(CharacterAbility::RESISTANCE_VS_BREATH_ATTACKS)));

		resistances_c.y = pos_y;
		resistances_c.x = pos_x + (offset_columns * _display->window->get_cell_width());
		resistances_c.colour =
			_adjust_ability_colour(_abilities.at(CharacterAbility::RESISTANCE_VS_POISON_GAS_TRAP),
				CharacterAbilityType::PERCENTAGE);
		_add_text(resistances_c, "{:>2}%",
			std::to_string(_abilities.at(CharacterAbility::RESISTANCE_VS_POISON_GAS_TRAP)));

		resistances_c.y += _display->window->get_cell_height();
		resistances_c.colour =
			_adjust_ability_colour(_abilities.at(CharacterAbility::RESISTANCE_VS_MAGE_PRIEST_TRAP),
				CharacterAbilityType::PERCENTAGE);
		_add_text(resistances_c, "{:>2}%",
			std::to_string(_abilities.at(CharacterAbility::RESISTANCE_VS_MAGE_PRIEST_TRAP)));

		resistances_c.y += _display->window->get_cell_height();
		resistances_c.colour = _adjust_ability_colour(
			_abilities.at(CharacterAbility::RECOVER_FROM_SLEEP), CharacterAbilityType::PERCENTAGE);
		_add_text(resistances_c, "{:>2}%",
			std::to_string(_abilities.at(CharacterAbility::RECOVER_FROM_SLEEP)));

		resistances_c.y += _display->window->get_cell_height();
		resistances_c.colour = _adjust_ability_colour(
			_abilities.at(CharacterAbility::RECOVER_FROM_FEAR), CharacterAbilityType::PERCENTAGE);
		_add_text(resistances_c, "{:>2}%",
			std::to_string(_abilities.at(CharacterAbility::RECOVER_FROM_FEAR)));

		resistances_c.y = pos_y;
		resistances_c.x = pos_x + (2 * offset_columns * _display->window->get_cell_width());
		resistances_c.colour =
			_adjust_ability_colour(_abilities.at(CharacterAbility::RESISTANCE_VS_SILENCE),
				CharacterAbilityType::PERCENTAGE);
		_add_text(resistances_c, "{:>2}%",
			std::to_string(_abilities.at(CharacterAbility::RESISTANCE_VS_SILENCE)));

		resistances_c.y += _display->window->get_cell_height();
		resistances_c.colour =
			_adjust_ability_colour(_abilities.at(CharacterAbility::RESISTANCE_VS_KATINO),
				CharacterAbilityType::PERCENTAGE);
		_add_text(resistances_c, "{:>2}%",
			std::to_string(_abilities.at(CharacterAbility::RESISTANCE_VS_KATINO)));

		resistances_c.y += _display->window->get_cell_height();
		resistances_c.colour = _adjust_ability_colour(
			_abilities.at(CharacterAbility::RESISTANCE_VS_BADI), CharacterAbilityType::PERCENTAGE);
		_add_text(resistances_c, "{:>2}%",
			std::to_string(_abilities.at(CharacterAbility::RESISTANCE_VS_BADI)));

		resistances_c.y += _display->window->get_cell_height();
		resistances_c.colour =
			_adjust_ability_colour(_abilities.at(CharacterAbility::RESISTANCE_VS_MANIFO),
				CharacterAbilityType::PERCENTAGE);
		_add_text(resistances_c, "{:>2}%",
			std::to_string(_abilities.at(CharacterAbility::RESISTANCE_VS_MANIFO)));
	} else if (_view == CharacterView::INVENTORY) {

		_display->generate_components("character_inventory", _v_sprites, _v_texts, _v_frames);

		_add_text(
			(*_display->layout)["character_inventory:name_and_summary_text"], "{}", summary_text());

		auto portrait{_get_character_portrait()};
		Component portrait_c{(*_display->layout)["character_strict:portrait"]};
		portrait.setScale(portrait_c.scale, portrait_c.scale);
		const int offset_x = [&] {
			if (portrait_c["offset_x"])
				return std::stoi(portrait_c["offset_x"].value());
			else
				return 0;
		}();
		const int offset_y = [&] {
			if (portrait_c["offset_y"])
				return std::stoi(portrait_c["offset_y"].value());
			else
				return 0;
		}();
		portrait.setPosition(portrait_c.x + offset_x, portrait_c.y + offset_y);
		_v_sprites.emplace(portrait_c.unique_key, portrait);

		Component carried_c((*_display->layout)["character_inventory:inventory_blank"]);
		int c_slots{std::stoi(carried_c["number_of_slots"].value())};
		for (int loop = 0; loop < c_slots; loop++) {
			_add_text(carried_c, "{}", (*_display->string)["INVENTORY_BLANK"]);
			carried_c.y += _display->window->get_cell_height();
		}

	} else if (_view == CharacterView::MAGE_SPELLS) {

		_display->generate_components("character_mage_spells", _v_sprites, _v_texts, _v_frames);

		_add_text((*_display->layout)["character_mage_spells:name_and_summary_text"], "{}",
			summary_text());

		auto portrait{_get_character_portrait()};
		Component portrait_c{(*_display->layout)["character_strict:portrait"]};
		portrait.setScale(portrait_c.scale, portrait_c.scale);
		const int offset_x = [&] {
			if (portrait_c["offset_x"])
				return std::stoi(portrait_c["offset_x"].value());
			else
				return 0;
		}();
		const int offset_y = [&] {
			if (portrait_c["offset_y"])
				return std::stoi(portrait_c["offset_y"].value());
			else
				return 0;
		}();
		portrait.setPosition(portrait_c.x + offset_x, portrait_c.y + offset_y);
		_v_sprites.emplace(portrait_c.unique_key, portrait);

		Component level_c{(*_display->layout)["character_mage_spells:level_label"]};
		Component sp_c{(*_display->layout)["character_mage_spells:spell_points"]};
		Component spell_name_c{(*_display->layout)["character_mage_spells:spell_name_label"]};
		Component spell_t_name_c{
			(*_display->layout)["character_mage_spells:spell_translated_name_label"]};

		int level_x{level_c.x};
		for (unsigned int level = 1; level <= 7; level++) {

			_add_text(level_c, "{}",
				fmt::format("{} {}", (*_display->string)["CHARACTER_SPELL_LEVEL"], level));

			sp_c.x = level_c.x + (std::stoi(sp_c["offset_columns"].value()) *
									 _display->window->get_cell_width());
			sp_c.y = level_c.y;
			_add_text(sp_c, "{}", _get_spell_points_per_level(SpellType::MAGE, level));

			spell_name_c.x = level_c.x;
			spell_name_c.y = level_c.y + _display->window->get_cell_height();

			auto spells = _spells | std::views::filter([&](Spell spell) {
				return (spell.type == SpellType::MAGE) && (spell.level == level);
			});
			for (auto spell : spells) {

				// Add the Spell
				auto spell_name{_add_text(spell_name_c, "{}", spell.name)};
				if (spell.known)
					spell_name->setFillColor(
						sf::Color(std::stoull(spell_name_c["known_colour"].value(), 0, 16)));
				else
					spell_name->setFillColor(
						sf::Color(std::stoull(spell_name_c["unknown_colour"].value(), 0, 16)));

				// And the Spell Category Icon
				Component spell_icon_c{(*_display->layout)["character_mage_spells:spell_icon"]};
				auto spell_icon{_get_spell_icon(spell.category)};
				if (spell_icon) {
					spell_icon.value().setScale(spell_icon_c.scale, spell_icon_c.scale);
					const int offset_x = [&] {
						if (spell_icon_c["offset_x"])
							return std::stoi(spell_icon_c["offset_x"].value());
						else
							return 0;
					}();
					const int offset_y = [&] {
						if (spell_icon_c["offset_y"])
							return std::stoi(spell_icon_c["offset_y"].value());
						else
							return 0;
					}();

					spell_icon.value().setPosition(
						spell_name_c.x + offset_x +
							(std::stoi(spell_icon_c["offset_columns"].value()) *
								_display->window->get_cell_width()),
						spell_name_c.y + offset_y);

					if (spell.known)
						spell_icon->setColor(
							sf::Color(std::stoull(spell_name_c["known_colour"].value(), 0, 16)));
					else
						spell_icon->setColor(
							sf::Color(std::stoull(spell_name_c["unknown_colour"].value(), 0, 16)));

					_v_sprites.emplace(GUID(), spell_icon.value());
				}

				// spell_name_c.y += spell_name_c.size;
				spell_name_c.y += _display->window->get_cell_height();
			}

			if ((level % 3 == 1) || (level % 3 == 2)) {
				level_c.x += (std::stoi(level_c["offset_columns"].value()) *
							  _display->window->get_cell_width());
			} else if (level % 3 == 0) {
				level_c.x = level_x;
				level_c.y += (std::stoi(level_c["offset_rows"].value()) *
							  _display->window->get_cell_height());
			}
		}
	} else if (_view == CharacterView::PRIEST_SPELLS) {

		_display->generate_components("character_priest_spells", _v_sprites, _v_texts, _v_frames);

		_add_text((*_display->layout)["character_priest_spells:name_and_summary_text"], "{}",
			summary_text());

		auto portrait{_get_character_portrait()};
		Component portrait_c{(*_display->layout)["character_strict:portrait"]};
		portrait.setScale(portrait_c.scale, portrait_c.scale);
		const int offset_x = [&] {
			if (portrait_c["offset_x"])
				return std::stoi(portrait_c["offset_x"].value());
			else
				return 0;
		}();
		const int offset_y = [&] {
			if (portrait_c["offset_y"])
				return std::stoi(portrait_c["offset_y"].value());
			else
				return 0;
		}();
		portrait.setPosition(portrait_c.x + offset_x, portrait_c.y + offset_y);
		_v_sprites.emplace(portrait_c.unique_key, portrait);

		Component level_c{(*_display->layout)["character_priest_spells:level_label"]};
		Component sp_c{(*_display->layout)["character_priest_spells:spell_points"]};
		Component spell_name_c{(*_display->layout)["character_priest_spells:spell_name_label"]};
		Component spell_t_name_c{
			(*_display->layout)["character_priest_spells:spell_translated_name_label"]};

		int level_x{level_c.x};
		for (unsigned int level = 1; level <= 7; level++) {

			_add_text(level_c, "{}",
				fmt::format("{} {}", (*_display->string)["CHARACTER_SPELL_LEVEL"], level));

			sp_c.x = level_c.x + (std::stoi(sp_c["offset_columns"].value()) *
									 _display->window->get_cell_width());
			sp_c.y = level_c.y;
			_add_text(sp_c, "{}", _get_spell_points_per_level(SpellType::PRIEST, level));

			spell_name_c.x = level_c.x;
			spell_name_c.y = level_c.y + _display->window->get_cell_height();

			auto spells = _spells | std::views::filter([&](Spell spell) {
				return (spell.type == SpellType::PRIEST) && (spell.level == level);
			});
			for (auto spell : spells) {
				auto spell_name{_add_text(spell_name_c, "{}", spell.name)};
				spell_name->setPosition(spell_name_c.x, spell_name_c.y);
				if (spell.known)
					spell_name->setFillColor(
						sf::Color(std::stoull(spell_name_c["known_colour"].value(), 0, 16)));
				else
					spell_name->setFillColor(
						sf::Color(std::stoull(spell_name_c["unknown_colour"].value(), 0, 16)));

				// And the Spell Category Icon
				Component spell_icon_c{(*_display->layout)["character_mage_spells:spell_icon"]};
				auto spell_icon{_get_spell_icon(spell.category)};
				if (spell_icon) {
					spell_icon.value().setScale(spell_icon_c.scale, spell_icon_c.scale);
					const int offset_x = [&] {
						if (spell_icon_c["offset_x"])
							return std::stoi(spell_icon_c["offset_x"].value());
						else
							return 0;
					}();
					const int offset_y = [&] {
						if (spell_icon_c["offset_y"])
							return std::stoi(spell_icon_c["offset_y"].value());
						else
							return 0;
					}();

					spell_icon.value().setPosition(
						spell_name_c.x + offset_x +
							(std::stoi(spell_icon_c["offset_columns"].value()) *
								_display->window->get_cell_width()),
						spell_name_c.y + offset_y);

					if (spell.known)
						spell_icon->setColor(
							sf::Color(std::stoull(spell_name_c["known_colour"].value(), 0, 16)));
					else
						spell_icon->setColor(
							sf::Color(std::stoull(spell_name_c["unknown_colour"].value(), 0, 16)));

					_v_sprites.emplace(GUID(), spell_icon.value());
				}

				spell_name_c.y += _display->window->get_cell_height();
			}

			if ((level % 3 == 1) || (level % 3 == 2)) {
				level_c.x += (std::stoi(level_c["offset_columns"].value()) *
							  _display->window->get_cell_width());
			} else if (level % 3 == 0) {
				level_c.x = level_x;
				level_c.y += (std::stoi(level_c["offset_rows"].value()) *
							  _display->window->get_cell_height());
			}
		}
	}
}

auto Sorcery::Character::_add_icon(Component &component, std::string icon_key) -> void {

	auto icon = (*_graphics->icons)[icon_key].value();
	const int offset_x = [&] {
		if (component["offset_x"])
			return std::stoi(component["offset_x"].value());
		else
			return 0;
	}();
	const int offset_y = [&] {
		if (component["offset_y"])
			return std::stoi(component["offset_y"].value());
		else
			return 0;
	}();
	icon.setPosition(component.x + offset_x, component.y + offset_y);
	icon.setScale(component.scale, component.scale);
	_v_sprites.emplace(component.unique_key, icon);
}

auto Sorcery::Character::_add_text(
	Component &component, std::string format, std::string value, bool is_view) -> sf::Text * {

	sf::Text text;
	std::string formatted_value{fmt::format(format, value)};
	text.setFont(_system->resources->fonts[component.font]);
	text.setCharacterSize(component.size);
	text.setFillColor(sf::Color(component.colour));
	text.setString(formatted_value);
	const int offset_x = [&] {
		if (component["offset_x"])
			return std::stoi(component["offset_x"].value());
		else
			return 0;
	}();
	const int offset_y = [&] {
		if (component["offset_y"])
			return std::stoi(component["offset_y"].value());
		else
			return 0;
	}();
	text.setPosition(component.x + offset_x, component.y + offset_y);

	// Generate a new key as this is a map, and we might call this with the same base component
	std::string new_unique_key{GUID()};
	if (is_view) {
		_v_texts.emplace(new_unique_key, text);
		return &_v_texts.at(new_unique_key);
	} else {
		_texts.emplace(new_unique_key, text);
		return &_texts.at(new_unique_key);
	}
}

auto Sorcery::Character::get_view() const -> CharacterView {

	return _view;
}

auto Sorcery::Character::_get_spell_icon(SpellCategory category) -> std::optional<sf::Sprite> {

	switch (category) {

	case SpellCategory::ATTACK:
		return (*_graphics->icons)["attack"].value();
		break;
	case SpellCategory::SUPPORT:
		return (*_graphics->icons)["support"].value();
		break;
	case SpellCategory::DISABLE:
		return (*_graphics->icons)["disable"].value();
		break;
	case SpellCategory::FIELD:
		return (*_graphics->icons)["field"].value();
		break;
	case SpellCategory::HEALING:
		return (*_graphics->icons)["healing"].value();
		break;
	default:
		return std::nullopt;
		break;
	}
}

// Setting the view will regenerate the display components
auto Sorcery::Character::set_view(const CharacterView value) -> void {

	_view = value;
	_generate_display();
}

auto Sorcery::Character::get_method() const -> CreateMethod {

	return _method;
}

auto Sorcery::Character::set_method(const CreateMethod value) -> void {

	_method = value;
}

auto Sorcery::Character::draw(sf::RenderTarget &target, sf::RenderStates states) const -> void {

	states.transform *= getTransform();

	// Draw the common components
	for (const auto &[unique_key, frame] : _frames)
		target.draw(*frame, states);

	for (const auto &[unique_key, sprite] : _sprites)
		target.draw(sprite, states);

	for (const auto &[unique_key, text] : _texts)
		target.draw(text, states);

	// Draw the section components
	for (const auto &[unique_key, v_frame] : _v_frames)
		target.draw(*v_frame, states);

	for (const auto &[unique_key, v_sprite] : _v_sprites)
		target.draw(v_sprite, states);

	for (const auto &[unique_key, v_text] : _v_texts)
		target.draw(v_text, states);

	/* // Draw the custom components
	if (_view == CharacterView::MAIN) {
		if (_ad->valid)
			target.draw(*_ad, states);

		if (_ss->valid)
			target.draw(*_ss, states);
	} */
}
