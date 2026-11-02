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

#include "display/ui/debugconsole.hpp"
#include "core/diagnostics.hpp"
#include "core/profiler.hpp"
#include "imgui.h"
#include <SDL_events.h>
#include <SDL_scancode.h>
#include <algorithm>
#include <cmath>
#include <format>
#include <string>
#include <string_view>
#include <vector>

namespace {

auto matches_level(Sorcery::LogLevel level, int filter) -> bool {

	using Sorcery::LogLevel;

	switch (filter) {
	case 1:
		return level == LogLevel::DEBUG;
	case 2:
		return level == LogLevel::INFO;
	case 3:
		return level == LogLevel::WARNING;
	case 4:
		return level == LogLevel::ERROR;
	default:
		return true;
	}
}

auto level_name(Sorcery::LogLevel level) -> const char * {

	using Sorcery::LogLevel;

	switch (level) {
	case LogLevel::DEBUG:
		return "DEBUG";
	case LogLevel::INFO:
		return "INFO";
	case LogLevel::WARNING:
		return "WARN";
	case LogLevel::ERROR:
		return "ERROR";
	}

	return "UNKNOWN";
}

auto matches_search(std::string_view message, std::string_view search) -> bool {

	return search.empty() || message.find(search) != std::string_view::npos;
}

}

