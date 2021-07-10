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

#include "create.hpp"

// Standard Constructor
Sorcery::Create::Create(
	System *system, Display *display, Graphics *graphics, Game *game)
	: _system{system}, _display{display}, _graphics{graphics}, _game{game} {

	// Get the Window and Graphics to Display
	_window = _display->window->get_window();

	// Create the On-Screen Keyboard
	_keyboard = std::make_shared<Keyboard>(system, display, graphics);

	// Get the Infopanel
	_ip = std::make_shared<InfoPanel>(_system, _display, _graphics);

	// Get the Allocate Stat Panel
	_ap = std::make_shared<AllocatePanel>(
		_system, _display, _graphics, &_candidate);

	// Get the Texture for the Potraits
	_potrait_texture = &system->resources->textures[GraphicsTexture::PORTRAITS];

	// Menus
	_method_menu = std::make_shared<Menu>(
		_system, _display, _graphics, _game, MenuType::CHOOSE_METHOD);
	_race_menu = std::make_shared<Menu>(
		_system, _display, _graphics, _game, MenuType::CHOOSE_CHARACTER_RACE);
	_alignment_menu = std::make_shared<Menu>(_system, _display, _graphics,
		_game, MenuType::CHOOSE_CHARACTER_ALIGNMENT);
	_attribute_menu = std::make_shared<Menu>(_system, _display, _graphics,
		_game, MenuType::ALLOCATE_CHARACTER_ATTRIBUTES);
	_class_menu = std::make_shared<Menu>(
		_system, _display, _graphics, _game, MenuType::CHOOSE_CHARACTER_CLASS);
	_final_menu = std::make_shared<Menu>(
		_system, _display, _graphics, _game, MenuType::REVIEW_AND_CONFIRM);

	Component _fmf_c{
		(*_display->layout)["character_create_stage_7:menu_frame"]};
	_final_menu_frame =
		std::make_unique<Frame>(_display->ui_texture, WindowFrameType::NORMAL,
			_fmf_c.w, _fmf_c.h, _fmf_c.colour, _fmf_c.background, _fmf_c.alpha);

	// Create the Candidate Character
	_stages.clear();
	_candidate = Character(_system, _display, _graphics);

	_frames.clear();
	_texts.clear();
	_sprites.clear();

	_show_final_menu = false;
	_show_saved_ok = false;

	// Create the Confirmation Dialogs
	_dialog_saved_ok = std::make_shared<Dialog>(_system, _display, _graphics,
		(*_display->layout)["character_create_stage_7:dialog_saved_ok"],
		(*_display->layout)["character_create_stage_7:dialog_saved_ok_text"],
		WindowDialogType::OK);
	_dialog_saved_ok->setPosition(
		(*_display->layout)["character_create_stage_7:dialog_saved_ok"].x,
		(*_display->layout)["character_create_stage_7:dialog_saved_ok"].y);
}

// Standard Destructor
Sorcery::Create::~Create() {}

auto Sorcery::Create::start() -> std::optional<MenuItem> {

	// Get the Background Display Components and load them into Display module
	// storage (not local)
	_display->generate("create");

	// Don't display the info panel yet
	_ip->valid = false;
	_ap->valid = false;

	// Get the Keyboard
	_keyboard->setPosition(
		(*_display->layout)["character_create_stage_1:keyboard"].x,
		(*_display->layout)["character_create_stage_1:keyboard"].y);

	const Component name_c{
		(*_display->layout)["character_create_stage_1:name_candidate"]};

	// Set to the beginning stage
	_candidate.set_stage(CharacterStage::CHOOSE_METHOD);
	_display->generate("choose_method", _sprites, _texts, _frames);
	_display->set_input_mode(WindowInputMode::CHOOSE_METHOD);
	_set_info_panel_contents(_method_menu->selected);

	// Clear the window
	_window->clear();
	_method = CreateMethod::FULL;

	// Play the background movie!
	_display->fit_bg_movie();
	_display->start_bg_movie();

	auto module_result{_do_event_loop()};
	if (module_result == ModuleResult::EXIT)
		_window->close();

	return std::nullopt;
}

auto Sorcery::Create::stop() -> void {

	// Stop the background movie!
	_display->stop_bg_movie();
}

auto Sorcery::Create::_do_event_loop() -> std::optional<ModuleResult> {

	// And do the main loop
	while (_window->isOpen()) {
		sf::Event event{};
		while (_window->pollEvent(event)) {

			// See what happens
			auto const module_result{_handle_input(event)};
			if (module_result) {
				if (module_result.value() == ModuleResult::CLOSE)
					return ModuleResult::CLOSE;
				else if (module_result.value() == ModuleResult::EXIT)
					return ModuleResult::EXIT;
				else if (module_result.value() == ModuleResult::CANCEL)
					return ModuleResult::CANCEL;
				else if (module_result.value() == ModuleResult::BACK) {
					if (_candidate.get_stage() == CharacterStage::CHOOSE_METHOD)
						return std::nullopt;
					else
						_go_to_previous_stage();
					continue;
				} else if (module_result.value() == ModuleResult::NEXT) {
					_go_to_next_stage();
					continue;
				}
			}

			// Redraw whilst in the module
			_window->clear();

			// Update Background Movie
			_display->start_bg_movie();
			_display->update_bg_movie();
			_display->draw_bg_movie();

			_draw();
			_window->display();
		}
	}

	return std::nullopt;
}

