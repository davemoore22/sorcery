
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
Sorcery::CharacterDisplay::CharacterDisplay(
	System *system, Display *display, Graphics *graphics)
	: _system{system}, _display{display}, _graphics{graphics} {

	_texts.clear();
	_sprites.clear();
	_frames.clear();
	_v_texts.clear();
	_v_sprites.clear();
	_v_frames.clear();
	_view = CHV::NO_VIEW;
	_hl_mage = SPI::DUMAPIC;
	_hl_priest = SPI::BADIOS;
	_hl_action = MIM::NO_MENU_ITEM;
	_hl_inv = 0;
	mage_bounds.clear();
	priest_bounds.clear();
	mage_texts.clear();
	priest_texts.clear();
	action_texts.clear();
	action_bounds.clear();
	inv_texts.clear();
	inv_bounds.clear();

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

auto Sorcery::CharacterDisplay::_generate_inventory(Component layout_c)
	-> void {

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

		auto bounds{text->getGlobalBounds()};
		inv_bounds[slot] = bounds;

		if (_hl_inv == slot) {
			sf::RectangleShape bg(sf::Vector2f(
				line.length() * _display->window->get_cw(), bounds.height));
			bg.setPosition(bounds.left, bounds.top);
			bg.setFillColor(_graphics->animation->select_col);
			text->setFillColor(sf::Color(layout_c.colour));
			text->setOutlineColor(sf::Color(0, 0, 0));
			text->setOutlineThickness(2);
			_hl_inv_bg = bg;
		} else
			text->setFillColor(sf::Color(layout_c.colour));

		++slot;
	}
}

auto Sorcery::CharacterDisplay::clear_inventory_item() -> void {

	_hl_inv = 0;
}

auto Sorcery::CharacterDisplay::set_mode(CHM value) -> void {

	_mode = value;
}

auto Sorcery::CharacterDisplay::inc_hl_spell(SPT type) -> void {

	if (type == SPT::MAGE) {
		auto index{+_hl_mage};
		if (index < unenum(SPI::TILTOWAIT)) {
			++index;
			_hl_mage = magic_enum::enum_cast<SPI>(index).value();
		}
	} else {
		auto index{+_hl_priest};
		if (index < unenum(SPI::MALIKTO)) {
			++index;
			_hl_priest = magic_enum::enum_cast<SPI>(index).value();
		}
	}

	_generate_display();
}

auto Sorcery::CharacterDisplay::dec_hl_spell(SPT type) -> void {

	if (type == SPT::MAGE) {
		auto index{unenum(_hl_mage)};
		if (index > (unenum(SPI::DUMAPIC))) {
			--index;
			_hl_mage = magic_enum::enum_cast<SPI>(index).value();
		}
	} else {
		auto index{unenum(_hl_priest)};
		if (index > (unenum(SPI::BADIOS))) {
			--index;
			_hl_priest = magic_enum::enum_cast<SPI>(index).value();
		}
	}

	_generate_display();
}

// Setting the view will regenerate the display components
auto Sorcery::CharacterDisplay::left_view() -> void {

	auto idx{unenum(_view)};
	if (idx == unenum(CHV::SUMMARY))
		idx = unenum(CHV::PRIEST_SPELLS);
	else
		--idx;
	_view = magic_enum::enum_cast<CHV>(idx).value();

	_display->layout->refresh_if_needed();

	_generate_display();
}

// Setting the view will regenerate the display components
auto Sorcery::CharacterDisplay::right_view() -> void {

	auto idx{unenum(_view)};
	if (idx == unenum(CHV::PRIEST_SPELLS))
		idx = unenum(CHV::SUMMARY);
	else
		++idx;
	_view = magic_enum::enum_cast<CHV>(idx).value();

	_display->layout->refresh_if_needed();

	_generate_display();
}

auto Sorcery::CharacterDisplay::_get_character_portrait() -> sf::Sprite {

	auto img{_graphics->textures
			->get(_character->get_portrait_index(), GTT::PORTRAIT)
			.value()};

	return img;
}

auto Sorcery::CharacterDisplay::get_icon(CHS type)
	-> std::optional<sf::Sprite> {

	switch (type) {
	case CHS::CHOOSE_ALIGNMENT: {
		auto str{_character->alignment_to_str(_character->get_alignment())};
		std::ranges::transform(str.begin(), str.end(), str.begin(), ::tolower);
		return (*_graphics->icons)[str].value();
	} break;
	case CHS::CHOOSE_RACE: {
		auto str{_character->race_to_str(_character->get_race())};
		std::ranges::transform(str.begin(), str.end(), str.begin(), ::tolower);
		return (*_graphics->icons)[str].value();
	} break;
	case CHS::CHOOSE_CLASS: {
		auto str{_character->class_to_str(_character->get_class())};
		std::ranges::transform(str.begin(), str.end(), str.begin(), ::tolower);
		return (*_graphics->icons)[str].value();
	} break;
	default:
		break;
	}

	return std::nullopt;
}

auto Sorcery::CharacterDisplay::_generate_summary_icons() -> void {

	auto c_icon{get_icon(CHS::CHOOSE_CLASS).value()};
	c_icon.setPosition((*_display->layout)["character:class_icon"].pos());
	c_icon.setScale((*_display->layout)["character:class_icon"].scl());
	_v_sprites.try_emplace(
		(*_display->layout)["character:class_icon"].unique_key, c_icon);

	auto r_icon{get_icon(CHS::CHOOSE_RACE).value()};
	r_icon.setPosition((*_display->layout)["character:race_icon"].pos());
	r_icon.setScale((*_display->layout)["character:race_icon"].scl());
	_v_sprites.try_emplace(
		(*_display->layout)["character:race_icon"].unique_key, r_icon);

	auto a_icon{get_icon(CHS::CHOOSE_ALIGNMENT).value()};
	a_icon.setPosition((*_display->layout)["character:alignment_icon"].pos());
	a_icon.setScale((*_display->layout)["character:alignment_icon"].scl());
	_v_sprites.try_emplace(
		(*_display->layout)["character:alignment_icon"].unique_key, a_icon);

	auto l_icon{(*_graphics->icons)["level"].value()};
	l_icon.setPosition((*_display->layout)["character:level_icon"].pos());
	l_icon.setScale((*_display->layout)["character:level_icon"].scl());
	_v_sprites.try_emplace(
		(*_display->layout)["character:level_icon"].unique_key, l_icon);

	_add_text((*_display->layout)["character:level_text"], "{}",
		std::to_string(_character->abilities().at(CAB::CURRENT_LEVEL)), true);
}

