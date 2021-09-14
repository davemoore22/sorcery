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

#include "render.hpp"

// Standard Constructor
Sorcery::Render::Render(
	System *system, Display *display, Graphics *graphics, Game *game)
	: _system{system}, _display{display}, _graphics{graphics}, _game{game} {

	theta = 0.0f;
	speed = 0.0f;
	turn = 0.0f;

	// Precalculatw the Rays
	tan_FOV = tan(FOV / 2);

	// A ray will be casted for every horizontal pixel
	for (int i = 0; i < MINIMUM_SCREEN_WIDTH; i++) {
		scr_pts[i] = tan_FOV - (2 * tan_FOV * (i + 1)) / MINIMUM_SCREEN_WIDTH;
		distortion[i] = 1.0 / sqrt(1 + scr_pts[i] * scr_pts[i]);
	}

	// Setup the Draw Surface
	_rtexture.create(MINIMUM_SCREEN_WIDTH, MINIMUM_SCREEN_HEIGHT);
	_rtexture.setSmooth(true);
	_rtexture.clear(sf::Color(255, 0, 0, 255));
}

auto Sorcery::Render::update() -> void {

	double coss = cos(theta);
	double sinn = sin(theta);

	int dsgn = sgn(speed); // for when walking backwards
	double sx = sgn(coss) * dsgn;
	double sy = sgn(sinn) * dsgn;

	// double dt = (diffTicks / 1000.0);
	double dp = speed;
	double dx = coss * dp;
	double dy = sinn * dp;

	double px_new = _game->state->world->player_pos.x + dx;
	double py_new = _game->state->world->player_pos.y + dy;

	// collision detection
	double cx_b = px_new - sx * MIN_DIST;
	double cx_f = px_new + sx * MIN_DIST;
	double cy_b = py_new - sy * MIN_DIST;
	double cy_f = py_new + sy * MIN_DIST;
	if (!_game->state->world->current_level->is_colliding(cx_f, cy_f) &&
		!_game->state->world->current_level->is_colliding(cx_b, cy_f) &&
		!_game->state->world->current_level->is_colliding(
			cx_f, cy_b)) { // direction where playing is looking at
		_game->state->world->player_pos.x = px_new;
		_game->state->world->player_pos.y = py_new;
	} else if (!_game->state->world->current_level->is_colliding(
				   cx_f, _game->state->world->player_pos.y + MIN_DIST) &&
			   !_game->state->world->current_level->is_colliding(
				   cx_f, _game->state->world->player_pos.y -
							 MIN_DIST)) { // X-direction
		_game->state->world->player_pos.x = px_new;
	} else if (!_game->state->world->current_level->is_colliding(
				   _game->state->world->player_pos.x + MIN_DIST, cy_f) &&
			   !_game->state->world->current_level->is_colliding(
				   _game->state->world->player_pos.x - MIN_DIST,
				   cy_f)) { // Y-direction
		_game->state->world->player_pos.y = py_new;
	}
	// else: no movement possible (corner)

	double diffTurn = turn;
	theta += diffTurn;
}

