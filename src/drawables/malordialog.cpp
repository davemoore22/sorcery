#include "drawables/malordialog.hpp"

#include "core/context.hpp"
#include "display/animation.hpp"
#include "display/ui/ui.hpp"
#include "display/ui/uimetrics.hpp"
#include "display/ui/uistyle.hpp"
#include "imgui.h"
#include "resources/fontstore.hpp"
#include "types/component.hpp"

#include <imgui_sugar.hpp>

Sorcery::MalorDialog::MalorDialog(Context &ctx)
	: Drawable{ctx} {}

auto Sorcery::MalorDialog::build(Component &component) -> void {

	Drawable::build(component);

	_destination = {};
}

auto Sorcery::MalorDialog::destination() const -> Magic::MalorDestination {

	return _destination;
}

auto Sorcery::MalorDialog::display() -> void {

	if (!is_open() || !_component)
		return;

	const auto grid{static_cast<float>(_ctx.ui->metrics->grid_sz())};
	const auto rounding{_ctx.ui->frame_rd};
	const auto width{static_cast<float>(_component->w) * grid};
	const auto height{static_cast<float>(_component->h) * grid};
	const auto centre{ImGui::GetMainViewport()->GetCenter()};

	set_Font(_ctx.ui->fonts->get_current_font(_component->font).value(), _ctx.ui->metrics->font_sz());

	ImGui::SetNextWindowPos(centre, ImGuiCond_Always, ImVec2{0.5f, 0.5f});
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

		_ctx.ui->draw_frame(
			p_min, p_max,
			ImVec4{_ctx.ui->ui_colour.x, _ctx.ui->ui_colour.y, _ctx.ui->ui_colour.z, _ctx.animation->fade},
			ImVec4{_ctx.ui->ui_bg_colour.x, _ctx.ui->ui_bg_colour.y, _ctx.ui->ui_bg_colour.z, _ctx.animation->fade},
			rounding);

		// Title
		constexpr auto title{"MALOR TELEPORT"};
		const auto title_width{ImGui::CalcTextSize(title).x};
		ImGui::SetCursorPos(ImVec2{(width - title_width) / 2.0f, grid * 1.5f});
		ImGui::TextUnformatted(title);

		// Displacement controls.
		const auto label_width{std::max({ImGui::CalcTextSize("NORTH / SOUTH").x, ImGui::CalcTextSize("EAST / WEST").x,
										 ImGui::CalcTextSize("DOWN / UP").x})};

		const auto gap{grid * 1.5f};

		// Fixed width for the number field + its +/- buttons
		const auto input_width{grid * 7.0f};
		const auto controls_width{label_width + gap + input_width};
		const auto label_x{(width - controls_width) / 2.0f};
		const auto input_x{label_x + label_width + gap};

		const auto draw_input = [&](const char *label, const char *id, int &value, const float y) {
			ImGui::SetCursorPos(ImVec2{label_x, y});
			ImGui::TextUnformatted(label);
			ImGui::SetCursorPos(ImVec2{input_x, y});
			ImGui::SetNextItemWidth(input_width);
			ImGui::InputInt(id, &value, 1, 10);
		};

		draw_input("NORTH / SOUTH", "##malor_ns", _destination.north, grid * 3.5f);
		draw_input("EAST / WEST", "##malor_ew", _destination.east, grid * 5.0f);
		draw_input("DOWN / UP", "##malor_ud", _destination.down, grid * 6.5f);

		// Help text
		constexpr auto help_1{"+ = NORTH / EAST / DOWN"};
		constexpr auto help_2{"- = SOUTH / WEST / UP  "};

		const auto help_width{ImGui::CalcTextSize(help_1).x};
		ImGui::SetCursorPos(ImVec2{(width - help_width) / 2.0f, grid * 8.0f});
		ImGui::TextUnformatted(help_1);
		ImGui::SetCursorPos(ImVec2{(width - help_width) / 2.0f, grid * 9.0f});
		ImGui::TextUnformatted(help_2);

		// Buttons
		const ImVec2 btn_size{ImGui::GetFontSize() * 8.0f, 0.0f};
		const auto button_y{height - (grid * 2.0f) - ImGui::GetFrameHeight()};
		const auto button_centre{width / 2.0f};

		ImGui::SetCursorPos(ImVec2{button_centre - btn_size.x - (grid * 0.5f), button_y});
		if (ImGui::Button("TELEPORT", btn_size)) {
			close(DrawableResult::ACCEPTED);
			ImGui::CloseCurrentPopup();
		}

		ImGui::SetCursorPos(ImVec2{button_centre + (grid * 0.5f), button_y});
		if (ImGui::Button("CANCEL", btn_size)) {
			close(DrawableResult::CANCELLED);
			ImGui::CloseCurrentPopup();
		}
	}
}