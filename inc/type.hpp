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

#include "main.hpp"

namespace Sorcery {

	// Note that we need to use full details in here as aliases haven't yet been
	// set up in main.hpp when we include enums.hpp

	// Struct to represent a menu entry
	struct MenuEntry {
		MenuEntry()
			: index{0}, type{Enums::Menu::ItemType::NONE},
			  item{Enums::Menu::Item::NONE}, key{}, enabled{false},
			  config{Enums::Options::NONE}, hint{} {};
		MenuEntry(unsigned int index_, Enums::Menu::ItemType type_,
			Enums::Menu::Item item_, std::string key_, bool enabled_,
			Enums::Options config_, std::string hint_)
			: index{index_}, type{type_}, item{item_}, key{key_},
			  enabled{enabled_}, config{config_}, hint{hint_} {};
		auto operator==(const MenuEntry &a) const -> bool {
			return ((index == a.index) && (type == a.type) &&
					(item == a.item) && (key == a.key) &&
					(enabled == a.enabled) && (config == a.config) &&
					(hint == a.hint));
		}

		unsigned int index;
		Enums::Menu::ItemType type;
		Enums::Menu::Item item;
		std::string key;
		bool enabled;
		Enums::Options config;
		std::string hint;
	};

	// Struct to represent a game
	struct GameEntry {
		GameEntry()
			: id{0}, key{}, status{}, start_time{}, time_point{}, data{} {};
		GameEntry(unsigned int id_, std::string key_, std::string status_,
			std::chrono::system_clock::time_point start_time_,
			std::chrono::system_clock::time_point time_point_,
			std::string data_)
			: id{id_}, key{key_}, status{status_}, start_time{start_time_},
			  time_point{time_point_}, data{data_} {};

		unsigned int id;
		std::string key;
		std::string status;
		std::chrono::system_clock::time_point start_time;
		std::chrono::system_clock::time_point time_point;
		std::string data;
	};

	// Struct to represent an icon
	struct Icon {
		Icon()
			: index{0}, item{Enums::Menu::Item::NONE}, key{}, filename{},
			  colour{} {};
		Icon(unsigned int index_, Enums::Menu::Item item_, std::string key_,
			std::string filename_, sf::Color colour_)
			: index{index_}, item{item_}, key{key_}, filename{filename_},
			  colour{colour_} {};
		auto operator==(const Icon &a) const -> bool {
			return ((index == a.index) && (item == a.item) && (key == a.key) &&
					(filename == a.filename) && (colour == a.colour));
		}

		unsigned int index;
		Enums::Menu::Item item;
		std::string key;
		std::string filename;
		sf::Color colour;
	};

	// Dungeon Graphics
	struct Texture {
		Texture()
			: index{0}, wall{0}, floor{0}, ceiling{0}, door{std::nullopt},
			  source{""}, comment{""} {};
		Texture(unsigned int index_, unsigned int wall_, unsigned int floor_,
			unsigned int ceiling_, std::optional<unsigned int> door_,
			std::string source_, std::string comment_ = "")
			: index{index_}, wall{wall_}, floor{floor_}, ceiling{ceiling_},
			  door{door_}, source{source_}, comment{comment_} {};
		auto operator==(const Texture &a) const -> bool {
			return (index == a.index && wall == a.wall && floor == a.floor &&
					ceiling == a.ceiling && door.value() == a.door.value() &&
					source == a.source);
		}

		unsigned int index;
		unsigned int wall;
		unsigned int floor;
		unsigned int ceiling;
		std::optional<unsigned int> door;
		std::string source;
		std::string comment;
	};

	// Struct to represent a coordinate
	struct Point {
		Point() : x{0}, y{0} {};
		Point(unsigned int x_, unsigned int y_) : x{x_}, y{y_} {};
		auto operator==(const Point &a) const -> bool {
			return (x == a.x && y == a.y);
		}

		template <class Archive> auto serialize(Archive &archive) -> void {
			archive(x, y);
		}

		unsigned int x;
		unsigned int y;
	};

	// Struct to represent an area of the screen (w, h)
	struct ScreenSize {
		ScreenSize() : w{0}, h{0} {};
		ScreenSize(unsigned int w_, unsigned int h_) : w{w_}, h{h_} {};
		ScreenSize(const ScreenSize &other) : w{other.w}, h{other.h} {};

