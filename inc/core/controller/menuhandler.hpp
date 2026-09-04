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

#include "common/enum.hpp" // for Direction, Event
#include "core/enum.hpp"   // for CharacterSlot (ptr only), Screen
#include <cstddef>		   // for size_t
#include <functional>	   // for reference_wrapper
#include <map>			   // for map
#include <optional>		   // for optional
#include <ostream>		   // for ostream
#include <string>		   // for basic_string, string
#include <string_view>	   // for string_view
#include <vector>		   // for vector

union SDL_Event; // Global FNamespace orward Declaration
namespace Sorcery { class Controller; }
namespace Sorcery { class Character; }
namespace Sorcery { class Game; }
namespace Sorcery { class UI; }
namespace Sorcery { struct Context; }
namespace Sorcery { struct MenuAction; }
namespace Sorcery {
namespace Enums {
	namespace MenuAction { enum class Function; }
}
}

namespace Sorcery {

class ControllerMenuHandler {

	public:
		explicit ControllerMenuHandler(Controller &host, Context &ctx);
		ControllerMenuHandler() = delete;

		auto handle_standard(std::string_view component,
							 const std::vector<std::string> &items, int data,
							 int selection) -> void;

		auto handle_dynamic(std::string_view component,
							const std::vector<std::string> &items, int data,
							int selection) -> bool;

		auto handle_actions(std::string_view menu, int selection, int data)
			-> bool;

		auto item_disabled(std::string_view component, int selection, int data)
			-> bool;

	private:
		Controller &_host;
		Context &_ctx;

		auto _execute(const MenuAction &action, int data) -> void;
};

}