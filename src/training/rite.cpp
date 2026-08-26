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

#include "training/rite.hpp"
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
#include "modules/inspect.hpp"
#include "resources/define.hpp"
#include "types/game.hpp"

Sorcery::Rite::Rite(Context &ctx)
	: Module{ctx} {

	_initialise();

	_inspect = std::make_unique<Inspect>(_ctx);
}

Sorcery::Rite::~Rite() {}

auto Sorcery::Rite::_initialise() -> bool {

	return true;
}

auto Sorcery::Rite::start() -> int {

	_ctx.controller->go_to(Enums::Screen::RITE);

	show_immediately();

	_ctx.controller->unset_flag("want_rite_ok");
	_ctx.ui->dialog_rite->show = true;

	_ctx.audio->set_volume(1.0f);
	_stage = 0;

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

			if (_ctx.controller->check_for_back(event)) {
				_ctx.ui->dialog_rite->show = false;
				_ctx.controller->unset_flag("want_rite_ok");
				return BACK_TO_EDIT;
			}
		}
		const auto stage{_stage.load()};
		_ctx.ui->display(Enums::Screen::RITE, stage);

		_ctx.tick();

		// Yes
		if (_ctx.controller->has_flag("want_rite_ok")) {
			_ctx.controller->unset_flag("want_rite_ok");
			break;
		}

		// No
		if (!_ctx.ui->dialog_rite->show)
			return BACK_TO_EDIT;
	}

	// Rite proper starts here.
	_stage = 1;
	_stage_visible = true;
	_rite_ready = false;
	_rite_tick = 0;

	_rite_tick = SDL_AddTimer(2000, &Rite::_callback_rite_tick, this);

	while (true) {

		SDL_Event event{};

		while (SDL_PollEvent(&event)) {

			switch (process_event(
				event,
				{.menu_key = true, .quicksave = false, .quickload = false})) {

			case ModuleEvent::ABORT:
				return ABORT_GAME;

			case ModuleEvent::QUICKLOAD:
				continue;

			case ModuleEvent::NONE:
				break;
			}

			if (_ctx.controller->check_for_back(event))
				return BACK_TO_EDIT;
		}

		const auto stage{_stage_visible.load() ? _stage.load() : 0};

		_ctx.ui->display(Enums::Screen::RITE, stage);

		_ctx.tick();

		// Ceremony complete, perform the actual rite on the selected character
		if (_rite_ready.load()) {

			_rite_tick = 0;

			auto &character{_ctx.game->characters.at(
				_ctx.controller->get_character(Enums::CharacterSlot::EDIT))};

			const auto alignment{character.get_alignment()};

			character.create().legate(alignment);

			_ctx.game->save_game();

			const auto result{_inspect->start(
				INSPECT_MODE_BASE,
				_ctx.controller->get_character(Enums::CharacterSlot::EDIT))};
			if (result == ABORT_GAME)
				return ABORT_GAME;
			_inspect->stop(INSPECT_MODE_BASE);

			return BACK_TO_EDIT;
		}
	}

	return ABORT_GAME;
}

auto Sorcery::Rite::stop() -> int {

	if (_rite_tick != 0) {
		SDL_RemoveTimer(_rite_tick);
		_rite_tick = 0;
	}

	_ctx.ui->dialog_rite->show = false;

	_ctx.controller->unset_flag("want_rite_ok");

	_ctx.controller->go_to(Enums::Screen::EDIT);
	;

	return 0;
}

auto Sorcery::Rite::_callback_rite_tick(std::uint32_t, void *param)
	-> std::uint32_t {

	auto *rite{static_cast<Rite *>(param)};

	// Current message has finished its two-second display.
	if (rite->_stage_visible.load()) {

		// Stage 5 is the final message. No trailing blank period;
		// the Rite can now be applied.
		if (rite->_stage.load() == 5) {

			rite->_rite_ready = true;
			return 0;
		}

		// Half-second blank interval before the next stage.
		rite->_stage_visible = false;

		return 500;
	}

	// Blank interval has finished: advance to the next message.
	++rite->_stage;
	rite->_stage_visible = true;

	return 2000;
}