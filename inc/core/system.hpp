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
#include "core/animation.hpp"
#include "core/database.hpp"
#include "core/include.hpp"
#include "core/random.hpp"
#include "resources/filestore.hpp"
#include "resources/stringstore.hpp"
#include "types/config.hpp"

namespace Sorcery {

class System {

	public:
		System(int argc, char **argv);

		auto convert_tp_to_str(
			const std::chrono::time_point<std::chrono::system_clock> tp) const
			-> std::string;
		auto dice_roll_to_str(const std::string &message, const int dice,
							  const int roll, const int needed) const
			-> std::string;

		std::unique_ptr<Animation> animation;
		std::unique_ptr<Config> config;
		std::unique_ptr<Database> db;
		std::unique_ptr<FileStore> files;
		std::unique_ptr<StringStore> strings;
		std::unique_ptr<Random> random;

	private:
		std::unique_ptr<CSimpleIniA> _settings;
};

}