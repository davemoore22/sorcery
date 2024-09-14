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

#include "gui/dialog.hpp"
#include "common/enum.hpp"
#include "common/macro.hpp"
#include "core/animation.hpp"
#include "core/display.hpp"
#include "core/graphics.hpp"
#include "core/input.hpp"
#include "core/system.hpp"
#include "core/window.hpp"
#include "gui/define.hpp"
#include "resources/componentstore.hpp"
#include "resources/resourcemanager.hpp"
#include "resources/stringstore.hpp"

// Standard Constructor
Sorcery::Dialog::Dialog(
	System *system, Display *display, Graphics *graphics, Component &frame_c, Component &string_c, WDT type)
	: _system{system}, _display{display}, _graphics{graphics}, _frame_c{frame_c}, _string_c{string_c}, _type{type} {

	_refresh(string_c, std::string{""});
}

auto Sorcery::Dialog::set(Component &string_c, const std::string &new_text) -> void {

	_refresh(string_c, new_text);
}

auto Sorcery::Dialog::get_width() const -> unsigned int {

	return _width;
}

auto Sorcery::Dialog::get_height() const -> unsigned int {

	return _height;
}

auto Sorcery::Dialog::get_size() const -> sf::Vector2f {

	return sf::Vector2f{_width * 1.0f, _height * 1.0f};
}

