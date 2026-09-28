#include "common/enum.hpp"
#include "core/enum.hpp"
#include "core/controller/inputmode.hpp"
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

template <Sorcery::Enum E> void check(E value, std::string_view name) {
    using namespace Sorcery;
    require(enum_name(value) == name, "enum name changed");
    require(enum_cast<E>(name) == value, "enum string roundtrip failed");
    require(enum_cast<E>(static_cast<std::intmax_t>(value)) == value, "signed enum roundtrip failed");
    if (static_cast<std::intmax_t>(value) >= 0)
        require(enum_cast<E>(static_cast<std::uintmax_t>(value)) == value, "unsigned enum roundtrip failed");
}
}

int main() {
    using namespace Sorcery;
    using namespace Sorcery::Enums;
    // These identifiers are stored in JSON resources and saved games.
    check(Character::Location::NO_LOCATION, "NO_LOCATION");
    check(Character::Class::NINJA, "NINJA");
    check(Character::Status::LOST, "LOST");
    check(Items::TypeID::BROKEN_ITEM, "BROKEN_ITEM");
    check(Items::TypeID::BLUE_RIBBON, "BLUE_RIBBON");
    check(Monsters::TypeID::LVL_7_FIGHTER, "LVL_7_FIGHTER");
    check(Magic::SpellID::NO_SPELL, "NO_SPELL");
    check(Magic::SpellID::MALOR, "MALOR");
    check(Magic::SpellID::MILWA, "MILWA");
    check(Magic::SpellID::MAPORFIC, "MAPORFIC");
    check(Magic::SpellID::MALIKTO, "MALIKTO");
    check(Magic::CastContext::FIELD, "FIELD");
    check(Magic::CastContext::COMBAT, "COMBAT");
    check(Magic::CastContext::TRAPS, "TRAPS");
    check(Input::Mode::ENGINE, "ENGINE");
    check(System::Random::ZERO_TO_437, "ZERO_TO_437");
    check(Screen::CREATE_CONFIRM, "CREATE_CONFIRM");
    require(!enum_cast<System::Error>(5), "undefined gap accepted");
    require(!enum_cast<Character::Class>("ninja"), "lookup must remain case sensitive");
    require(!enum_cast<Character::Class>("UNKNOWN"), "unknown name accepted");
    require(!enum_cast<Character::Class>(-1), "negative class accepted");
    require(enum_name(static_cast<Character::Class>(999)).empty(), "invalid enum must have no name");
    require(!enum_cast<Character::Location>(std::numeric_limits<std::uintmax_t>::max()),
            "unsigned overflow must not alias negative enum");
    require(!enum_cast<Character::Class>(std::numeric_limits<std::intmax_t>::max()),
            "signed overflow accepted");
    std::cout << "Enum resource/save compatibility checks passed\n";
}
