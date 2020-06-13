# Copyright (C) 2020 Dave Moore
#
# This file is part of Sorcery: Dreams of the Mad Overlord
#
# Sorcery is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 2 of the License, or
# (at your option) any later version.
#
# Sorcery is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with sorcery-sfml.  If not, see <http://www.gnu.org/licenses/>.
# Additional permission under GNU GPL version 3 section 7:
#
# If you modify this Program, or any covered work, by linking or combining it
# with the libraries referred to in README (or a modified version of said
# libraries), containing parts covered by the terms of said libraries, the
# licensors of this Program grant you additional permission to convey the
# resulting work.

MAX_PARALLEL_JOBS := 8
CLEAN_OUTPUT := true
DUMP_ASSEMBLY := false

_CFLAGS_STD := -std=c++20
_CFLAGS_WARNINGS := -Wall -Wextra -Wpedantic -Wunreachable-code -Wunused -Wignored-qualifiers -Wcast-align -Wformat-nonliteral -Wformat=2 -Winvalid-pch -Wmissing-declarations -Wmissing-format-attribute -Wmissing-include-dirs -Wredundant-decls -Wswitch-default -Wodr
_CFLAGS_OTHER := -Wfatal-errors -fdiagnostics-color=always -pthread
CFLAGS := $(_CFLAGS_STD) $(_CFLAGS_WARNINGS) $(_CFLAGS_OTHER)

LINK_LIBRARIES := \
	sfml-graphics \
	sfml-audio \
	sfml-network \
	sfml-window \
	sfml-system \
	tgui \
	sqlite3 \
	sfeMovie \
	jsoncpp

PRODUCTION_FOLDER := build

PRODUCTION_EXCLUDE := \
	*.psd \
	*.rar \
	*.7z \
	Thumbs.db \
	.DS_Store

PRODUCTION_DEPENDENCIES := \
	cfg \
	dat \
	doc \
	gfx


