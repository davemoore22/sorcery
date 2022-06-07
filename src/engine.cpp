// Copyright (C) 2021 Dave Moore
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

#include "engine.hpp"

// Standard Constructor
Sorcery::Engine::Engine(
	System *system, Display *display, Graphics *graphics, Game *game)
	: _system{system}, _display{display}, _graphics{graphics}, _game{game} {

	// Get the Window and Graphics to Display
	_window = _display->window->get_window();

	// Setup Custom Components
	_camp_menu = std::make_unique<Menu>(
		_system, _display, _graphics, _game, MenuType::CAMP);
	const Component menu_fc{
		(*_display->layout)["engine_base_ui:camp_menu_frame"]};
	_camp_menu_frame = std::make_unique<Frame>(_display->ui_texture,
		WindowFrameType::NORMAL, menu_fc.w, menu_fc.h, menu_fc.colour,
		menu_fc.background, menu_fc.alpha);
	_camp_menu_frame->setPosition(
		_display->window->get_x(_camp_menu_frame->sprite, menu_fc.x),
		_display->window->get_y(_camp_menu_frame->sprite, menu_fc.y));

	_confirm_exit = std::make_unique<Dialog>(_system, _display, _graphics,
		(*_display->layout)["engine_base_ui:dialog_exit"],
		(*_display->layout)["engine_base_ui:dialog_exit_text"],
		WindowDialogType::CONFIRM);
	_confirm_exit->setPosition(
		(*_display->layout)["engine_base_ui:dialog_exit"].x,
		(*_display->layout)["engine_base_ui:dialog_exit"].y);

	_confirm_stairs = std::make_unique<Dialog>(_system, _display, _graphics,
		(*_display->layout)["engine_base_ui:dialog_stairs"],
		(*_display->layout)["engine_base_ui:dialog_ladder_up_text"],
		WindowDialogType::CONFIRM);
	_confirm_stairs->setPosition(
		(*_display->layout)["engine_base_ui:dialog_stairs"].x,
		(*_display->layout)["engine_base_ui:dialog_stairs"].y);

	_ouch = std::make_unique<Dialog>(_system, _display, _graphics,
		(*_display->layout)["engine_base_ui:ouch"],
		(*_display->layout)["engine_base_ui:ouch_text"],
		WindowDialogType::TIMED);
	_ouch->setPosition((*_display->layout)["engine_base_ui:ouch"].x,
		(*_display->layout)["engine_base_ui:ouch"].y);

	// Modules
	_status_bar = std::make_unique<StatusBar>(_system, _display, _graphics,
		_game, (*_display->layout)["engine_base_ui:status_bar"],
		(*_display->layout)["engine_base_ui:status_bar_outer_frame"]);
	_reorder = std::make_unique<Reorder>(
		_system, _display, _graphics, _game, MenuMode::CAMP);
	_inspect = std::make_unique<Inspect>(
		_system, _display, _graphics, _game, MenuMode::CAMP);
	_render = std::make_unique<Render>(_system, _display, _graphics, _game);
	_automap = std::make_unique<AutoMap>(_system, _display, _graphics, _game,
		(*_display->layout)["global:automap"]);
	_compass = std::make_unique<Compass>(_system, _display, _graphics, _game,
		(*_display->layout)["global:compass"]);
	_console = std::make_unique<Console>(
		_display->window->get_gui(), _system, _display, _graphics, _game);
	_left_icon_panel = std::make_unique<IconPanel>(_system, _display, _graphics,
		_game, (*_display->layout)["engine_base_ui:left_icon_panel"], true);
	_right_icon_panel =
		std::make_unique<IconPanel>(_system, _display, _graphics, _game,
			(*_display->layout)["engine_base_ui:right_icon_panel"], false);

	_tile_note = std::make_unique<Message>(_system, _display, _graphics,
		(*_display->layout)["engine_base_ui:message_panel"],
		(*_display->layout)["engine_base_ui:message_text"]);

	_update_automap = false;
	_update_compass = false;
	_update_icon_panels = false;
	_update_status_bar = false;
	_update_render = false;
	_update_tile_note = false;
	_exit_maze_now = false;

	game->hide_console();
}

// Standard Destructor
Sorcery::Engine::~Engine() {}

