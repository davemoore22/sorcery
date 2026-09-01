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

	// Custom components
	/*
	dialog_exit = std::make_unique<Dialog>(
		_ctx, _ctx.components->get("main_menu:dialog_exit"),
		Enums::Layout::DialogType::CONFIRM);
	dialog_new = std::make_unique<Dialog>(
		_ctx, _ctx.components->get("main_menu:dialog_new"),
		Enums::Layout::DialogType::CONFIRM);
	dialog_leave = std::make_unique<Dialog>(
		_ctx, _ctx.components->get("main_menu:dialog_leave"),
		Enums::Layout::DialogType::CONFIRM);
	dialog_rite =
		std::make_unique<Dialog>(_ctx, _ctx.components->get("rite:dialog_rite"),
								 Enums::Layout::DialogType::CONFIRM);
	dialog_search = std::make_unique<Dialog>(
		_ctx, _ctx.components->get("engine_base_ui:dialog_search"),
		Enums::Layout::DialogType::CONFIRM);
	dialog_delete = std::make_unique<Dialog>(
		_ctx, _ctx.components->get("delete:dialog_delete"),
		Enums::Layout::DialogType::CONFIRM);
	notice_divvy = std::make_unique<Dialog>(
		_ctx, _ctx.components->get("global:notice_divvy"),
		Enums::Layout::DialogType::OK);
	notice_renamed_ok = std::make_unique<Dialog>(
		_ctx, _ctx.components->get("global:notice_renamed_ok"),
		Enums::Layout::DialogType::OK);
	notice_pool_gold = std::make_unique<Dialog>(
		_ctx, _ctx.components->get("global:notice_pool_gold"),
		Enums::Layout::DialogType::OK);
	notice_cannot_donate = std::make_unique<Dialog>(
		_ctx, _ctx.components->get("global:notice_cannot_donate"),
		Enums::Layout::DialogType::OK);
	notice_donated_ok = std::make_unique<Dialog>(
		_ctx, _ctx.components->get("global:notice_donated_ok"),
		Enums::Layout::DialogType::OK);
	notice_not_enough_gold = std::make_unique<Dialog>(
		_ctx, _ctx.components->get("global:notice_not_enough_gold"),
		Enums::Layout::DialogType::OK);
		dialog_stairs_up = std::make_unique<Dialog>(
		_ctx, _ctx.components->get("engine_base_ui:dialog_stairs_up"),
		Enums::Layout::DialogType::CONFIRM);
	dialog_stairs_down = std::make_unique<Dialog>(
		_ctx, _ctx.components->get("engine_base_ui:dialog_stairs_down"),
		Enums::Layout::DialogType::CONFIRM);*/
	modal_camp = std::make_unique<Modal>(
		_ctx, _ctx.components->get("engine_base_ui:modal_camp"));

	modal_elevator_top = std::make_unique<Modal>(
		_ctx, _ctx.components->get("global:modal_elevator_top"));
	modal_elevator_bottom = std::make_unique<Modal>(
		_ctx, _ctx.components->get("global:modal_elevator_bottom"));

	modal_drop = std::make_unique<Modal>(
		_ctx, _ctx.components->get("global:modal_drop"));
	modal_inspect = std::make_unique<Modal>(
		_ctx, _ctx.components->get("global:modal_inspect"));
	modal_identify = std::make_unique<Modal>(
		_ctx, _ctx.components->get("global:modal_identify"));
	modal_chest = std::make_unique<Modal>(
		_ctx, _ctx.components->get("global:modal_chest"));
	modal_equip = std::make_unique<Modal>(
		_ctx, _ctx.components->get("global:modal_equip"));
	modal_remove = std::make_unique<Modal>(
		_ctx, _ctx.components->get("global:modal_remove_item"));
	modal_trade = std::make_unique<Modal>(
		_ctx, _ctx.components->get("global:modal_trade"));
	modal_give = std::make_unique<Modal>(
		_ctx, _ctx.components->get("global:modal_give"));
	modal_use =
		std::make_unique<Modal>(_ctx, _ctx.components->get("global:modal_use"));
	modal_invoke = std::make_unique<Modal>(
		_ctx, _ctx.components->get("global:modal_invoke"));
	modal_spell = std::make_unique<Modal>(
		_ctx, _ctx.components->get("global:modal_spell"));

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
	ADD_POPUP(modal_camp);
	ADD_POPUP(modal_elevator_top);
	ADD_POPUP(modal_elevator_bottom);
	ADD_POPUP(modal_inspect);
	ADD_POPUP(modal_help);
	ADD_POPUP(modal_tithe);
	ADD_POPUP(modal_identify);
	ADD_POPUP(modal_chest);
	ADD_POPUP(modal_equip);
	ADD_POPUP(modal_remove);
	ADD_POPUP(modal_spell);
	ADD_POPUP(modal_drop);
	ADD_POPUP(modal_trade);
	ADD_POPUP(modal_give);
	ADD_POPUP(modal_use);
	ADD_POPUP(modal_invoke);

#undef ADD_POPUP

	return states;
}

