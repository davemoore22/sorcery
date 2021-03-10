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
Sorcery::Create::Create(System &system, Display &display, Graphics &graphics)
	: _system{system}, _display{display}, _graphics{graphics} {

	// Get the Window and Graphics to Display
	_window = _display.window->get_window();

	// Get the Background Display Components
	_display.generate_components("create");

	// Create the Candidate Character
	_candidate = std::make_shared<Character>(system, display, graphics);

	// Create the On-Screen Keyboard
	_keyboard = std::make_shared<Keyboard>(system, display, graphics);

	// Get the Infopanel
	_ip = std::make_shared<InfoPanel>(_system, _display, _graphics);

	// Get the Allocate Stat Panel
	_ap = std::make_shared<AllocatePanel>(_system, _display, _graphics, _candidate.get());

	// Layout Information
	_name_c = Component((*_display.layout)["character_create_stage_1:name_candidate"]);
	_keyb_c = Component((*_display.layout)["character_create_stage_1:keyboard"]);
	_ip_race_c = Component((*_display.layout)["character_create_stage_2:info_panel"]);
	_ip_alignment_c = Component((*_display.layout)["character_create_stage_3:info_panel"]);
	_ip_attribute_c = Component((*_display.layout)["character_create_stage_4:info_panel"]);
	_ap_c = Component((*_display.layout)["character_create_stage_4:allocate_panel"]);

	// Menus
	_race_menu =
		std::make_shared<Menu>(_system, _display, _graphics, MenuType::CHOOSE_CHARACTER_RACE);
	_alignment_menu =
		std::make_shared<Menu>(_system, _display, _graphics, MenuType::CHOOSE_CHARACTER_ALIGNMENT);
	_attribute_menu = std::make_shared<Menu>(
		_system, _display, _graphics, MenuType::ALLOCATE_CHARACTER_ATTRIBUTES);
}

// Standard Destructor
Sorcery::Create::~Create() {

	_display.stop_background_movie();
}

auto Sorcery::Create::start() -> std::optional<MenuItem> {

	// Get the custom components
	const Component bg_c{(*_display.layout)["create:background"]};
	const sf::IntRect bg_rect(1147, 249, 773, 388);
	_bg.setTexture(_system.resources->textures[TOWN_TEXTURE]);
	_bg.setTextureRect(bg_rect);
	_bg.setScale(bg_c.scale, bg_c.scale);
	_bg.setPosition(_display.window->get_x(_bg, bg_c.x), _display.window->get_y(_bg, bg_c.y));

	// Don't display the info panel yet
	_ip->valid = false;
	_ap->valid = false;

	// Get the Keyboard
	_keyboard->setPosition(_keyb_c.x, _keyb_c.y);
	_ap->setPosition(_ap_c.x, _ap_c.y);

	const Component name_c{(*_display.layout)["character_create_stage_1:name_candidate"]};
	_candidate->set_stage(CharacterStage::ENTER_NAME);

	// Clear the window
	_window->clear();

	// Play the background movie!
	_display.fit_background_movie();
	_display.start_background_movie();

	// Will need to change this for the seven screens as needed
	_display.window->input_mode = WindowInputMode::INPUT_TEXT;

	auto module_result = _do_event_loop();
	if (module_result == ModuleResult::EXIT)
		_window->close();

	return std::nullopt;
}

auto Sorcery::Create::stop() -> void {

	_display.stop_background_movie();
}

auto Sorcery::Create::_go_to_previous_stage() -> void {

	switch (_candidate->get_stage()) {

	case CharacterStage::CHOOSE_RACE:
		_candidate->set_stage(CharacterStage::ENTER_NAME);
		_display.window->input_mode = WindowInputMode::INPUT_TEXT;
		break;
	case CharacterStage::CHOOSE_ALIGNMENT:
		_candidate->set_stage(CharacterStage::CHOOSE_RACE);
		_race_menu->choose(_candidate->get_race());
		_set_info_panel_contents(_race_menu->selected);
		_display.window->input_mode = WindowInputMode::NORMAL;
		break;
	case CharacterStage::ALLOCATE_STATS:
		_candidate->set_stage(CharacterStage::CHOOSE_ALIGNMENT);
		_alignment_menu->choose(_candidate->get_alignment());
		_set_info_panel_contents(_alignment_menu->selected);
		_display.window->input_mode = WindowInputMode::NORMAL;
		_ap->valid = false;
	default:
		break;
	}
}

