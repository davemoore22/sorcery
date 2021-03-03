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
// be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
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

#pragma once

#include "display.hpp"
#include "graphics.hpp"
#include "main.hpp"
#include "system.hpp"

namespace Sorcery {

	class Character : public sf::Transformable, public sf::Drawable {

	  public:
		// Constructors
		Character(System &system, Display &display, Graphics &graphics);
		Character() = delete;

		// Overloaded Operators
		auto operator[](const CharacterAbility &key) -> int &;

		// Public Methods
		auto reset(const CharacterStage stage) -> void;
		auto get_stage() const -> CharacterStage;
		auto set_stage(const CharacterStage stage) -> void;
		auto set_possible_classes() -> void;
		auto finalise() -> void;
		auto level_up() -> void;
		auto level_down() -> void;
		// auto render(unsigned int y_position) -> void;
		auto get_alignment(const CharacterAlignment alignment) const -> std::string;
		auto get_race(const CharacterRace race) const -> std::string;
		auto get_class(const CharacterClass cclass) const -> std::string;
		auto create_random() -> void;

		// Public Members
		std::map<std::string, sf::Sprite> sprites;
		std::map<std::string, sf::Text> texts;
		std::map<std::string, std::shared_ptr<Frame>> frames;

		// Public Methods

	  private:
		// Private Methods
		auto virtual draw(sf::RenderTarget &target, sf::RenderStates states) const -> void;
		auto _save() -> unsigned int;
		auto _load(unsigned int character_id) -> void;
		auto _generate_starting_information() -> void;
		auto _generate_secondary_abilities() -> void;
		auto _set_starting_sp() -> void;
		auto _clear_sp() -> void;
		auto _set_starting_spells() -> void;
		auto _get_hp_gained_per_level() -> int;
		auto _update_hp_for_level() -> void;
		auto _try_to_learn_spells(SpellType spell_type, unsigned int spell_level) -> void;
		auto _calculate_sp(SpellType spell_type, unsigned int level_mod, unsigned int level_offset)
			-> void;
		auto _set_sp() -> void;
		auto _get_spells_known(SpellType spell_type, unsigned int spell_level) -> unsigned int;
		auto _get_xp_for_level(unsigned int level) const -> int;
		auto _create_spell_lists() -> void;

		// Private Members
		System _system;
		Display _display;
		Graphics _graphics;
		CharacterAbilities _abilities;
		SpellPoints _cleric_max_sp;
		SpellPoints _cleric_cur_sp;
		SpellPoints _mage_max_sp;
		SpellPoints _mage_cur_sp;
		std::vector<SpellEntry> _spells;
		// std::vector<CharacterStatus> _status;
		CharacterStage _current_stage;
		std::string _name;
		CharacterRace _race;
		CharacterClass _class;
		CharacterAlignment _alignment;
		CharacterAttributes _start_attr;
		CharacterAttributes _cur_attr;
		CharacterAttributes _max_attr;
		CharacterView _view;
		unsigned int _points_left;
		unsigned int _st_points;
		CharacterClassQualified _pos_classes;
		CharacterClassList _class_list;
		unsigned int _num_pos_classes;
		int _portrait_index;
	};
} // namespace Sorcery