#pragma once

#include <truecontext/io/stdout_writer.hpp>

namespace truecontext::cli {

/// Renders built-in help and version strings.
class Usage {
public:
    static void print_help(io::StdoutWriter& out);
    static void print_version(io::StdoutWriter& out);
};

} // namespace truecontext::cli
