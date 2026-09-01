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
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace Sorcery { class Message; }
namespace Sorcery { class Dialog; }
namespace Sorcery { struct Context; }

namespace Sorcery {
namespace Enums {
	namespace Map { enum class Event; }
	namespace Layout { enum class DialogType; }
}
}

namespace Sorcery {

struct PopupCompletion {
		std::string name;
		DrawableResult result{DrawableResult::NONE};
};

class PopupManager {

	public:
		explicit PopupManager(Context &ctx);
		~PopupManager();

		auto open_message(std::string_view component,
						  std::vector<std::string> strings,
						  Enums::Map::Event event_id) -> void;
		auto open_dialog(const std::string_view component,
						 const Enums::Layout::DialogType type) -> void;

		auto close() -> void;
		auto display() -> void;
		[[nodiscard]] auto consume_completed(std::string_view name) -> bool;
		[[nodiscard]] auto consume_result(const std::string_view name)
			-> std::optional<DrawableResult>;
		[[nodiscard]] auto consume_accepted(std::string_view name) -> bool;
		[[nodiscard]] auto active() const -> bool;
		[[nodiscard]] auto is_active(std::string_view name) const -> bool;

	private:
		Context &_ctx;

		std::unique_ptr<Message> _message;
		std::unique_ptr<Dialog> _dialog;
		Drawable *_active{};
		std::optional<PopupCompletion> _completed;
};

}