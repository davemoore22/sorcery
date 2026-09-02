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
#include <memory>
#include <optional>	   // for optional
#include <ostream>	   // for ostream
#include <string>	   // for basic_string, string
#include <string_view> // for string_view
#include <vector>	   // for vector

union SDL_Event; // Global FNamespace orward Declaration
namespace Sorcery { class Character; }
namespace Sorcery { class Game; }
namespace Sorcery { class UI; }
namespace Sorcery { struct Context; }
namespace Sorcery { struct MenuAction; }
namespace Sorcery { class ControllerMenuHandler; }
namespace Sorcery { class ControllerInputHandler; }
namespace Sorcery { class ControllerActionHandler; }

namespace Sorcery {

// UI Interaction Logic Controller
class Controller {

		friend class ControllerActionHandler;
		friend class ControllerMenuHandler;
		friend class ControllerInputHandler;

	public:
		// Standard Constructor
		Controller(Context &ctx);
		Controller() = delete;

		~Controller();

		Controller(const Controller &) = delete;
		auto operator=(const Controller &) -> Controller & = delete;

		Controller(Controller &&) = delete;
		auto operator=(Controller &&) -> Controller & = delete;

		// Serialisation
		template <class Archive> auto serialize(Archive &archive) -> void {
			archive(_selected, _busy, _last_screen, _last_event, _last_dir,
					_can_undo, _fullscreen, _candidate_party, _screen,
					_characters, _flags, _texts, _monochrome);
		}

		// Overloaded Operator
		auto friend operator<<(std::ostream &out_stream,
							   const Controller &controller) -> std::ostream &;

		// Public Methods
		auto has_saved_game() const -> bool;
		auto set_game(Game *game) -> void;
		auto clear_character(const Enums::CharacterSlot slot) -> void;
		auto has_character(const Enums::CharacterSlot slot) const -> bool;
		auto get_character(const Enums::CharacterSlot slot) const -> int;
		auto set_character(const Enums::CharacterSlot slot, const int value)
			-> void;
		auto set_selected(const std::string_view flag, int value) -> void;
		auto get_flag(const std::string_view flag) const -> bool;
		auto get_flag_ref(const std::string_view flag) -> bool &;
		auto set_flag(const std::string_view flag) -> void;
		auto set_flag_value(const std::string_view flag, const bool value)
			-> void;
		auto unset_flag(const std::string_view flag) -> void;
		auto has_flag(const std::string_view flag) const -> bool;
		auto toggle_flag(const std::string_view flag) -> void;
		auto get_flags() const -> std::string;
		auto get_characters() const -> std::string;
		auto has_text(const std::string_view flag) const -> bool;
		auto set_text(const std::string_view flag, const std::string &text)
			-> void;
		auto get_text(const std::string_view flag) const -> std::string;
		auto unset_text(const std::string_view flag) -> void;
		auto has_selected(const std::string_view flag) const -> bool;
		auto get_selected(const std::string_view flag) const -> int;
		auto unset_selected(const std::string_view flag) -> void;

		auto set_monochrome(const bool value) -> void;
		auto get_monochrome() const -> bool;
		auto set_fullscreen(const bool value) -> void;
		auto get_fullscreen() const -> bool;
		auto set_busy(const bool value) -> void;
		auto get_busy() const -> bool;
		auto set_can_undo(const bool value) -> void;
		auto get_can_undo() const -> bool;
		auto get_last_screen() const -> Enums::Screen;
		auto set_last_screen(const Enums::Screen value) -> void;
		auto get_last_event() const -> Enums::Map::Event;
		auto set_last_event(const Enums::Map::Event value) -> void;
		auto get_last_dir() const -> Enums::Map::Direction;
		auto set_last_dir(const Enums::Map::Direction value) -> void;
		auto add_to_candidate_party(unsigned int value) -> void;
		auto clear_candidate_party() -> void;
		auto get_candidate_party() -> std::vector<unsigned int> &;
		auto get_candidate_party() const -> const std::vector<unsigned int> &;
		auto get_candidate_character() const -> Character *;
		auto get_input_buffer() -> std::string &;
		auto set_input_buffer(const std::string &value) -> void;
		auto clear_input_buffer() -> void;

		auto initialise() -> void;
		auto go_to(const Enums::Screen screen) -> void;
		auto wants(const Enums::Screen value) const -> bool;
		auto is_at() const -> Enums::Screen;
		auto abort(const bool value = true) -> void;
		auto want_to_abort() const -> bool;
		auto clear_modal_flags() -> void;

		auto request_back() -> void;
		auto consume_back() -> bool;
		auto get_back() const -> bool;

		// Public Members
		std::unique_ptr<ControllerActionHandler> actions;
		std::unique_ptr<ControllerMenuHandler> menus;
		std::unique_ptr<ControllerInputHandler> input;

	private:
		// Private Members
		Context &_ctx;
		Enums::Screen _screen{};
		Enums::Screen _last_screen{};

		Game *_game{nullptr};
		bool _busy{};		// Currently busy (e.g. loading an asset etc)
		bool _has_save{};	// Is there a saved game present
		bool _monochrome{}; // Monochrome wireframe mode active
		bool _fullscreen{}; // Fullscreen mode active
		bool _can_undo{};	// Can "undo" a movement action
		bool _abort{};
		std::vector<unsigned int> _candidate_party; // Used for Reordering
		Enums::Map::Event _last_event;				// Last event in dungeon
		Enums::Map::Direction _last_dir;			// Last movement in dungeon
		std::map<Enums::CharacterSlot, int> _characters; // Character Selections
		std::map<std::string, bool, std::less<>> _flags; // Logic Flags
		std::map<std::string, std::string, std::less<>>
			_texts;										   // "Global" Texts
		std::map<std::string, int, std::less<>> _selected; // Menu Selections
		std::string _input_buffer; // Input Buffer for Text Input
		bool _go_back{};
};

};