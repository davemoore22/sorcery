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

#include <format>
#include <print>
#include <string_view>
#include <utility>

namespace Sorcery {

// Simple message logger
inline auto debug_log(std::string_view message) -> void {

#ifdef SORCERY_DEBUG
	std::println("[DEBUG] {}", message);
#endif
}

// Formatted logger
template <typename... Args>
inline auto debug_logf(std::format_string<Args...> fmt, Args &&...args)
	-> void {

#ifdef SORCERY_DEBUG
	auto message{std::format(fmt, std::forward<Args>(args)...)};

	std::println("[DEBUG] {}", message);
#endif
}

} // namespace Sorcery

#ifdef SORCERY_DEBUG

#define DEBUG_LOG(msg) Sorcery::debug_log(msg)
#define DEBUG_LOGF(...) Sorcery::debug_logf(__VA_ARGS__)

#else

#define DEBUG_LOG(msg) ((void)0)
#define DEBUG_LOGF(...) ((void)0)

#endif