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

#include "types/meta.hpp"
#include <cstdint>
#include <json/value.h>
#include <string>
#include <string_view>

namespace Sorcery::JsonHelper {

inline auto get_string(const Json::Value &value, const std::string_view key, const std::string_view fallback = {})
	-> std::string {

	const auto name{std::string{key}};

	if (!value.isMember(name))
		return std::string{fallback};

	return value[name].asString();
}

inline auto get_int(const Json::Value &value, const std::string_view key, const int fallback = 0) -> int {

	const auto name{std::string{key}};

	return value.isMember(name) ? value[name].asInt() : fallback;
}

inline auto get_uint(const Json::Value &value, const std::string_view key, const unsigned int fallback = 0)
	-> unsigned int {

	const auto name{std::string{key}};

	return value.isMember(name) ? value[name].asUInt() : fallback;
}

inline auto get_uint64(const Json::Value &value, const std::string_view key, const std::uint64_t fallback = 0)
	-> std::uint64_t {

	const auto name{std::string{key}};

	return value.isMember(name) ? value[name].asUInt64() : fallback;
}

inline auto get_bool(const Json::Value &value, const std::string_view key, const bool fallback = false) -> bool {

	const auto name{std::string{key}};

	return value.isMember(name) ? value[name].asBool() : fallback;
}

inline auto get_flag(const Json::Value &value, const std::string_view key, const std::string_view enabled) -> bool {

	const auto name{std::string{key}};

	return value.isMember(name) && value[name].asString() == enabled;
}

template <Enum E> auto get_enum(const Json::Value &value, const std::string_view key) -> std::optional<E> {

	const auto name{std::string{key}};

	if (!value.isMember(name))
		return std::nullopt;

	const auto &member{value[name]};

	if (member.isString()) {

		const auto text{member.asString()};

		if (text.empty())
			return std::nullopt;

		return enum_cast<E>(text);
	}

	if (member.isInt64())
		return enum_cast_signed<E>(member.asInt64());

	if (member.isUInt64())
		return enum_cast_unsigned<E>(member.asUInt64());

	return std::nullopt;
}

template <Enum E> auto get_enum(const Json::Value &value, const std::string_view key, const E fallback) -> E {

	return get_enum<E>(value, key).value_or(fallback);
}

template <Enum E> auto get_required_enum(const Json::Value &value, const std::string_view key) -> E {

	if (const auto result{get_enum<E>(value, key)})
		return *result;

	throw std::runtime_error{std::format("Missing or invalid enum value '{}'", key)};
}

} // namespace Sorcery::JsonHelper