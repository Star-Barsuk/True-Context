#pragma once

#include <cstddef>

#include <truecontext/walk/path_list.hpp>

namespace truecontext::ignore {

/// Default ignores + `.contextignore` (bash-style glob subset).
class Matcher {
public:
    Matcher() noexcept;
    ~Matcher();

    Matcher(const Matcher&) = delete;
    Matcher& operator=(const Matcher&) = delete;

    /// Load `.contextignore` from `project_root` if present.
    void load_from_project(const char* project_root);

    [[nodiscard]] bool should_ignore(const char* rel_path) const;
    [[nodiscard]] bool should_prune_dir(const char* rel_dir) const;

    [[nodiscard]] walk::PathList filter(const walk::PathList& input) const;

private:
    struct Pattern {
        char* text{nullptr};
    };

    Pattern* patterns_{nullptr};
    std::size_t count_{0U};
    std::size_t capacity_{0U};

    void add_pattern(const char* line);
    void add_default_patterns();
    [[nodiscard]] bool matches(const char* path, const char* pattern) const;
    void clear_patterns();
};

} // namespace truecontext::ignore
