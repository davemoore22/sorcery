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

#include "common/include.hpp"
#include "common/macro.hpp"
#include "core/include.hpp"

// Enums
namespace Sorcery {

struct Rect {

		// Struct to represent a rect on the screen
		Rect()
			: x{0},
			  y{0},
			  w{0},
			  h{0} {};
		Rect(unsigned int x_, unsigned int y_, unsigned int w_, unsigned int h_)
			: x{x_},
			  y{y_},
			  w{w_},
			  h{h_} {};
		Rect(const Rect &other)
			: x{other.x},
			  y{other.y},
			  w{other.w},
			  h{other.h} {};

		auto operator=(const Rect &other) -> Rect & {

			x = other.x;
			y = other.y;
			w = other.w;
			h = other.h;

			return *this;
		};

		auto contains(unsigned int i, unsigned int j) -> bool {
			return ((i >= x) && (j >= y) && (i < (x + w)) && (j <= (y + h)));
		}

		auto debug() const -> void {
			std::cout << x << "/" << y << " " << w << ":" << h << std::endl;
		}

		unsigned int x;
		unsigned int y;
		unsigned int w;
		unsigned int h;
};

struct Coordinate {

		// Equivalent on ImVec2 for Integers
		Coordinate()
			: x{0},
			  y{0} {};

		Coordinate(int x_, int y_)
			: x{x_},
			  y{y_} {};

		auto operator==(const Coordinate &a) const -> bool {
			return (x == a.x && y == a.y);
		}

		auto operator<(const Coordinate &a) const -> bool {
			return std::tie(x, y) < std::tie(a.x, a.y);
		}

		// Remember that in Wizardy Games we are used to coordinates that are N
		// and E, i.e. y and x, not x and y!
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

struct Size {

		// Struct to represent the size (of a map level, or a frame etc)
		Size()
			: w{0},
			  h{0} {};

		Size(unsigned int w_, unsigned int h_)
			: w{w_},
			  h{h_} {};

		Size(const Size &other)
			: w{other.w},
			  h{other.h} {};

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

struct Elevator {
		bool up;
		Coordinate up_loc;
		bool down;
		Coordinate down_loc;
		int top_depth;
		int bottom_depth;

		Elevator()
			: up{false},
			  up_loc{Coordinate{0, 0}},
			  down{false},
			  down_loc{Coordinate{0, 0}},
			  top_depth{0},
			  bottom_depth{0} {};

		Elevator(bool up_, Coordinate up_loc_, bool down_, Coordinate down_loc_,
				 int top_depth_, int bottom_depth_)
			: up{up_},
			  up_loc{up_loc_},
			  down{down_},
			  down_loc{down_loc_},
			  top_depth{top_depth_},
			  bottom_depth{bottom_depth_} {};

		// Serialisation
		template <class Archive> auto serialize(Archive &archive) -> void {
			archive(up, up_loc, down_loc, top_depth, bottom_depth);
		}
};

struct Teleport {
		int to_level;
		Coordinate to_loc;

		Teleport()
			: to_level{0},
			  to_loc{Coordinate{0, 0}} {};

		Teleport(int to_level_, Coordinate to_loc_)
			: to_level{to_level_},
			  to_loc{to_loc_} {}

		// Serialisation
		template <class Archive> auto serialize(Archive &archive) -> void {
			archive(to_level, to_loc);
		}
};

// Spells
struct Spell {

		Spell()
			: id{Enums::Magic::SpellID::NO_SPELL},
			  type{Enums::Magic::SpellType::NO_SPELL_TYPE},
			  category{Enums::Magic::SpellCategory::NO_CATEGORY},
			  level{0},
			  known{false},
			  name{""},
			  translated_name{""},
			  details{""} {};

		Spell(Enums::Magic::SpellID id_, Enums::Magic::SpellType type_,
			  Enums::Magic::SpellCategory category_, unsigned int level_,
			  bool known_, std::string name_, std::string translated_name_,
			  std::string details_)
			: id{id_},
			  type{type_},
			  category{category_},
			  level{level_},
			  known{known_},
			  name{name_},
			  translated_name{translated_name_},
			  details{details_} {};

		Spell(const Spell &other)
			: id{other.id},
			  type{other.type},
			  category{other.category},
			  level{other.level},
			  known{other.known},
			  name{other.name},
			  translated_name{other.translated_name},
			  details{other.details} {};

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

		Enums::Magic::SpellID id;
		Enums::Magic::SpellType type;
		Enums::Magic::SpellCategory category;
		unsigned int level;
		bool known;
		std::string name;
		std::string translated_name;
		std::string details;
};

// Struct to represent a game
struct GameEntry {

		GameEntry()
			: id{0},
			  key{},
			  status{},
			  start_time{},
			  time_point{},
			  data{} {};

		GameEntry(unsigned int id_, std::string key_, std::string status_,
				  std::chrono::system_clock::time_point start_time_,
				  std::chrono::system_clock::time_point time_point_,
				  std::string data_)
			: id{id_},
			  key{key_},
			  status{status_},
			  start_time{start_time_},
			  time_point{time_point_},
			  data{data_} {};

		unsigned int id;
		std::string key;
		std::string status;
		std::chrono::system_clock::time_point start_time;
		std::chrono::system_clock::time_point time_point;
		std::string data;
};

struct ConsoleMessage {

		long int id;
		Enums::Internal::MessageType type;
		std::chrono::time_point<std::chrono::system_clock> datetime;
		std::string text;
		static inline long s_id{0};

		ConsoleMessage() {}

		ConsoleMessage(Enums::Internal::MessageType message_type_,
					   std::string text_)
			: type{message_type_},
			  text{text_} {
			datetime = std::chrono::system_clock::now();
			id = s_id++;
		}

		ConsoleMessage(
			Enums::Internal::MessageType message_type_,
			std::chrono::time_point<std::chrono::system_clock> datetime_,
			std::string text_)
			: type{message_type_},
			  datetime{datetime_},
			  text{text_} {

			id = s_id++;
		};

		friend std::ostream &operator<<(std::ostream &os, ConsoleMessage &a) {

			return os << fmt::format("[{}: {}]", TP2STR(a.datetime), a.text);
		}

		// Serialisation
		template <class Archive> auto serialize(Archive &archive) -> void {
			archive(id, type, datetime, text, s_id);
		}
};

struct ShopStock {

		ShopStock()
			: initial_stock{0},
			  current_stock{0},
			  buyable{false},
			  sellable{false} {};

		ShopStock(int initial_stock_, int current_stock_, bool buyable_,
				  bool sellable_)
			: initial_stock{initial_stock_},
			  current_stock{current_stock_},
			  buyable{buyable_},
			  sellable{sellable_} {};

		template <class Archive> auto serialize(Archive &archive) -> void {
			archive(initial_stock, current_stock, buyable, sellable);
		}

		int initial_stock;
		int current_stock;
		bool buyable;
		bool sellable;
};

struct DungeonEvent {

		Enums::Map::Event event;
		std::string component_key;
		bool search_after;
		bool combat_after;
		bool go_back_after;
		bool go_town_after;
		bool enabled;
		int num;
		int count;

		DungeonEvent() = default;

		DungeonEvent(Enums::Map::Event event_, std::string component_key_,
					 bool search_after_, bool combat_after_,
					 bool go_back_after_, bool go_town_after_)
			: event{event_},
			  component_key{component_key_},
			  search_after{search_after_},
			  combat_after{combat_after_},
			  go_back_after{go_back_after_},
			  go_town_after{go_town_after_} {

			enabled = true;
			num = -1;
			count = -1;
		}
};

};