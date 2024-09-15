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

#include "modules/create.hpp"
#include "common/enum.hpp"
#include "common/macro.hpp"
#include "core/animation.hpp"
#include "core/display.hpp"
#include "core/game.hpp"
#include "core/graphics.hpp"
#include "core/input.hpp"
#include "core/system.hpp"
#include "core/window.hpp"
#include "modules/define.hpp"
#include "resources/componentstore.hpp"
#include "resources/itemstore.hpp"
#include "resources/resourcemanager.hpp"
#include "resources/texturestore.hpp"
#include "types/component.hpp"
#include "types/enum.hpp"
#include "types/inventory.hpp"

// Standard Constructor
Sorcery::Create::Create(System *system, Display *display, Graphics *graphics, Game *game)
	: _system{system}, _display{display}, _graphics{graphics}, _game{game} {
}

auto Sorcery::Create::_generate_display() -> void {

	_reset_components();
	_initalise_components();
	_display->generate("create");
	_place_components();
}

auto Sorcery::Create::_refresh_display() -> void {

	_window->clear();

	if (_display->layout->refresh_if_needed())
		_generate_display();

	_draw();
	_window->display();
}

auto Sorcery::Create::_reset_components() -> void {

	if (_keyboard.get())
		_keyboard.reset();
	if (_ip.get())
		_ip.reset();
	if (_ap.get())
		_ap.reset();
	if (_method_menu.get())
		_method_menu.reset();
	if (_race_menu.get())
		_race_menu.reset();
	if (_alignment_menu.get())
		_alignment_menu.reset();
	if (_attribute_menu.get())
		_attribute_menu.reset();
	if (_class_menu.get())
		_class_menu.reset();
	if (_final_menu_frame.get())
		_final_menu_frame.reset();
	if (_dialog_saved_ok.get())
		_dialog_saved_ok.reset();
	if (_character_display.get())
		_character_display.reset();
}

auto Sorcery::Create::_initalise_components() -> void {

	// Get the Window and Graphics to Display
	_window = _display->window->get_window();

	// Create the On-Screen Keyboard
	_keyboard = std::make_unique<Keyboard>(_system, _display, _graphics);

	// Get the Infopanel
	_ip = std::make_unique<InfoPanel>(_system, _display, _graphics);

	// Get the Allocate Stat Panel
	_ap = std::make_unique<AllocatePanel>(_system, _display, _graphics, &_candidate);

	// Get the Texture for the Potraits
	_potrait_texture = &_system->resources->textures[GraphicsTexture::PORTRAITS];

	// Character Display
	_character_display = std::make_unique<CharacterDisplay>(_system, _display, _graphics);

	// Menus
	_method_menu = std::make_unique<Menu>(_system, _display, _graphics, _game, MTP::CHOOSE_METHOD);
	_race_menu = std::make_unique<Menu>(_system, _display, _graphics, _game, MTP::CHOOSE_CHARACTER_RACE);
	_alignment_menu = std::make_unique<Menu>(_system, _display, _graphics, _game, MTP::CHOOSE_CHARACTER_ALIGNMENT);
	_attribute_menu = std::make_unique<Menu>(_system, _display, _graphics, _game, MTP::ALLOCATE_CHARACTER_ATTRIBUTES);
	_class_menu = std::make_unique<Menu>(_system, _display, _graphics, _game, MTP::CHOOSE_CHARACTER_CLASS);
	_final_menu = std::make_unique<Menu>(_system, _display, _graphics, _game, MTP::REVIEW_AND_CONFIRM);

	Component _fmf_c{(*_display->layout)["character_create_stage_7:menu_frame"]};
	_final_menu_frame = std::make_unique<Frame>(
		_display->ui_texture, _fmf_c.w, _fmf_c.h, _fmf_c.colour, _fmf_c.background, _fmf_c.alpha);

	// Create the Confirmation Dialogs
	_dialog_saved_ok = std::make_unique<Dialog>(_system, _display, _graphics,
		(*_display->layout)["character_create_stage_7:dialog_saved_ok"],
		(*_display->layout)["character_create_stage_7:dialog_saved_ok_text"], WDT::OK);
}

auto Sorcery::Create::_initialise_state() -> void {

	_stages.clear();

	_frames.clear();
	_texts.clear();
	_sprites.clear();

	_show_final_menu = false;
	_show_saved_ok = false;

	// Create the Candidate Character
	_candidate = Character(_system, _display, _graphics, _game->itemstore.get());
	_character_display->set(&_candidate);
}

auto Sorcery::Create::_place_components() -> void {

	_method_menu->generate((*_display->layout)["choose_method:menu"]);
	_method_menu->setPosition(
		_display->get_centre_x(_method_menu->get_width()), (*_display->layout)["choose_method:menu"].y);
	_race_menu->generate((*_display->layout)["character_create_stage_2:menu"]);
	_race_menu->setPosition(
		_display->get_centre_x(_race_menu->get_width()), (*_display->layout)["character_create_stage_2:menu"].y);
	_alignment_menu->generate((*_display->layout)["character_create_stage_3:menu"]);
	_alignment_menu->setPosition(
		_display->get_centre_x(_alignment_menu->get_width()), (*_display->layout)["character_create_stage_4:menu"].y);
	_attribute_menu->generate((*_display->layout)["character_create_stage_4:menu"]);
	_attribute_menu->setPosition(
		(*_display->layout)["character_create_stage_4:menu"].y, (*_display->layout)["character_create_stage_4:menu"].y);
	_class_menu->generate((*_display->layout)["character_create_stage_5:menu"]);
	_class_menu->setPosition(
		_display->get_centre_x(_class_menu->get_width()), (*_display->layout)["character_create_stage_5:menu"].y);
	_final_menu->generate((*_display->layout)["character_create_stage_7:menu"]);
	_final_menu->setPosition(
		_display->get_centre_x(_final_menu->get_width()), (*_display->layout)["character_create_stage_7:menu"].y);

	_keyboard->setPosition((*_display->layout)["character_create_stage_1:keyboard"].pos());
	_character_display->setPosition((*_display->layout)["character_create_stage_7:candidate"].pos());
	_dialog_saved_ok->setPosition(_display->get_centre_pos(_dialog_saved_ok->get_size()));
	_final_menu_frame->setPosition(_display->window->get_x(_final_menu_frame->sprite,
									   (*_display->layout)["character_create_stage_7:menu_frame"].x),
		_display->window->get_y(
			_final_menu_frame->sprite, (*_display->layout)["character_create_stage_7:menu_frame"].y));
}

