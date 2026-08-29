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

#include "core/module.hpp" // for Module
#include <memory>		   // for unique_ptr
namespace Sorcery {
class Reclass;
} // lines 36-36
namespace Sorcery {
class Rename;
} // lines 35-35
namespace Sorcery {
class Rite;
} // lines 37-37
namespace Sorcery {
class Select;
} // lines 34-34
namespace Sorcery {
struct Context;
} // lines 33-33

namespace Sorcery {

class Edit final : public Module {

	public:
		// Standard Constructor
		Edit(Context &ctx);

		// Standard Destructor
		~Edit();

		// Public Methods
		auto start() -> int;
		auto stop() -> int;

	private:
		// Private Methods
		auto _initialise() -> bool;

		// Private Members
		std::unique_ptr<Select> _select;
		std::unique_ptr<Rename> _rename;
		std::unique_ptr<Reclass> _reclass;
		std::unique_ptr<Rite> _rite;
};

};