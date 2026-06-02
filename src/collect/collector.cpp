#include <truecontext/collect/collector.hpp>

#include <truecontext/app/exit_code.hpp>
#include <truecontext/version.hpp>

namespace truecontext::collect {

Collector::Collector(cli::Options options) noexcept : options_{options} {}

int Collector::run(io::StderrWriter& err) {
    err.write_cstr(truecontext::kName);
    err.write_cstr(": collector not implemented yet (root: ");
    err.write_cstr(options_.root_path);
    err.write_line(")");
    return app::kExitError;
}

} // namespace truecontext::collect
