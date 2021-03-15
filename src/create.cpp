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

#include "create.hpp"

// Standard Constructor
Sorcery::Create::Create(System *system, Display *display, Graphics *graphics)
	: _system{system}, _display{display}, _graphics{graphics} {

	// Get the Window and Graphics to Display
	_window = _display->window->get_window();

	// Create the On-Screen Keyboard
	_keyboard = std::make_shared<Keyboard>(system, display, graphics);

	// Get the Infopanel
	_ip = std::make_shared<InfoPanel>(_system, _display, _graphics);

	// Get the Allocate Stat Panel
	_ap = std::make_shared<AllocatePanel>(_system, _display, _graphics, &_candidate);

	// Get the Attribute Display Panel
	_ad = std::make_shared<AttributeDisplay>(_system, _display, _graphics, &_candidate);

	// Layout Information
	_name_c = Component((*_display->layout)["character_create_stage_1:name_candidate"]);
	_keyb_c = Component((*_display->layout)["character_create_stage_1:keyboard"]);
	_ip_race_c = Component((*_display->layout)["character_create_stage_2:info_panel"]);
	_ip_alignment_c = Component((*_display->layout)["character_create_stage_3:info_panel"]);
	_ip_attribute_c = Component((*_display->layout)["character_create_stage_4:info_panel"]);
	_ip_class_c = Component((*_display->layout)["character_create_stage_5:info_panel"]);
	_ap_c = Component((*_display->layout)["character_create_stage_4:allocate_panel"]);
	_ad_c = Component((*_display->layout)["create:stage_4_attribute_display"]);
	// need component for portrait picker

	// Menus
	_race_menu =
		std::make_shared<Menu>(_system, _display, _graphics, MenuType::CHOOSE_CHARACTER_RACE);
	_alignment_menu =
		std::make_shared<Menu>(_system, _display, _graphics, MenuType::CHOOSE_CHARACTER_ALIGNMENT);
	_attribute_menu = std::make_shared<Menu>(
		_system, _display, _graphics, MenuType::ALLOCATE_CHARACTER_ATTRIBUTES);
	_class_menu =
		std::make_shared<Menu>(_system, _display, _graphics, MenuType::CHOOSE_CHARACTER_CLASS);

	// Create the Candidate Character
	_stages.clear();
	_candidate = Character(_system, _display, _graphics);

	for (auto &progress : _progress)
		progress = std::nullopt;
}

// Standard Destructor
Sorcery::Create::~Create() {}

auto Sorcery::Create::start() -> std::optional<MenuItem> {

	// Get the Background Display Components and load them into Display module storage (not local)
	_display->generate_components("create");

	// Get the custom components
	const Component bg_c{(*_display->layout)["create:background"]};
	const sf::IntRect bg_rect(1147, 249, 773, 388);
	_bg.setTexture(_system->resources->textures[GraphicsTexture::TOWN]);
	_bg.setTextureRect(bg_rect);
	_bg.setScale(bg_c.scale, bg_c.scale);
	_bg.setPosition(_display->window->get_x(_bg, bg_c.x), _display->window->get_y(_bg, bg_c.y));

	// Don't display the info panel yet
	_ip->valid = false;
	_ap->valid = false;
	_ad->valid = false;

	// Get the Keyboard
	_keyboard->setPosition(_keyb_c.x, _keyb_c.y);
	_ap->setPosition(_ap_c.x, _ap_c.y);

	const Component name_c{(*_display->layout)["character_create_stage_1:name_candidate"]};
	_candidate.set_stage(CharacterStage::ENTER_NAME);

	// Clear the window
	_window->clear();

	// Will need to change this for the seven screens as needed
	_display->window->input_mode = WindowInputMode::INPUT_TEXT;

	auto module_result = _do_event_loop();
	if (module_result == ModuleResult::EXIT)
		_window->close();

	return std::nullopt;
}

auto Sorcery::Create::stop() -> void {}

// create copy of shared_ptr and then push it onto the vector

