// Copyright (C) 2021 Dave Moore
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

#include "backward.hpp"
#include <iostream>

auto main(int argc, char *argv[]) -> int {

	std::cout << "Test Backtrace!" << std::endl << std::endl;

	backward::StackTrace st;
	st.load_here(32);
	backward::TraceResolver tr;
	tr.load_stacktrace(st);

	for (size_t i = 0; i < st.size(); ++i) {

		std::cout << i << std::endl;

		backward::ResolvedTrace trace = tr.resolve(st[i]);

		std::cout << "#" << i << " " << trace.object_filename << " " << trace.object_function
				  << " [" << trace.addr << "]" << std::endl;
	}

	// Quit
	return -1;
}