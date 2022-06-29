// Copyright (C) 2021 Dave Moore
//
// This file is part of Sorcery: Shadows under Llylgamyn.
//
// Sorcery: Shadows under Llylgamyn is free software: you can redistribute it
// and/or modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation, either version 2 of the License,
// or (at your option) any later version.
//
// Sorcery: Shadows under Llylgamyn is distributed in the hope that it will
// be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Sorcery: Shadows under Llylgamyn.  If not, see
// <http://www.gnu.org/licenses/>.
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

auto Sorcery::ControlOverlay::set_input_mode(WindowInputMode input_mode) -> void {

	valid = false;
	_input_mode = input_mode;
	_controls.clear();
	_sprites.clear();
	_texts.clear();

	// Populate the Available Control Methods
	switch (_input_mode) {
	case WindowInputMode::ALLOCATE_STATS:
		_controls.emplace_back(std::make_pair(
			(*_display->string)["CONTROL_MOUSE_ALLOCATE_STATS"], _get_control_gfx(WindowInputCategory::MOUSE_MOVE)));
		_controls.emplace_back(std::make_pair(
			(*_display->string)["CONTROL_UP_ALLOCATE_STATS"], _get_control_gfx(WindowInputCategory::UP)));
		_controls.emplace_back(std::make_pair(
			(*_display->string)["CONTROL_DOWN_ALLOCATE_STATS"], _get_control_gfx(WindowInputCategory::DOWN)));
		_controls.emplace_back(std::make_pair(
			(*_display->string)["CONTROL_LEFT_ALLOCATE_STATS"], _get_control_gfx(WindowInputCategory::LEFT)));
		_controls.emplace_back(std::make_pair(
			(*_display->string)["CONTROL_RIGHT_ALLOCATE_STATS"], _get_control_gfx(WindowInputCategory::RIGHT)));
		break;
	case WindowInputMode::ATTRACT_MODE:
		valid = false;
		return;
		break;
	case WindowInputMode::CHOOSE_METHOD:
		_controls.emplace_back(std::make_pair(
			(*_display->string)["CONTROL_MOUSE_METHOD"], _get_control_gfx(WindowInputCategory::MOUSE_MOVE)));
		_controls.emplace_back(
			std::make_pair((*_display->string)["CONTROL_LEFT_METHOD"], _get_control_gfx(WindowInputCategory::LEFT)));
		_controls.emplace_back(
			std::make_pair((*_display->string)["CONTROL_RIGHT_METHOD"], _get_control_gfx(WindowInputCategory::RIGHT)));
		_controls.emplace_back(std::make_pair(
			(*_display->string)["CONTROL_CONFIRM_METHOD"], _get_control_gfx(WindowInputCategory::CONFIRM)));
		break;
	case WindowInputMode::CHOOSE_PORTRAIT:
		_controls.emplace_back(std::make_pair(
			(*_display->string)["CONTROL_LEFT_CHOOSE_PORTRAIT"], _get_control_gfx(WindowInputCategory::LEFT)));
		_controls.emplace_back(std::make_pair(
			(*_display->string)["CONTROL_RIGHT_CHOOSE_PORTRAIT"], _get_control_gfx(WindowInputCategory::RIGHT)));
		_controls.emplace_back(std::make_pair(
			(*_display->string)["CONTROL_CONFIRM_CHOOSE_PORTRAIT"], _get_control_gfx(WindowInputCategory::CONFIRM)));
		break;
	case WindowInputMode::COMPENDIUM:
		break;
	case WindowInputMode::CONFIRM_LEAVE_GAME:
	case WindowInputMode::CONFIRM_QUIT_GAME:
	case WindowInputMode::CONFIRM_STRICT_MODE:
	case WindowInputMode::SAVE_CHANGES:
	case WindowInputMode::CANCEL_CHANGES:
		_controls.emplace_back(std::make_pair(
			(*_display->string)["CONTROL_MOUSE_CONFIRMATION"], _get_control_gfx(WindowInputCategory::MOUSE_MOVE)));
		_controls.emplace_back(std::make_pair(
			(*_display->string)["CONTROL_LEFT_CONFIRMATION"], _get_control_gfx(WindowInputCategory::LEFT)));
		_controls.emplace_back(std::make_pair(
			(*_display->string)["CONTROL_RIGHT_CONFIRMATION"], _get_control_gfx(WindowInputCategory::RIGHT)));
		_controls.emplace_back(std::make_pair(
			(*_display->string)["CONTROL_CONFIRM_CONFIRMATION"], _get_control_gfx(WindowInputCategory::CONFIRM)));
		_controls.emplace_back(std::make_pair(
			(*_display->string)["CONTROL_Y_N_CONFIRMATION"], _get_control_gfx(WindowInputCategory::YES_NO)));
		break;
	case WindowInputMode::DISPLAY_TEXT_FILE:
		_controls.emplace_back(
			std::make_pair((*_display->string)["CONTROL_UP_FILE"], _get_control_gfx(WindowInputCategory::UP)));
		_controls.emplace_back(
			std::make_pair((*_display->string)["CONTROL_DOWN_FILE"], _get_control_gfx(WindowInputCategory::DOWN)));
		_controls.emplace_back(std::make_pair(
			(*_display->string)["CONTROL_PAGE_UP_FILE"], _get_control_gfx(WindowInputCategory::PAGE_UP)));
		_controls.emplace_back(std::make_pair(
			(*_display->string)["CONTROL_PAGE_DOWN_FILE"], _get_control_gfx(WindowInputCategory::PAGE_DOWN)));
		_controls.emplace_back(
			std::make_pair((*_display->string)["CONTROL_HOME_FILE"], _get_control_gfx(WindowInputCategory::HOME)));
		_controls.emplace_back(
			std::make_pair((*_display->string)["CONTROL_END_FILE"], _get_control_gfx(WindowInputCategory::END)));
		break;
	case WindowInputMode::GAME_OPTIONS:
		_controls.emplace_back(std::make_pair(
			(*_display->string)["CONTROL_MOUSE_OPTION"], _get_control_gfx(WindowInputCategory::MOUSE_MOVE)));
		_controls.emplace_back(
			std::make_pair((*_display->string)["CONTROL_UP_OPTION"], _get_control_gfx(WindowInputCategory::UP)));
		_controls.emplace_back(
			std::make_pair((*_display->string)["CONTROL_DOWN_OPTION"], _get_control_gfx(WindowInputCategory::DOWN)));
		_controls.emplace_back(
			std::make_pair((*_display->string)["CONTROL_LEFT_OPTION"], _get_control_gfx(WindowInputCategory::LEFT)));
		_controls.emplace_back(
			std::make_pair((*_display->string)["CONTROL_RIGHT_OPTION"], _get_control_gfx(WindowInputCategory::RIGHT)));
		_controls.emplace_back(std::make_pair(
			(*_display->string)["CONTROL_CONFIRM_OPTION"], _get_control_gfx(WindowInputCategory::CONFIRM)));
		break;
	case WindowInputMode::INPUT_NAME:
		_controls.emplace_back(std::make_pair(
			(*_display->string)["CONTROL_MOUSE_INPUT_NAME"], _get_control_gfx(WindowInputCategory::MOUSE_MOVE)));
		_controls.emplace_back(
			std::make_pair((*_display->string)["CONTROL_UP_INPUT_NAME"], _get_control_gfx(WindowInputCategory::UP)));
		_controls.emplace_back(std::make_pair(
			(*_display->string)["CONTROL_DOWN_INPUT_NAME"], _get_control_gfx(WindowInputCategory::DOWN)));
		_controls.emplace_back(std::make_pair(
			(*_display->string)["CONTROL_LEFT_INPUT_NAME"], _get_control_gfx(WindowInputCategory::LEFT)));
		_controls.emplace_back(std::make_pair(
			(*_display->string)["CONTROL_RIGHT_INPUT_NAME"], _get_control_gfx(WindowInputCategory::RIGHT)));
		_controls.emplace_back(std::make_pair(
			(*_display->string)["CONTROL_CONFIRM_INPUT_NAME"], _get_control_gfx(WindowInputCategory::CONFIRM)));
		_controls.emplace_back(std::make_pair((*_display->string)["CONTROL_ALPHANUMERIC_INPUT_NAME"],
			_get_control_gfx(WindowInputCategory::ALPHANUMERIC)));
		break;
	case WindowInputMode::NAVIGATE_MENU:
		_controls.emplace_back(std::make_pair(
			(*_display->string)["CONTROL_MOUSE_MOVE_MENU"], _get_control_gfx(WindowInputCategory::MOUSE_MOVE)));
		_controls.emplace_back(
			std::make_pair((*_display->string)["CONTROL_UP_MENU"], _get_control_gfx(WindowInputCategory::UP)));
		_controls.emplace_back(
			std::make_pair((*_display->string)["CONTROL_DOWN_MENU"], _get_control_gfx(WindowInputCategory::DOWN)));
		_controls.emplace_back(std::make_pair(
			(*_display->string)["CONTROL_CONFIRM_MENU"], _get_control_gfx(WindowInputCategory::CONFIRM)));
		break;
	case WindowInputMode::BROWSE_CHARACTER:
		_controls.emplace_back(std::make_pair(
			(*_display->string)["CONTROL_UP_BROWSER_CHARACTER"], _get_control_gfx(WindowInputCategory::UP)));
		_controls.emplace_back(std::make_pair(
			(*_display->string)["CONTROL_DOWN_BROWSER_CHARACTER"], _get_control_gfx(WindowInputCategory::DOWN)));
		_controls.emplace_back(std::make_pair(
			(*_display->string)["CONTROL_LEFT_BROWSER_CHARACTER"], _get_control_gfx(WindowInputCategory::LEFT)));
		_controls.emplace_back(std::make_pair(
			(*_display->string)["CONTROL_RIGHT_BROWSER_CHARACTER"], _get_control_gfx(WindowInputCategory::RIGHT)));
		_controls.emplace_back(std::make_pair(
			(*_display->string)["CONTROL_CONFIRM_BROWSER_CHARACTER"], _get_control_gfx(WindowInputCategory::CONFIRM)));
		break;
	case WindowInputMode::REVIEW_AND_CONFIRM:
		_controls.emplace_back(std::make_pair(
			(*_display->string)["CONTROL_LEFT_REVIEW_AND_CONFIRM"], _get_control_gfx(WindowInputCategory::LEFT)));
		_controls.emplace_back(std::make_pair(
			(*_display->string)["CONTROL_RIGHT_REVIEW_AND_CONFIRM"], _get_control_gfx(WindowInputCategory::RIGHT)));
		_controls.emplace_back(std::make_pair(
			(*_display->string)["CONTROL_CONFIRM_REVIEW_AND_CONFIRM"], _get_control_gfx(WindowInputCategory::CONFIRM)));
		break;
	case WindowInputMode::IN_GAME:
		_controls.emplace_back(
			std::make_pair((*_display->string)["CONTROL_LEFT_IN_GAME"], _get_control_gfx(WindowInputCategory::LEFT)));
		_controls.emplace_back(
			std::make_pair((*_display->string)["CONTROL_RIGHT_IN_GAME"], _get_control_gfx(WindowInputCategory::RIGHT)));
		_controls.emplace_back(
			std::make_pair((*_display->string)["CONTROL_UP_IN_GAME"], _get_control_gfx(WindowInputCategory::UP)));
		_controls.emplace_back(
			std::make_pair((*_display->string)["CONTROL_DOWN_IN_GAME"], _get_control_gfx(WindowInputCategory::DOWN)));
		_controls.emplace_back(std::make_pair(
			(*_display->string)["CONTROL_CANCEL_IN_GAME"], _get_control_gfx(WindowInputCategory::BACK_DELETE_CANCEL)));
		_controls.emplace_back(std::make_pair(
			(*_display->string)["CONTROL_CONFIRM_IN_GAME"], _get_control_gfx(WindowInputCategory::CONFIRM)));
		break;
	default:
		break;
	};

	_controls.emplace_back(std::make_pair(
		(*_display->string)["CONTROL_DELETE_BACK_ALWAYS"], _get_control_gfx(WindowInputCategory::BACK_DELETE_CANCEL)));
	_controls.emplace_back(
		std::make_pair((*_display->string)["CONTROL_ESCAPE_ALWAYS"], _get_control_gfx(WindowInputCategory::ESCAPE)));
	_controls.emplace_back(
		std::make_pair((*_display->string)["CONTROL_HELP_ALWAYS"], _get_control_gfx(WindowInputCategory::HELP)));

	// Now generate the various Components
	auto count{_controls.size()};
	Component frame_c{(*_display->layout)["control_overlay:frame"]};
	if (_frame.get()) {
		_frame.release();
		_frame.reset();
	}
	_frame = std::make_unique<Frame>(_display->ui_texture, WindowFrameType::NORMAL, frame_c.w,
		(count + std::stoi(frame_c["padding_y"].value())) * std::stof(frame_c["scale_y"].value()), frame_c.colour,
		frame_c.background, frame_c.alpha);
	auto fsprite{_frame->sprite};
	fsprite.setPosition(0, 0);
	_sprites.emplace_back(fsprite);

	Component sprite_c{(*_display->layout)["control_overlay:graphics"]};
	Component text_c{(*_display->layout)["control_overlay:text"]};

	auto sprite_x{std::stoi(sprite_c["offset_x"].value())};
	auto sprite_y{std::stoi(sprite_c["offset_y"].value())};
	auto sprite_spacing_y{std::stoi(sprite_c["spacing_y"].value())};
	auto text_x{std::stoi(text_c["offset_x"].value())};
	auto text_y{std::stoi(text_c["offset_y"].value())};
	auto text_spacing_y{std::stoi(text_c["spacing_y"].value())};

	for (auto &control : _controls) {
		auto caption{control.first};
		sf::Sprite image{control.second};

		sf::Text text{};
		text.setFont(_system->resources->fonts[text_c.font]);
		text.setCharacterSize(text_c.size);
		text.setFillColor(sf::Color(text_c.colour));
		text.setString(caption);
		text.setPosition(text_x, text_y);
		_texts.emplace_back(text);
		text_y += text_spacing_y;

		image.setPosition(sprite_x, sprite_y);
		image.setScale(sprite_c.scale, sprite_c.scale);
		_sprites.emplace_back(image);
		sprite_y += sprite_spacing_y;
	}

	width = _frame->width;
	height = _frame->height;

	valid = true;
}

auto Sorcery::ControlOverlay::_get_control_gfx(WindowInputCategory input) -> sf::Sprite {

	constexpr auto row_height{100u};
	const auto row_width{_control_texture.getSize().x};
	const auto y{static_cast<unsigned int>(input) * row_height};
	const auto crect{sf::IntRect(0, y, row_width, row_height)};

	sf::Sprite control(_control_texture);
	control.setTextureRect(crect);

	return control;
}

auto Sorcery::ControlOverlay::draw(sf::RenderTarget &target, sf::RenderStates states) const -> void {

	states.transform *= getTransform();

	for (const auto &sprite : _sprites)
		target.draw(sprite, states);

	for (const auto &text : _texts)
		target.draw(text, states);
}