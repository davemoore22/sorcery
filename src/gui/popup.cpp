// Copyright (C) 2025 Dave Moore
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

#include "gui/popup.hpp"
#include "core/animation.hpp"
#include "core/context.hpp"
#include "core/system.hpp"
#include "core/ui.hpp"
#include "resources/fontstore.hpp"
#include "resources/stringstore.hpp"
#include "types/component.hpp"

Sorcery::Popup::Popup(Context &ctx, Component &component)
	: _ctx{ctx},
	  _component{component} {

	show = false;
	_name = _component.name;
}

auto Sorcery::Popup::id() const -> std::string {

	return _id;
}

auto Sorcery::Popup::name() const -> std::string {

	return _name;
}

auto Sorcery::Popup::display() -> void {

	if (!show)
		return;

	_id = _component.name + "##outer";
	const auto text{_ctx.get_string(_component.string_key)};
	const auto grid_sz{_ctx.ui->grid_sz};
	const auto rounding{_ctx.ui->frame_rd};

	set_Font(_ctx.ui->fontstore->get_current_font(_component.font).value());
	const auto width{ImGui::CalcTextSize(text.c_str()).x + (grid_sz * 4)};
	const auto height{_component.h * grid_sz};

	ImVec2 centre{ImGui::GetMainViewport()->GetCenter()};
	ImGui::SetNextWindowPos(centre, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	ImGui::SetNextWindowSize(ImVec2{width, height});
	ImGui::SetNextWindowBgAlpha(1.0f);

	const auto col{_ctx.ui->get_hl_colour(_ctx.animation->lerp)};
	set_StyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
	set_StyleVar(ImGuiStyleVar_WindowBorderSize, 0);
	set_StyleVar(ImGuiStyleVar_WindowRounding, _component.background);
	set_StyleColor(ImGuiCol_PopupBg, _component.background);
	set_StyleColor(ImGuiCol_Button,
				   ImVec4{0.0f, 0.0f, 0.0f, _ctx.animation->fade});
	set_StyleColor(ImGuiCol_ButtonHovered, (ImVec4)col);
	set_StyleColor(ImGuiCol_Text,
				   ImVec4{1.0f, 1.0f, 1.0f, _ctx.animation->fade});

	set_Font(_ctx.ui->fontstore->get_current_font(_component.font).value());
	if (show)
		ImGui::OpenPopup(CSTR(_id));

	with_Popup(CSTR(_id)) {
		const auto p_min{ImGui::GetWindowPos()};
		const auto p_max{ImVec2{ImGui::GetWindowPos().x + width,
								ImGui::GetWindowPos().y + height}};

		_ctx.ui->draw_frame(p_min, p_max,
							ImVec4{_ctx.ui->ui_colour.x, _ctx.ui->ui_colour.y,
								   _ctx.ui->ui_colour.z, _ctx.animation->fade},
							rounding);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-security"
		ImGui::SetCursorPos(ImVec2{grid_sz * 2, grid_sz * 2});
		ImGui::Text(text.c_str());
#pragma GCC diagnostic pop
	}
}