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

#include "charpanel.hpp"

// Standard Constructor
Sorcery::CharPanel::CharPanel(System *system, Display *display, Graphics *graphics)
	: _system{system}, _display{display}, _graphics{graphics} {

	// Get the standard layout information
	_layout = Component((*_display->layout)["global:char_panel"]);

	_icons.clear();
	_texts.clear();

	// Not valid until we call the set command
	valid = false;
}

auto Sorcery::CharPanel::set(Character *character) -> void {

	_icons.clear();
	_texts.clear();
	_character = character;

	// Get the Portrait

	// Workout the location of the potrait on the texture, noting that the potraits are all
	// square and are 600x600 pixels in size arranged in a grid of 6 by 5
	constexpr int portrait_size{600};
	int p_i{_character->get_portrait_index()};
	sf::Vector2u top_left{(p_i % 6) * portrait_size, (p_i / 6) * portrait_size};
	sf::IntRect rect{sf::IntRect(top_left.x, top_left.y, portrait_size, portrait_size)};

	// Grab the associated part of the texture and return it
	sf::Sprite portrait(_system->resources->textures[GraphicsTexture::PORTRAITS]);
	portrait.setTextureRect(rect);

	Component p_c{(*_display->layout)["character_panel:portrait"]};
	const int p_offset_x = [&] {
		if (p_c["offset_x"])
			return std::stoi(p_c["offset_x"].value());
		else
			return 0;
	}();
	const int p_offset_y = [&] {
		if (p_c["offset_y"])
			return std::stoi(p_c["offset_y"].value());
		else
			return 0;
	}();
	portrait.setPosition(p_c.x + p_offset_x, p_c.y + p_offset_y);
	portrait.setScale(p_c.scale, p_c.scale);

	_portrait = portrait;

	auto class_icon = _character->get_icon(CharacterStage::CHOOSE_CLASS).value();
	class_icon.setPosition((*_display->layout)["character_panel:class_icon"].x,
		(*_display->layout)["character_panel:class_icon"].y);
	class_icon.setScale((*_display->layout)["character_panel:class_icon"].scale,
		(*_display->layout)["character_panel:class_icon"].scale);
	_icons.push_back(class_icon);

	auto race_icon = _character->get_icon(CharacterStage::CHOOSE_RACE).value();
	race_icon.setPosition((*_display->layout)["character_panel:race_icon"].x,
		(*_display->layout)["character_panel:race_icon"].y);
	race_icon.setScale((*_display->layout)["character_panel:race_icon"].scale,
		(*_display->layout)["character_panel:race_icon"].scale);
	_icons.push_back(race_icon);

	auto alignment_icon = _character->get_icon(CharacterStage::CHOOSE_ALIGNMENT).value();
	alignment_icon.setPosition((*_display->layout)["character_panel:alignment_icon"].x,
		(*_display->layout)["character_panel:alignment_icon"].y);
	alignment_icon.setScale((*_display->layout)["character_panel:alignment_icon"].scale,
		(*_display->layout)["character_panel:alignment_icon"].scale);
	_icons.push_back(alignment_icon);

	auto level_icon = (*_graphics->icons)["level"].value();
	level_icon.setPosition((*_display->layout)["character_panel:level_icon"].x,
		(*_display->layout)["character_panel:level_icon"].y);
	level_icon.setScale((*_display->layout)["character_panel:level_icon"].scale,
		(*_display->layout)["character_panel:level_icon"].scale);
	_icons.push_back(level_icon);

	Component name_c{(*_display->layout)["character_panel:name_text"]};
	sf::Text name_text;
	name_text.setFont(_system->resources->fonts[name_c.font]);
	name_text.setCharacterSize(name_c.size);
	name_text.setFillColor(sf::Color(name_c.colour));
	name_text.setString(_character->get_name());
	const int offset_x = [&] {
		if (name_c["offset_x"])
			return std::stoi(name_c["offset_x"].value());
		else
			return 0;
	}();
	const int offset_y = [&] {
		if (name_c["offset_y"])
			return std::stoi(name_c["offset_y"].value());
		else
			return 0;
	}();
	name_text.setPosition(name_c.x + offset_x, name_c.y + offset_y);
	_texts.push_back(name_text);

	Component level_c{(*_display->layout)["character_panel:level_text"]};
	sf::Text level_text;
	level_text.setFont(_system->resources->fonts[level_c.font]);
	level_text.setCharacterSize(level_c.size);
	level_text.setFillColor(sf::Color(level_c.colour));
	const int l_offset_x = [&] {
		if (level_c["offset_x"])
			return std::stoi(level_c["offset_x"].value());
		else
			return 0;
	}();
	const int l_offset_y = [&] {
		if (level_c["offset_y"])
			return std::stoi(level_c["offset_y"].value());
		else
			return 0;
	}();
	level_text.setPosition(level_c.x + l_offset_x, level_c.y + l_offset_y);
	_texts.push_back(level_text);

	valid = true;
}

auto Sorcery::CharPanel::draw(sf::RenderTarget &target, sf::RenderStates states) const -> void {

	states.transform *= getTransform();

	for (auto icon : _icons)
		target.draw(icon, states);
	for (auto text : _texts)
		target.draw(text, states);

	target.draw(_portrait, states);
}
