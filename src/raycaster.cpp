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
#include "raycaster.hpp"

Sorcery::Raycaster::Raycaster(int width_, int height_) {
	mGfxWidth = width_;
	mGfxHeight = height_;

	mWallRenderMethod = Wall_Texture;
	mFloorRenderMethod = Floor_Texture;
	mCeilingRenderMethod = Ceiling_Texture;
	mEntityRenderMethod = Entity_Texture;

	// these distances will never change, so store them once to remove computation and division later.
	for (int i = 0; i < height_; ++i) {
		mHeightMap.push_back(mGfxHeight / (2.0 * i - mGfxHeight));
	}

	mZBuffer.clear();
	for (unsigned int i = 0; i < mGfxWidth; ++i) {
		mZBuffer.push_back(0);
	}

	mFloorcastingPixels = new sf::Uint8[mGfxWidth * mGfxHeight * 4];
	mFloorcastingTexture.create(mGfxWidth, mGfxHeight);
	mFloorcastingSprite.setTexture(mFloorcastingTexture);

	mMaxWallRenderDistance = 1000.0f;
	mMaxFloorRenderDistance = 10000.0f;
	mMaxObjectRenderDistance = 1000.0f;

	mCeilingRenderColor = sf::Color::Black;
	mFloorRenderColor = sf::Color::Black;
	mWallRenderColor = sf::Color::Blue;
}

void Sorcery::Raycaster::setMap(Sorcery::Map &map) {
	mMap = map;
}

void Sorcery::Raycaster::drawForCamera(sf::RenderWindow &window, Sorcery::Camera &camera) {
	sf::View view = sf::View(sf::FloatRect(0, 0, mGfxWidth, mGfxHeight));

	window.setView(view);

	drawForCamera(window, camera, view);

	window.setView(window.getDefaultView());
}

