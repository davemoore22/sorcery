// Copyright (C) 2024 Dave Moore
//
// This file is part of Sorcery: Shadows under Llylgamyn.
//
// Sorcery: Shadows under Llylgamyn is free software: you can redistribute
// it and/or modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation, either version 2 of the License,
// or (at your option) any later version.
//
// Sorcery: Shadows under Llylgamyn is distributed in the hope that it wil
// be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
// Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Sorcery: Shadows under Llylgamyn.  If not,
// see <http://www.gnu.org/licenses/>.
//
// If you modify this program, or any covered work, by linking or combining
// it with the libraries referred to in README (or a modified version of
// said libraries), containing parts covered by the terms of said libraries,
// the licensors of this program grant you additional permission to convey
// the resulting work.

#pragma once

#include "common/include.hpp"
#include "resources/type.hpp"

namespace Sorcery {

// Forward Declarations
class System;

class TextureStore {

	public:

		// Constructors
		TextureStore(System *system, const std::filesystem::path filename);
		TextureStore() = delete;

		// Overload [] operator
		auto operator[](const unsigned int index) const
			-> std::optional<Texture>;

		// Public Methods
		auto get(const unsigned int index, const GTT texture_type) const
			-> std::optional<sf::Sprite>;
		auto get(const std::string name, const GTT texture_type) const
			-> std::optional<sf::Sprite>;
		auto get_atlas(const Rect rect, bool feature) const -> sf::Sprite;

	private:

		// Private Members
		System *_system;
		bool _loaded;
		std::map<unsigned int, Texture> _texture_map;

		// Private Methods
		auto _get(const unsigned int index) const -> std::optional<Texture>;
		auto _get(const std::string name) const -> std::optional<Texture>;
		auto _get_rect(unsigned int index, GTT texture_type) const
			-> sf::IntRect;
		auto _load(const std::filesystem::path filename) -> bool;
};

}