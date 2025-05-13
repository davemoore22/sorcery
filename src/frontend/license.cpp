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

#include "frontend/license.hpp"
#include "common/macro.hpp"
#include "core/controller.hpp"
#include "core/display.hpp"
#include "core/system.hpp"
#include "core/ui.hpp"
#include "gui/define.hpp"

Sorcery::License::License(System *system, Display *display, UI *ui,
						  Controller *controller)
	: _system{system},
	  _display{display},
	  _ui{ui},
	  _controller{controller} {

	_initialise();
};

auto Sorcery::License::_initialise() -> bool {

	auto file_path{(*_system->files)[LICENSE_FILE].string()};

	if (std::ifstream file{(file_path.c_str()), std::ifstream::in};
		file.good()) {

		_license_text.assign((std::istreambuf_iterator<char>(file)),
							 (std::istreambuf_iterator<char>()));
	}

	return true;
}

auto Sorcery::License::start() -> int {

	_controller->initialise("license");
	_controller->set_flag("show_license");

	// Main loop
	auto done{false};
	while (!done) {

		SDL_Event event;
		while (SDL_PollEvent(&event)) {

			// Check for Quit or Back Events
			ImGui_ImplSDL2_ProcessEvent(&event);
			_controller->check_for_resize(event, _ui);
			done = _controller->check_for_abort(event);
			if (_controller->check_for_back(event))
				return GO_TO_FRONT_END;
		}

		_ui->display("license", _license_text);

		// If we have selected something, let's action it - either return to the
		// calling object, or handle front-end stuff like options, license, or
		// compendium here
		if (_controller->has_flag("want_abort"))
			return ABORT_GAME;
		else if (!_controller->has_flag("show_license"))
			return GO_TO_FRONT_END;
	}

	// Exit if we get to here having broken out of the loop
	return ABORT_GAME;
}

auto Sorcery::License::stop() -> int {

	_controller->move_screen("show_license", "show_main_menu");

	return 0;
}