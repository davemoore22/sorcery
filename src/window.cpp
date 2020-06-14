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

	// Load the Textures
	_textures.clear();
	sf::Texture background;
	background.loadFromFile(_system.files->get_path_as_string(BACKGROUND_TEXTURE));
	sf::Texture banner;
	banner.loadFromFile(_system.files->get_path_as_string(BANNER_TEXTURE));
	sf::Texture creatures;
	creatures.loadFromFile(_system.files->get_path_as_string(CREATURES_TEXTURE));
	sf::Texture logo;
	logo.loadFromFile(_system.files->get_path_as_string(LOGO_TEXTURE));
	sf::Texture splash;
	splash.loadFromFile(_system.files->get_path_as_string(SPLASH_TEXTURE));
	sf::Texture ui;
	ui.loadFromFile(_system.files->get_path_as_string(UI_TEXTURE));
	_textures[BACKGROUND_TEXTURE] = background;
	_textures[BANNER_TEXTURE] = banner;
	_textures[CREATURES_TEXTURE] = creatures;
	_textures[LOGO_TEXTURE] = logo;
	_textures[SPLASH_TEXTURE] = splash;
	_textures[UI_TEXTURE] = ui;

	// Change the Mouse Cursor
	_window.setMouseCursorVisible(false);

	_attract_mode_data.clear();
	_creature_sprite_width = 108;
	_creature_sprite_height = 108;
	_creature_sprite_spacing = 8;
	_creature_sprite_scaling =- 2.5f;
}

// Standard Destructor
Sorcery::Window::~Window() {
}

// Public Methods
auto Sorcery::Window::operator [](std::string_view key) const -> sf::Texture {
	if (_textures.find(key) != _textures.end())
		return _textures.at(key);
}

auto Sorcery::Window::clear_window() -> void {
	_window.clear();
}

auto Sorcery::Window::draw_attract_mode(std::vector<unsigned int> attract_mode_data) -> void {
	if (attract_mode_data.size() > 0) {

			// Move Logo inside Frame and add a bottom frame for the press any key

			// then bottom frame will have next menu - start game, new game, help


		// Get Constituent Parts for the Main Menu
		Component gui_frame {_layout["main_menu_attract:top_gui_frame"]};
		sf::Sprite frame { get_gui_frame(gui_frame.w, gui_frame.h, 0)};
		const sf::Vector2f frame_pos(_get_x(frame, gui_frame.x), _get_y(frame, gui_frame.y));
		frame.setPosition(frame_pos);

		Component attract_creatures {_layout["main_menu_attract:attract_creatures"]};
		sf::Sprite creatures {_get_attract_mode(attract_mode_data)};
		creatures.setScale(attract_creatures.scale, attract_creatures.scale);
		const sf::Vector2f creature_pos(_get_x(creatures, attract_creatures.x), _get_y(creatures, attract_creatures.y));
		creatures.setPosition(creature_pos);

		_window.draw(frame);
		_window.draw(creatures);

		// Draw Main Menu Text

		sf::Text text;
		text.setFont(_mono_system_font);
		text.setCharacterSize(24);

		text.setColor(sf::Color(0xd3d3d3ff));
		text.setString(_string["MAIN_MENU_SUBTITLE_1"]);
		text.setPosition(_current_size.w / 2.0f, 676);
		text.setOrigin(text.getLocalBounds().width / 2.0f, text.getLocalBounds().height / 2.0f);
		_window.draw(text);

		text.setString(_string["MAIN_MENU_SUBTITLE_2"]);
		text.setPosition(_current_size.w / 2.0f, 704);
		text.setOrigin(text.getLocalBounds().width / 2.0f, text.getLocalBounds().height / 2.0f);
		_window.draw(text);

		text.setColor(sf::Color(0xffffffff));
		text.setString(_string["BRIEF_COPYRIGHT_INFO"]);
		text.setPosition(_current_size.w / 2.0f, 740);
		text.setOrigin(text.getLocalBounds().width / 2.0f, text.getLocalBounds().height / 2.0f);
		_window.draw(text);

		text.setFont(_proportional_system_font);
  		text.setColor(sf::Color(0xffb419ff));
		text.setCharacterSize(36);
		text.setString(_string["PRESS_ANY_KEY"]);
		text.setPosition(_current_size.w / 2.0f, 820);
		text.setOrigin(text.getLocalBounds().width / 2.0f, text.getLocalBounds().height / 2.0f);
		_window.draw(text);
	}
}

