// Copyright (C) 2024 Dave Moore
//
// This file is part of Sorcery: Shadows under Llylgamyn.
//
// Sorcery: Shadows under Llylgamyn is free software: you can redistribute
// it and/or modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation, either version 2 of the License,
// or (at your option) any later version.
//
// Sorcery: Shadows under Llylgamyn is distributed in the hope that it wil
// be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
// Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Sorcery: Shadows under Llylgamyn.  If not,
// see <http://www.gnu.org/licenses/>.
//
// If you modify this program, or any covered work, by linking or combining
// it with the libraries referred to in README (or a modified version of
// said libraries), containing parts covered by the terms of said libraries,
// the licensors of this program grant you additional permission to convey
// the resulting work.

#pragma once

#include "common/include.hpp"
#include "common/type.hpp"
#include "gui/allocatepanel.hpp"
#include "gui/characterdisplay.hpp"
#include "gui/dialog.hpp"
#include "gui/frame.hpp"
#include "gui/infopanel.hpp"
#include "gui/keyboard.hpp"
#include "gui/menu.hpp"
#include "types/character.hpp"

namespace Sorcery {

// Forward Declarations
class Display;
class Factory;
class Game;
class Graphics;
class System;

class Create {

	public:

		// Constructor
		Create(
			System *system, Display *display, Graphics *graphics, Game *game);
		Create() = delete;

		// Public Members

		// Public Methods
		auto start() -> std::optional<MIM>;
		auto stop() -> void;

	private:

		// Private Methods
		auto _draw() -> void;
		auto _do_event_loop() -> std::optional<MDR>;
		auto _get_character_portrait(const unsigned int index)
			-> std::optional<sf::Sprite>;
		auto _go_to_next_stage() -> void;
		auto _go_to_previous_stage() -> void;
		auto _handle_allocate_attributes(const sf::Event &event)
			-> std::optional<MDR>;
		auto _handle_choose_create_method(const sf::Event &event)
			-> std::optional<MDR>;
		auto _handle_choose_name(const sf::Event &event) -> std::optional<MDR>;
		auto _handle_choose_race(const sf::Event &event) -> std::optional<MDR>;
		auto _handle_choose_alignment(const sf::Event &event)
			-> std::optional<MDR>;
		auto _handle_choose_class(const sf::Event &event) -> std::optional<MDR>;
		auto _handle_choose_potraits(const sf::Event &event)
			-> std::optional<MDR>;
		auto _handle_input(const sf::Event &event) -> std::optional<MDR>;
		auto _handle_review_and_confirm(const sf::Event &event)
			-> std::optional<MDR>;
		auto _set_classes_menu() -> void;
		auto _set_info_panel_contents(
			std::vector<Sorcery::MenuEntry>::const_iterator it) -> void;
		auto _update_character(const sf::Event &event) -> std::optional<MDR>;
		auto _generate_display() -> void;
		auto _reset_components() -> void;
		auto _initalise_components() -> void;
		auto _place_components() -> void;
		auto _initialise_state() -> void;
		auto _refresh_display() -> void;
		auto _add_starting_equipment() -> void;

		// Private Members
		System *_system;
		Display *_display;
		Graphics *_graphics;
		Game *_game;
		sf::RenderWindow *_window;
		sf::Sprite _bg;
		sf::Texture *_potrait_texture;
		Character _candidate;
		std::unique_ptr<Keyboard> _keyboard;
		std::unique_ptr<Menu> _method_menu;
		std::unique_ptr<Menu> _race_menu;
		std::unique_ptr<Menu> _alignment_menu;
		std::unique_ptr<Menu> _attribute_menu;
		std::unique_ptr<Menu> _class_menu;
		std::unique_ptr<Menu> _final_menu;
		std::unique_ptr<InfoPanel> _ip;
		std::unique_ptr<AllocatePanel> _ap;
		std::vector<Character> _stages;
		CRM _method;
		std::map<std::string, sf::Sprite> _sprites;
		std::map<std::string, sf::Text> _texts;
		std::map<std::string, std::shared_ptr<Frame>> _frames;
		std::unique_ptr<Frame> _final_menu_frame;
		bool _show_final_menu;
		bool _show_saved_ok;
		std::unique_ptr<Dialog> _dialog_saved_ok;
		std::unique_ptr<CharacterDisplay> _character_display;
		std::unique_ptr<Factory> _factory;
};
} // namespace Sorcery