auto Sorcery::Create::start() -> std::optional<MIM> {

	_generate_display();
	_initialise_state();

	// Don't display the info panel yet
	_ip->valid = false;
	_ap->valid = false;

	// Get the Keyboard

	const Component name_c{(*_display->layout)["character_create_stage_1:name_candidate"]};

	// Set to the beginning stage
	_character_display->set_mode(CharacterMode::IN_CREATE);
	_candidate.set_stage(CharacterStage::CHOOSE_METHOD);
	_display->set_input_mode(WIM::CHOOSE_METHOD);
	_display->generate("choose_method", _sprites, _texts, _frames);
	_set_info_panel_contents(_method_menu->selected);

	// Clear the window
	_window->clear();
	_method = CreateMethod::FULL;

	if (auto module_result{_do_event_loop()}; module_result == MDR::EXIT)
		return MIM::ITEM_ABORT;

	return std::nullopt;
}

auto Sorcery::Create::stop() -> void {
}

auto Sorcery::Create::_do_event_loop() -> std::optional<MDR> {

	using enum Enums::System::MDR;

	// And do the main loop
	while (_window->isOpen()) {
		sf::Event event{};
		while (_window->pollEvent(event)) {

			// See what happens
			if (auto const module_result{_handle_input(event)}; module_result) {
				if (module_result.value() == CLOSE)
					return CLOSE;
				else if (module_result.value() == EXIT)
					return EXIT;
				else if (module_result.value() == CANCEL)
					return CANCEL;
				else if (module_result.value() == BACK) {
					if (_candidate.get_stage() == CharacterStage::CHOOSE_METHOD)
						return std::nullopt;
					else
						_go_to_previous_stage();
					continue;
				} else if (module_result.value() == NEXT) {
					_go_to_next_stage();
					continue;
				}
			}

			// Redraw whilst in the module
			_refresh_display();
		}
	}

	return std::nullopt;
}

auto Sorcery::Create::_handle_input(const sf::Event &event) -> std::optional<MDR> {

	using enum Enums::Character::Stage;

	// Handle enabling help overlay
	if (_system->input->check(CIN::SHOW_CONTROLS, event)) {
		_display->show_overlay();
		return std::nullopt;
	} else
		_display->hide_overlay();

	// Now look for other events to respond to
	if (event.type == sf::Event::Closed)
		return MDR::EXIT;
	else if (_system->input->check(CIN::CANCEL, event))
		return MDR::CANCEL;
	if (_candidate.get_stage() == CHOOSE_METHOD)
		return _handle_choose_create_method(event);
	else if (_candidate.get_stage() == ENTER_NAME)
		return _handle_choose_name(event);
	else if (_candidate.get_stage() == CHOOSE_RACE)
		return _handle_choose_race(event);
	else if (_candidate.get_stage() == CHOOSE_ALIGNMENT)
		return _handle_choose_alignment(event);
	else if (_candidate.get_stage() == ALLOCATE_STATS)
		return _handle_allocate_attributes(event);
	else if (_candidate.get_stage() == CHOOSE_CLASS)
		return _handle_choose_class(event);
	else if (_candidate.get_stage() == CHOOSE_PORTRAIT)
		return _handle_choose_potraits(event);
	else if (_candidate.get_stage() == REVIEW_AND_CONFIRM)
		return _handle_review_and_confirm(event);
	else
		return std::nullopt;
}

