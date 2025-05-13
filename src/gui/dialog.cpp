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

#include "gui/dialog.hpp"
#include "core/system.hpp"
#include "core/ui.hpp"
#include "types/component.hpp"

Sorcery::Dialog::Dialog(System *system, UI *ui, Component &component,
						const Enums::Layout::DialogType type)
	: _system{system},
	  _ui{ui},
	  _component{component},
	  _type{type} {

	show = false;
	_name = _component.name;
}

auto Sorcery::Dialog::id() const -> std::string {

	return _id;
}

auto Sorcery::Dialog::name() const -> std::string {

	return _name;
}

auto Sorcery::Dialog::display(bool &is_yes) -> void {

	using Enums::Layout::DialogType;

	_id = _component.name + "##outer";
	const auto yes_lbl{(*_system->strings)["DIALOG_YES"]};
	const auto no_lbl{(*_system->strings)["DIALOG_NO"]};
	const auto ok_lbl{(*_system->strings)["DIALOG_OK"]};
	const auto grid_sz{_ui->grid_sz};
	const auto rounding{_ui->frame_rd};
	const auto width{
		ImGui::CalcTextSize((*_system->strings)[_component.string_key].c_str())
			.x +
		(grid_sz * 4)};
	const auto height{_component.h * grid_sz};

	ImVec2 centre{ImGui::GetMainViewport()->GetCenter()};
	ImGui::SetNextWindowPos(centre, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	ImGui::SetNextWindowSize(ImVec2{width, height});
	ImGui::SetNextWindowBgAlpha(1.0f);

	const auto col{_ui->get_hl_colour(_system->animation->lerp)};
	set_StyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
	set_StyleVar(ImGuiStyleVar_WindowBorderSize, 0);
	set_StyleVar(ImGuiStyleVar_WindowRounding, _component.background);
	set_StyleColor(ImGuiCol_PopupBg, _component.background);
	set_StyleColor(ImGuiCol_Button,
				   ImVec4{0.0f, 0.0f, 0.0f, _system->animation->fade});
	set_StyleColor(ImGuiCol_ButtonHovered, (ImVec4)col);
	set_StyleColor(ImGuiCol_Text,
				   ImVec4{1.0f, 1.0f, 1.0f, _system->animation->fade});

	set_Font(_ui->fonts.at(_component.font));
	if (show)
		ImGui::OpenPopup(CSTR(_id));

	with_PopupModal(CSTR(_id), nullptr, ImGuiWindowFlags_NoDecoration) {
		const auto p_min{ImGui::GetWindowPos()};
		const auto p_max{ImVec2{ImGui::GetWindowPos().x + width,
								ImGui::GetWindowPos().y + height}};

		_ui->draw_frame(p_min, p_max,
						ImVec4{1.0f, 1.0f, 1.0f, _system->animation->fade},
						rounding);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-security"
		ImGui::SetCursorPos(ImVec2{grid_sz * 2, grid_sz * 2});
		ImGui::TextWrapped((*_system->strings)[_component.string_key].c_str());
#pragma GCC diagnostic pop

		ImVec2 btn_size{ImGui::GetFontSize() * 7.0f, 0.0f};
		const auto centre{(width / 2)};

		if (_type == DialogType::CONFIRM) {

			ImGui::SetCursorPos(
				ImVec2{centre - (btn_size.x + grid_sz), grid_sz * 4});
			if (ImGui::Button(yes_lbl.c_str(), btn_size)) {
				is_yes = true;
				show = false;
				ImGui::CloseCurrentPopup();
			}
			ImGui::SetCursorPos(ImVec2{centre + grid_sz, grid_sz * 4});
			if (ImGui::Button(no_lbl.c_str(), btn_size)) {
				show = false;
				ImGui::CloseCurrentPopup();
			}
		} else if (_type == DialogType::OK) {

			ImGui::SetCursorPos(ImVec2{centre - (btn_size.x / 2), grid_sz * 4});
			if (ImGui::Button(ok_lbl.c_str(), btn_size)) {
				is_yes = true;
				show = false;
				ImGui::CloseCurrentPopup();
			}
		}
	}
}