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

#include "display/display.hpp"
#include "backends/imgui_impl_opengl3.h" // for ImGui_ImplOpenGL3_RenderDra...
#include "common/types.hpp"				 // for Size
#include "core/context.hpp"				 // for Context
#include "core/debug.hpp"				 // for DEBUG_LOGF, debug_logf
#include "display/framebuffer.hpp"		 // for FrameBuffer
#include <SDL2/SDL.h>					 // for SDL_INIT_GAMECONTROLLER
#include <SDL2/SDL_video.h>				 // for SDL_GLattr, SDL_GL_SetAttri...
#include <SDL_error.h>					 // for SDL_GetError
#include <algorithm>					 // for clamp, min
#include <print>						 // for println
#include <stdexcept>					 // for runtime_error
#include <string>						 // for basic_string, stoi, operator+

struct ImDrawData; // Global Namespace Forward Declaration

constexpr auto vertex_shader{R"(
	#version 330 core

	out vec2 uv;

	void main() {

		vec2 pos = vec2(
			(gl_VertexID << 1) & 2,
			gl_VertexID & 2);

		uv = pos;

		gl_Position = vec4(
			pos * 2.0 - 1.0,
			0.0,
			1.0);
	}
)"};

constexpr auto fragment_shader{R"(
	#version 330 core

	in vec2 uv;

	out vec4 frag_colour;

	uniform sampler2D screen_texture;
	uniform float fade;

	void main() {

		vec4 colour = texture(screen_texture, uv);

		colour.rgb = mix(
			colour.rgb,
			vec3(0.0),
			fade);

		frag_colour = colour;
	}
)"};

Sorcery::Display::Display(Context &ctx)
	: _ctx{ctx} {

	_initialise_SDL();
};

auto Sorcery::Display::_initialise_SDL() -> int {

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) !=
		0) {

		std::println("SDL initialisation failed: {}", SDL_GetError());
		return -1;
	}

	_GLSL_version = "#version 130";

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
						SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

	const auto window_title{_ctx.get_string("WINDOW_TITLE")};

	_base_window_w = std::stoi(_ctx.get_config("Window", "width"));

	_base_window_h = std::stoi(_ctx.get_config("Window", "height"));

	const auto min_window_w{std::stoi(_ctx.get_config("Window", "min_width"))};

	const auto min_window_h{std::stoi(_ctx.get_config("Window", "min_height"))};

	_SDL_window_flags = static_cast<SDL_WindowFlags>(
		SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE);

	_SDL_window = SDL_CreateWindow(window_title.c_str(), SDL_WINDOWPOS_CENTERED,
								   SDL_WINDOWPOS_CENTERED, _base_window_w,
								   _base_window_h, _SDL_window_flags);

	if (_SDL_window == nullptr) {
		std::println("SDL window creation failed: {}", SDL_GetError());

		return -1;
	}

	SDL_SetWindowMinimumSize(_SDL_window, min_window_w, min_window_h);

	_GL_context = SDL_GL_CreateContext(_SDL_window);

	if (_GL_context == nullptr) {
		std::println("OpenGL context creation failed: {}", SDL_GetError());

		SDL_DestroyWindow(_SDL_window);
		_SDL_window = nullptr;

		return -1;
	}

	if (SDL_GL_MakeCurrent(_SDL_window, _GL_context) != 0) {
		std::println("Making OpenGL context current failed: {}",
					 SDL_GetError());

		SDL_GL_DeleteContext(_GL_context);
		_GL_context = nullptr;

		SDL_DestroyWindow(_SDL_window);
		_SDL_window = nullptr;

		return -1;
	}

	glewExperimental = GL_TRUE;

	const auto glew_status{glewInit()};
	if (glew_status != GLEW_OK) {
		std::println(
			"GLEW initialisation failed: {}",
			reinterpret_cast<const char *>(glewGetErrorString(glew_status)));

		return -1;
	}

	if (SDL_GL_SetSwapInterval(1) != 0) {
		std::println("Warning: unable to enable VSync: {}", SDL_GetError());
	}

	update_display_metrics();

	_framebuffer.create(_metrics.drawable_w, _metrics.drawable_h);

	_create_post_processor();

	int gl_major{};
	int gl_minor{};
	int gl_profile{};

	SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &gl_major);

	SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &gl_minor);

	SDL_GL_GetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, &gl_profile);

	// DEBUG_LOGF("OpenGL context: {}.{}, profile mask: {}", gl_major, gl_minor,
	//		   gl_profile);

	return 1;
}
auto Sorcery::Display::get_SDL_window() -> SDL_Window * {

	return _SDL_window;
}

auto Sorcery::Display::get_SDL_window_size() -> Size {

	int w{0}, h{0};
	SDL_GetWindowSize(_SDL_window, &w, &h);
	return Size{w, h};
}

auto Sorcery::Display::get_GL_context() -> SDL_GLContext {

	return _GL_context;
}

auto Sorcery::Display::get_GLSL_version() const -> const char * {

	return _GLSL_version.c_str();
}

auto Sorcery::Display::get_display_metrics() const noexcept
	-> const DisplayMetrics & {

	return _metrics;
}