auto Sorcery::Engine::start() -> int {

	_display->generate("engine_base_ui");

	_render->refresh();
	_status_bar->refresh();
	_automap->refresh();
	_compass->refresh();
	_left_icon_panel->refresh(true);
	_right_icon_panel->refresh(true);

	// Generate the Custom Components
	const Component status_bar_c{
		(*_display->layout)["engine_base_ui:status_bar"]};
	_status_bar->setPosition(
		_display->window->get_x(_status_bar->sprite, status_bar_c.x),
		_display->window->get_y(_status_bar->sprite, status_bar_c.y));

	const Component automap_c{(*_display->layout)["global:automap"]};
	_automap->setPosition(automap_c.x, automap_c.y);
	const Component compass_c{(*_display->layout)["global:compass"]};
	_compass->setPosition(compass_c.x, compass_c.y);

	const Component l_icon_panel_c{
		(*_display->layout)["engine_base_ui:left_icon_panel"]};
	_left_icon_panel->setPosition(l_icon_panel_c.x, l_icon_panel_c.y);
	const Component r_icon_panel_c{
		(*_display->layout)["engine_base_ui:right_icon_panel"]};
	_right_icon_panel->setPosition(r_icon_panel_c.x, r_icon_panel_c.y);

	const Component t_n_c{(*_display->layout)["engine_base_ui:message"]};
	_tile_note->setPosition(t_n_c.x, t_n_c.y);

	const Component cc_fc{
		(*_display->layout)["engine_base_ui:character_frame"]};
	_cur_char_frame =
		std::make_unique<Frame>(_display->ui_texture, WindowFrameType::NORMAL,
			cc_fc.w, cc_fc.h, cc_fc.colour, cc_fc.background, cc_fc.alpha);
	_cur_char_frame->setPosition(
		_display->window->get_x(_cur_char_frame->sprite, cc_fc.x),
		_display->window->get_y(_cur_char_frame->sprite, cc_fc.y));

	// Start in camp as is tradition
	_in_camp = true;
	_in_character = false;
	_show_confirm_exit = false;
	_show_ouch = false;
	_show_tile_note = false;
	_exit_maze_now = false;
	_automap->refresh();
	auto &starting_tile{
		_game->state->level->at(_game->state->get_player_pos())};
	if (!starting_tile.is(TileProperty::EXPLORED))
		starting_tile.set_explored();
	_game->state->set_player_facing(MapDirection::NORTH);

	_show_confirm_stairs =
		(_game->state->get_player_pos() == Coordinate{0, 0}) &&
		(_game->state->level->depth() == -1);

	if ((_game->state->get_player_pos() == Coordinate{0, 0}) &&
		(_game->state->level->depth() == -1)) {
		_show_confirm_stairs = true;
		auto &this_tile{
			_game->state->level->at(_game->state->get_player_pos())};
		if (!this_tile.is(TileProperty::EXPLORED))
			this_tile.set(TileProperty::EXPLORED);

	} else
		_show_confirm_stairs = false;

	_game->hide_console();
	_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
	std::optional<std::vector<MenuEntry>::const_iterator> camp_option{
		_camp_menu->items.begin()};

	const auto current_loc{_game->state->get_player_pos()};
	auto &this_tile{_game->state->level->at(current_loc)};
	if (!this_tile.is(TileProperty::EXPLORED))
		this_tile.set(TileProperty::EXPLORED);

	sf::Event event{};
	while (_window->isOpen()) {
		while (_window->pollEvent(event)) {

			// Check for Window Close
			if (event.type == sf::Event::Closed)
				return EXIT_ALL;

			// Handle enabling help overlay
			if (_system->input->check(WindowInput::SHOW_CONTROLS, event)) {
				_display->show_overlay();
				continue;
			} else
				_display->hide_overlay();

			if (_show_confirm_exit) {

				if (_left_icon_panel->selected)
					_left_icon_panel->selected = std::nullopt;
				if (_right_icon_panel->selected)
					_right_icon_panel->selected = std::nullopt;
				if (_status_bar->selected)
					_status_bar->selected = std::nullopt;

				auto dialog_input{_confirm_exit->handle_input(event)};
				if (dialog_input) {
					if (dialog_input.value() == WindowDialogButton::CLOSE) {
						_display->set_input_mode(
							WindowInputMode::NAVIGATE_MENU);
						_show_confirm_exit = false;
					} else if (dialog_input.value() ==
							   WindowDialogButton::YES) {
						_window->close();
					} else if (dialog_input.value() == WindowDialogButton::NO) {
						_display->set_input_mode(
							WindowInputMode::NAVIGATE_MENU);
						_show_confirm_exit = false;
					}
				}
			} else if (_in_character) {
				if (_system->input->check(
						WindowInput::SHOW_HIDE_CONSOLE, event))
					_game->toggle_console();
				else if (_system->input->check(WindowInput::LEFT, event))
					_cur_char.value()->left_view();
				else if (_system->input->check(WindowInput::RIGHT, event))
					_cur_char.value()->right_view();
				else if (_system->input->check(WindowInput::CANCEL, event)) {
					_display->set_input_mode(WindowInputMode::NAVIGATE_MENU);
					_cur_char = std::nullopt;
				} else if (_system->input->check(WindowInput::BACK, event)) {
					_display->set_input_mode(WindowInputMode::IN_GAME);
					_cur_char = std::nullopt;
					_game->save_game();
					_status_bar->refresh();
					_in_character = false;
					_display->generate("engine_base_ui");
				} else if (_system->input->check(WindowInput::CONFIRM, event)) {
					_cur_char.value()->right_view();
				} else if (_system->input->check(WindowInput::UP, event)) {
					if (_cur_char.value()->get_view() ==
						CharacterView::MAGE_SPELLS)
						_cur_char.value()->dec_hl_spell(SpellType::MAGE);
					else if (_cur_char.value()->get_view() ==
							 CharacterView::PRIEST_SPELLS)
						_cur_char.value()->dec_hl_spell(SpellType::PRIEST);

				} else if (_system->input->check(WindowInput::DOWN, event)) {
					if (_cur_char.value()->get_view() ==
						CharacterView::MAGE_SPELLS)
						_cur_char.value()->inc_hl_spell(SpellType::MAGE);
					else if (_cur_char.value()->get_view() ==
							 CharacterView::PRIEST_SPELLS)
						_cur_char.value()->inc_hl_spell(SpellType::PRIEST);
				} else if (_system->input->check(WindowInput::MOVE, event)) {
					if (_cur_char.value()->check_for_mouse_move(sf::Vector2f(
							static_cast<float>(
								sf::Mouse::getPosition(*_window).x),
							static_cast<float>(
								sf::Mouse::getPosition(*_window).y)))) {
						_cur_char.value()->set_view(
							_cur_char.value()->get_view());
					}
				}
			} else if (_in_camp) {

				if (_left_icon_panel->selected)
					_left_icon_panel->selected = std::nullopt;
				if (_right_icon_panel->selected)
					_right_icon_panel->selected = std::nullopt;
				if (_status_bar->selected)
					_status_bar->selected = std::nullopt;

				if (_system->input->check(WindowInput::CANCEL, event))
					_in_camp = false;

				if (_system->input->check(WindowInput::BACK, event))
					_in_camp = false;

				if (_system->input->check(
						WindowInput::SHOW_HIDE_CONSOLE, event))
					_game->toggle_console();
				else if (_system->input->check(WindowInput::UP, event))
					camp_option = _camp_menu->choose_previous();
				else if (_system->input->check(WindowInput::DOWN, event))
					camp_option = _camp_menu->choose_next();
				else if (_system->input->check(WindowInput::MOVE, event))
					camp_option = _camp_menu->set_mouse_selected(
						static_cast<sf::Vector2f>(
							sf::Mouse::getPosition(*_window)));
				else if (_system->input->check(WindowInput::CONFIRM, event)) {

					// We have selected something from the menu
					if (camp_option) {

						if (const MenuItem option_chosen{
								(*camp_option.value()).item};
							option_chosen == MenuItem::CP_LEAVE) {

							_game->save_game();
							_status_bar->refresh();
							_in_camp = false;
							_display->generate("engine_base_ui");
							_display->set_input_mode(WindowInputMode::IN_GAME);
							continue;
						} else if (option_chosen == MenuItem::CP_SAVE) {
							_game->save_game();
							return EXIT_MODULE;
						} else if (option_chosen == MenuItem::QUIT) {
							_show_confirm_exit = true;
							continue;
						} else if (option_chosen == MenuItem::CP_OPTIONS) {

							auto options{std::make_unique<Options>(
								_system, _display, _graphics)};
							auto result{options->start()};
							if (result == EXIT_ALL) {
								options->stop();
								return EXIT_ALL;
							}
							options->stop();
							_status_bar->refresh();
							_display->generate("engine_base_ui");
						} else if (option_chosen == MenuItem::CP_INSPECT) {
							_status_bar->refresh();
							auto result{_inspect->start()};
							if (result == MenuItem::ABORT) {
								_inspect->stop();
								return EXIT_ALL;
							}
							_inspect->stop();
							_status_bar->refresh();
							_display->generate("engine_base_ui");
						} else if (option_chosen == MenuItem::CP_REORDER) {
							_status_bar->refresh();
							auto new_party{_reorder->start()};
							if (new_party) {

								// TODO: handle aborts here too
								_game->state->set_party(new_party.value());
								_game->save_game();
								_game->load_game();
								_status_bar->refresh();
							}
							_reorder->stop();
							_status_bar->refresh();
							_display->generate("engine_base_ui");
						}
					}
				}
			} else {

				if (_display->get_input_mode() == WindowInputMode::IN_GAME) {

					if (_show_ouch) {

						auto dialog_input{_ouch->handle_input(event)};
						if (dialog_input) {
							if (dialog_input.value() ==
								WindowDialogButton::OK) {

								_display->set_input_mode(
									WindowInputMode::IN_GAME);
								_show_ouch = false;
								_ouch->set_valid(false);
							}
						}
					} else if (_show_confirm_stairs) {

						if (_left_icon_panel->selected)
							_left_icon_panel->selected = std::nullopt;
						if (_right_icon_panel->selected)
							_right_icon_panel->selected = std::nullopt;
						if (_status_bar->selected)
							_status_bar->selected = std::nullopt;

						auto dialog_input{_confirm_stairs->handle_input(event)};
						if (dialog_input) {
							if (dialog_input.value() ==
								WindowDialogButton::CLOSE) {
								_display->set_input_mode(
									WindowInputMode::IN_GAME);
								_show_confirm_stairs = false;
							} else if (dialog_input.value() ==
									   WindowDialogButton::NO) {
								_display->set_input_mode(
									WindowInputMode::IN_GAME);
								_show_confirm_stairs = false;
							} else if (dialog_input.value() ==
									   WindowDialogButton::YES) {
								_show_confirm_stairs = false;

								const auto current_loc{
									_game->state->get_player_pos()};
								if ((current_loc == Coordinate{0, 0}) &&
									(_game->state->level->depth() == -1)) {

									_game->save_game();
									return EXIT_MODULE;
								} else {

									_stairs_if();
									auto &to_tile{_game->state->level->at(
										_game->state->get_player_pos())};
									if (!to_tile.is(TileProperty::EXPLORED))
										to_tile.set_explored();
									_update_automap = true;
									_show_confirm_stairs = true;
									_game->save_game();
								}
							}
						}
					} else {

						if (_system->input->check(WindowInput::LEFT, event)) {
							_turn_left();
							_update_automap = true;
							_update_compass = true;
							_update_render = true;
						} else if (_system->input->check(
									   WindowInput::RIGHT, event)) {
							_turn_right();
							_update_automap = true;
							_update_compass = true;
							_update_render = true;
						} else if (_system->input->check(
									   WindowInput::UP, event)) {
							auto has_moved{_move_forward()};
							if (!has_moved) {
								_show_ouch = true;
								_ouch->reset_timed();
							} else {
								_teleport_if();
								auto &to_tile{_game->state->level->at(
									_game->state->get_player_pos())};
								if (!to_tile.is(TileProperty::EXPLORED))
									to_tile.set_explored();
								_update_automap = true;
							}
							if (_exit_maze_now) {
								_game->save_game();
								_exit_maze_now = false;
								return EXIT_MODULE;
							}
							_update_automap = true;
							_update_compass = true;
							_update_render = true;
						} else if (_system->input->check(
									   WindowInput::DOWN, event)) {
							auto has_moved{_move_backward()};
							if (!has_moved) {
								_show_ouch = true;
								_ouch->reset_timed();
							} else {
								_teleport_if();
								auto &to_tile{_game->state->level->at(
									_game->state->get_player_pos())};
								if (!to_tile.is(TileProperty::EXPLORED))
									to_tile.set_explored();
								_update_automap = true;
							}
							if (_exit_maze_now) {
								_game->save_game();
								_exit_maze_now = false;
								return EXIT_MODULE;
							}
							_update_automap = true;
							_update_compass = true;
							_update_render = true;
						} else if (_system->input->check(
									   WindowInput::CANCEL, event))
							_in_camp = true;
						else if (_system->input->check(
									 WindowInput::BACK, event))
							_in_camp = true;
						else if (_system->input->check(
									 WindowInput::CONFIRM, event)) {
							if (_status_bar->selected) {

								// Remember here status-bar selected is
								// 1-indexed, not 0-index so we need to take
								// away 1
								const auto character_chosen{
									(_status_bar->selected.value())};
								_cur_char = &_game->characters.at(
									_game->state->get_party_characters().at(
										character_chosen - 1));
								if (_cur_char) {
									_display->set_input_mode(
										WindowInputMode::BROWSE_CHARACTER);
									_cur_char.value()->set_view(
										CharacterView::SUMMARY);
									_in_character = true;
									continue;
								}
							}
						} else if (_system->input->check(
									   WindowInput::SHOW_HIDE_CONSOLE, event))
							_game->toggle_console();
						else if (_system->input->check(
									 WindowInput::MOVE, event)) {

							// Check for Mouse Overs
							sf::Vector2f mouse_pos{static_cast<sf::Vector2f>(
								sf::Mouse::getPosition(*_window))};
							std::optional<std::string> left_selected{
								_left_icon_panel->set_mouse_selected(
									(*_display->layout)
										["engine_base_ui:left_icon_panel"],
									mouse_pos)};
							if (left_selected) {
								_left_icon_panel->selected =
									left_selected.value();
								if (_right_icon_panel->selected)
									_right_icon_panel->selected = std::nullopt;
								if (_status_bar->selected)
									_status_bar->selected = std::nullopt;
							}

							std::optional<std::string> right_selected{
								_right_icon_panel->set_mouse_selected(
									(*_display->layout)
										["engine_base_ui:right_icon_panel"],
									mouse_pos)};
							if (right_selected) {
								_right_icon_panel->selected =
									right_selected.value();
								if (_left_icon_panel->selected)
									_left_icon_panel->selected = std::nullopt;
								if (_status_bar->selected)
									_status_bar->selected = std::nullopt;
							}

							std::optional<unsigned int> status_bar_selected{
								_status_bar->set_mouse_selected(mouse_pos)};
							if (status_bar_selected) {
								_status_bar->selected =
									status_bar_selected.value();
								if (_right_icon_panel->selected)
									_right_icon_panel->selected = std::nullopt;
								if (_left_icon_panel->selected)
									_left_icon_panel->selected = std::nullopt;
							}
						};

						// If we are in-game, and are on a tile with note
						auto current_loc{_game->state->get_player_pos()};
						auto note{(*_game->state->level)(current_loc)};
						if ((note.text.length() > 0) && (note.visible)) {

							_show_tile_note = true;
							_tile_note->update(note);
						} else
							_show_tile_note = false;
						_update_tile_note = false;
					}
				}
			}

			if (_update_render) {
				_render->refresh();
				_update_render = false;
			}
			if (_update_automap) {
				_automap->refresh();
				_update_automap = false;
			}
			if (_update_compass) {
				_compass->refresh();
				_update_compass = false;
			}
			if (_update_icon_panels) {
				_left_icon_panel->refresh(_in_camp);
				_right_icon_panel->refresh(_in_camp);
				_update_icon_panels = false;
			}
			if (_update_status_bar) {
				_status_bar->refresh();
				_update_status_bar = false;
			}
			_window->clear();
			_draw();
			_window->display();
		}
	}

	return EXIT_MODULE;
}

