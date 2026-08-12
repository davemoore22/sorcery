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

#include "core/context.hpp"
#include "core/display.hpp"
#include "core/ui.hpp"
#include <algorithm>
#include <chrono>
#include <cmath>

auto Sorcery::Module::_fade(const Enums::Screen screen, const float from,
							const float to,
							const std::chrono::milliseconds duration) -> void {

	using clock = std::chrono::steady_clock;

	const auto start{clock::now()};

	while (true) {

		SDL_Event event{};

		while (SDL_PollEvent(&event))
			ImGui_ImplSDL2_ProcessEvent(&event);

		const auto elapsed{
			std::chrono::duration_cast<std::chrono::milliseconds>(clock::now() -
																  start)};

		const auto progress{std::clamp(static_cast<float>(elapsed.count()) /
										   static_cast<float>(duration.count()),
									   0.0f, 1.0f)};

		const auto fade{std::lerp(from, to, progress)};

		_ctx.display->set_fade(fade);

		_ctx.ui->display(screen);
		_ctx.tick();

		if (progress >= 1.0f)
			break;
	}

	_ctx.display->set_fade(to);
}

auto Sorcery::Module::fade_in(const Enums::Screen screen,
							  const std::chrono::milliseconds duration)
	-> void {

	_fade(screen, 1.0f, 0.0f, duration);
}

auto Sorcery::Module::fade_out(const Enums::Screen screen,
							   const std::chrono::milliseconds duration)
	-> void {

	_fade(screen, 0.0f, 1.0f, duration);
}