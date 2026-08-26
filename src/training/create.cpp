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

#include "training/create.hpp"
#include "common/macro.hpp"
#include "core/application.hpp"
#include "core/context.hpp"
#include "core/controller.hpp"
#include "core/define.hpp"
#include "core/enum.hpp"
#include "core/system.hpp"
#include "display/display.hpp"
#include "display/ui.hpp"
#include "gui/define.hpp"
#include "gui/dialog.hpp"
#include "gui/input.hpp"
#include "resources/define.hpp"
#include "types/character/character.hpp"
#include "types/game.hpp"

Sorcery::Create::Create(Context &ctx)
	: Module{ctx} {

	_initialise();
};

Sorcery::Create::~Create() {};

auto Sorcery::Create::_initialise() -> bool {

	return true;
}

auto Sorcery::Create::start() -> int {

	_ctx.controller->go_to(Enums::Screen::CREATE);
	_ctx.controller->initialise();

	show_immediately();

	_ctx.game->creation_candidate = std::make_shared<Character>(&_ctx);
	_ctx.game->creation_candidate->create().reset(
		Enums::Character::Stage::ENTER_NAME);

	auto candidate{_ctx.game->creation_candidate};

	_ctx.ui->first_frame = true;
	_ctx.controller->clear_input_buffer();

	// Main loop
	auto done{false};
	while (!done) {

		SDL_Event event;
		while (SDL_PollEvent(&event)) {

			switch (process_event(
				event,
				{.menu_key = true, .quicksave = false, .quickload = false})) {

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

		using enum Enums::Character::Stage;
		switch (candidate->create().get_stage()) {
		case ENTER_NAME:
			_ctx.ui->display(
				Enums::Screen::CREATE_NAME,
				std::to_underlying(candidate->create().get_stage()));

			if (candidate->create().get_stage() !=
				Enums::Character::Stage::ENTER_NAME) {
				candidate->create().set_stage(CHOOSE_RACE);
			}
			break;
		case CHOOSE_RACE:
			_ctx.ui->display(
				Enums::Screen::CREATE_RACE,
				std::to_underlying(candidate->create().get_stage()));
			if (candidate->create().get_stage() !=
				Enums::Character::Stage::CHOOSE_RACE) {
				candidate->create().set_stage(CHOOSE_ALIGNMENT);
			}
			break;
		case CHOOSE_ALIGNMENT:
			_ctx.ui->display(
				Enums::Screen::CREATE_ALIGNMENT,
				std::to_underlying(candidate->create().get_stage()));
			if (candidate->create().get_stage() !=
				Enums::Character::Stage::CHOOSE_ALIGNMENT) {
				candidate->create().set_stage(CHOOSE_CLASS);
			}
			break;
		case CHOOSE_CLASS:
			_ctx.ui->display(
				Enums::Screen::CREATE_CLASS,
				std::to_underlying(candidate->create().get_stage()));
			if (candidate->create().get_stage() !=
				Enums::Character::Stage::CHOOSE_CLASS) {
				candidate->create().finalise();
				candidate->create().set_stage(REVIEW_AND_CONFIRM);
			}
			break;
		case REVIEW_AND_CONFIRM:

			// Order is changed to avoid doing a display before returning after
			if (candidate->create().get_stage() !=
				Enums::Character::Stage::REVIEW_AND_CONFIRM) {
				return BACK_TO_TRAINING_GROUNDS;
			}

			_ctx.ui->display(
				Enums::Screen::CREATE_CONFIRM,
				std::to_underlying(candidate->create().get_stage()));

			if (_ctx.controller->has_flag("confirm_keep_character")) {

				candidate->create().set_stage(COMPLETED);
				candidate->set_location(Enums::Character::Location::TAVERN);

				auto char_id = _ctx.game->save_character(*candidate);

				_ctx.game->characters.try_emplace(char_id, *candidate);
				_ctx.game->creation_candidate.reset();
				_ctx.game->save_game();

				_ctx.controller->unset_flag("confirm_keep_character");
				return BACK_TO_TRAINING_GROUNDS;
			} else if (_ctx.controller->has_flag("confirm_discard_character")) {

				_ctx.game->creation_candidate.reset();
				_ctx.controller->unset_flag("confirm_discard_character");
				return BACK_TO_TRAINING_GROUNDS;
			}

			break;
		case COMPLETED:
			return BACK_TO_TRAINING_GROUNDS;
			break;
		default:
			break;
		}

		_ctx.tick();
	}

	// Exit if we get to here having broken out of the loop
	return ABORT_GAME;
}

auto Sorcery::Create::_go_back_stage() -> bool {

	auto candidate{_ctx.game->creation_candidate};

	using enum Enums::Character::Stage;

	switch (candidate->create().get_stage()) {

	case ENTER_NAME:
		return true;

	case CHOOSE_RACE:
		candidate->create().set_stage(ENTER_NAME);
		_ctx.controller->clear_input_buffer();
		_ctx.ui->first_frame = true;
		break;

	case CHOOSE_ALIGNMENT:
		candidate->create().set_stage(CHOOSE_RACE);
		break;

	case CHOOSE_CLASS:
		candidate->create().set_stage(CHOOSE_ALIGNMENT);
		break;

	case REVIEW_AND_CONFIRM:
		candidate->create().set_stage(CHOOSE_CLASS);
		break;

	default:
		break;
	}

	return false;
}

auto Sorcery::Create::stop() -> int {

	_ctx.controller->go_to(Enums::Screen::TRAINING);
	return 0;
}
