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

#include "core/controller/inputhandler.hpp" // for ScreenRenderer
#include "common/enum.hpp"					// for Attribute, Attribute::LUCK
#include "core/context.hpp"					// for Context
#include "core/controller/controller.hpp"	// for Controller
#include "core/controller/menuaction.hpp"	// for Controller
#include "core/debug.hpp"
#include "core/define.hpp" // for WINDOW_LAYER_MENUS, WINDOW_...
#include "core/enum.hpp"   // for Screen, CharacterSlot
#include "core/resources.hpp"
#include "display/display.hpp"			// for Display, DisplayMetrics
#include "display/render.hpp"			// for Render
#include "display/ui/ui.hpp"			// for UI
#include "display/ui/uimetrics.hpp"		// for UIMetrics
#include "display/ui/uistyle.hpp"		// for set_text_dim
#include "drawables/define.hpp"			// for RECOVERY_BIRTHDAY, CHOOSE_M...
#include "drawables/dialog.hpp"			// for Dialog
#include "drawables/modal.hpp"			// for Modal
#include "engine/define.hpp"			// for CHEST_GFX_ID
#include "imgui.h"						// for ImVec2, ImGuiWindowFlags_
#include "resources/componentstore.hpp" // for ComponentStore
#include "resources/define.hpp"			// for EVENTS_TEXTURE
#include "resources/fontstore.hpp"		// for FontStore
#include "resources/itemstore.hpp"
#include "types/character/character.hpp" // for Character
#include "types/character/create.hpp"	 // for CharacterCreate
#include "types/component.hpp"			 // for Component
#include "types/game.hpp"				 // for Game
#include "types/meta.hpp"				 // for enum_cast
#include <SDL_events.h>					 // for SDL_EventType, SDL_Event
#include <any>							 // for any_cast, any
#include <format>						 // for format
#include <functional>					 // for invoke
#include <imgui_sugar.hpp>				 // for BooleanGuard, with_Window
#include <map>							 // for map
#include <memory>						 // for unique_ptr
#include <optional>						 // for optional
#include <string>						 // for basic_string, string
#include <unordered_map>				 // for unordered_map, operator==
#include <utility>						 // for pair, to_underlying
#include <vector>						 // for vector

Sorcery::ControllerInputHandler::ControllerInputHandler(Controller &host,
														Context &ctx)
	: _host{host},
	  _ctx{ctx} {

	  };

auto Sorcery::ControllerInputHandler::debug(const SDL_Event &event) -> void {

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
	if (event.type == SDL_KEYDOWN &&
		(event.key.keysym.sym >= SDLK_F1 && event.key.keysym.sym <= SDLK_F11)) {
		if (_host._game != nullptr)
			_host._game->call_debug(event.key.keysym.sym);
	} else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_F12) {
		for (auto const &[key, val] : _host._flags)
			DEBUG_LOGF("{}", std::format("{:>32}: {}", key, val));
	}
#pragma GCC diagnostic pop
}

auto Sorcery::ControllerInputHandler::quicksave(const SDL_Event &event) const
	-> bool {

	return (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_F9);
}

auto Sorcery::ControllerInputHandler::quickload(const SDL_Event &event) const
	-> bool {

	return (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_F10);
}

auto Sorcery::ControllerInputHandler::automap(const SDL_Event &event) const
	-> bool {

	return (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_m);
}

auto Sorcery::ControllerInputHandler::ui_toggle(const SDL_Event &event)
	-> void {

	if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_s)
		_host.toggle_flag("interface_party_panel");
	else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_o)
		_host.toggle_flag("interface_ui");
	else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_n)
		_host._monochrome = !_host._monochrome;
	else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_p)
		_host.toggle_flag("debug_ui");
}

auto Sorcery::ControllerInputHandler::movement(const SDL_Event &event) const
	-> int {

	if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_x)
		return MOVE_TURN_AROUND;
	else if (event.type == SDL_KEYDOWN && (event.key.keysym.sym == SDLK_a ||
										   event.key.keysym.sym == SDLK_l ||
										   event.key.keysym.sym == SDLK_LEFT))
		return MOVE_TURN_LEFT;
	else if (event.type == SDL_KEYDOWN && (event.key.keysym.sym == SDLK_d ||
										   event.key.keysym.sym == SDLK_r ||
										   event.key.keysym.sym == SDLK_RIGHT))
		return MOVE_TURN_RIGHT;
	else if (event.type == SDL_KEYDOWN && (event.key.keysym.sym == SDLK_w ||
										   event.key.keysym.sym == SDLK_f ||
										   event.key.keysym.sym == SDLK_UP))
		return MOVE_FORWARD;
	else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_DOWN)
		return MOVE_BACKWARD;
	else
		return MOVE_NONE;
}

