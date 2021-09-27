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
	forward.setOrigin(forward.getLocalBounds().width / 2,
		forward.getLocalBounds().height / 2);
	forward.rotate(std::stof(icon["rotation"].value()));
	forward.setScale(icon.scale, icon.scale);
	forward.setColor(sf::Color(icon.colour));
	forward.setPosition(icon.x + offset_x, icon.y + offset_y);
	_sprites.emplace_back(forward);

	auto turn_left{(*_graphics->icons)["direction"].value()};
	icon = Component{(*_display->layout)["icon_panel:turn_left"]};
	turn_left.setOrigin(turn_left.getLocalBounds().width / 2,
		turn_left.getLocalBounds().height / 2);
	turn_left.rotate(std::stof(icon["rotation"].value()));
	turn_left.setScale(icon.scale, icon.scale);
	turn_left.setColor(sf::Color(icon.colour));
	turn_left.setPosition(icon.x + offset_x, icon.y + offset_y);
	_sprites.emplace_back(turn_left);

	auto camp{(*_graphics->icons)["field"].value()};
	icon = Component{(*_display->layout)["icon_panel:camp"]};
	camp.setOrigin(
		camp.getLocalBounds().width / 2, camp.getLocalBounds().height / 2);
	camp.rotate(std::stof(icon["rotation"].value()));
	camp.setScale(icon.scale, icon.scale);
	camp.setColor(sf::Color(icon.colour));
	camp.setPosition(icon.x + offset_x, icon.y + offset_y);
	_sprites.emplace_back(camp);

	auto turn_right{(*_graphics->icons)["direction"].value()};
	icon = Component{(*_display->layout)["icon_panel:turn_right"]};
	turn_right.setOrigin(turn_right.getLocalBounds().width / 2,
		turn_right.getLocalBounds().height / 2);
	turn_right.rotate(std::stof(icon["rotation"].value()));
	turn_right.setScale(icon.scale, icon.scale);
	turn_right.setColor(sf::Color(icon.colour));
	turn_right.setPosition(icon.x + offset_x, icon.y + offset_y);
	_sprites.emplace_back(turn_right);

	auto backward{(*_graphics->icons)["direction"].value()};
	icon = Component{(*_display->layout)["icon_panel:backward"]};
	backward.setOrigin(backward.getLocalBounds().width / 2,
		backward.getLocalBounds().height / 2);
	backward.rotate(std::stof(icon["rotation"].value()));
	backward.setScale(icon.scale, icon.scale);
	backward.setColor(sf::Color(icon.colour));
	backward.setPosition(icon.x + offset_x, icon.y + offset_y);
	_sprites.emplace_back(backward);
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