#include <truecontext/ignore/matcher.hpp>

#include <truecontext/util/cstring.hpp>
#include <truecontext/util/path.hpp>

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <fnmatch.h>

namespace truecontext::ignore {

namespace {

constexpr const char* kDefaultPatterns[] = {
    ".git",
    ".gitignore",
    ".contextignore",
    "collect_context.sh",
    "docs/context.md",
    "context.txt",
    ".idea",
    ".vscode",
    "__pycache__",
    ".ruff_cache",
    "*.pyc",
    "*.pyo",
    "*.swp",
    ".DS_Store",
    ".venv",
    "venv",
    "env",
    "dist",
    "build",
    "bin",
    "*.egg-info",
};

} // namespace

Matcher::Matcher() noexcept {
    add_default_patterns();
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

void Matcher::add_pattern(const char* line) {
    if (line == nullptr || line[0] == '\0') {
        return;
    }

    if (count_ >= capacity_) {
        const std::size_t next = (capacity_ == 0U) ? 32U : capacity_ * 2U;
        Pattern* grown = static_cast<Pattern*>(std::realloc(patterns_, next * sizeof(Pattern)));
        if (grown == nullptr) {
            return;
        }
        patterns_ = grown;
        capacity_ = next;
    }

    char* copy = static_cast<char*>(std::malloc(std::strlen(line) + 1U));
    if (copy == nullptr) {
        return;
    }
    std::strcpy(copy, line);
    patterns_[count_].text = copy;
    ++count_;
}

void Matcher::add_default_patterns() {
    for (const char* pattern : kDefaultPatterns) {
        add_pattern(pattern);
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
            add_pattern(start);
        }
    }

    std::fclose(file);
}

bool Matcher::matches(const char* path, const char* pattern) const {
    if (path == nullptr || pattern == nullptr) {
        return false;
    }

    if (fnmatch(pattern, path, FNM_PATHNAME) == 0) {
        return true;
    }

    char buf[4096];

    if (std::snprintf(buf, sizeof(buf), "*/%s", pattern) > 0) {
        if (fnmatch(buf, path, FNM_PATHNAME) == 0) {
            return true;
        }
    }

    if (std::snprintf(buf, sizeof(buf), "%s/*", pattern) > 0) {
        if (fnmatch(buf, path, FNM_PATHNAME) == 0) {
            return true;
        }
    }

    const char* slash = std::strrchr(path, '/');
    const char* base = (slash != nullptr) ? slash + 1 : path;
    if (fnmatch(pattern, base, 0) == 0) {
        return true;
    }

    return false;
}

bool Matcher::should_ignore(const char* rel_path) const {
    for (std::size_t i = 0U; i < count_; ++i) {
        if (patterns_[i].text != nullptr && matches(rel_path, patterns_[i].text)) {
            return true;
        }
    }
    return false;
}

bool Matcher::should_prune_dir(const char* rel_dir) const {
    if (rel_dir == nullptr || rel_dir[0] == '\0') {
        return false;
    }
    if (should_ignore(rel_dir)) {
        return true;
    }

    const char* base = std::strrchr(rel_dir, '/');
    base = (base != nullptr) ? base + 1 : rel_dir;

    for (std::size_t i = 0U; i < count_; ++i) {
        const char* pattern = patterns_[i].text;
        if (pattern == nullptr) {
            continue;
        }
        if (std::strchr(pattern, '/') == nullptr && std::strchr(pattern, '*') == nullptr) {
            if (util::streq(base, pattern)) {
                return true;
            }
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
