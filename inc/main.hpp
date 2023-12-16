// Copyright (C) 2023 Dave Moore
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
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <TGUI/TGUI.hpp>
#include <Thor/Graphics.hpp>
#include <Thor/Resources.hpp>
#include <jsoncpp/json/json.h>

// External Includes
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch-default"
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#pragma GCC diagnostic ignored "-Wvolatile" // NOLINT(clang-diagnostic-unknown-warning-option)
#pragma GCC diagnostic ignored "-Wignored-qualifiers"
#pragma GCC diagnostic ignored "-Wreorder"
#pragma GCC diagnostic ignored "-Wreturn-type"
#pragma GCC diagnostic ignored "-Weffc++"
#pragma GCC diagnostic ignored "-Wnon-virtual-dtor"
#pragma GCC diagnostic ignored "-Wshadow"
#include "backwardcpp/backward.hpp"
#include "simpleini/SimpleIni.h"
// clang-format off
#include "cereal/cereal.hpp"
#include "cereal/archives/json.hpp"
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
#define MAGIC_ENUM_RANGE_MIN -1 // https://github.com/Neargye/magic_enum/blob/master/doc/limitations.md
#define MAGIC_ENUM_RANGE_MAX 255
#include "magicenum/magic_enum.hpp"
#include "sfeMovie/Movie.hpp"
#include "sqlitemoderncpp/sqlite_modern_cpp.h"
#pragma GCC diagnostic pop

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
#include "core/platform.hpp"

// clang-format off
#include "define.hpp"
#include "enum.hpp"
#include "macro.hpp"
#include "types/type.hpp"
#include "types/component.hpp"
#include "types/error.hpp"

// clang-format on

// Aliases
namespace Sorcery {

using Alignment = Enums::Window::Alignment;
using AutoMapFeature = Enums::Automap::Feature;
using CharacterAbility = Enums::Character::Ability;
using CharacterAbilityType = Enums::Character::Ability_Type;
using CharacterAbilities = std::map<Enums::Character::Ability, int>;
using CharacterAlignment = Enums::Character::Align;
using CharacterAttribute = Enums::Character::Attribute;
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
using CombatType = Enums::Combat::Type;
using ComponentData = std::pair<std::string, std::string>;
using ComponentElement = Enums::Window::ComponentElement;
using ComponentType = Enums::Window::ComponentType;
using ConfigOption = Enums::Options;
using CreateMethod = Enums::Manage::Method;
using Destination = Enums::System::Destination;
using DoorType = Enums::Tile::DoorType;
using EventGraphic = Enums::Graphics::Event;
using FontType = Enums::Internal::FontType;
using GraphicsTexture = Enums::Graphics::Texture;
using GraphicsTextureType = Enums::Graphics::TextureType;
using IconLibrary = std::map<std::string, sf::Sprite>;
using IconStorage = std::vector<std::pair<std::string, sf::Sprite>>;
using InnStage = Enums::Castle::Inn;
using ItemCategory = Enums::Items::Category;
using ItemDef = Enums::Items::Effects::Defensive;
using ItemOff = Enums::Items::Effects::Offensive;
using ItemEffDef = std::array<bool, 22>;
using ItemEffOff = std::array<bool, 15>;
using ItemInv = Enums::Items::Effects::Invoke;
using ItemLocation = Enums::Items::Location;
using ItemQuest = Enums::Items::Quest;
using ItemTypeID = Enums::Items::TypeID;
using ItemUsableClass = std::array<bool, 9>;
using ItemUsableAlignment = std::array<bool, 4>;
using ItemView = Enums::Items::View;
using Justification = Enums::Window::Justification;
using LegateStage = Enums::Character::Legate;
using LevelID = std::pair<std::string, int>;
using MainMenuType = Enums::MainMenu::Type;
using MapDirection = Enums::Map::Direction;
using MapEvent = Enums::Map::Event;
using MapType = Enums::Map::Type;
using MenuItem = Enums::Menu::Item;
using MenuMode = Enums::Menu::Mode;
using MenuType = Enums::Menu::Type;
using MenuItemType = Enums::Menu::ItemType;
using MessagePosition = Enums::View::Message::Position;
using MessageType = Enums::Internal::MessageType;
using Method = Enums::Manage::Method;
using ModuleResult = Enums::System::ModuleResult;
using RandomType = Enums::System::Random;
using Range = std::tuple<unsigned int, unsigned int>;
using RestMode = Enums::Castle::Rest::Mode;
using RestStage = Enums::Castle::Rest::Stage;
using RestType = Enums::Castle::Rest::Type;
using RosterMode = Enums::Manage::Roster;
using ShopStage = Enums::Castle::Shop;
using SpellID = Enums::Magic::SpellID;
using SpellCategory = Enums::Magic::SpellCategory;
using SpellPoints = std::map<unsigned int, unsigned int>;
using SpellType = Enums::Magic::SpellType;
using SpellPointStatus = Enums::Magic::SpellPointStatus;
using Stage = Enums::Character::Stage;
using StringMap = std::map<std::string, std::string>;
using SystemError = Enums::System::Error;
using TavernStage = Enums::Castle::Tavern;
using TempleStage = Enums::Castle::Temple;
using TileFeature = Enums::Tile::Features;
using TileProperty = Enums::Tile::Properties;
using TileEdge = Enums::Tile::Edge;
using WindowInputMode = Enums::Window::InputMode;
using WindowInputCategory = Enums::Window::InputCategory;
using WindowInput = Enums::Controls::Input;
using WindowConfirm = Enums::Window::Confirm;
using WindowDialogButton = Enums::Window::DialogButton;
using WindowDialogType = Enums::Window::DialogType;
using WindowDrawMode = Enums::Window::DrawMode;
using WindowInputOption = Enums::Window::Option;
using WindowFrameParts = Enums::Window::FrameParts;

}