auto Sorcery::Render::render() -> void {

	_rtexture.clear(sf::Color(255, 0, 0, 255));
	_rtexture.display();
	_texture = _rtexture.getTexture();
	_sprite = sf::Sprite(_texture);

	// scale the sprite
	auto current_size{_display->window->size};
	float scale_x{(current_size.width * 1.0f) / _sprite.getLocalBounds().width};
	float scale_y{
		(current_size.height * 1.0f) / _sprite.getLocalBounds().height};
	_sprite.setScale(scale_x, scale_y);

	// render scene goes here
	// SDL code:
	/*
	for (int i = 0; i < SCREEN_WIDTH; i++)
	{
		double r_x = 1.0;
		double r_y = scr_pts[i]; // precalculated
		double base = sqrt(r_x * r_x + r_y * r_y);
		r_x = r_x / base;
		r_y = r_y / base;

		// rotate this ray with teta
		double rot_x = cos (teta) * r_x - sin (teta) * r_y;
		double rot_y = sin (teta) * r_x + cos (teta) * r_y;

		// step sizes
		int step_x = sgn(rot_x);
		int step_y = sgn(rot_y);

		// grid lines and hitpoints to calculate
		int l_vx = round(px + 0.5 * step_x);
		double l_vy = -1;
		int l_hy = round(py + 0.5 * step_y);
		double l_hx = -1;

		// find hitpoint
		double dist = -1;
		int txt_x = -1; // 0..63 (texture width = 64)

		double hit_x = 0, hit_y = 0;
		int wall_idx;
		while (dist < 0)
		{
			// calculate the hitpoints with the grid lines
			if (l_vy == -1 && step_x != 0)
				l_vy = py + (l_vx - px) * (rot_y / rot_x);

			if (l_hx == -1 && step_y != 0)
				l_hx = px + (l_hy - py) * (rot_x / rot_y);

			// determine which one "wins" (= shortest distance)
			bool vertWin;
			if (l_vy != -1 && l_hx != -1)
			{    // 2 candidates, choose closest one
				vertWin = step_x * (l_vx - px) < step_x * (l_hx - px);
			}
			else
			{    // one candidate
				vertWin = l_vy != -1;
			}

			// determine array indices
			int arr_x = -1, arr_y = -1;
			if (vertWin)
			{
				hit_x = l_vx;
				hit_y = l_vy;

				txt_x = 64 * (hit_y - (int) hit_y);
				if ( step_x == 1)
				{ 	// // looking from the left, mirror the texture to correct
					txt_x = 63 - txt_x;
				}

				l_vx += step_x;
				l_vy = -1;

				arr_x = step_x < 0 ? hit_x - 1: hit_x;
				arr_y = GRID_HEIGHT - hit_y;
			}
			else
			{
				hit_x = l_hx;
				hit_y = l_hy;

				txt_x = 64 * (hit_x - (int) hit_x);
				if ( step_y == -1)
				{ 	// looking from above, mirror the texture to correct
					txt_x = 63 - txt_x;
				}

				l_hx = -1;
				l_hy += step_y;

				arr_x = hit_x;
				arr_y = GRID_HEIGHT - (step_y < 0 ? hit_y - 1: hit_y) - 1;
			}

			wall_idx = world[arr_y][arr_x];
			if (wall_idx != 0)
			{    // we've hit a block
				double dx = hit_x - px;
				double dy = hit_y - py;
				dist = sqrt (dx * dx + dy * dy);
			}
		}

		// correct distance and calculate height
		double corrected = dist * distortion[i];
		int height = SCREEN_HEIGHT / corrected;

		int y = (SCREEN_HEIGHT - height) / 2;
		int darkness = 255;
		if (corrected > DARKEST_DIST)
			darkness = DARKNESS_ALPHA;
		else if (corrected <= MIN_DIST)
			darkness = 255;
		else // interpolate
			darkness = (int) ( (corrected - MIN_DIST) * (DARKNESS_ALPHA - 255) /
	(DARKEST_DIST - MIN_DIST) + 255);

		SDL_Rect src = { txt_x, 0, 1, 64 };
		SDL_Rect dst = { i, y, 1, height };

		if (wall_idx - 1 >= 3)
		{
			printf("%d\n", wall_idx);
		}
		SDL_Texture* txt = txt_walls[wall_idx - 1];

		SDL_SetTextureColorMod( txt, darkness, darkness, darkness );
		SDL_RenderCopy(gRenderer, txt, &src, &dst);

		// get floor texture pixels

		if (y > 0)
		{
			Uint32 floor [y];
			Uint32 ceilg [y];
			Uint32* pixsflr = (Uint32*) srf_floor->pixels;
			Uint32* pixsclg = (Uint32*) srf_ceilg->pixels;
			for (int j = y - 1 ; j >= 0; j--)
			{
				double rev_height =  SCREEN_HEIGHT - 2 * j;
				double rev_corr = SCREEN_HEIGHT / rev_height;
				double rev_dist = rev_corr / distortion[i];

				double real_x = px + rot_x * rev_dist;
				double real_y = py + rot_y * rev_dist;

				real_x = real_x - (int) real_x;
				real_y = real_y - (int) real_y;
				if (real_x < 0) real_x += 1;
				if (real_y < 0) real_y += 1;
				int tx = (int)(real_x * 64);
				int ty = (int)(real_y * 64);

				int darkflr = 255;
				if (rev_corr > DARKEST_DIST)
					darkflr = DARKNESS_ALPHA;
				else if (rev_corr <= MIN_DIST)
					darkflr = 255;
				else // interpolate
					darkflr = (int) ( (rev_corr - MIN_DIST) * (DARKNESS_ALPHA -
	255) / (DARKEST_DIST - MIN_DIST) + 255); double scale = 1.0 * darkflr / 255;

				Uint32 pixflr = (Uint32) pixsflr [64 * ty + tx];
				Uint32 pixclg = (Uint32) pixsclg [64 * ty + tx];

				Uint32 f_r = (( pixflr >> 16 ) & 0xFF ) * scale;
				Uint32 f_g = (( pixflr >> 8 ) & 0xFF ) * scale;
				Uint32 f_b = (( pixflr >> 0 ) & 0xFF ) * scale;
				floor[y - 1 - j] = (f_r << 16) + (f_g << 8) + (f_b << 0);

				Uint32 g_r = (( pixclg >> 16 ) & 0xFF ) * scale;
				Uint32 g_g = (( pixclg >> 8 ) & 0xFF ) * scale;
				Uint32 g_b = (( pixclg >> 0 ) & 0xFF ) * scale;
				ceilg[j] = (g_r << 16) + (g_g << 8) + (g_b << 0);
			}

			int pitch = 1 * sizeof(Uint32);
			SDL_Rect rect = {0, 0, 1, y};

			if (SDL_LockTexture(gFloorTexture, &rect, &mPixels, &pitch) != 0){
				printf("Error: %s\n", SDL_GetError() );
			}
			Uint8* pixels = (Uint8 *) mPixels;
			memcpy(pixels, floor, y * pitch);
			SDL_UnlockTexture(gFloorTexture);
			SDL_Rect dstflr = {i, y + height, 1, y};
			SDL_RenderCopy (gRenderer, gFloorTexture, &rect, &dstflr);

			if (SDL_LockTexture(gCeilgTexture, &rect, &mPixels, &pitch) != 0){
				printf("Error: %s\n", SDL_GetError() );
			}
			pixels = (Uint8 *) mPixels;
			memcpy(pixels, ceilg, y * pitch);
			SDL_UnlockTexture(gCeilgTexture);

			SDL_Rect dstclg = {i, 0, 1, y};
			SDL_RenderCopy (gRenderer, gCeilgTexture, &rect, &dstclg);
		}

	} */
}

auto Sorcery::Render::draw(
	sf::RenderTarget &target, sf::RenderStates states) const -> void {

	states.transform *= getTransform();

	target.draw(_sprite, states);
}
