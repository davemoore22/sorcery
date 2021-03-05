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

#include "keyboard.hpp"

// Standard Constructor
Sorcery::Keyboard::Keyboard(System &system, Display &display, Graphics &graphics)
	: _system{system}, _display{display}, _graphics{graphics} {

	// Get the Layout and Component Information (the Frame is the surrounding!)
	_frame_c = Component((*_display.layout)["keyboard:frame"]);
	_text_c = Component((*_display.layout)["keyboard:letters"]);
	_texts.clear();

	// Set up the Draw Surface
	_rtexture.create(_frame_c.w * _display.window->get_cell_width(),
		_frame_c.h * _display.window->get_cell_height());
	_rtexture.setSmooth(true);
	_rtexture.clear();

	// Use x and y for offset (remember to divide this by cell dimensions), w and h for letter
	// spacing, and the scale for the number of letters per row
	const sf::Vector2i offset(_text_c.x / _display.window->get_cell_width(),
		_text_c.y / _display.window->get_cell_height());
	const sf::Vector2u spacing{_text_c.w, _text_c.h};
	const int max_col{static_cast<int>(_text_c.scale)};
	sf::Vector2i letter_pos{offset.x, offset.y};
	int current_col{1};

	// Letters A to Z uppercase
	for (char ascii = 'A'; ascii <= 'Z'; ascii++) {
		std::string key{};
		key.push_back(ascii);
		_texts[key] = sf::Text();
		_texts[key].setString(key);
		_texts[key].setFont(_system.resources->fonts[_text_c.font]);
		_texts[key].setCharacterSize(_text_c.size);
		_texts[key].setFillColor(sf::Color(_text_c.colour));
		_texts[key].setPosition(letter_pos.x, letter_pos.y);
		_texts[key].setOrigin(
			_texts[key].getLocalBounds().width / 2.0f, _texts[key].getLocalBounds().height / 2.0f);
		if (current_col < max_col) {
			letter_pos.x += spacing.x;
			++current_col;
		} else {
			letter_pos.x = offset.x;
			letter_pos.y += spacing.y;
			current_col = 1;
		}
		if (_text_c.font == FontType::PROPORTIONAL)
			_texts[key].setPosition(_texts[key].getPosition().x,
				_texts[key].getPosition().y -
					((_text_c.size - _texts[key].getLocalBounds().height) / 2));
	}

	// Space
	std::string space{"Spc"};
	_texts[space] = sf::Text();
	_texts[space].setString(space);
	_texts[space].setFont(_system.resources->fonts[_text_c.font]);
	_texts[space].setCharacterSize(_text_c.size - 24);
	_texts[space].setFillColor(sf::Color(_text_c.colour));
	_texts[space].setPosition(
		letter_pos.x, letter_pos.y + _texts[space].getLocalBounds().height / 1.5f);
	_texts[space].setOrigin(
		_texts[space].getLocalBounds().width / 2.0f, _texts[space].getLocalBounds().height / 2.0f);
	if (current_col < max_col) {
		letter_pos.x += spacing.x;
		++current_col;
	} else {
		letter_pos.x = offset.x;
		letter_pos.y += spacing.y;
		current_col = 1;
	}
	if (_text_c.font == FontType::PROPORTIONAL)
		_texts[space].setPosition(_texts[space].getPosition().x,
			_texts[space].getPosition().y -
				((_text_c.size - _texts[space].getLocalBounds().height) / 2));

	// Letters a to z lowercase
	for (char ascii = 'a'; ascii <= 'z'; ascii++) {
		std::string key{};
		key.push_back(ascii);
		_texts[key] = sf::Text();
		_texts[key].setString(key);
		_texts[key].setFont(_system.resources->fonts[_text_c.font]);
		_texts[key].setCharacterSize(_text_c.size);
		_texts[key].setFillColor(sf::Color(_text_c.colour));
		_texts[key].setPosition(letter_pos.x, letter_pos.y);
		_texts[key].setOrigin(
			_texts[key].getLocalBounds().width / 2.0f, _texts[key].getLocalBounds().height / 2.0f);
		if (current_col < max_col) {
			letter_pos.x += spacing.x;
			++current_col;
		} else {
			letter_pos.x = offset.x;
			letter_pos.y += spacing.y;
			current_col = 1;
		}
		if (_text_c.font == FontType::PROPORTIONAL)
			_texts[key].setPosition(_texts[key].getPosition().x,
				_texts[key].getPosition().y -
					((_text_c.size - _texts[key].getLocalBounds().height) / 2));
	}

	// Del, End
	std::string del{"Del"};
	_texts[del] = sf::Text();
	_texts[del].setString(del);
	_texts[del].setFont(_system.resources->fonts[_text_c.font]);
	_texts[del].setCharacterSize(_text_c.size - 24);
	_texts[del].setFillColor(sf::Color(_text_c.colour));
	_texts[del].setPosition(
		letter_pos.x, letter_pos.y + _texts[del].getLocalBounds().height / 1.5f);
	_texts[del].setOrigin(
		_texts[del].getLocalBounds().width / 2.0f, _texts[del].getLocalBounds().height / 2.0f);
	if (current_col < max_col) {
		letter_pos.x += spacing.x;
		++current_col;
	} else {
		letter_pos.x = offset.x;
		letter_pos.y += spacing.y;
	}
	if (_text_c.font == FontType::PROPORTIONAL)
		_texts[del].setPosition(_texts[del].getPosition().x,
			_texts[del].getPosition().y -
				((_text_c.size - _texts[del].getLocalBounds().height) / 2));

	std::string end{"End"};
	_texts[end] = sf::Text();
	_texts[end].setString(end);
	_texts[end].setFont(_system.resources->fonts[_text_c.font]);
	_texts[end].setCharacterSize(_text_c.size - 24);
	_texts[end].setFillColor(sf::Color(_text_c.colour));
	_texts[end].setPosition(
		letter_pos.x, letter_pos.y + _texts[end].getLocalBounds().height / 1.5f);
	_texts[end].setOrigin(
		_texts[end].getLocalBounds().width / 2.0f, _texts[end].getLocalBounds().height / 2.0f);
	if (_text_c.font == FontType::PROPORTIONAL)
		_texts[end].setPosition(_texts[end].getPosition().x,
			_texts[end].getPosition().y -
				((_text_c.size - _texts[end].getLocalBounds().height) / 2));

	_frame = std::make_unique<Frame>(
		_display.ui_texture, WindowFrameType::NORMAL, _frame_c.w - 2, _frame_c.h, _frame_c.alpha);
	_frame->setPosition(_display.window->get_x(_frame->sprite, _frame_c.x),
		_display.window->get_y(_frame->sprite, _frame_c.y));

	// Render the background (inset by the frame)
	sf::RectangleShape rect(sf::Vector2f((_display.window->get_cell_width() * (_frame_c.w)) - 20,
		(_display.window->get_cell_height() * (_frame_c.h)) - 20));
	rect.setFillColor(sf::Color(255, 0, 0, _frame_c.alpha));
	rect.setPosition(10, 10);
	_rtexture.draw(rect);

	// and the Frame Background
	_fsprite = _frame->sprite;
	_fsprite.setPosition(0, 0);
	_rtexture.draw(_fsprite);

	_rtexture.display();
	_texture = _rtexture.getTexture();
	sprite = sf::Sprite(_texture);

	width = sprite.getLocalBounds().width;
	height = sprite.getLocalBounds().height;

	selected = "A";
}

