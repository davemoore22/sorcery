// Copyright (C) 2020 Dave Moore
//
// This file is part of Sorcery: Dreams of the Mad Overlord.
//
// Sorcery: Dreams of the Mad Overlord is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Sorcery: Dreams of the Mad Overlord is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Sorcery: Dreams of the Mad Overlord.  If not, see <http://www.gnu.org/licenses/>.
//
// If you modify this Program, or any covered work, by linking or combining it
// with the libraries referred to in README (or a modified version of said
// libraries), containing parts covered by the terms of said libraries, the
// licensors of this Program grant you additional permission to convey the
// resulting work.

#pragma once

#include "main.hpp"

namespace Sorcery {

	class String {

		public:

			// Standard Constructor
			String(const std::string& filename, const std::string &explain_filename);

			// Standard Destructor
			virtual ~String();

			// Overload [] operator
			auto operator[] (const std::string& key) -> std::string&;

			// Public Methods
			auto get(const std::string& key, const StringType string_type = StringType::NORMAL) -> std::string;
			auto get_explain(const std::string& key) -> std::string;

		private:

			// Private Members
			StringMap _strings; // String Storage Table
			StringMap _explain_strings;
			bool _loaded;

			// Private Methods
			auto _load(const std::string& filename, const StringType string_type = StringType::NORMAL) -> bool;
			auto _replace(std::string& subject, const std::string& search, const std::string& replace) -> void;
	};
}