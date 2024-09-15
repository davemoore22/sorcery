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

#include "common/enum.hpp"
#include "common/include.hpp"
#include "common/macro.hpp"
#include "common/type.hpp"
#include "core/macro.hpp"
#include "types/macro.hpp"

namespace Sorcery {

struct ConsoleMessage {

		long int id;
		IMT type;
		std::chrono::time_point<std::chrono::system_clock> datetime;
		std::string text;
		static inline long s_id{0};

		ConsoleMessage() {
		}

		ConsoleMessage(Enums::Internal::MessageType message_type_, std::string text_)
			: type{message_type_}, text{text_} {
			datetime = std::chrono::system_clock::now();
			id = s_id++;
		}

		ConsoleMessage(Enums::Internal::MessageType message_type_,
			std::chrono::time_point<std::chrono::system_clock> datetime_, std::string text_)
			: type{message_type_}, datetime{datetime_}, text{text_} {

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

// Struct to represent a game
struct GameEntry {
		GameEntry() : id{0}, key{}, status{}, start_time{}, time_point{}, data{} {};
		GameEntry(unsigned int id_, std::string key_, std::string status_,
			std::chrono::system_clock::time_point start_time_, std::chrono::system_clock::time_point time_point_,
			std::string data_)
			: id{id_}, key{key_}, status{status_}, start_time{start_time_}, time_point{time_point_}, data{data_} {};

		unsigned int id;
		std::string key;
		std::string status;
		std::chrono::system_clock::time_point start_time;
		std::chrono::system_clock::time_point time_point;
		std::string data;
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

struct ShopStock {

		ShopStock() : initial_stock{0}, current_stock{0}, buyable{false}, sellable{false} {};
		ShopStock(int initial_stock_, int current_stock_, bool buyable_, bool sellable_)
			: initial_stock{initial_stock_}, current_stock{current_stock_}, buyable{buyable_}, sellable{sellable_} {};

		template <class Archive> auto serialize(Archive &archive) -> void {
			archive(initial_stock, current_stock, buyable, sellable);
		}

		int initial_stock;
		int current_stock;
		bool buyable;
		bool sellable;
};

}