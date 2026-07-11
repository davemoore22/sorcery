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

#pragma once

#include <array>
#include <cassert>
#include <cstddef>

namespace Sorcery {

template <typename T, std::size_t Rows, std::size_t Columns> class Grid {
	public:
		using row_type = std::array<T, Columns>;
		using storage_type = std::array<row_type, Rows>;

		constexpr Grid() = default;

		constexpr explicit Grid(const storage_type &values)
			: _data{values} {}

		[[nodiscard]] constexpr auto
		operator[](const std::size_t row, const std::size_t column) noexcept
			-> T & {

			assert(row < Rows);
			assert(column < Columns);

			return _data[row][column];
		}

		[[nodiscard]] constexpr auto
		operator[](const std::size_t row,
				   const std::size_t column) const noexcept -> const T & {

			assert(row < Rows);
			assert(column < Columns);

			return _data[row][column];
		}

	private:
		storage_type _data{};
};

}