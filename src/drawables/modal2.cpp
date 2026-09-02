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

#include "drawables/modal2.hpp"

#include "core/context.hpp"
#include "core/controller/menubuilder.hpp"
#include "core/macro.hpp"
#include "display/animation.hpp"
#include "display/ui/ui.hpp"
#include "display/ui/uimetrics.hpp"
#include "resources/fontstore.hpp"
#include "types/component.hpp"

#include <imgui_sugar.hpp>

Sorcery::Modal2::Modal2(Context &ctx)
	: Drawable{ctx} {}

auto Sorcery::Modal2::build(Component &component) -> void {

	_build(component, component.get("menu_name").value_or(""));
}

auto Sorcery::Modal2::build(Component &component,
							const std::string_view menu_name) -> void {

	_build(component, menu_name);
}

auto Sorcery::Modal2::_build(Component &component,
							 const std::string_view menu_name) -> void {

	Drawable::build(component);

	// Modal has its own ImGui identity.
	_id = component.name + "##modal";

	// Restore all component-owned state. Modal2 is reusable, so none of the
	// previous incarnation's state should survive build().
	_width = component.w;
	_height = component.h;
	_font = component.font;

	_menu_name = menu_name;

	_title_key = component.string_key;
	_has_title = !_ctx.get_string(_title_key).empty();

	_build_menu();
	_update_height();
}

auto Sorcery::Modal2::_build_menu() -> void {

	_items.clear();
	_data.clear();

	_ctx.menubuilder->build(_menu_name, _width, _items, _data, false);
}

auto Sorcery::Modal2::_update_height() -> void {

	if (!_component)
		return;

	// Always restore the component's configured height first. This is important
	// when Modal2 has previously been used for a dynamically-sized modal.
	_height = _component->h;

	if (!_component->get_bool("dynamic"))
		return;

	const auto frame_rows{3U};
	const auto title_rows{_has_title ? 2U : 0U};

	_height =
		static_cast<unsigned int>(_items.size()) + frame_rows + title_rows;
}

auto Sorcery::Modal2::set_title(const std::string_view key) -> void {

	_title_key = key;
	_has_title = !_ctx.get_string(_title_key).empty();

	_update_height();
}

auto Sorcery::Modal2::display() -> void {

	if (!_component)
		return;

	const auto rounding{_ctx.ui->frame_rd};

	const auto width{(_width + 4) * _ctx.ui->metrics->grid_sz()};

	const auto height{_height * _ctx.ui->metrics->grid_sz()};

	const ImVec2 centre{ImGui::GetMainViewport()->GetCenter()};

	ImGui::SetNextWindowPos(centre, ImGuiCond_Appearing, ImVec2{0.5f, 0.5f});

	ImGui::SetNextWindowSize(ImVec2{width, height});

	ImGui::SetNextWindowBgAlpha(1.0f);

	set_StyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});

	set_StyleVar(ImGuiStyleVar_WindowBorderSize, 0);

	set_StyleVar(ImGuiStyleVar_WindowRounding, rounding);

	set_StyleColor(ImGuiCol_PopupBg, _component->background);

	set_Font(_ctx.ui->fonts->get_current_font(_component->font).value(),
			 _ctx.ui->metrics->font_sz());

	if (is_open())
		ImGui::OpenPopup(CSTR(_id));

	with_PopupModal(CSTR(_id), nullptr,
					ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove) {

		const auto p_min{ImGui::GetWindowPos()};

		const auto p_max{ImVec2{p_min.x + width, p_min.y + height}};

		const auto col{_ctx.ui->get_hl_colour(_ctx.animation->lerp)};

		const auto sz{ImVec2{
			static_cast<float>((_width + 2) * _ctx.ui->metrics->font_sz()),

			static_cast<float>(
				(_items.size() * ImGui::GetTextLineHeightWithSpacing()) + 2)}};

		_ctx.ui->draw_frame(p_min, p_max,
							ImVec4{_ctx.ui->ui_colour.x, _ctx.ui->ui_colour.y,
								   _ctx.ui->ui_colour.z, _ctx.animation->fade},
							rounding);

		const auto top{_has_title ? 3 : 1};

		if (_has_title) {

			const auto title{_ctx.get_string(_title_key)};

			const auto centre_x{(((_width + 4) / 2) - (title.length() / 2)) *
								_ctx.ui->metrics->grid_sz()};

			_ctx.ui->draw_text(
				title, ImVec4{1.0f, 1.0f, 1.0f, _ctx.animation->fade},
				ImVec2{centre_x, _ctx.ui->metrics->grid_sz()}, _font);
		}

		_ctx.ui->draw_menu(_menu_name, col, ImVec2{1, top}, sz, _font, _items,
						   _data, false, false, false);
	}
}