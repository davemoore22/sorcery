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

#include "application.hpp"

// Standard Constructor
Sorcery::Application::Application(int argc, char** argv) {

	// First set up System modules
	system = std::make_unique<System>(argc, argv);
	display = std::make_unique<Display>(system.get());
	graphics = std::make_unique<Graphics>(system.get(), display.get());

	// Set up Game files
	_help = std::make_shared<TextFile>((*system->files)[HELP_FILE]);
	_version = std::make_shared<TextFile>((*system->files)[VERSION_FILE]);
	_license = std::make_shared<TextFile>((*system->files)[LICENSE_FILE]);

	// Load the Background Movie
	_background_movie.openFromFile(system->files->get_path_as_string(MENU_VIDEO));

	// Show the Splash Screen and the Banner before starting the Main Menu
	sf::Event input_event {};
	sf::RenderWindow* window = display->window->get_window();
	_splash = std::make_shared<Splash>(*system, *display, *graphics);
	while (!_splash->finished) {
		window->pollEvent(input_event);
		window->clear();
		_splash->update();
		_splash->draw();
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		window->display();
		if ((input_event.type == sf::Event::KeyPressed) || (input_event.type == sf::Event::MouseButtonPressed))
			_splash->finished = true;
	}
	_banner = std::make_shared<Banner>(*system, *display, *graphics);
	while (!_banner->finished) {
		window->pollEvent(input_event);
		window->clear();
		_banner->update();
		_banner->draw();
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		window->display();
		if ((input_event.type == sf::Event::KeyPressed) || (input_event.type == sf::Event::MouseButtonPressed))
			_banner->finished = true;
	}

	// Load the Background Movie
	window->clear();
	_background_movie.fit(0, 0, window->getSize().x, window->getSize().y);
    _background_movie.play();

	// Get the Logo
	sf::Sprite logo = display->window->get_logo();
	ImageSize logo_size {static_cast<unsigned int>(logo.getLocalBounds().width),
		static_cast<unsigned int>(logo.getLocalBounds().height)};
	const ImageSize window_size {window->getSize().x, window->getSize().y};
	float scale_ratio_needed {1.0f};
	if ((logo_size.w > window_size.w) || (logo_size.h > window_size.h)) {
		float shrink_width_needed {static_cast<float>(window_size.w) / static_cast<float>(logo_size.w)};
		float shrink_height_needed {static_cast<float>(window_size.h) / static_cast<float>(logo_size.h)};
		scale_ratio_needed = std::min(shrink_width_needed, shrink_height_needed);
	}
	logo.setScale(scale_ratio_needed, scale_ratio_needed);
	const ImageSize resized {static_cast<unsigned int>(logo.getGlobalBounds().width),
		static_cast<unsigned int>(logo.getGlobalBounds().height)};
	logo.setPosition(display->window->centre.x - (resized.w / 2), (resized.h / 2) - 50);

	// Get the Cursor
	sf::Sprite cursor = display->window->get_cursor();
	sf::Sprite attract_mode;
	std::vector<unsigned int> attract_mode_data;
	attract_mode_data.clear();

	// Start animation worker threads
	graphics->animation->force_refresh_attract_mode();
	graphics->animation->start_attract_mode_animation_threads();

	// Main Menu Loop
	while (window->isOpen()) {

		attract_mode_data = graphics->animation->get_attract_mode_data();
		sf::Event event;
		while (window->pollEvent(event)) {

			if (event.type == sf::Event::Closed)
       			window->close();
		}

		if (_background_movie.getStatus() == sfe::Stopped) {
			_background_movie.play();
		}
		_background_movie.update();

		window->clear();
		window->draw(_background_movie);
		if (attract_mode_data.size() > 0)
			display->window->draw_attract_mode(attract_mode_data);

		cursor.setPosition(static_cast<sf::Vector2f>(sf::Mouse::getPosition(*window)));
		window->draw(logo);
		window->draw(cursor);
		window->display();
	}
}

// Standard Destructor
Sorcery::Application::~Application() {
	graphics->animation->stop_attract_mode_animation_threads();
}




