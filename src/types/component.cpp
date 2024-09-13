// Copyright (C) 2024 Dave Moore
//
// This file is part of Sorcery: Shadows under Llylgamyn.
//
// Sorcery: Shadows under Llylgamyn is free software: you can redistribute
// it and/or modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation, either version 2 of the License,
// or (at your option) any later version.
//
// Sorcery: Shadows under Llylgamyn is distributed in the hope that it wil
// be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
// Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Sorcery: Shadows under Llylgamyn.  If not,
// see <http://www.gnu.org/licenses/>.
//
// If you modify this program, or any covered work, by linking or combining
// it with the libraries referred to in README (or a modified version of
// said libraries), containing parts covered by the terms of said libraries,
// the licensors of this program grant you additional permission to convey
// the resulting work.

#include "types/component.hpp"

Sorcery::Component::Component()
	: screen{""}, name{""}, x{0}, y{0}, w{0}, h{0}, scale{0.0f}, font{0}, size{0}, colour{0}, animated{false},
	  string_key{""}, alpha{255}, background{0}, justification{}, type{}, priority{999}, drawmode{}, texture{},
	  _id{s_id++} {

	unique_key.clear();
	_data.clear();
	_enabled = false;
	_visible = false;
}

Sorcery::Component::Component(std::string screen_, std::string name_, int x_, int y_, unsigned int w_, unsigned int h_,
	float scale_, Enums::Internal::FontType font_, unsigned int size_, unsigned long long colour_, bool animated_,
	std::string string_key_, unsigned int alpha_, unsigned int width_, unsigned long long background_,
	Enums::Window::Justification justification_, Enums::Window::ComponentType type_, unsigned int priority_,
	Enums::Window::DrawMode drawmode_, Enums::Graphics::Texture texture_)
	: screen{screen_}, name{name_}, x{x_}, y{y_}, w{w_}, h{h_}, scale{scale_}, font{font_}, size{size_},
	  colour{colour_}, animated{animated_}, string_key{string_key_}, alpha{alpha_}, width{width_},
	  background{background_}, justification{justification_}, type{type_}, priority{priority_}, drawmode{drawmode_},
	  texture{texture_}, _id{s_id++} {

	// Unique Key is like this because it is used for runtime component-sorting
	// (std::map is sorted by key) (also needs to be replaced with std::format
	// when available)
	const auto priority_id{fmt::format("{:03d}", priority)};
	unique_key = fmt::format("{}_{}:{}", priority_id, screen, name);
	_data.clear();
	_enabled = true;
	_visible = true;
}

// Copy Constructors
Sorcery::Component::Component(const Component &other)
	: screen{other.screen}, name{other.name}, x{other.x}, y{other.y}, w{other.w}, h{other.h}, scale{other.scale},
	  font{other.font}, size{other.size}, colour{other.colour}, animated{other.animated}, string_key{other.string_key},
	  alpha{other.alpha}, width{other.width}, background{other.background}, justification{other.justification},
	  type{other.type}, priority{other.priority}, drawmode{other.drawmode}, texture{other.texture}, _id{other._id} {

	// Not sure why I'm doing this here and not in the initialiser list?
	unique_key = other.unique_key;
	_data = other._data;
	_enabled = other._enabled;
	_visible = other._visible;
}

auto Sorcery::Component::operator=(const Component &other) -> Component & {

	screen = other.screen;
	name = other.name;
	x = other.x;
	y = other.y;
	w = other.w;
	h = other.h;
	scale = other.scale;
	font = other.font;
	size = other.size;
	colour = other.colour;
	animated = other.animated;
	string_key = other.string_key;
	alpha = other.alpha;
	width = other.width;
	background = other.background;
	justification = other.justification;
	type = other.type;
	priority = other.priority;
	drawmode = other.drawmode;
	texture = other.texture;
	unique_key = other.unique_key;
	_data = other._data;
	_enabled = other._enabled;
	_visible = other._visible;
	_id = other._id;

	return *this;
}

