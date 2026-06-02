#pragma once

#include <cstdio>

#include <truecontext/walk/path_list.hpp>

namespace truecontext::render {

/// Builds a directory tree from relative paths and prints ASCII tree lines.
class TreeRenderer {
public:
    static void write(FILE* out, const walk::PathList& files);
};

} // namespace truecontext::render
