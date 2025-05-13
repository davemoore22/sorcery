// Copyright (C) 2025 Dave Moore
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

#include "core/system.hpp"
#include "common/define.hpp"
#include "common/enum.hpp"
#include "core/macro.hpp"

Sorcery::System::System(int argc __attribute__((unused)),
						char **argv __attribute__((unused))) {

	// Modules
	files = std::make_unique<FileStore>();
	strings = std::make_unique<StringStore>((*files)[STRINGS_FILE]);

	_settings = std::make_unique<CSimpleIniA>();
	_settings->SetUnicode();
	_settings->LoadFile(CSTR((*files)[CONFIG_FILE]));

	config = std::make_unique<Config>(_settings.get(), (*files)[CONFIG_FILE]);
	random = std::make_unique<Random>();
	animation = std::make_unique<Animation>(random.get());
	db = std::make_unique<Database>(CSTR((*files)[DATABASE_FILE]));
}

// Diceroll to String
auto Sorcery::System::dice_roll_to_str(const std::string &message,
									   const int dice, const int roll,
									   const int needed) const -> std::string {

	return fmt::format("d{:<3}: {:>3}/{:>3}: {}", dice, roll, needed, message);
}

// Timepoint to String
auto Sorcery::System::convert_tp_to_str(
	const std::chrono::time_point<std::chrono::system_clock> tp) const
	-> std::string {

	// Need to do it this way til std::chrono::locate_zone etc is supported
	auto t_t{std::chrono::system_clock::to_time_t(tp)};
	char t_s[std::size("yyyy-mm-dd hh:mm:ss")];
	std::strftime(std::data(t_s), std::size(t_s), "%F %T", std::gmtime(&t_t));
	std::string ts(t_s);

	return ts;
}