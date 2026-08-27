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

#include "training/edit.hpp"
#include "common/macro.hpp"
#include "core/application.hpp"
#include "core/audioplayer.hpp"
#include "core/context.hpp"
#include "core/controller.hpp"
#include "core/define.hpp"
#include "core/enum.hpp"
#include "core/system.hpp"
#include "display/display.hpp"
#include "display/ui/ui.hpp"
#include "drawables/define.hpp"
#include "drawables/dialog.hpp"
#include "resources/define.hpp"
#include "training/enum.hpp"
#include "training/reclass.hpp"
#include "training/rename.hpp"
#include "training/rite.hpp"
#include "training/select.hpp"
#include "types/game.hpp"

Sorcery::Edit::Edit(Context &ctx)
	: Module{ctx} {

	_initialise();

	_select = std::make_unique<Select>(_ctx);
	_rename = std::make_unique<Rename>(_ctx);
	_reclass = std::make_unique<Reclass>(_ctx);
	_rite = std::make_unique<Rite>(_ctx);
};

Sorcery::Edit::~Edit() {}

auto Sorcery::Edit::_initialise() -> bool {

	return true;
}

auto Sorcery::Edit::start() -> int {

	_ctx.controller->go_to(Enums::Screen::EDIT);
	_ctx.controller->initialise();

	show_immediately();

	_ctx.audio->set_volume(1.0f);

	// Main loop
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
				return BACK_TO_TRAINING_GROUNDS;
		}

		_ctx.ui->display(Enums::Screen::EDIT, _ctx.game);

		_ctx.tick();

		if (!_ctx.controller->wants(Enums::Screen::EDIT) &&
			_ctx.controller->wants(Enums::Screen::TRAINING)) {

			return BACK_TO_TRAINING_GROUNDS;
		}

		if (_ctx.controller->wants(Enums::Screen::SELECT)) {

			const auto result{_select->start(Enums::Selection::Edit::RENAME)};

			if (result == ABORT_GAME)
				return ABORT_GAME;

			if (result == CHARACTER_SELECTED) {
				const auto rename_result{_rename->start()};

				if (rename_result == ABORT_GAME)
					return ABORT_GAME;

				_rename->stop();
			}

			_select->stop();

		} else if (_ctx.controller->wants(Enums::Screen::RETRAIN)) {

			const auto result{_select->start(Enums::Selection::Edit::RECLASS)};

			if (result == ABORT_GAME)
				return ABORT_GAME;

			if (result == CHARACTER_SELECTED) {
				const auto reclass_result{_reclass->start()};

				if (reclass_result == ABORT_GAME)
					return ABORT_GAME;

				_reclass->stop();
			}

			_select->stop();

		} else if (_ctx.controller->wants(Enums::Screen::LEGATE)) {

			const auto result{_select->start(Enums::Selection::Edit::LEGATE)};

			if (result == ABORT_GAME)
				return ABORT_GAME;

			if (result == CHARACTER_SELECTED) {
				const auto rite_result{_rite->start()};

				if (rite_result == ABORT_GAME)
					return ABORT_GAME;

				_rite->stop();
			}

			_select->stop();
		}
	}

	// Exit if we get to here having broken out of the loop
	return ABORT_GAME;
}

auto Sorcery::Edit::stop() -> int {

	_ctx.controller->go_to(Enums::Screen::TRAINING);

	return 0;
}