// Copyright (C) 2021 Dave Moore
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

#include "dialog.hpp"

// Standard Constructor
Sorcery::Dialog::Dialog(System *system, Display *display, Graphics *graphics, Component &frame_c, Component &string_c,
	WindowDialogType type)
	: _system{system}, _display{display}, _graphics{graphics}, _frame_c{frame_c}, _string_c{string_c}, _type{type} {

	_refresh(string_c);
}

auto Sorcery::Dialog::set(Component &string_c) -> void {

	_refresh(string_c);
}

auto Sorcery::Dialog::_refresh(Component &string_c) -> void {
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
	_duration = 1000; // ms
	_floor_count = 0;

	// Get the Window
	_window = _display->window->get_window();

	// Get the Layout
	_text_c = Component((*_display->layout)["dialog:text"]);
	_buttons_c = Component((*_display->layout)["dialog:buttons"]);

	// Get the Text
	auto text_width{_frame_c.w - 4};
	auto string{(*_display->string)[string_c.string_key]};
	auto wrapped_text{WORDWRAP(string, text_width)};

	// Split the Text into lines
	const std::regex regex(R"([@]+)");
	std::sregex_token_iterator it{wrapped_text.begin(), wrapped_text.end(), regex, -1};
	std::vector<std::string> split{it, {}};
	split.erase(std::remove_if(split.begin(), split.end(),
					[](std::string_view s) {
						return s.size() == 0;
					}),
		split.end());
	_strings = split;

	// Now work out the vertical size of the Frame
	auto frame_h{static_cast<unsigned int>(_strings.size())};
	switch (_type) {
	case WindowDialogType::OK:
		frame_h += 5;
		_selected = WindowDialogButton::OK;
		break;
	case WindowDialogType::CONFIRM:
		frame_h += 5;
		_selected = WindowDialogButton::NO;
		break;
	case WindowDialogType::MENU:
		// frame_h += menu_size;
		break;
	case WindowDialogType::TIMED:
		_selected = WindowDialogButton::NONE;
		frame_h += 2;
		break;
	case WindowDialogType::ELEVATOR:
		if (string_c["button_count"])
			frame_h += (5 + 2 + std::stoi(string_c["button_count"].value()));
		else
			frame_h += 5;
		_selected = WindowDialogButton::LEAVE;
		break;
	default:
		break;
	}

	// Add the standard components - first the frame
	_frame = std::make_unique<Frame>(_display->ui_texture, WindowFrameType::NORMAL, _frame_c.w, frame_h,
		_frame_c.colour, _frame_c.background, _frame_c.alpha);
	_frame->setPosition(0, 0);

	// Then the strings
	auto x{_display->window->get_cw() * 1};
	auto y{_display->window->get_ch() * 1};
	auto index{0};
	for (const auto &each_string : _strings) {
		sf::Text text{};
		text.setFont(_system->resources->fonts[_string_c.font]);
		text.setCharacterSize(_string_c.size);
		text.setFillColor(sf::Color(_string_c.colour));
		text.setString(each_string);

		if (_string_c.justification == Justification::CENTRE) {
			text.setPosition(
				(x + (_frame_c.w * _display->window->get_cw())) / 2, y + (index * _display->window->get_ch()));
			text.setOrigin(text.getLocalBounds().width / 2.0f, text.getLocalBounds().height / 2.0f);
		} else if (_string_c.justification == Justification::RIGHT) {
			text.setPosition(x, y + (index * _display->window->get_ch()));
			const sf::FloatRect bounds{text.getLocalBounds()};
			text.setPosition(_string_c.x - bounds.width, _string_c.y);
		} else {
			text.setPosition(x, y + (index * _display->window->get_ch()));
			text.setOrigin(0, text.getLocalBounds().height / 2.0f);
		}

		++index;
		_texts.push_back(text);
	}

	// Add the additional option texts/menu
	y = _display->window->get_ch() * (1 + static_cast<unsigned int>(_strings.size()) + 1);
	const auto centre_x{(_frame_c.w * _display->window->get_cw()) / 2};

	switch (_type) {
	case WindowDialogType::OK: {
		const auto ok_x{((centre_x - (_display->window->get_cw() * 2))) + (_display->window->get_cw() * 2)};
		sf::Text ok_text{};
		ok_text.setFont(_system->resources->fonts[_buttons_c.font]);
		ok_text.setCharacterSize(_buttons_c.size);
		ok_text.setFillColor(sf::Color(_buttons_c.colour));
		ok_text.setString((*_display->string)["DIALOG_OK"]);
		ok_text.setPosition(ok_x, y);
		_buttons[WindowDialogButton::OK] = ok_text;

		sf::Text ok_text_hl{ok_text};
		ok_text_hl.setOutlineColor(sf::Color(0, 0, 0));
		ok_text_hl.setOutlineThickness(2);
		_buttons_hl[WindowDialogButton::OK] = ok_text_hl;

		const sf::FloatRect ok_text_rect{ok_text_hl.getGlobalBounds()};
		_buttons_fr[WindowDialogButton::OK] = ok_text_rect;

		sf::RectangleShape ok_text_bg(sf::Vector2(ok_text_rect.width + 6, ok_text_rect.height + 8));
		ok_text_bg.setPosition(ok_x, y);
		ok_text_bg.setOrigin(0, 0 - ok_text_hl.getLocalBounds().height + 16);

		_highlights[WindowDialogButton::OK] = ok_text_bg;

	} break;
	case WindowDialogType::ELEVATOR: {
		const auto button_text{(*_display->string)["ELEVATOR_BUTTONS"]};
		const auto leave_text{(*_display->string)["ELEVATOR_LEAVE"]};
		const auto button_count{std::stoi(string_c["button_count"].value())};
		_floor_count = button_count;
		const auto top_floor{std::stoi(string_c["top_floor"].value())};
		const auto bottom_floor{std::stoi(string_c["bottom_floor"].value())};
		const auto y_seperator{std::stoi(string_c["floor_seperator_y"].value())};
		auto current_y{_display->window->get_ch() * (1 + static_cast<unsigned int>(_strings.size()) + 1)};

		// Hard code for now

		const auto a_x{(centre_x - (_display->window->get_cw() * 1))};
		sf::Text a_text{};
		a_text.setFont(_system->resources->fonts[_buttons_c.font]);
		a_text.setCharacterSize(_buttons_c.size);
		a_text.setFillColor(sf::Color(_buttons_c.colour));
		a_text.setString((*_display->string)["ELEVATOR_BUTTONS"].substr(0, 1));
		a_text.setPosition(a_x, current_y);
		_buttons[WindowDialogButton::A] = a_text;

		sf::Text a_text_hl{a_text};
		a_text_hl.setPosition(a_x, current_y);
		a_text_hl.setOutlineColor(sf::Color(0, 0, 0));
		a_text_hl.setOutlineThickness(2);
		_buttons_hl[WindowDialogButton::A] = a_text_hl;

		const sf::FloatRect a_text_rect{a_text_hl.getGlobalBounds()};
		_buttons_fr[WindowDialogButton::A] = a_text_rect;

		sf::RectangleShape a_text_bg(sf::Vector2(a_text_rect.width + 6, a_text_rect.height + 8));
		a_text_bg.setPosition(a_x, current_y);
		a_text_bg.setOrigin(0, 0 - a_text_hl.getLocalBounds().height + 16);

		_highlights[WindowDialogButton::A] = a_text_bg;

		current_y += (_display->window->get_ch() + y_seperator);

		const auto b_x{(centre_x - (_display->window->get_cw() * 1))};
		sf::Text b_text{};
		b_text.setFont(_system->resources->fonts[_buttons_c.font]);
		b_text.setCharacterSize(_buttons_c.size);
		b_text.setFillColor(sf::Color(_buttons_c.colour));
		b_text.setString((*_display->string)["ELEVATOR_BUTTONS"].substr(1, 1));
		b_text.setPosition(b_x, current_y);
		_buttons[WindowDialogButton::B] = b_text;

		sf::Text b_text_hl{b_text};
		b_text_hl.setPosition(b_x, current_y);
		b_text_hl.setOutlineColor(sf::Color(0, 0, 0));
		b_text_hl.setOutlineThickness(2);
		_buttons_hl[WindowDialogButton::B] = b_text_hl;

		const sf::FloatRect b_text_rect{b_text_hl.getGlobalBounds()};
		_buttons_fr[WindowDialogButton::B] = b_text_rect;

		sf::RectangleShape b_text_bg(sf::Vector2(b_text_rect.width + 6, b_text_rect.height + 8));
		b_text_bg.setPosition(b_x, current_y);
		b_text_bg.setOrigin(0, 0 - b_text_hl.getLocalBounds().height + 16);

		_highlights[WindowDialogButton::B] = b_text_bg;

		current_y += (_display->window->get_ch() + y_seperator);

		const auto c_x{(centre_x - (_display->window->get_cw() * 1))};
		sf::Text c_text{};
		c_text.setFont(_system->resources->fonts[_buttons_c.font]);
		c_text.setCharacterSize(_buttons_c.size);
		c_text.setFillColor(sf::Color(_buttons_c.colour));
		c_text.setString((*_display->string)["ELEVATOR_BUTTONS"].substr(2, 1));
		c_text.setPosition(c_x, current_y);
		_buttons[WindowDialogButton::C] = c_text;

		sf::Text c_text_hl{c_text};
		c_text_hl.setPosition(c_x, current_y);
		c_text_hl.setOutlineColor(sf::Color(0, 0, 0));
		c_text_hl.setOutlineThickness(2);
		_buttons_hl[WindowDialogButton::C] = c_text_hl;

		const sf::FloatRect c_text_rect{c_text_hl.getGlobalBounds()};
		_buttons_fr[WindowDialogButton::C] = c_text_rect;

		sf::RectangleShape c_text_bg(sf::Vector2(c_text_rect.width + 6, c_text_rect.height + 8));
		c_text_bg.setPosition(c_x, current_y);
		c_text_bg.setOrigin(0, 0 - c_text_hl.getLocalBounds().height + 16);

		_highlights[WindowDialogButton::C] = c_text_bg;

		current_y += (_display->window->get_ch() + y_seperator);

		const auto d_x{(centre_x - (_display->window->get_cw() * 1))};
		sf::Text d_text{};
		d_text.setFont(_system->resources->fonts[_buttons_c.font]);
		d_text.setCharacterSize(_buttons_c.size);
		d_text.setFillColor(sf::Color(_buttons_c.colour));
		d_text.setString((*_display->string)["ELEVATOR_BUTTONS"].substr(3, 1));
		d_text.setPosition(d_x, current_y);
		_buttons[WindowDialogButton::D] = d_text;

		sf::Text d_text_hl{d_text};
		d_text_hl.setPosition(d_x, current_y);
		d_text_hl.setOutlineColor(sf::Color(0, 0, 0));
		d_text_hl.setOutlineThickness(2);
		_buttons_hl[WindowDialogButton::D] = d_text_hl;

		const sf::FloatRect d_text_rect{d_text_hl.getGlobalBounds()};
		_buttons_fr[WindowDialogButton::D] = d_text_rect;

		sf::RectangleShape d_text_bg(sf::Vector2(d_text_rect.width + 6, d_text_rect.height + 8));
		d_text_bg.setPosition(d_x, current_y);
		d_text_bg.setOrigin(0, 0 - d_text_hl.getLocalBounds().height + 16);

		_highlights[WindowDialogButton::D] = d_text_bg;

		current_y += (_display->window->get_ch() + y_seperator);

		const auto l_x{(centre_x - (_display->window->get_cw() * 3))};
		sf::Text l_text{};
		l_text.setFont(_system->resources->fonts[_buttons_c.font]);
		l_text.setCharacterSize(_buttons_c.size);
		l_text.setFillColor(sf::Color(_buttons_c.colour));
		l_text.setString((*_display->string)["ELEVATOR_LEAVE"]);
		l_text.setPosition(l_x, current_y);
		_buttons[WindowDialogButton::LEAVE] = l_text;

		sf::Text l_text_hl{l_text};
		l_text_hl.setPosition(l_x, current_y);
		l_text_hl.setOutlineColor(sf::Color(0, 0, 0));
		l_text_hl.setOutlineThickness(2);
		_buttons_hl[WindowDialogButton::LEAVE] = l_text_hl;

		const sf::FloatRect l_text_rect{l_text_hl.getGlobalBounds()};
		_buttons_fr[WindowDialogButton::LEAVE] = l_text_rect;

		sf::RectangleShape l_text_bg(sf::Vector2(l_text_rect.width + 6, l_text_rect.height + 8));
		l_text_bg.setPosition(l_x, current_y);
		l_text_bg.setOrigin(0, 0 - l_text_hl.getLocalBounds().height + 16);

		_highlights[WindowDialogButton::LEAVE] = l_text_bg;

	} break;
	case WindowDialogType::CONFIRM: {
		const auto yes_x{(centre_x - (_display->window->get_cw() * 4))};
		sf::Text yes_text{};
		yes_text.setFont(_system->resources->fonts[_buttons_c.font]);
		yes_text.setCharacterSize(_buttons_c.size);
		yes_text.setFillColor(sf::Color(_buttons_c.colour));
		yes_text.setString((*_display->string)["DIALOG_YES"]);
		yes_text.setPosition(yes_x, y);
		_buttons[WindowDialogButton::YES] = yes_text;

		sf::Text yes_text_hl{yes_text};
		yes_text_hl.setPosition(yes_x, y);
		yes_text_hl.setOutlineColor(sf::Color(0, 0, 0));
		yes_text_hl.setOutlineThickness(2);
		_buttons_hl[WindowDialogButton::YES] = yes_text_hl;

		const sf::FloatRect yes_text_rect{yes_text_hl.getGlobalBounds()};
		_buttons_fr[WindowDialogButton::YES] = yes_text_rect;

		sf::RectangleShape yes_text_bg(sf::Vector2(yes_text_rect.width + 6, yes_text_rect.height + 8));
		yes_text_bg.setPosition(yes_x, y);
		yes_text_bg.setOrigin(0, 0 - yes_text_hl.getLocalBounds().height + 16);

		_highlights[WindowDialogButton::YES] = yes_text_bg;

		const auto no_x{centre_x + (_display->window->get_cw() * 2)};
		sf::Text no_text{};
		no_text.setFont(_system->resources->fonts[_buttons_c.font]);
		no_text.setCharacterSize(_buttons_c.size);
		no_text.setFillColor(sf::Color(_buttons_c.colour));
		no_text.setString((*_display->string)["DIALOG_NO"]);
		no_text.setPosition(no_x, y);
		_buttons[WindowDialogButton::NO] = no_text;

		sf::Text no_text_hl{no_text};
		no_text_hl.setPosition(no_x, y);
		no_text_hl.setOutlineColor(sf::Color(0, 0, 0));
		no_text_hl.setOutlineThickness(2);
		_buttons_hl[WindowDialogButton::NO] = no_text_hl;

		const sf::FloatRect no_text_rect{no_text_hl.getGlobalBounds()};
		_buttons_fr[WindowDialogButton::NO] = no_text_rect;

		sf::RectangleShape no_text_bg(sf::Vector2(no_text_rect.width + 6, no_text_rect.height + 8));
		no_text_bg.setPosition(no_x, y);
		no_text_bg.setOrigin(0, 0 - no_text_hl.getLocalBounds().height + 16);
		_highlights[WindowDialogButton::NO] = no_text_bg;

	} break;
	case WindowDialogType::MENU:
		break;
	case WindowDialogType::TIMED:
		break;
	default:
		break;
	}
}

