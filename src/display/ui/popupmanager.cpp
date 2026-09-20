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

#include "display/ui/popupmanager.hpp"
#include "core/context.hpp"				  // for Context
#include "core/controller/controller.hpp" // for Controller
#include "core/controller/inputmode.hpp"  // for Mode, Mode::CONFIRMATION
#include "drawables/dialog.hpp"			  // for Dialog
#include "drawables/message.hpp"		  // for Message
#include "drawables/modal2.hpp"			  // for Modal2
#include "resources/componentstore.hpp"	  // for ComponentStore
#include "types/enum.hpp"				  // for DialogType
#include <utility>						  // for move

/// @brief
/// @param ctx
Sorcery::PopupManager::PopupManager(Context &ctx)
	: _ctx{ctx},
	  _message{std::make_unique<Message>(ctx)},
	  _dialog{std::make_unique<Dialog>(ctx)},
	  _modal2{std::make_unique<Modal2>(ctx)} {}

/// @brief
Sorcery::PopupManager::~PopupManager() = default;

/// @brief
/// @param component
/// @param strings
/// @param event_id
/// @return
auto Sorcery::PopupManager::open_message(const std::string_view component, std::vector<std::string> strings,
										 const Enums::Map::Event event_id) -> void {

	close();

	_completed.reset();

	auto &cmp{_ctx.components->get(component)};

	_message->build(cmp);
	_message->set(std::move(strings), event_id);
	_message->open();

	_active = _message.get();

	_ctx.controller->push_input_mode(Enums::Input::Mode::NOTIFICATION);
	_input_mode_pushed = true;
}

/// @brief
/// @return
auto Sorcery::PopupManager::close() -> void {

	if (!_active)
		return;

	_active->close();
	_active = nullptr;

	_pop_input_mode();
}

/// @brief
/// @return
auto Sorcery::PopupManager::display() -> void {

	if (!_active)
		return;

	auto *active{_active};

	_displaying = true;
	active->display();
	_displaying = false;

	if (_pending_modal) {

		auto pending{std::move(*_pending_modal)};
		_pending_modal.reset();

		if (pending.menu_name && pending.title_key)
			open_modal(pending.component, *pending.menu_name, *pending.title_key);
		else if (pending.menu_name)
			open_modal(pending.component, *pending.menu_name);
		else
			open_modal(pending.component);

		return;
	}

	if (_active != active)
		return;

	if (!active->is_open()) {
		_completed = PopupCompletion{.name = std::string{active->name()}, .result = active->result()};
		_active = nullptr;

		_pop_input_mode();
	}
}

/// @brief
/// @return
auto Sorcery::PopupManager::active() const -> bool {

	return _active != nullptr;
}

/// @brief
/// @param name
/// @return
auto Sorcery::PopupManager::is_active(const std::string_view name) const -> bool {

	return _active && _active->name() == name;
}

/// @brief
/// @param name
/// @return
auto Sorcery::PopupManager::consume_completed(const std::string_view name) -> bool {

	if (!_completed || _completed->name != name)
		return false;

	_completed.reset();

	return true;
}

/// @brief
/// @param name
/// @return
auto Sorcery::PopupManager::consume_result(const std::string_view name) -> std::optional<DrawableResult> {

	if (!_completed || _completed->name != name)
		return std::nullopt;

	const auto result{_completed->result};

	_completed.reset();

	return result;
}

/// @brief
/// @return
auto Sorcery::PopupManager::reset() -> void {

	if (_active) {
		_active->close();
		_active = nullptr;
	}
	_pop_input_mode();

	_completed.reset();
}

/// @brief
/// @param component
/// @param type
/// @return
auto Sorcery::PopupManager::open_dialog(const std::string_view component, const Enums::Layout::DialogType type)
	-> void {

	close();

	_completed.reset();

	auto &cmp{_ctx.components->get(component)};

	_dialog->build(cmp, type);
	_dialog->open();

	_active = _dialog.get();

	using enum Enums::Input::Mode;
	switch (type) {
	case Enums::Layout::DialogType::CONFIRM:
		_ctx.controller->push_input_mode(CONFIRMATION);
		break;
	case Enums::Layout::DialogType::OK:
		_ctx.controller->push_input_mode(NOTIFICATION);
		break;
	}
	_input_mode_pushed = true;
}

/// @brief
/// @param name
/// @return
auto Sorcery::PopupManager::consume_accepted(const std::string_view name) -> bool {
	const auto result{consume_result(name)};

	return result && *result == DrawableResult::ACCEPTED;
}

/// @brief
/// @param component
/// @return
auto Sorcery::PopupManager::open_modal(const std::string_view component) -> void {

	if (_displaying) {

		if (_active)
			_active->close();

		_pending_modal = PendingModal{.component = std::string{component}};

		return;
	}

	_open_modal(component);
}

/// @brief
/// @param component
/// @return
auto Sorcery::PopupManager::_open_modal(const std::string_view component) -> void {

	close();

	_completed.reset();

	auto &cmp{_ctx.components->get(component)};

	_modal2->build(cmp);
	_modal2->open();

	_active = _modal2.get();
}

/// @brief
/// @param component
/// @param menu_name
/// @return
auto Sorcery::PopupManager::open_modal(const std::string_view component, const std::string_view menu_name) -> void {

	if (_displaying) {
		if (_active)
			_active->close();

		_pending_modal = PendingModal{.component = std::string{component}};

		return;
	}

	_open_modal(component, menu_name);
}

/// @brief
/// @param component
/// @param menu_name
/// @return
auto Sorcery::PopupManager::_open_modal(const std::string_view component, const std::string_view menu_name) -> void {

	close();

	_completed.reset();

	auto &cmp{_ctx.components->get(component)};

	_modal2->build(cmp, menu_name);
	_modal2->open();

	_active = _modal2.get();
}

/// @brief
/// @param component
/// @param menu_name
/// @param title_key
/// @return
auto Sorcery::PopupManager::open_modal(const std::string_view component, const std::string_view menu_name,
									   const std::string_view title_key) -> void {

	if (_displaying) {
		if (_active)
			_active->close();

		_pending_modal = PendingModal{.component = std::string{component}};

		return;
	}

	_open_modal(component, menu_name, title_key);
}

/// @brief
/// @param component
/// @param menu_name
/// @param title_key
/// @return
auto Sorcery::PopupManager::_open_modal(const std::string_view component, const std::string_view menu_name,
										const std::string_view title_key) -> void {

	close();

	_completed.reset();

	auto &cmp{_ctx.components->get(component)};

	_modal2->build(cmp, menu_name);
	_modal2->set_title(title_key);
	_modal2->open();

	_active = _modal2.get();
}

/// @brief
/// @return
auto Sorcery::PopupManager::_pop_input_mode() -> void {

	if (!_input_mode_pushed)
		return;

	_ctx.controller->pop_input_mode();
	_input_mode_pushed = false;
}