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

#pragma once

#include "common/include.hpp"

namespace Sorcery {

class System;
class Image;

class ImageStore {

	public:
		ImageStore(System *system);

		auto get(const std::string file) -> Image;
		auto has_loaded(const std::string file) -> bool;
		auto load_image(const std::string file) -> bool;

		bool loaded;
		unsigned int progress;
		unsigned int capacity;
		bool busy;

	private:
		auto _initialise() -> bool;
		auto _load_texture_from_disc(const char *filename, GLuint *out_texture,
									 int *out_width, int *out_height) -> bool;
		auto _load_image(const std::string key) -> bool;

		System *_system;
		std::map<std::string, Image> _images;
		std::map<std::string, bool> _loaded;
		std::vector<std::string> _sources;
};
}