// Copyright (C) 2025 Dave Moore
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

#include "modules/temple.hpp"
#include "common/macro.hpp"
#include "core/context.hpp"
#include "core/controller.hpp"
#include "core/display.hpp"
#include "core/enum.hpp"
#include "core/system.hpp"
#include "core/ui.hpp"
#include "gui/define.hpp"
#include "gui/dialog.hpp"
#include "gui/input.hpp"
#include "gui/modal.hpp"
#include "modules/inspect.hpp"
#include "modules/pay.hpp"
#include "types/game.hpp"

Sorcery::Temple::Temple(Context &ctx)
	: _ctx{ctx} {

	_inspect = std::make_unique<Inspect>(_ctx);
	_pay = std::make_unique<Pay>(_ctx);

	_initialise();
};

Sorcery::Temple::~Temple() {}

auto Sorcery::Temple::_initialise() -> bool {

	_ctx.controller->set_selected("party_panel_selected", 0);

	return true;
}

auto Sorcery::Temple::start() -> int {

	// Helper function to ensure that back events when a modal is open only
	// close the modal as opposed to closing the whole module
	auto has_modal_open = [](const std::vector<bool> flags) -> bool {
		return std::ranges::any_of(flags, [](bool flag) {
			return flag == true;
		});
	};

	_ctx.controller->initialise();
	_ctx.controller->go_to(Enums::Screen::TEMPLE);

	// Need this before accessing dynamic modals!
	_ctx.ui->create_dynamic_modal("modal_inspect");
	_ctx.ui->create_dynamic_modal("modal_identify");
	_ctx.ui->create_dynamic_modal("modal_drop");
	_ctx.ui->create_dynamic_modal("modal_help");
	_ctx.ui->create_dynamic_modal("modal_tithe");
	_ctx.ui->create_dynamic_modal("modal_trade");
	_ctx.ui->create_dynamic_modal("modal_use");
	_ctx.ui->create_dynamic_modal("modal_invoke");
	_ctx.ui->modal_inspect->show = false;
	_ctx.ui->modal_help->show = false;
	_ctx.ui->modal_tithe->show = false;
	_ctx.ui->modal_identify->show = false;
	_ctx.ui->modal_drop->show = false;
	_ctx.ui->modal_trade->show = false;
	_ctx.ui->modal_use->show = false;
	_ctx.ui->modal_invoke->show = false;

	_ctx.ui->input_donate->show = false;
	_ctx.ui->input_donate->initialise(_ctx.game);

	_ctx.controller->clear_character("inspect");
	_ctx.controller->clear_character("help");
	_ctx.controller->clear_character("tithe");

	// Main loop
	auto done{false};
	while (!done) {

		SDL_Event event{};
		while (SDL_PollEvent(&event)) {

			// Check for Quit Events
			ImGui_ImplSDL2_ProcessEvent(&event);
			done = _ctx.controller->check_for_abort(event);

			// Check for Window Resize
			_ctx.controller->check_for_resize(event, _ctx.ui);

			// Check for Back Event (close a Modal if present, else return to
			// the Castle)
			if (_ctx.controller->check_for_back(event)) {
				if (!has_modal_open({_ctx.ui->modal_inspect->show,
									 _ctx.ui->modal_help->show,
									 _ctx.ui->modal_tithe->show,
									 _ctx.ui->input_donate->show}))
					return BACK_TO_CASTLE;
				else {

					// Close Modals
					if (_ctx.ui->modal_inspect->show) {
						_ctx.controller->clear_character("inspect");
						_ctx.controller->unset_flag("want_inspect");
						_ctx.controller->unset_flag("want_identify");
						_ctx.controller->unset_flag("want_drop");
						_ctx.controller->unset_flag("want_trade");
						_ctx.controller->unset_flag("want_pool_gold");
						_ctx.ui->modal_inspect->show = false;
					} else if (_ctx.ui->modal_help->show) {
						_ctx.controller->clear_character("help");
						_ctx.controller->unset_flag("want_help");
						_ctx.ui->modal_help->show = false;
					} else if (_ctx.ui->modal_tithe->show) {
						_ctx.controller->clear_character("tithe");
						_ctx.controller->unset_flag("want_tithe");
						_ctx.ui->modal_tithe->show = false;
						_ctx.ui->input_donate->show = true;
					} else if (_ctx.ui->input_donate->show) {
						_ctx.controller->unset_flag("want_donate");
						_ctx.ui->input_donate->show = false;
					} else if (_ctx.ui->notice_cannot_donate->show) {
						_ctx.controller->unset_flag("want_cannot_donate");
						_ctx.ui->notice_cannot_donate->show = false;
					} else if (_ctx.ui->notice_donated_ok->show) {
						_ctx.controller->unset_flag("want_donated_ok");
						_ctx.ui->notice_donated_ok->show = false;
					} else if (_ctx.ui->notice_not_enough_gold->show) {
						_ctx.controller->unset_flag("want_not_enough_gold");
						_ctx.ui->notice_not_enough_gold->show = false;
					}
				}
			}
		}

		_ctx.ui->display(Enums::Screen::TEMPLE, _ctx.game);

		if (!_ctx.controller->wants(Enums::Screen::TEMPLE) &&
			_ctx.controller->wants(Enums::Screen::CASTLE))
			return BACK_TO_CASTLE;

		if (_ctx.controller->has_character("inspect")) {
			_inspect->start(INSPECT_MODE_TEMPLE,
							_ctx.controller->get_character("inspect"));
			_inspect->stop(INSPECT_MODE_TEMPLE);
			_ctx.controller->clear_character("inspect");
		}

		if (_ctx.controller->has_character("help")) {
			auto heal_result{_pay->start()};
			if (heal_result == HEALED_NOT)
				_pay->stop(false);
			else {
				_pay->stop(true);
			}

			_ctx.controller->clear_character("help");
		}

		if (_ctx.controller->has_character("tithe")) {
			// We cannot clear the tithe character here, as we are still on the
			// same screen, only to display the donate popup
		}
	}

	// Exit if we get to here having broken out of the loop
	return ABORT_GAME;
}

auto Sorcery::Temple::stop() -> int {

	_ctx.controller->go_to(Enums::Screen::CASTLE);
	_ctx.controller->clear_character("inspect");

	return 0;
}