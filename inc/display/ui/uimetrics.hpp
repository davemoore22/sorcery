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

using GLuint = unsigned int;

#include "common/enum.hpp"
#include "common/imgui.hpp"
#include "core/enum.hpp"
#include "engine/enum.hpp"
#include "types/enum.hpp"

#include <any>
#include <array>
#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>

// UI Generation Class (sits on top of, and heavily uses, ImGui)
namespace Sorcery {

class DisplayMetrics;
class Context;

class UIMetrics {

	public:
		explicit UIMetrics(Context &ctx);

		auto update(const DisplayMetrics &display) noexcept -> void;

		[[nodiscard]] auto grid_pos(float x, float y) const noexcept -> ImVec2;
		[[nodiscard]] auto grid_delta(float x, float y) const noexcept
			-> ImVec2;

		[[nodiscard]] auto grid_x(float x) const noexcept -> float;
		[[nodiscard]] auto grid_y(float y) const noexcept -> float;
		[[nodiscard]] auto grid_sz() const noexcept -> unsigned int;

		[[nodiscard]] auto font_sz() const noexcept -> float;
		[[nodiscard]] auto base_font_sz() const noexcept -> float;

		[[nodiscard]] auto columns() const noexcept -> unsigned int;
		[[nodiscard]] auto rows() const noexcept -> unsigned int;

	private:
		Context &_ctx;

		static constexpr unsigned int _columns{60};
		static constexpr unsigned int _rows{35};

		static constexpr unsigned int _base_width{1024};
		static constexpr unsigned int _base_height{600};

		float _adj_grid_w{};
		float _adj_grid_h{};
		float _grid_sz{};

		float _base_font_sz{};
		float _font_sz{};

		float _offset_x{};
		float _offset_y{};
};

};