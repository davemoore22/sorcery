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

#include "core/diagnostics.hpp"

#include <format>
#include <string_view>
#include <utility>

namespace Sorcery {

inline auto debug_log(std::string_view message, std::source_location location = std::source_location::current())
	-> void {

	Diagnostics::instance().log(LogLevel::DEBUG, message, location);
}

template <typename... Args>
inline auto debug_logf(std::source_location location, std::format_string<Args...> fmt, Args &&...args) -> void {

	auto &diagnostics{Diagnostics::instance()};

	if (!diagnostics.debug_enabled())
		return;

	diagnostics.log(LogLevel::DEBUG, std::format(fmt, std::forward<Args>(args)...), location);
}

}

#define DEBUG_LOG(msg) Sorcery::debug_log(msg, std::source_location::current())

#define DEBUG_LOGF(...)                                                                                                \
	do {                                                                                                               \
		if (Sorcery::Diagnostics::instance().debug_enabled())                                                          \
			Sorcery::debug_logf(std::source_location::current(), __VA_ARGS__);                                         \
	} while (false)
