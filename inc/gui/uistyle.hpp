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

#pragma once

#include "common/include.hpp"
#include "core/animation.hpp"
#include "core/context.hpp"
#include "gui/uistyle.hpp"
#include "types/enum.hpp"

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
}
}