// Create a Modal on Demand (used whenever data items on it aren't fixed - for
// example the Party Members); normally otherwise fixed Modals are created at
// the beginning as part of the Form/Module create
auto Sorcery::PopupStore::create_dynamic_modal(const std::string name) -> void {

	// DEBUG_LOGF("Creating Dynamic Modal: {}", name);

	if (name == "modal_inspect") {
		if (modal_inspect.get())
			modal_inspect.reset();
		modal_inspect = std::make_unique<Modal>(
			_ctx, _ctx.components->get("global:modal_inspect"));
		modal_inspect->regenerate();
	} else if (name == "modal_help") {
		if (modal_help.get())
			modal_help.reset();
		modal_help = std::make_unique<Modal>(
			_ctx, _ctx.components->get("global:modal_help"));
		modal_help->regenerate();
	} else if (name == "modal_tithe") {
		if (modal_tithe.get())
			modal_tithe.reset();
		modal_tithe = std::make_unique<Modal>(
			_ctx, _ctx.components->get("global:modal_tithe"));
		modal_tithe->regenerate();
	} else if (name == "modal_identify") {
		if (modal_identify.get())
			modal_identify.reset();
		modal_identify = std::make_unique<Modal>(
			_ctx, _ctx.components->get("global:modal_identify"));
		modal_identify->regenerate();
	} else if (name == "modal_chest") {
		if (modal_chest.get())
			modal_chest.reset();
		modal_chest = std::make_unique<Modal>(
			_ctx, _ctx.components->get("global:modal_chest"));
		modal_chest->regenerate();
	} else if (name == "modal_equip") {
		if (modal_equip.get())
			modal_equip.reset();
		modal_equip = std::make_unique<Modal>(
			_ctx, _ctx.components->get("global:modal_equip"));
		modal_equip->regenerate();
	} else if (name == "modal_remove") {
		if (modal_remove.get())
			modal_remove.reset();
		modal_remove = std::make_unique<Modal>(
			_ctx, _ctx.components->get("global:modal_remove_item"));
		modal_remove->regenerate();
	} else if (name == "modal_spell") {
		if (modal_spell.get())
			modal_spell.reset();
		modal_spell = std::make_unique<Modal>(
			_ctx, _ctx.components->get("global:modal_spell"));
		modal_spell->regenerate();
	} else if (name == "modal_drop") {
		if (modal_drop.get())
			modal_drop.reset();
		modal_drop = std::make_unique<Modal>(
			_ctx, _ctx.components->get("global:modal_drop"));
		modal_drop->regenerate();
	} else if (name == "modal_trade") {
		if (modal_trade.get())
			modal_trade.reset();
		modal_trade = std::make_unique<Modal>(
			_ctx, _ctx.components->get("global:modal_trade"));
		modal_trade->regenerate();
	} else if (name == "modal_give") {
		if (modal_give.get())
			modal_give.reset();
		modal_give = std::make_unique<Modal>(
			_ctx, _ctx.components->get("global:modal_give"));
		modal_give->regenerate();
	} else if (name == "modal_use") {
		if (modal_use.get())
			modal_use.reset();
		modal_use = std::make_unique<Modal>(
			_ctx, _ctx.components->get("global:modal_use"));
		modal_use->regenerate();
	} else if (name == "modal_invoke") {
		if (modal_invoke.get())
			modal_invoke.reset();
		modal_invoke = std::make_unique<Modal>(
			_ctx, _ctx.components->get("global:modal_invoke"));
		modal_invoke->regenerate();
	}

	// Note that modal_camp is not dynamic and thus isn't handled here
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
	if (modal_camp)
		output.append(get_popup_status((void *)modal_camp.get(), "modal"));
	if (modal_chest)
		output.append(get_popup_status((void *)modal_chest.get(), "modal"));
	if (modal_elevator_bottom)
		output.append(
			get_popup_status((void *)modal_elevator_bottom.get(), "modal"));
	if (modal_elevator_top)
		output.append(
			get_popup_status((void *)modal_elevator_top.get(), "modal"));
	if (modal_drop)
		output.append(get_popup_status((void *)modal_drop.get(), "modal"));
	if (modal_equip)
		output.append(get_popup_status((void *)modal_equip.get(), "modal"));
	if (modal_give)
		output.append(get_popup_status((void *)modal_give.get(), "modal"));
	if (modal_help)
		output.append(get_popup_status((void *)modal_help.get(), "modal"));
	if (modal_identify)
		output.append(get_popup_status((void *)modal_identify.get(), "modal"));
	if (modal_inspect)
		output.append(get_popup_status((void *)modal_inspect.get(), "modal"));
	if (modal_invoke)
		output.append(get_popup_status((void *)modal_invoke.get(), "modal"));
	if (modal_remove)
		output.append(get_popup_status((void *)modal_remove.get(), "modal"));
	if (modal_spell)
		output.append(get_popup_status((void *)modal_spell.get(), "modal"));
	if (modal_tithe)
		output.append(get_popup_status((void *)modal_tithe.get(), "modal"));
	if (modal_trade)
		output.append(get_popup_status((void *)modal_trade.get(), "modal"));
	if (modal_use)
		output.append(get_popup_status((void *)modal_use.get(), "modal"));

	return output;
}