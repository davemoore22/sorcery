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
Sorcery::Inspect::Inspect(System *system, Display *display, Graphics *graphics, Game *game, MMD mode)
	: _system{system}, _display{display}, _graphics{graphics}, _game{game}, _mode{mode} {

	using enum Enums::Menu::Mode;

	// Get the Window and Graphics to Display
	_window = _display->window->get_window();
	_cur_char = std::nullopt;

	// Setup the Factory
	_factory = std::make_unique<Factory>(_system, _display, _graphics, _game);

	// Same object can be used in three different modes
	switch (_mode) {
	case TAVERN:
		_screen_key = "tavern_inspect";
		break;
	case INN:
		_screen_key = "inn_inspect";
		break;
	case SHOP:
		_screen_key = "shop_inspect";
		break;
	case TEMPLE:
		_screen_key = "temple_inspect";
		break;
	case CAMP:
		_screen_key = "engine_base_ui";
		break;
	default:
		break;
	}

	_char_panel = std::make_unique<CharacterPanel>(_system, _display, _graphics);
	_character_display = std::make_unique<CharacterDisplay>(_system, _display, _graphics);

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
	_in_item_action = false;
	_in_item_display = false;
}

// Standard Destructor
Sorcery::Inspect::~Inspect() {
}

auto Sorcery::Inspect::start(std::optional<unsigned int> character_id) -> std::optional<MIM> {

	using enum Enums::Menu::Mode;
	using enum Enums::Menu::Item;

	// Do we want to display a menu with all characters in the party, or just handle one character directly
	_restricted = character_id.has_value();
	if (!_restricted) {

		_in_character = false;
		const auto menu_type{_mode == MMD::TAVERN ? MTP::CHARACTER_ROSTER : MTP::PARTY_CHARACTERS};
		_menu.reset();
		_menu = _factory->make_menu(_screen_key + ":menu", menu_type, _mode, std::nullopt, true);

		_cur_char_id = -1;

		// Get the Background Display Components and load them into Display module
		// storage (not local) unless its camp in which case we save the screen
		switch (_mode) {
		case TAVERN:
			_display->generate("tavern_inspect");
			break;
		case INN:
			_display->generate("inn_inspect");
			break;
		case SHOP:
			_display->generate("shop_inspect");
			break;
		case TEMPLE:
			_display->generate("temple_inspect");
			break;
		case CAMP:
			_display->window->save_screen();
			break;
		default:
			break;
		}

		// Set up the Custom Components
		if (_screen_key != "engine_base_ui") {
			const Component bg_c{(*_display->layout)[_screen_key + ":background"]};
			sf::IntRect bg_rect{};
			bg_rect.width = std::stoi(bg_c["source_w"].value());
			bg_rect.height = std::stoi(bg_c["source_h"].value());
			bg_rect.top = 0;
			bg_rect.left = std::stoi(bg_c["source_w"].value()) * std::stoi(bg_c["source_index"].value());

			_bg.setTexture(_system->resources->textures[GraphicsTexture::TOWN]);
			_bg.setTextureRect(bg_rect);
			_bg.setScale(std::stof(bg_c["scale_x"].value()), std::stof(bg_c["scale_y"].value()));
			_bg.setPosition(_display->window->get_x(_bg, bg_c.x), _display->window->get_y(_bg, bg_c.y));
		}

		_menu_frame = _factory->make_menu_frame(_screen_key + ":menu_frame");
		_cur_char_frame = _factory->make_frame(_screen_key + ":character_frame");
		_preview_frame = _factory->make_frame("roster:preview_frame");

		// Clear the window
		_window->clear();

		_display->set_input_mode(WIM::NAVIGATE_MENU);
		std::optional<std::vector<MenuEntry>::const_iterator> selected{_menu->items.begin()};

		// And do the main loop
		sf::Event event{};
		while (_window->isOpen()) {
			while (_window->pollEvent(event)) {

				// Check for Window Close
				if (event.type == sf::Event::Closed)
					return ITEM_ABORT;

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
					selected = _menu->set_mouse_selected(static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
				else if (_system->input->check(CIN::CONFIRM, event)) {

					// We have selected something from the menu
					if (selected) {
						const MIM option_chosen{(*selected.value()).item};
						if (option_chosen == ET_TRAIN || option_chosen == CA_TAVERN || option_chosen == CA_TEMPLE ||
							option_chosen == CA_INN || option_chosen == ITEM_CAMP || option_chosen == CA_SHOP) {
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
						if ((*selected.value()).item != ET_TRAIN && (*selected.value()).item != CA_TAVERN &&
							(*selected.value()).item != CA_SHOP && (*selected.value()).item != CA_TEMPLE &&
							(*selected.value()).item != CA_INN && (*selected.value()).item != ITEM_CAMP) {
							const auto character_chosen{static_cast<int>((*selected.value()).index)};
							if (character_chosen != _cur_char_id) {
								auto character{&_game->characters[character_chosen]};
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

auto Sorcery::Inspect::_handle_in_character(unsigned int character_id) -> std::optional<MIM> {

	using enum Enums::Window::DialogButton;
	using enum Enums::Items::IdentifyOutcome;

	const auto character_chosen{character_id};
	_cur_char = &_game->characters[character_chosen];
	_character_display->set(_cur_char.value());
	if (_cur_char) {
		_display->set_input_mode(WIM::BROWSE_CHARACTER);
		_character_display->set_view(CharacterView::SUMMARY);
	}

	_in_character = true;
	_in_item_action = false;
	_in_item_display = false;
	_in_trade = false;

	_item_action_menu.reset();
	_item_action_menu_frame.reset();
	_item_display.reset();
	_item_display_frame.reset();
	_item_trade_menu.reset();
	_item_trade_frame.reset();

	_item_action_menu = _factory->make_menu("character_summary:item_action_menu", MTP::ITEM_ACTION, MMD::ACTION);
	_item_trade_menu = _factory->make_menu("inspect:item_trade_menu", MTP::CHARACTER_TRADE, MMD::NO_MODE, character_id);

	_item_action_menu_frame = _factory->make_menu_frame("character_summary:item_action_menu_frame");
	_item_display_frame = _factory->make_frame("inspect:item_display_frame");

	_item_display = std::make_unique<ItemDisplay>(_system, _display, _graphics, _game);
	_item_display->setPosition((*_display->layout)["inspect:item_display"].pos());

	std::optional<std::vector<MenuEntry>::const_iterator> item_action_selected{_item_action_menu->items.begin()};
	std::optional<std::vector<MenuEntry>::const_iterator> item_trade_selected{_item_trade_menu->items.begin()};

	// And do the main loop inside the character
	sf::Event event{};
	while (_window->isOpen()) {
		while (_window->pollEvent(event)) {

			if (event.type == sf::Event::Closed)
				return MIM::ITEM_ABORT;
			else if (_system->input->check(CIN::CANCEL, event) || _system->input->check(CIN::BACK, event)) {
				_display->set_input_mode(WIM::NAVIGATE_MENU);
				_cur_char = std::nullopt;
				_in_character = false;
				return std::nullopt;
			}

			if (_in_pool) {

				// Handle Pool Gold Dialog
				auto dialog_input{_pool->handle_input(event)};
				if (dialog_input) {
					if (dialog_input.value() == CLOSE) {
						_in_pool = false;
						_display->set_input_mode(WIM::NAVIGATE_MENU);
					} else if (dialog_input.value() == OK) {

						_game->pool_party_gold(character_id);
						_game->save_game();
						_in_pool = false;
						_display->set_input_mode(WIM::NAVIGATE_MENU);
					}
				}

			} else if (_in_drop) {

				// Handle Drop Item Dialog
				auto dialog_input{_drop->handle_input(event)};
				if (dialog_input) {
					if (dialog_input.value() == CLOSE || dialog_input.value() == NO) {

						_in_drop = false;
						_display->set_input_mode(WIM::NAVIGATE_MENU);
					} else if (dialog_input.value() == YES) {

						auto character{&_game->characters[character_id]};
						character->inventory.drop_item(_character_display->get_inventory_item());
						_game->save_game();
						_in_drop = false;
						_display->set_input_mode(WIM::NAVIGATE_MENU);
					}
				}
			} else if (_in_cursed) {

				// Handle Cursed Item Dialog
				auto dialog_input{_cursed->handle_input(event)};
				if (dialog_input) {
					if ((dialog_input.value() == CLOSE) || (dialog_input.value() == OK)) {
						_in_cursed = false;
						_display->set_input_mode(WIM::NAVIGATE_MENU);
					}
				}
			} else if (_in_failed) {

				// Handle Failed Item Identify
				auto dialog_input{_failed->handle_input(event)};
				if (dialog_input) {
					if ((dialog_input.value() == CLOSE) || (dialog_input.value() == OK)) {
						_in_failed = false;
						_display->set_input_mode(WIM::NAVIGATE_MENU);
					}
				}
			} else if (_in_success) {

				// Handle Item Identify
				auto dialog_input{_success->handle_input(event)};
				if (dialog_input) {
					if ((dialog_input.value() == CLOSE) || (dialog_input.value() == OK)) {
						_in_success = false;
						_display->set_input_mode(WIM::NAVIGATE_MENU);
					}
				}
			} else if (_in_item_display) {

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
					_in_item_display = false;
					_in_item_action = true;
					_in_cursed = false;
					_in_failed = false;
					_in_success = false;
					_in_trade = false;
				}

				if (_system->input->check(CIN::BACK, event)) {
					_in_item_display = false;
					_in_item_action = true;
					_in_cursed = false;
					_in_failed = false;
					_in_success = false;
					_in_trade = false;
				}

				if (_system->input->check(CIN::CONFIRM, event)) {
					_in_item_display = false;
					_in_item_action = true;
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
					_in_item_action = false;
				}

				if (_system->input->check(CIN::BACK, event)) {
					_in_trade = false;
					_in_item_action = false;
				}

				if (_system->input->check(CIN::UP, event))
					item_trade_selected = _item_trade_menu->choose_previous();
				else if (_system->input->check(CIN::DOWN, event))
					item_trade_selected = _item_trade_menu->choose_next();
				else if (_system->input->check(CIN::MOVE, event))
					item_trade_selected = _item_trade_menu->set_mouse_selected(
						static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
				else if (_system->input->check(CIN::CONFIRM, event)) {

					if (item_trade_selected) {
						if ((*item_trade_selected.value()).type == MIT::ENTRY) {
							const auto dest_char_id{(*item_trade_selected.value()).index};
							const auto slot_to_move{_character_display->get_inventory_item()};
							auto src_char{&_game->characters[character_id]};
							auto dest_char{&_game->characters[dest_char_id]};
							if (src_char->inventory.has(slot_to_move)) {
								auto item_to_move{src_char->inventory.get(slot_to_move)};
								dest_char->inventory.add(item_to_move);
								src_char->inventory.discard_item(slot_to_move);
								_character_display->clear_inventory_item();
							}

							_in_trade = false;
							_in_item_action = false;
						} else {
							_in_trade = false;
							_in_item_action = true;
							continue;
						}
					}
				}

			} else if (_in_item_action) {

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
					_in_item_action = false;

				if (_system->input->check(CIN::BACK, event))
					_in_item_action = false;

				if (_system->input->check(CIN::UP, event))
					item_action_selected = _item_action_menu->choose_previous();
				else if (_system->input->check(CIN::DOWN, event))
					item_action_selected = _item_action_menu->choose_next();
				else if (_system->input->check(CIN::MOVE, event))
					item_action_selected = _item_action_menu->set_mouse_selected(
						static_cast<sf::Vector2f>(sf::Mouse::getPosition(*_window)));
				else if (_system->input->check(CIN::CONFIRM, event)) {

					if (item_action_selected) {
						const MIM option_chosen{(*item_action_selected.value()).item};
						if (option_chosen == MIM::C_ACTION_LEAVE) {
							_in_item_action = false;
						} else if (option_chosen == MIM::C_ACTION_EXAMINE) {

							// Examine an Item (only possible if it is identified)
							_in_item_action = false;
							_in_item_display = true;
							_examine_item(character_id);
						} else if (option_chosen == MIM::C_ACTION_DROP) {

							// Drop an Item
							_in_item_action = false;
							_in_drop = true;
						} else if (option_chosen == MIM::C_ACTION_EQUIP) {

							// Equip an Item
							_in_item_action = false;

							auto character{&_game->characters[character_id]};
							character->inventory.equip_item(_character_display->get_inventory_item());

							if (character->inventory.is_equipped_cursed(_character_display->get_inventory_item()))
								_in_cursed = true;

							_in_item_action = false;
						} else if (option_chosen == MIM::C_ACTION_UNEQUIP) {
							auto character{&_game->characters[character_id]};
							auto slot_item{character->inventory[_character_display->get_inventory_item()]};
							if (slot_item.has_value()) {
								if (auto &item{slot_item.value()};
									item->get_equipped() && item->get_usable() && !item->get_cursed())
									item->set_equipped(false);

								_in_item_action = false;
							}
						} else if (option_chosen == MIM::C_ACTION_IDENTIFY) {

							// Attempt to Identify an Item
							auto character{&_game->characters[character_id]};
							if (character->get_class() == CharacterClass::BISHOP) {
								auto slot_item{character->inventory[_character_display->get_inventory_item()]};
								if (slot_item.has_value()) {
									auto &item{slot_item.value()};
									if (!item->get_known()) {
										auto dice{(*_system->random)[RandomType::D100]};
										auto result{
											character->inventory.identify_item(_character_display->get_inventory_item(),
												dice, character->abilities().at(CAB::IDENTIFY_ITEMS),
												character->abilities().at(CAB::IDENTIFY_CURSE))};

										if (result == CURSED_FAIL || result == CURSED_SUCCESS)
											_in_cursed = true;
										else if (result == FAIL)
											_in_failed = true;
										else if (result == SUCCESS)
											_in_success = true;
										else
											_in_item_action = false;
									}
								}
							}
						} else if (option_chosen == MIM::C_ACTION_TRADE) {
							const auto dest_char_id{(*item_trade_selected.value()).index};
							auto src_char{&_game->characters[character_id]};
							auto dest_char{&_game->characters[dest_char_id]};
							Item *item_to_move{src_char->inventory[_character_display->get_inventory_item()].value()};
							dest_char->inventory.items().emplace_back(std::move(*item_to_move));
							_in_item_action = false;
							_in_trade = true;
						}
					}
				}
			} else {

				if (_character_display->get_view() == CharacterView::SUMMARY) {
					if (_character_display->check_for_action_mouse_move(
							sf::Vector2f(static_cast<float>(sf::Mouse::getPosition(*_window).x),
								static_cast<float>(sf::Mouse::getPosition(*_window).y)))) {
						_character_display->generate_display();
					}
					if (_character_display->check_for_inventory_mouse_move(
							sf::Vector2f(static_cast<float>(sf::Mouse::getPosition(*_window).x),
								static_cast<float>(sf::Mouse::getPosition(*_window).y)))) {
						_character_display->generate_display();
					}

					if (_system->input->check(CIN::CONFIRM, event)) {
						if (_character_display->get_hl_action_item() == MIM::C_ACTION_POOL) {
							_in_pool = true;
						} else if (_character_display->get_hl_action_item() == MIM::C_ACTION_LEAVE) {
							_display->set_input_mode(WIM::NAVIGATE_MENU);
							_cur_char = std::nullopt;
							_in_character = false;
							return std::nullopt;
						} else if (_character_display->get_inventory_item() > 0) {
							_in_item_action = true;
							_set_in_item_action_menu(character_id, _character_display->get_inventory_item());
							_item_action_menu->generate(
								(*_display->layout)["character_summary:item_action_menu"], true);
						}
					}

					if (_system->input->check(CIN::LEFT, event))
						_character_display->left_view();
					else if (_system->input->check(CIN::RIGHT, event))
						_character_display->right_view();
				} else {
					if (_system->input->check(CIN::LEFT, event))
						_character_display->left_view();
					else if (_system->input->check(CIN::RIGHT, event))
						_character_display->right_view();
					if (_system->input->check(CIN::UP, event)) {
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
						if (_system->input->check(CIN::LEFT, event))
							_character_display->left_view();
						else if (_system->input->check(CIN::RIGHT, event))
							_character_display->right_view();
						else if ((_system->input->check(CIN::CANCEL, event)) ||
								 (_system->input->check(CIN::BACK, event))) {
							_display->set_input_mode(WIM::NAVIGATE_MENU);
							_cur_char = std::nullopt;
							_in_character = false;
							return std::nullopt;
						} else if (_system->input->check(CIN::CONFIRM, event)) {
							_character_display->right_view();
						} else if (_system->input->check(CIN::UP, event)) {
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
							if (_character_display->check_for_inventory_mouse_move(
									sf::Vector2f(static_cast<float>(sf::Mouse::getPosition(*_window).x),
										static_cast<float>(sf::Mouse::getPosition(*_window).y)))) {
								_character_display->generate_display();
							}
						}
						if (_character_display->check_for_action_mouse_move(
								sf::Vector2f(static_cast<float>(sf::Mouse::getPosition(*_window).x),
									static_cast<float>(sf::Mouse::getPosition(*_window).y)))) {
							_character_display->generate_display();
						}
						if (_character_display->check_for_inventory_mouse_move(
								sf::Vector2f(static_cast<float>(sf::Mouse::getPosition(*_window).x),
									static_cast<float>(sf::Mouse::getPosition(*_window).y)))) {
							_character_display->generate_display();
						}
					}
				}
			}
		}

		_character_display->generate_display();
		_draw();
		_window->display();
	}

	return std::nullopt;
}

auto Sorcery::Inspect::_examine_item(const unsigned int character_id) -> void {

	auto character{&_game->characters[character_id]};
	auto slot_item{character->inventory[_character_display->get_inventory_item()]};
	if (slot_item.has_value()) {
		auto item{slot_item.value()};
		_item_display->set(unenum(item->get_type_id()));

		const auto item_gfx_c{(*_display->layout)["inspect:picture"]};
		_item_display_gfx =
			_graphics->textures->get(unenum(item->get_type_id()) - 1, GraphicsTextureType::ITEMS).value();
		_item_display_gfx.setPosition(item_gfx_c.pos());
		_item_display_gfx.setScale(item_gfx_c.scl());
	}
}

auto Sorcery::Inspect::_set_in_item_action_menu(unsigned int character_id, unsigned int slot_id) -> void {

	auto character{&_game->characters[character_id]};
	auto slot_item{character->inventory[slot_id]};
	if (slot_item.has_value()) {

		auto item{slot_item.value()};
		auto has_already_cursed{character->inventory.has_cursed_equipped_item_category(item->get_category())};

		if (_mode == MMD::CAMP) {

			// Equip
			_item_action_menu->items[0].enabled = item->get_usable() && !item->get_equipped() && !has_already_cursed;

			// Unequip
			_item_action_menu->items[1].enabled = (!item->get_cursed()) && item->get_equipped();

			// Trade
			_item_action_menu->items[2].enabled = !item->get_equipped();

			// Examine
			_item_action_menu->items[3].enabled = item->get_known();

			// Invoke
			_item_action_menu->items[4].enabled =
				(*_game->itemstore)[item->get_type_id()].get_eff_inv() != ItemInv::NO_INV_EFFECT;

			// Use
			_item_action_menu->items[5].enabled =
				(*_game->itemstore)[item->get_type_id()].get_eff_use() != SpellID::NO_SPELL;

			// Identify
			_item_action_menu->items[6].enabled =
				(!item->get_known()) && character->get_class() == CharacterClass::BISHOP;

			// Drop
			_item_action_menu->items[7].enabled = (!item->get_cursed()) && !item->get_equipped();
		} else {

			// Equip
			_item_action_menu->items[0].enabled = item->get_usable() && !item->get_equipped() && !has_already_cursed;

			// Unequip
			_item_action_menu->items[1].enabled = (!item->get_cursed()) && item->get_equipped();

			// Trade
			_item_action_menu->items[2].enabled = !item->get_equipped();

			// Examine
			_item_action_menu->items[3].enabled = true;

			// Invoke
			_item_action_menu->items[4].enabled = false;

			// Use
			_item_action_menu->items[5].enabled = false;

			// Identify
			_item_action_menu->items[6].enabled = false;

			// Drop
			_item_action_menu->items[7].enabled = (!item->get_cursed()) && (!item->get_equipped());
		}

		// Leave
		_item_action_menu->items[8].enabled = true;
	}
}

auto Sorcery::Inspect::stop() -> void {

	_display->set_input_mode(WIM::NAVIGATE_MENU);
}

auto Sorcery::Inspect::_draw() -> void {

	using enum Enums::Menu::Mode;

	if (!_in_character) {

		_graphics->tile_bg(_window);

		// Display Components
		switch (_mode) {
		case TAVERN:
			_display->display("tavern");
			break;
		case INN:
			_display->display("inn");
			break;
		case SHOP:
			_display->display("shop");
			break;
		case TEMPLE:
			_display->display("temple");
			break;
		case CAMP:
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
				_char_panel->setPosition((*_display->layout)[_screen_key + ":info_panel"].pos());
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
					_char_panel->setPosition((*_display->layout)[_screen_key + ":info_panel"].pos());
					_window->draw(*_char_panel);
				}
			}

			// And finally the Cursor
			_display->display_overlay();
			_display->display_cursor();
		}
	} else {

		if (_cur_char) {

			if (_mode == CAMP)
				_character_display->set_mode(CharacterMode::IN_MAZE);
			else
				_character_display->set_mode(CharacterMode::AT_CASTLE);

			_character_display->setPosition((*_display->layout)[_screen_key + ":character"].pos());
			_character_display->update();
			_display->window->restore_screen();
			_window->draw(*_character_display);

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
			} else if (_in_item_action) {
				_window->draw(*_item_action_menu_frame);
				_item_action_menu->generate((*_display->layout)["character_summary:item_action_menu"]);
				_window->draw(*_item_action_menu);
			} else if (_in_item_display) {
				_window->draw(*_item_display_frame);
				_window->draw(*_item_display);
				_window->draw(_item_display_gfx);
			} else if (_in_trade) {
				_window->draw(*_item_trade_frame);
				_item_trade_menu->generate((*_display->layout)["inspect:item_trade_menu"]);
				_window->draw(*_item_trade_menu);
			}

			// And finally the Cursor
			_display->display_overlay();
			_display->display_cursor();
		}
	}
}
