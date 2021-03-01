// Copyright (C) 2021 Dave Moore
//
// This file is part of Sorcery: Dreams of the Mad Overlord.
//
// Sorcery: Dreams of the Mad Overlord is free software: you can redistribute
// it and/or modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation, either version 2 of the License,
// or (at your option) any later version.
//
// Sorcery: Dreams of the Mad Overlord is distributed in the hope that it wil
// be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
// Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Sorcery: Dreams of the Mad Overlord.  If not,
// see <http://www.gnu.org/licenses/>.
//
// If you modify this program, or any covered work, by linking or combining
// it with the libraries referred to in README (or a modified version of
// said libraries), containing parts covered by the terms of said libraries,
// the licensors of this program grant you additional permission to convey
// the resulting work.

#pragma once

#include "main.hpp"

// Class to handles game configuration
namespace Sorcery {

	class Config {

	  public:
		// Constructors
		Config(CSimpleIniA *settings, const std::filesystem::path config_file_path);
		Config() = delete;

		// Overload [] operator
		auto operator[](const unsigned int i) -> bool &;

		// Public Methods
		auto get(const std::string &section, const std::string &value) const -> std::string;
		auto has_changed() -> bool;
		auto load() -> bool;
		auto save() -> bool;
		auto set_recommended_mode() -> void;
		auto set_strict_mode() -> void;
		auto store_current_settings() -> void;
		auto is_strict_mode() -> bool;
		auto is_recommended_mode() -> bool;

	  private:
		// Private Members
		CSimpleIniA *_settings;								 // Dependency-injected
		std::array<bool, NUM_GAME_SETTINGS> _options;		 // Game Settings
		std::array<bool, NUM_GAME_SETTINGS> _options_backup; // To handle cancel
		std::filesystem::path _config_fp;

		// Private Methods
		auto _load() -> bool;
	};
} // namespace Sorcery