auto Sorcery::Dialog::_refresh(Component &string_c, const std::string &new_text) -> void {
	_strings.clear();
	_texts.clear();
	_sprites.clear();
	_rects.clear();
	_buttons.clear();
	_buttons_hl.clear();
	_highlights.clear();
	_buttons_fr.clear();

	// For timed dialogs
	_start = std::nullopt;
	_current_time = std::nullopt;
	_valid = true;
	_duration = DELAY_DIALOG; // ms

	// Get the Window
	_window = _display->window->get_window();

	// Get the Layout
	_text_c = Component((*_display->layout)["dialog:text"]);
	_buttons_c = Component((*_display->layout)["dialog:buttons"]);

	// Get the Text
	auto text_width{_frame_c.w - 4};
	auto string{std::invoke([&] {
		if (!new_text.empty())
			return new_text;
		else
			return (*_display->string)[string_c.string_key];
	})};
	auto wrapped_text{WORDWRAP(string, text_width)};

	// Split the Text into lines
	const std::regex regex(R"([@]+)");
	std::sregex_token_iterator it{wrapped_text.begin(), wrapped_text.end(), regex, -1};
	std::vector<std::string> split{it, {}};
	split.erase(
		std::remove_if(split.begin(), split.end(), [](std::string_view s) { return s.size() == 0; }), split.end());
	_strings = split;

	// Now work out the vertical size of the Frame
	auto frame_h{static_cast<unsigned int>(_strings.size())};
	switch (_type) {
	case WDT::OK:
		frame_h += 4;
		_selected = WDB::OK;
		break;
	case WDT::CONFIRM:
		frame_h += 4;
		_selected = WDB::NO;
		break;
	case WDT::MENU:
		// frame_h += menu_size;
		break;
	case WDT::TIMED:
		_selected = WDB::NO_BUTTON;
		frame_h += 2;
		break;
	default:
		break;
	}

	// Add the standard components - first the frame
	_frame = std::make_unique<Frame>(
		_display->ui_texture, _frame_c.w, frame_h, _frame_c.colour, _frame_c.background, _frame_c.alpha);
	_frame->setPosition(0, 0);

	// Note we can use the calculated size of the frame as the size of the component
	_width = _frame->get_width();
	_height = _frame->get_height();

	// Then the strings
	auto x{0};
	auto y{_display->window->get_ch() * 1};
	auto index{0};
	for (auto &each_string : _strings) {
		sf::Text text{};
		if (_display->get_bold())
			text.setStyle(sf::Text::Bold);
		text.setFont(_system->resources->fonts[_string_c.font]);
		text.setCharacterSize(_string_c.size);
		text.setFillColor(sf::Color(_string_c.colour));
		if (_display->get_upper())
			std::transform(each_string.begin(), each_string.end(), each_string.begin(), ::toupper);
		text.setString(each_string);

		if (_string_c.justification == Justification::CENTRE) {
			text.setPosition(
				(x + (_frame_c.w * _display->window->get_cw()) / 2), y + (index * _display->window->get_ch()));
			text.setOrigin(text.getLocalBounds().width / 2.0f, 0);
		} else if (_string_c.justification == Justification::RIGHT) {
			text.setPosition(x, y + (index * _display->window->get_ch()));
			const sf::FloatRect bounds{text.getLocalBounds()};
			text.setPosition(_string_c.x - bounds.width, _string_c.y);
		} else {
			text.setPosition(x, y + (index * _display->window->get_ch()));
			text.setOrigin(0, 0);
		}

		++index;
		_texts.push_back(text);
	}

	// Add the additional option texts/menu
	y = _display->window->get_ch() * (1 + static_cast<unsigned int>(_strings.size()) + 1);
	const auto centre_x{(_frame_c.w * _display->window->get_cw()) / 2};

	switch (_type) {
	case WDT::OK: {
		const auto ok_x{(centre_x - (_display->window->get_cw() * 2)) + (_display->window->get_cw() * 2)};
		sf::Text ok_text{};
		if (_display->get_bold())
			ok_text.setStyle(sf::Text::Bold);
		ok_text.setFont(_system->resources->fonts[_buttons_c.font]);
		ok_text.setCharacterSize(_buttons_c.size);
		ok_text.setFillColor(sf::Color(_buttons_c.colour));
		ok_text.setString((*_display->string)["DIALOG_OK"]);
		ok_text.setPosition(ok_x, y);
		_buttons[WDB::OK] = ok_text;

		sf::Text ok_text_hl{ok_text};
		ok_text_hl.setOutlineColor(sf::Color(0, 0, 0));
		ok_text_hl.setOutlineThickness(2);
		_buttons_hl[WDB::OK] = ok_text_hl;

		const sf::FloatRect ok_text_rect{ok_text_hl.getGlobalBounds()};
		_buttons_fr[WDB::OK] = ok_text_rect;

		sf::RectangleShape ok_text_bg(sf::Vector2(ok_text_rect.width, ok_text_rect.height));
		ok_text_bg.setPosition(ok_x, y);

		_highlights[WDB::OK] = ok_text_bg;

	} break;
	case WDT::CONFIRM: {
		const auto yes_x{(centre_x - (_display->window->get_cw() * 4))};
		sf::Text yes_text{};
		if (_display->get_bold())
			yes_text.setStyle(sf::Text::Bold);
		yes_text.setFont(_system->resources->fonts[_buttons_c.font]);
		yes_text.setCharacterSize(_buttons_c.size);
		yes_text.setFillColor(sf::Color(_buttons_c.colour));
		yes_text.setString((*_display->string)["DIALOG_YES"]);
		yes_text.setPosition(yes_x, y);
		_buttons[WDB::YES] = yes_text;

		sf::Text yes_text_hl{yes_text};
		yes_text_hl.setPosition(yes_x, y);
		yes_text_hl.setOutlineColor(sf::Color(0, 0, 0));
		yes_text_hl.setOutlineThickness(2);
		_buttons_hl[WDB::YES] = yes_text_hl;

		const sf::FloatRect yes_text_rect{yes_text_hl.getGlobalBounds()};
		_buttons_fr[WDB::YES] = yes_text_rect;

		sf::RectangleShape yes_text_bg(sf::Vector2(yes_text_rect.width, yes_text_rect.height));
		yes_text_bg.setPosition(yes_x, y);

		_highlights[WDB::YES] = yes_text_bg;

		const auto no_x{centre_x + (_display->window->get_cw() * 2)};
		sf::Text no_text{};
		if (_display->get_bold())
			no_text.setStyle(sf::Text::Bold);
		no_text.setFont(_system->resources->fonts[_buttons_c.font]);
		no_text.setCharacterSize(_buttons_c.size);
		no_text.setFillColor(sf::Color(_buttons_c.colour));
		no_text.setString((*_display->string)["DIALOG_NO"]);
		no_text.setPosition(no_x, y);
		_buttons[WDB::NO] = no_text;

		sf::Text no_text_hl{no_text};
		no_text_hl.setPosition(no_x, y);
		no_text_hl.setOutlineColor(sf::Color(0, 0, 0));
		no_text_hl.setOutlineThickness(2);
		_buttons_hl[WDB::NO] = no_text_hl;

		const sf::FloatRect no_text_rect{no_text_hl.getGlobalBounds()};
		_buttons_fr[WDB::NO] = no_text_rect;

		sf::RectangleShape no_text_bg(sf::Vector2(no_text_rect.width, no_text_rect.height));
		no_text_bg.setPosition(no_x, y);
		_highlights[WDB::NO] = no_text_bg;

	} break;
	case WDT::MENU:
		break;
	case WDT::TIMED:
		break;
	default:
		break;
	}
}

