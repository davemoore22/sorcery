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

#include "console.hpp"

Sorcery::Console::Console(tgui::Gui *gui, System *system, Display *display, Graphics *graphics, Game *game)
	: _gui{gui}, _system{system}, _display{display}, _graphics{graphics}, _game{game} {}

auto Sorcery::Console::refresh() -> void {

	static int a;
	a++;

	_gui->removeAllWidgets();

	auto background{tgui::Panel::create({"1024", "768"})};
	background->getRenderer()->setBackgroundColor({0, 0, 0, 175});
	_gui->add(background, "TransparentBackground");

	auto window{tgui::ChildWindow::create()};
	window->setTitle("Console");
	window->setTitleAlignment(tgui::ChildWindow::TitleAlignment::Center);
	window->setTitleButtons(0);
	window->setPosition({"0%", "0%"});
	window->setSize(1024, 768);
	background->add(window, "Window");

	auto body_panel{tgui::Panel::create()};
	body_panel->setPosition(16, 16);
	body_panel->setSize(1024 - 32, 786 - 64);
	window->add(body_panel, "BodyPanel");

	auto logs{tgui::TextArea::create()};
	logs->setPosition(32, 32);
	logs->setSize(1024 - 64, 786 - 64);
	logs->setTextSize(16);
	logs->setEnabled(true);
	logs->setReadOnly(true);

	for (const auto &log_item : _game->console_log)
		logs->addText(fmt::format("#{:06} {} {}\n", log_item.id, TP2STR(log_item.datetime), log_item.text));
	body_panel->add(logs, "InfoEdit");
}
