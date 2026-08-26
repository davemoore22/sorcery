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

#include "training/select.hpp"
#include "common/macro.hpp"
#include "core/application.hpp"
#include "core/audioplayer.hpp"
#include "core/context.hpp"
#include "core/controller.hpp"
#include "core/define.hpp"
#include "core/enum.hpp"
#include "core/system.hpp"
#include "display/display.hpp"
#include "display/ui.hpp"
#include "drawables/define.hpp"
#include "drawables/dialog.hpp"
#include "resources/define.hpp"
#include "types/game.hpp"

Sorcery::Select::Select(Context &ctx)
	: Module{ctx} {

	_initialise();
};

Sorcery::Select::~Select() {}

auto Sorcery::Select::_initialise() -> bool {

	return true;
}

auto Sorcery::Select::_screen(Enums::Selection::Edit mode) const
	-> Enums::Screen {

	using enum Enums::Selection::Edit;

	switch (mode) {
	case RENAME:
		return Enums::Screen::SELECT;
	case RECLASS:
		return Enums::Screen::RETRAIN;
	case LEGATE:
		return Enums::Screen::LEGATE;
	}

	return Enums::Screen::EDIT;
}

auto Sorcery::Select::start(const Enums::Selection::Edit mode) -> int {

	const auto screen{_screen(mode)};

	_ctx.controller->go_to(screen);
	_ctx.controller->initialise();

	show_immediately();

	_ctx.audio->set_volume(1.0f);

	_ctx.controller->clear_character(Enums::CharacterSlot::EDIT);

	auto done{false};
	while (!done) {

		SDL_Event event{};

		while (SDL_PollEvent(&event)) {

			switch (process_event(event)) {

			case ModuleEvent::ABORT:
				done = true;
				break;

			case ModuleEvent::QUICKLOAD:
				continue;

			case ModuleEvent::NONE:
				break;
			}

			if (_ctx.controller->check_for_back(event))
				return BACK_TO_EDIT;
		}

		_ctx.ui->display(screen, _ctx.game);

		_ctx.tick();

		// Character selected.
		if (_ctx.controller->has_character(Enums::CharacterSlot::EDIT))
			return CHARACTER_SELECTED;

		// Return menu item selected.
		if (!_ctx.controller->wants(screen) &&
			_ctx.controller->wants(Enums::Screen::EDIT))
			return BACK_TO_EDIT;
	}

	return ABORT_GAME;
}

auto Sorcery::Select::stop() -> int {

	return 0;
}