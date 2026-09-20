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

#include "core/help.hpp"
#include "core/controller/inputmode.hpp" // for Mode, Mode::ALLOCATE_STATS
#include <array>						 // for array

namespace Sorcery::Help {

using enum Enums::Controls::Input;
using enum Enums::Controls::HelpGlyph;

namespace {

	constexpr std::array mouse_glyphs{MOUSE_LEFT};

	constexpr std::array up_glyphs{KEY_ARROW_UP, XBOX_DPAD_UP};

	constexpr std::array down_glyphs{KEY_ARROW_DOWN, XBOX_DPAD_DOWN};

	constexpr std::array left_glyphs{KEY_ARROW_LEFT};

	constexpr std::array right_glyphs{KEY_ARROW_RIGHT};

	constexpr std::array confirm_glyphs{KEY_ENTER, XBOX_A, MOUSE_LEFT};

	constexpr std::array notice_glyphs{KEY_ENTER, XBOX_A, MOUSE_LEFT};

	constexpr std::array back_glyphs{KEY_ESCAPE, XBOX_B, MOUSE_RIGHT};

	constexpr std::array escape_glyphs{KEY_ESCAPE};

	constexpr std::array help_glyphs{KEY_F1, XBOX_Y};

	constexpr std::array cheat_glyphs{KEY_F2};

	constexpr std::array page_up_glyphs{KEY_PAGE_UP};

	constexpr std::array page_down_glyphs{KEY_PAGE_DOWN};

	constexpr std::array home_glyphs{KEY_HOME};

	constexpr std::array end_glyphs{KEY_END};

	constexpr std::array space_glyphs{KEY_SPACE};

	constexpr std::array yes_no_glyphs{KEY_Y, KEY_N};

	constexpr std::array quit_glyphs{KEY_Q};

	constexpr std::array move_forward_glyphs{KEY_W, KEY_F, KEY_ARROW_UP, XBOX_DPAD_UP};
	constexpr std::array move_backwards_glyphs{KEY_Z, KEY_ARROW_DOWN, XBOX_DPAD_DOWN};
	constexpr std::array turn_left_glyphs{KEY_A, KEY_L, KEY_ARROW_LEFT, XBOX_DPAD_LEFT};
	constexpr std::array turn_right_glyphs{KEY_D, KEY_R, KEY_ARROW_RIGHT, XBOX_DPAD_RIGHT};
	constexpr std::array turn_around_glyphs{KEY_X};
	constexpr std::array camp_glyphs{KEY_ENTER, XBOX_A, MOUSE_LEFT};
	constexpr std::array gui_toggle_glyphs{KEY_O, KEY_S};
	constexpr std::array colour_toggle_glypths{KEY_N};
	constexpr std::array automap_glyphs{KEY_M};
	constexpr std::array quick_load_save_glyphs{KEY_F9, KEY_F10};
	constexpr std::array front_row_glypths{KEY_1, KEY_2, KEY_3};
	constexpr std::array back_row_glypths{KEY_4, KEY_5, KEY_6};

	// spell - MIDDLE MOUSE, XBOX_X?
	// use - XBOX_Y?

	constexpr std::array menu{
		HelpEntry{CONFIRM, "HELP_SELECT_ITEM"},
		HelpEntry{UP, "HELP_PREVIOUS_ITEM"},
		HelpEntry{DOWN, "HELP_NEXT_ITEM"},
	};

	constexpr std::array confirm{
		HelpEntry{CONFIRM, "HELP_CONFIRM_CONFIRMATION"},
		HelpEntry{YES_NO, "HELP_Y_N_CONFIRMATION"},
	};

	constexpr std::array notice{
		HelpEntry{NOTICE, "HELP_NOTICE_OK"},
	};

	constexpr std::array engine{
		HelpEntry{FRONT_ROW, "HELP_FRONT_ROW"},
		HelpEntry{BACK_ROW, "HELP_BACK_ROW"},
		HelpEntry{MOVE_FORWARDS, "HELP_MOVE_FORWARDS"},
		HelpEntry{TURN_LEFT, "HELP_TURN_LEFT"},
		HelpEntry{TURN_RIGHT, "HELP_TURN_RIGHT"},
		HelpEntry{MOVE_BACKWARDS, "HELP_MOVE_BACKWARDS"},
		HelpEntry{TURN_AROUND, "HELP_TURN_AROUND"},
		HelpEntry{CAMP, "HELP_CAMP"},
		HelpEntry{GUI_TOGGLE, "HELP_TOGGLE_GUI"},
		HelpEntry{AUTOMAP, "HELP_AUTOMAP"},
		HelpEntry{QUICK_LOAD_SAVE, "HELP_QUICK_LOAD_SAVE"},
		HelpEntry{QUIT, "HELP_SAVE_AND_QUIT"},
	};

	constexpr std::array option{
		HelpEntry{MOUSE, "HELP_SELECT_OPTION"}, HelpEntry{UP, "HELP_PREVIOUS_OPTION"},
		HelpEntry{DOWN, "HELP_NEXT_OPTION"},	HelpEntry{LEFT, "HELP_TOGGLE_OPTION"},
		HelpEntry{RIGHT, "HELP_TOGGLE_OPTION"}, HelpEntry{CONFIRM, "HELP_TOGGLE_OPTION"},
	};

	constexpr std::array allocate_stats{
		HelpEntry{MOUSE, "HELP_SELECT_STAT"},	HelpEntry{UP, "HELP_PREVIOUS_STAT"},
		HelpEntry{DOWN, "HELP_NEXT_STAT"},		HelpEntry{LEFT, "HELP_DECREASE_STAT"},
		HelpEntry{RIGHT, "HELP_INCREASE_STAT"},
	};

	constexpr std::array always_entries{
		HelpEntry{BACK, "HELP_GO_BACK"},
		HelpEntry{HELP, "HELP_SHOW_CONTROLS"},
	};

} // namespace
auto entries(const Enums::Input::Mode mode) -> std::span<const HelpEntry> {

	using enum Enums::Input::Mode;

	switch (mode) {

	case MENU:
		return menu;

	case CONFIRMATION:
		return confirm;

	case NOTIFICATION:
		return notice;

	case ENGINE:
		return engine;

	case OPTION:
		return option;

	case ALLOCATE_STATS:
		return allocate_stats;

	default:
		return {};
	}
}

auto always() -> std::span<const HelpEntry> {

	return always_entries;
}

auto glyphs(const Enums::Controls::Input input) -> std::span<const Enums::Controls::HelpGlyph> {

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
	case NOTICE:
		return notice_glyphs;
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
	case YES_NO:
		return yes_no_glyphs;
	case FRONT_ROW:
		return front_row_glypths;
	case BACK_ROW:
		return back_row_glypths;
	case MOVE_FORWARDS:
		return move_forward_glyphs;
	case MOVE_BACKWARDS:
		return move_backwards_glyphs;
	case TURN_LEFT:
		return turn_left_glyphs;
	case TURN_RIGHT:
		return turn_right_glyphs;
	case TURN_AROUND:
		return turn_around_glyphs;
	case CAMP:
		return camp_glyphs;
	case GUI_TOGGLE:
		return gui_toggle_glyphs;
	case COLOUR_TOGGLE:
		return colour_toggle_glypths;
	case AUTOMAP:
		return automap_glyphs;
	case QUICK_LOAD_SAVE:
		return quick_load_save_glyphs;
	case QUIT:
		return quit_glyphs;
	default:
		return {};
	}
}

} // namespace Sorcery::Help