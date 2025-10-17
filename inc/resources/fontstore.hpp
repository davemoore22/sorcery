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

class FontStore {
	public:
		FontStore(System *system, ImGuiIO &io);

		auto set_current_font(Enums::Layout::Font type) -> void;
		auto set_monospace_variant(Enums::Layout::MonospaceVariant variant)
			-> void;

		auto get_current_font() const -> ImFont *;
		auto get_current_font_type() const -> Enums::Layout::Font;
		auto get_current_monospace_variant() const
			-> Enums::Layout::MonospaceVariant;

		auto is_valid_ttf(const std::string &path) const -> bool;
		auto is_monospace_ttf(const std::string &path) const -> bool;

	private:
		System *_system;
		ImGuiIO &_io;
		std::map<Enums::Layout::Font, ImFont *> _fonts;
		std::map<Enums::Layout::MonospaceVariant, ImFont *> _monospace_fonts;

		ImFont *_current_font{nullptr};
		Enums::Layout::Font _current_font_type{Enums::Layout::Font::DEFAULT};
		Enums::Layout::MonospaceVariant _current_mono_variant{
			Enums::Layout::MonospaceVariant::DEFAULT_MONOSPACE};
};

} // namespace Sorcery