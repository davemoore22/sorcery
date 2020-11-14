// Copyright (C) 2020 Dave Moore
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

#pragma once

#include "main.hpp"

namespace Sorcery {

		enum MapTileType {
			Floor = 0,
			Wall = 1
		};

		class MapTile {
		protected:
			MapTileType     mTileType;
			bool            mIsSolid;

			int             mTextureIndex;
			unsigned int    mTextureWidth;
			unsigned int    mTextureHeight;

		public:
			MapTile();

			bool            isWall();
			bool            isFloor();

			void            setTileType(MapTileType t);
			MapTileType     getTileType();

			bool            isSolid();
			void            setIsSolid(bool is_solid);

			int             getTextureIndex();
			void            setTextureIndex(int index);

			unsigned int    getTextureWidth();
			unsigned int    getTextureHeight();
			void            setTextureWidth(unsigned int w);
			void            setTextureHeight(unsigned int h);
			void            setTextureSize(unsigned int width, unsigned int height);
		};
	}