auto Sorcery::CharacterDisplay::generate_display() -> void {

	_generate_display();
}

auto Sorcery::CharacterDisplay::_add_icon(
	Component &component, std::string icon_key) -> void {

	auto icon{(*_graphics->icons)[icon_key].value()};
	const auto off_x{std::invoke([&] {
		if (component["offset_x"])
			return std::stoi(component["offset_x"].value());
		else
			return 0;
	})};
	const auto off_y{std::invoke([&] {
		if (component["offset_y"])
			return std::stoi(component["offset_y"].value());
		else
			return 0;
	})};
	icon.setPosition(component.x + off_x, component.y + off_y);
	icon.setScale(component.scl());
	_v_sprites.try_emplace(component.unique_key, icon);
}

auto Sorcery::CharacterDisplay::_add_text(Component &component,
	std::string format, std::string value, bool is_view) -> sf::Text * {

	sf::Text text{};

	// Note that Format v8 needs the format string wrapped in fmt::runtime -
	// this isn't available in < v8 - see
	// https://github.com/fmtlib/fmt/issues/2438 - check FMT_VERSION version in
	// fmt/core.h
	auto str{fmt::format(fmt::runtime(format), value)};
	if (_display->get_upper())
		std::transform(str.begin(), str.end(), str.begin(), ::toupper);
	text.setFont(_system->resources->fonts[component.font]);
	text.setCharacterSize(component.size);
	text.setFillColor(sf::Color(component.colour));
	text.setString(str);
	if (_display->get_bold())
		text.setStyle(sf::Text::Bold);
	const auto off_x{std::invoke([&] {
		if (component["offset_x"])
			return std::stoi(component["offset_x"].value());
		else
			return 0;
	})};
	const auto off_y{std::invoke([&] {
		if (component["offset_y"])
			return std::stoi(component["offset_y"].value());
		else
			return 0;
	})};
	text.setPosition(component.x + off_x, component.y + off_y);

	// Generate a new key as this is a map, and we might call this with the same
	// base component
	auto key{GUID()};
	if (is_view) {
		_v_texts.try_emplace(key, text);
		return &_v_texts.at(key);
	} else {
		_texts.try_emplace(key, text);
		return &_texts.at(key);
	}
}

auto Sorcery::CharacterDisplay::get_view() const -> CHV {

	return _view;
}

auto Sorcery::CharacterDisplay::_get_spell_icon(SPC category)
	-> std::optional<sf::Sprite> {

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
	_hl_action = MIM::NO_MENU_ITEM;
	_generate_display();
}

auto Sorcery::CharacterDisplay::check_for_inventory_mouse_move(
	sf::Vector2f mouse_pos) -> unsigned int {

	const auto global_pos{this->getPosition()};
	const auto local_pos{mouse_pos - global_pos};

	if (_view == CHV::SUMMARY) {

		// Check for Mouse Over on Inventory Slots
		auto it{std::find_if(inv_bounds.begin(), inv_bounds.end(),
			[&local_pos](
				const auto &item) { return item.second.contains(local_pos); })};
		if (it != inv_bounds.end()) {
			_hl_inv = (*it).first;
			return (*it).first;
		} else {
			_hl_inv = 0;
			return 0;
		}

	} else
		return 0;
}

auto Sorcery::CharacterDisplay::check_for_action_mouse_move(
	sf::Vector2f mouse_pos) -> std::optional<MIM> {

	const auto global_pos{this->getPosition()};
	const auto local_pos{mouse_pos - global_pos};

	if (_view == CHV::SUMMARY) {

		// Check for Mouse Over on Summary Action Items
		auto it{std::find_if(action_bounds.begin(), action_bounds.end(),
			[&local_pos](
				const auto &item) { return item.second.contains(local_pos); })};
		if (it != action_bounds.end()) {
			_hl_action = (*it).first;
			return (*it).first;
		} else {
			_hl_action = MIM::NO_MENU_ITEM;
			return std::nullopt;
		}
	}

	else
		return std::nullopt;
}

auto Sorcery::CharacterDisplay::check_for_mouse_move(sf::Vector2f mouse_pos)
	-> std::optional<SPI> {

	const auto global_pos{this->getPosition()};
	const auto local_pos{mouse_pos - global_pos};
	if (_view == CHV::MAGE_SPELLS) {

		auto it{std::find_if(mage_bounds.begin(), mage_bounds.end(),
			[&local_pos](
				const auto &item) { return item.second.contains(local_pos); })};
		if (it != mage_bounds.end()) {
			Component name_c{
				(*_display->layout)["character_mage_spells:spell_name_label"]};
			_hl_mage = (*it).first;

			std::vector<Spell>::iterator sit;
			sit = std::find_if(_character->spells().begin(),
				_character->spells().end(),
				[&](auto item) { return item.id == _hl_mage; });
			if (sit != _character->spells().end())
				_spell_panel->set(*sit);

			return (*it).first;
		} else
			return std::nullopt;

	} else if (_view == CHV::PRIEST_SPELLS) {
		auto it{std::find_if(priest_bounds.begin(), priest_bounds.end(),
			[&local_pos](
				const auto &item) { return item.second.contains(local_pos); })};
		if (it != priest_bounds.end()) {
			Component name_c{(
				*_display->layout)["character_priest_spells:spell_name_label"]};
			_hl_priest = (*it).first;

			std::vector<Spell>::iterator sit;
			sit = std::find_if(_character->spells().begin(),
				_character->spells().end(),
				[&](auto item) { return item.id == _hl_priest; });
			if (sit != _character->spells().end())
				_spell_panel->set(*sit);

			return (*it).first;
		} else
			return std::nullopt;
	} else
		return std::nullopt;
}

