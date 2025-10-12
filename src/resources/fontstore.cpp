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
#include "core/system.hpp"

Sorcery::FontStore::FontStore(System *system, ImGuiIO &io)
	: _system(system),
	  _io(io) {

	auto font_size{std::stof((*_system->config).get("Font", "size"))};

	// Load main font categories
	_fonts[Enums::Layout::Font::DEFAULT] = _io.Fonts->AddFontDefault();
	_fonts[Enums::Layout::Font::PROPORTIONAL] = _io.Fonts->AddFontFromFileTTF(
		CSTR((*_system->files)[PROPORTIONAL_FONT_FILE]), font_size);
	_fonts[Enums::Layout::Font::TEXT] = _io.Fonts->AddFontFromFileTTF(
		CSTR((*_system->files)[TEXT_FONT_FILE]), font_size);

	// Load monospace variants
	_monospace_fonts[Enums::Layout::MonospaceVariant::IBM_CGA] =
		_io.Fonts->AddFontFromFileTTF(
			CSTR((*_system->files)[MONOSPACE_D_FONT_FILE]), font_size);
	_monospace_fonts[Enums::Layout::MonospaceVariant::WIZ1_4_DOS] =
		_io.Fonts->AddFontFromFileTTF(
			CSTR((*_system->files)[MONOSPACE_A_FONT_FILE]), font_size);
	_monospace_fonts[Enums::Layout::MonospaceVariant::WIZ5_DOS] =
		_io.Fonts->AddFontFromFileTTF(
			CSTR((*_system->files)[MONOSPACE_B_FONT_FILE]), font_size);
	_monospace_fonts[Enums::Layout::MonospaceVariant::WIZ5_FMTOWNS] =
		_io.Fonts->AddFontFromFileTTF(
			CSTR((*_system->files)[MONOSPACE_C_FONT_FILE]), font_size);
	_monospace_fonts[Enums::Layout::MonospaceVariant::APPLE_II] =
		_io.Fonts->AddFontFromFileTTF(
			CSTR((*_system->files)[MONOSPACE_E_FONT_FILE]), font_size);
	_monospace_fonts[Enums::Layout::MonospaceVariant::AMSTRAD_CPC] =
		_io.Fonts->AddFontFromFileTTF(
			CSTR((*_system->files)[MONOSPACE_F_FONT_FILE]), font_size);

	// Default selection
	_current_font = _fonts[Enums::Layout::Font::DEFAULT];

	// Can't set current font here as its too early in the ImGui init process
	// ImGui::SetCurrentFont(_current_font);
}

auto Sorcery::FontStore::set_current_font(Enums::Layout::Font type) -> void {
	_current_font_type = type;

	if (type == Enums::Layout::Font::MONOSPACE) {
		auto mono = _monospace_fonts[_current_mono_variant];
		_current_font = mono ? mono : _fonts[Enums::Layout::Font::DEFAULT];
	} else if (auto it = _fonts.find(type); it != _fonts.end())
		_current_font = it->second;
	else
		_current_font = _fonts[Enums::Layout::Font::DEFAULT];

	ImGui::SetCurrentFont(_current_font);
}

auto Sorcery::FontStore::set_monospace_variant(
	Enums::Layout::MonospaceVariant variant) -> void {
	_current_mono_variant = variant;

	// If currently using MONOSPACE, apply the change immediately
	if (_current_font_type == Enums::Layout::Font::MONOSPACE) {
		if (auto it = _monospace_fonts.find(variant);
			it != _monospace_fonts.end())
			_current_font = it->second;
		else
			_current_font = _fonts[Enums::Layout::Font::DEFAULT];

		ImGui::SetCurrentFont(_current_font);
	}
}

auto Sorcery::FontStore::get_current_font() const -> ImFont * {
	return _current_font;
}

auto Sorcery::FontStore::get_current_font_type() const -> Enums::Layout::Font {
	return _current_font_type;
}

auto Sorcery::FontStore::get_current_monospace_variant() const
	-> Enums::Layout::MonospaceVariant {
	return _current_mono_variant;
}
