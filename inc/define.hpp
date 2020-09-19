// Copyright (C) 2018 Dave Moore
//
// This file is part of Sorcery: Dreams of the Mad Overlord ("Sorcery").
//
// Sorcery is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Sorcery is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Sorcery.  If not, see <http://www.gnu.org/licenses/>.
//
// Additional permission under GNU GPL version 3 section 7:
//
// If you modify this Program, or any covered work, by linking or combining it
// with the libraries referred to in README (or a modified version of said
// libraries), containing parts covered by the terms of said libraries, the
// licensors of this Program grant you additional permission to convey the
// resulting work.

#pragma once

#include "main.hpp"

namespace Sorcery
{
	// Options
	inline constexpr auto EMPTY = "";
	inline constexpr auto ON = "on";
	inline constexpr auto OFF = "off";

	using namespace std::literals;

		// Directories
		inline constexpr auto CONFIG_DIR = "cfg"sv;
		inline constexpr auto DATA_DIR = "dat"sv;
		inline constexpr auto DOCUMENTS_DIR = "doc"sv;
		inline constexpr auto GRAPHICS_DIR = "gfx"sv;
		inline constexpr auto VIDEO_DIR = "vfx"sv;

		// Files
		inline constexpr auto CONFIG_FILE = "config.ini"sv;
		inline constexpr auto DATABASE_FILE = "sorcery.db3"sv;
		inline constexpr auto EXPLAIN_FILE = "explain.json"sv;
		inline constexpr auto MONO_FONT_FILE = "monospace.otf"sv;
		inline constexpr auto PROPORTIONAL_FONT_FILE = "proportional.ttf"sv;
		inline constexpr auto HELP_FILE = "HELP"sv;
		inline constexpr auto LAYOUT_FILE = "layout.json"sv;
		inline constexpr auto LICENSE_FILE = "LICENSE"sv;
		inline constexpr auto README_FILE = "README"sv;
		inline constexpr auto STRINGS_FILE = "strings.json"sv;
		inline constexpr auto VERSION_FILE = "VERSION"sv;

		// Layout
		inline constexpr auto COMPONENT_SCREEN = 0;
		inline constexpr auto COMPONENT_NAME = 1;
		inline constexpr auto COMPONENT_X = 2;
		inline constexpr auto COMPONENT_Y = 3;
		inline constexpr auto COMPONENT_W = 4;
		inline constexpr auto COMPONENT_H = 5;
		inline constexpr auto COMPONENT_SCALE = 6;
		inline constexpr auto COMPONENT_FONT_TYPE = 7;
		inline constexpr auto COMPONENT_SIZE = 8;
		inline constexpr auto COMPONENT_COLOUR = 9;
		inline constexpr auto COMPONENT_STRING = 10;

		// Graphics
		inline constexpr auto BACKGROUND_TEXTURE = "background.png"sv;
		inline constexpr auto BANNER_TEXTURE = "banner.png"sv;
		inline constexpr auto CREATURES_TEXTURE = "creatures.png"sv;
		inline constexpr auto LOGO_TEXTURE = "logo.png"sv;
		inline constexpr auto NINEPATCH_TEXTURE = "ninepatch.png"sv;
		inline constexpr auto SPLASH_TEXTURE = "splash.png"sv;
		inline constexpr auto UI_TEXTURE = "ui.png"sv;

		// Videos
		inline constexpr auto MENU_VIDEO = "menu.mp4"sv;








	// For ease of use
	//static const std::string EMPTY = {""};
	//static const std::string ON = {"on"};
	//static const std::string OFF = {"off"};

	// Files
	/* static const std::string CFG_FILE = {"config.ini"};
	static const std::string DB_FILE = {"sorcery.db3"};
	static const std::string STR_FILE = {"strings.json"};
	static const std::string EXPLAIN_FILE = {"explain.json"};
	static const std::string LICENSE_FILE = {"LICENSE"};
	static const std::string HELP_FILE = {"HELP"};
	static const std::string README_FILE = {"README"};
	static const std::string VERSION_FILE = {"VERSION"}; */

