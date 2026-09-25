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

#include "drawables/drawable.hpp" // for DrawableResult, Drawable (ptr only)
#include "magic/malor.hpp"
#include <memory>	   // for unique_ptr
#include <optional>	   // for optional, nullopt, nullopt_t
#include <string>	   // for basic_string, string
#include <string_view> // for string_view
#include <vector>	   // for vector

namespace Sorcery { class Message; }
namespace Sorcery { class Dialog; }
namespace Sorcery { struct Context; }
namespace Sorcery { class Modal2; }
namespace Sorcery { class MalorDialog; }
namespace Sorcery {
namespace Magic { struct MalorDestination; }
}

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

struct PendingModal {

		std::string component;
		std::optional<std::string> menu_name{std::nullopt};
		std::optional<std::string> title_key{std::nullopt};
};

struct PendingDialog {
		std::string component;
		Enums::Layout::DialogType type;
		std::optional<std::string> text;
};

class PopupManager {

	public:
		explicit PopupManager(Context &ctx);
		~PopupManager();

		auto open_message(std::string_view component, std::vector<std::string> strings, Enums::Map::Event event_id)
			-> void;
		auto open_dialog(const std::string_view component, const Enums::Layout::DialogType type) -> void;
		auto open_dialog(std::string_view component, Enums::Layout::DialogType type, std::string text) -> void;
		auto open_modal(std::string_view component) -> void;
		auto open_modal(std::string_view component, std::string_view menu_name) -> void;
		auto open_modal(std::string_view component, std::string_view menu_name, std::string_view title_key) -> void;
		auto close() -> void;
		auto display() -> void;
		auto reset() -> void;
		[[nodiscard]] auto consume_completed(std::string_view name) -> bool;
		[[nodiscard]] auto consume_result(const std::string_view name) -> std::optional<DrawableResult>;
		[[nodiscard]] auto consume_accepted(std::string_view name) -> bool;
		[[nodiscard]] auto active() const -> bool;
		[[nodiscard]] auto is_active(std::string_view name) const -> bool;
		auto refresh_modal() -> void;
		auto open_malor(const std::string_view component) -> void;
		auto malor_destination() const -> Magic::MalorDestination;

	private:
		Context &_ctx;

		std::unique_ptr<Message> _message;
		std::unique_ptr<Dialog> _dialog;
		std::unique_ptr<Modal2> _modal2;
		Drawable *_active{};
		std::optional<PopupCompletion> _completed;
		bool _displaying{};
		std::optional<PendingModal> _pending_modal;
		std::optional<PendingDialog> _pending_dialog;
		std::unique_ptr<MalorDialog> _malor_dialog;
		std::optional<std::string> _pending_malor;
		bool _input_mode_pushed{false};
		bool _refresh_modal{false};

		auto _open_modal(const std::string_view component) -> void;
		auto _open_modal(std::string_view component, std::string_view menu_name) -> void;
		auto _open_modal(std::string_view component, std::string_view menu_name, std::string_view title_key) -> void;
		auto _open_dialog(const std::string_view component, const Enums::Layout::DialogType type,
						  std::optional<std::string> text) -> void;
		auto _pop_input_mode() -> void;
		auto _open_malor(const std::string_view component) -> void;
};

}