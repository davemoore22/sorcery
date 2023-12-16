// Copyright (C) 2023 Dave Moore
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

#include "core/graphics.hpp"

// Standard Constructor
Sorcery::Graphics::Graphics(System *system, Display *display) : _system{system}, _display{display} {

	animation = std::make_unique<Animation>(system, display);
	icons = std::make_unique<IconStore>(system, (*display->layout)["global:icon"], (*system->files)[ICONS_FILE]);
	textures = std::make_unique<TextureStore>(system, (*system->files)[TEXTURES_FILE]);

	_wallpaper_idx = 0;
}

auto Sorcery::Graphics::adjust_brightness(sf::Color colour, double colour_lerp) -> unsigned long long {

	thor::ColorGradient gradient{};
	gradient[0.0f] = sf::Color(0x404040ff);
	gradient[0.5f] = colour;
	gradient[1.0f] = sf::Color(0xbfbfbfff);

	return (gradient.sampleColor(colour_lerp)).toInteger();
}

auto Sorcery::Graphics::adjust_status_colour(Enums::Character::CStatus value, bool poisoned) -> unsigned long long {

	thor::ColorGradient gradient{};
	gradient[0.0f] = sf::Color(0xbf0000ff);
	gradient[(1.0f / 8.0f) * 2.0f] = sf::Color(0xffff00ff);
	gradient[1.0f] = sf::Color(0x00ff00ff);

	auto to_scale{unenum(value) * 1.0f};
	to_scale = 8.0f - to_scale;
	if (poisoned)
		to_scale = (1.0f / 8.0f) * 2.0f;
	auto scaled{to_scale / 8.0f};
	return (gradient.sampleColor(scaled)).toInteger();
}

auto Sorcery::Graphics::get_background_sprite() -> sf::Sprite {

	if (_wallpaper_idx != animation->wallpaper_idx) {
		_wallpaper = textures->get(animation->wallpaper_idx, GraphicsTextureType::WALLPAPER).value();
		_wallpaper_idx = animation->wallpaper_idx;
	}

	return _wallpaper;
}

auto Sorcery::Graphics::tile_bg(sf::RenderWindow *window) -> void {

	static unsigned int wallpaper_idx;
	static sf::Sprite bg;
	if (wallpaper_idx != animation->wallpaper_idx) {
		wallpaper_idx = animation->wallpaper_idx;
		bg = get_background_sprite();
	}

	const auto cols{std::floor(_display->window->size.width / bg.getLocalBounds().width) + 1};
	const auto rows{std::floor(_display->window->size.height / bg.getLocalBounds().height) + 1};
	for (auto x = 0; x <= cols; x++) {
		for (auto y = 0; y <= rows; y++) {
			bg.setPosition(x * bg.getLocalBounds().width, y * bg.getLocalBounds().height);
			window->draw(bg);
		}
	}
}

auto Sorcery::Graphics::adjust_colour(int value, CharacterAbilityType ability_type) -> unsigned long long {

	// Colours "borrowed" from
	// https://github.com/angband/angband/blob/master/src/ui-player.c
	// https://github.com/angband/angband/blob/master/src/z-color.h
	thor::ColorGradient gradient{};
	gradient[0.0f] = sf::Color(0xbf0000ff);
	gradient[0.5f] = sf::Color(0xffff00ff);
	gradient[1.0f] = sf::Color(0x00ff00ff);
	auto to_scale{value * 1.0f};
	switch (ability_type) {
	case CharacterAbilityType::NUMBER: {
		to_scale = [&] {
			to_scale *= 0.5f;
			if (to_scale > 5.0f)
				return 5.0f;
			else if (to_scale < -5.0f)
				return -5.0f;
			else
				return to_scale;
		}();
		to_scale += 5.0f;
		auto scaled{to_scale / 10.0f};
		return (gradient.sampleColor(scaled)).toInteger();
	} break;
	case CharacterAbilityType::AC: {
		to_scale = [&] {
			if (to_scale < -10.0f)
				return -10.0f;
			else if (to_scale > 10.0f)
				return 10.0f;
			else
				return to_scale;
		}();
		to_scale = 10.0f - to_scale;
		auto scaled{to_scale / 20.0f};
		return (gradient.sampleColor(scaled)).toInteger();
	} break;
	case CharacterAbilityType::STAT: {
		to_scale = [&] {
			to_scale -= 3.0f;
			if (to_scale < 0.0f)
				return 0.0f;
			else if (to_scale > 15.0f)
				return 15.0f;
			else
				return to_scale;
		}();
		auto scaled{to_scale / 15.0f};
		return (gradient.sampleColor(scaled)).toInteger();
	} break;
	case CharacterAbilityType::PERCENTAGE: {
		auto scaled{to_scale / 100.0f};
		return (gradient.sampleColor(scaled)).toInteger();
	} break;
	case CharacterAbilityType::MODIFIER: {
		to_scale = [&] {
			to_scale *= 0.5f;
			if (to_scale > 5.0f)
				return 5.0f;
			if (to_scale < -5.0f)
				return -5.0f;
			else
				return to_scale;
		}();
		to_scale += 5.0f;
		auto scaled{to_scale / 10.0f};
		return (gradient.sampleColor(scaled)).toInteger();
	} break;
	default:
		return 0;
		break;
	}
}
