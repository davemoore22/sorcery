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
}

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

auto Sorcery::ControlOverlay::set_input_mode(WindowInputMode input_mode) -> void {

	_input_mode = input_mode;
	_controls.clear();

	// Populate the Available Control Methods
	switch (_input_mode) {
	case WindowInputMode::ALLOCATE_STATS:
		_controls.emplace_back(std::make_pair((*_display->string)["CONTROL_MOUSE_ALLOCATE_STATS"],
			_get_control_gfx(WindowInputCategory::MOUSE_MOVE)));
		_controls.emplace_back(std::make_pair((*_display->string)["CONTROL_UP_ALLOCATE_STATS"],
			_get_control_gfx(WindowInputCategory::UP)));
		_controls.emplace_back(std::make_pair((*_display->string)["CONTROL_DOWN_ALLOCATE_STATS"],
			_get_control_gfx(WindowInputCategory::DOWN)));
		_controls.emplace_back(std::make_pair((*_display->string)["CONTROL_LEFT_ALLOCATE_STATS"],
			_get_control_gfx(WindowInputCategory::LEFT)));
		_controls.emplace_back(std::make_pair((*_display->string)["CONTROL_RIGHT_ALLOCATE_STATS"],
			_get_control_gfx(WindowInputCategory::RIGHT)));
		break;
	case WindowInputMode::ATTRACT_MODE:
		break;
	case WindowInputMode::CHOOSE_METHOD:
		_controls.emplace_back(std::make_pair((*_display->string)["CONTROL_MOUSE_METHOD"],
			_get_control_gfx(WindowInputCategory::MOUSE_MOVE)));
		_controls.emplace_back(std::make_pair((*_display->string)["CONTROL_LEFT_METHOD"],
			_get_control_gfx(WindowInputCategory::LEFT)));
		_controls.emplace_back(std::make_pair((*_display->string)["CONTROL_RIGHT_METHOD"],
			_get_control_gfx(WindowInputCategory::RIGHT)));
		_controls.emplace_back(std::make_pair((*_display->string)["CONTROL_CONFIRM_METHOD"],
			_get_control_gfx(WindowInputCategory::CONFIRM)));
		break;
	case WindowInputMode::CHOOSE_PORTRAIT:
		_controls.emplace_back(std::make_pair((*_display->string)["CONTROL_LEFT_CHOOSE_PORTRAIT"],
			_get_control_gfx(WindowInputCategory::LEFT)));
		_controls.emplace_back(std::make_pair((*_display->string)["CONTROL_RIGHT_CHOOSE_PORTRAIT"],
			_get_control_gfx(WindowInputCategory::RIGHT)));
		_controls.emplace_back(
			std::make_pair((*_display->string)["CONTROL_CONFIRM_CHOOSE_PORTRAIT"],
				_get_control_gfx(WindowInputCategory::CONFIRM)));
		break;
	case WindowInputMode::COMPENDIUM:
		break;
	case WindowInputMode::CONFIRM_LEAVE_GAME:
	case WindowInputMode::CONFIRM_QUIT_GAME:
	case WindowInputMode::CONFIRM_STRICT_MODE:
	case WindowInputMode::SAVE_CHANGES:
	case WindowInputMode::CANCEL_CHANGES:
		_controls.emplace_back(std::make_pair((*_display->string)["CONTROL_MOUSE_CONFIRMATION"],
			_get_control_gfx(WindowInputCategory::MOUSE_MOVE)));
		_controls.emplace_back(std::make_pair((*_display->string)["CONTROL_LEFT_CONFIRMATION"],
			_get_control_gfx(WindowInputCategory::LEFT)));
		_controls.emplace_back(std::make_pair((*_display->string)["CONTROL_RIGHT_CONFIRMATION"],
			_get_control_gfx(WindowInputCategory::RIGHT)));
		_controls.emplace_back(std::make_pair((*_display->string)["CONTROL_CONFIRM_CONFIRMATION"],
			_get_control_gfx(WindowInputCategory::CONFIRM)));
		_controls.emplace_back(std::make_pair((*_display->string)["CONTROL_Y_N_CONFIRMATION"],
			_get_control_gfx(WindowInputCategory::YES_NO)));
		break;
	case WindowInputMode::DISPLAY_TEXT_FILE:
		_controls.emplace_back(std::make_pair(
			(*_display->string)["CONTROL_UP_FILE"], _get_control_gfx(WindowInputCategory::UP)));
		_controls.emplace_back(std::make_pair(
			(*_display->string)["CONTROL_DOWN_FILE"], _get_control_gfx(WindowInputCategory::DOWN)));
		_controls.emplace_back(std::make_pair((*_display->string)["CONTROL_PAGE_UP_FILE"],
			_get_control_gfx(WindowInputCategory::PAGE_UP)));
		_controls.emplace_back(std::make_pair((*_display->string)["CONTROL_PAGE_DOWN_FILE"],
			_get_control_gfx(WindowInputCategory::PAGE_DOWN)));
		_controls.emplace_back(std::make_pair(
			(*_display->string)["CONTROL_HOME_FILE"], _get_control_gfx(WindowInputCategory::HOME)));
		_controls.emplace_back(std::make_pair(
			(*_display->string)["CONTROL_END_FILE"], _get_control_gfx(WindowInputCategory::END)));
		break;
	case WindowInputMode::GAME_OPTIONS:
		_controls.emplace_back(std::make_pair((*_display->string)["CONTROL_MOUSE_OPTION"],
			_get_control_gfx(WindowInputCategory::MOUSE_MOVE)));
		_controls.emplace_back(std::make_pair(
			(*_display->string)["CONTROL_UP_OPTION"], _get_control_gfx(WindowInputCategory::UP)));
		_controls.emplace_back(std::make_pair((*_display->string)["CONTROL_DOWN_OPTION"],
			_get_control_gfx(WindowInputCategory::DOWN)));
		_controls.emplace_back(std::make_pair((*_display->string)["CONTROL_LEFT_OPTION"],
			_get_control_gfx(WindowInputCategory::LEFT)));
		_controls.emplace_back(std::make_pair((*_display->string)["CONTROL_RIGHT_OPTION"],
			_get_control_gfx(WindowInputCategory::RIGHT)));
		_controls.emplace_back(std::make_pair((*_display->string)["CONTROL_CONFIRM_OPTION"],
			_get_control_gfx(WindowInputCategory::CONFIRM)));
		break;
	case WindowInputMode::INPUT_NAME:
		_controls.emplace_back(std::make_pair((*_display->string)["CONTROL_MOUSE_INPUT_NAME"],
			_get_control_gfx(WindowInputCategory::MOUSE_MOVE)));
		_controls.emplace_back(std::make_pair((*_display->string)["CONTROL_UP_INPUT_NAME"],
			_get_control_gfx(WindowInputCategory::UP)));
		_controls.emplace_back(std::make_pair((*_display->string)["CONTROL_DOWN_INPUT_NAME"],
			_get_control_gfx(WindowInputCategory::DOWN)));
		_controls.emplace_back(std::make_pair((*_display->string)["CONTROL_LEFT_INPUT_NAME"],
			_get_control_gfx(WindowInputCategory::LEFT)));
		_controls.emplace_back(std::make_pair((*_display->string)["CONTROL_RIGHT_INPUT_NAME"],
			_get_control_gfx(WindowInputCategory::RIGHT)));
		_controls.emplace_back(std::make_pair((*_display->string)["CONTROL_CONFIRM_INPUT_NAME"],
			_get_control_gfx(WindowInputCategory::CONFIRM)));
		_controls.emplace_back(
			std::make_pair((*_display->string)["CONTROL_ALPHANUMERIC_INPUT_NAME"],
				_get_control_gfx(WindowInputCategory::ALPHANUMERIC)));
		break;
	case WindowInputMode::NAVIGATE_MENU:
		_controls.emplace_back(std::make_pair((*_display->string)["CONTROL_MOUSE_MOVE_MENU"],
			_get_control_gfx(WindowInputCategory::MOUSE_MOVE)));
		_controls.emplace_back(std::make_pair(
			(*_display->string)["CONTROL_UP_MENU"], _get_control_gfx(WindowInputCategory::UP)));
		_controls.emplace_back(std::make_pair(
			(*_display->string)["CONTROL_DOWN_MENU"], _get_control_gfx(WindowInputCategory::DOWN)));
		_controls.emplace_back(std::make_pair((*_display->string)["CONTROL_CONFIRM_MENU"],
			_get_control_gfx(WindowInputCategory::CONFIRM)));
		break;
	case WindowInputMode::REVIEW_AND_CONFIRM:
		_controls.emplace_back(
			std::make_pair((*_display->string)["CONTROL_LEFT_REVIEW_AND_CONFIRM"],
				_get_control_gfx(WindowInputCategory::LEFT)));
		_controls.emplace_back(
			std::make_pair((*_display->string)["CONTROL_RIGHT_REVIEW_AND_CONFIRM"],
				_get_control_gfx(WindowInputCategory::RIGHT)));
		_controls.emplace_back(
			std::make_pair((*_display->string)["CONTROL_CONFIRM_REVIEW_AND_CONFIRM"],
				_get_control_gfx(WindowInputCategory::CONFIRM)));
		break;
	default:
		break;
	};
	_controls.emplace_back(std::make_pair((*_display->string)["CONTROL_DELETE_BACK_ALWAYS"],
		_get_control_gfx(WindowInputCategory::BACK_DELETE_CANCEL)));
	_controls.emplace_back(std::make_pair((*_display->string)["CONTROL_ESCAPE_ALWAYS"],
		_get_control_gfx(WindowInputCategory::ESCAPE)));

	// Now generate the various Components
}

auto Sorcery::ControlOverlay::_get_control_gfx(WindowInputCategory input) -> sf::Sprite {

	constexpr unsigned int row_height{100};
	const unsigned int row_width{_control_texture.getSize().x};
	const unsigned int y{static_cast<unsigned int>(input) * row_height};
	auto const crect = sf::IntRect(0, row_width, y, row_height);

	sf::Sprite control(_control_texture);
	control.setTextureRect(crect);

	return control;
}

auto Sorcery::ControlOverlay::draw(sf::RenderTarget &target, sf::RenderStates states) const
	-> void {

	states.transform *= getTransform();

	for (const auto &sprite : _sprites)
		target.draw(sprite, states);

	for (const auto &text : _texts)
		target.draw(text, states);
}