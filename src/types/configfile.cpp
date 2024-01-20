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

#include "types/configfile.hpp"

// Standard Constructor
Sorcery::ConfigFile::ConfigFile(CSimpleIniA *settings, const std::filesystem::path config_file_path)
	: _settings{settings}, _config_fp{config_file_path} {

	_load();
}

// Overload [] operator - remember if this is a pointer, use the (*options)[i]
// syntax to deference properly!
auto Sorcery::ConfigFile::operator[](const unsigned int i) -> bool & {

	return _options[i];
}

// Get a value from the config file
auto Sorcery::ConfigFile::get(std::string_view section, std::string_view value) const -> std::string {

	return _settings->GetValue(CSTR(std::string{section}), CSTR(std::string{value}));
}

// Check if the options have changed
bool Sorcery::ConfigFile::has_changed() {

	return _options == _options_backup;
}

auto Sorcery::ConfigFile::load() -> bool {

	return _load();
}

// Load settings from ini file
auto Sorcery::ConfigFile::_load() -> bool {

	// Attempt to read the settings from the Settings file if possible
	_options.fill(false);

	// Gameplay Settingss
	auto option{""s};
	option = _settings->GetValue("Options", CSTR(OPT_RECOMMENDED_MODE), CSTR(OPTION_OFF));
	_options[Enums::Config::Options::RECOMMENDED_MODE] = option.compare(OPTION_ON) == 0;
	option = _settings->GetValue("Options", CSTR(OPT_STRICT_MODE), CSTR(OPTION_OFF));
	_options[Enums::Config::Options::STRICT_MODE] = option.compare(OPTION_ON) == 0;
	option = _settings->GetValue("Options", CSTR(OPT_AUTO_SAVE), CSTR(OPTION_OFF));
	_options[Enums::Config::Options::AUTO_SAVE] = option.compare(OPTION_ON) == 0;
	option = _settings->GetValue("Options", CSTR(OPT_CHEAT_MODE), CSTR(OPTION_OFF));
	_options[Enums::Config::Options::CHEAT_MODE] = option.compare(OPTION_ON) == 0;
	option = _settings->GetValue("Options", CSTR(OPT_DICE_ROLLS), CSTR(OPTION_ON));
	_options[Enums::Config::Options::DICE_ROLLS] = option.compare(OPTION_ON) == 0;
	option = _settings->GetValue("Gameplay", CSTR(GAME_ALLOW_MIXED_ALIGNMENT_PARTY), CSTR(OPTION_ON));
	_options[Enums::Config::Options::ALLOW_MIXED_ALIGNMENT_PARTY] = option.compare(OPTION_ON) == 0;
	option = _settings->GetValue("Gameplay", CSTR(GAME_STAT_LOSS_ON_LEVEL_UP), CSTR(OPTION_OFF));
	_options[Enums::Config::Options::STAT_LOSS_ON_LEVEL_UP] = option.compare(OPTION_ON) == 0;
	option = _settings->GetValue("Gameplay", CSTR(GAME_REROLL_HIT_POINTS_ON_LEVEL_GAIN), CSTR(OPTION_OFF));
	_options[Enums::Config::Options::REROLL_HIT_POINTS_ON_LEVEL_GAIN] = option.compare(OPTION_ON) == 0;
	option = _settings->GetValue("Gameplay", CSTR(GAME_STAT_RESET_ON_CLASS_CHANGE), CSTR(OPTION_OFF));
	_options[Enums::Config::Options::STAT_RESET_ON_CLASS_CHANGE] = option.compare(OPTION_ON) == 0;
	option = _settings->GetValue("Gameplay", CSTR(GAME_AGING_ON_CLASS_CHANGE), CSTR(OPTION_OFF));
	_options[Enums::Config::Options::AGING_ON_CLASS_CHANGE] = option.compare(OPTION_ON) == 0;
	option = _settings->GetValue("Gameplay", CSTR(GAME_ALLOW_RANGED_WEAPONS), CSTR(OPTION_ON));
	_options[Enums::Config::Options::ALLOW_RANGED_WEAPONS] = option.compare(OPTION_ON) == 0;
	option = _settings->GetValue("Gameplay", CSTR(GAME_SPELLCASTING_IN_SURPRISE_ROUND), CSTR(OPTION_ON));
	_options[Enums::Config::Options::SPELLCASTING_IN_SURPRISE_ROUND] = option.compare(OPTION_ON) == 0;
	option = _settings->GetValue("Gameplay", CSTR(GAME_BATCH_HEALING_AFTER_RETURN_TO_CASTLE), CSTR(OPTION_ON));
	_options[Enums::Config::Options::BATCH_HEALING_AFTER_RETURN_TO_CASTLE] = option.compare(OPTION_ON) == 0;
	option = _settings->GetValue("Gameplay", CSTR(GAME_REROLL_ONES_ON_DICE), CSTR(OPTION_ON));
	_options[Enums::Config::Options::REROLL_ONES_ON_DICE] = option.compare(OPTION_ON) == 0;
	option = _settings->GetValue("Gameplay", CSTR(GAME_PROTECT_TELEPORT), CSTR(OPTION_ON));
	_options[Enums::Config::Options::PROTECT_TELEPORT] = option.compare(OPTION_ON) == 0;

	option = _settings->GetValue("Gameplay", CSTR(GAME_ALLOW_LOST_LEGATED), CSTR(OPTION_ON));
	_options[Enums::Config::Options::ALLOW_LOST_LEGATED] = option.compare(OPTION_ON) == 0;
	option = _settings->GetValue("Gameplay", CSTR(GAME_ALLOW_CURABLE_DRAIN), CSTR(OPTION_ON));
	_options[Enums::Config::Options::ALLOW_CURABLE_DRAIN] = option.compare(OPTION_ON) == 0;
	option = _settings->GetValue("Gameplay", CSTR(GAME_ALLOW_SHARED_INVENTORY), CSTR(OPTION_ON));
	_options[Enums::Config::Options::ALLOW_SHARED_INVENTORY] = option.compare(OPTION_ON) == 0;
	option = _settings->GetValue("Gameplay", CSTR(GAME_CAMPING_RESTORE_SPELL_POINTS), CSTR(OPTION_ON));
	_options[Enums::Config::Options::CAMPING_RESTORE_SPELL_POINTS] = option.compare(OPTION_ON) == 0;
	option = _settings->GetValue("Gameplay", CSTR(GAME_CHARGED_ITEMS), CSTR(OPTION_ON));
	_options[Enums::Config::Options::CHARGED_ITEMS] = option.compare(OPTION_ON) == 0;

	// Graphics Settings
	option = _settings->GetValue("Graphics", CSTR(GFX_WIREFRAME), CSTR(OPTION_OFF));
	_options[Enums::Config::Options::WIREFRAME_MODE] = option.compare(OPTION_ON) == 0;
	option = _settings->GetValue("Graphics", CSTR(GFX_TEXTURES), CSTR(OPTION_ON));
	_options[Enums::Config::Options::DISPLAY_TEXTURES] = option.compare(OPTION_ON) == 0;
	option = _settings->GetValue("Graphics", CSTR(GFX_TRAPS), CSTR(OPTION_ON));
	_options[Enums::Config::Options::DISPLAY_TRAPS] = option.compare(OPTION_ON) == 0;
	option = _settings->GetValue("Graphics", CSTR(GFX_TELEPORTERS), CSTR(OPTION_ON));
	_options[Enums::Config::Options::DISPLAY_TELEPORTERS] = option.compare(OPTION_ON) == 0;
	option = _settings->GetValue("Graphics", CSTR(GFX_ENCOUNTERS), CSTR(OPTION_ON));
	_options[Enums::Config::Options::DISPLAY_ENCOUNTERS] = option.compare(OPTION_ON) == 0;
	option = _settings->GetValue("Graphics", CSTR(GFX_PROGRESS), CSTR(OPTION_ON));
	_options[Enums::Config::Options::DISPLAY_PROGRESS] = option.compare(OPTION_ON) == 0;

	return true;
}

