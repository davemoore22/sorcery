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

#pragma once

#include "main.hpp"

namespace Sorcery {

	using namespace std::literals;

	// Options
	inline constexpr auto EMPTY{""};
	inline constexpr auto ON{"on"};
	inline constexpr auto OFF{"off"};

	// Exit Codes
	inline constexpr auto CONTINUE{-1};
	inline constexpr auto EXIT_ALL{0};
	inline constexpr auto EXIT_MODULE{1};
	inline constexpr auto EXIT_STRING{"exit_game"};
	inline constexpr auto EXIT_CHARACTER{-1};

	// Save Version
	inline constexpr auto SAVE_VERSION{1};

	// Party Size
	inline constexpr auto MAX_PARTY_SIZE{6};

	// Map Size
	inline constexpr auto MAP_SIZE{20};

	// Command Line Arguments
	inline constexpr auto SKIP_INTRO{"skip-intro"sv};
	inline constexpr auto CONTINUE_GAME{"continue-game"sv};
	inline constexpr auto GO_TO_MAZE{"go-to-maze"sv};
	inline constexpr auto GO_TO_TRAINING{"go-to-training"sv};
	inline constexpr auto GO_TO_TAVERN{"go-to-tavern"sv};
	inline constexpr auto GO_TO_INN{"go-to-inn"sv};

	// Directories
	inline constexpr auto CONFIG_DIR{"cfg"sv};
	inline constexpr auto DATA_DIR{"dat"sv};
	inline constexpr auto DOCUMENTS_DIR{"doc"sv};
	inline constexpr auto GUI_DIR{"gui"sv};
	inline constexpr auto GRAPHICS_DIR{"gfx"sv};
	inline constexpr auto VIDEO_DIR{"vfx"sv};

	// Files
	inline constexpr auto CONFIG_FILE{"config.ini"sv};
	inline constexpr auto DATABASE_FILE{"sorcery.db3"sv};
	inline constexpr auto MONO_FONT_FILE{"monospace.ttf"sv};
	inline constexpr auto PROPORTIONAL_FONT_FILE{"proportional.ttf"sv};
	inline constexpr auto TEXT_FONT_FILE{"text.ttf"sv};
	inline constexpr auto INPUT_FONT_FILE{"input.ttf"sv};
	inline constexpr auto GUI_FILE{"nanogui.style"sv};
	inline constexpr auto HELP_FILE{"HELP"sv};
	inline constexpr auto ICONS_FILE{"icons.json"sv};
	inline constexpr auto LAYOUT_FILE{"layout.json"sv};
	inline constexpr auto LICENSE_FILE{"LICENSE"sv};
	inline constexpr auto LEVELS_FILE{"levels.json"sv};
	inline constexpr auto README_FILE{"README"sv};
	inline constexpr auto STRINGS_FILE{"strings.json"sv};
	inline constexpr auto TEXTURES_FILE{"textures.json"sv};
	inline constexpr auto VERSION_FILE{"VERSION"sv};

	// Graphics
	inline constexpr auto AUTOMAP_TEXTURE{"automap.png"sv};
	inline constexpr auto BACKGROUND_TEXTURE{"background.png"sv};
	inline constexpr auto BANNER_TEXTURE{"banner.png"sv};
	inline constexpr auto CONTROLS_TEXTURE{"controls.png"sv};
	inline constexpr auto CREATURES_KNOWN_TEXTURE{"creaturesknown.jpg"sv};
	inline constexpr auto CREATURES_UNKNOWN_TEXTURE{"creaturesunknown.jpg"sv};
	inline constexpr auto DOORS_TEXTURE{"doors.png"sv};
	inline constexpr auto FLOORS_TEXTURE{"floors.png"sv};
	inline constexpr auto ICONS_TEXTURE{"icons.png"sv};
	inline constexpr auto LOGO_TEXTURE{"logo.png"sv};
	inline constexpr auto NINEPATCH_TEXTURE{"ninepatch.png"sv};
	inline constexpr auto PORTRAITS_TEXTURE{"potraits.png"sv};
	inline constexpr auto SPLASH_TEXTURE{"splash.png"sv};
	inline constexpr auto TOWN_TEXTURE{"town.png"sv};
	inline constexpr auto UI_TEXTURE{"ui.png"sv};
	inline constexpr auto VIEW_TEXTURE{"view.png"sv};
	inline constexpr auto WALLS_TEXTURE{"walls.png"sv};
	inline constexpr auto WIREFRAME_TEXTURE{"wireframe.png"sv};

	// Other Images
	inline constexpr auto LOADING_IMAGE{"loading.png"sv};

	// Videos
	inline constexpr auto MENU_VIDEO{"menu.mp4"sv};

	// Frame Limit
	inline constexpr auto FRAMERATE{60u};

	// View Depth
	inline constexpr auto DARK_VIEW_DEPTH{3u};
	inline constexpr auto LIGHT_VIEW_DEPTH{6u};

	// Timed Settings
	inline constexpr auto DELAY_OUCH{1000u};
	inline constexpr auto DELAY_PIT{2000u};
	inline constexpr auto DELAY_CHUTE{2000u};
	inline constexpr auto DELAY_ELEVATOR{2000u};
	inline constexpr auto DELAY_DEFAULT{1000u};
	inline constexpr auto DELAY_RESTING{2000u};