auto Sorcery::Create::_handle_input(const sf::Event &event)
	-> std::optional<ModuleResult> {

	// Handle enabling help overlay
	if (_system->input->check(WindowInput::SHOW_CONTROLS, event)) {
		_display->show_overlay();
		return std::nullopt;
	} else
		_display->hide_overlay();

	// Now look for other events to respond to
	if (event.type == sf::Event::Closed)
		return ModuleResult::EXIT;
	else if (_system->input->check(WindowInput::CANCEL, event))
		return ModuleResult::CANCEL;
	if (_candidate.get_stage() == CharacterStage::CHOOSE_METHOD)
		return _handle_choose_create_method(event);
	else if (_candidate.get_stage() == CharacterStage::ENTER_NAME)
		return _handle_choose_name(event);
	else if (_candidate.get_stage() == CharacterStage::CHOOSE_RACE)
		return _handle_choose_race(event);
	else if (_candidate.get_stage() == CharacterStage::CHOOSE_ALIGNMENT)
		return _handle_choose_alignment(event);
	else if (_candidate.get_stage() == CharacterStage::ALLOCATE_STATS)
		return _handle_allocate_attributes(event);
	else if (_candidate.get_stage() == CharacterStage::CHOOSE_CLASS)
		return _handle_choose_class(event);
	else if (_candidate.get_stage() == CharacterStage::CHOOSE_PORTRAIT)
		return _handle_choose_potraits(event);
	else if (_candidate.get_stage() == CharacterStage::REVIEW_AND_CONFIRM)
		return _handle_review_and_confirm(event);
	else
		return std::nullopt;
}

