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

#include "core/module.hpp"
#include "core/application.hpp"
#include "core/context.hpp"
#include "core/controller.hpp"
#include "core/display.hpp"
#include "core/ui.hpp"
#include "resources/define.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>

auto Sorcery::Module::_fade(const std::function<void()> &draw, const float from,
							const float to,
							const std::chrono::milliseconds duration) -> void {

	using clock = std::chrono::steady_clock;

	const auto start{clock::now()};

	_ctx.display->set_fade(from);

	while (true) {

		SDL_Event event{};

		while (SDL_PollEvent(&event))
			ImGui_ImplSDL2_ProcessEvent(&event);

		const auto elapsed{std::chrono::duration<float>(clock::now() - start)};

		const auto total{std::chrono::duration<float>(duration)};

		const auto progress{
			std::clamp(elapsed.count() / total.count(), 0.0f, 1.0f)};

		_ctx.display->set_fade(std::lerp(from, to, progress));

		draw();

		_ctx.tick();

		if (progress >= 1.0f)
			break;
	}

	_ctx.display->set_fade(to);

	draw();
}

auto Sorcery::Module::fade_in(const Enums::Screen screen,
							  const std::chrono::milliseconds duration)
	-> void {

	_fade(
		[this, screen] {
			_ctx.ui->display(screen);
		},
		1.0f, 0.0f, duration);
}

auto Sorcery::Module::fade_out(const Enums::Screen screen,
							   const std::chrono::milliseconds duration)
	-> void {

	_fade(
		[this, screen] {
			_ctx.ui->display(screen);
		},
		0.0f, 1.0f, duration);
}

auto Sorcery::Module::fade_in(const std::function<void()> &draw,
							  const std::chrono::milliseconds duration)
	-> void {

	_fade(draw, 1.0f, 0.0f, duration);
}

auto Sorcery::Module::fade_out(const std::function<void()> &draw,
							   const std::chrono::milliseconds duration)
	-> void {

	_fade(draw, 0.0f, 1.0f, duration);
}

auto Sorcery::Module::process_event(const SDL_Event &event,
									const EventOptions &options)
	-> ModuleEvent {

	ImGui_ImplSDL2_ProcessEvent(&event);

	if (_ctx.controller->check_for_abort(event))
		return ModuleEvent::ABORT;

	_ctx.controller->check_for_resize(event, _ctx.ui);

	if (options.menu_key)
		_ctx.controller->check_for_menu_key(event);

	if (options.debug)
		_ctx.controller->check_for_debug(event);

	if (options.quicksave && _ctx.controller->check_for_quicksave(event)) {

		_ctx.application->save_state_to_binary(
			_ctx.get_file(SAVE_STATE_FILENAME));

	} else if (options.quickload &&
			   _ctx.controller->check_for_quickload(event)) {

		_ctx.application->load_state_from_binary(
			_ctx.get_file(SAVE_STATE_FILENAME));

		return ModuleEvent::QUICKLOAD;
	}

	return ModuleEvent::NONE;
}

auto Sorcery::Module::show_immediately() -> void {

	_ctx.display->set_fade(0.0f);
}