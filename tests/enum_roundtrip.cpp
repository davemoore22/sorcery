#include "common/enum.hpp"
#include "core/controller/inputmode.hpp"
#include "core/enum.hpp"
#include "engine/enum.hpp"
#include "magic/enum.hpp"
#include "types/enum.hpp"
#include "types/meta.hpp"
#include <iostream>
#include <limits>
#include <stdexcept>

namespace {
void require(bool condition, const char *message) {
	if (!condition)
		throw std::runtime_error(message);
}

template <Sorcery::Enum E> void check(E value, std::string_view name, std::intmax_t saved_id) {
	using namespace Sorcery;
	require(static_cast<std::intmax_t>(value) == saved_id, "persisted enum ID changed");
	require(enum_name(value) == name, "enum name changed");
	require(enum_cast<E>(name) == value, "enum string roundtrip failed");
	require(enum_cast<E>(saved_id) == value, "signed enum roundtrip failed");
	if (saved_id >= 0)
		require(enum_cast<E>(static_cast<std::uintmax_t>(saved_id)) == value, "unsigned enum roundtrip failed");
}
}

int main() {
	using namespace Sorcery;
	using namespace Sorcery::Enums;
	// Expected names and IDs are independent literals, not round-trip-derived.
#include "fixtures/enum_contract.inc"
	require(!enum_cast<System::Error>(5), "undefined gap accepted");
	require(!enum_cast<Character::Class>("ninja"), "lookup must remain case sensitive");
	require(!enum_cast<Character::Class>("UNKNOWN"), "unknown name accepted");
	require(!enum_cast<Character::Class>(-1), "negative class accepted");
	require(enum_name(static_cast<Character::Class>(999)).empty(), "invalid enum must have no name");
	require(!enum_cast<Character::Location>(std::numeric_limits<std::uintmax_t>::max()),
			"unsigned overflow must not alias negative enum");
	require(!enum_cast<Character::Class>(std::numeric_limits<std::intmax_t>::max()), "signed overflow accepted");
	std::cout << "Enum resource/save compatibility checks passed\n";
}
