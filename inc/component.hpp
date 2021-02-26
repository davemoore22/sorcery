// Copyright (C) 2021 Dave Moore
//
// This file is part of Sorcery: Dreams of the Mad Overlord.
//
// Sorcery: Dreams of the Mad Overlord is free software: you can redistribute
// it and/or modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation, either version 2 of the License,
// or (at your option) any later version.
//
// Sorcery: Dreams of the Mad Overlord is distributed in the hope that it wil
// be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
// Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Sorcery: Dreams of the Mad Overlord.  If not,
// see <http://www.gnu.org/licenses/>.
//
// If you modify this program, or any covered work, by linking or combining
// it with the libraries referred to in README (or a modified version of
// said libraries), containing parts covered by the terms of said libraries,
// the licensors of this program grant you additional permission to convey
// the resulting work.

#pragma once

#include "main.hpp"

// Class to act as a graphical entity with layout information
namespace Sorcery {

	class Component {

	  public:
		// Constructors
		Component(std::string screen_, std::string name_, int x_, int y_, unsigned int w_, unsigned int h_,
			float scale_, Enums::Internal::FontType font_, unsigned int size_, unsigned long long colour_,
			bool animated_, std::string string_key_, unsigned int alpha_, unsigned int width_,
			unsigned long long background_, Enums::Window::Justification justification_,
			Enums::Window::ComponentType type_, unsigned int priority_, Enums::Window::DrawMode drawmode_,
			Enums::Graphics::Texture texture_);
		Component();

		// Copy Constructors
		Component(const Component &other);
		auto operator=(const Component &other) -> Component &;

		// Public Methods

		// Public Members
		std::string screen;
		std::string name;
		int x;
		int y;
		unsigned int w;
		unsigned int h;
		float scale;
		Enums::Internal::FontType font;
		unsigned int size;
		unsigned long long colour;
		bool animated;
		std::string string_key;
		unsigned int alpha;
		unsigned int width;
		unsigned long long background;
		Enums::Window::Justification justification;
		Enums::Window::ComponentType type;
		unsigned int priority;
		Enums::Window::DrawMode drawmode;
		Enums::Graphics::Texture texture;
		std::string unique_key;

	  private:
		// Private Methods

		// Private Members
	};
} // namespace Sorcery
