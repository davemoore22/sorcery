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

#pragma once

#include "resources/define.hpp"
#include <string_view>

namespace Sorcery {

namespace Enums::Audio {

	enum class Track {
		NONE,
		MAIN_MENU,
		TOWN,
		ENGINE,
		COMBAT,
		COMPENDIUM,
		VICTORY,
		GRAVEYARD
	};

	enum class State {
		STOPPED,
		PLAYING,
		FADING_IN,
		FADING_OUT
	};

} // namespace Sorcery::Enums::Audio
}

namespace Sorcery::Music {

[[nodiscard]]
constexpr auto resource(const Enums::Audio::Track track) -> std::string_view {

	using enum Enums::Audio::Track;

	switch (track) {
	case MAIN_MENU:
		return MAINMENU_MUSIC;

	case TOWN:
		return TOWN_MUSIC;

	case ENGINE:
		return ENGINE_MUSIC;

	case COMBAT:
		return COMBAT_MUSIC;

	case COMPENDIUM:
		return COMPENDIUM_MUSIC;

	case VICTORY:
		return VICTORY_MUSIC;

	case GRAVEYARD:
		return GRAVEYARD_MUSIC;

	case NONE:
		return {};
	}

	return {};
}

} // namespace Sorcery::Music
