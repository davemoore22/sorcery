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

#include "modules/temple.hpp"
#include "common/enum.hpp"
#include "common/type.hpp"
#include "core/display.hpp"
#include "core/game.hpp"
#include "core/graphics.hpp"
#include "core/input.hpp"
#include "core/random.hpp"
#include "core/state.hpp"
#include "core/system.hpp"
#include "core/window.hpp"
#include "engine/inspect.hpp"
#include "gui/console.hpp"
#include "gui/frame.hpp"
#include "gui/menu.hpp"
#include "gui/partypanel.hpp"
#include "modules/define.hpp"
#include "resources/componentstore.hpp"
#include "resources/stringstore.hpp"
#include "types/character.hpp"
#include "types/component.hpp"

// Standard Constructor
Sorcery::Temple::Temple(System *system, Display *display, Graphics *graphics, Game *game)
	: _system{system}, _display{display}, _graphics{graphics}, _game{game} {

	// Get the Window and Graphics to Display
	_window = _display->window->get_window();

	// Setup Custom Components
	_menu = std::make_unique<Menu>(_system, _display, _graphics, _game, MenuType::TEMPLE);
	_menu->generate((*_display->layout)["temple:menu"]);
	_menu->setPosition(_display->get_centre_x(_menu->get_width()), (*_display->layout)["temple:menu"].y);

	_help = std::make_unique<Menu>(_system, _display, _graphics, _game, MenuType::INVALID_CHARACTERS, MenuMode::TEMPLE);
	_help->generate((*_display->layout)["temple_help:menu"]);
	_help->setPosition(_display->get_centre_x(_help->get_width()), (*_display->layout)["temple_help:menu"].y);

	_pay = std::make_unique<Menu>(_system, _display, _graphics, _game, MenuType::PARTY_CHARACTERS, MenuMode::TEMPLE);
	_pay->generate((*_display->layout)["temple_pay:menu"]);
	_pay->setPosition(_display->get_centre_x(_pay->get_width()), (*_display->layout)["temple_pay:menu"].y);

	_continue_menu = std::make_unique<Menu>(_system, _display, _graphics, _game, MenuType::CONTINUE);
	_continue_menu->generate((*_display->layout)["temple_ress:continue_menu"]);
	_continue_menu->setPosition(
		_display->get_centre_x(_continue_menu->get_width()), (*_display->layout)["temple_ress:continue_menu"].y);

	_console = std::make_unique<Console>(_display->window->get_gui(), _system, _display, _graphics, _game);

	// Modules
	_party_panel =
		std::make_unique<PartyPanel>(_system, _display, _graphics, _game, (*_display->layout)["global:party_panel"]);
	_inspect = std::make_unique<Inspect>(_system, _display, _graphics, _game, MenuMode::TEMPLE);

	_stage = TempleStage::NO_STAGE;

	_cost = sf::Text();
	_ress_count = sf::Text();

	_game->hide_console();
}

// Standard Destructor
Sorcery::Temple::~Temple() {
}

