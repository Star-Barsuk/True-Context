#pragma once

#include <truecontext/walk/path_list.hpp>

namespace truecontext::sort {

/// Sorts paths in-place (README → config → src → tests → rest).
class Sorter {
public:
    static void sort(walk::PathList& paths);

private:
    [[nodiscard]] static int priority(const char* path) noexcept;
    static int compare_paths(const void* a, const void* b);
};

} // namespace truecontext::sort
