#include <truecontext/cli/usage.hpp>

#include <truecontext/version.hpp>

namespace truecontext::cli {

void Usage::print_help(io::StdoutWriter& out) {
    out.write_line("Usage: true_context [PATH] [OUTPUT.md]");
    out.write_line("       true_context --version");
    out.write_line("       true_context --help");
    out.write_line("");
    out.write_line("Collect project files into a markdown context file.");
    out.write_line("PATH        Root directory to scan (default: .)");
    out.write_line("OUTPUT.md   Output file (default: docs/context.md)");
    out.write_line("");
    out.write_line("See PROJECT_GOALS.md and docs/ARCHITECTURE.md.");
}

void Usage::print_version(io::StdoutWriter& out) {
    out.write_cstr(truecontext::kName);
    out.write_cstr(" ");
    out.write_line(truecontext::kVersion);
    out.write_line("(collector in progress)");
}

} // namespace truecontext::cli
