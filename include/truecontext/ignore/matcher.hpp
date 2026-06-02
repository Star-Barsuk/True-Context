#pragma once

#include <cstddef>

#include <truecontext/walk/path_list.hpp>

namespace truecontext::ignore {

/// Built-in self-ignore (`.contextignore`, `docs/context.md`) + project `.contextignore`.
class Matcher {
public:
    Matcher() noexcept;
    ~Matcher();

    Matcher(const Matcher&) = delete;
    Matcher& operator=(const Matcher&) = delete;

    void load_from_project(const char* project_root);

    [[nodiscard]] bool should_ignore(const char* rel_path) const;
    [[nodiscard]] bool should_prune_dir(const char* rel_dir) const;

    [[nodiscard]] walk::PathList filter(const walk::PathList& input) const;

private:
    struct Pattern {
        char* text{nullptr};
        bool negated{false};
        bool directory_only{false};
    };

    Pattern* patterns_{nullptr};
    std::size_t count_{0U};
    std::size_t capacity_{0U};

    void add_pattern_line(const char* line, bool from_user_file);
    void add_builtin_patterns();
    [[nodiscard]] bool path_matches(const char* rel_path, const Pattern& rule) const;
    [[nodiscard]] bool has_negated_under(const char* rel_dir) const;
    void clear_patterns();
};

} // namespace truecontext::ignore
