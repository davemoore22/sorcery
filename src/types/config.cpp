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
#include "common/enum.hpp"
#include "types/define.hpp"

#include <array>
#include <initializer_list>
#include <string>
#include <string_view>
#include <utility>

namespace Sorcery {

using Option = Enums::Config::Options;

struct ConfigOption {
		Option option;
		std::string_view section;
		std::string_view key;
};

constexpr std::array CONFIG_OPTIONS{
	ConfigOption{Option::RECOMMENDED_MODE, "Options", OPT_RECOMMENDED_MODE},
	ConfigOption{Option::STRICT_MODE, "Options", OPT_STRICT_MODE},
	ConfigOption{Option::CHEAT_MODE, "Options", OPT_CHEAT_MODE},
	ConfigOption{Option::AUTO_SAVE, "Options", OPT_AUTO_SAVE},
	ConfigOption{Option::DICE_ROLLS, "Options", OPT_DICE_ROLLS},

	ConfigOption{Option::MIXED_ALIGNMENT, "Gameplay", OPT_MIXED_ALIGNMENT},
	ConfigOption{Option::LEVEL_STAT_LOSS, "Gameplay", OPT_LEVEL_STAT_LOSS},
	ConfigOption{Option::LEVEL_REROLL_HP, "Gameplay", OPT_LEVEL_REROLL_HP},
	ConfigOption{Option::CLASS_CHANGE_RESET, "Gameplay", OPT_CLASS_CHANGE_RESET},
	ConfigOption{Option::CLASS_CHANGE_AGING, "Gameplay", OPT_CLASS_CHANGE_AGING},
	ConfigOption{Option::AMBUSH_HIDE, "Gameplay", OPT_AMBUSH_HIDE},
	ConfigOption{Option::SURPRISE_SPELLCASTING, "Gameplay", OPT_SURPRISE_SPELLCASTING},
	ConfigOption{Option::INN_HEALING, "Gameplay", OPT_INN_HEALING},
	ConfigOption{Option::REROLL_ONES, "Gameplay", OPT_REROLL_ONES},
	ConfigOption{Option::LOST_LEGATION, "Gameplay", OPT_LOST_LEGATION},
	ConfigOption{Option::CURABLE_DRAIN, "Gameplay", OPT_CURABLE_DRAINING},
	ConfigOption{Option::SHARED_INVENTORY, "Gameplay", OPT_SHARED_INVENTORY},
	ConfigOption{Option::PROTECT_TELEPORT, "Gameplay", OPT_PROTECT_TELEPORT},

	ConfigOption{Option::CGA_GRAPHICS, "Graphics", OPT_CGA_GRAPHICS},
	ConfigOption{Option::FULLSCREEN, "Graphics", OPT_FULLSCREEN},

	ConfigOption{Option::SOUND, "Media", OPT_SOUND},
	ConfigOption{Option::MUSIC, "Media", OPT_MUSIC},
};

/// @brief
/// @param enabled
/// @return
constexpr auto make_options(const std::initializer_list<Option> enabled) -> std::array<bool, NUM_GAME_SETTINGS> {

	std::array<bool, NUM_GAME_SETTINGS> options{};

	for (const auto option : enabled)
		options[std::to_underlying(option)] = true;

	return options;
}

constexpr auto RECOMMENDED_OPTIONS{make_options({
	Option::RECOMMENDED_MODE,
	Option::DICE_ROLLS,
	Option::MIXED_ALIGNMENT,
	Option::AMBUSH_HIDE,
	Option::INN_HEALING,
	Option::REROLL_ONES,
	Option::LOST_LEGATION,
	Option::CURABLE_DRAIN,
	Option::SHARED_INVENTORY,
	Option::PROTECT_TELEPORT,
	Option::CGA_GRAPHICS,
	Option::FULLSCREEN,
	Option::SOUND,
	Option::MUSIC,
})};

constexpr auto STRICT_OPTIONS{make_options({
	Option::STRICT_MODE,
	Option::AUTO_SAVE,
	Option::LEVEL_STAT_LOSS,
	Option::LEVEL_REROLL_HP,
	Option::CLASS_CHANGE_RESET,
	Option::CLASS_CHANGE_AGING,
})};

} // namespace

/// @brief
/// @param settings
/// @param cfg_path
Sorcery::Config::Config(CSimpleIniA *settings, std::filesystem::path cfg_path)
	: _settings{settings},
	  _cfg_path{std::move(cfg_path)} {

	_load();
}

// TODO: replace array of bools with an ordered std::map or similar so we can get rid of all the std::to_underlying

/// @brief
/// @param option
/// @return
auto Sorcery::Config::get(Enums::Config::Options option) const -> bool {

	return _options.at(std::to_underlying(option));
}

/// @brief
/// @param option
/// @return
auto Sorcery::Config::get(const Enums::Config::Options option) -> bool & {

	return _options.at(std::to_underlying(option));
}

/// @brief
/// @param section
/// @param key
/// @return
auto Sorcery::Config::get(const std::string_view section, const std::string_view key) const -> std::string {

	return _settings->GetValue(section.data(), key.data(), "");
}

/// @brief
/// @return
auto Sorcery::Config::has_changed() const -> bool {

	return _options != _options_bkp;
}

/// @brief
/// @return
auto Sorcery::Config::load() -> bool {

	return _load();
}

/// @brief
/// @return
auto Sorcery::Config::_load() -> bool {

	_options.fill(false);

	for (const auto &[option, section, key] : CONFIG_OPTIONS) {

		const auto value{_settings->GetValue(section.data(), key.data(), "off")};

		_options[std::to_underlying(option)] = std::string_view{value} == "on";
	}

	return true;
}

/// @brief
/// @return
auto Sorcery::Config::save() -> bool {

	for (const auto &[option, section, key] : CONFIG_OPTIONS) {

		_settings->SetValue(section.data(), key.data(), _options[std::to_underlying(option)] ? "on" : "off");
	}

	const auto filename{_cfg_path.string()};
	const auto result{_settings->SaveFile(filename.c_str())};

	return result >= 0;
}

/// @brief
/// @return
auto Sorcery::Config::store() -> void {

	_options_bkp = _options;
}

/// @brief
/// @return
auto Sorcery::Config::set_rec_mode() -> void {

	_options = RECOMMENDED_OPTIONS;
}

/// @brief
/// @return
auto Sorcery::Config::set_strict_mode() -> void {

	_options = STRICT_OPTIONS;
}

/// @brief
/// @return
auto Sorcery::Config::is_strict_mode() const -> bool {

	return _options == STRICT_OPTIONS;
}

/// @brief
/// @return
auto Sorcery::Config::is_rec_mode() const -> bool {

	return _options == RECOMMENDED_OPTIONS;
}