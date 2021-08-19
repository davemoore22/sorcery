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

#include "charpanel.hpp"

// Standard Constructor
Sorcery::CharPanel::CharPanel(
	System *system, Display *display, Graphics *graphics)
	: _system{system}, _display{display}, _graphics{graphics} {

	// Get the standard layout information
	_layout = Component((*_display->layout)["global:char_panel"]);

	_icons.clear();
	_texts.clear();

	// Get the Background Display Components
	_display->generate("character_panel", sprites, texts, frames);

	// Not valid until we call the set command
	valid = false;
}

auto Sorcery::CharPanel::set(Character *character) -> void {

	_icons.clear();
	_texts.clear();
	_character = character;

	// Get the Portrait

	// Workout the location of the potrait on the texture, noting that the
	// potraits are all square and are 600x600 pixels in size arranged in a grid
	// of 6 by 5
	constexpr auto portrait_size{600};
	auto p_i{_character->get_portrait_index()};
	sf::Vector2u top_left{(p_i % 6) * portrait_size, (p_i / 6) * portrait_size};
	sf::IntRect rect{
		sf::IntRect(top_left.x, top_left.y, portrait_size, portrait_size)};

	// Grab the associated part of the texture and return it
	sf::Sprite portrait(
		_system->resources->textures[GraphicsTexture::PORTRAITS]);
	portrait.setTextureRect(rect);

	Component p_c{(*_display->layout)["character_panel:portrait"]};
	_display->window->set_pos(&p_c, &portrait);
	portrait.setScale(p_c.scale, p_c.scale);
	_portrait = portrait;

	auto class_icon{_character->get_icon(CharacterStage::CHOOSE_CLASS).value()};
	_display->window->set_pos(
		&((*_display->layout)["character_panel:class_icon"]), &class_icon);
	class_icon.setScale((*_display->layout)["character_panel:class_icon"].scale,
		(*_display->layout)["character_panel:class_icon"].scale);
	_icons.push_back(class_icon);

	auto race_icon{_character->get_icon(CharacterStage::CHOOSE_RACE).value()};
	_display->window->set_pos(
		&((*_display->layout)["character_panel:race_icon"]), &race_icon);
	race_icon.setScale((*_display->layout)["character_panel:race_icon"].scale,
		(*_display->layout)["character_panel:race_icon"].scale);
	_icons.push_back(race_icon);

	auto alignment_icon{
		_character->get_icon(CharacterStage::CHOOSE_ALIGNMENT).value()};
	_display->window->set_pos(
		&((*_display->layout)["character_panel:alignment_icon"]),
		&alignment_icon);
	alignment_icon.setScale(
		(*_display->layout)["character_panel:alignment_icon"].scale,
		(*_display->layout)["character_panel:alignment_icon"].scale);
	_icons.push_back(alignment_icon);

	auto level_icon{(*_graphics->icons)["level"].value()};
	_display->window->set_pos(
		&((*_display->layout)["character_panel:level_icon"]), &level_icon);
	level_icon.setScale((*_display->layout)["character_panel:level_icon"].scale,
		(*_display->layout)["character_panel:level_icon"].scale);
	_icons.push_back(level_icon);

	Component name_c{(*_display->layout)["character_panel:name_text"]};
	sf::Text name_text{};
	name_text.setFont(_system->resources->fonts[name_c.font]);
	name_text.setCharacterSize(name_c.size);
	name_text.setFillColor(sf::Color(name_c.colour));

	std::string name{_character->get_name()};
	std::transform(name.begin(), name.end(), name.begin(), ::toupper);
	name_text.setString(name);
	_display->window->set_pos(&name_c, &name_text);
	_texts.push_back(name_text);

	Component level_c{(*_display->layout)["character_panel:level_text"]};
	sf::Text level_text{};
	level_text.setFont(_system->resources->fonts[level_c.font]);
	level_text.setCharacterSize(level_c.size);
	level_text.setFillColor(sf::Color(level_c.colour));
	level_text.setString(std::to_string(_character->get_level()));
	_display->window->set_pos(&level_c, &level_text);
	_texts.push_back(level_text);

	Component status_c{(*_display->layout)["character_panel:status_value"]};
	std::string status{fmt::format("{}", _character->get_status_string())};
	sf::Text status_text{};
	status_text.setFont(_system->resources->fonts[status_c.font]);
	status_text.setCharacterSize(status_c.size);
	status_text.setFillColor(sf::Color(_graphics->adjust_status_colour(
		_character->get_status(), _character->is_poisoned())));
	status_text.setString(status);
	_display->window->set_pos(&status_c, &status_text);
	_texts.push_back(status_text);

	Component hp_c{(*_display->layout)["character_panel:hp_value"]};
	std::string hp{fmt::format("{}", _character->get_hp_summary())};
	sf::Text hp_text{};
	hp_text.setFont(_system->resources->fonts[hp_c.font]);
	hp_text.setCharacterSize(hp_c.size);
	hp_text.setFillColor(sf::Color(hp_c.colour));
	hp_text.setString(hp);
	_display->window->set_pos(&hp_c, &hp_text);
	_texts.push_back(hp_text);

	valid = true;
}

auto Sorcery::CharPanel::draw(
	sf::RenderTarget &target, sf::RenderStates states) const -> void {

	states.transform *= getTransform();

	// Draw the standard components
	for (const auto &[unique_key, frame] : frames)
		target.draw(*frame, states);

	for (const auto &[unique_key, sprite] : sprites)
		target.draw(sprite, states);

	for (const auto &[unique_key, text] : texts)
		target.draw(text, states);

	// Draw the custom components
	for (auto icon : _icons)
		target.draw(icon, states);
	for (auto text : _texts)
		target.draw(text, states);

	target.draw(_portrait, states);
}
