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
// be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
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

#pragma once

// clang-format off
#include "core/display.hpp"
#include "core/graphics.hpp"
#include "main.hpp"
#include "types/item.hpp"
#include "types/inventory.hpp"
#include "core/system.hpp"

// clang-format on

namespace Sorcery {

class Game;

class Character {

	public:

		// Constructors
		Character();
		Character(System *system, Display *display, Graphics *graphics);

		// Destructor
		~Character() = default;

		// Overloaded Operators
		auto operator[](const CharacterAbility &key) -> int &;
		auto friend operator<<(std::ostream &out_stream, const Character &character) -> std::ostream &;

		// Serialisation
		template <class Archive> auto serialize(Archive &archive) -> void {
			archive(_location, coordinate, depth, _version, _name, _race, _class, _alignment, _start_attr, _cur_attr,
				_max_attr, _st_points, _portrait_index, _abilities, _priest_max_sp, _priest_cur_sp, _mage_max_sp,
				_mage_cur_sp, _status, _hidden, _spells_known, _legated, inventory);
		}

		// Public Methods
		auto reset(const CharacterStage stage) -> void;
		auto get_stage() const -> CharacterStage;
		auto set_stage(const CharacterStage stage) -> void;
		auto set_pos_class() -> void;
		auto finalise() -> void;
		auto level_up() -> std::string;
		auto level_down() -> void;
		auto alignment_to_str(const CharacterAlignment alignment) const -> std::string;
		auto race_to_str(const CharacterRace race) const -> std::string;
		auto class_to_str(const CharacterClass cclass) const -> std::string;
		auto create_random() -> void;
		auto create_quick() -> void;
		auto create_class_alignment(const CharacterClass cclass, const CharacterAlignment alignment) -> void;
		auto set_start_attr() -> void;
		auto get_name() const -> std::string;
		auto get_name_and_loc() const -> std::string;
		auto get_name_and_status() const -> std::string;
		auto set_name(std::string_view value) -> void;
		auto get_race() const -> CharacterRace;
		auto set_race(const CharacterRace &value) -> void;
		auto get_location() const -> CharacterLocation;
		auto set_location(const CharacterLocation value) -> void;
		auto get_level() const -> int;
		auto set_level(const int &value) -> void;
		auto get_class() const -> CharacterClass;
		auto set_class(const CharacterClass &value) -> void;
		auto get_alignment() const -> CharacterAlignment;
		auto set_alignment(const CharacterAlignment &value) -> void;
		auto get_cur_attr() const -> CharacterAttributes;
		auto get_pos_class() const -> CharacterClassQualified;
		auto get_start_attr() const -> CharacterAttributes;
		auto get_points_left() const -> unsigned int;
		auto set_points_left(const unsigned int &value) -> void;
		auto get_start_points() const -> unsigned int;
		auto get_num_pos_class() const -> unsigned int;
		auto set_start_points(const unsigned int &value) -> void;
		auto get_condition() const -> std::string;
		auto get_short_cond() const -> std::string;
		auto get_cur_attr(const CharacterAttribute attribute) const -> unsigned int;
		auto get_start_attr(const CharacterAttribute attribute) const -> unsigned int;
		auto set_cur_attr(const CharacterAttribute attribute, const int adjustment) -> void;
		auto get_portrait_index() const -> unsigned int;
		auto set_portrait_index(const unsigned int value) -> void;
		auto get_spell_points(const SpellType type, const SpellPointStatus status) const -> std::optional<SpellPoints>;
		auto get_gold() const -> unsigned int;
		auto set_gold(const unsigned int value) -> void;
		auto grant_gold(const int value) -> void;
		auto summary_text() -> std::string;
		auto get_method() const -> CreateMethod;
		auto set_method(const CreateMethod value) -> void;
		auto get_summary() -> std::string;
		auto create_spells() -> void;
		auto reset_spells() -> void;
		auto replenish_spells() -> void;
		auto set_spells() -> void;
		auto get_status() const -> CharacterStatus;
		auto get_status_string() const -> std::string;
		auto get_loc_str() const -> std::string;
		auto get_summary_and_out() -> std::string;
		auto set_status(CharacterStatus value) -> void;
		auto is_poisoned() const -> bool;
		auto get_poisoned_rate() const -> int;
		auto set_poisoned_rate(int value) -> void;
		auto get_poisoned_string() const -> std::string;
		auto get_hp_summary() const -> std::string;
		auto get_short_hp_summary() const -> std::string;
		auto change_class(const CharacterClass &value) -> void;
		auto legate(const CharacterAlignment &value) -> void;
		auto is_legated() const -> bool;
		auto get_version() const -> int;
		auto get_sb_text(const int position) -> std::string;
		auto get_current_hp() const -> int;
		auto get_max_hp() const -> int;
		auto set_current_hp(const int hp) -> void;
		auto get_hp_adjustment() const -> int;
		auto set_hp_gain_per_turn(const int adjustment) -> void;
		auto set_hp_loss_per_turn(const int adjustment) -> void;
		auto reset_adjustment_per_turn() -> void;
		auto get_hp_adjustment_symbol() const -> char;
		auto get_cur_ac() const -> int;
		auto get_cur_xp() const -> int;
		auto get_next_xp() const -> int;
		auto get_age() const -> int;
		auto set_age(const int adjustment) -> void;
		auto grant_xp(const int adjustment) -> int;
		auto can_level() const -> bool;
		auto get_cure_cost() const -> unsigned int;
		auto get_ress_chance(bool ashes) -> unsigned int;
		auto damage(const unsigned int adjustment) -> bool;
		auto heal(const unsigned int adjustment) -> void;
		auto attributes() -> CharacterAttributes &;
		auto abilities() -> CharacterAbilities &;
		auto spells() -> std::vector<Spell> &;
		auto priest_cur_sp() -> SpellPoints &;
		auto mage_cur_sp() -> SpellPoints &;
		auto priest_max_sp() -> SpellPoints &;
		auto mage_max_sp() -> SpellPoints &;
		// auto add_starting_equipment(Game *game) -> void;

