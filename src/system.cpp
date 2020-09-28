// Copyright (C) 2020 Dave Moore
//
// This file is part of Sorcery: Dreams of the Mad Overlord.
//
// Sorcery: Dreams of the Mad Overlord is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Sorcery: Dreams of the Mad Overlord is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Sorcery: Dreams of the Mad Overlord.  If not, see <http://www.gnu.org/licenses/>.
//
// If you modify this Program, or any covered work, by linking or combining it
// with the libraries referred to in README (or a modified version of said
// libraries), containing parts covered by the terms of said libraries, the
// licensors of this Program grant you additional permission to convey the
// resulting work.

#include "system.hpp"

// Standard Constructor
Sorcery::System::System(int argc __attribute__((unused)), char** argv __attribute__((unused))) {

	// Files Module
	files = std::make_shared<File>();

	// Settings File/Config Module
	settings = std::make_shared<CSimpleIniA>();
	settings->SetUnicode();
	const std::string settings_file_path {(*files)[CONFIG_FILE]};
	settings->LoadFile(CSTR(settings_file_path));
	config = std::make_shared<Config>(settings.get(), (*files)[CONFIG_FILE]);

	// Game Database
	database = std::make_shared<Database>((*files)[DATABASE_FILE]);

	// Random Module
	random = std::make_shared<Random>();

	// Resource Manager
	resources = std::make_shared<ResourceManager>(*files);
}

// Standard Destructor
Sorcery::System::~System() {
}


