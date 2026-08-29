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

#include "display/ui/uimetrics.hpp" // for UIMetrics
#include "core/context.hpp"			// for Context
#include "display/display.hpp"		// for DisplayMetrics, Display
#include "imgui.h"					// for ImVec2
#include <algorithm>				// for min

Sorcery::UIMetrics::UIMetrics(Context &ctx)
	: _ctx{ctx} {}

auto Sorcery::UIMetrics::update(const DisplayMetrics &metrics) noexcept
	-> void {

	const auto content_w{static_cast<float>(_base_width) * metrics.scale};
	const auto content_h{static_cast<float>(_base_height) * metrics.scale};

	_adj_grid_w = content_w / static_cast<float>(_columns);
	_adj_grid_h = content_h / static_cast<float>(_rows);

	_grid_sz = std::min(_adj_grid_w, _adj_grid_h);

	_base_font_sz =
		static_cast<float>(_base_width) / static_cast<float>(_columns);
	_font_sz = _base_font_sz * metrics.scale;

	_offset_x = metrics.offset_x;
	_offset_y = metrics.offset_y;
}

auto Sorcery::UIMetrics::grid_pos(const float x, const float y) const noexcept
	-> ImVec2 {

	const auto &metrics{_ctx.display->get_display_metrics()};

	return {metrics.offset_x + (x * _adj_grid_w),
			metrics.offset_y + (y * _adj_grid_h)};
}

auto Sorcery::UIMetrics::grid_delta(const float x, const float y) const noexcept
	-> ImVec2 {

	return {x * _adj_grid_w, y * _adj_grid_h};
}

auto Sorcery::UIMetrics::grid_x(const float x) const noexcept -> float {

	const auto &metrics{_ctx.display->get_display_metrics()};
	return metrics.offset_x + (x * _adj_grid_w);
}

auto Sorcery::UIMetrics::grid_y(const float y) const noexcept -> float {

	const auto &metrics{_ctx.display->get_display_metrics()};
	return metrics.offset_y + (y * _adj_grid_h);
}

auto Sorcery::UIMetrics::grid_sz() const noexcept -> unsigned int {

	return _grid_sz;
}

auto Sorcery::UIMetrics::columns() const noexcept -> unsigned int {

	return _columns;
}

auto Sorcery::UIMetrics::rows() const noexcept -> unsigned int {

	return _rows;
}

auto Sorcery::UIMetrics::font_sz() const noexcept -> float {

	return _font_sz;
}

auto Sorcery::UIMetrics::base_font_sz() const noexcept -> float {

	return _base_font_sz;
}
