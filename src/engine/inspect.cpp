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

#include "engine/inspect.hpp"
#include "common/enum.hpp"
#include "common/type.hpp"
#include "core/display.hpp"
#include "core/game.hpp"
#include "core/graphics.hpp"
#include "core/input.hpp"
#include "core/system.hpp"
#include "core/window.hpp"
#include "gui/characterdisplay.hpp"
#include "gui/characterpanel.hpp"
#include "gui/dialog.hpp"
#include "gui/frame.hpp"
#include "gui/itemdisplay.hpp"
#include "gui/menu.hpp"
#include "resources/componentstore.hpp"
#include "resources/factory.hpp"
#include "resources/resourcemanager.hpp"
#include "types/character.hpp"
#include "types/component.hpp"

// Standard Constructor
Sorcery::Inspect::Inspect(
	System *system, Display *display, Graphics *graphics, Game *game, MMD mode)
	: _system{system}, _display{display}, _graphics{graphics}, _game{game},
	  _mode{mode} {

	// Get the Window and Graphics to Display
	_window = _display->window->get_window();
	_cur_char = std::nullopt;

	// Setup the Factory
	_factory = std::make_unique<Factory>(_system, _display, _graphics, _game);

	// Same object can be used in three different modes
	switch (_mode) {
	case MMD::TAVERN:
		_screen_key = "tavern_inspect";
		break;
	case MMD::INN:
		_screen_key = "inn_inspect";
		break;
	case MMD::SHOP:
		_screen_key = "shop_inspect";
		break;
	case MMD::TEMPLE:
		_screen_key = "temple_inspect";
		break;
	case MMD::CAMP:
		_screen_key = "engine_base_ui";
		break;
	default:
		break;
	}

	_char_panel =
		std::make_unique<CharacterPanel>(_system, _display, _graphics);
	_char_disp =
		std::make_unique<CharacterDisplay>(_system, _display, _graphics);

	_pool = _factory->make_dialog("inspect:dialog_pool_gold_ok");
	_cursed = _factory->make_dialog("inspect:dialog_cursed_ok");
	_failed = _factory->make_dialog("inspect:dialog_failed_ok");
	_success = _factory->make_dialog("inspect:dialog_success_ok");
	_drop = _factory->make_dialog("inspect:dialog_confirm_drop", WDT::CONFIRM);

	_in_pool = false;
	_in_cursed = false;
	_in_failed = false;
	_in_success = false;
	_in_drop = false;

	_in_trade = false;
	_in_action = false;
	_in_examine = false;
}

// Standard Destructor
Sorcery::Inspect::~Inspect() {
}