auto Sorcery::Dialog::handle_input(sf::Event event) -> std::optional<WDB> {

	if (event.type == sf::Event::Closed)
		return WDB::CLOSE;

	if (_system->input->check(CIN::SHOW_CONTROLS, event)) {
		_display->show_overlay();
		return std::nullopt;
	} else
		_display->hide_overlay();

	switch (_type) {
	case WDT::OK:

		if (_system->input->check(CIN::MOVE, event))
			check_for_mouse_move(static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
		else if (_system->input->check(CIN::CONFIRM, event)) {
			std::optional<WDB> button_chosen{
				check_if_option_selected(static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)))};
			if (button_chosen) {
				if (button_chosen.value() == WDB::OK)
					return WDB::OK;
			} else {

				if (get_selected() == WDB::OK) {
					return WDB::OK;
				}
			}
		}

		break;
	case WDT::CONFIRM:
		if (_system->input->check(CIN::LEFT, event))
			toggle_highlighted();
		else if (_system->input->check(CIN::RIGHT, event))
			toggle_highlighted();
		else if (_system->input->check(CIN::YES, event))
			set_selected(WDB::YES);
		else if (_system->input->check(CIN::NO, event))
			set_selected(WDB::NO);
		else if (_system->input->check(CIN::CANCEL, event))
			return WDB::NO;
		else if (_system->input->check(CIN::BACK, event))
			return WDB::NO;
		else if (_system->input->check(CIN::MOVE, event))
			check_for_mouse_move(static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
		else if (_system->input->check(CIN::CONFIRM, event)) {
			std::optional<WDB> button_chosen{
				check_if_option_selected(static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)))};

			// Mouse click only
			if (button_chosen) {
				if (button_chosen.value() == WDB::YES)
					return WDB::YES;
				else if (button_chosen.value() == WDB::NO)
					return WDB::NO;

			} else {

				// Button/Keyboard
				if (get_selected() == WDB::YES) {
					return WDB::YES;
				} else if (get_selected() == WDB::NO)
					return WDB::NO;
			}
		}
		break;
	case WDT::MENU:
		break;
	case WDT::TIMED:
		if (_valid) {
			if (_system->input->check(CIN::ANYTHING, event)) {
				return WDB::OK;
				_valid = false;
			}
		}
		break;
	default:
		break;
	}

	return std::nullopt;
}

auto Sorcery::Dialog::toggle_highlighted() -> WDB {

	switch (_type) {
	case WDT::OK:
		// Can't be toggled
		break;
	case WDT::CONFIRM:
		if (_selected == WDB::YES)
			_selected = WDB::NO;
		else if (_selected == WDB::NO)
			_selected = WDB::YES;
		return _selected;
	case WDT::MENU:
		break;
	case WDT::TIMED:
		break;
	default:
		break;
	}

	return WDB::NO_BUTTON; // TODO optional
}

auto Sorcery::Dialog::check_for_mouse_move(const sf::Vector2f mouse_pos) -> std::optional<WDB> {

	const sf::Vector2f global_pos{this->getPosition()};
	const sf::Vector2f local_mouse_pos{mouse_pos - global_pos};

	switch (_type) {
	case WDT::OK:
		if (_buttons_fr.at(WDB::OK).contains(local_mouse_pos)) {
			_selected = WDB::OK;
			return WDB::OK;
		}
		break;
	case WDT::CONFIRM:
		if (_buttons_fr.at(WDB::YES).contains(local_mouse_pos)) {
			_selected = WDB::YES;
			return WDB::YES;
		} else if (_buttons_fr.at(WDB::NO).contains(local_mouse_pos)) {
			_selected = WDB::NO;
			return WDB::NO;
		}
		break;
	case WDT::MENU:
		break;
	case WDT::TIMED:
		break;
	default:
		break;
	}

	return std::nullopt;
}