auto Sorcery::Create::_handle_choose_create_method(const sf::Event &event) -> std::optional<MDR> {

	using enum Enums::Manage::Method;
	using enum Enums::Menu::Item;

	std::optional<std::vector<MenuEntry>::const_iterator> selected{_method_menu->selected};
	if (_system->input->check(CIN::UP, event))
		_method_menu->choose_previous();
	else if (_system->input->check(CIN::DOWN, event))
		_method_menu->choose_next();
	else if (_system->input->check(CIN::MOVE, event))
		_method_menu->set_mouse_selected(static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
	else if (_system->input->check(CIN::BACK, event))
		return MDR::BACK;
	else if (_system->input->check(CIN::DELETE, event))
		return MDR::BACK;
	else if (_system->input->check(CIN::CONFIRM, event)) {

		// We have selected something from the menu
		if (selected) {

			switch ((*selected.value()).item) {
			case CM_FULL:
				_method = FULL;
				break;
			case CM_QUICK:
				_method = QUICK;
				break;
			case CM_RANDOM:
				_method = RANDOM;
				break;
			default:
				return std::nullopt;
				break;
			}

			// We have chosen a method
			return MDR::NEXT;
		}
	}

	// If we get here then we are still on the Method Choice screen so set the
	// Info Panel
	_set_info_panel_contents(_method_menu->selected);

	return std::nullopt;
}

auto Sorcery::Create::_handle_choose_name(const sf::Event &event) -> std::optional<MDR> {

	auto candidate_name{_candidate.get_name()};
	if (_system->input->check(CIN::MOVE, event)) {

		sf::Vector2f mouse_pos{static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window))};
		std::optional<std::string> mouse_selected{
			_keyboard->set_mouse_selected((*_display->layout)["character_create_stage_1:keyboard"], mouse_pos)};
		if (mouse_selected)
			_keyboard->selected = mouse_selected.value();
	} else if (_system->input->check(CIN::ALPHANUMERIC, event) || _system->input->check(CIN::SPACE, event)) {
		if (candidate_name.length() < 16) {
			candidate_name += static_cast<char>(event.text.unicode);
			_candidate.set_name(candidate_name);
		}
		if (static_cast<char>(event.text.unicode) == ' ') {
			_keyboard->selected = "Spc";
		} else {
			auto key_pressed{""s};
			key_pressed.push_back(static_cast<char>(event.text.unicode));
			_keyboard->selected = key_pressed;
		}
	} else if (_system->input->check(CIN::DELETE, event)) {
		if (candidate_name.length() > 0) {
			candidate_name.pop_back();
			_candidate.set_name(candidate_name);
			_keyboard->selected = "Del";
		}
	} else if (_system->input->check(CIN::BACK, event)) {
		if (candidate_name.length() > 0) {
			candidate_name.pop_back();
			_candidate.set_name(candidate_name);
			_keyboard->selected = "Del";
		} else {

			// Return if Back Button is selected and no character name is chosen
			return MDR::BACK;
		}
	} else if (_system->input->check(CIN::SELECT, event)) {
		if (_keyboard->selected == "End") {
			if (TRIM_COPY(candidate_name).length() > 0) {
				_candidate.set_name(candidate_name);

				// Return if End Icon is selected on the keyboard
				return MDR::NEXT;
			}
		} else if (_keyboard->selected == "Spc") {
			if (candidate_name.length() < 16) {
				candidate_name += " ";
				_candidate.set_name(candidate_name);
			}
		} else if (_keyboard->selected == "Del") {
			if (candidate_name.length() > 0) {
				candidate_name.pop_back();
				_candidate.set_name(candidate_name);
			}
		} else {
			candidate_name += _keyboard->selected;
			_candidate.set_name(candidate_name);
		}
	} else if (_system->input->check(CIN::CONFIRM_NO_SPACE, event)) {

		if (_keyboard->selected == "End") {
			if (TRIM_COPY(candidate_name).length() > 0) {
				_candidate.set_name(candidate_name);

				// Return if End Icon is highlighted and Confirmed
				return MDR::NEXT;
			}
		} else {
			if (TRIM_COPY(candidate_name).length() > 0) {
				_candidate.set_name(candidate_name);

				// Return if Confirmation is selected
				return MDR::NEXT;
			}
		}
	} else if (_system->input->check(CIN::LEFT, event))
		_keyboard->set_selected(CIN::LEFT);
	else if (_system->input->check(CIN::RIGHT, event))
		_keyboard->set_selected(CIN::RIGHT);
	else if (_system->input->check(CIN::UP, event))
		_keyboard->set_selected(CIN::UP);
	else if (_system->input->check(CIN::DOWN, event))
		_keyboard->set_selected(CIN::DOWN);

	return std::nullopt;
}