// Remember Y is reversed
auto Sorcery::Engine::_move_forward() -> bool {

	auto current_loc{_game->state->get_player_pos()};
	auto x_d{current_loc.x};
	auto y_d{current_loc.y};

	switch (_game->state->get_player_facing()) {
	case MapDirection::NORTH:
		++y_d;
		break;
	case MapDirection::SOUTH:
		--y_d;
		break;
	case MapDirection::EAST:
		++x_d;
		break;
	case MapDirection::WEST:
		--x_d;
		break;
	default:
		break;
	}

	if (x_d < 0)
		x_d = MAP_SIZE - 1;
	else if (x_d > MAP_SIZE - 1)
		x_d = 0;
	if (y_d < 0)
		y_d = MAP_SIZE - 1;
	else if (y_d > MAP_SIZE - 1)
		y_d = 0;

	const auto next_loc{Coordinate{x_d, y_d}};

	auto this_tile{_game->state->level->at(current_loc)};
	auto &next_tile{_game->state->level->at(next_loc)};

	auto this_wall_to_check{_game->state->get_player_facing()};
	auto next_wall_to_check{MapDirection::NONE};
	switch (_game->state->get_player_facing()) {
	case MapDirection::NORTH:
		next_wall_to_check = MapDirection::SOUTH;
		break;
	case MapDirection::SOUTH:
		next_wall_to_check = MapDirection::NORTH;
		break;
	case MapDirection::EAST:
		next_wall_to_check = MapDirection::WEST;
		break;
	case MapDirection::WEST:
		next_wall_to_check = MapDirection::EAST;
		break;
	default:
		break;
	}

	if (this_tile.walkable(this_wall_to_check)) {
		// if ((this_tile.walkable(this_wall_to_check)) &&
		//		(next_tile.walkable(next_wall_to_check))) {

		_game->state->set_player_pos(next_loc);
		if (!next_tile.is(TileProperty::EXPLORED))
			next_tile.set_explored();

		if (_game->state->level->stairs_at(next_loc)) {
			const auto current_loc{_game->state->get_player_pos()};
			const auto &to_tile{_game->state->level->at(current_loc)};
			if (to_tile.has(TileFeature::LADDER_UP))
				_confirm_stairs->set((
					*_display->layout)["engine_base_ui:dialog_ladder_up_text"]);
			else if (to_tile.has(TileFeature::LADDER_DOWN))
				_confirm_stairs->set((
					*_display
						 ->layout)["engine_base_ui:dialog_ladder_down_text"]);
			else if (to_tile.has(TileFeature::STAIRS_UP))
				_confirm_stairs->set((
					*_display->layout)["engine_base_ui:dialog_stairs_up_text"]);
			else if (to_tile.has(TileFeature::STAIRS_DOWN))
				_confirm_stairs->set((
					*_display
						 ->layout)["engine_base_ui:dialog_stairs_down_text"]);
			_show_confirm_stairs = true;
		} else
			_show_confirm_stairs = false;

		return true;
	} else
		return false;
}