auto Sorcery::Dialog::handle_input(sf::Event event) -> std::optional<WindowDialogButton> {

	if (event.type == sf::Event::Closed)
		return WindowDialogButton::CLOSE;

	if (_system->input->check(WindowInput::SHOW_CONTROLS, event)) {
		_display->show_overlay();
		return std::nullopt;
	} else
		_display->hide_overlay();

	switch (_type) {
	case WindowDialogType::OK:

		if (_system->input->check(WindowInput::MOVE, event))
			check_for_mouse_move(static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
		else if (_system->input->check(WindowInput::CONFIRM, event)) {
			std::optional<WindowDialogButton> button_chosen{
				check_if_option_selected(static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)))};
			if (button_chosen) {
				if (button_chosen.value() == WindowDialogButton::OK)
					return WindowDialogButton::OK;
			} else {

				if (get_selected() == WindowDialogButton::OK) {
					return WindowDialogButton::OK;
				}
			}
		}

		break;
	case WindowDialogType::CONFIRM:
		if (_system->input->check(WindowInput::LEFT, event))
			toggle_highlighted();
		else if (_system->input->check(WindowInput::RIGHT, event))
			toggle_highlighted();
		else if (_system->input->check(WindowInput::YES, event))
			set_selected(WindowDialogButton::YES);
		else if (_system->input->check(WindowInput::NO, event))
			set_selected(WindowDialogButton::NO);
		else if (_system->input->check(WindowInput::CANCEL, event))
			return WindowDialogButton::NO;
		else if (_system->input->check(WindowInput::BACK, event))
			return WindowDialogButton::NO;
		else if (_system->input->check(WindowInput::MOVE, event))
			check_for_mouse_move(static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
		else if (_system->input->check(WindowInput::CONFIRM, event)) {
			std::optional<WindowDialogButton> button_chosen{
				check_if_option_selected(static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)))};

			// Mouse click only
			if (button_chosen) {
				if (button_chosen.value() == WindowDialogButton::YES)
					return WindowDialogButton::YES;
				else if (button_chosen.value() == WindowDialogButton::NO)
					return WindowDialogButton::NO;

			} else {

				// Button/Keyboard
				if (get_selected() == WindowDialogButton::YES) {
					return WindowDialogButton::YES;
				} else if (get_selected() == WindowDialogButton::NO)
					return WindowDialogButton::NO;
			}
		}
		break;
	case WindowDialogType::MENU:
		break;
	case WindowDialogType::ELEVATOR:
		if (_system->input->check(WindowInput::MOVE, event))
			check_for_mouse_move(static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
		else if (_system->input->check(WindowInput::CONFIRM, event)) {
			std::optional<WindowDialogButton> button_chosen{
				check_if_option_selected(static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)))};
			if (button_chosen) {
				if (button_chosen.value() == WindowDialogButton::A)
					return WindowDialogButton::A;
				else if (button_chosen.value() == WindowDialogButton::B)
					return WindowDialogButton::B;
				else if (button_chosen.value() == WindowDialogButton::C)
					return WindowDialogButton::C;
				else if (button_chosen.value() == WindowDialogButton::D)
					return WindowDialogButton::D;
				else if (button_chosen.value() == WindowDialogButton::E)
					return WindowDialogButton::E;
				else if (button_chosen.value() == WindowDialogButton::F)
					return WindowDialogButton::F;
				else if (button_chosen.value() == WindowDialogButton::LEAVE)
					return WindowDialogButton::LEAVE;
			} else {
				if (get_selected() == WindowDialogButton::LEAVE) {
					return WindowDialogButton::LEAVE;
				}
			}
		} else if (_system->input->check(WindowInput::LEFT, event))
			toggle_highlighted();
		else if (_system->input->check(WindowInput::RIGHT, event))
			toggle_highlighted();
		else if (_system->input->check(WindowInput::YES, event))
			set_selected(WindowDialogButton::YES);
		else if (_system->input->check(WindowInput::NO, event))
			set_selected(WindowDialogButton::NO);
		else if (_system->input->check(WindowInput::CANCEL, event))
			return WindowDialogButton::NO;
		else if (_system->input->check(WindowInput::BACK, event))
			return WindowDialogButton::NO;
		else if (_system->input->check(WindowInput::MOVE, event))
			check_for_mouse_move(static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
		else if (_system->input->check(WindowInput::CONFIRM, event)) {
			std::optional<WindowDialogButton> button_chosen{
				check_if_option_selected(static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)))};

			// Mouse click only
			if (button_chosen) {
				if (button_chosen.value() == WindowDialogButton::A)
					return WindowDialogButton::A;
				else if (button_chosen.value() == WindowDialogButton::B)
					return WindowDialogButton::B;
				else if (button_chosen.value() == WindowDialogButton::C)
					return WindowDialogButton::C;
				else if (button_chosen.value() == WindowDialogButton::D)
					return WindowDialogButton::D;
				else if (button_chosen.value() == WindowDialogButton::E)
					return WindowDialogButton::E;
				else if (button_chosen.value() == WindowDialogButton::F)
					return WindowDialogButton::F;
				else if (button_chosen.value() == WindowDialogButton::LEAVE)
					return WindowDialogButton::LEAVE;

			} else {

				// Button/Keyboard
				if (get_selected() == WindowDialogButton::A) {
					return WindowDialogButton::A;
				} else if (get_selected() == WindowDialogButton::B)
					return WindowDialogButton::B;
				else if (get_selected() == WindowDialogButton::C)
					return WindowDialogButton::C;
				else if (get_selected() == WindowDialogButton::D)
					return WindowDialogButton::D;
				else if (get_selected() == WindowDialogButton::E)
					return WindowDialogButton::E;
				else if (get_selected() == WindowDialogButton::F)
					return WindowDialogButton::F;
				else if (get_selected() == WindowDialogButton::LEAVE)
					return WindowDialogButton::LEAVE;
			}
		}

		break;
	case WindowDialogType::TIMED:
		if (_valid) {
			if (_system->input->check(WindowInput::ANYTHING, event)) {
				return WindowDialogButton::OK;
				_valid = false;
			}
		}
		break;
	default:
		break;
	}

	return std::nullopt;
}