auto Sorcery::Create::_handle_choose_create_method(const sf::Event &event)
	-> std::optional<ModuleResult> {

	std::optional<std::vector<MenuEntry>::const_iterator> selected{
		_method_menu->selected};
	if (_system->input->check(WindowInput::UP, event))
		selected = _method_menu->choose_previous();
	else if (_system->input->check(WindowInput::DOWN, event))
		selected = _method_menu->choose_next();
	else if (_system->input->check(WindowInput::MOVE, event))
		selected = _method_menu->set_mouse_selected(
			static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
	else if (_system->input->check(WindowInput::BACK, event))
		return ModuleResult::BACK;
	else if (_system->input->check(WindowInput::DELETE, event))
		return ModuleResult::BACK;
	else if (_system->input->check(WindowInput::CONFIRM, event)) {

		// We have selected something from the menu
		if (selected) {

			switch ((*selected.value()).item) {
			case MenuItem::CM_FULL:
				_method = CreateMethod::FULL;
				break;
			case MenuItem::CM_QUICK:
				_method = CreateMethod::QUICK;
				break;
			case MenuItem::CM_RANDOM:
				_method = CreateMethod::RANDOM;
				break;
			default:
				return std::nullopt;
				break;
			}

			// We have chosen a method
			return ModuleResult::NEXT;
		}
	}

	// If we get here then we are still on the Method Choice screen so set the
	// Info Panel
	_set_info_panel_contents(_method_menu->selected);

	return std::nullopt;
}

auto Sorcery::Create::_handle_choose_name(const sf::Event &event)
	-> std::optional<ModuleResult> {

	std::string candidate_name{_candidate.get_name()};
	if (_system->input->check(WindowInput::MOVE, event)) {

		sf::Vector2f mouse_pos{
			static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window))};
		std::optional<std::string> mouse_selected{_keyboard->set_mouse_selected(
			(*_display->layout)["character_create_stage_1:keyboard"],
			mouse_pos)};
		if (mouse_selected)
			_keyboard->selected = mouse_selected.value();
	} else if ((_system->input->check(WindowInput::ALPHANUMERIC, event)) ||
			   (_system->input->check(WindowInput::SPACE, event))) {
		if (candidate_name.length() < 16) {
			candidate_name += static_cast<char>(event.text.unicode);
			_candidate.set_name(candidate_name);
		}
		if (static_cast<char>(event.text.unicode) == ' ') {
			_keyboard->selected = "Spc";
		} else {
			std::string key_pressed{};
			key_pressed.push_back(static_cast<char>(event.text.unicode));
			_keyboard->selected = key_pressed;
		}
	} else if (_system->input->check(WindowInput::DELETE, event)) {
		if (candidate_name.length() > 0) {
			candidate_name.pop_back();
			_candidate.set_name(candidate_name);
			_keyboard->selected = "Del";
		}
	} else if (_system->input->check(WindowInput::BACK, event)) {
		if (candidate_name.length() > 0) {
			candidate_name.pop_back();
			_candidate.set_name(candidate_name);
			_keyboard->selected = "Del";
		} else {

			// Return if Back Button is selected and no character name is chosen
			return ModuleResult::BACK;
		}
	} else if (_system->input->check(WindowInput::SELECT, event)) {
		if (_keyboard->selected == "End") {
			if (TRIM_COPY(candidate_name).length() > 0) {
				_candidate.set_name(candidate_name);

				// Return if End Icon is selected on the keyboard
				return ModuleResult::NEXT;
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
	} else if (_system->input->check(WindowInput::CONFIRM_NO_SPACE, event)) {

		if (_keyboard->selected == "End") {
			if (TRIM_COPY(candidate_name).length() > 0) {
				_candidate.set_name(candidate_name);

				// Return if End Icon is highlighted and Confirmed
				return ModuleResult::NEXT;
			}
		} else {
			if (TRIM_COPY(candidate_name).length() > 0) {
				_candidate.set_name(candidate_name);

				// Return if Confirmation is selected
				return ModuleResult::NEXT;
			}
		}
	} else if (_system->input->check(WindowInput::LEFT, event))
		_keyboard->set_selected(WindowInput::LEFT);
	else if (_system->input->check(WindowInput::RIGHT, event))
		_keyboard->set_selected(WindowInput::RIGHT);
	else if (_system->input->check(WindowInput::UP, event))
		_keyboard->set_selected(WindowInput::UP);
	else if (_system->input->check(WindowInput::DOWN, event))
		_keyboard->set_selected(WindowInput::DOWN);

	return std::nullopt;
}

auto Sorcery::Create::_handle_choose_race(const sf::Event &event)
	-> std::optional<ModuleResult> {

	std::optional<std::vector<MenuEntry>::const_iterator> selected{
		_race_menu->selected};
	if (_system->input->check(WindowInput::UP, event))
		selected = _race_menu->choose_previous();
	else if (_system->input->check(WindowInput::DOWN, event))
		selected = _race_menu->choose_next();
	else if (_system->input->check(WindowInput::MOVE, event))
		selected = _race_menu->set_mouse_selected(
			static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
	else if (_system->input->check(WindowInput::BACK, event))
		return ModuleResult::BACK;
	else if (_system->input->check(WindowInput::DELETE, event))
		return ModuleResult::BACK;
	else if (_system->input->check(WindowInput::CONFIRM, event)) {

		// We have selected something from the menu
		if (selected) {

			switch ((*selected.value()).item) {
			case MenuItem::CR_HUMAN:
				_candidate.set_race(CharacterRace::HUMAN);
				break;
			case MenuItem::CR_ELF:
				_candidate.set_race(CharacterRace::ELF);
				break;
			case MenuItem::CR_DWARF:
				_candidate.set_race(CharacterRace::DWARF);
				break;
			case MenuItem::CR_GNOME:
				_candidate.set_race(CharacterRace::GNOME);
				break;
			case MenuItem::CR_HOBBIT:
				_candidate.set_race(CharacterRace::HOBBIT);
				break;
			default:
				return std::nullopt;
				break;
			}

			// We have chosen a class
			return ModuleResult::NEXT;
		}
	}

	// If we get here then we are still on the Race Choice screen so set the
	// Info Panel
	_set_info_panel_contents(_race_menu->selected);

	return std::nullopt;
}

auto Sorcery::Create::_handle_choose_alignment(const sf::Event &event)
	-> std::optional<ModuleResult> {

	std::optional<std::vector<MenuEntry>::const_iterator> selected{
		_alignment_menu->selected};
	if (_system->input->check(WindowInput::UP, event))
		selected = _alignment_menu->choose_previous();
	else if (_system->input->check(WindowInput::DOWN, event))
		selected = _alignment_menu->choose_next();
	else if (_system->input->check(WindowInput::MOVE, event))
		selected = _alignment_menu->set_mouse_selected(
			static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
	else if (_system->input->check(WindowInput::BACK, event))
		return ModuleResult::BACK;
	else if (_system->input->check(WindowInput::DELETE, event))
		return ModuleResult::BACK;
	else if (_system->input->check(WindowInput::CONFIRM, event)) {

		// We have selected something from the menu
		if (selected) {

			switch ((*selected.value()).item) {
			case MenuItem::CA_GOOD:
				_candidate.set_alignment(CharacterAlignment::GOOD);
				break;
			case MenuItem::CA_NEUTRAL:
				_candidate.set_alignment(CharacterAlignment::NEUTRAL);
				break;
			case MenuItem::CA_EVIL:
				_candidate.set_alignment(CharacterAlignment::EVIL);
				break;
			default:
				return std::nullopt;
				break;
			}

			// We have chosen an alignment, so what we need to do is to set the
			// candidate's starting stats and enable the allocation panel before
			// continuing
			_candidate.set_start_attr();
			_ap->set();
			return ModuleResult::NEXT;
		}
	}

	// If we get here then we are still on the Alignment Choice screen so set
	// the Info Panel
	_set_info_panel_contents(_alignment_menu->selected);

	return std::nullopt;
}

auto Sorcery::Create::_handle_allocate_attributes(const sf::Event &event)
	-> std::optional<ModuleResult> {

	std::optional<std::vector<MenuEntry>::const_iterator> selected{
		_attribute_menu->selected};
	if (_system->input->check(WindowInput::UP, event))
		selected = _attribute_menu->choose_previous();
	else if (_system->input->check(WindowInput::DOWN, event))
		selected = _attribute_menu->choose_next();
	else if (_system->input->check(WindowInput::MOVE, event))
		selected = _attribute_menu->set_mouse_selected(
			static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
	else if ((_system->input->check(WindowInput::LEFT, event)) ||
			 (_system->input->check(WindowInput::BACK, event)) ||
			 (_system->input->check(WindowInput::DELETE, event))) {
		if (selected) {
			std::optional<CharacterAttribute> stat_to_adjust{};
			switch (selected.value()->item) {
			case MenuItem::CS_STRENGTH:
				stat_to_adjust = CharacterAttribute::STRENGTH;
				break;
			case MenuItem::CS_IQ:
				stat_to_adjust = CharacterAttribute::IQ;
				break;
			case MenuItem::CS_PIETY:
				stat_to_adjust = CharacterAttribute::PIETY;
				break;
			case MenuItem::CS_VITALITY:
				stat_to_adjust = CharacterAttribute::VITALITY;
				break;
			case MenuItem::CS_AGILITY:
				stat_to_adjust = CharacterAttribute::AGILITY;
				break;
			case MenuItem::CS_LUCK:
				stat_to_adjust = CharacterAttribute::LUCK;
				break;
			default:
				break;
			}
			if (stat_to_adjust) {
				if (_candidate.get_points_left() <
					_candidate.get_start_points()) {
					if (_candidate.get_cur_attr(stat_to_adjust.value()) >
						_candidate.get_start_attr(stat_to_adjust.value())) {
						_candidate.set_cur_attr(stat_to_adjust.value(), -1);
						_candidate.set_points_left(
							_candidate.get_points_left() + 1);
					}
				}
			}
			_candidate.set_pos_class();

			// Handle going back if we have no assigned points
			if (_system->input->check(WindowInput::BACK, event)) {
				if (_candidate.get_points_left() ==
					_candidate.get_start_points())
					return ModuleResult::BACK;
			}
		}
	} else if ((_system->input->check(WindowInput::RIGHT, event)) ||
			   (_system->input->check(WindowInput::CONFIRM, event))) {

		if ((_system->input->check(WindowInput::CONFIRM, event)) &&
			(_candidate.get_points_left() == 0) &&
			(_candidate.get_num_pos_class() > 0)) {

			// If we have confirmation AND have spent all the points AND have at
			// least one possible class to choose
			_ap->valid = false;
			return ModuleResult::NEXT;
		}

		if (selected) {
			std::optional<CharacterAttribute> stat_to_adjust{};
			switch (selected.value()->item) {
			case MenuItem::CS_STRENGTH:
				stat_to_adjust = CharacterAttribute::STRENGTH;
				break;
			case MenuItem::CS_IQ:
				stat_to_adjust = CharacterAttribute::IQ;
				break;
			case MenuItem::CS_PIETY:
				stat_to_adjust = CharacterAttribute::PIETY;
				break;
			case MenuItem::CS_VITALITY:
				stat_to_adjust = CharacterAttribute::VITALITY;
				break;
			case MenuItem::CS_AGILITY:
				stat_to_adjust = CharacterAttribute::AGILITY;
				break;
			case MenuItem::CS_LUCK:
				stat_to_adjust = CharacterAttribute::LUCK;
				break;
			default:
				break;
			}
			if (stat_to_adjust) {

				if (_candidate.get_points_left() > 0) {
					if (_candidate.get_cur_attr(stat_to_adjust.value()) < 18) {
						_candidate.set_cur_attr(stat_to_adjust.value(), 1);
						_candidate.set_points_left(
							_candidate.get_points_left() - 1);
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

auto Sorcery::Create::_handle_choose_class(const sf::Event &event)
	-> std::optional<ModuleResult> {

	std::optional<std::vector<MenuEntry>::const_iterator> class_selected{
		_class_menu->selected};
	if (_system->input->check(WindowInput::UP, event))
		class_selected = _class_menu->choose_previous();
	else if (_system->input->check(WindowInput::DOWN, event))
		class_selected = _class_menu->choose_next();
	else if (_system->input->check(WindowInput::MOVE, event))
		class_selected = _class_menu->set_mouse_selected(
			static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
	else if (_system->input->check(WindowInput::BACK, event))
		return ModuleResult::BACK;
	else if (_system->input->check(WindowInput::DELETE, event))
		return ModuleResult::BACK;
	else if (_system->input->check(WindowInput::CONFIRM, event)) {

		// We have selected something from the menu
		if (class_selected) {

			switch ((*class_selected.value()).item) {
			case MenuItem::CC_SAMURAI:
				_candidate.set_class(CharacterClass::SAMURAI);
				break;
			case MenuItem::CC_FIGHTER:
				_candidate.set_class(CharacterClass::FIGHTER);
				break;
			case MenuItem::CC_LORD:
				_candidate.set_class(CharacterClass::LORD);
				break;
			case MenuItem::CC_THIEF:
				_candidate.set_class(CharacterClass::THIEF);
				break;
			case MenuItem::CC_NINJA:
				_candidate.set_class(CharacterClass::NINJA);
				break;
			case MenuItem::CC_PRIEST:
				_candidate.set_class(CharacterClass::PRIEST);
				break;
			case MenuItem::CC_BISHOP:
				_candidate.set_class(CharacterClass::BISHOP);
				break;
			case MenuItem::CC_MAGE:
				_candidate.set_class(CharacterClass::MAGE);
				break;
			default:
				return std::nullopt;
				break;
			}

			// We have chosen the Class ok so continue
			return ModuleResult::NEXT;
		}
	}

	_set_info_panel_contents(_class_menu->selected);
	return std::nullopt;
}

auto Sorcery::Create::_handle_choose_potraits(const sf::Event &event)
	-> std::optional<ModuleResult> {

	unsigned int index{_candidate.get_portrait_index()};
	if (_system->input->check(WindowInput::BACK, event))
		return ModuleResult::BACK;
	else if (_system->input->check(WindowInput::DELETE, event))
		return ModuleResult::BACK;
	else if (_system->input->check(WindowInput::LEFT, event)) {
		if (index > 0)
			_candidate.set_portrait_index(--index);
		else
			_candidate.set_portrait_index(MAX_PORTRAIT_INDEX);
	} else if (_system->input->check(WindowInput::RIGHT, event)) {
		if (index < MAX_PORTRAIT_INDEX)
			_candidate.set_portrait_index(++index);
		else
			_candidate.set_portrait_index(0);
	} else if (_system->input->check(WindowInput::CONFIRM, event))
		return ModuleResult::NEXT;

	return std::nullopt;
}

auto Sorcery::Create::_handle_review_and_confirm(const sf::Event &event)
	-> std::optional<ModuleResult> {

	std::optional<std::vector<MenuEntry>::const_iterator> selected{
		_final_menu->selected};
	if (_show_final_menu) {

		if (_show_saved_ok) {

			auto dialog_input{_dialog_saved_ok->handle_input(event)};
			if (dialog_input) {
				if (dialog_input.value() == WindowDialogButton::CLOSE) {
					_show_final_menu = false;
					_show_saved_ok = false;
					_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
					return std::nullopt;
				} else if (dialog_input.value() == WindowDialogButton::OK) {
					_show_final_menu = false;
					_show_saved_ok = false;
					_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
					return ModuleResult::CLOSE;
				}
			};
		} else {

			if (_system->input->check(WindowInput::BACK, event)) {
				_show_final_menu = false;
				_display->set_input_mode(WindowInputMode::REVIEW_AND_CONFIRM);
			} else if (_system->input->check(WindowInput::DELETE, event)) {
				_show_final_menu = false;
				_display->set_input_mode(WindowInputMode::REVIEW_AND_CONFIRM);
			} else if (_system->input->check(WindowInput::CONFIRM, event)) {
				if (selected) {
					switch ((*selected.value()).item) {
					case MenuItem::RC_ACCEPT:
						_game->add_char(_candidate);
						_show_saved_ok = true;
						break;
					case MenuItem::RC_REJECT:
						_show_final_menu = false;
						return ModuleResult::CLOSE;
						break;
					case MenuItem::RC_CANCEL:
						_show_final_menu = false;
						_display->set_input_mode(
							WindowInputMode::REVIEW_AND_CONFIRM);
						break;
					default:
						break;
					}
				}

			} else if (_system->input->check(WindowInput::UP, event))
				selected = _final_menu->choose_previous();
			else if (_system->input->check(WindowInput::DOWN, event))
				selected = _final_menu->choose_next();
			else if (_system->input->check(WindowInput::MOVE, event))
				selected =
					_final_menu->set_mouse_selected(static_cast<sf::Vector2f>(
						sf::Mouse::getPosition(*_window)));
		}
	} else {

		if (_system->input->check(WindowInput::CONFIRM, event)) {
			_show_final_menu = true;
			_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
		} else if (_system->input->check(WindowInput::BACK, event))
			return ModuleResult::BACK;
		else if (_system->input->check(WindowInput::DELETE, event))
			return ModuleResult::BACK;
		else if (_system->input->check(WindowInput::LEFT, event))
			_candidate.left_view();
		else if (_system->input->check(WindowInput::RIGHT, event))
			_candidate.right_view();
		else if (_system->input->check(WindowInput::UP, event)) {
			if (_candidate.get_view() == CharacterView::MAGE_SPELLS)
				_candidate.dec_hl_spell(SpellType::MAGE);
			else if (_candidate.get_view() == CharacterView::PRIEST_SPELLS)
				_candidate.dec_hl_spell(SpellType::PRIEST);
		} else if (_system->input->check(WindowInput::DOWN, event)) {
			if (_candidate.get_view() == CharacterView::MAGE_SPELLS)
				_candidate.inc_hl_spell(SpellType::MAGE);
			else if (_candidate.get_view() == CharacterView::PRIEST_SPELLS)
				_candidate.inc_hl_spell(SpellType::PRIEST);
		} else if (_system->input->check(WindowInput::MOVE, event)) {
			if (_candidate.check_for_mouse_move(sf::Vector2f(
					static_cast<float>(sf::Mouse::getPosition(*_window).x),
					static_cast<float>(sf::Mouse::getPosition(*_window).y)))) {
				_candidate.set_view(_candidate.get_view());
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
			_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
		} break;
		case CharacterStage::CHOOSE_RACE: {
			auto popped{_stages.back()};
			_candidate = popped;
			_candidate.set_stage(CharacterStage::ENTER_NAME);
			_display->generate(
				"character_create_stage_1", _sprites, _texts, _frames);
			_stages.pop_back();
			_display->set_input_mode(WindowInputMode::INPUT_NAME);
		} break;
		case CharacterStage::CHOOSE_ALIGNMENT: {
			auto popped{_stages.back()};
			_candidate = popped;
			_candidate.set_stage(CharacterStage::CHOOSE_RACE);
			_display->generate(
				"character_create_stage_2", _sprites, _texts, _frames);
			_stages.pop_back();
			_race_menu->choose(_candidate.get_race());
			_set_info_panel_contents(_race_menu->selected);
			_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
		} break;
		case CharacterStage::ALLOCATE_STATS: {
			auto popped{_stages.back()};
			_candidate = popped;
			_candidate.set_stage(CharacterStage::CHOOSE_ALIGNMENT);
			_display->generate(
				"character_create_stage_3", _sprites, _texts, _frames);
			_stages.pop_back();
			_alignment_menu->choose(_candidate.get_alignment());
			_set_info_panel_contents(_alignment_menu->selected);
			_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
			_ap->valid = false;
		} break;
		case CharacterStage::CHOOSE_CLASS: {
			auto popped{_stages.back()};
			_candidate = popped;
			_candidate.set_stage(CharacterStage::ALLOCATE_STATS);
			_display->generate(
				"character_create_stage_4", _sprites, _texts, _frames);
			_stages.pop_back();
			_display->set_input_mode(WindowInputMode::ALLOCATE_STATS);
			_attribute_menu->selected = _attribute_menu->items.begin();
			_ap->set();
			_set_info_panel_contents(_attribute_menu->selected);
		} break;
		case CharacterStage::CHOOSE_PORTRAIT: {
			auto popped{_stages.back()};
			_candidate = popped;
			_candidate.set_stage(CharacterStage::CHOOSE_CLASS);
			_display->generate(
				"character_create_stage_5", _sprites, _texts, _frames);
			_stages.pop_back();
			_set_classes_menu();
			_class_menu->choose(_candidate.get_class());
			_set_info_panel_contents(_class_menu->selected);
			_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
			_ap->valid = false;
		} break;
		case CharacterStage::REVIEW_AND_CONFIRM: {
			auto popped{_stages.back()};
			_candidate = popped;
			_candidate.set_stage(CharacterStage::CHOOSE_PORTRAIT);
			_display->generate(
				"character_create_stage_6", _sprites, _texts, _frames);
			_display->set_input_mode(WindowInputMode::CHOOSE_PORTRAIT);
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
		} else if (_candidate.get_stage() ==
				   CharacterStage::REVIEW_AND_CONFIRM) {
			_candidate.set_stage(CharacterStage::CHOOSE_METHOD);
			_method_menu->choose(_candidate.get_method());
			_display->generate("choose_method", _sprites, _texts, _frames);
			_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
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
			_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
			_set_info_panel_contents(_method_menu->selected);
		} break;
		case CharacterStage::CHOOSE_PORTRAIT: {
			auto popped{_stages.back()};
			_candidate = popped;
			_candidate.set_stage(CharacterStage::ENTER_NAME);
			_display->generate(
				"character_create_stage_1", _sprites, _texts, _frames);
			_stages.pop_back();
			_display->set_input_mode(WindowInputMode::INPUT_NAME);
			_ap->valid = false;
		} break;
		case CharacterStage::REVIEW_AND_CONFIRM: {
			auto popped{_stages.back()};
			_candidate = popped;
			_candidate.set_stage(CharacterStage::CHOOSE_PORTRAIT);
			_display->generate(
				"character_create_stage_6", _sprites, _texts, _frames);
			_stages.pop_back();
			_ap->valid = false;
			_display->set_input_mode(WindowInputMode::CHOOSE_PORTRAIT);
			_show_final_menu = false;
		}
		default:
			break;
		}
	}
}

auto Sorcery::Create::_go_to_next_stage() -> void {

	if (_method == CreateMethod::FULL) {

		switch (_candidate.get_stage()) {
		case CharacterStage::CHOOSE_METHOD: {
			auto to_push(_candidate);
			_stages.emplace_back(to_push);
			_candidate.set_stage(CharacterStage::ENTER_NAME);
			_display->generate(
				"character_create_stage_1", _sprites, _texts, _frames);
			_display->set_input_mode(WindowInputMode::INPUT_NAME);
		} break;
		case CharacterStage::ENTER_NAME: {
			auto to_push(_candidate);
			_stages.emplace_back(to_push);
			_candidate.set_stage(CharacterStage::CHOOSE_RACE);
			_display->generate(
				"character_create_stage_2", _sprites, _texts, _frames);
			_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
			_race_menu->choose_first();
			_set_info_panel_contents(_race_menu->selected);

		} break;
		case CharacterStage::CHOOSE_RACE: {
			auto to_push(_candidate);
			_stages.emplace_back(to_push);
			_candidate.set_stage(CharacterStage::CHOOSE_ALIGNMENT);
			_display->generate(
				"character_create_stage_3", _sprites, _texts, _frames);
			_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
			_alignment_menu->choose_first();
		} break;
		case CharacterStage::CHOOSE_ALIGNMENT: {
			auto to_push(_candidate);
			_stages.emplace_back(to_push);
			_candidate.set_stage(CharacterStage::ALLOCATE_STATS);
			_display->generate(
				"character_create_stage_4", _sprites, _texts, _frames);
			_display->set_input_mode(WindowInputMode::ALLOCATE_STATS);
			_attribute_menu->choose_first();
			_ap->set();
			_set_info_panel_contents(_attribute_menu->selected);
		} break;
		case CharacterStage::ALLOCATE_STATS: {
			auto to_push(_candidate);
			_stages.emplace_back(to_push);
			_candidate.set_stage(CharacterStage::CHOOSE_CLASS);
			_display->generate(
				"character_create_stage_5", _sprites, _texts, _frames);
			_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
			_ap->valid = false;

			// Set and enable the class menu depending on the possible classes!
			_set_classes_menu();
			_class_menu->choose_first();
			_set_info_panel_contents(_class_menu->selected);
		} break;
		case CharacterStage::CHOOSE_CLASS: {
			auto to_push(_candidate);
			_stages.emplace_back(to_push);
			_candidate.set_stage(CharacterStage::CHOOSE_PORTRAIT);
			_display->generate(
				"character_create_stage_6", _sprites, _texts, _frames);
			_display->set_input_mode(WindowInputMode::CHOOSE_PORTRAIT);
			_candidate.set_portrait_index(0);
		} break;
		case CharacterStage::CHOOSE_PORTRAIT: {
			auto to_push(_candidate);
			_stages.emplace_back(to_push);
			_candidate.finalise();
			_candidate.set_stage(CharacterStage::REVIEW_AND_CONFIRM);
			_display->generate(
				"character_create_stage_7", _sprites, _texts, _frames);
			_display->set_input_mode(WindowInputMode::REVIEW_AND_CONFIRM);
			_show_final_menu = false;
		} break;
		default:
			break;
		}
	} else if (_method == CreateMethod::RANDOM) {
		_candidate.create_random();
		_candidate.finalise();
		_candidate.set_stage(CharacterStage::REVIEW_AND_CONFIRM);
		_display->generate(
			"character_create_stage_7", _sprites, _texts, _frames);
		_display->set_input_mode(WindowInputMode::REVIEW_AND_CONFIRM);
	} else if (_method == CreateMethod::QUICK) {
		switch (_candidate.get_stage()) {
		case CharacterStage::CHOOSE_METHOD: {
			auto to_push(_candidate);
			_stages.emplace_back(to_push);
			_candidate.set_stage(CharacterStage::ENTER_NAME);
			_display->generate(
				"character_create_stage_1", _sprites, _texts, _frames);
			_display->set_input_mode(WindowInputMode::INPUT_NAME);
		} break;
		case CharacterStage::ENTER_NAME: {
			auto to_push(_candidate);
			_stages.emplace_back(to_push);
			_candidate.create_quick();
			_candidate.set_stage(CharacterStage::CHOOSE_PORTRAIT);
			_display->generate(
				"character_create_stage_6", _sprites, _texts, _frames);
			_display->set_input_mode(WindowInputMode::CHOOSE_PORTRAIT);
			_candidate.set_portrait_index(0);
			_ap->valid = false;
		} break;
		case CharacterStage::CHOOSE_PORTRAIT: {
			auto to_push(_candidate);
			_stages.emplace_back(to_push);
			_candidate.finalise();
			_candidate.set_stage(CharacterStage::REVIEW_AND_CONFIRM);
			_display->generate(
				"character_create_stage_7", _sprites, _texts, _frames);
			_display->set_input_mode(WindowInputMode::REVIEW_AND_CONFIRM);
			_show_final_menu = false;
		} break;
		default:
			break;
		}
	}
}

auto Sorcery::Create::_set_classes_menu() -> void {

	auto possible_classes{_candidate.get_pos_class()};
	(*_class_menu)[0].enabled = possible_classes[CharacterClass::SAMURAI];
	(*_class_menu)[1].enabled = possible_classes[CharacterClass::FIGHTER];
	(*_class_menu)[2].enabled = possible_classes[CharacterClass::LORD];
	(*_class_menu)[3].enabled = possible_classes[CharacterClass::THIEF];
	(*_class_menu)[4].enabled = possible_classes[CharacterClass::NINJA];
	(*_class_menu)[5].enabled = possible_classes[CharacterClass::PRIEST];
	(*_class_menu)[6].enabled = possible_classes[CharacterClass::BISHOP];
	(*_class_menu)[7].enabled = possible_classes[CharacterClass::MAGE];
}

auto Sorcery::Create::_set_info_panel_contents(
	std::vector<Sorcery::MenuEntry>::const_iterator it) -> void {

	// Set the Text
	if ((*it).type == MenuItemType::ENTRY) {
		std::string ip_contents{(*it).hint};
		_ip->set_text(ip_contents);
		_ip->valid = true;
	} else
		_ip->valid = false;

	// Set the Icon
	if ((*it).type == MenuItemType::ENTRY)
		_ip->set_icon((*it).item);
}

auto Sorcery::Create::_draw() -> void {

	// Display Base Components
	_display->display("create", _candidate.get_stage());

	// And draw the current state of the character!
	double lerp{_graphics->animation->colour_lerp};
	sf::Text summary_text{};
	if (_candidate.get_stage() == CharacterStage::CHOOSE_METHOD) {

		_display->display("choose_method", _sprites, _texts, _frames);

		_method_menu->generate((*_display->layout)["choose_method:menu"]);
		const sf::Vector2f menu_pos((*_display->layout)["choose_method:menu"].x,
			(*_display->layout)["choose_method:menu"].y);
		_method_menu->setPosition(menu_pos);
		_window->draw(*_method_menu);

		_display->window->draw_text(summary_text,
			(*_display->layout)["choose_method:summary_text"],
			_candidate.summary_text());

		if (_ip->valid) {
			_ip->setPosition((*_display->layout)["choose_method:info_panel"].x,
				(*_display->layout)["choose_method:info_panel"].y);
			_window->draw(*_ip);
		}

	} else if (_candidate.get_stage() == CharacterStage::ENTER_NAME) {

		_display->display(
			"character_create_stage_1", _sprites, _texts, _frames);
		std::string display_name{">" + _candidate.get_name() + "_"};
		sf::Text name_text{};
		_display->window->draw_text(name_text,
			(*_display->layout)["character_create_stage_1:name_candidate"],
			display_name, lerp);
		_display->window->draw_text(summary_text,
			(*_display->layout)["character_create_stage_1:summary_text"],
			_candidate.summary_text());

		// Draw the On Screen Keyboard
		_keyboard->set_selected_background();
		_window->draw(*_keyboard);
	} else if (_candidate.get_stage() == CharacterStage::CHOOSE_RACE) {

		_display->display(
			"character_create_stage_2", _sprites, _texts, _frames);
		_race_menu->generate(
			(*_display->layout)["character_create_stage_2:menu"]);
		const sf::Vector2f menu_pos(
			(*_display->layout)["character_create_stage_2:menu"].x,
			(*_display->layout)["character_create_stage_2:menu"].y);
		_race_menu->setPosition(menu_pos);
		_window->draw(*_race_menu);

		_display->window->draw_text(summary_text,
			(*_display->layout)["character_create_stage_2:summary_text"],
			_candidate.summary_text());

		// Display bottom text depending on the menu item selected
		if (_ip->valid) {
			_ip->setPosition(
				(*_display->layout)["character_create_stage_2:info_panel"].x,
				(*_display->layout)["character_create_stage_2:info_panel"].y);
			_window->draw(*_ip);
		}
	} else if (_candidate.get_stage() == CharacterStage::CHOOSE_ALIGNMENT) {

		_display->display(
			"character_create_stage_3", _sprites, _texts, _frames);
		_alignment_menu->generate(
			(*_display->layout)["character_create_stage_3:menu"]);
		const sf::Vector2f menu_pos(
			(*_display->layout)["character_create_stage_3:menu"].x,
			(*_display->layout)["character_create_stage_3:menu"].y);
		_alignment_menu->setPosition(menu_pos);
		_window->draw(*_alignment_menu);

		_display->window->draw_text(summary_text,
			(*_display->layout)["character_create_stage_3:summary_text"],
			_candidate.summary_text());

		// Display bottom text depending on the menu item selected
		if (_ip->valid) {
			_ip->setPosition(
				(*_display->layout)["character_create_stage_3:info_panel"].x,
				(*_display->layout)["character_create_stage_3:info_panel"].y);
			_window->draw(*_ip);
		}
	} else if (_candidate.get_stage() == CharacterStage::ALLOCATE_STATS) {

		_display->display(
			"character_create_stage_4", _sprites, _texts, _frames);
		_attribute_menu->generate(
			(*_display->layout)["character_create_stage_4:menu"]);
		const sf::Vector2f menu_pos(
			(*_display->layout)["character_create_stage_4:menu"].x,
			(*_display->layout)["character_create_stage_4:menu"].y);
		_attribute_menu->setPosition(menu_pos);
		_window->draw(*_attribute_menu);

		_display->window->draw_text(summary_text,
			(*_display->layout)["character_create_stage_4:summary_text"],
			_candidate.summary_text());

		if (_ap->valid) {
			_ap->setPosition(
				(*_display->layout)["character_create_stage_4:allocate_panel"]
					.x,
				(*_display->layout)["character_create_stage_4:allocate_panel"]
					.y);
			_window->draw(*_ap);
		}

		// Display bottom text depending on the menu item selected
		if (_ip->valid) {
			_ip->setPosition(
				(*_display->layout)["character_create_stage_4:info_panel"].x,
				(*_display->layout)["character_create_stage_4:info_panel"].y);
			_window->draw(*_ip);
		}
	} else if (_candidate.get_stage() == CharacterStage::CHOOSE_CLASS) {
		_display->display(
			"character_create_stage_5", _sprites, _texts, _frames);
		_class_menu->generate(
			(*_display->layout)["character_create_stage_5:menu"]);
		const sf::Vector2f menu_pos(
			(*_display->layout)["character_create_stage_5:menu"].x,
			(*_display->layout)["character_create_stage_5:menu"].y);
		_class_menu->setPosition(menu_pos);
		_window->draw(*_class_menu);

		if (_ap->valid) {
			_ap->setPosition(
				(*_display->layout)["character_create_stage_4:allocate_panel"]
					.x,
				(*_display->layout)["character_create_stage_4:allocate_panel"]
					.y);
			_window->draw(*_ap);
		}

		_display->window->draw_text(summary_text,
			(*_display->layout)["character_create_stage_5:summary_text"],
			_candidate.summary_text());

		// Display bottom text depending on the menu item selected
		if (_ip->valid) {
			_ip->setPosition(
				(*_display->layout)["character_create_stage_5:info_panel"].x,
				(*_display->layout)["character_create_stage_5:info_panel"].y);
			_window->draw(*_ip);
		}

	} else if (_candidate.get_stage() == CharacterStage::CHOOSE_PORTRAIT) {

		_display->display(
			"character_create_stage_6", _sprites, _texts, _frames);

		sf::Sprite portrait{
			_get_character_portrait(_candidate.get_portrait_index()).value()};
		portrait.setPosition(
			(*_display->layout)["character_create_stage_6:current_portrait"].x,
			(*_display->layout)["character_create_stage_6:current_portrait"].y);
		portrait.setScale(
			(*_display->layout)["character_create_stage_6:current_portrait"]
				.scale,
			(*_display->layout)["character_create_stage_6:current_portrait"]
				.scale);

		_display->window->draw_text(summary_text,
			(*_display->layout)["character_create_stage_6:summary_text"],
			_candidate.summary_text());

		_window->draw(portrait);

	} else if (_candidate.get_stage() == CharacterStage::REVIEW_AND_CONFIRM) {

		_display->display(
			"character_create_stage_7", _sprites, _texts, _frames);
		_candidate.setPosition(
			(*_display->layout)["character_create_stage_7:candidate"].x,
			(*_display->layout)["character_create_stage_7:candidate"].y);

		_candidate.update();
		_window->draw(_candidate);

		if (_show_final_menu) {
			_final_menu->generate(
				(*_display->layout)["character_create_stage_7:menu"]);
			const sf::Vector2f menu_pos(
				(*_display->layout)["character_create_stage_7:menu"].x,
				(*_display->layout)["character_create_stage_7:menu"].y);
			const sf::Vector2f frame_pos(
				(*_display->layout)["character_create_stage_7:menu_frame"].x,
				(*_display->layout)["character_create_stage_7:menu_frame"].y);
			_final_menu->setPosition(menu_pos);
			_final_menu_frame->setPosition(frame_pos);
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

auto Sorcery::Create::_get_character_portrait(const unsigned int index)
	-> std::optional<sf::Sprite> {

	// Workout the location of the potrait on the texture, noting that the
	// potraits are all square and are 600x600 pixels in size arranged in a grid
	// of 6 by 5
	sf::Vector2u top_left{(index % 6) * 600, (index / 6) * 600};
	sf::IntRect rect{sf::IntRect(top_left.x, top_left.y, 600, 600)};

	// Grab the associated part of the texture and return it
	sf::Sprite portrait(
		_system->resources->textures[GraphicsTexture::PORTRAITS]);
	portrait.setTextureRect(rect);

	return portrait;
}
