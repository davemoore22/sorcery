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

#include "types/component.hpp"

Sorcery::Component::Component()
	: form{""},
	  name{""},
	  x{0},
	  y{0},
	  w{0},
	  h{0},
	  font{Enums::Layout::Font::NO_FONT},
	  colour{ImU32{}},
	  animated{false},
	  string_key{""},
	  alpha{255},
	  background{ImU32{}},
	  justification{Enums::Layout::Justification::NO_JUST},
	  type{Enums::Layout::ComponentType::NO_CT},
	  priority{999},
	  drawmode{Enums::Layout::DrawMode::NO_DM},
	  _id{_s_id++} {

	unique_key.clear();
	_data.clear();
	_enabled = false;
	_visible = false;
}

Sorcery::Component::Component(
	const std::string &form_, const std::string &name_, const int _x,
	const int y_, const unsigned int w_, const unsigned int h_,
	const Enums::Layout::Font font_, const ImU32 colour_, const bool animated_,
	const std::string &string_key_, const float alpha_, const ImU32 background_,
	const Enums::Layout::Justification justification_,
	const Enums::Layout::ComponentType _type, const unsigned int priority_,
	const Enums::Layout::DrawMode drawmode_)
	: form{form_},
	  name{name_},
	  x{_x},
	  y{y_},
	  w{w_},
	  h{h_},
	  font{font_},
	  colour{colour_},
	  animated{animated_},
	  string_key{string_key_},
	  alpha{alpha_},
	  background{background_},
	  justification{justification_},
	  type{_type},
	  priority{priority_},
	  drawmode{drawmode_},
	  _id{_s_id++} {

	// Unique Key is like this because it is used for runtime component-sorting
	// (std::map is sorted by key) (also needs to be replaced with std::format
	// when available)
	const auto priority_id{fmt::format("{:03d}", priority)};
	unique_key = fmt::format("{}_{}:{}", priority_id, form, name);
	_data.clear();
	_enabled = true;
	_visible = true;
}

// Overload the [] operator
auto Sorcery::Component::operator[](std::string_view key)
	-> std::optional<std::string> {

	return get(key);
}

auto Sorcery::Component::operator[](std::string_view key) const
	-> std::optional<std::string> {

	return get(key);
}

auto Sorcery::Component::set(std::string_view key, std::string_view value)
	-> void {

	auto it{std::ranges::find_if(_data.begin(), _data.end(), [&key](auto item) {
		return item.first == key;
	})};
	if (it == _data.end())
		_data.emplace_back(key, value);
}

auto Sorcery::Component::set_enabled(bool value) -> void {

	_enabled = value;
}

auto Sorcery::Component::get_enabled() const -> bool {

	return _enabled;
}

auto Sorcery::Component::set_visible(bool value) -> void {

	_visible = value;
}

auto Sorcery::Component::get_visible() const -> bool {

	return _visible;
}

auto Sorcery::Component::get(std::string_view key) const
	-> std::optional<std::string> {

	if (_data.capacity() == 0)
		return std::nullopt;
	auto it{std::ranges::find_if(_data.begin(), _data.end(), [&key](auto item) {
		return item.first == std::string{key};
	})};

	if (it != _data.end())
		return it->second;
	else
		return std::nullopt;
}

auto Sorcery::Component::id() const -> long {

	return _id;
}
