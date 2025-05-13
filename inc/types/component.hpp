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

#include "common/define.hpp"
#include "common/include.hpp"
#include "types/define.hpp"
#include "types/enum.hpp"
#include "types/include.hpp"

namespace Sorcery {

class Component {

	public:
		// Constructors
		Component(const std::string &form_, const std::string &name_,
				  const int _x, const int y_, const unsigned int w_,
				  const unsigned int h_, const Enums::Layout::Font font_,
				  const ImU32 colour_, const bool animated_,
				  const std::string &string_key_, const float alpha_,
				  const ImU32 background_,
				  const Enums::Layout::Justification justification_,
				  const Enums::Layout::ComponentType _type,
				  const unsigned int priority_,
				  const Enums::Layout::DrawMode drawmode_);
		Component();

		auto operator[](std::string_view key) -> std::optional<std::string>;
		auto operator[](std::string_view key) const
			-> std::optional<std::string>;

		auto set(std::string_view key, std::string_view value) -> void;
		auto set_enabled(bool value) -> void;
		auto get_enabled() const -> bool;
		auto set_visible(bool value) -> void;
		auto get_visible() const -> bool;
		auto id() const -> long;

		std::string form;
		std::string name;
		int x;
		int y;
		unsigned int w;
		unsigned int h;
		Enums::Layout::Font font;
		ImU32 colour;
		bool animated;
		std::string string_key;
		float alpha;
		ImU32 background;
		Enums::Layout::Justification justification;
		Enums::Layout::ComponentType type;
		unsigned int priority;
		Enums::Layout::DrawMode drawmode;
		std::string unique_key;

	private:
		auto get(std::string_view key) const -> std::optional<std::string>;

		bool _enabled;
		bool _visible;
		std::vector<std::pair<std::string, std::string>> _data;
		long _id;
		static inline long _s_id{0};
};

}