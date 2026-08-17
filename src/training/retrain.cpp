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

#include "training/retrain.hpp"
#include "common/macro.hpp"
#include "core/application.hpp"
#include "core/audioplayer.hpp"
#include "core/context.hpp"
#include "core/controller.hpp"
#include "core/define.hpp"
#include "core/display.hpp"
#include "core/enum.hpp"
#include "core/system.hpp"
#include "core/ui.hpp"
#include "gui/define.hpp"
#include "gui/dialog.hpp"
#include "resources/define.hpp"
// #include "training/rename.hpp"
#include "types/game.hpp"

Sorcery::Retrain::Retrain(Context &ctx)
	: Module{ctx} {

	_initialise();

	//_rename = std::make_unique<Rename>(_ctx);
};

Sorcery::Retrain::~Retrain() {}

auto Sorcery::Retrain::_initialise() -> bool {

	return true;
}

auto Sorcery::Retrain::start(const int mode) -> int {

	_ctx.controller->go_to(Enums::Screen::RETRAIN);
	_ctx.controller->initialise();

	show_immediately();

	_ctx.audio->set_volume(1.0f);
	_ctx.controller->clear_character(Enums::CharacterSlot::EDIT);

	// Main loop
	auto done{false};
	while (!done) {

		SDL_Event event;
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

		_ctx.ui->display(Enums::Screen::RETRAIN, _ctx.game);
		_ctx.tick();

		if (!_ctx.controller->wants(Enums::Screen::RETRAIN) &&
			_ctx.controller->wants(Enums::Screen::EDIT)) {
			_ctx.game->save_game();
			_ctx.controller->clear_character(Enums::CharacterSlot::EDIT);
			return BACK_TO_TRAINING_GROUNDS;
		} else if (_ctx.controller->has_character(Enums::CharacterSlot::EDIT)) {
			if (mode == EDIT_MODE_RECLASS) {

				// const auto result{_rename->start()};
				// if (result == ABORT_GAME)
				//	return ABORT_GAME;
				//_rename->stop();
				//_ctx.controller->go_to(Enums::Screen::EDIT);
				//} else if (mode == EDIT_MODE_RECLASS) {
				//
				//} else if (mode == EDIT_MODE_LEGATE) {
				//}
			}
		}
	}

	// Exit if we get to here having broken out of the loop
	return ABORT_GAME;
}

auto Sorcery::Retrain::stop() -> int {

	//_ctx.controller->go_to(Enums::Screen::TRAINING);

	return 0;
}