auto Sorcery::Engine::_move_backward() -> bool {

	// Work out our new position
	auto current_loc{_game->state->get_player_pos()};
	auto x_d{current_loc.x};
	auto y_d{current_loc.y};

	switch (_game->state->get_player_facing()) {
	case MapDirection::NORTH:
		--y_d;
		break;
	case MapDirection::SOUTH:
		++y_d;
		break;
	case MapDirection::EAST:
		--x_d;
		break;
	case MapDirection::WEST:
		++x_d;
		break;
	default:
		break;
	}

	if (x_d < 0)
		x_d = MAP_SIZE - 1;
	else if (x_d > MAP_SIZE - 1)
		x_d = 0;
	if (y_d < 0)
		y_d = MAP_SIZE - 1;
	else if (y_d > MAP_SIZE - 1)
		y_d = 0;

	const auto next_loc{Coordinate{x_d, y_d}};

	// Check for walls etc between current square and new square
	auto this_tile{_game->state->level->at(current_loc)};
	auto &next_tile{_game->state->level->at(next_loc)};

	auto this_wall_to_check{MapDirection::NONE};
	switch (_game->state->get_player_facing()) {
	case MapDirection::NORTH:
		this_wall_to_check = MapDirection::SOUTH;
		break;
	case MapDirection::SOUTH:
		this_wall_to_check = MapDirection::NORTH;
		break;
	case MapDirection::EAST:
		this_wall_to_check = MapDirection::WEST;
		break;
	case MapDirection::WEST:
		this_wall_to_check = MapDirection::EAST;
		break;
	default:
		break;
	}
	auto next_wall_to_check{_game->state->get_player_facing()};

	if (this_tile.walkable(this_wall_to_check)) {
		// if ((this_tile.walkable(this_wall_to_check)) &&
		//	(next_tile.walkable(next_wall_to_check))) {

		_game->state->set_player_pos(next_loc);
		if (!next_tile.is(TileProperty::EXPLORED))
			next_tile.set_explored();
		if (_game->state->level->stairs_at(next_loc)) {
			const auto current_loc{_game->state->get_player_pos()};
			const auto &this_tile{_game->state->level->at(current_loc)};
			if (this_tile.has(TileFeature::LADDER_UP))
				_confirm_stairs->set((
					*_display->layout)["engine_base_ui:dialog_ladder_up_text"]);
			else if (this_tile.has(TileFeature::LADDER_DOWN))
				_confirm_stairs->set((
					*_display
						 ->layout)["engine_base_ui:dialog_ladder_down_text"]);
			else if (this_tile.has(TileFeature::STAIRS_UP))
				_confirm_stairs->set((
					*_display->layout)["engine_base_ui:dialog_stairs_up_text"]);
			else if (this_tile.has(TileFeature::STAIRS_DOWN))
				_confirm_stairs->set((
					*_display
						 ->layout)["engine_base_ui:dialog_stairs_down_text"]);
			_show_confirm_stairs = true;
		} else
			_show_confirm_stairs = false;
		return true;
	} else
		return false;
}

