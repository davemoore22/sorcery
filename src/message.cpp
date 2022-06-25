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

#include "message.hpp"

// Standard Constructor
Sorcery::Message::Message(System *system, Display *display, Graphics *graphics, Component &frame_c, Component &text_c)
	: _system{system}, _display{display}, _graphics{graphics}, _frame_c{frame_c}, _text_c{text_c} {}

auto Sorcery::Message::update(TileNote tile_note) -> void {

	_tile_note = tile_note;

	_strings.clear();
	_texts.clear();
	_sprites.clear();

	auto text_width{_frame_c.w - 4};
	std::vector<std::string> output;

	// Get the Window
	_window = _display->window->get_window();

	// Get the text to display
	const auto metadata_pos{_tile_note.text.find(" METADATA")};
	auto stripped_text{_tile_note.text};
	if (metadata_pos != std::string::npos)
		stripped_text.erase(metadata_pos);

	const std::regex regex(R"([@]+)");

	// Split into seperate lines if we have any delimiters (the @ character)
	std::sregex_token_iterator it{stripped_text.begin(), stripped_text.end(), regex, -1};
	std::vector<std::string> split{it, {}};
	split.erase(std::remove_if(split.begin(), split.end(),
					[](std::string_view s) {
						return s.size() == 0;
					}),
		split.end());

	if (split.size() > 1) {

		for (auto &text : split) {

			auto wrapped_text{WORDWRAP(text, text_width)};
			text = wrapped_text;

			std::sregex_token_iterator it_sub{text.begin(), text.end(), regex, -1};

			std::vector<std::string> sub_split{it_sub, {}};
			sub_split.erase(std::remove_if(sub_split.begin(), sub_split.end(),
								[](std::string_view s) {
									return s.size() == 0;
								}),
				sub_split.end());

			for (auto &sub_text : sub_split) {
				std::transform(sub_text.begin(), sub_text.end(), sub_text.begin(), ::toupper);
				output.emplace_back(sub_text);
			}
		}
	} else {
		auto wrapped_text{WORDWRAP(split[0], text_width)};

		std::sregex_token_iterator it_sub{wrapped_text.begin(), wrapped_text.end(), regex, -1};

		std::vector<std::string> sub_split{it_sub, {}};
		sub_split.erase(std::remove_if(sub_split.begin(), sub_split.end(),
							[](std::string_view s) {
								return s.size() == 0;
							}),
			sub_split.end());

		for (auto &sub_text : sub_split) {
			std::transform(sub_text.begin(), sub_text.end(), sub_text.begin(), ::toupper);
			output.emplace_back(sub_text);
		}
	}

	_strings = output;

	// Now work out the vertical size of the Frame
	auto frame_h{static_cast<unsigned int>(_strings.size())};
	frame_h += 2;

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
		text.setFont(_system->resources->fonts[_text_c.font]);
		text.setCharacterSize(_text_c.size);
		text.setFillColor(sf::Color(_text_c.colour));
		text.setString(each_string);

		if (_text_c.justification == Justification::CENTRE) {
			text.setPosition(
				(x + (_frame_c.w * _display->window->get_cw())) / 2, y + (index * _display->window->get_ch()));
			text.setOrigin(text.getLocalBounds().width / 2.0f, text.getLocalBounds().height / 2.0f);
		} else if (_text_c.justification == Justification::RIGHT) {
			text.setPosition(x, y + (index * _display->window->get_ch()));
			const sf::FloatRect bounds{text.getLocalBounds()};
			text.setPosition(_text_c.x - bounds.width, _text_c.y);
		} else {
			text.setPosition(x, y + (index * _display->window->get_ch()));
			text.setOrigin(0, text.getLocalBounds().height / 2.0f);
		}

		++index;
		_texts.push_back(text);
	}
}

auto Sorcery::Message::draw(sf::RenderTarget &target, sf::RenderStates state) const -> void {

	state.transform *= getTransform();
	target.draw(*_frame, state);
	for (const auto &text : _texts)
		target.draw(text, state);
}
