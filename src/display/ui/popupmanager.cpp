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
#include "core/context.hpp"
#include "drawables/dialog.hpp"
#include "drawables/message.hpp"
#include "drawables/modal2.hpp"
#include "resources/componentstore.hpp"

Sorcery::PopupManager::PopupManager(Context &ctx)
	: _ctx{ctx},
	  _message{std::make_unique<Message>(ctx)},
	  _dialog{std::make_unique<Dialog>(ctx)},
	  _modal2{std::make_unique<Modal2>(ctx)} {}

Sorcery::PopupManager::~PopupManager() = default;

auto Sorcery::PopupManager::open_message(const std::string_view component,
										 std::vector<std::string> strings,
										 const Enums::Map::Event event_id)
	-> void {
	close();

	_completed.reset();

	auto &cmp{_ctx.components->get(component)};

	_message->build(cmp);
	_message->set(std::move(strings), event_id);
	_message->open();

	_active = _message.get();
}

auto Sorcery::PopupManager::close() -> void {
	if (!_active)
		return;

	_active->close();
	_active = nullptr;
}

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

		if (pending.menu_name && pending.title_key) {

			open_modal(pending.component, *pending.menu_name,
					   *pending.title_key);

		} else if (pending.menu_name) {

			open_modal(pending.component, *pending.menu_name);

		} else {

			open_modal(pending.component);
		}

		return;
	}

	if (_active != active)
		return;

	if (!active->is_open()) {

		_completed = PopupCompletion{.name = std::string{active->name()},
									 .result = active->result()};

		_active = nullptr;
	}
}

auto Sorcery::PopupManager::active() const -> bool {
	return _active != nullptr;
}

auto Sorcery::PopupManager::is_active(const std::string_view name) const
	-> bool {
	return _active && _active->name() == name;
}

auto Sorcery::PopupManager::consume_completed(const std::string_view name)
	-> bool {
	if (!_completed || _completed->name != name)
		return false;

	_completed.reset();

	return true;
}

auto Sorcery::PopupManager::consume_result(const std::string_view name)
	-> std::optional<DrawableResult> {
	if (!_completed || _completed->name != name)
		return std::nullopt;

	const auto result{_completed->result};

	_completed.reset();

	return result;
}

auto Sorcery::PopupManager::reset() -> void {
	if (_active) {
		_active->close();
		_active = nullptr;
	}

	_completed.reset();
}

auto Sorcery::PopupManager::open_dialog(const std::string_view component,
										const Enums::Layout::DialogType type)
	-> void {
	close();

	_completed.reset();

	auto &cmp{_ctx.components->get(component)};

	_dialog->build(cmp, type);
	_dialog->open();

	_active = _dialog.get();
}

auto Sorcery::PopupManager::consume_accepted(const std::string_view name)
	-> bool {
	const auto result{consume_result(name)};

	return result && *result == DrawableResult::ACCEPTED;
}

auto Sorcery::PopupManager::open_modal(const std::string_view component)
	-> void {

	if (_displaying) {

		if (_active)
			_active->close();

		_pending_modal = PendingModal{.component = std::string{component}};

		return;
	}

	_open_modal(component);
}

auto Sorcery::PopupManager::_open_modal(const std::string_view component)
	-> void {

	close();

	_completed.reset();

	auto &cmp{_ctx.components->get(component)};

	_modal2->build(cmp);
	_modal2->open();

	_active = _modal2.get();
}

auto Sorcery::PopupManager::open_modal(const std::string_view component,
									   const std::string_view menu_name)
	-> void {

	if (_displaying) {

		if (_active)
			_active->close();

		_pending_modal = PendingModal{.component = std::string{component}};

		return;
	}

	_open_modal(component, menu_name);
}

auto Sorcery::PopupManager::_open_modal(const std::string_view component,
										const std::string_view menu_name)
	-> void {

	close();

	_completed.reset();

	auto &cmp{_ctx.components->get(component)};

	_modal2->build(cmp, menu_name);
	_modal2->open();

	_active = _modal2.get();
}

auto Sorcery::PopupManager::open_modal(const std::string_view component,
									   const std::string_view menu_name,
									   const std::string_view title_key)
	-> void {

	if (_displaying) {

		if (_active)
			_active->close();

		_pending_modal = PendingModal{.component = std::string{component}};

		return;
	}

	_open_modal(component, menu_name, title_key);
}

auto Sorcery::PopupManager::_open_modal(const std::string_view component,
										const std::string_view menu_name,
										const std::string_view title_key)
	-> void {

	close();

	_completed.reset();

	auto &cmp{_ctx.components->get(component)};

	_modal2->build(cmp, menu_name);
	_modal2->set_title(title_key);
	_modal2->open();

	_active = _modal2.get();
}