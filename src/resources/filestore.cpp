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

#include "resources/filestore.hpp"

// Standard Constructor
Sorcery::FileStore::FileStore() {

	// Generate and add the file paths

#ifdef linux
	_base_path = _get_exe_path();
#endif

	_file_paths.clear();
	_add_path(CONFIG_DIR, CONFIG_FILE);
	_add_path(DATA_DIR, DATABASE_FILE);
	_add_path(DATA_DIR, ICONS_FILE);
	_add_path(DATA_DIR, ITEMS_FILE);
	_add_path(DATA_DIR, LAYOUT_FILE);
	_add_path(DATA_DIR, INPUT_FONT_FILE);
	_add_path(DATA_DIR, LEVELS_FILE);
	_add_path(DATA_DIR, MONSTERS_FILE);
	_add_path(DATA_DIR, MONO_FONT_FILE);
	_add_path(DATA_DIR, PROPORTIONAL_FONT_FILE);
	_add_path(DATA_DIR, STRINGS_FILE);
	_add_path(DATA_DIR, TEXT_FONT_FILE);
	_add_path(DATA_DIR, TEXTURES_FILE);
	_add_path(DOCUMENTS_DIR, LICENSE_FILE);
	_add_path(DOCUMENTS_DIR, HELP_FILE);
	_add_path(DOCUMENTS_DIR, README_FILE);
	_add_path(DOCUMENTS_DIR, VERSION_FILE);
	_add_path(GUI_DIR, GUI_FILE);
	_add_path(GRAPHICS_DIR, AUTOMAP_TEXTURE);
	_add_path(GRAPHICS_DIR, BACKGROUND_TEXTURE);
	_add_path(GRAPHICS_DIR, BANNER_TEXTURE);
	_add_path(GRAPHICS_DIR, CONTROLS_TEXTURE);
	_add_path(GRAPHICS_DIR, CREATURES_KNOWN_TEXTURE);
	_add_path(GRAPHICS_DIR, CREATURES_UNKNOWN_TEXTURE);
	_add_path(GRAPHICS_DIR, DOORS_TEXTURE);
	_add_path(GRAPHICS_DIR, EVENTS_TEXTURE);
	_add_path(GRAPHICS_DIR, FLOORS_TEXTURE);
	_add_path(GRAPHICS_DIR, ICONS_TEXTURE);
	_add_path(GRAPHICS_DIR, ITEMS_TEXTURE);
	_add_path(GRAPHICS_DIR, LOGO_TEXTURE);
	_add_path(GRAPHICS_DIR, PORTRAITS_TEXTURE);
	_add_path(GRAPHICS_DIR, SPLASH_TEXTURE);
	_add_path(GRAPHICS_DIR, TOWN_TEXTURE);
	_add_path(GRAPHICS_DIR, UI_TEXTURE);
	_add_path(GRAPHICS_DIR, VIEW_TEXTURE);
	_add_path(GRAPHICS_DIR, WALLS_TEXTURE);
	_add_path(GRAPHICS_DIR, WIREFRAME_TEXTURE);
	_add_path(VIDEO_DIR, VIDEO_FILE);
}

// Overload [] Operator
auto Sorcery::FileStore::operator[](std::string_view key) const -> std::filesystem::path {

	return _file_paths.contains(key) ? _file_paths.at(key) : std::filesystem::path{FILE_NOT_FOUND};
}

auto Sorcery::FileStore::get_path(std::string_view key) const -> std::string {

	return _file_paths.contains(key) ? _file_paths.at(key).string() : FILE_NOT_FOUND;
}

auto Sorcery::FileStore::get_base_path() const -> std::filesystem::path {

	return _base_path;
}

auto Sorcery::FileStore::_add_path(const std::string_view dir, const std::string_view file) -> void {

	const std::filesystem::path file_path{_base_path / dir / file};
	_file_paths[file] = file_path;
}

// This is linux only
auto Sorcery::FileStore::_get_exe_path() -> std::string_view {

	char result[PATH_MAX];
	if (const ssize_t count{readlink("/proc/self/exe", result, PATH_MAX)}; count != -1) {
		const char *path{dirname(result)};
		std::string_view base_path{path};
		return base_path;
	} else
		return "";
}
