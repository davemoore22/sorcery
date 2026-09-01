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
#include "drawables/message.hpp"
#include "resources/componentstore.hpp"

Sorcery::PopupManager::PopupManager(Context &ctx)
	: _ctx{ctx},
	  _message{std::make_unique<Message>(ctx)} {}

Sorcery::PopupManager::~PopupManager() = default;

auto Sorcery::PopupManager::open_message(const std::string_view component,
										 std::vector<std::string> strings,
										 const Enums::Map::Event event_id)
	-> void {

	close();

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

	_active->display();

	if (!_active->is_open()) {

		_completed = PopupCompletion{.name = std::string{_active->name()},
									 .result = _active->result()};

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