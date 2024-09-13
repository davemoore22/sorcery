// Copyright (C) 2024 Dave Moore
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

#include "gui/controloverlay.hpp"
#include "core/display.hpp"
#include "core/system.hpp"
#include "gui/frame.hpp"

// Standard Constructor
Sorcery::ControlOverlay::ControlOverlay(System *system, Display *display, Component layout)
	: _system{system}, _display{display}, _layout{layout} {

	_sprites.clear();
	_texts.clear();

	_input_mode = WIM::NO_INPUTMODE;
	_control_texture = (*_system->resources).textures[GraphicsTexture::CONTROLS];

	_controls.clear();
}

auto Sorcery::ControlOverlay::set_input_mode(WIM input_mode) -> void {

	valid = false;
	_input_mode = input_mode;
	_controls.clear();
	_sprites.clear();
	_texts.clear();

	// Populate the Available Control Methods
	switch (_input_mode) {
	case WIM::ALLOCATE_STATS:
		_controls.emplace_back(
			std::make_pair((*_display->string)["CONTROL_MOUSE_ALLOCATE_STATS"], _get_control_gfx(WIC::MOUSE_MOVE)));
		_controls.emplace_back(
			std::make_pair((*_display->string)["CONTROL_UP_ALLOCATE_STATS"], _get_control_gfx(WIC::UP)));
		_controls.emplace_back(
			std::make_pair((*_display->string)["CONTROL_DOWN_ALLOCATE_STATS"], _get_control_gfx(WIC::DOWN)));
		_controls.emplace_back(
			std::make_pair((*_display->string)["CONTROL_LEFT_ALLOCATE_STATS"], _get_control_gfx(WIC::LEFT)));
		_controls.emplace_back(
			std::make_pair((*_display->string)["CONTROL_RIGHT_ALLOCATE_STATS"], _get_control_gfx(WIC::RIGHT)));
		break;
	case WIM::MENU_ATTRACT_MODE:
		valid = false;
		return;
		break;
	case WIM::CHOOSE_METHOD:
		_controls.emplace_back(
			std::make_pair((*_display->string)["CONTROL_MOUSE_METHOD"], _get_control_gfx(WIC::MOUSE_MOVE)));
		_controls.emplace_back(std::make_pair((*_display->string)["CONTROL_LEFT_METHOD"], _get_control_gfx(WIC::LEFT)));
		_controls.emplace_back(
			std::make_pair((*_display->string)["CONTROL_RIGHT_METHOD"], _get_control_gfx(WIC::RIGHT)));
		_controls.emplace_back(
			std::make_pair((*_display->string)["CONTROL_CONFIRM_METHOD"], _get_control_gfx(WIC::CONFIRM)));
		break;
	case WIM::CHOOSE_PORTRAIT:
		_controls.emplace_back(
			std::make_pair((*_display->string)["CONTROL_LEFT_CHOOSE_PORTRAIT"], _get_control_gfx(WIC::LEFT)));
		_controls.emplace_back(
			std::make_pair((*_display->string)["CONTROL_RIGHT_CHOOSE_PORTRAIT"], _get_control_gfx(WIC::RIGHT)));
		_controls.emplace_back(
			std::make_pair((*_display->string)["CONTROL_CONFIRM_CHOOSE_PORTRAIT"], _get_control_gfx(WIC::CONFIRM)));
		break;
	case WIM::COMPENDIUM:
		break;
	case WIM::CONFIRM_LEAVE_GAME:
	case WIM::CONFIRM_QUIT_GAME:
	case WIM::CONFIRM_STRICT_MODE:
	case WIM::SAVE_CHANGES:
	case WIM::CANCEL_CHANGES:
		_controls.emplace_back(
			std::make_pair((*_display->string)["CONTROL_MOUSE_CONFIRMATION"], _get_control_gfx(WIC::MOUSE_MOVE)));
		_controls.emplace_back(
			std::make_pair((*_display->string)["CONTROL_LEFT_CONFIRMATION"], _get_control_gfx(WIC::LEFT)));
		_controls.emplace_back(
			std::make_pair((*_display->string)["CONTROL_RIGHT_CONFIRMATION"], _get_control_gfx(WIC::RIGHT)));
		_controls.emplace_back(
			std::make_pair((*_display->string)["CONTROL_CONFIRM_CONFIRMATION"], _get_control_gfx(WIC::CONFIRM)));
		_controls.emplace_back(
			std::make_pair((*_display->string)["CONTROL_Y_N_CONFIRMATION"], _get_control_gfx(WIC::YES_NO)));
		break;
	case WIM::DISPLAY_TEXT_FILE:
		_controls.emplace_back(std::make_pair((*_display->string)["CONTROL_UP_FILE"], _get_control_gfx(WIC::UP)));
		_controls.emplace_back(std::make_pair((*_display->string)["CONTROL_DOWN_FILE"], _get_control_gfx(WIC::DOWN)));
		_controls.emplace_back(
			std::make_pair((*_display->string)["CONTROL_PAGE_UP_FILE"], _get_control_gfx(WIC::PAGE_UP)));
		_controls.emplace_back(
			std::make_pair((*_display->string)["CONTROL_PAGE_DOWN_FILE"], _get_control_gfx(WIC::PAGE_DOWN)));
		_controls.emplace_back(std::make_pair((*_display->string)["CONTROL_HOME_FILE"], _get_control_gfx(WIC::HOME)));
		_controls.emplace_back(std::make_pair((*_display->string)["CONTROL_END_FILE"], _get_control_gfx(WIC::END)));
		break;
	case WIM::GAME_OPTIONS:
		_controls.emplace_back(
			std::make_pair((*_display->string)["CONTROL_MOUSE_OPTION"], _get_control_gfx(WIC::MOUSE_MOVE)));
		_controls.emplace_back(std::make_pair((*_display->string)["CONTROL_UP_OPTION"], _get_control_gfx(WIC::UP)));
		_controls.emplace_back(std::make_pair((*_display->string)["CONTROL_DOWN_OPTION"], _get_control_gfx(WIC::DOWN)));
		_controls.emplace_back(std::make_pair((*_display->string)["CONTROL_LEFT_OPTION"], _get_control_gfx(WIC::LEFT)));
		_controls.emplace_back(
			std::make_pair((*_display->string)["CONTROL_RIGHT_OPTION"], _get_control_gfx(WIC::RIGHT)));
		_controls.emplace_back(
			std::make_pair((*_display->string)["CONTROL_CONFIRM_OPTION"], _get_control_gfx(WIC::CONFIRM)));
		break;
	case WIM::INPUT_NAME:
		_controls.emplace_back(
			std::make_pair((*_display->string)["CONTROL_MOUSE_INPUT_NAME"], _get_control_gfx(WIC::MOUSE_MOVE)));
		_controls.emplace_back(std::make_pair((*_display->string)["CONTROL_UP_INPUT_NAME"], _get_control_gfx(WIC::UP)));
		_controls.emplace_back(
			std::make_pair((*_display->string)["CONTROL_DOWN_INPUT_NAME"], _get_control_gfx(WIC::DOWN)));
		_controls.emplace_back(
			std::make_pair((*_display->string)["CONTROL_LEFT_INPUT_NAME"], _get_control_gfx(WIC::LEFT)));
		_controls.emplace_back(
			std::make_pair((*_display->string)["CONTROL_RIGHT_INPUT_NAME"], _get_control_gfx(WIC::RIGHT)));
		_controls.emplace_back(
			std::make_pair((*_display->string)["CONTROL_CONFIRM_INPUT_NAME"], _get_control_gfx(WIC::CONFIRM)));
		_controls.emplace_back(std::make_pair(
			(*_display->string)["CONTROL_ALPHANUMERIC_INPUT_NAME"], _get_control_gfx(WIC::ALPHANUMERIC)));
		break;
	case WIM::NAVIGATE_MENU:
		_controls.emplace_back(
			std::make_pair((*_display->string)["CONTROL_MOUSE_MOVE_MENU"], _get_control_gfx(WIC::MOUSE_MOVE)));
		_controls.emplace_back(std::make_pair((*_display->string)["CONTROL_UP_MENU"], _get_control_gfx(WIC::UP)));
		_controls.emplace_back(std::make_pair((*_display->string)["CONTROL_DOWN_MENU"], _get_control_gfx(WIC::DOWN)));
		_controls.emplace_back(
			std::make_pair((*_display->string)["CONTROL_CONFIRM_MENU"], _get_control_gfx(WIC::CONFIRM)));
		break;
	case WIM::BROWSE_CHARACTER:
		_controls.emplace_back(
			std::make_pair((*_display->string)["CONTROL_UP_BROWSER_CHARACTER"], _get_control_gfx(WIC::UP)));
		_controls.emplace_back(
			std::make_pair((*_display->string)["CONTROL_DOWN_BROWSER_CHARACTER"], _get_control_gfx(WIC::DOWN)));
		_controls.emplace_back(
			std::make_pair((*_display->string)["CONTROL_LEFT_BROWSER_CHARACTER"], _get_control_gfx(WIC::LEFT)));
		_controls.emplace_back(
			std::make_pair((*_display->string)["CONTROL_RIGHT_BROWSER_CHARACTER"], _get_control_gfx(WIC::RIGHT)));
		_controls.emplace_back(
			std::make_pair((*_display->string)["CONTROL_CONFIRM_BROWSER_CHARACTER"], _get_control_gfx(WIC::CONFIRM)));
		break;
	case WIM::REVIEW_AND_CONFIRM:
		_controls.emplace_back(
			std::make_pair((*_display->string)["CONTROL_LEFT_REVIEW_AND_CONFIRM"], _get_control_gfx(WIC::LEFT)));
		_controls.emplace_back(
			std::make_pair((*_display->string)["CONTROL_RIGHT_REVIEW_AND_CONFIRM"], _get_control_gfx(WIC::RIGHT)));
		_controls.emplace_back(
			std::make_pair((*_display->string)["CONTROL_CONFIRM_REVIEW_AND_CONFIRM"], _get_control_gfx(WIC::CONFIRM)));
		break;
	case WIM::IN_GAME:
		_controls.emplace_back(
			std::make_pair((*_display->string)["CONTROL_LEFT_IN_GAME"], _get_control_gfx(WIC::MAZE_LEFT)));
		_controls.emplace_back(
			std::make_pair((*_display->string)["CONTROL_RIGHT_IN_GAME"], _get_control_gfx(WIC::MAZE_RIGHT)));
		_controls.emplace_back(
			std::make_pair((*_display->string)["CONTROL_UP_IN_GAME"], _get_control_gfx(WIC::MAZE_FORWARD)));
		_controls.emplace_back(
			std::make_pair((*_display->string)["CONTROL_DOWN_IN_GAME"], _get_control_gfx(WIC::MAZE_BACKWARD)));
		_controls.emplace_back(std::make_pair(
			(*_display->string)["CONTROL_TURN_AROUND_IN_GAME"], _get_control_gfx(WIC::MAZE_TURN_AROUND)));
		_controls.emplace_back(
			std::make_pair((*_display->string)["CONTROL_CAMP_IN_GAME"], _get_control_gfx(WIC::MAZE_CAMP)));
		_controls.emplace_back(
			std::make_pair((*_display->string)["CONTROL_ACTION_IN_GAME"], _get_control_gfx(WIC::MAZE_ACTION)));
		_controls.emplace_back(
			std::make_pair((*_display->string)["CONTROL_SEARCH_IN_GAME"], _get_control_gfx(WIC::MAZE_SEARCH)));
		_controls.emplace_back(
			std::make_pair((*_display->string)["CONTROL_INSPECT_IN_GAME"], _get_control_gfx(WIC::MAZE_INSPECT)));
		_controls.emplace_back(
			std::make_pair((*_display->string)["CONTROL_1_TO_6_IN_GAME"], _get_control_gfx(WIC::SHOW_CHARACTER)));
		_controls.emplace_back(
			std::make_pair((*_display->string)["CONTROL_GUI_TOGGLE_IN_GAME"], _get_control_gfx(WIC::MAZE_GUI_TOGGLE)));
		_controls.emplace_back(std::make_pair(
			(*_display->string)["CONTROL_STATUS_TOGGLE_IN_GAME"], _get_control_gfx(WIC::MAZE_STATUSBAR_TOGGLE)));
		_controls.emplace_back(
			std::make_pair((*_display->string)["CONTROL_QUIT_IN_GAME"], _get_control_gfx(WIC::MAZE_QUIT)));
		break;
	default:
		break;
	};

	switch (_input_mode) {
	case WIM::IN_GAME:
		break;
	default:
		_controls.emplace_back(std::make_pair(
			(*_display->string)["CONTROL_DELETE_BACK_ALWAYS"], _get_control_gfx(WIC::BACK_DELETE_CANCEL)));
		_controls.emplace_back(
			std::make_pair((*_display->string)["CONTROL_ESCAPE_ALWAYS"], _get_control_gfx(WIC::ESCAPE)));
		break;
	}

	_controls.emplace_back(std::make_pair((*_display->string)["CONTROL_HELP_ALWAYS"], _get_control_gfx(WIC::HELP)));

	// Now generate the various Components
	auto count{_controls.size()};
	Component frame_c{(*_display->layout)["control_overlay:frame"]};
	if (_frame.get()) {
		_frame.release();
		_frame.reset();
	}
	_frame = std::make_unique<Frame>(_display->ui_texture, frame_c.w,
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
		if (_display->get_upper())
			std::transform(caption.begin(), caption.end(), caption.begin(), ::toupper);
		text.setString(caption);
		text.setPosition(text_x, text_y);
		if (_display->get_bold())
			text.setStyle(sf::Text::Bold);
		_texts.emplace_back(text);
		text_y += text_spacing_y;

		image.setPosition(sprite_x, sprite_y);
		image.setScale(sprite_c.scl());
		_sprites.emplace_back(image);
		sprite_y += sprite_spacing_y;
	}

	width = _frame->get_width();
	height = _frame->get_height();

	valid = true;
}

auto Sorcery::ControlOverlay::_get_control_gfx(WIC input) -> sf::Sprite {

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