auto Sorcery::Create::_go_to_previous_stage() -> void {

	switch (_candidate.get_stage()) {
	case CharacterStage::CHOOSE_RACE: {
		auto popped = _stages.back();
		_candidate = popped;
		_candidate.set_stage(CharacterStage::ENTER_NAME);
		_stages.pop_back();
		_display->window->input_mode = WindowInputMode::INPUT_TEXT;
		_set_progress_panel_contents();
	} break;
	case CharacterStage::CHOOSE_ALIGNMENT: {
		auto popped = _stages.back();
		_candidate = popped;
		_candidate.set_stage(CharacterStage::CHOOSE_RACE);
		_stages.pop_back();
		_race_menu->choose(_candidate.get_race());
		_set_info_panel_contents(_race_menu->selected);
		_display->window->input_mode = WindowInputMode::NORMAL;
		_set_progress_panel_contents();
	} break;
	case CharacterStage::ALLOCATE_STATS: {
		auto popped = _stages.back();
		_candidate = popped;
		_candidate.set_stage(CharacterStage::CHOOSE_ALIGNMENT);
		_stages.pop_back();
		_alignment_menu->choose(_candidate.get_alignment());
		_set_info_panel_contents(_alignment_menu->selected);
		_display->window->input_mode = WindowInputMode::NORMAL;
		_ap->valid = false;
		_set_progress_panel_contents();
	} break;
	case CharacterStage::CHOOSE_CLASS: {
		auto popped = _stages.back();
		_candidate = popped;
		_candidate.set_stage(CharacterStage::ALLOCATE_STATS);
		_stages.pop_back();
		_display->window->input_mode = WindowInputMode::ALLOCATE_STATS;
		_attribute_menu->selected = _attribute_menu->items.begin();
		_ap->set();
		_ad->valid = false;
		_set_info_panel_contents(_attribute_menu->selected);
		_set_progress_panel_contents();
	} break;
	case CharacterStage::CHOOSE_PORTRAIT: {
		auto popped = _stages.back();
		_candidate = popped;
		_candidate.set_stage(CharacterStage::CHOOSE_CLASS);
		_stages.pop_back();
		_class_menu->choose(_candidate.get_class());
		_set_info_panel_contents(_class_menu->selected);
		_display->window->input_mode = WindowInputMode::NORMAL;
		_ap->valid = false;
		_ad->set();
		_set_progress_panel_contents();
	}
	default:
		break;
	}
}

auto Sorcery::Create::_go_to_next_stage() -> void {

	switch (_candidate.get_stage()) {
	case CharacterStage::ENTER_NAME: {
		auto to_push(_candidate);
		_stages.emplace_back(to_push);
		_candidate.set_stage(CharacterStage::CHOOSE_RACE);
		_display->window->input_mode = WindowInputMode::NORMAL;
		_race_menu->selected = _race_menu->items.begin();
		_set_info_panel_contents(_race_menu->selected);
		_set_progress_panel_contents();
	} break;
	case CharacterStage::CHOOSE_RACE: {
		auto to_push(_candidate);
		_stages.emplace_back(to_push);
		_candidate.set_stage(CharacterStage::CHOOSE_ALIGNMENT);
		_display->window->input_mode = WindowInputMode::NORMAL;
		_alignment_menu->selected = _alignment_menu->items.begin();
		_set_info_panel_contents(_alignment_menu->selected);
		_set_progress_panel_contents();
	} break;
	case CharacterStage::CHOOSE_ALIGNMENT: {
		auto to_push(_candidate);
		_stages.emplace_back(to_push);
		_candidate.set_stage(CharacterStage::ALLOCATE_STATS);
		_display->window->input_mode = WindowInputMode::ALLOCATE_STATS;
		_attribute_menu->selected = _attribute_menu->items.begin();
		_ap->set();
		_set_info_panel_contents(_attribute_menu->selected);
		_set_progress_panel_contents();
	} break;
	case CharacterStage::ALLOCATE_STATS: {
		auto to_push(_candidate);
		_stages.emplace_back(to_push);
		_candidate.set_stage(CharacterStage::CHOOSE_CLASS);
		_display->window->input_mode = WindowInputMode::NORMAL;
		_ap->valid = false;
		_ad->set();

		// Set and enable the class menu depending on the possible classes!
		_set_classes_menu();
		_class_menu->choose_first();
		_set_info_panel_contents(_class_menu->selected);
		_set_progress_panel_contents();
	} break;
	case CharacterStage::CHOOSE_CLASS: {
		auto to_push(_candidate);
		_stages.emplace_back(to_push);
		_candidate.set_stage(CharacterStage::CHOOSE_PORTRAIT);
		_display->window->input_mode = WindowInputMode::NORMAL; // TODO: Portrait!
		// set portrait class
		_set_progress_panel_contents();
	} break;
	default:
		break;
	}
}

