// Copyright (C) 2025 Dave Moore
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

#include "common/include.hpp"
#include "common/types.hpp"
#include "core/macro.hpp"
#include "types/enum.hpp"

namespace Sorcery {

class System;

struct FontInfo {
		std::string name;
		std::string path;
		ImFont *font = nullptr;
		bool is_monospace = false;
		Enums::Layout::Font font_type = Enums::Layout::Font::NO_FONT;
};

class FontStore {
	public:
		FontStore(System *system, ImGuiIO &io);

		auto scan_and_load(const std::string &directory,
						   float font_size = 16.0f) -> void;
		auto get_font_by_name(const std::string &name) const
			-> std::optional<ImFont *>;
		auto set_current_font(Enums::Layout::Font type, ImFont *font) -> void;
		auto set_current_font(Enums::Layout::Font type, const std::string &name)
			-> void;
		auto get_current_font(Enums::Layout::Font type) const
			-> std::optional<ImFont *>;
		auto get_current_monospace_font() const -> std::optional<ImFont *>;
		auto get_all_fonts() const -> const std::vector<FontInfo> &;

	private:
		System *_system;
		ImGuiIO &_io;
		std::vector<FontInfo> fonts;
		std::unordered_map<Enums::Layout::Font, ImFont *> current_fonts;
		ImFont *_current_font{nullptr};

		auto _is_valid_ttf(const std::string &path) const -> bool;
		auto _is_monospace_ttf(const std::string &path) const -> bool;
		auto _get_font_full_name(const std::vector<unsigned char> &buffer)
			-> std::string;
		auto _get_fonts() const -> const std::vector<FontInfo> &;
		auto _load_font(const std::string &path, float size, bool is_monospace,
						Enums::Layout::Font font_type) -> void;

		std::map<Enums::Layout::Font, ImFont *> _fonts;
};
} // namespace Sorcery