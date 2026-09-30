#include "test_assert.hpp"
#include "display/ui/menunavigation.hpp"
#include "imgui.h"
#include <array>
#include <iostream>

namespace {

using Sorcery::Test::require;

// Exercise the production navigation helper with real ImGui frames and input
// events. No platform window, renderer, or user configuration is needed.
class MenuFixture {
	public:
		MenuFixture() {
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			auto &io{ImGui::GetIO()};
			io.IniFilename = nullptr;
			io.DisplaySize = {800.0f, 600.0f};
			io.DeltaTime = 1.0f / 60.0f;
			io.AddMousePosEvent(700.0f, 550.0f);
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
			unsigned char *pixels{};
			int width{};
			int height{};
			io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
		}

		~MenuFixture() {
			ImGui::DestroyContext();
		}
		MenuFixture(const MenuFixture &) = delete;
		auto operator=(const MenuFixture &) -> MenuFixture & = delete;

		int selected{1};
		int activated{-1};
		std::array<bool, 3> disabled{};
		std::array<ImVec2, 3> centers{};

		auto frame(const bool visible = true) -> void {
			activated = -1;
			ImGui::NewFrame();
			if (visible)
				draw_menu();
			ImGui::Render();
		}

		auto settle(const bool visible = true) -> void {
			for (int i{}; i < 4; ++i)
				frame(visible);
		}

		auto key(const ImGuiKey key) -> int {
			ImGui::GetIO().AddKeyEvent(key, true);
			frame();
			const auto result{activated};
			ImGui::GetIO().AddKeyEvent(key, false);
			frame();
			return result;
		}

		auto hover(const int row) -> void {
			ImGui::GetIO().AddMousePosEvent(centers[row].x, centers[row].y);
			settle();
		}

	private:
		auto draw_menu() -> void {
			ImGui::SetNextWindowPos({0.0f, 0.0f});
			ImGui::SetNextWindowSize({500.0f, 400.0f});
			if (ImGui::Begin("Navigation test") && ImGui::BeginListBox("##menu", {350.0f, 150.0f})) {
				draw_rows();
				ImGui::EndListBox();
			}
			ImGui::End();
		}

		auto draw_rows() -> void {
			constexpr std::array labels{"First", "Second", "Third"};
			for (int i{}; i < 3; ++i) {
				const bool is_selected{selected == i};
				ImGui::BeginDisabled(disabled[i]);
				if (ImGui::Selectable(labels[i], is_selected))
					activated = i;
				centers[i] = ImGui::GetItemRectMin();
				centers[i].x += 20.0f;
				centers[i].y += 5.0f;
				if (Sorcery::update_menu_navigation(is_selected, disabled[i]))
					selected = i;
				ImGui::EndDisabled();
			}
		}
};

auto check_initial_and_reopened_focus() -> void {
	MenuFixture menu;
	menu.settle();
	require(menu.key(ImGuiKey_Enter) == 1, "First Enter did not activate the remembered row");
	menu.hover(0);
	menu.settle(false);
	menu.settle();
	require(menu.key(ImGuiKey_Enter) == 0, "Reopened menu required arrow navigation before Enter");
}

auto check_mouse_keyboard_handoff() -> void {
	MenuFixture menu;
	menu.settle();
	require(menu.key(ImGuiKey_DownArrow) == -1, "Arrow navigation activated an item");
	require(menu.selected == 2, "Arrow navigation did not update the selection");
	menu.hover(0);
	require(menu.selected == 0, "Retained keyboard focus overrode an earlier hovered row");
	menu.hover(2);
	require(menu.selected == 2, "Mouse hover did not select a later row");
	menu.key(ImGuiKey_UpArrow);
	require(menu.selected == 1, "Keyboard navigation did not resume after mouse use");
	require(menu.key(ImGuiKey_Enter) == 1, "Enter activated a different row from the keyboard selection");
}

auto check_disabled_rows() -> void {
	MenuFixture menu;
	menu.selected = 0;
	menu.disabled[1] = true;
	menu.settle();
	menu.key(ImGuiKey_DownArrow);
	require(menu.selected == 2, "Keyboard navigation selected a disabled row");
	menu.hover(1);
	require(menu.selected == 2, "Mouse hover selected a disabled row");
}

}

int main() {
	try {
		check_initial_and_reopened_focus();
		check_mouse_keyboard_handoff();
		check_disabled_rows();
		std::cout << "Menu focus and mouse/keyboard handoff checks passed\n";
	} catch (const std::exception &error) {
		std::cerr << error.what() << '\n';
		return 1;
	}
}
