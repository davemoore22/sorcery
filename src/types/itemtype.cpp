

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

std::random_device Sorcery::ItemType::_device;
std::mt19937_64 Sorcery::ItemType::_random(_device());

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

auto Sorcery::ItemType::get_damage() const -> std::string {

	return _damage_str;
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

auto Sorcery::ItemType::get_damage_dice_number() const -> int {

	return _damage_dice_number;
}

auto Sorcery::ItemType::get_damage_dice_type() const -> int {

	return _damage_dice_type;
}

auto Sorcery::ItemType::get_random_damage() const -> int {

	if (_damage_dice_number > 0) {
		{
			auto dist{std::uniform_int_distribution<unsigned int>(1, _damage_dice_type)};
			return _damage_dice_number * dist(_random) + _damage_dice_modifer;
		}
	} else
		return 0;
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

auto Sorcery::ItemType::get_eff_use() const -> SpellID {

	return _use_effect;
}

auto Sorcery::ItemType::get_eff_use_decay() const -> unsigned int {

	return _use_decay_chance;
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

auto Sorcery::ItemType::is_class_usable(const CharacterClass cclass) const -> bool {

	auto cc{unenum(cclass)};

	return _usable[cc];
}

auto Sorcery::ItemType::is_align_usable(const CharacterAlignment calign) const -> bool {

	auto ca{unenum(calign)};

	return _alignment[ca];
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

auto Sorcery::ItemType::set_damage(const std::string value) -> void {

	_damage_str = value;
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

auto Sorcery::ItemType::set_damage_dice(const int num, const int type, const int mod) -> void {

	_damage_dice_number = num;
	_damage_dice_type = type;
	_damage_dice_modifer = mod;
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

auto Sorcery::ItemType::get_eff_def_str() const -> std::string {

	auto effects{""s};
	for (auto i = unenum(ItemDef::RESIST_COLD); i <= unenum(ItemDef::PREVENT_DECAPITATION); i++) {
		if (_defensive_effects[i]) {
			const auto eff_enum{magic_enum::enum_value<ItemDef>(i)};
			std::string str{magic_enum::enum_name<ItemDef>(eff_enum)};
			std::replace(str.begin(), str.end(), '_', ' ');
			effects.append(str);
			effects.append(", ");
		}
	}
	for (auto i = unenum(ItemDef::PROTECTION_VS_ANIMAL); i <= unenum(ItemDef::PROTECTION_VS_WERE); i++) {
		if (_defensive_effects[i]) {
			const auto eff_enum{magic_enum::enum_value<ItemDef>(i)};
			std::string str{magic_enum::enum_name<ItemDef>(eff_enum)};
			std::replace(str.begin(), str.end(), '_', ' ');
			effects.append(str);
			effects.append(", ");
		}
	}

	return effects;
}

auto Sorcery::ItemType::get_eff_off_str() const -> std::string {

	auto effects{""s};
	for (auto i = unenum(ItemOff::PURPOSED_VS_ANIMAL); i <= unenum(ItemOff::AUTOKILL); i++) {
		if (_offensive_effects[i]) {
			const auto eff_enum{magic_enum::enum_value<ItemOff>(i)};
			std::string str{magic_enum::enum_name<ItemOff>(eff_enum)};
			std::replace(str.begin(), str.end(), '_', ' ');
			effects.append(str);
			effects.append(", ");
		}
	}

	return effects;
}

auto Sorcery::ItemType::set_eff_inv_decay(const unsigned int value) -> void {

	_invocation_decay_chance = value;
}

auto Sorcery::ItemType::set_eff_use(const SpellID value) -> void {

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

auto Sorcery::ItemType::get_buy() const -> bool {

	return _buy;
}

auto Sorcery::ItemType::get_sell() const -> bool {

	return _sell;
}

auto Sorcery::ItemType::set_buy(const bool value) -> void {

	_buy = value;
}

auto Sorcery::ItemType::set_sell(const bool value) -> void {

	_sell = value;
}

namespace Sorcery {

auto operator<<(std::ostream &out_stream, const Sorcery::ItemType &itemtype) -> std::ostream & {

	const auto type{unenum(itemtype.get_type_id())};
	const auto name{itemtype.get_known_name()};

	const auto text{fmt::format("{}: {}", type, name)};

	return out_stream << text << std::flush;
}
} // namespace Sorcery
