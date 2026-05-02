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

#include "gui/input.hpp"
#include "core/animation.hpp"
#include "core/context.hpp"
#include "core/controller.hpp"
#include "core/macro.hpp"
#include "core/system.hpp"
#include "core/ui.hpp"
#include "gui/dialog.hpp"
#include "gui/frame.hpp"
#include "resources/stringstore.hpp"
#include "types/component.hpp"
#include "types/game.hpp"

Sorcery::Input::Input(Context &ctx, Component &component)
	: _ctx{ctx},
	  _component{component} {

	show = false;

	_width = _component.w;
	_height = _component.h;
	_colour = _component.colour;
	_bg_colour = _component.background;
	_hi_colour = _component.get_float("highlight");
	_font = _component.font;
	_title = _ctx.get_string(_component.string_key);
	_input = "";
	_input_width = std::stoi(_component.get("input_width").value());
	_game = nullptr;
	_name = _component.name;
}

auto Sorcery::Input::id() const -> std::string {

	return _id;
}

auto Sorcery::Input::name() const -> std::string {

	return _name;
}

auto Sorcery::Input::initialise(Game *game) -> void {

	_game = game;
}

auto Sorcery::Input::set(const std::string value) -> void {

	_input = value;
}

auto Sorcery::Input::get() const -> std::string {

	return _input;
}

auto Sorcery::Input::display([[maybe_unused]] bool &is_yes) -> void {

	_id = _component.name + "##input";

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

	if (show)
		ImGui::OpenPopup(CSTR(_id));

	with_PopupModal(CSTR(_id), nullptr,
					ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove) {

		const auto p_min{ImGui::GetWindowPos()};
		const auto p_max{ImVec2{ImGui::GetWindowPos().x + width,
								ImGui::GetWindowPos().y + height}};

		const auto col{_ctx.ui->get_hl_colour(_ctx.animation->lerp)};
		const auto sz{
			ImVec2{static_cast<float>((_width + 2) * _ctx.ui->font_sz),
				   static_cast<float>(_height * _ctx.ui->font_sz)}};

		_ctx.ui->draw_frame(p_min, p_max,
							ImVec4{_ctx.ui->ui_colour.x, _ctx.ui->ui_colour.y,
								   _ctx.ui->ui_colour.z, _ctx.animation->fade},
							rounding);

		const auto title{_ctx.get_string(_component.string_key)};
		auto centre_x{(((_width + 4) / 2) - (title.length() / 2)) * grid_sz};
		_ctx.ui->draw_text(_ctx.get_string(_component.string_key),
						   ImVec4{1.0f, 1.0f, 1.0f, _ctx.animation->fade},
						   ImVec2{centre_x, grid_sz}, _font);

		auto input_y{3 * grid_sz};
		auto input_x{(((_width + 4) / 2) - (_input_width / 2)) * grid_sz};
		ImGui::SetCursorPos(ImVec2{input_x, input_y});
		with_ItemWidth(ImGui::GetFontSize() * _input_width) {
			ImGui::InputText("##input_text", &_input,
							 ImGuiInputTextFlags_CharsDecimal);
		}

		ImVec2 btn_size{ImGui::GetFontSize() * 7.0f, 0.0f};
		const auto centre{(width / 2)};

		const auto ok_lbl{_ctx.get_string("INPUT_OK")};
		ImGui::SetCursorPos(ImVec2{centre - (btn_size.x / 2), grid_sz * 5});
		if (ImGui::Button(ok_lbl.c_str(), btn_size)) {
			is_yes = true;
			show = false;
			if (_input.length() == 0) {

				// Abort
			} else {

				// Also todo: move this into controller (need to pass in
				// reference to the input flags)
				if (_component.name == "input_donate") {

					auto &character{_game->characters.at(
						_ctx.controller->get_character("tithe"))};
					const unsigned int amount{std::stoi(_input)};
					if (amount > character.get_gold()) {

						// Too much!
						_ctx.controller->unset_flag("want_tithe");
						_ctx.controller->unset_flag("want_gold");
						_ctx.controller->set_flag("want_not_enough_gold");
						_ctx.ui->notice_not_enough_gold->show = true;
						_ctx.controller->clear_character("tithe");
					} else {

						_ctx.controller->unset_flag("want_tithe");
						_ctx.controller->unset_flag("want_gold");
						_ctx.controller->set_flag("want_donated_ok");
						_ctx.controller->clear_character("tithe");
						character.grant_xp(amount);
						character.grant_gold(0 - amount);
						_ctx.ui->notice_donated_ok->show = true;
						_ctx.controller->clear_character("tithe");

						// TODO: Check highest level XP and only allow tithing
						// up to that amount!
					}
				}
			}

			ImGui::CloseCurrentPopup();
		}
	}
}
