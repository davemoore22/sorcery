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
#include "core/controller.hpp"
#include "core/display.hpp"
#include "core/system.hpp"
#include "core/ui.hpp"
#include "gui/define.hpp"
#include "gui/dialog.hpp"
#include "gui/input.hpp"
#include "gui/modal.hpp"
#include "modules/inspect.hpp"
#include "modules/pay.hpp"
#include "types/game.hpp"

Sorcery::Temple::Temple(System *system, Display *display, UI *ui,
						Controller *controller)
	: _system{system},
	  _display{display},
	  _ui{ui},
	  _controller{controller} {

	_inspect = std::make_unique<Inspect>(_system, _display, _ui, _controller);
	_pay = std::make_unique<Pay>(_system, _display, _ui, _controller);

	_initialise();
};

auto Sorcery::Temple::_initialise() -> bool {

	_controller->selected["party_panel_selected"] = 0;

	return true;
}

auto Sorcery::Temple::start(Game *game) -> int {

	// Helper function to ensure that back events when a modal is open only
	// close the modal as opposed to closing the whole module
	auto has_modal_open = [](const std::vector<bool> flags) -> bool {
		return std::ranges::any_of(flags, [](bool flag) {
			return flag == true;
		});
	};

	_controller->initialise("inn");
	_controller->move_screen("show_castle", "show_temple");

	// Need this before accessing dynamic modals!
	_ui->create_dynamic_modal(game, "modal_inspect");
	_ui->create_dynamic_modal(game, "modal_identify");
	_ui->create_dynamic_modal(game, "modal_help");
	_ui->create_dynamic_modal(game, "modal_tithe");
	_ui->modal_inspect->show = false;
	_ui->modal_help->show = false;
	_ui->modal_tithe->show = false;
	_ui->modal_identify->show = false;

	_ui->input_donate->show = false;
	_ui->input_donate->initialise(game);

	_controller->clear_character("inspect");
	_controller->clear_character("help");
	_controller->clear_character("tithe");

	// Main loop
	auto done{false};
	while (!done) {

		SDL_Event event{};
		while (SDL_PollEvent(&event)) {

			// Check for Quit Events
			ImGui_ImplSDL2_ProcessEvent(&event);
			done = _controller->check_for_abort(event);

			// Check for Window Resize
			_controller->check_for_resize(event, _ui);

			// Check for Back Event (close a Modal if present, else return to
			// the Castle)
			if (_controller->check_for_back(event)) {
				if (!has_modal_open(
						{_ui->modal_inspect->show, _ui->modal_help->show,
						 _ui->modal_tithe->show, _ui->input_donate->show}))
					return BACK_TO_CASTLE;
				else {

					// Close Modals
					if (_ui->modal_inspect->show) {
						_controller->clear_character("inspect");
						_controller->unset_flag("want_inspect");
						_controller->unset_flag("want_identify");
						_ui->modal_inspect->show = false;
					} else if (_ui->modal_help->show) {
						_controller->clear_character("help");
						_controller->unset_flag("want_help");
						_ui->modal_help->show = false;
					} else if (_ui->modal_tithe->show) {
						_controller->clear_character("tithe");
						_controller->unset_flag("want_tithe");
						_ui->modal_tithe->show = false;
						_ui->input_donate->show = true;
					} else if (_ui->input_donate->show) {
						_controller->unset_flag("want_donate");
						_ui->input_donate->show = false;
					} else if (_ui->notice_cannot_donate->show) {
						_controller->unset_flag("want_cannot_donate");
						_ui->notice_cannot_donate->show = false;
					} else if (_ui->notice_donated_ok->show) {
						_controller->unset_flag("want_donated_ok");
						_ui->notice_donated_ok->show = false;
					} else if (_ui->notice_not_enough_gold->show) {
						_controller->unset_flag("want_not_enough_gold");
						_ui->notice_not_enough_gold->show = false;
					}
				}
			}
		}

		_ui->display("temple", game);

		if (!_controller->has_flag("show_temple") &&
			_controller->has_flag("show_castle"))
			return BACK_TO_CASTLE;

		if (_controller->has_character("inspect")) {
			_inspect->start(game, INSPECT_MODE_TEMPLE,
							_controller->get_character("inspect"));
			_inspect->stop(game, INSPECT_MODE_TEMPLE);
			_controller->clear_character("inspect");
		}

		if (_controller->has_character("help")) {
			auto heal_result{_pay->start(game)};
			if (heal_result == HEALED_NOT)
				_pay->stop(false);
			else {
				_pay->stop(true);

				std::cout << "heal results!";
			}

			_controller->clear_character("help");
		}

		if (_controller->has_character("tithe")) {

			// We cannot clear the tithe character here, as we are still on the
			// same screen, only to display the donate popup
		}
	}

	// Exit if we get to here having broken out of the loop
	return ABORT_GAME;
}

auto Sorcery::Temple::stop() -> int {

	_controller->move_screen("show_temple", "show_castle");
	_controller->clear_character("inspect");

	return 0;
}