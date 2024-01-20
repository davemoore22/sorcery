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
#include "common/macro.hpp"
#include "core/display.hpp"
#include "core/game.hpp"
#include "core/graphics.hpp"
#include "core/system.hpp"
#include "types/component.hpp"
#include "types/enum.hpp"

namespace Sorcery {

class Menu: public sf::Transformable, public sf::Drawable {

	public:

		// Constructors
		Menu(System *system, Display *display, Graphics *graphics, Game *game, const MenuType type,
			std::optional<MenuMode> mode = std::nullopt);
		Menu() = delete;

		// Overload operators
		auto operator[](const unsigned int index) -> MenuEntry &;

		// Public Members
		std::vector<MenuEntry> items;
		std::vector<sf::FloatRect> bounds;
		std::vector<MenuEntry>::const_iterator selected;
		unsigned int count;

		// Public Methods
		auto choose_next() -> std::optional<std::vector<MenuEntry>::const_iterator>;
		auto choose_previous() -> std::optional<std::vector<MenuEntry>::const_iterator>;
		auto choose_first() -> std::optional<std::vector<MenuEntry>::const_iterator>;
		auto choose_last() -> std::optional<std::vector<MenuEntry>::const_iterator>;
		auto choose(const unsigned int index) -> std::optional<std::vector<MenuEntry>::const_iterator>;
		auto choose(std::any option) -> std::optional<std::vector<MenuEntry>::const_iterator>;
		auto set_mouse_selected(sf::Vector2f mouse_pos) -> std::optional<std::vector<MenuEntry>::const_iterator>;
		auto get_mouse_clicked(
			const sf::Event::MouseButtonEvent mb_event) -> std::optional<std::vector<MenuEntry>::const_iterator>;
		auto get_type() const -> MenuType;
		auto generate(const Component &component, bool force_refresh = false) -> void;
		auto reload() -> void;
		auto get_by_index(unsigned int index) -> std::optional<std::vector<MenuEntry>::iterator>;
		auto num_enabled() -> unsigned int;
		auto num_disabled() -> unsigned int;
		auto enable_entry(const Component &component, unsigned int index) -> void;
		auto disable_entry(const Component &component, unsigned int index) -> void;
		auto get_width() const -> unsigned int;
		auto get_height() const -> unsigned int;
		auto get_size() const -> sf::Vector2f;
		auto set_visible_size(const unsigned int value) -> void;
		auto print() -> void;

	private:

		// Private Methods
		auto virtual draw(sf::RenderTarget &target, sf::RenderStates states) const -> void;
		auto _add_item(const int index, const MenuItemType itemtype, const MenuItem code, std::string key,
			const bool enabled, const ConfigOption option, const std::string &hint) -> void;
		auto _add_item(int index, const MenuItemType itemtype, const MenuItem code, std::string key) -> void;
		auto _add_item(
			int index, const MenuItemType itemtype, const MenuItem code, std::string key, unsigned int idx) -> void;
		auto _populate_chars() -> void;
		auto _select_first() -> std::optional<std::vector<MenuEntry>::const_iterator>;
		auto _select_last() -> std::optional<std::vector<MenuEntry>::const_iterator>;
		auto _add_all_items() -> void;
		auto _add_all_spells() -> void;
		auto _add_all_monsters() -> void;
		auto _choose_by_index(const unsigned int index) -> std::optional<std::vector<MenuEntry>::const_iterator>;

		// Private Members
		System *_system;
		Display *_display;
		Graphics *_graphics;
		Game *_game;
		unsigned int _width;
		unsigned int _height;
		MenuType _type;
		sf::RenderTexture _rtexture;
		sf::Texture _texture;
		std::vector<sf::Text> _texts;
		std::vector<sf::Text> _options;
		sf::RectangleShape _selected_bg;
		std::optional<MenuMode> _mode;
		bool _go_first;
		unsigned int _previous;
		unsigned int _visible_size;
		std::span<Sorcery::MenuEntry> _visible_items;
};

}
