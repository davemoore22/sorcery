// Copyright (C) 2024 Dave Moore
//
// This file is part of Sorcery: Shadows under Llylgamyn.
//
// Sorcery: Shadows under Llylgamyn is free software: you can redistribute
// it and/or modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation, either version 2 of the License,
// or (at your option) any later version.
//
// Sorcery: Shadows under Llylgamyn is distributed in the hope that it wil
// be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
// Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Sorcery: Shadows under Llylgamyn.  If not,
// see <http://www.gnu.org/licenses/>.
//
// If you modify this program, or any covered work, by linking or combining
// it with the libraries referred to in README (or a modified version of
// said libraries), containing parts covered by the terms of said libraries,
// the licensors of this program grant you additional permission to convey
// the resulting work.

#include "core/input.hpp"

// Standard Constructor
Sorcery::Input::Input() {

	_jp_max = 100;
	_jp_min = -100;
}

// Check for a type of input
auto Sorcery::Input::check(CIN input, sf::Event event) -> bool {

	switch (input) {
	case CIN::SHOW_HIDE_CONSOLE:
		return ((event.type == sf::Event::KeyPressed) &&
				(event.key.code == sf::Keyboard::F12));
		break;
	case CIN::SHOW_CONTROLS:
		return (((event.type == sf::Event::KeyPressed) &&
					(event.key.code == sf::Keyboard::F1)) ||
				((event.type == sf::Event::MouseButtonPressed) &&
					(event.mouseButton.button == sf::Mouse::Middle)) ||
				((event.type == sf::Event::JoystickButtonPressed) &&
					(event.joystickButton.button == 3)));
		break;
	case CIN::ANYTHING:
		return ((event.type == sf::Event::KeyPressed) ||
				(event.type == sf::Event::MouseButtonPressed) ||
				(event.type == sf::Event::JoystickButtonPressed));
		break;
	case CIN::UP:
		return (
			((event.type == sf::Event::KeyReleased) &&
				(event.key.code == sf::Keyboard::Up)) ||
			((event.type == sf::Event::KeyReleased) &&
				(event.key.code == sf::Keyboard::Numpad8)) ||
			((event.type == sf::Event::JoystickMoved) &&
				(sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::Y) ==
					_jp_min)) ||
			((event.type == sf::Event::JoystickMoved) &&
				(sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::PovY) ==
					_jp_min)) ||
			((event.type == sf::Event::JoystickMoved) &&
				(sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::U) ==
					_jp_min)) ||
			((event.type == sf::Event::MouseWheelScrolled) &&
				(event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) &&
				(event.mouseWheelScroll.delta > 0)));
		break;
	case CIN::DOWN:
		return (
			((event.type == sf::Event::KeyReleased) &&
				(event.key.code == sf::Keyboard::Down)) ||
			((event.type == sf::Event::KeyReleased) &&
				(event.key.code == sf::Keyboard::Numpad2)) ||
			((event.type == sf::Event::JoystickMoved) &&
				(sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::Y) ==
					_jp_max)) ||
			((event.type == sf::Event::JoystickMoved) &&
				(sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::PovY) ==
					_jp_max)) ||
			((event.type == sf::Event::JoystickMoved) &&
				(sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::U) ==
					_jp_max)) ||
			((event.type == sf::Event::MouseWheelScrolled) &&
				(event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) &&
				(event.mouseWheelScroll.delta < 0)));
		break;
	case CIN::LEFT:
		return (
			((event.type == sf::Event::KeyReleased) &&
				(event.key.code == sf::Keyboard::Left)) ||
			((event.type == sf::Event::KeyReleased) &&
				(event.key.code == sf::Keyboard::Numpad4)) ||
			((event.type == sf::Event::JoystickMoved) &&
				(sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::X) ==
					_jp_min)) ||
			((event.type == sf::Event::JoystickMoved) &&
				(sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::PovX) ==
					_jp_min)) ||
			((event.type == sf::Event::JoystickMoved) &&
				(sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::V) ==
					_jp_min)) ||
			((event.type == sf::Event::MouseWheelScrolled) &&
				(event.mouseWheelScroll.wheel == sf::Mouse::HorizontalWheel) &&
				(event.mouseWheelScroll.delta < 0)));
		break;
	case CIN::RIGHT:
		return (
			((event.type == sf::Event::KeyReleased) &&
				(event.key.code == sf::Keyboard::Right)) ||
			((event.type == sf::Event::KeyReleased) &&
				(event.key.code == sf::Keyboard::Numpad6)) ||
			((event.type == sf::Event::JoystickMoved) &&
				(sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::X) ==
					_jp_max)) ||
			((event.type == sf::Event::JoystickMoved) &&
				(sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::PovX) ==
					_jp_max)) ||
			((event.type == sf::Event::JoystickMoved) &&
				(sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::V) ==
					_jp_max)) ||
			((event.type == sf::Event::MouseWheelScrolled) &&
				(event.mouseWheelScroll.wheel == sf::Mouse::HorizontalWheel) &&
				(event.mouseWheelScroll.delta > 0)));
		break;
	case CIN::MOVE:
		return (event.type == sf::Event::MouseMoved);
		break;
	case CIN::CONFIRM:
		return (((event.type == sf::Event::MouseButtonReleased) &&
					(event.mouseButton.button == sf::Mouse::Button::Left)) ||
				((event.type == sf::Event::KeyReleased) &&
					((event.key.code == sf::Keyboard::Space) ||
						(event.key.code == sf::Keyboard::Enter))) ||
				((event.type == sf::Event::JoystickButtonReleased) &&
					(event.joystickButton.button == 0)));
		break;
	case CIN::CONFIRM_NO_SPACE:
		return (((event.type == sf::Event::MouseButtonReleased) &&
					(event.mouseButton.button == sf::Mouse::Button::Left)) ||
				((event.type == sf::Event::KeyReleased) &&
					(event.key.code == sf::Keyboard::Enter)) ||
				((event.type == sf::Event::JoystickButtonReleased) &&
					(event.joystickButton.button == 0)));
		break;
	case CIN::SELECT:
		return (((event.type == sf::Event::MouseButtonReleased) &&
					(event.mouseButton.button == sf::Mouse::Button::Left)) ||
				((event.type == sf::Event::JoystickButtonReleased) &&
					(event.joystickButton.button == 0)));
		break;
	case CIN::CANCEL:
		return (((event.type == sf::Event::KeyReleased) &&
					(event.key.code == sf::Keyboard::Escape)) ||
				((event.type == sf::Event::JoystickButtonReleased) &&
					(event.joystickButton.button == 2)));
		break;
	case CIN::BACK:
		return (((event.type == sf::Event::MouseButtonReleased) &&
					(event.mouseButton.button == sf::Mouse::Right)) ||
				((event.type == sf::Event::JoystickButtonReleased) &&
					(event.joystickButton.button == 1)));
		break;
	case CIN::YES:
		return ((event.type == sf::Event::KeyPressed) &&
				(event.key.code == sf::Keyboard::Y));
		break;
	case CIN::NO:
		return ((event.type == sf::Event::KeyPressed) &&
				(event.key.code == sf::Keyboard::N));
		break;
	case CIN::PAGE_UP:
		return (((event.type == sf::Event::KeyPressed) &&
					(event.key.code == sf::Keyboard::PageUp)) ||
				((event.type == sf::Event::KeyPressed) &&
					(event.key.code == sf::Keyboard::Numpad9)));
		break;
	case CIN::PAGE_DOWN:
		return (((event.type == sf::Event::KeyPressed) &&
					(event.key.code == sf::Keyboard::PageDown)) ||
				((event.type == sf::Event::KeyPressed) &&
					(event.key.code == sf::Keyboard::Numpad3)));
		break;
	case CIN::HOME:
		return (((event.type == sf::Event::KeyPressed) &&
					(event.key.code == sf::Keyboard::Home)) ||
				((event.type == sf::Event::KeyPressed) &&
					(event.key.code == sf::Keyboard::Numpad7)));
	case CIN::END:
		return (((event.type == sf::Event::KeyPressed) &&
					(event.key.code == sf::Keyboard::End)) ||
				((event.type == sf::Event::KeyPressed) &&
					(event.key.code == sf::Keyboard::Numpad1)));
		break;
	case CIN::ALPHANUMERIC:
		return (
			(event.type == sf::Event::TextEntered) &&
			(((event.text.unicode >= 65) && (event.text.unicode <= 90)) ||
				((event.text.unicode >= 97) && (event.text.unicode <= 122))));
		break;
	case CIN::DELETE:
		return ((event.type == sf::Event::KeyPressed) &&
				((event.key.code == sf::Keyboard::Delete) ||
					(event.key.code == sf::Keyboard::BackSpace)));
		break;
	case CIN::SPACE:
		return ((event.type == sf::Event::TextEntered) &&
				(event.text.unicode == 32));
		break;
	case CIN::MAZE_TURN_AROUND:
		return ((event.type == sf::Event::KeyPressed) &&
				(event.key.code == sf::Keyboard::X));
		break;
	case CIN::MAZE_LEFT:
		return (((event.type == sf::Event::KeyPressed) &&
					(event.key.code == sf::Keyboard::A)) ||
				((event.type == sf::Event::KeyPressed) &&
					(event.key.code == sf::Keyboard::L)) ||
				((event.type == sf::Event::KeyReleased) &&
					(event.key.code == sf::Keyboard::Left)));
		break;
	case CIN::MAZE_RIGHT:
		return (((event.type == sf::Event::KeyPressed) &&
					(event.key.code == sf::Keyboard::D)) ||
				((event.type == sf::Event::KeyPressed) &&
					(event.key.code == sf::Keyboard::R)) ||
				((event.type == sf::Event::KeyReleased) &&
					(event.key.code == sf::Keyboard::Right)));
		break;
	case CIN::MAZE_FORWARD:
		return (((event.type == sf::Event::KeyPressed) &&
					(event.key.code == sf::Keyboard::W)) ||
				((event.type == sf::Event::KeyPressed) &&
					(event.key.code == sf::Keyboard::F)) ||
				((event.type == sf::Event::KeyReleased) &&
					(event.key.code == sf::Keyboard::Up)));
		break;
	case CIN::MAZE_BACKWARD:
		return ((event.type == sf::Event::KeyPressed) &&
				(event.key.code == sf::Keyboard::Down));
		break;
	case CIN::MAZE_CAMP:
		return ((event.type == sf::Event::KeyPressed) &&
				(event.key.code == sf::Keyboard::C));
		break;
	case CIN::MAZE_CHARACTER_1:
		return ((event.type == sf::Event::KeyPressed) &&
				(event.key.code == sf::Keyboard::Num1));
		break;
	case CIN::MAZE_CHARACTER_2:
		return ((event.type == sf::Event::KeyPressed) &&
				(event.key.code == sf::Keyboard::Num2));
		break;
	case CIN::MAZE_CHARACTER_3:
		return ((event.type == sf::Event::KeyPressed) &&
				(event.key.code == sf::Keyboard::Num3));
		break;
	case CIN::MAZE_CHARACTER_4:
		return ((event.type == sf::Event::KeyPressed) &&
				(event.key.code == sf::Keyboard::Num4));
		break;
	case CIN::MAZE_CHARACTER_5:
		return ((event.type == sf::Event::KeyPressed) &&
				(event.key.code == sf::Keyboard::Num5));
		break;
	case CIN::MAZE_CHARACTER_6:
		return ((event.type == sf::Event::KeyPressed) &&
				(event.key.code == sf::Keyboard::Num6));
		break;
	case CIN::MAZE_QUIT:
		return ((event.type == sf::Event::KeyPressed) &&
				(event.key.code == sf::Keyboard::Q));
		break;
	case CIN::MAZE_ACTION:
		return ((event.type == sf::Event::KeyPressed) &&
				(event.key.code == sf::Keyboard::T));
		break;
	case CIN::MAZE_SEARCH:
		return ((event.type == sf::Event::KeyPressed) &&
				(event.key.code == sf::Keyboard::Z));
		break;
	case CIN::MAZE_INSPECT:
		return ((event.type == sf::Event::KeyPressed) &&
				(event.key.code == sf::Keyboard::I));
		break;
	case CIN::MAZE_GUI_TOGGLE:
		return ((event.type == sf::Event::KeyPressed) &&
				(event.key.code == sf::Keyboard::O));
		break;
	case CIN::MAZE_STATUSBAR_TOGGLE:
		return ((event.type == sf::Event::KeyPressed) &&
				(event.key.code == sf::Keyboard::S));
		break;
	case CIN::MAZE_SHOW_MAP:
		return ((event.type == sf::Event::KeyPressed) &&
				(event.key.code == sf::Keyboard::M));
		break;
	default:
		return false;
	}

	return false;
}
