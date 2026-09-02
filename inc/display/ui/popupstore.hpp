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

#pragma once

#include <memory> // for unique_ptr
#include <string> // for string
#include <vector> // for vector

namespace Sorcery { class Dialog; }
namespace Sorcery { class Input; }
namespace Sorcery { class Message; }
namespace Sorcery { class Modal; }
namespace Sorcery { struct Context; }

namespace Sorcery {

class PopupStore {

	public:
		explicit PopupStore(Context &ctx);
		~PopupStore();

		std::unique_ptr<Input> input_donate;
		std::unique_ptr<Input> input_name;

		std::unique_ptr<Modal> modal_inspect;
		std::unique_ptr<Modal> modal_help;
		std::unique_ptr<Modal> modal_tithe;
		std::unique_ptr<Modal> modal_identify;
		std::unique_ptr<Modal> modal_equip;
		std::unique_ptr<Modal> modal_remove;
		std::unique_ptr<Modal> modal_spell;
		std::unique_ptr<Modal> modal_drop;
		std::unique_ptr<Modal> modal_trade;
		std::unique_ptr<Modal> modal_give;
		std::unique_ptr<Modal> modal_use;
		std::unique_ptr<Modal> modal_invoke;
		std::unique_ptr<Modal> modal_elevator_top;
		std::unique_ptr<Modal> modal_elevator_bottom;
		std::unique_ptr<Modal> modal_chest;

		auto in_popup() const -> bool;
		auto close_all_popups() -> void;
		auto create_dynamic_modal(const std::string name) -> void;
		auto get_popups() const -> std::string;

	private:
		Context &_ctx;

		auto _active_popup_count() const -> int;
		auto _popup_states() const -> std::vector<bool *>;
};
};