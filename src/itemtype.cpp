

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

#include "itemtype.hpp"

auto Sorcery::ItemType::get_type_id() const -> ItemTypeID {

	return _type;
}

auto Sorcery::ItemType::get_known_name() const -> std::string {

	return _known_name;
}

auto Sorcery::ItemType::get_display_name() const -> std::string {

	return _display_name;
}

auto Sorcery::ItemType::get_unknown_name() const -> std::string {

	return _unknown_name;
}

auto Sorcery::ItemType::get_category() const -> ItemCategory {

	return _category;
}

auto Sorcery::ItemType::get_cursed() const -> bool {

	return _cursed;
}

auto Sorcery::ItemType::get_value() const -> unsigned int {

	return _value;
}

auto Sorcery::ItemType::get_sellable() const -> bool {

	return _sellable;
}

auto Sorcery::ItemType::get_usable_class() const -> ItemUsableClass {

	return _usable;
}

auto Sorcery::ItemType::get_usable_alignment() const -> ItemUsableAlignment {

	return _alignment;
}

auto Sorcery::ItemType::get_swings() const -> int {

	return _swings;
}

auto Sorcery::ItemType::get_to_hit_mod() const -> int {

	return _to_hit_modifier;
}

auto Sorcery::ItemType::get_damage_dice() const -> RandomType {

	return _damage_dice;
}

auto Sorcery::ItemType::get_damage_dice_mod() const -> int {

	return _damage_dice_modifer;
}

auto Sorcery::ItemType::get_ac_mod() const -> int {

	return _ac_modifier;
}

auto Sorcery::ItemType::get_curse_ac_mod() const -> int {

	return _curse_ac_modifier;
}

auto Sorcery::ItemType::get_regen() const -> int {

	return _regeneration;
}

auto Sorcery::ItemType::get_eff_def() const -> ItemEffDef {

	return _defensive_effects;
}

auto Sorcery::ItemType::get_eff_off() const -> ItemEffOff {

	return _offensive_effects;
}

auto Sorcery::ItemType::get_eff_inv() const -> ItemInv {

	return _invocation_effect;
}

auto Sorcery::ItemType::get_eff_inv_decay() const -> unsigned int {

	return _invocation_decay_chance;
}

auto Sorcery::ItemType::get_eff_use() const -> SpellType {

	return _use_effect;
}

auto Sorcery::ItemType::get_eff_use_decay() const -> unsigned int {

	return _invocation_decay_chance;
}

auto Sorcery::ItemType::get_decay_type_id() const -> ItemTypeID {

	return _decay_type;
}

auto Sorcery::ItemType::get_shop_inital_stock() const -> int {

	return _shop_initial_stock;
}

auto Sorcery::ItemType::get_discovered() const -> bool {

	return _discovered_by_player;
}

auto Sorcery::ItemType::get_desc() const -> std::string {

	return _description;
}

auto Sorcery::ItemType::get_gfx() const -> unsigned int {

	return _gfx;
}

auto Sorcery::ItemType::is_usable(const CharacterClass cclass) -> bool {

	return _usable[magic_enum::enum_integer<CharacterClass>(cclass)];
}

auto Sorcery::ItemType::is_usable(const CharacterAlignment calign) -> bool {

	return _alignment[magic_enum::enum_integer<CharacterAlignment>(calign)];
}

auto Sorcery::ItemType::set_type_id(const ItemTypeID value) -> void {

	_type = value;
}

auto Sorcery::ItemType::set_known_name(const std::string value) -> void {

	_known_name = value;
}

auto Sorcery::ItemType::set_display_name(const std::string value) -> void {

	_display_name = value;
}

auto Sorcery::ItemType::set_unknown_name(const std::string value) -> void {

	_unknown_name = value;
}

auto Sorcery::ItemType::set_category(const ItemCategory value) -> void {

	_category = value;
}

auto Sorcery::ItemType::set_cursed(const bool value) -> void {

	_cursed = value;
}

auto Sorcery::ItemType::set_value(const unsigned int value) -> void {

	_value = value;
}

auto Sorcery::ItemType::set_sellable(const bool value) -> void {

	_sellable = value;
}

auto Sorcery::ItemType::set_usable_class(const ItemUsableClass value) -> void {

	_usable = value;
}

auto Sorcery::ItemType::set_usable_alignment(const ItemUsableAlignment value) -> void {

	_alignment = value;
}

auto Sorcery::ItemType::set_swings(const int value) -> void {

	_swings = value;
}

auto Sorcery::ItemType::set_to_hit_mod(const int value) -> void {

	_to_hit_modifier = value;
}

auto Sorcery::ItemType::set_damage_dice(const RandomType value) -> void {

	_damage_dice = value;
}

auto Sorcery::ItemType::set_damage_dice_mod(const int value) -> void {

	_damage_dice_modifer = value;
}

auto Sorcery::ItemType::set_ac_mod(const int value) -> void {

	_ac_modifier = value;
}

auto Sorcery::ItemType::set_curse_ac_mod(const int value) -> void {

	_curse_ac_modifier = value;
}

auto Sorcery::ItemType::set_regen(const int value) -> void {

	_regeneration = value;
}

auto Sorcery::ItemType::set_eff_def(const ItemEffDef value) -> void {

	_defensive_effects = value;
}

auto Sorcery::ItemType::set_eff_off(const ItemEffOff value) -> void {

	_offensive_effects = value;
}

auto Sorcery::ItemType::set_eff_inv(const ItemInv value) -> void {

	_invocation_effect = value;
}

auto Sorcery::ItemType::set_eff_inv_decay(const unsigned int value) -> void {

	_invocation_decay_chance = value;
}

auto Sorcery::ItemType::set_eff_use(const SpellType value) -> void {

	_use_effect = value;
}

auto Sorcery::ItemType::set_eff_use_decay(const unsigned int value) -> void {

	_use_decay_chance = value;
}

auto Sorcery::ItemType::set_decay_type_id(const ItemTypeID value) -> void {

	_decay_type = value;
}

auto Sorcery::ItemType::set_shop_inital_stock(const int value) -> void {

	_shop_initial_stock = value;
}

auto Sorcery::ItemType::set_discovered(const bool value) -> void {

	_discovered_by_player = value;
}

auto Sorcery::ItemType::set_desc(const std::string value) -> void {

	_description = value;
}

auto Sorcery::ItemType::set_gfx(const unsigned int value) -> void {

	_gfx = value;
}

namespace Sorcery {

auto operator<<(std::ostream &out_stream, const Sorcery::ItemType &itemtype) -> std::ostream & {

	const auto type{magic_enum::enum_integer<ItemTypeID>(itemtype.get_type_id())};
	const auto name{itemtype.get_known_name()};

	const auto text{fmt::format("{}: {}", type, name)};

	return out_stream << text << std::flush;
}
} // namespace Sorcery
