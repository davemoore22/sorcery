
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

#include "gui/characterdisplay.hpp"
#include "common/enum.hpp"
#include "common/macro.hpp"
#include "common/type.hpp"
#include "core/animation.hpp"
#include "core/display.hpp"
#include "core/graphics.hpp"
#include "core/system.hpp"
#include "core/window.hpp"
#include "gui/frame.hpp"
#include "gui/macro.hpp"
#include "gui/spellpanel.hpp"
#include "resources/componentstore.hpp"
#include "resources/iconstore.hpp"
#include "resources/resourcemanager.hpp"
#include "resources/stringstore.hpp"
#include "resources/texturestore.hpp"
#include "types/character.hpp"

// Standard Constructor
Sorcery::CharacterDisplay::CharacterDisplay(System *system, Display *display, Graphics *graphics)
	: _system{system}, _display{display}, _graphics{graphics} {

	_texts.clear();
	_sprites.clear();
	_frames.clear();
	_v_texts.clear();
	_v_sprites.clear();
	_v_frames.clear();
	_view = CHV::NO_VIEW;
	_hl_mage_spell = SPI::DUMAPIC;
	_hl_priest_spell = SPI::BADIOS;
	_hl_action_item = MIM::NO_MENU_ITEM;
	_hl_inventory_item = 0;
	mage_spell_bounds.clear();
	priest_spell_bounds.clear();
	mage_spell_texts.clear();
	priest_spell_texts.clear();
	action_menu_texts.clear();
	action_menu_bounds.clear();
	inventory_texts.clear();
	inventory_bounds.clear();

	_spell_panel = std::make_shared<SpellPanel>(_system, _display, _graphics);
	_spell_panel->setPosition((*_display->layout)["global:spell_panel"].pos());

	_mode = CHM::NO_MODE;
	_character = nullptr;

	_window = _display->window->get_window();
}

auto Sorcery::CharacterDisplay::set(Character *character) -> void {

	_character = character;
	_generate_display();
}

auto Sorcery::CharacterDisplay::_generate_inventory(Component layout_c) -> void {

	auto slot{1u};
	auto x{std::stoi(layout_c["offset_x"].value())};
	const auto start_x{x};
	auto y{std::stoi(layout_c["offset_y"].value())};
	const auto row_y{std::stoi(layout_c["row_y"].value())};
	const auto column_x{std::stoi(layout_c["column_x"].value())};

	for (const auto &item : _character->inventory.items()) {
		const std::string flag{std::invoke([&] {
			if (!item.get_known())
				return "?";
			else if (item.get_cursed() && item.get_equipped())
				return "-";
			else if (item.get_equipped())
				return "*";
			else if (!item.get_usable())
				return "#";
			else
				return " ";
		})};

		auto line{fmt::format("{}){}{}", slot, flag, item.get_display_name())};

		if (slot % 2 == 1) {
			x = start_x;
			y += row_y;
		} else {
			x = start_x + column_x;
		}

		auto text{_add_text(layout_c, "{}", line)};

		text->setFont(_system->resources->fonts[layout_c.font]);
		text->setCharacterSize(layout_c.size);

		text->setString(line);
		if (_display->get_bold())
			text->setStyle(sf::Text::Bold);
		text->setPosition(x, y);

		auto text_hl_bounds{text->getGlobalBounds()};
		inventory_bounds[slot] = text_hl_bounds;

		if (_hl_inventory_item == slot) {
			sf::RectangleShape bg(sf::Vector2f(line.length() * _display->window->get_cw(), text_hl_bounds.height));
			bg.setPosition(text_hl_bounds.left, text_hl_bounds.top);
			bg.setFillColor(_graphics->animation->select_col);
			text->setFillColor(sf::Color(layout_c.colour));
			text->setOutlineColor(sf::Color(0, 0, 0));
			text->setOutlineThickness(2);
			_hl_inventory_item_bg = bg;
		} else
			text->setFillColor(sf::Color(layout_c.colour));

		++slot;
	}
}

auto Sorcery::CharacterDisplay::clear_inventory_item() -> void {

	_hl_inventory_item = 0;
}

auto Sorcery::CharacterDisplay::set_mode(CHM value) -> void {

	_mode = value;
}

auto Sorcery::CharacterDisplay::inc_hl_spell(SPT type) -> void {

	if (type == SPT::MAGE) {
		auto index{+_hl_mage_spell};
		if (index < unenum(SPI::TILTOWAIT)) {
			++index;
			_hl_mage_spell = magic_enum::enum_cast<SPI>(index).value();
		}
	} else {
		auto index{+_hl_priest_spell};
		if (index < unenum(SPI::MALIKTO)) {
			++index;
			_hl_priest_spell = magic_enum::enum_cast<SPI>(index).value();
		}
	}

	_generate_display();
}

auto Sorcery::CharacterDisplay::dec_hl_spell(SPT type) -> void {

	if (type == SPT::MAGE) {
		auto index{unenum(_hl_mage_spell)};
		if (index > (unenum(SPI::DUMAPIC))) {
			--index;
			_hl_mage_spell = magic_enum::enum_cast<SPI>(index).value();
		}
	} else {
		auto index{unenum(_hl_priest_spell)};
		if (index > (unenum(SPI::BADIOS))) {
			--index;
			_hl_priest_spell = magic_enum::enum_cast<SPI>(index).value();
		}
	}

	_generate_display();
}

// Setting the view will regenerate the display components
auto Sorcery::CharacterDisplay::left_view() -> void {

	auto view_index{unenum(_view)};
	if (view_index == unenum(CHV::SUMMARY))
		view_index = unenum(CHV::PRIEST_SPELLS);
	else
		--view_index;
	_view = magic_enum::enum_cast<CHV>(view_index).value();

	_display->layout->refresh_if_needed();

	_generate_display();
}

// Setting the view will regenerate the display components
auto Sorcery::CharacterDisplay::right_view() -> void {

	auto view_index{unenum(_view)};
	if (view_index == unenum(CHV::PRIEST_SPELLS))
		view_index = unenum(CHV::SUMMARY);
	else
		++view_index;
	_view = magic_enum::enum_cast<CHV>(view_index).value();

	_display->layout->refresh_if_needed();

	_generate_display();
}

auto Sorcery::CharacterDisplay::_get_character_portrait() -> sf::Sprite {

	sf::Sprite portrait{_graphics->textures->get(_character->get_portrait_index(), GTT::PORTRAIT).value()};

	return portrait;
}

