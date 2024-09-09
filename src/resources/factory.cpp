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

#include "resources/factory.hpp"
#include "common/define.hpp"
#include "core/display.hpp"
#include "core/game.hpp"
#include "core/graphics.hpp"
#include "core/system.hpp"
#include "gui/dialog.hpp"
#include "resources/define.hpp"

Sorcery::Factory::Factory(System *system, Display *display, Graphics *graphics, Game *game)
	: _system{system}, _display{display}, _graphics{graphics}, _game{game} {
}

auto Sorcery::Factory::make_dialog(const std::string &component, const WindowDialogType type)
	-> std::unique_ptr<Dialog> {

	auto component_text{component};
	component_text.append("_text");

	auto dialog = std::make_unique<Dialog>(
		_system, _display, _graphics, (*_display->layout)[component], (*_display->layout)[component_text], type);
	dialog->setPosition(_display->get_centre_pos(dialog->get_size()));

	return dialog;
}

auto Sorcery::Factory::make_dialog(
	const std::string &component, const WindowDialogType type, const unsigned int duration) -> std::unique_ptr<Dialog> {

	auto dialog = make_dialog(component, type);
	dialog->set_duration(duration);

	return dialog;
}
