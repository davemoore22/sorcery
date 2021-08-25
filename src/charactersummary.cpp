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
	System *system, Display *display, Graphics *graphics, Character *character)
	: _system{system}, _display{display}, _graphics{graphics}, _character{
																   character} {

	// Get the standard layout information
	_layout = Component((*_display->layout)["global:summary"]);

	// Not valid until we call the set command
	valid = false;

	_sprites.clear();
	_texts.clear();
	_width = 0;
	_height = 0;

	constexpr auto portrait_size{600};
	auto p_i{_character->get_portrait_index()};
	sf::Vector2u top_left{(p_i % 6) * portrait_size, (p_i / 6) * portrait_size};
	sf::IntRect rect{
		sf::IntRect(top_left.x, top_left.y, portrait_size, portrait_size)};

	// Grab the associated part of the texture and return it
	sf::Sprite portrait(
		_system->resources->textures[GraphicsTexture::PORTRAITS]);
	portrait.setTextureRect(rect);

	Component p_c{(*_display->layout)["character_row:portrait"]};
	_display->window->set_pos(&p_c, &portrait);
	portrait.setScale(p_c.scale, p_c.scale);
	_sprites.push_back(portrait);

	auto class_icon{_character->get_icon(CharacterStage::CHOOSE_CLASS).value()};
	_display->window->set_pos(
		&((*_display->layout)["character_row:class_icon"]), &class_icon);
	class_icon.setScale((*_display->layout)["character_row:class_icon"].scale,
		(*_display->layout)["character_row:class_icon"].scale);
	_sprites.push_back(class_icon);

	auto race_icon{_character->get_icon(CharacterStage::CHOOSE_RACE).value()};
	_display->window->set_pos(
		&((*_display->layout)["character_row:race_icon"]), &race_icon);
	race_icon.setScale((*_display->layout)["character_row:race_icon"].scale,
		(*_display->layout)["character_row:race_icon"].scale);
	_sprites.push_back(race_icon);

	auto alignment_icon{
		_character->get_icon(CharacterStage::CHOOSE_ALIGNMENT).value()};
	_display->window->set_pos(
		&((*_display->layout)["character_row:alignment_icon"]),
		&alignment_icon);
	alignment_icon.setScale(
		(*_display->layout)["character_row:alignment_icon"].scale,
		(*_display->layout)["character_row:alignment_icon"].scale);
	_sprites.push_back(alignment_icon);

	auto level_icon{(*_graphics->icons)["level"].value()};
	_display->window->set_pos(
		&((*_display->layout)["character_row:level_icon"]), &level_icon);
	level_icon.setScale((*_display->layout)["character_row:level_icon"].scale,
		(*_display->layout)["character_row:level_icon"].scale);
	_sprites.push_back(level_icon);

	Component name_c{(*_display->layout)["character_row:name_text"]};
	sf::Text name_text{};
	name_text.setFont(_system->resources->fonts[name_c.font]);
	name_text.setCharacterSize(name_c.size);
	name_text.setFillColor(sf::Color(name_c.colour));

	auto name{_character->get_name()};
	std::transform(name.begin(), name.end(), name.begin(), ::toupper);
	name_text.setString(name);
	_display->window->set_pos(&name_c, &name_text);
	_texts.push_back(name_text);

	Component level_c{(*_display->layout)["character_row:level_text"]};
	sf::Text level_text{};
	level_text.setFont(_system->resources->fonts[level_c.font]);
	level_text.setCharacterSize(level_c.size);
	level_text.setFillColor(sf::Color(level_c.colour));
	level_text.setString(std::to_string(_character->get_level()));
	_display->window->set_pos(&level_c, &level_text);
	_texts.push_back(level_text);

	valid = true;
}

auto Sorcery::CharacterSummary::draw(
	sf::RenderTarget &target, sf::RenderStates states) const -> void {

	states.transform *= getTransform();

	for (auto each_text : _texts) {
		target.draw(each_text, states);
	}
	for (auto each_sprite : _sprites)
		target.draw(each_sprite, states);
}