void Sorcery::Raycaster::drawForCamera(sf::RenderWindow &window, Sorcery::Camera &camera, sf::View &view) {

	int times = 0;

	/* FIRST STEP:
		* The background. This is can be nothing, a color, or the texture
		* created via the floor casting later on.
		*
		* If either the floor or ceiling rendering method is "Texture",
		* we start by cleaning up the texture that fits across the entire
		* background.
		*/
	if (mFloorRenderMethod == Floor_Texture || mCeilingRenderMethod == Ceiling_Texture) {

		for (unsigned int i = 3; i < mGfxWidth * mGfxHeight * 4; i += 4) {
			mFloorcastingPixels[i] = 0;
		}
	}
	/* If we aren't texturing the floor, then we don't do the raycasting
		* later on, and instead we go ahead and draw our colored floor.
		*/
	if (mFloorRenderMethod != Floor_Texture) {
		sf::RectangleShape floorshape;
		floorshape.setFillColor(mFloorRenderColor);
		floorshape.setSize(sf::Vector2f(mGfxWidth, mGfxHeight / 2));
		floorshape.setPosition(0, mGfxHeight / 2);
		window.draw(floorshape);
	}
	/* The ceiling works in the same way as the floor.
		*/
	if (mCeilingRenderMethod != Ceiling_Texture) {
		sf::RectangleShape ceilshape;
		ceilshape.setFillColor(mCeilingRenderColor);
		ceilshape.setSize(sf::Vector2f(mGfxWidth, mGfxHeight / 2));
		ceilshape.setPosition(0, 0);
		window.draw(ceilshape);
	}

	/* SECOND STEP:
		* Raycasting.
		* Now we start doing our raycasting, taking vertical slices across
		* the width of our drawing area.
		*
		* For each slice, we cast a ray from the camera to find out if and
		* where it hits a wall. We then calculate the distance and
		* perspective for that part of the wall. If we are using a texture,
		* we use this to figure out what part of the texture to draw. For
		* both textured and colored walls, we also figure out how "big" to
		* draw this slice of the wall based on distance away.
		*/
	for (unsigned int x = 0; x < mGfxWidth; ++x) {

		/* We start by figuring out where our camera is located
			* and where it is looking.
			*/
		float cameraX = 2 * x / float(mGfxWidth) - 1;

		float rayPosX = camera.getPosition().x;
		float rayPosY = camera.getPosition().y;

		float rayDirX = camera.getDirection().x + camera.getPlane().x * cameraX;
		float rayDirY = camera.getDirection().y + camera.getPlane().y * cameraX;

		//which box of the map we're in
		int mapX = int(rayPosX);
		int mapY = int(rayPosY);

		// length of ray from current position to next x or y-side
		float sideDistX = 0.0;
		float sideDistY = 0.0;

		// length of ray from one x or y-side to next x or y-side
		float deltaDistX = sqrtf(1 + (rayDirY * rayDirY) / (rayDirX * rayDirX));
		float deltaDistY = sqrtf(1 + (rayDirX * rayDirX) / (rayDirY * rayDirY));
		float perpWallDist;

		// what direction to step in x or y-direction (either +1 or -1)
		int stepX = 0;
		int stepY = 0;

		int hit = 0;
		int side;

		if (rayDirX < 0) {
			stepX = -1;
			sideDistX = (rayPosX - mapX) * deltaDistX;
		} else {
			stepX = 1;
			sideDistX = (mapX + 1.0 - rayPosX) * deltaDistX;
		}

		if (rayDirY < 0) {
			stepY = -1;
			sideDistY = (rayPosY - mapY) * deltaDistY;
		} else {
			stepY = 1;
			sideDistY = (mapY + 1.0 - rayPosY) * deltaDistY;
		}

		// perform DDA
		while (hit == 0) {
			// jump to next map square, OR in x-direction OR in y-direction
			if (sideDistX < sideDistY) {
				sideDistX += deltaDistX;
				mapX += stepX;
				side = 0;
			} else {
				sideDistY += deltaDistY;
				mapY += stepY;
				side = 1;
			}

			// check if ray has hit a wall
			if (mMap.getTile(mapX, mapY).isWall()) {
				hit = 1;
			}
		}

		// calculate distance projected on camera direction
		if (side == 0) {
			perpWallDist = fabs((float(mapX) - rayPosX + float(1 - stepX) / 2.0) / rayDirX);
		} else {
			perpWallDist = fabs((float(mapY) - rayPosY + float(1 - stepY) / 2.0) / rayDirY);
		}

		// we save this for later. To be used for drawing things
		// like sprites.
		mZBuffer[x] = perpWallDist; // perpendicular distance is used

		// calculate height of line to draw on screen
		int lineHeight = abs(int(mGfxHeight / perpWallDist));

		// calculate lowest and highest pixel to fill in current stripe
		int drawStart = -lineHeight / 2 + mGfxHeight / 2;
		int drawEnd = lineHeight / 2 + mGfxHeight / 2;

		// texturing calculations
		int texNum = mMap.getTile(mapX, mapY).getTextureIndex();

		// calculate value of wallX
		float wallX;
		if (side == 1) {
			wallX = rayPosX + ((mapY - rayPosY + (1 - stepY) / 2) / rayDirY) * rayDirX;
		} else {
			wallX = rayPosY + ((mapX - rayPosX + (1 - stepX) / 2) / rayDirX) * rayDirY;
		}
		wallX -= floor((wallX));

		// x coordinate on the texture
		int TEXTURE_WIDTH = mMap.getTexture(texNum).getSize().x;
		int TEXTURE_HEIGHT = mMap.getTexture(texNum).getSize().y;

		int texX = int(wallX * float(TEXTURE_WIDTH));
		if (side == 0 && rayDirX > 0) {
			texX = TEXTURE_WIDTH - texX - 1;
		}
		if (side == 1 && rayDirY < 0) {
			texX = TEXTURE_WIDTH - texX - 1;
		}

		/* Draw the slice of wall to the screen.
			*
			* Use a Quad sf::VertexArray to do the drawing.
			* This gives more flexibility and keeps from having
			* to draw each pixel one at a time, which boosts our
			* performance.
			*/
		if (perpWallDist <= mMaxWallRenderDistance) {
			if (mWallRenderMethod == Wall_Texture) {
				sf::VertexArray slice(sf::Quads, 4);
				slice[0].position = sf::Vector2f(x, drawStart);
				slice[1].position = sf::Vector2f(x + 1, drawStart);
				slice[2].position = sf::Vector2f(x + 1, drawEnd);
				slice[3].position = sf::Vector2f(x, drawEnd);

				slice[0].texCoords = sf::Vector2f(texX, 0);
				slice[1].texCoords = sf::Vector2f(texX + 1, 0);
				slice[2].texCoords = sf::Vector2f(texX + 1, TEXTURE_HEIGHT);
				slice[3].texCoords = sf::Vector2f(texX, TEXTURE_HEIGHT);

				window.draw(slice, &mMap.getTexture(texNum));
			} else if (mWallRenderMethod == Wall_Color) {
				sf::VertexArray slice(sf::Quads, 4);
				slice[0].position = sf::Vector2f(x, drawStart);
				slice[1].position = sf::Vector2f(x + 1, drawStart);
				slice[2].position = sf::Vector2f(x + 1, drawEnd);
				slice[3].position = sf::Vector2f(x, drawEnd);

				sf::Color wallColor = mWallRenderColor;

				if (side == 1) {
					wallColor.r = wallColor.r * 0.5;
					wallColor.g = wallColor.g * 0.5;
					wallColor.b = wallColor.b * 0.5;
				}

				slice[0].color = wallColor;
				slice[1].color = wallColor;
				slice[2].color = wallColor;
				slice[3].color = wallColor;

				window.draw(slice);
			}
		}

		++times;

		/* THIRD STEP
			* Floor and ceiling.
			*
			* This is where we take the biggest hit in performance.
			*
			* For a long time, games would draw a flat color instead
			* of a texture, because it is not as simple as drawing the
			* slices of walls. Instead, each point on the floor has to be
			* mapped to part of a texture, which is a lot of math calculations.
			* Then, each pixel has to be individually drawn to the screen, which
			* is also a major performance hit.
			*
			* For each slice, we need to figure out (remember from before) where
			* the wall (if there was one) starts and ends. This way, we can draw
			* floor and ceiling below and above it, respectively.
			*/
		float floorXWall;
		float floorYWall;

		// 4 different wall directions possible
		if (side == 0 && rayDirX > 0) {
			floorXWall = mapX;
			floorYWall = mapY + wallX;
		} else if (side == 0 && rayDirX < 0) {
			floorXWall = mapX + 1.0;
			floorYWall = mapY + wallX;
		} else if (side == 1 && rayDirY > 0) {
			floorXWall = mapX + wallX;
			floorYWall = mapY;
		} else {
			floorXWall = mapX + wallX;
			floorYWall = mapY + 1.0;
		}

		float distWall;
		float distPlayer;
		float currentDist;

		distWall = perpWallDist;
		distPlayer = 0.0;

		if (drawEnd < 0) {
			drawEnd = mGfxHeight;
		}

		/* We only do all of this work if a few conditions are met:
			*
			* 1) We have to be drawing the textures for either the floor or ceiling.
			*    if they're just the color, then we have no need to be here.
			* 2) If the camera has moved since last time. If the camera has not moved
			*    then we do not need to redo all of our math and drawing. We can just
			*    redraw what we had from before.
			*
			* THIS IS VERY IMPORTANT FOR PERFORMANCE.
			*/
		if ((mFloorRenderMethod == Floor_Texture || mCeilingRenderMethod == Ceiling_Texture) && camera.moved == true) {
			for (unsigned int y = drawEnd + 1; y < mGfxHeight; ++y) {
				currentDist = mHeightMap[y];

				float weight = (currentDist - distPlayer) / (distWall - distPlayer);

				float currentFloorX = weight * floorXWall + (1.0 - weight) * camera.getPosition().x;
				float currentFloorY = weight * floorYWall + (1.0 - weight) * camera.getPosition().y;

				float distanceToFloorTile =
					((camera.getPosition().x - int(currentFloorX)) * (camera.getPosition().x - int(currentFloorX)) +
						(camera.getPosition().y - int(currentFloorY)) * (camera.getPosition().y - int(currentFloorY)));
				if (distanceToFloorTile > mMaxFloorRenderDistance) {
					continue;
				}
				Sorcery::MapTile tile = mMap.getTile(int(currentFloorX), int(currentFloorY));

				int floorTextureWidth = tile.getTextureWidth();
				int floorTextureHeight = tile.getTextureHeight();

				int floorTexX = int(currentFloorX * floorTextureWidth) % floorTextureWidth;
				int floorTexY = int(currentFloorY * floorTextureHeight) % floorTextureHeight;

				sf::Color pix = mMap.getPixelFromTexture(tile.getTextureIndex(), floorTexX, floorTexY);

				if (mFloorRenderMethod == Floor_Texture) {
					unsigned int index = (y * (mGfxWidth * 4)) + (x * 4);
					mFloorcastingPixels[index] = pix.r;
					mFloorcastingPixels[index + 1] = pix.g;
					mFloorcastingPixels[index + 2] = pix.b;
					mFloorcastingPixels[index + 3] = pix.a;
				}

				if (mCeilingRenderMethod == Ceiling_Texture) {
					unsigned int index2 = ((mGfxHeight - y) * (mGfxWidth * 4)) + (x * 4);
					mFloorcastingPixels[index2] = pix.r * 0.5;
					mFloorcastingPixels[index2 + 1] = pix.g * 0.5;
					mFloorcastingPixels[index2 + 2] = pix.b * 0.5;
					mFloorcastingPixels[index2 + 3] = pix.a;
				}
			}
		}
	}
	/* Again, check if camera has moved and if we are drawing textures before
		* updating the background texture.
		*/
	if (camera.moved && (mFloorRenderMethod == Floor_Texture || mCeilingRenderMethod == Ceiling_Texture)) {
		mFloorcastingTexture.update(mFloorcastingPixels);
	}

	/* If we ARE drawing the textures, then we draw the background,
		* every time, whether or not the player has moved.
		*/
	if (mFloorRenderMethod == Floor_Texture || mCeilingRenderMethod == Ceiling_Texture) {
		window.draw(mFloorcastingSprite);
	}

	/* STEP, THE LAST
		* In which we draw our sprite entities.
		* These are supposed to be drawn above the floor; however,
		* we do have to check and make sure they are not too far away
		* or obstructed by walls or other objects.
		*/
	if (mEntityRenderMethod != Entity_None) {

		/* Start by sorting all of the entities by distance
			* to the camera.
			*/
		std::vector<Sorcery::Entity *> sprites = mMap.getEntities();
		std::vector<int> mSpriteOrder;
		std::vector<float> mSpriteDistance;
		for (unsigned int i = 0; i < sprites.size(); ++i) {
			float dist = ((camera.getPosition().x - sprites[i]->getPosition().x) *
							  (camera.getPosition().x - sprites[i]->getPosition().x) +
						  (camera.getPosition().y - sprites[i]->getPosition().y) *
							  (camera.getPosition().y - sprites[i]->getPosition().y));

			if (dist > mMaxObjectRenderDistance) {
				continue;
			}
			mSpriteOrder.push_back(i);
			mSpriteDistance.push_back(dist); // sqrt not taken, unneeded
		}
		combSort(mSpriteOrder, mSpriteDistance, mSpriteOrder.size());

		// after sorting the sprites, do the projection and draw them
		for (unsigned int i = 0; i < mSpriteOrder.size(); ++i) {
			// translate the sprite position to relative to camera
			float spriteX = sprites[mSpriteOrder[i]]->getPosition().x - camera.getPosition().x;
			float spriteY = sprites[mSpriteOrder[i]]->getPosition().y - camera.getPosition().y;

			// transform sprite with the inverse camera matrix
			float invDet =
				1.0 / (camera.getPlane().x * camera.getDirection().y -
						  camera.getDirection().x * camera.getPlane().y); // require for correct matrix multiplication.

			float transformX = invDet * (camera.getDirection().y * spriteX - camera.getDirection().x * spriteY);
			float transformY =
				invDet * (-camera.getPlane().y * spriteX +
							 camera.getPlane().x *
								 spriteY); // this is actually the depth inside the screen, that what Z is in the 3D

			/* If we cannot see this object because of rotation,
				* then we skip it and move on.
				*/
			if (transformY < 0) {
				continue;
			}

			int spriteScreenX = int((mGfxWidth / 2) * (1 + transformX / transformY));

			// calculate height of the sprite on screen
			int spriteHeight =
				abs(int(mGfxHeight / (transformY))); // using <transformY> instead of the real distance prevents fisheye
			// calculate lowest and highest pixel to fill in current stripe
			int drawStartY = -spriteHeight / 2 + mGfxHeight / 2;
			int drawEndY = spriteHeight / 2 + mGfxHeight / 2;

			// calculate width of the sprite
			int spriteWidth = abs(int(mGfxHeight / (transformY)));
			int drawStartX = -spriteWidth / 2 + spriteScreenX;
			int drawEndX = spriteWidth / 2 + spriteScreenX;

			int drawWidth = drawEndX - drawStartX;
			int drawOrigStartX = drawStartX;

			// draws vertical slices, iterating through x.
			sf::Texture spriteTexture = mMap.getTexture(sprites[mSpriteOrder[i]]->getTextureUID());
			int TEXTURE_WIDTH = spriteTexture.getSize().x;
			int TEXTURE_HEIGHT = spriteTexture.getSize().y;

			int spriteLeft = 0;
			int spriteRight = TEXTURE_WIDTH;
			int spriteTop = 0;
			int spriteBottom = TEXTURE_HEIGHT;

			int width = (int)mGfxWidth;

			if (drawStartX > width || drawEndX < 0) {
				continue;
			}

			for (int stripe = drawStartX; stripe <= drawEndX; ++stripe) {
				if (stripe < 0) {
					drawStartX += 1;
					continue;
				}
				// are other walls in front
				if (transformY > mZBuffer[stripe]) {
					drawStartX += 1;
					continue;
				}

				break;
			}

			int newWidth = drawEndX - drawStartX;

			float d = (float)drawWidth / (float)newWidth;

			spriteLeft = TEXTURE_WIDTH - (float)TEXTURE_WIDTH / d;

			for (int stripe = drawEndX; stripe > drawStartX; --stripe) {
				if (stripe > width) {
					drawEndX -= 1;
					continue;
				}

				if (transformY > mZBuffer[stripe]) {
					drawEndX -= 1;
					continue;
				}

				break;
			}

			newWidth = drawEndX - drawOrigStartX;
			d = (float)drawWidth / (float)newWidth;

			spriteRight = (float)TEXTURE_WIDTH / d;

			sf::VertexArray spriteQuad(sf::Quads, 4);

			spriteQuad[0].position = sf::Vector2f(drawStartX, drawStartY);
			spriteQuad[1].position = sf::Vector2f(drawEndX, drawStartY);
			spriteQuad[2].position = sf::Vector2f(drawEndX, drawEndY);
			spriteQuad[3].position = sf::Vector2f(drawStartX, drawEndY);

			spriteQuad[0].texCoords = sf::Vector2f(spriteLeft, spriteTop);
			spriteQuad[1].texCoords = sf::Vector2f(spriteRight, spriteTop);
			spriteQuad[2].texCoords = sf::Vector2f(spriteRight, spriteBottom);
			spriteQuad[3].texCoords = sf::Vector2f(spriteLeft, spriteBottom);

			window.draw(spriteQuad, &spriteTexture);
		}
	}
}

