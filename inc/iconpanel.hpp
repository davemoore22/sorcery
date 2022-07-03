// Copyright (C) 2021 Dave Moore
//
// This file is part of Sorcery: Shadows under Llylgamyn.
//
// Sorcery: Shadows under Llylgamyn is free software: you can redistribute it
// and/or modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation, either version 2 of the License,
// or (at your option) any later version.
//
// Sorcery: Shadows under Llylgamyn is distributed in the hope that it will
// be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Sorcery: Shadows under Llylgamyn.  If not, see
// <http://www.gnu.org/licenses/>.
//
// If you modify this Program, or any covered work, by linking or combining it
// with the libraries referred to in README (or a modified version of said
// libraries), containing parts covered by the terms of said libraries, the
// licensors of this Program grant you additional permission to convey the
// resulting work.

#include "display.hpp"
#include "game.hpp"
#include "graphics.hpp"
#include "main.hpp"
#include "system.hpp"

namespace Sorcery {

	class IconPanel : public sf::Transformable, public sf::Drawable {

	  public:
		// Constructors
		IconPanel(System *system, Display *display, Graphics *graphics, Game *game, Component layout, bool is_left);
		IconPanel() = delete;

		// Public Members
		std::optional<std::string> selected;

		// Public Methods
		auto refresh(bool in_camp) -> void;
		auto set_mouse_selected(Component &component, sf::Vector2f mouse_pos) -> std::optional<std::string>;
		auto set_selected_background() -> void;
		auto is_mouse_over(Component &component, sf::Vector2f mouse_pos) const -> bool;

	  private:
		// Private Methods
		auto virtual draw(sf::RenderTarget &target, sf::RenderStates states) const -> void;
		auto _set_icon(sf::Sprite &sprite, Component layout, int offset_x, int offset_y) -> void;
		auto _add_icon(std::string_view icon_key, std::string_view component_key) -> void;

		// Private Members
		System *_system;
		Display *_display;
		Graphics *_graphics;
		Game *_game;
		Component _layout;
		unsigned int _width;
		unsigned int _height;
		std::unique_ptr<Frame> _frame;
		std::vector<sf::Text> _texts;
		std::vector<sf::Sprite> _sprites;
		IconStorage _icons;
		bool _is_left;
		sf::RectangleShape _selected_bg;
	};
} // namespace Sorcery