// Only works for the Mouse
auto Sorcery::Dialog::check_if_option_selected(const sf::Vector2f mouse_pos) -> std::optional<WDB> {

	const sf::Vector2f global_pos{this->getPosition()};
	const sf::Vector2f local_mouse_pos{mouse_pos - global_pos};

	switch (_type) {
	case WDT::OK:
		if (_buttons_fr.at(WDB::OK).contains(local_mouse_pos)) {
			_selected = WDB::OK;
			return WDB::OK;
		}
		break;
	case WDT::CONFIRM:
		if (_buttons_fr.at(WDB::YES).contains(local_mouse_pos)) {
			_selected = WDB::YES;
			return WDB::YES;
		} else if (_buttons_fr.at(WDB::NO).contains(local_mouse_pos)) {
			_selected = WDB::NO;
			return WDB::NO;
		}
		break;
	case WDT::MENU:
		break;
	case WDT::TIMED:
		break;
	default:
		break;
	}

	return std::nullopt;
}

auto Sorcery::Dialog::set_selected(WDB value) -> void {

	_selected = value;
}

auto Sorcery::Dialog::get_selected() const -> WDB {

	return _selected;
}

auto Sorcery::Dialog::reset_timed() -> void {

	_valid = true;
	_start = std::nullopt;
	_current_time = std::nullopt;
}

auto Sorcery::Dialog::update() -> void {

	switch (_type) {
	case WDT::OK: {
		if (_selected == WDB::OK) {
			sf::RectangleShape &hl{_highlights.at(WDB::OK)};
			hl.setFillColor(_graphics->animation->selected_colour);
		}
	} break;
	case WDT::CONFIRM: {
		if (_selected == WDB::YES) {
			sf::RectangleShape &hl{_highlights.at(WDB::YES)};
			hl.setFillColor(_graphics->animation->selected_colour);
		} else if (_selected == WDB::NO) {
			sf::RectangleShape &hl{_highlights.at(WDB::NO)};
			hl.setFillColor(_graphics->animation->selected_colour);
		}
	} break;
	case WDT::MENU:
		break;
	case WDT::TIMED:

		// Handle timing
		if (_valid) {
			if (!_start)
				_start = std::chrono::system_clock::now();

			_current_time = std::chrono::system_clock::now();

			const auto time_elapsed{_current_time.value() - _start.value()};
			const auto time_elapsed_msec{std::chrono::duration_cast<std::chrono::milliseconds>(time_elapsed)};
			if (time_elapsed_msec.count() > _duration)
				_valid = false;
		}

		break;
	default:
		break;
	}
}

auto Sorcery::Dialog::set_duration(unsigned int duration) -> void {

	_duration = duration;
}

auto Sorcery::Dialog::set_valid(const bool valid) -> void {

	_valid = valid;
}

auto Sorcery::Dialog::get_valid() const -> bool {

	return _valid;
}

auto Sorcery::Dialog::draw(sf::RenderTarget &target, sf::RenderStates state) const -> void {

	if (_valid) {
		state.transform *= getTransform();
		target.draw(*_frame, state);
		for (auto text : _texts)
			target.draw(text, state);

		switch (_type) {
		case WDT::OK:
			if (_selected == WDB::OK) {
				target.draw(_highlights.at(WDB::OK), state);
				target.draw(_buttons_hl.at(WDB::OK), state);
			}
			break;
		case WDT::CONFIRM:
			if (_selected == WDB::YES) {
				target.draw(_buttons.at(WDB::NO), state);
				target.draw(_highlights.at(WDB::YES), state);
				target.draw(_buttons_hl.at(WDB::YES), state);
			} else if (_selected == WDB::NO) {
				target.draw(_buttons.at(WDB::YES), state);
				target.draw(_highlights.at(WDB::NO), state);
				target.draw(_buttons_hl.at(WDB::NO), state);
			}
			break;
		case WDT::MENU:
			break;
		case WDT::TIMED:
			break;
		default:
			break;
		}
	}
}
