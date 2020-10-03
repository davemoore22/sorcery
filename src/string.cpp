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

#include "string.hpp"

// This is the Game Message Handling Class

// Standard Constructor
Sorcery::String::String(const std::string &filename, const std::string &explain_filename) {

	// Load strings from file
	const bool loadresult = {_load(filename, StringType::NORMAL)};
	const bool explain_loadresult = {_load(explain_filename, StringType::EXPLAIN)};

	// Check result
	_loaded = (loadresult) && (explain_loadresult);

}

// Standard Destructor
Sorcery::String::~String() {
}

// Overload [] Operator
auto Sorcery::String::operator[] (const std::string &key) -> std::string& {
	return _loaded ? _strings[key] : _strings["NONE"];
}

// Load File into Game Strings
auto Sorcery::String::_load(const std::string &filename, const StringType string_type) -> bool {

	// Work out the destination, but load an empty string anyway in case of error
	StringMap* string_set {string_type == StringType::NORMAL ? &_strings : &_explain_strings};
	string_set->clear();
	(*string_set)["NONE"] = STRINGS_NOT_LOADED;

	// Attempt to load Strings File
	if (std::ifstream strings_file {filename, std::ifstream::binary}; strings_file.good()) {

		// Iterate through the file
		Json::Value root{};
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
		Json::Reader reader {};
#pragma GCC diagnostic pop
		Json::StreamWriterBuilder builder {};
		builder.settings_["indentation"] = "";
		if (reader.parse(strings_file, root, false)) {
			for (Json::Value::iterator it = root.begin(); it !=root.end(); ++it) {
				Json::Value key {it.key()};
				Json::Value value {(*it)};
				std::string string_key {Json::writeString(builder, key)};
				std::string string_value {Json::writeString(builder, value)};

				// Remove Special Characters from file
				string_key.erase(remove(string_key.begin(), string_key.end(), '\"' ), string_key.end());
				string_value.erase(remove(string_value.begin(), string_value.end(), '\"' ), string_value.end());
				string_key.erase(remove(string_key.begin(), string_key.end(), '\n' ), string_key.end());

				// Insert it into the map
				(*string_set)[string_key] = string_value;
			}
		} else
			return false;
	} else
		return false;

	return true;
}

// Get Text
auto Sorcery::String::get(const std::string& key, const StringType string_type) -> std::string {
	if (_loaded) {
		const StringMap* string_set {string_type == StringType::NORMAL ? &_strings : &_explain_strings};
		return string_set->find(key) != string_set->end() ? string_set->at(key) : KEY_NOT_FOUND;
	} else
		return STRINGS_NOT_LOADED;
}

// Get Explain Text
auto Sorcery::String::get_explain(const std::string& key) -> std::string {
	if (_loaded)
		return _explain_strings.find(key) != _explain_strings.end() ? _explain_strings.at(key) : EXPLAIN_KEY_NOT_FOUND;
	else
		return STRINGS_NOT_LOADED;
}
// Utility function due to lack of std::string::replace_with_substring
auto Sorcery::String::_replace(std::string& subject, const std::string& search, const std::string& replace) -> void {
	size_t pos {0};
	while((pos = subject.find(search, pos)) != std::string::npos) {
		subject.replace(pos, search.length(), replace);
		pos += replace.length();
	}
}