// Visit the Tavern
auto Sorcery::Temple::start() -> std::optional<MenuItem> {

	// Get the Background Display Components and load them into Display module storage (not local - and note that due to
	// the way both menus are combined in this class, we need to have the menu stage set first in this case and this
	// case only)
	_display->generate("temple");
	_display->generate("temple_help", _h_sprites, _h_texts, _h_frames);
	_display->generate("temple_pay", _p_sprites, _p_texts, _p_frames);
	_display->generate("temple_ress", _r_sprites, _r_texts, _r_frames);

	// Clear the window
	_window->clear();

	// Refresh the Party characters and the Invalid Characters
	_party_panel->refresh();
	_help->reload();
	_help->generate((*_display->layout)["temple_help:menu"]);
	_pay->reload();
	_pay->generate((*_display->layout)["temple_pay:menu"]);

	_duration = DELAY_TEMPLE;
	_ress_text = "";

	// Generate the Components
	const Component party_banel_c{(*_display->layout)["global:party_panel"]};
	_party_panel->setPosition(_display->get_centre_x(_party_panel->width), (*_display->layout)["global:party_panel"].y);

	// Start at the entry menu
	_stage = TempleStage::MENU;

	// And do the main loop
	_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
	std::optional<std::vector<MenuEntry>::const_iterator> option{_menu->items.begin()};
	std::optional<std::vector<MenuEntry>::const_iterator> option_help{_help->items.begin()};
	std::optional<std::vector<MenuEntry>::const_iterator> option_pay{_pay->items.begin()};
	std::optional<std::vector<MenuEntry>::const_iterator> option_continue{_continue_menu->items.begin()};
	sf::Event event{};
	auto heal_char_id{0u};
	while (_window->isOpen()) {
		while (_window->pollEvent(event)) {

			// If we are in normal input mode
			if (_display->get_input_mode() == WindowInputMode::NAVIGATE_MENU) {

				// Check for Window Close
				if (event.type == sf::Event::Closed)
					return MenuItem::ITEM_ABORT;

				// Handle enabling help overlay
				if (_system->input->check(WindowInput::SHOW_CONTROLS, event)) {
					_display->show_overlay();
					continue;
				} else
					_display->hide_overlay();

				if (_system->input->check(WindowInput::SHOW_HIDE_CONSOLE, event))
					_game->toggle_console();

				if (_system->input->check(WindowInput::CANCEL, event))
					return std::nullopt;

				if (_system->input->check(WindowInput::BACK, event))
					return std::nullopt;

				// And handle input on the main menu
				if (_stage == TempleStage::MENU) {
					if (_system->input->check(WindowInput::UP, event))
						option = _menu->choose_previous();
					else if (_system->input->check(WindowInput::DOWN, event))
						option = _menu->choose_next();
					else if (_system->input->check(WindowInput::MOVE, event))
						option = _menu->set_mouse_selected(static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
					else if (_system->input->check(WindowInput::CONFIRM, event)) {

						// We have selected something from the menu
						if (option) {
							if (const MenuItem option_chosen{(*option.value()).item};
								option_chosen == MenuItem::TE_CASTLE) {
								return MenuItem::TE_CASTLE;
							} else if (option_chosen == MenuItem::TE_INSPECT) {
								if (auto result{_inspect->start()}; result && result.value() == MenuItem::ITEM_ABORT) {
									_inspect->stop();
									return MenuItem::ITEM_ABORT;
								}
								_inspect->stop();
								_display->generate("temple");
								_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
								continue;
							} else if (const MenuItem option_chosen{(*option.value()).item};
									   option_chosen == MenuItem::TE_HELP) {
								_stage = TempleStage::HELP;
								_party_panel->refresh();
								_help->reload();
								_help->generate((*_display->layout)["temple_help:menu"]);
								continue;
							}
						}
					}
				} else if (_stage == TempleStage::HELP) {
					if (_system->input->check(WindowInput::CANCEL, event)) {
						_stage = TempleStage::MENU;
						_party_panel->refresh();
						_help->reload();
						_help->generate((*_display->layout)["temple_help:menu"]);
					} else if (_system->input->check(WindowInput::BACK, event)) {
						_stage = TempleStage::MENU;
						_party_panel->refresh();
						_help->reload();
						_help->generate((*_display->layout)["temple_help:menu"]);
					} else if (_system->input->check(WindowInput::UP, event))
						option_help = _help->choose_previous();
					else if (_system->input->check(WindowInput::DOWN, event))
						option_help = _help->choose_next();
					else if (_system->input->check(WindowInput::MOVE, event))
						option_help =
							_help->set_mouse_selected(static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
					else if (_system->input->check(WindowInput::CONFIRM, event)) {

						// We have selected something from the menu
						if (option_help) {
							if (const MenuItem option_chosen{(*option_help.value()).item};
								option_chosen == MenuItem::CA_TEMPLE) {
								_stage = TempleStage::MENU;
								_party_panel->refresh();
								_help->reload();
								_help->generate((*_display->layout)["temple_help:menu"]);
								continue;
							} else {
								heal_char_id = (*option_help.value()).index;
								const auto &help_character{_game->characters[heal_char_id]};
								const auto cost{help_character.get_cure_cost()};
								if (heal_char_id > 0) {
									_stage = TempleStage::PAY;
									_party_panel->refresh();
									_pay->reload();
									_refresh_pay_menu(cost);
									_pay->generate((*_display->layout)["temple_pay:menu"]);
									_update_cost(cost);
								}
							}
						}
					}
				} else if (_stage == TempleStage::PAY) {

					if (_system->input->check(WindowInput::CANCEL, event)) {
						_stage = TempleStage::HELP;
						_party_panel->refresh();
						_help->reload();
						_help->generate((*_display->layout)["temple_help:menu"]);
					} else if (_system->input->check(WindowInput::BACK, event)) {
						_stage = TempleStage::HELP;
						_party_panel->refresh();
						_help->reload();
						_help->generate((*_display->layout)["temple_help:menu"]);
					} else if (_system->input->check(WindowInput::UP, event))
						option_pay = _pay->choose_previous();
					else if (_system->input->check(WindowInput::DOWN, event))
						option_pay = _pay->choose_next();
					else if (_system->input->check(WindowInput::MOVE, event))
						option_pay =
							_pay->set_mouse_selected(static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
					else if (_system->input->check(WindowInput::CONFIRM, event)) {

						// We have selected something from the menu
						if (option_help) {
							if (const MenuItem option_chosen{(*option_pay.value()).item};
								option_chosen == MenuItem::CA_TEMPLE) {
								_stage = TempleStage::HELP;
								_party_panel->refresh();
								_help->reload();
								_help->generate((*_display->layout)["temple_help:menu"]);
								continue;
							} else {
								const auto pay_char_id{(*option_pay.value()).index};
								_stage = TempleStage::RESS;
								_t_finished = false;
								_start_count_thread();
								if (pay_char_id > 0) {
									_result_text = "";
									_try_cure_or_ress(heal_char_id, pay_char_id);
									_party_panel->refresh();
									_game->save_game();
								}
							}
						}
					}
				} else if (_stage == TempleStage::RESS) {
					if (_t_finished) {
						_stop_count_thread();
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
									_stage = TempleStage::MENU;
									_party_panel->refresh();
									continue;
								}
							}
						}
					}
				}
			}
		}

		if (_get_ress_count() > 0)
			_set_ress_status();

		_window->clear();

		_draw();
		_window->display();
	}

	return std::nullopt;
}

