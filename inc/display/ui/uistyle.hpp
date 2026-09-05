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

#pragma once

#include "common/imgui.hpp"
#include "core/context.hpp"
#include "core/define.hpp"
#include "display/animation.hpp"
#include "display/ui/imageeffect.hpp"

namespace Sorcery {

namespace UIStyle {

	// RAII shortcuts for Imgui Styles
	inline void set_faded(Context &ctx) {
		const float a{ctx.animation->fade};
		set_StyleColor(ImGuiCol_Text, ImVec4{1.0f, 1.0f, 1.0f, a});
		set_StyleColor(ImGuiCol_Button, ImVec4{0.0f, 0.0f, 0.0f, a});
	}

	inline void set_faded_with_disabled(Context &ctx) {
		const float a{ctx.animation->fade};
		set_StyleColor(ImGuiCol_Text, ImVec4{1.0f, 1.0f, 1.0f, a});
		set_StyleColor(ImGuiCol_TextDisabled, ImVec4{0.5f, 0.5f, 0.5f, a});
		set_StyleColor(ImGuiCol_Button, ImVec4{0.0f, 0.0f, 0.0f, a});
	}

	inline void set_text_bright(Context &ctx) {
		const float a{ctx.animation->fade};
		set_StyleColor(ImGuiCol_Text, ImVec4{1.0f, 1.0f, 1.0f, a});
	}

	inline void set_text_dark(Context &ctx) {
		const float a{ctx.animation->fade};
		set_StyleColor(ImGuiCol_Text, ImVec4{0.5f, 0.5f, 0.5f, a});
	}

	inline void set_text_dim(Context &ctx) {
		const float a{ctx.animation->fade};
		set_StyleColor(ImGuiCol_Text, ImVec4{0.8f, 0.8f, 0.8f, a});
	}

	inline void set_tab_black(Context &ctx) {
		const float a{ctx.animation->fade};
		set_StyleColor(ImGuiCol_Tab, ImVec4{0.0f, 0.0f, 0.0f, a});
	}

	[[nodiscard]]
	inline auto icon_depth(const float scale) -> ImageOffsetEffect {

		return {.offset = ImVec2{2.0f * scale, 2.0f * scale},
				.brightness = 0.4f,
				.alpha = 0.8f,
				.colour = std::nullopt};
	}

	[[nodiscard]]
	inline auto icon_colour(const int icon) -> ImVec4 {

		switch (icon) {

		case ICON_CAMP:
			return {0.20f, 0.55f, 1.00f, 1.0f};

		case ICON_PARTY:
			return {0.15f, 0.85f, 1.00f, 1.0f};

		case ICON_MAP:
			return {0.15f, 1.00f, 0.70f, 1.0f};

		case ICON_LOOK:
			return {0.45f, 0.55f, 1.00f, 1.0f};

		case ICON_CAST:
			return {0.75f, 0.35f, 1.00f, 1.0f};

		case ICON_USE:
			return {0.75f, 0.75f, 0.75f, 1.0f};

		case ICON_BUFF_EXTRA_LIGHT:
			return {1.00f, 0.90f, 0.45f, 1.0f};

		case ICON_BUFF_LIGHT:
			return {1.00f, 0.55f, 0.15f, 1.0f};

		case ICON_COMPASS_NORTH:
			[[fallthrough]];
		case ICON_COMPASS_SOUTH:
			[[fallthrough]];
		case ICON_COMPASS_EAST:
			[[fallthrough]];
		case ICON_COMPASS_WEST:
			return {0.80f, 0.55f, 0.20f, 1.0f};

		case ICON_SAVE_AND_QUIT:
			return {1.00f, 0.30f, 0.25f, 1.0f};

		default:
			return {0.33f, 1.00f, 1.00f, 1.0f};
		}
	}
}
}