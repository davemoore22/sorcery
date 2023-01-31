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
#include "display.hpp"
#include "graphics.hpp"
#include "main.hpp"
#include "system.hpp"
#include "spellpanel.hpp"

// clang-format on

namespace Sorcery {

class SpellSummary;

class Character: public sf::Transformable, public sf::Drawable {

	public:

		// Constructors
		Character();
		Character(System *system, Display *display, Graphics *graphics);

		// Destructor
		~Character() = default;

		// Copy Constructors
		Character(const Character &other);
		auto operator=(const Character &other) -> Character &;

		// Move Constructors
		Character(Character &&other) noexcept;
		auto operator=(Character &&other) noexcept -> Character &;

		// Overloaded Operators
		auto operator[](const CharacterAbility &key) -> int &;
		auto friend operator<<(std::ostream &out_stream, const Character &character) -> std::ostream &;

		// Serialisation
		template <class Archive> auto serialize(Archive &archive) -> void {
			archive(_location, coordinate, depth, _version, _name, _race, _class, _alignment, _start_attr, _cur_attr,
				_max_attr, _st_points, _portrait_index, _abilities, _priest_max_sp, _priest_cur_sp, _mage_max_sp,
				_mage_cur_sp, _status, _hidden, _spells_known, _legated);
		}

		// Public Methods
		auto reset(const CharacterStage stage) -> void;
		auto get_stage() const -> CharacterStage;
		auto set_stage(const CharacterStage stage) -> void;
		auto set_pos_class() -> void;
		auto finalise() -> void;
		auto level_up() -> std::string;
		auto level_down() -> void;
		auto get_alignment(const CharacterAlignment alignment) const -> std::string;
		auto get_race(const CharacterRace race) const -> std::string;
		auto get_class(const CharacterClass cclass) const -> std::string;
		auto create_random() -> void;
		auto create_quick() -> void;
		auto set_start_attr() -> void;
		auto get_name() const -> std::string;
		auto get_name_and_location() const -> std::string;
		auto get_name_and_status() const -> std::string;
		auto set_name(std::string_view value) -> void;
		auto get_race() const -> CharacterRace;
		auto set_race(const CharacterRace &value) -> void;
		auto get_location() const -> CharacterLocation;
		auto set_location(const CharacterLocation &value) -> void;
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
		auto get_cur_attr(const CharacterAttribute attribute) const -> unsigned int;
		auto get_start_attr(const CharacterAttribute attribute) const -> unsigned int;
		auto set_cur_attr(const CharacterAttribute attribute, const int adjustment) -> void;
		auto get_icon(CharacterStage type) -> std::optional<sf::Sprite>;
		auto get_portrait_index() const -> unsigned int;
		auto set_portrait_index(const unsigned int value) -> void;
		auto get_spell_points(const SpellType type, const SpellPointStatus status) const -> std::optional<SpellPoints>;
		auto get_view() const -> CharacterView;
		auto set_view(const CharacterView value) -> void;
		auto left_view() -> void;
		auto right_view() -> void;
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
		auto get_location_string() const -> std::string;
		auto get_summary_and_out() -> std::string;
		auto set_status(CharacterStatus value) -> void;
		auto is_poisoned() const -> bool;
		auto get_poisoned_rate() const -> int;
		auto set_poisoned_rate(int value) -> void;
		auto get_poisoned_string() const -> std::string;
		auto get_hp_summary() const -> std::string;
		auto inc_hl_spell(SpellType type) -> void;
		auto dec_hl_spell(SpellType type) -> void;
		auto update() -> void;
		auto check_for_mouse_move(sf::Vector2f mouse_pos) -> std::optional<SpellID>;
		// auto check_for_action_mouse_move(sf::Vector2f mouse_pos) -> std::optional<MenuItem>;
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

		// Public Members
		std::map<SpellID, sf::FloatRect> mage_spell_bounds;
		std::map<SpellID, sf::FloatRect> priest_spell_bounds;
		std::map<SpellID, sf::Text *> mage_spell_texts;
		std::map<SpellID, sf::Text *> priest_spell_texts;
		std::map<MenuItem, sf::Text *> action_menu_texts;
		std::map<MenuItem, sf::FloatRect> action_menu_bounds;

		std::optional<Coordinate> coordinate;
		std::optional<int> depth;

	private:

		// Private Methods
		auto virtual draw(sf::RenderTarget &target, sf::RenderStates states) const -> void;
		auto _generate_display() -> void;
		auto _generate_summary_icons() -> void;
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
		auto _get_character_portrait() -> sf::Sprite;
		auto _add_text(Component &component, std::string format, std::string value, bool is_view = true) -> sf::Text *;
		auto _add_icon(Component &component, std::string icon_key) -> void;
		auto _get_mage_status(bool current) -> std::string;
		auto _get_priest_status(bool current) -> std::string;
		auto _get_spell_icon(SpellCategory category) -> std::optional<sf::Sprite>;
		auto _get_sp_per_level(const SpellType type, int level) -> std::string;
		auto _get_condition() const -> std::string;
		auto _update_stat_for_level(CharacterAttribute attribute, std::string stat) -> std::string;
		auto _learn_spell(SpellID spell_id) -> void;
		auto _damage(const unsigned int adjustment)
			-> bool; // returns true is character is alive, or dead if damage was fatal
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
		CharacterView _view;
		unsigned int _points_left;
		unsigned int _st_points;
		CharacterClassQualified _pos_classes;
		CharacterClassList _class_list;
		unsigned int _num_pos_classes;
		unsigned int _portrait_index;
		CharacterStatus _status;
		bool _hidden;
		std::map<std::string, sf::Sprite> _sprites;
		std::map<std::string, sf::Text> _texts;
		std::map<std::string, std::shared_ptr<Frame>> _frames;
		std::map<std::string, sf::Sprite> _v_sprites;
		std::map<std::string, sf::Text> _v_texts;
		std::map<std::string, std::shared_ptr<Frame>> _v_frames;
		std::shared_ptr<SpellPanel> _spell_panel;
		Component _spell_panel_c;
		CreateMethod _method;
		SpellID _hl_mage_spell;
		SpellID _hl_priest_spell;
		MenuItem _hl_action_item;
		sf::RectangleShape _hl_mage_spell_bg;
		sf::RectangleShape _hl_priest_spell_bg;
		sf::RectangleShape _hl_action_item_bg;
		bool _legated;
		CharacterLocation _location;
};

}