auto Sorcery::DebugConsole::handle_event(const SDL_Event &event) -> bool {

	const auto console_available{Diagnostics::instance().debug_enabled() || Profiler::instance().available()};
	if (!console_available)
		return false;

	// Toggle using the physical grave/backtick key.
	if (event.type == SDL_KEYDOWN && event.key.repeat == 0 && event.key.keysym.scancode == SDL_SCANCODE_GRAVE) {

		_visible = !_visible;
		return true;
	}

	if (!_visible)
		return false;

	// Escape closes the console, not the current game screen.
	if (event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {

		_visible = false;
		return true;
	}

	// Prevent game input while the console is open.
	switch (event.type) {
	case SDL_KEYDOWN:
		[[fallthrough]];
	case SDL_KEYUP:
		[[fallthrough]];
	case SDL_TEXTINPUT:
		[[fallthrough]];
	case SDL_TEXTEDITING:
		[[fallthrough]];
	case SDL_MOUSEBUTTONDOWN:
		[[fallthrough]];
	case SDL_MOUSEBUTTONUP:
		[[fallthrough]];
	case SDL_MOUSEWHEEL:
		[[fallthrough]];
	case SDL_MOUSEMOTION:
		[[fallthrough]];
	case SDL_CONTROLLERBUTTONDOWN:
		[[fallthrough]];
	case SDL_CONTROLLERBUTTONUP:
		[[fallthrough]];
	case SDL_CONTROLLERAXISMOTION:
		return true;

	default:
		return false;
	}
}

auto Sorcery::DebugConsole::_draw_log() -> void {

	auto &diagnostics{Diagnostics::instance()};

	// Filters
	ImGui::SetNextItemWidth(130.0f);

	constexpr const char *levels[]{"All", "Debug", "Info", "Warning", "Error"};

	ImGui::Combo("Level", &_level_filter, levels, 5);

	ImGui::SameLine();

	ImGui::SetNextItemWidth(250.0f);
	ImGui::InputTextWithHint("##search", "Search messages...", _search, sizeof(_search));

	ImGui::SameLine();
	ImGui::Checkbox("Auto-scroll", &_auto_scroll);

	ImGui::Separator();

	// Snapshot of the diagnostics buffer.
	const auto entries{diagnostics.entries()};

	if (ImGui::BeginChild("##log_messages", ImVec2{0.0f, -ImGui::GetFrameHeightWithSpacing() - 8.0f},
						  ImGuiChildFlags_Borders, ImGuiWindowFlags_HorizontalScrollbar)) {

		const bool was_at_bottom{ImGui::GetScrollY() >= ImGui::GetScrollMaxY() - 1.0f};

		const std::string_view search{_search};

		for (const auto &entry : entries) {

			if (!matches_level(entry.level, _level_filter))
				continue;

			if (!matches_search(entry.message, search))
				continue;

			ImGui::TextDisabled("[%s]", level_name(entry.level));
			ImGui::SameLine();

			ImGui::TextUnformatted(entry.message.c_str());

			if (ImGui::IsItemHovered()) {
				ImGui::BeginTooltip();

				ImGui::Text("%s:%u", entry.location.file_name(), static_cast<unsigned>(entry.location.line()));

				ImGui::Separator();

				ImGui::TextUnformatted(entry.location.function_name());

				ImGui::EndTooltip();
			}
		}

		if (_auto_scroll && was_at_bottom)
			ImGui::SetScrollHereY(1.0f);
	}

	ImGui::EndChild();

	if (ImGui::Button("Copy Log")) {

		std::string text;

		for (const auto &entry : entries) {

			if (!matches_level(entry.level, _level_filter))
				continue;

			if (!matches_search(entry.message, _search))
				continue;

			text += std::format("[{}] [{}:{}] {}\n", level_name(entry.level), entry.location.file_name(),
								entry.location.line(), entry.message);
		}

		ImGui::SetClipboardText(text.c_str());
	}

	ImGui::SameLine();

	if (ImGui::Button("Clear Log"))
		diagnostics.clear();

	ImGui::SameLine();

	ImGui::TextDisabled("%zu messages", entries.size());
}

// TODO: convert over to using ImguiSugar
auto Sorcery::DebugConsole::draw() -> void {

	// The console is only available in debug mode for now
	const auto console_available{Diagnostics::instance().debug_enabled() || Profiler::instance().available()};
	if (!console_available)
		return;

	// Toggle with the grave/backtick key
	if (ImGui::IsKeyPressed(ImGuiKey_GraveAccent, false))
		_visible = !_visible;

	if (!_visible)
		return;

	// Use a second window
	const auto *viewport{ImGui::GetMainViewport()};
	ImGui::SetNextWindowPos(ImVec2{viewport->Pos.x + viewport->Size.x + 20.0f, viewport->Pos.y},
							ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2{800.0f, 500.0f}, ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowBgAlpha(0.95f);

	constexpr auto flags{ImGuiWindowFlags_NoCollapse};

	if (!ImGui::Begin("Sorcery Developer Console", &_visible, flags)) {
		ImGui::End();
		return;
	}

	if (ImGui::BeginTabBar("##diagnostics_tabs")) {

		if (ImGui::BeginTabItem("Log")) {
			_draw_log();
			ImGui::EndTabItem();
		}

		if (Profiler::instance().available()) {
			if (ImGui::BeginTabItem("Profiler")) {
				_draw_profiler();
				ImGui::EndTabItem();
			}
		}

		ImGui::EndTabBar();
	}

	ImGui::End(); // Close Sorcery Developer Console
}

auto Sorcery::DebugConsole::_draw_profiler() -> void {

	auto &profiler{Profiler::instance()};

	// Profiler controls
	if (ImGui::Button(profiler.paused() ? "Resume" : "Pause"))
		profiler.set_paused(!profiler.paused());

	ImGui::SameLine();

	if (ImGui::Button("Reset Statistics"))
		profiler.clear();

	ImGui::SameLine();
	ImGui::TextDisabled("%s", profiler.paused() ? "Paused" : "Recording");

	ImGui::Separator();

	auto entries{profiler.entries()};

	const auto timing_text{[](double us) -> std::string {
		if (us >= 1'000'000.0)
			return std::format("{:.3f} s", us / 1'000'000.0);

		if (us >= 1'000.0)
			return std::format("{:.3f} ms", us / 1'000.0);

		return std::format("{:.2f} us", us);
	}};

	constexpr auto flags{ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable |
						 ImGuiTableFlags_Reorderable | ImGuiTableFlags_Sortable | ImGuiTableFlags_ScrollY |
						 ImGuiTableFlags_SizingStretchProp};

	if (ImGui::BeginTable("##profiler_table", 7, flags, ImVec2{0.0f, 0.0f})) {

		ImGui::TableSetupColumn("Scope", ImGuiTableColumnFlags_DefaultSort, 2.5f);

		ImGui::TableSetupColumn("Calls", 0, 0.8f);
		ImGui::TableSetupColumn("Last", 0, 1.1f);
		ImGui::TableSetupColumn("Average", 0, 1.1f);
		ImGui::TableSetupColumn("Minimum", 0, 1.1f);
		ImGui::TableSetupColumn("Maximum", 0, 1.1f);
		ImGui::TableSetupColumn("Total", 0, 1.1f);

		ImGui::TableSetupScrollFreeze(0, 1);
		ImGui::TableHeadersRow();

		if (const auto *sort_specs{ImGui::TableGetSortSpecs()}; sort_specs && sort_specs->SpecsCount > 0) {

			const auto &spec{sort_specs->Specs[0]};

			const bool ascending{spec.SortDirection == ImGuiSortDirection_Ascending};

			const auto less{[&](const ProfileEntry &a, const ProfileEntry &b) -> bool {
				switch (spec.ColumnIndex) {
				case 0:
					return a.name < b.name;
				case 1:
					return a.count < b.count;
				case 2:
					return a.last_us < b.last_us;
				case 3:
					return a.average_us() < b.average_us();
				case 4:
					return a.min_us < b.min_us;
				case 5:
					return a.max_us < b.max_us;
				case 6:
					return a.total_us < b.total_us;
				default:
					return a.name < b.name;
				}
			}};

			std::ranges::stable_sort(entries, [&](const ProfileEntry &a, const ProfileEntry &b) -> bool {
				if (ascending)
					return less(a, b);

				return less(b, a);
			});
		}

		for (const auto &entry : entries) {

			ImGui::TableNextRow();

			ImGui::TableSetColumnIndex(0);
			ImGui::TextUnformatted(entry.name.c_str());

			ImGui::TableSetColumnIndex(1);
			ImGui::Text("%llu", static_cast<unsigned long long>(entry.count));

			ImGui::TableSetColumnIndex(2);
			ImGui::TextUnformatted(timing_text(entry.last_us).c_str());

			ImGui::TableSetColumnIndex(3);
			ImGui::TextUnformatted(timing_text(entry.average_us()).c_str());

			ImGui::TableSetColumnIndex(4);
			ImGui::TextUnformatted(timing_text(entry.min_us).c_str());

			ImGui::TableSetColumnIndex(5);
			ImGui::TextUnformatted(timing_text(entry.max_us).c_str());

			ImGui::TableSetColumnIndex(6);
			ImGui::TextUnformatted(timing_text(entry.total_us).c_str());
		}

		ImGui::EndTable();
	}
}
