// Copyright (C) 2021 Dave Moore
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

#include "charactersummary.hpp"
#include "display.hpp"
#include "frame.hpp"
#include "game.hpp"
#include "graphics.hpp"
#include "layout.hpp"
#include "main.hpp"
#include "system.hpp"

namespace Sorcery {

	class Display;

	class StatusBar : public sf::Transformable, public sf::Drawable {

	  public:
		// Constructors
		StatusBar(System *system, Display *display, Graphics *graphics, Game *game,
			std::optional<Component> layout = std::nullopt, std::optional<Component> frame = std::nullopt);
		StatusBar() = delete;

		// Public Members
		unsigned int width;
		unsigned int height;
		sf::Sprite sprite;
		std::optional<unsigned int> selected;
		std::vector<sf::FloatRect> bounds;

		// Public Methods
		auto refresh() -> void;
		auto set_mouse_selected(sf::Vector2f mouse_pos) -> std::optional<unsigned int>;
		auto set_selected_background() -> void;

	  private:
		// Private Methods
		auto virtual draw(sf::RenderTarget &target, sf::RenderStates states) const -> void;
		auto _generate() -> void;

		// Private Members
		System *_system;
		Display *_display;
		Graphics *_graphics;
		Game *_game;
		Component _layout;
		Component _frame_c;
		std::unique_ptr<Frame> _frame;
		sf::RenderTexture _rtexture;
		sf::Sprite _fsprite;
		sf::Texture _texture;
		std::map<std::string, sf::Text> _texts;
		std::vector<std::shared_ptr<CharacterSummary>> _summaries;
		sf::RectangleShape _selected_bg;
	};
} // namespace Sorcery