auto Sorcery::Window::draw_gui() -> void {
	_gui.draw();
}

auto Sorcery::Window::display_window() -> void {
	_window.display();
}


auto Sorcery::Window::get_gui_frame(const unsigned int width, const unsigned int height, const unsigned int alpha = 0)-> sf::Sprite {

	// Defijne the sources of the gui elements
	sf::IntRect top_left_rect(865, 399, 18, 18);
	sf::IntRect top_rect(899, 399, 24, 10); // width of 74 is available in texture
	sf::IntRect top_right_rect(982, 399, 18, 18);
	sf::IntRect left_rect(865, 428, 10, 24); // height of 59 is available in texture
	sf::IntRect bottom_left_rect(865, 498, 18, 18);
	sf::IntRect bottom_rect(899, 506, 24, 10); // width of 74 is available in texture
	sf::IntRect bottom_right_rect(982, 498, 18, 18);
	sf::IntRect right_rect(989, 428, 10, 24); // height of 59 is available in texture

	// Work out total size of texture needed from units
	const sf::Vector2f texture_size(18 + (24 * width) + 18, 18 + (24 * height) + 18);
	sf::RenderTexture gui_frame_texture;
	gui_frame_texture.create(texture_size.x, texture_size.y);
	gui_frame_texture.clear();

	// Get the Frame Components
	sf::Sprite top_left(_textures[UI_TEXTURE]);
	top_left.setTextureRect(top_left_rect);
	sf::Sprite top(_textures[UI_TEXTURE]);
	top.setTextureRect(top_rect);
	sf::Sprite top_right(_textures[UI_TEXTURE]);
	top_right.setTextureRect(top_right_rect);
	sf::Sprite left(_textures[UI_TEXTURE]);
	left.setTextureRect(left_rect);
	sf::Sprite bottom_left(_textures[UI_TEXTURE]);
	bottom_left.setTextureRect(bottom_left_rect);
	sf::Sprite bottom(_textures[UI_TEXTURE]);
	bottom.setTextureRect(bottom_rect);
	sf::Sprite bottom_right(_textures[UI_TEXTURE]);
	bottom_right.setTextureRect(bottom_right_rect);
	sf::Sprite right(_textures[UI_TEXTURE]);
	right.setTextureRect(right_rect);

	// Draw the Corners
	top_left.setPosition(0, 0);
	gui_frame_texture.draw(top_left);
	top_right.setPosition(texture_size.x - 18, 0);
	gui_frame_texture.draw(top_right);
	bottom_left.setPosition(0, texture_size.y - 18);
	gui_frame_texture.draw(bottom_left);
	bottom_right.setPosition(texture_size.x - 18, texture_size.y - 18);
	gui_frame_texture.draw(bottom_right);

	// Fill in the Sides
	for (unsigned int x = 0; x < width; x++) {
		int x_pos {18 + (24 * x)};
		top.setPosition(x_pos, 0);
		gui_frame_texture.draw(top);
		bottom.setPosition(x_pos, texture_size.y - 10);
		gui_frame_texture.draw(bottom);
	}
	for (unsigned int y = 0; y < height; y++) {
		int y_pos {18 + (24 * y)};
		left.setPosition(0, y_pos);
		gui_frame_texture.draw(left);
		right.setPosition(texture_size.x - 11, y_pos);
		gui_frame_texture.draw(right);
	}

	gui_frame_texture.display();
	_gui_frame_texture = gui_frame_texture.getTexture();
	sf::Sprite gui_frame_sprite(_gui_frame_texture);
	return gui_frame_sprite;
}

