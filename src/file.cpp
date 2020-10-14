// Copyright (C) 2020 Dave Moore
//
// This file is part of Sorcery: Dreams of the Mad Overlord.
//
// Sorcery: Dreams of the Mad Overlord is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Sorcery: Dreams of the Mad Overlord is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Sorcery: Dreams of the Mad Overlord.  If not, see <http://www.gnu.org/licenses/>.
//
// If you modify this Program, or any covered work, by linking or combining it
// with the libraries referred to in README (or a modified version of said
// libraries), containing parts covered by the terms of said libraries, the
// licensors of this Program grant you additional permission to convey the
// resulting work.

#include "file.hpp"

// Standard Constructor
Sorcery::File::File()
{
	_base_path = _get_exe_path();

	// Generate and add the file paths
	_file_paths.clear();
	_add_path(CONFIG_DIR, DATABASE_FILE);
	_add_path(CONFIG_DIR, CONFIG_FILE);
	_add_path(DATA_DIR, EXPLAIN_FILE);
	_add_path(DATA_DIR, LAYOUT_FILE);
	_add_path(DATA_DIR, MONO_FONT_FILE);
	_add_path(DATA_DIR, PROPORTIONAL_FONT_FILE);
	_add_path(DATA_DIR, TEXT_FONT_FILE);
	_add_path(DATA_DIR, STRINGS_FILE);
	_add_path(DOCUMENTS_DIR, LICENSE_FILE);
	_add_path(DOCUMENTS_DIR, HELP_FILE);
	_add_path(DOCUMENTS_DIR, README_FILE);
	_add_path(DOCUMENTS_DIR, VERSION_FILE);
	_add_path(GRAPHICS_DIR, BACKGROUND_TEXTURE);
	_add_path(GRAPHICS_DIR, BANNER_TEXTURE);
	_add_path(GRAPHICS_DIR, CREATURES_TEXTURE);
	_add_path(GRAPHICS_DIR, LOGO_TEXTURE);
	_add_path(GRAPHICS_DIR, NINEPATCH_TEXTURE);
	_add_path(GRAPHICS_DIR, SPLASH_TEXTURE);
	_add_path(GRAPHICS_DIR, UI_TEXTURE);
	_add_path(VIDEO_DIR, MENU_VIDEO);
}

// Standard Destructor
Sorcery::File::~File()
{
}

// Overload [] Operator
auto Sorcery::File::operator [](std::string_view key) const -> std::filesystem::path
{
	if (_file_paths.find(key) != _file_paths.end())
		return (_file_paths.at(key));
	else
		return FILE_NOT_FOUND;
}

auto Sorcery::File::get_path_as_string(std::string_view key) const -> std::string
{
	if (_file_paths.find(key) != _file_paths.end())
		return (_file_paths.at(key)).string();
	else
		return FILE_NOT_FOUND;
}

auto Sorcery::File::get_base_path() const -> std::filesystem::path
{
    return _base_path;
}

auto Sorcery::File::_add_path(const std::string_view dir, const std::string_view file) -> void
{
	//const std::string _file {file};
	const std::filesystem::path file_path {_base_path / dir / file};
	_file_paths[file] = file_path;
}

// This is linux only
auto Sorcery::File::_get_exe_path() -> std::string_view
{
	char result[PATH_MAX];
	const ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
	const char *path;
	if (count != -1) {
		path = dirname(result);
		std::string_view base_path {path};
		return base_path;
	} else {
		return "";
	}
}