auto Sorcery::Temple::stop() -> void {
}

auto Sorcery::Temple::_try_cure_or_ress(unsigned int heal_char_id, unsigned int pay_char_id) -> bool {

	auto &pay_char{_game->characters[pay_char_id]};
	auto &heal_char{_game->characters[heal_char_id]};

	// subtract money cost from selected character
	const auto cost{heal_char.get_cure_cost()};
	pay_char.grant_gold(0 - cost);

	if (heal_char.get_status() == CharacterStatus::DEAD) {

		const auto chance{heal_char.get_ress_chance(false)};
		const auto roll((*_system->random)[RandomType::D100]);

		_game->state->add_log_dice_roll(
			fmt::format("{:>16} - {}", heal_char.get_name(), "Ress from Dead"), 100, roll, chance);
		if (roll < chance) {

			_result_text = fmt::format("{} {} {}", (*_display->string)["TEMPLE_HEALED_PREFIX"], heal_char.get_name(),
				(*_display->string)["TEMPLE_HEALED_SUFFIX"]);
			heal_char.set_status(CharacterStatus::OK);
			heal_char.set_current_hp(1);
			heal_char.set_location(CharacterLocation::TAVERN);
			return true;
		} else {

			_result_text = fmt::format("{} {} {}", (*_display->string)["TEMPLE_OOPS_DEAD_PREFIX"], heal_char.get_name(),
				(*_display->string)["TEMPLE_OOPS_DEAD_SUFFIX"]);
			heal_char.set_status(CharacterStatus::ASHES);
			return false;
		}

	} else if (heal_char.get_status() == CharacterStatus::ASHES) {

		const auto chance{heal_char.get_ress_chance(true)};
		const auto roll((*_system->random)[RandomType::D100]);
		_game->state->add_log_dice_roll(
			fmt::format("{:>16} - {}", heal_char.get_name(), "Ress from Ashes"), 100, roll, chance);
		if (roll < chance) {

			_result_text = fmt::format("{} {} {}", (*_display->string)["TEMPLE_HEALED_PREFIX"], heal_char.get_name(),
				(*_display->string)["TEMPLE_HEALED_SUFFIX"]);
			heal_char.set_status(CharacterStatus::OK);
			heal_char.set_current_hp(1);
			heal_char.set_location(CharacterLocation::TAVERN);

			return true;

		} else {

			_result_text = fmt::format("{} {} {}", (*_display->string)["TEMPLE_OOPS_ASHES_PREFIX"],
				heal_char.get_name(), (*_display->string)["TEMPLE_OOPS_ASHES_SUFFIX"]);
			heal_char.set_status(CharacterStatus::LOST);
			heal_char.set_location(CharacterLocation::TRAINING);
			heal_char.set_current_hp(0);

			return false;
		}

	} else {

		_result_text = fmt::format("{} {} {}", (*_display->string)["TEMPLE_HEALED_PREFIX"], heal_char.get_name(),
			(*_display->string)["TEMPLE_HEALED_SUFFIX"]);
		heal_char.set_status(CharacterStatus::OK);

		return true;
	}
}

