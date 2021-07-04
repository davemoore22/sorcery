// Copyright (C) 2021 Dave Moore
//
// This file is part of Sorcery: Dreams of the Mad Overlord
//
// Sorcery is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Sorcery is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with sorcery-sfml.  If not, see <http://www.gnu.org/licenses/>.
// Additional permission under GNU GPL version 3 section 7:
//
// If you modify this Program, or any covered work, by linking or combining it
// with the libraries referred to in README (or a modified version of said
// libraries), containing parts covered by the terms of said libraries, the
// licensors of this Program grant you additional permission to convey the
// resulting work.

#include "main.hpp"
#include "application.hpp"

auto main(int argc, char *argv[]) -> int {

	// Create an application
	auto app{std::make_unique<Sorcery::Application>(argc, argv)};

	// Start it
	int exit_code{app->start()};

	// Quit
	return exit_code;
}