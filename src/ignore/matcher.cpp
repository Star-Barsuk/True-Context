#include <truecontext/ignore/matcher.hpp>

#include <truecontext/ignore/gitignore_match.hpp>
#include <truecontext/util/cstring.hpp>
#include <truecontext/util/path.hpp>

#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace truecontext::ignore {

namespace {

/// Always applied — collector must not include its rule file or default output.
constexpr const char* kBuiltinPatterns[] = {
    ".contextignore",
    "docs/context.md",
};

} // namespace

Matcher::Matcher() noexcept {
    add_builtin_patterns();
}

Matcher::~Matcher() {
    clear_patterns();
}

void Matcher::clear_patterns() {
    for (std::size_t i = 0U; i < count_; ++i) {
        std::free(patterns_[i].text);
        patterns_[i].text = nullptr;
    }
    std::free(patterns_);
    patterns_ = nullptr;
    count_ = 0U;
    capacity_ = 0U;
}

void Matcher::add_pattern_line(const char* line, bool /*from_user_file*/) {
    if (line == nullptr || line[0] == '\0') {
        return;
    }

    bool negated = false;
    const char* pattern = line;
    if (pattern[0] == '!') {
        negated = true;
        ++pattern;
        if (pattern[0] == '\0') {
            return;
        }
    }

    bool directory_only = false;
    const std::size_t plen = std::strlen(pattern);
    if (plen > 0U && pattern[plen - 1U] == '/') {
        directory_only = true;
    }

    char* copy = static_cast<char*>(std::malloc(plen + 1U));
    if (copy == nullptr) {
        return;
    }
    std::memcpy(copy, pattern, plen + 1U);
    if (directory_only) {
        copy[plen - 1U] = '\0';
    }

    if (count_ >= capacity_) {
        const std::size_t next = (capacity_ == 0U) ? 32U : capacity_ * 2U;
        Pattern* grown = static_cast<Pattern*>(std::realloc(patterns_, next * sizeof(Pattern)));
        if (grown == nullptr) {
            std::free(copy);
            return;
        }
        patterns_ = grown;
        capacity_ = next;
    }

    patterns_[count_].text = copy;
    patterns_[count_].negated = negated;
    patterns_[count_].directory_only = directory_only;
    ++count_;
}

void Matcher::add_builtin_patterns() {
    for (const char* pattern : kBuiltinPatterns) {
        add_pattern_line(pattern, false);
    }
}

void Matcher::load_from_project(const char* project_root) {
    if (project_root == nullptr) {
        return;
    }

    char ignore_path[4096];
    if (!util::path_join(ignore_path, sizeof(ignore_path), project_root, ".contextignore")) {
        return;
    }

    FILE* file = std::fopen(ignore_path, "r");
    if (file == nullptr) {
        return;
    }

    char line[4096];
    while (std::fgets(line, sizeof(line), file) != nullptr) {
        char* start = line;
        while (*start == ' ' || *start == '\t') {
            ++start;
        }
        if (*start == '\0' || *start == '#' || *start == '\n') {
            continue;
        }
        char* end = start + std::strlen(start);
        while (end > start && (end[-1] == '\n' || end[-1] == '\r' || end[-1] == ' ')) {
            --end;
        }
        *end = '\0';
        if (*start != '\0') {
            add_pattern_line(start, true);
        }
    }

    std::fclose(file);
}

bool Matcher::path_matches(const char* rel_path, const Pattern& rule) const {
    if (rule.text == nullptr) {
        return false;
    }
    return gitignore_match(rel_path, rule.text, rule.directory_only, false);
}

bool Matcher::should_ignore(const char* rel_path) const {
    if (rel_path == nullptr || rel_path[0] == '\0') {
        return false;
    }

    bool ignored = false;
    for (std::size_t i = 0U; i < count_; ++i) {
        if (path_matches(rel_path, patterns_[i])) {
            ignored = !patterns_[i].negated;
        }
    }
    return ignored;
}

bool Matcher::has_negated_under(const char* rel_dir) const {
    if (rel_dir == nullptr || rel_dir[0] == '\0') {
        return false;
    }

    char prefix[4096];
    if (std::snprintf(prefix, sizeof(prefix), "%s/", rel_dir) <= 0) {
        return false;
    }
    const std::size_t prefix_len = std::strlen(prefix);

    for (std::size_t i = 0U; i < count_; ++i) {
        if (!patterns_[i].negated || patterns_[i].text == nullptr) {
            continue;
        }
        const char* pattern = patterns_[i].text;
        if (std::strncmp(pattern, prefix, prefix_len) == 0) {
            return true;
        }
        if (gitignore_match(prefix, pattern, patterns_[i].directory_only, true)) {
            return true;
        }
    }
    return false;
}

bool Matcher::should_prune_dir(const char* rel_dir) const {
    if (rel_dir == nullptr || rel_dir[0] == '\0') {
        return false;
    }
    if (has_negated_under(rel_dir)) {
        return false;
    }

    if (should_ignore(rel_dir)) {
        return true;
    }

    for (std::size_t i = 0U; i < count_; ++i) {
        if (patterns_[i].negated) {
            continue;
        }
        if (!patterns_[i].directory_only) {
            continue;
        }
        if (gitignore_match(rel_dir, patterns_[i].text, true, true)) {
            return true;
        }
    }

    const char* base = std::strrchr(rel_dir, '/');
    base = (base != nullptr) ? base + 1 : rel_dir;

    for (std::size_t i = 0U; i < count_; ++i) {
        if (patterns_[i].negated || patterns_[i].directory_only) {
            continue;
        }
        if (patterns_[i].text == nullptr) {
            continue;
        }
        if (std::strchr(patterns_[i].text, '/') == nullptr &&
            std::strchr(patterns_[i].text, '*') == nullptr && util::streq(base, patterns_[i].text)) {
            return true;
        }
    }

    return false;
}

walk::PathList Matcher::filter(const walk::PathList& input) const {
    walk::PathList out;
    for (std::size_t i = 0U; i < input.size(); ++i) {
        const char* path = input.get(i);
        if (path != nullptr && !should_ignore(path)) {
            out.push_copy(path);
        }
    }
    return out;
}

} // namespace truecontext::ignore
