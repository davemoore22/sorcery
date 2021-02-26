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

#pragma once

#include "main.hpp"
#include "entity.hpp"
#include "maptile.hpp"
#include "maputils.hpp"

namespace Sorcery {

	/* TileStoreOrder relates to how the
	* 2D map is stored in a 1D array.
	*/
	enum TileStoreOrder {
		ColumnFirstOrder = 0, // x and then y
		RowFirstOrder // y and then x
	};

	class Map {
	protected:
		TileStoreOrder mTileStoreOrder;
		std::vector<MapTile> mData;
		std::map<int,sf::Texture> mTextures;
		std::map<int, std::vector<sf::Color>> mTexturePixelData;
		int mWidth;
		int mHeight;

		std::vector<Sorcery::Entity*> mEntities;

	public:
		Map();
		~Map();

		std::vector<RoomDim> Rooms;

		// getters
		int getWidth() { return mWidth; };
		int getHeight() { return mHeight; };

		MapTile& getTile(const unsigned int x, const unsigned int y);
		MapTile& getTile(const unsigned int index);

		TileStoreOrder getTileStoreOrder();
		void setTileStoreOrder(TileStoreOrder order);

		// Set map data
		void setDataFromIntArray(const std::vector<int> data, unsigned int width, unsigned int height);
		void setDataFromMapTileArray(std::vector<MapTile> data, unsigned int width, unsigned int height);

		void loadTexture(int numeric_index, const std::string &path);

		sf::Texture&getTexture(int index);
		sf::Color getPixelFromTexture(int index, int x, int y);

		std::vector<Sorcery::Entity*> getEntities();
		void addEntity(Sorcery::Entity* entity);
    };
}