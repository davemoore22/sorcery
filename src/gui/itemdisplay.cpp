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

#include "gui/itemdisplay.hpp"
#include "common/enum.hpp"
#include "common/include.hpp"
#include "common/macro.hpp"
#include "core/display.hpp"
#include "core/game.hpp"
#include "core/graphics.hpp"
#include "core/system.hpp"
#include "core/window.hpp"
#include "resources/componentstore.hpp"
#include "resources/iconstore.hpp"
#include "resources/itemstore.hpp"
#include "resources/resourcemanager.hpp"
#include "types/enum.hpp"
#include "types/itemtype.hpp"

Sorcery::ItemDisplay::ItemDisplay(System *system, Display *display, Graphics *graphics, Game *game)
	: _system{system}, _display{display}, _graphics{graphics}, _game{game} {

	_layout = Component((*_display->layout)["global:item_display"]);
	_itemstore = _game->itemstore.get();
	valid = false;

	using enum Enums::Menu::Item;

	// Stored in different order to enum, argh
	_class_icons[0] = (*_graphics->icons)[CC_SAMURAI].value();
	_class_icons[1] = (*_graphics->icons)[CC_FIGHTER].value();
	_class_icons[2] = (*_graphics->icons)[CC_LORD].value();
	_class_icons[3] = (*_graphics->icons)[CC_THIEF].value();
	_class_icons[4] = (*_graphics->icons)[CC_NINJA].value();
	_class_icons[5] = (*_graphics->icons)[CC_PRIEST].value();
	_class_icons[6] = (*_graphics->icons)[CC_BISHOP].value();
	_class_icons[7] = (*_graphics->icons)[CC_MAGE].value();

	_align_icons[0] = (*_graphics->icons)[CA_GOOD].value();
	_align_icons[1] = (*_graphics->icons)[CA_NEUTRAL].value();
	_align_icons[2] = (*_graphics->icons)[CA_EVIL].value();

	auto icon_scaling{std::stof(_layout["icon_scale"].value())};

	{
		auto pos_x{std::stoi(_layout["icon_x_classes"].value())};
		auto pos_y{std::stoi(_layout["icon_y_classes"].value())};
		for (auto &icon : _class_icons) {
			icon.setScale(icon_scaling, icon_scaling);
			icon.setPosition(pos_x, pos_y);
			pos_x = pos_x + std::stoi(_layout["icon_offset_x"].value());
		}
	}
	{
		auto pos_x{std::stoi(_layout["icon_x_alignments"].value())};
		auto pos_y{std::stoi(_layout["icon_y_alignments"].value())};
		for (auto &icon : _align_icons) {
			icon.setScale(icon_scaling, icon_scaling);
			icon.setPosition(pos_x, pos_y);
			pos_x = pos_x + std::stoi(_layout["icon_offset_x"].value());
		}
	}
}

