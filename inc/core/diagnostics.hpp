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

#include <atomic>
#include <chrono>
#include <cstddef>
#include <deque>
#include <mutex>
#include <source_location>
#include <string>
#include <string_view>
#include <vector>

namespace Sorcery {

enum class LogLevel {
	DEBUG,
	INFO,
	WARNING,
	ERROR
};

struct LogEntry {
		std::chrono::system_clock::time_point timestamp;
		LogLevel level;
		std::string message;
		std::source_location location;
};

class Diagnostics {
	public:
		static auto instance() -> Diagnostics &;

		auto set_debug_enabled(bool enabled) -> void;
		auto debug_enabled() const -> bool;

		auto set_terminal_enabled(bool enabled) -> void;

		auto log(LogLevel level, std::string_view message,
				 std::source_location location = std::source_location::current()) -> void;

		auto entries() const -> std::vector<LogEntry>;
		auto clear() -> void;

	private:
		Diagnostics() = default;

		static constexpr std::size_t MAX_ENTRIES{2000};

		mutable std::mutex _mutex;
		std::deque<LogEntry> _entries;

		std::atomic_bool _debug_enabled{false};
		std::atomic_bool _terminal_enabled{false};
};

}