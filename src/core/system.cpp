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

#include "core/system.hpp"
#include "common/define.hpp"
#include "common/enum.hpp"
#include "core/macro.hpp"

// Standard Constructor
Sorcery::System::System(int argc __attribute__((unused)), char **argv __attribute__((unused))) {

	// Files Module
	files = std::make_unique<FileStore>();

	// Settings File/Config Module
	settings = std::make_unique<CSimpleIniA>();
	settings->SetUnicode();
	const auto settings_fp{(*files)[CONFIG_FILE]};
	settings->LoadFile(CSTR(settings_fp));
	config = std::make_unique<ConfigFile>(settings.get(), (*files)[CONFIG_FILE]);

	// Random Module
	random = std::make_unique<Random>();

	// Game Database
	database = std::make_unique<Database>((*files)[DATABASE_FILE]);

	// Resource Manager
	resources = std::make_unique<ResourceManager>(*files);
	resources->textures[Enums::Graphics::Texture::BACKGROUND].setRepeated(true);

	// Input Module
	input = std::make_unique<Input>();

	// Pause Clock
	_pause_clock_start = std::nullopt;
	_clock_duration = std::nullopt;
}

auto Sorcery::System::set_pause(unsigned int milliseconds) -> void {

	if (!_pause_clock_start) {
		_clock_duration = milliseconds;
		_pause_clock_start = std::chrono::steady_clock::now();
	}
}

auto Sorcery::System::get_pause() -> bool {

	if (_pause_clock_start && _clock_duration) {
		auto elapsed{std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::steady_clock::now() - _pause_clock_start.value())
				.count()};

		return (elapsed < _clock_duration.value());
	} else
		return false;
}

// Only returns true when the tiner has expired
auto Sorcery::System::update_pause() -> bool {

	if (_pause_clock_start && _clock_duration) {
		auto elapsed{std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::steady_clock::now() - _pause_clock_start.value())
				.count()};
		if (elapsed > _clock_duration.value()) {
			_pause_clock_start = std::nullopt;
			_clock_duration = std::nullopt;
			return true;
		} else
			return false;
	} else
		return false;
}

auto Sorcery::System::stop_pause() -> void {

	_pause_clock_start = std::nullopt;
	_clock_duration = std::nullopt;
}

// Diceroll to String
auto Sorcery::System::dice_roll_to_str(
	const std::string &message, const int dice, const int roll, const int needed) const -> std::string {

	return fmt::format("d{:<3}: {:>3}/{:>3}: {}", dice, roll, needed, message);
}

// Timepoint to String
auto Sorcery::System::convert_tp_to_str(const TimePoint tp) const -> std::string {

	// Need to do it this way til std::chrono::locate_zone etc is supported
	auto t_t{std::chrono::system_clock::to_time_t(tp)};
	char t_s[std::size("yyyy-mm-dd hh:mm:ss")];
	std::strftime(std::data(t_s), std::size(t_s), "%F %T", std::gmtime(&t_t));
	std::string ts(t_s);

	return ts;
}
