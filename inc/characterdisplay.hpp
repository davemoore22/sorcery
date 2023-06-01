
// Copyright (C) 2023 Dave Moore
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

#pragma once

// clang-format off
#include "character.hpp"
#include "display.hpp"
#include "graphics.hpp"
#include "main.hpp"
#include "system.hpp"
#include "spellpanel.hpp"


namespace Sorcery {

class SpellSummary;

class CharacterDisplay: public sf::Transformable, public sf::Drawable {

	public:

	// Constructor
	CharacterDisplay(System *system, Display *display, Graphics *graphics, Character *character);

	// Public Methods
	auto set(Character* character) -> void;
	auto get_view() const -> CharacterView;
	auto set_view(const CharacterView value) -> void;
	auto left_view() -> void;
	auto right_view() -> void;
	auto inc_hl_spell(SpellType type) -> void;
	auto dec_hl_spell(SpellType type) -> void;
	auto update() -> void;
	auto check_for_mouse_move(sf::Vector2f mouse_pos) -> std::optional<SpellID>;
	auto check_for_action_mouse_move(sf::Vector2f mouse_pos) -> std::optional<MenuItem>;
	auto generate_display() -> void;
	auto set_mode(CharacterMode value) -> void;

	// Public Members
	std::map<SpellID, sf::FloatRect> mage_spell_bounds;
	std::map<SpellID, sf::FloatRect> priest_spell_bounds;
	std::map<SpellID, sf::Text *> mage_spell_texts;
	std::map<SpellID, sf::Text *> priest_spell_texts;
	std::map<MenuItem, sf::Text *> action_menu_texts;
	std::map<MenuItem, sf::FloatRect> action_menu_bounds;

	private:

	// Private Methods
	auto virtual draw(sf::RenderTarget &target, sf::RenderStates states) const -> void;
	auto _generate_display() -> void;
	auto _generate_summary_icons() -> void;
	auto _get_character_portrait() -> sf::Sprite;
	auto _add_text(Component &component, std::string format, std::string value, bool is_view = true) -> sf::Text *;
	auto _add_icon(Component &component, std::string icon_key) -> void;
	auto _get_spell_icon(SpellCategory category) -> std::optional<sf::Sprite>;

	// Private Members
	System *_system;
	Display *_display;
	Graphics *_graphics;
	Character *_character;

	std::map<std::string, sf::Sprite> _sprites;
	std::map<std::string, sf::Text> _texts;
	std::map<std::string, std::shared_ptr<Frame>> _frames;
	std::map<std::string, sf::Sprite> _v_sprites;
	std::map<std::string, sf::Text> _v_texts;
	std::map<std::string, std::shared_ptr<Frame>> _v_frames;
	std::shared_ptr<SpellPanel> _spell_panel;
	Component _spell_panel_c;

	SpellID _hl_mage_spell;
	SpellID _hl_priest_spell;
	MenuItem _hl_action_item;
	sf::RectangleShape _hl_mage_spell_bg;
	sf::RectangleShape _hl_priest_spell_bg;
	sf::RectangleShape _hl_action_item_bg;
};

}