auto Sorcery::Inspect::start(std::optional<unsigned int> character_id)
	-> std::optional<MIM> {

	// Do we want to display a menu with all characters in the party, or just
	// handle one character directly
	_restricted = character_id.has_value();
	if (!_restricted) {

		_in_character = false;
		const auto menu_type{_mode == MMD::TAVERN ? MTP::CHARACTER_ROSTER
												  : MTP::PARTY_CHARACTERS};
		_menu.reset();
		_menu = _factory->make_menu(
			_screen_key + ":menu", menu_type, _mode, std::nullopt, true);

		_cur_char_id = -1;

		// Get the Background Display Components and load them into Display
		// module storage (not local) unless its camp in which case we save the
		// screen
		switch (_mode) {
		case MMD::TAVERN:
			_display->generate("tavern_inspect");
			break;
		case MMD::INN:
			_display->generate("inn_inspect");
			break;
		case MMD::SHOP:
			_display->generate("shop_inspect");
			break;
		case MMD::TEMPLE:
			_display->generate("temple_inspect");
			break;
		case MMD::CAMP:
			_display->window->save_screen();
			break;
		default:
			break;
		}

		// Set up the Custom Components
		if (_screen_key != "engine_base_ui") {
			const Component bg_c{
				(*_display->layout)[_screen_key + ":background"]};
			sf::IntRect bg_rect{};
			bg_rect.width = std::stoi(bg_c["source_w"].value());
			bg_rect.height = std::stoi(bg_c["source_h"].value());
			bg_rect.top = 0;
			bg_rect.left = std::stoi(bg_c["source_w"].value()) *
						   std::stoi(bg_c["source_index"].value());

			_bg.setTexture(*_system->resources->get_texture(GTX::TOWN));
			_bg.setTextureRect(bg_rect);
			_bg.setScale(std::stof(bg_c["scale_x"].value()),
				std::stof(bg_c["scale_y"].value()));
			_bg.setPosition(_display->window->get_x(_bg, bg_c.x),
				_display->window->get_y(_bg, bg_c.y));
		}

		_menu_frame = _factory->make_menu_frame(_screen_key + ":menu_frame");
		_cur_char_frame =
			_factory->make_frame(_screen_key + ":character_frame");
		_preview_frame = _factory->make_frame("roster:preview_frame");

		// Clear the window
		_window->clear();

		_display->set_input_mode(WIM::NAVIGATE_MENU);
		MenuSelect selected{_menu->items.begin()};

		// And do the main loop
		sf::Event event{};
		while (_window->isOpen()) {
			while (_window->pollEvent(event)) {

				// Check for Window Close
				if (event.type == sf::Event::Closed)
					return MIM::ITEM_ABORT;

				// Handle enabling help overlay
				if (_system->input->check(CIN::SHOW_CONTROLS, event)) {
					_display->show_overlay();
					continue;
				} else
					_display->hide_overlay();

				if (_system->input->check(CIN::CANCEL, event))
					return std::nullopt;

				if (_system->input->check(CIN::BACK, event))
					return std::nullopt;

				if (_system->input->check(CIN::UP, event))
					selected = _menu->choose_previous();
				else if (_system->input->check(CIN::DOWN, event))
					selected = _menu->choose_next();
				else if (_system->input->check(CIN::MOVE, event))
					selected = _menu->set_mouse_selected(_display->get_cur());
				else if (_system->input->check(CIN::CONFIRM, event)) {

					// We have selected something from the menu
					if (selected) {
						const MIM opt{(*selected.value()).item};
						if (opt == MIM::ET_TRAIN || opt == MIM::CA_TAVERN ||
							opt == MIM::CA_TEMPLE || opt == MIM::CA_INN ||
							opt == MIM::ITEM_CAMP || opt == MIM::CA_SHOP) {
							_display->set_input_mode(WIM::NAVIGATE_MENU);
							_cur_char = std::nullopt;
							return std::nullopt;
						} else {

							const auto character_id{(*selected.value()).index};
							_display->window->save_screen();
							_handle_in_character(character_id);
							_window->clear();

							_draw();
							_window->display();
						}
					}

					// Display Character Panel
					if (selected) {
						if ((*selected.value()).item != MIM::ET_TRAIN &&
							(*selected.value()).item != MIM::CA_TAVERN &&
							(*selected.value()).item != MIM::CA_SHOP &&
							(*selected.value()).item != MIM::CA_TEMPLE &&
							(*selected.value()).item != MIM::CA_INN &&
							(*selected.value()).item != MIM::ITEM_CAMP) {
							const auto character_chosen{
								static_cast<int>((*selected.value()).index)};
							if (character_chosen != _cur_char_id) {
								auto character{
									&_game->characters[character_chosen]};
								_char_panel->set(character);
								_cur_char_id = character_chosen;
							}
						} else {
							_char_panel->valid = false;
							_cur_char_id = -1;
						}
					}
				}
			}

			_window->clear();

			_draw();
			_window->display();
		}

	} else {
		_display->window->save_screen();
		_handle_in_character(character_id.value());
	}
	_game->save_game();

	return std::nullopt;
}