auto Sorcery::Dialog::toggle_next() -> WindowDialogButton {

	if (_type == WindowDialogType::ELEVATOR) {
		if (_selected == WindowDialogButton::A)
			_selected = WindowDialogButton::B;
		else if (_selected == WindowDialogButton::B)
			_selected = WindowDialogButton::C;
		else if (_selected == WindowDialogButton::C)
			_selected = WindowDialogButton::D;
		else if ((_selected == WindowDialogButton::D) && (_floor_count == 4))
			_selected = WindowDialogButton::LEAVE;
		else if (_selected == WindowDialogButton::D)
			_selected = WindowDialogButton::E;
		else if (_selected == WindowDialogButton::E)
			_selected = WindowDialogButton::F;
		else if (_selected == WindowDialogButton::F)
			_selected = WindowDialogButton::LEAVE;
		else if (_selected == WindowDialogButton::LEAVE)
			_selected = WindowDialogButton::A;
	}
}
auto Sorcery::Dialog::toggle_previous() -> WindowDialogButton {

	if (_type == WindowDialogType::ELEVATOR) {
		if (_selected == WindowDialogButton::A)
			_selected = WindowDialogButton::LEAVE;
		else if (_selected == WindowDialogButton::B)
			_selected = WindowDialogButton::A;
		else if (_selected == WindowDialogButton::C)
			_selected = WindowDialogButton::B;
		else if (_selected == WindowDialogButton::D)
			_selected = WindowDialogButton::C;
		else if (_selected == WindowDialogButton::E)
			_selected = WindowDialogButton::D;
		else if (_selected == WindowDialogButton::F)
			_selected = WindowDialogButton::E;
		else if ((_selected == WindowDialogButton::LEAVE) && (_floor_count == 4))
			_selected = WindowDialogButton::D;
		else if (_selected == WindowDialogButton::LEAVE)
			_selected = WindowDialogButton::F;
	}
}

