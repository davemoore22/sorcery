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

#include "modules/inspect.hpp"
#include "common/macro.hpp"
#include "core/application.hpp"
#include "core/context.hpp"
#include "core/controller.hpp"
#include "core/define.hpp"
#include "core/enum.hpp"
#include "core/system.hpp"
#include "display/display.hpp"
#include "display/ui.hpp"
#include "drawables/define.hpp"
#include "drawables/dialog.hpp"
#include "drawables/modal.hpp"
#include "drawables/popup.hpp"
#include "engine/define.hpp"
#include "resources/define.hpp"
#include "types/character/character.hpp"
#include "types/game.hpp"
#include "types/state.hpp"

Sorcery::Inspect::Inspect(Context &ctx)
	: Module{ctx} {

	_initialise();
};

Sorcery::Inspect::~Inspect() {}

auto Sorcery::Inspect::_initialise() -> bool {

	return true;
}

auto Sorcery::Inspect::set(const int char_id) -> void {

	_ctx.controller->set_character(Enums::CharacterSlot::INSPECT, char_id);
}

auto Sorcery::Inspect::start(const int mode, const int start_char) -> int {

	_ctx.controller->go_to(Enums::Screen::INSPECT);
	_ctx.controller->initialise();
	_ctx.controller->set_character(Enums::CharacterSlot::INSPECT, start_char);

	show_immediately();

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
				return BACK_FROM_INSPECT;
		}

		if (!_ctx.controller->wants(Enums::Screen::INSPECT))
			return BACK_FROM_INSPECT;

		if (_ctx.controller->go_back) {
			_ctx.controller->go_back = false;
			return BACK_FROM_INSPECT;
		}

		_ctx.ui->display(Enums::Screen::INSPECT, mode);
		_ctx.tick();

		if (_ctx.controller->has_flag("select_previous_character")) {

			const auto p_size{_ctx.game->state->get_party_size()};
			const int char_id{
				_ctx.controller->get_character(Enums::CharacterSlot::INSPECT)};
			int pos{_ctx.game->state->get_char_slot(char_id).value()};
			if (pos > 1)
				--pos;
			else
				pos = static_cast<int>(p_size);
			_ctx.controller->set_character(
				Enums::CharacterSlot::INSPECT,
				_ctx.game->state->get_party_char(pos).value());
			_ctx.controller->unset_flag("select_previous_character");
		} else if (_ctx.controller->has_flag("select_next_character")) {

			const auto p_size{_ctx.game->state->get_party_size()};
			const int char_id{
				_ctx.controller->get_character(Enums::CharacterSlot::INSPECT)};
			int pos{_ctx.game->state->get_char_slot(char_id).value()};
			if (pos == static_cast<int>(p_size))
				pos = 1;
			else
				++pos;
			_ctx.controller->set_character(
				Enums::CharacterSlot::INSPECT,
				_ctx.game->state->get_party_char(pos).value());
			_ctx.controller->unset_flag("select_next_character");
		}
	}

	// Exit if we get to here having broken out of the loop
	return ABORT_GAME;
}

auto Sorcery::Inspect::stop(const int mode) -> void {

	_ctx.controller->clear_character(Enums::CharacterSlot::INSPECT);
	if (mode & INSPECT_MODE_CAMP)
		_ctx.controller->go_to(Enums::Screen::ENGINE);
	else if (mode & INSPECT_MODE_TAVERN)
		_ctx.controller->go_to(Enums::Screen::TAVERN);
	else if (mode & INSPECT_MODE_INN)
		_ctx.controller->go_to(Enums::Screen::INN);
	else if (mode & INSPECT_MODE_TEMPLE)
		_ctx.controller->go_to(Enums::Screen::TEMPLE);
}