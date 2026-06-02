#pragma once

#include <truecontext/cli/options.hpp>
#include <truecontext/io/stdout_writer.hpp>

namespace truecontext::collect {

/// Runs the collect pipeline: walk → filter → sort → render.
class Collector {
public:
    explicit Collector(cli::Options options) noexcept;

    /// Returns a process exit code (`app::kExit*`).
    [[nodiscard]] int run(io::StderrWriter& err);

private:
    cli::Options options_;
};

} // namespace truecontext::collect