auto Sorcery::CharacterDisplay::get_icon(CHS type) -> std::optional<sf::Sprite> {

	switch (type) {
	case CHS::CHOOSE_ALIGNMENT: {
		auto alignment{_character->alignment_to_str(_character->get_alignment())};
		std::ranges::transform(alignment.begin(), alignment.end(), alignment.begin(), ::tolower);
		return (*_graphics->icons)[alignment].value();
	} break;
	case CHS::CHOOSE_RACE: {
		auto race{_character->race_to_str(_character->get_race())};
		std::ranges::transform(race.begin(), race.end(), race.begin(), ::tolower);
		return (*_graphics->icons)[race].value();
	} break;
	case CHS::CHOOSE_CLASS: {
		auto cclass{_character->class_to_str(_character->get_class())};
		std::ranges::transform(cclass.begin(), cclass.end(), cclass.begin(), ::tolower);
		return (*_graphics->icons)[cclass].value();
	} break;
	default:
		break;
	}

	return std::nullopt;
}

auto Sorcery::CharacterDisplay::_generate_summary_icons() -> void {

	auto class_icon{get_icon(CHS::CHOOSE_CLASS).value()};
	class_icon.setPosition((*_display->layout)["character:class_icon"].pos());
	class_icon.setScale((*_display->layout)["character:class_icon"].scl());
	_v_sprites.try_emplace((*_display->layout)["character:class_icon"].unique_key, class_icon);

	auto race_icon{get_icon(CHS::CHOOSE_RACE).value()};
	race_icon.setPosition((*_display->layout)["character:race_icon"].pos());
	race_icon.setScale((*_display->layout)["character:race_icon"].scl());
	_v_sprites.try_emplace((*_display->layout)["character:race_icon"].unique_key, race_icon);

	auto alignment_icon{get_icon(CHS::CHOOSE_ALIGNMENT).value()};
	alignment_icon.setPosition((*_display->layout)["character:alignment_icon"].pos());
	alignment_icon.setScale((*_display->layout)["character:alignment_icon"].scl());
	_v_sprites.try_emplace((*_display->layout)["character:alignment_icon"].unique_key, alignment_icon);

	auto level_icon{(*_graphics->icons)["level"].value()};
	level_icon.setPosition((*_display->layout)["character:level_icon"].pos());
	level_icon.setScale((*_display->layout)["character:level_icon"].scl());
	_v_sprites.try_emplace((*_display->layout)["character:level_icon"].unique_key, level_icon);

	_add_text((*_display->layout)["character:level_text"], "{}",
		std::to_string(_character->abilities().at(CAB::CURRENT_LEVEL)), true);
}

auto Sorcery::CharacterDisplay::generate_display() -> void {

	_generate_display();
}

auto Sorcery::CharacterDisplay::_add_icon(Component &component, std::string icon_key) -> void {

	auto icon{(*_graphics->icons)[icon_key].value()};
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
	icon.setPosition(component.x + offset_x, component.y + offset_y);
	icon.setScale(component.scl());
	_v_sprites.try_emplace(component.unique_key, icon);
}

auto Sorcery::CharacterDisplay::_add_text(Component &component, std::string format, std::string value, bool is_view)
	-> sf::Text * {

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
	if (is_view) {
		_v_texts.try_emplace(new_unique_key, text);
		return &_v_texts.at(new_unique_key);
	} else {
		_texts.try_emplace(new_unique_key, text);
		return &_texts.at(new_unique_key);
	}
}

auto Sorcery::CharacterDisplay::get_view() const -> CHV {

	return _view;
}

auto Sorcery::CharacterDisplay::_get_spell_icon(SPC category) -> std::optional<sf::Sprite> {

	switch (category) {
	case SPC::ATTACK:
		return (*_graphics->icons)["attack"].value();
		break;
	case SPC::SUPPORT:
		return (*_graphics->icons)["support"].value();
		break;
	case SPC::DISABLE:
		return (*_graphics->icons)["disable"].value();
		break;
	case SPC::FIELD:
		return (*_graphics->icons)["field"].value();
		break;
	case SPC::HEALING:
		return (*_graphics->icons)["healing"].value();
		break;
	default:
		return std::nullopt;
		break;
	}
}

// Setting the view will regenerate the display components
auto Sorcery::CharacterDisplay::set_view(const CHV value) -> void {

	_view = value;
	_hl_action_item = MIM::NO_MENU_ITEM;
	_generate_display();
}

auto Sorcery::CharacterDisplay::check_for_inventory_mouse_move(sf::Vector2f mouse_pos) -> unsigned int {

	const sf::Vector2f global_pos{this->getPosition()};
	const sf::Vector2f local_mouse_pos{mouse_pos - global_pos};

	if (_view == CHV::SUMMARY) {

		// Check for Mouse Over on Inventory Slots
		auto it{std::find_if(inventory_bounds.begin(), inventory_bounds.end(),
			[&local_mouse_pos](const auto &item) { return item.second.contains(local_mouse_pos); })};
		if (it != inventory_bounds.end()) {
			_hl_inventory_item = (*it).first;
			return (*it).first;
		} else {
			_hl_inventory_item = 0;
			return 0;
		}

	} else
		return 0;
}

auto Sorcery::CharacterDisplay::check_for_action_mouse_move(sf::Vector2f mouse_pos) -> std::optional<MIM> {

	const sf::Vector2f global_pos{this->getPosition()};
	const sf::Vector2f local_mouse_pos{mouse_pos - global_pos};

	if (_view == CHV::SUMMARY) {

		// Check for Mouse Over on Summary Action Items
		auto it{std::find_if(action_menu_bounds.begin(), action_menu_bounds.end(),
			[&local_mouse_pos](const auto &item) { return item.second.contains(local_mouse_pos); })};
		if (it != action_menu_bounds.end()) {
			_hl_action_item = (*it).first;
			return (*it).first;
		} else {
			_hl_action_item = MIM::NO_MENU_ITEM;
			return std::nullopt;
		}
	}

	else
		return std::nullopt;
}