// Move Constructors
Sorcery::Component::Component(Component &&other) noexcept {

	screen = other.screen;
	name = other.name;
	x = other.x;
	y = other.y;
	w = other.w;
	h = other.h;
	scale = other.scale;
	font = other.font;
	size = other.size;
	colour = other.colour;
	animated = other.animated;
	string_key = other.string_key;
	alpha = other.alpha;
	width = other.width;
	background = other.background;
	justification = other.justification;
	type = other.type;
	priority = other.priority;
	drawmode = other.drawmode;
	texture = other.texture;
	unique_key = other.unique_key;
	_data = std::move(other._data);
	_enabled = other._enabled;
	_visible = other._visible;
	_id = other._id;

	other.screen.clear();
	other.name.clear();
	other.x = 0;
	other.y = 0;
	other.w = 0;
	other.h = 0;
	other.scale = 0;
	other.font = FontType::NO_FONT;
	other.size = 0;
	other.colour = 0;
	other.animated = false;
	other.string_key.clear();
	other.alpha = 0;
	other.width = 0;
	other.background = 0;
	other.justification = Justification::NO_JUSTIFY;
	other.type = ComponentType::NO_COMPONENT_TYPE;
	other.priority = 0;
	other.drawmode = WDM::NO_DRAWMODE;
	other.texture = GraphicsTexture::NO_TEXTURE;
	other.unique_key.clear();
	other._data.clear();
	other._enabled = false;
	other._visible = false;
	other._id = 0;
}

auto Sorcery::Component::operator=(Component &&other) noexcept -> Component & {

	if (this != &other) {
		screen = other.screen;
		name = other.name;
		x = other.x;
		y = other.y;
		w = other.w;
		h = other.h;
		scale = other.scale;
		font = other.font;
		size = other.size;
		colour = other.colour;
		animated = other.animated;
		string_key = other.string_key;
		alpha = other.alpha;
		width = other.width;
		background = other.background;
		justification = other.justification;
		type = other.type;
		priority = other.priority;
		drawmode = other.drawmode;
		texture = other.texture;
		unique_key = other.unique_key;
		_data = std::move(other._data);
		_enabled = other._enabled;
		_visible = other._visible;
		_id = other._id;

		other.screen.clear();
		other.name.clear();
		other.x = 0;
		other.y = 0;
		other.w = 0;
		other.h = 0;
		other.scale = 0;
		other.font = FontType::NO_FONT;
		other.size = 0;
		other.colour = 0;
		other.animated = false;
		other.string_key.clear();
		other.alpha = 0;
		other.width = 0;
		other.background = 0;
		other.justification = Justification::NO_JUSTIFY;
		other.type = ComponentType::NO_COMPONENT_TYPE;
		other.priority = 0;
		other.drawmode = WDM::NO_DRAWMODE;
		other.texture = GraphicsTexture::NO_TEXTURE;
		other.unique_key.clear();
		other._data.clear();
		other._enabled = false;
		other._visible = false;
		other._id = 0;
	}
	return *this;
}

// Overload [] operator
auto Sorcery::Component::operator[](std::string_view key) -> std::optional<std::string> {

	return _get(std::string{key});
}

auto Sorcery::Component::operator[](std::string_view key) const -> std::optional<std::string> {

	return _get(std::string{key});
}

auto Sorcery::Component::set(std::string_view key, std::string_view value) -> void {

	auto it{std::find_if(_data.begin(), _data.end(), [&key](auto item) { return item.first == key; })};
	if (it == _data.end())
		_data.push_back(std::make_pair(std::string{key}, std::string{value}));
}

auto Sorcery::Component::set_enabled(bool value) -> void {

	_enabled = value;
}

auto Sorcery::Component::get_enabled() -> bool {

	return _enabled;
}

auto Sorcery::Component::set_visible(bool value) -> void {

	_visible = value;
}

auto Sorcery::Component::get_visible() -> bool {

	return _visible;
}

auto Sorcery::Component::pos() const -> sf::Vector2f {

	return {static_cast<float>(x), static_cast<float>(y)};
}

auto Sorcery::Component::scl() const -> sf::Vector2f {

	return {scale, scale};
}

auto Sorcery::Component::_get(std::string_view key) const -> std::optional<std::string> {

	if (_data.capacity() == 0)
		return std::nullopt;
	auto it{std::find_if(_data.begin(), _data.end(), [&key](auto item) { return item.first == std::string{key}; })};
	if (it != _data.end())
		return it->second;
	else
		return std::nullopt;
}

auto Sorcery::Component::id() const -> long {

	return _id;
}
