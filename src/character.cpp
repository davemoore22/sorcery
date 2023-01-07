// Copyright (C) 2023 Dave Moore
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

#include "character.hpp"

Sorcery::Character::Character() {
}

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
	_view = CharacterView::NO_VIEW;
	_hl_mage_spell = SpellID::DUMAPIC;
	_hl_priest_spell = SpellID::BADIOS;
	mage_spell_bounds.clear();
	priest_spell_bounds.clear();
	mage_spell_texts.clear();
	priest_spell_texts.clear();

	_hidden = false;
	set_status(CharacterStatus::OK);

	_spell_panel = std::make_shared<SpellPanel>(_system, _display, _graphics);
	_spell_panel->setPosition((*_display->layout)["global:spell_panel"].x, (*_display->layout)["global:spell_panel"].y);

	_version = SAVE_VERSION;

	_legated = false;

	location = CharacterLocation::TAVERN;
	coordinate = std::nullopt;
	depth = std::nullopt;
}

// Note for the copy constuctors we only copy the character data/PODs within
Sorcery::Character::Character(const Character &other)
	: location{other.location}, coordinate{other.coordinate}, depth{other.depth}, _version{other._version},
	  _system{other._system}, _display{other._display}, _graphics{other._graphics}, _abilities{other._abilities},
	  _priest_max_sp{other._priest_max_sp}, _priest_cur_sp{other._priest_cur_sp}, _mage_max_sp{other._mage_max_sp},
	  _mage_cur_sp{other._mage_cur_sp}, _spells{other._spells}, _spells_known{other._spells_known},
	  _current_stage{other._current_stage}, _name{other._name}, _race{other._race}, _class{other._class},
	  _alignment{other._alignment}, _start_attr{other._start_attr}, _cur_attr{other._cur_attr},
	  _max_attr{other._max_attr}, _view{other._view}, _points_left{other._points_left}, _st_points{other._st_points},
	  _pos_classes{other._pos_classes}, _class_list{other._class_list}, _num_pos_classes{other._num_pos_classes},
	  _portrait_index{other._portrait_index}, _status{other._status}, _hidden{other._hidden},
	  _hl_mage_spell{other._hl_mage_spell}, _hl_priest_spell{other._hl_priest_spell}, _legated{other._legated} {

	_spell_panel = other._spell_panel;
	_spell_panel_c = other._spell_panel_c;
}

auto Sorcery::Character::operator=(const Character &other) -> Character & {

	location = other.location;
	coordinate = other.coordinate;
	depth = other.depth;

	_version = other._version;

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
	_hl_mage_spell = other._hl_mage_spell;
	_hl_priest_spell = other._hl_priest_spell;
	_hidden = other._hidden;
	_status = other._status;
	_legated = other._legated;

	_spell_panel = other._spell_panel;
	_spell_panel_c = other._spell_panel_c;

	return *this;
}

// Move Constructors
Sorcery::Character::Character(Character &&other) noexcept {

	if (this != &other) {

		location = other.location;
		coordinate = other.coordinate;
		depth = other.depth;

		_version = other._version;

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
		_hl_mage_spell = other._hl_mage_spell;
		_hl_priest_spell = other._hl_priest_spell;
		_hidden = other._hidden;
		_status = other._status;
		_legated = other._legated;

		_spell_panel = std::move(other._spell_panel);
		_spell_panel_c = std::move(other._spell_panel_c);

		other.location = CharacterLocation::NO_LOCATION;
		other.coordinate = std::nullopt;
		other.depth = std::nullopt;

		other._version = 0;

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
		other._current_stage = CharacterStage::NOT_STARTED;
		other._name.clear();
		other._race = CharacterRace::NO_RACE;
		other._class = CharacterClass::NO_CLASS;
		other._alignment = CharacterAlignment::NO_ALIGN;
		other._start_attr.clear();
		other._cur_attr.clear();
		other._max_attr.clear();
		other._view = CharacterView::NO_VIEW;
		other._points_left = 0;
		other._st_points = 0;
		other._pos_classes.clear();
		other._class_list.clear();
		other._num_pos_classes = 0;
		other._portrait_index = 0;
		other._hl_mage_spell = SpellID::NO_SPELL;
		other._hl_priest_spell = SpellID::NO_SPELL;
		other._hidden = false;
		other._status = CharacterStatus::OK;
		other._legated = false;

		other._spell_panel.reset();
		other._spell_panel_c = Component();
	}
}