Sorcery::WallRenderMethod Sorcery::Raycaster::getWallRenderMethod() {
	return mWallRenderMethod;
}

void Sorcery::Raycaster::setWallRenderMethod(WallRenderMethod method) {
	mWallRenderMethod = method;
}

Sorcery::FloorRenderMethod Sorcery::Raycaster::getFloorRenderMethod() {
	return mFloorRenderMethod;
}

void Sorcery::Raycaster::setFloorRenderMethod(FloorRenderMethod method) {
	mFloorRenderMethod = method;
}

Sorcery::CeilingRenderMethod Sorcery::Raycaster::getCeilingRenderMethod() {
	return mCeilingRenderMethod;
}

void Sorcery::Raycaster::setCeilingRenderMethod(CeilingRenderMethod method) {
	mCeilingRenderMethod = method;
}

Sorcery::EntityRenderMethod Sorcery::Raycaster::getEntityRenderMethod() {
	return mEntityRenderMethod;
}

void Sorcery::Raycaster::setEntityRenderMethod(EntityRenderMethod method) {
	mEntityRenderMethod = method;
}

void Sorcery::Raycaster::updateForSize() {
	// distances
	mHeightMap.clear();
	for (unsigned int i = 0; i < mGfxHeight; ++i) {
		mHeightMap.push_back(mGfxHeight / (2.0 * i - mGfxHeight));
	}

	if (mFloorcastingPixels != nullptr) {
		delete mFloorcastingPixels;
		mFloorcastingPixels = nullptr;
	}

	mFloorcastingPixels = new sf::Uint8[mGfxWidth * mGfxHeight * 4];
	mFloorcastingTexture.create(mGfxWidth, mGfxHeight);
	mFloorcastingSprite = sf::Sprite(mFloorcastingTexture);

	mFloorcastingSprite.setPosition(0, 0);

	mZBuffer.clear();
	for (unsigned int i = 0; i < mGfxWidth; ++i) {
		mZBuffer.push_back(0);
	}
}