auto Sorcery::Create::_do_event_loop() -> std::optional<ModuleResult> {

	// And do the main loop
	while (_window->isOpen()) {
		sf::Event event{};
		while (_window->pollEvent(event)) {

			auto const module_result = _handle_input(event);
			if (module_result) {
				if (module_result.value() == ModuleResult::CLOSE)
					return ModuleResult::CLOSE;
				if (module_result.value() == ModuleResult::BACK) {

					// Just skip a frame refresh at this point to avoid calling _generate_character
					// with a back result which will result in a pop back of the name since we have
					// already set the state back to ENTER_NAME (TODO: why does this happen?)
					_go_to_previous_stage();
					continue;
				}
				if (module_result.value() == ModuleResult::EXIT)
					return ModuleResult::EXIT;
				if (module_result.value() == ModuleResult::CANCEL)
					return ModuleResult::CANCEL;
				if (module_result.value() == ModuleResult::NEXT)
					_go_to_next_stage();
			}

			auto result = _update_character(event);
			if (result) {

				// TODO: save character or reject it
			}

			_window->clear();

			_draw();
			_window->display();
		}
	}

	return std::nullopt;
}

auto Sorcery::Create::_handle_input(const sf::Event &event) -> std::optional<ModuleResult> {

	// Check for Window Close
	if (event.type == sf::Event::Closed)
		return ModuleResult::EXIT;
	else if (_system->input->check_for_event(WindowInput::CANCEL, event))
		return ModuleResult::CANCEL;
	else if (_system->input->check_for_event(WindowInput::BACK, event)) {
		if (_candidate.get_stage() == CharacterStage::ENTER_NAME) {

			// Back in Enter Name Stage only works if you have no name selected
			if (_candidate.get_name().length() == 0)
				return ModuleResult::DELETE;

		} else if (_candidate.get_stage() == CharacterStage::ALLOCATE_STATS) {

			// Back in Allocate Stats only works if you have not allocated any points!
			if (_candidate.get_bonus_points_to_allocate() == _candidate.get_starting_bonus_points())
				return ModuleResult::BACK;
		} else
			return ModuleResult::BACK;

	} else if (_system->input->check_for_event(WindowInput::DELETE, event)) {

		// Check for Deletes but map them to Back where needed
		if (_candidate.get_stage() == CharacterStage::ENTER_NAME) {
			if (_candidate.get_name().length() == 0)
				return ModuleResult::BACK;
			else
				return ModuleResult::DELETE;
		} else if (_candidate.get_stage() == CharacterStage::ALLOCATE_STATS) {

			// Back in Allocate Stats only works if you have not allocated any points!
			if (_candidate.get_bonus_points_to_allocate() == _candidate.get_starting_bonus_points())
				return ModuleResult::BACK;
		} else
			return ModuleResult::BACK;
	} else if (_system->input->check_for_event(WindowInput::CONFIRM, event)) {

		// Only proceed if we have spent all the points!
		if (_candidate.get_stage() == CharacterStage::ALLOCATE_STATS) {
			if (_candidate.get_bonus_points_to_allocate() == 0)
				return ModuleResult::NEXT;
		}
	}

	return std::nullopt;
}

