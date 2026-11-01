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

#include "core/diagnostics.hpp"

#include <print>

auto Sorcery::Diagnostics::instance() -> Diagnostics & {

	static Diagnostics diagnostics{};
	return diagnostics;
}

auto Sorcery::Diagnostics::set_debug_enabled(bool enabled) -> void {

	_debug_enabled.store(enabled, std::memory_order_relaxed);
}

auto Sorcery::Diagnostics::debug_enabled() const -> bool {

	return _debug_enabled.load(std::memory_order_relaxed);
}

auto Sorcery::Diagnostics::set_terminal_enabled(bool enabled) -> void {

	_terminal_enabled.store(enabled, std::memory_order_relaxed);
}

auto Sorcery::Diagnostics::log(LogLevel level, std::string_view message, std::source_location location) -> void {

	if (level == LogLevel::DEBUG && !debug_enabled())
		return;

	LogEntry entry{.timestamp = std::chrono::system_clock::now(),
				   .level = level,
				   .message = std::string{message},
				   .location = location};

	{
		std::scoped_lock lock{_mutex};

		if (_entries.size() >= MAX_ENTRIES)
			_entries.pop_front();

		_entries.push_back(entry);
	}

	/*
	if (_terminal_enabled.load(std::memory_order_relaxed))
		std::println("[{}] {}",
					 level == LogLevel::DEBUG	  ? "DEBUG"
					 : level == LogLevel::INFO	  ? "INFO"
					 : level == LogLevel::WARNING ? "WARNING"
												  : "ERROR",
					 entry.message); */

	if (_terminal_enabled.load(std::memory_order_relaxed)) {
		std::println("[{}:{}] {}", entry.location.file_name(), entry.location.line(), entry.message);
	}
}

auto Sorcery::Diagnostics::entries() const -> std::vector<LogEntry> {

	std::scoped_lock lock{_mutex};

	return {_entries.begin(), _entries.end()};
}

auto Sorcery::Diagnostics::clear() -> void {

	std::scoped_lock lock{_mutex};
	_entries.clear();
}