auto Sorcery::CharacterDisplay::check_for_mouse_move(sf::Vector2f mouse_pos) -> std::optional<SPI> {

	const sf::Vector2f global_pos{this->getPosition()};
	const sf::Vector2f local_mouse_pos{mouse_pos - global_pos};
	if (_view == CHV::MAGE_SPELLS) {

		auto it{std::find_if(mage_spell_bounds.begin(), mage_spell_bounds.end(),
			[&local_mouse_pos](const auto &item) { return item.second.contains(local_mouse_pos); })};
		if (it != mage_spell_bounds.end()) {
			Component spell_name_c{(*_display->layout)["character_mage_spells:spell_name_label"]};
			_hl_mage_spell = (*it).first;

			std::vector<Spell>::iterator sit;
			sit = std::find_if(_character->spells().begin(), _character->spells().end(),
				[&](auto item) { return item.id == _hl_mage_spell; });
			if (sit != _character->spells().end())
				_spell_panel->set(*sit);

			return (*it).first;
		} else
			return std::nullopt;

	} else if (_view == CHV::PRIEST_SPELLS) {
		auto it{std::find_if(priest_spell_bounds.begin(), priest_spell_bounds.end(),
			[&local_mouse_pos](const auto &item) { return item.second.contains(local_mouse_pos); })};
		if (it != priest_spell_bounds.end()) {
			Component spell_name_c{(*_display->layout)["character_priest_spells:spell_name_label"]};
			_hl_priest_spell = (*it).first;

			std::vector<Spell>::iterator sit;
			sit = std::find_if(_character->spells().begin(), _character->spells().end(),
				[&](auto item) { return item.id == _hl_priest_spell; });
			if (sit != _character->spells().end())
				_spell_panel->set(*sit);

			return (*it).first;
		} else
			return std::nullopt;
	} else
		return std::nullopt;
}

auto Sorcery::CharacterDisplay::update() -> void {

	_hl_mage_spell_bg.setFillColor(_graphics->animation->select_col);
	_hl_priest_spell_bg.setFillColor(_graphics->animation->select_col);
	_hl_action_item_bg.setFillColor(_graphics->animation->select_col);

	//_inventory_display->update();
}

auto Sorcery::CharacterDisplay::draw(sf::RenderTarget &target, sf::RenderStates states) const -> void {

	states.transform *= getTransform();

	// Draw the common components
	for (const auto &[unique_key, frame] : _frames)
		target.draw(*frame, states);

	for (const auto &[unique_key, sprite] : _sprites)
		target.draw(sprite, states);

	for (const auto &[unique_key, text] : _texts)
		target.draw(text, states);

	if (_view == CHV::MAGE_SPELLS)
		target.draw(_hl_mage_spell_bg, states);
	else if (_view == CHV::PRIEST_SPELLS)
		target.draw(_hl_priest_spell_bg, states);

	// Draw the section components
	for (const auto &[unique_key, v_frame] : _v_frames)
		target.draw(*v_frame, states);

	for (const auto &[unique_key, v_sprite] : _v_sprites)
		target.draw(v_sprite, states);

	if (_view == CHV::SUMMARY && _hl_action_item != MIM::NO_MENU_ITEM)
		target.draw(_hl_action_item_bg, states);

	if (_view == CHV::SUMMARY && _hl_inventory_item != 0)
		target.draw(_hl_inventory_item_bg, states);

	for (const auto &[unique_key, v_text] : _v_texts)
		target.draw(v_text, states);

	// And the preview panels
	if (_view == CHV::MAGE_SPELLS)
		target.draw(*_spell_panel, states);

	if (_view == CHV::PRIEST_SPELLS)
		target.draw(*_spell_panel, states);

	_display->window->draw_cursor_coord(_display->get_cur());
}

