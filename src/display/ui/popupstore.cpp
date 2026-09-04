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

#include "display/ui/popupstore.hpp"	// for PopupStore
#include "core/context.hpp"				// for Context
#include "drawables/dialog.hpp"			// for Dialog
#include "drawables/input.hpp"			// for Input
#include "drawables/modal.hpp"			// for Modal
#include "drawables/popup.hpp"			// for Popup
#include "resources/componentstore.hpp" // for ComponentStore
#include "types/enum.hpp"				// for DialogType
#include <algorithm>					// for count_if
#include <format>						// for format
#include <memory>						// for unique_ptr, make_unique
#include <string>						// for basic_string, operator==
#include <string_view>					// for string_view
#include <vector>						// for vector

Sorcery::PopupStore::PopupStore(Context &ctx)
	: _ctx{ctx} {

	// Custom component
	input_donate = std::make_unique<Input>(
		_ctx, _ctx.components->get("global:input_donate"));
	input_name = std::make_unique<Input>(
		_ctx, _ctx.components->get("global:input_name"));
}

Sorcery::PopupStore::~PopupStore() {}

auto Sorcery::PopupStore::in_popup() const -> bool {

	return _active_popup_count() > 0;
}

auto Sorcery::PopupStore::close_all_popups() -> void {

	for (auto *show : _popup_states())
		*show = false;
}

auto Sorcery::PopupStore::_active_popup_count() const -> int {

	const auto states{_popup_states()};

	return std::count_if(states.begin(), states.end(), [](const bool *show) {
		return *show;
	});
}

auto Sorcery::PopupStore::_popup_states() const -> std::vector<bool *> {

	std::vector<bool *> states;

	auto add = [&](const auto &ptr, const std::string_view name) {
		if (ptr) {
			// DEBUG_LOGF("Popup state: {:<24} {}", name, ptr->show);
			states.emplace_back(&ptr->show);
		}
	};

#define ADD_POPUP(popup) add(popup, #popup)

	ADD_POPUP(input_donate);
	ADD_POPUP(input_name);

#undef ADD_POPUP

	return states;
}

// Create a Modal on Demand (used whenever data items on it aren't fixed - for
// example the Party Members); normally otherwise fixed Modals are created at
// the beginning as part of the Form/Module create
auto Sorcery::PopupStore::create_dynamic_modal(const std::string name) -> void {

	// DEBUG_LOGF("Creating Dynamic Modal: {}", name);
}

// Not an ideal function, really need to maintain a pointer status map instead
auto Sorcery::PopupStore::get_popups() const -> std::string {

	auto get_popup_status{[](void *component, std::string type) -> std::string {
		if (component != nullptr) {
			if (type == "modal") {
				auto casted{(Modal *)component};
				auto name{casted->name()};
				return std::format("{:>26}: {}\n", name, casted->show);
			} else if (type == "popup") {
				auto casted{(Popup *)component};
				auto name{casted->name()};
				return std::format("{:>26}: {}\n", name, casted->show);
			} else if (type == "input") {
				auto casted{(Input *)component};
				auto name{casted->name()};
				return std::format("{:>26}: {}\n", name, casted->show);
			}
		}

		return "";
	}};

	std::string output{};
	if (input_donate)
		output.append(get_popup_status((void *)input_donate.get(), "input"));
	if (input_name)
		output.append(get_popup_status((void *)input_name.get(), "input"));

	return output;
}