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

#include "resources/componentstore.hpp"
#include "common/enum.hpp"	   // for Error
#include "common/macro.hpp"	   // for COL2NUM
#include "imgui.h"			   // for ImU32
#include "resources/json.hpp"  // for get_string
#include "types/component.hpp" // for Component
#include "types/enum.hpp"	   // for ComponentType, Font, DrawMode, Justif...
#include "types/error.hpp"	   // for Error, operator<<
#include <algorithm>		   // for __sort_fn, sort
#include <array>			   // for array
#include <charconv>			   // for from_chars
#include <chrono>			   // for operator==
#include <cstdlib>			   // for exit, EXIT_FAILURE, size_t
#include <exception>		   // for exception
#include <filesystem>		   // for path, last_write_time
#include <format>			   // for format
#include <fstream>			   // for basic_ifstream, basic_ostream, ifstream
#include <functional>		   // for less
#include <iostream>			   // for cerr
#include <json/reader.h>	   // for CharReaderBuilder, parseFromStream
#include <json/value.h>		   // for Value
#include <map>				   // for map, operator==
#include <optional>			   // for optional, nullopt, nullopt_t
#include <stdexcept>		   // for out_of_range, runtime_error
#include <string>			   // for basic_string, operator<=>, string
#include <string_view>		   // for basic_string_view, string_view, opera...
#include <system_error>		   // for error_code, errc
#include <utility>			   // for pair, get, move
#include <vector>			   // for vector

namespace {

using ComponentType = Sorcery::Enums::Layout::ComponentType;
using DrawMode = Sorcery::Enums::Layout::DrawMode;
using Font = Sorcery::Enums::Layout::Font;
using Justification = Sorcery::Enums::Layout::Justification;

constexpr std::array COMPONENT_TYPES{
	std::pair{std::string_view{"frame"}, ComponentType::FRAME},
	std::pair{std::string_view{"image_fg"}, ComponentType::IMAGE_FG},
	std::pair{std::string_view{"image_bg"}, ComponentType::IMAGE_BG},
	std::pair{std::string_view{"menu"}, ComponentType::MENU},
	std::pair{std::string_view{"text"}, ComponentType::TEXT},
	std::pair{std::string_view{"button"}, ComponentType::BUTTON},
	std::pair{std::string_view{"paragraph"}, ComponentType::PARAGRAPH},
	std::pair{std::string_view{"other"}, ComponentType::OTHER},
};

constexpr std::array FONTS{
	std::pair{std::string_view{"monospace"}, Font::MONOSPACE},
	std::pair{std::string_view{"proportional"}, Font::PROPORTIONAL},
	std::pair{std::string_view{"text"}, Font::TEXT},
	std::pair{std::string_view{"default"}, Font::DEFAULT},
};

constexpr std::array JUSTIFICATIONS{
	std::pair{std::string_view{"left"}, Justification::LEFT},
	std::pair{std::string_view{"centre"}, Justification::CENTRE},
	std::pair{std::string_view{"right"}, Justification::RIGHT},
};

constexpr std::array DRAW_MODES{
	std::pair{std::string_view{"manual"}, DrawMode::MANUAL},
	std::pair{std::string_view{"automatic"}, DrawMode::AUTOMATIC},
};

template <typename T> auto parse_number(const std::string_view value, const T fallback = {}) -> T {

	if (value.empty())
		return fallback;

	T result{};

	const auto [ptr, ec]{std::from_chars(value.data(), value.data() + value.size(), result)};

	if (ec != std::errc{} || ptr != value.data() + value.size())
		return fallback;

	return result;
}

auto get_position(const Json::Value &component, const std::string_view key) -> int {

	const auto value{Sorcery::JsonHelper::get_string(component, key)};

	if (value == "centre")
		return -1;

	return parse_number<int>(value);
}

template <typename T>
auto get_number(const Json::Value &component, const std::string_view key, const T fallback = {}) -> T {

	return parse_number<T>(Sorcery::JsonHelper::get_string(component, key), fallback);
}

template <typename E, std::size_t N>
auto get_mapped_enum(const Json::Value &component, const std::string_view key,
					 const std::array<std::pair<std::string_view, E>, N> &mapping, const E fallback) -> E {

	const auto value{Sorcery::JsonHelper::get_string(component, key)};

	for (const auto &[name, result] : mapping)
		if (value == name)
			return result;

	return fallback;
}

auto get_colour(const Json::Value &component, const std::string_view key, const std::string_view fallback) -> ImU32 {

	const auto value{Sorcery::JsonHelper::get_string(component, key)};

	if (!value.empty())
		return Sorcery::COL2NUM(value);

	return Sorcery::COL2NUM(std::string{fallback});
}

auto get_components(const std::map<std::string, Sorcery::Component> &components, const std::string_view screen,
					const DrawMode drawmode) -> std::vector<Sorcery::Component> {

	std::vector<Sorcery::Component> results{};

	for (const auto &entry : components) {

		const auto &component{entry.second};

		if (component.form == screen && component.drawmode == drawmode)
			results.emplace_back(component);
	}

	std::ranges::sort(results, {}, &Sorcery::Component::priority);

	return results;
}

} // namespace