auto Sorcery::Engine::_turn_left() -> void {

	switch (_game->state->get_player_facing()) {
	case MapDirection::NORTH:
		_game->state->set_player_facing(MapDirection::WEST);
		break;
	case MapDirection::SOUTH:
		_game->state->set_player_facing(MapDirection::EAST);
		break;
	case MapDirection::EAST:
		_game->state->set_player_facing(MapDirection::NORTH);
		break;
	case MapDirection::WEST:
		_game->state->set_player_facing(MapDirection::SOUTH);
		break;
	default:
		break;
	}
}
auto Sorcery::Engine::_turn_right() -> void {

	switch (_game->state->get_player_facing()) {
	case MapDirection::NORTH:
		_game->state->set_player_facing(MapDirection::EAST);
		break;
	case MapDirection::SOUTH:
		_game->state->set_player_facing(MapDirection::WEST);
		break;
	case MapDirection::EAST:
		_game->state->set_player_facing(MapDirection::SOUTH);
		break;
	case MapDirection::WEST:
		_game->state->set_player_facing(MapDirection::NORTH);
		break;
	default:
		break;
	}
}

auto Sorcery::Engine::_stairs_if() -> bool {

	const auto tile{_game->state->level->at(_game->state->get_player_pos())};

	if (tile.has_stairs()) {

		auto destination{tile.has_stairs().value()};
		auto to_level{destination.to_level};

		// Floors are negative
		if (to_level < 0) {
			Level level{((*_game->levelstore)[to_level]).value()};
			_game->state->set_current_level(&level);
			auto &next_tile{_game->state->level->at(destination.to_loc)};
			_game->state->set_player_pos(destination.to_loc);
			next_tile.set_explored();
			if (next_tile.has(TileFeature::LADDER_UP))
				_confirm_stairs->set((
					*_display->layout)["engine_base_ui:dialog_ladder_up_text"]);
			else if (next_tile.has(TileFeature::LADDER_DOWN))
				_confirm_stairs->set((
					*_display
						 ->layout)["engine_base_ui:dialog_ladder_down_text"]);
			else if (next_tile.has(TileFeature::STAIRS_UP))
				_confirm_stairs->set((
					*_display->layout)["engine_base_ui:dialog_stairs_up_text"]);
			else if (next_tile.has(TileFeature::STAIRS_DOWN))
				_confirm_stairs->set((
					*_display
						 ->layout)["engine_base_ui:dialog_stairs_down_text"]);

			return true;
		}
	}

	return false;
}