// Check if the SDL event is go-back-to-previous event
auto Sorcery::ControllerInputHandler::back(const SDL_Event &event) const
	-> bool {

	if (event.type == SDL_KEYDOWN &&
		event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
		return true;
	else if (event.type == SDL_MOUSEBUTTONDOWN &&
			 event.button.button == SDL_BUTTON_RIGHT)
		return true;

	return false;
}

// Check for a resize event
auto Sorcery::ControllerInputHandler::resize(const SDL_Event &event) -> void {

	if (event.type != SDL_WINDOWEVENT)
		return;

	if (event.window.event != SDL_WINDOWEVENT_RESIZED &&
		event.window.event != SDL_WINDOWEVENT_SIZE_CHANGED)
		return;

	_ctx.display->update_display_metrics();
	_ctx.ui->metrics->update(_ctx.display->get_display_metrics());
}

// Check if the SDL event is go-back-to-previous event (override to
// set a flag, for example to display a dialog box!)
auto Sorcery::ControllerInputHandler::back(const SDL_Event &event,
										   bool &flag) const -> void {

	if (event.type == SDL_KEYDOWN &&
		event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
		flag = true;
	else if (event.type == SDL_MOUSEBUTTONDOWN &&
			 event.button.button == SDL_BUTTON_RIGHT)
		flag = true;
}

auto Sorcery::ControllerInputHandler::quick_inspect(
	const SDL_Event &event) const -> int {

	if (event.type != SDL_KEYDOWN)
		return -1;

	const auto scancode{event.key.keysym.scancode};

	int position{-1};

	// Main keyboard number row
	if (scancode >= SDL_SCANCODE_1 && scancode <= SDL_SCANCODE_6)
		position = static_cast<int>(scancode - SDL_SCANCODE_1) + 1;

	// Numeric keypad
	else if (scancode >= SDL_SCANCODE_KP_1 && scancode <= SDL_SCANCODE_KP_6)
		position = static_cast<int>(scancode - SDL_SCANCODE_KP_1) + 1;

	if (position == -1)
		return -1;

	const int party_count{_host._game->state->get_party_size()};

	if (position > party_count)
		return -1;

	return position;
}

// Check if the SDL event is a Window-Shut-Down event
auto Sorcery::ControllerInputHandler::abort(const SDL_Event &event) -> bool {

	// SDL_QUIT event
	if (event.type == SDL_QUIT) {
		_host._abort = true;
		return true;
	}

	// Window Close event
	if (event.type == SDL_WINDOWEVENT &&
		event.window.event == SDL_WINDOWEVENT_CLOSE &&
		event.window.windowID ==
			SDL_GetWindowID(_ctx.display->get_SDL_window())) {
		_host._abort = true;
		return true;
	}

	return false;
}

auto Sorcery::ControllerInputHandler::consume_menu_key(
	const std::size_t item_count) -> std::optional<std::size_t> {

	if (!_menu_key || item_count == 0)
		return std::nullopt;

	const auto key{std::exchange(_menu_key, std::nullopt)};

	if (*key == 0)
		return item_count - 1;

	const auto index{static_cast<std::size_t>(*key - 1)};

	if (index >= item_count)
		return std::nullopt;

	return index;
}

auto Sorcery::ControllerInputHandler::menu_key(const SDL_Event &event) -> void {

	if (event.type != SDL_KEYDOWN)
		return;

	switch (event.key.keysym.sym) {

	case SDLK_1:
	case SDLK_KP_1:
		_menu_key = 1;
		break;

	case SDLK_2:
	case SDLK_KP_2:
		_menu_key = 2;
		break;

	case SDLK_3:
	case SDLK_KP_3:
		_menu_key = 3;
		break;

	case SDLK_4:
	case SDLK_KP_4:
		_menu_key = 4;
		break;

	case SDLK_5:
	case SDLK_KP_5:
		_menu_key = 5;
		break;

	case SDLK_6:
	case SDLK_KP_6:
		_menu_key = 6;
		break;

	case SDLK_7:
	case SDLK_KP_7:
		_menu_key = 7;
		break;

	case SDLK_8:
	case SDLK_KP_8:
		_menu_key = 8;
		break;

	case SDLK_9:
	case SDLK_KP_9:
		_menu_key = 9;
		break;

	case SDLK_0:
	case SDLK_KP_0:
		_menu_key = 0;
		break;

	default:
		break;
	}
}