		// Public Members
		std::optional<Coordinate> coordinate;
		std::optional<int> depth;
		Inventory inventory;

	private:

		// Private Methods
		auto _generate_start_info() -> void;
		auto _regenerate_start_info() -> void;
		auto _legate_start_info() -> void;
		auto _generate_secondary_abil(bool initial, bool change_class, bool legate) -> void;
		auto _set_starting_sp() -> void;
		auto _reset_starting_sp() -> void;
		auto _clear_sp() -> void;
		auto _set_start_spells() -> void;
		auto _reset_start_spells() -> void;
		auto _get_hp_per_level() -> int;
		auto _update_hp_for_level() -> int;
		auto _try_learn_spell(SpellType spell_type, unsigned int spell_level) -> bool;
		auto _calculate_sp(SpellType spell_type, unsigned int level_mod, unsigned int level_offset) -> void;
		auto _set_sp() -> bool;
		auto _get_spells_known(SpellType spell_type, unsigned int spell_level) -> unsigned int;
		auto _get_xp_for_level(unsigned int level) const -> int;
		auto _get_mage_status(bool current) -> std::string;
		auto _get_priest_status(bool current) -> std::string;
		auto _get_sp_per_level(const SpellType type, int level) -> std::string;
		auto _get_condition() const -> std::string;
		auto _update_stat_for_level(CharacterAttribute attribute, std::string stat) -> std::string;
		auto _learn_spell(SpellID spell_id) -> void;
		auto _damage(
			const unsigned int adjustment) -> bool; // returns true is character is alive, or dead if damage was fatal
		auto _heal(const unsigned int adjustment) -> void;

		// Private Members
		int _version;

		System *_system;
		Display *_display;
		Graphics *_graphics;
		CharacterAbilities _abilities;
		SpellPoints _priest_max_sp;
		SpellPoints _priest_cur_sp;
		SpellPoints _mage_max_sp;
		SpellPoints _mage_cur_sp;
		std::vector<Spell> _spells;
		std::map<SpellID, bool> _spells_known;
		CharacterStage _current_stage;
		std::string _name;
		CharacterRace _race;
		CharacterClass _class;
		CharacterAlignment _alignment;
		CharacterAttributes _start_attr;
		CharacterAttributes _cur_attr;
		CharacterAttributes _max_attr;
		unsigned int _points_left;
		unsigned int _st_points;
		CharacterClassQualified _pos_classes;
		CharacterClassList _class_list;
		unsigned int _num_pos_classes;
		unsigned int _portrait_index;
		CharacterStatus _status;
		bool _hidden;
		CreateMethod _method;
		bool _legated;
		CharacterLocation _location;
};

}
