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

#include "gui/monsterdisplay.hpp"
#include "common/enum.hpp"
#include "common/include.hpp"
#include "common/macro.hpp"
#include "core/display.hpp"
#include "core/game.hpp"
#include "core/graphics.hpp"
#include "core/system.hpp"
#include "core/window.hpp"
#include "resources/componentstore.hpp"
#include "resources/monsterstore.hpp"
#include "resources/resourcemanager.hpp"
#include "types/dice.hpp"
#include "types/enum.hpp"
#include "types/monstertype.hpp"

Sorcery::MonsterDisplay::MonsterDisplay(System *system, Display *display, Graphics *graphics, Game *game)
	: _system{system}, _display{display}, _graphics{graphics}, _game{game} {

	_layout = Component((*_display->layout)["global:monster_display"]);
	_monsterstore = _game->monsterstore.get();
	valid = false;
}

auto Sorcery::MonsterDisplay::set(const unsigned int monster_idx) -> void {

	if (_monster.get()) {
		_monster.release();
		_monster.reset();
	}

	_monster = std::make_unique<Monster>((*_monsterstore)[(magic_enum::enum_cast<MonsterTypeID>(monster_idx).value())]);
	const auto mon{(*_monsterstore)[_monster->get_type_id()]};

	_display->generate("monster_display", _sprites, _texts, _frames);

	valid = true;

	const auto name{fmt::format("{} ({})", mon.get_known_name(), mon.get_unknown_name())};
	_add_text((*_display->layout)["monster_display:known_name"], "{}", name);

	const std::string mon_type{magic_enum::enum_name(mon.get_category())};
	_add_text((*_display->layout)["monster_display:type_label_value"], "{}", mon_type);
	const std::string mon_class{
		mon.get_class() != MonsterClass::NO_CLASS ? magic_enum::enum_name(mon.get_class()) : "N/A"};
	_add_text((*_display->layout)["monster_display:class_label_value"], "{}", mon_class);
	_add_text((*_display->layout)["monster_display:xp_label_value"], "{}", std::to_string(mon.get_xp()));
	_add_text((*_display->layout)["monster_display:ac_label_value"], "{}", std::to_string(mon.get_armour_class()));
	_add_text((*_display->layout)["monster_display:level_label_value"], "{}", std::to_string(mon.get_level()));
	_add_text((*_display->layout)["monster_display:group_size_label_value"], "{}", mon.get_group_size().str());
	_add_text((*_display->layout)["monster_display:hit_dice_label_value"], "{}", mon.get_hit_dice().str());
	_add_text((*_display->layout)["monster_display:sr_label_value"], "{}%", std::to_string(mon.get_spell_resistance()));
	_add_text((*_display->layout)["monster_display:hit_dice_label_value"], "{}", mon.get_hit_dice().str());
	_add_text((*_display->layout)["monster_display:attacks_label_value"], "{}", mon.get_attacks_str());

	auto traits{mon.get_traits()};
	if (traits.length() > 0) {
		TRIM(traits);

		std::vector<std::string> strings;
		const std::regex regex(R"([|]+)");
		std::sregex_token_iterator it{traits.begin(), traits.end(), regex, -1};
		std::vector<std::string> split{it, {}};
		split.erase(std::remove_if(split.begin(), split.end(), [](std::string const &s) { return s.size() == 0; }),
			split.end());
		strings = split;

		Component effects_c{(*_display->layout)["monster_display:traits_label_value"]};
		auto x{effects_c.x};
		auto y{effects_c.y};
		for (const auto &each_string : strings) {
			auto text{_add_text((*_display->layout)["monster_display:traits_label_value"], "{}", each_string)};
			text->setPosition(x, y);
			y += _display->window->get_ch();
		}
	}
}

auto Sorcery::MonsterDisplay::_add_text(Component &component, std::string format, std::string value) -> sf::Text * {

	sf::Text text{};

	// Note that Format v8 needs the format string wrapped in fmt::runtime - this isn't available in < v8 - see
	// https://github.com/fmtlib/fmt/issues/2438 - check FMT_VERSION version in fmt/core.h
	auto formatted_value{fmt::format(fmt::runtime(format), value)};
	if (_display->get_upper())
		std::transform(formatted_value.begin(), formatted_value.end(), formatted_value.begin(), ::toupper);
	text.setFont(_system->resources->fonts[component.font]);
	text.setCharacterSize(component.size);
	text.setFillColor(sf::Color(component.colour));

	text.setString(formatted_value);
	if (_display->get_bold())
		text.setStyle(sf::Text::Bold);
	const auto offset_x{std::invoke([&] {
		if (component["offset_x"])
			return std::stoi(component["offset_x"].value());
		else
			return 0;
	})};
	const auto offset_y{std::invoke([&] {
		if (component["offset_y"])
			return std::stoi(component["offset_y"].value());
		else
			return 0;
	})};
	text.setPosition(component.x + offset_x, component.y + offset_y);

	// Generate a new key as this is a map, and we might call this with the same base component
	auto new_unique_key{GUID()};
	_texts.try_emplace(new_unique_key, text);
	return &_texts.at(new_unique_key);
}

auto Sorcery::MonsterDisplay::draw(sf::RenderTarget &target, sf::RenderStates states) const -> void {

	if (valid) {

		states.transform *= getTransform();

		// Draw the standard components
		for (const auto &[unique_key, frame] : _frames)
			target.draw(*frame, states);

		for (const auto &[unique_key, sprite] : _sprites)
			target.draw(sprite, states);

		for (const auto &[unique_key, text] : _texts)
			target.draw(text, states);
	}
}