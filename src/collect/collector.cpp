#include <truecontext/collect/collector.hpp>

#include <truecontext/app/exit_code.hpp>
#include <truecontext/collect/config.hpp>
#include <truecontext/render/renderer.hpp>
#include <truecontext/sort/sorter.hpp>
#include <truecontext/util/path.hpp>
#include <truecontext/version.hpp>
#include <truecontext/walk/walker.hpp>

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <unistd.h>

namespace truecontext::collect {

namespace {

bool resolve_output_path(const char* output_rel, char* out, std::size_t out_size) {
    char cwd[4096];
    if (getcwd(cwd, sizeof(cwd)) == nullptr) {
        return false;
    }
    return util::path_join(out, out_size, cwd, output_rel);
}

} // namespace

Collector::Collector(cli::Options options) noexcept : options_{options} {}

int Collector::run(io::StderrWriter& err, io::StdoutWriter& out) {
    char root_abs[4096];
    if (realpath(options_.root_path, root_abs) == nullptr) {
        err.write_cstr(truecontext::kName);
        err.write_cstr(": cannot resolve root path: ");
        err.write_line(options_.root_path);
        return app::kExitError;
    }

    const char* output_rel =
        (options_.output_path != nullptr) ? options_.output_path : kDefaultOutputPath;

    char output_abs[4096];
    if (!resolve_output_path(output_rel, output_abs, sizeof(output_abs))) {
        err.write_cstr(truecontext::kName);
        err.write_line(": cannot resolve output path");
        return app::kExitError;
    }

    walk::Walker walker{root_abs};
    walk::PathList files = walker.collect();
    sort::Sorter::sort(files);

    render::Renderer renderer{root_abs, output_abs};
    if (!renderer.render(files)) {
        err.write_cstr(truecontext::kName);
        err.write_line(": failed to write context file");
        return app::kExitError;
    }

    out.write_line("Context written:");
    out.write_line(output_abs);

    char summary[128];
    const int n = std::snprintf(summary, sizeof(summary), "   (%zu files)", files.size());
    if (n > 0) {
        out.write_line(summary);
    }

    return app::kExitOk;
}

} // namespace truecontext::collect
