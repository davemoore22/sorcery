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

#include "gui/frame.hpp"
#include "common/macro.hpp"
#include "core/animation.hpp"
#include "core/context.hpp"
#include "core/define.hpp"
#include "core/system.hpp"
#include "core/ui.hpp"
#include "resources/fontstore.hpp"
#include "resources/stringstore.hpp"
#include "types/component.hpp"

Sorcery::Frame::Frame(Context &ctx, Component *component)
	: _ctx{ctx},
	  _component{component} {

	_pos = ImVec2{_component->x, _component->y};
	_size = Size{_component->w, _component->h};
	_colour = _component->colour;
	_bg_colour = _component->background;
	_name = _component->name;

	if (_component->get("background"))
		_bg_image = _component->get("background").value();
	else
		_bg_image = std::nullopt;
	if (_component->get("title"))
		_title = _component->get("title");
	else
		_title = std::nullopt;

	if (_component->get("foreground")) {
		if (_component->get("foreground").value() == "yes")
			_draw(true);
		else
			_draw(false);
	} else
		_draw(false);
}

Sorcery::Frame::Frame(Context &ctx, std::string_view name, const ImVec2 pos,
					  const Size size, const ImU32 colour,
					  const ImU32 bg_colour)
	: _ctx{ctx},
	  _name{name},
	  _pos{pos},
	  _size{size},
	  _colour{colour},
	  _bg_colour{bg_colour} {

	_bg_image = std::nullopt;
	_title = std::nullopt;
	_draw(false);
}

auto Sorcery::Frame::_draw(const bool foreground) -> void {

	const auto grid_sz{_ctx.ui->grid_sz};
	const auto font_sz{_ctx.ui->font_sz};
	const auto rounding{_ctx.ui->frame_rd};

	const auto outer_id{"##layer_frame_" + _name};

	const auto x{std::invoke([&] {
		if (_pos.x == -1) {
			const auto viewport{ImGui::GetMainViewport()};
			return (viewport->Size.x - grid_sz * _size.w) / 2;
		} else
			return _ctx.ui->adj_grid_w * _pos.x;
	})};
	const auto y{std::invoke([&] {
		if (_pos.y == -1) {
			const auto viewport{ImGui::GetMainViewport()};
			return (viewport->Size.y - grid_sz * _size.h) / 2;
		} else
			return _ctx.ui->adj_grid_h * _pos.y;
	})};

	const auto layer{foreground ? WINDOW_LAYER_TEXTS : WINDOW_LAYER_FRAMES};

	with_Window(layer, nullptr,
				ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs) {

		if (_bg_image) {

			// Optionally draw background
		}

		// Draw a Frame using the Direct Helper function on the Current Window
		_ctx.ui->draw_frame(
			ImVec2{x, y},
			ImVec2{x + (grid_sz * _size.w), y + (grid_sz * _size.h)},
			ImVec4{_ctx.ui->ui_colour.x, _ctx.ui->ui_colour.y,
				   _ctx.ui->ui_colour.z, _ctx.animation->fade},
			rounding);

		if (_title) {

			set_Font(_ctx.ui->fontstore
						 ->get_current_font(Enums::Layout::Font::MONOSPACE)
						 .value());
			const auto title_txt{_ctx.get_string(_title.value())};
			const auto title_sz{
				Size{ImGui::CalcTextSize(title_txt.c_str()).x + (font_sz * 2),
					 grid_sz * 3}};
			const auto title_pos{
				ImVec2{(x + ((grid_sz * _size.w) / 2)) - (title_sz.w / 2),
					   y - grid_sz}};
			const auto text_pos{
				ImVec2{title_pos.x + grid_sz, title_pos.y + grid_sz}};
			_ctx.ui->draw_frame(
				title_pos,
				ImVec2{title_pos.x + title_sz.w, title_pos.y + title_sz.h},
				ImVec4{_ctx.ui->ui_colour.x, _ctx.ui->ui_colour.y,
					   _ctx.ui->ui_colour.z, _ctx.animation->fade},
				rounding);
			_ctx.ui->draw_text(title_txt,
							   ImVec4{1.0f, 1.0f, 1.0f, _ctx.animation->fade},
							   text_pos, Enums::Layout::Font::MONOSPACE);
		}
	}
}
