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
#include "imgui.h"
#include <SDL_events.h>
#include <SDL_scancode.h>
#include <format>
#include <string>
#include <string_view>

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

	if (!Diagnostics::instance().debug_enabled())
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

auto Sorcery::DebugConsole::draw() -> void {

	auto &diagnostics{Diagnostics::instance()};

	// The console is only available in debug mode for now
	if (!diagnostics.debug_enabled())
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

	ImGui::End();
}
