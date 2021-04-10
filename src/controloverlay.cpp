// Copyright (C) 2021 Dave Moore
//
// This file is part of Sorcery: Dreams of the Mad Overlord.
//
// Sorcery: Dreams of the Mad Overlord is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Sorcery: Dreams of the Mad Overlord is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Sorcery: Dreams of the Mad Overlord.  If not, see <http://www.gnu.org/licenses/>.
//
// If you modify this Program, or any covered work, by linking or combining it
// with the libraries referred to in README (or a modified version of said
// libraries), containing parts covered by the terms of said libraries, the
// licensors of this Program grant you additional permission to convey the
// resulting work.

#include "controloverlay.hpp"

// Standard Constructor
Sorcery::ControlOverlay::ControlOverlay(System *system, Display *display, Component layout)
	: _system{system}, _display{display}, _layout{layout} {

	_sprites.clear();
	_texts.clear();

	_input_mode = WindowInputMode::NONE;
	_control_texture = (*_system->resources).textures[GraphicsTexture::CONTROLS];

	_controls.clear();

	// turn this into a vector of std::pair<string, sprites> for each screen

	/*
	// MOUSE-MOVE
	// UP
	// DOWN
	// LEFT
	// RIGHT
	// CONFIRM
	// Y/N
	// ALPHANUMERIC
	// PAGE_UP
	// PAGE_DOWN
	// HOME
	// END
	// BACK/DELETE (always set)
	// ESCAPE (always set)
	*/

	/* Control Menu Items

			TAB Key or YELLOW Y Buttonm to Activate as long as held

			ANYTHING:			ALL
			UP: 				CURSOR/JOYPAD UP,
			DOWN: 				CURSOR/JOYPAD DOWN,
			LEFT: 				CURSOR/JOYPAD LEFT,
			RIGHT:				CURSOR/JOYPAD RIGHT,
			MOVE:				MOUSE/JOYPAD MOVE,
			CONFIRM:			LEFT-MOUSE, SPACE, ENTER, JOYPAD GREEN A
			CONFIRM-NO-SPACE:	LEFT-MOUSE, ENTER, JOYPAD GREEN A,
			SELECT:				LEFT-MOUSE, JOYPAD GREEN A
			CANCEL:				ESCAPE
			BACK:				RIGHT-MOUSE, JOYPAD RED B
			YES:				KEY Y
			NO:					KEY N
			DELETE:				DELETE/BACKSPACE
			NAVIGATE FILE:		9/PAGE_UP
								3/PAGE_DOWN
								7/HOME,
								1/END
			SPACE:
			ALPHANUMERIC:		KEYS A to Z, and SPACE

		ATTRACT_MODE:

		NAVIGATE_MENU:

			MOUSE MOVE:		Choose Menu Item
			UP: 			Previous Menu Item
			DOWN:			Next Menu Item
			CONFIRM: 		Select Menu Item

		DISPLAY_TEXT_FILE:

			UP: 			Previous Line
			DOWN:			Next Line
			PAGE_UP:		Previous Page
			PAGE_DOWN:		Next Page
			HOME:			Beginning of Text
			END:			End of Text

		GAME_OPTIONS:

			MOUSE MOVE:		Select Option
			UP: 			Previous Option
			DOWN:			Next Option
			CONFIRM: 		Toggle Option On/Off

		COMPENDIUM:

		CONFIRM_QUIT_GAME:
		CONFIRM_STRICT_MODE:
		CONFIRM_LEAVE_GAME:
		SAVE_CHANGES:
		CANCEL_CHANGES:

			MOUSE MOVE:		Change Option
			LEFT:			Change Option
			RIGHT:			Change Option
			CONFIRM:		Choose Option
			Y/N:			Choose Y or N

		INPUT_NAME:

			ALPHANUMERIC:		KEYS A to Z, and SPACE

		ALLOCATE_STATS:

			UP: 			Previous Stat
			DOWN:			Next Stat
			LEFT: 			Decrease Stat
			RIGHT:			Increase Stat

		CHOOSE_METHOD:

			LEFT: 			Change Method
			RIGHT:			Change Method
			CONFIRM:		Choose Method

		CHOOSE_PORTRAIT:

			LEFT: 			Previous Portrait
			RIGHT:			Next Portrait
			CONFIRM:		Choose Portrait

		REVIEW_AND_CONFIRM:

			LEFT: 			Previous Section
			RIGHT:			Next Section
			CONFIRM:		Confirm Character

		Always (add to bottom of frame):

			BACK/DELETE: 	Cancel and Return to Previous Screen
			ESCAPE: 		Quit to Main Menu

		*/
}

// Need a texture holding all the keys
auto Sorcery::ControlOverlay::set_input_mode(WindowInputMode input_mode) -> void {

	_input_mode = input_mode;

	switch (input_mode) {
	case WindowInputMode::ALLOCATE_STATS:
		break;
	case WindowInputMode::ATTRACT_MODE:
		break;
	case WindowInputMode::CHOOSE_METHOD:
		break;
	case WindowInputMode::CHOOSE_PORTRAIT:
		break;
	case WindowInputMode::COMPENDIUM:
		break;
	case WindowInputMode::CONFIRM_LEAVE_GAME:
	case WindowInputMode::CONFIRM_QUIT_GAME:
	case WindowInputMode::CONFIRM_STRICT_MODE:
	case WindowInputMode::SAVE_CHANGES:
	case WindowInputMode::CANCEL_CHANGES:
		break;
	case WindowInputMode::DISPLAY_TEXT_FILE:
		break;
	case WindowInputMode::GAME_OPTIONS:
		break;
	case WindowInputMode::INPUT_NAME:
		break;
	case WindowInputMode::NAVIGATE_MENU:
		break;
	case WindowInputMode::REVIEW_AND_CONFIRM:
		break;
	default:
		break;
	};
}

auto Sorcery::ControlOverlay::draw(sf::RenderTarget &target, sf::RenderStates states) const
	-> void {

	states.transform *= getTransform();

	for (const auto &sprite : _sprites)
		target.draw(sprite, states);

	for (const auto &text : _texts)
		target.draw(text, states);
}