auto Sorcery::Inspect::_handle_in_character(unsigned int character_id)
	-> std::optional<MIM> {

	const auto character_chosen{character_id};
	_cur_char = &_game->characters[character_chosen];
	_char_disp->set(_cur_char.value());
	if (_cur_char) {
		_display->set_input_mode(WIM::BROWSE_CHARACTER);
		_char_disp->set_view(CHV::SUMMARY);
	}

	_in_character = true;
	_in_action = false;
	_in_examine = false;
	_in_trade = false;

	_action_menu.reset();
	_action_menu_frame.reset();
	_examine.reset();
	_examine_frame.reset();
	_trade_menu.reset();
	_trade_frame.reset();

	_action_menu = _factory->make_menu(
		"character_summary:item_action_menu", MTP::ITEM_ACTION, MMD::ACTION);
	_trade_menu = _factory->make_menu("inspect:item_trade_menu",
		MTP::CHARACTER_TRADE, MMD::NO_MODE, character_id);

	_action_menu_frame =
		_factory->make_menu_frame("character_summary:item_action_menu_frame");
	_examine_frame = _factory->make_frame("inspect:item_display_frame");

	_examine =
		std::make_unique<ItemDisplay>(_system, _display, _graphics, _game);
	_examine->setPosition((*_display->layout)["inspect:item_display"].pos());

	MenuSelect action_select{_action_menu->items.begin()};
	MenuSelect trade_select{_trade_menu->items.begin()};

	// And do the main loop inside the character
	sf::Event event{};
	while (_window->isOpen()) {
		while (_window->pollEvent(event)) {

			if (event.type == sf::Event::Closed)
				return MIM::ITEM_ABORT;
			else if (_system->input->check(CIN::CANCEL, event) ||
					 _system->input->check(CIN::BACK, event)) {
				_display->set_input_mode(WIM::NAVIGATE_MENU);
				_cur_char = std::nullopt;
				_in_character = false;
				return std::nullopt;
			}

			if (_in_pool) {

				// Handle Pool Gold Dialog
				auto input{_pool->handle_input(event)};
				if (input) {
					if (input.value() == WDB::CLOSE) {
						_in_pool = false;
						_display->set_input_mode(WIM::NAVIGATE_MENU);
					} else if (input.value() == WDB::OK) {

						_game->pool_party_gold(character_id);
						_game->save_game();
						_in_pool = false;
						_display->set_input_mode(WIM::NAVIGATE_MENU);
					}
				}

			} else if (_in_drop) {

				// Handle Drop Item Dialog
				auto input{_drop->handle_input(event)};
				if (input) {
					if (input.value() == WDB::CLOSE ||
						input.value() == WDB::NO) {

						_in_drop = false;
						_display->set_input_mode(WIM::NAVIGATE_MENU);
					} else if (input.value() == WDB::YES) {

						auto character{&_game->characters[character_id]};
						character->inventory.drop_item(
							_char_disp->get_inventory_item());
						_game->save_game();
						_in_drop = false;
						_display->set_input_mode(WIM::NAVIGATE_MENU);
					}
				}
			} else if (_in_cursed) {

				// Handle Cursed Item Dialog
				auto input{_cursed->handle_input(event)};
				if (input) {
					if ((input.value() == WDB::CLOSE) ||
						(input.value() == WDB::OK)) {
						_in_cursed = false;
						_display->set_input_mode(WIM::NAVIGATE_MENU);
					}
				}
			} else if (_in_failed) {

				// Handle Failed Item Identify
				auto input{_failed->handle_input(event)};
				if (input) {
					if ((input.value() == WDB::CLOSE) ||
						(input.value() == WDB::OK)) {
						_in_failed = false;
						_display->set_input_mode(WIM::NAVIGATE_MENU);
					}
				}
			} else if (_in_success) {

				// Handle Item Identify

				if (auto input{_success->handle_input(event)}; input) {
					if ((input.value() == WDB::CLOSE) ||
						(input.value() == WDB::OK)) {
						_in_success = false;
						_display->set_input_mode(WIM::NAVIGATE_MENU);
					}
				}
			} else if (_in_examine) {

				// Check for Window Close
				if (event.type == sf::Event::Closed)
					return MIM::ITEM_ABORT;

				// Handle enabling help overlay
				if (_system->input->check(CIN::SHOW_CONTROLS, event)) {
					_display->show_overlay();
					continue;
				} else
					_display->hide_overlay();

				if (_system->input->check(CIN::CANCEL, event)) {
					_in_examine = false;
					_in_action = true;
					_in_cursed = false;
					_in_failed = false;
					_in_success = false;
					_in_trade = false;
				}

				if (_system->input->check(CIN::BACK, event)) {
					_in_examine = false;
					_in_action = true;
					_in_cursed = false;
					_in_failed = false;
					_in_success = false;
					_in_trade = false;
				}

				if (_system->input->check(CIN::CONFIRM, event)) {
					_in_examine = false;
					_in_action = true;
					_in_cursed = false;
					_in_failed = false;
					_in_success = false;
					_in_trade = false;
				}
			} else if (_in_trade) {

				// Check for Window Close
				if (event.type == sf::Event::Closed)
					return MIM::ITEM_ABORT;

				// Handle enabling help overlay
				if (_system->input->check(CIN::SHOW_CONTROLS, event)) {
					_display->show_overlay();
					continue;
				} else
					_display->hide_overlay();

				if (_system->input->check(CIN::CANCEL, event)) {
					_in_trade = false;
					_in_action = false;
				}

				if (_system->input->check(CIN::BACK, event)) {
					_in_trade = false;
					_in_action = false;
				}

				if (_system->input->check(CIN::UP, event))
					trade_select = _trade_menu->choose_previous();
				else if (_system->input->check(CIN::DOWN, event))
					trade_select = _trade_menu->choose_next();
				else if (_system->input->check(CIN::MOVE, event))
					trade_select =
						_trade_menu->set_mouse_selected(_display->get_cur());
				else if (_system->input->check(CIN::CONFIRM, event)) {

					if (trade_select) {
						if ((*trade_select.value()).type == MIT::ENTRY) {
							const auto dest_char_id{
								(*trade_select.value()).index};
							const auto slot_to_move{
								_char_disp->get_inventory_item()};
							auto src_char{&_game->characters[character_id]};
							auto dest_char{&_game->characters[dest_char_id]};
							if (src_char->inventory.has(slot_to_move)) {
								auto item_to_move{
									src_char->inventory.get(slot_to_move)};
								dest_char->inventory.add(item_to_move);
								src_char->inventory.discard_item(slot_to_move);
								_char_disp->clear_inventory_item();
							}

							_in_trade = false;
							_in_action = false;
						} else {
							_in_trade = false;
							_in_action = true;
							continue;
						}
					}
				}

			} else if (_in_action) {

				// Check for Window Close
				if (event.type == sf::Event::Closed)
					return MIM::ITEM_ABORT;

				// Handle enabling help overlay
				if (_system->input->check(CIN::SHOW_CONTROLS, event)) {
					_display->show_overlay();
					continue;
				} else
					_display->hide_overlay();

				if (_system->input->check(CIN::CANCEL, event))
					_in_action = false;

				if (_system->input->check(CIN::BACK, event))
					_in_action = false;

				if (_system->input->check(CIN::UP, event))
					action_select = _action_menu->choose_previous();
				else if (_system->input->check(CIN::DOWN, event))
					action_select = _action_menu->choose_next();
				else if (_system->input->check(CIN::MOVE, event))
					action_select =
						_action_menu->set_mouse_selected(_display->get_cur());
				else if (_system->input->check(CIN::CONFIRM, event)) {

					if (action_select) {
						const MIM opt{(*action_select.value()).item};
						if (opt == MIM::C_ACTION_LEAVE) {
							_in_action = false;
						} else if (opt == MIM::C_ACTION_EXAMINE) {

							// Examine an Item (only possible if it is
							// identified)
							_in_action = false;
							_in_examine = true;
							_examine_item(character_id);
						} else if (opt == MIM::C_ACTION_DROP) {

							// Drop an Item
							_in_action = false;
							_in_drop = true;
						} else if (opt == MIM::C_ACTION_EQUIP) {

							// Equip an Item
							_in_action = false;

							auto character{&_game->characters[character_id]};
							character->inventory.equip_item(
								_char_disp->get_inventory_item());

							if (character->inventory.is_equipped_cursed(
									_char_disp->get_inventory_item()))
								_in_cursed = true;

							_in_action = false;
						} else if (opt == MIM::C_ACTION_UNEQUIP) {
							auto character{&_game->characters[character_id]};
							auto slot_item{character->inventory[_char_disp
									->get_inventory_item()]};
							if (slot_item.has_value()) {
								if (auto &item{slot_item.value()};
									item->get_equipped() &&
									item->get_usable() && !item->get_cursed())
									item->set_equipped(false);

								_in_action = false;
							}
						} else if (opt == MIM::C_ACTION_IDENTIFY) {

							// Attempt to Identify an Item
							auto character{&_game->characters[character_id]};
							if (character->get_class() == CHC::BISHOP) {
								auto slot_item{character->inventory[_char_disp
										->get_inventory_item()]};
								if (slot_item.has_value()) {
									auto &item{slot_item.value()};
									if (!item->get_known()) {
										auto dice{
											(*_system->random)[RNT::D100]};
										auto result{
											character->inventory.identify_item(
												_char_disp
													->get_inventory_item(),
												dice,
												character->abilities().at(
													CAB::IDENTIFY_ITEMS),
												character->abilities().at(
													CAB::IDENTIFY_CURSE))};

										if (result == IIR::CURSED_FAIL ||
											result == IIR::CURSED_SUCCESS)
											_in_cursed = true;
										else if (result == IIR::FAIL)
											_in_failed = true;
										else if (result == IIR::SUCCESS)
											_in_success = true;
										else
											_in_action = false;
									}
								}
							}
						} else if (opt == MIM::C_ACTION_TRADE) {
							const auto dest_char_id{
								(*trade_select.value()).index};
							auto src_char{&_game->characters[character_id]};
							auto dest_char{&_game->characters[dest_char_id]};
							Item *item_to_move{src_char
									->inventory[_char_disp
											->get_inventory_item()]
									.value()};
							dest_char->inventory.items().emplace_back(
								std::move(*item_to_move));
							_in_action = false;
							_in_trade = true;
						}
					}
				}
			} else {

				if (_char_disp->get_view() == CHV::SUMMARY) {
					if (_char_disp->check_for_action_mouse_move(
							_display->window->get_cur())) {
						_char_disp->generate_display();
					}
					if (_char_disp->check_for_inventory_mouse_move(
							_display->window->get_cur())) {
						_char_disp->generate_display();
					}

					if (_system->input->check(CIN::CONFIRM, event)) {
						if (_char_disp->get_hl_action_item() ==
							MIM::C_ACTION_POOL) {
							_in_pool = true;
						} else if (_char_disp->get_hl_action_item() ==
								   MIM::C_ACTION_LEAVE) {
							_display->set_input_mode(WIM::NAVIGATE_MENU);
							_cur_char = std::nullopt;
							_in_character = false;
							return std::nullopt;
						} else if (_char_disp->get_inventory_item() > 0) {
							_in_action = true;
							_set_in_item_action_menu(
								character_id, _char_disp->get_inventory_item());
							_action_menu->generate(
								(*_display->layout)
									["character_summary:item_action_menu"],
								true);
						}
					}

					if (_system->input->check(CIN::LEFT, event))
						_char_disp->left_view();
					else if (_system->input->check(CIN::RIGHT, event))
						_char_disp->right_view();
				} else {
					if (_system->input->check(CIN::LEFT, event))
						_char_disp->left_view();
					else if (_system->input->check(CIN::RIGHT, event))
						_char_disp->right_view();
					if (_system->input->check(CIN::UP, event)) {
						if (_char_disp->get_view() == CHV::MAGE_SPELLS)
							_char_disp->dec_hl_spell(SPT::MAGE);
						else if (_char_disp->get_view() == CHV::PRIEST_SPELLS)
							_char_disp->dec_hl_spell(SPT::PRIEST);

					} else if (_system->input->check(CIN::DOWN, event)) {
						if (_char_disp->get_view() == CHV::MAGE_SPELLS)
							_char_disp->inc_hl_spell(SPT::MAGE);
						else if (_char_disp->get_view() == CHV::PRIEST_SPELLS)
							_char_disp->inc_hl_spell(SPT::PRIEST);
					} else if (_system->input->check(CIN::MOVE, event)) {
						if (_char_disp->check_for_mouse_move(
								_display->window->get_cur())) {
							_char_disp->set_view(_char_disp->get_view());
						}
						if (_system->input->check(CIN::LEFT, event))
							_char_disp->left_view();
						else if (_system->input->check(CIN::RIGHT, event))
							_char_disp->right_view();
						else if ((_system->input->check(CIN::CANCEL, event)) ||
								 (_system->input->check(CIN::BACK, event))) {
							_display->set_input_mode(WIM::NAVIGATE_MENU);
							_cur_char = std::nullopt;
							_in_character = false;
							return std::nullopt;
						} else if (_system->input->check(CIN::CONFIRM, event)) {
							_char_disp->right_view();
						} else if (_system->input->check(CIN::UP, event)) {
							if (_char_disp->get_view() == CHV::MAGE_SPELLS)
								_char_disp->dec_hl_spell(SPT::MAGE);
							else if (_char_disp->get_view() ==
									 CHV::PRIEST_SPELLS)
								_char_disp->dec_hl_spell(SPT::PRIEST);

						} else if (_system->input->check(CIN::DOWN, event)) {
							if (_char_disp->get_view() == CHV::MAGE_SPELLS)
								_char_disp->inc_hl_spell(SPT::MAGE);
							else if (_char_disp->get_view() ==
									 CHV::PRIEST_SPELLS)
								_char_disp->inc_hl_spell(SPT::PRIEST);
						} else if (_system->input->check(CIN::MOVE, event)) {
							if (_char_disp->check_for_mouse_move(
									_display->window->get_cur())) {
								_char_disp->set_view(_char_disp->get_view());
							}
							if (_char_disp->check_for_action_mouse_move(
									_display->window->get_cur())) {
								_char_disp->generate_display();
							}
							if (_char_disp->check_for_inventory_mouse_move(
									_display->window->get_cur())) {
								_char_disp->generate_display();
							}
						}
						if (_char_disp->check_for_action_mouse_move(
								_display->window->get_cur())) {
							_char_disp->generate_display();
						}
						if (_char_disp->check_for_inventory_mouse_move(
								_display->window->get_cur())) {
							_char_disp->generate_display();
						}
					}
				}
			}
		}

		_char_disp->generate_display();
		_draw();
		_window->display();
	}

	return std::nullopt;
}

