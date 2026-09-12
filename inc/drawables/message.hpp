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

#include "drawables/drawable.hpp"

#include <string>
#include <vector>

namespace Sorcery {
namespace Enums {
	namespace Map { enum class Event; }
}
}

namespace Sorcery {

class Message final : public Drawable {

	public:
		explicit Message(Context &ctx);

		auto build(Component &component) -> void override;
		auto display() -> void override;
		auto set(std::vector<std::string> strings, Enums::Map::Event event_id) -> void;
		[[nodiscard]] auto event_id() const -> Enums::Map::Event;

	private:
		std::vector<std::string> _strings;
		Enums::Map::Event _event_id;
};

}