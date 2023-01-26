// Copyright (C) 2023 Dave Moore
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

#include "confirm.hpp"

// Standard Constructor
Sorcery::Confirm::Confirm(System *system, Display *display, Graphics *graphics, Component &gui_c, Component &text_c)
	: _system{system}, _display{display}, _graphics{graphics}, _gui_c{gui_c}, _text_c{text_c} {

	_window = _display->window->get_window();
	highlighted = WindowConfirm::NO;

	_strings.clear();
	_texts.clear();

	// Wrap the display lines
	auto chunk_size{_gui_c.w};
	auto string{(*_display->string)[_text_c.string_key]};
	auto wrapped_text{WORDWRAP(string, chunk_size)};

	// Split the display lines into a vector
	const std::regex regex(R"([@]+)");
	std::sregex_token_iterator it{wrapped_text.begin(), wrapped_text.end(), regex, -1};
	std::vector<std::string> split{it, {}};
	split.erase(
		std::remove_if(split.begin(), split.end(), [](std::string const &s) { return s.size() == 0; }), split.end());
	_strings = split;

	auto x{_gui_c.x + _display->window->get_cw() * 2};
	auto y{_gui_c.y + _display->window->get_ch() * 2};
	auto index{0};
	for (auto &each_string : _strings) {
		sf::Text text{};
		text.setFont(_system->resources->fonts[_text_c.font]);
		text.setCharacterSize(_text_c.size);
		text.setFillColor(sf::Color(_text_c.colour));
		std::transform(each_string.begin(), each_string.end(), each_string.begin(), ::toupper);
		text.setString(each_string);
		text.setPosition(x, y + (index * _display->window->get_ch()));
		// text.setStyle(sf::Text::Bold);
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
}

auto Sorcery::Confirm::check_for_mouse_move(const sf::Vector2f mouse_pos) -> std::optional<WindowConfirm> {

	if (_yes_bg_rect.contains(mouse_pos)) {
		highlighted = WindowConfirm::YES;
		return highlighted;
	} else if (_no_bg_tect.contains(mouse_pos)) {
		highlighted = WindowConfirm::NO;
		return highlighted;
	} else
		return std::nullopt;
}

// Only works for the Mouse
auto Sorcery::Confirm::check_if_option_selected(const sf::Vector2f mouse_pos) -> std::optional<WindowConfirm> {

	if (_yes_bg_rect.contains(mouse_pos)) {
		highlighted = WindowConfirm::YES;
		return highlighted;
	} else if (_no_bg_tect.contains(mouse_pos)) {
		highlighted = WindowConfirm::NO;
		return highlighted;
	} else
		return std::nullopt;
}

auto Sorcery::Confirm::toggle_highlighted() -> WindowConfirm {

	if (highlighted == WindowConfirm::YES)
		highlighted = WindowConfirm::NO;
	else if (highlighted == WindowConfirm::NO)
		highlighted = WindowConfirm::YES;
	return highlighted;
}

auto Sorcery::Confirm::draw(const double lerp) -> void {

	// Generate back frame
	_frame = std::make_unique<Frame>(
		_display->ui_texture, _gui_c.w, _strings.size() + 5, _gui_c.colour, _gui_c.background, _gui_c.alpha);
	_frame->setPosition(
		_display->window->get_x(_frame->sprite, _gui_c.x), _display->window->get_y(_frame->sprite, _gui_c.y));
	_window->draw(*_frame);

	// Display Confirmation Message
	for (const auto &text : _texts)
		_window->draw(text);

	// Draw Yes / No (and highlight them depending on which one chosen)
	const auto yes_no_y{_text_c.y + (_display->window->get_ch() * 2)};
	const auto yes_x{_display->window->centre.x - (_display->window->get_cw() * 4)};
	const auto no_x{_display->window->centre.x + (_display->window->get_cw() * 2)};

	// And the Buttons
	_yes_text.setFont(_system->resources->fonts[_text_c.font]);
	_yes_text.setCharacterSize(_text_c.size);
	_yes_text.setFillColor(sf::Color(_text_c.colour));
	_yes_text.setString("YES");
	_yes_text.setPosition(yes_x, yes_no_y);

	_no_text.setFont(_system->resources->fonts[_text_c.font]);
	_no_text.setCharacterSize(_text_c.size);
	_no_text.setFillColor(sf::Color(_text_c.colour));
	_no_text.setString("NO");
	_no_text.setPosition(no_x, yes_no_y);

	// Draw backgrounds
	if (highlighted == WindowConfirm::YES) {
		const sf::RectangleShape yes_bg{_display->window->hl_text(_yes_text, _text_c, lerp)};
		_window->draw(yes_bg, _yes_text.getTransform());
	} else if (highlighted == WindowConfirm::NO) {
		const sf::RectangleShape no_bg{_display->window->hl_text(_no_text, _text_c, lerp)};
		_window->draw(no_bg, _no_text.getTransform());
	}

	_yes_bg_rect = _yes_text.getGlobalBounds();
	_no_bg_tect = _no_text.getGlobalBounds();
	_window->draw(_yes_text);
	_window->draw(_no_text);
}