auto Sorcery::Create::_go_to_next_stage() -> void {

	switch (_candidate->get_stage()) {
	case CharacterStage::ENTER_NAME:
		_candidate->set_stage(CharacterStage::CHOOSE_RACE);
		_display.window->input_mode = WindowInputMode::NORMAL;
		_race_menu->selected = _race_menu->items.begin();
		_set_info_panel_contents(_race_menu->selected);
		break;
	case CharacterStage::CHOOSE_RACE:
		_candidate->set_stage(CharacterStage::CHOOSE_ALIGNMENT);
		_display.window->input_mode = WindowInputMode::NORMAL;
		_alignment_menu->selected = _alignment_menu->items.begin();
		_set_info_panel_contents(_alignment_menu->selected);
		break;
	case CharacterStage::CHOOSE_ALIGNMENT:
		_candidate->set_stage(CharacterStage::ALLOCATE_STATS);
		_display.window->input_mode = WindowInputMode::ALLOCATE_STATS;
		_attribute_menu->selected = _attribute_menu->items.begin();
		_ap->set();
		_set_info_panel_contents(_attribute_menu->selected);
		break;
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
				if (module_result.value() == ModuleResult::BACK)
					_go_to_previous_stage();
				if (module_result.value() == ModuleResult::EXIT)
					return ModuleResult::EXIT;
				if (module_result.value() == ModuleResult::CANCEL)
					return ModuleResult::CANCEL;
				if (module_result.value() == ModuleResult::NEXT)
					_go_to_next_stage();
			}

			auto result = _generate_character(event);
			if (result) {
				// TODO: save character or reject it
			}

			_window->clear();

			_display.start_background_movie();
			_display.update_background_movie();
			_display.draw_background_movie();

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
	else if (_system.input->check_for_event(WindowInput::CANCEL, event))
		return ModuleResult::CANCEL;
	else if (_system.input->check_for_event(WindowInput::BACK, event)) {
		if (_candidate->get_stage() == CharacterStage::ENTER_NAME) {

			// Back in Enter Name Stage only works if you have no name selected
			if (_candidate->get_name().length() == 0)
				return ModuleResult::BACK;

		} else if (_candidate->get_stage() == CharacterStage::ALLOCATE_STATS) {

			// Back in Allocate Stats only works if you have not allocated any points!
			if (_candidate->get_bonus_points_to_allocate() ==
				_candidate->get_starting_bonus_points())
				return ModuleResult::BACK;
		} else
			return ModuleResult::BACK;
	}

	return std::nullopt;
}