auto Sorcery::Engine::_teleport_if() -> bool {

	const auto tile{_game->state->level->at(_game->state->get_player_pos())};

	// TODO: handle anti-teleport here in the future
	if (tile.has_teleport()) {

		auto destination{tile.has_teleport().value()};
		if (destination.to_level == 0) {

			// Special case of teleporting Back to castle
			_exit_maze_now = true;
			return true;
		} else if (destination.to_level == _game->state->level->depth()) {

			auto &next_tile{_game->state->level->at(destination.to_loc)};
			_game->state->set_player_pos(destination.to_loc);

			if (!next_tile.is(TileProperty::EXPLORED))
				next_tile.set_explored();
			if (_game->state->level->stairs_at(
					_game->state->get_player_pos())) {
				const auto current_loc{_game->state->get_player_pos()};
				const auto &this_tile{_game->state->level->at(current_loc)};
				if (this_tile.has(TileFeature::LADDER_UP))
					_confirm_stairs->set((
						*_display
							 ->layout)["engine_base_ui:dialog_ladder_up_text"]);
				else if (this_tile.has(TileFeature::LADDER_DOWN))
					_confirm_stairs->set((*_display->layout)
							["engine_base_ui:dialog_ladder_down_text"]);
				else if (this_tile.has(TileFeature::STAIRS_UP))
					_confirm_stairs->set((
						*_display
							 ->layout)["engine_base_ui:dialog_stairs_up_text"]);
				else if (this_tile.has(TileFeature::STAIRS_DOWN))
					_confirm_stairs->set((*_display->layout)
							["engine_base_ui:dialog_stairs_down_text"]);
				_show_confirm_stairs = true;
			} else
				_show_confirm_stairs = false;
		} else {

			// TODO: different level teleport
		}
	}

	return true;
}

