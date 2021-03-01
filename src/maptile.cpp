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
// said libraries), containing parts covered by the terms of said libraries,
// the licensors of this program grant you additional permission to convey
// the resulting work.

#include "maptile.hpp"

Sorcery::MapTile::MapTile() : mTileType(Floor), mIsSolid(false), mTextureIndex(0) {}

bool Sorcery::MapTile::isWall() {
	return mTileType == MapTileType::Wall;
}

bool Sorcery::MapTile::isFloor() {
	return mTileType == MapTileType::Floor;
}

bool Sorcery::MapTile::isSolid() {
	return mIsSolid;
}

void Sorcery::MapTile::setIsSolid(bool is_solid) {
	mIsSolid = is_solid;
}

void Sorcery::MapTile::setTileType(MapTileType t) {
	mTileType = t;
	if (mTileType == MapTileType::Wall) {
		mIsSolid = true;
	} else {
		mIsSolid = false;
	}
}

Sorcery::MapTileType Sorcery::MapTile::getTileType() {
	return mTileType;
}

int Sorcery::MapTile::getTextureIndex() {
	return mTextureIndex;
}

void Sorcery::MapTile::setTextureIndex(int index) {
	mTextureIndex = index;
}

unsigned int Sorcery::MapTile::getTextureWidth() {
	return mTextureWidth;
}

unsigned int Sorcery::MapTile::getTextureHeight() {
	return mTextureHeight;
}

void Sorcery::MapTile::setTextureWidth(unsigned int w) {
	mTextureWidth = w;
}

void Sorcery::MapTile::setTextureHeight(unsigned int h) {
	mTextureHeight = h;
}

void Sorcery::MapTile::setTextureSize(unsigned int width, unsigned int height) {
	mTextureWidth = width;
	mTextureHeight = height;
}