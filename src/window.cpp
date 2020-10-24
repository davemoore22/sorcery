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

	tooltips.clear();
}

// Standard Destructor
Sorcery::Window::~Window() {
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

auto Sorcery::Window::get_gui_frame(sf::RenderTexture& gui_frame_rt, sf::Texture& gui_frame_t,
	const unsigned int width_units, const unsigned int height_units, const unsigned int alpha) -> sf::Sprite {

	// Defijne the sources of the gui elements
	sf::IntRect top_left_rect(0, 550, 20, 20);
	sf::IntRect top_rect(20, 550, 20, 20);
	sf::IntRect top_right_rect(40, 550, 20, 20);
	sf::IntRect left_rect(0, 570, 20, 20);
	sf::IntRect bottom_left_rect(0, 590, 20, 20);
	sf::IntRect bottom_rect(20, 590, 20, 20);
	sf::IntRect bottom_right_rect(40, 590, 20, 20);
	sf::IntRect right_rect(40, 570, 20, 20);

	// Work out total size of texture needed from units
	const sf::Vector2f texture_size(20 + (20 * width_units) + 20, 20 + (20 * height_units) + 20);
	gui_frame_rt.create(texture_size.x, texture_size.y);

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
	top_right.setPosition(texture_size.x - 20, 0);
	gui_frame_rt.draw(top_right);
	bottom_left.setPosition(0, texture_size.y - 20);
	gui_frame_rt.draw(bottom_left);
	bottom_right.setPosition(texture_size.x - 20, texture_size.y - 20);
	gui_frame_rt.draw(bottom_right);

	// Draw the sides
	for (unsigned int x = 0; x < width_units; x++) {
		unsigned int x_pos {20 + (20 * x)};
		top.setPosition(x_pos, 0);
		gui_frame_rt.draw(top);
		bottom.setPosition(x_pos, texture_size.y - 20);
		gui_frame_rt.draw(bottom);
	}
	for (unsigned int y = 0; y < height_units; y++) {
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


auto Sorcery::Window::draw_menu(std::vector<MenuEntry>& items, std::vector<sf::FloatRect>& bounds,
	std::vector<MenuEntry>::const_iterator selected, Component& component, MenuType menu_type, double lerp) -> void {
	_draw_menu(items, bounds, selected, component, menu_type, lerp);
}

auto Sorcery::Window::_draw_menu(std::vector<MenuEntry>& items, std::vector<sf::FloatRect>& bounds,
	std::vector<MenuEntry>::const_iterator selected, Component& component,  MenuType menu_type, double lerp) -> void {

	unsigned int width {component.width};
	int x {0};
	int y {0};
	int option_x {0};
	int option_y {0};
	int count {0};

	Component on_component =  _layout["options:option_on"];
	Component off_component =  _layout["options:option_off"];

	bounds.clear();
	for (std::vector<MenuEntry>::const_iterator it = items.begin(); it != items.end(); ++it) {
		if (((*it).type == MenuItemType::TEXT) || ((*it).type == MenuItemType::ENTRY) ||
			((*it).type == MenuItemType::SAVE) || ((*it).type == MenuItemType::CANCEL)) {
			std::string text_string {(*it).key};
			sf::Text text;
			text.setFont(_system.resources->fonts[component.font]);
			text.setCharacterSize(component.size);
			text.setFillColor(sf::Color(component.colour));
			text.setString(text_string);
			x = component.x == -1 ? centre.x :  component.x;
			y = component.y == -1 ? centre.y :  component.y;
			y = y + (count * get_cell_height());
			text.setPosition(x, y);
			if (selected == it) {
				sf::FloatRect background_rect {text.getLocalBounds()};
				sf::RectangleShape background(sf::Vector2f(width * get_cell_width(),
					background_rect.height + 2));
				if (component.justification == Justification::CENTRE)
					background.setOrigin(background.getGlobalBounds().width / 2.0f, -3);
				else
					background.setOrigin(0, -3);
				if (component.animated)
					background.setFillColor(change_colour(sf::Color(component.background), lerp));
				else
					background.setFillColor(sf::Color(component.background));
				text.setFillColor(sf::Color(component.colour));
				text.setOutlineColor(sf::Color(0, 0, 0));
				text.setOutlineThickness(2);
				_window.draw(background, text.getTransform());
			}
			if (menu_type == MenuType::OPTIONS) {
				if ((*it).type == MenuItemType::ENTRY) {
					if (component.justification == Justification::CENTRE)
						text.setOrigin(text.getLocalBounds().width / 2.0f, text.getLocalBounds().height / 2.0f);
					else
						text.setOrigin(0, text.getLocalBounds().height / 2.0f);
				} else if (((*it).type == MenuItemType::SAVE) || ((*it).type == MenuItemType::CANCEL)) {
					x =  (component.x / 2) + ((component.width * get_cell_height()) / 2);
					text.setPosition(x, y);
					text.setOrigin(text.getLocalBounds().width / 2.0f, text.getLocalBounds().height / 2.0f);
				}
			} else {
				if (component.justification == Justification::CENTRE)
					text.setOrigin(text.getLocalBounds().width / 2.0f, text.getLocalBounds().height / 2.0f);
				else
					text.setOrigin(0, text.getLocalBounds().height / 2.0f);
			}
			draw_text(text);
			if (((*it).type == MenuItemType::ENTRY) || ((*it).type == MenuItemType::SAVE) ||
				((*it).type == MenuItemType::CANCEL)) {
				sf::FloatRect actual_rect {text.getGlobalBounds()};
				bounds.push_back(actual_rect);
				WindowTooltipList::iterator tooltipit = tooltips.find((*it).hint);
				if (tooltipit == tooltips.end())
					tooltips[(*it).hint] = actual_rect;
			} else {
				sf::FloatRect actual_rect;
				bounds.push_back(actual_rect);
					WindowTooltipList::iterator tooltipit = tooltips.find((*it).hint);
				if (tooltipit == tooltips.end())
					tooltips[(*it).hint] = actual_rect;
			}

			// Add options in case of the Options Menu
			if ((menu_type == MenuType::OPTIONS) && ((*it).type == MenuItemType::ENTRY)) {
				option_y = y;
				option_x = x + (component.width * get_cell_width());
				const bool option_value {(*_system.config)[(*it).config] ? true : false};
				sf::Text option_text {};
				if (option_value) {

					// On
					option_text.setFont(_system.resources->fonts[on_component.font]);
					option_text.setCharacterSize(on_component.size);
					option_text.setFillColor(sf::Color(on_component.colour));
					option_text.setString(_string[on_component.string_key]);
					sf::FloatRect bounds = option_text.getLocalBounds();
					option_text.setPosition(option_x - bounds.width, option_y);
					option_text.setOrigin(0, option_text.getLocalBounds().height / 2.0f);
				} else {

					// Off
					option_text.setFont(_system.resources->fonts[off_component.font]);
					option_text.setCharacterSize(off_component.size);
					option_text.setFillColor(sf::Color(off_component.colour));
					option_text.setString(_string[off_component.string_key]);
					sf::FloatRect bounds = option_text.getLocalBounds();
					option_text.setPosition(option_x - bounds.width, option_y);
					option_text.setOrigin(0, option_text.getLocalBounds().height / 2.0f);
				}

				if (selected == it) {
					option_text.setOutlineColor(sf::Color(0, 0, 0));
					option_text.setOutlineThickness(2);
				}

				draw_text(option_text);
			}

		} else {
			sf::FloatRect actual_rect;
			bounds.push_back(actual_rect);
		}
		count++;
	}
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

auto Sorcery::Window::get_cell_height() -> unsigned int {
	return _cell_height;
}

auto Sorcery::Window::get_cell_width() -> unsigned int {
	return _cell_width;
}

// Given a colour, change its brightness
auto Sorcery::Window::change_colour(sf::Color colour, double lerp) -> sf::Color {
	return _change_colour(colour, lerp);
}

auto Sorcery::Window::_change_colour(sf::Color colour, double lerp) -> sf::Color {
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

auto Sorcery::Window::highlight_text(sf::Text& text, Component component, double lerp) -> sf::RectangleShape {
	sf::FloatRect text_rect {text.getGlobalBounds()};
	sf::RectangleShape text_background(sf::Vector2(text_rect.width + 6, text_rect.height + 8));
	text_background.setOrigin(0, 0 - text.getLocalBounds().height + 16);
	text_background.setFillColor(change_colour(sf::Color(component.background), lerp));
	text.setFillColor(sf::Color(component.colour));
	text.setOutlineColor(sf::Color(0, 0, 0));
	text.setOutlineThickness(2);
	return text_background;
}

auto Sorcery::Window::get_hint_frame(sf::RenderTexture& hint_frame_rt, sf::Texture& hint_frame_t,
	const unsigned int width_units, const unsigned int height_units, unsigned int alpha) -> sf::Sprite {

	// Defijne the sources of the gui elements
	sf::IntRect top_left_rect(865, 399, 18, 18);
	sf::IntRect top_rect(899, 399, 24, 10); // width of 74 is available in texture
	sf::IntRect top_right_rect(982, 399, 18, 18);
	sf::IntRect left_rect(865, 428, 10, 24); // height of 59 is available in texture
	sf::IntRect bottom_left_rect(865, 498, 18, 18);
	sf::IntRect bottom_rect(899, 506, 24, 10); // width of 74 is available in texture
	sf::IntRect bottom_right_rect(982, 498, 18, 18);
	sf::IntRect right_rect(989, 428, 10, 24);

	// Work out total size of texture needed from units
	const sf::Vector2f texture_size(18 + (24 * width_units) + 18, 18 + (24 * height_units) + 18);
	hint_frame_rt.create(texture_size.x, texture_size.y);

	// Draw the background
	sf::RectangleShape rectangle(sf::Vector2f(texture_size.x - 10, texture_size.y - 10));
	rectangle.setFillColor(sf::Color(0, 0, 0, alpha));
	rectangle.setPosition(5, 5);
	hint_frame_rt.draw(rectangle);

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
	hint_frame_rt.draw(top_left);
	top_right.setPosition(texture_size.x - 18, 0);
	hint_frame_rt.draw(top_right);
	bottom_left.setPosition(0, texture_size.y - 18);
	hint_frame_rt.draw(bottom_left);
	bottom_right.setPosition(texture_size.x - 18, texture_size.y - 18);
	hint_frame_rt.draw(bottom_right);

	// Draw the sides
	for (unsigned int x = 0; x < width_units; x++) {
		unsigned int x_pos {18 + (24 * x)};
		top.setPosition(x_pos, 0);
		hint_frame_rt.draw(top);
		bottom.setPosition(x_pos, texture_size.y - 10);
		hint_frame_rt.draw(bottom);
	}
	for (unsigned int y = 0; y < height_units; y++) {
		unsigned int y_pos {18 + (24 * y)};
		left.setPosition(0, y_pos);
		hint_frame_rt.draw(left);
		right.setPosition(texture_size.x - 11, y_pos);
		hint_frame_rt.draw(right);
	}

	// And draw
	hint_frame_rt.display();

	// And return
	hint_frame_t = hint_frame_rt.getTexture();
	sf::Sprite hint_frame_sprite(hint_frame_t);
	return hint_frame_sprite;
}