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

#include "common/include.hpp"
#include "types/dice.hpp"
#include "types/enum.hpp"

namespace Sorcery {

class ItemType {

	public:

		// Default Constructor
		ItemType() = default;

		// Overloaded Operators
		auto friend operator<<(std::ostream &out_stream, const ItemType &ItemType) -> std::ostream &;

		// Public Methods
		auto get_type_id() const -> ItemTypeID;
		auto get_known_name() const -> std::string;
		auto get_display_name() const -> std::string;
		auto get_unknown_name() const -> std::string;
		auto get_category() const -> ItemCategory;
		auto get_cursed() const -> bool;
		auto get_value() const -> unsigned int;
		auto get_sellable() const -> bool;
		auto get_usable_class() const -> ItemUsableClass;
		auto get_usable_alignment() const -> ItemUsableAlignment;
		auto get_swings() const -> int;
		auto get_to_hit_mod() const -> int;
		auto get_damage_dice() const -> Dice;
		auto get_damage() const -> std::string;
		auto get_damage_dice_number() const -> int;
		auto get_damage_dice_type() const -> int;
		auto get_damage_dice_mod() const -> int;
		auto get_random_damage() const -> int;
		auto get_ac_mod() const -> int;
		auto get_curse_ac_mod() const -> int;
		auto get_regen() const -> int;
		auto get_eff_def() const -> ItemEffDef;
		auto get_eff_def_str() const -> std::string;
		auto get_eff_off() const -> ItemEffOff;
		auto get_eff_off_str() const -> std::string;
		auto get_eff_inv() const -> ItemInv;
		auto get_eff_inv_decay() const -> unsigned int;
		auto get_eff_use() const -> SPI;
		auto get_eff_use_decay() const -> unsigned int;
		auto get_decay_type_id() const -> ItemTypeID;
		auto get_shop_inital_stock() const -> int;
		auto get_discovered() const -> bool;
		auto get_buy() const -> bool;
		auto get_sell() const -> bool;
		auto get_desc() const -> std::string;
		auto get_gfx() const -> unsigned int;
		auto is_class_usable(const CharacterClass cclass) const -> bool;
		auto is_align_usable(const CAL calign) const -> bool;
		auto set_type_id(const ItemTypeID value) -> void;
		auto set_known_name(const std::string value) -> void;
		auto set_damage(const std::string value) -> void;
		auto set_display_name(const std::string value) -> void;
		auto set_unknown_name(const std::string value) -> void;
		auto set_category(const ItemCategory value) -> void;
		auto set_cursed(const bool value) -> void;
		auto set_value(const unsigned int value) -> void;
		auto set_sellable(const bool value) -> void;
		auto set_usable_class(const ItemUsableClass value) -> void;
		auto set_usable_alignment(const ItemUsableAlignment value) -> void;
		auto set_swings(const int value) -> void;
		auto set_to_hit_mod(const int value) -> void;
		auto set_damage_dice(const std::string value) -> void;
		auto set_ac_mod(const int value) -> void;
		auto set_curse_ac_mod(const int value) -> void;
		auto set_regen(const int value) -> void;
		auto set_eff_def(const ItemEffDef value) -> void;
		auto set_eff_off(const ItemEffOff value) -> void;
		auto set_eff_inv(const ItemInv value) -> void;
		auto set_eff_inv_decay(const unsigned int value) -> void;
		auto set_eff_use(const SPI value) -> void;
		auto set_eff_use_decay(const unsigned int value) -> void;
		auto set_decay_type_id(const ItemTypeID value) -> void;
		auto set_shop_inital_stock(const int value) -> void;
		auto set_discovered(const bool value) -> void;
		auto set_desc(const std::string value) -> void;
		auto set_gfx(const unsigned int value) -> void;
		auto set_sell(const bool value) -> void;
		auto set_buy(const bool value) -> void;
		auto set_effects(const std::string value) -> void;
		auto get_effects() const -> std::string;

	private:

		// Private Members
		ItemTypeID _type;				// e.g. LONG_SWORD, LONG_SWORD_PLUS_1 etc
		std::string _known_name;		// Friendly name once identified
		std::string _display_name;		// Short display name once identified (max 16 characters)
		std::string _unknown_name;		// Unknown name if not identified
		ItemCategory _category;			// e.g, WEAPON, ARMOUR etc
		bool _cursed;					// Is a cursed item
		unsigned int _value;			// Price to buy in shop (Sell/Identify Price is half this)
		bool _sellable;					// Can be sold
		ItemUsableClass _usable;		// Usable by class list
		ItemUsableAlignment _alignment; // Usable by alignment (otherwise cursed)
		int _swings;					// If a weapon, number of attacks granted
		int _to_hit_modifier;			// Bonus to hit using this weapon
		std::string _damage_str;		// 3d8+2
		Dice _damage_dice;
		int _ac_modifier;					   // AC modifier if worn normally
		int _curse_ac_modifier;				   // If a cursed item, AC modifier
		int _regeneration;					   // Passive hp adjustment (25% chance per turn of this happening)
		ItemEffOff _offensive_effects;		   // Offensive effects wielding this item bestows
		ItemEffDef _defensive_effects;		   // Defensive effects wearing this item bestows
		ItemInv _invocation_effect;			   // Effect when invoked
		unsigned int _invocation_decay_chance; // % chance of turning into _decay_type when invoked
		SPI _use_effect;					   // Effect when used (same as associated spell effect)
		unsigned int _use_decay_chance;		   // % chance of turning into _decay_type when used
		ItemTypeID _decay_type;				   // Item type decays to when used/invoked
		int _shop_initial_stock;			   // Number in stock in shop at beginning of game
		bool _discovered_by_player;			   // Has been discovered in this game
		std::string _description;			   // Flowery Description
		unsigned int _gfx;					   // Index of Item Graphic
		bool _buy;							   // Can Player buy this item from Shop
		bool _sell;							   // Can Player sell this item to Shop
		std::string _effects;				   // Textual Representation of Effects

		static std::random_device _device; // Shared RNG
		static std::mt19937_64 _random;
};

}
