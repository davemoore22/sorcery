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

#include "gui/spellpanel.hpp"
#include "common/enum.hpp"
#include "common/include.hpp"
#include "common/macro.hpp"
#include "common/type.hpp"
#include "core/display.hpp"
#include "core/graphics.hpp"
#include "core/system.hpp"
#include "core/window.hpp"
#include "resources/componentstore.hpp"
#include "resources/iconstore.hpp"
#include "resources/resourcemanager.hpp"

// Standard Constructor
Sorcery::SpellPanel::SpellPanel(
	System *system, Display *display, Graphics *graphics)
	: _system{system}, _display{display}, _graphics{graphics} {

	// Not valid until we call the set command
	valid = false;
}

// Standard Destructor
Sorcery::SpellPanel::~SpellPanel() {
}

auto Sorcery::SpellPanel::set(Spell spell) -> void {

	_strings.clear();
	_texts.clear();
	_width = 0;
	_height = 0;

	// Get the standard layout information
	_layout = Component((*_display->layout)["global:spell_panel"]);

	Component icon_c{(*_display->layout)["spell_panel:icon"]};
	switch (spell.category) {
	case SPC::ATTACK:
		_icon = (*_graphics->icons)["attack"].value();
		break;
	case SPC::SUPPORT:
		_icon = (*_graphics->icons)["support"].value();
		break;
	case SPC::DISABLE:
		_icon = (*_graphics->icons)["disable"].value();
		break;
	case SPC::FIELD:
		_icon = (*_graphics->icons)["field"].value();
		break;
	case SPC::HEALING:
		_icon = (*_graphics->icons)["healing"].value();
		break;
	default:
		_icon = sf::Sprite();
		break;
	}

	_icon.setScale(icon_c.scl());
	_icon.setPosition(icon_c.pos());
	if (spell.known)
		_icon.setColor(
			sf::Color(std::stoull(icon_c["known_colour"].value(), 0, 16)));
	else
		_icon.setColor(
			sf::Color(std::stoull(icon_c["unknown_colour"].value(), 0, 16)));

	Component name_c{(*_display->layout)["spell_panel:name_text"]};
	auto translated_name{spell.translated_name};
	auto name{fmt::format("{} ({})", spell.name, translated_name)};
	sf::Text name_text{};
	name_text.setFont(_system->resources->fonts[name_c.font]);
	name_text.setCharacterSize(name_c.size);
	name_text.setFillColor(sf::Color(name_c.colour));
	name_text.setString(name);
	name_text.setPosition(name_c.pos());
	if (_display->get_bold())
		name_text.setStyle(sf::Text::Bold);
	_texts.push_back(name_text);

	auto spell_type{spell.type == SPT::MAGE ? "Mage" : "Priest"};
	std::string spell_category{magic_enum::enum_name<SPC>(spell.category)};
	std::transform(spell_category.begin(), spell_category.end(),
		spell_category.begin(), ::tolower);
	auto summary{fmt::format(
		"Level {} {} {} spell", spell.level, spell_type, spell_category)};
	Component summary_c{(*_display->layout)["spell_panel:summary_text"]};
	sf::Text summary_text{};
	summary_text.setFont(_system->resources->fonts[summary_c.font]);
	summary_text.setCharacterSize(summary_c.size);
	summary_text.setFillColor(sf::Color(summary_c.colour));
	summary_text.setString(summary);
	summary_text.setPosition(summary_c.pos());
	if (_display->get_bold())
		summary_text.setStyle(sf::Text::Bold);
	_texts.push_back(summary_text);

	// Wrap the display lines
	auto description{spell.details};
	auto chunk_size{_layout.w};
	auto wrapped_text{WORDWRAP(description, chunk_size)};

	// Split the display lines into a vector
	const std::regex regex(R"([@]+)");
	std::sregex_token_iterator it{
		wrapped_text.begin(), wrapped_text.end(), regex, -1};
	std::vector<std::string> split{it, {}};
	split.erase(std::remove_if(split.begin(), split.end(),
					[](std::string const &s) { return s.size() == 0; }),
		split.end());
	_strings = split;

	Component description_c{
		(*_display->layout)["spell_panel:description_text"]};
	auto x{description_c.x};
	auto y{description_c.y};
	for (const auto &each_string : _strings) {
		sf::Text text{};
		text.setFont(_system->resources->fonts[_layout.font]);
		text.setCharacterSize(_layout.size);
		text.setFillColor(sf::Color(_layout.colour));
		text.setString(each_string);
		text.setPosition(x, y);
		y += _display->window->get_ch();
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

auto Sorcery::SpellPanel::draw(
	sf::RenderTarget &target, sf::RenderStates states) const -> void {

	states.transform *= getTransform();
	for (auto each_text : _texts) {
		target.draw(each_text, states);
	}
	target.draw(_icon, states);
}
