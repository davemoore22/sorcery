// Copyright (C) 2021 Dave Moore
//
// This file is part of Sorcery: Shadows under Llylgamyn.
//
// Sorcery: Shadows under Llylgamyn is free software: you can redistribute it
// and/or modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation, either version 2 of the License,
// or (at your option) any later version.
//
// Sorcery: Shadows under Llylgamyn is distributed in the hope that it will
// be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Sorcery: Shadows under Llylgamyn.  If not, see
// <http://www.gnu.org/licenses/>.
//
// If you modify this Program, or any covered work, by linking or combining it
// with the libraries referred to in README (or a modified version of said
// libraries), containing parts covered by the terms of said libraries, the
// licensors of this Program grant you additional permission to convey the
// resulting work.

#include "iconpanel.hpp"

Sorcery::IconPanel::IconPanel(System *system, Display *display,
	Graphics *graphics, Game *game, Component layout)
	: _system{system}, _display{display}, _graphics{graphics}, _game{game},
	  _layout{layout} {

	_sprites.clear();
	_texts.clear();

	// Frame sprite is always sprite 0
	if (_frame.get()) {
		_frame.release();
		_frame.reset();
	}
	_frame = std::make_unique<Frame>(_display->ui_texture,
		WindowFrameType::NORMAL, _layout.w, _layout.h, _layout.colour,
		_layout.background, _layout.alpha);
	auto fsprite{_frame->sprite};
	fsprite.setPosition(0, 0);
	_sprites.emplace_back(fsprite);

	auto offset_x{std::stoi(_layout["offset_x"].value())};
	auto offset_y{std::stoi(_layout["offset_y"].value())};

	// Order here is done by the value of the IconPanelButton enum
	auto forward{(*_graphics->icons)["direction"].value()};
	Component icon{(*_display->layout)["icon_panel:forward"]};
	_set_icon(forward, icon, offset_x, offset_y);
	_sprites.emplace_back(forward);

	auto turn_left{(*_graphics->icons)["direction"].value()};
	icon = Component{(*_display->layout)["icon_panel:turn_left"]};
	_set_icon(turn_left, icon, offset_x, offset_y);
	_sprites.emplace_back(turn_left);

	auto camp{(*_graphics->icons)["field"].value()};
	icon = Component{(*_display->layout)["icon_panel:camp"]};
	_set_icon(camp, icon, offset_x, offset_y);
	_sprites.emplace_back(camp);

	auto turn_right{(*_graphics->icons)["direction"].value()};
	icon = Component{(*_display->layout)["icon_panel:turn_right"]};
	_set_icon(turn_right, icon, offset_x, offset_y);
	_sprites.emplace_back(turn_right);

	auto backward{(*_graphics->icons)["direction"].value()};
	icon = Component{(*_display->layout)["icon_panel:backward"]};
	_set_icon(backward, icon, offset_x, offset_y);
	_sprites.emplace_back(backward);

	auto party{(*_graphics->icons)["human"].value()};
	icon = Component{(*_display->layout)["icon_panel:party"]};
	_set_icon(party, icon, offset_x, offset_y);
	_sprites.emplace_back(party);

	auto magic{(*_graphics->icons)["exp"].value()};
	icon = Component{(*_display->layout)["icon_panel:magic"]};
	_set_icon(magic, icon, offset_x, offset_y);
	_sprites.emplace_back(magic);

	auto achievements{(*_graphics->icons)["marks"].value()};
	icon = Component{(*_display->layout)["icon_panel:achivements"]};
	_set_icon(achievements, icon, offset_x, offset_y);
	_sprites.emplace_back(achievements);

	auto examine{(*_graphics->icons)["disable"].value()};
	icon = Component{(*_display->layout)["icon_panel:examine"]};
	_set_icon(examine, icon, offset_x, offset_y);
	_sprites.emplace_back(examine);
}

auto Sorcery::IconPanel::_set_icon(
	sf::Sprite &sprite, Component layout, int offset_x, int offset_y) -> void {

	sprite.setOrigin(
		sprite.getLocalBounds().width / 2, sprite.getLocalBounds().height / 2);
	sprite.rotate(std::stof(layout["rotation"].value()));
	sprite.setScale(layout.scale, layout.scale);
	sprite.setColor(sf::Color(layout.colour));
	const auto comp_offset_x{[&] {
		if (layout["offset_x"])
			return std::stoi(layout["offset_x"].value());
		else
			return 0;
	}()};
	const auto comp_offset_y{[&] {
		if (layout["offset_y"])
			return std::stoi(layout["offset_y"].value());
		else
			return 0;
	}()};

	sprite.setPosition(layout.x + offset_x + comp_offset_x,
		layout.y + offset_y + comp_offset_y);
}

auto Sorcery::IconPanel::refresh(bool in_camp) -> void {

	// This only changes the colour and enables/disables them as appropriate
	Component forward{(*_display->layout)["icon_panel:forward"]};
	Component turn_left{(*_display->layout)["icon_panel:turn_left"]};
	Component turn_right{(*_display->layout)["icon_panel:turn_right"]};
	Component backward{(*_display->layout)["icon_panel:backward"]};
	Component camp{(*_display->layout)["icon_panel:camp"]};

	if (in_camp) {

	} else {
	}
}

auto Sorcery::IconPanel::draw(
	sf::RenderTarget &target, sf::RenderStates states) const -> void {

	states.transform *= getTransform();

	// Draw the standard components
	for (const auto &sprite : _sprites)
		target.draw(sprite, states);

	for (const auto &text : _texts)
		target.draw(text, states);
}
