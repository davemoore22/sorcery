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

#include <array>
#include <span>
#include <string_view>

namespace Sorcery {

namespace Enums::Input {

	enum class Mode {
		NONE,
		MENU,
		OPTION,
		CONFIRMATION,
		TEXT_INPUT,
		NUMBER_INPUT,
		ALLOCATE_STATS,
		FILE_VIEWER,
		REVIEW_AND_CONFIRM,
		CHARACTER_BROWSER,
		ENGINE
	};
};

namespace Enums::Controls {

	enum class Input {
		NO_INPUT,
		MOUSE,
		UP,
		DOWN,
		LEFT,
		RIGHT,
		CONFIRM,
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
}

struct HelpEntry {
		Enums::Controls::Input control;
		std::string_view string_key;
};

namespace Help {

	using enum Enums::Controls::Input;
	using enum Enums::Controls::HelpGlyph;

	inline constexpr std::size_t MAX_GLYPHS{3};

	inline constexpr std::array mouse_glyphs{MOUSE_LEFT};
	inline constexpr std::array up_glyphs{KEY_ARROW_UP, XBOX_DPAD_UP};
	inline constexpr std::array down_glyphs{KEY_ARROW_DOWN, XBOX_DPAD_DOWN};
	inline constexpr std::array left_glyphs{KEY_ARROW_LEFT};
	inline constexpr std::array right_glyphs{KEY_ARROW_RIGHT};
	inline constexpr std::array confirm_glyphs{KEY_ENTER, XBOX_A, MOUSE_LEFT};
	inline constexpr std::array back_glyphs{KEY_ESCAPE, XBOX_B, MOUSE_RIGHT};
	inline constexpr std::array escape_glyphs{KEY_ESCAPE};
	inline constexpr std::array help_glyphs{KEY_F1, XBOX_Y};
	inline constexpr std::array cheat_glyphs{KEY_F2};
	inline constexpr std::array page_up_glyphs{KEY_PAGE_UP};
	inline constexpr std::array page_down_glyphs{KEY_PAGE_DOWN};
	inline constexpr std::array home_glyphs{KEY_HOME};
	inline constexpr std::array end_glyphs{KEY_END};
	inline constexpr std::array space_glyphs{KEY_SPACE};

	inline constexpr std::array menu{
		HelpEntry{CONFIRM, "HELP_SELECT_ITEM"},
		HelpEntry{UP, "HELP_PREVIOUS_ITEM"},
		HelpEntry{DOWN, "HELP_NEXT_ITEM"},
	};

	inline constexpr std::array option{
		HelpEntry{MOUSE, "HELP_SELECT_OPTION"}, HelpEntry{UP, "HELP_PREVIOUS_OPTION"},
		HelpEntry{DOWN, "HELP_NEXT_OPTION"},	HelpEntry{LEFT, "HELP_TOGGLE_OPTION"},
		HelpEntry{RIGHT, "HELP_TOGGLE_OPTION"}, HelpEntry{CONFIRM, "HELP_TOGGLE_OPTION"},
	};

	inline constexpr std::array allocate_stats{
		HelpEntry{MOUSE, "HELP_SELECT_STAT"},	HelpEntry{UP, "HELP_PREVIOUS_STAT"},
		HelpEntry{DOWN, "HELP_NEXT_STAT"},		HelpEntry{LEFT, "HELP_DECREASE_STAT"},
		HelpEntry{RIGHT, "HELP_INCREASE_STAT"},
	};

	inline constexpr std::array always{
		HelpEntry{BACK, "HELP_GO_BACK"},
		HelpEntry{HELP, "HELP_SHOW_CONTROLS"},
		HelpEntry{CHEAT, "HELP_SHOW_CHEAT"},
	};

	[[nodiscard]]
	constexpr auto entries(const Enums::Input::Mode mode) -> std::span<const HelpEntry> {

		using enum Enums::Input::Mode;

		switch (mode) {

		case MENU:
			return menu;

		case OPTION:
			return option;

		case ALLOCATE_STATS:
			return allocate_stats;

		default:
			return {};
		}
	}

	[[nodiscard]]
	constexpr auto glyphs(const Enums::Controls::Input input) -> std::span<const Enums::Controls::HelpGlyph> {

		switch (input) {

		case CHEAT:
			return cheat_glyphs;

		case MOUSE:
			return mouse_glyphs;

		case UP:
			return up_glyphs;

		case DOWN:
			return down_glyphs;

		case LEFT:
			return left_glyphs;

		case RIGHT:
			return right_glyphs;

		case CONFIRM:
			return confirm_glyphs;

		case BACK:
			return back_glyphs;

		case ESCAPE:
			return escape_glyphs;

		case HELP:
			return help_glyphs;

		case PAGE_UP:
			return page_up_glyphs;

		case PAGE_DOWN:
			return page_down_glyphs;

		case HOME:
			return home_glyphs;

		case END:
			return end_glyphs;

		case SPACE:
			return space_glyphs;

		default:
			return {};
		}
	}

}
};