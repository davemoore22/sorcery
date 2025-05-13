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

#pragma once

// Library Includes
// clang-format off
#include "cereal/cereal.hpp"
#include "cereal/archives/xml.hpp"
#include "cereal/types/array.hpp"
#include "cereal/types/bitset.hpp"
#include "cereal/types/chrono.hpp"
#include "cereal/types/map.hpp"
#include "cereal/types/memory.hpp"
#include "cereal/types/optional.hpp"
#include "cereal/types/string.hpp"
#include "cereal/types/unordered_map.hpp"
#include "cereal/types/vector.hpp"
// clang-format on
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#include "sqlite3.h"
#include "sqlite_modern_cpp/sqlite_modern_cpp.h"
#pragma GCC diagnostic pop

// Standard Includes
#include "common/include.hpp"