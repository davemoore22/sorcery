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

// https://github.com/lxndrdagreat/sfray
#pragma once

#include "main.hpp"
#include "map.hpp"
#include "camera.hpp"

namespace Sorcery {

	enum WallRenderMethod {
		Wall_None = 0,
		Wall_Color,
		Wall_Texture
	};

	enum FloorRenderMethod {
		Floor_None = 0,
		Floor_Color,
		Floor_Texture
	};

	enum CeilingRenderMethod {
		Ceiling_None = 0,
		Ceiling_Color,
		Ceiling_Texture
	};

	enum EntityRenderMethod {
		Entity_None = 0,
		Entity_Texture
	};

	class Raycaster{
	private:
		unsigned int mGfxWidth;
		unsigned int mGfxHeight;

		// render options
		WallRenderMethod	mWallRenderMethod;
		FloorRenderMethod	mFloorRenderMethod;
		CeilingRenderMethod	mCeilingRenderMethod;
		float               mMaxWallRenderDistance;
		float               mMaxFloorRenderDistance;
		float				mMaxObjectRenderDistance;
		EntityRenderMethod	mEntityRenderMethod;

		sf::Color			mWallRenderColor;
		sf::Color			mCeilingRenderColor;
		sf::Color			mFloorRenderColor;

		// Z Buffer and getHeight Map used to speed up calculations in the loop.
		std::vector<int> 	mZBuffer;
		std::vector<float> 	mHeightMap;

		// Floorcasting pieces
		sf::Uint8*			mFloorcastingPixels;
		sf::Texture			mFloorcastingTexture;
		sf::Sprite			mFloorcastingSprite;

		// The world map
		Map 				mMap;

		void updateForSize();
		void combSort(std::vector<int>& order, std::vector<float>& dist, int amount);

	public:
		Raycaster(int width_=320, int height_=240);

		// size accessors and setters
		int getWidth() { return mGfxWidth; };
		int getHeight() { return mGfxHeight; };
		void setWidth(unsigned int amount);
		void setHeight(unsigned int amount);
		void setSize(unsigned int width_, unsigned int height_);

		// Set our map
		void setMap(Sorcery::Map &map);

		// Draw
		void drawForCamera(sf::RenderWindow& window, Camera& camera);
		void drawForCamera(sf::RenderWindow& window, Camera& camera, sf::View& view);

		// Rendering option getters/setters
		WallRenderMethod getWallRenderMethod();
		void setWallRenderMethod(WallRenderMethod method);
		FloorRenderMethod getFloorRenderMethod();
		void setFloorRenderMethod(FloorRenderMethod method);
		CeilingRenderMethod getCeilingRenderMethod();
		void setCeilingRenderMethod(CeilingRenderMethod method);
		EntityRenderMethod getEntityRenderMethod();
		void setEntityRenderMethod(EntityRenderMethod method);

		float getMaxObjectRenderDistance();
		void setMaxObjectRenderDistance(float distance);

		float getMaxWallRenderDistance();
		void setMaxWallRenderDistance(float distance);

		float getMaxFloorRenderDistance();
		void setMaxFloorRenderDistance(float distance);

		void setCeilingRenderColor(const sf::Color& color);
		void setFloorRenderColor(const sf::Color& color);
		void setWallRenderColor(const sf::Color& color);
	};
}