Sorcery::ComponentStore::ComponentStore(const std::filesystem::path filename)
	: _loaded{false},
	  _file{filename} {

	_loaded = _load(_file);
}

auto Sorcery::ComponentStore::get(const std::string_view combined_key) -> Component & {

	try {

		if (_need_refresh() && _load(_file))
			_loaded = true;

		if (!_loaded)
			throw std::runtime_error{"ComponentStore is not loaded"};

		const auto key{std::string{combined_key}};

		const auto it{_components.find(key)};

		if (it == _components.end())
			throw std::out_of_range{std::format("Component '{}' not found", combined_key)};

		return it->second;

	} catch (const std::exception &e) {

		Error error{Enums::System::Error::UNKNOWN_COMPONENT, e,
					std::format("Unable to find Component '{}' in layout.json!", combined_key)};

		std::cerr << error;
		std::exit(EXIT_FAILURE);
	}
}

auto Sorcery::ComponentStore::operator()(const std::string_view screen) -> std::optional<std::vector<Component>> {

	if (_need_refresh() && _load(_file))
		_loaded = true;

	if (!_loaded)
		return std::nullopt;

	return get_components(_components, screen, Enums::Layout::DrawMode::AUTOMATIC);
}

auto Sorcery::ComponentStore::get_custom(const std::string_view screen) -> std::optional<std::vector<Component>> {

	if (_need_refresh() && _load(_file))
		_loaded = true;

	if (!_loaded)
		return std::nullopt;

	return get_components(_components, screen, Enums::Layout::DrawMode::MANUAL);
}

auto Sorcery::ComponentStore::_load(const std::filesystem::path filename) -> bool {

	std::ifstream file{filename};

	if (!file)
		return false;

	Json::CharReaderBuilder builder{};
	Json::Value layout{};
	std::string errors{};

	if (!Json::parseFromStream(builder, file, &layout, &errors))
		return false;

	const auto &forms{layout["form"]};

	if (!forms.isArray())
		return false;

	// Build a replacement map rather than modifying the live map.
	// A failed hot reload therefore leaves the last valid UI intact.
	decltype(_components) new_components{};

	for (const auto &form : forms) {

		const auto form_name{JsonHelper::get_string(form, "name")};

		const auto &components{form["component"]};

		if (!components.isArray())
			continue;

		for (const auto &entry : components) {

			const auto name{JsonHelper::get_string(entry, "name")};

			const auto component_type{get_mapped_enum(entry, "type", COMPONENT_TYPES, ComponentType::NO_CT)};

			const auto x{get_position(entry, "x")};

			const auto y{get_position(entry, "y")};

			const auto w{get_number<unsigned int>(entry, "w")};

			const auto h{get_number<unsigned int>(entry, "h")};

			const auto font{get_mapped_enum(entry, "font", FONTS, Font::NO_FONT)};

			const auto colour{get_colour(entry, "colour", "0")};

			const auto animated{JsonHelper::get_string(entry, "animated") == "true"};

			const auto string_key{JsonHelper::get_string(entry, "string")};

			const auto alpha{get_number<float>(entry, "alpha", 0.0f)};

			const auto background{get_colour(entry, "bg_colour", "0x000000ff")};

			const auto justification{get_mapped_enum(entry, "justification", JUSTIFICATIONS, Justification::LEFT)};

			const auto priority{get_number<unsigned int>(entry, "priority", 999u)};

			const auto drawmode{get_mapped_enum(entry, "drawmode", DRAW_MODES, DrawMode::AUTOMATIC)};

			Component component{form_name,	   name,		   x,		 y,			 w,		h,
								font,		   colour,		   animated, string_key, alpha, background,
								justification, component_type, priority, drawmode};

			const auto &data{entry["data"]};

			if (data.isArray() && !data.empty() && data[0].isObject()) {

				const auto &extra_data{data[0]};

				for (const auto &data_key : extra_data.getMemberNames()) {

					component.set(data_key, extra_data[data_key].asString());
				}
			}

			const auto key{std::format("{}:{}", form_name, name)};

			new_components.insert_or_assign(key, std::move(component));
		}
	}

	_components = std::move(new_components);

	_file = filename;
	_last_mod = std::filesystem::last_write_time(_file);

	return true;
}

auto Sorcery::ComponentStore::_need_refresh() -> bool {

	if (_file.empty())
		return false;

	std::error_code error{};

	const auto last_mod{std::filesystem::last_write_time(_file, error)};

	if (error)
		return false;

	return last_mod != _last_mod;
}