auto Sorcery::Create::_generate_character(const sf::Event &event) -> std::optional<ModuleResult> {

	if (_candidate->get_stage() == CharacterStage::ENTER_NAME) {

		std::string candidate_name{};
		std::optional<std::string> mouse_selected{};
		sf::Vector2f mouse_pos;

		candidate_name = _candidate->get_name();
		if (_system.input->check_for_event(WindowInput::MOVE, event)) {

			mouse_pos = static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window));
			mouse_selected = _keyboard->set_mouse_selected(_keyb_c, mouse_pos);
			if (mouse_selected)
				_keyboard->selected = mouse_selected.value();

		} else if ((_system.input->check_for_event(WindowInput::ALPHANUMERIC, event)) ||
				   (_system.input->check_for_event(WindowInput::SPACE, event))) {
			if (candidate_name.length() < 24) {
				candidate_name += static_cast<char>(event.text.unicode);
				_candidate->set_name(candidate_name);
			}
			if (static_cast<char>(event.text.unicode) == ' ') {
				std::string key_pressed{"Spc"};
				_keyboard->selected = key_pressed;
			} else {
				std::string key_pressed{};
				key_pressed.push_back(static_cast<char>(event.text.unicode));
				_keyboard->selected = key_pressed;
			}
		} else if ((_system.input->check_for_event(WindowInput::DELETE, event)) ||
				   (_system.input->check_for_event(WindowInput::BACK, event))) {
			if (candidate_name.length() > 0) {
				candidate_name.pop_back();
				_candidate->set_name(candidate_name);
				std::string key_pressed{"Del"};
				_keyboard->selected = key_pressed;
			}
		} else if (_system.input->check_for_event(WindowInput::SELECT, event)) {
			if (_keyboard->selected == "End") {
				if (TRIM_COPY(candidate_name).length() > 0) {
					_candidate->set_stage(CharacterStage::CHOOSE_RACE);
					_display.window->input_mode = WindowInputMode::NORMAL;
				}
			} else if (_keyboard->selected == "Spc") {
				if (candidate_name.length() < 24) {
					candidate_name += " ";
					_candidate->set_name(candidate_name);
				}
			} else if (_keyboard->selected == "Del") {
				if (candidate_name.length() > 0) {
					candidate_name.pop_back();
					_candidate->set_name(candidate_name);
				}
			} else {
				candidate_name += _keyboard->selected;
				_candidate->set_name(candidate_name);
			}
		} else if (_system.input->check_for_event(WindowInput::CONFIRM_NO_SPACE, event)) {

			if (_keyboard->selected == "End") {
				if (TRIM_COPY(candidate_name).length() > 0) {
					_go_to_next_stage();
					_candidate->set_name(candidate_name);
					return std::nullopt;
				}
			} else {
				if (TRIM_COPY(candidate_name).length() > 0) {
					_go_to_next_stage();
					_candidate->set_name(candidate_name);
					return std::nullopt;
				}
			}
		} else if (_system.input->check_for_event(WindowInput::LEFT, event))
			_keyboard->set_selected(WindowInput::LEFT);
		else if (_system.input->check_for_event(WindowInput::RIGHT, event))
			_keyboard->set_selected(WindowInput::RIGHT);
		else if (_system.input->check_for_event(WindowInput::UP, event))
			_keyboard->set_selected(WindowInput::UP);
		else if (_system.input->check_for_event(WindowInput::DOWN, event))
			_keyboard->set_selected(WindowInput::DOWN);

		return std::nullopt;

	} else if (_candidate->get_stage() == CharacterStage::CHOOSE_RACE) {

		std::optional<std::vector<MenuEntry>::const_iterator> race_selected{_race_menu->selected};
		if (_system.input->check_for_event(WindowInput::UP, event))
			race_selected = _race_menu->choose_previous();
		else if (_system.input->check_for_event(WindowInput::DOWN, event))
			race_selected = _race_menu->choose_next();
		else if (_system.input->check_for_event(WindowInput::MOVE, event))
			race_selected = _race_menu->set_mouse_selected(
				static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
		else if (_system.input->check_for_event(WindowInput::CONFIRM, event)) {

			// We have selected something from the menu
			if (race_selected) {

				switch (const MenuItem option_chosen{(*race_selected.value()).item}) {
				case MenuItem::CR_HUMAN:
					_candidate->set_race(CharacterRace::HUMAN);
					break;
				case MenuItem::CR_ELF:
					_candidate->set_race(CharacterRace::ELF);
					break;
				case MenuItem::CR_DWARF:
					_candidate->set_race(CharacterRace::DWARF);
					break;
				case MenuItem::CR_GNOME:
					_candidate->set_race(CharacterRace::GNOME);
					break;
				case MenuItem::CR_HOBBIT:
					_candidate->set_race(CharacterRace::HOBBIT);
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
	} else if (_candidate->get_stage() == CharacterStage::CHOOSE_ALIGNMENT) {

		std::optional<std::vector<MenuEntry>::const_iterator> alignment_selected{
			_alignment_menu->selected};
		if (_system.input->check_for_event(WindowInput::UP, event))
			alignment_selected = _alignment_menu->choose_previous();
		else if (_system.input->check_for_event(WindowInput::DOWN, event))
			alignment_selected = _alignment_menu->choose_next();
		else if (_system.input->check_for_event(WindowInput::MOVE, event))
			alignment_selected = _alignment_menu->set_mouse_selected(
				static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
		else if (_system.input->check_for_event(WindowInput::CONFIRM, event)) {

			// We have selected something from the menu
			if (alignment_selected) {

				switch (const MenuItem option_chosen{(*alignment_selected.value()).item}) {
				case MenuItem::CA_GOOD:
					_candidate->set_alignment(CharacterAlignment::GOOD);
					break;
				case MenuItem::CA_NEUTRAL:
					_candidate->set_alignment(CharacterAlignment::NEUTRAL);
					break;
				case MenuItem::CA_EVIL:
					_candidate->set_alignment(CharacterAlignment::EVIL);
					break;
				default:
					break;
				}

				// TODO: don't like the order of this
				_go_to_next_stage();
				_candidate->set_starting_attributes();
				_ap->set();
				return std::nullopt;
			}
		}

		_set_info_panel_contents(_alignment_menu->selected);

		return std::nullopt;
	} else if (_candidate->get_stage() == CharacterStage::ALLOCATE_STATS) {
		std::optional<std::vector<MenuEntry>::const_iterator> attribute_selected{
			_attribute_menu->selected};
		if (_system.input->check_for_event(WindowInput::UP, event))
			attribute_selected = _attribute_menu->choose_previous();
		else if (_system.input->check_for_event(WindowInput::DOWN, event))
			attribute_selected = _attribute_menu->choose_next();
		else if (_system.input->check_for_event(WindowInput::MOVE, event))
			attribute_selected = _attribute_menu->set_mouse_selected(
				static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
		else if ((_system.input->check_for_event(WindowInput::LEFT, event)) ||
				 (_system.input->check_for_event(WindowInput::BACK, event))) {

			if (attribute_selected) {
				std::optional<CharacterAttribute> stat_to_adjust{};
				switch (attribute_selected.value()->item) {
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
					if (_candidate->get_bonus_points_to_allocate() <
						_candidate->get_starting_bonus_points()) {
						if (_candidate->get_attribute(stat_to_adjust.value()) >
							_candidate->get_starting_attribute(stat_to_adjust.value())) {
							_candidate->set_attribute(stat_to_adjust.value(), -1);
							_candidate->set_bonus_points_to_allocate(
								_candidate->get_bonus_points_to_allocate() + 1);
						}
					}
				}
			}
		} else if ((_system.input->check_for_event(WindowInput::RIGHT, event)) ||
				   (_system.input->check_for_event(WindowInput::CONFIRM, event))) {
			if (attribute_selected) {
				std::optional<CharacterAttribute> stat_to_adjust{};
				switch (attribute_selected.value()->item) {
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

					if (_candidate->get_bonus_points_to_allocate() > 0) {
						if (_candidate->get_attribute(stat_to_adjust.value()) < 18) {
							_candidate->set_attribute(stat_to_adjust.value(), 1);
							_candidate->set_bonus_points_to_allocate(
								_candidate->get_bonus_points_to_allocate() - 1);
						}
					}
				}
			}
		}

		// LEFT AND RIGHT for add and remove stat, and confirm to select a stat, and if no stat left
		// then confirm will move o9n

		// also need to display possible classes too - using a possible class pane, which also takes
		// in character

		_ap->set();
		_set_info_panel_contents(_attribute_menu->selected);

		return std::nullopt;
	}

	return std::nullopt;
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

	double lerp{_graphics.animation->colour_lerp};
	sf::Text name_text;
	std::string display_name{};

	// Display Components
	_display.display_components("create");

	// Custom Layering
	_window->draw(_bg);

	// And draw the current state of the character!
	if (_candidate->get_stage() == CharacterStage::ENTER_NAME) {

		_display.display_components(
			"character_create_stage_1", _candidate->sprites, _candidate->texts, _candidate->frames);

		// TODO: use character-<draw for this!
		display_name = _candidate->get_name() + "_";
		_display.window->draw_text(name_text, _name_c, display_name, lerp);

		// Draw the On Screen Keyboard
		_keyboard->set_selected_background();
		_window->draw(*_keyboard);
	} else if (_candidate->get_stage() == CharacterStage::CHOOSE_RACE) {

		_display.display_components(
			"character_create_stage_2", _candidate->sprites, _candidate->texts, _candidate->frames);

		double lerp{_graphics.animation->colour_lerp};
		_race_menu->generate((*_display.layout)["character_create_stage_2:menu"], lerp);
		const sf::Vector2f menu_pos((*_display.layout)["character_create_stage_2:menu"].x,
			(*_display.layout)["character_create_stage_2:menu"].y);
		_race_menu->setPosition(menu_pos);
		_window->draw(*_race_menu);

		// Display bottom text depending on the menu item selected
		if (_ip->valid) {
			_ip->setPosition(_ip_race_c.x, _ip_race_c.y);
			_window->draw(*_ip);
		}
	} else if (_candidate->get_stage() == CharacterStage::CHOOSE_ALIGNMENT) {

		_display.display_components(
			"character_create_stage_3", _candidate->sprites, _candidate->texts, _candidate->frames);

		double lerp{_graphics.animation->colour_lerp};
		_alignment_menu->generate((*_display.layout)["character_create_stage_3:menu"], lerp);
		const sf::Vector2f menu_pos((*_display.layout)["character_create_stage_3:menu"].x,
			(*_display.layout)["character_create_stage_3:menu"].y);
		_alignment_menu->setPosition(menu_pos);
		_window->draw(*_alignment_menu);

		// Display bottom text depending on the menu item selected
		if (_ip->valid) {
			_ip->setPosition(_ip_alignment_c.x, _ip_alignment_c.y);
			_window->draw(*_ip);
		}
	} else if (_candidate->get_stage() == CharacterStage::ALLOCATE_STATS) {

		_display.display_components(
			"character_create_stage_4", _candidate->sprites, _candidate->texts, _candidate->frames);

		double lerp{_graphics.animation->colour_lerp};
		_attribute_menu->generate((*_display.layout)["character_create_stage_4:menu"], lerp);
		const sf::Vector2f menu_pos((*_display.layout)["character_create_stage_4:menu"].x,
			(*_display.layout)["character_create_stage_4:menu"].y);
		_attribute_menu->setPosition(menu_pos);
		_window->draw(*_attribute_menu);

		if (_ap->valid) {
			_ap->setPosition(_ap_c.x, _ap_c.y);
			_window->draw(*_ap);
			//_display.display_components("allocate_panel", _ap->sprites, _ap->texts, _ap->frames);
		}

		// Display bottom text depending on the menu item selected
		if (_ip->valid) {
			_ip->setPosition(_ip_attribute_c.x, _ip_attribute_c.y);
			_window->draw(*_ip);
		}

		// and possible classes panel too
	}

	// And finally the Cursor
	_display.display_cursor();
}