auto Sorcery::Display::update_display_metrics() noexcept -> void {

	if (_SDL_window == nullptr)
		return;

	SDL_GetWindowSize(_SDL_window, &_metrics.window_w, &_metrics.window_h);

	SDL_GL_GetDrawableSize(_SDL_window, &_metrics.drawable_w,
						   &_metrics.drawable_h);

	if (_metrics.window_w <= 0 || _metrics.window_h <= 0)
		return;

	_metrics.framebuffer_scale_x = static_cast<float>(_metrics.drawable_w) /
								   static_cast<float>(_metrics.window_w);

	_metrics.framebuffer_scale_y = static_cast<float>(_metrics.drawable_h) /
								   static_cast<float>(_metrics.window_h);

	_metrics.scale_x = static_cast<float>(_metrics.window_w) /
					   static_cast<float>(_base_window_w);

	_metrics.scale_y = static_cast<float>(_metrics.window_h) /
					   static_cast<float>(_base_window_h);

	_metrics.scale = std::min(_metrics.scale_x, _metrics.scale_y);

	const auto content_w{static_cast<float>(_base_window_w) * _metrics.scale};

	const auto content_h{static_cast<float>(_base_window_h) * _metrics.scale};

	_metrics.offset_x =
		(static_cast<float>(_metrics.window_w) - content_w) / 2.0f;

	_metrics.offset_y =
		(static_cast<float>(_metrics.window_h) - content_h) / 2.0f;

	// DEBUG_LOGF("window={}x{} drawable={}x{} fbscale={}x{}",
	// _metrics.window_w, 		   _metrics.window_h, _metrics.drawable_w,
	//_metrics.drawable_h, 		   _metrics.framebuffer_scale_x,
	//_metrics.framebuffer_scale_y);
}

auto Sorcery::Display::resize() -> void {

	update_display_metrics();

	if (_metrics.drawable_w <= 0 || _metrics.drawable_h <= 0)
		return;

	_framebuffer.resize(_metrics.drawable_w, _metrics.drawable_h);
}

auto Sorcery::Display::present(ImDrawData *draw_data) -> void {

	const auto width{_metrics.drawable_w};
	const auto height{_metrics.drawable_h};

	if (width <= 0 || height <= 0)
		return;

	if (_framebuffer.width() != width || _framebuffer.height() != height) {
		_framebuffer.resize(width, height);
	}

	// Pass 1: Render ImGui into the offscreen framebuffer.
	_framebuffer.bind();

	glViewport(0, 0, width, height);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT);

	ImGui_ImplOpenGL3_RenderDrawData(draw_data);

	// Pass 2: Render framebuffer texture to the window throughthe
	// post-processing shader.

	FrameBuffer::unbind();

	glViewport(0, 0, width, height);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT);

	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_SCISSOR_TEST);

	glUseProgram(_post_program);

	glActiveTexture(GL_TEXTURE0);

	glBindTexture(GL_TEXTURE_2D, _framebuffer.texture());

	glUniform1i(_screen_texture_location, 0);

	glUniform1f(_fade_location, _fade);

	glBindVertexArray(_post_vao);

	glDrawArrays(GL_TRIANGLES, 0, 3);

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);

	SDL_GL_SwapWindow(_SDL_window);

	// std::println("present: drawable={}x{} framebuffer={}x{}",
	//			 _metrics.drawable_w, _metrics.drawable_h, _framebuffer.width(),
	//			 _framebuffer.height());
}

auto Sorcery::Display::set_fade(const float fade) -> void {

	_fade = std::clamp(fade, 0.0f, 1.0f);
}

auto Sorcery::Display::_compile_shader(const GLenum type, const char *source)
	-> GLuint {

	const auto shader{glCreateShader(type)};

	glShaderSource(shader, 1, &source, nullptr);
	glCompileShader(shader);

	GLint success{};
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	if (!success) {
		GLchar log[1024]{};

		glGetShaderInfoLog(shader, sizeof(log), nullptr, log);

		glDeleteShader(shader);

		throw std::runtime_error{std::string{"Shader compilation failed: "} +
								 log};
	}

	return shader;
}

auto Sorcery::Display::_create_post_processor() -> void {

	const auto vertex{_compile_shader(GL_VERTEX_SHADER, vertex_shader)};

	const auto fragment{_compile_shader(GL_FRAGMENT_SHADER, fragment_shader)};

	_post_program = glCreateProgram();

	glAttachShader(_post_program, vertex);
	glAttachShader(_post_program, fragment);

	glLinkProgram(_post_program);

	glDeleteShader(vertex);
	glDeleteShader(fragment);

	GLint success{};
	glGetProgramiv(_post_program, GL_LINK_STATUS, &success);

	if (!success) {
		GLchar log[1024]{};

		glGetProgramInfoLog(_post_program, sizeof(log), nullptr, log);

		throw std::runtime_error{std::string{"Shader link failed: "} + log};
	}

	glGenVertexArrays(1, &_post_vao);

	_screen_texture_location =
		glGetUniformLocation(_post_program, "screen_texture");

	_fade_location = glGetUniformLocation(_post_program, "fade");

	if (_screen_texture_location == -1 || _fade_location == -1) {

		throw std::runtime_error{
			"Unable to find post-processing shader uniforms."};
	}
}