auto Sorcery::CharacterDisplay::update() -> void {

	_hl_mage_bg.setFillColor(_graphics->animation->select_col);
	_hl_priest_bg.setFillColor(_graphics->animation->select_col);
	_hl_action_bg.setFillColor(_graphics->animation->select_col);

	//_inventory_display->update();
}

auto Sorcery::CharacterDisplay::draw(
	sf::RenderTarget &target, sf::RenderStates states) const -> void {

	states.transform *= getTransform();

	// Draw the common components
	for (const auto &[unique_key, frame] : _frames)
		target.draw(*frame, states);

	for (const auto &[unique_key, sprite] : _sprites)
		target.draw(sprite, states);

	for (const auto &[unique_key, text] : _texts)
		target.draw(text, states);

	if (_view == CHV::MAGE_SPELLS)
		target.draw(_hl_mage_bg, states);
	else if (_view == CHV::PRIEST_SPELLS)
		target.draw(_hl_priest_bg, states);

	// Draw the section components
	for (const auto &[unique_key, v_frame] : _v_frames)
		target.draw(*v_frame, states);

	for (const auto &[unique_key, v_sprite] : _v_sprites)
		target.draw(v_sprite, states);

	if (_view == CHV::SUMMARY && _hl_action != MIM::NO_MENU_ITEM)
		target.draw(_hl_action_bg, states);

	if (_view == CHV::SUMMARY && _hl_inv != 0)
		target.draw(_hl_inv_bg, states);

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
	mage_bounds.clear();
	priest_bounds.clear();
	mage_texts.clear();
	priest_texts.clear();
	action_bounds.clear();
	action_texts.clear();
	inv_bounds.clear();
	inv_texts.clear();

	_display->generate("character", _sprites, _texts, _frames);

	if (_view == CHV::SUMMARY) {

		_display->generate(
			"character_summary", _v_sprites, _v_texts, _v_frames);

		_add_text(
			(*_display->layout)["character_summary:name_and_summary_text"],
			"{}", _character->summary_text());

		Component s_c{(*_display->layout)["character_summary:strength_value"]};
		s_c.colour = _graphics->adjust_colour(
			_character->attributes().at(CAR::STRENGTH), CAT::STAT);
		_add_text(s_c, "{:>2}",
			std::to_string(_character->attributes().at(CAR::STRENGTH)));

		Component i_c{(*_display->layout)["character_summary:iq_value"]};
		i_c.colour = _graphics->adjust_colour(
			_character->attributes().at(CAR::IQ), CAT::STAT);
		_add_text(
			i_c, "{:>2}", std::to_string(_character->attributes().at(CAR::IQ)));

		Component p_c{(*_display->layout)["character_summary:piety_value"]};
		p_c.colour = _graphics->adjust_colour(
			_character->attributes().at(CAR::PIETY), CAT::STAT);
		_add_text(p_c, "{:>2}",
			std::to_string(_character->attributes().at(CAR::PIETY)));

		Component a_c{(*_display->layout)["character_summary:agility_value"]};
		a_c.colour = _graphics->adjust_colour(
			_character->attributes().at(CAR::AGILITY), CAT::STAT);
		_add_text(a_c, "{:>2}",
			std::to_string(_character->attributes().at(CAR::AGILITY)));

		Component v_c{(*_display->layout)["character_summary:vitality_value"]};
		v_c.colour = _graphics->adjust_colour(
			_character->attributes().at(CAR::VITALITY), CAT::STAT);
		_add_text(v_c, "{:>2}",
			std::to_string(_character->attributes().at(CAR::VITALITY)));

		Component l_c{(*_display->layout)["character_summary:luck_value"]};
		l_c.colour = _graphics->adjust_colour(
			_character->attributes().at(CAR::LUCK), CAT::STAT);
		_add_text(l_c, "{:>2}",
			std::to_string(_character->attributes().at(CAR::LUCK)));

		Component inv_c{(*_display->layout)["global:inventory_display"]};
		_generate_inventory(inv_c);

		_add_text((*_display->layout)["character_summary:hp_value"], "{}",
			fmt::format("{}/{}",
				std::to_string(_character->abilities().at(CAB::CURRENT_HP)),
				std::to_string(_character->abilities().at(CAB::MAX_HP))));
		_add_text((*_display->layout)["character_summary:ac_value"], "{:>3}",
			std::to_string(_character->get_cur_ac()));
		_add_text((*_display->layout)["character_summary:age_value"], "{:>2}",
			std::to_string(
				static_cast<int>(_character->abilities().at(CAB::AGE) / 52)));
		_add_text((*_display->layout)["character_summary:swim_value"], "{:>2}",
			std::to_string(_character->abilities().at(CAB::SWIM)));
		auto status{
			_add_text((*_display->layout)["character_summary:status_value"],
				"{}", _character->get_status_string())};
		status->setFillColor(sf::Color(_graphics->adjust_status_colour(
			_character->get_status(), _character->is_poisoned())));

		_add_text((*_display->layout)["character_summary:exp_value"], "{:>11}",
			std::to_string(_character->abilities().at(CAB::CURRENT_XP)));
		_add_text((*_display->layout)["character_summary:next_value"], "{:>11}",
			std::to_string(_character->abilities().at(CAB::NEXT_LEVEL_XP)));
		_add_text((*_display->layout)["character_summary:gold_value"], "{:>11}",
			std::to_string(_character->abilities().at(CAB::GOLD)));
		_add_text((*_display->layout)["character_summary:marks_value"],
			"{:>11}", std::to_string(_character->abilities().at(CAB::MARKS)));
		_add_text((*_display->layout)["character_summary:deaths_value"],
			"{:>2}", std::to_string(_character->abilities().at(CAB::DEATHS)));

		auto mage{fmt::format("{}/{}/{}/{}/{}/{}/{}",
			_character->mage_cur_sp().at(1), _character->mage_cur_sp().at(2),
			_character->mage_cur_sp().at(3), _character->mage_cur_sp().at(4),
			_character->mage_cur_sp().at(5), _character->mage_cur_sp().at(6),
			_character->mage_cur_sp().at(7))};
		auto priest{fmt::format("{}/{}/{}/{}/{}/{}/{}",
			_character->priest_cur_sp().at(1),
			_character->priest_cur_sp().at(2),
			_character->priest_cur_sp().at(3),
			_character->priest_cur_sp().at(4),
			_character->priest_cur_sp().at(5),
			_character->priest_cur_sp().at(6),
			_character->priest_cur_sp().at(7))};

		_add_text(
			(*_display->layout)["character_summary:mage_spells"], "{}", mage);
		_add_text((*_display->layout)["character_summary:priest_spells"], "{}",
			priest);

		Component act_c{(*_display->layout)["character_summary:action_panel"]};

		const auto action_x{act_c.x};
		const auto action_y{act_c.y};
		const auto sm_off_x{std::stoi(act_c["offset_x_small"].value())};
		const auto lg_off_x{std::stoi(act_c["offset_x_big"].value())};

		_add_action_button(
			act_c, MIM::C_ACTION_READ, "{:5}", "C_ACTION_READ", true);
		act_c.y += _display->window->get_ch();
		_add_action_button(act_c, MIM::C_ACTION_EQUIP, "{:5}", "C_ACTION_EQUIP",
			_mode == CHM::AT_CASTLE || _mode == CHM::IN_MAZE);
		act_c.x = action_x + (sm_off_x * _display->window->get_cw());
		act_c.y = action_y;
		_add_action_button(act_c, MIM::C_ACTION_TRADE, "{:5}", "C_ACTION_TRADE",
			_mode == CHM::AT_CASTLE || _mode == CHM::IN_MAZE);
		act_c.y += _display->window->get_ch();
		_add_action_button(act_c, MIM::C_ACTION_DROP, "{:5}", "C_ACTION_DROP",
			_mode == CHM::AT_CASTLE || _mode == CHM::IN_MAZE);
		act_c.x = act_c.x + (sm_off_x * _display->window->get_cw());
		act_c.y = action_y;
		_add_action_button(act_c, MIM::C_ACTION_POOL, "{:9}", "C_ACTION_POOL",
			_mode == CHM::AT_CASTLE || _mode == CHM::IN_MAZE);
		act_c.y += _display->window->get_ch();
		_add_action_button(act_c, MIM::C_ACTION_IDENTIFY, "{:9}",
			"C_ACTION_IDENTIFY", _mode == CHM::IN_MAZE);
		act_c.x = act_c.x + (lg_off_x * _display->window->get_cw());
		act_c.y = action_y;
		_add_action_button(act_c, MIM::C_ACTION_SPELL, "{:5}", "C_ACTION_SPELL",
			_mode == CHM::IN_MAZE);
		act_c.y += _display->window->get_ch();
		_add_action_button(act_c, MIM::C_ACTION_USE, "{:5}", "C_ACTION_USE",
			_mode == CHM::IN_MAZE);
		act_c.y += _display->window->get_ch();
		act_c.x = act_c.x + (sm_off_x * _display->window->get_cw());
		act_c.y = action_y;
		_add_action_button(act_c, MIM::C_ACTION_INVOKE, "{:6}",
			"C_ACTION_INVOKE",
			_mode == CHM::AT_CASTLE || _mode == CHM::IN_MAZE);
		act_c.y += _display->window->get_ch();
		_add_action_button(
			act_c, MIM::C_ACTION_LEAVE, "{:6}", "C_ACTION_LEAVE", true);

	} else if (_view == CHV::DETAILED) {

		_display->generate(
			"character_detailed", _v_sprites, _v_texts, _v_frames);

		_add_text(
			(*_display->layout)["character_detailed:name_and_summary_text"],
			"{}", _character->summary_text());

		Component s_c{(*_display->layout)["character_detailed:strength_value"]};
		s_c.colour = _graphics->adjust_colour(
			_character->attributes().at(CAR::STRENGTH), CAT::STAT);
		_add_text(s_c, "{:>2}",
			std::to_string(_character->attributes().at(CAR::STRENGTH)));

		Component i_c{(*_display->layout)["character_detailed:iq_value"]};
		i_c.colour = _graphics->adjust_colour(
			_character->attributes().at(CAR::IQ), CAT::STAT);
		_add_text(
			i_c, "{:>2}", std::to_string(_character->attributes().at(CAR::IQ)));

		Component p_c{(*_display->layout)["character_detailed:piety_value"]};
		p_c.colour = _graphics->adjust_colour(
			_character->attributes().at(CAR::PIETY), CAT::STAT);
		_add_text(p_c, "{:>2}",
			std::to_string(_character->attributes().at(CAR::PIETY)));

		Component a_c{(*_display->layout)["character_detailed:agility_value"]};
		a_c.colour = _graphics->adjust_colour(
			_character->attributes().at(CAR::AGILITY), CAT::STAT);
		_add_text(a_c, "{:>2}",
			std::to_string(_character->attributes().at(CAR::AGILITY)));

		Component v_c{(*_display->layout)["character_detailed:vitality_value"]};
		v_c.colour = _graphics->adjust_colour(
			_character->attributes().at(CAR::VITALITY), CAT::STAT);
		_add_text(v_c, "{:>2}",
			std::to_string(_character->attributes().at(CAR::VITALITY)));

		Component l_c{(*_display->layout)["character_detailed:luck_value"]};
		l_c.colour = _graphics->adjust_colour(
			_character->attributes().at(CAR::LUCK), CAT::STAT);
		_add_text(l_c, "{:>2}",
			std::to_string(_character->attributes().at(CAR::LUCK)));

		Component str_c(
			(*_display->layout)["character_detailed:strength_detailed_values"]);

		str_c.colour = _graphics->adjust_colour(
			_character->abilities().at(CAB::ATTACK_MODIFIER), CAT::MODIFIER);
		_add_text(str_c, "{:+>2}",
			std::to_string(_character->abilities().at(CAB::ATTACK_MODIFIER)));

		str_c.y += _display->window->get_ch();
		str_c.colour = _graphics->adjust_colour(
			_character->abilities().at(CAB::HIT_PROBABILITY), CAT::MODIFIER);
		_add_text(str_c, "{:+>2}",
			std::to_string(_character->abilities().at(CAB::HIT_PROBABILITY)));

		str_c.y += _display->window->get_ch();
		str_c.colour = _graphics->adjust_colour(
			_character->abilities().at(CAB::BONUS_DAMAGE), CAT::MODIFIER);
		_add_text(str_c, "{:+>2}",
			std::to_string(_character->abilities().at(CAB::BONUS_DAMAGE)));

		str_c.y += _display->window->get_ch();
		str_c.colour = _graphics->adjust_colour(
			_character->abilities().at(CAB::BASE_NUMBER_OF_ATTACKS),
			CAT::NUMBER);
		_add_text(str_c, "{:>2}",
			std::to_string(
				_character->abilities().at(CAB::BASE_NUMBER_OF_ATTACKS)));

		str_c.y += _display->window->get_ch();
		str_c.colour = _graphics->adjust_colour(
			_character->abilities().at(CAB::UNARMED_DAMAGE), CAT::NUMBER);
		_add_text(str_c, "{:>2}",
			std::to_string(_character->abilities().at(CAB::UNARMED_DAMAGE)));

		Component iq_c(
			(*_display->layout)["character_detailed:iq_detailed_values"]);

		iq_c.colour = _graphics->adjust_colour(
			_character->abilities().at(CAB::MAGE_SPELL_LEARN), CAT::PERCENTAGE);
		_add_text(iq_c, "{:>2}%",
			std::to_string(_character->abilities().at(CAB::MAGE_SPELL_LEARN)));

		iq_c.y += _display->window->get_ch();
		iq_c.colour = _graphics->adjust_colour(
			_character->abilities().at(CAB::IDENTIFY_ITEMS), CAT::PERCENTAGE);
		_add_text(iq_c, "{:>2}%",
			std::to_string(_character->abilities().at(CAB::IDENTIFY_ITEMS)));

		iq_c.y += _display->window->get_ch();
		iq_c.colour = _graphics->adjust_colour(
			_character->abilities().at(CAB::IDENTIFY_CURSE), CAT::PERCENTAGE);
		_add_text(iq_c, "{:>2}%",
			std::to_string(_character->abilities().at(CAB::IDENTIFY_CURSE)));

		iq_c.y += _display->window->get_ch();
		iq_c.colour = _graphics->adjust_colour(
			_character->abilities().at(CAB::IDENTIFY_FOES), CAT::PERCENTAGE);
		_add_text(iq_c, "{:>2}%",
			std::to_string(_character->abilities().at(CAB::IDENTIFY_FOES)));

		iq_c.y += _display->window->get_ch();
		iq_c.colour = _graphics->adjust_colour(
			_character->abilities().at(CAB::BONUS_MAGE_SPELLS), CAT::NUMBER);
		_add_text(iq_c, "{:>2}",
			std::to_string(_character->abilities().at(CAB::BONUS_MAGE_SPELLS)));

		Component pie_c(
			(*_display->layout)["character_detailed:piety_detailed_values"]);
		pie_c.colour = _graphics->adjust_colour(
			_character->abilities().at(CAB::PRIEST_SPELL_LEARN),
			CAT::PERCENTAGE);
		_add_text(pie_c, "{:>2}%",
			std::to_string(
				_character->abilities().at(CAB::PRIEST_SPELL_LEARN)));

		pie_c.y += _display->window->get_ch();
		pie_c.colour = _graphics->adjust_colour(
			_character->abilities().at(CAB::LOKTOFELT_SUCCESS),
			CAT::PERCENTAGE);
		_add_text(pie_c, "{:>2}%",
			std::to_string(_character->abilities().at(CAB::LOKTOFELT_SUCCESS)));

		pie_c.y += _display->window->get_ch();
		pie_c.colour = _graphics->adjust_colour(
			_character->abilities().at(CAB::BASE_DISPELL), CAT::PERCENTAGE);
		_add_text(pie_c, "{:>2}%",
			std::to_string(_character->abilities().at(CAB::BASE_DISPELL)));

		pie_c.y += _display->window->get_ch();
		pie_c.colour = _graphics->adjust_colour(
			_character->abilities().at(CAB::BONUS_PRIEST_SPELLS), CAT::NUMBER);
		_add_text(pie_c, "{:>2}",
			std::to_string(
				_character->abilities().at(CAB::BONUS_PRIEST_SPELLS)));

		Component vit_c(
			(*_display->layout)["character_detailed:vitality_detailed_values"]);
		vit_c.colour = _graphics->adjust_colour(
			_character->abilities().at(CAB::VITALITY_BONUS), CAT::MODIFIER);
		_add_text(vit_c, "{:+>2}",
			std::to_string(_character->abilities().at(CAB::VITALITY_BONUS)));

		vit_c.y += _display->window->get_ch();
		vit_c.colour = _graphics->adjust_colour(
			_character->abilities().at(CAB::BONUS_HIT_POINTS), CAT::MODIFIER);
		_add_text(vit_c, "{:+>2}",
			std::to_string(_character->abilities().at(CAB::BONUS_HIT_POINTS)));

		vit_c.y += _display->window->get_ch();
		vit_c.colour = _graphics->adjust_colour(
			_character->abilities().at(CAB::DEAD_RESURRECT), CAT::PERCENTAGE);
		_add_text(vit_c, "{:>2}%",
			std::to_string(_character->abilities().at(CAB::DEAD_RESURRECT)));

		vit_c.y += _display->window->get_ch();
		vit_c.colour = _graphics->adjust_colour(
			_character->abilities().at(CAB::ASHES_RESURRECT), CAT::PERCENTAGE);
		_add_text(vit_c, "{:>2}%",
			std::to_string(_character->abilities().at(CAB::ASHES_RESURRECT)));

		vit_c.y += _display->window->get_ch();
		vit_c.colour = _graphics->adjust_colour(
			_character->abilities().at(CAB::DI_KADORTO_RESURRECT),
			CAT::PERCENTAGE);
		_add_text(vit_c, "{:>2}%",
			std::to_string(
				_character->abilities().at(CAB::DI_KADORTO_RESURRECT)));

		Component agi_c(
			(*_display->layout)["character_detailed:agility_detailed_values"]);

		agi_c.colour = _graphics->adjust_colour(
			_character->abilities().at(CAB::INITIATIVE_MODIFIER),
			CAT::MODIFIER);
		_add_text(agi_c, "{:+>2}",
			std::to_string(
				_character->abilities().at(CAB::INITIATIVE_MODIFIER)));

		agi_c.y += _display->window->get_ch();
		agi_c.colour = _graphics->adjust_colour(
			_character->abilities().at(CAB::BASE_CRITICAL_HIT),
			CAT::PERCENTAGE);
		_add_text(agi_c, "{:>2}%",
			std::to_string(_character->abilities().at(CAB::BASE_CRITICAL_HIT)));

		agi_c.y += _display->window->get_ch();
		agi_c.colour = _graphics->adjust_colour(
			_character->abilities().at(CAB::IDENTIFY_TRAP), CAT::PERCENTAGE);
		_add_text(agi_c, "{:>2}%",
			std::to_string(_character->abilities().at(CAB::IDENTIFY_TRAP)));

		agi_c.y += _display->window->get_ch();
		agi_c.colour = _graphics->adjust_colour(
			_character->abilities().at(CAB::BASE_DISARM_TRAP), CAT::PERCENTAGE);
		_add_text(agi_c, "{:>2}%",
			std::to_string(_character->abilities().at(CAB::BASE_DISARM_TRAP)));

		agi_c.y += _display->window->get_ch();
		agi_c.colour = _graphics->adjust_colour(
			(100 - _character->abilities().at(CAB::ACTIVATE_TRAP)),
			CAT::PERCENTAGE);
		_add_text(agi_c, "{:>2}%",
			std::to_string(
				100 - _character->abilities().at(CAB::ACTIVATE_TRAP)));

		agi_c.y += _display->window->get_ch();
		agi_c.colour = _graphics->adjust_colour(
			_character->abilities().at(CAB::BASE_AVOID_PIT), CAT::PERCENTAGE);
		_add_text(agi_c, "{:>2}%",
			std::to_string(_character->abilities().at(CAB::BASE_AVOID_PIT)));

		agi_c.y += _display->window->get_ch();
		agi_c.colour = _graphics->adjust_colour(
			_character->abilities().at(CAB::BASE_ARMOUR_CLASS), CAT::AC);
		_add_text(agi_c, "{:>2}",
			std::to_string(_character->abilities().at(CAB::BASE_ARMOUR_CLASS)));

		Component luc_c(
			(*_display->layout)["character_detailed:luck_detailed_values"]);

		luc_c.colour = _graphics->adjust_colour(
			_character->abilities().at(CAB::BASE_RESIST_BONUS) * 5,
			CAT::PERCENTAGE);
		_add_text(luc_c, "{:>2}%",
			std::to_string(
				_character->abilities().at(CAB::BASE_RESIST_BONUS) * 5));

		luc_c.y += _display->window->get_ch();
		luc_c.colour = _graphics->adjust_colour(
			_character->abilities().at(CAB::EQUIPMENT_INTACT_ON_WIPE),
			CAT::PERCENTAGE);
		_add_text(luc_c, "{:>2}%",
			std::to_string(
				_character->abilities().at(CAB::EQUIPMENT_INTACT_ON_WIPE)));

	} else if (_view == CHV::RESISTANCES) {

		_display->generate(
			"character_resistances", _v_sprites, _v_texts, _v_frames);

		_add_text(
			(*_display->layout)["character_resistances:name_and_summary_text"],
			"{}", _character->summary_text());

		Component res_c((*_display
				->layout)["character_resistances:resistances_detailed_values"]);
		res_c.colour = _graphics->adjust_colour(
			_character->abilities().at(CAB::RESISTANCE_VS_CRITICAL_HIT) * 5,
			CAT::PERCENTAGE);
		_add_text(res_c, "{:>2}%",
			std::to_string(
				_character->abilities().at(CAB::RESISTANCE_VS_CRITICAL_HIT) *
				5));

		res_c.y += _display->window->get_ch();
		res_c.colour = _graphics->adjust_colour(
			_character->abilities().at(CAB::RESISTANCE_VS_POISON_PARALYSIS) * 5,
			CAT::PERCENTAGE);
		_add_text(res_c, "{:>2}%",
			std::to_string(_character->abilities().at(
							   CAB::RESISTANCE_VS_POISON_PARALYSIS) *
						   5));

		res_c.y += _display->window->get_ch();
		res_c.colour = _graphics->adjust_colour(
			_character->abilities().at(CAB::RESISTANCE_VS_STONING) * 5,
			CAT::PERCENTAGE);
		_add_text(res_c, "{:>2}%",
			std::to_string(
				_character->abilities().at(CAB::RESISTANCE_VS_STONING) * 5));

		res_c.y += _display->window->get_ch();
		res_c.colour = _graphics->adjust_colour(
			_character->abilities().at(CAB::RESISTANCE_VS_BREATH_ATTACKS) * 5,
			CAT::PERCENTAGE);
		_add_text(res_c, "{:>2}%",
			std::to_string(
				_character->abilities().at(CAB::RESISTANCE_VS_BREATH_ATTACKS) *
				5));

		res_c.y += _display->window->get_ch();
		res_c.colour = _graphics->adjust_colour(
			_character->abilities().at(CAB::RESISTANCE_VS_POISON_GAS_TRAP) * 5,
			CAT::PERCENTAGE);
		_add_text(res_c, "{:>2}%",
			std::to_string(
				_character->abilities().at(CAB::RESISTANCE_VS_POISON_GAS_TRAP) *
				5));

		res_c.y += _display->window->get_ch();
		res_c.colour = _graphics->adjust_colour(
			_character->abilities().at(CAB::RESISTANCE_VS_MAGE_PRIEST_TRAP) * 5,
			CAT::PERCENTAGE);
		_add_text(res_c, "{:>2}%",
			std::to_string(_character->abilities().at(
							   CAB::RESISTANCE_VS_MAGE_PRIEST_TRAP) *
						   5));

		res_c.y += _display->window->get_ch();
		res_c.colour = _graphics->adjust_colour(
			_character->abilities().at(CAB::RECOVER_FROM_SLEEP),
			CAT::PERCENTAGE);
		_add_text(res_c, "{:>2}%",
			std::to_string(
				_character->abilities().at(CAB::RECOVER_FROM_SLEEP)));

		res_c.y += _display->window->get_ch();
		res_c.colour = _graphics->adjust_colour(
			_character->abilities().at(CAB::RECOVER_FROM_FEAR),
			CAT::PERCENTAGE);
		_add_text(res_c, "{:>2}%",
			std::to_string(_character->abilities().at(CAB::RECOVER_FROM_FEAR)));

		res_c.y += _display->window->get_ch();
		res_c.colour = _graphics->adjust_colour(
			_character->abilities().at(CAB::RESISTANCE_VS_SILENCE) * 5,
			CAT::PERCENTAGE);
		_add_text(res_c, "{:>2}%",
			std::to_string(
				_character->abilities().at(CAB::RESISTANCE_VS_SILENCE) * 5));

		res_c.y += _display->window->get_ch();
		res_c.colour = _graphics->adjust_colour(
			_character->abilities().at(CAB::RESISTANCE_VS_KATINO),
			CAT::PERCENTAGE);
		_add_text(res_c, "{:>2}%",
			std::to_string(
				_character->abilities().at(CAB::RESISTANCE_VS_KATINO)));

		res_c.y += _display->window->get_ch();
		res_c.colour = _graphics->adjust_colour(
			_character->abilities().at(CAB::RESISTANCE_VS_BADI),
			CAT::PERCENTAGE);
		_add_text(res_c, "{:>2}%",
			std::to_string(
				_character->abilities().at(CAB::RESISTANCE_VS_BADI)));

		res_c.y += _display->window->get_ch();
		res_c.colour = _graphics->adjust_colour(
			_character->abilities().at(CAB::RESISTANCE_VS_MANIFO),
			CAT::PERCENTAGE);
		_add_text(res_c, "{:>2}%",
			std::to_string(
				_character->abilities().at(CAB::RESISTANCE_VS_MANIFO)));

	} else if (_view == CHV::MAGE_SPELLS) {

		_display->generate(
			"character_mage_spells", _v_sprites, _v_texts, _v_frames);

		_add_text(
			(*_display->layout)["character_mage_spells:name_and_summary_text"],
			"{}", _character->summary_text());

		Component level_c{
			(*_display->layout)["character_mage_spells:level_label"]};
		Component sp_c{
			(*_display->layout)["character_mage_spells:spell_points"]};
		Component name_c{
			(*_display->layout)["character_mage_spells:spell_name_label"]};

		auto level_x{level_c.x};
		for (auto level = 1; level <= 7; level++) {

			sp_c.x = level_c.x + (std::stoi(sp_c["offset_columns"].value()) *
									 _display->window->get_cw());
			sp_c.y = level_c.y;

			name_c.x = level_c.x;
			name_c.y = level_c.y + _display->window->get_ch();

			auto spells{
				_character->spells() | std::views::filter([&](Spell spell) {
					return (spell.type == SPT::MAGE) &&
						   (static_cast<int>(spell.level) == level);
				})};
			for (auto spell : spells) {

				// Add the Spell
				auto str{
					PADSTR(spell.name, std::stoi(name_c["bar_width"].value()))};
				auto name{_add_text(name_c, "{}", str)};
				auto hl_bounds{name->getGlobalBounds()};
				mage_bounds[spell.id] = hl_bounds;
				mage_texts[spell.id] = name;

				if (spell.id == _hl_mage) {
					sf::RectangleShape bg(
						sf::Vector2f(std::stoi(name_c["bar_width"].value()) *
										 _display->window->get_cw(),
							hl_bounds.height));
					bg.setPosition(hl_bounds.left, hl_bounds.top);
					bg.setFillColor(_graphics->animation->select_col);
					name->setFillColor(sf::Color(name_c.colour));
					name->setOutlineColor(sf::Color(0, 0, 0));
					name->setOutlineThickness(2);
					_hl_mage_bg = bg;
				} else {
					if (spell.known)
						name->setFillColor(sf::Color(std::stoull(
							name_c["known_colour"].value(), 0, 16)));
					else
						name->setFillColor(sf::Color(std::stoull(
							name_c["unknown_colour"].value(), 0, 16)));
				}

				// And the Spell Category Icon
				Component icon_c{
					(*_display->layout)["character_mage_spells:spell_icon"]};
				auto icon{_get_spell_icon(spell.category)};
				if (icon) {
					icon.value().setScale(icon_c.scl());
					const auto off_x{std::invoke([&] {
						if (icon_c["offset_x"])
							return std::stoi(icon_c["offset_x"].value());
						else
							return 0;
					})};
					const auto off_y{std::invoke([&] {
						if (icon_c["offset_y"])
							return std::stoi(icon_c["offset_y"].value());
						else
							return 0;
					})};

					icon.value().setPosition(
						name_c.x + off_x +
							(std::stoi(icon_c["offset_columns"].value()) *
								_display->window->get_cw()),
						name_c.y + off_y);

					if (spell.known)
						icon->setColor(sf::Color(std::stoull(
							name_c["known_colour"].value(), 0, 16)));
					else
						icon->setColor(sf::Color(std::stoull(
							name_c["unknown_colour"].value(), 0, 16)));

					_v_sprites.emplace(GUID(), icon.value());
				}

				name_c.y += _display->window->get_ch();
			}

			if (level % 3 == 1 || level % 3 == 2)
				level_c.x += (std::stoi(level_c["offset_columns"].value()) *
							  _display->window->get_cw());
			else if (level % 3 == 0) {
				level_c.x = level_x;
				level_c.y += (std::stoi(level_c["offset_rows"].value()) *
							  _display->window->get_ch());
			}
		}
	} else if (_view == CHV::PRIEST_SPELLS) {

		_display->generate(
			"character_priest_spells", _v_sprites, _v_texts, _v_frames);

		_add_text(
			(*_display
					->layout)["character_priest_spells:name_and_summary_text"],
			"{}", _character->summary_text());

		Component level_c{
			(*_display->layout)["character_priest_spells:level_label"]};
		Component sp_c{
			(*_display->layout)["character_priest_spells:spell_points"]};
		Component name_c{
			(*_display->layout)["character_priest_spells:spell_name_label"]};

		auto level_x{level_c.x};
		for (auto level = 1; level <= 7; level++) {

			sp_c.x = level_c.x + (std::stoi(sp_c["offset_columns"].value()) *
									 _display->window->get_cw());
			sp_c.y = level_c.y;
			name_c.x = level_c.x;
			name_c.y = level_c.y + _display->window->get_ch();

			auto spells{
				_character->spells() | std::views::filter([&](Spell spell) {
					return (spell.type == SPT::PRIEST) &&
						   (static_cast<int>(spell.level) == level);
				})};
			for (auto spell : spells) {

				auto str{
					PADSTR(spell.name, std::stoi(name_c["bar_width"].value()))};
				auto spell_name{_add_text(name_c, "{}", str)};
				spell_name->setPosition(name_c.x, name_c.y);
				auto hl_bounds{spell_name->getGlobalBounds()};
				priest_bounds[spell.id] = hl_bounds;
				priest_texts[spell.id] = spell_name;

				if (spell.id == _hl_priest) {
					sf::RectangleShape bg(
						sf::Vector2f(std::stoi(name_c["bar_width"].value()) *
										 _display->window->get_cw(),
							hl_bounds.height));
					bg.setPosition(hl_bounds.left, hl_bounds.top);
					bg.setFillColor(_graphics->animation->select_col);
					spell_name->setFillColor(sf::Color(name_c.colour));
					spell_name->setOutlineColor(sf::Color(0, 0, 0));
					spell_name->setOutlineThickness(2);
					_hl_priest_bg = bg;
				} else {
					if (spell.known)
						spell_name->setFillColor(sf::Color(std::stoull(
							name_c["known_colour"].value(), 0, 16)));
					else
						spell_name->setFillColor(sf::Color(std::stoull(
							name_c["unknown_colour"].value(), 0, 16)));
				}

				// And the Spell Category Icon
				Component icon_c{
					(*_display->layout)["character_mage_spells:spell_icon"]};
				if (auto icon{_get_spell_icon(spell.category)}; icon) {
					icon.value().setScale(icon_c.scl());
					const auto off_x{std::invoke([&] {
						if (icon_c["offset_x"])
							return std::stoi(icon_c["offset_x"].value());
						else
							return 0;
					})};
					const auto off_y{std::invoke([&] {
						if (icon_c["offset_y"])
							return std::stoi(icon_c["offset_y"].value());
						else
							return 0;
					})};

					icon.value().setPosition(
						name_c.x + off_x +
							(std::stoi(icon_c["offset_columns"].value()) *
								_display->window->get_cw()),
						name_c.y + off_y);

					if (spell.known)
						icon->setColor(sf::Color(std::stoull(
							name_c["known_colour"].value(), 0, 16)));
					else
						icon->setColor(sf::Color(std::stoull(
							name_c["unknown_colour"].value(), 0, 16)));

					_v_sprites.emplace(GUID(), icon.value());
				}

				name_c.y += _display->window->get_ch();
			}

			if (level % 3 == 1 || level % 3 == 2)
				level_c.x += (std::stoi(level_c["offset_columns"].value()) *
							  _display->window->get_cw());
			else if (level % 3 == 0) {
				level_c.x = level_x;
				level_c.y += (std::stoi(level_c["offset_rows"].value()) *
							  _display->window->get_ch());
			}
		}
	}
}

