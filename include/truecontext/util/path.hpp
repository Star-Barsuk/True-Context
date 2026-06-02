#pragma once

#include <cstddef>

namespace truecontext::util {

[[nodiscard]] bool path_join(
    char* out,
    std::size_t out_size,
    const char* base,
    const char* relative
);

/// Creates parent directories for `file_path` (e.g. docs/context.md).
[[nodiscard]] bool ensure_parent_dirs(const char* file_path);

[[nodiscard]] bool is_binary_content(const unsigned char* data, std::size_t len) noexcept;

} // namespace truecontext::util
