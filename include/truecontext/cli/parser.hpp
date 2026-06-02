#pragma once

#include <truecontext/cli/options.hpp>

namespace truecontext::cli {

/// Converts `argc`/`argv` into `Options` without heap allocation.
class Parser {
public:
    [[nodiscard]] static Options parse(int argc, char* argv[]);
};

} // namespace truecontext::cli
