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

#include <array>

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

	constexpr std::array menu{
		HelpEntry{CONFIRM, "HELP_SELECT_ITEM"},
		HelpEntry{UP, "HELP_PREVIOUS_ITEM"},
		HelpEntry{DOWN, "HELP_NEXT_ITEM"},
	};

	constexpr std::array confirm{
		HelpEntry{CONFIRM, "HELP_CONFIRM_CONFIRMATION"},
		HelpEntry{YES_NO, "HELP_Y_N_CONFIRMATION"},
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

	default:
		return {};
	}
}

} // namespace Sorcery::Help