auto Sorcery::Dialog::toggle_highlighted() -> WindowDialogButton {

	switch (_type) {
	case WindowDialogType::OK:
		// Can't be toggled
		break;
	case WindowDialogType::CONFIRM:
		if (_selected == WindowDialogButton::YES)
			_selected = WindowDialogButton::NO;
		else if (_selected == WindowDialogButton::NO)
			_selected = WindowDialogButton::YES;
		return _selected;
	case WindowDialogType::MENU:
		break;
	case WindowDialogType::TIMED:
		break;
	default:
		break;
	}

	return WindowDialogButton::NONE; // TODO optional
}

auto Sorcery::Dialog::check_for_mouse_move(const sf::Vector2f mouse_pos) -> std::optional<WindowDialogButton> {

	const sf::Vector2f global_pos{this->getPosition()};
	const sf::Vector2f local_mouse_pos{mouse_pos - global_pos};

	switch (_type) {
	case WindowDialogType::OK:
		if (_buttons_fr.at(WindowDialogButton::OK).contains(local_mouse_pos)) {
			_selected = WindowDialogButton::OK;
			return WindowDialogButton::OK;
		}
		break;
	case WindowDialogType::CONFIRM:
		if (_buttons_fr.at(WindowDialogButton::YES).contains(local_mouse_pos)) {
			_selected = WindowDialogButton::YES;
			return WindowDialogButton::YES;
		} else if (_buttons_fr.at(WindowDialogButton::NO).contains(local_mouse_pos)) {
			_selected = WindowDialogButton::NO;
			return WindowDialogButton::NO;
		}
		break;
	case WindowDialogType::ELEVATOR:
		if (_floor_count == 4) {
			if (_buttons_fr.at(WindowDialogButton::A).contains(local_mouse_pos)) {
				_selected = WindowDialogButton::A;
				return WindowDialogButton::A;
			} else if (_buttons_fr.at(WindowDialogButton::B).contains(local_mouse_pos)) {
				_selected = WindowDialogButton::B;
				return WindowDialogButton::B;
			} else if (_buttons_fr.at(WindowDialogButton::C).contains(local_mouse_pos)) {
				_selected = WindowDialogButton::C;
				return WindowDialogButton::C;
			} else if (_buttons_fr.at(WindowDialogButton::D).contains(local_mouse_pos)) {
				_selected = WindowDialogButton::D;
				return WindowDialogButton::D;
			} /* else if (_buttons_fr.at(WindowDialogButton::E).contains(local_mouse_pos)) {
				_selected = WindowDialogButton::E;
				return WindowDialogButton::E;
			} else if (_buttons_fr.at(WindowDialogButton::F).contains(local_mouse_pos)) {
				_selected = WindowDialogButton::F;
				return WindowDialogButton::F;
			} */
			else if (_buttons_fr.at(WindowDialogButton::LEAVE).contains(local_mouse_pos)) {
				_selected = WindowDialogButton::LEAVE;
				return WindowDialogButton::LEAVE;
			}
		}
	case WindowDialogType::MENU:
		break;
	case WindowDialogType::TIMED:
		break;
	default:
		break;
	}

	return std::nullopt;
}