auto Sorcery::Keyboard::set_selected_background() -> void {

	// Find the text that is highlighted
	const sf::FloatRect text_rect{_texts.at(selected).getPosition().x * 1.0f,
		_texts.at(selected).getPosition().y * 1.0f, _text_c.size * 1.0f, _text_c.size * 1.0f};
	_selected_bg = sf::RectangleShape(sf::Vector2(text_rect.width, text_rect.height));
	_selected_bg.setFillColor(_display.window->change_colour(
		sf::Color(_text_c.background), _graphics.animation->colour_lerp));
	_selected_bg.setPosition(_texts.at(selected).getPosition().x - (_text_c.size / 2),
		_texts.at(selected).getPosition().y - 8);
	_texts.at(selected).setFillColor(sf::Color(_text_c.colour));
	_texts.at(selected).setOutlineColor(sf::Color(0, 0, 0));
	_texts.at(selected).setOutlineThickness(2);
}

auto Sorcery::Keyboard::set_selected(WindowInput input) -> void {

	// TODO: This will sadly break if the columns in the key grid is changed - need to change this
	// to use something else to handle precedent and succedent selected
	switch (input) {
	case WindowInput::UP:
		if (selected == "J")
			selected = "End";
		else if (selected == "I")
			selected = "Del";
		else if (selected == "Del")
			selected = "l";
		else if (selected == "End")
			selected = "m";
		else if (selected == "Spc")
			selected = "L";
		else if (selected == "o")
			selected = "Spc";
		else {
			std::transform(selected.begin(), selected.end(), selected.begin(),
				[](unsigned char c) -> unsigned char {
					if ((c >= 'A') && (c <= 'H'))
						return c += 50;
					else if ((c >= 'K') && (c <= 'O'))
						return c += 35;
					else if ((c >= 'P') && (c <= 'Z'))
						return c -= 15;
					else if ((c >= 'a') && (c <= 'n'))
						return c -= 20;
					else if ((c >= 'p') && (c <= 'z'))
						return c -= 15;
					else
						return c;
				});
		}
		break;
	case WindowInput::DOWN:
		if (selected == "End")
			selected = "J";
		else if (selected == "Del")
			selected = "I";
		else if (selected == "Spc")
			selected = "o";
		else if (selected == "l")
			selected = "Del";
		else if (selected == "m")
			selected = "End";
		else if (selected == "L")
			selected = "Spc";
		else {
			std::transform(selected.begin(), selected.end(), selected.begin(),
				[](unsigned char c) -> unsigned char {
					if ((c >= 'A') && (c <= 'K'))
						return c += 15;
					else if ((c >= 'M') && (c <= 'Z'))
						return c += 20;
					else if ((c >= 'a') && (c <= 'k'))
						return c += 15;
					else if ((c >= 'n') && (c <= 'r'))
						return c -= 35;
					else if ((c >= 's') && (c <= 'z'))
						return c -= 50;
					else
						return c;
				});
		}
		break;
	case WindowInput::LEFT:
		if (selected == "A")
			selected = "End";
		else if (selected == "End")
			selected = "Del";
		else if (selected == "Del")
			selected = "z";
		else if (selected == "a")
			selected = "Spc";
		else if (selected == "Spc")
			selected = "Z";
		else {
			std::transform(selected.begin(), selected.end(), selected.begin(),
				[](unsigned char c) -> unsigned char {
					return --c;
				});
		}
		break;
	case WindowInput::RIGHT:
		if (selected == "End")
			selected = "A";
		else if (selected == "Del")
			selected = "End";
		else if (selected == "z")
			selected = "Del";
		else if (selected == "Spc")
			selected = "a";
		else if (selected == "Z")
			selected = "Spc";
		else {
			std::transform(selected.begin(), selected.end(), selected.begin(),
				[](unsigned char c) -> unsigned char {
					return ++c;
				});
		}
		break;
	case WindowInput::BACK:
	case WindowInput::CANCEL:
		selected = "Del";
		break;
	default:
		break;
	}
}

auto Sorcery::Keyboard::set_mouse_selected(Component &component, sf::Vector2f mouse_pos)
	-> std::optional<std::string> {

	// Now look through the global positions of each text object and see if it matches the mouse
	// position (remembering that we need to add in the position of the this keyboard object)

	// TODO: for all drawables, pass its position in in the constructor
	for (auto &[key, text] : _texts) {
		sf::Rect text_area = text.getGlobalBounds();
		text_area.left += component.x;
		text_area.top += component.y;
		if (text_area.contains(mouse_pos))
			return key;
	}

	return std::nullopt;
}

auto Sorcery::Keyboard::draw(sf::RenderTarget &target, sf::RenderStates states) const -> void {

	// Draw the backgroud and Frame
	states.transform *= getTransform();
	target.draw(sprite, states);

	// Selected Background
	target.draw(_selected_bg, states);

	// Now each text
	for (auto &[key, text] : _texts) {
		target.draw(text, states);
	}
}
