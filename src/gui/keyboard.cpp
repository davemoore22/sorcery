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

#include "gui/keyboard.hpp"
#include "common/enum.hpp"
#include "core/animation.hpp"
#include "core/display.hpp"
#include "core/graphics.hpp"
#include "core/system.hpp"
#include "core/window.hpp"
#include "resources/componentstore.hpp"
#include "resources/resourcemanager.hpp"

// Standard Constructor
Sorcery::Keyboard::Keyboard(System *system, Display *display, Graphics *graphics)
	: _system{system}, _display{display}, _graphics{graphics} {

	// Get the Layout and Component Information (the Frame is the surrounding!)
	_frame_c = Component((*_display->layout)["keyboard:frame"]);
	_text_c = Component((*_display->layout)["keyboard:letters"]);
	_texts.clear();

	// Set up the Draw Surface
	_rtexture.create(_frame_c.w * _display->window->get_cw(), _frame_c.h * _display->window->get_ch());
	_rtexture.setSmooth(true);
	_rtexture.clear();

	// Get the Keyboard Layout
	const sf::Vector2i offset(std::stoi(_text_c["offset_x"].value()), std::stoi(_text_c["offset_y"].value()));
	const sf::Vector2i spacing{std::stoi(_text_c["spacing_x"].value()), std::stoi(_text_c["spacing_y"].value())};
	const auto max_col{std::stoi(_text_c["columns_per_row"].value())};

	sf::Vector2i letter_pos{offset.x, offset.y};
	auto current_col{1};

	// Letters A to Z uppercase
	for (char ascii = 'A'; ascii <= 'Z'; ascii++) {
		auto key{""s};
		key.push_back(ascii);
		_texts[key] = sf::Text();
		_texts[key].setString(key);
		_texts[key].setFont(_system->resources->fonts[_text_c.font]);
		_texts[key].setCharacterSize(_text_c.size);
		_texts[key].setFillColor(sf::Color(_text_c.colour));
		_texts[key].setPosition(letter_pos.x, letter_pos.y);
		//_texts[key].setOrigin(_texts[key].getLocalBounds().width / 2.0f, _texts[key].getLocalBounds().height / 2.0f);
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
				_texts[key].getPosition().y - ((_text_c.size - _texts[key].getLocalBounds().height) / 2));
	}

	// Space
	auto space{"Spc"s};
	_texts[space] = sf::Text();
	_texts[space].setString(space);
	_texts[space].setFont(_system->resources->fonts[_text_c.font]);
	_texts[space].setCharacterSize(_text_c.size - 24);
	_texts[space].setFillColor(sf::Color(_text_c.colour));
	_texts[space].setPosition(letter_pos.x, letter_pos.y + _texts[space].getLocalBounds().height / 1.5f);
	//_texts[space].setOrigin(_texts[space].getLocalBounds().width / 2.0f, _texts[space].getLocalBounds().height
	/// 2.0f);
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
			_texts[space].getPosition().y - ((_text_c.size - _texts[space].getLocalBounds().height) / 2));

	// Letters a to z lowercase
	for (char ascii = 'a'; ascii <= 'z'; ascii++) {
		auto key{""s};
		key.push_back(ascii);
		_texts[key] = sf::Text();
		_texts[key].setString(key);
		_texts[key].setFont(_system->resources->fonts[_text_c.font]);
		_texts[key].setCharacterSize(_text_c.size);
		_texts[key].setFillColor(sf::Color(_text_c.colour));
		_texts[key].setPosition(letter_pos.x, letter_pos.y);
		//_texts[key].setOrigin(_texts[key].getLocalBounds().width / 2.0f, _texts[key].getLocalBounds().height / 2.0f);
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
				_texts[key].getPosition().y - ((_text_c.size - _texts[key].getLocalBounds().height) / 2));
	}

	// Del, End
	auto del{"Del"s};
	_texts[del] = sf::Text();
	_texts[del].setString(del);
	_texts[del].setFont(_system->resources->fonts[_text_c.font]);
	_texts[del].setCharacterSize(_text_c.size - 24);
	_texts[del].setFillColor(sf::Color(_text_c.colour));
	_texts[del].setPosition(letter_pos.x, letter_pos.y + _texts[del].getLocalBounds().height / 1.5f);
	//_texts[del].setOrigin(_texts[del].getLocalBounds().width / 2.0f, _texts[del].getLocalBounds().height / 2.0f);
	if (current_col < max_col) {
		letter_pos.x += spacing.x;
		++current_col;
	} else {
		letter_pos.x = offset.x;
		letter_pos.y += spacing.y;
	}
	if (_text_c.font == FontType::PROPORTIONAL)
		_texts[del].setPosition(_texts[del].getPosition().x,
			_texts[del].getPosition().y - ((_text_c.size - _texts[del].getLocalBounds().height) / 2));

	auto end{"End"s};
	_texts[end] = sf::Text();
	_texts[end].setString(end);
	_texts[end].setFont(_system->resources->fonts[_text_c.font]);
	_texts[end].setCharacterSize(_text_c.size - 24);
	_texts[end].setFillColor(sf::Color(_text_c.colour));
	_texts[end].setPosition(letter_pos.x, letter_pos.y + _texts[end].getLocalBounds().height / 1.5f);
	//_texts[end].setOrigin(_texts[end].getLocalBounds().width / 2.0f, _texts[end].getLocalBounds().height / 2.0f);
	if (_text_c.font == FontType::PROPORTIONAL)
		_texts[end].setPosition(_texts[end].getPosition().x,
			_texts[end].getPosition().y - ((_text_c.size - _texts[end].getLocalBounds().height) / 2));

	_frame = std::make_unique<Frame>(
		_display->ui_texture, _frame_c.w - 2, _frame_c.h, _frame_c.colour, _frame_c.background, _frame_c.alpha);
	_frame->setPosition(
		_display->window->get_x(_frame->sprite, _frame_c.x), _display->window->get_y(_frame->sprite, _frame_c.y));

	// Render the background (inset by the frame)
	sf::RectangleShape rect(sf::Vector2f(
		(_display->window->get_cw() * (_frame_c.w)) - 20, (_display->window->get_ch() * (_frame_c.h)) - 20));
	rect.setFillColor(sf::Color(0, 0, 0, _frame_c.alpha));
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

	// Handle END DEL SPC differently!
	const auto offset_x{std::stoi(_text_c["highlight_offset_x"].value())};
	const auto offset_y{std::stoi(_text_c["highlight_offset_y"].value())};
	const auto highlight_div_x{std::stof(_text_c["highlight_scale_x"].value())};
	const auto highlight_div_y{std::stof(_text_c["highlight_scale_y"].value())};
	sf::FloatRect text_rect;
	if (const std::string letter{_texts.at(selected).getString()}; letter.length() > 1) {
		text_rect =
			sf::FloatRect(_texts.at(selected).getPosition().x * 1.0f, _texts.at(selected).getPosition().y * 1.0f,
				_text_c.size * (letter.length() / highlight_div_x), _text_c.size * highlight_div_y);
	} else {
		text_rect = sf::FloatRect(_texts.at(selected).getPosition().x * 1.0f,
			_texts.at(selected).getPosition().y * 1.0f, _text_c.size * 1.0f, _text_c.size * 1.0f);
	}

	// Find the text that is highlighted

	_selected_bg = sf::RectangleShape(sf::Vector2(text_rect.width, text_rect.height));
	_selected_bg.setFillColor(_graphics->animation->selected_colour);
	_selected_bg.setPosition(
		_texts.at(selected).getPosition().x + offset_x, _texts.at(selected).getPosition().y + offset_y);
	_texts.at(selected).setFillColor(sf::Color(_text_c.colour));
	_texts.at(selected).setOutlineColor(sf::Color(0, 0, 0));
	_texts.at(selected).setOutlineThickness(2);
}

