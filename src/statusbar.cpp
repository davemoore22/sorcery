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

#include "statusbar.hpp"

// Standard Constructor
Sorcery::StatusBar::StatusBar(System *system, Display *display, Graphics *graphics, Game *game,
	std::optional<Component> layout, std::optional<Component> frame)
	: _system{system}, _display{display}, _graphics{graphics}, _game{game} {

	_texts.clear();
	bounds.clear();

	// Get any Layout Information
	if (layout)
		_layout = layout.value();
	else
		_layout = Component((*_display->layout)["status_bar:status_bar"]);
	if (frame)
		_frame_c = frame.value();
	else
		_frame_c = Component((*_display->layout)["status_bar:outer_frame"]);

	_rtexture.create(_layout.w * _display->window->get_cw(), _layout.h * _display->window->get_ch());
	_rtexture.setSmooth(true);
	_rtexture.clear();

	// Create the Outside Frame
	_frame = std::make_unique<Frame>(
		_display->ui_texture, _frame_c.w, _frame_c.h, _frame_c.colour, _frame_c.background, _frame_c.alpha);

	// Render the background (inset by the frame)
	sf::RectangleShape rect(
		sf::Vector2f((_display->window->get_cw() * (_layout.w)) - 32, (_display->window->get_ch() * (_layout.h)) - 32));
	rect.setFillColor(sf::Color(0, 0, 0, _layout.alpha));
	rect.setPosition(16, 16);
	_rtexture.draw(rect);

	_fsprite = _frame->sprite;
	_fsprite.setPosition(0, 0);
	_rtexture.draw(_fsprite);

	_generate();
	_rtexture.display();
	_texture = _rtexture.getTexture();
	sprite = sf::Sprite(_texture);

	width = sprite.getLocalBounds().width;
	height = sprite.getLocalBounds().height;

	_summaries.clear();
}

auto Sorcery::StatusBar::refresh() -> void {

	const auto offset_x{[&] {
		if (_layout["offset_x"])
			return std::stoi(_layout["offset_x"].value());
		else
			return 0;
	}()};
	const auto offset_y{[&] {
		if (_layout["offset_y"])
			return std::stoi(_layout["offset_y"].value());
		else
			return 0;
	}()};
	auto x{std::stoi(_layout["summary_x"].value()) + offset_x};
	auto y{std::stoi(_layout["summary_y"].value()) + offset_y};
	auto summary_offset_x{std::stoi(_layout["summary_offset_x"].value())};
	auto summary_offset_y{std::stoi(_layout["summary_offset_y"].value())};

	Component text{(*_display->layout)["character_row:text"]};

	// Remember here position is 1-indexed, not 0-index
	_summaries.clear();
	bounds.clear();
	auto count{0};
	auto party{_game->state->get_party_characters()};
	for (auto _id : party) {
		auto character{_game->characters[_id]};
		auto summary{std::make_shared<CharacterSummary>(_system, _display, _graphics, &character, count + 1)};
		summary->setPosition(x + summary_offset_x, y + summary_offset_y);
		summary->set_local_bounds(x + summary_offset_x, y);
		y += _display->window->get_ch();
		bounds.emplace_back(summary->get_local_bounds());
		_summaries.emplace_back(std::move(summary));
		++count;
	}
}

auto Sorcery::StatusBar::set_selected_background() -> void {

	if (selected) {

		// Find the text that is highlighted (note we are 1-indexed here)
		auto it{std::find_if(_summaries.begin(), _summaries.end(),
			[&](auto &summary) { return summary->get_position() == selected.value(); })};
		if (it != _summaries.end()) {
			auto &summary{*it};

			_selected_bg = sf::RectangleShape(
				sf::Vector2f(summary->get_global_bounds().width, summary->get_global_bounds().height));
			_selected_bg.setFillColor(_graphics->animation->selected_colour);

			// Bounds is 0-indexed
			_selected_bg.setPosition(bounds.at(selected.value() - 1).left, bounds.at(selected.value() - 1).top);
		}
	}
}

auto Sorcery::StatusBar::set_mouse_selected(sf::Vector2f mouse_pos) -> std::optional<unsigned int> {
	if (bounds.size() > 0) {

		const sf::Vector2f global_pos{this->getPosition()};
		mouse_pos -= global_pos;
		auto it{std::find_if(
			bounds.begin(), bounds.end(), [&mouse_pos](const auto &item) { return item.contains(mouse_pos); })};
		if (it != bounds.end())
			return std::distance(bounds.begin(), it) + 1;
		else
			return std::nullopt;
	} else
		return std::nullopt;
}

auto Sorcery::StatusBar::draw(sf::RenderTarget &target, sf::RenderStates states) const -> void {
	states.transform *= getTransform();
	target.draw(sprite, states);

	if (selected)
		target.draw(_selected_bg, states);

	for (auto &[unique_key, text] : _texts)
		target.draw(text, states);
	for (auto &summary : _summaries)
		target.draw(*summary, states);
}

auto Sorcery::StatusBar::_generate() -> void {

	// For now - this is copied from display.cpp
	auto components{(*_display->layout)("status_bar")};
	if (components) {
		for (const auto &component : components.value()) {
			if (component.type == ComponentType::TEXT) {
				sf::Text text{};
				if (_display->get_bold())
					text.setStyle(sf::Text::Bold);
				text.setFont(_system->resources->fonts[component.font]);
				text.setCharacterSize(component.size);
				text.setFillColor(sf::Color(component.colour));
				text.setString((*_display->string)[component.string_key]);
				const auto x{component.x == -1 ? _display->window->centre.x : component.x};
				const auto y{component.y == -1 ? _display->window->centre.y : component.y};
				const auto offset_x{[&] {
					if (component["offset_x"])
						return std::stoi(component["offset_x"].value());
					else
						return 0;
				}()};
				const auto offset_y{[&] {
					if (component["offset_y"])
						return std::stoi(component["offset_y"].value());
					else
						return 0;
				}()};
				if (component.justification == Justification::CENTRE) {
					text.setPosition(x + offset_x, y + offset_y);
					text.setOrigin(text.getLocalBounds().width / 2.0f, 0);
				} else if (component.justification == Justification::RIGHT) {
					text.setPosition(x + offset_x, y + offset_y);
					const sf::FloatRect bounds{text.getLocalBounds()};
					text.setPosition(component.x - bounds.width, component.y);
				} else {
					text.setPosition(x + offset_x, y + offset_y);
					text.setOrigin(0, text.getLocalBounds().height / 2.0f);
				}

				_texts[component.unique_key] = text;
			}
		}
	}
}
