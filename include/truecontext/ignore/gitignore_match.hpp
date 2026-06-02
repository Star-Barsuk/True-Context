#pragma once

namespace truecontext::ignore {

/// GitIgnore-style path matching (no negation — applied by Matcher).
[[nodiscard]] bool gitignore_match(
    const char* rel_path,
    const char* pattern,
    bool directory_only,
    bool path_is_dir
);

} // namespace truecontext::ignore
