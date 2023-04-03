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

#include "main.hpp"
#include "system.hpp"

namespace Sorcery {

class ItemRegistry {

	public:

		ItemRegistry();
		ItemRegistry(ItemType item_type);

	private:

		ItemType _type;							 // e.g. LONG_SWORD, LONG_SWORD_PLUS_1 etc
		std::string _known_name;				 // Friendly name once identified
		std::string _unknown_name;				 // Unknown name if not identified
		ItemCategory _category;					 // e.g, WEAPON, ARMOUR etc
		bool _cursed;							 // Is a cursed item
		unsigned int _value;					 // Price to buy in shop (Sell/Identify Price is half this)
		bool _sellable;							 // Can be sold
		std::array<CharacterClass, 9> _usable;	 // Usable by class list
		CharacterAlignment _alignment;			 // Usable by alignment (otherwise cursed)
		int _num_attacks;						 // If a weapon, number of attacks granted
		int _to_hit_modifier;					 // Bonus to hit using this weapon
		RandomType _damage_dice;				 // e.g. the 2d8 of 2d8+2
		int _damage_dice_modifer;				 // e.g. the 2 of 2d8+2
		int _ac_modifier;						 // AC modifier if worn normally
		int _curse_ac_modifier;					 // If a cursed item, AC modifier
		int _regeneration;						 // Passive hp adjustment (25% chance per turn of this happening)
		std::vector<ItemOff> _offensive_effects; // Offensive effects wielding this item bestows
		std::vector<ItemDef> _defensive_effects; // Defensive effects wearing this item bestows
		ItemInv _invocation_effect;				 // Effect when invoked
		unsigned int _invocation_decay_chance;	 // % chance of turning into _decay_type when invoked
		SpellType _use_effect;					 // Effect when used (same as associated spell effect)
		unsigned int _use_decay_chance;			 // % chance of turning into _decay_type when used
		ItemType _decay_type;					 // Item type decays to when used/invoked
		int _shop_initial_stock;				 // Number in stock in shop at beginning of game
		bool _discovered_by_player;				 // Has been discovered in this game
		std::string _description;				 // Flowery Description
		unsigned int _gfx;						 // Index of Item Graphic

		long _id;
		static inline long s_id{0};
};

}