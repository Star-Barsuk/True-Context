#include <truecontext/ignore/gitignore_match.hpp>

#include <truecontext/util/cstring.hpp>

#include <cstdio>
#include <cstring>

#include <fnmatch.h>

namespace truecontext::ignore {

namespace {

bool fnmatch_path(const char* pattern, const char* path) {
    return fnmatch(pattern, path, FNM_PATHNAME) == 0;
}

bool fnmatch_anywhere(const char* pattern, const char* path) {
    if (fnmatch_path(pattern, path)) {
        return true;
    }
    for (const char* slash = path; (slash = std::strchr(slash, '/')) != nullptr; ++slash) {
        if (fnmatch_path(pattern, slash + 1)) {
            return true;
        }
    }
    const char* base = std::strrchr(path, '/');
    base = (base != nullptr) ? base + 1 : path;
    return fnmatch(pattern, base, 0) == 0;
}

bool anchored_pattern(const char* pattern) {
    return pattern[0] == '/' || std::strstr(pattern, "/**/") != nullptr ||
           std::strncmp(pattern, "**/", 3) == 0;
}

bool match_directory_only(const char* path, const char* pattern, bool path_is_dir) {
    if (fnmatch_path(pattern, path)) {
        return path_is_dir;
    }

    const std::size_t plen = std::strlen(pattern);
    if (plen == 0U) {
        return false;
    }

    if (std::strncmp(path, pattern, plen) == 0 && path[plen] == '/') {
        return true;
    }

    char scoped[4096];
    if (std::snprintf(scoped, sizeof(scoped), "*/%s", pattern) > 0) {
        if (std::strncmp(path, scoped + 2, plen) == 0 && path[plen] == '/') {
            return true;
        }
        if (fnmatch_path(scoped, path)) {
            return true;
        }
    }

    if (std::snprintf(scoped, sizeof(scoped), "*/%s/*", pattern) > 0) {
        if (fnmatch_path(scoped, path)) {
            return true;
        }
    }

    return fnmatch_anywhere(pattern, path);
}

bool match_double_star(const char* path, const char* pattern) {
    if (util::streq(pattern, "**")) {
        return true;
    }

    const char* star = std::strstr(pattern, "**");
    if (star == nullptr) {
        return false;
    }

    char left[4096];
    char right[4096];
    std::size_t left_len = static_cast<std::size_t>(star - pattern);
    while (left_len > 0U && pattern[left_len - 1U] == '/') {
        --left_len;
    }
    if (left_len >= sizeof(left)) {
        return false;
    }
    std::memcpy(left, pattern, left_len);
    left[left_len] = '\0';

    const char* right_start = star + 2;
    if (*right_start == '/') {
        ++right_start;
    }
    // `tests/**` → suffix after `**` is `*` or empty → match whole subtree
    if (right_start[0] == '*' && right_start[1] == '\0') {
        right_start = "";
    }
    std::strncpy(right, right_start, sizeof(right) - 1U);
    right[sizeof(right) - 1U] = '\0';

    if (left[0] == '\0') {
        if (right[0] == '\0') {
            return true;
        }
        return fnmatch_anywhere(right, path);
    }

    if (right[0] == '\0') {
        if (std::strncmp(path, left, left_len) == 0 &&
            (path[left_len] == '\0' || path[left_len] == '/')) {
            return true;
        }
        char left_any[4096];
        if (std::snprintf(left_any, sizeof(left_any), "*/%s", left) > 0) {
            const std::size_t any_len = std::strlen(left_any + 2);
            if (std::strncmp(path, left_any + 2, any_len) == 0 &&
                (path[any_len] == '\0' || path[any_len] == '/')) {
                return true;
            }
        }
        return false;
    }

    if (!fnmatch_anywhere(left, path)) {
        return false;
    }

    const char* cursor = path;
    while (cursor != nullptr) {
        if (fnmatch_anywhere(right, cursor)) {
            return true;
        }
        cursor = std::strchr(cursor, '/');
        if (cursor != nullptr) {
            ++cursor;
        }
    }
    return false;
}

} // namespace

bool gitignore_match(
    const char* rel_path,
    const char* pattern,
    bool directory_only,
    bool path_is_dir
) {
    if (rel_path == nullptr || pattern == nullptr || pattern[0] == '\0') {
        return false;
    }

    if (directory_only) {
        return match_directory_only(rel_path, pattern, path_is_dir);
    }

    if (std::strstr(pattern, "**") != nullptr) {
        return match_double_star(rel_path, pattern);
    }

    if (pattern[0] == '/') {
        return fnmatch_path(pattern + 1, rel_path);
    }

    if (anchored_pattern(pattern)) {
        return fnmatch_path(pattern, rel_path);
    }

    return fnmatch_anywhere(pattern, rel_path);
}

} // namespace truecontext::ignore
