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

#include "common/define.hpp"
#include "common/include.hpp"
#include "types/define.hpp"
#include "types/include.hpp"

namespace Sorcery {

class Config {

	public:
		Config(CSimpleIniA *settings, const std::filesystem::path cfg_path);
		Config() = delete;

		auto operator[](const unsigned int i) -> bool &;

		auto get(std::string_view section, std::string_view value) const
			-> std::string;
		auto has_changed() -> bool;
		auto load() -> bool;
		auto save() -> bool;
		auto store() -> void;
		auto set_rec_mode() -> void;
		auto set_strict_mode() -> void;
		auto is_strict_mode() const -> bool;
		auto is_rec_mode() const -> bool;

	private:
		CSimpleIniA *_settings;
		std::filesystem::path _cfg_path;
		std::array<bool, NUM_GAME_SETTINGS> _options;
		std::array<bool, NUM_GAME_SETTINGS> _options_bkp;

		auto _load() -> bool;
};

// Macro to convert bool to C string
inline auto BOOL2OPTIONCSTR(bool bool_to_convert) -> const char * {

	return bool_to_convert ? "on" : "off";
}

}