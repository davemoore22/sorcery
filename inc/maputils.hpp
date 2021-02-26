// Copyright (C) 2021 Dave Moore
//
// This file is part of Sorcery: Dreams of the Mad Overlord.
//
// Sorcery: Dreams of the Mad Overlord is free software: you can redistribute
// it and/or modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation, either version 2 of the License,
// or (at your option) any later version.
//
// Sorcery: Dreams of the Mad Overlord is distributed in the hope that it wil
// be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
// Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Sorcery: Dreams of the Mad Overlord.  If not,
// see <http://www.gnu.org/licenses/>.
//
// If you modify this program, or any covered work, by linking or combining
// it with the libraries referred to in README (or a modified version of
// said  libraries), containing parts covered by the terms of said libraries,
// the licensors of this program grant you additional permission to convey
// the resulting work.

// https://github.com/lxndrdagreat/sfray
#pragma once

#include "main.hpp"

#define GRIDS 3
#define MAP_SIZE 50
#define DOOR_CHANCE 35

#define MAP_TILE_FLOOR 0
#define MAP_TILE_WALL 1

namespace Sorcery {

	struct RoomDim {
		int x;
		int y;
		int width;
		int height;
	};

	struct GenerationResults {
		std::vector<std::vector<int> > mapArray;
		std::vector<RoomDim> rooms;
	};

	static void CreateDoors(std::vector<std::vector<int> >& mapArray, std::vector<RoomDim>& rooms){
		int width = mapArray.size();
		int height = mapArray[0].size();

		// make doors
		for (unsigned int ri = 0; ri < rooms.size(); ++ri){
			RoomDim room = rooms[ri];
			for (int x = room.x-1; x < room.x-1+room.width+1; ++x){
				int y = room.y-1;

				if (mapArray[x][y] == 1
					&& (x-1 >= 0 && mapArray[x-1][y] != 1)
					&& (x+1 < width && mapArray[x+1][y] != 1)){

					int chance = rand() % 100;

					if (chance <= DOOR_CHANCE){
						mapArray[x][y] = 2;
					}

				}

				y = room.y + room.height;
				if (mapArray[x][y] == 1
					&& (x-1 >= 0 && mapArray[x-1][y] != 1)
					&& (x+1 < width && mapArray[x+1][y] != 1)){

					int chance = rand() % 100;

					if (chance <= DOOR_CHANCE){
						mapArray[x][y] = 2;
					}

				}
			}

			for (int y = room.y-1; y < room.y + room.height+1; ++y){
				int x = room.x - 1;

				if (mapArray[x][y] == 1
					&& (y-1 >= 0 && mapArray[x][y-1] != 1)
					&& (y+1 < height && mapArray[x][y+1] != 1)){

					int chance = rand() % 100;

					if (chance <= DOOR_CHANCE){
						mapArray[x][y] = 2;
					}
				}

				x = room.x + room.width;
				if (mapArray[x][y] == 1
					&& (y-1 >= 0 && mapArray[x][y-1] != 1)
					&& (y+1 < height && mapArray[x][y+1] != 1)){

					int chance = rand() % 100;

					if (chance <= DOOR_CHANCE){
						mapArray[x][y] = 2;
					}

				}
			}
		}

	}

	static void ConnectRoomsInOrder(std::vector<std::vector<int> >& mapArray, std::vector<RoomDim>& rooms)
	{
		for (unsigned int i = 0; i < rooms.size()-1; ++i){
			// pick a random room from the queue

			RoomDim room =  rooms[i];
			RoomDim other_room = rooms[i+1];

			// connect the two rooms
			int x = room.x + room.width / 2;
			int y = room.y + room.height / 2;
			int x2 = other_room.x + other_room.width / 2;
			int y2 = other_room.y + other_room.height / 2;

			while (x != x2){
				if (x < x2){
					++x;
				}
				else{
					--x;
				}
				mapArray[x][y] = MAP_TILE_FLOOR;

			}
			while (y != y2){
				if (y < y2){
					++y;
				}else{
					--y;
				}
				mapArray[x][y] = MAP_TILE_FLOOR;
			}
		}
	}
}