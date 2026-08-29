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

#pragma once

#include <filesystem>  // for path
#include <string>	   // for string
#include <string_view> // for string_view
namespace Sorcery {
class Animation;
} // lines 43-43
namespace Sorcery {
class Application;
} // lines 49-49
namespace Sorcery {
class AudioPlayer;
} // lines 44-44
namespace Sorcery {
class Component;
} // lines 35-35
namespace Sorcery {
class ComponentStore;
} // lines 42-42
namespace Sorcery {
class Config;
} // lines 38-38
namespace Sorcery {
class Controller;
} // lines 34-34
namespace Sorcery {
class Display;
} // lines 36-36
namespace Sorcery {
class FileStore;
} // lines 39-39
namespace Sorcery {
class FontStore;
} // lines 40-40
namespace Sorcery {
class Game;
} // lines 37-37
namespace Sorcery {
class ImageStore;
} // lines 41-41
namespace Sorcery {
class MenuBuilder;
} // lines 50-50
namespace Sorcery {
class Random;
} // lines 46-46
namespace Sorcery {
class Resources;
} // lines 33-33
namespace Sorcery {
class SaveStore;
} // lines 51-51
namespace Sorcery {
class StringStore;
} // lines 45-45
namespace Sorcery {
class System;
} // lines 47-47
namespace Sorcery {
class UI;
} // lines 48-48
namespace Sorcery {
namespace Enums {
	namespace System {
		enum class Random;
	}
}
}

namespace Sorcery {

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
		FileStore *files = nullptr;
		Random *random = nullptr;
		StringStore *strings = nullptr;
		ComponentStore *components = nullptr;
		FontStore *fonts = nullptr;
		ImageStore *images = nullptr;
		MenuBuilder *menubuilder = nullptr;
		SaveStore *saves = nullptr;

		// Helpers
		auto get_random(const Enums::System::Random random_type)
			-> unsigned int;
		auto get_string(std::string_view key) -> std::string;
		auto get_config(const unsigned int i) -> bool &;
		auto get_config(std::string_view section, std::string_view value) const
			-> std::string;
		auto get_directory(std::string_view key) const -> std::filesystem::path;
		auto get_file(std::string_view key) const -> std::filesystem::path;
		auto get_component(std::string_view combined_key) -> Component &;
		auto get_flag_ref(const std::string &flag) -> bool &;
		auto get_flag(const std::string &flag) -> bool;
		auto get_selected(const std::string &flag) const -> int;

		auto tick() -> void;
};

}