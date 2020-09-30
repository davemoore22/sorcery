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

	// Fonts
	_mono_system_font.loadFromFile(_system.files->get_path_as_string(MONO_FONT_FILE));
	_proportional_system_font.loadFromFile(_system.files->get_path_as_string(PROPORTIONAL_FONT_FILE));

	// Change the Mouse Cursor
	_window.setMouseCursorVisible(false);

	_creature_sprite_width = 108;
	_creature_sprite_height = 108;
	_creature_sprite_spacing = 8;
	_creature_sprite_scaling =- 2.5f;
}

// Standard Destructor
Sorcery::Window::~Window() {
}

auto Sorcery::Window::clear_window() -> void {
	_window.clear();
}

auto Sorcery::Window::draw_gui() -> void {
	_gui.draw();
}

auto Sorcery::Window::display_window() -> void {
	_window.display();
}

auto Sorcery::Window::get_gui_frame(sf::RenderTexture& gui_frame_rt, sf::Texture& gui_frame_t,
	const unsigned int width_units, const unsigned int height_units, const unsigned int alpha) -> sf::Sprite {

	// Defijne the sources of the gui elements
	//sf::IntRect top_left_rect(865, 399, 18, 18);
	//sf::IntRect top_rect(899, 399, 24, 10); // width of 74 is available in texture
	//sf::IntRect top_right_rect(982, 399, 18, 18);
	//sf::IntRect left_rect(865, 428, 10, 24); // height of 59 is available in texture
	//sf::IntRect bottom_left_rect(865, 498, 18, 18);
	//sf::IntRect bottom_rect(899, 506, 24, 10); // width of 74 is available in texture
	//sf::IntRect bottom_right_rect(982, 498, 18, 18);
	//sf::IntRect right_rect(989, 428, 10, 24); // height of 59 is available in texture

	sf::IntRect top_left_rect(0, 550, 20, 20);
	sf::IntRect top_rect(20, 550, 20, 20);
	sf::IntRect top_right_rect(40, 550, 20, 20);
	sf::IntRect left_rect(0, 570, 20, 20);
	sf::IntRect bottom_left_rect(0, 590, 20, 20);
	sf::IntRect bottom_rect(20, 590, 20, 20);
	sf::IntRect bottom_right_rect(40, 590, 20, 20);
	sf::IntRect right_rect(40, 570, 20, 20);

	// Work out total size of texture needed from units
	//const sf::Vector2f texture_size(18 + (24 * width_units) + 18, 18 + (24 * height_units) + 18);
	const sf::Vector2f texture_size(20 + (20 * width_units) + 20, 20 + (20 * height_units) + 20);
	gui_frame_rt.create(texture_size.x, texture_size.y);
	//gui_frame_rt.clear(sf::Color(0, 0, 0, 200));

	// Get the Frame Components
	sf::Sprite top_left(_system.resources->textures[UI_TEXTURE]);
	top_left.setTextureRect(top_left_rect);
	sf::Sprite top(_system.resources->textures[UI_TEXTURE]);
	top.setTextureRect(top_rect);
	sf::Sprite top_right(_system.resources->textures[UI_TEXTURE]);
	top_right.setTextureRect(top_right_rect);
	sf::Sprite left(_system.resources->textures[UI_TEXTURE]);
	left.setTextureRect(left_rect);
	sf::Sprite bottom_left(_system.resources->textures[UI_TEXTURE]);
	bottom_left.setTextureRect(bottom_left_rect);
	sf::Sprite bottom(_system.resources->textures[UI_TEXTURE]);
	bottom.setTextureRect(bottom_rect);
	sf::Sprite bottom_right(_system.resources->textures[UI_TEXTURE]);
	bottom_right.setTextureRect(bottom_right_rect);
	sf::Sprite right(_system.resources->textures[UI_TEXTURE]);
	right.setTextureRect(right_rect);

	// Draw the Corners
	top_left.setPosition(0, 0);
	gui_frame_rt.draw(top_left);
	//top_right.setPosition(texture_size.x - 18, 0);
	top_right.setPosition(texture_size.x - 20, 0);
	gui_frame_rt.draw(top_right);
	//bottom_left.setPosition(0, texture_size.y - 18);
	bottom_left.setPosition(0, texture_size.y - 20);
	gui_frame_rt.draw(bottom_left);
	//bottom_right.setPosition(texture_size.x - 18, texture_size.y - 18);
	bottom_right.setPosition(texture_size.x - 20, texture_size.y - 20);
	gui_frame_rt.draw(bottom_right);

	// Draw the sides
	for (unsigned int x = 0; x < width_units; x++) {
		//int x_pos {18 + (24 * x)};
		unsigned int x_pos {20 + (20 * x)};
		top.setPosition(x_pos, 0);
		gui_frame_rt.draw(top);
		bottom.setPosition(x_pos, texture_size.y - 20);
		gui_frame_rt.draw(bottom);
	}
	for (unsigned int y = 0; y < height_units; y++) {
		//int y_pos {18 + (24 * y)};
		unsigned int y_pos {20 + (20 * y)};
		left.setPosition(0, y_pos);
		gui_frame_rt.draw(left);
		right.setPosition(texture_size.x - 20, y_pos);
		gui_frame_rt.draw(right);
	}

	// Draw the background
	sf::RectangleShape rectangle(sf::Vector2f(texture_size.x - 20, texture_size.y - 20));
	rectangle.setFillColor(sf::Color(0, 0, 0, alpha));
	rectangle.setPosition(10, 10);
	gui_frame_rt.draw(rectangle);

	// And draw
	gui_frame_rt.display();

	// And return
	gui_frame_t = gui_frame_rt.getTexture();
	sf::Sprite gui_frame_sprite(gui_frame_t);
	return gui_frame_sprite;
}
// Draw Text on the Screen
auto Sorcery::Window::draw_centered_text(sf::Text& text, Component& component, double lerp) -> void {
	_draw_centered_text(text, component, lerp);
}

