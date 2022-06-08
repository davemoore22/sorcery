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
			: name{0}, index{0}, wall{0}, floor{0}, ceiling{0}, door{0},
			  source{""}, comment{""} {};
		Texture(std::string name, unsigned int index_, unsigned int wall_,
			unsigned int floor_, unsigned int ceiling_, unsigned int door_,
			std::string source_, std::string comment_ = "")
			: name{name}, index{index_}, wall{wall_}, floor{floor_},
			  ceiling{ceiling_}, door{door_}, source{source_}, comment{
																   comment_} {};
		auto operator==(const Texture &a) const -> bool {
			return (name == a.name && index == a.index && wall == a.wall &&
					floor == a.floor && ceiling == a.ceiling &&
					door == a.door && source == a.source);
		}

		std::string name;
		unsigned int index;
		unsigned int wall;
		unsigned int floor;
		unsigned int ceiling;
		unsigned int door;
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

		// Remember that we are used to coordinates that are N and E, i.e. y and
		// x, not x and y!
		friend std::ostream &operator<<(std::ostream &os, Point const &a) {
			return os << fmt::format("[{}/{}]", a.y, a.x) << std::endl;
		}

		template <class Archive> auto serialize(Archive &archive) -> void {
			archive(x, y);
		}

		unsigned int x;
		unsigned int y;
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

	struct Coordinate3 {
		Coordinate3() : x{0}, y{0}, z{0} {};
		Coordinate3(int x_, int y_, int z_) : x{x_}, y{y_}, z{z_} {};
		Coordinate3(int x_, int z_) : x{x_}, y{0}, z{z_} {};

		auto operator==(const Coordinate3 &a) const -> bool {
			return (x == a.x && y == a.y && z == a.z);
		}
		auto operator<(const Coordinate3 &a) const -> bool {
			return std::tie(x, y, z) < std::tie(a.x, a.y, a.z);
		};
		friend std::ostream &operator<<(
			std::ostream &os, Coordinate3 const &a) {
			return os << fmt::format("[{}/{}/{}]", a.x, a.y, a.z) << std::endl;
		}

		template <class Archive> auto serialize(Archive &archive) -> void {
			archive(x, y, z);
		}

		int x;
		int y;
		int z;
	}; // namespace Sorcery

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

	// Struct to represent a rect on the screen
	struct Rect {
		Rect() : x{0}, y{0}, w{0}, h{0} {};
		Rect(unsigned int x_, unsigned int y_, unsigned int w_, unsigned int h_)
			: x{x_}, y{y_}, w{w_}, h{h_} {};
		Rect(const Rect &other)
			: x{other.x}, y{other.y}, w{other.w}, h{other.h} {};

		auto operator=(const Rect &other) -> Rect & {

			x = other.x;
			y = other.y;
			w = other.w;
			h = other.h;

			return *this;
		};

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
			: visible{true}, walkable{false}, direction{direction_}, gfx{-1},
			  door{std::nullopt} {};

		// Serialisation
		template <class Archive> auto serialize(Archive &archive) -> void {
			archive(visible, walkable, direction, gfx, door);
		}
	};

	struct Teleport {
		int to_level;
		Coordinate to_loc;

		Teleport() : to_level{1}, to_loc{Coordinate{0, 0}} {};
		Teleport(int to_level_, Coordinate to_loc_)
			: to_level{to_level_}, to_loc{to_loc_} {}

		// Serialisation
		template <class Archive> auto serialize(Archive &archive) -> void {
			archive(to_level, to_loc);
		}
	};

	struct TileNote {
		Coordinate loc;
		std::string text;
		bool visible;
		Enums::View::Message::Position position;

		TileNote()
			: loc{Coordinate{-1, -1}}, text{""}, visible{true},
			  position{Enums::View::Message::Position::NONE} {}

		TileNote(int x_, int y_, std::string text_) {
			loc = Coordinate{x_, y_};
			text = text_;
			visible = true;
			position = Enums::View::Message::Position::MIDDLE;
		}

		TileNote(Coordinate loc_, std::string text_, bool visible_)
			: loc{loc_}, text{text_}, visible{visible_},
			  position{Enums::View::Message::Position::MIDDLE} {};

		// Serialisation
		template <class Archive> auto serialize(Archive &archive) -> void {
			archive(loc, text, visible);
		}
	};

	struct TileView {

		Coordinate3 offset;
		sf::VertexArray floor;
		sf::VertexArray ceiling;
		sf::VertexArray up;
		sf::VertexArray down;
		sf::VertexArray darkness;
		sf::VertexArray side_darkness;
		sf::VertexArray back_wall;
		sf::VertexArray back_door;
		sf::VertexArray left_side_wall;
		sf::VertexArray left_side_door;
		sf::VertexArray right_side_wall;
		sf::VertexArray right_side_door;

		TileView() {

			offset = Coordinate3{0, 0, 0};
			floor.setPrimitiveType(sf::Quads);
			floor.resize(4);
			ceiling.setPrimitiveType(sf::Quads);
			ceiling.resize(4);
			up.setPrimitiveType(sf::Quads);
			up.resize(4);
			down.setPrimitiveType(sf::Quads);
			down.resize(4);
			darkness.setPrimitiveType(sf::Quads);
			darkness.resize(4);
			side_darkness.setPrimitiveType(sf::Quads);
			side_darkness.resize(4);
			back_door.setPrimitiveType(sf::Quads);
			back_door.resize(4);
			back_wall.setPrimitiveType(sf::Quads);
			back_wall.resize(4);
			left_side_wall.setPrimitiveType(sf::Quads);
			left_side_wall.resize(4);
			left_side_door.setPrimitiveType(sf::Quads);
			left_side_door.resize(4);
			right_side_wall.setPrimitiveType(sf::Quads);
			right_side_wall.resize(4);
			right_side_door.setPrimitiveType(sf::Quads);
			right_side_door.resize(4);
		};

		TileView(Coordinate3 offset_) : offset{offset_} {

			floor.setPrimitiveType(sf::Quads);
			floor.resize(4);
			ceiling.setPrimitiveType(sf::Quads);
			ceiling.resize(4);
			up.setPrimitiveType(sf::Quads);
			up.resize(4);
			down.setPrimitiveType(sf::Quads);
			down.resize(4);
			darkness.setPrimitiveType(sf::Quads);
			darkness.resize(4);
			side_darkness.setPrimitiveType(sf::Quads);
			side_darkness.resize(4);
			back_door.setPrimitiveType(sf::Quads);
			back_door.resize(4);
			back_wall.setPrimitiveType(sf::Quads);
			back_wall.resize(4);
			left_side_wall.setPrimitiveType(sf::Quads);
			left_side_wall.resize(4);
			left_side_door.setPrimitiveType(sf::Quads);
			left_side_door.resize(4);
			right_side_wall.setPrimitiveType(sf::Quads);
			right_side_wall.resize(4);
			right_side_door.setPrimitiveType(sf::Quads);
			right_side_door.resize(4);
		}
	};

	struct ViewNodeKey {
		ViewNodeKey()
			: layer{Enums::View::Cell::Layer::NONE}, x{0}, y{0}, z{0} {};
		ViewNodeKey(Enums::View::Cell::Layer layer_, int x_, int y_, int z_)
			: layer{layer_}, x{x_}, y{y_}, z{z_} {};
		ViewNodeKey(Enums::View::Cell::Layer layer_, int x_, int z_)
			: layer{layer_}, x{x_}, y{0}, z{z_} {};

		auto operator==(const ViewNodeKey &a) const -> bool {
			return (layer == a.layer && x == a.x && y == a.y && z == a.z);
		}
		auto operator<(const ViewNodeKey &a) const -> bool {
			return std::tie(layer, z, y, x) < std::tie(a.layer, a.z, a.y, a.x);
		};
		friend std::ostream &operator<<(
			std::ostream &os, ViewNodeKey const &a) {
			return os << fmt::format("[{}:{}/{}/{}]", a.layer, a.x, a.y, a.z)
					  << std::endl;
		}

		template <class Archive> auto serialize(Archive &archive) -> void {
			archive(layer, x, y, z);
		}

		Enums::View::Cell::Layer layer;
		int x;
		int y;
		int z;
	}; // namespace Sorcery

	struct ViewNode {

		Enums::View::Cell::Layer layer;
		Enums::View::Cell::Type type;
		bool flipped;
		Coordinate3 coords;
		Point dest;
		unsigned int dest_width;
		Rect source_rect;
		long int id;
		bool used;
		static inline long s_id{0};

		ViewNode() : id{s_id++} {

			used = false;
		};

		ViewNode(Enums::View::Cell::Layer layer_, Enums::View::Cell::Type type_,
			bool flipped_, Coordinate3 coords_, Point dest_,
			unsigned int dest_width_, Rect source_rect_)
			: layer{layer_}, type{type_}, flipped{flipped_}, coords{coords_},
			  dest{dest_}, dest_width{dest_width_},
			  source_rect{source_rect_}, id{s_id++} {

			used = true;
		};

		ViewNode(const ViewNode &other)
			: layer{other.layer}, type{other.type}, flipped{other.flipped},
			  coords{other.coords}, dest{other.dest},
			  dest_width{other.dest_width},
			  source_rect{other.source_rect}, id{other.id}, used{other.used} {};

		auto operator=(const ViewNode &other) -> ViewNode & {

			layer = other.layer;
			type = other.type;
			flipped = other.flipped;
			coords = other.coords;
			dest = other.dest;
			dest_width = other.dest_width;
			source_rect = other.source_rect;
			id = other.id;
			used = other.used;

			return *this;
		};
		friend std::ostream &operator<<(std::ostream &os, ViewNode &a) {
			return os << fmt::format("[{}: {}/{}/{}]", a.layer, a.coords.x,
							 a.coords.y, a.coords.z)
					  << std::endl;
		}
		auto operator<(const ViewNode &a) const -> bool {
			return std::tie(layer, coords.z, coords.y, coords.x) <
				   std::tie(layer, a.coords.z, a.coords.y, a.coords.x);
		}
	};

	struct SpellDetails {};

	struct ConsoleMessage {

		long int id;
		Enums::Internal::MessageType type;
		std::chrono::time_point<std::chrono::system_clock> datetime;
		std::string text;
		static inline long s_id{0};

		ConsoleMessage(
			Enums::Internal::MessageType message_type_, std::string text_)
			: type{message_type_}, text{text_} {
			datetime = std::chrono::system_clock::now();
			id = s_id++;
		}

		ConsoleMessage(Enums::Internal::MessageType message_type_,
			std::chrono::time_point<std::chrono::system_clock> datetime_,
			std::string text_)
			: type{message_type_}, datetime{datetime_}, text{text_} {

			id = s_id++;
		};

		friend std::ostream &operator<<(std::ostream &os, ConsoleMessage &a) {

			return os << fmt::format("[{}: {}]", TP2STR(a.datetime), a.text);
		}

	}; // namespace Sorcery

} // namespace Sorcery
