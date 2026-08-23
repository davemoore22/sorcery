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

#include "types/meta.hpp"
#include "common/enum.hpp"
#include "core/enum.hpp"
#include "engine/enum.hpp"
#include "types/enum.hpp"

#include <array>
#include <meta>
#include <utility>

namespace Sorcery {

namespace {

	template <Enum E> consteval auto make_enum_entries() {

		static constexpr auto enumerators{
			std::define_static_array(std::meta::enumerators_of(^^E))};

		std::array<std::pair<E, std::string_view>, enumerators.size()>
			entries{};

		std::size_t index{};

		template for (constexpr auto enumerator : enumerators) {
			entries[index++] = {[:enumerator:], std::meta::identifier_of(
													enumerator)};
		}

		return entries;
	}

	template <Enum E>
	inline constexpr auto enum_entries{make_enum_entries<E>()};

} // namespace

template <Enum E> auto enum_name(E value) -> std::string_view {

	for (const auto &[enum_value, name] : enum_entries<E>)
		if (enum_value == value)
			return name;

	return {};
}

template <Enum E> auto enum_cast(std::string_view name) -> std::optional<E> {

	for (const auto &[enum_value, enum_name] : enum_entries<E>)
		if (enum_name == name)
			return enum_value;

	return std::nullopt;
}

template <Enum E>
auto enum_cast_signed(std::intmax_t value) -> std::optional<E> {

	for (const auto &entry : enum_entries<E>)
		if (std::cmp_equal(std::to_underlying(entry.first), value))
			return entry.first;

	return std::nullopt;
}

template <Enum E>
auto enum_cast_unsigned(std::uintmax_t value) -> std::optional<E> {

	for (const auto &entry : enum_entries<E>)
		if (std::cmp_equal(std::to_underlying(entry.first), value))
			return entry.first;

	return std::nullopt;
}

#define INSTANTIATE_ENUM(E)                                                    \
	template auto enum_name(E)->std::string_view;                              \
	template auto enum_cast<E>(std::string_view)->std::optional<E>;            \
	template auto enum_cast_signed<E>(std::intmax_t)->std::optional<E>;        \
	template auto enum_cast_unsigned<E>(std::uintmax_t) -> std::optional<E>

INSTANTIATE_ENUM(Enums::CharacterSlot);
INSTANTIATE_ENUM(Enums::Screen);
INSTANTIATE_ENUM(Enums::Character::Align);
INSTANTIATE_ENUM(Enums::Character::Attribute);
INSTANTIATE_ENUM(Enums::Character::Class);
INSTANTIATE_ENUM(Enums::Character::Location);
INSTANTIATE_ENUM(Enums::Character::Race);
INSTANTIATE_ENUM(Enums::Character::Status);
INSTANTIATE_ENUM(Enums::Chests::State);
INSTANTIATE_ENUM(Enums::Items::Category);
INSTANTIATE_ENUM(Enums::Items::TypeID);
INSTANTIATE_ENUM(Enums::Items::Effects::Defensive);
INSTANTIATE_ENUM(Enums::Items::Effects::Invoke);
INSTANTIATE_ENUM(Enums::Items::Effects::Offensive);
INSTANTIATE_ENUM(Enums::Magic::SpellCategory);
INSTANTIATE_ENUM(Enums::Magic::SpellID);
INSTANTIATE_ENUM(Enums::Magic::SpellType);
INSTANTIATE_ENUM(Enums::Monsters::Category);
INSTANTIATE_ENUM(Enums::Monsters::Class);
INSTANTIATE_ENUM(Enums::Monsters::TypeID);
INSTANTIATE_ENUM(Enums::System::Error);
INSTANTIATE_ENUM(Enums::System::Random);

// Add the other enum types used with enum_name()/enum_cast() here.

#undef INSTANTIATE_ENUM

} // namespace Sorcery