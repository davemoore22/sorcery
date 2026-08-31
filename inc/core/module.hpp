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

#pragma once

#include <SDL_events.h> // for SDL_Event
#include <chrono>		// for milliseconds, chrono_literals
#include <functional>	// for function

namespace Sorcery {
namespace Enums { enum class Screen; }
}
namespace Sorcery { struct Context; }

namespace Sorcery {

enum class ModuleEvent {
	NONE,
	ABORT,
	QUICKLOAD
};

struct EventOptions {
		bool menu_key{};
		bool debug{};
		bool quicksave{true};
		bool quickload{true};
};

using namespace std::chrono_literals;

inline constexpr auto QUICK_FADE{500ms};

class Module {

	public:
		explicit Module(Context &ctx)
			: _ctx{ctx} {}

		virtual ~Module() = default;

	protected:
		auto fade_in(Enums::Screen screen, std::chrono::milliseconds duration)
			-> void;
		auto fade_out(Enums::Screen screen, std::chrono::milliseconds duration)
			-> void;
		auto fade_in_with_string(const Enums::Screen screen,
								  const std::chrono::milliseconds duration,
								  const std::string &string) -> void;
		auto fade_out_with_string(const Enums::Screen screen,
								  const std::chrono::milliseconds duration,
								  const std::string &string) -> void;
		auto fade_in(const std::function<void()> &draw,
					 std::chrono::milliseconds duration) -> void;
		auto fade_out(const std::function<void()> &draw,
					  std::chrono::milliseconds duration) -> void;
		auto process_event(const SDL_Event &event,
						   const EventOptions &options = {}) -> ModuleEvent;
		auto show_immediately() -> void;

		Context &_ctx;

	private:
		auto _fade(const std::function<void()> &draw, float from, float to,
				   std::chrono::milliseconds duration) -> void;
};

}