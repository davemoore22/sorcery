#pragma once

#include <stdexcept>

namespace Sorcery::Test {

class AssertionFailure final : public std::runtime_error {
	public:
		using std::runtime_error::runtime_error;
};

inline auto require(const bool condition, const char *message) -> void {
	if (!condition)
		throw AssertionFailure{message};
}

}
