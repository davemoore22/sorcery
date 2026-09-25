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

#include "core/context.hpp"
#include "core/application.hpp"			  // for Application
#include "core/controller/controller.hpp" // for Controller
#include "core/random.hpp"				  // for Random
#include "resources/componentstore.hpp"	  // for ComponentStore
#include "resources/filestore.hpp"		  // for FileStore
#include "resources/stringstore.hpp"	  // for StringStore
#include "types/config.hpp"				  // for Config

namespace Sorcery {
namespace Enums {
	namespace System { enum class Random; }
}
}

/// @brief
/// @return
auto Sorcery::Context::tick() -> void {

	application->update();
};

/// @brief
/// @param random_type
/// @return
auto Sorcery::Context::get_random(const Enums::System::Random random_type) -> unsigned int {

	return random->get(random_type);
}

/// @brief
/// @param key
/// @return
auto Sorcery::Context::get_string(std::string_view key) -> std::string {

	return strings->get(key);
}

/// @brief
/// @param option
/// @return
auto Sorcery::Context::get_config(Enums::Config::Options option) -> bool & {

	return config->get(option);
}

/// @brief
/// @param option
/// @return
auto Sorcery::Context::get_config(Enums::Config::Options option) const -> bool {

	return config->get(option);
}

/// @brief
/// @param section
/// @param value
/// @return
auto Sorcery::Context::get_config(std::string_view section, std::string_view value) const -> std::string {

	return config->get(section, value);
}

/// @brief
/// @param key
/// @return
auto Sorcery::Context::get_file(std::string_view key) const -> std::filesystem::path {

	return files->get(key);
}

/// @brief
/// @param key
/// @return
auto Sorcery::Context::get_directory(std::string_view key) const -> std::filesystem::path {

	return files->get_directory(key);
}

/// @brief
/// @param combined_key
/// @return
auto Sorcery::Context::get_component(std::string_view combined_key) -> Component & {

	return components->get(combined_key);
}

/// @brief
/// @param flag
/// @return
auto Sorcery::Context::get_flag_ref(const std::string &flag) -> bool & {

	return controller->get_flag_ref(flag);
}

/// @brief
/// @param flag
/// @return
auto Sorcery::Context::get_flag(const std::string &flag) -> bool {

	return controller->get_flag(flag);
}

/// @brief
/// @param flag
/// @return
auto Sorcery::Context::get_selected(const std::string &flag) const -> int {

	return controller->get_selected(flag);
}