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
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch-default"
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#pragma GCC diagnostic ignored                                                 \
	"-Wvolatile" // NOLINT(clang-diagnostic-unknown-warning-option)
#pragma GCC diagnostic ignored "-Wignored-qualifiers"
#pragma GCC diagnostic ignored "-Wsuggest-attribute=format"
#pragma GCC diagnostic ignored "-Wreorder"
#pragma GCC diagnostic ignored "-Wreturn-type"
#pragma GCC diagnostic ignored "-Weffc++"
#pragma GCC diagnostic ignored "-Wnon-virtual-dtor"
#pragma GCC diagnostic ignored "-Wshadow"
// clang-format off
#include <GL/glew.h>
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"

#include "imgui.h"
#include "imgui_internal.h"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_opengl3.h"
#include "misc/cpp/imgui_stdlib.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_pixels.h>
#include <SDL_opengl.h>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/dict.h>
}

// clang-format on
#pragma GCC diagnostic pop

// https://github.com/Neargye/magic_enum/blob/master/doc/limitations.md
#define MAGIC_ENUM_RANGE_MIN -1
#define MAGIC_ENUM_RANGE_MAX 255
#include "magic_enum/magic_enum.hpp"
#define BACKWARD_HAS_BFD 1
#include "backward_cpp/backward.hpp"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "=Wimplicit-fallthrough"
#define IMSPINNER_DEMO
#include "imgui_spinner/imspinner.h"
#include "imgui_sugar/imgui_sugar.hpp"
#include "imgui_toggle/imgui_toggle.h"
#pragma GCC diagnostic pop

// Standard Includes
#include <algorithm>
#include <any>
#include <array>
#include <atomic>
#include <bitset>
#include <chrono>
#include <codecvt>
#include <cstdarg>
#include <cstdio>
#include <ctime>
#include <exception>
#include <filesystem>
#include <fmt/core.h> // #include <format> when supported
#include <fstream>
#include <iomanip>
#include <ios>
#include <iostream>
#include <iterator>
#include <locale>
#include <memory>
#include <mutex>
#include <numbers>
#include <random>
#include <ranges>
#include <regex>
#include <span>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <thread>
#include <tuple>
#include <typeinfo>
#include <utility>
#include <uuid/uuid.h>
#include <variant>
#include <vector>

// Internal Includes
#include "common/enum.hpp"
#include "types/error.hpp"