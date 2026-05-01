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

#include "common/include.hpp"
#include "common/types.hpp"
#include "core/macro.hpp"
#include "resources/define.hpp"
#include "types/enum.hpp"
// #pragma GCC diagnostic push
// #pragma GCC diagnostic ignored "-Wswitch-default"
// #pragma GCC diagnostic ignored "-Wmissing-declarations"
#include <ft2build.h>
#include FT_FREETYPE_H
// #pragma GCC diagnostic pop

namespace Sorcery {

struct Context;

struct FontInfo {
		std::string name;
		std::string path;
		ImFont *font = nullptr;
		bool is_monospace = false;
		Enums::Layout::Font font_type = Enums::Layout::Font::NO_FONT;
};

class FontStore {
	public:
		FontStore(Context &ctx, ImGuiIO &io);
		~FontStore();

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
		auto get_current_monospace_font_name() const -> std::string;
		auto get_all_fonts() const -> const std::vector<FontInfo> &;
		auto get_default_font() const -> ImFont *;
		auto get_all_monospace_fonts() const -> const std::vector<FontInfo>;

	private:
		Context &_ctx;
		ImGuiIO &_io;
		std::vector<FontInfo> fonts;
		std::map<Enums::Layout::Font, ImFont *> current_fonts;
		ImFont *_current_font{nullptr};
		ImFont *_default_font{nullptr};
		FT_Library _ft;

		auto _is_valid_ttf(const std::string &path) const -> bool;
		auto _is_monospace_ttf(const std::string &path) const -> bool;
		auto _get_font_full_name(const std::vector<unsigned char> &buffer)
			-> std::string;
		auto _get_fonts() const -> const std::vector<FontInfo> &;
		auto _load_font(const std::string &path, float size, bool is_monospace,
						Enums::Layout::Font font_type) -> void;
		auto _sort_fonts_by_name(bool case_insensitive = true) -> void;
};
} // namespace Sorcery