auto Sorcery::Character::operator=(Character &&other) noexcept -> Character & {

	if (this != &other) {

		location = other.location;
		coordinate = other.coordinate;
		depth = other.depth;

		_version = other._version;

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
		_hl_mage_spell = other._hl_mage_spell;
		_hl_priest_spell = other._hl_priest_spell;
		_hidden = other._hidden;
		_status = other._status;
		_legated = other._legated;

		_spell_panel = std::move(other._spell_panel);
		_spell_panel_c = std::move(other._spell_panel_c);

		other.location = CharacterLocation::NO_LOCATION;
		other.coordinate = std::nullopt;
		other.depth = std::nullopt;

		other._version = 0;

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
		other._current_stage = CharacterStage::NOT_STARTED;
		other._name.clear();
		other._race = CharacterRace::NO_RACE;
		other._class = CharacterClass::NO_CLASS;
		other._alignment = CharacterAlignment::NO_ALIGN;
		other._start_attr.clear();
		other._cur_attr.clear();
		other._max_attr.clear();
		other._view = CharacterView::NO_VIEW;
		other._points_left = 0;
		other._st_points = 0;
		other._pos_classes.clear();
		other._class_list.clear();
		other._num_pos_classes = 0;
		other._portrait_index = 0;
		other._hidden = false;
		other._status = CharacterStatus::OK;
		other._legated = false;

		other._spell_panel.reset();
		other._spell_panel_c = Component();
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
		_view = NO_VIEW;
		break;
	case REVIEW_AND_CONFIRM:

		// Handle the generation of the Character Display Here
		_view = SUMMARY;
		_generate_display();
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

auto Sorcery::Character::get_name_and_location() const -> std::string {

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

auto Sorcery::Character::inc_hl_spell(SpellType type) -> void {

	if (type == SpellType::MAGE) {
		auto index{magic_enum::enum_integer<SpellID>(_hl_mage_spell)};
		if (index < magic_enum::enum_integer<SpellID>(SpellID::TILTOWAIT)) {
			++index;
			_hl_mage_spell = magic_enum::enum_cast<SpellID>(index).value();
		}
	} else {
		auto index{magic_enum::enum_integer<SpellID>(_hl_priest_spell)};
		if (index < magic_enum::enum_integer<SpellID>(SpellID::MALIKTO)) {
			++index;
			_hl_priest_spell = magic_enum::enum_cast<SpellID>(index).value();
		}
	}

	_generate_display();
}

auto Sorcery::Character::dec_hl_spell(SpellType type) -> void {

	if (type == SpellType::MAGE) {
		auto index{magic_enum::enum_integer<SpellID>(_hl_mage_spell)};
		if (index > magic_enum::enum_integer<SpellID>(SpellID::DUMAPIC)) {
			--index;
			_hl_mage_spell = magic_enum::enum_cast<SpellID>(index).value();
		}
	} else {
		auto index{magic_enum::enum_integer<SpellID>(_hl_priest_spell)};
		if (index > magic_enum::enum_integer<SpellID>(SpellID::BADIOS)) {
			--index;
			_hl_priest_spell = magic_enum::enum_cast<SpellID>(index).value();
		}
	}

	_generate_display();
}

// Setting the view will regenerate the display components
auto Sorcery::Character::left_view() -> void {

	auto view_index{magic_enum::enum_integer<CharacterView>(_view)};
	if (view_index == magic_enum::enum_integer<CharacterView>(CharacterView::SUMMARY))
		view_index = magic_enum::enum_integer<CharacterView>(CharacterView::INVENTORY);
	else
		--view_index;
	_view = magic_enum::enum_cast<CharacterView>(view_index).value();

	_generate_display();
}

// Setting the view will regenerate the display components
auto Sorcery::Character::right_view() -> void {

	auto view_index{magic_enum::enum_integer<CharacterView>(_view)};
	if (view_index == magic_enum::enum_integer<CharacterView>(CharacterView::INVENTORY))
		view_index = magic_enum::enum_integer<CharacterView>(CharacterView::SUMMARY);
	else
		++view_index;
	_view = magic_enum::enum_cast<CharacterView>(view_index).value();

	_generate_display();
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

	// Formula soured from http://www.zimlab.com/wizardry/walk/w123calc.htm
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

auto Sorcery::Character::get_icon(CharacterStage type) -> std::optional<sf::Sprite> {

	using enum Enums::Character::Stage;

	switch (type) {
	case CHOOSE_ALIGNMENT: {
		auto alignment{get_alignment(_alignment)};
		std::transform(alignment.begin(), alignment.end(), alignment.begin(), ::tolower);
		return (*_graphics->icons)[alignment].value();
	} break;
	case CHOOSE_RACE: {
		auto race{get_race(_race)};
		std::transform(race.begin(), race.end(), race.begin(), ::tolower);
		return (*_graphics->icons)[race].value();
	} break;
	case CHOOSE_CLASS: {
		auto cclass{get_class(_class)};
		std::transform(cclass.begin(), cclass.end(), cclass.begin(), ::tolower);
		return (*_graphics->icons)[cclass].value();
	} break;
	default:
		break;
	}

	return std::nullopt;
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
auto Sorcery::Character::get_alignment(CharacterAlignment character_alignment) const -> std::string {

	using enum Enums::Character::Align;

	switch (character_alignment) {
	case GOOD:
		return (*_display->string)["CHARACTER_ALIGNMENT_GOOD"];
		break;
	case NEUTRAL:
		return (*_display->string)["CHARACTER_ALIGNMENT_NEUTRAL"];
		break;
	case EVIL:
		return (*_display->string)["CHARACTER_ALIGNMENT_EVIL"];
		break;
	default:
		return "";
		break;
	}
}

auto Sorcery::Character::get_race(CharacterRace character_race) const -> std::string {

	using enum Enums::Character::Race;

	switch (character_race) {
	case HUMAN:
		return (*_display->string)["CHARACTER_RACE_HUMAN"];
		break;
	case ELF:
		return (*_display->string)["CHARACTER_RACE_ELF"];
		break;
	case DWARF:
		return (*_display->string)["CHARACTER_RACE_DWARF"];
		break;
	case GNOME:
		return (*_display->string)["CHARACTER_RACE_GNOME"];
		break;
	case HOBBIT:
		return (*_display->string)["CHARACTER_RACE_HOBBIT"];
		break;
	default:
		return "";
		break;
	}
}

auto Sorcery::Character::get_class(CharacterClass character_class) const -> std::string {

	using enum Enums::Character::Class;

	switch (character_class) {
	case FIGHTER:
		return (*_display->string)["CHARACTER_CLASS_FIGHTER"];
		break;
	case MAGE:
		return (*_display->string)["CHARACTER_CLASS_MAGE"];
		break;
	case PRIEST:
		return (*_display->string)["CHARACTER_CLASS_PRIEST"];
		break;
	case THIEF:
		return (*_display->string)["CHARACTER_CLASS_THIEF"];
		break;
	case BISHOP:
		return (*_display->string)["CHARACTER_CLASS_BISHOP"];
		break;
	case SAMURAI:
		return (*_display->string)["CHARACTER_CLASS_SAMURAI"];
		break;
	case LORD:
		return (*_display->string)["CHARACTER_CLASS_LORD"];
		break;
	case NINJA:
		return (*_display->string)["CHARACTER_CLASS_NINJA"];
		break;
	default:
		return "";
		break;
	}
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
	location = CharacterLocation::TAVERN;
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
	set_status(CharacterStatus::OK);
	_legated = true;

	location = CharacterLocation::TAVERN;
	coordinate = std::nullopt;
	depth = std::nullopt;
}

// Change Class
auto Sorcery::Character::change_class(const CharacterClass &value) -> void {

	if (_class != value) {
		_class = value;

		_regenerate_start_info();
		_generate_secondary_abil(false, true, false);
		_reset_start_spells(); // TODO: check this, not sure this works properly
		_reset_starting_sp();

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
		_abilities[IDENTIFY_TRAP] = 0;
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

	// This is taken from "KEEPCHYN" which hard codes the spells known to beginning characters!
	switch (_class) { // NOLINT(clang-diagnostic-switch)
	case BISHOP:
	case MAGE:
		_learn_spell(SpellID::KATINO);
		_learn_spell(SpellID::HALITO);
		break;
	case PRIEST:
		_learn_spell(SpellID::DIOS);
		_learn_spell(SpellID::BADIOS);
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
		location = CharacterLocation::TRAINING;
	}

	return results;
}

// Level a character down (e.g. drain levels or give/increase negative levels_
auto Sorcery::Character::level_down() -> void {

	using enum Enums::Character::Ability;

	if (_abilities.at(CURRENT_LEVEL) == 1) {
		_status = CharacterStatus::LOST;
		location = CharacterLocation::TRAINING;
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

	_abilities[NEXT_LEVEL_XP] = _get_xp_for_level(_abilities.at(CURRENT_LEVEL));
	_abilities[CURRENT_XP] = _abilities[NEXT_LEVEL_XP] - 1; // not sure about this TODO

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
auto Sorcery::Character::_calculate_sp(SpellType spell_type, unsigned int level_mod, unsigned int level_offset)
	-> void {

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
	// though note that the orignal is bugged and selects the wrong level spells sometimes)
	for (auto spell_level = 1; spell_level <= 7; spell_level++) {

		if ((_priest_max_sp[spell_level] > 0) && (_get_spells_known(SpellType::PRIEST, spell_level) == 0)) {
			switch (spell_level) {
			case 1:
				_learn_spell(SpellID::BADIOS);
				break;
			case 2:
				_learn_spell(SpellID::MONTINO);
				new_spells_learnt = true;
				break;
			case 3:
				if ((*_system->random)[RandomType::D100] > 33)
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
				if ((*_system->random)[RandomType::D100] > 33)
					_learn_spell(SpellID::BADIALMA);
				else
					_learn_spell(SpellID::BADI);
				new_spells_learnt = true;
				break;
			case 6:
				if ((*_system->random)[RandomType::D100] > 33)
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
		if ((_mage_max_sp[spell_level] > 0) && (_get_spells_known(SpellType::MAGE, spell_level) == 0)) {
			switch (spell_level) {
			case 1:
				if ((*_system->random)[RandomType::D100] > 33)
					_learn_spell(SpellID::KATINO);
				else
					_learn_spell(SpellID::HALITO);
				new_spells_learnt = true;
				break;
			case 2:
				if ((*_system->random)[RandomType::D100] > 33)
					_learn_spell(SpellID::DILTO);
				else
					_learn_spell(SpellID::SOPIC);
				new_spells_learnt = true;
				break;
			case 3:
				if ((*_system->random)[RandomType::D100] > 33)
					_learn_spell(SpellID::MOLITO);
				else
					_learn_spell(SpellID::MAHALITO);
				new_spells_learnt = true;
				break;
			case 4:
				if ((*_system->random)[RandomType::D100] > 33)
					_learn_spell(SpellID::DALTO);
				else
					_learn_spell(SpellID::LAHALITO);
				new_spells_learnt = true;
				break;
			case 5:
				if ((*_system->random)[RandomType::D100] > 33)
					_learn_spell(SpellID::MAMORLIS);
				else
					_learn_spell(SpellID::MADALTO);
				new_spells_learnt = true;
				break;
			case 6:
				if ((*_system->random)[RandomType::D100] > 33)
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
	// for example, 134586 is stored at &0002013C as EA 11 0D, or 4586 - to get the actuaL value for the level we add
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
	auto c_index{magic_enum::enum_integer<CharacterClass>(_class) - 1};
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
	_spells.emplace_back(SpellID::DUMAPIC, SpellType::MAGE, SpellCategory::FIELD, level, false, "DUMAPIC", "Clarity",
		"Restablishes the party's bearings and shows their location in the maze.");
	_spells.emplace_back(SpellID::HALITO, SpellType::MAGE, SpellCategory::ATTACK, level, false, "HALITO", "Little Fire",
		"Flings a a ball of fire at one foe inflicting 1d8 points of fire damage.");
	_spells.emplace_back(SpellID::KATINO, SpellType::MAGE, SpellCategory::DISABLE, level, false, "KATINO", "Bad Air",
		"Temporarily puts to sleep one group of foes.");
	_spells.emplace_back(SpellID::MOGREF, SpellType::MAGE, SpellCategory::SUPPORT, level, false, "MOGREF", "Body Iron",
		"Grants a -2 bonus to armour class to the caster for the duration of combat.");

	// Level 2
	++level;
	_spells.emplace_back(SpellID::DILTO, SpellType::MAGE, SpellCategory::DISABLE, level, false, "DILTO", "Darkness",
		"Causes one group of monsters to be enveloped in darkness lowering their defense.");
	_spells.emplace_back(SpellID::SOPIC, SpellType::MAGE, SpellCategory::SUPPORT, level, false, "SOPIC", "Glass",
		"Causes the caster to become transparent, granting a -4 bonus to armour class to the caster for the "
		"duration "
		"of combat");

	// Level 3
	++level;
	_spells.emplace_back(SpellID::MAHALITO, SpellType::MAGE, SpellCategory::ATTACK, level, false, "MAHALITO",
		"Big Fire", "Causes an explosion in a group of foes, inflicting 3d8 points of fire damage.");
	_spells.emplace_back(SpellID::MOLITO, SpellType::MAGE, SpellCategory::ATTACK, level, false, "MOLITO", "Spark Storm",
		"Causes sparks to damage half of the foes in a group for 3d6 points of damage");

	// Level 4
	++level;
	_spells.emplace_back(SpellID::DALTO, SpellType::MAGE, SpellCategory::ATTACK, level, false, "DALTO", "Blizzard",
		"Inflicts 6d6 points of cold damage to a group of foes.");
	_spells.emplace_back(SpellID::LAHALITO, SpellType::MAGE, SpellCategory::ATTACK, level, false, "LAHALITO", "Torch",
		"Inflicts 6d6 points of fire damage to a group of foes.");
	_spells.emplace_back(SpellID::MORLIS, SpellType::MAGE, SpellCategory::DISABLE, level, false, "MORLIS", "Fear",
		"Causes a group of foes to fear the party, lowering their defense twice as much as Dilto.");

	// Level 5
	++level;
	_spells.emplace_back(SpellID::MADALTO, SpellType::MAGE, SpellCategory::ATTACK, level, false, "MADALTO", "Frost",
		"Inflicts 8d8 points of cold damage to a group of foes.");
	_spells.emplace_back(SpellID::MAKANITO, SpellType::MAGE, SpellCategory::ATTACK, level, false, "MAKANITO",
		"Deadly Air", "Kills any foes of less than 8th level.");
	_spells.emplace_back(SpellID::MAMORLIS, SpellType::MAGE, SpellCategory::DISABLE, level, false, "MAMORLIS", "Terror",
		"Causes all foes to fear the party");

	// Level 6
	++level;
	_spells.emplace_back(SpellID::HAMAN, SpellType::MAGE, SpellCategory::SUPPORT, level, false, "HAMAN", "Change",
		"Causes random but beneficial effects to the entire party but the caster loses one level of experience.");
	_spells.emplace_back(SpellID::LAKANITO, SpellType::MAGE, SpellCategory::ATTACK, level, false, "LAKANITO",
		"Ice Storm", "Kills all foes affected by this spell; some types of foes are immune.");
	_spells.emplace_back(SpellID::MASOPIC, SpellType::MAGE, SpellCategory::ATTACK, level, false, "MASOPIC", "Big Glass",
		"Grants of a bonus of -4 to the armor class of the entire party for the duration of combat");
	_spells.emplace_back(SpellID::ZILWAN, SpellType::MAGE, SpellCategory::ATTACK, level, false, "ZILWAN", "Dispel",
		"Inflicts 200d10 points of positive energy damage to one undead creature.");

	// Level 7
	++level;
	_spells.emplace_back(SpellID::MAHAMAN, SpellType::MAGE, SpellCategory::SUPPORT, level, false, "MAHAMAN",
		"Great Change",
		"Causes random but beneficial major effects to the entire party but the caster loses one level and the "
		"spell "
		"is forgotten.");
	_spells.emplace_back(SpellID::MALOR, SpellType::MAGE, SpellCategory::FIELD, level, false, "MALOR", "Apport",
		"Teleports the party to a random nearby location when cast in combat, but to a specified location when "
		"cast "
		"outside of combat.");
	_spells.emplace_back(SpellID::TILTOWAIT, SpellType::MAGE, SpellCategory::ATTACK, level, false, "TILTOWAIT",
		"Explosion", "Inflicts 10d15 points of fire and force damage to all foes.");

	// Priest Spells (grouped by level)

	// Level 1
	level = 1u;
	_spells.emplace_back(SpellID::BADIOS, SpellType::PRIEST, SpellCategory::ATTACK, level, false, "BADIOS", "Harm",
		"Inflicts 1d8 points of negative energy damage to one foe.");
	_spells.emplace_back(SpellID::DIOS, SpellType::PRIEST, SpellCategory::HEALING, level, false, "DIOS", "Heal",
		"Restores 1d8 hp to a party member.");
	_spells.emplace_back(SpellID::KALKI, SpellType::PRIEST, SpellCategory::SUPPORT, level, false, "KALKI", "Blessings",
		"Grants a -1 bonus to armour class to the entire party for the duration of combat.");
	_spells.emplace_back(SpellID::MILWA, SpellType::PRIEST, SpellCategory::FIELD, level, false, "MILWA", "Light",
		"Causes a softly glowing light to follow the party, increasing vision and revealing some secret doors for "
		"15d2 "
		"turns.");
	_spells.emplace_back(SpellID::PORFIC, SpellType::PRIEST, SpellCategory::SUPPORT, level, false, "PORFIC", "Shield",
		"Grants a -4 bonus to armour class to the caster for the duration of combat.");

	// Level 2
	++level;
	_spells.emplace_back(SpellID::CALFO, SpellType::PRIEST, SpellCategory::FIELD, level, false, "CALFO", "X-Ray Vision",
		"Allows the caster to identify the trap on a chest with 95% accuracy.");
	_spells.emplace_back(SpellID::MANIFO, SpellType::PRIEST, SpellCategory::DISABLE, level, false, "MANIFO", "Statue",
		"Causes ones group of foes to become temporarily paralyzed.");
	_spells.emplace_back(SpellID::MATU, SpellType::PRIEST, SpellCategory::SUPPORT, level, false, "MATU", "Blessing",
		"Lowers armor class of all party members by 2 during combat.");
	_spells.emplace_back(SpellID::MONTINO, SpellType::PRIEST, SpellCategory::DISABLE, level, false, "MONTINO",
		"Still Air", "Stills the air around a group of foes, making it impossible for them to cast spells.");

	// Level 3
	++level;
	_spells.emplace_back(SpellID::BAMATU, SpellType::PRIEST, SpellCategory::SUPPORT, level, false, "BAMATU", "Prayer",
		"Grants a -4 bonus to armour class to the entire party for the duration of combat.");
	_spells.emplace_back(SpellID::DIALKO, SpellType::PRIEST, SpellCategory::HEALING, level, false, "DIALKO", "Softness",
		"Cures one party member of paralysis, silence, or sleep.");
	_spells.emplace_back(SpellID::LATUMAPIC, SpellType::PRIEST, SpellCategory::FIELD, level, false, "LATUMAPIC",
		"Identify", "Full identifies unknown foes.");
	_spells.emplace_back(SpellID::LOMILWA, SpellType::PRIEST, SpellCategory::FIELD, level, false, "LOMILWA",
		"More Light",
		"Extends the party's field of vision and reveals most secret doors. Lasts until leaving the maze or "
		"entering "
		"an area of magical darkness.");

	// Level 4
	++level;
	_spells.emplace_back(SpellID::BADIAL, SpellType::PRIEST, SpellCategory::ATTACK, level, false, "BADIAL", "More Hurt",
		"Inflicts 2d8 points of negative energy damage to one foe.");
	_spells.emplace_back(SpellID::DIAL, SpellType::PRIEST, SpellCategory::HEALING, level, false, "DIAL", "More Heal",
		"Restores 2d8 hp to a party member.");
	_spells.emplace_back(SpellID::LATUMOFIS, SpellType::PRIEST, SpellCategory::HEALING, level, false, "LATUMOFIS",
		"Cure Poison", "Cures a party member of poisoning.");
	_spells.emplace_back(SpellID::MAPORFIC, SpellType::PRIEST, SpellCategory::FIELD, level, false, "MAPORFIC",
		"Big Shield",
		"Grants a -2 bonus to armour class to the entire party. Lasts until leaving the maze or dispelled");

	// Level 5
	++level;
	_spells.emplace_back(SpellID::BADI, SpellType::PRIEST, SpellCategory::ATTACK, level, false, "BADI", "Death",
		"Attempts to slay one foe outright.");
	_spells.emplace_back(SpellID::BADIALMA, SpellType::PRIEST, SpellCategory::ATTACK, level, false, "BADIALMA",
		"Great Hurt", "Causes 3d8 points of damage to one foe");
	_spells.emplace_back(SpellID::DI, SpellType::PRIEST, SpellCategory::HEALING, level, false, "DI", "Life",
		"Attempts to resurrect a dead party member. If it succeeds, the party member has 1 hp, and loses 1 point "
		"of "
		"vitality. If it fails, the dead member is turned to ashes.");
	_spells.emplace_back(SpellID::DIALMA, SpellType::PRIEST, SpellCategory::HEALING, level, false, "DIALMA",
		"Great Heal", "Restores 3d8 hp to a party member.");
	_spells.emplace_back(SpellID::KANDI, SpellType::PRIEST, SpellCategory::FIELD, level, false, "KANDI", "Locate Soul",
		"Gives the direction of the creature the party is attempting to locate; the location is relative to the "
		"position of the caster.");

	// Level 6
	++level;
	_spells.emplace_back(SpellID::LOKTOFEIT, SpellType::PRIEST, SpellCategory::FIELD, level, false, "LOKTOFEIT",
		"Recall",
		"Causes all party members to be transported back to the castle, minus all of their equipment and most of "
		"their "
		"gold");
	_spells.emplace_back(SpellID::LORTO, SpellType::PRIEST, SpellCategory::ATTACK, level, false, "LORTO", "Blades",
		"Causes sharp blades to slice through a group of foes, causing 6d6 points of damage");
	_spells.emplace_back(SpellID::MABADI, SpellType::PRIEST, SpellCategory::ATTACK, level, false, "MABADI", "Harming",
		"Attempts to drain all but 1d8 hp from a foe");
	_spells.emplace_back(SpellID::MADI, SpellType::PRIEST, SpellCategory::HEALING, level, false, "MADI", "Healing",
		"Fills a party member with positive energy, causeing all hp to be restored to a party member and curing "
		"any "
		"condition except death.");

	// Level 7
	++level;
	_spells.emplace_back(SpellID::KADORTO, SpellType::PRIEST, SpellCategory::HEALING, level, false, "KADORTO",
		"Resurrection",
		"Restores the dead to life, and restores all hp and cures all conditions, even if the party member is "
		"ashes. "
		"If the attempt fails, the character is lost forever.");
	_spells.emplace_back(SpellID::MALIKTO, SpellType::PRIEST, SpellCategory::ATTACK, level, false, "MALIKTO",
		"Word of Death", "Inflicts 12d6 points of damage to all foes.");
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

auto Sorcery::Character::_get_character_portrait() -> sf::Sprite {

	sf::Sprite portrait{_graphics->textures->get(_portrait_index, GraphicsTextureType::PORTRAIT).value()};

	return portrait;
}

auto Sorcery::Character::get_status() const -> CharacterStatus {

	return _status;
}

auto Sorcery::Character::get_condition() const -> std::string {

	return _get_condition();
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

auto Sorcery::Character::get_location_string() const -> std::string {

	using enum Enums::Character::Location;

	switch (location) {
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
		auto candidate{magic_enum::enum_integer<CharacterStatus>(value)};
		auto current{magic_enum::enum_integer<CharacterStatus>(_status)};
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

auto Sorcery::Character::get_hp_summary() const -> std::string {

	using enum Enums::Character::Ability;

	return fmt::format("{}/{}{}", std::to_string(_abilities.at(CURRENT_HP)), std::to_string(_abilities.at(MAX_HP)),
		get_hp_adjustment_symbol());
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

auto Sorcery::Character::_generate_summary_icons() -> void {

	using enum Enums::Character::Ability;
	using enum Enums::Character::Stage;

	auto class_icon{get_icon(CHOOSE_CLASS).value()};
	class_icon.setPosition(
		(*_display->layout)["character:class_icon"].x, (*_display->layout)["character:class_icon"].y);
	class_icon.setScale(
		(*_display->layout)["character:class_icon"].scale, (*_display->layout)["character:class_icon"].scale);
	_v_sprites.emplace((*_display->layout)["character:class_icon"].unique_key, class_icon);

	auto race_icon{get_icon(CHOOSE_RACE).value()};
	race_icon.setPosition((*_display->layout)["character:race_icon"].x, (*_display->layout)["character:race_icon"].y);
	race_icon.setScale(
		(*_display->layout)["character:race_icon"].scale, (*_display->layout)["character:race_icon"].scale);
	_v_sprites.emplace((*_display->layout)["character:race_icon"].unique_key, race_icon);

	auto alignment_icon{get_icon(CHOOSE_ALIGNMENT).value()};
	alignment_icon.setPosition(
		(*_display->layout)["character:alignment_icon"].x, (*_display->layout)["character:alignment_icon"].y);
	alignment_icon.setScale(
		(*_display->layout)["character:alignment_icon"].scale, (*_display->layout)["character:alignment_icon"].scale);
	_v_sprites.emplace((*_display->layout)["character:alignment_icon"].unique_key, alignment_icon);

	auto level_icon{(*_graphics->icons)["level"].value()};
	level_icon.setPosition(
		(*_display->layout)["character:level_icon"].x, (*_display->layout)["character:level_icon"].y);
	level_icon.setScale(
		(*_display->layout)["character:level_icon"].scale, (*_display->layout)["character:level_icon"].scale);
	_v_sprites.emplace((*_display->layout)["character:level_icon"].unique_key, level_icon);

	_add_text((*_display->layout)["character:level_text"], "{}", std::to_string(_abilities.at(CURRENT_LEVEL)), true);
}

auto Sorcery::Character::get_summary() -> std::string {

	using enum Enums::Character::Ability;

	auto name{_name};
	std::transform(name.begin(), name.end(), name.begin(), ::toupper);
	return fmt::format("{:<15} L {:>2} {}-{} {}", name, _abilities.at(CURRENT_LEVEL),
		get_alignment(_alignment).substr(0, 1), get_class(_class).substr(0, 3), get_race(_race).substr(0, 3));
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
	const std::string indicator{can_level() ? "*" : " "};
	std::transform(name.begin(), name.end(), name.begin(), ::toupper);
	return fmt::format("{} {:<15} {:>2}{} {}-{} {:>3} {:>8} {:^7}", position, name, _abilities.at(CURRENT_LEVEL),
		indicator, get_alignment(_alignment).substr(0, 1), get_class(_class).substr(0, 3),
		_abilities.at(CURRENT_ARMOUR_CLASS), get_hp_summary(), _get_condition());
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
	std::transform(name.begin(), name.end(), name.begin(), ::toupper);
	auto legacy{_legated ? " (D)" : ""};
	switch (_current_stage) {
	case CHOOSE_METHOD:
	case ENTER_NAME:
		return fmt::format("{:<15} L ?? ?-??? ???", "???");
		break;
	case CHOOSE_RACE:
		return fmt::format("{:<15} L {:>2} ?-??? ???", name, _abilities.at(CURRENT_LEVEL));
		break;
	case CHOOSE_ALIGNMENT:
		return fmt::format("{:<15} L {:>2} ?-??? {}", name, _abilities.at(CURRENT_LEVEL), get_race(_race));
		break;
	case ALLOCATE_STATS:
		return fmt::format("{:<15} L {:>2} {}-??? {}", name, _abilities.at(CURRENT_LEVEL),
			get_alignment(_alignment).substr(0, 1), get_race(_race));
		break;
	case CHOOSE_CLASS:
		return fmt::format("{:<15} L {:>2} {}-??? {}", name, _abilities.at(CURRENT_LEVEL),
			get_alignment(_alignment).substr(0, 1), get_race(_race));
		break;
	case CHOOSE_PORTRAIT:
		return fmt::format("{:<15} L {:>2} {}-{} {}", name, _abilities.at(CURRENT_LEVEL),
			get_alignment(_alignment).substr(0, 1), get_class(_class).substr(0, 3), get_race(_race));
		break;
	case REVIEW_AND_CONFIRM:
	case COMPLETED:
		return fmt::format("{} L {:>2} {}-{} {}{}", name, _abilities.at(CURRENT_LEVEL),
			get_alignment(_alignment).substr(0, 1), get_class(_class).substr(0, 3), get_race(_race), legacy);
		break;
	default:
		return "";
		break;
	}

	// starting equipment

	// priest, bishop: staff and robes
	// fighter: longsword leather armour
	// bishop: staff and robes
	// mage: dagger robes
	// thief: short sword leather armour
	// samurai, lord, ninja?
}

auto Sorcery::Character::_generate_display() -> void {

	using enum Enums::Character::Attribute;
	using enum Enums::Character::Ability;
	using enum Enums::Character::Ability_Type;
	using enum Enums::Character::View;

	_sprites.clear();
	_texts.clear();
	_frames.clear();
	_v_sprites.clear();
	_v_texts.clear();
	_v_frames.clear();
	mage_spell_bounds.clear();
	priest_spell_bounds.clear();
	mage_spell_texts.clear();
	priest_spell_texts.clear();

	_display->generate("character", _sprites, _texts, _frames);

	if (_view == SUMMARY) {

		_display->generate("character_summary", _v_sprites, _v_texts, _v_frames);

		_add_text((*_display->layout)["character_summary:name_and_summary_text"], "{}", summary_text());

		auto portrait{_get_character_portrait()};
		Component portrait_c{(*_display->layout)["character_summary:portrait"]};
		portrait.setScale(portrait_c.scale, portrait_c.scale);
		const auto offset_x{[&] {
			if (portrait_c["offset_x"])
				return std::stoi(portrait_c["offset_x"].value());
			else
				return 0;
		}()};
		const auto offset_y{[&] {
			if (portrait_c["offset_y"])
				return std::stoi(portrait_c["offset_y"].value());
			else
				return 0;
		}()};
		portrait.setPosition(portrait_c.x + offset_x, portrait_c.y + offset_y);
		_v_sprites.emplace(portrait_c.unique_key, portrait);

		Component s_c{(*_display->layout)["character_summary:strength_value"]};
		s_c.colour = _graphics->adjust_colour(_cur_attr.at(STRENGTH), STAT);
		_add_text(s_c, "{:>2}", std::to_string(_cur_attr.at(STRENGTH)));

		Component i_c{(*_display->layout)["character_summary:iq_value"]};
		i_c.colour = _graphics->adjust_colour(_cur_attr.at(IQ), STAT);
		_add_text(i_c, "{:>2}", std::to_string(_cur_attr.at(IQ)));

		Component p_c{(*_display->layout)["character_summary:piety_value"]};
		p_c.colour = _graphics->adjust_colour(_cur_attr.at(PIETY), STAT);
		_add_text(p_c, "{:>2}", std::to_string(_cur_attr.at(PIETY)));

		Component a_c{(*_display->layout)["character_summary:agility_value"]};
		a_c.colour = _graphics->adjust_colour(_cur_attr.at(AGILITY), STAT);
		_add_text(a_c, "{:>2}", std::to_string(_cur_attr.at(AGILITY)));

		Component v_c{(*_display->layout)["character_summary:vitality_value"]};
		v_c.colour = _graphics->adjust_colour(_cur_attr.at(VITALITY), STAT);
		_add_text(v_c, "{:>2}", std::to_string(_cur_attr.at(VITALITY)));

		Component l_c{(*_display->layout)["character_summary:luck_value"]};
		l_c.colour = _graphics->adjust_colour(_cur_attr.at(LUCK), STAT);
		_add_text(l_c, "{:>2}", std::to_string(_cur_attr.at(LUCK)));

		_add_text((*_display->layout)["character_summary:hp_value"], "{}",
			fmt::format("{}/{}", std::to_string(_abilities.at(CURRENT_HP)), std::to_string(_abilities.at(MAX_HP))));
		_add_text((*_display->layout)["character_summary:ac_value"], "{}",
			std::to_string(_abilities.at(CURRENT_ARMOUR_CLASS)));
		_add_text((*_display->layout)["character_summary:age_value"], "{}",
			std::to_string(static_cast<int>(_abilities.at(AGE) / 52)));
		_add_text((*_display->layout)["character_summary:swim_value"], "{}", std::to_string(_abilities.at(SWIM)));
		auto status_text{_add_text((*_display->layout)["character_summary:status_value"], "{}", get_status_string())};
		status_text->setFillColor(sf::Color(_graphics->adjust_status_colour(_status, is_poisoned())));

		_add_text((*_display->layout)["character_summary:exp_value"], "{}", std::to_string(_abilities.at(CURRENT_XP)));
		_add_text(
			(*_display->layout)["character_summary:next_value"], "{}", std::to_string(_abilities.at(NEXT_LEVEL_XP)));
		_add_text((*_display->layout)["character_summary:gold_value"], "{}", std::to_string(_abilities.at(GOLD)));
		_add_text((*_display->layout)["character_summary:marks_value"], "{}", std::to_string(_abilities.at(MARKS)));
		_add_text((*_display->layout)["character_summary:deaths_value"], "{}", std::to_string(_abilities.at(DEATHS)));

		auto mage_spells{fmt::format("{}/{}/{}/{}/{}/{}/{}", _mage_cur_sp.at(1), _mage_cur_sp.at(2), _mage_cur_sp.at(3),
			_mage_cur_sp.at(4), _mage_cur_sp.at(5), _mage_cur_sp.at(6), _mage_cur_sp.at(7))};
		auto priest_spells{
			fmt::format("{}/{}/{}/{}/{}/{}/{}", _priest_cur_sp.at(1), _priest_cur_sp.at(2), _priest_cur_sp.at(3),
				_priest_cur_sp.at(4), _priest_cur_sp.at(5), _priest_cur_sp.at(6), _priest_cur_sp.at(7))};

		_add_text((*_display->layout)["character_summary:mage_spells"], "{}", mage_spells);
		_add_text((*_display->layout)["character_summary:priest_spells"], "{}", priest_spells);

	} else if (_view == DETAILED) {

		_display->generate("character_detailed", _v_sprites, _v_texts, _v_frames);

		_add_text((*_display->layout)["character_detailed:name_and_summary_text"], "{}", summary_text());

		auto portrait{_get_character_portrait()};
		Component portrait_c{(*_display->layout)["character_summary:portrait"]};
		portrait.setScale(portrait_c.scale, portrait_c.scale);
		const auto offset_x{[&] {
			if (portrait_c["offset_x"])
				return std::stoi(portrait_c["offset_x"].value());
			else
				return 0;
		}()};
		const auto offset_y{[&] {
			if (portrait_c["offset_y"])
				return std::stoi(portrait_c["offset_y"].value());
			else
				return 0;
		}()};
		portrait.setPosition(portrait_c.x + offset_x, portrait_c.y + offset_y);
		_v_sprites.emplace(portrait_c.unique_key, portrait);

		Component s_c{(*_display->layout)["character_detailed:strength_value"]};
		s_c.colour = _graphics->adjust_colour(_cur_attr.at(STRENGTH), STAT);
		_add_text(s_c, "{:>2}", std::to_string(_cur_attr.at(STRENGTH)));

		Component i_c{(*_display->layout)["character_detailed:iq_value"]};
		i_c.colour = _graphics->adjust_colour(_cur_attr.at(IQ), STAT);
		_add_text(i_c, "{:>2}", std::to_string(_cur_attr.at(IQ)));

		Component p_c{(*_display->layout)["character_detailed:piety_value"]};
		p_c.colour = _graphics->adjust_colour(_cur_attr.at(PIETY), STAT);
		_add_text(p_c, "{:>2}", std::to_string(_cur_attr.at(PIETY)));

		Component a_c{(*_display->layout)["character_detailed:agility_value"]};
		a_c.colour = _graphics->adjust_colour(_cur_attr.at(AGILITY), STAT);
		_add_text(a_c, "{:>2}", std::to_string(_cur_attr.at(AGILITY)));

		Component v_c{(*_display->layout)["character_detailed:vitality_value"]};
		v_c.colour = _graphics->adjust_colour(_cur_attr.at(VITALITY), STAT);
		_add_text(v_c, "{:>2}", std::to_string(_cur_attr.at(VITALITY)));

		Component l_c{(*_display->layout)["character_detailed:luck_value"]};
		l_c.colour = _graphics->adjust_colour(_cur_attr.at(LUCK), STAT);
		_add_text(l_c, "{:>2}", std::to_string(_cur_attr.at(LUCK)));

		Component strength_c((*_display->layout)["character_detailed:strength_detailed_values"]);

		strength_c.colour = _graphics->adjust_colour(_abilities.at(ATTACK_MODIFIER), MODIFIER);
		_add_text(strength_c, "{:+>2}", std::to_string(_abilities.at(ATTACK_MODIFIER)));

		strength_c.y += _display->window->get_ch();
		strength_c.colour = _graphics->adjust_colour(_abilities.at(HIT_PROBABILITY), MODIFIER);
		_add_text(strength_c, "{:+>2}", std::to_string(_abilities.at(HIT_PROBABILITY)));

		strength_c.y += _display->window->get_ch();
		strength_c.colour = _graphics->adjust_colour(_abilities.at(BONUS_DAMAGE), MODIFIER);
		_add_text(strength_c, "{:+>2}", std::to_string(_abilities.at(BONUS_DAMAGE)));

		strength_c.y += _display->window->get_ch();
		strength_c.colour = _graphics->adjust_colour(_abilities.at(BASE_NUMBER_OF_ATTACKS), NUMBER);
		_add_text(strength_c, "{:>2}", std::to_string(_abilities.at(BASE_NUMBER_OF_ATTACKS)));

		strength_c.y += _display->window->get_ch();
		strength_c.colour = _graphics->adjust_colour(_abilities.at(UNARMED_DAMAGE), NUMBER);
		_add_text(strength_c, "{:>2}", std::to_string(_abilities.at(UNARMED_DAMAGE)));

		Component iq_c((*_display->layout)["character_detailed:iq_detailed_values"]);

		iq_c.colour = _graphics->adjust_colour(_abilities.at(MAGE_SPELL_LEARN), PERCENTAGE);
		_add_text(iq_c, "{:>2}%", std::to_string(_abilities.at(MAGE_SPELL_LEARN)));

		iq_c.y += _display->window->get_ch();
		iq_c.colour = _graphics->adjust_colour(_abilities.at(IDENTIFY_ITEMS), PERCENTAGE);
		_add_text(iq_c, "{:>2}%", std::to_string(_abilities.at(IDENTIFY_ITEMS)));

		iq_c.y += _display->window->get_ch();
		iq_c.colour = _graphics->adjust_colour(_abilities.at(IDENTIFY_CURSE), PERCENTAGE);
		_add_text(iq_c, "{:>2}%", std::to_string(_abilities.at(IDENTIFY_CURSE)));

		iq_c.y += _display->window->get_ch();
		iq_c.colour = _graphics->adjust_colour(_abilities.at(IDENTIFY_FOES), PERCENTAGE);
		_add_text(iq_c, "{:>2}%", std::to_string(_abilities.at(IDENTIFY_FOES)));

		iq_c.y += _display->window->get_ch();
		iq_c.colour = _graphics->adjust_colour(_abilities.at(BONUS_MAGE_SPELLS), NUMBER);
		_add_text(iq_c, "{:>2}", std::to_string(_abilities.at(BONUS_MAGE_SPELLS)));

		Component piety_c((*_display->layout)["character_detailed:piety_detailed_values"]);
		piety_c.colour = _graphics->adjust_colour(_abilities.at(PRIEST_SPELL_LEARN), PERCENTAGE);
		_add_text(piety_c, "{:>2}%", std::to_string(_abilities.at(PRIEST_SPELL_LEARN)));

		piety_c.y += _display->window->get_ch();
		piety_c.colour = _graphics->adjust_colour(_abilities.at(LOKTOFELT_SUCCESS), PERCENTAGE);
		_add_text(piety_c, "{:>2}%", std::to_string(_abilities.at(LOKTOFELT_SUCCESS)));

		piety_c.y += _display->window->get_ch();
		piety_c.colour = _graphics->adjust_colour(_abilities.at(BASE_DISPELL), PERCENTAGE);
		_add_text(piety_c, "{:>2}%", std::to_string(_abilities.at(BASE_DISPELL)));

		piety_c.y += _display->window->get_ch();
		piety_c.colour = _graphics->adjust_colour(_abilities.at(BONUS_PRIEST_SPELLS), NUMBER);
		_add_text(piety_c, "{:>2}", std::to_string(_abilities.at(BONUS_PRIEST_SPELLS)));

		Component vitality_c((*_display->layout)["character_detailed:vitality_detailed_values"]);
		vitality_c.colour = _graphics->adjust_colour(_abilities.at(VITALITY_BONUS), MODIFIER);
		_add_text(vitality_c, "{:+>2}", std::to_string(_abilities.at(VITALITY_BONUS)));

		vitality_c.y += _display->window->get_ch();
		vitality_c.colour = _graphics->adjust_colour(_abilities.at(BONUS_HIT_POINTS), MODIFIER);
		_add_text(vitality_c, "{:+>2}", std::to_string(_abilities.at(BONUS_HIT_POINTS)));

		vitality_c.y += _display->window->get_ch();
		vitality_c.colour = _graphics->adjust_colour(_abilities.at(DEAD_RESURRECT), PERCENTAGE);
		_add_text(vitality_c, "{:>2}%", std::to_string(_abilities.at(DEAD_RESURRECT)));

		vitality_c.y += _display->window->get_ch();
		vitality_c.colour = _graphics->adjust_colour(_abilities.at(ASHES_RESURRECT), PERCENTAGE);
		_add_text(vitality_c, "{:>2}%", std::to_string(_abilities.at(ASHES_RESURRECT)));

		vitality_c.y += _display->window->get_ch();
		vitality_c.colour = _graphics->adjust_colour(_abilities.at(DI_KADORTO_RESURRECT), PERCENTAGE);
		_add_text(vitality_c, "{:>2}%", std::to_string(_abilities.at(DI_KADORTO_RESURRECT)));

		Component agility_c((*_display->layout)["character_detailed:agility_detailed_values"]);

		agility_c.colour = _graphics->adjust_colour(_abilities.at(INITIATIVE_MODIFIER), MODIFIER);
		_add_text(agility_c, "{:+>2}", std::to_string(_abilities.at(INITIATIVE_MODIFIER)));

		agility_c.y += _display->window->get_ch();
		agility_c.colour = _graphics->adjust_colour(_abilities.at(BASE_CRITICAL_HIT), PERCENTAGE);
		_add_text(agility_c, "{:>2}%", std::to_string(_abilities.at(BASE_CRITICAL_HIT)));

		agility_c.y += _display->window->get_ch();
		agility_c.colour = _graphics->adjust_colour(_abilities.at(IDENTIFY_TRAP), PERCENTAGE);
		_add_text(agility_c, "{:>2}%", std::to_string(_abilities.at(IDENTIFY_TRAP)));

		agility_c.y += _display->window->get_ch();
		agility_c.colour = _graphics->adjust_colour(_abilities.at(BASE_DISARM_TRAP), PERCENTAGE);
		_add_text(agility_c, "{:>2}%", std::to_string(_abilities.at(BASE_DISARM_TRAP)));

		agility_c.y += _display->window->get_ch();
		agility_c.colour = _graphics->adjust_colour((100 - _abilities.at(ACTIVATE_TRAP)), PERCENTAGE);
		_add_text(agility_c, "{:>2}%", std::to_string(100 - _abilities.at(ACTIVATE_TRAP)));

		agility_c.y += _display->window->get_ch();
		agility_c.colour = _graphics->adjust_colour(_abilities.at(BASE_AVOID_PIT), PERCENTAGE);
		_add_text(agility_c, "{:>2}%", std::to_string(_abilities.at(BASE_AVOID_PIT)));

		agility_c.y += _display->window->get_ch();
		agility_c.colour = _graphics->adjust_colour(_abilities.at(BASE_ARMOUR_CLASS), AC);
		_add_text(agility_c, "{:>2}", std::to_string(_abilities.at(BASE_ARMOUR_CLASS)));

		Component luck_c((*_display->layout)["character_detailed:luck_detailed_values"]);

		luck_c.colour = _graphics->adjust_colour(_abilities.at(BASE_RESIST_BONUS) * 5, PERCENTAGE);
		_add_text(luck_c, "{:>2}%", std::to_string(_abilities.at(BASE_RESIST_BONUS) * 5));

		luck_c.y += _display->window->get_ch();
		luck_c.colour = _graphics->adjust_colour(_abilities.at(EQUIPMENT_INTACT_ON_WIPE), PERCENTAGE);
		_add_text(luck_c, "{:>2}%", std::to_string(_abilities.at(EQUIPMENT_INTACT_ON_WIPE)));

		Component resistances_c((*_display->layout)["character_detailed:resistances_detailed_values"]);
		auto pos_x{resistances_c.x};
		auto pos_y{resistances_c.y};
		auto offset_columns{std::stoi(resistances_c["offset_columns"].value())};

		resistances_c.colour = _graphics->adjust_colour(_abilities.at(RESISTANCE_VS_CRITICAL_HIT) * 5, PERCENTAGE);
		_add_text(resistances_c, "{:>2}%", std::to_string(_abilities.at(RESISTANCE_VS_CRITICAL_HIT) * 5));

		resistances_c.y += _display->window->get_ch();
		resistances_c.colour = _graphics->adjust_colour(_abilities.at(RESISTANCE_VS_POISON_PARALYSIS) * 5, PERCENTAGE);
		_add_text(resistances_c, "{:>2}%", std::to_string(_abilities.at(RESISTANCE_VS_POISON_PARALYSIS) * 5));

		resistances_c.y += _display->window->get_ch();
		resistances_c.colour = _graphics->adjust_colour(_abilities.at(RESISTANCE_VS_STONING) * 5, PERCENTAGE);
		_add_text(resistances_c, "{:>2}%", std::to_string(_abilities.at(RESISTANCE_VS_STONING) * 5));

		resistances_c.y += _display->window->get_ch();
		resistances_c.colour = _graphics->adjust_colour(_abilities.at(RESISTANCE_VS_BREATH_ATTACKS) * 5, PERCENTAGE);
		_add_text(resistances_c, "{:>2}%", std::to_string(_abilities.at(RESISTANCE_VS_BREATH_ATTACKS) * 5));

		resistances_c.y = pos_y;
		resistances_c.x = pos_x + (offset_columns * _display->window->get_cw());
		resistances_c.colour = _graphics->adjust_colour(_abilities.at(RESISTANCE_VS_POISON_GAS_TRAP) * 5, PERCENTAGE);
		_add_text(resistances_c, "{:>2}%", std::to_string(_abilities.at(RESISTANCE_VS_POISON_GAS_TRAP) * 5));

		resistances_c.y += _display->window->get_ch();
		resistances_c.colour = _graphics->adjust_colour(_abilities.at(RESISTANCE_VS_MAGE_PRIEST_TRAP) * 5, PERCENTAGE);
		_add_text(resistances_c, "{:>2}%", std::to_string(_abilities.at(RESISTANCE_VS_MAGE_PRIEST_TRAP) * 5));

		resistances_c.y += _display->window->get_ch();
		resistances_c.colour = _graphics->adjust_colour(_abilities.at(RECOVER_FROM_SLEEP), PERCENTAGE);
		_add_text(resistances_c, "{:>2}%", std::to_string(_abilities.at(RECOVER_FROM_SLEEP)));

		resistances_c.y += _display->window->get_ch();
		resistances_c.colour = _graphics->adjust_colour(_abilities.at(RECOVER_FROM_FEAR), PERCENTAGE);
		_add_text(resistances_c, "{:>2}%", std::to_string(_abilities.at(RECOVER_FROM_FEAR)));

		resistances_c.y = pos_y;
		resistances_c.x = pos_x + (2 * offset_columns * _display->window->get_cw());
		resistances_c.colour = _graphics->adjust_colour(_abilities.at(RESISTANCE_VS_SILENCE) * 5, PERCENTAGE);
		_add_text(resistances_c, "{:>2}%", std::to_string(_abilities.at(RESISTANCE_VS_SILENCE) * 5));

		resistances_c.y += _display->window->get_ch();
		resistances_c.colour = _graphics->adjust_colour(_abilities.at(RESISTANCE_VS_KATINO), PERCENTAGE);
		_add_text(resistances_c, "{:>2}%", std::to_string(_abilities.at(RESISTANCE_VS_KATINO)));

		resistances_c.y += _display->window->get_ch();
		resistances_c.colour = _graphics->adjust_colour(_abilities.at(RESISTANCE_VS_BADI), PERCENTAGE);
		_add_text(resistances_c, "{:>2}%", std::to_string(_abilities.at(RESISTANCE_VS_BADI)));

		resistances_c.y += _display->window->get_ch();
		resistances_c.colour = _graphics->adjust_colour(_abilities.at(RESISTANCE_VS_MANIFO), PERCENTAGE);
		_add_text(resistances_c, "{:>2}%", std::to_string(_abilities.at(RESISTANCE_VS_MANIFO)));
	} else if (_view == INVENTORY) {

		_display->generate("character_inventory", _v_sprites, _v_texts, _v_frames);

		_add_text((*_display->layout)["character_inventory:name_and_summary_text"], "{}", summary_text());

		auto portrait{_get_character_portrait()};
		Component portrait_c{(*_display->layout)["character_summary:portrait"]};
		portrait.setScale(portrait_c.scale, portrait_c.scale);
		const auto offset_x{[&] {
			if (portrait_c["offset_x"])
				return std::stoi(portrait_c["offset_x"].value());
			else
				return 0;
		}()};
		const auto offset_y{[&] {
			if (portrait_c["offset_y"])
				return std::stoi(portrait_c["offset_y"].value());
			else
				return 0;
		}()};
		portrait.setPosition(portrait_c.x + offset_x, portrait_c.y + offset_y);
		_v_sprites.emplace(portrait_c.unique_key, portrait);

		Component carried_c((*_display->layout)["character_inventory:inventory_blank"]);
		auto c_slots{std::stoi(carried_c["number_of_slots"].value())};
		for (auto loop = 0; loop < c_slots; loop++) {
			_add_text(carried_c, "{}", (*_display->string)["INVENTORY_BLANK"]);
			carried_c.y += _display->window->get_ch();
		}

	} else if (_view == CharacterView::MAGE_SPELLS) {

		_display->generate("character_mage_spells", _v_sprites, _v_texts, _v_frames);

		_add_text((*_display->layout)["character_mage_spells:name_and_summary_text"], "{}", summary_text());

		auto portrait{_get_character_portrait()};
		Component portrait_c{(*_display->layout)["character_summary:portrait"]};
		portrait.setScale(portrait_c.scale, portrait_c.scale);
		const auto offset_x{[&] {
			if (portrait_c["offset_x"])
				return std::stoi(portrait_c["offset_x"].value());
			else
				return 0;
		}()};
		const auto offset_y{[&] {
			if (portrait_c["offset_y"])
				return std::stoi(portrait_c["offset_y"].value());
			else
				return 0;
		}()};
		portrait.setPosition(portrait_c.x + offset_x, portrait_c.y + offset_y);
		_v_sprites.emplace(portrait_c.unique_key, portrait);

		Component level_c{(*_display->layout)["character_mage_spells:level_label"]};
		Component sp_c{(*_display->layout)["character_mage_spells:spell_points"]};
		Component spell_name_c{(*_display->layout)["character_mage_spells:spell_name_label"]};

		auto level_x{level_c.x};
		for (auto level = 1; level <= 7; level++) {

			_add_text(level_c, "{}", fmt::format("{} {}", (*_display->string)["CHARACTER_SPELL_LEVEL"], level));

			sp_c.x = level_c.x + (std::stoi(sp_c["offset_columns"].value()) * _display->window->get_cw());
			sp_c.y = level_c.y;
			_add_text(sp_c, "{}", _get_sp_per_level(SpellType::MAGE, level));

			spell_name_c.x = level_c.x;
			spell_name_c.y = level_c.y + _display->window->get_ch();

			auto spells{_spells | std::views::filter([&](Spell spell) {
				return (spell.type == SpellType::MAGE) && (static_cast<int>(spell.level) == level);
			})};
			for (auto spell : spells) {

				// Add the Spell
				auto spell_name_text{PADSTR(spell.name, 13)};
				auto spell_name{_add_text(spell_name_c, "{}", spell_name_text)};
				auto hl_bounds{spell_name->getGlobalBounds()};
				mage_spell_bounds[spell.id] = hl_bounds;
				mage_spell_texts[spell.id] = spell_name;

				if (spell.id == _hl_mage_spell) {
					sf::RectangleShape bg(sf::Vector2f(
						std::stoi(spell_name_c["bar_width"].value()) * _display->window->get_cw(), hl_bounds.height));
					bg.setPosition(hl_bounds.left, hl_bounds.top);
					bg.setFillColor(_graphics->animation->selected_colour);
					spell_name->setFillColor(sf::Color(spell_name_c.colour));
					spell_name->setOutlineColor(sf::Color(0, 0, 0));
					spell_name->setOutlineThickness(2);
					_hl_mage_spell_bg = bg;
				} else {

					if (spell.known)
						spell_name->setFillColor(sf::Color(std::stoull(spell_name_c["known_colour"].value(), 0, 16)));
					else
						spell_name->setFillColor(sf::Color(std::stoull(spell_name_c["unknown_colour"].value(), 0, 16)));
				}

				// And the Spell Category Icon
				Component spell_icon_c{(*_display->layout)["character_mage_spells:spell_icon"]};
				auto spell_icon{_get_spell_icon(spell.category)};
				if (spell_icon) {
					spell_icon.value().setScale(spell_icon_c.scale, spell_icon_c.scale);
					const auto offset_x{[&] {
						if (spell_icon_c["offset_x"])
							return std::stoi(spell_icon_c["offset_x"].value());
						else
							return 0;
					}()};
					const auto offset_y{[&] {
						if (spell_icon_c["offset_y"])
							return std::stoi(spell_icon_c["offset_y"].value());
						else
							return 0;
					}()};

					spell_icon.value().setPosition(
						spell_name_c.x + offset_x +
							(std::stoi(spell_icon_c["offset_columns"].value()) * _display->window->get_cw()),
						spell_name_c.y + offset_y);

					if (spell.known)
						spell_icon->setColor(sf::Color(std::stoull(spell_name_c["known_colour"].value(), 0, 16)));
					else
						spell_icon->setColor(sf::Color(std::stoull(spell_name_c["unknown_colour"].value(), 0, 16)));

					_v_sprites.emplace(GUID(), spell_icon.value());
				}

				spell_name_c.y += _display->window->get_ch();
			}

			if ((level % 3 == 1) || (level % 3 == 2))
				level_c.x += (std::stoi(level_c["offset_columns"].value()) * _display->window->get_cw());
			else if (level % 3 == 0) {
				level_c.x = level_x;
				level_c.y += (std::stoi(level_c["offset_rows"].value()) * _display->window->get_ch());
			}
		}
	} else if (_view == PRIEST_SPELLS) {

		_display->generate("character_priest_spells", _v_sprites, _v_texts, _v_frames);

		_add_text((*_display->layout)["character_priest_spells:name_and_summary_text"], "{}", summary_text());

		auto portrait{_get_character_portrait()};
		Component portrait_c{(*_display->layout)["character_summary:portrait"]};
		portrait.setScale(portrait_c.scale, portrait_c.scale);
		const auto offset_x{[&] {
			if (portrait_c["offset_x"])
				return std::stoi(portrait_c["offset_x"].value());
			else
				return 0;
		}()};
		const auto offset_y{[&] {
			if (portrait_c["offset_y"])
				return std::stoi(portrait_c["offset_y"].value());
			else
				return 0;
		}()};
		portrait.setPosition(portrait_c.x + offset_x, portrait_c.y + offset_y);
		_v_sprites.emplace(portrait_c.unique_key, portrait);

		Component level_c{(*_display->layout)["character_priest_spells:level_label"]};
		Component sp_c{(*_display->layout)["character_priest_spells:spell_points"]};
		Component spell_name_c{(*_display->layout)["character_priest_spells:spell_name_label"]};

		auto level_x{level_c.x};
		for (auto level = 1; level <= 7; level++) {

			_add_text(level_c, "{}", fmt::format("{} {}", (*_display->string)["CHARACTER_SPELL_LEVEL"], level));

			sp_c.x = level_c.x + (std::stoi(sp_c["offset_columns"].value()) * _display->window->get_cw());
			sp_c.y = level_c.y;
			_add_text(sp_c, "{}", _get_sp_per_level(SpellType::PRIEST, level));

			spell_name_c.x = level_c.x;
			spell_name_c.y = level_c.y + _display->window->get_ch();

			auto spells{_spells | std::views::filter([&](Spell spell) {
				return (spell.type == SpellType::PRIEST) && (static_cast<int>(spell.level) == level);
			})};
			for (auto spell : spells) {

				auto spell_name_text{PADSTR(spell.name, 13)};
				auto spell_name{_add_text(spell_name_c, "{}", spell_name_text)};
				spell_name->setPosition(spell_name_c.x, spell_name_c.y);
				auto hl_bounds{spell_name->getGlobalBounds()};
				priest_spell_bounds[spell.id] = hl_bounds;
				priest_spell_texts[spell.id] = spell_name;

				if (spell.id == _hl_priest_spell) {
					sf::RectangleShape bg(sf::Vector2f(
						std::stoi(spell_name_c["bar_width"].value()) * _display->window->get_cw(), hl_bounds.height));
					bg.setPosition(hl_bounds.left, hl_bounds.top);
					bg.setFillColor(_graphics->animation->selected_colour);
					spell_name->setFillColor(sf::Color(spell_name_c.colour));
					spell_name->setOutlineColor(sf::Color(0, 0, 0));
					spell_name->setOutlineThickness(2);
					_hl_priest_spell_bg = bg;
				} else {
					if (spell.known)
						spell_name->setFillColor(sf::Color(std::stoull(spell_name_c["known_colour"].value(), 0, 16)));
					else
						spell_name->setFillColor(sf::Color(std::stoull(spell_name_c["unknown_colour"].value(), 0, 16)));
				}

				// And the Spell Category Icon
				Component spell_icon_c{(*_display->layout)["character_mage_spells:spell_icon"]};
				auto spell_icon{_get_spell_icon(spell.category)};
				if (spell_icon) {
					spell_icon.value().setScale(spell_icon_c.scale, spell_icon_c.scale);
					const auto offset_x{[&] {
						if (spell_icon_c["offset_x"])
							return std::stoi(spell_icon_c["offset_x"].value());
						else
							return 0;
					}()};
					const auto offset_y{[&] {
						if (spell_icon_c["offset_y"])
							return std::stoi(spell_icon_c["offset_y"].value());
						else
							return 0;
					}()};

					spell_icon.value().setPosition(
						spell_name_c.x + offset_x +
							(std::stoi(spell_icon_c["offset_columns"].value()) * _display->window->get_cw()),
						spell_name_c.y + offset_y);

					if (spell.known)
						spell_icon->setColor(sf::Color(std::stoull(spell_name_c["known_colour"].value(), 0, 16)));
					else
						spell_icon->setColor(sf::Color(std::stoull(spell_name_c["unknown_colour"].value(), 0, 16)));

					_v_sprites.emplace(GUID(), spell_icon.value());
				}

				spell_name_c.y += _display->window->get_ch();
			}

			if ((level % 3 == 1) || (level % 3 == 2))
				level_c.x += (std::stoi(level_c["offset_columns"].value()) * _display->window->get_cw());
			else if (level % 3 == 0) {
				level_c.x = level_x;
				level_c.y += (std::stoi(level_c["offset_rows"].value()) * _display->window->get_ch());
			}
		}
	}
}

auto Sorcery::Character::_add_icon(Component &component, std::string icon_key) -> void {

	auto icon{(*_graphics->icons)[icon_key].value()};
	const auto offset_x{[&] {
		if (component["offset_x"])
			return std::stoi(component["offset_x"].value());
		else
			return 0;
	}()};
	const auto offset_y{[&] {
		if (component["offset_y"])
			return std::stoi(component["offset_y"].value());
		else
			return 0;
	}()};
	icon.setPosition(component.x + offset_x, component.y + offset_y);
	icon.setScale(component.scale, component.scale);
	_v_sprites.emplace(component.unique_key, icon);
}

auto Sorcery::Character::_add_text(Component &component, std::string format, std::string value, bool is_view)
	-> sf::Text * {

	sf::Text text{};

	// Note that Format v8 needs the format string wrapped in fmt::runtime - this isn't available in < v8 - see
	// https://github.com/fmtlib/fmt/issues/2438 - check FMT_VERSION version in fmt/core.h
	auto formatted_value{fmt::format(fmt::runtime(format), value)};
	text.setFont(_system->resources->fonts[component.font]);
	text.setCharacterSize(component.size);
	text.setFillColor(sf::Color(component.colour));
	text.setString(formatted_value);
	const auto offset_x{[&] {
		if (component["offset_x"])
			return std::stoi(component["offset_x"].value());
		else
			return 0;
	}()};
	const auto offset_y{[&] {
		if (component["offset_y"])
			return std::stoi(component["offset_y"].value());
		else
			return 0;
	}()};
	text.setPosition(component.x + offset_x, component.y + offset_y);

	// Generate a new key as this is a map, and we might call this with the same base component
	auto new_unique_key{GUID()};
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

	using enum Enums::Magic::SpellCategory;

	switch (category) {
	case ATTACK:
		return (*_graphics->icons)["attack"].value();
		break;
	case SUPPORT:
		return (*_graphics->icons)["support"].value();
		break;
	case DISABLE:
		return (*_graphics->icons)["disable"].value();
		break;
	case FIELD:
		return (*_graphics->icons)["field"].value();
		break;
	case HEALING:
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

auto Sorcery::Character::check_for_mouse_move(sf::Vector2f mouse_pos) -> std::optional<SpellID> {

	using enum Enums::Character::View;

	const sf::Vector2f global_pos{this->getPosition()};
	const sf::Vector2f local_mouse_pos{mouse_pos - global_pos};
	if (_view == MAGE_SPELLS) {

		auto it{std::find_if(mage_spell_bounds.begin(), mage_spell_bounds.end(),
			[&local_mouse_pos](const auto &item) { return item.second.contains(local_mouse_pos); })};
		if (it != mage_spell_bounds.end()) {
			Component spell_name_c{(*_display->layout)["character_mage_spells:spell_name_label"]};
			_hl_mage_spell = (*it).first;

			std::vector<Spell>::iterator sit;
			sit = std::find_if(_spells.begin(), _spells.end(), [&](auto item) { return item.id == _hl_mage_spell; });
			if (sit != _spells.end())
				_spell_panel->set(*sit);

			return (*it).first;
		} else
			return std::nullopt;

	} else if (_view == PRIEST_SPELLS) {
		auto it{std::find_if(priest_spell_bounds.begin(), priest_spell_bounds.end(),
			[&local_mouse_pos](const auto &item) { return item.second.contains(local_mouse_pos); })};
		if (it != priest_spell_bounds.end()) {
			Component spell_name_c{(*_display->layout)["character_priest_spells:spell_name_label"]};
			_hl_priest_spell = (*it).first;

			std::vector<Spell>::iterator sit;
			sit = std::find_if(_spells.begin(), _spells.end(), [&](auto item) { return item.id == _hl_priest_spell; });
			if (sit != _spells.end())
				_spell_panel->set(*sit);

			return (*it).first;
		} else
			return std::nullopt;
	} else
		return std::nullopt;
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

auto Sorcery::Character::update() -> void {

	_hl_mage_spell_bg.setFillColor(_graphics->animation->selected_colour);
	_hl_priest_spell_bg.setFillColor(_graphics->animation->selected_colour);
}

auto Sorcery::Character::draw(sf::RenderTarget &target, sf::RenderStates states) const -> void {

	using enum Enums::Character::View;

	states.transform *= getTransform();

	// Draw the common components
	for (const auto &[unique_key, frame] : _frames)
		target.draw(*frame, states);

	for (const auto &[unique_key, sprite] : _sprites)
		target.draw(sprite, states);

	for (const auto &[unique_key, text] : _texts)
		target.draw(text, states);

	if (_view == MAGE_SPELLS)
		target.draw(_hl_mage_spell_bg, states);
	else if (_view == PRIEST_SPELLS)
		target.draw(_hl_priest_spell_bg, states);

	// Draw the section components
	for (const auto &[unique_key, v_frame] : _v_frames)
		target.draw(*v_frame, states);

	for (const auto &[unique_key, v_sprite] : _v_sprites)
		target.draw(v_sprite, states);

	for (const auto &[unique_key, v_text] : _v_texts)
		target.draw(v_text, states);

	// And the preview panels
	if (_view == MAGE_SPELLS)
		target.draw(*_spell_panel, states);

	if (_view == PRIEST_SPELLS)
		target.draw(*_spell_panel, states);
}

namespace Sorcery {

auto operator<<(std::ostream &out_stream, const Sorcery::Character &character) -> std::ostream & {

	auto name{character.get_name()};
	auto cclass{character.get_class()};
	auto alignment{character.get_alignment()};
	auto hp{character.get_hp_summary()};

	std::transform(name.begin(), name.end(), name.begin(), ::toupper);

	auto body{fmt::format("{:<15} {:>2} {}-{} {:>3} {:>6} {:^10}", name, character.get_level(),
		character.get_alignment(alignment).substr(0, 1), character.get_class(cclass).substr(0, 3),
		character.get_cur_ac(), character.get_hp_summary(), character.get_condition())};

	return out_stream << body << std::endl;
}
} // namespace Sorcery
