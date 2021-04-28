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

#include "spellsummary.hpp"

Sorcery::SpellSummary::SpellSummary(
	System *system, Display *display, Graphics *graphics, Character *character)
	: _system{system}, _display{display}, _graphics{graphics}, _character{character} {

	// Get the standard layout information
	_layout = Component((*_display->layout)["global:spell_summary"]);

	// Get the Background Display Components
	_display->generate_components("spell_summary", sprites, texts, frames);

	// Not valid until we call the set command
	valid = false;
	_width = 0;
	_height = 0;
}

// Copy Constructors
Sorcery::SpellSummary::SpellSummary(const SpellSummary &other) {

	_system = other._system;
	_display = other._display;
	_graphics = other._graphics;
	_character = other._character; // Non-owning Pointer

	_layout = other._layout;
	_width = other._width;
	_height = other._height;

	valid = other.valid;
	sprites = other.sprites;
	texts = other.texts;
	frames = other.frames;

	_texts = other._texts;
	_icons = other._icons;
}

auto Sorcery::SpellSummary::operator=(const SpellSummary &other) -> SpellSummary & {

	_system = other._system;
	_display = other._display;
	_graphics = other._graphics;
	_character = other._character; // Non-owning Pointer

	_layout = other._layout;
	_width = other._width;
	_height = other._height;

	valid = other.valid;
	sprites = other.sprites;
	texts = other.texts;
	frames = other.frames;

	_texts = other._texts;
	_icons = other._icons;

	return *this;
}

// Move Constructors
Sorcery::SpellSummary::SpellSummary(SpellSummary &&other) noexcept {

	_system = other._system;
	_display = other._display;
	_graphics = other._graphics;
	_character = other._character; // Non-owning Pointer

	_layout = other._layout;
	_width = other._width;
	_height = other._height;

	valid = other.valid;
	sprites = std::move(other.sprites);
	texts = std::move(other.texts);
	frames = std::move(other.frames);

	_texts = std::move(other._texts);
	_icons = std::move(other._icons);

	other._system = nullptr;
	other._display = nullptr;
	other._graphics = nullptr;
	other._character = nullptr; // Non-owning Pointer

	other._layout = Component();
	other._width = 0;
	other._height = 0;

	other.valid = false;
	other.sprites.clear();
	other.texts.clear();
	other.frames.clear();

	other._texts.clear();
	other._icons.clear();
}

auto Sorcery::SpellSummary::operator=(SpellSummary &&other) noexcept -> SpellSummary & {

	if (this != &other) {
		_system = other._system;
		_display = other._display;
		_graphics = other._graphics;
		_character = other._character; // Non-owning Pointer

		_layout = other._layout;
		_width = other._width;
		_height = other._height;

		valid = other.valid;
		sprites = std::move(other.sprites);
		texts = std::move(other.texts);
		frames = std::move(other.frames);

		_texts = std::move(other._texts);
		_icons = std::move(other._icons);

		other._system = nullptr;
		other._display = nullptr;
		other._graphics = nullptr;
		other._character = nullptr; // Non-owning Pointer

		other._layout = Component();
		other._width = 0;
		other._height = 0;

		other.valid = false;
		other.sprites.clear();
		other.texts.clear();
		other.frames.clear();

		other._texts.clear();
		other._icons.clear();
	}
	return *this;
}

auto Sorcery::SpellSummary::set() -> void {

	_texts.clear();
	_icons.clear();

	// Set the Custom Components
	Component msl{(*_display->layout)["spell_summary:cs1_mage_spell_levels"]};
	Component psl{(*_display->layout)["spell_summary:cs1_priest_spell_levels"]};
	int x{msl.x};
	int y{msl.y};
	auto current_mage_sp{_character->get_spell_points(SpellType::MAGE, SpellPointStatus::CURRENT)};
	for (auto level = 1; level <= 7; level++) {
		auto sp_text{
			_add_text(msl, x, y, level, "{}", std::to_string(current_mage_sp.value().at(level)))};
		x += std::stoi(msl["spacing_x"].value());
		if (current_mage_sp.value().at(level) == 0) {
			auto zero_color{std::stoull(msl["zero_colour"].value(), 0, 16)};
			sp_text->setFillColor(sf::Color(zero_color));
		}
	}
	x = psl.x;
	y = psl.y;
	auto current_priest_sp{
		_character->get_spell_points(SpellType::PRIEST, SpellPointStatus::CURRENT)};
	for (auto level = 1; level <= 7; level++) {
		auto sp_text{
			_add_text(psl, x, y, level, "{}", std::to_string(current_priest_sp.value().at(level)))};
		x += std::stoi(psl["spacing_x"].value());
		if (current_priest_sp.value().at(level) == 0) {
			auto zero_color{std::stoull(psl["zero_colour"].value(), 0, 16)};
			sp_text->setFillColor(sf::Color(zero_color));
		}
	}

	valid = true;
}

auto Sorcery::SpellSummary::_add_text(Component &component, int x, int y, int index,
	std::string format, std::string value) -> sf::Text * {

	sf::Text text;
	std::string formatted_value{fmt::format(format, value)};
	text.setFont(_system->resources->fonts[component.font]);
	text.setCharacterSize(component.size);
	text.setFillColor(sf::Color(component.colour));
	text.setString(formatted_value);
	const int offset_x = [&] {
		if (component["offset_x"])
			return std::stoi(component["offset_x"].value());
		else
			return 0;
	}();
	const int offset_y = [&] {
		if (component["offset_y"])
			return std::stoi(component["offset_y"].value());
		else
			return 0;
	}();
	std::string key{fmt::format("{}_{}", component.unique_key, index)};
	text.setPosition(x + offset_x, y + offset_y);
	_texts.emplace_back(text);
	return &_texts.back();
}

auto Sorcery::SpellSummary::draw(sf::RenderTarget &target, sf::RenderStates states) const -> void {

	states.transform *= getTransform();

	// Draw the standard components
	for (const auto &[unique_key, frame] : frames)
		target.draw(*frame, states);

	for (const auto &[unique_key, sprite] : sprites)
		target.draw(sprite, states);

	for (const auto &[unique_key, text] : texts)
		target.draw(text, states);

	// Draw the custom components
	for (const auto &text : _texts)
		target.draw(text, states);

	for (const auto &icon : _icons)
		target.draw(icon, states);
}