// Copyright (C) 2023 Dave Moore
//
// This file is part of Sorcery: Shadows under Llylgamyn.
//
// Sorcery: Shadows under Llylgamyn is free software: you can redistribute
// it and/or modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation, either version 2 of the License,
// or (at your option) any later version.
//
// Sorcery: Shadows under Llylgamyn is distributed in the hope that it wil
// be useful, but WITHOUT ANY WARRANTY; wiWthout even the implied warranty of
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

#include "core/display.hpp"
#include "core/graphics.hpp"
#include "core/system.hpp"
#include "gui/frame.hpp"
#include "gui/keyboard.hpp"
#include "gui/menu.hpp"
#include "gui/text.hpp"
#include "main.hpp"
#include "resources/componentstore.hpp"

namespace Sorcery {

class ChangeName {

	public:

		// Standard Constructor
		ChangeName(System *system, Display *display, Graphics *graphics, std::string old_name);
		ChangeName() = delete;

		// Standard Destructor
		~ChangeName();

		// Public Members

		// Public Methods
		auto start() -> std::optional<std::string>;
		auto stop() -> void;
		auto get_new_name() -> std::string;
		auto is_changed() -> bool;

	private:

		// Private Methods
		auto _draw() -> void;
		auto _handle_change_name(const sf::Event &event) -> std::optional<bool>;

		// Private Members
		System *_system;
		Display *_display;
		Graphics *_graphics;
		sf::RenderWindow *_window;
		sf::Sprite _bg;
		std::unique_ptr<Keyboard> _keyboard;
		std::string _old_name;
		std::string _new_name;
		std::unique_ptr<Text> _name_candidate;
		bool _valid;
};

}