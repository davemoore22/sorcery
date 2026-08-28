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

#include "common/enum.hpp"
#include "common/imgui.hpp"
#include "core/enum.hpp"
#include "display/ui/ui.hpp"
#include "engine/enum.hpp"
#include "types/enum.hpp"

#include <any>
#include <array>
#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace Sorcery {

class UI;
class Context;

class ScreenRenderer {

	public:
		explicit ScreenRenderer(UI &ui, Context &ctx);

		// auto display(Enums::Screen screen) -> void;
		// auto display(Enums::Screen screen, int value) -> void;
		// auto display(Enums::Screen screen, const std::string &value) -> void;

		std::map<Enums::Screen, std::function<void()>> draw_modules;
		std::map<Enums::Screen, std::function<void(int)>> draw_modules_with_int;
		std::map<Enums::Screen, std::function<void(const std::string &)>>
			draw_modules_with_string;

	private:
		UI &_ui;
		Context &_ctx;

		// Private Methods
		auto _display_atlas() -> void;
		auto _display_bestiary() -> void;
		auto _display_compendium() -> void;
		auto _display_main_menu() -> void;
		auto _display_museum() -> void;
		auto _display_options() -> void;
		auto _display_spellbook() -> void;
		auto _display_splash() -> void;

		auto _display_add() -> void;
		auto _display_buy() -> void;
		auto _display_castle() -> void;
		auto _display_edit() -> void;
		auto _display_edge_of_town() -> void;
		auto _display_identify() -> void;
		auto _display_inn() -> void;
		auto _display_legate() -> void;
		auto _display_pay() -> void;
		auto _display_remove() -> void;
		auto _display_reclass() -> void;
		auto _display_rename() -> void;
		auto _display_restart() -> void;
		auto _display_sell() -> void;
		auto _display_shop() -> void;
		auto _display_stay() -> void;
		auto _display_store() -> void;
		auto _display_tavern() -> void;
		auto _display_temple() -> void;
		auto _display_training_grounds() -> void;
		auto _display_uncurse() -> void;
		auto _display_automap() -> void;
		auto _display_graveyard() -> void;
		auto _display_victory() -> void;
		auto _display_choose(const int mode) -> void;
		auto _display_create_name(const int stage) -> void;
		auto _display_create_race(const int stage) -> void;
		auto _display_create_alignment(const int stage) -> void;
		auto _display_create_class(const int stage) -> void;
		auto _display_create_confirm(const int stage) -> void;
		auto _display_delete() -> void;
		auto _display_heal(const int stage) -> void;
		auto _display_rite(const int stage) -> void;
		auto _display_inspect(const int mode) -> void;
		auto _display_level_up(const int mode) -> void;
		auto _display_no_level_up(const int mode) -> void;
		auto _display_recovery(const int mode) -> void;
		auto _display_reorder(const int mode) -> void;
		auto _display_retrain() -> void;
		auto _display_roster() -> void;
		auto _display_select() -> void;
		auto _display_chest(const int stage) -> void;
		auto _display_license(const std::string &string) -> void;

		auto _draw_buy() -> void;
		auto _draw_chest(const Enums::Chests::State state) -> void;
		auto _draw_create_alignment(const int mode) -> void;
		auto _draw_create_class(const int mode) -> void;
		auto _draw_create_confirm(const int mode) -> void;
		auto _draw_create_name(const int mode) -> void;
		auto _draw_create_race(const int mode) -> void;
		auto _draw_choose(const int mode) -> void;
		auto _draw_heal(const int stage) -> void;
		auto _draw_identify() -> void;
		auto _draw_level_up(const int mode) -> void;
		auto _draw_license(Component *component, const std::string &string)
			-> void;
		auto _draw_no_level_up(const int mode) -> void;
		auto _draw_recovery(const int mode) -> void;
		auto _draw_reclass() -> void;
		auto _draw_rename() -> void;
		auto _draw_rite(const int stage) -> void;
		auto _draw_sell() -> void;
		auto _draw_stay() -> void;
		auto _draw_store() -> void;
		auto _draw_uncurse() -> void;
};

};