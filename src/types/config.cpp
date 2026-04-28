// Copyright (C) 2026 Dave Moore
//
// This file is part of Sorcery.
//
// Sorcery is free software: you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software
// Foundation, either version 2 of the License, or (at your option) any later
// version.
//
// Sorcery is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with
// Sorcery.  If not, see <http://www.gnu.org/licenses/>.
//
// If you modify this program, or any covered work, by linking or combining
// it with the libraries referred to in README (or a modified version of
// said libraries), containing parts covered by the terms of said libraries,
// the licensors of this program grant you additional permission to convey
// the resulting work.

#include "types/config.hpp"
#include "core/macro.hpp"
#include "types/define.hpp"
#include "types/enum.hpp"

Sorcery::Config::Config(CSimpleIniA *settings,
						const std::filesystem::path cfg_path)
	: _settings{settings},
	  _cfg_path{cfg_path} {

	_load();
}

auto Sorcery::Config::get(const unsigned int i) -> bool & {

	return _options.at(i);
}

auto Sorcery::Config::get(std::string_view section,
						  std::string_view value) const -> std::string {

	// Get a value from the config file
	return _settings->GetValue(CSTR(std::string{section}),
							   CSTR(std::string{value}));
}

bool Sorcery::Config::has_changed() {

	// Check if the options have changed
	return _options == _options_bkp;
}

auto Sorcery::Config::load() -> bool {

	return _load();
}

auto Sorcery::Config::_load() -> bool {

	// Attempt to read the settings from the Settings file if possible
	_options.fill(false);

	// Gameplay Settings
	auto option{""s};
	const char *on{"on"};
	const char *off{"on"};

	using enum Enums::Config::Options;
	option = _settings->GetValue("Options", CSTR(OPT_RECOMMENDED_MODE), off);
	_options[RECOMMENDED_MODE] = option.compare(on) == 0;
	option = _settings->GetValue("Options", CSTR(OPT_STRICT_MODE), off);
	_options[STRICT_MODE] = option.compare(on) == 0;
	option = _settings->GetValue("Options", CSTR(OPT_CHEAT_MODE), off);
	_options[CHEAT_MODE] = option.compare(on) == 0;
	option = _settings->GetValue("Options", CSTR(OPT_AUTO_SAVE), off);
	_options[AUTO_SAVE] = option.compare(on) == 0;
	option = _settings->GetValue("Options", CSTR(OPT_DICE_ROLLS), off);
	_options[DICE_ROLLS] = option.compare(on) == 0;
	option = _settings->GetValue("Gameplay", CSTR(OPT_MIXED_ALIGNMENT), off);
	_options[MIXED_ALIGNMENT] = option.compare(on) == 0;
	option = _settings->GetValue("Gameplay", CSTR(OPT_LEVEL_STAT_LOSS), off);
	_options[LEVEL_STAT_LOSS] = option.compare(on) == 0;
	option = _settings->GetValue("Gameplay", CSTR(OPT_LEVEL_REROLL_HP), off);
	_options[LEVEL_REROLL_HP] = option.compare(on) == 0;
	option = _settings->GetValue("Gameplay", CSTR(OPT_CLASS_CHANGE_RESET), off);
	_options[CLASS_CHANGE_RESET] = option.compare(on) == 0;
	option = _settings->GetValue("Gameplay", CSTR(OPT_CLASS_CHANGE_AGING), off);
	_options[CLASS_CHANGE_AGING] = option.compare(on) == 0;
	option = _settings->GetValue("Gameplay", CSTR(OPT_AMBUSH_HIDE), off);
	_options[AMBUSH_HIDE] = option.compare(on) == 0;
	option =
		_settings->GetValue("Gameplay", CSTR(OPT_SURPRISE_SPELLCASTING), off);
	_options[SURPRISE_SPELLCASTING] = option.compare(on) == 0;
	option = _settings->GetValue("Gameplay", CSTR(OPT_INN_HEALING), off);
	_options[INN_HEALING] = option.compare(on) == 0;
	option = _settings->GetValue("Gameplay", CSTR(OPT_REROLL_ONES), off);
	_options[REROLL_ONES] = option.compare(on) == 0;
	option = _settings->GetValue("Gameplay", CSTR(OPT_LOST_LEGATION), off);
	_options[LOST_LEGATION] = option.compare(on) == 0;
	option = _settings->GetValue("Gameplay", CSTR(OPT_CURABLE_DRAINING), off);
	_options[CURABLE_DRAIN] = option.compare(on) == 0;
	option = _settings->GetValue("Gameplay", CSTR(OPT_SHARED_INVENTORY), off);
	_options[SHARED_INVENTORY] = option.compare(on) == 0;
	option = _settings->GetValue("Gameplay", CSTR(OPT_PROTECT_TELEPORT), off);
	_options[PROTECT_TELEPORT] = option.compare(on) == 0;
	option = _settings->GetValue("Graphics", CSTR(OPT_COLOURED_WIREFRAME), off);
	_options[COLOURED_WIREFRAME] = option.compare(on) == 0;
	option = _settings->GetValue("Graphics", CSTR(OPT_FULLSCREEN), off);
	_options[FULLSCREEN] = option.compare(on) == 0;

	return true;
}

