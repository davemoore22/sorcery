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

#include "core/context.hpp"
#include "core/random.hpp"
#include "resources/filestore.hpp"
#include "resources/stringstore.hpp"
#include "types/config.hpp"

auto Sorcery::Context::get_random(const Enums::System::Random random_type)
	-> unsigned int {

	return random->get(random_type);
}

auto Sorcery::Context::get_string(std::string_view key) -> std::string {

	return strings->get(key);
}

auto Sorcery::Context::get_config(const unsigned int i) -> bool & {

	return config->get(i);
}

auto Sorcery::Context::get_file(std::string_view key) const
	-> std::filesystem::path {

	return files->get(key);
}