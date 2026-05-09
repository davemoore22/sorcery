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

#include <functional>
#include <map>
#include <memory>
#include <ostream>
#include <string>
#include <string_view>
#include <vector>

#include "common/enum.hpp"
#include "core/enum.hpp"

union SDL_Event;

namespace Sorcery {

// Forward Declarations
struct Context;
class Game;
class UI;
class Character;
struct MenuAction;

// UI Interaction Logic Controller
class Controller {

	public:
		// Standard Constructor
		Controller(Context &ctx);
		Controller() = default;

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
		auto check_for_abort(const SDL_Event event) -> bool;
		auto check_for_back(const SDL_Event event) -> bool;
		auto check_for_back(const SDL_Event event, bool &flag) -> void;
		auto check_for_debug(const SDL_Event event) -> void;
		auto check_for_movement(const SDL_Event event) -> int;
		auto check_for_quickload(const SDL_Event event) -> bool;
		auto check_for_quicksave(const SDL_Event event) -> bool;
		auto check_for_resize(const SDL_Event event, UI *ui) -> void;
		auto check_for_ui_toggle(const SDL_Event event) -> void;
		auto handle_button_click(const std::string &component, UI *ui,
								 const int data) -> void;
		auto handle_input_button_click(const std::string &component, UI *ui,
									   std::string *data) -> void;
		auto handle_menu(const std::string &component,
						 const std::vector<std::string> &items, const int data,
						 const int selection) -> void;
		auto handle_menu_with_flags(
			const std::string &component, const std::vector<std::string> &items,
			const int data, const int selection,
			std::vector<std::reference_wrapper<bool>> in_flags) -> void;
		auto handle_stepper_button_click(const std::string &component, UI *ui,
										 const bool positive, int *data)
			-> void;
		auto handle_toggle(const std::string &component, const std::string &tab,
						   const int selection) -> void;
		auto has_saved_game() const -> bool;
		auto is_menu_item_disabled(const std::string &component,
								   const int selection, const int data) -> bool;
		auto set_game(Game *game) -> void;
		auto clear_character(const std::string &flag) -> void;
		auto get_character(const std::string &flag) const -> int;
		auto has_character(const std::string &flag) const -> bool;
		auto set_character(const std::string &flag, const int value) -> void;
		auto get_flag(const std::string &flag) const -> bool;
		auto get_flag_ref(const std::string &flag) -> bool &;
		auto set_flag(const std::string &flag) -> void;
		auto set_flag_value(const std::string &flag, const bool value) -> void;
		auto unset_flag(const std::string &flag) -> void;
		auto has_flag(const std::string &flag) const -> bool;
		auto toggle_flag(const std::string &flag) -> void;
		auto get_flags() const -> std::string;
		auto get_characters() const -> std::string;
		auto has_text(const std::string &flag) const -> bool;
		auto set_text(const std::string &flag, const std::string &text) -> void;
		auto get_text(const std::string &flag) const -> const std::string;
		auto unset_text(const std::string &flag) -> void;
		auto has_selected(const std::string &flag) const -> bool;
		auto set_selected(const std::string &flag, const int value) -> void;
		auto get_selected(const std::string &flag) const -> int;
		auto unset_selected(const std::string &flag) -> void;

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
		auto get_character() const -> Character *;
		auto get_input_buffer() -> std::string &;
		auto set_input_buffer(const std::string &value) -> void;

		auto initialise() -> void;
		auto go_to(const Enums::Screen screen) -> void;
		auto wants(const Enums::Screen value) const -> bool;
		auto is_at() const -> Enums::Screen;
		auto abort(const bool value = true) -> void;
		auto leave_game(const bool value = true) -> void;
		auto want_to_abort() const -> bool;
		auto want_to_leave_game() -> bool &;

		auto handle_menu(std::string_view menu, int selection, int data,
						 std::vector<std::reference_wrapper<bool>> &ui_flags)
			-> bool;
		auto execute_action(const MenuAction &action, int data,
							std::vector<std::reference_wrapper<bool>> &ui_flags)
			-> void;

		// Public Members

	private:
		// Private Members
		Context &_ctx;
		Enums::Screen _screen;
		Enums::Screen _last_screen;

		Game *_game;
		bool _busy;		  // Currently busy (e.g. loading an asset etc)
		bool _has_save;	  // Is there a saved game present
		bool _monochrome; // Monochrome wireframe mode active
		bool _fullscreen; // Fullscreen mode active
		bool _can_undo;	  // Can "undo" a movement action
		bool _abort;
		bool _leave;
		std::vector<unsigned int> _candidate_party; // Used for Reordering
		Enums::Map::Event _last_event;				// Last event in dungeon
		Enums::Map::Direction _last_dir;			// Last movement in dungeon
		std::map<std::string, int> _characters;		// Character Selections
		std::map<std::string, bool> _flags;			// Logic Flags
		std::map<std::string, std::string> _texts;	// "Global" Texts
		std::map<std::string, int> _selected;		// Menu Selections
		std::string _input_buffer; // Input Buffer for Text Input
};

};