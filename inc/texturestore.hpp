// Copyright (C) 2021 Dave Moore
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

#include "main.hpp"
#include "system.hpp"

namespace Sorcery {

	class TextureStore {

	  public:
		// Constructors
		TextureStore(System *system, const std::filesystem::path filename);
		TextureStore() = delete;

		// Overload [] operator
		auto operator[](const unsigned int index) const
			-> std::optional<Texture>;

		// Public Methods
		auto get(const unsigned int index) const -> std::optional<Texture>;
		auto get(const unsigned int index,
			const GraphicsTextureType texture_type) const
			-> std::optional<sf::Sprite>;
		auto get(const std::string name,
			const GraphicsTextureType texture_type) const
			-> std::optional<sf::Sprite>;

	  private:
		// Private Members
		System *_system;
		bool _loaded;
		sf::Texture *_automap_t;
		sf::Texture *_wall_t;
		sf::Texture *_ceiling_t;
		sf::Texture *_floor_t;
		sf::Texture *_door_t;
		sf::Texture *_creatures_known_t;
		sf::Texture *_creatures_unknown_t;
		sf::Texture *_portrait_t;
		std::map<unsigned int, Texture> _texture_map;

		// Private Methods
		auto _get_rect(unsigned int index,
			GraphicsTextureType texture_type) const -> sf::IntRect;
		auto _load(const std::filesystem::path filename) -> bool;
	};
} // namespace Sorcery