auto Sorcery::Inspect::_examine_item(const unsigned int character_id) -> void {

	auto character{&_game->characters[character_id]};
	if (auto slot_item{character->inventory[_char_disp->get_inventory_item()]};
		slot_item.has_value()) {
		auto item{slot_item.value()};
		_examine->set(unenum(item->get_type_id()));

		const auto item_gfx_c{(*_display->layout)["inspect:picture"]};
		_gfx = _graphics->textures
				   ->get(unenum(item->get_type_id()) - 1, GTT::ITEMS)
				   .value();
		_gfx.setPosition(item_gfx_c.pos());
		_gfx.setScale(item_gfx_c.scl());
	}
}

auto Sorcery::Inspect::_set_in_item_action_menu(
	unsigned int character_id, unsigned int slot_id) -> void {

	auto character{&_game->characters[character_id]};
	auto slot_item{character->inventory[slot_id]};
	if (slot_item.has_value()) {

		auto item{slot_item.value()};
		auto has_already_cursed{
			character->inventory.has_cursed_equipped_item_category(
				item->get_category())};

		if (_mode == MMD::CAMP) {

			// Equip
			_action_menu->items[0].enabled = item->get_usable() &&
											 !item->get_equipped() &&
											 !has_already_cursed;

			// Unequip
			_action_menu->items[1].enabled =
				(!item->get_cursed()) && item->get_equipped();

			// Trade
			_action_menu->items[2].enabled = !item->get_equipped();

			// Examine
			_action_menu->items[3].enabled = item->get_known();

			// Invoke
			_action_menu->items[4].enabled =
				(*_game->itemstore)[item->get_type_id()].get_eff_inv() !=
				ITV::NO_INV_EFFECT;

			// Use
			_action_menu->items[5].enabled =
				(*_game->itemstore)[item->get_type_id()].get_eff_use() !=
				SPI::NO_SPELL;

			// Identify
			_action_menu->items[6].enabled =
				(!item->get_known()) && character->get_class() == CHC::BISHOP;

			// Drop
			_action_menu->items[7].enabled =
				(!item->get_cursed()) && !item->get_equipped();
		} else {

			// Equip
			_action_menu->items[0].enabled = item->get_usable() &&
											 !item->get_equipped() &&
											 !has_already_cursed;

			// Unequip
			_action_menu->items[1].enabled =
				(!item->get_cursed()) && item->get_equipped();

			// Trade
			_action_menu->items[2].enabled = !item->get_equipped();

			// Examine
			_action_menu->items[3].enabled = true;

			// Invoke
			_action_menu->items[4].enabled = false;

			// Use
			_action_menu->items[5].enabled = false;

			// Identify
			_action_menu->items[6].enabled = false;

			// Drop
			_action_menu->items[7].enabled =
				(!item->get_cursed()) && (!item->get_equipped());
		}

		// Leave
		_action_menu->items[8].enabled = true;
	}
}

