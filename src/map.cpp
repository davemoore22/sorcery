// Copyright (C) 2021 Dave Moore
//
// This file is part of Sorcery: Dreams of the Mad Overlord.
//
// Sorcery: Dreams of the Mad Overlord is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Sorcery: Dreams of the Mad Overlord is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Sorcery: Dreams of the Mad Overlord.  If not, see <http://www.gnu.org/licenses/>.
//
// If you modify this Program, or any covered work, by linking or combining it
// with the libraries referred to in README (or a modified version of said
// libraries), containing parts covered by the terms of said libraries, the
// licensors of this Program grant you additional permission to convey the
// resulting work.

#include "map.hpp"

Sorcery::Map::Map() {
	mWidth = 0;
	mHeight = 0;
	mTileStoreOrder = TileStoreOrder::ColumnFirstOrder;
}

Sorcery::Map::~Map() {
	mData.clear();
	mTextures.clear();
	mTexturePixelData.clear();
	Rooms.clear();

	//		for (auto entity : mEntities){
	//			delete entity;
	//		}
	mEntities.clear();
}

Sorcery::MapTile &Sorcery::Map::getTile(const unsigned int x, const unsigned int y) {
	unsigned int index;
	if (mTileStoreOrder == TileStoreOrder::RowFirstOrder) {
		index = y * mWidth + x;
	} else if (mTileStoreOrder == TileStoreOrder::ColumnFirstOrder) {
		index = x * mWidth + y;
	}
	return this->getTile(index);
}

Sorcery::MapTile &Sorcery::Map::getTile(const unsigned int index) {
	if (index >= mData.size()) {
		throw "Map::getTile ERROR: index out of bounds.";
	}
	return mData[index];
}

void Sorcery::Map::loadTexture(int numeric_index, const std::string &path) {
	mTextures[numeric_index] = sf::Texture();
	if (!mTextures[numeric_index].loadFromFile(path)) {
		throw "Could not load texture needed by map: '" + path + "'";
	}

	// store pixel data
	sf::Image imageCopy = mTextures[numeric_index].copyToImage();
	const sf::Uint8 *data = imageCopy.getPixelsPtr();
	if (data == nullptr) {
		throw "Could not get pixel data from image.";
	}
	unsigned int textureWidth = mTextures[numeric_index].getSize().x;
	unsigned int textureHeight = mTextures[numeric_index].getSize().y;
	for (unsigned int i = 0; i < textureWidth * textureHeight * 4; i += 4) {
		sf::Color c(data[i], data[i + 1], data[i + 2], data[i + 3]);
		mTexturePixelData[numeric_index].push_back(c);
	}
}

sf::Texture &Sorcery::Map::getTexture(int index) {
	return mTextures[index];
}

sf::Color Sorcery::Map::getPixelFromTexture(int index, int x, int y) {
	int pixel_index = y * mTextures[index].getSize().x + x;
	return mTexturePixelData[index][pixel_index];
}

void Sorcery::Map::addEntity(Entity *entity) {
	mEntities.push_back(entity);
}

std::vector<Sorcery::Entity *> Sorcery::Map::getEntities() {
	return mEntities;
}

void Sorcery::Map::setDataFromIntArray(const std::vector<int> data, unsigned int width, unsigned int height) {
	mWidth = width;
	mHeight = height;

	// clear any existing data
	mData.clear();

	for (auto &intTile : data) {
		MapTile tile;
		tile.setTextureIndex(intTile);
		if (intTile == 0) {
			// floor
			tile.setTileType(Sorcery::MapTileType::Floor);
		} else {
			tile.setTileType(Sorcery::MapTileType::Wall);
		}

		tile.setTextureWidth(mTextures[tile.getTextureIndex()].getSize().x);
		tile.setTextureHeight(mTextures[tile.getTextureIndex()].getSize().y);

		mData.push_back(tile);
	}
}

void Sorcery::Map::setDataFromMapTileArray(std::vector<MapTile> data, unsigned int width, unsigned int height) {
	mWidth = width;
	mHeight = height;

	// clear any existing data
	mData.clear();

	for (auto &tile : data) {
		tile.setTextureWidth(mTextures[tile.getTextureIndex()].getSize().x);
		tile.setTextureHeight(mTextures[tile.getTextureIndex()].getSize().y);

		mData.push_back(tile);
	}
}

Sorcery::TileStoreOrder Sorcery::Map::getTileStoreOrder() {
	return mTileStoreOrder;
}

void Sorcery::Map::setTileStoreOrder(TileStoreOrder order) {
	mTileStoreOrder = order;
}
