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

#include <string>
#include <string_view>

namespace Sorcery { class Component; }
namespace Sorcery { struct Context; }

namespace Sorcery {

enum class DrawableResult {
	NONE,
	ACCEPTED,
	CANCELLED
};

class Drawable {

	public:
		explicit Drawable(Context &ctx);
		virtual ~Drawable() = default;

		Drawable(const Drawable &) = delete;
		auto operator=(const Drawable &) -> Drawable & = delete;
		Drawable(Drawable &&) = delete;
		auto operator=(Drawable &&) -> Drawable & = delete;

		virtual auto build(Component &component) -> void;
		virtual auto display() -> void = 0;

		auto open() -> void;
		auto close(DrawableResult result = DrawableResult::CANCELLED) -> void;

		[[nodiscard]] auto is_open() const -> bool;
		[[nodiscard]] auto result() const -> DrawableResult;
		[[nodiscard]] auto id() const -> std::string_view;
		[[nodiscard]] auto name() const -> std::string_view;

	protected:
		Context &_ctx;
		Component *_component{};

		std::string _id;
		bool _open{false};
		DrawableResult _result{DrawableResult::NONE};
};

}