auto Sorcery::Inspect::stop() -> void {

	_display->set_input_mode(WIM::NAVIGATE_MENU);
}

auto Sorcery::Inspect::_draw() -> void {

	if (!_in_character) {

		_graphics->tile_bg(_window);

		// Display Components
		switch (_mode) {
		case MMD::TAVERN:
			_display->display("tavern");
			break;
		case MMD::INN:
			_display->display("inn");
			break;
		case MMD::SHOP:
			_display->display("shop");
			break;
		case MMD::TEMPLE:
			_display->display("temple");
			break;
		case MMD::CAMP:
			_display->window->restore_screen();
			break;
		default:
			break;
		}

		if (_display->get_input_mode() == WIM::CONFIRM_DELETE_CHARACTER) {

			// Menu Frame
			_window->draw(*_menu_frame);
			_window->draw(*_preview_frame);

			// And the Menu
			_menu->generate((*_display->layout)[_screen_key + ":menu"]);
			_window->draw(*_menu);

			// Character Preview
			if (_char_panel->valid) {
				_char_panel->setPosition(
					(*_display->layout)[_screen_key + ":info_panel"].pos());
				_window->draw(*_char_panel);
			}
		} else if (_display->get_input_mode() != WIM::BROWSE_CHARACTER) {

			// Menu Frame
			_window->draw(*_menu_frame);
			if (_screen_key != "engine_base_ui")
				_window->draw(*_preview_frame);

			// And the Menu
			_menu->generate((*_display->layout)[_screen_key + ":menu"]);
			_window->draw(*_menu);

			if (_screen_key != "engine_base_ui") {

				// Character Preview
				if (_char_panel->valid) {
					_char_panel->setPosition(
						(*_display->layout)[_screen_key + ":info_panel"].pos());
					_window->draw(*_char_panel);
				}
			}

			// And finally the Cursor
			_display->display_overlay();
			_display->display_cursor();
		}
	} else {

		if (_cur_char) {

			if (_mode == MMD::CAMP)
				_char_disp->set_mode(CHM::IN_MAZE);
			else
				_char_disp->set_mode(CHM::AT_CASTLE);

			_char_disp->setPosition(
				(*_display->layout)[_screen_key + ":character"].pos());
			_char_disp->update();
			_display->window->restore_screen();
			_window->draw(*_char_disp);

			if (_in_pool) {
				_pool->update();
				_window->draw(*_pool);
			} else if (_in_cursed) {
				_cursed->update();
				_window->draw(*_cursed);
			} else if (_in_success) {
				_success->update();
				_window->draw(*_success);
			} else if (_in_failed) {
				_failed->update();
				_window->draw(*_failed);
			} else if (_in_drop) {
				_drop->update();
				_window->draw(*_drop);
			} else if (_in_action) {
				_window->draw(*_action_menu_frame);
				_action_menu->generate(
					(*_display->layout)["character_summary:item_action_menu"]);
				_window->draw(*_action_menu);
			} else if (_in_examine) {
				_window->draw(*_examine_frame);
				_window->draw(*_examine);
				_window->draw(_gfx);
			} else if (_in_trade) {
				_window->draw(*_trade_frame);
				_trade_menu->generate(
					(*_display->layout)["inspect:item_trade_menu"]);
				_window->draw(*_trade_menu);
			}

			// And finally the Cursor
			_display->display_overlay();
			_display->display_cursor();
		}
	}
}
