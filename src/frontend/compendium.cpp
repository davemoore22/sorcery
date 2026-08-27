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

#include "frontend/compendium.hpp"
#include "common/sdl2.hpp"
#include "core/context.hpp"
#include "core/controller.hpp"
#include "core/enum.hpp"
#include "display/ui/ui.hpp"
#include "drawables/define.hpp"
#include "frontend/atlas.hpp"
#include "frontend/bestiary.hpp"
#include "frontend/license.hpp"
#include "frontend/museum.hpp"
#include "frontend/spellbook.hpp"

Sorcery::Compendium::Compendium(Context &ctx)
	: Module{ctx} {

	_bestiary = std::make_unique<Bestiary>(_ctx);
	_museum = std::make_unique<Museum>(_ctx);
	_atlas = std::make_unique<Atlas>(_ctx);
	_spellbook = std::make_unique<SpellBook>(_ctx);

	_initialise();
};

Sorcery::Compendium::~Compendium() {}

auto Sorcery::Compendium::_initialise() -> bool {

	_ctx.controller->set_selected("compendium_selected", 0);

	return true;
}

auto Sorcery::Compendium::start() -> int {

	_ctx.controller->go_to(Enums::Screen::COMPENDIUM);
	_ctx.controller->initialise();

	fade_in(Enums::Screen::COMPENDIUM, QUICK_FADE);

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
				return GO_TO_FRONT_END;
		}

		_ctx.ui->display(Enums::Screen::COMPENDIUM);

		_ctx.tick();

		// If we have selected something, let's action it - either return to the
		// calling object, or handle front-end stuff like options, license, or
		// compendium here
		if (_ctx.controller->wants(Enums::Screen::BESTIARY)) {
			const auto result{_bestiary->start()};
			_bestiary->stop();
			if (result == ABORT_GAME)
				return ABORT_GAME;
			fade_in(Enums::Screen::COMPENDIUM, QUICK_FADE);
		} else if (_ctx.controller->wants(Enums::Screen::MUSEUM)) {
			const auto result{_museum->start()};
			_museum->stop();
			if (result == ABORT_GAME)
				return ABORT_GAME;
			fade_in(Enums::Screen::COMPENDIUM, QUICK_FADE);
		} else if (_ctx.controller->wants(Enums::Screen::ATLAS)) {
			const auto result{_atlas->start()};
			_atlas->stop();
			if (result == ABORT_GAME)
				return ABORT_GAME;
			fade_in(Enums::Screen::COMPENDIUM, QUICK_FADE);
		} else if (_ctx.controller->wants(Enums::Screen::SPELLBOOK)) {
			const auto result{_spellbook->start()};
			_spellbook->stop();
			if (result == ABORT_GAME)
				return ABORT_GAME;
			fade_in(Enums::Screen::COMPENDIUM, QUICK_FADE);
		}

		if (_ctx.controller->want_to_abort())
			return ABORT_GAME;
		else if (!_ctx.controller->wants(Enums::Screen::COMPENDIUM))
			return GO_TO_FRONT_END;
	}

	// Exit if we get to here having broken out of the loop
	return ABORT_GAME;
}

auto Sorcery::Compendium::stop() -> int {

	_ctx.controller->go_to(Enums::Screen::MAINMENU);

	fade_out(Enums::Screen::COMPENDIUM, QUICK_FADE);

	return 0;
}