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

#include "resources/fontstore.hpp"
#include "common/imgui.hpp"
#include "core/context.hpp"
#include "core/system.hpp"
#include "resources/define.hpp"
#include "types/config.hpp"

#include <fstream>

Sorcery::FontStore::FontStore(Context &ctx, ImGuiIO *io)
	: _ctx(ctx),
	  _io(io) {

	FT_Init_FreeType(&_ft);

	// Get the Font Size from config
	auto font_size{std::stof(_ctx.get_config("Font", "size"))};

	// Now scan the data directory for TTF fonts
	const std::filesystem::path file_path{DATA_DIR};
	scan_and_load(file_path.string(), font_size);
	_sort_fonts_by_name();
}

Sorcery::FontStore::~FontStore() {

	if (_ft)
		FT_Done_FreeType(_ft);
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
	_default_font = _io->Fonts->AddFontDefault();

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

	if (!_default_font)
		_default_font = _io->Fonts->AddFontDefault();

	// Ensure all font slots are populated
	using enum Enums::Layout::Font;

	for (auto type : {DEFAULT, TEXT, PROPORTIONAL, MONOSPACE}) {
		if (!current_fonts.contains(type))
			current_fonts[type] = _default_font;
	}

	_io->Fonts->Build();
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

	ImFont *font{_io->Fonts->AddFontFromFileTTF(path.c_str(), size, &config)};
	if (!font) {

		std::cerr << "Failed to load font: " << path << "\n";
		return;
	}

	fonts.push_back({name, path, font, is_monospace, font_type});
}

// Attempt to validate a TTF font file by loading its header using stb_truetype
auto Sorcery::FontStore::_is_valid_ttf(const std::string &path) const -> bool {

	FT_Face face = nullptr;

	const FT_Error err = FT_New_Face(_ft, path.c_str(), 0, &face);

	if (err != 0) {
		return false;
	}

	FT_Done_Face(face);
	return true;
}

auto Sorcery::FontStore::_get_font_full_name(
	const std::vector<unsigned char> &buffer) -> std::string {

	FT_Face face = nullptr;

	// Load font from memory instead of file
	if (FT_New_Memory_Face(_ft, buffer.data(),
						   static_cast<FT_Long>(buffer.size()), 0,
						   &face) != 0) {
		return "";
	}

	std::string result;

	// Prefer full name if available
	if (face->family_name && face->style_name) {
		result = std::string(face->family_name) + " " + face->style_name;
	} else if (face->family_name) {
		result = face->family_name;
	}

	FT_Done_Face(face);

	return result;
}

// Is a valid TTF font a monospace font?
auto Sorcery::FontStore::_is_monospace_ttf(const std::string &path) const
	-> bool {

	FT_Face face = nullptr;

	if (FT_New_Face(_ft, path.c_str(), 0, &face) != 0)
		return false;

	// 1. Trust font metadata first
	if (FT_IS_FIXED_WIDTH(face)) {
		FT_Done_Face(face);
		return true;
	}

	// 2. Fallback heuristic (your old logic, but using FreeType)
	const char *test_chars = "iIlLWMw1 0";

	FT_Set_Pixel_Sizes(face, 0, 16);

	int ref_advance = -1;

	for (const char *c = test_chars; *c; ++c) {
		if (FT_Load_Char(face, *c, FT_LOAD_DEFAULT) != 0)
			continue;

		int advance = face->glyph->advance.x;

		if (ref_advance == -1)
			ref_advance = advance;
		else if (advance != ref_advance) {
			FT_Done_Face(face);
			return false;
		}
	}

	FT_Done_Face(face);
	return true;
};

auto Sorcery::FontStore::set_current_font(Enums::Layout::Font type,
										  ImFont *font) -> void {

	if (!font)
		return;

	current_fonts[type] = font;

	// Also set ImGui's default if Default font type
	if (type == Enums::Layout::Font::DEFAULT)
		_io->FontDefault = font;
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

	if (case_insensitive) {

		const auto icompare = [](std::string_view lhs,
								 std::string_view rhs) -> bool {
			return std::ranges::lexicographical_compare(
				lhs, rhs, {},
				[](unsigned char c) {
					return static_cast<char>(std::tolower(c));
				},
				[](unsigned char c) {
					return static_cast<char>(std::tolower(c));
				});
		};

		std::ranges::sort(fonts, [&](const FontInfo &a, const FontInfo &b) {
			return icompare(a.name, b.name);
		});

	} else {

		std::ranges::sort(fonts, {}, &FontInfo::name);
	}
}