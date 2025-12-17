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

#include "resources/fontstore.hpp"
#include "core/context.hpp"
#include "core/system.hpp"
#include "resources/define.hpp"
#include "types/config.hpp"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch-default"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb/stb_truetype.h"
#pragma GCC diagnostic pop

Sorcery::FontStore::FontStore(Context &ctx, ImGuiIO &io)
	: _ctx(ctx),
	  _io(io) {

	// Get the Font Size from config
	auto font_size{std::stof(_ctx.get_config("Font", "size"))};

	// Now scan the data directory for TTF fonts
	const std::filesystem::path file_path{DATA_DIR};
	scan_and_load(file_path.string(), font_size);
	_sort_fonts_by_name();
}

auto Sorcery::FontStore::_get_fonts() const -> const std::vector<FontInfo> & {

	return fonts;
}

auto Sorcery::FontStore::get_font_by_name(const std::string &name) const
	-> std::optional<ImFont *> {

	for (const auto &f : fonts)
		if (f.name == name)
			return f.font;
	return std::nullopt;
}

auto Sorcery::FontStore::get_default_font() const -> ImFont * {

	return _default_font;
}

auto Sorcery::FontStore::scan_and_load(const std::string &directory,
									   float font_size) -> void {

	fonts.clear();
	current_fonts.clear();

	// Always load internal ImGui font
	_default_font = _io.Fonts->AddFontDefault();

	for (const auto &entry : std::filesystem::directory_iterator(directory)) {
		if (!entry.is_regular_file())
			continue;

		auto ext{entry.path().extension().string()};
		auto stem{entry.path().stem().string()};

		using enum Enums::Layout::Font;
		auto font_type{NO_FONT};
		if (stem == "text")
			font_type = TEXT;
		else if (stem == "proportional")
			font_type = PROPORTIONAL;
		else
			font_type = MONOSPACE;

		if (ext == ".ttf" || ext == ".TTF") {
			const auto font_path{entry.path().string()};
			if (_is_valid_ttf(font_path)) {
				auto mono{_is_monospace_ttf(font_path)};
				_load_font(font_path, font_size, mono, font_type);
			} else {
				std::cerr << "Invalid font skipped: " << font_path << "\n";
			}
		}
	}

	ImGui::GetIO().Fonts->Build();
}

auto Sorcery::FontStore::_load_font(const std::string &path, float size,
									bool is_monospace,
									Enums::Layout::Font font_type) -> void {

	std::ifstream file(path, std::ios::binary);
	if (!file.is_open())
		return;

	std::vector<unsigned char> buffer((std::istreambuf_iterator<char>(file)),
									  {});

	std::string name = _get_font_full_name(buffer);
	if (name.empty())
		name = std::filesystem::path(path).stem().string();

	ImFontConfig config;
	config.OversampleH = 3;
	config.OversampleV = 3;
	config.PixelSnapH = false;

	ImFont *font{_io.Fonts->AddFontFromFileTTF(path.c_str(), size, &config)};
	if (!font) {

		std::cerr << "Failed to load font: " << path << "\n";
		return;
	}

	fonts.push_back({name, path, font, is_monospace, font_type});
}

// Attempt to validate a TTF font file by loading its header using stb_truetype
auto Sorcery::FontStore::_is_valid_ttf(const std::string &path) const -> bool {

	std::ifstream file(path, std::ios::binary);
	if (!file.is_open())
		return false;

	std::vector<unsigned char> buffer((std::istreambuf_iterator<char>(file)),
									  {});
	if (buffer.empty())
		return false;

	stbtt_fontinfo info;
	auto offset{stbtt_GetFontOffsetForIndex(buffer.data(), 0)};
	if (offset < 0)
		return false;

	return stbtt_InitFont(&info, buffer.data(), offset) != 0;
}