// Save current settings to ini file
bool Sorcery::ConfigFile::save() {

	_settings->SetValue(
		"Options", CSTR(OPT_RECOMMENDED_MODE), BOOL2OPTIONCSTR(_options[Enums::Config::Options::RECOMMENDED_MODE]));
	_settings->SetValue(
		"Options", CSTR(OPT_STRICT_MODE), BOOL2OPTIONCSTR(_options[Enums::Config::Options::STRICT_MODE]));
	_settings->SetValue("Options", CSTR(OPT_AUTO_SAVE), BOOL2OPTIONCSTR(_options[Enums::Config::Options::AUTO_SAVE]));
	_settings->SetValue("Options", CSTR(OPT_CHEAT_MODE), BOOL2OPTIONCSTR(_options[Enums::Config::Options::CHEAT_MODE]));
	_settings->SetValue("Options", CSTR(OPT_DICE_ROLLS), BOOL2OPTIONCSTR(_options[Enums::Config::Options::DICE_ROLLS]));
	_settings->SetValue(
		"Graphics", CSTR(GFX_WIREFRAME), BOOL2OPTIONCSTR(_options[Enums::Config::Options::WIREFRAME_MODE]));
	_settings->SetValue(
		"Graphics", CSTR(GFX_TEXTURES), BOOL2OPTIONCSTR(_options[Enums::Config::Options::DISPLAY_TEXTURES]));
	_settings->SetValue("Graphics", CSTR(GFX_TRAPS), BOOL2OPTIONCSTR(_options[Enums::Config::Options::DISPLAY_TRAPS]));
	_settings->SetValue(
		"Graphics", CSTR(GFX_TELEPORTERS), BOOL2OPTIONCSTR(_options[Enums::Config::Options::DISPLAY_TELEPORTERS]));
	_settings->SetValue(
		"Graphics", CSTR(GFX_ENCOUNTERS), BOOL2OPTIONCSTR(_options[Enums::Config::Options::DISPLAY_ENCOUNTERS]));
	_settings->SetValue(
		"Graphics", CSTR(GFX_PROGRESS), BOOL2OPTIONCSTR(_options[Enums::Config::Options::DISPLAY_PROGRESS]));
	_settings->SetValue("Gameplay", CSTR(GAME_ALLOW_MIXED_ALIGNMENT_PARTY),
		BOOL2OPTIONCSTR(_options[Enums::Config::Options::ALLOW_MIXED_ALIGNMENT_PARTY]));
	_settings->SetValue(
		"Gameplay", CSTR(GAME_STAT_LOSS_ON_LEVEL_UP), BOOL2OPTIONCSTR(_options[Enums::Config::Options::DICE_ROLLS]));
	_settings->SetValue("Gameplay", CSTR(GAME_REROLL_HIT_POINTS_ON_LEVEL_GAIN),
		BOOL2OPTIONCSTR(_options[Enums::Config::Options::REROLL_HIT_POINTS_ON_LEVEL_GAIN]));
	_settings->SetValue("Gameplay", CSTR(GAME_STAT_RESET_ON_CLASS_CHANGE),
		BOOL2OPTIONCSTR(_options[Enums::Config::Options::STAT_RESET_ON_CLASS_CHANGE]));
	_settings->SetValue("Gameplay", CSTR(GAME_AGING_ON_CLASS_CHANGE),
		BOOL2OPTIONCSTR(_options[Enums::Config::Options::AGING_ON_CLASS_CHANGE]));
	_settings->SetValue(
		"Gameplay", CSTR(GAME_PROTECT_TELEPORT), BOOL2OPTIONCSTR(_options[Enums::Config::Options::PROTECT_TELEPORT]));
	_settings->SetValue("Gameplay", CSTR(GAME_ALLOW_RANGED_WEAPONS),
		BOOL2OPTIONCSTR(_options[Enums::Config::Options::ALLOW_RANGED_WEAPONS]));
	_settings->SetValue("Gameplay", CSTR(GAME_SPELLCASTING_IN_SURPRISE_ROUND),
		BOOL2OPTIONCSTR(_options[Enums::Config::Options::SPELLCASTING_IN_SURPRISE_ROUND]));
	_settings->SetValue("Gameplay", CSTR(GAME_BATCH_HEALING_AFTER_RETURN_TO_CASTLE),
		BOOL2OPTIONCSTR(_options[Enums::Config::Options::BATCH_HEALING_AFTER_RETURN_TO_CASTLE]));
	_settings->SetValue("Gameplay", CSTR(GAME_REROLL_ONES_ON_DICE),
		BOOL2OPTIONCSTR(_options[Enums::Config::Options::REROLL_ONES_ON_DICE]));

	_settings->SetValue("Gameplay", CSTR(GAME_ALLOW_LOST_LEGATED),
		BOOL2OPTIONCSTR(_options[Enums::Config::Options::ALLOW_LOST_LEGATED]));
	_settings->SetValue("Gameplay", CSTR(GAME_ALLOW_CURABLE_DRAIN),
		BOOL2OPTIONCSTR(_options[Enums::Config::Options::ALLOW_CURABLE_DRAIN]));
	_settings->SetValue("Gameplay", CSTR(GAME_ALLOW_SHARED_INVENTORY),
		BOOL2OPTIONCSTR(_options[Enums::Config::Options::ALLOW_SHARED_INVENTORY]));
	_settings->SetValue("Gameplay", CSTR(GAME_CAMPING_RESTORE_SPELL_POINTS),
		BOOL2OPTIONCSTR(_options[Enums::Config::Options::CAMPING_RESTORE_SPELL_POINTS]));
	_settings->SetValue(
		"Gameplay", CSTR(GAME_CHARGED_ITEMS), BOOL2OPTIONCSTR(_options[Enums::Config::Options::CHARGED_ITEMS]));

	SI_Error result{_settings->SaveFile(CSTR(_config_fp))};
	return (result >= 0);
}

