// Copyright (C) 2026 Dave Moore
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
#include "core/context.hpp"		 // for Context
#include "core/debug.hpp"		 // for DEBUG_LOGF, debug_logf
#include "resources/define.hpp"	 // for BACKGROUNDS_TEXTURE, BANNER_TEXTURE
#include "types/image.hpp"		 // for Image
#include "types/scopedtimer.hpp" // for PROFILE_SCOPE
#include <filesystem>			 // for path
#define STB_IMAGE_IMPLEMENTATION
#include <fstream>
#include <print>
#include <stb_image.h> // for stbi_load, stbi_image_free
#include <string_view> // for basic_string_view
#include <vector>

Sorcery::ImageStore::ImageStore(Context &ctx)
	: _ctx{ctx} {

	_initialise();
}

auto Sorcery::ImageStore::get(const std::string &file) -> Image {

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
	show_images = true;

	// Work out what we need to load and just store it in a list
	_sources.insert(_sources.end(),
					{std::string{BANNER_TEXTURE}, std::string{BACKGROUNDS_TEXTURE}, std::string{CONTROLS_TEXTURE},
					 std::string{EVENTS_TEXTURE}, std::string{ICONS_TEXTURE}, std::string{ITEMS_TEXTURE},
					 std::string{KNOWN_CREATURES_TEXTURE}, std::string{LOGO_TEXTURE}, std::string{MAPS_TEXTURE},
					 std::string{TOWN_TEXTURE}, std::string{TRAINING_TEXTURE}, std::string{UNKNOWN_CREATURES_TEXTURE},
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
auto Sorcery::ImageStore::load_image(const std::string &file) -> bool {
	return _load_image(file);
}

auto Sorcery::ImageStore::has_loaded(const std::string &file) -> bool {
	return _loaded.at(file);
}

// Wrapper method to load an image
auto Sorcery::ImageStore::_load_image(const std::string &file) -> bool {
	// Check to make sure we don't reload an image (in future,
	// store the file modified timestamp so we can dynamically
	// reload images if we want to)
	if (_loaded.at(file))
		return false;
	else {

		// PROFILE_SCOPE("ImageStore::_load_image");
		//  DEBUG_LOGF("Loading Resource: {}", file);

		const auto path{_ctx.get_file(file)};

		// If not loaded, load the image
		Image image{};
		_load_texture_from_disc(path, &image.texture, &image.width, &image.height);

		_images.try_emplace(file, image);
		_loaded[file] = true;
		++progress;
		return true;
	}
}

auto Sorcery::ImageStore::_load_texture_from_disc(const std::filesystem::path &filename, GLuint *out_texture,
												  int *out_width, int *out_height) -> bool {
	busy = true;

	std::ifstream stream{filename, std::ios::binary | std::ios::ate};
	if (!stream.is_open()) {
		busy = false;
		return false;
	}

	const auto size{stream.tellg()};
	if (size <= 0) {
		busy = false;
		return false;
	}

	std::vector<unsigned char> buffer(static_cast<std::size_t>(size));

	stream.seekg(0, std::ios::beg);
	if (!stream.read(reinterpret_cast<char *>(buffer.data()), size)) {
		busy = false;
		return false;
	}

	int image_width{0};
	int image_height{0};

	unsigned char *image_data{
		stbi_load_from_memory(buffer.data(), static_cast<int>(buffer.size()), &image_width, &image_height, nullptr, 4)};

	if (image_data == nullptr) {
		busy = false;
		return false;
	}

	GLuint image_texture{};
	glGenTextures(1, &image_texture);
	glBindTexture(GL_TEXTURE_2D, image_texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);

	if (const auto error{glGetError()}; error != GL_NO_ERROR) {
		DEBUG_LOGF("Texture allocation failed: {} ({}x{}) OpenGL error=0x{:04x}", filename, image_width, image_height,
				   static_cast<unsigned int>(error));
	}

	stbi_image_free(image_data);

	*out_texture = image_texture;
	*out_width = image_width;
	*out_height = image_height;

	busy = false;
	return true;
}