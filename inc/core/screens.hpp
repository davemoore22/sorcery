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

#include "common/include.hpp"

#pragma once

namespace Sorcery::Screens {

inline constexpr std::string_view MAINMENU = "show_main_menu";
inline constexpr std::string_view OPTIONS = "show_options";
inline constexpr std::string_view LICENSE = "show_license";

inline constexpr std::string_view COMPENDIUM = "show_compendium";
inline constexpr std::string_view ATLAS = "show_atlas";
inline constexpr std::string_view BESTIARY = "show_bestiary";
inline constexpr std::string_view MUSEUM = "show_museum";
inline constexpr std::string_view SPELLBOOK = "show_spellbook";

inline constexpr std::string_view CASTLE = "show_castle";
inline constexpr std::string_view EDGEOFTOWN = "show_edge_of_town";
inline constexpr std::string_view TAVERN = "show_tavern";
inline constexpr std::string_view INN = "show_inn";
inline constexpr std::string_view TEMPLE = "show_temple";
inline constexpr std::string_view SHOP = "show_shop";
inline constexpr std::string_view TRAINING = "show_training";
inline constexpr std::string_view RESTART = "show_restart";
inline constexpr std::string_view ADD = "show_add";
inline constexpr std::string_view REMOVE = "show_remove";

inline constexpr std::string_view ENGINE = "show_engine";
inline constexpr std::string_view INSPECT = "show_inspect";
inline constexpr std::string_view REORDER = "show_reorder";

inline constexpr std::string_view STAY = "show_stay";
inline constexpr std::string_view RECOVERY = "show_recovery";
inline constexpr std::string_view LEVELUP = "show_levelup";
inline constexpr std::string_view NOLEVELUP = "show_nolevelup";

inline constexpr std::string_view PAY = "show_pay";
inline constexpr std::string_view HEAL = "show_heal";
inline constexpr std::string_view RESULTS = "show_results";

inline constexpr std::string_view CREATE = "show_create";
inline constexpr std::string_view METHOD = "show_method";
inline constexpr std::string_view ROSTER = "show_roster";

} // namespace Sorcery::ScreenID