// Preset Options
auto Sorcery::ConfigFile::set_rec_mode() -> void {

	std::array<bool, NUM_GAME_SETTINGS> _rec_settings{true, false, true, false, true, false, true, true, true, true,
		true, true, false, false, false, false, true, true, true, true, true, true, true, false, false, true};
	std::swap(_options, _rec_settings);
}

// Preset Options
auto Sorcery::ConfigFile::set_strict_mode() -> void {

	std::array<bool, NUM_GAME_SETTINGS> _strict_settings{false, true, true, false, false, true, false, false, false,
		false, false, false, true, true, true, true, false, false, false, false, false, false, false, false, false};
	std::swap(_options, _strict_settings);
}

// Store to enable comparison for checking if anything has changed on cancel
auto Sorcery::ConfigFile::store() -> void {

	_options_backup = _options;
}

// Compare to Strict Mode
auto Sorcery::ConfigFile::is_strict_mode() -> bool {

	std::array<bool, NUM_GAME_SETTINGS> _strict_settings{false, true, true, false, false, true, false, false, false,
		false, false, false, true, true, true, true, false, false, false, false, false, false, false, false, false,
		false};
	return _options == _strict_settings;
}

// Compare to Recommended Mode
auto Sorcery::ConfigFile::is_rec_mode() -> bool {

	std::array<bool, NUM_GAME_SETTINGS> _rec_settings{true, false, true, false, true, false, true, true, true, true,
		true, true, false, false, false, false, true, true, true, true, true, true, true, false, false, true};
	return _options == _rec_settings;
}
