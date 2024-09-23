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

#include "common/define.hpp"
#include "common/enum.hpp"
#include "common/include.hpp"
#include "common/macro.hpp"
#include "types/enum.hpp"

namespace Sorcery {

// Preemptive Aliases
using CFG = Enums::Config::Options;
using MIT = Enums::Menu::ItemType;
using MIM = Enums::Menu::Item;
using SPC = Enums::Magic::SpellCategory;
using SPI = Enums::Magic::SpellID;
using SPT = Enums::Magic::SpellType;
using MAV = Enums::Map::Event;

// Struct to represent a menu entry
struct MenuEntry {

		MenuEntry()
			: index{0}, type{MIT::NO_MENU_ITEM_TYPE}, item{MIM::NO_MENU_ITEM},
			  key{}, enabled{false}, config{CFG::NONE}, hint{}, idx{0} {};

		MenuEntry(unsigned int index_, MIT type_, MIM item_, std::string key_,
			bool enabled_, CFG config_, std::string hint_)
			: index{index_}, type{type_}, item{item_}, key{key_},
			  enabled{enabled_}, config{config_}, hint{hint_} {};

		MenuEntry(unsigned int index_, MIT type_, MIM item_, std::string key_,
			bool enabled_, unsigned int idx_)
			: index{index_}, type{type_}, item{item_}, key{key_},
			  enabled{enabled_}, idx{idx_} {};

		auto operator==(const MenuEntry &a) const -> bool {
			return ((index == a.index) && (type == a.type) &&
					(item == a.item) && (key == a.key) &&
					(enabled == a.enabled) && (config == a.config) &&
					(hint == a.hint) && (idx == a.idx));
		}

		auto operator=(const MenuEntry &other) -> MenuEntry & {

			index = other.index;
			type = other.type;
			item = other.item;
			key = other.key;
			enabled = other.enabled;
			config = other.config;
			hint = other.hint;
			idx = other.idx;

			return *this;
		};

		unsigned int index;
		MIT type;
		MIM item;
		std::string key;
		bool enabled;
		CFG config;
		std::string hint;
		unsigned int idx;
};

// Could also hold an optional bounds for maximum and minimum values?
struct Coordinate {

		Coordinate() : x{0}, y{0} {};

		Coordinate(int x_, int y_) : x{x_}, y{y_} {};

		auto operator==(const Coordinate &a) const -> bool {
			return (x == a.x && y == a.y);
		}

		auto operator<(const Coordinate &a) const -> bool {
			return std::tie(x, y) < std::tie(a.x, a.y);
		}

		// Remember that we are used to coordinates that are N and E, i.e. y and
		// x, not x and y!
		friend std::ostream &operator<<(std::ostream &os, Coordinate const &a) {
			return os << fmt::format("({}/{})", a.y, a.x) << std::endl;
		}

		template <class Archive> auto serialize(Archive &archive) -> void {
			archive(x, y);
		}

		auto inc_x() -> void {
			++x;
		};

		auto dec_x() -> void {
			--x;
		};

		auto inc_y() -> void {
			++y;
		};

		auto dec_y() -> void {
			--y;
		};

		int x;
		int y;
};

// Struct to represent the size of a map level
struct Size {

		Size() : w{0}, h{0} {};

		Size(unsigned int w_, unsigned int h_) : w{w_}, h{h_} {};

		Size(const Size &other) : w{other.w}, h{other.h} {};

		auto operator=(const Size &other) -> Size & {

			w = other.w;
			h = other.h;

			return *this;
		};

		friend std::ostream &operator<<(std::ostream &os, Size const &a) {
			return os << fmt::format("[{}/{}]", a.w, a.h) << std::endl;
		}

		template <class Archive> auto serialize(Archive &archive) -> void {
			archive(w, h);
		}

		unsigned int w;
		unsigned int h;
};

// Spells
struct Spell {

		Spell()
			: id{SPI::NO_SPELL}, type{SPT::NO_SPELL_TYPE},
			  category{SPC::NO_CATEGORY}, level{0}, known{false}, name{""},
			  translated_name{""}, details{""} {};

		Spell(SPI id_, SPT type_, SPC category_, unsigned int level_,
			bool known_, std::string name_, std::string translated_name_,
			std::string details_)
			: id{id_}, type{type_}, category{category_}, level{level_},
			  known{known_}, name{name_}, translated_name{translated_name_},
			  details{details_} {};

		Spell(const Spell &other)
			: id{other.id}, type{other.type}, category{other.category},
			  level{other.level}, known{other.known}, name{other.name},
			  translated_name{other.translated_name}, details{other.details} {};

		auto operator=(const Spell &other) -> Spell & {

			id = other.id;
			type = other.type;
			category = other.category;
			level = other.level;
			name = other.name;
			translated_name = other.translated_name;
			details = other.details;

			return *this;
		};

		Spell(Spell &&other) = default;
		Spell &operator=(Spell &&other) = default;

		SPI id;
		SPT type;
		SPC category;
		unsigned int level;
		bool known;
		std::string name;
		std::string translated_name;
		std::string details;
};

struct DungeonEvent {

		MAV event;
		std::string component_key;
		bool search_after;
		bool combat_after;
		bool go_back_after;
		bool go_town_after;
		bool enabled;
		int num;
		int count;

		DungeonEvent() = default;

		DungeonEvent(MAV event_, std::string component_key_, bool search_after_,
			bool combat_after_, bool go_back_after_, bool go_town_after_)
			: event{event_}, component_key{component_key_},
			  search_after{search_after_}, combat_after{combat_after_},
			  go_back_after{go_back_after_}, go_town_after{go_town_after_} {

			enabled = true;
			num = -1;
			count = -1;
		}
};

}