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

#include "gui/menu.hpp"
#include "common/macro.hpp"
#include "core/animation.hpp"
#include "core/context.hpp"
#include "core/controller.hpp"
#include "core/define.hpp"
#include "core/resources.hpp"
#include "core/system.hpp"
#include "core/ui.hpp"
#include "gui/define.hpp"
#include "gui/dialog.hpp"
#include "gui/menubuilder.hpp"
#include "gui/modal.hpp"
#include "resources/fontstore.hpp"
#include "resources/itemstore.hpp"
#include "resources/monsterstore.hpp"
#include "resources/spellstore.hpp"
#include "resources/stringstore.hpp"
#include "types/component.hpp"
#include "types/game.hpp"
#include "types/state.hpp"

Sorcery::Menu::Menu(Context &ctx, Component *component, Game *game)
	: _ctx{ctx},
	  _component{component},
	  _game{game} {

	_name = _component->name;
	_pos = ImVec2{_component->x, _component->y};
	_width = _component->get_int("width");
	_height = _component->get_int("height");
	_colour = _component->colour;
	_bg_colour = _component->background;
	_hi_colour = _component->get_float("highlight");
	_font = _component->font;
	if (component->get("reorder"))
		_reorder = component->get("reorder").value() == "yes";
	else
		_reorder = false;
	if (component->get("across"))
		_across = component->get("across").value() == "yes";
	else
		_across = false;
}

Sorcery::Menu::~Menu() {}

auto Sorcery::Menu::regenerate() -> void {

	_ctx.menubuilder->build(_name, _width, _items, _data, _reorder);
}

auto Sorcery::Menu::draw() -> void {

	with_Window(WINDOW_LAYER_MENUS, nullptr,
				ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoTitleBar) {

		const auto col{_ctx.ui->get_hl_colour(_ctx.animation->lerp)};
		set_Font(
			_ctx.ui->fontstore->get_current_font(Enums::Layout::Font::MONOSPACE)
				.value());
		const auto sz{
			ImVec2{static_cast<float>(_width * _ctx.ui->font_sz),
				   static_cast<float>(
					   (_height * ImGui::GetTextLineHeightWithSpacing()) + 2)}};

		// Note that _pos is in grid units whereas sz is in pixels!
		_ctx.ui->draw_menu(_name, col, _pos, sz, _font, _items, _data, _reorder,
						   _across);

		// Handle SpecialEvents such as Reordering Party Menu
		if (_ctx.controller->has_flag("party_order_changed")) {

			_game->state->reorder_party(_ctx.controller->get_candidate_party());
			regenerate();
			_ctx.controller->clear_candidate_party();
			_ctx.controller->unset_flag("party_order_changed");
		}
	}
}
