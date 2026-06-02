#pragma once

#include <cstring>

namespace truecontext::util {

[[nodiscard]] inline bool streq(const char* a, const char* b) noexcept {
    return std::strcmp(a, b) == 0;
}

[[nodiscard]] inline bool starts_with(const char* s, char c) noexcept {
    return s != nullptr && s[0] == c;
}

} // namespace truecontext::util