auto Sorcery::CharacterDisplay::_generate_display() -> void {

	_sprites.clear();
	_texts.clear();
	_frames.clear();
	_v_sprites.clear();
	_v_texts.clear();
	_v_frames.clear();
	mage_spell_bounds.clear();
	priest_spell_bounds.clear();
	mage_spell_texts.clear();
	priest_spell_texts.clear();
	action_menu_bounds.clear();
	action_menu_texts.clear();
	inventory_bounds.clear();
	inventory_texts.clear();

	_display->generate("character", _sprites, _texts, _frames);

	if (_view == CHV::SUMMARY) {

		_display->generate("character_summary", _v_sprites, _v_texts, _v_frames);

		_add_text((*_display->layout)["character_summary:name_and_summary_text"], "{}", _character->summary_text());

		Component s_c{(*_display->layout)["character_summary:strength_value"]};
		s_c.colour = _graphics->adjust_colour(_character->attributes().at(CAR::STRENGTH), CAT::STAT);
		_add_text(s_c, "{:>2}", std::to_string(_character->attributes().at(CAR::STRENGTH)));

		Component i_c{(*_display->layout)["character_summary:iq_value"]};
		i_c.colour = _graphics->adjust_colour(_character->attributes().at(CAR::IQ), CAT::STAT);
		_add_text(i_c, "{:>2}", std::to_string(_character->attributes().at(CAR::IQ)));

		Component p_c{(*_display->layout)["character_summary:piety_value"]};
		p_c.colour = _graphics->adjust_colour(_character->attributes().at(CAR::PIETY), CAT::STAT);
		_add_text(p_c, "{:>2}", std::to_string(_character->attributes().at(CAR::PIETY)));

		Component a_c{(*_display->layout)["character_summary:agility_value"]};
		a_c.colour = _graphics->adjust_colour(_character->attributes().at(CAR::AGILITY), CAT::STAT);
		_add_text(a_c, "{:>2}", std::to_string(_character->attributes().at(CAR::AGILITY)));

		Component v_c{(*_display->layout)["character_summary:vitality_value"]};
		v_c.colour = _graphics->adjust_colour(_character->attributes().at(CAR::VITALITY), CAT::STAT);
		_add_text(v_c, "{:>2}", std::to_string(_character->attributes().at(CAR::VITALITY)));

		Component l_c{(*_display->layout)["character_summary:luck_value"]};
		l_c.colour = _graphics->adjust_colour(_character->attributes().at(CAR::LUCK), CAT::STAT);
		_add_text(l_c, "{:>2}", std::to_string(_character->attributes().at(CAR::LUCK)));

		Component inv_c{(*_display->layout)["global:inventory_display"]};
		_generate_inventory(inv_c);

		_add_text((*_display->layout)["character_summary:hp_value"], "{}",
			fmt::format("{}/{}", std::to_string(_character->abilities().at(CAB::CURRENT_HP)),
				std::to_string(_character->abilities().at(CAB::MAX_HP))));
		_add_text((*_display->layout)["character_summary:ac_value"], "{:>3}", std::to_string(_character->get_cur_ac()));
		_add_text((*_display->layout)["character_summary:age_value"], "{:>2}",
			std::to_string(static_cast<int>(_character->abilities().at(CAB::AGE) / 52)));
		_add_text((*_display->layout)["character_summary:swim_value"], "{:>2}",
			std::to_string(_character->abilities().at(CAB::SWIM)));
		auto status_text{
			_add_text((*_display->layout)["character_summary:status_value"], "{}", _character->get_status_string())};
		status_text->setFillColor(
			sf::Color(_graphics->adjust_status_colour(_character->get_status(), _character->is_poisoned())));

		_add_text((*_display->layout)["character_summary:exp_value"], "{:>11}",
			std::to_string(_character->abilities().at(CAB::CURRENT_XP)));
		_add_text((*_display->layout)["character_summary:next_value"], "{:>11}",
			std::to_string(_character->abilities().at(CAB::NEXT_LEVEL_XP)));
		_add_text((*_display->layout)["character_summary:gold_value"], "{:>11}",
			std::to_string(_character->abilities().at(CAB::GOLD)));
		_add_text((*_display->layout)["character_summary:marks_value"], "{:>11}",
			std::to_string(_character->abilities().at(CAB::MARKS)));
		_add_text((*_display->layout)["character_summary:deaths_value"], "{:>2}",
			std::to_string(_character->abilities().at(CAB::DEATHS)));

		auto mage_spells{fmt::format("{}/{}/{}/{}/{}/{}/{}", _character->mage_cur_sp().at(1),
			_character->mage_cur_sp().at(2), _character->mage_cur_sp().at(3), _character->mage_cur_sp().at(4),
			_character->mage_cur_sp().at(5), _character->mage_cur_sp().at(6), _character->mage_cur_sp().at(7))};
		auto priest_spells{fmt::format("{}/{}/{}/{}/{}/{}/{}", _character->priest_cur_sp().at(1),
			_character->priest_cur_sp().at(2), _character->priest_cur_sp().at(3), _character->priest_cur_sp().at(4),
			_character->priest_cur_sp().at(5), _character->priest_cur_sp().at(6), _character->priest_cur_sp().at(7))};

		_add_text((*_display->layout)["character_summary:mage_spells"], "{}", mage_spells);
		_add_text((*_display->layout)["character_summary:priest_spells"], "{}", priest_spells);

		Component action_c{(*_display->layout)["character_summary:action_panel"]};

		const auto action_x{action_c.x};
		const auto action_y{action_c.y};
		const auto offset_x_small{std::stoi(action_c["offset_x_small"].value())};
		const auto offset_x_big{std::stoi(action_c["offset_x_big"].value())};

		_add_action_button(action_c, MIM::C_ACTION_READ, "{:5}", "C_ACTION_READ", true);
		action_c.y += _display->window->get_ch();
		_add_action_button(
			action_c, MIM::C_ACTION_EQUIP, "{:5}", "C_ACTION_EQUIP", _mode == CHM::AT_CASTLE || _mode == CHM::IN_MAZE);
		action_c.x = action_x + (offset_x_small * _display->window->get_cw());
		action_c.y = action_y;
		_add_action_button(
			action_c, MIM::C_ACTION_TRADE, "{:5}", "C_ACTION_TRADE", _mode == CHM::AT_CASTLE || _mode == CHM::IN_MAZE);
		action_c.y += _display->window->get_ch();
		_add_action_button(
			action_c, MIM::C_ACTION_DROP, "{:5}", "C_ACTION_DROP", _mode == CHM::AT_CASTLE || _mode == CHM::IN_MAZE);
		action_c.x = action_c.x + (offset_x_small * _display->window->get_cw());
		action_c.y = action_y;
		_add_action_button(
			action_c, MIM::C_ACTION_POOL, "{:9}", "C_ACTION_POOL", _mode == CHM::AT_CASTLE || _mode == CHM::IN_MAZE);
		action_c.y += _display->window->get_ch();
		_add_action_button(action_c, MIM::C_ACTION_IDENTIFY, "{:9}", "C_ACTION_IDENTIFY", _mode == CHM::IN_MAZE);
		action_c.x = action_c.x + (offset_x_big * _display->window->get_cw());
		action_c.y = action_y;
		_add_action_button(action_c, MIM::C_ACTION_SPELL, "{:5}", "C_ACTION_SPELL", _mode == CHM::IN_MAZE);
		action_c.y += _display->window->get_ch();
		_add_action_button(action_c, MIM::C_ACTION_USE, "{:5}", "C_ACTION_USE", _mode == CHM::IN_MAZE);
		action_c.y += _display->window->get_ch();
		action_c.x = action_c.x + (offset_x_small * _display->window->get_cw());
		action_c.y = action_y;
		_add_action_button(action_c, MIM::C_ACTION_INVOKE, "{:6}", "C_ACTION_INVOKE",
			_mode == CHM::AT_CASTLE || _mode == CHM::IN_MAZE);
		action_c.y += _display->window->get_ch();
		_add_action_button(action_c, MIM::C_ACTION_LEAVE, "{:6}", "C_ACTION_LEAVE", true);

	} else if (_view == CHV::DETAILED) {

		_display->generate("character_detailed", _v_sprites, _v_texts, _v_frames);

		_add_text((*_display->layout)["character_detailed:name_and_summary_text"], "{}", _character->summary_text());

		Component s_c{(*_display->layout)["character_detailed:strength_value"]};
		s_c.colour = _graphics->adjust_colour(_character->attributes().at(CAR::STRENGTH), CAT::STAT);
		_add_text(s_c, "{:>2}", std::to_string(_character->attributes().at(CAR::STRENGTH)));

		Component i_c{(*_display->layout)["character_detailed:iq_value"]};
		i_c.colour = _graphics->adjust_colour(_character->attributes().at(CAR::IQ), CAT::STAT);
		_add_text(i_c, "{:>2}", std::to_string(_character->attributes().at(CAR::IQ)));

		Component p_c{(*_display->layout)["character_detailed:piety_value"]};
		p_c.colour = _graphics->adjust_colour(_character->attributes().at(CAR::PIETY), CAT::STAT);
		_add_text(p_c, "{:>2}", std::to_string(_character->attributes().at(CAR::PIETY)));

		Component a_c{(*_display->layout)["character_detailed:agility_value"]};
		a_c.colour = _graphics->adjust_colour(_character->attributes().at(CAR::AGILITY), CAT::STAT);
		_add_text(a_c, "{:>2}", std::to_string(_character->attributes().at(CAR::AGILITY)));

		Component v_c{(*_display->layout)["character_detailed:vitality_value"]};
		v_c.colour = _graphics->adjust_colour(_character->attributes().at(CAR::VITALITY), CAT::STAT);
		_add_text(v_c, "{:>2}", std::to_string(_character->attributes().at(CAR::VITALITY)));

		Component l_c{(*_display->layout)["character_detailed:luck_value"]};
		l_c.colour = _graphics->adjust_colour(_character->attributes().at(CAR::LUCK), CAT::STAT);
		_add_text(l_c, "{:>2}", std::to_string(_character->attributes().at(CAR::LUCK)));

		Component strength_c((*_display->layout)["character_detailed:strength_detailed_values"]);

		strength_c.colour = _graphics->adjust_colour(_character->abilities().at(CAB::ATTACK_MODIFIER), CAT::MODIFIER);
		_add_text(strength_c, "{:+>2}", std::to_string(_character->abilities().at(CAB::ATTACK_MODIFIER)));

		strength_c.y += _display->window->get_ch();
		// strength_c.colour = _graphics->adjust_colour(_character->get_cur_to_hit(), MODIFIER);
		//_add_text(strength_c, "{:+>2}", std::to_string(_character->get_cur_to_hit()));
		strength_c.colour = _graphics->adjust_colour(_character->abilities().at(CAB::HIT_PROBABILITY), CAT::MODIFIER);
		_add_text(strength_c, "{:+>2}", std::to_string(_character->abilities().at(CAB::HIT_PROBABILITY)));

		strength_c.y += _display->window->get_ch();
		strength_c.colour = _graphics->adjust_colour(_character->abilities().at(CAB::BONUS_DAMAGE), CAT::MODIFIER);
		_add_text(strength_c, "{:+>2}", std::to_string(_character->abilities().at(CAB::BONUS_DAMAGE)));

		strength_c.y += _display->window->get_ch();
		// strength_c.colour = _graphics->adjust_colour(_character->get_cur_num_attacks(), NUMBER);
		// ANCHOR - (strength_c, "{:>2}", std::to_string(_character->get_cur_num_attacks()));
		strength_c.colour =
			_graphics->adjust_colour(_character->abilities().at(CAB::BASE_NUMBER_OF_ATTACKS), CAT::NUMBER);
		_add_text(strength_c, "{:>2}", std::to_string(_character->abilities().at(CAB::BASE_NUMBER_OF_ATTACKS)));

		strength_c.y += _display->window->get_ch();
		strength_c.colour = _graphics->adjust_colour(_character->abilities().at(CAB::UNARMED_DAMAGE), CAT::NUMBER);
		_add_text(strength_c, "{:>2}", std::to_string(_character->abilities().at(CAB::UNARMED_DAMAGE)));

		Component iq_c((*_display->layout)["character_detailed:iq_detailed_values"]);

		iq_c.colour = _graphics->adjust_colour(_character->abilities().at(CAB::MAGE_SPELL_LEARN), CAT::PERCENTAGE);
		_add_text(iq_c, "{:>2}%", std::to_string(_character->abilities().at(CAB::MAGE_SPELL_LEARN)));

		iq_c.y += _display->window->get_ch();
		iq_c.colour = _graphics->adjust_colour(_character->abilities().at(CAB::IDENTIFY_ITEMS), CAT::PERCENTAGE);
		_add_text(iq_c, "{:>2}%", std::to_string(_character->abilities().at(CAB::IDENTIFY_ITEMS)));

		iq_c.y += _display->window->get_ch();
		iq_c.colour = _graphics->adjust_colour(_character->abilities().at(CAB::IDENTIFY_CURSE), CAT::PERCENTAGE);
		_add_text(iq_c, "{:>2}%", std::to_string(_character->abilities().at(CAB::IDENTIFY_CURSE)));

		iq_c.y += _display->window->get_ch();
		iq_c.colour = _graphics->adjust_colour(_character->abilities().at(CAB::IDENTIFY_FOES), CAT::PERCENTAGE);
		_add_text(iq_c, "{:>2}%", std::to_string(_character->abilities().at(CAB::IDENTIFY_FOES)));

		iq_c.y += _display->window->get_ch();
		iq_c.colour = _graphics->adjust_colour(_character->abilities().at(CAB::BONUS_MAGE_SPELLS), CAT::NUMBER);
		_add_text(iq_c, "{:>2}", std::to_string(_character->abilities().at(CAB::BONUS_MAGE_SPELLS)));

		Component piety_c((*_display->layout)["character_detailed:piety_detailed_values"]);
		piety_c.colour = _graphics->adjust_colour(_character->abilities().at(CAB::PRIEST_SPELL_LEARN), CAT::PERCENTAGE);
		_add_text(piety_c, "{:>2}%", std::to_string(_character->abilities().at(CAB::PRIEST_SPELL_LEARN)));

		piety_c.y += _display->window->get_ch();
		piety_c.colour = _graphics->adjust_colour(_character->abilities().at(CAB::LOKTOFELT_SUCCESS), CAT::PERCENTAGE);
		_add_text(piety_c, "{:>2}%", std::to_string(_character->abilities().at(CAB::LOKTOFELT_SUCCESS)));

		piety_c.y += _display->window->get_ch();
		piety_c.colour = _graphics->adjust_colour(_character->abilities().at(CAB::BASE_DISPELL), CAT::PERCENTAGE);
		_add_text(piety_c, "{:>2}%", std::to_string(_character->abilities().at(CAB::BASE_DISPELL)));

		piety_c.y += _display->window->get_ch();
		piety_c.colour = _graphics->adjust_colour(_character->abilities().at(CAB::BONUS_PRIEST_SPELLS), CAT::NUMBER);
		_add_text(piety_c, "{:>2}", std::to_string(_character->abilities().at(CAB::BONUS_PRIEST_SPELLS)));

		Component vitality_c((*_display->layout)["character_detailed:vitality_detailed_values"]);
		vitality_c.colour = _graphics->adjust_colour(_character->abilities().at(CAB::VITALITY_BONUS), CAT::MODIFIER);
		_add_text(vitality_c, "{:+>2}", std::to_string(_character->abilities().at(CAB::VITALITY_BONUS)));

		vitality_c.y += _display->window->get_ch();
		vitality_c.colour = _graphics->adjust_colour(_character->abilities().at(CAB::BONUS_HIT_POINTS), CAT::MODIFIER);
		_add_text(vitality_c, "{:+>2}", std::to_string(_character->abilities().at(CAB::BONUS_HIT_POINTS)));

		vitality_c.y += _display->window->get_ch();
		vitality_c.colour = _graphics->adjust_colour(_character->abilities().at(CAB::DEAD_RESURRECT), CAT::PERCENTAGE);
		_add_text(vitality_c, "{:>2}%", std::to_string(_character->abilities().at(CAB::DEAD_RESURRECT)));

		vitality_c.y += _display->window->get_ch();
		vitality_c.colour = _graphics->adjust_colour(_character->abilities().at(CAB::ASHES_RESURRECT), CAT::PERCENTAGE);
		_add_text(vitality_c, "{:>2}%", std::to_string(_character->abilities().at(CAB::ASHES_RESURRECT)));

		vitality_c.y += _display->window->get_ch();
		vitality_c.colour =
			_graphics->adjust_colour(_character->abilities().at(CAB::DI_KADORTO_RESURRECT), CAT::PERCENTAGE);
		_add_text(vitality_c, "{:>2}%", std::to_string(_character->abilities().at(CAB::DI_KADORTO_RESURRECT)));

		Component agility_c((*_display->layout)["character_detailed:agility_detailed_values"]);

		agility_c.colour =
			_graphics->adjust_colour(_character->abilities().at(CAB::INITIATIVE_MODIFIER), CAT::MODIFIER);
		_add_text(agility_c, "{:+>2}", std::to_string(_character->abilities().at(CAB::INITIATIVE_MODIFIER)));

		agility_c.y += _display->window->get_ch();
		agility_c.colour =
			_graphics->adjust_colour(_character->abilities().at(CAB::BASE_CRITICAL_HIT), CAT::PERCENTAGE);
		_add_text(agility_c, "{:>2}%", std::to_string(_character->abilities().at(CAB::BASE_CRITICAL_HIT)));

		agility_c.y += _display->window->get_ch();
		agility_c.colour = _graphics->adjust_colour(_character->abilities().at(CAB::IDENTIFY_TRAP), CAT::PERCENTAGE);
		_add_text(agility_c, "{:>2}%", std::to_string(_character->abilities().at(CAB::IDENTIFY_TRAP)));

		agility_c.y += _display->window->get_ch();
		agility_c.colour = _graphics->adjust_colour(_character->abilities().at(CAB::BASE_DISARM_TRAP), CAT::PERCENTAGE);
		_add_text(agility_c, "{:>2}%", std::to_string(_character->abilities().at(CAB::BASE_DISARM_TRAP)));

		agility_c.y += _display->window->get_ch();
		agility_c.colour =
			_graphics->adjust_colour((100 - _character->abilities().at(CAB::ACTIVATE_TRAP)), CAT::PERCENTAGE);
		_add_text(agility_c, "{:>2}%", std::to_string(100 - _character->abilities().at(CAB::ACTIVATE_TRAP)));

		agility_c.y += _display->window->get_ch();
		agility_c.colour = _graphics->adjust_colour(_character->abilities().at(CAB::BASE_AVOID_PIT), CAT::PERCENTAGE);
		_add_text(agility_c, "{:>2}%", std::to_string(_character->abilities().at(CAB::BASE_AVOID_PIT)));

		agility_c.y += _display->window->get_ch();
		agility_c.colour = _graphics->adjust_colour(_character->abilities().at(CAB::BASE_ARMOUR_CLASS), CAT::AC);
		_add_text(agility_c, "{:>2}", std::to_string(_character->abilities().at(CAB::BASE_ARMOUR_CLASS)));

		Component luck_c((*_display->layout)["character_detailed:luck_detailed_values"]);

		luck_c.colour =
			_graphics->adjust_colour(_character->abilities().at(CAB::BASE_RESIST_BONUS) * 5, CAT::PERCENTAGE);
		_add_text(luck_c, "{:>2}%", std::to_string(_character->abilities().at(CAB::BASE_RESIST_BONUS) * 5));

		luck_c.y += _display->window->get_ch();
		luck_c.colour =
			_graphics->adjust_colour(_character->abilities().at(CAB::EQUIPMENT_INTACT_ON_WIPE), CAT::PERCENTAGE);
		_add_text(luck_c, "{:>2}%", std::to_string(_character->abilities().at(CAB::EQUIPMENT_INTACT_ON_WIPE)));

	} else if (_view == CHV::RESISTANCES) {

		_display->generate("character_resistances", _v_sprites, _v_texts, _v_frames);

		_add_text((*_display->layout)["character_resistances:name_and_summary_text"], "{}", _character->summary_text());

		Component resistances_c((*_display->layout)["character_resistances:resistances_detailed_values"]);
		resistances_c.colour =
			_graphics->adjust_colour(_character->abilities().at(CAB::RESISTANCE_VS_CRITICAL_HIT) * 5, CAT::PERCENTAGE);
		_add_text(
			resistances_c, "{:>2}%", std::to_string(_character->abilities().at(CAB::RESISTANCE_VS_CRITICAL_HIT) * 5));

		resistances_c.y += _display->window->get_ch();
		resistances_c.colour = _graphics->adjust_colour(
			_character->abilities().at(CAB::RESISTANCE_VS_POISON_PARALYSIS) * 5, CAT::PERCENTAGE);
		_add_text(resistances_c, "{:>2}%",
			std::to_string(_character->abilities().at(CAB::RESISTANCE_VS_POISON_PARALYSIS) * 5));

		resistances_c.y += _display->window->get_ch();
		resistances_c.colour =
			_graphics->adjust_colour(_character->abilities().at(CAB::RESISTANCE_VS_STONING) * 5, CAT::PERCENTAGE);
		_add_text(resistances_c, "{:>2}%", std::to_string(_character->abilities().at(CAB::RESISTANCE_VS_STONING) * 5));

		resistances_c.y += _display->window->get_ch();
		resistances_c.colour = _graphics->adjust_colour(
			_character->abilities().at(CAB::RESISTANCE_VS_BREATH_ATTACKS) * 5, CAT::PERCENTAGE);
		_add_text(
			resistances_c, "{:>2}%", std::to_string(_character->abilities().at(CAB::RESISTANCE_VS_BREATH_ATTACKS) * 5));

		resistances_c.y += _display->window->get_ch();
		resistances_c.colour = _graphics->adjust_colour(
			_character->abilities().at(CAB::RESISTANCE_VS_POISON_GAS_TRAP) * 5, CAT::PERCENTAGE);
		_add_text(resistances_c, "{:>2}%",
			std::to_string(_character->abilities().at(CAB::RESISTANCE_VS_POISON_GAS_TRAP) * 5));

		resistances_c.y += _display->window->get_ch();
		resistances_c.colour = _graphics->adjust_colour(
			_character->abilities().at(CAB::RESISTANCE_VS_MAGE_PRIEST_TRAP) * 5, CAT::PERCENTAGE);
		_add_text(resistances_c, "{:>2}%",
			std::to_string(_character->abilities().at(CAB::RESISTANCE_VS_MAGE_PRIEST_TRAP) * 5));

		resistances_c.y += _display->window->get_ch();
		resistances_c.colour =
			_graphics->adjust_colour(_character->abilities().at(CAB::RECOVER_FROM_SLEEP), CAT::PERCENTAGE);
		_add_text(resistances_c, "{:>2}%", std::to_string(_character->abilities().at(CAB::RECOVER_FROM_SLEEP)));

		resistances_c.y += _display->window->get_ch();
		resistances_c.colour =
			_graphics->adjust_colour(_character->abilities().at(CAB::RECOVER_FROM_FEAR), CAT::PERCENTAGE);
		_add_text(resistances_c, "{:>2}%", std::to_string(_character->abilities().at(CAB::RECOVER_FROM_FEAR)));

		resistances_c.y += _display->window->get_ch();
		resistances_c.colour =
			_graphics->adjust_colour(_character->abilities().at(CAB::RESISTANCE_VS_SILENCE) * 5, CAT::PERCENTAGE);
		_add_text(resistances_c, "{:>2}%", std::to_string(_character->abilities().at(CAB::RESISTANCE_VS_SILENCE) * 5));

		resistances_c.y += _display->window->get_ch();
		resistances_c.colour =
			_graphics->adjust_colour(_character->abilities().at(CAB::RESISTANCE_VS_KATINO), CAT::PERCENTAGE);
		_add_text(resistances_c, "{:>2}%", std::to_string(_character->abilities().at(CAB::RESISTANCE_VS_KATINO)));

		resistances_c.y += _display->window->get_ch();
		resistances_c.colour =
			_graphics->adjust_colour(_character->abilities().at(CAB::RESISTANCE_VS_BADI), CAT::PERCENTAGE);
		_add_text(resistances_c, "{:>2}%", std::to_string(_character->abilities().at(CAB::RESISTANCE_VS_BADI)));

		resistances_c.y += _display->window->get_ch();
		resistances_c.colour =
			_graphics->adjust_colour(_character->abilities().at(CAB::RESISTANCE_VS_MANIFO), CAT::PERCENTAGE);
		_add_text(resistances_c, "{:>2}%", std::to_string(_character->abilities().at(CAB::RESISTANCE_VS_MANIFO)));

	} else if (_view == CHV::MAGE_SPELLS) {

		_display->generate("character_mage_spells", _v_sprites, _v_texts, _v_frames);

		_add_text((*_display->layout)["character_mage_spells:name_and_summary_text"], "{}", _character->summary_text());

		Component level_c{(*_display->layout)["character_mage_spells:level_label"]};
		Component sp_c{(*_display->layout)["character_mage_spells:spell_points"]};
		Component spell_name_c{(*_display->layout)["character_mage_spells:spell_name_label"]};

		auto level_x{level_c.x};
		for (auto level = 1; level <= 7; level++) {

			sp_c.x = level_c.x + (std::stoi(sp_c["offset_columns"].value()) * _display->window->get_cw());
			sp_c.y = level_c.y;

			spell_name_c.x = level_c.x;
			spell_name_c.y = level_c.y + _display->window->get_ch();

			auto spells{_character->spells() | std::views::filter([&](Spell spell) {
				return (spell.type == SPT::MAGE) && (static_cast<int>(spell.level) == level);
			})};
			for (auto spell : spells) {

				// Add the Spell
				auto spell_name_text{PADSTR(spell.name, std::stoi(spell_name_c["bar_width"].value()))};
				auto spell_name{_add_text(spell_name_c, "{}", spell_name_text)};
				auto hl_bounds{spell_name->getGlobalBounds()};
				mage_spell_bounds[spell.id] = hl_bounds;
				mage_spell_texts[spell.id] = spell_name;

				if (spell.id == _hl_mage_spell) {
					sf::RectangleShape bg(sf::Vector2f(
						std::stoi(spell_name_c["bar_width"].value()) * _display->window->get_cw(), hl_bounds.height));
					bg.setPosition(hl_bounds.left, hl_bounds.top);
					bg.setFillColor(_graphics->animation->select_col);
					spell_name->setFillColor(sf::Color(spell_name_c.colour));
					spell_name->setOutlineColor(sf::Color(0, 0, 0));
					spell_name->setOutlineThickness(2);
					_hl_mage_spell_bg = bg;
				} else {
					if (spell.known)
						spell_name->setFillColor(sf::Color(std::stoull(spell_name_c["known_colour"].value(), 0, 16)));
					else
						spell_name->setFillColor(sf::Color(std::stoull(spell_name_c["unknown_colour"].value(), 0, 16)));
				}

				// And the Spell Category Icon
				Component spell_icon_c{(*_display->layout)["character_mage_spells:spell_icon"]};
				auto spell_icon{_get_spell_icon(spell.category)};
				if (spell_icon) {
					spell_icon.value().setScale(spell_icon_c.scl());
					const auto offset_x{std::invoke([&] {
						if (spell_icon_c["offset_x"])
							return std::stoi(spell_icon_c["offset_x"].value());
						else
							return 0;
					})};
					const auto offset_y{std::invoke([&] {
						if (spell_icon_c["offset_y"])
							return std::stoi(spell_icon_c["offset_y"].value());
						else
							return 0;
					})};

					spell_icon.value().setPosition(
						spell_name_c.x + offset_x +
							(std::stoi(spell_icon_c["offset_columns"].value()) * _display->window->get_cw()),
						spell_name_c.y + offset_y);

					if (spell.known)
						spell_icon->setColor(sf::Color(std::stoull(spell_name_c["known_colour"].value(), 0, 16)));
					else
						spell_icon->setColor(sf::Color(std::stoull(spell_name_c["unknown_colour"].value(), 0, 16)));

					_v_sprites.emplace(GUID(), spell_icon.value());
				}

				spell_name_c.y += _display->window->get_ch();
			}

			if (level % 3 == 1 || level % 3 == 2)
				level_c.x += (std::stoi(level_c["offset_columns"].value()) * _display->window->get_cw());
			else if (level % 3 == 0) {
				level_c.x = level_x;
				level_c.y += (std::stoi(level_c["offset_rows"].value()) * _display->window->get_ch());
			}
		}
	} else if (_view == CHV::PRIEST_SPELLS) {

		_display->generate("character_priest_spells", _v_sprites, _v_texts, _v_frames);

		_add_text(
			(*_display->layout)["character_priest_spells:name_and_summary_text"], "{}", _character->summary_text());

		Component level_c{(*_display->layout)["character_priest_spells:level_label"]};
		Component sp_c{(*_display->layout)["character_priest_spells:spell_points"]};
		Component spell_name_c{(*_display->layout)["character_priest_spells:spell_name_label"]};

		auto level_x{level_c.x};
		for (auto level = 1; level <= 7; level++) {

			sp_c.x = level_c.x + (std::stoi(sp_c["offset_columns"].value()) * _display->window->get_cw());
			sp_c.y = level_c.y;

			spell_name_c.x = level_c.x;
			spell_name_c.y = level_c.y + _display->window->get_ch();

			auto spells{_character->spells() | std::views::filter([&](Spell spell) {
				return (spell.type == SPT::PRIEST) && (static_cast<int>(spell.level) == level);
			})};
			for (auto spell : spells) {

				auto spell_name_text{PADSTR(spell.name, std::stoi(spell_name_c["bar_width"].value()))};
				auto spell_name{_add_text(spell_name_c, "{}", spell_name_text)};
				spell_name->setPosition(spell_name_c.x, spell_name_c.y);
				auto hl_bounds{spell_name->getGlobalBounds()};
				priest_spell_bounds[spell.id] = hl_bounds;
				priest_spell_texts[spell.id] = spell_name;

				if (spell.id == _hl_priest_spell) {
					sf::RectangleShape bg(sf::Vector2f(
						std::stoi(spell_name_c["bar_width"].value()) * _display->window->get_cw(), hl_bounds.height));
					bg.setPosition(hl_bounds.left, hl_bounds.top);
					bg.setFillColor(_graphics->animation->select_col);
					spell_name->setFillColor(sf::Color(spell_name_c.colour));
					spell_name->setOutlineColor(sf::Color(0, 0, 0));
					spell_name->setOutlineThickness(2);
					_hl_priest_spell_bg = bg;
				} else {
					if (spell.known)
						spell_name->setFillColor(sf::Color(std::stoull(spell_name_c["known_colour"].value(), 0, 16)));
					else
						spell_name->setFillColor(sf::Color(std::stoull(spell_name_c["unknown_colour"].value(), 0, 16)));
				}

				// And the Spell Category Icon
				Component spell_icon_c{(*_display->layout)["character_mage_spells:spell_icon"]};
				if (auto spell_icon{_get_spell_icon(spell.category)}; spell_icon) {
					spell_icon.value().setScale(spell_icon_c.scl());
					const auto offset_x{std::invoke([&] {
						if (spell_icon_c["offset_x"])
							return std::stoi(spell_icon_c["offset_x"].value());
						else
							return 0;
					})};
					const auto offset_y{std::invoke([&] {
						if (spell_icon_c["offset_y"])
							return std::stoi(spell_icon_c["offset_y"].value());
						else
							return 0;
					})};

					spell_icon.value().setPosition(
						spell_name_c.x + offset_x +
							(std::stoi(spell_icon_c["offset_columns"].value()) * _display->window->get_cw()),
						spell_name_c.y + offset_y);

					if (spell.known)
						spell_icon->setColor(sf::Color(std::stoull(spell_name_c["known_colour"].value(), 0, 16)));
					else
						spell_icon->setColor(sf::Color(std::stoull(spell_name_c["unknown_colour"].value(), 0, 16)));

					_v_sprites.emplace(GUID(), spell_icon.value());
				}

				spell_name_c.y += _display->window->get_ch();
			}

			if (level % 3 == 1 || level % 3 == 2)
				level_c.x += (std::stoi(level_c["offset_columns"].value()) * _display->window->get_cw());
			else if (level % 3 == 0) {
				level_c.x = level_x;
				level_c.y += (std::stoi(level_c["offset_rows"].value()) * _display->window->get_ch());
			}
		}
	}
}

