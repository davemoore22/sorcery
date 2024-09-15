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
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

// https://github.com/Neargye/magic_enum/blob/master/doc/limitations.md
#define MAGIC_ENUM_RANGE_MIN -1
#define MAGIC_ENUM_RANGE_MAX 255
#include "magicenum/magic_enum.hpp"

// IWYU pragma: end_keep

// Standard Includes
#include <algorithm>
#include <any>
#include <array>
#include <atomic>
#include <bitset>
#include <chrono>
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
#include <vector>

// Internal Includes
#include "common/enum.hpp"
#include "common/type.hpp"
#include "types/component.hpp"
#include "types/dice.hpp"
#include "types/error.hpp"

// Aliases
namespace Sorcery {

using CAB = Enums::Character::Ability;
using CAT = Enums::Character::Ability_Type;
using CharacterAbilities = std::map<Enums::Character::Ability, int>;
using CAL = Enums::Character::Align;
using CAR = Enums::Character::Attribute;
using CharacterAttributes = std::map<Enums::Character::Attribute, int>;
using CharacterClass = Enums::Character::Class;
using CharacterClassList = std::map<Enums::Character::Class, std::string>;
using CharacterClassMenu = std::pair<Enums::Character::Class, Enums::Menu::Item>;
using CharacterClassQualified = std::map<Enums::Character::Class, bool>;
using CharacterMode = Enums::Character::Mode;
using CharacterRace = Enums::Character::Race;
using CharacterStage = Enums::Character::Stage;
using CharacterStatus = Enums::Character::CStatus;
using CharacterView = Enums::Character::View;
using CharacterLocation = Enums::Character::Location;
using ComponentData = std::pair<std::string, std::string>;
using ComponentElement = Enums::Window::ComponentElement;
using ComponentType = Enums::Window::ComponentType;
using CIN = Enums::Controls::Input;
using CreateMethod = Enums::Manage::Method;
using Destination = Enums::System::Destination;
using FontType = Enums::Internal::FontType;
using GraphicsTexture = Enums::Graphics::Texture;
using GraphicsTextureType = Enums::Graphics::TextureType;
using IconLibrary = std::map<std::string, sf::Sprite>;
using IconStorage = std::vector<std::pair<std::string, sf::Sprite>>;
using ItemEffDef = std::array<bool, 22>;
using ItemEffOff = std::array<bool, 15>;
using ItemUsableClass = std::array<bool, 9>;
using ItemUsableAlignment = std::array<bool, 4>;
using Justification = Enums::Window::Justification;
using LegateStage = Enums::Character::Legate;
using LevelID = std::pair<std::string, int>;
using MMT = Enums::MainMenu::Type;
using MapDirection = Enums::Map::Direction;
using MapEvent = Enums::Map::Event;
using MapType = Enums::Map::Type;
using MIM = Enums::Menu::Item;
using MMD = Enums::Menu::Mode;
using MTP = Enums::Menu::Type;
using MIT = Enums::Menu::ItemType;
using MessageType = Enums::Internal::MessageType;
using MDR = Enums::System::MDR;
using RandomType = Enums::System::Random;
using Range = std::tuple<unsigned int, unsigned int>;
using SpellID = Enums::Magic::SpellID;
using SpellCategory = Enums::Magic::SpellCategory;
using SpellPoints = std::map<unsigned int, unsigned int>;
using SpellType = Enums::Magic::SpellType;
using SpellPointStatus = Enums::Magic::SpellPointStatus;
using Stage = Enums::Character::Stage;
using StringMap = std::map<std::string, std::string>;
using SystemError = Enums::System::Error;
using TileFeature = Enums::Tile::Features;
using TileProperty = Enums::Tile::Properties;
using TileEdge = Enums::Tile::Edge;
using WIM = Enums::Window::InputMode;
using WDT = Enums::Window::DialogType;
using WDM = Enums::Window::DrawMode;
using WDB = Enums::Window::DialogButton;

}
