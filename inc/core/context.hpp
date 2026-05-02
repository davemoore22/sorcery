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

#include "common/enum.hpp"
#include "common/include.hpp"

#pragma once

namespace Sorcery {

class Resources;
class Controller;
class Component;
class Display;
class Game;
class Config;
class Database;
class FileStore;
class FontStore;
class ImageStore;
class ComponentStore;
class Animation;
class AudioPlayer;
class StringStore;
class Random;
class System;
class UI;
class Application;
class MenuBuilder;

// Context struct for simplying DI
struct Context {

		Application *application = nullptr;
		System *system = nullptr;
		Resources *resources = nullptr;
		UI *ui = nullptr;
		Controller *controller = nullptr;
		Display *display = nullptr;
		Game *game = nullptr;
		Animation *animation = nullptr;
		AudioPlayer *audio = nullptr;
		Config *config = nullptr;
		Database *database = nullptr;
		FileStore *files = nullptr;
		Random *random = nullptr;
		StringStore *strings = nullptr;
		ComponentStore *components = nullptr;
		FontStore *fonts = nullptr;
		ImageStore *images = nullptr;
		MenuBuilder *menubuilder = nullptr;

		// Helpers
		auto get_random(const Enums::System::Random random_type)
			-> unsigned int;
		auto get_string(std::string_view key) -> std::string;
		auto get_config(const unsigned int i) -> bool &;
		auto get_config(std::string_view section, std::string_view value) const
			-> std::string;
		auto get_file(std::string_view key) const -> std::filesystem::path;
		auto get_component(std::string_view combined_key) -> Component &;
		auto get_flag_ref(const std::string &flag) -> bool &;
		auto get_flag(const std::string &flag) -> bool;
		auto get_selected(const std::string &flag) const -> int;
};

}