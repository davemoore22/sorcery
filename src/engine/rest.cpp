// Copyright (C) 2023 Dave Moore
//
// This file is part of Sorcery: Shadows under Llylgamyn
//
// Sorcery is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Sorcery is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with sorcery-sfml.  If not, see <http://www.gnu.org/licenses/>.
// Additional permission under GNU GPL version 3 section 7:
//
// If you modify this Program, or any covered work, by linking or combining it
// with the libraries referred to in README (or a modified version of said
// libraries), containing parts covered by the terms of said libraries, the
// licensors of this Program grant you additional permission to convey the
// resulting work.

#include "engine/rest.hpp"

// Standard Constructor
Sorcery::Rest::Rest(System *system, Display *display, Graphics *graphics, Game *game)
	: _system{system}, _display{display}, _graphics{graphics}, _game{game} {

	// Get the Window and Graphics to Display
	_window = _display->window->get_window();

	// Custom Components
	_continue_menu = std::make_unique<Menu>(_system, _display, _graphics, _game, MenuType::CONTINUE);
	_continue_menu->generate((*_display->layout)["rest:continue_menu"]);
	_continue_menu->setPosition(
		_display->get_centre_x(_continue_menu->get_width()), (*_display->layout)["rest:continue_menu"].y);

	_stop_menu = std::make_unique<Menu>(_system, _display, _graphics, _game, MenuType::STOP);
	_stop_menu->generate((*_display->layout)["rest:stop_menu"]);
	_stop_menu->setPosition(_display->get_centre_x(_stop_menu->get_width()), (*_display->layout)["rest:stop_menu"].y);

	// Ugly - need to change this
	_nap_text = sf::Text();
	_no_level_text_1 = sf::Text();
	_no_level_text_2 = sf::Text();
	_level_up_message = sf::Text();
	_level_text = sf::Text();

	Component _smf_c{(*_display->layout)["rest:stop_frame"]};
	_stop_frame = std::make_unique<Frame>(
		_display->ui_texture, _smf_c.w, _smf_c.h, _smf_c.colour, _smf_c.background, _smf_c.alpha);
	_stop_frame->setPosition(
		_display->window->get_x(_stop_frame->sprite, _smf_c.x), _display->window->get_y(_stop_frame->sprite, _smf_c.y));

	Component _cmf_c{(*_display->layout)["rest:continue_frame"]};
	_continue_frame = std::make_unique<Frame>(
		_display->ui_texture, _cmf_c.w, _cmf_c.h, _cmf_c.colour, _cmf_c.background, _cmf_c.alpha);
	_continue_frame->setPosition(_display->window->get_x(_continue_frame->sprite, _cmf_c.x),
		_display->window->get_y(_continue_frame->sprite, _cmf_c.y));

	// Modules
	_party_panel =
		std::make_unique<PartyPanel>(_system, _display, _graphics, _game, (*_display->layout)["global:party_panel"]);
	_results = std::make_unique<TextPanel>(_system, _display, _graphics, (*_display->layout)["rest:results_panel"]);
}