auto Sorcery::FontStore::_get_font_full_name(
	const std::vector<unsigned char> &buffer) -> std::string {

	stbtt_fontinfo info;
	int offset{stbtt_GetFontOffsetForIndex(buffer.data(), 0)};
	if (offset < 0)
		return "";
	if (!stbtt_InitFont(&info, buffer.data(), offset))
		return "";

	const int NAME_ID_FULL_FONT_NAME{4};

	// Windows platform, Unicode BMP, English
	const int PLATFORM_ID{3};
	const int ENCODING_ID{1};
	const int LANGUAGE_ID{0x0409};

	char *name_string{nullptr};
	int name_length{0};
	name_string = (char *)stbtt_GetFontNameString(
		&info, &name_length, PLATFORM_ID, ENCODING_ID, LANGUAGE_ID,
		NAME_ID_FULL_FONT_NAME);
	if (!name_string || name_length <= 0)
		return "";

	// Some fonts store UTF-16BE strings
	std::string result;
	if (PLATFORM_ID == 3) { // Windows, UTF-16BE
		result.reserve(name_length / 2);
		for (int i = 0; i < name_length; i += 2)
			result.push_back(name_string[i + 1]);
	} else {
		result.assign(name_string, name_length);
	}

	return result;
}

// Is a valid TTF font a monospace font?
auto Sorcery::FontStore::_is_monospace_ttf(const std::string &path) const
	-> bool {

	std::ifstream file(path, std::ios::binary);
	if (!file.is_open())
		return false;

	std::vector<unsigned char> buffer((std::istreambuf_iterator<char>(file)),
									  {});
	if (buffer.empty())
		return false;

	stbtt_fontinfo info;
	auto offset{stbtt_GetFontOffsetForIndex(buffer.data(), 0)};
	if (offset < 0)
		return false;

	if (!stbtt_InitFont(&info, buffer.data(), offset))
		return false;

	// Use a simple heuristic across key glyphs
	const char *test_chars = "iIlLWMw1 0";
	auto ref_advance{-1};

	for (const char *c = test_chars; *c; ++c) {

		int advance{}, lsb{};
		stbtt_GetCodepointHMetrics(&info, *c, &advance, &lsb);
		if (ref_advance == -1)
			ref_advance = advance;
		else if (advance != ref_advance)
			return false;
	}
	return true;
};

auto Sorcery::FontStore::set_current_font(Enums::Layout::Font type,
										  ImFont *font) -> void {

	if (!font)
		return;

	current_fonts[type] = font;

	// Also set ImGui's default if Default font type
	if (type == Enums::Layout::Font::DEFAULT)
		_io.FontDefault = font;
}

auto Sorcery::FontStore::set_current_font(Enums::Layout::Font type,
										  const std::string &name) -> void {

	auto font{get_font_by_name(name)};
	if (font)
		set_current_font(type, font.value());
}

auto Sorcery::FontStore::get_current_font(Enums::Layout::Font type) const
	-> std::optional<ImFont *> {

	if (auto it = current_fonts.find(type); it != current_fonts.end())
		return it->second;

	return std::nullopt;
}

auto Sorcery::FontStore::get_current_monospace_font() const
	-> std::optional<ImFont *> {

	return get_current_font(Enums::Layout::Font::MONOSPACE);
}

auto Sorcery::FontStore::get_current_monospace_font_name() const
	-> std::string {

	ImFont *current_mono =
		get_current_font(Enums::Layout::Font::MONOSPACE).value();
	if (!current_mono)
		return "";

	for (const auto &font : fonts) {
		if (font.font == current_mono)
			return font.name;
	}

	return "";
}

auto Sorcery::FontStore::get_all_fonts() const
	-> const std::vector<FontInfo> & {

	return fonts;
}

auto Sorcery::FontStore::get_all_monospace_fonts() const
	-> const std::vector<FontInfo> {

	std::vector<FontInfo> monospace_fonts;
	for (const auto &font : fonts) {
		if (font.is_monospace &&
			font.font_type == Enums::Layout::Font::MONOSPACE) {
			monospace_fonts.push_back(font);
		}
	}
	return monospace_fonts;
}

auto Sorcery::FontStore::_sort_fonts_by_name(bool case_insensitive) -> void {

	if (fonts.empty())
		return;

	if (case_insensitive) {
		std::ranges::sort(fonts, [](const FontInfo &a, const FontInfo &b) {
			auto toLower = [](std::string_view s) {
				std::string result;
				result.reserve(s.size());
				for (unsigned char c : s)
					result.push_back(static_cast<char>(std::tolower(c)));
				return result;
			};
			return toLower(a.name) < toLower(b.name);
		});
	} else {
		std::ranges::sort(fonts, {},
						  &FontInfo::name); // sort by member directly
	}
}
