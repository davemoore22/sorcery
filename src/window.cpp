// Copyright (C) 2021 Dave Moore
//
// This file is part of Sorcery: Dreams of the Mad Overlord.
//
// Sorcery: Dreams of the Mad Overlord is free software: you can redistribute
// it and/or modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation, either version 2 of the License,
// or (at your option) any later version.
//
// Sorcery: Dreams of the Mad Overlord is distributed in the hope that it wil
// be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
// Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Sorcery: Dreams of the Mad Overlord.  If not,
// see <http://www.gnu.org/licenses/>.
//
// If you modify this program, or any covered work, by linking or combining
// it with the libraries referred to in README (or a modified version of
// said libraries), containing parts covered by the terms of said libraries,
// the licensors of this program grant you additional permission to convey
// the resulting work.

#include "window.hpp"

// Standard Constructor
Sorcery::Window::Window(
	System *system, String *string, Layout *layout, const std::string &title)
	: _system{system}, _string{string}, _layout{layout}, _title{title} {

	// First get the Window Size from System Config
	_default_size.w =
		std::stoi(_system->config->get("Window", DEFAULT_SCREEN_WIDTH));
	_default_size.h =
		std::stoi(_system->config->get("Window", DEFAULT_SCREEN_HEIGHT));
	_current_size.w =
		std::stoi(_system->config->get("Window", CURRENT_SCREEN_WIDTH));
	_current_size.h =
		std::stoi(_system->config->get("Window", CURRENT_SCREEN_HEIGHT));

	// And then the Positioning Grid Cell Size
	_cell_height = std::stoi(_system->config->get("Grid", CELL_HEIGHT));
	_cell_width = std::stoi(_system->config->get("Grid", CELL_WIDTH));
	_layout->set_grid(_cell_width, _cell_height);

#ifdef linux
	XInitThreads();
#endif

	// Then create the SFML Window and the Main GUI
	_window.create(sf::VideoMode(_current_size.w, _current_size.h), _title,
		sf::Style::Titlebar | sf::Style::Resize | sf::Style::Close);
	_window.setFramerateLimit(FRAMERATE);
	centre.x = _current_size.w / 2;
	centre.y = _current_size.h / 2;
	_window.setVerticalSyncEnabled(true);
	_gui.setTarget(_window);
	_theme = tgui::Theme((*system->files)[GUI_FILE]);
	tgui::Theme::setDefault(&_theme);

	view = _window.getView();

	// Change the Mouse Cursor
	_window.setMouseCursorVisible(false);
	size = sf::IntRect(0, 0, _current_size.w, _current_size.h);

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
auto Sorcery::Window::draw_text(sf::Text &text) -> void {

	_draw_text(text);
}

auto Sorcery::Window::draw_text(sf::Text &text, const Component &component,
	const std::string &string) -> void {

	_draw_text(text, component, string);
}

auto Sorcery::Window::draw_text(sf::Text &text, const Component &component,
	const std::string &string, const double lerp) -> void {

	_draw_text(text, component, string, lerp);
}

auto Sorcery::Window::draw_text(
	sf::Text &text, const Component &component, const double lerp) -> void {

	_draw_text(text, component, lerp);
}

auto Sorcery::Window::_draw_text(sf::Text &text) -> void {

	_window.draw(text);
}

auto Sorcery::Window::_draw_text(
	sf::Text &text, const Component &component, const double lerp) -> void {

	int x{0};
	int y{0};
	text.setFont(_system->resources->fonts[component.font]);
	text.setCharacterSize(component.size);
	if (component.animated)
		text.setFillColor(
			sf::Color(_adjust_brightness(sf::Color(component.colour), lerp)));
	else
		text.setFillColor(sf::Color(component.colour));
	text.setString((*_string)[component.string_key]);
	x = component.x == -1 ? centre.x : component.x;
	y = component.y == -1 ? centre.y : component.y;
	if (component.justification == Justification::CENTRE) {
		text.setPosition(x, y);
		text.setOrigin(text.getLocalBounds().width / 2.0f,
			text.getLocalBounds().height / 2.0f);
	} else if (component.justification == Justification::RIGHT) {
		text.setPosition(x, y);
		const sf::FloatRect bounds{text.getLocalBounds()};
		text.setPosition(component.x - bounds.width, component.y);
		// TODO: Right doesn't put it down for some reason?
		// text.setOrigin(0, text.getLocalBounds().height / 2.0f);
	} else {
		text.setPosition(x, y);
		text.setOrigin(0, text.getLocalBounds().height / 2.0f);
	}

	// Handle varying height of proportional fonts
	if (component.font == FontType::PROPORTIONAL)
		text.setPosition(text.getPosition().x,
			text.getPosition().y -
				((component.size - text.getLocalBounds().height) / 2));

	_window.draw(text);
}

auto Sorcery::Window::_draw_text(sf::Text &text, const Component &component,
	const std::string &string) -> void {

	int x{0};
	int y{0};
	text.setFont(_system->resources->fonts[component.font]);
	text.setCharacterSize(component.size);
	text.setFillColor(sf::Color(component.colour));
	text.setString(string);
	x = component.x == -1 ? centre.x : component.x;
	y = component.y == -1 ? centre.y : component.y;
	if (component.justification == Justification::CENTRE) {
		text.setPosition(x, y);
		text.setOrigin(text.getLocalBounds().width / 2.0f,
			text.getLocalBounds().height / 2.0f);
	} else if (component.justification == Justification::RIGHT) {
		text.setPosition(x, y);
		const sf::FloatRect bounds{text.getLocalBounds()};
		text.setPosition(component.x - bounds.width, component.y);
		// TODO: Right doesn't put it down for some reason?
		// text.setOrigin(0, text.getLocalBounds().height / 2.0f);
	} else {
		text.setPosition(x, y);
		text.setOrigin(0, text.getLocalBounds().height / 2.0f);
	}

	// Handle varying height of proportional fonts
	if (component.font == FontType::PROPORTIONAL)
		text.setPosition(text.getPosition().x,
			text.getPosition().y -
				((component.size - text.getLocalBounds().height) / 2));

	_window.draw(text);
}

// Shove one text component next to another!
auto Sorcery::Window::shove_text(
	sf::Text &shovee, sf::Text &shover, unsigned int gap_units) -> void {

	shover.setPosition(shovee.getGlobalBounds().left +
						   shovee.getGlobalBounds().width +
						   (_cell_width * gap_units),
		shovee.getGlobalBounds().top - (shovee.getGlobalBounds().height / 4));
}

auto Sorcery::Window::set_pos(Component *component, sf::Transformable *object)
	-> void {

	const auto off_x{[&] {
		if ((*component)["offset_x"])
			return std::stoi((*component)["offset_x"].value());
		else
			return 0;
	}()};
	const auto off_y{[&] {
		if ((*component)["offset_y"])
			return std::stoi((*component)["offset_y"].value());
		else
			return 0;
	}()};

	object->setPosition(component->x + off_x, component->y + off_y);
}

auto Sorcery::Window::shove_text(
	sf::Text &shovee, sf::Text &shover, float gap_units) -> void {
	shover.setPosition(shovee.getGlobalBounds().left +
						   shovee.getGlobalBounds().width +
						   (_cell_width * gap_units),
		shovee.getGlobalBounds().top - (shovee.getGlobalBounds().height / 2) -
			2);
}

auto Sorcery::Window::_draw_text(sf::Text &text, const Component &component,
	const std::string &string, const double lerp) -> void {

	int x{0};
	int y{0};
	text.setFont(_system->resources->fonts[component.font]);
	text.setCharacterSize(component.size);
	if (component.animated)
		text.setFillColor(
			sf::Color(_adjust_brightness(sf::Color(component.colour), lerp)));
	else
		text.setFillColor(sf::Color(component.colour));
	text.setString(string);
	x = component.x == -1 ? centre.x : component.x;
	y = component.y == -1 ? centre.y : component.y;
	if (component.justification == Justification::CENTRE) {
		text.setPosition(x, y);
		text.setOrigin(text.getLocalBounds().width / 2.0f,
			text.getLocalBounds().height / 2.0f);
	} else if (component.justification == Justification::RIGHT) {
		text.setPosition(x, y);
		const sf::FloatRect bounds{text.getLocalBounds()};
		text.setPosition(component.x - bounds.width, component.y);
		// TODO: Right doesn't put it down for some reason?
		// text.setOrigin(0, text.getLocalBounds().height / 2.0f);
	} else {
		text.setPosition(x, y);
		text.setOrigin(0, text.getLocalBounds().height / 2.0f);
	}

	// Handle varying height of proportional fonts
	if (component.font == FontType::PROPORTIONAL)
		text.setPosition(text.getPosition().x,
			text.getPosition().y -
				((component.size - text.getLocalBounds().height) / 2));

	_window.draw(text);
}

auto Sorcery::Window::get_cursor() const -> sf::Sprite {

	sf::Sprite cursor(_system->resources->textures[GraphicsTexture::UI]);
	const sf::IntRect cursor_rect(710, 310, 21, 28);
	cursor.setTextureRect(cursor_rect);
	return cursor;
}

auto Sorcery::Window::get_gui() -> tgui::Gui * {

	return &_gui;
}

auto Sorcery::Window::get_window() -> sf::RenderWindow * {

	return &_window;
}

auto Sorcery::Window::get_x(const sf::Sprite &sprite, int x_position) const
	-> unsigned int {

	return _get_x(sprite, x_position);
}

auto Sorcery::Window::get_y(const sf::Sprite &sprite, int y_position) const
	-> unsigned int {

	return _get_y(sprite, y_position);
}

auto Sorcery::Window::get_x(const sf::Text &text, int x_position) const
	-> unsigned int {

	return _get_x(text, x_position);
}

auto Sorcery::Window::get_y(const sf::Text &text, int y_position) const
	-> unsigned int {

	return _get_y(text, y_position);
}

auto Sorcery::Window::get_x(unsigned int width, int x_position) const
	-> unsigned int {

	return _get_x(width, x_position);
}

auto Sorcery::Window::get_y(unsigned int width, int y_position) const
	-> unsigned int {

	return _get_y(width, y_position);
}

auto Sorcery::Window::_get_centre_x(unsigned int width) const -> unsigned int {

	return (_current_size.w - width) / 2.0f;
}

auto Sorcery::Window::_get_centre_x(const sf::Sprite &sprite) const
	-> unsigned int {

	return (_current_size.w - sprite.getGlobalBounds().width) / 2.0f;
}

auto Sorcery::Window::_get_centre_x(const sf::Text &text) const
	-> unsigned int {

	return (_current_size.w - text.getGlobalBounds().width) / 2.0f;
}

auto Sorcery::Window::_get_centre_y(unsigned int width) const -> unsigned int {

	return (_current_size.h - width) / 2.0f;
}

auto Sorcery::Window::_get_centre_y(const sf::Sprite &sprite) const
	-> unsigned int {

	return (_current_size.h - sprite.getGlobalBounds().height) / 2.0f;
}

auto Sorcery::Window::_get_centre_y(const sf::Text &text) const
	-> unsigned int {

	return (_current_size.h - text.getGlobalBounds().height) / 2.0f;
}

auto Sorcery::Window::_get_x(
	const sf::Sprite &sprite, const int x_position) const -> unsigned int {

	return x_position == -1 ? _get_centre_x(sprite) : x_position;
}

auto Sorcery::Window::_get_x(const sf::Text &text, const int x_position) const
	-> unsigned int {

	return x_position == -1 ? _get_centre_x(text) : x_position;
}

auto Sorcery::Window::_get_x(unsigned int width, const int x_position) const
	-> unsigned int {

	return x_position == -1 ? _get_centre_x(width) : x_position;
}

auto Sorcery::Window::_get_y(
	const sf::Sprite &sprite, const int y_position) const -> unsigned int {

	return y_position == -1 ? _get_centre_y(sprite) : y_position;
}

auto Sorcery::Window::_get_y(const sf::Text &text, const int y_position) const
	-> unsigned int {

	return y_position == -1 ? _get_centre_y(text) : y_position;
}

auto Sorcery::Window::_get_y(unsigned int width, const int y_position) const
	-> unsigned int {

	return y_position == -1 ? _get_centre_y(width) : y_position;
}

auto Sorcery::Window::get_ch() const -> unsigned int {

	return _cell_height;
}

auto Sorcery::Window::get_cw() const -> unsigned int {

	return _cell_width;
}

auto Sorcery::Window::hl_text(sf::Text &text, Component component,
	const double lerp) -> sf::RectangleShape {

	const sf::FloatRect text_rect{text.getGlobalBounds()};
	sf::RectangleShape text_bg(
		sf::Vector2(text_rect.width + 6, text_rect.height + 8));
	text_bg.setOrigin(0, 0 - text.getLocalBounds().height + 16);
	text_bg.setFillColor(
		sf::Color(_adjust_brightness(sf::Color(component.background), lerp)));
	text.setFillColor(sf::Color(component.colour));
	text.setOutlineColor(sf::Color(0, 0, 0));
	text.setOutlineThickness(2);
	return text_bg;
}

auto Sorcery::Window::set_input_mode(WindowInputMode value) -> void {

	_input_mode = value;
}
auto Sorcery::Window::get_input_mode() const -> WindowInputMode {

	return _input_mode;
}

auto Sorcery::Window::_adjust_brightness(sf::Color colour, double colour_lerp)
	-> unsigned long long {

	thor::ColorGradient gradient{};
	gradient[0.0f] = sf::Color(0x404040ff);
	gradient[0.5f] = colour;
	gradient[1.0f] = sf::Color(0xbfbfbfff);

	return (gradient.sampleColor(colour_lerp)).toInteger();
}