auto Sorcery::Temple::_update_cost(const unsigned int cost) -> void {

	_cost_text = fmt::format(
		"{} {} {}", (*_display->string)["TEMPLE_COST_PREFIX"], cost, (*_display->string)["TEMPLE_COST_SUFFIX"]);
}

auto Sorcery::Temple::_refresh_pay_menu(const unsigned int cost) -> void {

	// disable any character menu item who doesn't have enough gold
	for (auto &item : _pay->items) {
		if (item.item == MenuItem::IC_CHARACTER) {
			const auto &character{_game->characters[item.index]};
			item.enabled = character.get_gold() >= cost;
		}
	}
	_pay->choose_first();
}

auto Sorcery::Temple::_draw() -> void {

	// Play the background movie!
	_graphics->tile_bg(_window);

	// Custom Components
	_display->display("temple");
	_window->draw(*_party_panel);

	if (_stage == TempleStage::MENU) {

		// And the Menu
		_menu->generate((*_display->layout)["temple:menu"]);
		_window->draw(*_menu);
	} else if (_stage == TempleStage::HELP) {

		// Choose Invalid Character
		_help->generate((*_display->layout)["temple_help:menu"]);
		_display->display("inn_choose", _h_sprites, _h_texts, _h_frames);
		_window->draw(*_help);
	} else if (_stage == TempleStage::PAY) {

		_pay->generate((*_display->layout)["temple_pay:menu"]);
		_display->display("inn_pay", _p_sprites, _p_texts, _p_frames);
		_display->window->draw_text(_cost, (*_display->layout)["temple_pay:cost_text"], _cost_text);
		_window->draw(*_pay);
	} else if (_stage == TempleStage::RESS) {

		_display->display("inn_ress", _r_sprites, _r_texts, _r_frames);
		_display->window->draw_text(_ress_count, (*_display->layout)["temple_ress:ress_message"], _get_ress_status());
		if (_get_ress_count() > 5) {
			_display->window->draw_text(_ress_result, (*_display->layout)["temple_ress:ress_result"], _result_text);
			_continue_menu->generate((*_display->layout)["temple_ress:continue_menu"]);
			_window->draw(*_continue_menu);
		}
	}

	if (_game->get_console_status()) {
		_console->refresh();
		_display->window->get_gui()->draw();
	}

	// Always draw the following
	_display->display_overlay();
	_display->display_cursor();
}

auto Sorcery::Temple::_thread_count() -> void {

	do {
		if (_allow_count)
			_do_count();

		std::this_thread::sleep_for(std::chrono::milliseconds(_duration));
	} while (!_t_finished);
}

auto Sorcery::Temple::_do_count() -> void {

	std::scoped_lock<std::mutex> _scoped_lock(_count_mutex);

	++_t_count;
	if (_t_count > 5)
		_t_finished = true;
}

auto Sorcery::Temple::_start_count() -> void {

	_t_count = 0;
	_allow_count = true;
}

auto Sorcery::Temple::_stop_count() -> void {

	_allow_count = false;
}

auto Sorcery::Temple::_start_count_thread() -> void {

	_start_count();
	if (!_count_thread.joinable())
		_count_thread = std::jthread(&Temple::_thread_count, this);
}

auto Sorcery::Temple::_stop_count_thread() -> void {

	_t_finished = true;
	_stop_count();
	if (_count_thread.joinable())
		_count_thread.join();
}

auto Sorcery::Temple::_get_ress_status() -> std::string {

	return _ress_text;
}

auto Sorcery::Temple::_get_ress_count() -> unsigned int {

	std::scoped_lock<std::mutex> _scoped_lock(_count_mutex);

	return _t_count;
}

auto Sorcery::Temple::_set_ress_status() -> void {

	_ress_text = "";
	if (_get_ress_count() > 1)
		_ress_text.append((*_display->string)["TEMPLE_HEAL_1"]);
	if (_get_ress_count() > 2)
		_ress_text.append((*_display->string)["TEMPLE_HEAL_2"]);
	if (_get_ress_count() > 3)
		_ress_text.append((*_display->string)["TEMPLE_HEAL_3"]);
	if (_get_ress_count() > 4)
		_ress_text.append((*_display->string)["TEMPLE_HEAL_4"]);
}