// We generate the attract mode graphic in the main thread, though we generate the IDs in the animation threads
// https://en.sfml-dev.org/forums/index.php?topic=18672.0
auto Sorcery::Window::_get_attract_mode(std::vector<unsigned int> attract_mode_data) -> sf::Sprite {

	// Only regenerate if we have a change
	if (_attract_mode_data != attract_mode_data) {
		_attract_mode_data = attract_mode_data;
		const unsigned int number_to_display {attract_mode_data.size()};
		const sf::Vector2f texture_size(_creature_sprite_width * number_to_display + (_creature_sprite_spacing *
			(number_to_display - 1)), _creature_sprite_height);
		sf::RenderTexture attract_texture;
		attract_texture.create(texture_size.x, texture_size.y);
		attract_texture.setSmooth(true);
		attract_texture.clear();

		// Work out their Indexes and Positions
		unsigned int sprite_index {};
		unsigned int sprite_x {0};
		for (auto i: _attract_mode_data) {
			sf::Sprite sprite = get_creature_gfx(i, true);
			sprite.setPosition(sprite_x, 0);
			attract_texture.draw(sprite);
			sprite_x += (_creature_sprite_width + _creature_sprite_spacing);
		}

		attract_texture.display();
		_attract_mode_texture = attract_texture.getTexture();
		sf::Sprite attract_sprite(_attract_mode_texture);
		return attract_sprite;
	} else {
		sf::Sprite attract_sprite(_attract_mode_texture);
		return attract_sprite;
	}
}

auto Sorcery::Window::get_banner() -> sf::Sprite {
	sf::Sprite banner(_textures[BANNER_TEXTURE]);
	return banner;
}

auto Sorcery::Window::get_centre_x(int component_width) -> unsigned int {
	return (_current_size.w - component_width) / 2.0f;
}

auto Sorcery::Window::get_centre_y(int component_height) -> unsigned int {
	return (_current_size.h - component_height) / 2.0f;
}

auto Sorcery::Window::get_centre_x(sf::Transformable component) -> unsigned int {
	sf::Sprite sprite_type;
	sf::Text text_type;
	if (typeid(component) == typeid(sprite_type)) {
			auto sprite = dynamic_cast<sf::Sprite*>(&component);
			return (_current_size.w - sprite->getGlobalBounds().width) / 2.0f;
	} else if (typeid(component) == typeid(text_type)) {
			auto text = dynamic_cast<sf::Text*>(&component);
			return (_current_size.w - text->getGlobalBounds().width) / 2.0f;
	}
}

auto Sorcery::Window::get_centre_y(sf::Transformable component) -> unsigned int {
	sf::Sprite sprite_type;
	sf::Text text_type;
	if (typeid(component) == typeid(sprite_type)) {
			auto sprite = dynamic_cast<sf::Sprite*>(&component);
			return (_current_size.h - sprite->getGlobalBounds().height) / 2.0f;
	} else if (typeid(component) == typeid(text_type)) {
			auto text = dynamic_cast<sf::Text*>(&component);
			return (_current_size.h - text->getGlobalBounds().height) / 2.0f;
	}
}

auto Sorcery::Window::get_creature_gfx(const int creature_id, const bool known = true) -> sf::Sprite {
	sf::IntRect creature_rect {};
	sf::Sprite creature(_textures[CREATURES_TEXTURE]);
	creature_rect.left = (creature_id - 1) * _creature_sprite_width;
	creature_rect.width = _creature_sprite_width;
	creature_rect.top = known ? 0 : _creature_sprite_height;
	creature_rect.height = _creature_sprite_height;
	creature.setTextureRect(creature_rect);
	return creature;
}

auto Sorcery::Window::get_cursor() -> sf::Sprite {
	sf::Sprite cursor(_textures[UI_TEXTURE]);
	sf::IntRect cursor_rect(710, 310, 21, 28);
	cursor.setTextureRect(cursor_rect);
	return cursor;
}

auto Sorcery::Window::get_gui() -> tgui::Gui* {
	return &_gui;
}

auto Sorcery::Window::get_logo() -> sf::Sprite {
	sf::Sprite splash(_textures[LOGO_TEXTURE]);
	return splash;
}

auto Sorcery::Window::get_splash() -> sf::Sprite {
	sf::Sprite splash(_textures[SPLASH_TEXTURE]);
	return splash;
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
