// Copyright (C) 2024 Dave Moore
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

#include "modules/rest.hpp"
#include "common/enum.hpp"
#include "common/type.hpp"
#include "core/display.hpp"
#include "core/game.hpp"
#include "core/graphics.hpp"
#include "core/input.hpp"
#include "core/system.hpp"
#include "core/window.hpp"
#include "engine/define.hpp"
#include "resources/componentstore.hpp"
#include "resources/factory.hpp"
#include "resources/stringstore.hpp"
#include "types/character.hpp"
#include "types/component.hpp"

// Standard Constructor
Sorcery::Rest::Rest(
	System *system, Display *display, Graphics *graphics, Game *game)
	: _system{system}, _display{display}, _graphics{graphics}, _game{game} {

	// Get the Window and Graphics to Display
	_window = _display->window->get_window();

	// Custom Components
	_cont_menu = std::make_unique<Menu>(
		_system, _display, _graphics, _game, MTP::CONTINUE);
	_cont_menu->generate((*_display->layout)["rest:continue_menu"]);
	_cont_menu->setPosition(_display->get_centre_x(_cont_menu->get_width()),
		(*_display->layout)["rest:continue_menu"].y);

	_stop_menu =
		std::make_unique<Menu>(_system, _display, _graphics, _game, MTP::STOP);
	_stop_menu->generate((*_display->layout)["rest:stop_menu"]);
	_stop_menu->setPosition(_display->get_centre_x(_stop_menu->get_width()),
		(*_display->layout)["rest:stop_menu"].y);

	// Ugly - need to change this
	_nap_text = sf::Text();
	_no_level_text_1 = sf::Text();
	_no_level_text_2 = sf::Text();
	_level_up_message = sf::Text();
	_level_text = sf::Text();

	Component _smf_c{(*_display->layout)["rest:stop_frame"]};
	_stop_frame = std::make_unique<Frame>(_display->ui_texture, _smf_c.w,
		_smf_c.h, _smf_c.colour, _smf_c.background, _smf_c.alpha);
	_stop_frame->setPosition(
		_display->window->get_x(_stop_frame->sprite, _smf_c.x),
		_display->window->get_y(_stop_frame->sprite, _smf_c.y));

	Component _cmf_c{(*_display->layout)["rest:continue_frame"]};
	_cont_frame = std::make_unique<Frame>(_display->ui_texture, _cmf_c.w,
		_cmf_c.h, _cmf_c.colour, _cmf_c.background, _cmf_c.alpha);
	_cont_frame->setPosition(
		_display->window->get_x(_cont_frame->sprite, _cmf_c.x),
		_display->window->get_y(_cont_frame->sprite, _cmf_c.y));

	// Modules
	_party_panel = std::make_unique<PartyPanel>(_system, _display, _graphics,
		_game, (*_display->layout)["global:party_panel"]);
	_results = std::make_unique<TextPanel>(_system, _display, _graphics,
		(*_display->layout)["rest:results_panel"]);
}

