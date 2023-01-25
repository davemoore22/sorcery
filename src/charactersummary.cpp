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
// be useful, but WITHOUT ANY WARRANTY; wiWthout even the implied warranty of
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

#include "charactersummary.hpp"

// Standard Constructor
Sorcery::CharacterSummary::CharacterSummary(
	System *system, Display *display, Graphics *graphics, Character *character, unsigned int num)
	: _system{system}, _display{display}, _graphics{graphics}, _character{character}, _num{num} {

	// Get the standard layout information
	_layout = Component((*_display->layout)["global:summary"]);
	_health = Component((*_display->layout)["character_row:stat_bar"]);

	// Not valid until we call the set command
	valid = false;

	_sprites.clear();
	_texts.clear();
	_width = 0;
	_height = 0;

	Component text{(*_display->layout)["character_row:text"]};
	sf::Text name_text{};
	name_text.setFont(_system->resources->fonts[text.font]);
	name_text.setCharacterSize(text.size);
	// name_text.setStyle(sf::Text::Bold);

	switch (character->get_status()) {
	case CharacterStatus::OK:
		if (character->get_poisoned_rate() > 0)
			name_text.setFillColor(sf::Color(std::stoull(text["colour_poisoned"].value(), 0, 16)));
		else if (character->get_max_hp() / character->get_current_hp() > 5)
			name_text.setFillColor(sf::Color(std::stoull(text["colour_low_health"].value(), 0, 16)));
		else
			name_text.setFillColor(sf::Color(std::stoull(text["colour_ok"].value(), 0, 16)));
		break;
	case CharacterStatus::AFRAID:
		[[fallthrough]];
	case CharacterStatus::SILENCED:
		[[fallthrough]];
	case CharacterStatus::ASLEEP:
		if (character->get_max_hp() / character->get_current_hp() > 5)
			name_text.setFillColor(sf::Color(std::stoull(text["colour_low_health"].value(), 0, 16)));
		else
			name_text.setFillColor(sf::Color(std::stoull(text["colour_ok"].value(), 0, 16)));
		break;
	case CharacterStatus::ASHES:
		name_text.setFillColor(sf::Color(std::stoull(text["colour_ashes"].value(), 0, 16)));
		break;
	case CharacterStatus::DEAD:
		name_text.setFillColor(sf::Color(std::stoull(text["colour_dead"].value(), 0, 16)));
		break;
	case CharacterStatus::HELD:
		name_text.setFillColor(sf::Color(std::stoull(text["colour_held"].value(), 0, 16)));
		break;
	case CharacterStatus::LOST:
		name_text.setFillColor(sf::Color(std::stoull(text["colour_lost"].value(), 0, 16)));
		break;
	case CharacterStatus::STONED:
		name_text.setFillColor(sf::Color(std::stoull(text["colour_stoned"].value(), 0, 16)));
		break;
	default:
		name_text.setFillColor(sf::Color(text.colour));
	}
	name_text.setString(character->get_sb_text(_num));
	_display->window->set_pos(&text, &name_text);
	_texts.push_back(name_text);
	_width = name_text.getLocalBounds().width;
	_height = name_text.getLocalBounds().height;
	_x = name_text.getLocalBounds().top;
	_y = name_text.getLocalBounds().left;

	valid = true;
}

auto Sorcery::CharacterSummary::get_local_bounds() const -> sf::FloatRect {

	return sf::FloatRect(_x, _y, _width, _height);
}

auto Sorcery::CharacterSummary::set_local_bounds(int x, int y) -> void {

	_x = x;
	_y = y;
}

auto Sorcery::CharacterSummary::get_global_bounds() const -> sf::FloatRect {

	// Works since we only have a text for now displayed
	return _texts.at(0).getGlobalBounds();
}

auto Sorcery::CharacterSummary::get_position() const -> unsigned int {

	// Position is 1-indexed, not 0-indexed
	return _num;
}

auto Sorcery::CharacterSummary::draw(sf::RenderTarget &target, sf::RenderStates states) const -> void {

	states.transform *= getTransform();

	for (auto text : _texts)
		target.draw(text, states);

	for (auto sprite : _sprites)
		target.draw(sprite, states);

	for (auto bar : _bars)
		target.draw(bar, states);
}
