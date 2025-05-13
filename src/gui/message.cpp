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

#include "gui/message.hpp"
#include "core/system.hpp"
#include "core/ui.hpp"
#include "types/component.hpp"

Sorcery::Message::Message(System *system, UI *ui, Component &component)
	: _system{system},
	  _ui{ui},
	  _component{component} {

	show = false;
	_str = "NONE";
	_event_id = Enums::Map::Event::NO_EVENT;
	_name = _component.name;
}

auto Sorcery::Message::set(std::vector<std::string> strings,
						   const Enums::Map::Event event_id) -> void {

	_strings = strings;
	_event_id = event_id;
}

auto Sorcery::Message::id() const -> std::string {

	return _id;
}

auto Sorcery::Message::name() const -> std::string {

	return _name;
}

auto Sorcery::Message::display(bool &is_yes) -> void {

	_id = _component.name + "##outer";
	const auto continue_lbl{(*_system->strings)["MESSAGE_CONTINUE"]};
	const auto grid_sz{_ui->grid_sz};
	const auto rounding{_ui->frame_rd};
	const auto width{grid_sz * _component.w};
	const auto height{(6 + _strings.size()) * grid_sz};

	// Work out where to draw the image
	const auto y{std::invoke([&] {
		if (_component.y == -1) {
			return 0.5f;
		} else
			return static_cast<float>(_ui->grid_sz * _component.y);
	})};
	const auto pivot_y{std::invoke([&] {
		if (_component.y == -1) {
			return 0.5f;
		} else
			return 0.0f;
	})};

	ImVec2 centre{ImGui::GetMainViewport()->GetCenter()};
	ImGui::SetNextWindowPos(ImVec2{centre.x, y}, ImGuiCond_Appearing,
							ImVec2(0.5f, pivot_y));
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
		auto y_pos{grid_sz * 2};
		for (const auto &string : _strings) {
			ImGui::SetCursorPos(ImVec2{grid_sz * 2, y_pos});
			auto str{_system->strings->get(string)};
			ImGui::Text(str.c_str());
			y_pos += grid_sz;
		}
#pragma GCC diagnostic pop

		ImVec2 btn_size{ImGui::GetFontSize() * 8.0f, 0.0f};
		const auto centre{(width / 2)};
		ImGui::SetCursorPos(
			ImVec2{centre - (btn_size.x / 2), (3 + _strings.size()) * grid_sz});
		if (ImGui::Button(continue_lbl.c_str(), btn_size)) {
			is_yes = true;
			show = false;
			ImGui::CloseCurrentPopup();
		}
	}
}