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

#include <algorithm>
#include <ranges>
#include <regex>
#include <vector>

#include "common/enum.hpp"
#include "common/imgui.hpp"
#include "common/macro.hpp"
#include "common/opengl.hpp"
#include "common/sdl2.hpp"
#include "common/types.hpp"
#include "core/context.hpp"
#include "core/controller.hpp"
#include "core/debug.hpp"
#include "core/enum.hpp"
#include "core/macro.hpp"
#include "core/resources.hpp"
#include "core/system.hpp"
#include "display/animation.hpp"
#include "display/display.hpp"
#include "display/render.hpp"
#include "display/ui/popupstore.hpp"
#include "display/ui/ui.hpp"
#include "display/ui/uimetrics.hpp"
#include "display/ui/uistyle.hpp"
#include "drawables/dialog.hpp"
#include "drawables/frame.hpp"
#include "drawables/input.hpp"
#include "drawables/menu.hpp"
#include "drawables/menubuilder.hpp"
#include "drawables/message.hpp"
#include "drawables/modal.hpp"
#include "drawables/popup.hpp"
#include "drawables/videoplayer.hpp"
#include "engine/define.hpp"
#include "engine/types.hpp"
#include "resources/componentstore.hpp"
#include "resources/filestore.hpp"
#include "resources/fontstore.hpp"
#include "resources/imagestore.hpp"
#include "resources/itemstore.hpp"
#include "resources/levelstore.hpp"
#include "resources/monsterstore.hpp"
#include "resources/spellstore.hpp"
#include "resources/stringstore.hpp"
#include "types/component.hpp"
#include "types/config.hpp"
#include "types/enum.hpp"
#include "types/error.hpp"
#include "types/game.hpp"
#include "types/image.hpp"
#include "types/meta.hpp"
#include "types/state.hpp"
#include "types/world/tile.hpp"

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
