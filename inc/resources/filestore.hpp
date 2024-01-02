// Copyright (C) 2024 Dave Moore
//
// This file is part of Sorcery: Shadows under Llylgamyn.
//
// Sorcery: Shadows under Llylgamyn is free software: you can redistribute
// it and/or modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation, either version 2 of the License,
// or (at your option) any later version.
//
// Sorcery: Shadows under Llylgamyn is distributed in the hope that it wil
// be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
// Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Sorcery: Shadows under Llylgamyn.  If not,
// see <http://www.gnu.org/licenses/>.
//
// If you modify this program, or any covered work, by linking or combining
// it with the libraries referred to in README (or a modified version of
// said libraries), containing parts covered by the terms of said libraries,
// the licensors of this program grant you additional permission to convey
// the resulting work.

#pragma once

#include "main.hpp"
#include <libgen.h>
#include <limits.h>
#include <unistd.h>

// Class to handles managing filepaths
namespace Sorcery {

class FileStore {

	public:

		// Constructors
		FileStore();

		// Overloaded Operators
		auto operator[](std::string_view key) const -> std::filesystem::path;

		// Public Methods
		auto get_path(std::string_view key) const -> std::string;

		// Public Members
		auto get_base_path() const -> std::filesystem::path;

	private:

		// Private Members
		std::map<std::string_view, std::filesystem::path> _file_paths;
		std::filesystem::path _base_path;

		// Private Methods
		auto _add_path(const std::string_view dir, const std::string_view file) -> void;
		auto _get_exe_path() -> std::string_view;
};

} // namespace Sorcery