auto Sorcery::Create::_handle_choose_race(const sf::Event &event) -> std::optional<MDR> {

	using enum Enums::Character::Race;
	using enum Enums::Menu::Item;

	std::optional<std::vector<MenuEntry>::const_iterator> selected{_race_menu->selected};
	if (_system->input->check(CIN::UP, event))
		_race_menu->choose_previous();
	else if (_system->input->check(CIN::DOWN, event))
		_race_menu->choose_next();
	else if (_system->input->check(CIN::MOVE, event))
		_race_menu->set_mouse_selected(static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
	else if (_system->input->check(CIN::BACK, event))
		return MDR::BACK;
	else if (_system->input->check(CIN::DELETE, event))
		return MDR::BACK;
	else if (_system->input->check(CIN::CONFIRM, event)) {

		// We have selected something from the menu
		if (selected) {

			switch ((*selected.value()).item) {
			case CR_HUMAN:
				_candidate.set_race(HUMAN);
				break;
			case CR_ELF:
				_candidate.set_race(ELF);
				break;
			case CR_DWARF:
				_candidate.set_race(DWARF);
				break;
			case CR_GNOME:
				_candidate.set_race(GNOME);
				break;
			case CR_HOBBIT:
				_candidate.set_race(HOBBIT);
				break;
			default:
				return std::nullopt;
				break;
			}

			// We have chosen a class
			return MDR::NEXT;
		}
	}

	// If we get here then we are still on the Race Choice screen so set the Info Panel
	_set_info_panel_contents(_race_menu->selected);

	return std::nullopt;
}

auto Sorcery::Create::_handle_choose_alignment(const sf::Event &event) -> std::optional<MDR> {

	using enum Enums::Character::Align;
	using enum Enums::Menu::Item;

	std::optional<std::vector<MenuEntry>::const_iterator> selected{_alignment_menu->selected};
	if (_system->input->check(CIN::UP, event))
		_alignment_menu->choose_previous();
	else if (_system->input->check(CIN::DOWN, event))
		_alignment_menu->choose_next();
	else if (_system->input->check(CIN::MOVE, event))
		_alignment_menu->set_mouse_selected(static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
	else if (_system->input->check(CIN::BACK, event))
		return MDR::BACK;
	else if (_system->input->check(CIN::DELETE, event))
		return MDR::BACK;
	else if (_system->input->check(CIN::CONFIRM, event)) {

		// We have selected something from the menu
		if (selected) {

			switch ((*selected.value()).item) {
			case CA_GOOD:
				_candidate.set_alignment(GOOD);
				break;
			case CA_NEUTRAL:
				_candidate.set_alignment(NEUTRAL);
				break;
			case CA_EVIL:
				_candidate.set_alignment(EVIL);
				break;
			default:
				return std::nullopt;
				break;
			}

			// We have chosen an alignment, so what we need to do is to set the candidate's starting stats and enable
			// the allocation panel before continuing
			_candidate.set_start_attr();
			_ap->set();
			return MDR::NEXT;
		}
	}

	// If we get here then we are still on the Alignment Choice screen so set the Info Panel
	_set_info_panel_contents(_alignment_menu->selected);

	return std::nullopt;
}

auto Sorcery::Create::_handle_allocate_attributes(const sf::Event &event) -> std::optional<MDR> {

	using enum Enums::Character::Attribute;
	using enum Enums::Menu::Item;

	std::optional<std::vector<MenuEntry>::const_iterator> selected{_attribute_menu->selected};
	if (_system->input->check(CIN::UP, event))
		_attribute_menu->choose_previous();
	else if (_system->input->check(CIN::DOWN, event))
		_attribute_menu->choose_next();
	else if (_system->input->check(CIN::MOVE, event))
		_attribute_menu->set_mouse_selected(static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
	else if (_system->input->check(CIN::LEFT, event) || _system->input->check(CIN::BACK, event) ||
			 _system->input->check(CIN::DELETE, event)) {
		if (selected) {
			std::optional<CAR> stat_to_adjust{};
			switch (selected.value()->item) {
			case CS_STRENGTH:
				stat_to_adjust = STRENGTH;
				break;
			case CS_IQ:
				stat_to_adjust = IQ;
				break;
			case CS_PIETY:
				stat_to_adjust = PIETY;
				break;
			case CS_VITALITY:
				stat_to_adjust = VITALITY;
				break;
			case CS_AGILITY:
				stat_to_adjust = AGILITY;
				break;
			case CS_LUCK:
				stat_to_adjust = LUCK;
				break;
			default:
				break;
			}
			if (stat_to_adjust) {
				if (_candidate.get_points_left() < _candidate.get_start_points()) {
					if (_candidate.get_cur_attr(stat_to_adjust.value()) >
						_candidate.get_start_attr(stat_to_adjust.value())) {
						_candidate.set_cur_attr(stat_to_adjust.value(), -1);
						_candidate.set_points_left(_candidate.get_points_left() + 1);
					}
				}
			}
			_candidate.set_pos_class();

			// Handle going back if we have no assigned points
			if (_system->input->check(CIN::BACK, event)) {
				if (_candidate.get_points_left() == _candidate.get_start_points())
					return MDR::BACK;
			}
		}
	} else if (_system->input->check(CIN::RIGHT, event) || _system->input->check(CIN::CONFIRM, event)) {

		if (_system->input->check(CIN::CONFIRM, event) && _candidate.get_points_left() == 0 &&
			_candidate.get_num_pos_class() > 0) {

			// If we have confirmation AND have spent all the points AND have at
			// least one possible class to choose
			_ap->valid = false;
			return MDR::NEXT;
		}

		if (selected) {
			std::optional<CAR> stat_to_adjust{};
			switch (selected.value()->item) {
			case CS_STRENGTH:
				stat_to_adjust = STRENGTH;
				break;
			case CS_IQ:
				stat_to_adjust = IQ;
				break;
			case CS_PIETY:
				stat_to_adjust = PIETY;
				break;
			case CS_VITALITY:
				stat_to_adjust = VITALITY;
				break;
			case CS_AGILITY:
				stat_to_adjust = AGILITY;
				break;
			case CS_LUCK:
				stat_to_adjust = LUCK;
				break;
			default:
				break;
			}
			if (stat_to_adjust) {

				if (_candidate.get_points_left() > 0) {
					if (_candidate.get_cur_attr(stat_to_adjust.value()) < 18) {
						_candidate.set_cur_attr(stat_to_adjust.value(), 1);
						_candidate.set_points_left(_candidate.get_points_left() - 1);
					}
				}
			}

			_candidate.set_pos_class();
		}
	}

	_ap->set();
	_set_info_panel_contents(_attribute_menu->selected);
	return std::nullopt;
}

auto Sorcery::Create::_handle_choose_class(const sf::Event &event) -> std::optional<MDR> {

	using enum Enums::Character::Class;
	using enum Enums::Menu::Item;

	std::optional<std::vector<MenuEntry>::const_iterator> class_selected{_class_menu->selected};
	if (_system->input->check(CIN::UP, event))
		_class_menu->choose_previous();
	else if (_system->input->check(CIN::DOWN, event))
		_class_menu->choose_next();
	else if (_system->input->check(CIN::MOVE, event))
		_class_menu->set_mouse_selected(static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
	else if (_system->input->check(CIN::BACK, event))
		return MDR::BACK;
	else if (_system->input->check(CIN::DELETE, event))
		return MDR::BACK;
	else if (_system->input->check(CIN::CONFIRM, event)) {

		// We have selected something from the menu
		if (class_selected) {

			switch ((*class_selected.value()).item) {
			case CC_SAMURAI:
				_candidate.set_class(SAMURAI);
				break;
			case CC_FIGHTER:
				_candidate.set_class(FIGHTER);
				break;
			case CC_LORD:
				_candidate.set_class(LORD);
				break;
			case CC_THIEF:
				_candidate.set_class(THIEF);
				break;
			case CC_NINJA:
				_candidate.set_class(NINJA);
				break;
			case CC_PRIEST:
				_candidate.set_class(PRIEST);
				break;
			case CC_BISHOP:
				_candidate.set_class(BISHOP);
				break;
			case CC_MAGE:
				_candidate.set_class(MAGE);
				break;
			default:
				return std::nullopt;
				break;
			}

			// We have chosen the Class ok so continue
			return MDR::NEXT;
		}
	}

	_set_info_panel_contents(_class_menu->selected);
	return std::nullopt;
}

auto Sorcery::Create::_handle_choose_potraits(const sf::Event &event) -> std::optional<MDR> {

	auto index{_candidate.get_portrait_index()};
	if (_system->input->check(CIN::BACK, event))
		return MDR::BACK;
	else if (_system->input->check(CIN::DELETE, event))
		return MDR::BACK;
	else if (_system->input->check(CIN::LEFT, event)) {
		if (index > 0) {
			--index;
			_candidate.set_portrait_index(index);
		} else
			_candidate.set_portrait_index(MAX_PORTRAIT_INDEX);
	} else if (_system->input->check(CIN::RIGHT, event)) {
		if (index < MAX_PORTRAIT_INDEX) {
			++index;
			_candidate.set_portrait_index(index);
		} else
			_candidate.set_portrait_index(0);
	} else if (_system->input->check(CIN::CONFIRM, event))
		return MDR::NEXT;

	return std::nullopt;
}

auto Sorcery::Create::_handle_review_and_confirm(const sf::Event &event) -> std::optional<MDR> {

	using enum Enums::Menu::Item;

	std::optional<std::vector<MenuEntry>::const_iterator> selected{_final_menu->selected};
	if (_show_final_menu) {
		if (_show_saved_ok) {
			if (auto dialog_input{_dialog_saved_ok->handle_input(event)}; dialog_input) {
				if (dialog_input.value() == WDB::CLOSE) {
					_show_final_menu = false;
					_show_saved_ok = false;
					_display->set_input_mode(WIM::NAVIGATE_MENU);
					return std::nullopt;
				} else if (dialog_input.value() == WDB::OK) {
					_show_final_menu = false;
					_show_saved_ok = false;
					_display->set_input_mode(WIM::NAVIGATE_MENU);
					return MDR::CLOSE;
				}
			}
		} else {

			if (_system->input->check(CIN::BACK, event) || _system->input->check(CIN::DELETE, event)) {
				_show_final_menu = false;
				_display->set_input_mode(WIM::REVIEW_AND_CONFIRM);
			} else if (_system->input->check(CIN::CONFIRM, event)) {
				if (selected) {
					switch ((*selected.value()).item) {
					case RC_ACCEPT: {
						_candidate.set_stage(CharacterStage::COMPLETED);
						auto new_id{_game->add_character(_candidate)};
						_game->characters[new_id] = _candidate; // TODO: HERE
						_game->save_game();
						_game->load_game();
						_show_saved_ok = true;
					} break;
					case RC_REJECT:
						_show_final_menu = false;
						return MDR::CLOSE;
						break;
					case RC_CANCEL:
						_show_final_menu = false;
						_display->set_input_mode(WIM::REVIEW_AND_CONFIRM);
						break;
					default:
						break;
					}
				}

			} else if (_system->input->check(CIN::UP, event))
				_final_menu->choose_previous();
			else if (_system->input->check(CIN::DOWN, event))
				_final_menu->choose_next();
			else if (_system->input->check(CIN::MOVE, event))
				_final_menu->set_mouse_selected(static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
		}
	} else {

		if (_system->input->check(CIN::CONFIRM, event)) {
			_show_final_menu = true;
			_display->set_input_mode(WIM::NAVIGATE_MENU);
		} else if (_system->input->check(CIN::BACK, event))
			return MDR::BACK;
		else if (_system->input->check(CIN::DELETE, event))
			return MDR::BACK;
		else if (_system->input->check(CIN::LEFT, event))
			_character_display->left_view();
		else if (_system->input->check(CIN::RIGHT, event))
			_character_display->right_view();
		else if (_system->input->check(CIN::UP, event)) {
			if (_character_display->get_view() == CharacterView::MAGE_SPELLS)
				_character_display->dec_hl_spell(SpellType::MAGE);
			else if (_character_display->get_view() == CharacterView::PRIEST_SPELLS)
				_character_display->dec_hl_spell(SpellType::PRIEST);
		} else if (_system->input->check(CIN::DOWN, event)) {
			if (_character_display->get_view() == CharacterView::MAGE_SPELLS)
				_character_display->inc_hl_spell(SpellType::MAGE);
			else if (_character_display->get_view() == CharacterView::PRIEST_SPELLS)
				_character_display->inc_hl_spell(SpellType::PRIEST);
		} else if (_system->input->check(CIN::MOVE, event)) {
			if (_character_display->check_for_mouse_move(
					sf::Vector2f(static_cast<float>(sf::Mouse::getPosition(*_window).x),
						static_cast<float>(sf::Mouse::getPosition(*_window).y)))) {
				_character_display->set_view(_character_display->get_view());
			}
			if (_character_display->check_for_action_mouse_move(
					sf::Vector2f(static_cast<float>(sf::Mouse::getPosition(*_window).x),
						static_cast<float>(sf::Mouse::getPosition(*_window).y)))) {
				_character_display->generate_display();
			}
		}
	}

	return std::nullopt;
}

auto Sorcery::Create::_go_to_previous_stage() -> void {

	if (_method == CreateMethod::FULL) {
		switch (_candidate.get_stage()) {
		case CharacterStage::CHOOSE_METHOD:
			// Don't do anything special here - handled in the calling function
			break;
		case CharacterStage::ENTER_NAME: {
			auto popped{_stages.back()};
			_candidate = popped;
			_candidate.set_stage(CharacterStage::CHOOSE_METHOD);
			_display->generate("choose_method", _sprites, _texts, _frames);
			_stages.pop_back();
			_method_menu->choose(_candidate.get_method());
			_set_info_panel_contents(_method_menu->selected);
			_display->set_input_mode(WIM::NAVIGATE_MENU);
		} break;
		case CharacterStage::CHOOSE_RACE: {
			auto popped{_stages.back()};
			_candidate = popped;
			_candidate.set_stage(CharacterStage::ENTER_NAME);
			_display->generate("character_create_stage_1", _sprites, _texts, _frames);
			_stages.pop_back();
			_display->set_input_mode(WIM::INPUT_NAME);
		} break;
		case CharacterStage::CHOOSE_ALIGNMENT: {
			auto popped{_stages.back()};
			_candidate = popped;
			_candidate.set_stage(CharacterStage::CHOOSE_RACE);
			_display->generate("character_create_stage_2", _sprites, _texts, _frames);
			_stages.pop_back();
			_race_menu->choose(_candidate.get_race());
			_set_info_panel_contents(_race_menu->selected);
			_display->set_input_mode(WIM::NAVIGATE_MENU);
		} break;
		case CharacterStage::ALLOCATE_STATS: {
			auto popped{_stages.back()};
			_candidate = popped;
			_candidate.set_stage(CharacterStage::CHOOSE_ALIGNMENT);
			_display->generate("character_create_stage_3", _sprites, _texts, _frames);
			_stages.pop_back();
			_alignment_menu->choose(_candidate.get_alignment());
			_set_info_panel_contents(_alignment_menu->selected);
			_display->set_input_mode(WIM::NAVIGATE_MENU);
			_ap->valid = false;
		} break;
		case CharacterStage::CHOOSE_CLASS: {
			auto popped{_stages.back()};
			_candidate = popped;
			_candidate.set_stage(CharacterStage::ALLOCATE_STATS);
			_display->generate("character_create_stage_4", _sprites, _texts, _frames);
			_stages.pop_back();
			_display->set_input_mode(WIM::ALLOCATE_STATS);
			_attribute_menu->selected = _attribute_menu->items.begin();
			_ap->set();
			_set_info_panel_contents(_attribute_menu->selected);
		} break;
		case CharacterStage::CHOOSE_PORTRAIT: {
			auto popped{_stages.back()};
			_candidate = popped;
			_candidate.set_stage(CharacterStage::CHOOSE_CLASS);
			_display->generate("character_create_stage_5", _sprites, _texts, _frames);
			_stages.pop_back();
			_set_classes_menu();
			_class_menu->generate((*_display->layout)["character_create_stage_5:menu"]);
			_class_menu->choose(_candidate.get_class());
			_set_info_panel_contents(_class_menu->selected);
			_display->set_input_mode(WIM::NAVIGATE_MENU);
			_ap->valid = false;
		} break;
		case CharacterStage::REVIEW_AND_CONFIRM: {
			auto popped{_stages.back()};
			_candidate = popped;
			_candidate.set_stage(CharacterStage::CHOOSE_PORTRAIT);
			_character_display->generate_display();
			_display->generate("character_create_stage_6", _sprites, _texts, _frames);
			_display->set_input_mode(WIM::CHOOSE_PORTRAIT);
			_stages.pop_back();
			_ap->valid = false;
			_show_final_menu = false;
		}
		default:
			break;
		}
	} else if (_method == CreateMethod::RANDOM) {
		if (_candidate.get_stage() == CharacterStage::CHOOSE_METHOD) {
			// Don't do anything special here - handled in the calling function
		} else if (_candidate.get_stage() == CharacterStage::REVIEW_AND_CONFIRM) {
			_candidate.set_stage(CharacterStage::CHOOSE_METHOD);
			_method_menu->choose(_candidate.get_method());
			_display->generate("choose_method", _sprites, _texts, _frames);
			_display->set_input_mode(WIM::NAVIGATE_MENU);
			_show_final_menu = false;
			_set_info_panel_contents(_method_menu->selected);
		}
	} else if (_method == CreateMethod::QUICK) {
		switch (_candidate.get_stage()) {
		case CharacterStage::CHOOSE_METHOD:

			// Don't do anything special here
			break;
		case CharacterStage::ENTER_NAME: {
			auto popped{_stages.back()};
			_candidate = popped;
			_candidate.set_stage(CharacterStage::CHOOSE_METHOD);
			_display->generate("choose_method", _sprites, _texts, _frames);
			_stages.pop_back();
			_method_menu->choose(_candidate.get_method());
			_display->set_input_mode(WIM::NAVIGATE_MENU);
			_set_info_panel_contents(_method_menu->selected);
		} break;
		case CharacterStage::CHOOSE_PORTRAIT: {
			auto popped{_stages.back()};
			_candidate = popped;
			_candidate.set_stage(CharacterStage::ENTER_NAME);
			_display->generate("character_create_stage_1", _sprites, _texts, _frames);
			_stages.pop_back();
			_display->set_input_mode(WIM::INPUT_NAME);
			_ap->valid = false;
		} break;
		case CharacterStage::REVIEW_AND_CONFIRM: {
			auto popped{_stages.back()};
			_candidate = popped;
			_candidate.set_stage(CharacterStage::CHOOSE_PORTRAIT);
			_display->generate("character_create_stage_6", _sprites, _texts, _frames);
			_stages.pop_back();
			_ap->valid = false;
			_display->set_input_mode(WIM::CHOOSE_PORTRAIT);
			_show_final_menu = false;
		}
		default:
			break;
		}
	}
}

auto Sorcery::Create::_go_to_next_stage() -> void {

	using enum Enums::Character::Stage;

	if (_method == CreateMethod::FULL) {

		switch (_candidate.get_stage()) {
		case CHOOSE_METHOD: {
			_stages.emplace_back(_candidate);
			_candidate.set_stage(ENTER_NAME);
			_display->generate("character_create_stage_1", _sprites, _texts, _frames);
			_display->set_input_mode(WIM::INPUT_NAME);
		} break;
		case ENTER_NAME: {
			_stages.emplace_back(_candidate);
			_candidate.set_stage(CHOOSE_RACE);
			_display->generate("character_create_stage_2", _sprites, _texts, _frames);
			_display->set_input_mode(WIM::NAVIGATE_MENU);
			_race_menu->choose_first();
			_set_info_panel_contents(_race_menu->selected);

		} break;
		case CHOOSE_RACE: {
			_stages.emplace_back(_candidate);
			_candidate.set_stage(CHOOSE_ALIGNMENT);
			_display->generate("character_create_stage_3", _sprites, _texts, _frames);
			_display->set_input_mode(WIM::NAVIGATE_MENU);
			_alignment_menu->choose_first();
		} break;
		case CHOOSE_ALIGNMENT: {
			_stages.emplace_back(_candidate);
			_candidate.set_stage(ALLOCATE_STATS);
			_display->generate("character_create_stage_4", _sprites, _texts, _frames);
			_display->set_input_mode(WIM::ALLOCATE_STATS);
			_attribute_menu->choose_first();
			_ap->set();
			_set_info_panel_contents(_attribute_menu->selected);
		} break;
		case ALLOCATE_STATS: {
			_stages.emplace_back(_candidate);
			_candidate.set_stage(CHOOSE_CLASS);
			_display->generate("character_create_stage_5", _sprites, _texts, _frames);
			_display->set_input_mode(WIM::NAVIGATE_MENU);
			_ap->valid = false;

			// Set and enable the class menu depending on the possible classes!
			_set_classes_menu();
			_class_menu->generate((*_display->layout)["character_create_stage_5:menu"]);
			_class_menu->choose_first();
			_set_info_panel_contents(_class_menu->selected);
		} break;
		case CHOOSE_CLASS: {
			_stages.emplace_back(_candidate);
			_candidate.set_stage(CHOOSE_PORTRAIT);
			_display->generate("character_create_stage_6", _sprites, _texts, _frames);
			_display->set_input_mode(WIM::CHOOSE_PORTRAIT);
			_candidate.set_portrait_index(0);
		} break;
		case CHOOSE_PORTRAIT: {
			_stages.emplace_back(_candidate);
			_candidate.finalise();
			_add_starting_equipment();
			_candidate.set_stage(REVIEW_AND_CONFIRM);
			_character_display->generate_display();
			_display->generate("character_create_stage_7", _sprites, _texts, _frames);
			_display->set_input_mode(WIM::REVIEW_AND_CONFIRM);
			_show_final_menu = false;
		} break;
		default:
			break;
		}
	} else if (_method == CreateMethod::RANDOM) {
		_candidate.create_random();
		_candidate.finalise();
		_add_starting_equipment();
		_candidate.set_stage(CharacterStage::REVIEW_AND_CONFIRM);
		_character_display->set(&_candidate);
		_character_display->set_view(CharacterView::SUMMARY);
		_character_display->generate_display();
		_character_display->update();
		_display->generate("character_create_stage_7", _sprites, _texts, _frames);
		_display->set_input_mode(WIM::REVIEW_AND_CONFIRM);
	} else if (_method == CreateMethod::QUICK) {
		switch (_candidate.get_stage()) {
		case CHOOSE_METHOD: {
			_stages.emplace_back(_candidate);
			_candidate.set_stage(ENTER_NAME);
			_display->generate("character_create_stage_1", _sprites, _texts, _frames);
			_display->set_input_mode(WIM::INPUT_NAME);
		} break;
		case ENTER_NAME: {
			_stages.emplace_back(_candidate);
			_candidate.create_quick();
			_candidate.set_stage(CHOOSE_PORTRAIT);
			_display->generate("character_create_stage_6", _sprites, _texts, _frames);
			_display->set_input_mode(WIM::CHOOSE_PORTRAIT);
			_candidate.set_portrait_index(0);
			_ap->valid = false;
		} break;
		case CHOOSE_PORTRAIT: {
			_stages.emplace_back(_candidate);
			_candidate.finalise();
			_add_starting_equipment();
			_candidate.set_stage(REVIEW_AND_CONFIRM);
			_character_display->set(&_candidate);
			_character_display->set_view(CharacterView::SUMMARY);
			_character_display->generate_display();
			_character_display->update();
			_display->generate("character_create_stage_7", _sprites, _texts, _frames);
			_display->set_input_mode(WIM::REVIEW_AND_CONFIRM);
			_show_final_menu = false;
		} break;
		default:
			break;
		}
	}
}

auto Sorcery::Create::_set_classes_menu() -> void {

	using enum Enums::Character::Class;

	auto possible_classes{_candidate.get_pos_class()};
	(*_class_menu)[0].enabled = possible_classes[SAMURAI];
	(*_class_menu)[1].enabled = possible_classes[FIGHTER];
	(*_class_menu)[2].enabled = possible_classes[LORD];
	(*_class_menu)[3].enabled = possible_classes[THIEF];
	(*_class_menu)[4].enabled = possible_classes[NINJA];
	(*_class_menu)[5].enabled = possible_classes[PRIEST];
	(*_class_menu)[6].enabled = possible_classes[BISHOP];
	(*_class_menu)[7].enabled = possible_classes[MAGE];
}

auto Sorcery::Create::_set_info_panel_contents(std::vector<Sorcery::MenuEntry>::const_iterator it) -> void {

	// Set the Text
	if ((*it).type == MIT::ENTRY) {
		const auto ip_contents{(*it).hint};
		_ip->set_text(ip_contents);
		_ip->valid = true;
	} else
		_ip->valid = false;

	// Set the Icon
	if ((*it).type == MIT::ENTRY)
		_ip->set_icon((*it).item);
}

auto Sorcery::Create::_draw() -> void {

	// Background Wallpaper
	_graphics->tile_bg(_window);

	// Display Base Components
	_display->display("create", _candidate.get_stage());

	// And draw the current state of the character!
	const auto lerp{_graphics->animation->colour_lerp};
	sf::Text summary_text{};
	if (_candidate.get_stage() == CharacterStage::CHOOSE_METHOD) {

		_display->display("choose_method", _sprites, _texts, _frames);

		_method_menu->generate((*_display->layout)["choose_method:menu"]);
		_window->draw(*_method_menu);

		_display->window->draw_text(
			summary_text, (*_display->layout)["choose_method:summary_text"], _candidate.summary_text());

		if (_ip->valid) {
			_ip->setPosition((*_display->layout)["choose_method:info_panel"].pos());
			_window->draw(*_ip);
		}

	} else if (_candidate.get_stage() == CharacterStage::ENTER_NAME) {

		_display->display("character_create_stage_1", _sprites, _texts, _frames);

		// Font we are using replaces the { with a left cursor character
		auto display_name{_candidate.get_name() + "{"};
		sf::Text name_text{};
		_display->window->draw_text(
			name_text, (*_display->layout)["character_create_stage_1:name_candidate"], display_name, lerp);
		_display->window->draw_text(
			summary_text, (*_display->layout)["character_create_stage_1:summary_text"], _candidate.summary_text());

		// Draw the On Screen Keyboard
		_keyboard->set_selected_background();
		_window->draw(*_keyboard);
	} else if (_candidate.get_stage() == CharacterStage::CHOOSE_RACE) {

		_display->display("character_create_stage_2", _sprites, _texts, _frames);
		_race_menu->generate((*_display->layout)["character_create_stage_2:menu"]);
		_window->draw(*_race_menu);

		_display->window->draw_text(
			summary_text, (*_display->layout)["character_create_stage_2:summary_text"], _candidate.summary_text());

		// Display bottom text depending on the menu item selected
		if (_ip->valid) {
			_ip->setPosition((*_display->layout)["character_create_stage_2:info_panel"].pos());
			_window->draw(*_ip);
		}
	} else if (_candidate.get_stage() == CharacterStage::CHOOSE_ALIGNMENT) {

		_display->display("character_create_stage_3", _sprites, _texts, _frames);
		_alignment_menu->generate((*_display->layout)["character_create_stage_3:menu"]);
		_window->draw(*_alignment_menu);

		_display->window->draw_text(
			summary_text, (*_display->layout)["character_create_stage_3:summary_text"], _candidate.summary_text());

		// Display bottom text depending on the menu item selected
		if (_ip->valid) {
			_ip->setPosition((*_display->layout)["character_create_stage_3:info_panel"].pos());
			_window->draw(*_ip);
		}
	} else if (_candidate.get_stage() == CharacterStage::ALLOCATE_STATS) {

		_display->display("character_create_stage_4", _sprites, _texts, _frames);
		_attribute_menu->generate((*_display->layout)["character_create_stage_4:menu"]);
		_window->draw(*_attribute_menu);

		_display->window->draw_text(
			summary_text, (*_display->layout)["character_create_stage_4:summary_text"], _candidate.summary_text());

		if (_ap->valid) {
			_ap->setPosition((*_display->layout)["character_create_stage_4:allocate_panel"].pos());
			_window->draw(*_ap);
		}

		// Display bottom text depending on the menu item selected
		if (_ip->valid) {
			_ip->setPosition((*_display->layout)["character_create_stage_4:info_panel"].pos());
			_window->draw(*_ip);
		}
	} else if (_candidate.get_stage() == CharacterStage::CHOOSE_CLASS) {
		_display->display("character_create_stage_5", _sprites, _texts, _frames);
		_class_menu->generate((*_display->layout)["character_create_stage_5:menu"]);
		_window->draw(*_class_menu);

		if (_ap->valid) {
			_ap->setPosition((*_display->layout)["character_create_stage_4:allocate_panel"].pos());
			_window->draw(*_ap);
		}

		_display->window->draw_text(
			summary_text, (*_display->layout)["character_create_stage_5:summary_text"], _candidate.summary_text());

		// Display bottom text depending on the menu item selected
		if (_ip->valid) {
			_ip->setPosition((*_display->layout)["character_create_stage_5:info_panel"].pos());
			_window->draw(*_ip);
		}

	} else if (_candidate.get_stage() == CharacterStage::CHOOSE_PORTRAIT) {

		_display->display("character_create_stage_6", _sprites, _texts, _frames);

		sf::Sprite portrait{_get_character_portrait(_candidate.get_portrait_index()).value()};
		portrait.setPosition((*_display->layout)["character_create_stage_6:current_portrait"].pos());
		portrait.setScale((*_display->layout)["character_create_stage_6:current_portrait"].scl());

		_display->window->draw_text(
			summary_text, (*_display->layout)["character_create_stage_6:summary_text"], _candidate.summary_text());

		_window->draw(portrait);

	} else if (_candidate.get_stage() == CharacterStage::REVIEW_AND_CONFIRM) {

		_display->display("character_create_stage_7", _sprites, _texts, _frames);

		_character_display->update();
		_window->draw(*_character_display);

		if (_show_final_menu) {
			_final_menu->generate((*_display->layout)["character_create_stage_7:menu"]);

			_window->draw(*_final_menu_frame);
			_window->draw(*_final_menu);

			if (_show_saved_ok) {
				_dialog_saved_ok->update();
				_window->draw(*_dialog_saved_ok);
			}
		}
	}

	// Draw the Overlay if present
	_display->display_overlay();

	// And finally the Cursor
	_display->display_cursor();
}

auto Sorcery::Create::_get_character_portrait(const unsigned int index) -> std::optional<sf::Sprite> {

	sf::Sprite portrait{_graphics->textures->get(index, GraphicsTextureType::PORTRAIT).value()};

	return portrait;
}

auto Sorcery::Create::_add_starting_equipment() -> void {

	using enum Enums::Character::Class;
	using enum Enums::Items::TypeID;

	_candidate.inventory.clear();

	switch (_candidate.get_class()) { // NOLINT(clang-diagnostic-switch)
	case FIGHTER:
	case LORD:
	case SAMURAI:
		_candidate.inventory.add_type((*_game->itemstore)[LEATHER_ARMOR], true);
		_candidate.inventory.add_type((*_game->itemstore)[LONG_SWORD], true);
		break;
	case MAGE:
		_candidate.inventory.add_type((*_game->itemstore)[ROBES], true);
		_candidate.inventory.add_type((*_game->itemstore)[DAGGER], true);
		break;
	case PRIEST:
	case BISHOP:
		_candidate.inventory.add_type((*_game->itemstore)[ROBES], true);
		_candidate.inventory.add_type((*_game->itemstore)[STAFF], true);
		break;
	case THIEF:
	case NINJA:
		_candidate.inventory.add_type((*_game->itemstore)[LEATHER_ARMOR], true);
		_candidate.inventory.add_type((*_game->itemstore)[SHORT_SWORD], true);
	default:
		break;
	}
}
