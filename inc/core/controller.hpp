// Copyright (C) 2025 Dave Moore
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

#include "common/include.hpp"

namespace Sorcery {

// Forward Declarations
class System;
class Display;
class Game;
class Resources;
class UI;

// UI Interaction Logic Controller
class Controller {

	public:
		// Standard Constructor
		Controller(System *system, Display *display, Resources *resources);

		template <class Archive> auto serialize(Archive &archive) -> void {
			archive(selected, busy, last, last_event, last_dir, can_undo,
					show_party_panel, show_ui, show_automap, monochrome,
					fullscreen, candidate_party, _screen, _characters, _flags,
					_texts);
		}

		// Public Methods
		auto check_for_abort(const SDL_Event event) -> bool;
		auto check_for_back(const SDL_Event event) -> bool;
		auto check_for_back(const SDL_Event event, bool &flag) -> void;
		auto check_for_debug(const SDL_Event event) -> void;
		auto check_for_movement(const SDL_Event event) -> int;
		auto check_for_resize(const SDL_Event event, UI *ui) -> void;
		auto check_for_ui_toggle(const SDL_Event event) -> void;
		auto handle_button_click(const std::string &component, UI *ui,
								 const int data) -> void;
		auto handle_menu(const std::string &component,
						 const std::vector<std::string> &items, const int data,
						 const int selection) -> void;
		auto handle_menu_with_flags(
			const std::string &component, const std::vector<std::string> &items,
			const int data, const int selection,
			std::vector<std::reference_wrapper<bool>> in_flags) -> void;
		auto handle_toggle(const std::string &component, const std::string &tab,
						   const int selection) -> void;
		auto has_saved_game() const -> bool;
		auto initialise(std::string_view value) -> void;
		auto is_menu_item_disabled(const std::string &component,
								   const int selection, const int data) -> bool;
		auto set_screen(std::string_view value) -> void;
		auto set_game(Game *game) -> void;
		auto get_screen() const -> std::string_view;
		auto move_screen(const std::string &from_screen,
						 const std::string &to_screen) -> void;
		auto clear_character(const std::string &flag) -> void;
		auto get_character(const std::string &flag) const -> int;
		auto has_character(const std::string &flag) const -> bool;
		auto set_character(const std::string &flag, const int value) -> void;
		auto get_flag(const std::string &flag) const -> bool;
		auto get_flag_ref(const std::string &flag) -> bool &;
		auto set_flag(const std::string &flag) -> void;
		auto unset_flag(const std::string &flag) -> void;
		auto has_flag(const std::string &flag) const -> bool;
		auto get_flags() const -> std::string;
		auto get_characters() const -> std::string;
		auto has_text(const std::string &flag) const -> bool;
		auto set_text(const std::string &flag, const std::string &text) -> void;
		auto get_text(const std::string &flag) const -> const std::string;
		auto unset_text(const std::string &flag) -> void;
		auto has_selected(const std::string &flag) const -> bool;
		auto set_selected(const std::string &flag, const int &value) -> void;
		auto get_selected(const std::string &flag) const -> int;
		auto unset_selected(const std::string &flag) -> void;

		// Public Members
		std::map<std::string, int> selected; // Menu Selections
		bool busy; // Currently busy (e.g. loading an asset or saving a game)
		std::string last; // Last screen redrawn by the ui

		// In-Dungeon Flags
		Enums::Map::Event last_event;	// Last event in dungeon
		Enums::Map::Direction last_dir; // Last movement in dungeon
		bool can_undo;					// Can "undo" a movement action
		bool show_party_panel;			// Show party panel
		bool show_ui;					// Show UI
		bool show_automap;				// Show automap
		bool monochrome;				// Draw in monochrome;
		bool fullscreen;				// Fullscreen mode

		// Front-end Flags
		std::vector<unsigned int> candidate_party; // Used for Reordering

	private:
		// Private Members
		System *_system;
		Display *_display;
		Resources *_resources;
		Game *_game;
		bool _has_save;
		std::string _screen;					   // Where we currently are
		std::map<std::string, int> _characters;	   // Character Selections
		std::map<std::string, bool> _flags;		   // Logic Flags
		std::map<std::string, std::string> _texts; // "Global" Texts
};

};