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

#include "gui/partypanel.hpp"
#include "common/enum.hpp"
#include "core/animation.hpp"
#include "core/display.hpp"
#include "core/game.hpp"
#include "core/graphics.hpp"
#include "core/state.hpp"
#include "core/system.hpp"
#include "core/window.hpp"
#include "resources/resourcemanager.hpp"
#include "resources/stringstore.hpp"
#include "types/character.hpp"

// Standard Constructor
Sorcery::PartyPanel::PartyPanel(System *system, Display *display, Graphics *graphics, Game *game, Component layout)
	: _system{system}, _display{display}, _graphics{graphics}, _game{game}, _layout{layout} {

	_texts.clear();
	_sprites.clear();
	bounds.clear();

	selected = std::nullopt;

	// Frame and Legend
	_frame = std::make_unique<Frame>(
		_display->ui_texture, _layout.w, _layout.h, _layout.colour, _layout.background, _layout.alpha);
	auto fsprite{_frame->sprite};
	fsprite.setPosition(0, 0);
	_sprites.emplace_back(fsprite);

	width = fsprite.getLocalBounds().width;
	height = fsprite.getLocalBounds().height;

	// Do the Legend
	auto legend_x{std::stoi(_layout["legend_offset_x"].value())};
	auto legend_y{std::stoi(_layout["legend_offset_x"].value())};

	sf::Text legend_text{};
	legend_text.setFont(_system->resources->fonts[_layout.font]);
	legend_text.setCharacterSize(_layout.size);
	legend_text.setFillColor(sf::Color(_layout.colour));
	if (_display->get_bold())
		legend_text.setStyle(sf::Text::Bold);
	legend_text.setPosition(legend_x * _display->window->get_cw(), legend_y * _display->window->get_ch());

	auto legend{(*_display->string)["PARTY_PANEL_LEGEND"]};
	if (_display->get_upper())
		std::transform(legend.begin(), legend.end(), legend.begin(), ::toupper);
	legend_text.setString(legend);

	_texts.push_back(legend_text);
}

auto Sorcery::PartyPanel::refresh() -> void {

	using enum Enums::Character::CStatus;

	// We keep the frame and the legend but clear everything else
	_sprites.resize(1);
	_texts.resize(1);
	bounds.clear();

	if (_game->state->get_party_size() > 0) {

		// Now do each character in the party
		auto count{0u};
		auto position{1u};
		auto character_x{std::stoi(_layout["party_offset_x"].value())};
		auto character_y{std::stoi(_layout["party_offset_y"].value())};
		auto bar_width{std::stoi(_layout["bar_width"].value())};
		const auto party{_game->state->get_party_characters()};
		for (auto _id : party) {
			auto character{_game->characters.at(_id)};
			auto summary{character.get_sb_text(position)};
			if (_display->get_upper())
				std::transform(summary.begin(), summary.end(), summary.begin(), ::toupper);

			sf::Text character_text{};
			if (_display->get_bold())
				character_text.setStyle(sf::Text::Bold);
			character_text.setFont(_system->resources->fonts[_layout.font]);
			character_text.setCharacterSize(_layout.size);
			character_text.setString(summary);

			if (selected) {
				if (selected.value() == position) {

					character_text.setOutlineColor(sf::Color(0, 0, 0));
					character_text.setOutlineThickness(1);
				}
			}

			switch (character.get_status()) {
			case OK:
				if (character.get_poisoned_rate() > 0)
					character_text.setFillColor(
						sf::Color(std::stoull(_layout["colour_poisoned"].value(), nullptr, 16)));
				else if (character.get_max_hp() / character.get_current_hp() > 5)
					character_text.setFillColor(
						sf::Color(std::stoull(_layout["colour_low_health"].value(), nullptr, 16)));
				else
					character_text.setFillColor(sf::Color(std::stoull(_layout["colour_ok"].value(), nullptr, 16)));
				break;
			case AFRAID:
				[[fallthrough]];
			case SILENCED:
				[[fallthrough]];
			case ASLEEP:
				if (character.get_max_hp() / character.get_current_hp() > 5)
					character_text.setFillColor(
						sf::Color(std::stoull(_layout["colour_low_health"].value(), nullptr, 16)));
				else
					character_text.setFillColor(sf::Color(std::stoull(_layout["colour_ok"].value(), nullptr, 16)));
				break;
			case ASHES:
				character_text.setFillColor(sf::Color(std::stoull(_layout["colour_ashes"].value(), nullptr, 16)));
				break;
			case DEAD:
				character_text.setFillColor(sf::Color(std::stoull(_layout["colour_dead"].value(), nullptr, 16)));
				break;
			case HELD:
				character_text.setFillColor(sf::Color(std::stoull(_layout["colour_held"].value(), nullptr, 16)));
				break;
			case LOST:
				character_text.setFillColor(sf::Color(std::stoull(_layout["colour_lost"].value(), nullptr, 16)));
				break;
			case STONED:
				character_text.setFillColor(sf::Color(std::stoull(_layout["colour_stoned"].value(), nullptr, 16)));
				break;
			default:
				character_text.setFillColor(sf::Color(_layout.colour));
			}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnarrowing"
			const sf::Vector2f pos{
				character_x * _display->window->get_cw(), (character_y + position - 1) * _display->window->get_ch()};
			character_text.setPosition(pos.x, pos.y - character_text.getLocalBounds().top);

#pragma GCC diagnostic pop
			bounds.emplace_back(pos.x, pos.y, bar_width * _display->window->get_cw(), _display->window->get_ch());
			_texts.emplace_back(character_text);

			if (selected) {
				if (selected.value() == position) {
					sf::RectangleShape bg{
						sf::Vector2f(bar_width * _display->window->get_cw(), _display->window->get_ch())};
					bg.setPosition(pos.x, pos.y);
					if (_layout.animated)
						bg.setFillColor(_graphics->animation->selected_colour);
					else
						bg.setFillColor(sf::Color(_layout.background));
					_selected_bg = bg;
				}
			}

			++position;
			++count;
		}
	}
}

auto Sorcery::PartyPanel::set_mouse_selected(sf::Vector2f mouse_pos) -> std::optional<unsigned int> {

	if (bounds.size() > 0) {
		const sf::Vector2f global_pos{this->getPosition()};
		mouse_pos -= global_pos;
		auto it{std::find_if(
			bounds.begin(), bounds.end(), [&mouse_pos](const auto &item) { return item.contains(mouse_pos); })};
		if (it != bounds.end())
			return std::distance(bounds.begin(), it) + 1; // we're working on position which is 1-indexed
		else
			return std::nullopt;
	} else
		return std::nullopt;
}

auto Sorcery::PartyPanel::draw(sf::RenderTarget &target, sf::RenderStates states) const -> void {

	states.transform *= getTransform();

	// Draw the standard components
	for (const auto &sprite : _sprites)
		target.draw(sprite, states);

	if (selected)
		target.draw(_selected_bg, states);

	for (const auto &text : _texts) {
		target.draw(text, states);
	}
}