void Sorcery::Raycaster::setWidth(unsigned int amount) {
	mGfxWidth = amount;
	updateForSize();
}

void Sorcery::Raycaster::setHeight(unsigned int amount) {
	mGfxHeight = amount;
	updateForSize();
}

void Sorcery::Raycaster::setSize(unsigned int width_, unsigned int height_) {
	mGfxWidth = width_;
	mGfxHeight = height_;
	updateForSize();
}

// sorting algorithm
void Sorcery::Raycaster::combSort(std::vector<int> &order, std::vector<float> &dist, int amount) {
	//std::cout << "start sort" << std::endl;

	int gap = amount;
	bool swapped = false;
	while (gap > 1 || swapped) {
		// shrink factor 1.3
		gap = (gap * 10) / 13;
		if (gap == 9 || gap == 10) {
			gap = 11;
		}
		if (gap < 1) {
			gap = 1;
		}
		swapped = false;
		for (int i = 0; i < amount - gap; ++i) {
			int j = i + gap;
			if (dist[i] < dist[j]) {
				std::swap(dist[i], dist[j]);
				std::swap(order[i], order[j]);
				swapped = true;
			}
		}
	}

	//std::cout << "end sort" << std::endl;
}

float Sorcery::Raycaster::getMaxObjectRenderDistance() {
	return mMaxObjectRenderDistance;
}

void Sorcery::Raycaster::setMaxObjectRenderDistance(float distance) {
	mMaxObjectRenderDistance = distance;
}

void Sorcery::Raycaster::setWallRenderColor(const sf::Color &color) {
	mWallRenderColor = color;
}

void Sorcery::Raycaster::setCeilingRenderColor(const sf::Color &color) {
	mCeilingRenderColor = color;
}

void Sorcery::Raycaster::setFloorRenderColor(const sf::Color &color) {
	mFloorRenderColor = color;
}

float Sorcery::Raycaster::getMaxWallRenderDistance() {
	return mMaxWallRenderDistance;
}

void Sorcery::Raycaster::setMaxWallRenderDistance(float distance) {
	mMaxWallRenderDistance = distance;
}

float Sorcery::Raycaster::getMaxFloorRenderDistance() {
	return mMaxFloorRenderDistance;
}

void Sorcery::Raycaster::setMaxFloorRenderDistance(float distance) {
	mMaxFloorRenderDistance = distance;
}