auto Sorcery::Create::_update_character(const sf::Event &event) -> std::optional<ModuleResult> {

	if (_candidate.get_stage() == CharacterStage::ENTER_NAME) {

		std::string candidate_name{_candidate.get_name()};
		if (_system->input->check_for_event(WindowInput::MOVE, event)) {

			sf::Vector2f mouse_pos{static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window))};
			std::optional<std::string> mouse_selected{
				_keyboard->set_mouse_selected(_keyb_c, mouse_pos)};
			if (mouse_selected)
				_keyboard->selected = mouse_selected.value();

		} else if ((_system->input->check_for_event(WindowInput::ALPHANUMERIC, event)) ||
				   (_system->input->check_for_event(WindowInput::SPACE, event))) {
			if (candidate_name.length() < 24) {
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
		} else if ((_system->input->check_for_event(WindowInput::DELETE, event)) ||
				   (_system->input->check_for_event(WindowInput::BACK, event))) {
			if (candidate_name.length() > 0) {
				candidate_name.pop_back();
				_candidate.set_name(candidate_name);
				_keyboard->selected = "Del";
			}
		} else if (_system->input->check_for_event(WindowInput::SELECT, event)) {
			if (_keyboard->selected == "End") {
				if (TRIM_COPY(candidate_name).length() > 0) {
					_candidate.set_name(candidate_name);
					_candidate.set_stage(CharacterStage::CHOOSE_RACE);
					_go_to_next_stage();
					return std::nullopt;
				}
			} else if (_keyboard->selected == "Spc") {
				if (candidate_name.length() < 24) {
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
		} else if (_system->input->check_for_event(WindowInput::CONFIRM_NO_SPACE, event)) {

			if (_keyboard->selected == "End") {
				if (TRIM_COPY(candidate_name).length() > 0) {
					_candidate.set_name(candidate_name);
					_go_to_next_stage();
					return std::nullopt;
				}
			} else {
				if (TRIM_COPY(candidate_name).length() > 0) {
					_candidate.set_name(candidate_name);
					_go_to_next_stage();
					return std::nullopt;
				}
			}
		} else if (_system->input->check_for_event(WindowInput::LEFT, event))
			_keyboard->set_selected(WindowInput::LEFT);
		else if (_system->input->check_for_event(WindowInput::RIGHT, event))
			_keyboard->set_selected(WindowInput::RIGHT);
		else if (_system->input->check_for_event(WindowInput::UP, event))
			_keyboard->set_selected(WindowInput::UP);
		else if (_system->input->check_for_event(WindowInput::DOWN, event))
			_keyboard->set_selected(WindowInput::DOWN);

		return std::nullopt;

	} else if (_candidate.get_stage() == CharacterStage::CHOOSE_RACE) {

		std::optional<std::vector<MenuEntry>::const_iterator> selected{_race_menu->selected};
		if (_system->input->check_for_event(WindowInput::UP, event))
			selected = _race_menu->choose_previous();
		else if (_system->input->check_for_event(WindowInput::DOWN, event))
			selected = _race_menu->choose_next();
		else if (_system->input->check_for_event(WindowInput::MOVE, event))
			selected = _race_menu->set_mouse_selected(
				static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
		else if (_system->input->check_for_event(WindowInput::CONFIRM, event)) {

			// We have selected something from the menu
			if (selected) {

				switch (const MenuItem option_chosen{(*selected.value()).item}) {
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
					break;
				}
				_go_to_next_stage();
				return std::nullopt;
			}
		}

		_set_info_panel_contents(_race_menu->selected);

		return std::nullopt;
	} else if (_candidate.get_stage() == CharacterStage::CHOOSE_ALIGNMENT) {

		std::optional<std::vector<MenuEntry>::const_iterator> selected{_alignment_menu->selected};
		if (_system->input->check_for_event(WindowInput::UP, event))
			selected = _alignment_menu->choose_previous();
		else if (_system->input->check_for_event(WindowInput::DOWN, event))
			selected = _alignment_menu->choose_next();
		else if (_system->input->check_for_event(WindowInput::MOVE, event))
			selected = _alignment_menu->set_mouse_selected(
				static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
		else if (_system->input->check_for_event(WindowInput::CONFIRM, event)) {

			// We have selected something from the menu
			if (selected) {

				switch (const MenuItem option_chosen{(*selected.value()).item}) {
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
					break;
				}

				// TODO: don't like the order of this
				_go_to_next_stage();
				_candidate.set_starting_attributes();
				_ap->set();
				return std::nullopt;
			}
		}

		_set_info_panel_contents(_alignment_menu->selected);

		return std::nullopt;
	} else if (_candidate.get_stage() == CharacterStage::ALLOCATE_STATS) {

		std::optional<std::vector<MenuEntry>::const_iterator> selected{_attribute_menu->selected};
		if (_system->input->check_for_event(WindowInput::UP, event))
			selected = _attribute_menu->choose_previous();
		else if (_system->input->check_for_event(WindowInput::DOWN, event))
			selected = _attribute_menu->choose_next();
		else if (_system->input->check_for_event(WindowInput::MOVE, event))
			selected = _attribute_menu->set_mouse_selected(
				static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
		else if ((_system->input->check_for_event(WindowInput::LEFT, event)) ||
				 (_system->input->check_for_event(WindowInput::BACK, event))) {

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
					if (_candidate.get_bonus_points_to_allocate() <
						_candidate.get_starting_bonus_points()) {
						if (_candidate.get_attribute(stat_to_adjust.value()) >
							_candidate.get_starting_attribute(stat_to_adjust.value())) {
							_candidate.set_attribute(stat_to_adjust.value(), -1);
							_candidate.set_bonus_points_to_allocate(
								_candidate.get_bonus_points_to_allocate() + 1);
						}
					}
				}
				_candidate.set_possible_classes();
			}
		} else if ((_system->input->check_for_event(WindowInput::RIGHT, event)) ||
				   (_system->input->check_for_event(WindowInput::CONFIRM, event))) {

			if ((_system->input->check_for_event(WindowInput::CONFIRM, event)) &&
				(_candidate.get_bonus_points_to_allocate() == 0) &&
				(_candidate.get_number_possible_classes() > 0)) {

				_go_to_next_stage();
				_ap->set();
				return std::nullopt;
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

					if (_candidate.get_bonus_points_to_allocate() > 0) {
						if (_candidate.get_attribute(stat_to_adjust.value()) < 18) {
							_candidate.set_attribute(stat_to_adjust.value(), 1);
							_candidate.set_bonus_points_to_allocate(
								_candidate.get_bonus_points_to_allocate() - 1);
						}
					}
				}
				_candidate.set_possible_classes();
			}

			_set_info_panel_contents(_attribute_menu->selected);
		}

		_ap->set();
		_set_info_panel_contents(_attribute_menu->selected);
		return std::nullopt;
	} else if (_candidate.get_stage() == CharacterStage::CHOOSE_CLASS) {

		std::optional<std::vector<MenuEntry>::const_iterator> class_selected{_class_menu->selected};
		if (_system->input->check_for_event(WindowInput::UP, event))
			class_selected = _class_menu->choose_previous();
		else if (_system->input->check_for_event(WindowInput::DOWN, event))
			class_selected = _class_menu->choose_next();
		else if (_system->input->check_for_event(WindowInput::MOVE, event))
			class_selected = _class_menu->set_mouse_selected(
				static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
		else if (_system->input->check_for_event(WindowInput::CONFIRM, event)) {

			// We have selected something from the menu
			if (class_selected) {

				switch (const MenuItem option_chosen{(*class_selected.value()).item}) {
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
					break;
				}

				_go_to_next_stage();
				return std::nullopt;
			}
		}

		_set_info_panel_contents(_class_menu->selected);
		return std::nullopt;
	}

	return std::nullopt;
}

auto Sorcery::Create::_set_progress_panel_contents() -> void {

	switch (_candidate.get_stage()) {
	case CharacterStage::ENTER_NAME: {
		for (auto &progress : _progress)
			progress = std::nullopt;
	} break;
	case CharacterStage::CHOOSE_RACE: {
		_progress[0] = sf::Text();
		_progress[1] = std::nullopt; // TODO: ranges library has something for last 4 elements?
		_progress[2] = std::nullopt;
		_progress[3] = std::nullopt;
		_progress[4] = std::nullopt;
	} break;
	case CharacterStage::CHOOSE_ALIGNMENT: {
		_progress[0] = sf::Text();
		auto stage_2 = _candidate.get_icon(CharacterStage::CHOOSE_RACE);
		stage_2->setPosition((*_display->layout)["create:stage_2_icon"].x,
			(*_display->layout)["create:stage_2_icon"].y);
		_progress[1] = stage_2;
		_progress[2] = std::nullopt;
		_progress[3] = std::nullopt;
		_progress[4] = std::nullopt;

	} break;
	case CharacterStage::ALLOCATE_STATS: {
		_progress[0] = sf::Text();
		auto stage_2 = _candidate.get_icon(CharacterStage::CHOOSE_RACE);
		stage_2->setPosition((*_display->layout)["create:stage_2_icon"].x,
			(*_display->layout)["create:stage_2_icon"].y);
		_progress[1] = stage_2;
		auto stage_3 = _candidate.get_icon(CharacterStage::CHOOSE_ALIGNMENT);
		stage_3->setPosition((*_display->layout)["create:stage_3_icon"].x,
			(*_display->layout)["create:stage_3_icon"].y);
		_progress[2] = stage_3;
		_progress[3] = std::nullopt;
		_progress[4] = std::nullopt;
	} break;
	case CharacterStage::CHOOSE_CLASS: {

		// progress[3] is a sf:drawable
		_progress[0] = sf::Text();
		auto stage_2 = _candidate.get_icon(CharacterStage::CHOOSE_RACE);
		stage_2->setPosition((*_display->layout)["create:stage_2_icon"].x,
			(*_display->layout)["create:stage_2_icon"].y);
		_progress[1] = stage_2;
		auto stage_3 = _candidate.get_icon(CharacterStage::CHOOSE_ALIGNMENT);
		stage_3->setPosition((*_display->layout)["create:stage_3_icon"].x,
			(*_display->layout)["create:stage_3_icon"].y);
		_progress[2] = stage_3;
		_progress[3] = std::nullopt;
		_progress[4] = std::nullopt;
	} break;
	case CharacterStage::CHOOSE_PORTRAIT: {
		// progress[3] is a sf:drawable
		_progress[0] = sf::Text();
		auto stage_2 = _candidate.get_icon(CharacterStage::CHOOSE_RACE);
		stage_2->setPosition((*_display->layout)["create:stage_2_icon"].x,
			(*_display->layout)["create:stage_2_icon"].y);
		_progress[1] = stage_2;
		auto stage_3 = _candidate.get_icon(CharacterStage::CHOOSE_ALIGNMENT);
		stage_3->setPosition((*_display->layout)["create:stage_3_icon"].x,
			(*_display->layout)["create:stage_3_icon"].y);
		_progress[2] = stage_3;
		_progress[3] = std::nullopt;
		auto stage_5 = _candidate.get_icon(CharacterStage::CHOOSE_CLASS);
		stage_5->setPosition((*_display->layout)["create:stage_5_icon"].x,
			(*_display->layout)["create:stage_5_icon"].y);
		_progress[4] = stage_5;
	}
	default:
		break;
	}
}

auto Sorcery::Create::_set_classes_menu() -> void {

	auto possible_classes{_candidate.get_possible_classes()};
	(*_class_menu)[0].enabled = possible_classes[CharacterClass::SAMURAI];
	(*_class_menu)[1].enabled = possible_classes[CharacterClass::FIGHTER];
	(*_class_menu)[2].enabled = possible_classes[CharacterClass::LORD];
	(*_class_menu)[3].enabled = possible_classes[CharacterClass::THIEF];
	(*_class_menu)[4].enabled = possible_classes[CharacterClass::NINJA];
	(*_class_menu)[5].enabled = possible_classes[CharacterClass::PRIEST];
	(*_class_menu)[6].enabled = possible_classes[CharacterClass::BISHOP];
	(*_class_menu)[7].enabled = possible_classes[CharacterClass::MAGE];
}

auto Sorcery::Create::_set_info_panel_contents(std::vector<Sorcery::MenuEntry>::const_iterator it)
	-> void {

	// Set the Text
	if ((*it).type == MenuItemType::ENTRY) {
		std::string ip_contents{(*it).hint};
		_ip->set_text(ip_contents);
		_ip->valid = true;
	} else {
		_ip->valid = false;
	}

	// Set the Icon
	if ((*it).type == MenuItemType::ENTRY)
		_ip->set_icon((*it).item);
}

auto Sorcery::Create::_draw() -> void {

	double lerp{_graphics->animation->colour_lerp};
	sf::Text name_text;
	std::string display_name{};

	// Display Components
	_display->display_components("create");

	// Custom Layering
	_window->draw(_bg);

	// And draw the current state of the character!
	if (_candidate.get_stage() == CharacterStage::ENTER_NAME) {

		_display->display_components(
			"character_create_stage_1", _candidate.sprites, _candidate.texts, _candidate.frames);

		// TODO: use character-<draw for this!
		display_name = _candidate.get_name() + "_";
		_display->window->draw_text(name_text, _name_c, display_name, lerp);

		// Draw the On Screen Keyboard
		_keyboard->set_selected_background();
		_window->draw(*_keyboard);
	} else if (_candidate.get_stage() == CharacterStage::CHOOSE_RACE) {

		_display->display_components(
			"character_create_stage_2", _candidate.sprites, _candidate.texts, _candidate.frames);

		double lerp{_graphics->animation->colour_lerp};
		_race_menu->generate((*_display->layout)["character_create_stage_2:menu"], lerp);
		const sf::Vector2f menu_pos((*_display->layout)["character_create_stage_2:menu"].x,
			(*_display->layout)["character_create_stage_2:menu"].y);
		_race_menu->setPosition(menu_pos);
		_window->draw(*_race_menu);

		// Display bottom text depending on the menu item selected
		if (_ip->valid) {
			_ip->setPosition(_ip_race_c.x, _ip_race_c.y);
			_window->draw(*_ip);
		}
	} else if (_candidate.get_stage() == CharacterStage::CHOOSE_ALIGNMENT) {

		_display->display_components(
			"character_create_stage_3", _candidate.sprites, _candidate.texts, _candidate.frames);

		double lerp{_graphics->animation->colour_lerp};
		_alignment_menu->generate((*_display->layout)["character_create_stage_3:menu"], lerp);
		const sf::Vector2f menu_pos((*_display->layout)["character_create_stage_3:menu"].x,
			(*_display->layout)["character_create_stage_3:menu"].y);
		_alignment_menu->setPosition(menu_pos);
		_window->draw(*_alignment_menu);

		// Display bottom text depending on the menu item selected
		if (_ip->valid) {
			_ip->setPosition(_ip_alignment_c.x, _ip_alignment_c.y);
			_window->draw(*_ip);
		}
	} else if (_candidate.get_stage() == CharacterStage::ALLOCATE_STATS) {

		_display->display_components(
			"character_create_stage_4", _candidate.sprites, _candidate.texts, _candidate.frames);

		double lerp{_graphics->animation->colour_lerp};
		_attribute_menu->generate((*_display->layout)["character_create_stage_4:menu"], lerp);
		const sf::Vector2f menu_pos((*_display->layout)["character_create_stage_4:menu"].x,
			(*_display->layout)["character_create_stage_4:menu"].y);
		_attribute_menu->setPosition(menu_pos);
		_window->draw(*_attribute_menu);

		if (_ap->valid) {
			_ap->setPosition(_ap_c.x, _ap_c.y);
			_window->draw(*_ap);
		}

		// Display bottom text depending on the menu item selected
		if (_ip->valid) {
			_ip->setPosition(_ip_attribute_c.x, _ip_attribute_c.y);
			_window->draw(*_ip);
		}
	} else if (_candidate.get_stage() == CharacterStage::CHOOSE_CLASS) {
		_display->display_components(
			"character_create_stage_5", _candidate.sprites, _candidate.texts, _candidate.frames);

		double lerp{_graphics->animation->colour_lerp};
		_class_menu->generate((*_display->layout)["character_create_stage_5:menu"], lerp);
		const sf::Vector2f menu_pos((*_display->layout)["character_create_stage_5:menu"].x,
			(*_display->layout)["character_create_stage_5:menu"].y);
		_class_menu->setPosition(menu_pos);
		_window->draw(*_class_menu);

		if (_ap->valid) {
			_ap->setPosition(_ap_c.x, _ap_c.y);
			_window->draw(*_ap);
		}

		// Display bottom text depending on the menu item selected
		if (_ip->valid) {
			_ip->setPosition(_ip_class_c.x, _ip_class_c.y);
			_window->draw(*_ip);
		}

		// And the Attribute Bar
		if (_ad->valid) {
			_ad->setPosition(_ad_c.x, _ad_c.y);
			_window->draw(*_ad);
		}
	} else if (_candidate.get_stage() == CharacterStage::CHOOSE_PORTRAIT) {

		_display->display_components(
			"character_create_stage_6", _candidate.sprites, _candidate.texts, _candidate.frames);

		// double lerp{_graphics->animation->colour_lerp};

		// todo stuff here

		// And the Attribute Bar
		if (_ad->valid) {
			_ad->setPosition(_ad_c.x, _ad_c.y);
			_window->draw(*_ad);
		}
	}

	// Draw the progress bars (TODO: can't use a visit lambda here for some reason)
	for (auto &item : _progress) {
		if (item) {
			if (std::holds_alternative<sf::Text>(item.value())) {
				auto text{std::get<sf::Text>(item.value())};
				_display->window->draw_text(
					text, (*_display->layout)["create:stage_1_name"], _candidate.get_name());
			} else if (std::holds_alternative<sf::Sprite>(item.value())) {
				auto sprite{std::get<sf::Sprite>(item.value())};
				_window->draw(sprite);
			}
		}
	}

	// And finally the Cursor
	_display->display_cursor();
}