auto Sorcery::Engine::stop() -> void {}

auto Sorcery::Engine::_draw() -> void {

	// Scale the Render appropriately
	const auto vf_c{(*_display->layout)["engine_base_ui:view_frame"]};
	const auto wfr_c{(*_display->layout)["engine_base_ui:wireframe_view"]};
	_render->setScale(std::stof(wfr_c["scale_x"].value()),
		std::stof(wfr_c["scale_y"].value()));

	// Standard Components
	_display->display("engine_base_ui");

	// Draw the Render
	_render->setPosition(wfr_c.x, wfr_c.y);
	_window->draw(*_render);

	if (_status_bar->selected)
		_status_bar->set_selected_background();
	_window->draw(*_status_bar);

	_window->draw(*_automap);
	_window->draw(*_compass);

	if (_left_icon_panel->selected)
		_left_icon_panel->set_selected_background();
	if (_right_icon_panel->selected)
		_right_icon_panel->set_selected_background();

	_window->draw(*_left_icon_panel);
	_window->draw(*_right_icon_panel);

	if (_show_tile_note) {
		_window->draw(*_tile_note);
	}

	// And the Menu
	if (_in_camp) {
		_window->draw(*_camp_menu_frame);
		_camp_menu->generate((*_display->layout)["engine_base_ui:camp_menu"]);
		const sf::Vector2f menu_pos(
			(*_display->layout)["engine_base_ui:camp_menu"].x,
			(*_display->layout)["engine_base_ui:camp_menu"].y);
		_camp_menu->setPosition(menu_pos);
		_window->draw(*_camp_menu);
	} else {
		if (_display->get_input_mode() == WindowInputMode::BROWSE_CHARACTER) {
			if (_cur_char) {

				// If we have a character
				_window->draw(*_cur_char_frame);

				_cur_char.value()->setPosition(
					(*_display->layout)["engine_base_ui:character"].x,
					(*_display->layout)["engine_base_ui:character"].y);
				_cur_char.value()->update();
				_window->draw(*_cur_char.value());
			}
		} else if (_display->get_input_mode() == WindowInputMode::IN_GAME) {

			if (_show_confirm_stairs) {
				_confirm_stairs->update();
				_window->draw(*_confirm_stairs);
			}

			if (_show_ouch) {
				_ouch->update();
				if (_ouch->get_valid())
					_window->draw(*_ouch);
				else {
					_show_ouch = false;
					_ouch->set_valid(false);
				}
			}
		}
	}

	if (_show_confirm_exit) {
		_confirm_exit->update();
		_window->draw(*_confirm_exit);
	}

	// Always draw the following
	_display->display_overlay();
	_display->display_cursor();

	if (_game->get_console_status()) {
		_console->refresh();
		_display->window->get_gui()->draw();
	}
}