auto Sorcery::Rest::start(Character *character, REM mode, RET type)
	-> std::optional<MIM> {

	_character = character;
	_mode = mode;
	_type = type;

	_level_up = false;
	_birthday = false;

	_level_up_messages.clear();

	auto name{character->get_name()};
	_nap_message =
		fmt::format("{} {}", name, (*_display->string)["REST_NAPPING"]);
	_recup_message =
		fmt::format("{} {}", name, (*_display->string)["REST_RECUPERATING"]);

	_display->generate("rest");

	// Clear the window
	_window->clear();

	// Refresh the Party Characters and the Display
	_party_panel->refresh();
	_party_panel->setPosition(_display->get_centre_x(_party_panel->width),
		(*_display->layout)["global:party_panel"].y);
	_results->setPosition((*_display->layout)["rest:results_panel"].pos());

	// Start at the Regen
	_stage = STR::REGEN;

	_start = std::nullopt;
	_current_time = std::nullopt;
	_duration = DELAY_RESTING; // ms

	_display->set_input_mode(WIM::NAVIGATE_MENU);
	MenuSelect opt_cont{_cont_menu->items.begin()};
	MenuSelect opt_stop{_stop_menu->items.begin()};
	bool proceed{false};
	bool skip{false};

	sf::Event event{};
	_start = std::chrono::system_clock::now();
	while (_window->isOpen()) {

		if (_stage == STR::REGEN) {

			_current_time = std::chrono::system_clock::now();
			const auto elapsed{_current_time.value() - _start.value()};
			if (const auto elapsed_ms{
					std::chrono::duration_cast<std::chrono::milliseconds>(
						elapsed)};
				elapsed_ms.count() > _duration)
				proceed = true;

			if (proceed && !skip) {
				if (_type == RET::STABLES) {
					if (_stage == STR::REGEN) {
						_go_to_results();
						_game->save_game();
						_stage = STR::RESULTS;
					}
				} else {
					const auto is_fully_rested_or_stopped{_recuperate()};
					if (is_fully_rested_or_stopped) {
						_go_to_results();
						_game->save_game();
						skip = true;
						_stage = STR::RESULTS;

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
				return MIM::ITEM_ABORT;

			// Handle enabling help overlay
			if (_system->input->check(CIN::SHOW_CONTROLS, event)) {
				_display->show_overlay();
				continue;
			} else
				_display->hide_overlay();

			// Stables
			if (_type == RET::STABLES) {

				if (_stage == STR::REGEN) {

					if (_system->input->check(CIN::ANYTHING, event)) {
						_go_to_results();
						_stage = STR::RESULTS;
					}

				} else if (_stage == STR::RESULTS) {

					if (_system->input->check(CIN::CANCEL, event))
						return MIM::ITEM_CONTINUE;
					else if (_system->input->check(CIN::BACK, event))
						return MIM::ITEM_CONTINUE;
					else if (_system->input->check(CIN::UP, event))
						opt_cont = _cont_menu->choose_previous();
					else if (_system->input->check(CIN::DOWN, event))
						opt_cont = _cont_menu->choose_next();
					else if (_system->input->check(CIN::MOVE, event))
						opt_cont =
							_cont_menu->set_mouse_selected(_display->get_cur());
					else if (_system->input->check(CIN::CONFIRM, event)) {

						if (opt_cont) {
							if (const MIM opt{(*opt_cont.value()).item};
								opt == MIM::ITEM_CONTINUE) {
								return MIM::ITEM_CONTINUE;
							}
						}
					}
				}
			} else {

				// Do other methods of resting that cost money
				if (_stage == STR::REGEN) {

					if (_system->input->check(CIN::CANCEL, event))
						return MIM::CP_LEAVE;
					else if (_system->input->check(CIN::BACK, event))
						return MIM::CP_LEAVE;
					else if (_system->input->check(CIN::UP, event))
						opt_stop = _stop_menu->choose_previous();
					else if (_system->input->check(CIN::DOWN, event))
						opt_stop = _stop_menu->choose_next();
					else if (_system->input->check(CIN::MOVE, event))
						opt_stop =
							_stop_menu->set_mouse_selected(_display->get_cur());
					else if (_system->input->check(CIN::CONFIRM, event)) {

						if (opt_stop) {
							if (const MIM opt{(*opt_stop.value()).item};
								opt == MIM::ITEM_STOP) {
								_go_to_results();
								_stage = STR::RESULTS;
								continue;
							}
						}
					}
				} else if (_stage == STR::RESULTS) {

					if (_system->input->check(CIN::CANCEL, event))
						return MIM::CP_LEAVE;
					else if (_system->input->check(CIN::BACK, event))
						return MIM::CP_LEAVE;
					else if (_system->input->check(CIN::UP, event))
						opt_cont = _cont_menu->choose_previous();
					else if (_system->input->check(CIN::DOWN, event))
						opt_cont = _cont_menu->choose_next();
					else if (_system->input->check(CIN::MOVE, event))
						opt_cont =
							_cont_menu->set_mouse_selected(_display->get_cur());
					else if (_system->input->check(CIN::CONFIRM, event)) {

						if (opt_cont) {
							if (const MIM opt{(*opt_cont.value()).item};
								opt == MIM::ITEM_CONTINUE) {
								return MIM::ITEM_CONTINUE;
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
	case RET::COT:
		inc_hp = 1;
		dec_gold = 10;
		break;
	case RET::ECONOMY:
		inc_hp = 3;
		dec_gold = 50;
		break;
	case RET::MERCHANT:
		inc_hp = 5;
		dec_gold = 200;
		break;
	case RET::ROYAL:
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

	_recup_message_1 = fmt::format("{} ({:>5}/{:>5})",
		(*_display->string)["REST_HP"], hp, _character->get_max_hp());
	_recup_message_2 =
		fmt::format("{} {}", (*_display->string)["REST_GOLD"], gold);

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

		const auto no_level_up_results{fmt::format("{} {} {}@{}",
			(*_display->string)["REST_XP_PREFIX_YOU"], need_xp,
			(*_display->string)["REST_XP_SUFFIX_YOU"],
			(*_display->string)["REST_XP_2_YOU"])};
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

	if (_type == RET::STABLES) {

		if (_stage == STR::REGEN) {

			_display->window->draw_text(
				_nap_text, (*_display->layout)["rest:nap_text"], _nap_message);

		} else if (_stage == STR::RESULTS) {

			if (_results->valid)
				_window->draw(*_results);

			// And the Menu
			_cont_menu->generate((*_display->layout)["rest:continue_menu"]);

			_window->draw(*_cont_frame);
			_window->draw(*_cont_menu);
		}
	} else {

		if (_stage == STR::REGEN) {

			_display->window->draw_text(_recup_text,
				(*_display->layout)["rest:nap_text"], _recup_message);
			_display->window->draw_text(_recup_text_1,
				(*_display->layout)["rest:recup_text_1"], _recup_message_1);
			_display->window->draw_text(_recup_text_2,
				(*_display->layout)["rest:recup_text_2"], _recup_message_2);

			_stop_menu->generate((*_display->layout)["rest:stop_menu"]);

			_window->draw(*_stop_frame);
			_window->draw(*_stop_menu);

		} else if (_stage == STR::RESULTS) {

			if (_results->valid)
				_window->draw(*_results);

			// And the Menu
			_cont_menu->generate((*_display->layout)["rest:continue_menu"]);

			_window->draw(*_cont_frame);
			_window->draw(*_cont_menu);
		}
	}

	// Always draw the following
	_display->display_overlay();
	_display->display_cursor();
}