auto Sorcery::Rest::start(Character *character, RestMode mode, RestType type) -> std::optional<MenuItem> {

	_character = character;
	_mode = mode;
	_type = type;

	_level_up = false;
	_birthday = false;

	_level_up_messages.clear();

	auto name{character->get_name()};
	_nap_message = fmt::format("{} {}", name, (*_display->string)["REST_NAPPING"]);
	_recup_message = fmt::format("{} {}", name, (*_display->string)["REST_RECUPERATING"]);

	_display->generate("rest");

	// Clear the window
	_window->clear();

	// Refresh the Party characters
	_party_panel->refresh();

	// Generate the Components
	const Component party_banel_c{(*_display->layout)["global:party_panel"]};
	_party_panel->setPosition(_display->get_centre_x(_party_panel->width), (*_display->layout)["global:party_panel"].y);

	const Component results_c{(*_display->layout)["rest:results_panel"]};
	_results->setPosition((*_display->layout)["rest:results_panel"].pos());

	// Start at the Regen
	_stage = RestStage::REGEN;

	_start = std::nullopt;
	_current_time = std::nullopt;
	_duration = DELAY_RESTING; // ms

	_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
	std::optional<std::vector<MenuEntry>::const_iterator> option_continue{_continue_menu->items.begin()};
	std::optional<std::vector<MenuEntry>::const_iterator> option_stop{_stop_menu->items.begin()};
	bool proceed{false};
	bool skip{false};

	sf::Event event{};
	_start = std::chrono::system_clock::now();
	while (_window->isOpen()) {

		if (_stage == RestStage::REGEN) {

			_current_time = std::chrono::system_clock::now();
			const auto time_elapsed{_current_time.value() - _start.value()};
			if (const auto time_elapsed_msec{std::chrono::duration_cast<std::chrono::milliseconds>(time_elapsed)};
				time_elapsed_msec.count() > _duration)
				proceed = true;

			if ((proceed) && (!skip)) {
				if (_type == RestType::STABLES) {
					if (_stage == RestStage::REGEN) {
						_go_to_results();
						_game->save_game();
						_stage = RestStage::RESULTS;
					}
				} else {
					const auto is_fully_rested_or_stopped{_recuperate()};
					if (is_fully_rested_or_stopped) {
						_go_to_results();
						_game->save_game();
						skip = true;
						_stage = RestStage::RESULTS;

					} else {

						// Restart clock for another round of resting
						proceed = false;
						_start = std::chrono::system_clock::now();
						continue;
					}
				}
			}
		}

		while (_window->pollEvent(event)) {

			if (event.type == sf::Event::Closed)
				return MenuItem::ITEM_ABORT;

			// Handle enabling help overlay
			if (_system->input->check(WindowInput::SHOW_CONTROLS, event)) {
				_display->show_overlay();
				continue;
			} else
				_display->hide_overlay();

			// Stables
			if (_type == RestType::STABLES) {

				if (_stage == RestStage::REGEN) {

					if (_system->input->check(WindowInput::ANYTHING, event)) {
						_go_to_results();
						_stage = RestStage::RESULTS;
					}

				} else if (_stage == RestStage::RESULTS) {

					if (_system->input->check(WindowInput::CANCEL, event))
						return MenuItem::ITEM_CONTINUE;
					else if (_system->input->check(WindowInput::BACK, event))
						return MenuItem::ITEM_CONTINUE;
					else if (_system->input->check(WindowInput::UP, event))
						option_continue = _continue_menu->choose_previous();
					else if (_system->input->check(WindowInput::DOWN, event))
						option_continue = _continue_menu->choose_next();
					else if (_system->input->check(WindowInput::MOVE, event))
						option_continue = _continue_menu->set_mouse_selected(
							static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
					else if (_system->input->check(WindowInput::CONFIRM, event)) {

						if (option_continue) {
							if (const MenuItem option_chosen{(*option_continue.value()).item};
								option_chosen == MenuItem::ITEM_CONTINUE) {
								return MenuItem::ITEM_CONTINUE;
							}
						}
					}
				}
			} else {

				// Do other methods of resting that cost money
				if (_stage == RestStage::REGEN) {

					if (_system->input->check(WindowInput::CANCEL, event))
						return MenuItem::CP_LEAVE;
					else if (_system->input->check(WindowInput::BACK, event))
						return MenuItem::CP_LEAVE;
					else if (_system->input->check(WindowInput::UP, event))
						option_stop = _stop_menu->choose_previous();
					else if (_system->input->check(WindowInput::DOWN, event))
						option_stop = _stop_menu->choose_next();
					else if (_system->input->check(WindowInput::MOVE, event))
						option_stop =
							_stop_menu->set_mouse_selected(static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
					else if (_system->input->check(WindowInput::CONFIRM, event)) {

						if (option_stop) {
							if (const MenuItem option_chosen{(*option_stop.value()).item};
								option_chosen == MenuItem::ITEM_STOP) {
								_go_to_results();
								_stage = RestStage::RESULTS;
								continue;
							}
						}
					}
				} else if (_stage == RestStage::RESULTS) {

					if (_system->input->check(WindowInput::CANCEL, event))
						return MenuItem::CP_LEAVE;
					else if (_system->input->check(WindowInput::BACK, event))
						return MenuItem::CP_LEAVE;
					else if (_system->input->check(WindowInput::UP, event))
						option_continue = _continue_menu->choose_previous();
					else if (_system->input->check(WindowInput::DOWN, event))
						option_continue = _continue_menu->choose_next();
					else if (_system->input->check(WindowInput::MOVE, event))
						option_continue = _continue_menu->set_mouse_selected(
							static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
					else if (_system->input->check(WindowInput::CONFIRM, event)) {

						if (option_continue) {
							if (const MenuItem option_chosen{(*option_continue.value()).item};
								option_chosen == MenuItem::ITEM_CONTINUE) {
								return MenuItem::ITEM_CONTINUE;
							}
						}
					}
				}
			}
		}

		_window->clear();

		_draw();
		_window->display();
	}

	return std::nullopt;
}

auto Sorcery::Rest::_recuperate() -> bool {

	// get hp and gold
	auto gold{_character->get_gold()};
	auto hp{_character->get_current_hp()};
	auto age{_character->get_age()};

	auto inc_hp{0u};
	auto dec_gold{0u};
	auto inc_age{1u};

	switch (_type) {
	case RestType::COT:
		inc_hp = 1;
		dec_gold = 10;
		break;
	case RestType::ECONOMY:
		inc_hp = 3;
		dec_gold = 50;
		break;
	case RestType::MERCHANT:
		inc_hp = 5;
		dec_gold = 200;
		break;
	case RestType::ROYAL:
		inc_hp = 10;
		dec_gold = 500;
		break;
	default:
		break;
	}

	if (gold < dec_gold)
		return true;

	if (hp >= _character->get_max_hp()) {
		if (_character->get_current_hp() > _character->get_max_hp())
			_character->set_current_hp(_character->get_max_hp());
		return true;
	}

	_recup_message_1 = fmt::format("{} ({:>5}/{:>5})", (*_display->string)["REST_HP"], hp, _character->get_max_hp());
	_recup_message_2 = fmt::format("{} {}", (*_display->string)["REST_GOLD"], gold);

	_party_panel->refresh();

	_birthday = age % 52 == 0;
	// need to do happy birthday message;

	_character->set_gold(gold - dec_gold);
	_character->set_current_hp(hp + inc_hp);
	if (_character->get_current_hp() > _character->get_max_hp()) {
		_character->set_current_hp(_character->get_max_hp());
		return true;
	}
	_character->set_age(inc_age);

	return false;
}

auto Sorcery::Rest::_go_to_results() -> void {

	_strings.clear();
	_texts.clear();

	const auto current_xp{_character->get_cur_xp()};
	const auto next_xp{_character->get_next_xp()};
	if (const auto need_xp{next_xp - current_xp}; need_xp < 0) {

		// Level up!
		_level_up = true;
		const auto level_up_results{_character->level_up()};
		_results->set_text(level_up_results);

	} else {

		const auto no_level_up_results{fmt::format("{} {} {}@{}", (*_display->string)["REST_XP_PREFIX_YOU"], need_xp,
			(*_display->string)["REST_XP_SUFFIX_YOU"], (*_display->string)["REST_XP_2_YOU"])};
		_results->set_text(no_level_up_results);
	}

	// both cases restore spells to max!
	_character->replenish_spells();
	_party_panel->refresh();
}

auto Sorcery::Rest::stop() -> void {
}

auto Sorcery::Rest::_draw() -> void {

	// Play the background movie!
	_graphics->tile_bg(_window);

	// Custom Components
	_display->display("rest");
	_window->draw(*_party_panel);

	if (_type == RestType::STABLES) {

		if (_stage == RestStage::REGEN) {

			_display->window->draw_text(_nap_text, (*_display->layout)["rest:nap_text"], _nap_message);

		} else if (_stage == RestStage::RESULTS) {

			if (_results->valid)
				_window->draw(*_results);

			// And the Menu
			_continue_menu->generate((*_display->layout)["rest:continue_menu"]);

			_window->draw(*_continue_frame);
			_window->draw(*_continue_menu);
		}
	} else {

		if (_stage == RestStage::REGEN) {

			_display->window->draw_text(_recup_text, (*_display->layout)["rest:nap_text"], _recup_message);
			_display->window->draw_text(_recup_text_1, (*_display->layout)["rest:recup_text_1"], _recup_message_1);
			_display->window->draw_text(_recup_text_2, (*_display->layout)["rest:recup_text_2"], _recup_message_2);

			_stop_menu->generate((*_display->layout)["rest:stop_menu"]);

			_window->draw(*_stop_frame);
			_window->draw(*_stop_menu);

		} else if (_stage == RestStage::RESULTS) {

			if (_results->valid)
				_window->draw(*_results);

			// And the Menu
			_continue_menu->generate((*_display->layout)["rest:continue_menu"]);

			_window->draw(*_continue_frame);
			_window->draw(*_continue_menu);
		}
	}

	// Always draw the following
	_display->display_overlay();
	_display->display_cursor();
}