	// Graphics
	/* static const std::string FONT_FILE = {"font.png"};
	static const std::string LOGO_FILE = {"logo.png"};
	static const std::string BKG_FILE = {"background.png"};
	static const std::string BLANK_CREATURE_FILE = {"blank.png"};
	static const std::string ICONS_FILE = {"icons.png"};
	static const std::string CONCEPTS_FILE = {"concepts.png"};
	static const std::string PORTRAITS_FILE = {"portraits.png"};
	static const std::string CHARACTER_TEMPLATE_FILE_MAIN = {"character-main.xp"};
	static const std::string CHARACTER_TEMPLATE_FILE_DETAILED = {"character-detailed.xp"};
	static const std::string CHARACTER_TEMPLATE_FILE_INVENTORY = {"character-inventory.xp"};
	static const std::string CHARACTER_TEMPLATE_FILE_SPELLS_MAGE = {"character-spells-mage.xp"};
	static const std::string CHARACTER_TEMPLATE_FILE_SPELLS_CLERIC = {"character-spells-cleric.xp"};
	static const std::string CHARACTER_TEMPLATE_FILE_MESSAGES = {"character-messages.xp"};
	static const std::string CHARACTER_TEMPLATE_FILE_ACHIEVEMENTS = {"character-achievements.xp"}; */
	// Game config settings
	static const std::string DEFAULT_SCREEN_HEIGHT = {"default_height"};
	static const std::string DEFAULT_SCREEN_WIDTH = {"default_width"};
	static const std::string CURRENT_SCREEN_HEIGHT = {"current_height"};
	static const std::string CURRENT_SCREEN_WIDTH = {"current_width"};

	// Gameplay settings
	static const int NUM_GAME_SETTINGS = {20};
	static const std::string OPT_AUTO_SAVE = {"autosave"};
	static const std::string OPT_STRICT_MODE = {"strict_mode"};
	static const std::string OPT_CHEAT_MODE = {"cheat_mode"};
	static const std::string OPT_DICE_ROLLS = {"dice_rolls"};
	static const std::string GFX_WIREFRAME = {"wireframe_mode"};
	static const std::string GFX_TEXTURES = {"display_textures"};
	static const std::string GFX_TRAPS = {"display_traps"};
	static const std::string GFX_TELEPORTERS = {"display_teleporters"};
	static const std::string GFX_ENCOUNTERS = {"display_encounters"};
	static const std::string GFX_PROGRESS = {"display_progress"};
	static const std::string GAME_ALLOW_MIXED_ALIGNMENT_PARTY = {"allow_mixed_alignment_party"};
	static const std::string GAME_STAT_LOSS_ON_LEVEL_UP = {"stat_loss_on_level_up"};
	static const std::string GAME_REROLL_HIT_POINTS_ON_LEVEL_GAIN = {"reroll_hit_points_on_level_gain"};
	static const std::string GAME_STAT_RESET_ON_CLASS_CHANGE = {"stat_reset_on_class_change"};
	static const std::string GAME_AGING_ON_CLASS_CHANGE = {"aging_on_class_change"};
	static const std::string GAME_ALLOW_RANGED_WEAPONS = {"allow_ranged_weapons"};
	static const std::string GAME_SPELLCASTING_IN_SURPRISE_ROUND = {"spellcasting_in_surprise_round"};
	static const std::string GAME_BATCH_HEALING_AFTER_RETURN_TO_CASTLE = {"batch_healing_after_return_to_castle"};
	static const std::string GAME_REROLL_ONES_ON_DICE = {"reroll_ones_on_dice"};
	static const std::string GAME_ALLOW_AMBUSH_HIDE = {"allow_ambush_hide"};

	// Screen Size settings
	static const unsigned int MINIMUM_SCREEN_WIDTH = {1280};
	static const unsigned int MINIMUM_SCREEN_HEIGHT = {720};
	static const unsigned int TEXT_FILE_WIDTH = {80};
	static const unsigned int MAP_CELL_GRID_WIDTH = {100};

	// Miscellaneous error strings
	static const std::string EXPLAIN_KEY_NOT_FOUND = {"EXPLAIN KEY NOT FOUND"};
	static const std::string KEY_NOT_FOUND = {"KEY NOT FOUND"};
	static const std::string STRINGS_NOT_LOADED = {"GAME STRINGS NOT LOADED"};
	static const std::string FILE_NOT_FOUND = {"FILE NOT FOUND"};
} // namespace