	// Miscellaneous Settings
	inline constexpr auto MAX_PORTRAIT_INDEX{29u};
	inline constexpr auto CREATURE_TILE_SIZE{400};
	inline constexpr auto DUNGEON_TILE_SIZE{400};
	inline constexpr auto PORTRAIT_TILE_SIZE{600};
	inline constexpr auto VIEW_OFFSET_SIZE{1046};
	inline constexpr auto VIEW_MIRROR_SIZE{1046};
	inline constexpr auto ICON_TILE_SIZE{511.f};
	inline constexpr auto CREATURE_TILE_ROW_COUNT{20};
	inline constexpr auto DUNGEON_TILE_ROW_COUNT{15};
	inline constexpr auto PORTRAIT_TILE_ROW_COUNT{6};
	inline constexpr auto ICON_TILE_ROW_COUNT{15};
	inline constexpr auto ATTRACT_MODE_TILE_SPACING{8};
	inline constexpr auto AUTOMAP_TILE_SIZE{21};
	inline constexpr auto AUTOMAP_TILE_ROW_COUNT{10};
	inline constexpr auto VIEW_WIDTH{304};
	inline constexpr auto VIEW_HEIGHT{176};

	// Game config settings
	static const std::string DEFAULT_SCREEN_HEIGHT{"default_height"};
	static const std::string DEFAULT_SCREEN_WIDTH{"default_width"};
	static const std::string CURRENT_SCREEN_HEIGHT{"current_height"};
	static const std::string CURRENT_SCREEN_WIDTH{"current_width"};
	static const std::string CELL_HEIGHT{"cell_height"};
	static const std::string CELL_WIDTH{"cell_width"};

	// Gameplay settings
	static const int NUM_GAME_SETTINGS{26};
	static const std::string OPT_AUTO_SAVE{"autosave"};
	static const std::string OPT_RECOMMENDED_MODE{"recommended_mode"};
	static const std::string OPT_STRICT_MODE{"strict_mode"};
	static const std::string OPT_CHEAT_MODE{"cheat_mode"};
	static const std::string OPT_DICE_ROLLS{"dice_rolls"};
	static const std::string GFX_WIREFRAME{"wireframe_mode"};
	static const std::string GFX_TEXTURES{"display_textures"};
	static const std::string GFX_TRAPS{"display_traps"};
	static const std::string GFX_TELEPORTERS{"display_teleporters"};
	static const std::string GFX_ENCOUNTERS{"display_encounters"};
	static const std::string GFX_PROGRESS{"display_progress"};
	static const std::string GAME_ALLOW_MIXED_ALIGNMENT_PARTY{"allow_mixed_alignment_party"};
	static const std::string GAME_STAT_LOSS_ON_LEVEL_UP{"stat_loss_on_level_up"};
	static const std::string GAME_REROLL_HIT_POINTS_ON_LEVEL_GAIN{"reroll_hit_points_on_level_gain"};
	static const std::string GAME_STAT_RESET_ON_CLASS_CHANGE{"stat_reset_on_class_change"};
	static const std::string GAME_AGING_ON_CLASS_CHANGE{"aging_on_class_change"};
	static const std::string GAME_ALLOW_RANGED_WEAPONS{"allow_ranged_weapons"};
	static const std::string GAME_SPELLCASTING_IN_SURPRISE_ROUND{"spellcasting_in_surprise_round"};
	static const std::string GAME_BATCH_HEALING_AFTER_RETURN_TO_CASTLE{"batch_healing_after_return_to_castle"};
	static const std::string GAME_REROLL_ONES_ON_DICE{"reroll_ones_on_dice"};
	static const std::string GAME_PROTECT_TELEPORT{"protect_teleport_rock"};
	static const std::string GAME_ALLOW_LOST_LEGATED{"allow_lost_legated"};
	static const std::string GAME_ALLOW_CURABLE_DRAIN{"allow_curable_drained_levels"};
	static const std::string GAME_ALLOW_SHARED_INVENTORY{"allow_shared_inventory"};
	static const std::string GAME_CAMPING_RESTORE_SPELL_POINTS{"camping_restores_spell_points"};
	static const std::string GAME_CHARGED_ITEMS{"charged_items"};

	// Screen Size settings
	static const unsigned int MINIMUM_SCREEN_WIDTH{1280};
	static const unsigned int MINIMUM_SCREEN_HEIGHT{720};
	static const unsigned int TEXT_FILE_WIDTH{80};
	static const unsigned int MAP_CELL_GRID_WIDTH{100};

	// Miscellaneous error strings
	static const std::string EXPLAIN_KEY_NOT_FOUND{"EXPLAIN KEY NOT FOUND"};
	static const std::string KEY_NOT_FOUND{"KEY NOT FOUND"};
	static const std::string STRINGS_NOT_LOADED{"GAME STRINGS NOT LOADED"};
	static const std::string FILE_NOT_FOUND{"FILE NOT FOUND"};
} // namespace Sorcery
