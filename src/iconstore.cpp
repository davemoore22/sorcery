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

#include "iconstore.hpp"

// Standard Constructor
Sorcery::IconStore::IconStore(System &system, Display &display)
	: _system{system}, _display{display} {

	// First get the Icon Texture
	_texture = _system.resources->textures[ICONS_TEXTURE];

	// Set the scaling size (note we are using square icons here!) before loading!
	_size = sf::Vector2f{128, 128};
	float texture_size{static_cast<float>(_texture.getSize().y)};
	_scale = sf::Vector2f(_size.x / texture_size, _size.y / texture_size);

	// Set the Icons TODO: use a json file for the keys
	_loaded = _set_icons();
}

// Overload [] Operator
auto Sorcery::IconStore::operator[](const std::string &key) -> std::optional<sf::Sprite> {

	if (_loaded)
		return _icons.at(key);
	else
		return std::nullopt;
}

auto Sorcery::IconStore::get(const std::string &key) -> std::optional<sf::Sprite> {

	if (_loaded)
		return _icons.at(key);
	else
		return std::nullopt;
}

auto Sorcery::IconStore::_set_icons() -> bool {

	// Icon List load here
	_index = 0;
	_load_icon("samurai");
	_load_icon("fighter");
	_load_icon("lord");
	_load_icon("thief");
	_load_icon("ninja");
	_load_icon("priest");
	_load_icon("bishop");
	_load_icon("mage");
	_load_icon("good");
	_load_icon("neutral");
	_load_icon("evil");
	_load_icon("strength");
	_load_icon("iq");
	_load_icon("piety");
	_load_icon("vitality");
	_load_icon("agility");
	_load_icon("luck");
	_load_icon("human");
	_load_icon("elf");
	_load_icon("dwarf");
	_load_icon("gnome");
	_load_icon("hobbit");

	return true;
}

// Fortunately we are using square icons!
auto Sorcery::IconStore::_get_rect(unsigned int index) const -> sf::IntRect {

	return sf::IntRect(_texture.getSize().y * index, 0, _texture.getSize().y, _texture.getSize().y);
}

auto Sorcery::IconStore::_load_icon(std::string key) -> void {

	sf::Sprite sprite(_texture);
	sprite.setTextureRect(_get_rect(_index));
	sprite.setScale(_scale);
	_icons[key] = sprite;
	++_index;
}
