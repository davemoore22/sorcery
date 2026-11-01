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
#include <cstdint>
#include <mutex>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace Sorcery {

struct ProfileEntry {
		std::string name;

		std::uint64_t count{0};

		double last_us{0.0};
		double total_us{0.0};
		double min_us{0.0};
		double max_us{0.0};

		[[nodiscard]] auto average_us() const -> double {

			return count ? total_us / static_cast<double>(count) : 0.0;
		}
};

class Profiler {
	public:
		static auto instance() -> Profiler &;

		auto set_enabled(bool enabled) -> void;

		[[nodiscard]] auto enabled() const -> bool;

		auto record(std::string_view name, double elapsed_us) -> void;

		[[nodiscard]] auto entries() const -> std::vector<ProfileEntry>;

		auto clear() -> void;

	private:
		Profiler() = default;

		mutable std::mutex _mutex;
		std::unordered_map<std::string, ProfileEntry> _entries;

		std::atomic_bool _enabled{false};
};

}