// Only works for the Mouse
auto Sorcery::Dialog::check_if_option_selected(const sf::Vector2f mouse_pos) -> std::optional<WindowDialogButton> {

	const sf::Vector2f global_pos{this->getPosition()};
	const sf::Vector2f local_mouse_pos{mouse_pos - global_pos};

	switch (_type) {
	case WindowDialogType::OK:
		if (_buttons_fr.at(WindowDialogButton::OK).contains(local_mouse_pos)) {
			_selected = WindowDialogButton::OK;
			return WindowDialogButton::OK;
		}
		break;
	case WindowDialogType::CONFIRM:
		if (_buttons_fr.at(WindowDialogButton::YES).contains(local_mouse_pos)) {
			_selected = WindowDialogButton::YES;
			return WindowDialogButton::YES;
		} else if (_buttons_fr.at(WindowDialogButton::NO).contains(local_mouse_pos)) {
			_selected = WindowDialogButton::NO;
			return WindowDialogButton::NO;
		}
		break;
	case WindowDialogType::ELEVATOR:
		if (_buttons_fr.at(WindowDialogButton::A).contains(local_mouse_pos)) {
			_selected = WindowDialogButton::A;
			return WindowDialogButton::A;
		} else if (_buttons_fr.at(WindowDialogButton::B).contains(local_mouse_pos)) {
			_selected = WindowDialogButton::B;
			return WindowDialogButton::B;
		} else if (_buttons_fr.at(WindowDialogButton::C).contains(local_mouse_pos)) {
			_selected = WindowDialogButton::C;
			return WindowDialogButton::C;
		} else if (_buttons_fr.at(WindowDialogButton::D).contains(local_mouse_pos)) {
			_selected = WindowDialogButton::D;
			return WindowDialogButton::D;
		} /*  else if (_buttons_fr.at(WindowDialogButton::E).contains(local_mouse_pos)) {
			_selected = WindowDialogButton::E;
			return WindowDialogButton::E;
		} else if (_buttons_fr.at(WindowDialogButton::F).contains(local_mouse_pos)) {
			_selected = WindowDialogButton::F;
			return WindowDialogButton::F;
		} */
		else if (_buttons_fr.at(WindowDialogButton::LEAVE).contains(local_mouse_pos)) {
			_selected = WindowDialogButton::LEAVE;
			return WindowDialogButton::LEAVE;
		}
		break;
	case WindowDialogType::MENU:
		break;
	case WindowDialogType::TIMED:
		break;
	default:
		break;
	}

	return std::nullopt;
}

