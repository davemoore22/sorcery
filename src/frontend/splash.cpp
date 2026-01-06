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

#include "frontend/splash.hpp"
#include "core/context.hpp"
#include "core/controller.hpp"
#include "core/display.hpp"
#include "core/enum.hpp"
#include "core/system.hpp"
#include "core/ui.hpp"
#include "resources/define.hpp"
#include "resources/imagestore.hpp"

Sorcery::Splash::Splash(Context &ctx)
	: _ctx{ctx} {

	_initialise();
};

auto Sorcery::Splash::_initialise() -> bool {

	// Load initial textures so that they are immediately available
	_ctx.images->load_image(std::string{BANNER_TEXTURE});

	return true;
}

auto Sorcery::Splash::start() -> int {

	ImGui::SetMouseCursor(ImGuiMouseCursor_None);

	_ctx.controller->initialise();
	_ctx.controller->set_busy(true);

	// Main loop
	auto done{false};
	while (!done) {

		SDL_Event event;
		while (SDL_PollEvent(&event)) {
		}

		_ctx.ui->display(Enums::Screen::SPLASH);
		// done = _ui->images->load_next();
		done = true;
	}

	_ctx.controller->set_busy(false);

	return 0;
}

auto Sorcery::Splash::stop() -> int {

	return 0;
}