auto Sorcery::CharacterDisplay::get_hl_action_item() const -> MIM {

	return _hl_action;
}

auto Sorcery::CharacterDisplay::get_inventory_item() const -> unsigned int {

	return _hl_inv;
}

auto Sorcery::CharacterDisplay::_add_action_button(Component layout_c,
	const MIM item, const std::string format, const std::string str,
	const bool enabled) -> void {

	auto text{_add_text(layout_c, format, (*_display->string)[str])};
	action_texts[item] = text;
	if (enabled) {
		auto bounds{text->getGlobalBounds()};
		action_bounds[item] = bounds;
		if (_hl_action == item) {
			sf::RectangleShape bg(sf::Vector2f(
				(*_display->string)[str].length() * _display->window->get_cw(),
				bounds.height));
			bg.setPosition(bounds.left, bounds.top);
			bg.setFillColor(_graphics->animation->select_col);
			text->setFillColor(sf::Color(layout_c.colour));
			text->setOutlineColor(sf::Color(0, 0, 0));
			text->setOutlineThickness(2);
			_hl_action_bg = bg;
		} else
			text->setFillColor(sf::Color(
				std::stoull(layout_c["enabled_colour"].value(), nullptr, 16)));
	} else
		text->setFillColor(sf::Color(
			std::stoull(layout_c["disabled_colour"].value(), nullptr, 16)));
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