auto Sorcery::Keyboard::set_selected(CIN input) -> void {

	// TODO: This will sadly break if the columns in the key grid is changed -
	// need to change this to use something else to handle precedent and
	// succedent selected
	switch (input) {
	case CIN::UP:
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
			std::transform(selected.begin(), selected.end(), selected.begin(), [](unsigned char c) -> unsigned char {
				if (c >= 'A' && c <= 'H')
					return c += 50;
				else if (c >= 'K' && c <= 'O')
					return c += 35;
				else if (c >= 'P' && c <= 'Z')
					return c -= 15;
				else if (c >= 'a' && c <= 'n')
					return c -= 20;
				else if (c >= 'p' && c <= 'z')
					return c -= 15;
				else
					return c;
			});
		}
		break;
	case CIN::DOWN:
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
			std::transform(selected.begin(), selected.end(), selected.begin(), [](unsigned char c) -> unsigned char {
				if (c >= 'A' && c <= 'K')
					return c += 15;
				else if (c >= 'M' && c <= 'Z')
					return c += 20;
				else if (c >= 'a' && c <= 'k')
					return c += 15;
				else if (c >= 'n' && c <= 'r')
					return c -= 35;
				else if (c >= 's' && c <= 'z')
					return c -= 50;
				else
					return c;
			});
		}
		break;
	case CIN::LEFT:
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
				[](unsigned char c) -> unsigned char { return --c; });
		}
		break;
	case CIN::RIGHT:
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
				[](unsigned char c) -> unsigned char { return ++c; });
		}
		break;
	case CIN::BACK:
	case CIN::CANCEL:
		selected = "Del";
		break;
	default:
		break;
	}
}

auto Sorcery::Keyboard::set_mouse_selected(Component &component, sf::Vector2f mouse_pos) -> std::optional<std::string> {

	// Now look through the global positions of each text object and see if it
	// matches the mouse position (remembering that we need to add in the
	// position of the this keyboard object)

	// TODO: for all drawables, pass its position in in the constructor
	for (auto &[key, text] : _texts) {
		sf::Rect text_area{text.getGlobalBounds()};
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