auto Sorcery::Dialog::set_selected(WindowDialogButton value) -> void {

	_selected = value;
}

auto Sorcery::Dialog::get_selected() const -> WindowDialogButton {

	return _selected;
}

auto Sorcery::Dialog::reset_timed() -> void {

	_valid = true;
	_start = std::nullopt;
	_current_time = std::nullopt;
}

auto Sorcery::Dialog::update() -> void {

	switch (_type) {
	case WindowDialogType::OK: {
		if (_selected == WindowDialogButton::OK) {
			sf::RectangleShape &hl{_highlights.at(WindowDialogButton::OK)};
			hl.setFillColor(_graphics->animation->selected_colour);
		}
	} break;
	case WindowDialogType::CONFIRM: {
		if (_selected == WindowDialogButton::YES) {
			sf::RectangleShape &hl{_highlights.at(WindowDialogButton::YES)};
			hl.setFillColor(_graphics->animation->selected_colour);
		} else if (_selected == WindowDialogButton::NO) {
			sf::RectangleShape &hl{_highlights.at(WindowDialogButton::NO)};
			hl.setFillColor(_graphics->animation->selected_colour);
		}
	} break;
	case WindowDialogType::ELEVATOR: {
		if (_selected == WindowDialogButton::A) {
			sf::RectangleShape &hl{_highlights.at(WindowDialogButton::A)};
			hl.setFillColor(_graphics->animation->selected_colour);
		} else if (_selected == WindowDialogButton::B) {
			sf::RectangleShape &hl{_highlights.at(WindowDialogButton::B)};
			hl.setFillColor(_graphics->animation->selected_colour);
		} else if (_selected == WindowDialogButton::C) {
			sf::RectangleShape &hl{_highlights.at(WindowDialogButton::C)};
			hl.setFillColor(_graphics->animation->selected_colour);
		} else if (_selected == WindowDialogButton::D) {
			sf::RectangleShape &hl{_highlights.at(WindowDialogButton::D)};
			hl.setFillColor(_graphics->animation->selected_colour);
		} else if (_selected == WindowDialogButton::E) {
			sf::RectangleShape &hl{_highlights.at(WindowDialogButton::E)};
			hl.setFillColor(_graphics->animation->selected_colour);
		} else if (_selected == WindowDialogButton::F) {
			sf::RectangleShape &hl{_highlights.at(WindowDialogButton::F)};
			hl.setFillColor(_graphics->animation->selected_colour);
		} else if (_selected == WindowDialogButton::LEAVE) {
			sf::RectangleShape &hl{_highlights.at(WindowDialogButton::LEAVE)};
			hl.setFillColor(_graphics->animation->selected_colour);
		}

	} break;
	case WindowDialogType::MENU:
		break;
	case WindowDialogType::TIMED:

		// Handle timing
		if (_valid) {
			if (!_start)
				_start = std::chrono::system_clock::now();

			_current_time = std::chrono::system_clock::now();

			const auto time_elapsed{_current_time.value() - _start.value()};
			const auto time_elapsed_sec{std::chrono::duration_cast<std::chrono::milliseconds>(time_elapsed)};
			if (time_elapsed_sec.count() > _duration)
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
		case WindowDialogType::OK:
			if (_selected == WindowDialogButton::OK) {
				target.draw(_highlights.at(WindowDialogButton::OK), state);
				target.draw(_buttons_hl.at(WindowDialogButton::OK), state);
			}
			break;
		case WindowDialogType::CONFIRM:
			if (_selected == WindowDialogButton::YES) {
				target.draw(_buttons.at(WindowDialogButton::NO), state);
				target.draw(_highlights.at(WindowDialogButton::YES), state);
				target.draw(_buttons_hl.at(WindowDialogButton::YES), state);
			} else if (_selected == WindowDialogButton::NO) {
				target.draw(_buttons.at(WindowDialogButton::YES), state);
				target.draw(_highlights.at(WindowDialogButton::NO), state);
				target.draw(_buttons_hl.at(WindowDialogButton::NO), state);
			}
			break;
		case WindowDialogType::ELEVATOR:
			switch (_selected) {
			case WindowDialogButton::A:
				target.draw(_highlights.at(WindowDialogButton::A), state);
				target.draw(_buttons_hl.at(WindowDialogButton::A), state);
				target.draw(_buttons.at(WindowDialogButton::B), state);
				target.draw(_buttons.at(WindowDialogButton::C), state);
				target.draw(_buttons.at(WindowDialogButton::D), state);
				if (_floor_count > 4) {
				}
				target.draw(_buttons.at(WindowDialogButton::LEAVE), state);
				break;
			case WindowDialogButton::B:
				target.draw(_buttons.at(WindowDialogButton::A), state);
				target.draw(_buttons.at(WindowDialogButton::LEAVE), state);
				target.draw(_buttons.at(WindowDialogButton::C), state);
				target.draw(_buttons.at(WindowDialogButton::D), state);
				target.draw(_highlights.at(WindowDialogButton::B), state);
				target.draw(_buttons_hl.at(WindowDialogButton::B), state);
				break;
			case WindowDialogButton::C:
				target.draw(_buttons.at(WindowDialogButton::A), state);
				target.draw(_buttons.at(WindowDialogButton::LEAVE), state);
				target.draw(_buttons.at(WindowDialogButton::B), state);
				target.draw(_buttons.at(WindowDialogButton::D), state);
				target.draw(_highlights.at(WindowDialogButton::C), state);
				target.draw(_buttons_hl.at(WindowDialogButton::C), state);
				break;
			case WindowDialogButton::D:
				target.draw(_buttons.at(WindowDialogButton::A), state);
				target.draw(_buttons.at(WindowDialogButton::LEAVE), state);
				target.draw(_buttons.at(WindowDialogButton::B), state);
				target.draw(_buttons.at(WindowDialogButton::C), state);
				target.draw(_highlights.at(WindowDialogButton::D), state);
				target.draw(_buttons_hl.at(WindowDialogButton::D), state);
				break;
			case WindowDialogButton::E:
				break;
			case WindowDialogButton::F:
				break;
			case WindowDialogButton::LEAVE:
				target.draw(_buttons.at(WindowDialogButton::A), state);
				target.draw(_buttons.at(WindowDialogButton::B), state);
				target.draw(_buttons.at(WindowDialogButton::C), state);
				target.draw(_buttons.at(WindowDialogButton::D), state);
				target.draw(_highlights.at(WindowDialogButton::LEAVE), state);
				target.draw(_buttons_hl.at(WindowDialogButton::LEAVE), state);
				break;
			default:
				break;
			}

			break;
		case WindowDialogType::MENU:
			break;
		case WindowDialogType::TIMED:
			break;
		default:
			break;
		}
	}
}