auto Sorcery::ItemDisplay::set(const unsigned int item_idx) -> void {

	if (_item.get()) {
		_item.release();
		_item.reset();
	}

	_item = std::make_unique<Item>((*_itemstore)[(magic_enum::enum_cast<ItemTypeID>(item_idx).value())]);
	const auto it{(*_itemstore)[_item->get_type_id()]};

	_display->generate("item_display", _sprites, _texts, _frames);

	using enum Enums::Character::Class;
	using enum Enums::Character::Align;

	for (auto &icon : _class_icons)
		icon.setColor(sf::Color(0xff1a1aff));
	for (auto &icon : _align_icons)
		icon.setColor(sf::Color(0xff1a1aff));

	// Stored in different order to enum, argh
	if (it.is_class_usable(FIGHTER))
		_class_icons[1].setColor(sf::Color(0x169016ff));
	if (it.is_class_usable(MAGE))
		_class_icons[7].setColor(sf::Color(0x169016ff));
	if (it.is_class_usable(PRIEST))
		_class_icons[5].setColor(sf::Color(0x169016ff));
	if (it.is_class_usable(THIEF))
		_class_icons[3].setColor(sf::Color(0x169016ff));
	if (it.is_class_usable(BISHOP))
		_class_icons[6].setColor(sf::Color(0x169016ff));
	if (it.is_class_usable(SAMURAI))
		_class_icons[0].setColor(sf::Color(0x169016ff));
	if (it.is_class_usable(LORD))
		_class_icons[2].setColor(sf::Color(0x169016ff));
	if (it.is_class_usable(NINJA))
		_class_icons[4].setColor(sf::Color(0x169016ff));

	if (it.is_align_usable(GOOD))
		_align_icons[0].setColor(sf::Color(0x169016ff));
	if (it.is_align_usable(NEUTRAL))
		_align_icons[1].setColor(sf::Color(0x169016ff));
	if (it.is_align_usable(EVIL))
		_align_icons[2].setColor(sf::Color(0x169016ff));

	valid = true;

	_add_text((*_display->layout)["item_display:name_label_item"], "{}", it.get_display_name());
	const std::string it_type{magic_enum::enum_name(it.get_category())};
	_add_text((*_display->layout)["item_display:type_label_item"], "{}", it_type);
	const std::string it_cursed{it.get_cursed() == true ? "Yes" : "No"};
	_add_text((*_display->layout)["item_display:cursed_label_item"], "{}", it_cursed);
	const std::string it_disc{it.get_discovered() == true ? "Yes" : "No"};
	_add_text((*_display->layout)["item_display:discovered_label_item"], "{}", it_disc);
	_add_text((*_display->layout)["item_display:value_label_item"], "{}", std::to_string(it.get_value()));
	_add_text((*_display->layout)["item_display:ac_mod_label_item"], "{}", std::to_string(it.get_ac_mod()));
	_add_text((*_display->layout)["item_display:attack_mod_label_item"], "{}", std::to_string(it.get_to_hit_mod()));
	_add_text((*_display->layout)["item_display:damage_label_item"], "{}", it.get_damage());
	_add_text((*_display->layout)["item_display:swings_label_item"], "{}", std::to_string(it.get_swings()));

	const std::string it_invoke{[&] {
		if (it.get_eff_inv() == ItemInv::NO_INV_EFFECT)
			return std::string{""};
		else {
			const std::string inv{magic_enum::enum_name(it.get_eff_inv())};
			auto str{fmt::format("{} ({}%)", inv, it.get_eff_inv_decay())};
			std::replace(str.begin(), str.end(), '_', ' ');
			return str;
		}
	}()};
	_add_text((*_display->layout)["item_display:invoke_label_item"], "{}", it_invoke);

	const std::string it_use{[&] {
		if (it.get_eff_use() == SpellID::NO_SPELL)
			return std::string{""};
		else {
			const std::string inv{magic_enum::enum_name(it.get_eff_use())};
			auto str{fmt::format("{} ({}%)", inv, it.get_eff_use_decay())};
			std::replace(str.begin(), str.end(), '_', ' ');
			return str;
		}
	}()};
	_add_text((*_display->layout)["item_display:use_label_item"], "{}", it_use);

	auto effects{it.get_effects()};
	if (effects.length() > 0) {
		TRIM(effects);

		std::vector<std::string> strings;
		const std::regex regex(R"([|]+)");
		std::sregex_token_iterator it{effects.begin(), effects.end(), regex, -1};
		std::vector<std::string> split{it, {}};
		split.erase(std::remove_if(split.begin(), split.end(), [](std::string const &s) { return s.size() == 0; }),
			split.end());
		strings = split;

		Component effects_c{(*_display->layout)["item_display:properties_label_item"]};
		auto x{effects_c.x};
		auto y{effects_c.y};
		for (const auto &each_string : strings) {
			auto text{_add_text((*_display->layout)["item_display:properties_label_item"], "{}", each_string)};
			text->setPosition(x, y);
			y += _display->window->get_ch();
		}
	}
}

auto Sorcery::ItemDisplay::_add_text(Component &component, std::string format, std::string value) -> sf::Text * {

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
	const auto offset_x{[&] {
		if (component["offset_x"])
			return std::stoi(component["offset_x"].value());
		else
			return 0;
	}()};
	const auto offset_y{[&] {
		if (component["offset_y"])
			return std::stoi(component["offset_y"].value());
		else
			return 0;
	}()};
	text.setPosition(component.x + offset_x, component.y + offset_y);

	// Generate a new key as this is a map, and we might call this with the same base component
	auto new_unique_key{GUID()};
	_texts.try_emplace(new_unique_key, text);
	return &_texts.at(new_unique_key);
}

auto Sorcery::ItemDisplay::draw(sf::RenderTarget &target, sf::RenderStates states) const -> void {

	if (valid) {

		states.transform *= getTransform();

		// Draw the standard components
		for (const auto &[unique_key, frame] : _frames)
			target.draw(*frame, states);

		for (const auto &[unique_key, sprite] : _sprites)
			target.draw(sprite, states);

		for (const auto &[unique_key, text] : _texts)
			target.draw(text, states);

		for (const auto &icon : _class_icons)
			target.draw(icon, states);

		for (const auto &icon : _align_icons)
			target.draw(icon, states);
	}
}
