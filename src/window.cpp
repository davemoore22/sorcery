// Copyright (C) 2020 Dave Moore
//
// This file is part of Sorcery: Dreams of the Mad Overlord.
//
// Sorcery: Dreams of the Mad Overlord is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Sorcery: Dreams of the Mad Overlord is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Sorcery: Dreams of the Mad Overlord.  If not, see <http://www.gnu.org/licenses/>.
//
// If you modify this Program, or any covered work, by linking or combining it
// with the libraries referred to in README (or a modified version of said
// libraries), containing parts covered by the terms of said libraries, the
// licensors of this Program grant you additional permission to convey the
// resulting work.

#include "window.hpp"

// also add a TGUI to this

// Standard Constructor
Sorcery::Window::Window(std::string title, System& system, String& string, Layout& layout): _title {title},
	_system {system}, _string {string}, _layout {layout} {

	// First get the Window Size from System Config
	_default_size.w = std::stoi(_system.config->get("Window", DEFAULT_SCREEN_WIDTH));
	_default_size.h = std::stoi(_system.config->get("Window", DEFAULT_SCREEN_HEIGHT));
	_current_size.w = std::stoi(_system.config->get("Window", CURRENT_SCREEN_WIDTH));
	_current_size.h = std::stoi(_system.config->get("Window", CURRENT_SCREEN_HEIGHT));

	// And then the Positioning Grid Cell Size
	_cell_height = std::stoi(_system.config->get("Grid", CELL_HEIGHT));
	_cell_width = std::stoi(_system.config->get("Grid", CELL_WIDTH));
	_layout.set_grid(_cell_width, _cell_height);

	// Then create the SFML Window and the Main GUI
	XInitThreads();
	_window.create(sf::VideoMode(_current_size.w, _current_size.h), _title, sf::Style::Titlebar | sf::Style::Resize |
		sf::Style::Close);
	centre.x = _current_size.w / 2;
	centre.y = _current_size.h / 2;
	_window.setFramerateLimit(60);
  	_window.setVerticalSyncEnabled(true);
	_gui.setTarget(_window);
	view = _window.getView();

	// Change the Mouse Cursor
	_window.setMouseCursorVisible(false);

	_creature_sprite_width = 108;
	_creature_sprite_height = 108;
	_creature_sprite_spacing = 8;
	_creature_sprite_scaling =- 2.5f;

	tooltips.clear();
}

auto Sorcery::Window::clear_window() -> void {

	_window.clear();
}

auto Sorcery::Window::display_window() -> void {

	_window.display();
}

auto Sorcery::Window::draw_gui() -> void {

	_gui.draw();
}

// Draw Text on the Screen
auto Sorcery::Window::draw_text(sf::Text& text) -> void {

	_draw_text(text);
}

auto Sorcery::Window::draw_text(sf::Text& text, Component& component, std::string& string) -> void {

	_draw_text(text, component, string);
}

auto Sorcery::Window::draw_text(sf::Text& text, Component& component, double lerp) -> void {

	_draw_text(text, component, lerp);
}

auto Sorcery::Window::_draw_text(sf::Text& text) -> void {

	_window.draw(text);
}

auto Sorcery::Window::_draw_text(sf::Text& text, Component& component, double lerp) -> void {

	int x {0};
	int y {0};
	text.setFont(_system.resources->fonts[component.font]);
	text.setCharacterSize(component.size);
	if (component.animated)
		text.setFillColor(_change_colour(sf::Color(component.colour), lerp));
	else
		text.setFillColor(sf::Color(component.colour));
	text.setString(_string[component.string_key]);
	x = component.x == -1 ? centre.x :  component.x;
	y = component.y == -1 ? centre.y :  component.y;
	if (component.justification == Justification::CENTRE) {
		text.setPosition(x, y);
		text.setOrigin(text.getLocalBounds().width / 2.0f, text.getLocalBounds().height / 2.0f);
	} else if (component.justification == Justification::RIGHT) {
		text.setPosition(x, y);
		sf::FloatRect bounds = text.getLocalBounds();
		text.setPosition(component.x - bounds.width, component.y);
	} else {
		text.setPosition(x, y);
		text.setOrigin(0, text.getLocalBounds().height / 2.0f);
	}
	_window.draw(text);
}

