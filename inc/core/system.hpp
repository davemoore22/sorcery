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

#pragma once

#include "core/config.hpp"
#include "core/database.hpp"
#include "core/input.hpp"
#include "core/random.hpp"
#include "main.hpp"
#include "resources/filestore.hpp"
#include "resources/resourcemanager.hpp"
#include "resources/stringstore.hpp"

// Superclass to handle all system level requirements such as Random Number
// Generation, File and Database Access, etc
namespace Sorcery {

class System {

	public:

		// Constructors
		System(int argc, char **argv);

		// Public Methods
		auto set_pause(unsigned int milliseconds) -> void;
		auto update_pause() -> bool;
		auto stop_pause() -> void;
		auto get_pause() -> bool;
		auto convert_tp_to_str(const std::chrono::time_point<std::chrono::system_clock> tp) const -> std::string;
		auto dice_roll_to_str(
			const std::string &message, const int dice, const int roll, const int needed) const -> std::string;

		// Public Members
		std::unique_ptr<FileStore> files;
		std::unique_ptr<CSimpleIniA> settings;
		std::unique_ptr<Config> config;
		std::unique_ptr<Random> random;
		std::unique_ptr<Database> database;
		std::unique_ptr<ResourceManager> resources;
		std::unique_ptr<Input> input;

	private:

		// Private Members
		std::optional<std::chrono::steady_clock::time_point> _pause_clock_start;
		std::optional<unsigned int> _clock_duration;

		// Private Methods
};

}