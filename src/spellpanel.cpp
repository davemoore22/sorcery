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

#include "spellpanel.hpp"

// Standard Constructor
Sorcery::SpellPanel::SpellPanel(System *system, Display *display, Graphics *graphics)
	: _system{system}, _display{display}, _graphics{graphics} {

	// Get the standard layout information
	_layout = Component((*_display->layout)["global:spell_panel"]);

	// Not valid until we call the set command
	valid = false;
}

auto Sorcery::SpellPanel::set(Spell spell) -> void {

	_strings.clear();
	_texts.clear();
	_width = 0;
	_height = 0;

	Component icon_c{(*_display->layout)["spell_panel:icon"]};
	switch (spell.category) {
	case SpellCategory::ATTACK:
		_icon = (*_graphics->icons)["attack"].value();
		break;
	case SpellCategory::SUPPORT:
		_icon = (*_graphics->icons)["support"].value();
		break;
	case SpellCategory::DISABLE:
		_icon = (*_graphics->icons)["disable"].value();
		break;
	case SpellCategory::FIELD:
		_icon = (*_graphics->icons)["field"].value();
		break;
	case SpellCategory::HEALING:
		_icon = (*_graphics->icons)["healing"].value();
		break;
	default:
		_icon = sf::Sprite();
		break;
	}

	_icon.setScale(icon_c.scale, icon_c.scale);
	_icon.setPosition(icon_c.x, icon_c.y);
	if (spell.known)
		_icon.setColor(sf::Color(std::stoull(icon_c["known_colour"].value(), 0, 16)));
	else
		_icon.setColor(sf::Color(std::stoull(icon_c["unknown_colour"].value(), 0, 16)));

	Component name_c{(*_display->layout)["spell_panel:name_text"]};
	std::string name{fmt::format("{} ({})", spell.name, spell.translated_name)};
	sf::Text name_text{};
	name_text.setFont(_system->resources->fonts[name_c.font]);
	name_text.setCharacterSize(name_c.size);
	name_text.setFillColor(sf::Color(name_c.colour));
	name_text.setString(name);
	name_text.setPosition(name_c.x, name_c.y);
	_texts.push_back(name_text);

	std::string spell_type{spell.type == SpellType::MAGE ? "MAGE" : "PRIEST"};
	std::string spell_category{magic_enum::enum_name<SpellCategory>(spell.category)};
	std::string summary{
		fmt::format("LEVEL {} {} {} SPELL", spell.level, spell_type, spell_category)};
	Component summary_c{(*_display->layout)["spell_panel:summary_text"]};
	sf::Text summary_text{};
	summary_text.setFont(_system->resources->fonts[summary_c.font]);
	summary_text.setCharacterSize(summary_c.size);
	summary_text.setFillColor(sf::Color(summary_c.colour));
	summary_text.setString(summary);
	summary_text.setPosition(summary_c.x, summary_c.y);
	_texts.push_back(summary_text);

	// Wrap the display lines
	std::string description{spell.details};
	unsigned int chunk_size{_layout.w};
	std::string wrapped_text{WORDWRAP(description, chunk_size)};

	// Split the display lines into a vector
	const std::regex regex(R"([@]+)");
	std::sregex_token_iterator it{wrapped_text.begin(), wrapped_text.end(), regex, -1};
	std::vector<std::string> split{it, {}};
	split.erase(std::remove_if(split.begin(), split.end(),
					[](std::string const &s) {
						return s.size() == 0;
					}),
		split.end());
	_strings = split;

	int x{164};
	int y{2};
	for (const auto &each_string : _strings) {
		sf::Text text{};
		text.setFont(_system->resources->fonts[_layout.font]);
		text.setCharacterSize(_layout.size);
		text.setFillColor(sf::Color(_layout.colour));
		text.setString(each_string);
		text.setPosition(x, 18 + y * 24);
		++y;
		_texts.push_back(text);
	}

	// Workout the size of the panel
	size_t max_length{0};
	for (const auto &each_string : _strings) {
		if (each_string.length() > max_length) {
			max_length = each_string.length();
		}
	}
	_width = max_length + 4;
	_height = _strings.size() + 4;

	// We're ok to draw it now
	valid = true;
}

auto Sorcery::SpellPanel::draw(sf::RenderTarget &target, sf::RenderStates states) const -> void {

	states.transform *= getTransform();
	for (auto each_text : _texts) {
		target.draw(each_text, states);
	}
	target.draw(_icon, states);
}
