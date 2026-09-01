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

#include "drawables/message.hpp"
#include "common/enum.hpp"			// for Event
#include "core/context.hpp"			// for Context
#include "display/animation.hpp"	// for Animation
#include "display/ui/ui.hpp"		// for UI
#include "display/ui/uimetrics.hpp" // for UIMetrics
#include "display/ui/uistyle.hpp"	// for set_faded
#include "imgui.h"					// for ImVec2, ImVec4, ImGuiViewport
#include "resources/fontstore.hpp"	// for FontStore
#include "types/component.hpp"		// for Component
#include <algorithm>				// for max
#include <cmath>					// for round
#include <imgui_sugar.hpp>			// for BooleanGuard, set_StyleVar, with...
#include <memory>					// for unique_ptr
#include <optional>					// for optional

Sorcery::Message::Message(Context &ctx)
	: Drawable{ctx},
	  _event_id{Enums::Map::Event::NO_EVENT} {}

auto Sorcery::Message::build(Component &component) -> void {

	Drawable::build(component);

	_strings.clear();
	_event_id = Enums::Map::Event::NO_EVENT;
}

auto Sorcery::Message::set(std::vector<std::string> strings,
						   const Enums::Map::Event event_id) -> void {

	_strings = std::move(strings);
	_event_id = event_id;
}

auto Sorcery::Message::display() -> void {

	if (!is_open() || !_component)
		return;

	const auto continue_lbl{_ctx.get_string("MESSAGE_CONTINUE")};
	const auto rounding{_ctx.ui->frame_rd};
	const auto grid{_ctx.ui->metrics->grid_sz()};

	set_Font(_ctx.ui->fonts->get_current_font(_component->font).value(),
			 _ctx.ui->metrics->font_sz());

	//
	// Work out the size of the text frame
	//
	auto text_width{0.0f};

	for (const auto &key : _strings) {

		const auto text{_ctx.get_string(key)};

		text_width = std::max(text_width, ImGui::CalcTextSize(text.c_str()).x);
	}

	const auto text_padding{grid * 2.0f};

	const auto message_width{text_width + (text_padding * 2.0f)};

	// One spare line at the bottom before the continue frame overlaps it.
	const auto message_height{(static_cast<float>(_strings.size() + 1) * grid) +
							  (text_padding * 2.0f)};

	//
	// Continue button/frame
	//
	const auto label_size{ImGui::CalcTextSize(continue_lbl.c_str())};

	const auto actual_button_size{
		ImVec2{label_size.x + (ImGui::GetStyle().FramePadding.x * 2.0f),
			   label_size.y + (ImGui::GetStyle().FramePadding.y * 2.0f)}};

	const auto button_width{actual_button_size.x + (grid * 2.0f)};

	const auto button_height{grid * 3.0f};

	const auto overlap{grid * 2.0f};

	const auto total_width{std::max(message_width, button_width)};

	const auto total_height{message_height + button_height - overlap};

	//
	// Centre the complete message + continue frame
	//
	const auto viewport{ImGui::GetMainViewport()};

	const auto pos{ImVec2{
		std::round(viewport->Pos.x + ((viewport->Size.x - total_width) * 0.5f)),
		std::round(viewport->Pos.y +
				   ((viewport->Size.y - total_height) * 0.5f))}};

	//
	// Draw modal-style background dimming
	//
	{
		const auto dim_colour{
			ImGui::GetStyleColorVec4(ImGuiCol_ModalWindowDimBg)};

		ImGui::SetNextWindowPos(viewport->Pos, ImGuiCond_Always);

		ImGui::SetNextWindowSize(viewport->Size, ImGuiCond_Always);

		ImGui::SetNextWindowBgAlpha(dim_colour.w);

		set_StyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0.0f, 0.0f});

		set_StyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

		set_StyleColor(ImGuiCol_WindowBg, dim_colour);

		with_Window("##message_dim", nullptr,
					ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
						ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoNav |
						ImGuiWindowFlags_NoSavedSettings |
						ImGuiWindowFlags_NoBringToFrontOnFocus) {}
	}

	//
	// Draw message
	//
	ImGui::SetNextWindowPos(pos, ImGuiCond_Always);

	ImGui::SetNextWindowSize(ImVec2{total_width, total_height});

	ImGui::SetNextWindowBgAlpha(0.0f);

	set_StyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0.0f, 0.0f});

	set_StyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

	UIStyle::set_faded(_ctx);

	with_Window("##message", nullptr,
				ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
					ImGuiWindowFlags_NoSavedSettings) {

		//
		// Message frame
		//
		const auto message_x{std::round((total_width - message_width) * 0.5f)};

		const auto message_min{ImVec2{ImGui::GetWindowPos().x + message_x,
									  ImGui::GetWindowPos().y}};

		const auto message_max{ImVec2{message_min.x + message_width,
									  message_min.y + message_height}};

		_ctx.ui->draw_frame(message_min, message_max,
							ImVec4{_ctx.ui->ui_colour.x, _ctx.ui->ui_colour.y,
								   _ctx.ui->ui_colour.z, _ctx.animation->fade},
							rounding);

		//
		// Message text
		//
		auto y_pos{text_padding};

		for (const auto &key : _strings) {

			const auto text{_ctx.get_string(key)};
			const auto size{ImGui::CalcTextSize(text.c_str())};

			ImGui::SetCursorPos(
				ImVec2{std::round((total_width - size.x) * 0.5f), y_pos});

			ImGui::TextUnformatted(text.c_str());

			y_pos += grid;
		}

		//
		// Continue frame
		//
		const auto button_y{message_height - overlap};

		const auto button_x{std::round((total_width - button_width) * 0.5f)};

		const auto button_min{ImVec2{ImGui::GetWindowPos().x + button_x,
									 ImGui::GetWindowPos().y + button_y}};

		const auto button_max{
			ImVec2{button_min.x + button_width, button_min.y + button_height}};

		_ctx.ui->draw_frame(button_min, button_max,
							ImVec4{_ctx.ui->ui_colour.x, _ctx.ui->ui_colour.y,
								   _ctx.ui->ui_colour.z, _ctx.animation->fade},
							rounding);

		//
		// Natural-sized Continue button centred within its frame
		//
		const auto actual_button_x{std::round(
			button_x + ((button_width - actual_button_size.x) * 0.5f))};

		const auto actual_button_y{std::round(
			button_y + ((button_height - actual_button_size.y) * 0.5f))};

		ImGui::SetCursorPos(ImVec2{actual_button_x, actual_button_y});

		if (ImGui::Button(continue_lbl.c_str(), actual_button_size))
			close();
	}
}