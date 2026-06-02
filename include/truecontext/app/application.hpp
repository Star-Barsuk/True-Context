#pragma once

#include <truecontext/cli/options.hpp>
#include <truecontext/io/stdout_writer.hpp>

namespace truecontext::app {

/// Top-level entry: parse CLI and dispatch to help, version, or collector.
class Application {
public:
    Application(int argc, char* argv[]) noexcept;

    [[nodiscard]] int run();

private:
    [[nodiscard]] int dispatch(const cli::Options& options);

    int argc_;
    char** argv_;
    io::StdoutWriter stdout_;
    io::StderrWriter stderr_{2};
};

} // namespace truecontext::app
