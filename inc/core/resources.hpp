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

#include <memory>

namespace Sorcery {

struct Context;
class ItemStore;
class LevelStore;
class MonsterStore;
class SpellStore;
class SaveStore;
class Resources {

	public:
		Resources(Context &ctx);
		~Resources();

		std::unique_ptr<ItemStore> items;
		std::unique_ptr<LevelStore> levels;
		std::unique_ptr<MonsterStore> monsters;
		std::unique_ptr<SpellStore> spells;
		std::unique_ptr<SaveStore> saves;

	private:
		Context &_ctx;
};
};
