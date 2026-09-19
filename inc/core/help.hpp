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

#include "core/controller/inputmode.hpp"

#include <cstddef>
#include <span>
#include <string_view>

namespace Sorcery::Enums::Controls {

enum class Input {
	NO_INPUT,
	MOUSE,
	UP,
	DOWN,
	LEFT,
	RIGHT,
	CONFIRM,
	YES_NO,
	BACK,
	ESCAPE,
	HELP,
	CHEAT,

	PAGE_UP,
	PAGE_DOWN,
	HOME,
	END,

	ALPHANUMERIC,
	SPACE,

	TURN_AROUND,
	CAMP,
	ACTION,
	SEARCH,
	INSPECT,
	GUI_TOGGLE,
	STATUS_TOGGLE,
	QUIT
};

enum class HelpGlyph : unsigned int {

	KEY_0 = 0,
	KEY_1,
	KEY_2,
	KEY_3,
	KEY_4,
	KEY_5,
	KEY_6,
	KEY_7,
	KEY_8,
	KEY_9,

	KEY_A = 10,
	KEY_B,
	KEY_C,
	KEY_D,
	KEY_E,
	KEY_F,
	KEY_G,
	KEY_H,
	KEY_I,
	KEY_J,
	KEY_K,
	KEY_L,
	KEY_M,
	KEY_N,
	KEY_O,
	KEY_P,
	KEY_Q,
	KEY_R,
	KEY_S,
	KEY_T,
	KEY_U,
	KEY_V,
	KEY_W,
	KEY_X,
	KEY_Y,
	KEY_Z,

	KEY_F1 = 36,
	KEY_F2,
	KEY_F3,
	KEY_F4,
	KEY_F5,
	KEY_F6,
	KEY_F7,
	KEY_F8,
	KEY_F9,
	KEY_F10,
	KEY_F11,
	KEY_F12,

	XBOX_A = 48,
	XBOX_B,
	XBOX_DPAD,
	XBOX_DPAD_DOWN,
	XBOX_DPAD_LEFT,
	XBOX_DPAD_RIGHT,
	XBOX_DPAD_UP,
	XBOX_LB,
	XBOX_LT,
	XBOX_LEFT_STICK,
	XBOX_LEFT_STICK_CLICK,
	XBOX_MENU,
	XBOX_RB,
	XBOX_RT,
	XBOX_RIGHT_STICK,
	XBOX_RIGHT_STICK_CLICK,
	XBOX_SHARE,
	XBOX_VIEW,
	XBOX_X,
	XBOX_Y,

	KEY_ALT = 68,
	KEY_ARROW_DOWN,
	KEY_ARROW_LEFT,
	KEY_ARROW_RIGHT,
	KEY_ARROW_UP,
	KEY_ASTERISK,
	KEY_BACKSPACE_ALT,
	KEY_BACKSPACE,
	KEY_BRACKET_LEFT,
	KEY_BRACKET_RIGHT,
	KEY_CAPS_LOCK,
	KEY_COMMAND,
	KEY_CTRL,
	KEY_DELETE,
	KEY_END,
	KEY_ENTER_ALT,
	KEY_ENTER,
	KEY_ENTER_TALL,
	KEY_ESCAPE,
	KEY_HOME,
	KEY_INSERT,
	KEY_MARK_LEFT,
	KEY_MARK_RIGHT,
	KEY_MINUS,
	KEY_NUM_LOCK,
	KEY_PAGE_DOWN,
	KEY_PAGE_UP,
	KEY_PLUS,
	KEY_PLUS_TALL,
	KEY_PRINT_SCREEN,
	KEY_QUESTION,
	KEY_QUOTE,
	KEY_SEMICOLON,
	KEY_SHIFT_ALT,
	KEY_SHIFT,
	KEY_SLASH,
	KEY_SPACE,
	KEY_TAB,
	KEY_TILDE,
	KEY_WIN,

	MOUSE_LEFT = 108,
	MOUSE_MIDDLE,
	MOUSE_RIGHT,
	MOUSE_SIMPLE
};

} // namespace Sorcery::Enums::Controls

namespace Sorcery {

struct HelpEntry {
		Enums::Controls::Input control;
		std::string_view string_key;
};

namespace Help {

	inline constexpr std::size_t MAX_GLYPHS{3};

	[[nodiscard]]
	auto entries(Enums::Input::Mode mode) -> std::span<const HelpEntry>;

	[[nodiscard]]
	auto glyphs(Enums::Controls::Input input) -> std::span<const Enums::Controls::HelpGlyph>;

	[[nodiscard]]
	auto always() -> std::span<const HelpEntry>;

} // namespace Help

} // namespace Sorcery