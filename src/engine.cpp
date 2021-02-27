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

#include "engine.hpp"

// Standard Constructor
Sorcery::Engine::Engine(System &system, Display &display, Graphics &graphics)
	: _system{system}, _display{display}, _graphics{graphics} {

	// Get the Window and Graphics to Display
	_window = _display.window->get_window();
}

// Standard Destructor
Sorcery::Engine::~Engine() {}

auto Sorcery::Engine::start() -> void {

	// This example map is a large room with the different features
	// showcased inside of it.
	// It's dimensions are 20x20.
	unsigned int mapWidth = 20;
	unsigned int mapHeight = 20;
	std::vector<int> basicLargeRoom{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0,
		0, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		1, 1, 0, 0, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0,
		0, 0, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 2, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0,
		0, 0, 0, 0, 0, 2, 0, 0, 2, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 2, 0, 0, 0, 0, 0, 0, 1, 1,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1};

	// This map is for testing the drawing distance
	std::vector<std::vector<int>> hugeSpace;
	for (int x = 0; x < 50; ++x) {
		hugeSpace.push_back(std::vector<int>());
		for (int y = 0; y < 50; ++y) {
			if (x == 0 || x == 49) {
				hugeSpace[x].push_back(1);
			} else if (y == 0 || y == 49) {
				hugeSpace[x].push_back(2);
			} else {
				hugeSpace[x].push_back(0);
			}
		}
	}

	// Create the actual map object that the renderer uses
	Sorcery::Map worldMap;

	// Load textures into the map object
	const std::filesystem::path file_path{_system.files->get_base_path() / "gfx"};
	worldMap.loadTexture(1, file_path.string() + "/wall1.png");
	worldMap.loadTexture(1, file_path.string() + "/wall1.png");	  // 128x128 version
	worldMap.loadTexture(2, file_path.string() + "/wall2.png");	  // 128x128 version
	worldMap.loadTexture(3, file_path.string() + "/wall3.png");	  // 128x128 version
	worldMap.loadTexture(0, file_path.string() + "/floor1.png");  // 128x128 version
	worldMap.loadTexture(-1, file_path.string() + "/floor2.png"); // 128x128 version
	worldMap.loadTexture(4, file_path.string() + "/sprite1.png");
	worldMap.loadTexture(5, file_path.string() + "/sprite2.png");

	// load in the map data from above
	worldMap.setDataFromIntArray(basicLargeRoom, mapWidth, mapHeight);

	// Some example entities for testing
	for (unsigned int i = 0; i < 10; ++i) {
		Sorcery::Entity *ent = new Sorcery::Entity();
		ent->setTextureUID((rand() % 2 == 0 ? 4 : 5));
		int x = 1 + rand() % (worldMap.getWidth() - 2);
		int y = 1 + rand() % (worldMap.getHeight() - 2);
		while (!worldMap.getTile(x, y).isFloor()) {
			x = 1 + rand() % (worldMap.getWidth() - 2);
			y = 1 + rand() % (worldMap.getHeight() - 2);
		}
		ent->setPosition(x, y);
		worldMap.addEntity(ent);
	}

	// Create the raycaster instance
	Sorcery::Raycaster raycaster;

	// Set the map for the raycaster
	raycaster.setMap(worldMap);

	// Let's make the floor and ceiling slightly different colors
	raycaster.setFloorRenderColor(sf::Color(50, 50, 50));
	raycaster.setCeilingRenderColor(sf::Color(80, 80, 80));

	// Max Object Render Distance
	// Change this to change how close entities have to be
	// to the camera in order to be drawn to the screen.
	// The default is an absurdly high number, since there
	// is not a major performance hit drawing the sprites.
	// However, they do not really look good at a far distance,
	// so you will probably want to change this to meet your needs.
	raycaster.setMaxObjectRenderDistance(100);

	// Set the raycaster engine's drawing size
	raycaster.setSize(1600, 900);

	// Set up the camera
	Sorcery::Camera camera;
	camera.setPosition(2, 2);
	camera.setDirectionInDegrees(0);

	// GAME TIME CLOCK
	sf::Clock clock;

	float fps = 0.0f;
	float fps_timer = 0.0f;

	// Framerate counter TEXT OBJECT
	sf::Text fps_text;
	//fps_text.setFont(_proportional_system_font);
	fps_text.setCharacterSize(32);
	fps_text.setColor(sf::Color::Yellow);
	fps_text.setPosition(10, 10);

	// Start the game loop. This is the normal basics used in SFML.
	while (_window->isOpen()) {
		// Process events
		sf::Event event;
		while (_window->pollEvent(event)) {
			// Close window : exit
			if (event.type == sf::Event::Closed) {
				_window->close();
			}

			// Escape pressed : exit
			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
				_window->close();
			}

			// render settings for debug
			// Z: Change floor render method
			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Z) {
				int current = raycaster.getFloorRenderMethod();
				current += 1;
				if (current > 2) {
					current = 0;
				}
				raycaster.setFloorRenderMethod((Sorcery::FloorRenderMethod)current);
			}
			// X: Change ceiling render method
			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::X) {
				int current = raycaster.getCeilingRenderMethod();
				current += 1;
				if (current > 2) {
					current = 0;
				}
				raycaster.setCeilingRenderMethod((Sorcery::CeilingRenderMethod)current);
			}
			// C: Change wall render method
			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::C) {
				int current = raycaster.getWallRenderMethod();
				current += 1;
				if (current > 2) {
					current = 0;
				}
				raycaster.setWallRenderMethod((Sorcery::WallRenderMethod)current);
			}
			// V: change entity render method
			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::V) {
				int current = raycaster.getEntityRenderMethod();
				current += 1;
				if (current > 1) {
					current = 0;
				}
				raycaster.setEntityRenderMethod((Sorcery::EntityRenderMethod)current);
			}
		}

		double delta = clock.restart().asSeconds();
		fps = 1.0f / delta;

		double rotSpeed = 3.0 * delta;
		float degreesToRotate = 128.0f * delta;
		double moveSpeed = 5.0 * delta;
		double moveCheck = 0.5;

		camera.moved = true;

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
			camera.moved = true;
			// rotate camera left
			camera.rotateByDegrees(degreesToRotate);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
			camera.moved = true;
			// rotate camera right
			camera.rotateByDegrees(-degreesToRotate);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
			// move camera forwards
			camera.moved = true;
			sf::Vector2f pos = camera.getPosition();
			if (worldMap
					.getTile(
						int(camera.getPosition().x + camera.getDirection().x * moveCheck), int(camera.getPosition().y))
					.isWall() == false) {
				pos.x += camera.getDirection().x * moveSpeed;
			}
			if (worldMap
					.getTile(
						int(camera.getPosition().x), int(camera.getPosition().y + camera.getDirection().y * moveCheck))
					.isWall() == false) {
				pos.y += camera.getDirection().y * moveSpeed;
			}
			camera.setPosition(pos);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
			// move camera backwards
			camera.moved = true;
			sf::Vector2f pos = camera.getPosition();
			if (worldMap
					.getTile(
						int(camera.getPosition().x - camera.getDirection().x * moveCheck), int(camera.getPosition().y))
					.isWall() == false) {
				pos.x -= camera.getDirection().x * moveSpeed;
			}
			if (worldMap
					.getTile(
						int(camera.getPosition().x), int(camera.getPosition().y - camera.getDirection().y * moveCheck))
					.isWall() == false) {
				pos.y -= camera.getDirection().y * moveSpeed;
			}
			camera.setPosition(pos);
		}

		// DRAWING BEGINS HERE

		// Clear the screen
		_window->clear();

		// Have the raycasting engine draw for our camera
		raycaster.drawForCamera(*_window, camera);

		// RESET THE VIEW
		//_window->setView(_window->getDefaultView());

		// Draw the framerate counter
		fps_timer -= delta;
		if (fps_timer <= 0.0f) {
			fps_timer = 1.0f;
			fps_text.setString(std::to_string((int)fps));
		}
		_window->draw(fps_text);

		// Update the window
		_window->display();
	}
}

auto Sorcery::Engine::stop() -> void {}

auto Sorcery::Engine::_draw() -> void {}
