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

// Enums
namespace Sorcery {

namespace Enums::Castle {

	namespace Rest {

		enum class Mode {
			SINGLE,
			ALL
		};

		enum class Stage {
			REGEN,
			RESULTS
		};

		enum class Type {
			STABLES,
			COT,
			ECONOMY,
			MERCHANT,
			ROYAL
		};

	}

	enum class Tavern {
		NO_STAGE,
		MENU,
		ADD,
		REMOVE,
		REORDER,
		INSPECT
	};

	enum class Inn {
		NO_STAGE,
		MENU,
		BED,
		INPECT,
		CHOOSE
	};

	enum class Shop {
		NO_STAGE,
		MENU,
		WHO,
		ACTION,
		UNCURSE,
		IDENTIFY,
		INSPECT
	};

	enum class Temple {
		NO_STAGE,
		MENU,
		HELP,
		PAY,
		RESS,
		TITHE,
		INSPECT
	};

	enum class Roster {
		NO_MODE,
		INSPECT,
		EDIT,
		DELETE
	};
}

using InnStage = Enums::Castle::Inn;
using RestMode = Enums::Castle::Rest::Mode;
using RestStage = Enums::Castle::Rest::Stage;
using RestType = Enums::Castle::Rest::Type;
using RosterMode = Enums::Castle::Roster;
using ShopStage = Enums::Castle::Shop;
using TavernStage = Enums::Castle::Tavern;
using TempleStage = Enums::Castle::Temple;

}