		unsigned int w;
		unsigned int h;
	};

	// Struct to represent the size of an image
	struct ImageSize {
		ImageSize() : w{0}, h{0} {};
		ImageSize(unsigned int w_, unsigned int h_) : w{w_}, h{h_} {};
		ImageSize(const ImageSize &other) : w{other.w}, h{other.h} {};

		unsigned int w;
		unsigned int h;
	};

	// Struct to represent a rect on the screen
	struct Rect {
		Rect() : x{0}, y{0}, w{0}, h{0} {};
		Rect(unsigned int x_, unsigned int y_, unsigned int w_, unsigned int h_)
			: x{x_}, y{y_}, w{w_}, h{h_} {};
		Rect(const Rect &other)
			: x{other.x}, y{other.y}, w{other.w}, h{other.h} {};

		auto menu_contains(unsigned int i, unsigned int j) -> bool {
			// Note the >= etc here - menu rects start at y - if not = then
			// first item in menu would fail this test
			return ((i >= x) && (j >= y) && (i < (x + w - 1)) && (j < (y + h)));
		}

		auto highlight_contains(unsigned int i, unsigned int j) -> bool {
			return ((i >= x) && (j >= y) && (i < (x + w)) && (j <= (y + h)));
		}

		auto debug() const -> void {
			std::cout << x << "/" << y << " " << w << ":" << h << std::endl;
		}

		auto scale(int change) -> void {
			x -= change;
			y -= change;
			w += (change * 2);
			h += (change * 2);
		}

		unsigned int x;
		unsigned int y;
		unsigned int w;
		unsigned int h;
	};

	// Spells
	struct Spell {

		Spell()
			: id{Enums::Magic::SpellID::NONE},
			  type{Enums::Magic::SpellType::NONE},
			  category{Enums::Magic::SpellCategory::NONE}, level{0},
			  known{false}, name{""}, translated_name{""}, details{""} {};
		Spell(Enums::Magic::SpellID id_, Enums::Magic::SpellType type_,
			Enums::Magic::SpellCategory category_, unsigned int level_,
			bool known_, std::string name_, std::string translated_name_,
			std::string details_)
			: id{id_}, type{type_}, category{category_}, level{level_},
			  known{known_}, name{name_},
			  translated_name{translated_name_}, details{details_} {};
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

		Enums::Magic::SpellID id;
		Enums::Magic::SpellType type;
		Enums::Magic::SpellCategory category;
		unsigned int level;
		bool known;
		std::string name;
		std::string translated_name;
		std::string details;
	};

	struct Door {

		Enums::Tile::DoorType type;
		bool secret;
		int gfx;

		Door() : type{Enums::Tile::DoorType::NONE}, secret{false}, gfx{-1} {};

		// Serialisation
		template <class Archive> auto serialize(Archive &archive) -> void {
			archive(type, secret, gfx);
		}
	};

	struct FloorCeiling {

		bool visible;
		int gfx;

		FloorCeiling() : visible{false}, gfx{-1} {};
		FloorCeiling(int gfx_) : visible{true}, gfx{gfx_} {};

		// Serialisation
		template <class Archive> auto serialize(Archive &archive) -> void {
			archive(visible, gfx);
		}
	};

	struct Wall {

		bool visible;
		bool walkable;
		Enums::Map::Direction direction;
		int gfx;
		std::optional<Door> door;

		Wall()
			: visible{false}, walkable{false},
			  direction(Enums::Map::Direction::NONE), gfx{-1},
			  door{std::nullopt} {};

		Wall(Enums::Map::Direction direction_)
			: visible{true}, walkable{false}, direction{direction_}, gfx{0},
			  door{std::nullopt} {};

		// Serialisation
		template <class Archive> auto serialize(Archive &archive) -> void {
			archive(visible, walkable, direction, gfx, door);
		}
	};

	struct ViewNode {

		int id;
		int x;
		int y;
		int z;
		bool flipped;
		Enums::View::Cell::Layer layer;
		Enums::View::Cell::Type type;
		unsigned int dest_width;
		unsigned int dest_x;
		unsigned int dest_y;
	};

	struct SpellDetails {};
} // namespace Sorcery
