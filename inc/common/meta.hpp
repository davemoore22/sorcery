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

#include <concepts>
#include <meta>
#include <optional>
#include <string_view>
#include <type_traits>
#include <utility>

namespace Sorcery {

template <typename T>
concept Enum = std::is_enum_v<T>;

template <Enum E>
[[nodiscard]]
constexpr auto enum_name(E value) -> std::string_view {

	template for (constexpr auto enumerator :
				  std::define_static_array(std::meta::enumerators_of(^^E))) {

		if (value == [:enumerator:])
			return std::meta::identifier_of(enumerator);
	}

	return {};
}

template <Enum E>
[[nodiscard]]
constexpr auto enum_cast(std::string_view name) -> std::optional<E> {

	template for (constexpr auto enumerator :
				  std::define_static_array(std::meta::enumerators_of(^^E))) {

		if (name == std::meta::identifier_of(enumerator))
			return [:enumerator:];
	}

	return std::nullopt;
}

template <Enum E, std::integral T>
[[nodiscard]]
constexpr auto enum_cast(T value) -> std::optional<E> {

	template for (constexpr auto enumerator :
				  std::define_static_array(std::meta::enumerators_of(^^E))) {

		if (std::to_underlying([:enumerator:]) == value)
			return [:enumerator:];
	}

	return std::nullopt;
}

} // namespace Sorcery