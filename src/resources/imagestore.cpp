// Copyright (C) 2025 Dave Moore
//
// This file is part of Sorcery.
//
// Sorcery is free software: you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software
// Foundation, either version 2 of the License, or (at your option) any later
// version.
//
// Sorcery is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along with
// Sorcery.  If not, see <http://www.gnu.org/licenses/>.
//
// If you modify this program, or any covered work, by linking or combining
// it with the libraries referred to in README (or a modified version of
// said libraries), containing parts covered by the terms of said libraries,
// the licensors of this program grant you additional permission to convey
// the resulting work.

#include "resources/imagestore.hpp"
#include "core/system.hpp"
#include "types/image.hpp"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch-default"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#pragma GCC diagnostic pop

Sorcery::ImageStore::ImageStore(System *system)
	: _system{system} {

	_initialise();
}

auto Sorcery::ImageStore::get(const std::string file) -> Image {

	if (!_loaded.at(file)) {
		_load_image(file);
		return _images.at(file);
	} else
		return _images.at(file);
}

auto Sorcery::ImageStore::_initialise() -> bool {

	loaded = false;
	_images.clear();
	_sources.clear();
	_loaded.clear();

	// Work out what we need to load and just store it in a list
	_sources.insert(
		_sources.end(),
		{std::string{BANNER_TEXTURE}, std::string{BACKGROUND_MAIN_MENU_TEXTURE},
		 std::string{ICONS_TEXTURE}, std::string{ITEMS_TEXTURE},
		 std::string{KNOWN_CREATURES_TEXTURE}, std::string{LOGO_TEXTURE},
		 std::string{MAPS_TEXTURE}, std::string{UNKNOWN_CREATURES_TEXTURE},
		 std::string{WIREFRAME_TEXTURE}});
	capacity = _sources.size();

	// Now set things as unloaded to begin with
	for (const auto &source : _sources)
		_loaded[source] = false;

	progress = 1;
	busy = false;

	return true;
}

// Load a specific image
auto Sorcery::ImageStore::load_image(const std::string file) -> bool {

	return _load_image(file);
}

auto Sorcery::ImageStore::has_loaded(const std::string file) -> bool {

	return _loaded.at(file);
}

// Wrapper method to load an image
auto Sorcery::ImageStore::_load_image(const std::string file) -> bool {

	// Check to make sure we don't reload an image (in future,
	// store the file modified timestamp so we can dynamically
	// reload images if we want to)
	if (_loaded.at(file))
		return false;
	else {

		// If not loaded, load the image
		Image image{};
		_load_texture_from_disc((*_system->files)[file].c_str(), &image.texture,
								&image.width, &image.height);
		_images.try_emplace(file, image);
		_loaded[file] = true;
		++progress;
		return true;
	}
}

// Load an image file from disk into a texture (using stb)
auto Sorcery::ImageStore::_load_texture_from_disc(const char *filename,
												  GLuint *out_texture,
												  int *out_width,
												  int *out_height) -> bool {

	busy = true;

	// Load from file
	int image_width{0};
	int image_height{0};

	// Get image data
	unsigned char *image_data{
		stbi_load(filename, &image_width, &image_height, nullptr, 4)};
	if (image_data == nullptr)
		return false;

	// Create a OpenGL texture identifier
	GLuint image_texture;
	glGenTextures(1, &image_texture);
	glBindTexture(GL_TEXTURE_2D, image_texture);

	// Setup filtering parameters for display
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// Transfer image pixels into the OpenGL texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0,
				 GL_RGBA, GL_UNSIGNED_BYTE, image_data);

	// Free up image data
	stbi_image_free(image_data);

	// Pass back metadata needed for ImGui
	*out_texture = image_texture;
	*out_width = image_width;
	*out_height = image_height;

	busy = false;

	return true;
}