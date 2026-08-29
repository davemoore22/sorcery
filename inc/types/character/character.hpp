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

#pragma once

#include "common/enum.hpp"
#include "common/types.hpp"
#include "types/character/create.hpp"
#include "types/character/inventory.hpp"
#include "types/character/magic.hpp"
#include <map>
#include <optional>
#include <ostream>
#include <string>
#include <vector>

namespace Sorcery {

// Forward Declarations
struct Context;
class System;
class Resources;

class Character {

		friend class ConstCharacterMagic;
		friend class CharacterMagic;
		friend class ConstCharacterCreate;
		friend class CharacterCreate;

	public:
		// Constructors
		Character();
		Character(Context *ctx);

		// Destructor
		~Character();

		// Overloaded Operators
		auto operator[](const Enums::Character::Ability &key) -> int &;
		auto friend operator<<(std::ostream &out_stream,
							   const Character &character) -> std::ostream &;

		// Serialisation
		template <class Archive> auto serialize(Archive &archive) -> void {
			archive(_location, coordinate, depth, _version, _name, _race,
					_class, _alignment, _start_attr, _cur_attr, _max_attr,
					_st_points, _abilities, _priest_max_sp, _priest_cur_sp,
					_mage_max_sp, _mage_cur_sp, _status, _hidden, _spells_known,
					_legated, inventory, _current_stage, _wiz_1_award);
		}

		// Composition Classes
		auto magic() -> CharacterMagic;
		auto magic() const -> ConstCharacterMagic;
		auto create() -> CharacterCreate;
		auto create() const -> ConstCharacterCreate;

		// Public Methods
		auto post_construct(Context *ctx) -> void;
		auto alignment_to_str(const Enums::Character::Align alignment) const
			-> std::string;
		auto race_to_str(const Enums::Character::Race race) const
			-> std::string;
		auto class_to_str(const Enums::Character::Class cclass) const
			-> std::string;
		auto get_name() const -> std::string;
		auto get_name_and_loc() const -> std::string;
		auto get_name_and_status() const -> std::string;
		auto get_name_status_and_loc() const -> std::string;
		auto get_race() const -> Enums::Character::Race;
		auto get_location() const -> Enums::Character::Location;
		auto set_location(const Enums::Character::Location value) -> void;
		auto get_level() const -> int;
		auto get_class() const -> Enums::Character::Class;
		auto get_alignment() const -> Enums::Character::Align;
		auto get_cur_attr() const -> std::map<Enums::Character::Attribute, int>;
		auto get_attr_ptr(Enums::Character::Attribute attribute) -> int *;
		auto get_condition() const -> std::string;
		auto get_short_cond() const -> std::string;
		auto get_cur_attr(const Enums::Character::Attribute attribute) const
			-> unsigned int;
		auto get_gold() const -> unsigned int;
		auto set_gold(const unsigned int value) -> void;
		auto grant_gold(const int value) -> void;
		auto summary_text() const -> std::string;
		auto summary_text_with_awards() const -> std::string;
		auto full_desc_text() const -> std::string;
		auto get_summary() -> std::string;
		auto get_status() const -> Enums::Character::Status;
		auto get_status_string() const -> std::string;
		auto get_loc_str() const -> std::string;
		auto get_summary_and_out() -> std::string;
		auto set_status(Enums::Character::Status value) -> void;
		auto is_poisoned() const -> bool;
		auto get_poisoned_rate() const -> int;
		auto set_poisoned_rate(int value) -> void;
		auto get_poisoned_string() const -> std::string;
		auto get_hp_summary() const -> std::string;
		auto get_short_hp_summary() const -> std::string;
		auto is_legated() const -> bool;
		auto get_version() const -> int;
		auto get_party_panel_text(const int position) -> std::string;
		auto get_current_hp() const -> int;
		auto get_max_hp() const -> int;
		auto set_current_hp(const int hp) -> void;
		auto get_hp_adjustment() const -> int;
		auto set_hp_gain_per_turn(const int adjustment) -> void;
		auto set_hp_loss_per_turn(const int adjustment) -> void;
		auto reset_adjustment_per_turn() -> void;
		auto get_hp_adjustment_symbol() const -> char;
		auto get_cur_ac() const -> int;
		auto get_cur_to_hit() const -> int;
		auto get_cur_num_attacks() const -> int;
		auto get_cur_xp() const -> int;
		auto get_next_xp() const -> int;
		auto get_identify_trap() const -> int;
		auto get_activate_trap() const -> int;
		auto get_disarm_trap() const -> int;
		auto get_age() const -> int;
		auto set_age(const int adjustment) -> void;
		auto grant_xp(const int adjustment) -> int;
		auto can_level() const -> bool;
		auto get_cure_cost() const -> unsigned int;
		auto get_ress_chance(bool ashes) -> unsigned int;
		auto damage(const unsigned int adjustment) -> bool;
		auto heal(const unsigned int adjustment) -> void;
		auto attributes() -> std::map<Enums::Character::Attribute, int> &;
		auto abilities() -> std::map<Enums::Character::Ability, int> &;
		auto abilities() const
			-> const std::map<Enums::Character::Ability, int> &;
		auto get_wiz_1_award() const -> bool;
		auto set_wiz_1_awatd(const bool value) -> void;

		// Public Members
		std::optional<Coordinate> coordinate;
		std::optional<int> depth;
		Inventory inventory;
		int mode;
		std::vector<std::string> level_up_results;

	private:
		// Private Methods
		auto _get_condition() const -> std::string;
		auto _damage(const unsigned int adjustment)
			-> bool; // returns true is character is alive, or dead if damage
					 // was fatal
		auto _heal(const unsigned int adjustment) -> void;

		// Private
		Context *_ctx;
		int _version;
		std::map<Enums::Character::Ability, int> _abilities;
		std::map<unsigned int, unsigned int> _priest_max_sp;
		std::map<unsigned int, unsigned int> _priest_cur_sp;
		std::map<unsigned int, unsigned int> _mage_max_sp;
		std::map<unsigned int, unsigned int> _mage_cur_sp;
		std::vector<Spell> _spells;
		std::map<Enums::Magic::SpellID, bool> _spells_known;
		Enums::Character::Stage _current_stage;
		std::string _name;
		Enums::Character::Race _race;
		Enums::Character::Class _class;
		Enums::Character::Align _alignment;
		std::map<Enums::Character::Attribute, int> _start_attr;
		std::map<Enums::Character::Attribute, int> _cur_attr;
		std::map<Enums::Character::Attribute, int> _max_attr;
		unsigned int _points_left;
		unsigned int _st_points;
		std::map<Enums::Character::Class, bool> _pos_classes;
		std::map<Enums::Character::Class, std::string> _class_list;
		unsigned int _num_pos_classes;
		Enums::Character::Status _status;
		bool _hidden;
		bool _legated;
		Enums::Character::Location _location;
		bool _wiz_1_award;
};
}