auto Sorcery::Window::draw_centered_text(sf::Text& text) -> void {
	_draw_centered_text(text);
}

auto Sorcery::Window::_draw_centered_text(sf::Text& text) -> void {
	_window.draw(text);
}

auto Sorcery::Window::_draw_centered_text(sf::Text& text, Component& component, double lerp) -> void {
	int x {0};
	int y {0};
	text.setFont(_system.resources->fonts[component.font]);
	text.setCharacterSize(component.size);
	if ((component.animated) && (lerp >= 0.0l))
		text.setFillColor(_change_colour(sf::Color(component.colour), lerp));
	else
		text.setFillColor(sf::Color(component.colour));
	text.setString(_string[component.string_key]);
	x = component.x == -1 ? centre.x :  component.x;
	y = component.y == -1 ? centre.y :  component.y;
	text.setPosition(x, y);
	text.setOrigin(text.getLocalBounds().width / 2.0f, text.getLocalBounds().height / 2.0f);
	_window.draw(text);
}


auto Sorcery::Window::get_cursor() -> sf::Sprite {
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

auto Sorcery::Window::_get_centre_x(sf::Sprite& sprite) -> unsigned int {
	return (_current_size.w - sprite.getGlobalBounds().width) / 2.0f;
}

auto Sorcery::Window::_get_centre_y(sf::Sprite& sprite) -> unsigned int {
	return (_current_size.h - sprite.getGlobalBounds().height) / 2.0f;
}

auto Sorcery::Window::_get_centre_x(sf::Text& text) -> unsigned int {
	return (_current_size.w - text.getGlobalBounds().width) / 2.0f;
}

auto Sorcery::Window::_get_centre_y(sf::Text& text) -> unsigned int {
	return (_current_size.h - text.getGlobalBounds().height) / 2.0f;
}

auto Sorcery::Window::_get_x(sf::Sprite& sprite, int x_position) -> unsigned int {
	return x_position ==  -1 ? _get_centre_x(sprite) : x_position;
}

auto Sorcery::Window::_get_x(sf::Text& text, int x_position) -> unsigned int {
	return x_position ==  -1 ? _get_centre_x(text) : x_position;
}

auto Sorcery::Window::_get_y(sf::Sprite& sprite, int y_position) -> unsigned int {
	return y_position ==  -1 ? _get_centre_y(sprite) : y_position;
}

auto Sorcery::Window::_get_y(sf::Text& text, int y_position) -> unsigned int {
	return y_position ==  -1 ? _get_centre_y(text) : y_position;
}

auto Sorcery::Window::get_x(sf::Sprite& sprite, int x_position) -> unsigned int {
	return x_position ==  -1 ? _get_centre_x(sprite) : x_position;
}

auto Sorcery::Window::get_y(sf::Sprite& sprite, int y_position) -> unsigned int {
	return y_position ==  -1 ? _get_centre_y(sprite) : y_position;
}

// Given a colour, change its brightness
auto Sorcery::Window::change_colour(sf::Color colour, double lerp) -> sf::Color {
	return _change_colour(colour, lerp);
}

auto Sorcery::Window::_change_colour(sf::Color colour, double lerp) -> sf::Color {
	int r {colour.r};
	int g {colour.g};
	int b {colour.b};
	r = std::min(255, (int) (r + 128 * lerp));
	g = std::min(255, (int) (g + 128 * lerp));
	b = std::min(255, (int) (b + 128 * lerp));
	return sf::Color(r, g, b);
}

auto Sorcery::Window::get_cell_height() -> unsigned int {
	return _cell_height;
}

auto Sorcery::Window::get_cell_width() -> unsigned int {
	return _cell_width;
}
