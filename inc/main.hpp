// Copyright (C) 2020 Dave Moore
//
// This file is part of Sorcery: Dreams of the Mad Overlord
//
// Sorcery is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Sorcery is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Sorcery.  If not, see <http://www.gnu.org/licenses/>.
//
// If you modify this Program, or any covered work, by linking or combining it
// with the libraries referred to in README (or a modified version of said
// libraries), containing parts covered by the terms of said libraries, the
// licensors of this Program grant you additional permission to convey the
// resulting work.

#pragma once

// Library Includes
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <TGUI/TGUI.hpp>
#include <jsoncpp/json/json.h>
#include <Thor/Resources.hpp>

// External Includes
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch-default"
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#pragma GCC diagnostic ignored "-Wvolatile"
#pragma GCC diagnostic ignored "-Wignored-qualifiers"
#pragma GCC diagnostic ignored "-Wreorder"
#pragma GCC diagnostic ignored "-Wreturn-type"
//#include "json/json.h"
#include "sqlite_modern_cpp.h"
#include "SimpleIni.h"
#include "sfeMovie/Movie.hpp"
#pragma GCC diagnostic pop

// Include this here to avoid Status enums defined within clashing with above
#include <X11/Xlib.h>

// Standard Includes
#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <ctime>
#include <cstdio>
#include <exception>
#include <filesystem>
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
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <thread>
#include <tuple>
#include <typeinfo>
#include <utility>
#include <vector>

// Internal Includes
#include "define.hpp"
#include "enum.hpp"
#include "macro.hpp"
#include "type.hpp"
#include "operator.hpp"
#include "component.hpp"

// Aliases
namespace Sorcery {

	using Background = Enums::Graphics::Background;
	using Category = Enums::Manage::Category;
	using CharacterAbility = Enums::Character::Ability;
	using CharacterAbilities = std::map<Enums::Character::Ability, int>;
	using CharacterAlignment = Enums::Character::Align;
	using CharacterAttribute = Enums::Character::Attribute;
	using CharacterAttributes = std::map<Enums::Character::Attribute, int>;
	using CharacterClass = Enums::Character::Class;
	using CharacterClassList = std::map<Enums::Character::Class, std::string>;
	using CharacterClassMenu = std::pair<Enums::Character::Class, Enums::Menu::Item>;
	using CharacterClassQualified = std::map<Enums::Character::Class, bool>;
	using CharacterList = std::tuple<std::string, std::string, unsigned int, unsigned int, unsigned int, std::string,
		std::string>;
	using CharacterRace = Enums::Character::Race;
	// using CharacterStatus = Enums::Character::Status;
	using CharacterView = Enums::Character::Display;
	using ComponentType = Enums::Window::ComponentType;
	using Colour = Enums::Display::Colour;
	using ConfigOption = Enums::Options;
	using FileType = Enums::File::Type;
	using FontType = Enums::Internal::FontType;
	using GameMenuType = Enums::GameMenu::Type;
	using GraphicsTexture = Enums::Graphics::Texture;
	using Import = Enums::Manage::Import;
	using Justification = Enums::Window::Justification;
	using MainMenuType = Enums::MainMenu::Type;
	using MenuItem = Enums::Menu::Item;
	using MenuType = Enums::Menu::Type;
	using MenuField = Enums::Menu::Field;
	using MenuButtonState = Enums::Menu::ButtonState;
	using MenuItemType = Enums::Menu::ItemType;
	using Method = Enums::Manage::Method;
	using RandomType = Enums::System::Random;
	using Range = std::tuple<unsigned int, unsigned int>;
	using Spell = Enums::Magic::Spell;
	using SpellCategory = Enums::Magic::SpellCategory;
	using SpellEntry = std::tuple<Enums::Magic::Spell, Enums::Magic::SpellType, unsigned int,
		Enums::Magic::SpellCategory, bool>;
	using SpellPoints = std::map<unsigned int, unsigned int>;
	using SpellType = Enums::Magic::SpellType;
	using Stage = Enums::Character::Stage;
	using StringType = Enums::Internal::StringType;
	using StringMap = std::map<std::string, std::string>;
	using WindowTooltipList = std::map<std::string, sf::FloatRect>;
	using WindowInputMode = Enums::Window::InputMode;
	using WindowInput = Enums::Controls::Input;
	using WindowConfirm = Enums::Window::Confirm;
	using WindowDrawMode = Enums::Window::DrawMode;
	using WindowInputOption = Enums::Window::Option;
	using WindowFrameType = Enums::Window::FrameType;
	using WindowFrameParts = Enums::Window::FrameParts;
}