auto Sorcery::CharacterDisplay::get_hl_action_item() const -> MIM {

	return _hl_action_item;
}

auto Sorcery::CharacterDisplay::get_inventory_item() const -> unsigned int {

	return _hl_inventory_item;
}

auto Sorcery::CharacterDisplay::_add_action_button(
	Component layout_c, const MIM item, const std::string format, const std::string str, const bool enabled) -> void {

	auto text{_add_text(layout_c, format, (*_display->string)[str])};
	action_menu_texts[item] = text;
	if (enabled) {
		auto text_hl_bounds{text->getGlobalBounds()};
		action_menu_bounds[item] = text_hl_bounds;
		if (_hl_action_item == item) {
			sf::RectangleShape bg(
				sf::Vector2f((*_display->string)[str].length() * _display->window->get_cw(), text_hl_bounds.height));
			bg.setPosition(text_hl_bounds.left, text_hl_bounds.top);
			bg.setFillColor(_graphics->animation->select_col);
			text->setFillColor(sf::Color(layout_c.colour));
			text->setOutlineColor(sf::Color(0, 0, 0));
			text->setOutlineThickness(2);
			_hl_action_item_bg = bg;
		} else
			text->setFillColor(sf::Color(std::stoull(layout_c["enabled_colour"].value(), nullptr, 16)));
	} else
		text->setFillColor(sf::Color(std::stoull(layout_c["disabled_colour"].value(), nullptr, 16)));
}

/*
 * 	READ	TRADE	POOL		SPELL	NEXT
 * 	EQUIP	DROP	IDENTIFY	USE		LEAVE
 *
 * 	TRAINING:
 * 	READ	xxxxx	xxxx		xxxxx	xxxx
 * 	xxxxx	xxxx	xxxxxxxx	xxx		LEAVE
 *
 * 	CASTLE:
 *	READ	TRADE	POOL		xxxxx	NEXT
 * 	EQUIP	DROP	xxxxxxxx	xxx		LEAVE
 *
 * 	MAZE:
 * 	READ	TRADE	POOL		SPELL	NEXT
 *	EQUIP	DROP	IDENTIFY	USE		LEAVE
 *
 * */
auto Sorcery::CharacterDisplay::left_hl_action() -> void {
}

auto Sorcery::CharacterDisplay::right_hl_action() -> void {
}

auto Sorcery::CharacterDisplay::up_hl_action() -> void {
}

auto Sorcery::CharacterDisplay::down_hl_action() -> void {
}
