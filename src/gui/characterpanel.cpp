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

#include "gui/characterpanel.hpp"
#include "core/display.hpp"
#include "core/graphics.hpp"
#include "core/system.hpp"
#include "core/window.hpp"
#include "gui/frame.hpp"
#include "resources/componentstore.hpp"
#include "resources/iconstore.hpp"
#include "resources/resourcemanager.hpp"
#include "resources/texturestore.hpp"
#include "types/character.hpp"

// Standard Constructor
Sorcery::CharacterPanel::CharacterPanel(System *system, Display *display, Graphics *graphics)
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

auto Sorcery::CharacterPanel::set(Character *character) -> void {

	_icons.clear();
	_texts.clear();
	_character = character;

	// Get the Portrait
	auto p_i{_character->get_portrait_index()};
	sf::Sprite portrait{_graphics->textures->get(p_i, GraphicsTextureType::PORTRAIT).value()};

	Component p_c{(*_display->layout)["character_panel:portrait"]};
	_display->window->set_pos(&p_c, &portrait);
	portrait.setScale(p_c.scl());
	_portrait = portrait;

	auto class_icon{_get_icon(CharacterStage::CHOOSE_CLASS).value()};
	_display->window->set_pos(&((*_display->layout)["character_panel:class_icon"]), &class_icon);
	class_icon.setScale((*_display->layout)["character_panel:class_icon"].scl());
	_icons.push_back(class_icon);

	auto race_icon{_get_icon(CharacterStage::CHOOSE_RACE).value()};
	_display->window->set_pos(&((*_display->layout)["character_panel:race_icon"]), &race_icon);
	race_icon.setScale((*_display->layout)["character_panel:race_icon"].scl());
	_icons.push_back(race_icon);

	auto alignment_icon{_get_icon(CharacterStage::CHOOSE_ALIGNMENT).value()};
	_display->window->set_pos(&((*_display->layout)["character_panel:alignment_icon"]), &alignment_icon);
	alignment_icon.setScale((*_display->layout)["character_panel:alignment_icon"].scl());
	_icons.push_back(alignment_icon);

	auto level_icon{(*_graphics->icons)["level"].value()};
	_display->window->set_pos(&((*_display->layout)["character_panel:level_icon"]), &level_icon);
	level_icon.setScale((*_display->layout)["character_panel:level_icon"].scl());
	_icons.push_back(level_icon);

	Component name_c{(*_display->layout)["character_panel:name_text"]};
	sf::Text name_text{};
	name_text.setFont(_system->resources->fonts[name_c.font]);
	name_text.setCharacterSize(name_c.size);
	name_text.setFillColor(sf::Color(name_c.colour));
	if (_display->get_bold())
		name_text.setStyle(sf::Text::Bold);

	auto name{_character->get_name()};
	if (_display->get_upper())
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
	if (_display->get_bold())
		level_text.setStyle(sf::Text::Bold);
	_display->window->set_pos(&level_c, &level_text);
	_texts.push_back(level_text);

	Component status_c{(*_display->layout)["character_panel:status_value"]};
	auto status{fmt::format("{}", _character->get_status_string())};
	sf::Text status_text{};
	status_text.setFont(_system->resources->fonts[status_c.font]);
	status_text.setCharacterSize(status_c.size);
	status_text.setFillColor(
		sf::Color(_graphics->adjust_status_colour(_character->get_status(), _character->is_poisoned())));
	if (_display->get_upper())
		std::transform(status.begin(), status.end(), status.begin(), ::toupper);
	status_text.setString(status);
	if (_display->get_bold())
		status_text.setStyle(sf::Text::Bold);
	_display->window->set_pos(&status_c, &status_text);
	_texts.push_back(status_text);

	Component hp_c{(*_display->layout)["character_panel:hp_value"]};
	auto hp{fmt::format("{}", _character->get_hp_summary())};
	sf::Text hp_text{};
	hp_text.setFont(_system->resources->fonts[hp_c.font]);
	hp_text.setCharacterSize(hp_c.size);
	hp_text.setFillColor(sf::Color(hp_c.colour));
	hp_text.setString(hp);
	if (_display->get_bold())
		hp_text.setStyle(sf::Text::Bold);
	_display->window->set_pos(&hp_c, &hp_text);
	_texts.push_back(hp_text);

	Component loc_c{(*_display->layout)["character_panel:location_value"]};
	auto loc{fmt::format("{:>13}", _character->get_loc_str())};
	sf::Text loc_text{};
	loc_text.setFont(_system->resources->fonts[loc_c.font]);
	loc_text.setCharacterSize(loc_c.size);
	loc_text.setFillColor(sf::Color(loc_c.colour));
	std::transform(loc.begin(), loc.end(), loc.begin(), ::toupper);
	loc_text.setString(loc);
	if (_display->get_bold())
		loc_text.setStyle(sf::Text::Bold);
	_display->window->set_pos(&loc_c, &loc_text);
	_texts.push_back(loc_text);

	valid = true;
}

auto Sorcery::CharacterPanel::_get_icon(CharacterStage type) -> std::optional<sf::Sprite> {

	using enum Enums::Character::Stage;

	switch (type) {
	case CHOOSE_ALIGNMENT: {
		auto alignment{_character->alignment_to_str(_character->get_alignment())};
		std::ranges::transform(alignment.begin(), alignment.end(), alignment.begin(), ::tolower);
		return (*_graphics->icons)[alignment].value();
	} break;
	case CHOOSE_RACE: {
		auto race{_character->race_to_str(_character->get_race())};
		std::ranges::transform(race.begin(), race.end(), race.begin(), ::tolower);
		return (*_graphics->icons)[race].value();
	} break;
	case CHOOSE_CLASS: {
		auto cclass{_character->class_to_str(_character->get_class())};
		std::ranges::transform(cclass.begin(), cclass.end(), cclass.begin(), ::tolower);
		return (*_graphics->icons)[cclass].value();
	} break;
	default:
		break;
	}

	return std::nullopt;
}

auto Sorcery::CharacterPanel::draw(sf::RenderTarget &target, sf::RenderStates states) const -> void {

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
