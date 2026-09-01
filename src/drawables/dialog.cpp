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

#include "drawables/dialog.hpp"
#include "core/context.hpp"			// for Context
#include "core/macro.hpp"			// for CSTR
#include "display/animation.hpp"	// for Animation
#include "display/ui/ui.hpp"		// for UI
#include "display/ui/uimetrics.hpp" // for UIMetrics
#include "display/ui/uistyle.hpp"	// for set_faded
#include "imgui.h"					// for ImVec2, GetWindowPos, ImVec4
#include "resources/fontstore.hpp"	// for FontStore
#include "types/component.hpp"		// for Component
#include "types/enum.hpp"			// for DialogType, DialogType::CONFIRM
#include <imgui_sugar.hpp>			// for set_StyleVar, BooleanGuard, set_...
#include <memory>					// for unique_ptr
#include <optional>					// for optional

Sorcery::Dialog::Dialog(Context &ctx)
	: Drawable{ctx},
	  _type{Enums::Layout::DialogType::CONFIRM} {}

auto Sorcery::Dialog::build(Component &component) -> void {

	Drawable::build(component);

	_type = Enums::Layout::DialogType::CONFIRM;
}

auto Sorcery::Dialog::build(Component &component,
							const Enums::Layout::DialogType type) -> void {

	build(component);

	_type = type;
}

auto Sorcery::Dialog::display() -> void {

	if (!is_open() || !_component)
		return;

	const auto yes_lbl{_ctx.get_string("DIALOG_YES")};
	const auto no_lbl{_ctx.get_string("DIALOG_NO")};
	const auto ok_lbl{_ctx.get_string("DIALOG_OK")};

	const auto rounding{_ctx.ui->frame_rd};
	const auto grid{_ctx.ui->metrics->grid_sz()};

	set_Font(_ctx.ui->fonts->get_current_font(_component->font).value(),
			 _ctx.ui->metrics->font_sz());

	const auto text{_ctx.get_string(_component->string_key)};

	const auto width{ImGui::CalcTextSize(text.c_str()).x + (grid * 4.0f)};

	const auto height{_component->h * grid};

	const auto centre{ImGui::GetMainViewport()->GetCenter()};

	ImGui::SetNextWindowPos(centre, ImGuiCond_Appearing, ImVec2{0.5f, 0.5f});

	ImGui::SetNextWindowSize(ImVec2{width, height});
	ImGui::SetNextWindowBgAlpha(1.0f);

	const auto col{_ctx.ui->get_hl_colour(_ctx.animation->lerp)};

	set_StyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0.0f, 0.0f});
	set_StyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	set_StyleVar(ImGuiStyleVar_WindowRounding, static_cast<float>(rounding));

	set_StyleColor(ImGuiCol_PopupBg, _component->background);
	set_StyleColor(ImGuiCol_ButtonHovered, static_cast<ImVec4>(col));

	UIStyle::set_faded(_ctx);

	ImGui::OpenPopup(_id.c_str());

	with_PopupModal(_id.c_str(), nullptr, ImGuiWindowFlags_NoDecoration) {

		const auto p_min{ImGui::GetWindowPos()};

		const auto p_max{ImVec2{p_min.x + width, p_min.y + height}};

		_ctx.ui->draw_frame(p_min, p_max,
							ImVec4{_ctx.ui->ui_colour.x, _ctx.ui->ui_colour.y,
								   _ctx.ui->ui_colour.z, _ctx.animation->fade},
							rounding);

		ImGui::SetCursorPos(ImVec2{grid * 2.0f, grid * 2.0f});

		ImGui::TextWrapped("%s", text.c_str());

		const ImVec2 btn_size{ImGui::GetFontSize() * 7.0f, 0.0f};

		const auto button_centre{width / 2.0f};

		using enum Enums::Layout::DialogType;

		if (_type == CONFIRM) {

			ImGui::SetCursorPos(
				ImVec2{button_centre - (btn_size.x + grid), grid * 4.0f});

			if (ImGui::Button(yes_lbl.c_str(), btn_size)) {

				close(DrawableResult::ACCEPTED);
				ImGui::CloseCurrentPopup();
			}

			ImGui::SetCursorPos(ImVec2{button_centre + grid, grid * 4.0f});

			if (ImGui::Button(no_lbl.c_str(), btn_size)) {

				close(DrawableResult::CANCELLED);
				ImGui::CloseCurrentPopup();
			}

		} else if (_type == OK) {

			ImGui::SetCursorPos(
				ImVec2{button_centre - (btn_size.x / 2.0f), grid * 4.0f});

			if (ImGui::Button(ok_lbl.c_str(), btn_size)) {

				close(DrawableResult::ACCEPTED);
				ImGui::CloseCurrentPopup();
			}
		}
	}
}