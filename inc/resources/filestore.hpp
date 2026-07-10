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

#include <filesystem>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace Sorcery {

class FileStore {
	public:
		FileStore();

		[[nodiscard]] auto get(std::string_view key) const
			-> std::filesystem::path;

		[[nodiscard]] auto get_path(std::string_view key) const -> std::string;

		[[nodiscard]] auto get_base_path() const -> std::filesystem::path;

	private:
		auto _add_path(std::string_view dir, std::string_view file,
					   bool required = true) -> void;

		[[nodiscard]] auto _get_exe_path() const -> std::filesystem::path;

		auto _validate_files() const -> void;

		std::filesystem::path _base_path;
		std::unordered_map<std::string, std::filesystem::path> _file_paths;
		std::vector<std::filesystem::path> _required_files;
};

}