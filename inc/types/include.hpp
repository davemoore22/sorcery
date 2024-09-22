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

#pragma once

// Library Includes
// IWYU pragma: begin_keep
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch-default"
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#ifndef __clang__
#pragma GCC diagnostic ignored "-Wvolatile" // NOLINT(clang-diagnostic-unknown-warning-option)
#endif
#pragma GCC diagnostic ignored "-Wignored-qualifiers"
#pragma GCC diagnostic ignored "-Wreorder"
#pragma GCC diagnostic ignored "-Wreturn-type"
#pragma GCC diagnostic ignored "-Weffc++"
#pragma GCC diagnostic ignored "-Wnon-virtual-dtor"
#pragma GCC diagnostic ignored "-Wshadow"
#ifdef __clang__
#pragma GCC diagnostic ignored "-Wunqualified-std-cast-call"
#endif
#include "backwardcpp/backward.hpp"
#include "simpleini/SimpleIni.h"
#include <TGUI/TGUI.hpp>
#include <jsoncpp/json/json.h>
#pragma GCC diagnostic pop

// IWYU pragma: end_keep

// Aliases for Enums and Types
namespace Sorcery {

using CFG = Enums::Config::Options;
using ITC = Enums::Items::Category;
using ITD = Enums::Items::Effects::Defensive;
using ITO = Enums::Items::Effects::Offensive;
using ITV = Enums::Items::Effects::Invoke;
using ITT = Enums::Items::TypeID;
using IIR = Enums::Items::IdentifyOutcome;
using MBR = Enums::Monsters::Breath;
using MCL = Enums::Monsters::Class;
using MCT = Enums::Monsters::Category;
using MRE = Enums::Monsters::Resistance;
using MPR = Enums::Monsters::Property;
using MTI = Enums::Monsters::TypeID;

using MonsterResistances = std::array<bool, 7>;
using MonsterProperties = std::array<bool, 7>;
using TimePoint = std::chrono::time_point<std::chrono::system_clock>;

}