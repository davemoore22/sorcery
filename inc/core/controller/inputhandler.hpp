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

class ControllerInputHandler {

	public:
		explicit ControllerInputHandler(Controller &host, Context &_ctx);

		ControllerInputHandler() = delete;

		auto abort(const SDL_Event &event) -> bool;
		auto back(const SDL_Event &event) const -> bool;
		auto back(const SDL_Event &event, bool &flag) const -> void;

		auto debug(const SDL_Event &event) -> void;
		auto automap(const SDL_Event &event) const -> bool;
		auto movement(const SDL_Event &event) const -> int;

		auto quickload(const SDL_Event &event) const -> bool;
		auto quicksave(const SDL_Event &event) const -> bool;
		auto quick_inspect(const SDL_Event &event) const -> int;

		auto resize(const SDL_Event &event) -> void;
		auto ui_toggle(const SDL_Event &event) -> void;

		auto menu_key(const SDL_Event &event) -> void;
		auto consume_menu_key(std::size_t item_count)
			-> std::optional<std::size_t>;

	private:
		Controller &_host;
		Context &_ctx;

		std::optional<int> _menu_key;
};

}
