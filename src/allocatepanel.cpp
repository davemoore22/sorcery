// Copyright (C) 2021 Dave Moore
//
// This file is part of Sorcery: Dreams of the Mad Overlord.
//
// Sorcery: Dreams of the Mad Overlord is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Sorcery: Dreams of the Mad Overlord is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Sorcery: Dreams of the Mad Overlord.  If not, see <http://www.gnu.org/licenses/>.
//
// If you modify this Program, or any covered work, by linking or combining it
// with the libraries referred to in README (or a modified version of said
// libraries), containing parts covered by the terms of said libraries, the
// licensors of this Program grant you additional permission to convey the
// resulting work.

#include "allocatepanel.hpp"

Sorcery::AllocatePanel::AllocatePanel(
	System &system, Display &display, Graphics &graphics, Character *character)
	: _system{system}, _display{display}, _graphics{graphics}, _character{character} {

	// Get the standard layout information
	_layout = Component((*_display.layout)["global:allocate_panel"]);

	// Get the standard components
	_display.generate_components("allocate_panel", sprites, texts, frames);

	// Not valid until we call the set command
	valid = false;
	_width = 0;
	_height = 0;
}

auto Sorcery::AllocatePanel::set() -> void {

	_stats.clear();
	_bars.clear();

	int x{18};
	int y{0};

	for (const auto &[attribute, value] : _character->get_current_attributes()) {
		sf::Text text;
		text.setFont(_system.resources->fonts[_layout.font]);
		text.setCharacterSize(_layout.size);
		text.setFillColor(sf::Color(_layout.colour));
		text.setString(std::to_string(value));
		text.setOrigin(0, text.getLocalBounds().height / 2.0f);
		text.setPosition(x, y * _display.window->get_cell_height());
		++y;
		_stats.push_back(text);
	}

	valid = true;
}

auto Sorcery::AllocatePanel::draw(sf::RenderTarget &target, sf::RenderStates states) const -> void {

	states.transform *= getTransform();
	for (const auto &text : _stats) {
		target.draw(text, states);
	}
	for (const auto &bar : _bars) {
		target.draw(bar, states);
	}
}
