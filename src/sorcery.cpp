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

#include "core/application.hpp"

#include <memory>
#include <meta>
#include <string_view>

struct Test {
		int health;
		int mana;
};

// Executablele Entry Point
auto main(int argc, char *argv[]) -> int {

	constexpr auto info = ^^Test;

	static_assert(std::meta::is_type(info));
	static_assert(std::meta::display_string_of(info) == "Test");

	// Create an Application Instance
	auto app{std::make_unique<Sorcery::Application>(argc, argv)};

	// Start it
	const auto exit_code{app->start()};

	// Stop it
	app->stop();

	// Quit
	return exit_code;
}