auto Sorcery::Window::_draw_text(sf::Text& text, Component& component, std::string& string) -> void {

	int x {0};
	int y {0};
	text.setFont(_system.resources->fonts[component.font]);
	text.setCharacterSize(component.size);
	text.setFillColor(sf::Color(component.colour));
	text.setString(string);
	x = component.x == -1 ? centre.x :  component.x;
	y = component.y == -1 ? centre.y :  component.y;
	if (component.justification == Justification::CENTRE) {
		text.setPosition(x, y);
		text.setOrigin(text.getLocalBounds().width / 2.0f, text.getLocalBounds().height / 2.0f);
	} else if (component.justification == Justification::RIGHT) {
		text.setPosition(x, y);
		sf::FloatRect bounds = text.getLocalBounds();
		text.setPosition(component.x - bounds.width, component.y);
	} else {
		text.setPosition(x, y);
		text.setOrigin(0, text.getLocalBounds().height / 2.0f);
	}
	_window.draw(text);
}

auto Sorcery::Window::get_cursor() const -> sf::Sprite {

	sf::Sprite cursor(_system.resources->textures[UI_TEXTURE]);
	sf::IntRect cursor_rect(710, 310, 21, 28);
	cursor.setTextureRect(cursor_rect);
	return cursor;
}

auto Sorcery::Window::get_gui() -> tgui::Gui* {

	return &_gui;
}

auto Sorcery::Window::get_window() -> sf::RenderWindow* {

	return &_window;
}

auto Sorcery::Window::_get_centre_x(const sf::Sprite& sprite) const -> unsigned int {

	return (_current_size.w - sprite.getGlobalBounds().width) / 2.0f;
}

auto Sorcery::Window::_get_centre_y(const sf::Sprite& sprite) const -> unsigned int {

	return (_current_size.h - sprite.getGlobalBounds().height) / 2.0f;
}

auto Sorcery::Window::_get_centre_x(const sf::Text& text) const -> unsigned int {

	return (_current_size.w - text.getGlobalBounds().width) / 2.0f;
}

auto Sorcery::Window::_get_centre_y(const sf::Text& text) const -> unsigned int {

	return (_current_size.h - text.getGlobalBounds().height) / 2.0f;
}

auto Sorcery::Window::_get_x(const sf::Sprite& sprite, const int x_position) const -> unsigned int {

	return x_position ==  -1 ? _get_centre_x(sprite) : x_position;
}

auto Sorcery::Window::_get_x(const sf::Text& text, const int x_position) const -> unsigned int {

	return x_position ==  -1 ? _get_centre_x(text) : x_position;
}

auto Sorcery::Window::_get_y(const sf::Sprite& sprite, const int y_position) const -> unsigned int {

	return y_position ==  -1 ? _get_centre_y(sprite) : y_position;
}

auto Sorcery::Window::_get_y(const sf::Text& text, const int y_position) const -> unsigned int {

	return y_position ==  -1 ? _get_centre_y(text) : y_position;
}

auto Sorcery::Window::get_x(const sf::Sprite& sprite, const int x_position) const -> unsigned int {

	return x_position ==  -1 ? _get_centre_x(sprite) : x_position;
}

auto Sorcery::Window::get_y(const sf::Sprite& sprite, const int y_position) const -> unsigned int {

	return y_position ==  -1 ? _get_centre_y(sprite) : y_position;
}

auto Sorcery::Window::get_cell_height() const -> unsigned int {

	return _cell_height;
}

auto Sorcery::Window::get_cell_width() const -> unsigned int {

	return _cell_width;
}

// Given a colour, change its brightness
auto Sorcery::Window::change_colour(sf::Color colour, double lerp) const -> sf::Color {

	return _change_colour(colour, lerp);
}

auto Sorcery::Window::_change_colour(sf::Color colour, double lerp) const -> sf::Color {

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnarrowing"
	double red {colour.r};
	double green {colour.g};
	double blue {colour.b};
#pragma GCC diagnostic pop
	if (lerp < 0) {
		lerp = 1 + lerp;
		red *= lerp;
		green *= lerp;
		blue *= lerp;
	} else {
		red = (255 - red) * lerp + red;
		green = (255 - green) * lerp + green;
		blue = (255 - blue) * lerp + blue;
	}

	return sf::Color(red, green, blue);
}

auto Sorcery::Window::highlight_text(sf::Text& text, Component component, const double lerp) -> sf::RectangleShape {

	sf::FloatRect text_rect {text.getGlobalBounds()};
	sf::RectangleShape text_background(sf::Vector2(text_rect.width + 6, text_rect.height + 8));
	text_background.setOrigin(0, 0 - text.getLocalBounds().height + 16);
	text_background.setFillColor(change_colour(sf::Color(component.background), lerp));
	text.setFillColor(sf::Color(component.colour));
	text.setOutlineColor(sf::Color(0, 0, 0));
	text.setOutlineThickness(2);
	return text_background;
}