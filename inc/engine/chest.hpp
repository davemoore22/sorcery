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

#include "core/module.hpp" // for Module
#include "engine/enum.hpp" // for State, Type, Result
#include <array>		   // for array
#include <optional>		   // for optional
#include <string>		   // for string, basic_string
#include <string_view>	   // for string_view
namespace Sorcery {
struct Context;
} // lines 38-38

namespace Sorcery {

class Chest final : public Module {

		struct StateData {
				Enums::Chests::State state{Enums::Chests::State::MENU};
				Enums::Chests::State after_result{Enums::Chests::State::MENU};

				Enums::Traps::Type actual_trap{
					Enums::Traps::Type::POISON_NEEDLE};
				std::optional<Enums::Traps::Type> selected_trap;
				std::optional<int> actor;

				std::array<bool, 6> inspected{};

				std::string result;
		};

	public:
		// Standard Constructor
		Chest(Context &ctx);

		// Standard Destructor
		~Chest();

		// Public Members

		// Public Methods
		auto start(void) -> Enums::Chests::Result;
		auto stop(void) -> void;
		[[nodiscard]] auto has_inspected(int character_id) const -> bool;

	private:
		// Private Methods
		auto _initialise() -> bool;
		auto _open(int character_id) -> void;
		auto _inspect(int character_id) -> void;
		auto _cast_calfo(int character_id) -> void;
		auto _disarm(int character_id, Enums::Traps::Type trap) -> void;
		auto _trigger_trap(int character_id) -> void;
		auto _show_character_modal(const std::string_view menu_name) -> void;
		auto _process_menu_action() -> void;
		auto _process_character_action() -> void;
		auto _process_trap_action() -> void;
		auto _trap_name(const Enums::Traps::Type trap) const -> std::string;
		[[nodiscard]] auto _random_trap() const -> Enums::Traps::Type;
		auto _show_trap_modal() -> void;

		// Private Members
		StateData _state;
};

}