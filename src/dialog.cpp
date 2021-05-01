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

#include "dialog.hpp"

// Standard Constructor
Sorcery::Dialog::Dialog(System *system, Display *display, Graphics *graphics, Component &frame_c,
	Component &text_c, WindowDialogType type)
	: _system{system}, _display{display}, _graphics{graphics}, _frame_c{frame_c}, _text_c{text_c},
	  _type{type} {

	_strings.clear();
	_texts.clear();
	_sprites.clear();
	_rects.clear();

	// Get the display text
	unsigned int text_width{_frame_c.w - 4};
	std::string string{(*_display->string)[_text_c.string_key]};
	std::string wrapped_text = WORDWRAP(string, text_width);

	// Split the display text into lines
	const std::regex regex(R"([@]+)");
	std::sregex_token_iterator it{wrapped_text.begin(), wrapped_text.end(), regex, -1};
	std::vector<std::string> split{it, {}};
	split.erase(std::remove_if(split.begin(), split.end(),
					[](std::string const &s) {
						return s.size() == 0;
					}),
		split.end());
	_strings = split;

	// Now work out the vertical size of the Frame
	unsigned int frame_h{static_cast<unsigned int>(_strings.size())};
	switch (_type) {
	case WindowDialogType::OK:
	case WindowDialogType::CONFIRM:
		frame_h += 5;
		break;
	case WindowDialogType::MENU:
		// frame_h += menu_size;
		break;
	case WindowDialogType::TIMED:
		frame_h += 3;
		break;
	default:
		break;
	}

	// Add the standard components - first the frame
	_frame = std::make_unique<Frame>(_display->ui_texture, WindowFrameType::NORMAL, _frame_c.w,
		frame_h, _frame_c.colour, _frame_c.alpha);
	_frame->setPosition(0, 0);

	// Then the strings
	unsigned int x{_display->window->get_cell_width() * 2};
	unsigned int y{_display->window->get_cell_height() * 2};
	int index{0};
	for (const auto &each_string : _strings) {
		sf::Text text;
		text.setFont(_system->resources->fonts[_text_c.font]);
		text.setCharacterSize(_text_c.size);
		text.setFillColor(sf::Color(_text_c.colour));
		text.setString(each_string);
		text.setPosition(x, y + (index * _display->window->get_cell_height()));
		if (_text_c.justification == Justification::CENTRE)
			text.setOrigin(text.getLocalBounds().width / 2.0f, text.getLocalBounds().height / 2.0f);
		else if (_text_c.justification == Justification::RIGHT) {
			const sf::FloatRect bounds{text.getLocalBounds()};
			text.setPosition(_text_c.x - bounds.width, _text_c.y);
		} else
			text.setOrigin(0, text.getLocalBounds().height / 2.0f);

		++index;
		_texts.push_back(text);
	}

	// Add the additional option texts/menu
	y = _display->window->get_cell_height() * (2 + static_cast<unsigned int>(_strings.size()) + 2);
	switch (_type) {
	case WindowDialogType::OK:

		break;
	case WindowDialogType::CONFIRM:
		break;
	case WindowDialogType::MENU:
		break;
	case WindowDialogType::TIMED:
		break;
	default:
		break;
	}
}

auto Sorcery::Dialog::check_for_mouse_move(const sf::Vector2f mouse_pos)
	-> std::optional<WindowDialogButton> {}

// Only works for the Mouse
auto Sorcery::Dialog::check_if_option_selected(const sf::Vector2f mouse_pos)
	-> std::optional<WindowDialogButton> {}

auto Sorcery::Dialog::set_selected(WindowDialogButton value) -> void {}

auto Sorcery::Dialog::get_selected() -> WindowDialogButton {}

auto Sorcery::Dialog::draw(sf::RenderTarget &target, sf::RenderStates state) const -> void {}
