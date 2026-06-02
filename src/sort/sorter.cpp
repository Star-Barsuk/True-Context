#include <truecontext/sort/sorter.hpp>

#include <cstdlib>
#include <cstring>

#include <fnmatch.h>

namespace truecontext::sort {

int Sorter::priority(const char* path) noexcept {
    if (path == nullptr) {
        return 99;
    }
    if (fnmatch("README*", path, FNM_PATHNAME) == 0) {
        return 1;
    }
    if (fnmatch("PROJECT_GOALS.md", path, FNM_PATHNAME) == 0) {
        return 1;
    }
    if (std::strncmp(path, "docs/", 5) == 0) {
        return 2;
    }
    if (std::strncmp(path, "config/", 7) == 0) {
        return 3;
    }
    if (std::strncmp(path, "include/", 8) == 0) {
        return 4;
    }
    if (std::strncmp(path, "src/", 4) == 0) {
        return 5;
    }
    if (std::strncmp(path, "tests/", 6) == 0) {
        return 6;
    }
    return 10;
}

int Sorter::compare_paths(const void* a, const void* b) {
    const char* const* left = static_cast<const char* const*>(a);
    const char* const* right = static_cast<const char* const*>(b);
    const char* pa = (left != nullptr) ? *left : "";
    const char* pb = (right != nullptr) ? *right : "";

    const int ra = priority(pa);
    const int rb = priority(pb);
    if (ra != rb) {
        return (ra < rb) ? -1 : 1;
    }
    return std::strcmp(pa, pb);
}

void Sorter::sort(walk::PathList& paths) {
    if (paths.size() <= 1U) {
        return;
    }
    std::qsort(paths.data(), paths.size(), sizeof(char*), compare_paths);
}

} // namespace truecontext::sort
