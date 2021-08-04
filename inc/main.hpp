// Copyright (C) 2021 Dave Moore
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
#pragma GCC diagnostic ignored                                                 \
	"-Wvolatile" // NOLINT(clang-diagnostic-unknown-warning-option)
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
#include "cereal/types/string.hpp"
#include "cereal/types/unordered_map.hpp"
#include "cereal/types/vector.hpp"
// clang-format on
#include "magicenum/magic_enum.hpp"
#include "sfeMovie/Movie.hpp"
#include "sqlitemoderncpp/sqlite_modern_cpp.h"
#pragma GCC diagnostic pop

// Standard Includes
#include <algorithm>
#include <any>
#include <array>
#include <atomic>
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
#include <map>
#include <memory>
#include <mutex>
#include <numbers>
#include <random>
#include <ranges>
#include <regex>
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
#include "platform.hpp"

// clang-format off
#include "define.hpp"
#include "enum.hpp"
#include "macro.hpp"
#include "type.hpp"
#include "component.hpp"
#include "error.hpp"
// clang-format on

// Aliases
namespace Sorcery {

	using Alignment = Enums::Window::Alignment;
	using Background = Enums::Graphics::Background;
	using Category = Enums::Manage::Category;
	using CharacterAbility = Enums::Character::Ability;
	using CharacterAbilityType = Enums::Character::Ability_Type;
	using CharacterAbilities = std::map<Enums::Character::Ability, int>;
	using CharacterAlignment = Enums::Character::Align;
	using CharacterAttribute = Enums::Character::Attribute;
	using CharacterAttributes = std::map<Enums::Character::Attribute, int>;
	using CharacterClass = Enums::Character::Class;
	using CharacterClassList = std::map<Enums::Character::Class, std::string>;
	using CharacterClassMenu =
		std::pair<Enums::Character::Class, Enums::Menu::Item>;
	using CharacterClassQualified = std::map<Enums::Character::Class, bool>;
	using CharacterRace = Enums::Character::Race;
	using CharacterStage = Enums::Character::Stage;
	using CharacterStatus = Enums::Character::CStatus;
	using CharacterView = Enums::Character::View;
	using ComponentData = std::pair<std::string, std::string>;
	using ComponentType = Enums::Window::ComponentType;
	using ConfigOption = Enums::Options;
	using CreateMethod = Enums::Manage::Method;
	using FileType = Enums::File::Type;
	using FontType = Enums::Internal::FontType;
	using GameMenuType = Enums::GameMenu::Type;
	using GameEntry = std::tuple<unsigned int, std::string, std::string,
		std::chrono::system_clock::time_point,
		std::chrono::system_clock::time_point>;
	using GraphicsTexture = Enums::Graphics::Texture;
	using IconLibrary = std::map<std::string, sf::Sprite>;
	using Import = Enums::Manage::Import;
	using Justification = Enums::Window::Justification;
	using MainMenuType = Enums::MainMenu::Type;
	using MenuItem = Enums::Menu::Item;
	using MenuType = Enums::Menu::Type;
	using MenuField = Enums::Menu::Field;
	using MenuButtonState = Enums::Menu::ButtonState;
	using MenuItemType = Enums::Menu::ItemType;
	using Method = Enums::Manage::Method;
	using ModuleResult = Enums::System::ModuleResult;
	using RandomType = Enums::System::Random;
	using Range = std::tuple<unsigned int, unsigned int>;
	using RosterMode = Enums::Manage::Roster;
	using SpellID = Enums::Magic::SpellID;
	using SpellCategory = Enums::Magic::SpellCategory;
	using SpellPoints = std::map<unsigned int, unsigned int>;
	using SpellType = Enums::Magic::SpellType;
	using SpellPointStatus = Enums::Magic::SpellPointStatus;
	using Stage = Enums::Character::Stage;
	using StringType = Enums::Internal::StringType;
	using StringMap = std::map<std::string, std::string>;
	using SystemError = Enums::System::Error;
	using WindowTooltipList = std::map<std::string, sf::FloatRect>;
	using WindowInputMode = Enums::Window::InputMode;
	using WindowInputCategory = Enums::Window::InputCategory;
	using WindowInput = Enums::Controls::Input;

	using WindowConfirm = Enums::Window::Confirm;
	using WindowDialogButton = Enums::Window::DialogButton;
	using WindowDialogType = Enums::Window::DialogType;
	using WindowDrawMode = Enums::Window::DrawMode;
	using WindowInputOption = Enums::Window::Option;
	using WindowFrameType = Enums::Window::FrameType;
	using WindowFrameParts = Enums::Window::FrameParts;
} // namespace Sorcery
