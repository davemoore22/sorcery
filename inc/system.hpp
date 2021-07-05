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

#include "config.hpp"
#include "database.hpp"
#include "file.hpp"
#include "input.hpp"
#include "main.hpp"
#include "random.hpp"
#include "resourcemanager.hpp"
#include "string.hpp"

// Superclass to handle all system level requirements such as Random Number
// Generation, File and Database Access, etc
namespace Sorcery {

	class System {

	  public:
		// Constructors
		System(int argc, char **argv);

		// Copy Constructors
		System(const System &other);
		auto operator=(const System &other) -> System &;

		// Public Methods

		// Public Members
		std::shared_ptr<File> files;
		std::shared_ptr<CSimpleIniA> settings;
		std::shared_ptr<Config> config;
		std::shared_ptr<Random> random;
		std::shared_ptr<Database> database;
		std::shared_ptr<ResourceManager> resources;
		std::shared_ptr<Input> input;

		// also need for each class to contain a list of mouse-sensitive area
		// based upon component, so that you can check if a mouse cursor is in a
		// component

	  private:
		// Private Members

		// Private Methods
	};
} // namespace Sorcery