bool Sorcery::Config::save() {

	using enum Enums::Config::Options;
	_settings->SetValue("Options", CSTR(OPT_RECOMMENDED_MODE),
						BOOL2OPTIONCSTR(_options[RECOMMENDED_MODE]));
	_settings->SetValue("Options", CSTR(OPT_STRICT_MODE),
						BOOL2OPTIONCSTR(_options[STRICT_MODE]));
	_settings->SetValue("Options", CSTR(OPT_CHEAT_MODE),
						BOOL2OPTIONCSTR(_options[CHEAT_MODE]));
	_settings->SetValue("Options", CSTR(OPT_AUTO_SAVE),
						BOOL2OPTIONCSTR(_options[AUTO_SAVE]));
	_settings->SetValue("Options", CSTR(OPT_DICE_ROLLS),
						BOOL2OPTIONCSTR(_options[DICE_ROLLS]));
	_settings->SetValue("Gameplay", CSTR(OPT_MIXED_ALIGNMENT),
						BOOL2OPTIONCSTR(_options[MIXED_ALIGNMENT]));
	_settings->SetValue("Gameplay", CSTR(OPT_LEVEL_STAT_LOSS),
						BOOL2OPTIONCSTR(_options[LEVEL_STAT_LOSS]));
	_settings->SetValue("Gameplay", CSTR(OPT_LEVEL_REROLL_HP),
						BOOL2OPTIONCSTR(_options[LEVEL_REROLL_HP]));
	_settings->SetValue("Gameplay", CSTR(OPT_CLASS_CHANGE_RESET),
						BOOL2OPTIONCSTR(_options[CLASS_CHANGE_RESET]));
	_settings->SetValue("Gameplay", CSTR(OPT_CLASS_CHANGE_AGING),
						BOOL2OPTIONCSTR(_options[CLASS_CHANGE_AGING]));
	_settings->SetValue("Gameplay", CSTR(OPT_AMBUSH_HIDE),
						BOOL2OPTIONCSTR(_options[AMBUSH_HIDE]));
	_settings->SetValue("Gameplay", CSTR(OPT_SURPRISE_SPELLCASTING),
						BOOL2OPTIONCSTR(_options[SURPRISE_SPELLCASTING]));
	_settings->SetValue("Gameplay", CSTR(OPT_INN_HEALING),
						BOOL2OPTIONCSTR(_options[INN_HEALING]));
	_settings->SetValue("Gameplay", CSTR(OPT_REROLL_ONES),
						BOOL2OPTIONCSTR(_options[REROLL_ONES]));
	_settings->SetValue("Gameplay", CSTR(OPT_LOST_LEGATION),
						BOOL2OPTIONCSTR(_options[LOST_LEGATION]));
	_settings->SetValue("Gameplay", CSTR(OPT_CURABLE_DRAINING),
						BOOL2OPTIONCSTR(_options[CURABLE_DRAIN]));
	_settings->SetValue("Gameplay", CSTR(OPT_SHARED_INVENTORY),
						BOOL2OPTIONCSTR(_options[SHARED_INVENTORY]));
	_settings->SetValue("Gameplay", CSTR(OPT_PROTECT_TELEPORT),
						BOOL2OPTIONCSTR(_options[PROTECT_TELEPORT]));
	_settings->SetValue("Graphics", CSTR(OPT_COLOURED_WIREFRAME),
						BOOL2OPTIONCSTR(_options[COLOURED_WIREFRAME]));
	_settings->SetValue("Graphics", CSTR(OPT_FULLSCREEN),
						BOOL2OPTIONCSTR(_options[FULLSCREEN]));

	// Save current settings to ini file
	SI_Error result{_settings->SaveFile(CSTR(_cfg_path))};
	return (result >= 0);
}

auto Sorcery::Config::store() -> void {

	// Store to enable comparison for checking if anything has changed on cancel
	_options_bkp = _options;
}

auto Sorcery::Config::set_rec_mode() -> void {

	std::array<bool, NUM_GAME_SETTINGS> rec{
		true, false, false, false, true, true, false, false, false, false,
		true, false, true,	true,  true, true, true,  true,	 true};
	std::swap(_options, rec);
}

auto Sorcery::Config::set_strict_mode() -> void {

	std::array<bool, NUM_GAME_SETTINGS> strict{
		false, true,  false, true,	false, false, true,	 true,	true, true,
		false, false, false, false, false, false, false, false, false};
	std::swap(_options, strict);
}

auto Sorcery::Config::is_strict_mode() const -> bool {

	std::array<bool, NUM_GAME_SETTINGS> strict{
		false, true,  false, true,	false, true,  true,	 true,	true, true,
		false, false, false, false, false, false, false, false, false};

	return _options == strict;
}

auto Sorcery::Config::is_rec_mode() const -> bool {

	std::array<bool, NUM_GAME_SETTINGS> rec{
		true, false, false, false, true, true, false, false, false, false,
		true, false, true,	true,  true, true, true,  true,	 true};

	return _options == rec;
}
