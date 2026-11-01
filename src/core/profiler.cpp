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

#include "core/profiler.hpp"

#include <algorithm>
#include <utility>

auto Sorcery::Profiler::instance() -> Profiler & {

	static Profiler profiler{};
	return profiler;
}

auto Sorcery::Profiler::set_enabled(bool enabled) -> void {

	_enabled.store(enabled, std::memory_order_relaxed);
}

auto Sorcery::Profiler::enabled() const -> bool {

	return _enabled.load(std::memory_order_relaxed);
}

auto Sorcery::Profiler::record(std::string_view name, double elapsed_us) -> void {

	if (!enabled())
		return;

	std::scoped_lock lock{_mutex};

	const auto key{std::string{name}};
	auto [it, inserted]{_entries.try_emplace(key)};

	auto &entry{it->second};

	if (inserted) {
		entry.name = key;
		entry.min_us = elapsed_us;
		entry.max_us = elapsed_us;
	} else {
		entry.min_us = std::min(entry.min_us, elapsed_us);
		entry.max_us = std::max(entry.max_us, elapsed_us);
	}

	entry.last_us = elapsed_us;
	entry.total_us += elapsed_us;
	++entry.count;
}

auto Sorcery::Profiler::entries() const -> std::vector<ProfileEntry> {

	std::scoped_lock lock{_mutex};

	std::vector<ProfileEntry> result;
	result.reserve(_entries.size());

	for (const auto &[name, entry] : _entries)
		result.push_back(entry);

	std::ranges::sort(result, {}, &ProfileEntry::name);

	return result;
}

auto Sorcery::Profiler::clear() -> void {

	std::scoped_lock lock{_mutex};
	_entries.clear();
}
