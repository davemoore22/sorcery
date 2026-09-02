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

#include "frontend/splash.hpp"
#include "backends/imgui_impl_sdl2.h"		// for SDL_Event
#include "core/context.hpp"					// for Context
#include "core/controller/controller.hpp"	// for Controller
#include "core/controller/inputhandler.hpp" // For ControllerInputHandler
#include "core/enum.hpp"					// for Screen
#include "display/ui/ui.hpp"				// for UI
#include "imgui.h"					// for ImGuiMouseCursor_, SetMouseCursor
#include "resources/define.hpp"		// for BANNER_TEXTURE
#include "resources/imagestore.hpp" // for ImageStore
#include <SDL_events.h>				// for SDL_PollEvent
#include <chrono>					// for chrono_literals
#include <string>					// for basic_string, string
#include <string_view>				// for basic_string_view

Sorcery::Splash::Splash(Context &ctx)
	: Module{ctx} {

	_initialise();
};

auto Sorcery::Splash::_initialise() -> bool {

	// Load initial textures so that they are immediately available
	_ctx.images->load_image(std::string{BANNER_TEXTURE});

	return true;
}

auto Sorcery::Splash::start() -> int {

	using namespace std::chrono_literals;

	ImGui::SetMouseCursor(ImGuiMouseCursor_None);

	_ctx.controller->initialise();
	_ctx.controller->set_busy(true);

	fade_in(Enums::Screen::SPLASH, QUICK_FADE);

	// Main loop
	bool done{false};
	while (!done) {

		SDL_Event event{};
		while (SDL_PollEvent(&event)) {
		}

		_ctx.ui->display_screen(Enums::Screen::SPLASH);
		_ctx.tick();

		done = true;
	}

	_ctx.controller->set_busy(false);

	return 0;
}

auto Sorcery::Splash::stop() -> int {

	using namespace std::chrono_literals;

	fade_out(Enums::Screen::SPLASH, QUICK_FADE);

	return 0;
}