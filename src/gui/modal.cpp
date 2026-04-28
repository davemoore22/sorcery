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

#include "gui/modal.hpp"
#include "core/animation.hpp"
#include "core/context.hpp"
#include "core/controller.hpp"
#include "core/system.hpp"
#include "core/ui.hpp"
#include "gui/frame.hpp"
#include "gui/menu.hpp"
#include "gui/menubuilder.hpp"
#include "resources/fontstore.hpp"
#include "resources/stringstore.hpp"
#include "types/component.hpp"
#include "types/game.hpp"

Sorcery::Modal::Modal(Context &ctx, Component &component)
	: _ctx{ctx},
	  _component{component} {

	show = false;

	_menu_name = component.get("menu_name").value();
	_width = _component.w;
	_height = _component.h;
	_colour = _component.colour;
	_bg_colour = _component.background;
	_hi_colour = _component.get_float("highlight");
	_font = _component.font;
	_has_title = _ctx.get_string(_component.string_key).length() > 0;
	_name = _component.name;
}

Sorcery::Modal::~Modal() {}

auto Sorcery::Modal::name() const -> std::string {

	return _name;
}

auto Sorcery::Modal::regenerate() -> void {

	_ctx.menubuilder->build(_component.name, _width, _items, _data, false);
}

auto Sorcery::Modal::id() const -> std::string {

	return _id;
}

auto Sorcery::Modal::display([[maybe_unused]] bool &is_yes) -> void {

	_id = _component.name + "##modal";

	const auto grid_sz{_ctx.ui->grid_sz};
	const auto rounding{_ctx.ui->frame_rd};
	const auto width{(_width + 4) * grid_sz};
	const auto height{_height * grid_sz};

	ImVec2 centre{ImGui::GetMainViewport()->GetCenter()};
	ImGui::SetNextWindowPos(centre, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	ImGui::SetNextWindowSize(ImVec2{width, height});
	ImGui::SetNextWindowBgAlpha(1.0f);

	set_StyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
	set_StyleVar(ImGuiStyleVar_WindowBorderSize, 0);
	set_StyleVar(ImGuiStyleVar_WindowRounding, rounding);
	set_StyleColor(ImGuiCol_PopupBg, _component.background);
	set_Font(_ctx.ui->fontstore->get_current_font(_component.font).value());

	if (show)
		ImGui::OpenPopup(CSTR(_id));

	with_PopupModal(CSTR(_id), nullptr,
					ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove) {

		const auto p_min{ImGui::GetWindowPos()};
		const auto p_max{ImVec2{ImGui::GetWindowPos().x + width,
								ImGui::GetWindowPos().y + height}};

		const auto col{_ctx.ui->get_hl_colour(_ctx.animation->lerp)};
		const auto sz{ImVec2{
			static_cast<float>((_width + 2) * _ctx.ui->font_sz),
			static_cast<float>(
				(_items.size() * ImGui::GetTextLineHeightWithSpacing()) + 2)}};

		_ctx.ui->draw_frame(p_min, p_max,
							ImVec4{_ctx.ui->ui_colour.x, _ctx.ui->ui_colour.y,
								   _ctx.ui->ui_colour.z, _ctx.animation->fade},
							rounding);
		auto top{_has_title ? 3 : 1};
		if (_has_title) {
			const auto title{_ctx.get_string(_component.string_key)};
			auto centre_x{(((_width + 4) / 2) - (title.length() / 2)) *
						  grid_sz};
			_ctx.ui->draw_text(_ctx.get_string(_component.string_key),
							   ImVec4{1.0f, 1.0f, 1.0f, _ctx.animation->fade},
							   ImVec2{centre_x, grid_sz}, _font);
		}

		// Note that pos is in grid units whereas sz is in pixels!
		_ctx.ui->draw_menu(_menu_name, col, ImVec2{1, top}, sz, _font, _items,
						   _data, false, false);
	}
}