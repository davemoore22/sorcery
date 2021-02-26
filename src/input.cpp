// Copyright (C) 2020 Dave Moore
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

#include "input.hpp"

// Standard Constructor
Sorcery::Input::Input() {

	_axis_sensitivity_max = 100;
	_axis_sensitivity_min = -100;
}

// Check for a type of input
auto Sorcery::Input::check_for_event(WindowInput input, sf::Event event) -> bool {

	switch (input) {
	case WindowInput::ANYTHING:
		return ((event.type == sf::Event::KeyPressed) || (event.type == sf::Event::MouseButtonPressed) ||
				(event.type == sf::Event::JoystickButtonPressed));
		break;
	case WindowInput::UP:
		return (((event.type == sf::Event::KeyReleased) && (event.key.code == sf::Keyboard::Up)) ||
				((event.type == sf::Event::KeyReleased) && (event.key.code == sf::Keyboard::Numpad8)) ||
				((event.type == sf::Event::JoystickMoved) &&
					(sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::Y) == _axis_sensitivity_min)) ||
				((event.type == sf::Event::JoystickMoved) &&
					(sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::PovY) == _axis_sensitivity_min)) ||
				((event.type == sf::Event::JoystickMoved) &&
					(sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::U) == _axis_sensitivity_min)) ||
				((event.type == sf::Event::MouseWheelScrolled) &&
					(event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) && (event.mouseWheelScroll.delta > 0)));
		break;
	case WindowInput::DOWN:
		return (((event.type == sf::Event::KeyReleased) && (event.key.code == sf::Keyboard::Down)) ||
				((event.type == sf::Event::KeyReleased) && (event.key.code == sf::Keyboard::Numpad2)) ||
				((event.type == sf::Event::JoystickMoved) &&
					(sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::Y) == _axis_sensitivity_max)) ||
				((event.type == sf::Event::JoystickMoved) &&
					(sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::PovY) == _axis_sensitivity_max)) ||
				((event.type == sf::Event::JoystickMoved) &&
					(sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::U) == _axis_sensitivity_max)) ||
				((event.type == sf::Event::MouseWheelScrolled) &&
					(event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) && (event.mouseWheelScroll.delta < 0)));
		break;
	case WindowInput::LEFT:
		return (
			((event.type == sf::Event::KeyReleased) && (event.key.code == sf::Keyboard::Left)) ||
			((event.type == sf::Event::KeyReleased) && (event.key.code == sf::Keyboard::Numpad4)) ||
			((event.type == sf::Event::JoystickMoved) &&
				(sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::X) == _axis_sensitivity_min)) ||
			((event.type == sf::Event::JoystickMoved) &&
				(sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::PovX) == _axis_sensitivity_min)) ||
			((event.type == sf::Event::JoystickMoved) &&
				(sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::V) == _axis_sensitivity_min)) ||
			((event.type == sf::Event::MouseWheelScrolled) &&
				(event.mouseWheelScroll.wheel == sf::Mouse::HorizontalWheel) && (event.mouseWheelScroll.delta < 0)));
		break;
	case WindowInput::RIGHT:
		return (
			((event.type == sf::Event::KeyReleased) && (event.key.code == sf::Keyboard::Right)) ||
			((event.type == sf::Event::KeyReleased) && (event.key.code == sf::Keyboard::Numpad6)) ||
			((event.type == sf::Event::JoystickMoved) &&
				(sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::X) == _axis_sensitivity_max)) ||
			((event.type == sf::Event::JoystickMoved) &&
				(sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::PovX) == _axis_sensitivity_max)) ||
			((event.type == sf::Event::JoystickMoved) &&
				(sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::V) == _axis_sensitivity_max)) ||
			((event.type == sf::Event::MouseWheelScrolled) &&
				(event.mouseWheelScroll.wheel == sf::Mouse::HorizontalWheel) && (event.mouseWheelScroll.delta > 0)));
		break;
	case WindowInput::MOVE:
		return (event.type == sf::Event::MouseMoved);
		break;
	case WindowInput::CONFIRM:
		return (
			((event.type == sf::Event::MouseButtonReleased) && (event.mouseButton.button == sf::Mouse::Button::Left)) ||
			((event.type == sf::Event::KeyReleased) &&
				((event.key.code == sf::Keyboard::Space) || (event.key.code == sf::Keyboard::Enter))) ||
			((event.type == sf::Event::JoystickButtonReleased) &&
				((event.joystickButton.button == 0) || (event.joystickButton.button == 0))));
		break;
	case WindowInput::CANCEL:
		return (((event.type == sf::Event::MouseButtonReleased) && (event.mouseButton.button == sf::Mouse::Right)) ||
				((event.type == sf::Event::KeyReleased) && (event.key.code == sf::Keyboard::Escape)) ||
				((event.type == sf::Event::JoystickButtonReleased) && (event.joystickButton.button == 1)));
		break;
	case WindowInput::YES:
		return ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Y));
		break;
	case WindowInput::NO:
		return ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::N));
		break;
	case WindowInput::PAGE_UP:
		return (((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::PageUp)) ||
				((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Numpad9)));
		break;
	case WindowInput::PAGE_DOWN:
		return (((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::PageDown)) ||
				((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Numpad3)));
		break;
	case WindowInput::HOME:
		return (((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Home)) ||
				((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Numpad7)));
	case WindowInput::END:
		return (((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::End)) ||
				((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Numpad1)));
		break;
	case WindowInput::ALPHANUMERIC:
		// TODO
		break;
	default:
		return false;
	}

	return false;
}
